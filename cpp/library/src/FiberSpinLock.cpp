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
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/FiberException.hpp"
#include "finjin/common/JobFiber.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
FiberSpinLock::FiberSpinLock() : state(Status::UNLOCKED)
{
}

void FiberSpinLock::lock()
{
    try
    {
        //Busy wait
        do
        {
            while (this->state.load(std::memory_order_relaxed) == Status::LOCKED)
                JobFiber::GetActiveFiber()->ScheduleYield();
        } while (this->state.exchange(Status::LOCKED, std::memory_order_acquire) == Status::LOCKED);
    }
    catch (FiberInterruptedException& ex)
    {
        throw ex;
    }
}

void FiberSpinLock::unlock()
{
    assert(this->state == Status::LOCKED);

    this->state.store(Status::UNLOCKED);
}
