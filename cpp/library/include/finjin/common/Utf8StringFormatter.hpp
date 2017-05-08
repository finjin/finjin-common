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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class Path;
    class TimeDuration;
    class Uuid;

    class FINJIN_COMMON_LIBRARY_API Utf8StringFormatter
    {
    public:
        Utf8StringFormatter(const Utf8StringFormatter& other) = delete;
        Utf8StringFormatter& operator = (const Utf8StringFormatter& other) = delete;

        Utf8StringFormatter(Utf8StringFormatter&& Utf8StringFormatter) = delete;
        Utf8StringFormatter& operator = (Utf8StringFormatter&& other) = delete;

    public:
        Utf8StringFormatter(const char* format);
        Utf8StringFormatter(const Utf8String& format);
        ~Utf8StringFormatter();

        Utf8StringFormatter& operator % (int8_t value);
        Utf8StringFormatter& operator % (int16_t value);
        Utf8StringFormatter& operator % (int32_t value);
        Utf8StringFormatter& operator % (int64_t value);

        Utf8StringFormatter& operator % (uint8_t value);
        Utf8StringFormatter& operator % (uint16_t value);
        Utf8StringFormatter& operator % (uint32_t value);
        Utf8StringFormatter& operator % (uint64_t value);

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS || FINJIN_TARGET_PLATFORM_IS_APPLE
        Utf8StringFormatter& operator % (long value);
        Utf8StringFormatter& operator % (unsigned long value);
    #endif

        Utf8StringFormatter& operator % (float value);
        Utf8StringFormatter& operator % (double value);

        Utf8StringFormatter& operator % (const char* value);
        Utf8StringFormatter& operator % (const Utf8String& value);
        Utf8StringFormatter& operator % (const Utf8StringView& value);
        Utf8StringFormatter& operator % (const Path& value);
        Utf8StringFormatter& operator % (const TimeDuration& value);
        Utf8StringFormatter& operator % (const Uuid& value);

        Utf8StringFormatter& operator % (const void* value);

        Utf8String ToString() const;

        template <typename... Args>
        static Utf8String Format(const char* formatString, const Args&... args)
        {
            Utf8StringFormatter formatter(formatString);
            return _Format(formatter, args...).ToString();
        }

        static Utf8String Format(const char* formatString)
        {
            return formatString;
        }

    private:
        template <typename Arg>
        static Utf8StringFormatter& _Format(Utf8StringFormatter& formatter, const Arg& arg)
        {
            return formatter % arg;
        }

        template <typename FirstArg, typename... Args>
        static Utf8StringFormatter& _Format(Utf8StringFormatter& formatter, const FirstArg& arg, const Args&... args)
        {
            return _Format(formatter % arg, args...);
        }

    private:
        struct Impl;
        Impl* impl;
    };

} }
