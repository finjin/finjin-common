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


//Includes---------------------------------------------------------------------
#include "finjin/common/Error.hpp"


//Classes----------------------------------------------------------------------
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

    #if FINJIN_TARGET_OS_IS_APPLE
        static void Parse(size_t& value, const Utf8String& stringValue, Error& error);
    #endif

        static void Parse(uint32_t& value, const Utf8String& stringValue, Error& error);
        static uint32_t Parse32(const Utf8String& stringValue, uint32_t defaultValue = 0);
      
        static void Parse(uint64_t& value, const Utf8String& stringValue, Error& error);
        static uint64_t Parse64(const Utf8String& stringValue, uint64_t defaultValue = 0);

        static Utf8String Format(uint64_t value);
    };

} }


//Functions--------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Prefer using Allocator::AlignSizeUp() where possible
    constexpr size_t AlignSizeUp(size_t size, size_t alignment)
    {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

} }
