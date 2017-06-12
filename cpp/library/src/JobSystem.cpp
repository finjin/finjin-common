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
#include "finjin/common/JobSystem.hpp"
#include "FiberJobScheduler.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/FiberWaitingQueue.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/ThisThread.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
struct JobSystemThreadCount
{
    JobSystemThreadCount()
    {
        this->total = this->perThread = this->lastThread = 0;
    }

    size_t total;
    size_t perThread;
    size_t lastThread;
};

static JobSystemThreadCount GetBestNumberOfFibers(size_t requestedCount, const LogicalCpus& logicalCpus)
{
    JobSystemThreadCount count;

    //FiberWaitingQueue::MAX_ITEMS is the limiting factor in this calculation.
    //This calculation accounts for the 'main' fiber of each thread, and the main app thread
    //NOTE: Subtracting for the 'main' fiber of each thread and the main app thread may ultimately not be needed
    count.total = std::min(requestedCount, FiberWaitingQueue::MAX_ITEMS - logicalCpus.size() - 1);

    if (!logicalCpus.empty())
    {
        count.perThread = count.total / logicalCpus.size();
        count.lastThread = count.perThread + count.total % logicalCpus.size();
    }

    return count;
}


//Local types-------------------------------------------------------------------
struct JobSystem::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator)
    {
        this->currentProcessingJobGroupID = 0;
        this->nextJobGroupID = 0;

        this->currentPublicJobThreadIndex = 0;
        this->currentReservedJobThreadIndex = 0;

        this->started = false;
    }

    JobThread* SelectJobThread(JobThreadIndex index)
    {
        switch (index.type)
        {
            case JobThreadType::PUBLIC:
            {
                if (index.index != (size_t)-1)
                    return this->threads.GetPublicAt(index.index);
                else
                    return this->threads.GetPublicAt(this->currentPublicJobThreadIndex++ % this->threads.GetPublicSize());
            }
            case JobThreadType::RESERVED:
            {
                if (index.index != (size_t)-1)
                    return this->threads.GetReservedAt(index.index);
                else
                    return this->threads.GetReservedAt(this->currentReservedJobThreadIndex++ % this->threads.GetReservedSize());
            }
            default:
            {
                return nullptr;
            }
        }
    }

    /** Collection of job threads. One job thread each logical CPU. */
    class JobThreads
    {
    public:
        JobThreads()
        {
            this->count = 0;
            this->publicCount = 0;
            this->reservedCount = 0;
        }
        ~JobThreads()
        {
            this->count = 0;
        }

        size_t size() const { return this->count; }
        size_t GetPublicSize() const { return this->publicCount; }
        size_t GetReservedSize() const { return this->reservedCount; }

        void WaitForAllFibersToStart() const
        {
            //Busy wait
            for (;;)
            {
                size_t runningCount = 0;
                for (size_t i = 0; i < this->count; i++)
                {
                    if (this->items[i].IsRunningFibers())
                        runningCount++;
                }

                if (runningCount == this->count)
                    break;
            }
        }

        bool empty() const
        {
            return this->count == 0;
        }

        void Destroy()
        {
            for (size_t i = 0; i < this->count; i++)
                this->items[i].Destroy();
            this->count = 0;

            this->publicCount = 0;
            this->reservedCount = 0;
        }

        void push_back(JobThread&& item)
        {
            this->items[this->count] = std::move(item);

            switch (this->items[this->count].GetType())
            {
                case JobThreadType::PUBLIC: this->publicItems[this->publicCount++] = &this->items[this->count]; break;
                case JobThreadType::RESERVED: this->reservedItems[this->reservedCount++] = &this->items[this->count]; break;
            }

            this->count++;
        }

        const JobThread* begin() const { return &this->items[0]; }
        JobThread* begin() { return &this->items[0]; }

        const JobThread* end() const { return &this->items[this->count]; }
        JobThread* end() { return &this->items[this->count]; }

        const JobThread& operator [] (size_t i) const { return this->items[i]; }
        JobThread& operator [] (size_t i) { return this->items[i]; }

        const JobThread* GetPublicAt(size_t i) const { return this->publicItems[i]; }
        JobThread* GetPublicAt(size_t i) { return this->publicItems[i]; }

        const JobThread* GetReservedAt(size_t i) const { return this->reservedItems[i]; }
        JobThread* GetReservedAt(size_t i) { return this->reservedItems[i]; }

    private:
        std::array<JobThread, LogicalCpus::MAX_ITEMS> items;
        size_t count;

        std::array<JobThread*, LogicalCpus::MAX_ITEMS> publicItems;
        size_t publicCount;

        std::array<JobThread*, LogicalCpus::MAX_ITEMS> reservedItems;
        size_t reservedCount;
    };

    std::atomic_size_t currentProcessingJobGroupID;
    std::atomic_size_t nextJobGroupID;

    std::atomic_size_t currentPublicJobThreadIndex;
    std::atomic_size_t currentReservedJobThreadIndex;

    bool started;

    JobFiber defaultActiveFiber;
    FiberJobScheduler defaultFiberScheduler;

    JobThreads threads;
};


//Implementation----------------------------------------------------------------

//JobSystem::Settings
JobSystem::Settings::Settings()
{
    this->allocator = nullptr;
    this->maxJobCount = 128;
    this->threadJobObjectHeapSize = MemorySize::MEBIBYTE * 4;
    this->fiberCount = 64;
    this->threadStackByteCount = MemorySize::MEBIBYTE / 2;
    this->threadStackReserveByteCount = MemorySize::MEBIBYTE / 2;
}

void JobSystem::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (section.empty())
                {
                    if (key == "max-job-count")
                    {
                        Convert::ToInteger(this->maxJobCount, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'max-job-count'.", value));
                            return;
                        }
                    }
                    else if (key == "thread-job-object-heap-size")
                    {
                        MemorySize::Parse(this->threadJobObjectHeapSize, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'thread-job-object-heap-size'.", value));
                            return;
                        }
                    }
                    else if (key == "fiber-count")
                    {
                        Convert::ToInteger(this->fiberCount.requested, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'fiber-count'.", value));
                            return;
                        }
                    }
                    else if (key == "stack-size")
                    {
                        MemorySize::Parse(this->threadStackByteCount, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'stack-size'.", value));
                            return;
                        }
                    }
                    else if (key == "stack-reserve")
                    {
                        MemorySize::Parse(this->threadStackReserveByteCount, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'stack-reserve'.", value));
                            return;
                        }
                    }
                }

                break;
            }
            default: break;
        }
    }
}

void JobSystem::Settings::Finalize(const LogicalCpus& logicalCpus)
{
    auto jobFiberCount = GetBestNumberOfFibers(this->fiberCount.requested, logicalCpus);
    this->fiberCount.actual = jobFiberCount.total;

    this->jobThreadsSetup.resize(logicalCpus.size());
    for (size_t i = 0; i < this->jobThreadsSetup.size(); i++)
    {
        auto& jobThreadSetup = this->jobThreadsSetup[i];
        
        jobThreadSetup.logicalCpu = logicalCpus[i];
        jobThreadSetup.fiberCount = (i < logicalCpus.size() - 1) ? jobFiberCount.perThread : jobFiberCount.lastThread;
        jobThreadSetup.stackByteCount = this->threadStackByteCount;
        jobThreadSetup.stackReserveByteCount = this->threadStackReserveByteCount;
        jobThreadSetup.type = JobThreadType::PUBLIC;
    }
}

//JobSystem
JobSystem::JobSystem()
{
}

JobSystem::JobSystem(JobSystem&& other)
{
    impl = std::move(other.impl);
}

JobSystem& JobSystem::operator = (JobSystem&& other)
{
    Destroy();

    impl = std::move(other.impl);

    return *this;
}

JobSystem::~JobSystem()
{
}

bool JobSystem::IsStarted() const
{
    return impl->started;
}

void JobSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Validation--------------------------------
    if (impl != nullptr && !impl->threads.empty())
    {
        FINJIN_SET_ERROR(error, "Job system has already been initialized.");
        return;
    }

    if (settings.jobThreadsSetup.empty())
    {
        FINJIN_SET_ERROR(error, "No thread setups specified. At least one is required for the job system to operate.");
        return;
    }

    for (auto& threadSetup : settings.jobThreadsSetup)
    {
        if (threadSetup.stackByteCount == 0)
        {
            FINJIN_SET_ERROR(error, "All specified logical CPUs must have a stack size greater than 0.");
            return;
        }
    }

    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    //Initialize----------------------------
    impl.reset(AllocatedClass::New<Impl>(settings.allocator, FINJIN_CALLER_ARGUMENTS));
    assert(impl != nullptr);
    if (impl == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to allocate internal state.");
        return;
    }

    impl->defaultActiveFiber.InitializeDefaultFiber(settings.allocator, "job-system-default-fiber", &impl->defaultFiberScheduler);

    impl->currentProcessingJobGroupID = 0;
    impl->nextJobGroupID = 0;

    impl->currentPublicJobThreadIndex = 0;
    impl->currentReservedJobThreadIndex = 0;

    //Create threads
    for (size_t threadIndex = 0; threadIndex < settings.jobThreadsSetup.size(); threadIndex++)
    {
        auto& threadSetup = settings.jobThreadsSetup[threadIndex];

        JobThread thread;

        thread.Create
            (
            threadIndex,
            settings.allocator,
            threadSetup.type,
            threadSetup.logicalCpu,
            settings.maxJobCount,
            settings.threadJobObjectHeapSize,
            threadSetup.fiberCount,
            threadSetup.stackByteCount,
            threadSetup.stackReserveByteCount,
            error
            );
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create thread for job thread '%1%'.", threadIndex));
            return;
        }

        //Store
        impl->threads.push_back(std::move(thread));
    }
}

void JobSystem::Destroy()
{
    if (impl == nullptr)
        return;

    if (impl->started)
        Stop();

    impl->threads.Destroy();

    impl->defaultActiveFiber.ShutdownDefaultFiber();
}

void JobSystem::Start(bool validate, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl != nullptr && !impl->started && !impl->threads.empty())
    {
        for (size_t threadIndex = 0; threadIndex < impl->threads.size(); threadIndex++)
        {
            auto& thread = impl->threads[threadIndex];

            thread.SetMaxJobGroupID(impl->currentProcessingJobGroupID);

            thread.Start(error);
            if (error)
            {
                //Stop previously started threads
                for (size_t i = 0; i < threadIndex; i++)
                {
                    for (auto& thread : impl->threads)
                        thread.InterruptFibers();

                    for (auto& thread : impl->threads)
                        thread.Stop();
                }

                FINJIN_SET_ERROR(error, "Failed to start all job system threads.");
                return;
            }
        }

        impl->started = true;
    }

    if (validate)
    {
        Validate(error);
        if (error)
            FINJIN_SET_ERROR(error, "Failed to validate job system.");
    }
}

Allocator* JobSystem::GetActiveJobAllocator()
{
    assert(JobThread::GetActiveFiber() != nullptr);
    return JobThread::GetActiveFiber()->GetActiveJobAllocator();
}

size_t JobSystem::GetCurrentGroupID() const
{
    if (impl->nextJobGroupID > 0)
        return impl->nextJobGroupID - 1;
    else
        return 0;
}

size_t JobSystem::StartGroupFromMainThread()
{
    assert(JobThread::GetActiveFiber() == &impl->defaultActiveFiber); //Can only be called from main thread

    if (JobThread::GetActiveFiber() == &impl->defaultActiveFiber)
    {
        while (impl->currentProcessingJobGroupID != impl->nextJobGroupID)
        {
            //A group has already been started. Wait for it to finish (it will finish in a non-main thread)
        }

        return impl->nextJobGroupID++;
    }
    else
        return 0; //Error
}

void JobSystem::FinishGroupFromNonMainThread()
{
    assert(JobThread::GetActiveFiber() != &impl->defaultActiveFiber); //Can only be called from thread OTHER THAN main thread
    assert(impl->currentProcessingJobGroupID == impl->nextJobGroupID - 1); //A group should have already been started

    if (JobThread::GetActiveFiber() != &impl->defaultActiveFiber)
    {
        if (impl->currentProcessingJobGroupID == impl->nextJobGroupID - 1)
        {
            for (auto& thread : impl->threads)
                thread.SetMaxJobGroupID(impl->nextJobGroupID);

            impl->currentProcessingJobGroupID.store(impl->nextJobGroupID);
        }
        else
        {
            //Error
            for (auto& thread : impl->threads)
                thread.SetMaxJobGroupID(0);

            impl->currentProcessingJobGroupID = 0;
            impl->nextJobGroupID = 0;
        }
    }
}

void JobSystem::Stop()
{
    if (impl != nullptr && impl->started)
    {
        impl->threads.WaitForAllFibersToStart();

        for (auto& thread : impl->threads)
            thread.InterruptFibers();

        for (auto& thread : impl->threads)
            thread.Stop();

        impl->currentProcessingJobGroupID = 0;
        impl->nextJobGroupID = 0;

        impl->currentPublicJobThreadIndex = 0;
        impl->currentReservedJobThreadIndex = 0;

        impl->started = false;
    }
}

void JobSystem::Validate(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl != nullptr && impl->started)
    {
        if (impl->threads.empty())
        {
            FINJIN_SET_ERROR(error, "There are no job threads.");
            return;
        }

        impl->threads.WaitForAllFibersToStart();

        for (auto& thread : impl->threads)
        {
            thread.Validate(error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to validate job thread '%1%'.", thread.GetName()));
                return;
            }
        }
    }
}

JobThread* JobSystem::SelectJobThread(JobThreadIndex index)
{
    return impl->SelectJobThread(index);
}
