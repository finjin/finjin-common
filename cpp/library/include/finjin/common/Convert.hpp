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

    /** Conversion utilities. */
    class FINJIN_COMMON_LIBRARY_API Convert
    {
    public:
        //Number
    #if FINJIN_TARGET_PLATFORM_IS_APPLE
        static size_t ToNumber(const Utf8String& stringValue, size_t defaultValue = 0);
    #endif
        static int8_t ToNumber(const Utf8String& stringValue, int8_t defaultValue = 0);
        static uint8_t ToNumber(const Utf8String& stringValue, uint8_t defaultValue = 0);
        static int16_t ToNumber(const Utf8String& stringValue, int16_t defaultValue = 0);
        static uint16_t ToNumber(const Utf8String& stringValue, uint16_t defaultValue = 0);
        static int32_t ToNumber(const Utf8String& stringValue, int32_t defaultValue = 0);
        static uint32_t ToNumber(const Utf8String& stringValue, uint32_t defaultValue = 0);
        static int64_t ToNumber(const Utf8String& stringValue, int64_t defaultValue = 0);
        static uint64_t ToNumber(const Utf8String& stringValue, uint64_t defaultValue = 0);
        static float ToNumber(const Utf8String& stringValue, float defaultValue = 0);
        static double ToNumber(const Utf8String& stringValue, double defaultValue = 0);

    #if FINJIN_TARGET_PLATFORM_IS_APPLE
        static void ToNumber(size_t& value, const Utf8String& stringValue, Error& error);
    #endif
        static void ToNumber(int8_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(uint8_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(int16_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(uint16_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(int32_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(uint32_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(int64_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(uint64_t& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(float& value, const Utf8String& stringValue, Error& error);
        static void ToNumber(double& value, const Utf8String& stringValue, Error& error);

        static void ToNumberInRange(int& value, const Utf8String& stringValue, const int minValue, const int maxValue, Error& error);
        static void ToNumberInRange(unsigned short& value, const Utf8String& stringValue, const unsigned short minValue, const unsigned short maxValue, Error& error);
        static void ToNumberInRange(float& value, const Utf8String& stringValue, const float minValue, const float maxValue, Error& error);

        //Integer
    #if FINJIN_TARGET_PLATFORM_IS_APPLE
        static size_t ToInteger(const Utf8String& stringValue, size_t defaultValue = 0);
    #endif
        static int8_t ToInteger(const Utf8String& stringValue, int8_t defaultValue = 0);
        static uint8_t ToInteger(const Utf8String& stringValue, uint8_t defaultValue = 0);
        static int16_t ToInteger(const Utf8String& stringValue, int16_t defaultValue = 0);
        static uint16_t ToInteger(const Utf8String& stringValue, uint16_t defaultValue = 0);
        static int32_t ToInteger(const Utf8String& stringValue, int32_t defaultValue = 0);
        static uint32_t ToInteger(const Utf8String& stringValue, uint32_t defaultValue = 0);
        static int64_t ToInteger(const Utf8String& stringValue, int64_t defaultValue = 0);
        static uint64_t ToInteger(const Utf8String& stringValue, uint64_t defaultValue = 0);

    #if FINJIN_TARGET_PLATFORM_IS_APPLE
        static void ToInteger(size_t& value, const Utf8String& stringValue, Error& error);
    #endif
        static void ToInteger(int8_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(uint8_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(int16_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(uint16_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(int32_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(uint32_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(int64_t& value, const Utf8String& stringValue, Error& error);
        static void ToInteger(uint64_t& value, const Utf8String& stringValue, Error& error);

        static void ToIntegerInRange(int& value, const Utf8String& stringValue, const int minValue, const int maxValue, Error& error);
        static void ToIntegerInRange(unsigned short& value, const Utf8String& stringValue, const unsigned short minValue, const unsigned short maxValue, Error& error);
        static void ToIntegerInRange(float& value, const Utf8String& stringValue, const float minValue, const float maxValue, Error& error);

        static bool ToBool(const Utf8String& stringValue, bool defaultValue = false);
        static const Utf8String& ToString(bool value);

        static Utf8String ToString(int8_t value);
        static Utf8String ToString(uint8_t value);
        static Utf8String ToString(int16_t value);
        static Utf8String ToString(uint16_t value);
        static Utf8String ToString(int32_t value);
        static Utf8String ToString(uint32_t value);
        static Utf8String ToString(int64_t value);
        static Utf8String ToString(uint64_t value);

        static Utf8String ToHexString(uint8_t value);
        static Utf8String ToHexString(uint16_t value);
        static Utf8String ToHexString(uint32_t value);
        static Utf8String ToHexString(uint64_t value);
        static Utf8String ToHexString(int8_t value);
        static Utf8String ToHexString(int16_t value);
        static Utf8String ToHexString(int32_t value);
        static Utf8String ToHexString(int64_t value);

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS || FINJIN_TARGET_PLATFORM_IS_APPLE
        static Utf8String ToString(long value);
        static Utf8String ToString(unsigned long value);
    #endif

        static Utf8String ToString(float value);
        static Utf8String ToString(double value);
    };

} }
