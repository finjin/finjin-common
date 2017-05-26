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
#include "finjin/common/StaticUnorderedMap.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(LogLevel, LogLevel::COUNT)& GetLogLevelLookup()
{
    static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(LogLevel, LogLevel::COUNT) lookup
        (
        "info", LogLevel::INFO_LEVEL,
        "warning", LogLevel::WARNING_LEVEL,
        "error", LogLevel::ERROR_LEVEL,
        "debug", LogLevel::DEBUG_LEVEL,
        "trace", LogLevel::TRACE_LEVEL
        );

    return lookup;
}


//Implementation----------------------------------------------------------------
void LogLevelUtilities::Parse(LogLevel& result, const Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    result = Parse(value, LogLevel::COUNT);
    if (result == LogLevel::COUNT)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse log level '%1%'.", value));
}

LogLevel LogLevelUtilities::Parse(const Utf8String& value, LogLevel defaultValue)
{
    return GetLogLevelLookup().GetOrDefault(value, defaultValue);
}

const char* LogLevelUtilities::ToString(LogLevel value)
{
    auto& lookup = GetLogLevelLookup();
    for (auto& item : lookup)
    {
        if (item.second == value)
            return item.first;
    }

    return FINJIN_ENUM_UNKNOWN_STRING;
}
