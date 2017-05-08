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
#include "finjin/common/FiberMutex.hpp"
#include "finjin/common/JobFiber.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
FiberMutex::FiberMutex()
{
    this->state = Status::UNLOCKED;
}

FiberMutex::~FiberMutex()
{
    assert(this->waiting.empty());
}

void FiberMutex::lock()
{
    auto f = JobFiber::GetActiveFiber();
    assert(f != nullptr);

    for (;;)
    {
        std::unique_lock<FiberSpinLock> thisLock(this->spinLock);

        if (LockIfUnlocked())
        {
            //Changed from unlocked to locked. Done
            return;
        }
        else
        {
            //Already locked.
            //Store active fiber in order to be notified later
            assert(!this->waiting.Contains(f));
            this->waiting.push_back(f);

            //Suspend this fiber
            JobFiber::GetActiveFiber()->ScheduleWait(thisLock);
        }
    }
}

void FiberMutex::unlock()
{
    assert(this->state == Status::LOCKED);

    std::unique_lock<FiberSpinLock> thisLock(this->spinLock);
    JobFiber* f = nullptr;
    if (!this->waiting.empty())
    {
        f = this->waiting.front();
        this->waiting.pop_front();
        assert(f != nullptr);
    }
    this->state = Status::UNLOCKED;
    thisLock.unlock();

    if (f != nullptr)
    {
        assert(!f->IsTerminated());
        f->SetReady();
    }
}

bool FiberMutex::LockIfUnlocked()
{
    if (this->state != Status::UNLOCKED)
    {
        //Locked, do nothing
        return false;
    }
    else
    {
        //Unlocked

        //Change state
        this->state = Status::LOCKED;

        return true;
    }
}
