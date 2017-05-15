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
#include "finjin/common/Error.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    struct FINJIN_COMMON_LIBRARY_API MemorySize
    {
        static const size_t UNKNOWN_SIZE = (size_t)-1;

        enum { KILOBYTE = 1000 };
        enum { MEGABYTE = KILOBYTE * KILOBYTE };
        enum { GIGABYTE = KILOBYTE * KILOBYTE * KILOBYTE };

        enum { KIBIBYTE = 1024 };
        enum { MEBIBYTE = KIBIBYTE * KIBIBYTE };
        enum { GIBIBYTE = KIBIBYTE * KIBIBYTE * KIBIBYTE };

        //Parse with default value
        static uint32_t Parse32(const char* stringValue, uint32_t defaultValue = 0) { Utf8StringView stringValueView(stringValue); return Parse32(stringValueView, defaultValue); }
        static uint64_t Parse64(const char* stringValue, uint64_t defaultValue = 0) { Utf8StringView stringValueView(stringValue); return Parse64(stringValueView, defaultValue); }

        static uint32_t Parse32(const Utf8String& stringValue, uint32_t defaultValue = 0) { Utf8StringView stringValueView(stringValue); return Parse32(stringValueView, defaultValue); }
        static uint64_t Parse64(const Utf8String& stringValue, uint64_t defaultValue = 0) { Utf8StringView stringValueView(stringValue); return Parse64(stringValueView, defaultValue); }

        static uint32_t Parse32(const Utf8StringView& stringValue, uint32_t defaultValue = 0);
        static uint64_t Parse64(const Utf8StringView& stringValue, uint64_t defaultValue = 0);

        //Parse with error
        static void Parse(uint32_t& value, const char* stringValue, Error& error) { Utf8StringView stringValueView(stringValue); Parse(value, stringValueView, error); } //Wrapper - No need to set error
        static void Parse(uint64_t& value, const char* stringValue, Error& error) { Utf8StringView stringValueView(stringValue); Parse(value, stringValueView, error); } //Wrapper - No need to set error

        static void Parse(uint32_t& value, const Utf8String& stringValue, Error& error) { Utf8StringView stringValueView(stringValue); Parse(value, stringValueView, error); } //Wrapper - No need to set error
        static void Parse(uint64_t& value, const Utf8String& stringValue, Error& error) { Utf8StringView stringValueView(stringValue); Parse(value, stringValueView, error); } //Wrapper - No need to set error

        static void Parse(uint32_t& value, const Utf8StringView& stringValue, Error& error);
        static void Parse(uint64_t& value, const Utf8StringView& stringValue, Error& error);
        
    #if FINJIN_TARGET_PLATFORM_IS_APPLE
        static void Parse(size_t& value, const char* stringValue, Error& error) { Utf8StringView stringValueView(stringValue); Parse(value, stringValueView, error); } //Wrapper - No need to set error
        static void Parse(size_t& value, const Utf8String& stringValue, Error& error) { Utf8StringView stringValueView(stringValue); Parse(value, stringValueView, error); } //Wrapper - No need to set error
        static void Parse(size_t& value, const Utf8StringView& stringValue, Error& error);
    #endif

        static Utf8String ToString(uint64_t value, uint64_t base = KILOBYTE);
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Prefer using Allocator::AlignSizeUp() where possible
    constexpr size_t AlignSizeUp(size_t size, size_t alignment)
    {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

} }
