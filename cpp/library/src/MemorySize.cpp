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
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Common;


//Local functions--------------------------------------------------------------
static uint64_t Bytes(int64_t value)
{
    return value;
}

static uint64_t Kilobytes(int64_t value)
{
    return MemorySize::KILOBYTE * value;
}

static uint64_t Megabytes(int64_t value)
{
    return MemorySize::MEGABYTE * value;
}

static uint64_t Gigabytes(int64_t value)
{
    return MemorySize::GIGABYTE * value;
}

static uint64_t Kibibytes(int64_t value)
{
    return MemorySize::KIBIBYTE * value;
}

static uint64_t Mebibytes(int64_t value)
{
    return MemorySize::MEBIBYTE * value;
}

static uint64_t Gibibytes(int64_t value)
{
    return MemorySize::GIBIBYTE * value;
}

#define PARSE_SIZE(unitName, generatorFunction) \
    {\
        auto integerString = s;\
        integerString.ReplaceAll(unitName, Utf8String::Empty());\
        \
        int64_t integerValue;\
        Convert::ToInteger(integerValue, integerString, error);\
        if (error)\
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse integer %1% size from '%2%'.", unitName, integerString));\
        else \
            sizeValue = generatorFunction(integerValue);\
    }

template <typename T>
void MemorySizeParse(T& sizeValue, const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    if (s.empty())
        FINJIN_SET_ERROR(error, "Failed to parse empty memory size.");

    else if (s.StartsWith("-"))
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse non-negative memory size. Negative specifier in '%1%'.", s));

    else if (s.EndsWith("kilobytes"))
        PARSE_SIZE("kilobytes", Kilobytes)
    else if (s.EndsWith("kilobyte"))
        PARSE_SIZE("kilobyte", Kilobytes)

    else if (s.EndsWith("megabytes"))
        PARSE_SIZE("megabytes", Megabytes)
    else if (s.EndsWith("megabyte"))
        PARSE_SIZE("megabyte", Megabytes)

    else if (s.EndsWith("gigabytes"))
        PARSE_SIZE("gigabytes", Gigabytes)
    else if (s.EndsWith("gigabyte"))
        PARSE_SIZE("gigabyte", Gigabytes)

    else if (s.EndsWith("kibibytes"))
        PARSE_SIZE("kibibytes", Kibibytes)
    else if (s.EndsWith("kibibyte"))
        PARSE_SIZE("kibibyte", Kibibytes)

    else if (s.EndsWith("mebibytes"))
        PARSE_SIZE("mebibytes", Mebibytes)
    else if (s.EndsWith("mebibyte"))
        PARSE_SIZE("mebibyte", Mebibytes)

    else if (s.EndsWith("gibibytes"))
        PARSE_SIZE("gibibytes", Gibibytes)
    else if (s.EndsWith("gibibyte"))
        PARSE_SIZE("gibibyte", Gibibytes)

    else if (s.EndsWith("bytes"))
        PARSE_SIZE("bytes", Bytes)
    else if (s.EndsWith("byte"))
        PARSE_SIZE("byte", Bytes)

    else if (s.EndsWith("KB"))
        PARSE_SIZE("KB", Kilobytes)
    else if (s.EndsWith("kB"))
        PARSE_SIZE("kB", Kilobytes)

    else if (s.EndsWith("MB"))
        PARSE_SIZE("MB", Megabytes)
    else if (s.EndsWith("mB"))
        PARSE_SIZE("mB", Megabytes)

    else if (s.EndsWith("GB"))
        PARSE_SIZE("GB", Gigabytes)
    else if (s.EndsWith("gB"))
        PARSE_SIZE("gB", Gigabytes)

    else if (s.EndsWith("KiB"))
        PARSE_SIZE("KiB", Kibibytes)

    else if (s.EndsWith("MiB"))
        PARSE_SIZE("MiB", Mebibytes)

    else if (s.EndsWith("GiB"))
        PARSE_SIZE("GiB", Gibibytes)

    else if (s.EndsWith("B") || s.IsDigits())
        PARSE_SIZE("B", Bytes)
            
    else
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse non-negative memory size. Unrecognized or unspecified memory size unit in '%1%'.", s));
}


//Implementation---------------------------------------------------------------
#if FINJIN_TARGET_OS_IS_APPLE
void MemorySize::Parse(size_t& sizeValue, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    MemorySizeParse(sizeValue, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}
#endif
    
void MemorySize::Parse(uint32_t& sizeValue, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    uint64_t sizeValue64;
    MemorySizeParse(sizeValue64, stringValue, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
    
    if (sizeValue64 > std::numeric_limits<uint32_t>::max())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("'%1%' could not be converted to a 32-bit memory size. The value should be within [0 - %2%]", stringValue, std::numeric_limits<uint32_t>::max()));
        return;
    }
    
    sizeValue = static_cast<uint32_t>(sizeValue64);
}

uint32_t MemorySize::Parse32(const Utf8String& stringValue, uint32_t defaultValue)
{
    FINJIN_DECLARE_ERROR(error);
    
    uint32_t parsed;
    Parse(parsed, stringValue, error);
    return error ? defaultValue : parsed;
}

void MemorySize::Parse(uint64_t& sizeValue, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    MemorySizeParse(sizeValue, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

uint64_t MemorySize::Parse64(const Utf8String& stringValue, uint64_t defaultValue)
{
    FINJIN_DECLARE_ERROR(error);

    uint64_t parsed;
    Parse(parsed, stringValue, error);
    return error ? defaultValue : parsed;
}

Utf8String MemorySize::Format(uint64_t value)
{
    auto gibibytes = value / GIBIBYTE;
    value -= gibibytes * GIBIBYTE;

    auto mebibytes = value / MEBIBYTE;
    value -= mebibytes * MEBIBYTE;

    auto kibibytes = value / KIBIBYTE;
    value -= kibibytes * KIBIBYTE;

    auto bytes = value;

    Utf8String result;

    if (gibibytes > 0)
    {
        auto whole = gibibytes;
        auto fractional = mebibytes;
        RoundFractionalInt(whole, fractional, 2);

        result = Convert::ToString(whole);
        result += ".";
        result += Convert::ToString(fractional);
        result += "GiB";
    }
    else if (mebibytes > 0)
    {
        auto whole = mebibytes;
        auto fractional = kibibytes;
        RoundFractionalInt(whole, fractional, 2);

        result = Convert::ToString(whole);
        result += ".";
        result += Convert::ToString(fractional);
        result += "MiB";
    }
    else if (kibibytes > 0)
    {
        auto whole = kibibytes;
        auto fractional = bytes;
        RoundFractionalInt(whole, fractional, 2);

        result = Convert::ToString(whole);
        result += ".";
        result += Convert::ToString(fractional);
        result += "KiB";
    }
    else
    {
        result = Convert::ToString(bytes);
        result += "B";
    }

    return result;
}
