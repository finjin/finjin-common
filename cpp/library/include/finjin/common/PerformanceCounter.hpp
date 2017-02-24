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
#include "finjin/common/LogLevel.hpp"
#include "finjin/common/Utf8String.hpp"
#include <boost/timer/timer.hpp>


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    using CpuTimes = boost::timer::cpu_times;
    using CpuTimer = boost::timer::cpu_timer;

    //Use PERFORMANCE_COUNTER when you want to create an instance of this class
    class PerformanceCounter
    {
    public:
        PerformanceCounter(const Utf8String& descriptionPrefix = "");
                
        CpuTimer& GetTimer();
        CpuTimes GetElapsedTime() const;
        
        Utf8String FormatElapsedTime() const;
        
    private:
        Utf8String descriptionPrefix;
        Utf8String formatString;
        
        CpuTimer timer;
    };    
    
    //Use AUTO_PERFORMANCE_COUNTER_LOGGED when you want to create an instance of this class
    class AutoPerformanceCounterLogged : public PerformanceCounter
    {        
    public:
        AutoPerformanceCounterLogged(const Utf8String& logChannel, LogLevel logLevel, const Utf8String& descriptionPrefix);
        ~AutoPerformanceCounterLogged();
        
    private:
        Utf8String logChannel;
        LogLevel logLevel;
    };

} }


//Macros------------------------------------------------------------------------
#define FINJIN_ENABLE_PERFORMACE_COUNTERS 1

#if FINJIN_ENABLE_PERFORMACE_COUNTERS
    #define FINJIN_PERFORMANCE_COUNTER(name) Finjin::Common::PerformanceCounter name

    #define FINJIN_AUTO_PERFORMANCE_COUNTER_LOGGED(name, logLevel, descriptionPrefix) Finjin::Common::AutoPerformanceCounterLogged name(logLevel, descriptionPrefix)        
#else
    #define FINJIN_PERFORMANCE_COUNTER(name)
    
    #define FINJIN_AUTO_PERFORMANCE_COUNTER_LOGGED(name, logLevel, descriptionPrefix)
#endif
