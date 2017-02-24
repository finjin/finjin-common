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
#include "finjin/common/ThisThread.hpp"
#if FINJIN_TARGET_OS_IS_WINDOWS
    //From https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
    #include <Windows.h>

    #pragma pack(push,8)
    struct THREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
    };
    #pragma pack(pop)
#else
    #include <pthread.h>
#endif

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
void ThisThread::SetName(const Utf8String& name)
{    
#if FINJIN_TARGET_OS_IS_WINDOWS
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
    
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name.c_str();
    info.dwThreadID = GetCurrentThreadId();
    info.dwFlags = 0;
    
    #pragma warning(push)
    #pragma warning(disable: 6320 6322)
    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    #pragma warning(pop)
#elif FINJIN_TARGET_OS_IS_APPLE
    pthread_setname_np(name.c_str());
#elif FINJIN_TARGET_OS_IS_LINUX
    pthread_setname_np(pthread_self(), name.c_str());
#else
    #error Implement this!
#endif
}

ThreadID ThisThread::GetID()
{
#if FINJIN_TARGET_OS_IS_WINDOWS
    return GetCurrentThreadId();
#elif FINJIN_TARGET_OS_IS_APPLE
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return tid;
#elif FINJIN_TARGET_OS_IS_LINUX
    return pthread_self();
#else
    #error Implement this!
#endif
}

void ThisThread::SleepFor(TimeDuration duration)
{
#if FINJIN_TARGET_OS_IS_WINDOWS
    Sleep(std::max((DWORD)1, static_cast<DWORD>(duration.ToMilliseconds())));
#elif FINJIN_TARGET_OS_IS_APPLE || FINJIN_TARGET_OS_IS_LINUX
    auto nanos = duration.ToNanoseconds();
    auto seconds = nanos / 1000000000ll;
    nanos %= 1000000000ll;
    
    timespec ts;
    ts.tv_sec = static_cast<time_t>(seconds);
    if (ts.tv_sec == 0)
        ts.tv_nsec = std::max((long)1, static_cast<long>(nanos));
    else 
        ts.tv_nsec = static_cast<long>(nanos);
    nanosleep(&ts, nullptr);
#else
    #error Implement this!
#endif
}
