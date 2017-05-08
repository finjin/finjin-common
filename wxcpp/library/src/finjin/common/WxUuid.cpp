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
#include "WxUuid.hpp"
#include <chrono>

using namespace Finjin::Common;


//Static initialization---------------------------------------------------------
const WxUuid WxUuid::NIL;


//Local values------------------------------------------------------------------
static WxRandomUuidCreator* randomUuidGenerator = nullptr;


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

//WxRandomUuidCreator
WxRandomUuidCreator::WxRandomUuidCreator() : randomGenerator(std::chrono::system_clock::now().time_since_epoch().count())
{
    if (randomUuidGenerator == nullptr)
        randomUuidGenerator = this;
}

WxRandomUuidCreator::~WxRandomUuidCreator()
{
    if (randomUuidGenerator == this)
        randomUuidGenerator = nullptr;
}

WxUuid WxRandomUuidCreator::NewUuid()
{
    WxUuid result;
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

//WxUuid
WxUuid WxUuid::CreateRandom()
{
    WxUuid result;

    if (randomUuidGenerator != nullptr)
        result = randomUuidGenerator->NewUuid();

    return result;
}

WxUuid::WxUuid()
{
    memset(&this->data, 0, sizeof(this->data));
}

WxUuid::WxUuid(const wxString& stringValue)
{
    *this = Parse(stringValue);
}

WxUuid::WxUuid(const char* stringValue)
{
    *this = Parse(wxString::FromUTF8(stringValue));
}

WxUuid::WxUuid(const uint8_t* bytes)
{
    FINJIN_COPY_MEMORY(&this->data, bytes, sizeof(this->data));
}

WxUuid::WxUuid(uint32_t data1, uint16_t data2, uint16_t data3, uint8_t data4_0, uint8_t data4_1, uint8_t data4_2, uint8_t data4_3, uint8_t data4_4, uint8_t data4_5, uint8_t data4_6, uint8_t data4_7)
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
WxUuid::WxUuid(const GUID& guid)
{
    FINJIN_COPY_MEMORY(&this->data, &guid, sizeof(GUID));

    ReorderBytesIfCpuLittleEndian(this->data);
}

WxUuid::operator GUID () const
{
    auto reordered = *this;
    ReorderBytesIfCpuLittleEndian(reordered.data);
    return reordered.guid;
}

#endif

bool WxUuid::IsZero() const
{
    for (size_t i = 0; i < 16; i++)
    {
        if (this->data[i] != 0)
            return false;
    }

    return true;
}

size_t WxUuid::GetHash() const
{
    //djb2: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx (Bernstein hash)

    size_t h = 0;
    for (size_t i = 0; i < 16; i++)
        h = 33 * h + this->data[i];
    return h;
}

std::array<uint8_t, 16> WxUuid::GetBytes() const
{
    std::array<uint8_t, 16> bytes;
    FINJIN_COPY_MEMORY(bytes.data(), this->data, sizeof(this->data));
    return bytes;
}

wxString WxUuid::ToString() const
{
    return wxString::Format
        (
        wxT("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
        this->data[0], this->data[1], this->data[2], this->data[3],
        this->data[4], this->data[5],
        this->data[6], this->data[7],
        this->data[8], this->data[9], this->data[10], this->data[11], this->data[12], this->data[13], this->data[14], this->data[15]
        );
}

bool WxUuid::operator == (const WxUuid& other) const
{
    return memcmp(this->data, other.data, 16) == 0;
}

bool WxUuid::operator != (const WxUuid& other) const
{
    return memcmp(this->data, other.data, 16) != 0;
}

bool WxUuid::operator < (const WxUuid& other) const
{
    return memcmp(this->data, other.data, 16) < 0;
}

bool WxUuid::Parse(WxUuid& value, const wxString& stringValue)
{
    value = NIL;

    if (stringValue.empty())
        return false;

    //Split string
    StaticVector<wxString, 5> stringParts;
    wxStringTokenizer partsParser(stringValue, wxT("-"));
    while (partsParser.HasMoreTokens())
    {
        if (stringParts.push_back(partsParser.GetNextToken()).HasErrorOrValue(false))
            return false;
    }

    if (!stringParts.full())
        return false;

    //Parse first 3 elements: 1 32-bit int, 2 16-bit ints
    long longParts[3];
    if (!stringParts[0].ToLong(&longParts[0], 16))
        return false;
    if (!stringParts[1].ToLong(&longParts[1], 16) || longParts[1] < 0 || longParts[1] > 65535)
        return false;
    if (!stringParts[2].ToLong(&longParts[2], 16) || longParts[2] < 0 || longParts[2] > 65535)
        return false;

    //Take remaining elements: 8 8-bit ints (2 chars at a time)
    wxString bytesStringPart = stringParts[3] + stringParts[4];
    uint8_t byteParts[8];
    for (size_t i = 0; i < 8; i++)
    {
        auto bytePartString = bytesStringPart.Mid(i * 2, 2);

        long bytePart;
        if (!bytePartString.ToLong(&bytePart, 16) || bytePart < 0 || bytePart > 255)
            return false;

        byteParts[i] = bytePart;
    }

    //Assign results
    value.cpuOrderedParts.data1 = static_cast<uint32_t>(longParts[0]);
    value.cpuOrderedParts.data2 = static_cast<uint16_t>(longParts[1]);
    value.cpuOrderedParts.data3 = static_cast<uint16_t>(longParts[2]);
    for (size_t i = 0; i < 8; i++)
        value.cpuOrderedParts.data4[i] = static_cast<uint8_t>(byteParts[i]);

    //Reorder bytes as necessary
    ReorderBytesIfCpuLittleEndian(value.data);

    return true;
}

WxUuid WxUuid::Parse(const wxString& stringValue)
{
    WxUuid value;

    if (!stringValue.empty())
    {
        Parse(value, stringValue);
    }

    return value;
}
