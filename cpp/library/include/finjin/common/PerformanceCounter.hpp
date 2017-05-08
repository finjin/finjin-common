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
#include "finjin/common/Utf8String.hpp"
#include <boost/timer/timer.hpp>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    using CpuTimes = boost::timer::cpu_times;
    using CpuTimer = boost::timer::cpu_timer;

    //Use FINJIN_DECLARE_PERFORMANCE_COUNTER when you want to create an instance of this class. This makes it easier to find instances in code later
    class PerformanceCounter
    {
    public:
        PerformanceCounter();
        PerformanceCounter(const Utf8String& descriptionPrefix);
        PerformanceCounter(PerformanceCounter&& other) = delete;
        PerformanceCounter(const PerformanceCounter& other) = delete;

        void operator = (PerformanceCounter&& other) = delete;
        void operator = (const PerformanceCounter& other) = delete;

        CpuTimer& GetTimer();
        CpuTimes GetElapsedTime() const;

        Utf8String ToString() const;

    private:
        Utf8String formatString;

        CpuTimer timer;
    };

    //Use FINJIN_AUTO_PERFORMANCE_COUNTER_LOGGED when you want to create an instance of this class. This makes it easier to find instances in code later
    class AutoPerformanceCounterLogged : public PerformanceCounter
    {
    public:
        AutoPerformanceCounterLogged(const Utf8String& descriptionPrefix);
        ~AutoPerformanceCounterLogged();
    };

} }


//Macros------------------------------------------------------------------------
#define FINJIN_ENABLE_PERFORMACE_COUNTERS 1

#if FINJIN_ENABLE_PERFORMACE_COUNTERS
    #define FINJIN_DECLARE_PERFORMANCE_COUNTER(name) Finjin::Common::PerformanceCounter name

    #define FINJIN_AUTO_PERFORMANCE_COUNTER_LOGGED(name, descriptionPrefix) Finjin::Common::AutoPerformanceCounterLogged name(descriptionPrefix)
#else
    #define FINJIN_DECLARE_PERFORMANCE_COUNTER(name)

    #define FINJIN_AUTO_PERFORMANCE_COUNTER_LOGGED(name, descriptionPrefix)
#endif
