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
#include "finjin/common/LogLevel.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
void LogLevelUtilities::Parse(LogLevel& result, const Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    if (value.EqualsNoCaseAscii("info"))
        result = LogLevel::INFO_LEVEL;
    else if (value.EqualsNoCaseAscii("warning"))
        result = LogLevel::WARNING_LEVEL;
    else if (value.EqualsNoCaseAscii("error"))
        result = LogLevel::ERROR_LEVEL;    
    else if (value.EqualsNoCaseAscii("debug"))
        result = LogLevel::DEBUG_LEVEL;
    else if (value.EqualsNoCaseAscii("trace"))
        result = LogLevel::TRACE_LEVEL;
    else
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse log level '%1%'.", value));
}

LogLevel LogLevelUtilities::Parse(const Utf8String& value, LogLevel defaultValue)
{
    LogLevel result = defaultValue;

    if (value.EqualsNoCaseAscii("info"))
        result = LogLevel::INFO_LEVEL;
    else if (value.EqualsNoCaseAscii("warning"))
        result = LogLevel::WARNING_LEVEL;
    else if (value.EqualsNoCaseAscii("error"))
        result = LogLevel::ERROR_LEVEL;
    else if (value.EqualsNoCaseAscii("debug"))
        result = LogLevel::DEBUG_LEVEL;
    else if (value.EqualsNoCaseAscii("trace"))
        result = LogLevel::TRACE_LEVEL;
    
    return result;
}

Utf8String LogLevelUtilities::ToString(LogLevel value)
{
    Utf8String result;

    switch (value)
    {
        case LogLevel::INFO_LEVEL: result = "info"; break;
        case LogLevel::WARNING_LEVEL: result = "warning"; break;
        case LogLevel::ERROR_LEVEL: result = "error"; break;
        case LogLevel::DEBUG_LEVEL: result = "debug"; break;
        case LogLevel::TRACE_LEVEL: result = "trace"; break;
        default: result = "<Unknown log level>"; break;
    }

    return result;
}
