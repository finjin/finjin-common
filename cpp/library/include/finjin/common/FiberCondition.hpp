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
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/FiberException.hpp"
#include "finjin/common/FiberWaitingQueue.hpp"
#include <mutex>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FiberCondition
    {
    public:
        FiberCondition(const FiberCondition&) = delete;
        FiberCondition& operator = (const FiberCondition&) = delete;

    public:
        FiberCondition() {}

        void NotifyOne()
        {
            JobFiber* f = nullptr;

            std::unique_lock<FiberSpinLock> thisLock(this->spinLock);

            //Get one waiting fiber
            if (!this->waiting.empty())
            {
                f = this->waiting.front();
                this->waiting.pop_front();
            }
            thisLock.unlock();

            if (f != nullptr)
            {
                //Notify waiting fiber
                Fiber_SetReady(f);
            }
        }

        void NotifyAll()
        {
            FiberWaitingQueue waiting;

            std::unique_lock<FiberSpinLock> thisLock(this->spinLock);

            //Get all waiting fibers
            waiting.swap(this->waiting);
            thisLock.unlock();

            //Notify all waiting fibers
            while (!waiting.empty())
            {
                JobFiber* f = waiting.front();
                waiting.pop_front();

                assert(f != nullptr);
                Fiber_SetReady(f);
            }
        }

        template <typename LockType, typename Pred>
        void wait(LockType& callerLock, Pred pred)
        {
            while (!pred())
                wait(callerLock);
        }

        template <typename LockType>
        void wait(LockType& callerLock)
        {
            auto f = Fiber_GetActiveFiber();

            try
            {
                //Lock
                std::unique_lock<FiberSpinLock> thisLock(this->spinLock);

                Fiber_ThrowIfIsInterrupted(f);

                //Ensure the fiber hasn't already been added to wait list
                assert(!this->waiting.Contains(f));

                //Store this fiber in wait list
                this->waiting.push_back(f);

                //Unlock caller
                callerLock.unlock();

                //Suspend this fiber
                //Locked spinlock will be released if this fiber was stored inside manager's waiting queue
                Fiber_ScheduleWait(Fiber_GetActiveFiber(), thisLock);

                //Lock caller again before returning
                callerLock.lock();
            }
            catch (FiberInterruptedException& ex)
            {
                std::unique_lock<FiberSpinLock> thisLock(this->spinLock);
                this->waiting.RemoveOnly(f);

                throw ex;
            }
        }

    private:
        //Utility methods that make it possible to not include JobFiber.hpp
        static void Fiber_SetReady(JobFiber* f);
        static void Fiber_ThrowIfIsInterrupted(JobFiber* f);
        static JobFiber* Fiber_GetActiveFiber();
        static void Fiber_ScheduleWait(JobFiber* f, std::unique_lock<FiberSpinLock>& lk);

    private:
        FiberSpinLock spinLock;
        FiberWaitingQueue waiting;
    };

} }
