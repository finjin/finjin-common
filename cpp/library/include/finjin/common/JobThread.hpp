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
#include "finjin/common/JobFiber.hpp"
#include "finjin/common/FiberJob.hpp"
#include "finjin/common/LogicalCpu.hpp"
#include "finjin/common/JobThreadType.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Contains the state for a single thread of execution.
     * Derives from JobFiber since it converts itself to/from a fiber during execution.
     */
    class FINJIN_COMMON_LIBRARY_API JobThread : public JobFiber
    {
    public:
        JobThread(const JobThread& other) = delete;
        JobThread& operator = (const JobThread& other) = delete;

    public:
        JobThread();
        JobThread(JobThread&& other);
        JobThread& operator = (JobThread&& other);
        ~JobThread();

        void Create
            (
            size_t fiberIndex,
            const Utf8String& name, 
            Allocator* allocator, 
            JobThreadType type, 
            const LogicalCpu& logicalCpu, 
            size_t maxJobCount,
            size_t jobObjectHeapSize,
            size_t fiberCount,
            size_t stackCommitSize, 
            size_t stackReserveSize,
            Error& error
            );
        void Destroy();
        
        void Start(Error& error);
        void Stop();

        void AcquireJobObjectState(Allocator*& allocator, SimpleSpinLockMutex*& inUseMutex);
        
        JobThreadType GetType() const;

        void InterruptFibers(); //Should be called before calling Stop 
        bool IsRunningFibers() const; 

        void SetMaxJobGroupID(size_t value);
        
        void Submit(FiberJob::ptr_t&& job);

    private:
        struct Impl;
        Impl* impl;
    };

} }
