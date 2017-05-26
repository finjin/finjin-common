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


//Includes----------------------------------------------------------------------
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FiberJob.hpp"
#include "finjin/common/JobFuture.hpp"
#include "finjin/common/JobInvokeHelper.hpp"
#include "finjin/common/JobPackagedTask.hpp"
#include "finjin/common/JobThread.hpp"
#include "finjin/common/JobThreadType.hpp"
#include "finjin/common/LogicalCpu.hpp"
#include "finjin/common/RequestedValue.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    struct FINJIN_COMMON_LIBRARY_API JobThreadIndex
    {
        JobThreadType type;
        size_t index;

        JobThreadIndex(JobThreadType type, size_t index)
        {
            this->type = type;
            this->index = index;
        }

        static JobThreadIndex AnyPublic() {return JobThreadIndex(JobThreadType::PUBLIC, -1);}
        static JobThreadIndex AnyReserved() {return JobThreadIndex(JobThreadType::RESERVED, -1);}

        static JobThreadIndex Public(size_t index) {return JobThreadIndex(JobThreadType::PUBLIC, index);}
        static JobThreadIndex Reserved(size_t index) {return JobThreadIndex(JobThreadType::RESERVED, index);}
    };

    class FINJIN_COMMON_LIBRARY_API JobSystem
    {
    public:
        JobSystem(const JobSystem& other) = delete;
        JobSystem& operator = (const JobSystem& other) = delete;

    public:
        JobSystem();
        JobSystem(JobSystem&& other);
        JobSystem& operator = (JobSystem&& other);
        ~JobSystem();

        struct Settings
        {
            Settings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            void Finalize(const LogicalCpus& logicalCpus);

            Allocator* allocator;
            size_t maxJobCount; //Per thread max # of jobs
            size_t threadJobObjectHeapSize; //Per thread size of heap for creating jobs and job specific data
            RequestedValue<size_t> fiberCount; //Total # of fibers. Will be clamped to global max and divided up for use in JobThreadSetup
            size_t threadStackByteCount; //Per thread and per fiber thread stack size. Will be copied into JobThreadSetup
            size_t threadStackReserveByteCount; //Per thread and per fiber thread stack reserve size. Will be copied into JobThreadSetup

            struct JobThreadSetup
            {
                LogicalCpu logicalCpu;
                size_t fiberCount;
                size_t stackByteCount;
                size_t stackReserveByteCount;
                JobThreadType type;
            };
            StaticVector<JobThreadSetup, LogicalCpus::MAX_ITEMS> jobThreadsSetup;
        };

        bool IsStarted() const;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        void Validate(Error& error);

        void Start(bool validate, Error& error);
        void Stop();

        size_t GetCurrentGroupID() const;
        size_t StartGroupFromMainThread();
        void FinishGroupFromNonMainThread();

        Allocator* GetActiveJobAllocator(); //Must be called from within the closure of a job function

        template <typename Fn, typename... Args>
        future<typename std::result_of<Fn&&(Args&&...)>::type>
        Submit(Fn&& fn, Args&&... args)
        {
            assert(impl != nullptr);
            assert(IsStarted());

            auto groupID = GetCurrentGroupID();
            auto index = JobThreadIndex::AnyPublic();
            auto jobThread = SelectJobThread(index);

            Allocator* allocator;
            SimpleSpinLockMutex* inUseMutex;
            jobThread->AcquireJobObjectState(allocator, inUseMutex);

            using result_type = typename std::result_of<Fn&&(Args&&...)>::type;
            packaged_task<result_type(typename std::decay<Args>::type...)> pt(allocator, std::forward<Fn>(fn));
            future<result_type> f(pt.get_future());

            auto job = CreateFiberJob(allocator, groupID, inUseMutex, std::move(pt), std::forward<Args>(args)...);
            jobThread->Submit(std::move(job));

            return std::move(f);
        }

    private:
        JobThread* SelectJobThread(JobThreadIndex index);

        template <typename Fn>
        FiberJob* CreateWrappedFiberJob(Allocator* allocator, size_t groupID, SimpleSpinLockMutex* inUseMutex, Fn&& fn)
        {
            return AllocatedClass::New<WrappedFiberJob<Fn> >(allocator, FINJIN_CALLER_ARGUMENTS, groupID, inUseMutex, std::forward<Fn>(fn));
        }

        template <typename Fn, typename... Args>
        FiberJob::ptr_t CreateFiberJob(Allocator* allocator, size_t groupID, SimpleSpinLockMutex* inUseMutex, Fn&& fn, Args&&... args)
        {
            return FiberJob::ptr_t
                (
                CreateWrappedFiberJob
                    (
                    allocator,
                    groupID,
                    inUseMutex,
                    [fn = std::forward<Fn>(fn), tpl = std::make_tuple(std::forward<Args>(args)...)]() mutable -> decltype(auto)
                    {
                        invoke_helper(std::move(fn), std::move(tpl));
                    }
                    )
                );
        }

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
