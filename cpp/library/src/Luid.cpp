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
#include "finjin/common/Luid.hpp"
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Hash.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static void ReorderBytesIfCpuLittleEndian(uint8_t* bytes)
{
    if (IsLittleEndian())
    {
        //Quad
        std::swap(bytes[0], bytes[3]);
        std::swap(bytes[1], bytes[2]);

        //Quad
        std::swap(bytes[4], bytes[7]);
        std::swap(bytes[5], bytes[6]);
    }
}


//Implementation----------------------------------------------------------------
const Luid& Luid::GetZero()
{
    static const Luid value;
    return value;
}

Luid::Luid()
{
    this->data64 = 0;
}

Luid::Luid(const Utf8String& stringValue)
{
    if (stringValue.empty())
        this->data64 = 0;
    else
        *this = Parse(stringValue);
}

Luid::Luid(const char* stringValue)
{
    if (stringValue == nullptr || stringValue[0] == 0)
        this->data64 = 0;
    else
        *this = Parse(stringValue);
}

Luid::Luid(const uint8_t* bytes)
{
    FINJIN_COPY_MEMORY(this->data, bytes, sizeof(this->data));
}

Luid::Luid(uint32_t data1, int32_t data2)
{
    this->cpuOrderedParts.data1 = data1;
    this->cpuOrderedParts.data2 = data2;

    ReorderBytesIfCpuLittleEndian(this->data);
}

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
Luid::Luid(const LUID& luid)
{
    FINJIN_COPY_MEMORY(this->data, &luid, sizeof(LUID));

    ReorderBytesIfCpuLittleEndian(this->data);
}

Luid::operator LUID () const
{
    auto reordered = *this;
    ReorderBytesIfCpuLittleEndian(reordered.data);
    return reordered.luid;
}
#endif

bool Luid::IsZero() const
{
    return this->data64 == 0;
}

size_t Luid::GetHash() const
{
    return Hash::Bytes(this->data, sizeof(this->data));
}

std::array<uint8_t, 8> Luid::GetBytes() const
{
    std::array<uint8_t, 8> bytes;
    FINJIN_COPY_MEMORY(bytes.data(), this->data, sizeof(this->data));
    return bytes;
}

Utf8String Luid::ToString() const
{
    Utf8String result;

    for (size_t i = 0; i < 8; i++)
        result += Convert::ToHexString(this->data[i]);

    return result;
}

bool Luid::operator == (const Luid& other) const
{
    return memcmp(this->data, other.data, 8) == 0;
}

bool Luid::operator != (const Luid& other) const
{
    return memcmp(this->data, other.data, 8) != 0;
}

bool Luid::operator < (const Luid& other) const
{
    return memcmp(this->data, other.data, 8) < 0;
}

void Luid::Parse(Luid& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");
        return;
    }

    value = GetZero();

    Utf8String hexPrefixStringPart;
    for (size_t quadIndex = 0; quadIndex < 2; quadIndex++)
    {
        Utf8StringView quadPartString;
        stringValue.substr(quadPartString, quadIndex * 8, 8);
        if (quadPartString.length() < 8)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("LUID quad %1% '%2%' is of insufficient length. Must be 8 characters.", quadIndex, quadPartString));
            return;
        }

        hexPrefixStringPart = "h";
        hexPrefixStringPart += quadPartString;
        Convert::ToInteger(value.data32[quadIndex], hexPrefixStringPart, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("LUID quad '%1%' is invalid.", quadPartString));
            return;
        }
    }

    //Reorder bytes as necessary
    ReorderBytesIfCpuLittleEndian(value.data);
}

Luid Luid::Parse(const Utf8String& stringValue)
{
    Luid value;

    if (stringValue.empty())
        return value;

    Utf8String hexPrefixStringPart;
    for (size_t quadIndex = 0; quadIndex < 2; quadIndex++)
    {
        Utf8StringView quadPartString;
        stringValue.substr(quadPartString, quadIndex * 8, 8);

        hexPrefixStringPart = "h";
        hexPrefixStringPart += quadPartString;
        value.data32[quadIndex] = Convert::ToInteger(hexPrefixStringPart, (uint32_t)0);
    }

    //Reorder bytes as necessary
    ReorderBytesIfCpuLittleEndian(value.data);

    return value;
}
