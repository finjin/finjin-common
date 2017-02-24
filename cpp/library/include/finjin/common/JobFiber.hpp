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


#pragma once


//Includes---------------------------------------------------------------------
#include "finjin/common/Allocator.hpp"
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/TaggedMemoryBlockAllocator.hpp"
#include "finjin/common/Utf8String.hpp"
#include <mutex>
#include <boost/thread/null_mutex.hpp>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FiberJobQueue;
    class FiberJobScheduler;

    class JobFiber
    {
    public:
        JobFiber(const JobFiber& other) = delete;
        JobFiber& operator = (const JobFiber& other) = delete;

    public:
        enum class State
        {
            READY, //Ready to run, but not running
            RUNNING, //Running
            WAITING, //Waiting for something
            INTERRUPTED, //Interrupted. No longer running but not yet terminated
            TERMINATED //Terminated
        };

        JobFiber();
        JobFiber(JobFiber&& other);
        JobFiber& operator = (JobFiber&& other);
        ~JobFiber();

        /**
         * Initialization method for a "regular" fiber - a fiber created within a spawned thread procedure.
         */
        void Create
            (
            size_t fiberIndex,
            const Utf8String& name, 
            Allocator* allocator, 
            JobFiber* mainFiber, 
            FiberJobScheduler& scheduler,
            FiberJobQueue& jobQueue,
            size_t stackCommitSize, 
            size_t stackReserveSize
            );
        void Destroy();
        
        /**
         * Initializes the "default" fiber - typically the main application thread (the one that calls JobSystem::Create())
         * This isn't actually a true fiber, it just sets up a few of the necessary default data structures.
         */
        void InitializeDefaultFiber(Allocator* allocator, const Utf8String& name, FiberJobScheduler* scheduler);
        void ShutdownDefaultFiber();
        
        /**
         * Initializes the main fiber - converts the current thread into a fiber and sets it as the active fiber.
         */
        void InitializeMainFiber(Allocator* allocator, const Utf8String& name, FiberJobScheduler* scheduler);
        void ShutdownMainFiber();

        bool IsReady() const;
        void SetReady();

        bool IsRunning() const;
        void SetRunning();

        bool IsWaiting() const;
        void SetWaiting();

        bool IsInterrupted() const;
        void SetInterrupted();

        void ThrowIfIsInterrupted();

        bool IsTerminated() const;
        void SetTerminated();

        void ScheduleTerminate();
        void ScheduleYield();
        void ScheduleWait(std::unique_lock<FiberSpinLock>& callerLock);

        void ScheduleJoin();
        bool _JoinInto(JobFiber* f); //Internally used

        void SwitchTo();

        Allocator* GetActiveJobAllocator();
        
        State GetState() const;

        const Utf8String& GetName() const;

        static JobFiber* GetActiveFiber();
        static void SetActiveFiber(JobFiber* active);

        struct Impl;

    protected:        
        Impl* impl;

    public:
        JobFiber* next; //Used by scheduler        
    };

} }
