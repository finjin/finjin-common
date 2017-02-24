//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Common (finjin-common).
//
//Finjin Common is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/JobFiber.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/FiberException.hpp"
#include "finjin/common/FiberJob.hpp"
#include "finjin/common/FiberWaitingQueue.hpp"
#include "finjin/common/TaggedMemoryBlockAllocator.hpp"
#include "FiberJobQueue.hpp"
#include "FiberJobScheduler.hpp"
#if FINJIN_TARGET_OS_IS_WINDOWS
    #include <Windows.h>
#elif FINJIN_TARGET_OS_IS_ANDROID || FINJIN_TARGET_OS == FINJIN_TARGET_OS_IOS || FINJIN_TARGET_OS == FINJIN_TARGET_OS_APPLE_TV
    #include <boost/context/all.hpp>

    #define FINJIN_USE_BOOST_CONTEXT 1
#elif FINJIN_TARGET_OS_IS_LINUX || FINJIN_TARGET_OS == FINJIN_TARGET_OS_MAC
    //TODO: In the future, make changes to the development/build environment so that boost is used instead
    #include <sys/mman.h>
    #if !defined(_XOPEN_SOURCE)
        #define _XOPEN_SOURCE 600
    #endif
    #include <ucontext.h>    
    
    #define FINJIN_USE_POSIX_CONTEXT 1
#endif

using namespace Finjin::Common;


//Local classes----------------------------------------------------------------
struct JobFiber::Impl : public AllocatedClass
{
    Impl(Allocator* allocator);
    ~Impl();

    bool IsMainFiber() const { return this->mainFiber == nullptr; }

    bool IsReady() const { return this->state == State::READY; }
    void SetReady() { this->state = State::READY; }

    bool IsRunning() const { return this->state == State::RUNNING; }
    void SetRunning() { this->state = State::RUNNING; }

    bool IsWaiting() const { return this->state == State::WAITING; }
    void SetWaiting() { this->state = State::WAITING; }

    bool IsInterrupted() const { return this->state == State::INTERRUPTED; }
    void SetInterrupted() { this->state = State::INTERRUPTED; }

    bool IsTerminated() const { return this->state == State::TERMINATED; }
    void SetTerminated() { this->state = State::TERMINATED; }

    void ScheduleTerminate()
    {
        assert(this->scheduler != nullptr);
        this->scheduler->TerminateActiveFiber();
    }

    void FiberFunc();

#if FINJIN_TARGET_OS_IS_WINDOWS
    void* fiberHandle; //Operating system handle to fiber
#elif FINJIN_USE_POSIX_CONTEXT
    uint8_t* stack;
    ucontext_t context;
#elif FINJIN_USE_BOOST_CONTEXT
    uint8_t* stack;
    boost::context::fcontext_t context;
#endif

    FiberJob* activeJob; //The current job being Execute()'ed
    FiberJobQueue* jobQueue; //Pointer to job queue shared with other fibers in a thread
    FiberJobScheduler* scheduler; //Pointer to scheduler that determines which fiber is active in a thread
    FiberWaitingQueue waiting; //Fibers that are waiting for this fiber to exit
    FiberSpinLock spinLock; //Spin lock for waiting list
    JobFiber* mainFiber; //Pointer to the main fiber that spawned this fiber
    JobFiber::State state; //The current state of the fiber
    size_t stackSize;
    size_t stackReserveSize;
    Utf8String name; //The name of the fiber
};


//Local functions--------------------------------------------------------------
static size_t CalculateStackSize(size_t requestedSize)
{
    auto finalSize = requestedSize;
    
    //Round up to nearest page boundary. Note: Probably not needed.
#if FINJIN_TARGET_OS_IS_WINDOWS
    SYSTEM_INFO systemInfo = {};
    GetNativeSystemInfo(&systemInfo);
    if (systemInfo.dwPageSize > 0)
    {
        if ((requestedSize % (size_t)systemInfo.dwPageSize) != 0)
            finalSize += (systemInfo.dwPageSize - (requestedSize % (size_t)systemInfo.dwPageSize));
    }
#elif FINJIN_USE_POSIX_CONTEXT
    auto pageSize = sysconf(_SC_PAGE_SIZE);
    if (pageSize != -1)
    {
        if ((requestedSize % (size_t)pageSize) != 0)
            finalSize += (pageSize - (requestedSize % (size_t)pageSize));
    }
#endif
    
    return finalSize;
}

#if FINJIN_TARGET_OS_IS_WINDOWS
    static void WINAPI FiberFuncWin32(void* parameter)
    {
        auto impl = static_cast<JobFiber::Impl*>(parameter);
        impl->FiberFunc();
    }
#elif FINJIN_USE_POSIX_CONTEXT || FINJIN_USE_BOOST_CONTEXT
    static uint8_t* AllocateStack(Allocator* allocator, size_t stackSize)
    {
        return static_cast<uint8_t*>(allocator->Allocate(stackSize, FINJIN_CALLER_ARGUMENTS));
    }

    static void DeallocateStack(Allocator* allocator, uint8_t* buffer)
    {
        if (buffer == nullptr)
            return;
    
        allocator->Deallocate(buffer);
    }

    #if FINJIN_USE_POSIX_CONTEXT && __WORDSIZE == 32
        static void ContextFunction32(int a0)
        {
            assert(sizeof(int) == 4);
  
            auto impl = reinterpret_cast<JobFiber::Impl*>(a0);
            assert(impl != nullptr);
            impl->FiberFunc();
        }
    #elif FINJIN_USE_POSIX_CONTEXT && __WORDSIZE == 64
        static void ContextFunction64(int a0, int a1)
        {
            assert(sizeof(int) == 4);
  
            auto impl = reinterpret_cast<JobFiber::Impl*>(((uint64_t)a0 << 32) | (uint64_t)(uint32_t)a1);
            assert(impl != nullptr);
            impl->FiberFunc();
        }
    #elif FINJIN_USE_BOOST_CONTEXT
        static void BoostContextFunction(intptr_t objPtr)
        {
            auto impl = reinterpret_cast<JobFiber::Impl*>(objPtr);
            assert(impl != nullptr);
            impl->FiberFunc();
        }
    #endif
#endif


//Static initialization--------------------------------------------------------
static FINJIN_THREAD_LOCAL JobFiber* activeFiber = nullptr; //Per-thread variable indicating which fiber is active in a thread


//Implementation---------------------------------------------------------------
//JobFiber::Impl
JobFiber::Impl::Impl(Allocator* allocator) : AllocatedClass(allocator)
{
    this->mainFiber = nullptr;
    this->state = JobFiber::State::READY;
    this->stackSize = 0;
    this->stackReserveSize = 0;
    this->activeJob = nullptr;
    this->jobQueue = nullptr;
    this->scheduler = nullptr;

#if FINJIN_TARGET_OS_IS_WINDOWS
    this->fiberHandle = nullptr;
#elif FINJIN_USE_POSIX_CONTEXT
    this->stack = nullptr;
    FINJIN_ZERO_ITEM(this->context);
#elif FINJIN_USE_BOOST_CONTEXT
    this->stack = nullptr;
#endif
}

JobFiber::Impl::~Impl()
{
    if (!IsMainFiber())
    {
    #if FINJIN_TARGET_OS_IS_WINDOWS
        if (this->fiberHandle != nullptr)
            DeleteFiber(this->fiberHandle);
        this->fiberHandle = nullptr;
    #elif FINJIN_USE_POSIX_CONTEXT
        DeallocateStack(GetAllocator(), this->stack);
        this->stack = nullptr;
    #elif FINJIN_USE_BOOST_CONTEXT
        DeallocateStack(GetAllocator(), this->stack);
        this->stack = nullptr;
    #else
        #error Implement this!
    #endif        
    }
}

void JobFiber::Impl::FiberFunc()
{
    //Switch back to main fiber
    assert(this->mainFiber != nullptr);
    this->mainFiber->SwitchTo();

    //We'll get here at some point later
    auto isQueueInterrupted = false;
    SimpleSpinLockMutex* inUseMutex = nullptr;
    while (!IsInterrupted() && !isQueueInterrupted)
    {
        inUseMutex = nullptr;
        
        {
            FiberJob::ptr_t jobToExecute;
            switch (this->jobQueue->Pop(jobToExecute, this->scheduler->GetMaxJobGroupID()))
            {
                case FiberJobQueue::PushPopResult::SUCCESS:
                {
                    this->activeJob = jobToExecute.get();
                    
                    jobToExecute->Execute();
                    
                    this->activeJob = nullptr;
                    
                    inUseMutex = jobToExecute->GetInUseMutex();
                    
                    break;
                }
                case FiberJobQueue::PushPopResult::GROUP_CHECK_FAILED:
                {
                    //Do nothing
                    break;
                }
                case FiberJobQueue::PushPopResult::INTERRUPTED:
                {
                    isQueueInterrupted = true;
                    break;
                }
            }
        }
        
        if (inUseMutex != nullptr)
            inUseMutex->unlock();
    }

    ScheduleTerminate();
}

//JobFiber
JobFiber::JobFiber()
{
    impl = nullptr;

    this->next = nullptr;
}

JobFiber::JobFiber(JobFiber&& other)
{
    impl = other.impl;
    next = other.next;
    
    other.impl = nullptr;
    other.next = nullptr;
}

JobFiber& JobFiber::operator = (JobFiber&& other)
{
    delete impl;

    impl = other.impl;
    next = other.next;

    other.impl = nullptr;
    other.next = nullptr;

    return *this;
}

JobFiber::~JobFiber()
{
    delete impl;
}

void JobFiber::Create
    (
    size_t fiberIndex,
    const Utf8String& name, 
    Allocator* allocator, 
    JobFiber* mainFiber, 
    FiberJobScheduler& scheduler, 
    FiberJobQueue& jobQueue,
    size_t stackCommitSize, 
    size_t stackReserveSize
    )
{
    if (impl == nullptr)
        impl = AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS);

    impl->name = name;
    
    impl->mainFiber = mainFiber;
    
    impl->scheduler = &scheduler;
    
    impl->jobQueue = &jobQueue;    
    
    impl->stackSize = CalculateStackSize(stackCommitSize);
    impl->stackReserveSize = CalculateStackSize(stackReserveSize);
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    impl->fiberHandle = CreateFiberEx(impl->stackSize, impl->stackReserveSize, FIBER_FLAG_FLOAT_SWITCH, FiberFuncWin32, impl);
    assert(impl->fiberHandle != nullptr);
#elif FINJIN_USE_POSIX_CONTEXT
    auto totalStackSize = impl->stackSize + impl->stackReserveSize;
    impl->stack = AllocateStack(allocator, totalStackSize);
    assert(impl->stack != nullptr);
    
    auto getContextSupported = getcontext(&impl->context);
    assert(getContextSupported != -1); //If this fails, it's because getcontext() is not supported on this platform
    
    impl->context.uc_stack.ss_sp = impl->stack;
    impl->context.uc_stack.ss_size = totalStackSize;
    impl->context.uc_link = nullptr;
    
    #if __WORDSIZE == 32
        makecontext(&impl->context, (void(*)())ContextFunction32, 1, (uint32_t)impl);
    #elif __WORDSIZE == 64
        makecontext(&impl->context, (void(*)())ContextFunction64, 2, (uint64_t)impl >> 32, (uint64_t)impl & 0xffffffff);
    #endif
#elif FINJIN_USE_BOOST_CONTEXT
    auto totalStackSize = impl->stackSize + impl->stackReserveSize;
    impl->stack = AllocateStack(allocator, totalStackSize);
    assert(impl->stack != nullptr);
    
    impl->context = boost::context::make_fcontext(impl->stack + totalStackSize, totalStackSize, BoostContextFunction);
#else
    #error Implement this!
#endif
}

void JobFiber::Destroy()
{
    delete impl;
    impl = nullptr;
}

void JobFiber::InitializeDefaultFiber(Allocator* allocator, const Utf8String& name, FiberJobScheduler* scheduler)
{
    assert(scheduler != nullptr);
    assert(GetActiveFiber() == nullptr);

    if (impl == nullptr)
        impl = AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS);
    
    impl->name = name;
    impl->state = State::RUNNING; //The calling thread is already running
    impl->scheduler = scheduler;
    
    scheduler->SetMainFiber(this);    
    SetActiveFiber(this); 
}

void JobFiber::ShutdownDefaultFiber()
{
    if (impl != nullptr)
    {
        SetActiveFiber(nullptr);
        
        delete impl;
        impl = nullptr;
    }
}

void JobFiber::InitializeMainFiber(Allocator* allocator, const Utf8String& name, FiberJobScheduler* scheduler)
{
    assert(scheduler != nullptr);

    if (impl == nullptr)
        impl = AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS);

    impl->name = name;
    impl->state = State::RUNNING; //The calling thread is already running
    impl->scheduler = scheduler;
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    impl->fiberHandle = ConvertThreadToFiberEx(nullptr, 0);
#elif FINJIN_USE_POSIX_CONTEXT || FINJIN_USE_BOOST_CONTEXT
    //Do nothing
#else
    #error Implement this!
#endif
    
    scheduler->SetMainFiber(this);    
    SetActiveFiber(this);
}

void JobFiber::ShutdownMainFiber()
{    
    if (impl != nullptr)
    {
    #if FINJIN_TARGET_OS_IS_WINDOWS
        ConvertFiberToThread();
    #elif FINJIN_USE_POSIX_CONTEXT || FINJIN_USE_BOOST_CONTEXT
        //Do nothing
    #else
        #error Implement this!
    #endif

        SetActiveFiber(nullptr);

        delete impl;
        impl = nullptr;
    }
}

bool JobFiber::IsReady() const
{
    assert(impl != nullptr);
    return impl->state == State::READY;
}

void JobFiber::SetReady()
{
    assert(impl != nullptr);
    impl->state = State::READY;
}

bool JobFiber::IsRunning() const
{
    assert(impl != nullptr);
    return impl->state == State::RUNNING;
}

void JobFiber::SetRunning()
{
    assert(impl != nullptr);
    impl->state = State::RUNNING;
}

bool JobFiber::IsWaiting() const
{
    assert(impl != nullptr);
    return impl->state == State::WAITING;
}

void JobFiber::SetWaiting()
{
    assert(impl != nullptr);
    impl->state = State::WAITING;
}

bool JobFiber::IsInterrupted() const
{
    assert(impl != nullptr);
    return impl->state == State::INTERRUPTED;
}

void JobFiber::SetInterrupted()
{
    assert(impl != nullptr);
    impl->state = State::INTERRUPTED;
}

void JobFiber::ThrowIfIsInterrupted()
{
    if (IsInterrupted())
        throw FiberInterruptedException(this);
}

bool JobFiber::IsTerminated() const
{
    assert(impl != nullptr);
    return impl->state == State::TERMINATED;
}

void JobFiber::SetTerminated()
{
    assert(impl != nullptr);
    impl->state = State::TERMINATED;
}

void JobFiber::ScheduleTerminate()
{
    assert(impl != nullptr);
    assert(this == GetActiveFiber());

    impl->ScheduleTerminate();
}

void JobFiber::ScheduleYield()
{
    assert(impl != nullptr);
    assert(impl->scheduler != nullptr);
    assert(this == GetActiveFiber());

    impl->scheduler->YieldActiveFiber();
}

void JobFiber::ScheduleWait(std::unique_lock<FiberSpinLock>& callerLock)
{    
    assert(impl != nullptr);
    assert(impl->scheduler != nullptr);
    assert(this == GetActiveFiber());

    impl->scheduler->WaitActiveFiber(callerLock);
}

void JobFiber::ScheduleJoin()
{
    assert(impl != nullptr);
    assert(this != GetActiveFiber());

    impl->scheduler->JoinIntoActiveFiber(this);
}

bool JobFiber::_JoinInto(JobFiber* f)
{
    assert(impl != nullptr);

    std::unique_lock<FiberSpinLock> thisLock(impl->spinLock);
    if (IsTerminated())
        return false;
    else
    {
        impl->waiting.push_back(f);
        return true;
    }
}

void JobFiber::SwitchTo()
{
    assert(impl != nullptr);

    auto previous = GetActiveFiber();
    assert(previous != nullptr);
    
    SetActiveFiber(this);
        
#if FINJIN_TARGET_OS_IS_WINDOWS
    SwitchToFiber(impl->fiberHandle);
#elif FINJIN_USE_POSIX_CONTEXT
    swapcontext(&previous->impl->context, &impl->context);
#elif FINJIN_USE_BOOST_CONTEXT
    boost::context::jump_fcontext(&previous->impl->context, impl->context, reinterpret_cast<intptr_t>(impl));
#else
    #error Implement this!
#endif
}

Allocator* JobFiber::GetActiveJobAllocator()
{
    assert(impl != nullptr);
    assert(impl->activeJob != nullptr);
    if (impl->activeJob != nullptr)
        return impl->activeJob->GetAllocator();
    else
        return nullptr;
}

JobFiber::State JobFiber::GetState() const
{
    assert(impl != nullptr);
    return impl->state;
}

const Utf8String& JobFiber::GetName() const
{
    assert(impl != nullptr);
    return impl->name;
}

JobFiber* JobFiber::GetActiveFiber()
{
    return activeFiber;
}

void JobFiber::SetActiveFiber(JobFiber* fiber)
{
    activeFiber = fiber;
}
