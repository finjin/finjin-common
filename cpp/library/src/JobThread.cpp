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
#include "finjin/common/JobThread.hpp"
#include "FiberJobQueue.hpp"
#include "FiberJobScheduler.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/GeneralAllocator.hpp"
#include "finjin/common/TaggedMemoryBlockAllocator.hpp"
#include "finjin/common/Thread.hpp"
#include "finjin/common/ThisThread.hpp"

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
using JobObjectAllocatorPool = TaggedMemoryBlockPool<ByteMemoryArena, uint64_t, (uint64_t)-1, SimpleSpinLockMutex>;

struct JobThread::Impl : public AllocatedClass
{
    Impl(Allocator* allocator, JobThread* owner) : AllocatedClass(allocator)
    {
        this->owner = owner;
    }

    void ThreadFunc();

    class JobFibers
    {
    public:
        JobFibers() { this->count = 0; }
        ~JobFibers() { this->count = 0; }

        size_t size() const { return this->count; }

        bool empty() const { return this->count == 0; }

        void Destroy()
        {
            for (size_t i = 0; i < this->count; i++)
                this->items[i].Destroy();

            this->count = 0;
        }

        void push_back(JobFiber&& item)
        {
            assert(this->count < CommonConstants::MAX_FIBERS);
            this->items[this->count++] = std::move(item);
        }

        const JobFiber* begin() const { return &this->items[0]; }
        JobFiber* begin() { return &this->items[0]; }

        const JobFiber* end() const { return &this->items[this->count]; }
        JobFiber* end() { return &this->items[this->count]; }

        const JobFiber& operator [] (size_t i) const { return this->items[i]; }
        JobFiber& operator [] (size_t i) { return this->items[i]; }

    private:
        std::array<JobFiber, CommonConstants::MAX_FIBERS> items;
        size_t count;
    };

    JobThread* owner;

    JobThreadType type;
    size_t fiberCount;
    size_t stackCommitSize;
    size_t stackReserveSize;
    bool runningFibers;
    std::atomic_size_t jobSlotIndex;

    JobFibers fibers;
    Thread thread;
    JobObjectAllocatorPool jobObjectAllocatorPool; //The pool used to allocate job objects for the thread
    struct JobSlot
    {
        TaggedMemoryBlockAllocator<JobObjectAllocatorPool> objectAllocator;
        SimpleSpinLockMutex inUseMutex;
    };
    DynamicVector<JobSlot> jobSlots;
    FiberJobQueue jobQueue;
    FiberJobScheduler scheduler;
};


//Implementation----------------------------------------------------------------

//JobThread::Impl
void JobThread::Impl::ThreadFunc()
{
    //Convert this thread to main fiber
    this->owner->InitializeMainFiber(GetAllocator(), this->thread.GetName().c_str(), &this->scheduler);

    //Create other fibers
    for (size_t fiberIndex = 0; fiberIndex < this->fiberCount; fiberIndex++)
    {
        JobFiber jobFiber;

        //Create the fiber
        auto fiberCreateResult = jobFiber.Create
            (
            fiberIndex,
            this->thread.GetName(),
            GetAllocator(),
            this->owner,
            this->scheduler,
            this->jobQueue,
            this->stackCommitSize,
            this->stackReserveSize
            );
        if (fiberCreateResult == JobFiber::CreateResult::SUCCESS)
        {
            //Store
            this->fibers.push_back(std::move(jobFiber));
            this->scheduler.AddFiber(&this->fibers[fiberIndex]);

            //Switch to the fiber, which will initialize and then switch back
            this->fibers[fiberIndex].SwitchTo();
        }
    }

    if (!this->fibers.empty())
    {
        this->runningFibers = true;

        //Run until the scheduler stops
        while (this->scheduler.HasMoreFibersToRun())
            this->scheduler.RunFiber();

        //Destroy fibers
        //Some interrupted fibers may not have had a chance to fully terminate
        //Switch back to those and let them properly schedule themselves for termination
        for (auto& fiber : this->fibers)
        {
            if (fiber.IsInterrupted())
                fiber.SwitchTo();
        }

        this->fibers.Destroy();
        this->scheduler.Clear();

        //Convert main fiber back to a thread
        this->owner->ShutdownMainFiber();

        this->jobQueue.clear();
    }

    this->runningFibers = false;
}

//JobThread
JobThread::JobThread()
{
    impl = nullptr;
}

JobThread::JobThread(JobThread&& other)
{
    impl = other.impl;
    if (impl != nullptr)
        impl->owner = this;

    other.impl = nullptr;
}

JobThread& JobThread::operator = (JobThread&& other)
{
    delete impl;

    impl = other.impl;
    if (impl != nullptr)
        impl->owner = this;

    other.impl = nullptr;

    return *this;
}

JobThread::~JobThread()
{
    delete impl;
}

void JobThread::Create
    (
    size_t threadIndex,
    Allocator* allocator,
    JobThreadType type,
    const LogicalCpu& logicalCpu,
    size_t maxJobCount,
    size_t jobObjectHeapSize,
    size_t fiberCount,
    size_t stackCommitSize,
    size_t stackReserveSize,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl == nullptr)
    {
        impl = AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, this);
        if (impl == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate internal state for job thread '%1%'.", threadIndex));
            return;
        }
    }

    impl->type = type;
    impl->fiberCount = std::min(fiberCount, (size_t)CommonConstants::MAX_FIBERS);
    impl->stackCommitSize = stackCommitSize;
    impl->stackReserveSize = stackReserveSize;
    impl->runningFibers = false;
    impl->jobSlotIndex = 0;

    auto jobObjectArena = allocator->AllocateArena(jobObjectHeapSize, 0, FINJIN_CALLER_ARGUMENTS);
    if (jobObjectArena.IsNull())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate memory arena for job thread '%1%'.", threadIndex));
        return;
    }

    JobObjectAllocatorPool::Settings jobObjectAllocatorPoolSettings;
    jobObjectAllocatorPoolSettings.blockCount = jobObjectHeapSize / 4096; //Job data seems to go over 2k so 4k blocks should be fine
    jobObjectAllocatorPoolSettings.tagCount = maxJobCount; //One for each job
    jobObjectAllocatorPoolSettings.allocator = allocator;
    impl->jobObjectAllocatorPool.Create(jobObjectAllocatorPoolSettings, std::move(jobObjectArena), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate job allocator pool for job thread '%1%'.", threadIndex));
        return;
    }

    impl->jobSlots.Create(maxJobCount, allocator);
    for (size_t jobTag = 0; jobTag < impl->jobSlots.size(); jobTag++)
        impl->jobSlots[jobTag].objectAllocator.Create(jobTag, &impl->jobObjectAllocatorPool);

    //Name
    impl->thread.Create(allocator, "job-thread-", threadIndex, &logicalCpu, std::bind(&JobThread::Impl::ThreadFunc, impl), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create thread object for job thread '%1%'.", threadIndex));
        return;
    }
}

void JobThread::Destroy()
{
    delete impl;
    impl = nullptr;
}

void JobThread::Validate(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(impl != nullptr);
    assert(impl->jobObjectAllocatorPool.GetBytesFree() > 0);

    if (impl != nullptr && impl->fibers.size() < impl->fiberCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Only %1% of %2% fibers were created.", impl->fibers.size(), impl->fiberCount));
        return;
    }
}

void JobThread::Start(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl == nullptr)
    {
        FINJIN_SET_ERROR(error, "Job thread has not yet been initialized.");
        return;
    }

    impl->thread.Start(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to start job thread '%1%'.", impl->thread.GetName().c_str()));
        return;
    }
}

void JobThread::Stop()
{
    if (impl == nullptr)
        return;

    assert(impl->jobQueue.empty());

    impl->thread.Stop();

    impl->scheduler.SetMaxJobGroupID(0);
}

void JobThread::InterruptFibers()
{
    if (impl == nullptr)
        return;

    for (auto& fiber : impl->fibers)
        fiber.SetInterrupted();
}

bool JobThread::IsRunningFibers() const
{
    if (impl == nullptr)
        return false;

    return impl->runningFibers;
}

void JobThread::SetMaxJobGroupID(size_t value)
{
    if (impl == nullptr)
        return;

    impl->scheduler.SetMaxJobGroupID(value);
}

void JobThread::AcquireJobObjectState(Allocator*& allocator, SimpleSpinLockMutex*& inUseMutex)
{
    auto jobIndex = impl->jobSlotIndex++ % impl->jobSlots.size();

    allocator = &impl->jobSlots[jobIndex].objectAllocator;
    inUseMutex = &impl->jobSlots[jobIndex].inUseMutex;

    //Wait for the job slot to no longer be in use
    inUseMutex->lock();

    //Deallocate the objects related to the job slot
    allocator->DeallocateAll();
}

JobThreadType JobThread::GetType() const
{
    assert(impl != nullptr);

    if (impl == nullptr)
        return JobThreadType::PUBLIC;

    return impl->type;
}

void JobThread::Submit(FiberJob::ptr_t&& job)
{
    assert(impl != nullptr);

    if (impl == nullptr)
        return;

    impl->jobQueue.Push(std::move(job));
}
