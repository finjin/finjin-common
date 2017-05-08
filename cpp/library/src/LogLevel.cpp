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

    if (value == "info")
        result = LogLevel::INFO_LEVEL;
    else if (value == "warning")
        result = LogLevel::WARNING_LEVEL;
    else if (value == "error")
        result = LogLevel::ERROR_LEVEL;
    else if (value == "debug")
        result = LogLevel::DEBUG_LEVEL;
    else if (value == "trace")
        result = LogLevel::TRACE_LEVEL;
    else
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse log level '%1%'.", value));
}

LogLevel LogLevelUtilities::Parse(const Utf8String& value, LogLevel defaultValue)
{
    if (value == "info")
        return LogLevel::INFO_LEVEL;
    else if (value == "warning")
        return LogLevel::WARNING_LEVEL;
    else if (value == "error")
        return LogLevel::ERROR_LEVEL;
    else if (value == "debug")
        return LogLevel::DEBUG_LEVEL;
    else if (value == "trace")
        return LogLevel::TRACE_LEVEL;
    else
        return defaultValue;
}

Utf8String LogLevelUtilities::ToString(LogLevel value)
{
    switch (value)
    {
        case LogLevel::INFO_LEVEL: return "info";
        case LogLevel::WARNING_LEVEL: return "warning";
        case LogLevel::ERROR_LEVEL: return "error";
        case LogLevel::DEBUG_LEVEL: return "debug";
        case LogLevel::TRACE_LEVEL: return "trace";
        default: return "<Unknown log level>";
    }
}
