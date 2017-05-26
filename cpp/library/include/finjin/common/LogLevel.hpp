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
#include "Utf8String.hpp"


//Macros------------------------------------------------------------------------
#define FINJIN_MIN_LOG_LEVEL Finjin::Common::LogLevel::INFO_LEVEL
#define FINJIN_MAX_LOG_LEVEL Finjin::Common::LogLevel::TRACE_LEVEL

#define FINJIN_DEFAULT_LOG_LEVEL Finjin::Common::LogLevel::ERROR_LEVEL


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Ordered from least to most informative
    enum class LogLevel
    {
        INFO_LEVEL,
        WARNING_LEVEL,
        ERROR_LEVEL,
        DEBUG_LEVEL,
        TRACE_LEVEL,

        COUNT
    };

    class FINJIN_COMMON_LIBRARY_API LogLevelUtilities
    {
    public:
        static void Parse(LogLevel& result, const Utf8String& value, Error& error);
        static LogLevel Parse(const Utf8String& value, LogLevel defaultValue = FINJIN_DEFAULT_LOG_LEVEL);

        static const char* ToString(LogLevel value);
    };

} }


//Functions---------------------------------------------------------------------
namespace std
{
    inline ostream& operator << (ostream& out, Finjin::Common::LogLevel value)
    {
        out << Finjin::Common::LogLevelUtilities::ToString(value);

        return out;
    }
}
