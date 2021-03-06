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


//Macros------------------------------------------------------------------------
#define PARSE_MEMORY_SIZE(unitName, generatorFunction) \
    { \
        auto doubleString = s; \
        doubleString.pop_back(unitName); \
        \
        double doubleValue; \
        Convert::ToNumber(doubleValue, doubleString, error); \
        if (error) \
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse double %1% size from '%2%'.", unitName, doubleString)); \
        else \
            sizeValue = generatorFunction(doubleValue); \
    }


//Local functions---------------------------------------------------------------
static uint64_t Bytes(double value)
{
    return RoundToUInt64(value);
}

static uint64_t Kilobytes(double value)
{
    return RoundToUInt64(MemorySize::KILOBYTE * value);
}

static uint64_t Megabytes(double value)
{
    return RoundToUInt64(MemorySize::MEGABYTE * value);
}

static uint64_t Gigabytes(double value)
{
    return RoundToUInt64(MemorySize::GIGABYTE * value);
}

static uint64_t Kibibytes(double value)
{
    return RoundToUInt64(MemorySize::KIBIBYTE * value);
}

static uint64_t Mebibytes(double value)
{
    return RoundToUInt64(MemorySize::MEBIBYTE * value);
}

static uint64_t Gibibytes(double value)
{
    return RoundToUInt64(MemorySize::GIBIBYTE * value);
}

template <typename T, typename StringType>
void ParseMemorySize(T& sizeValue, const StringType& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (s.empty())
        FINJIN_SET_ERROR(error, "Failed to parse empty memory size.");

    else if (s.StartsWith("-"))
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse non-negative memory size. Negative specifier in '%1%'.", s));

    else if (s.EndsWith("kilobytes"))
        PARSE_MEMORY_SIZE("kilobytes", Kilobytes)
    else if (s.EndsWith("kilobyte"))
        PARSE_MEMORY_SIZE("kilobyte", Kilobytes)

    else if (s.EndsWith("megabytes"))
        PARSE_MEMORY_SIZE("megabytes", Megabytes)
    else if (s.EndsWith("megabyte"))
        PARSE_MEMORY_SIZE("megabyte", Megabytes)

    else if (s.EndsWith("gigabytes"))
        PARSE_MEMORY_SIZE("gigabytes", Gigabytes)
    else if (s.EndsWith("gigabyte"))
        PARSE_MEMORY_SIZE("gigabyte", Gigabytes)

    else if (s.EndsWith("kibibytes"))
        PARSE_MEMORY_SIZE("kibibytes", Kibibytes)
    else if (s.EndsWith("kibibyte"))
        PARSE_MEMORY_SIZE("kibibyte", Kibibytes)

    else if (s.EndsWith("mebibytes"))
        PARSE_MEMORY_SIZE("mebibytes", Mebibytes)
    else if (s.EndsWith("mebibyte"))
        PARSE_MEMORY_SIZE("mebibyte", Mebibytes)

    else if (s.EndsWith("gibibytes"))
        PARSE_MEMORY_SIZE("gibibytes", Gibibytes)
    else if (s.EndsWith("gibibyte"))
        PARSE_MEMORY_SIZE("gibibyte", Gibibytes)

    else if (s.EndsWith("bytes"))
        PARSE_MEMORY_SIZE("bytes", Bytes)
    else if (s.EndsWith("byte"))
        PARSE_MEMORY_SIZE("byte", Bytes)

    else if (s.EndsWith("KB"))
        PARSE_MEMORY_SIZE("KB", Kilobytes)
    else if (s.EndsWith("kB"))
        PARSE_MEMORY_SIZE("kB", Kilobytes)

    else if (s.EndsWith("MB"))
        PARSE_MEMORY_SIZE("MB", Megabytes)
    else if (s.EndsWith("mB"))
        PARSE_MEMORY_SIZE("mB", Megabytes)

    else if (s.EndsWith("GB"))
        PARSE_MEMORY_SIZE("GB", Gigabytes)
    else if (s.EndsWith("gB"))
        PARSE_MEMORY_SIZE("gB", Gigabytes)

    else if (s.EndsWith("KiB"))
        PARSE_MEMORY_SIZE("KiB", Kibibytes)

    else if (s.EndsWith("MiB"))
        PARSE_MEMORY_SIZE("MiB", Mebibytes)

    else if (s.EndsWith("GiB"))
        PARSE_MEMORY_SIZE("GiB", Gibibytes)

    else if (s.EndsWith("B") || s.IsDigits())
        PARSE_MEMORY_SIZE("B", Bytes)

    else
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse non-negative memory size. Unrecognized or unspecified memory size unit in '%1%'.", s));
}


//Implementation----------------------------------------------------------------
uint32_t MemorySize::Parse32(const Utf8StringView& stringValue, uint32_t defaultValue)
{
    FINJIN_DECLARE_ERROR(error);

    uint32_t parsed;
    Parse(parsed, stringValue, error);
    return error ? defaultValue : parsed;
}

uint64_t MemorySize::Parse64(const Utf8StringView& stringValue, uint64_t defaultValue)
{
    FINJIN_DECLARE_ERROR(error);

    uint64_t parsed;
    Parse(parsed, stringValue, error);
    return error ? defaultValue : parsed;
}

void MemorySize::Parse(uint32_t& sizeValue, const Utf8StringView& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    uint64_t sizeValue64;
    ParseMemorySize(sizeValue64, stringValue, error);
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

void MemorySize::Parse(uint64_t& sizeValue, const Utf8StringView& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParseMemorySize(sizeValue, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

#if FINJIN_TARGET_PLATFORM_IS_APPLE
void MemorySize::Parse(size_t& sizeValue, const Utf8StringView& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParseMemorySize(sizeValue, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}
#endif

Utf8String MemorySize::ToString(uint64_t bytes, uint64_t base)
{
    if (base != KILOBYTE && base != KIBIBYTE)
        base = KILOBYTE;

    auto gb = bytes / (base * base * base);
    bytes -= gb * (base * base * base);

    auto mb = bytes / (base * base);
    bytes -= mb * (base * base);

    auto kb = bytes / base;
    bytes -= kb * base;

    Utf8String result;

    if (gb > 0)
    {
        result = Convert::ToString(gb);
        auto fractional = RoundToInt(10.0 * static_cast<double>(mb) / static_cast<double>(base * base));
        if (fractional > 0)
        {
            result += ".";
            result += Convert::ToString(fractional);
        }
        result += base == KILOBYTE ? "GB" : "GiB";
    }
    else if (mb > 0)
    {
        result = Convert::ToString(mb);
        auto fractional = RoundToInt(10.0 * static_cast<double>(kb) / static_cast<double>(base));
        if (fractional > 0)
        {
            result += ".";
            result += Convert::ToString(fractional);
        }
        result += base == KILOBYTE ? "MB" : "MiB";
    }
    else if (kb > 0)
    {
        result = Convert::ToString(kb);
        result += base == KILOBYTE ? "KB" : "KiB";
    }
    else
    {
        result = Convert::ToString(bytes);
        result += "B";
    }

    return result;
}
