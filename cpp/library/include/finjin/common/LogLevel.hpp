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
#include "Error.hpp"
#include "StaticUnorderedMap.hpp"
#include "Utf8String.hpp"
#include "Utf8StringFormatter.hpp"


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
        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(LogLevel, LogLevel::COUNT)& GetLookup()
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
        
        template <typename T>
        static void Parse(LogLevel& result, const T& value, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);
            
            result = Parse(value, LogLevel::COUNT);
            if (result == LogLevel::COUNT)
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse log level '%1%'.", value));
        }
        
        template <typename T>
        static LogLevel Parse(const T& value, LogLevel defaultValue)
        {
            return GetLookup().GetOrDefault(value, defaultValue);
        }
        
        static const char* ToString(LogLevel value)
        {
            auto& lookup = GetLookup();
            for (auto& item : lookup)
            {
                if (item.second == value)
                    return item.first;
            }
            
            return FINJIN_ENUM_UNKNOWN_STRING;
        }
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
