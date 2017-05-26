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
#include "finjin/common/ChunkName.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Common;


//Static initialization---------------------------------------------------------
const ChunkPropertyName StandardChunkPropertyNames::SIGNATURE("magic");
const ChunkPropertyName StandardChunkPropertyNames::FORMAT("format");
const ChunkPropertyName StandardChunkPropertyNames::FORMAT_VERSION("format-version");
const ChunkPropertyName StandardChunkPropertyNames::MAX_BYTES_PER_LINE("max-bytes-per-line");
const ChunkPropertyName StandardChunkPropertyNames::BLOB_TEXT_FORMAT("blob-text-format");


//Local functions---------------------------------------------------------------
template <typename T>
T ChunkPropertyHash(const char* s)
{
    //djb2: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx (Bernstein hash)

    auto p = reinterpret_cast<const uint8_t*>(s);
    T h = 0;
    for (size_t i = 0; s[i] != 0; i++)
        h = 33 * h + p[i];
    return h;
}


//Implementation----------------------------------------------------------------

//ChunkName
ChunkName::ChunkName() : name(nullptr), index((Index)-1)
{
}

ChunkName::ChunkName(const char* _name) : name(_name), index((Index)-1)
{
}

ChunkName::ChunkName(const char* _name, const Uuid& _id) : name(_name), id(_id), index((Index)-1)
{
}

ChunkName::ChunkName(size_t _index) : name(nullptr), index(static_cast<Index>(_index))
{
}

ChunkName::ChunkName(const ChunkName& other, size_t _index) : name(other.name), id(other.id), index(static_cast<Index>(_index))
{
}

bool ChunkName::IsValid() const
{
    return
        (this->index != (Index)-1) ||
        !this->id.IsZero() ||
        (this->name != nullptr && strlen(this->name) <= NAME_MAX_LENGTH)
        ;
}

bool ChunkName::operator == (const ChunkName& other) const
{
    if (!this->id.IsZero() && !other.id.IsZero())
        return this->id == other.id;
    else if (this->id.IsZero() != other.id.IsZero())
        return false;
    else if (this->name != nullptr && other.name != nullptr)
        return this->name == other.name || strcmp(this->name, other.name) == 0;
    else if (this->name != other.name)
        return false;
    else if (this->index != (Index)-1 && other.index != (Index)-1)
        return this->index == other.index;
    else
        return false;
}

bool ChunkName::operator == (const Utf8String& other) const
{
    return other == this->name;
}

bool ChunkName::operator == (const Uuid& other) const
{
    return other == this->id;
}

bool ChunkName::operator == (const ParsedChunkName& other) const
{
    if (!other.id.IsZero())
        return this->id == other.id;
    else if (!other.name.empty())
        return other.name == this->name;
    else
        return this->index == other.index;
}

bool ChunkName::operator != (const ChunkName& other) const
{
    return !(*this == other);
}

bool ChunkName::operator != (const Utf8String& other) const
{
    return !(*this == other);
}

bool ChunkName::operator != (const Uuid& other) const
{
    return !(*this == other);
}

bool ChunkName::operator != (const ParsedChunkName& other) const
{
    return !(*this == other);
}

Utf8String ChunkName::ToString() const
{
    Utf8String s = this->name;
    if (!this->id.IsZero())
    {
        if (!s.empty())
            s += "|";
        s += this->id.ToString();
    }
    else if (this->index != (Index)-1)
    {
        if (!s.empty())
            s += "|";
        s += Convert::ToString(this->index);
    }
    return s;
}

//ParsedChunkName
ParsedChunkName::ParsedChunkName()
{
    this->index = (Index)-1;
}

ParsedChunkName::ParsedChunkName(const Utf8String& s)
{
    Parse(s);
}

ParsedChunkName::ParsedChunkName(Index index)
{
    this->index = index;
}

void ParsedChunkName::Parse(const Utf8String& s)
{
    this->name.clear();
    this->index = (Index)-1;
    this->id = Uuid::GetZero();

    if (s.IsDigits())
        this->index = Convert::ToInteger(s, this->index);
    else
    {
        auto splitIndex = s.find('|');
        if (splitIndex == Utf8String::npos)
            this->name = s;
        else
        {
            s.substr(this->name, 0, splitIndex);

            Utf8String idString;
            s.substr(idString, splitIndex + 1);
            this->id = Uuid(idString);
        }
    }
}

bool ParsedChunkName::operator == (const ParsedChunkName& other) const
{
    if (!this->id.IsZero() && !other.id.IsZero())
        return this->id == other.id;
    else if (this->id.IsZero() != other.id.IsZero())
        return false;
    else if (!this->name.empty() && !other.name.empty())
        return this->name == other.name;
    else if (this->name.empty() != other.name.empty())
        return false;
    else if (this->index != (Index)-1 && other.index != (Index)-1)
        return this->index == other.index;
    else
        return false;
}

bool ParsedChunkName::operator == (const ChunkName& other) const
{
    return other == *this;
}

bool ParsedChunkName::operator != (const ParsedChunkName& other) const
{
    return !(*this == other);
}

bool ParsedChunkName::operator != (const ChunkName& other) const
{
    return !(*this == other);
}

bool ParsedChunkName::IsValid() const
{
    return
        (this->index != (Index)-1) ||
        !this->id.IsZero() ||
        (!this->name.empty() && this->name.length() <= ChunkName::NAME_MAX_LENGTH)
        ;
}

Utf8String ParsedChunkName::ToString() const
{
    Utf8String s = this->name;
    if (!this->id.IsZero())
    {
        if (!s.empty())
            s += "|";
        s += this->id.ToString();
    }
    else if (this->index != (Index)-1)
    {
        if (!s.empty())
            s += "|";
        s += Convert::ToString(this->index);
    }
    return s;
}

//ChunkPropertyName
ChunkPropertyName::ChunkPropertyName() : name(nullptr), id((ID)-1), index((Index)-1)
{
}

ChunkPropertyName::ChunkPropertyName(const char* _name) : name(_name), id(ChunkPropertyHash<ID>(_name)), index((Index)-1)
{
}

ChunkPropertyName::ChunkPropertyName(const char* _name, ID _id) : name(_name), id(_id), index((Index)-1)
{
}

ChunkPropertyName::ChunkPropertyName(size_t _index) : name(nullptr), id((ID)-1), index(static_cast<Index>(_index))
{
}

bool ChunkPropertyName::IsValid() const
{
    return
        (this->index != (Index)-1) ||
        (this->name != nullptr && this->id != 0)
        ;
}

bool ChunkPropertyName::operator == (const ChunkPropertyName& other) const
{
    return this->id == other.id;
}

bool ChunkPropertyName::operator == (const Utf8String& other) const
{
    return other == this->name;
}

bool ChunkPropertyName::operator == (ID id) const
{
    return this->id == id;
}

bool ChunkPropertyName::operator == (const ParsedChunkPropertyName& other) const
{
    return this->id == other.id;
}

bool ChunkPropertyName::operator != (const ChunkPropertyName& other) const
{
    return this->id != other.id;
}

bool ChunkPropertyName::operator != (const Utf8String& other) const
{
    return other != this->name;
}

bool ChunkPropertyName::operator != (ID id) const
{
    return this->id != id;
}

bool ChunkPropertyName::operator != (const ParsedChunkPropertyName& other) const
{
    return this->id != other.id;
}

Utf8String ChunkPropertyName::ToString() const
{
    Utf8String s = this->name;
    if (this->id != (Index)-1 && this->id != 0)
    {
        if (!s.empty())
            s += "|";
        s += Convert::ToString(this->id);
    }
    else if (this->index != (Index)-1)
    {
        if (!s.empty())
            s += "|";
        s += Convert::ToString(this->index);
    }
    return s;
}

//ParsedChunkPropertyName
ParsedChunkPropertyName::ParsedChunkPropertyName()
{
    this->id = (ID)-1;
    this->index = (Index)-1;
}

ParsedChunkPropertyName::ParsedChunkPropertyName(const Utf8String& s)
{
    Parse(s);
}

ParsedChunkPropertyName::ParsedChunkPropertyName(size_t _index) : id((ID)-1), index(static_cast<Index>(_index))
{
}

void ParsedChunkPropertyName::Parse(const Utf8String& s)
{
    this->name.clear();
    this->id = (ID)-1;
    this->index = (Index)-1;

    if (s.IsDigits())
        this->index = Convert::ToInteger(s, this->index);
    else
    {
        this->name = s;
        this->id = ChunkPropertyHash<ID>(s.c_str());
    }
}

bool ParsedChunkPropertyName::operator == (const ChunkPropertyName& other) const
{
    if (this->id != (ID)-1)
        return this->id == other.id;
    else
        return this->name == other.name;
}

bool ParsedChunkPropertyName::operator != (const ChunkPropertyName& other) const
{
    if (this->id != (ID)-1)
        return this->id != other.id;
    else
        return this->name != other.name;
}

bool ParsedChunkPropertyName::IsValid() const
{
    return
        this->index != (Index)-1 ||
        !this->name.empty() ||
        this->id != (ID)-1
        ;
}

Utf8String ParsedChunkPropertyName::ToString() const
{
    Utf8String s = this->name;
    if (this->id != (Index)-1 && this->id != 0)
    {
        if (!s.empty())
            s += "|";
        s += Convert::ToString(this->id);
    }
    else if (this->index != (Index)-1)
    {
        if (!s.empty())
            s += "|";
        s += Convert::ToString(this->index);
    }
    return s;
}

//DataChunkBlobTextFormatUtilities
const char* DataChunkBlobTextFormatUtilities::ToString(DataChunkBlobTextFormat value)
{
    switch (value)
    {
        case DataChunkBlobTextFormat::BYTE_ARRAY: return "byte-array";
        case DataChunkBlobTextFormat::BASE64: return "base64";
        default: return FINJIN_ENUM_UNKNOWN_STRING;
    }
}

DataChunkBlobTextFormat DataChunkBlobTextFormatUtilities::Parse(const Utf8String& value)
{
    if (value == "byte-array")
        return DataChunkBlobTextFormat::BYTE_ARRAY;
    else if (value == "base64")
        return DataChunkBlobTextFormat::BASE64;
    else
        return DataChunkBlobTextFormat::BYTE_ARRAY;
}
