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
#include "FiberJobScheduler.hpp"
#include "FiberSchedulingAlgorithm.hpp"
#include "finjin/common/JobFiber.hpp"
#include "finjin/common/ThisThread.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
//FiberJobScheduler::WaitingQueue
FiberJobScheduler::WaitingQueue::WaitingQueue()
{
    clear();
}

FiberJobScheduler::WaitingQueue::~WaitingQueue()
{
}

bool FiberJobScheduler::WaitingQueue::empty() const
{
    return this->head == nullptr;
}

void FiberJobScheduler::WaitingQueue::clear()
{
    this->head = this->tail = nullptr;
    this->count = 0;
}

void FiberJobScheduler::WaitingQueue::push(JobFiber* item)
{
    assert(nullptr != item);
    assert(nullptr == item->next);

    if (this->head == nullptr)
    {
        this->head = this->tail = item;
    }
    else
    {
        this->tail->next = item;
        this->tail = this->tail->next;
    }

    this->count++;
}

JobFiber* FiberJobScheduler::WaitingQueue::top() const
{
    assert(!empty());

    return head;
}

void FiberJobScheduler::WaitingQueue::MoveReadyToScheduler(FiberSchedulingAlgorithm* scheduleAlgorithm)
{
    assert(scheduleAlgorithm != nullptr);

    auto prev = this->head;
    for (auto f = this->head; f != nullptr;)
    {
        assert(!f->IsRunning());

        if (!f->IsWaiting())
        {
            //Fiber is not waiting
            if (this->head == f)
            {
                //The fiber is also the head of the queue

                //Remove it from head
                this->head = f->next;

                prev = this->head;

                auto item = f;
                f = this->head;
                if (this->head == nullptr)
                    this->tail = this->head;
                item->next = nullptr;

                //Pass the newly unlinked JobFiber to scheduleAlgorithm
                scheduleAlgorithm->OnFiberAwakened(item);

                this->count--;
            }
            else
            {
                //The fiber is somewhere in the queue, not at head

                //Remove it from list
                prev->next = f->next;
                if (prev->next == nullptr)
                    this->tail = prev;

                auto item = f;
                f = f->next;
                item->next = nullptr;

                //Pass the newly unlinked JobFiber to scheduleAlgorithm
                scheduleAlgorithm->OnFiberAwakened(item);

                this->count--;
            }
        }
        else
        {
            //Skip
            prev = f;
            f = f->next;
        }
    }
}

void FiberJobScheduler::WaitingQueue::InterruptAll()
{
    for (auto f = head; f != nullptr; f = f->next)
        f->SetInterrupted();
}

void FiberJobScheduler::WaitingQueue::swap(WaitingQueue& other)
{
    std::swap(this->head, other.head);
    std::swap(this->tail, other.tail);
    std::swap(this->count, other.count);
}

size_t FiberJobScheduler::WaitingQueue::size() const
{
    return this->count;
}

//FiberJobScheduler
FiberJobScheduler::FiberJobScheduler() :
    defaultSchedulingAlgorithm(),
    schedulingAlgorithm(&defaultSchedulingAlgorithm),
    waitInterval(TimeDuration::Microseconds(1))
{
    this->maxJobGroupID = 0;
    this->mainFiber = nullptr;
}

FiberJobScheduler::~FiberJobScheduler()
{
}

void FiberJobScheduler::ResumeFiber(JobFiber* f)
{
    assert(f != nullptr);

    if (!f->IsInterrupted())
    {
        //Restore running state for everything except interrupted fibers
        //Leaving the fiber in its interrupted state allows FiberCondition to break out of a loop on a formerly waiting fiber that became interrupted
        f->SetRunning();
    }

    auto old = JobFiber::GetActiveFiber();
    if (f != old)
    {
        //Resuming a new fiber
        f->SwitchTo();
    }
}

void FiberJobScheduler::Clear()
{
    this->schedulingAlgorithm->Clear();
    this->waitingQueue.clear();
}

bool FiberJobScheduler::HasMoreFibersToRun() const
{
    return !this->waitingQueue.empty() || this->schedulingAlgorithm->GetScheduledCount() > 0;
}

void FiberJobScheduler::RunFiber()
{
    assert(this->mainFiber != nullptr);

    if (!HasMoreFibersToRun())
    {
        ResumeFiber(this->mainFiber);
        return;
    }

    //Loop until a fiber is resumed or the scheduler is stopped
    while (HasMoreFibersToRun())
    {
        //Move all fibers which are no longer waiting from waiting queue to scheduler
        this->waitingQueue.MoveReadyToScheduler(this->schedulingAlgorithm);

        //Pop new fiber from scheduler
        auto f = this->schedulingAlgorithm->PickNext();
        if (f != nullptr)
        {
            switch (f->GetState())
            {
                case JobFiber::State::READY:
                {
                    ResumeFiber(f);
                    return; //A fiber has been run, so exit
                }
                case JobFiber::State::INTERRUPTED:
                {
                    ResumeFiber(f);
                    break; //A fiber has only been run to allow it to finish. Do NOT exit
                }
                default:
                {
                    break;
                }
            }
        }
        else
        {
            //No fibers ready to run. Put the thread to sleep
            ThisThread::SleepFor(this->waitInterval);
        }
    }
}

void FiberJobScheduler::WaitActiveFiber(std::unique_lock<FiberSpinLock>& callerLock)
{
    JobFiber::GetActiveFiber()->ThrowIfIsInterrupted();

    //Set active fiber to waiting
    JobFiber::GetActiveFiber()->SetWaiting();

    //Release lock
    callerLock.unlock();

    //Push active fiber to waiting queue
    this->waitingQueue.push(JobFiber::GetActiveFiber());

    //Switch to another fiber
    RunFiber();

    //Fiber has been resumed
    JobFiber::GetActiveFiber()->ThrowIfIsInterrupted();
}

void FiberJobScheduler::YieldActiveFiber()
{
    JobFiber::GetActiveFiber()->ThrowIfIsInterrupted();

    //Set active fiber to ready (not running)
    JobFiber::GetActiveFiber()->SetReady();

    //Push active fiber to waiting queue
    this->waitingQueue.push(JobFiber::GetActiveFiber());

    //Switch to another fiber
    RunFiber();

    JobFiber::GetActiveFiber()->ThrowIfIsInterrupted();
}

void FiberJobScheduler::TerminateActiveFiber()
{
    assert(JobFiber::GetActiveFiber()->IsRunning() || JobFiber::GetActiveFiber()->IsInterrupted());

    //Set active fiber to terminated
    JobFiber::GetActiveFiber()->SetTerminated();

    //Switch to another fiber
    RunFiber();
}

void FiberJobScheduler::JoinIntoActiveFiber(JobFiber* f)
{
    assert(f != nullptr);
    assert(f != JobFiber::GetActiveFiber());

    //Set active fiber to waiting
    JobFiber::GetActiveFiber()->SetWaiting();

    //Push active fiber to waiting queue
    this->waitingQueue.push(JobFiber::GetActiveFiber());

    //Add active fiber to joining list of f
    if (!f->_JoinInto(JobFiber::GetActiveFiber()))
    {
        //Fiber was already terminated
        JobFiber::GetActiveFiber()->SetReady();
    }

    //Switch to another fiber
    RunFiber();

    //Fiber has been resumed
    JobFiber::GetActiveFiber()->ThrowIfIsInterrupted();

    //Check that fiber f has terminated
    assert(f->IsTerminated());
}

FiberSchedulingAlgorithm* FiberJobScheduler::GetSchedulingAlgorithm()
{
    return this->schedulingAlgorithm;
}

void FiberJobScheduler::SetSchedulingAlgorithm(std::unique_ptr<FiberSchedulingAlgorithm> algo)
{
    this->schedulingAlgorithmPtr = std::move(algo);
    this->schedulingAlgorithm = this->schedulingAlgorithmPtr ? this->schedulingAlgorithmPtr.get() : &this->defaultSchedulingAlgorithm;
}

size_t FiberJobScheduler::GetScheduledCount() const
{
    return this->schedulingAlgorithm->GetScheduledCount();
}

void FiberJobScheduler::AddFiber(JobFiber* fiber)
{
    this->schedulingAlgorithm->OnFiberAwakened(fiber);
}

JobFiber* FiberJobScheduler::GetMainFiber()
{
    return this->mainFiber;
}

void FiberJobScheduler::SetMainFiber(JobFiber* fiber)
{
    this->mainFiber = fiber;
}

size_t FiberJobScheduler::GetMaxJobGroupID() const
{
    return this->maxJobGroupID;
}

void FiberJobScheduler::SetMaxJobGroupID(size_t value)
{
    this->maxJobGroupID = value;
}
