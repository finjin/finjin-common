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
#include "WxChunkName.hpp"

using namespace Finjin::Common;


//Static initialization---------------------------------------------------------
const WxChunkPropertyName WxStandardChunkPropertyNames::SIGNATURE("magic");
const WxChunkPropertyName WxStandardChunkPropertyNames::FORMAT("format");
const WxChunkPropertyName WxStandardChunkPropertyNames::FORMAT_VERSION("format-version");
const WxChunkPropertyName WxStandardChunkPropertyNames::MAX_BYTES_PER_LINE("max-bytes-per-line");
const WxChunkPropertyName WxStandardChunkPropertyNames::BLOB_TEXT_FORMAT("blob-text-format");


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

//WxChunkName
#if wxUSE_UNICODE
WxChunkName::WxChunkName(const char* _name) : name(wxString::FromUTF8(_name)), index((Index)-1)
{
}

WxChunkName::WxChunkName(const char* _name, const WxUuid& _id) : name(wxString::FromUTF8(_name)), id(_id), index((Index)-1)
{
}
#endif

WxChunkName::WxChunkName(const wchar_t* _name) : name(_name), index((Index)-1)
{
}

WxChunkName::WxChunkName(const wchar_t* _name, const WxUuid& _id) : name(_name), id(_id), index((Index)-1)
{
}

WxChunkName::WxChunkName(size_t _index) : index(static_cast<Index>(_index))
{
}

WxChunkName::WxChunkName(const WxChunkName& other, size_t _index) : name(other.name), id(other.id), index(static_cast<Index>(_index))
{
}

bool WxChunkName::IsValid() const
{
    return
        (this->index != (Index)-1) ||
        !id.IsZero() ||
        (!this->name.empty() && this->name.Length() <= NAME_MAX_LENGTH)
        ;
}

bool WxChunkName::operator == (const WxChunkName& other) const
{
    return this->id == other.id;
}

bool WxChunkName::operator == (const wxString& other) const
{
    return other == this->name;
}

bool WxChunkName::operator == (const WxUuid& other) const
{
    return other == this->id;
}

wxString WxChunkName::ToString() const
{
    wxString s = this->name;
    if (!this->id.IsZero())
    {
        if (!s.empty())
            s += wxT("|");
        s += this->id.ToString();
    }
    else if (this->index != (Index)-1)
    {
        if (!s.empty())
            s += wxT("|");
        s << this->index;
    }
    return s;
}

//WxChunkPropertyName
WxChunkPropertyName::WxChunkPropertyName() : name(nullptr), id((ID)-1), index((Index)-1)
{
}

WxChunkPropertyName::WxChunkPropertyName(const char* s) : name(s), id(ChunkPropertyHash<ID>(s)), index((Index)-1)
{
}

WxChunkPropertyName::WxChunkPropertyName(size_t _index) : name(nullptr), id(0), index(static_cast<Index>(_index))
{
}

bool WxChunkPropertyName::IsValid() const
{
    return
        (this->index != (Index)-1) ||
        (this->name != nullptr && this->id != 0)
        ;
}

bool WxChunkPropertyName::operator == (const WxChunkPropertyName& other) const
{
    return this->id == other.id;
}

bool WxChunkPropertyName::operator != (const WxChunkPropertyName& other) const
{
    return this->id != other.id;
}

wxString WxChunkPropertyName::ToString() const
{
    wxString s;
    if (this->name != nullptr)
        s = wxString::FromUTF8(this->name);
    if (this->id != (ID)-1)
    {
        if (!s.empty())
            s += wxT("|");
        s << this->id;
    }
    else if (this->index != (Index)-1)
    {
        if (!s.empty())
            s += wxT("|");
        s << this->index;
    }
    return s;
}
