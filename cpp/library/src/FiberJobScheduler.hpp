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
#include "FiberJobQueue.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/LogicalCpu.hpp"
#include "FiberSchedulingAlgorithm.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class JobFiber;
    
    class FiberJobScheduler
    {
    public:
        FiberJobScheduler(const FiberJobScheduler&) = delete;
        FiberJobScheduler& operator = (const FiberJobScheduler&) = delete;

    public:
        FiberJobScheduler();
        virtual ~FiberJobScheduler();
        
        void Clear();

        bool HasMoreFibersToRun() const;
        void RunFiber();

        void WaitActiveFiber(std::unique_lock<FiberSpinLock>& callerLock);

        void YieldActiveFiber();

        void TerminateActiveFiber();

        void JoinIntoActiveFiber(JobFiber* f);

        size_t GetScheduledCount() const;

        FiberSchedulingAlgorithm* GetSchedulingAlgorithm();
        void SetSchedulingAlgorithm(std::unique_ptr<FiberSchedulingAlgorithm>);

        void AddFiber(JobFiber* fiber);

        JobFiber* GetMainFiber();
        void SetMainFiber(JobFiber* fiber);
        
        size_t GetMaxJobGroupID() const;
        void SetMaxJobGroupID(size_t value);
        
    private:
        void ResumeFiber(JobFiber* fiber);

    private:
        std::unique_ptr<FiberSchedulingAlgorithm> schedulingAlgorithmPtr;
        RoundRobinFiberSchedulingAlgorithm defaultSchedulingAlgorithm;        
        FiberSchedulingAlgorithm* schedulingAlgorithm;

        /** Fibers that are waiting. All queued fibers will either be waiting or interrupted, though they all start off as waiting. */
        class WaitingQueue
        {
        public:
            WaitingQueue(const WaitingQueue&) = delete;
            WaitingQueue& operator = (const WaitingQueue&) = delete;

        public:
            WaitingQueue();
            ~WaitingQueue();

            bool empty() const;
            
            void clear();

            void push(JobFiber* item);

            JobFiber* top() const;

            void MoveReadyToScheduler(FiberSchedulingAlgorithm*);

            void InterruptAll();

            void swap(WaitingQueue& other);

            size_t size() const;

        private:
            size_t count;
            JobFiber* head;
            JobFiber* tail;
        };
        WaitingQueue waitingQueue;

        TimeDuration waitInterval;
        
        volatile size_t maxJobGroupID;

        JobFiber* mainFiber;
    };

} }
