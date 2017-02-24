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
#include "finjin/common/FiberCondition.hpp"
#include "finjin/common/JobFiber.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
void FiberCondition::Fiber_SetReady(JobFiber* f)
{
    f->SetReady();
}

void FiberCondition::Fiber_ThrowIfIsInterrupted(JobFiber* f)
{
    return f->ThrowIfIsInterrupted();
}

JobFiber* FiberCondition::Fiber_GetActiveFiber()
{
    return JobFiber::GetActiveFiber();
}

void FiberCondition::Fiber_ScheduleWait(JobFiber* f, std::unique_lock<FiberSpinLock>& lk)
{
    f->ScheduleWait(lk);
}
