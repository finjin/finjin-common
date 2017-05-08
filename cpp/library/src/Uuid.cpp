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
#include "finjin/common/Uuid.hpp"
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Hash.hpp"

using namespace Finjin::Common;


//Local values------------------------------------------------------------------
static RandomUuidCreator* randomUuidGenerator = nullptr;


//Local functions---------------------------------------------------------------
static void ReorderBytesIfCpuLittleEndian(uint8_t* bytes)
{
    if (IsLittleEndian())
    {
        //Reorder the first quad and 2 words

        std::swap(bytes[0], bytes[3]); //Quad
        std::swap(bytes[1], bytes[2]);

        std::swap(bytes[4], bytes[5]); //Word
        std::swap(bytes[6], bytes[7]); //Word
    }
}


//Implementation----------------------------------------------------------------

//RandomUuidCreator
RandomUuidCreator::RandomUuidCreator() : randomGenerator(static_cast<unsigned int>(DateTime::NowUtc().ToUnixTimestamp()))
{
    if (randomUuidGenerator == nullptr)
        randomUuidGenerator = this;
}

RandomUuidCreator::~RandomUuidCreator()
{
    if (randomUuidGenerator == this)
        randomUuidGenerator = nullptr;
}

Uuid RandomUuidCreator::NewUuid()
{
    Uuid result;
    for (int i = 0; i < 4; i++)
        result.data32[i] = this->randomGenerator();

    //See version 4 UUID format https://en.wikipedia.org/wiki/Universally_unique_identifier
    //Format will be: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx

    //Top 4 bits of data[6] needs to be 4
    result.data[6] = (result.data[6] & 0x0f) | 0x40;

    //Top 4 bits of data[8] needs to be 8, 9, a, or b
    result.data[8] = (result.data[8] & 0x0f) | 0xa0;

    return result;
}

//Uuid
const Uuid& Uuid::Zero()
{
    static const Uuid value;
    return value;
}

Uuid Uuid::CreateRandom()
{
    Uuid result;

    if (randomUuidGenerator != nullptr)
        result = randomUuidGenerator->NewUuid();
    else
    {
        static RandomUuidCreator defaultRandomUuidCreator;
        return defaultRandomUuidCreator.NewUuid();
    }

    return result;
}

Uuid::Uuid()
{
    FINJIN_ZERO_ITEM(this->data);
}

Uuid::Uuid(const Utf8String& stringValue)
{
    if (stringValue.empty())
        FINJIN_ZERO_ITEM(this->data);
    else
        *this = Parse(stringValue);
}

Uuid::Uuid(const char* stringValue)
{
    if (stringValue == nullptr || stringValue[0] == 0)
        FINJIN_ZERO_ITEM(this->data);
    else
        *this = Parse(stringValue);
}

Uuid::Uuid(const uint8_t* bytes)
{
    FINJIN_COPY_MEMORY(&this->data, bytes, sizeof(this->data));
}

Uuid::Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint8_t data4_0, uint8_t data4_1, uint8_t data4_2, uint8_t data4_3, uint8_t data4_4, uint8_t data4_5, uint8_t data4_6, uint8_t data4_7)
{
    this->cpuOrderedParts.data1 = data1;
    this->cpuOrderedParts.data2 = data2;
    this->cpuOrderedParts.data3 = data3;
    this->cpuOrderedParts.data4[0] = data4_0;
    this->cpuOrderedParts.data4[1] = data4_1;
    this->cpuOrderedParts.data4[2] = data4_2;
    this->cpuOrderedParts.data4[3] = data4_3;
    this->cpuOrderedParts.data4[4] = data4_4;
    this->cpuOrderedParts.data4[5] = data4_5;
    this->cpuOrderedParts.data4[6] = data4_6;
    this->cpuOrderedParts.data4[7] = data4_7;

    ReorderBytesIfCpuLittleEndian(this->data);
}

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
Uuid::Uuid(const GUID& guid)
{
    FINJIN_COPY_MEMORY(&this->data, &guid, sizeof(GUID));

    ReorderBytesIfCpuLittleEndian(this->data);
}

Uuid::operator GUID () const
{
    auto reordered = *this;
    ReorderBytesIfCpuLittleEndian(reordered.data);
    return reordered.guid;
}
#endif

bool Uuid::IsZero() const
{
    return
        this->data32[0] == 0 &&
        this->data32[1] == 0 &&
        this->data32[2] == 0 &&
        this->data32[3] == 0
        ;
}

size_t Uuid::GetHash() const
{
    return Hash::Bytes(this->data, sizeof(this->data));
}

std::array<uint8_t, 16> Uuid::GetBytes() const
{
    std::array<uint8_t, 16> bytes;
    FINJIN_COPY_MEMORY(bytes.data(), this->data, sizeof(this->data));
    return bytes;
}

Utf8String Uuid::ToString() const
{
    Utf8String result;

    result += Convert::ToHexString(this->data[0]);
    result += Convert::ToHexString(this->data[1]);
    result += Convert::ToHexString(this->data[2]);
    result += Convert::ToHexString(this->data[3]);
    result += "-";
    result += Convert::ToHexString(this->data[4]);
    result += Convert::ToHexString(this->data[5]);
    result += "-";
    result += Convert::ToHexString(this->data[6]);
    result += Convert::ToHexString(this->data[7]);
    result += "-";
    result += Convert::ToHexString(this->data[8]);
    result += Convert::ToHexString(this->data[9]);
    result += "-";
    result += Convert::ToHexString(this->data[10]);
    result += Convert::ToHexString(this->data[11]);
    result += Convert::ToHexString(this->data[12]);
    result += Convert::ToHexString(this->data[13]);
    result += Convert::ToHexString(this->data[14]);
    result += Convert::ToHexString(this->data[15]);

    return result;
}

bool Uuid::operator == (const Uuid& other) const
{
    return memcmp(this->data, other.data, 16) == 0;
}

bool Uuid::operator != (const Uuid& other) const
{
    return memcmp(this->data, other.data, 16) != 0;
}

bool Uuid::operator < (const Uuid& other) const
{
    return memcmp(this->data, other.data, 16) < 0;
}

void Uuid::Parse(Uuid& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");
        return;
    }

    value = Zero();

    //Split string
    StaticVector<Utf8StringView, 5> stringParts;
    Split(stringValue, '-', [&stringParts](Utf8StringView& value)
    {
        return stringParts.push_back(value);
    });
    if (!stringParts.full())
    {
        FINJIN_SET_ERROR(error, "Insufficient number of UUID parts.");
        return;
    }

    Utf8String hexPrefixStringPart;

    //Parse first 3 elements: 1 32-bit int, 2 16-bit ints
    uint32_t part1 = 0;
    hexPrefixStringPart = "h";
    hexPrefixStringPart += stringParts[0];
    Convert::ToInteger(part1, hexPrefixStringPart, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("UUID part 1 '%1%' is invalid.", stringParts[0]));
        return;
    }

    uint16_t part2 = 0;
    hexPrefixStringPart = "h";
    hexPrefixStringPart += stringParts[1];
    Convert::ToInteger(part2, hexPrefixStringPart, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("UUID part 2 '%1%' is invalid.", stringParts[1]));
        return;
    }

    uint16_t part3 = 0;
    hexPrefixStringPart = "h";
    hexPrefixStringPart += stringParts[2];
    Convert::ToInteger(part3, hexPrefixStringPart, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("UUID part 3 '%1%' is invalid.", stringParts[2]));
        return;
    }

    //Take remaining elements: 8 8-bit ints (2 chars at a time)
    auto bytesStringPart = stringParts[3].ToString();
    bytesStringPart += stringParts[4];
    uint8_t byteParts[8];
    for (size_t i = 0; i < 8; i++)
    {
        Utf8StringView bytePartString;
        bytesStringPart.substr(bytePartString, i * 2, 2);
        if (bytePartString.length() < 2)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("UUID byte pair %1% '%2%' is of insufficient length. Must be 2 characters.", i, bytePartString));
            return;
        }

        uint8_t bytePart = 0;
        hexPrefixStringPart = "h";
        hexPrefixStringPart += bytePartString;
        Convert::ToInteger(bytePart, hexPrefixStringPart, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("UUID byte pair '%1%' is invalid.", bytePartString));
            return;
        }

        byteParts[i] = bytePart;
    }

    //Assign results
    value.cpuOrderedParts.data1 = part1;
    value.cpuOrderedParts.data2 = part2;
    value.cpuOrderedParts.data3 = part3;
    for (size_t i = 0; i < 8; i++)
        value.cpuOrderedParts.data4[i] = byteParts[i];

    //Reorder bytes as necessary
    ReorderBytesIfCpuLittleEndian(value.data);
}

Uuid Uuid::Parse(const Utf8String& stringValue)
{
    Uuid value;

    if (stringValue.empty())
        return value;

    //Split string
    StaticVector<Utf8StringView, 5> stringParts;
    Split(stringValue, '-', [&stringParts](Utf8StringView& value)
    {
        return stringParts.push_back(value);
    });
    if (!stringParts.full())
        return value;

    Utf8String hexPrefixStringPart;

    //Parse first 3 elements: 1 32-bit int, 2 16-bit ints
    hexPrefixStringPart = "h";
    hexPrefixStringPart += stringParts[0];
    auto part1 = Convert::ToInteger(hexPrefixStringPart, (uint32_t)0);

    hexPrefixStringPart = "h";
    hexPrefixStringPart += stringParts[1];
    auto part2 = Convert::ToInteger(hexPrefixStringPart, (uint16_t)0);

    hexPrefixStringPart = "h";
    hexPrefixStringPart += stringParts[2];
    auto part3 = Convert::ToInteger(hexPrefixStringPart, (uint16_t)0);

    //Take remaining elements: 8 8-bit ints (2 chars at a time)
    auto bytesStringPart = stringParts[3].ToString();
    bytesStringPart += stringParts[4];
    uint8_t byteParts[8];
    for (size_t i = 0; i < 8; i++)
    {
        Utf8StringView bytePartString;
        bytesStringPart.substr(bytePartString, i * 2, 2);
        if (bytePartString.empty())
            return value;

        hexPrefixStringPart = "h";
        hexPrefixStringPart += bytePartString;
        auto bytePart = Convert::ToInteger(hexPrefixStringPart, (uint8_t)0);

        byteParts[i] = bytePart;
    }

    //Assign results
    value.cpuOrderedParts.data1 = part1;
    value.cpuOrderedParts.data2 = part2;
    value.cpuOrderedParts.data3 = part3;
    for (size_t i = 0; i < 8; i++)
        value.cpuOrderedParts.data4[i] = byteParts[i];

    //Reorder bytes as necessary
    ReorderBytesIfCpuLittleEndian(value.data);

    return value;
}
