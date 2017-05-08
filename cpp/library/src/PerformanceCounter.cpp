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
#include "finjin/common/PerformanceCounter.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define PERFORMANCE_FORMAT "%ws wall, %us user + %ss system = %ts CPU (%p%)"


//Implementation----------------------------------------------------------------

//PerformanceCounter
PerformanceCounter::PerformanceCounter()
{
}

PerformanceCounter::PerformanceCounter(const Utf8String& descriptionPrefix)
{
    if (!descriptionPrefix.empty())
    {
        this->formatString = descriptionPrefix;
        this->formatString += " performance: " PERFORMANCE_FORMAT;
    }
}

CpuTimer& PerformanceCounter::GetTimer()
{
    return this->timer;
}

CpuTimes PerformanceCounter::GetElapsedTime() const
{
    return this->timer.elapsed();
}

Utf8String PerformanceCounter::ToString() const
{
    if (!this->formatString.empty())
        return boost::timer::format(this->timer.elapsed(), boost::timer::default_places, this->formatString.c_str()).c_str();
    else
        return boost::timer::format(this->timer.elapsed(), boost::timer::default_places, PERFORMANCE_FORMAT).c_str();
}

//AutoPerformanceCounterLogged
AutoPerformanceCounterLogged::AutoPerformanceCounterLogged(const Utf8String& descriptionPrefix) : PerformanceCounter(descriptionPrefix)
{
}

AutoPerformanceCounterLogged::~AutoPerformanceCounterLogged()
{
    FINJIN_DEBUG_LOG_INFO("%1%", ToString());
}
