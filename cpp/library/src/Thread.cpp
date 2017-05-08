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
#include "finjin/common/Thread.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Settings.hpp"
#include "finjin/common/ThisThread.hpp"

#if FINJIN_TARGET_PLATFORM_IS_APPLE
    #include <pthread.h>
    #include <mach/mach.h>

    #define FINJIN_THREAD_USE_PTHREAD 1
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    #include <pthread.h>

    #define FINJIN_THREAD_USE_PTHREAD 1
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#else
    #include <thread>
#endif

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
struct Thread::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
    {
    #if FINJIN_THREAD_USE_PTHREAD
        this->t = 0;
    #elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
        this->t = nullptr;
    #endif
    }

    ~Impl()
    {
        Stop();
    }

    void Stop()
    {
    #if FINJIN_THREAD_USE_PTHREAD
        if (this->t != 0)
        {
            pthread_join(this->t, nullptr);
            this->t = 0;
        }
    #elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
        if (this->t != nullptr)
        {
            WaitForSingleObject(this->t, INFINITE);
            this->t = nullptr;
        }
    #else
        if (this->t.joinable())
            this->t.join();
    #endif
    }

    void ThreadProc()
    {
        ThisThread::SetName(this->name);

        this->threadProc();
    }

    Utf8String name;
    Setting<LogicalCpu> logicalCpu;
    std::function<void()> threadProc;

#if FINJIN_THREAD_USE_PTHREAD
    pthread_t t;
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    HANDLE t;
#else
    std::thread t;
#endif
};

#if FINJIN_THREAD_USE_PTHREAD
    #if FINJIN_TARGET_PLATFORM_IS_APPLE
        static void* MachPThreadProc(void* data)
        {
            auto impl = reinterpret_cast<Thread::Impl*>(data);
            impl->ThreadProc();
            return nullptr;
        }
    #elif FINJIN_TARGET_PLATFORM_IS_LINUX
        static void* LinuxPThreadProc(void* data)
        {
            auto impl = reinterpret_cast<Thread::Impl*>(data);

            if (impl->logicalCpu.IsSet())
                impl->logicalCpu.value.AssociateThread(impl->t);

            impl->ThreadProc();
            return nullptr;
        }
    #endif
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    static DWORD WINAPI WindowsThreadProc(void* data)
    {
        auto impl = reinterpret_cast<Thread::Impl*>(data);
        impl->ThreadProc();
        return 0;
    }
#endif


//Implementation----------------------------------------------------------------
Thread::Thread()
{
    impl = nullptr;
}

Thread::Thread(Thread&& other)
{
    impl = other.impl;

    other.impl = nullptr;
}

Thread& Thread::operator = (Thread&& other)
{
    delete impl;

    impl = other.impl;

    other.impl = nullptr;

    return *this;
}

Thread::~Thread()
{
    delete impl;
}

void Thread::Create(Allocator* allocator, const Utf8String& name, std::function<void()> threadProc)
{
    if (impl == nullptr)
        impl = AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS);

    impl->name = name;
    impl->logicalCpu.Reset();
    impl->threadProc = threadProc;
}

void Thread::Create(Allocator* allocator, const Utf8String& name, const LogicalCpu& logicalCpu, std::function<void()> threadProc)
{
    if (impl == nullptr)
        impl = AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS);

    impl->name = name;
    impl->logicalCpu = logicalCpu;
    impl->threadProc = threadProc;
}

void Thread::Start(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl == nullptr)
    {
        FINJIN_SET_ERROR(error, "Thread has not yet been initialized.");
        return;
    }

#if FINJIN_TARGET_PLATFORM_IS_APPLE
    auto res = pthread_create_suspended_np(&impl->t, nullptr, MachPThreadProc, impl);
    if (res != 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create thread: %1%", res));
        return;
    }

    if (impl->logicalCpu.IsSet())
    {
        impl->logicalCpu.value.AssociateThread(impl->t, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to associated thread with processor.");
            return;
        }
    }

    mach_port_t machThread = pthread_mach_thread_np(impl->t);
    thread_resume(machThread);
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    auto res = pthread_create(&impl->t, nullptr, LinuxPThreadProc, impl);
    if (res != 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create thread: %1%", res));
        return;
    }
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    impl->t = CreateThread(nullptr, 0, WindowsThreadProc, impl, CREATE_SUSPENDED, nullptr);
    if (impl->t == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to create thread");
        return;
    }

    if (impl->logicalCpu.IsSet())
    {
        impl->logicalCpu.value.AssociateThread(impl->t, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to associated thread with processor.");
            return;
        }
    }

    ResumeThread(impl->t);
#else
    impl->t = std::thread([this]()
    {
        if (impl->logicalCpu.IsSet())
            impl->logicalCpu.value.AssociateCurrentThread();

        ThreadProc();
    });
#endif
}

void Thread::Stop()
{
    if (impl == nullptr)
        return;

    impl->Stop();
}

const Utf8String& Thread::GetName() const
{
    if (impl == nullptr)
        return Utf8String::Empty();

    return impl->name;
}

const LogicalCpu* Thread::GetLogicalCpu() const
{
    if (impl == nullptr)
        return nullptr;

    return impl->logicalCpu.IsSet() ? &impl->logicalCpu.value : nullptr;
}

ThreadHandle Thread::GetNativeHandle()
{
    if (impl == nullptr)
        return 0;

#if FINJIN_THREAD_USE_PTHREAD || FINJIN_TARGET_PLATFORM_IS_WINDOWS
    return impl->t;
#else
    return impl->t.native_handle();
#endif
}

Thread::Impl* Thread::GetImpl()
{
    return impl;
}
