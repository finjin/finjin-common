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
#include "finjin/common/NamePrefix.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
NamePrefix::NamePrefix()
{
}

NamePrefix::NamePrefix(const NamePrefix& other)
{
    this->parts = other.parts;
}

NamePrefix::NamePrefix(NamePrefix&& other)
{
    this->parts.resize(other.parts.size());
    for (size_t i = 0; i < other.parts.size(); i++)
        this->parts[i] = std::move(other.parts[i]);
    other.parts.clear();
}

NamePrefix::NamePrefix(const Utf8String& prefix)
{
    if (!prefix.empty())
        this->parts.push_back(prefix);
}

NamePrefix& NamePrefix::operator = (const NamePrefix& other)
{
    if (this != &other)
        this->parts = other.parts;

    return *this;
}

NamePrefix& NamePrefix::operator = (NamePrefix&& other)
{
    if (this != &other)
    {
        this->parts.resize(other.parts.size());
        for (size_t i = 0; i < other.parts.size(); i++)
            this->parts[i] = std::move(other.parts[i]);
        other.parts.clear();
    }

    return *this;
}

NamePrefix& NamePrefix::operator = (const Utf8String& prefix)
{
    this->parts.clear();
    if (!prefix.empty())
        this->parts.push_back(prefix);

    return *this;
}

NamePrefix NamePrefix::operator + (const Utf8String& prefix) const
{
    NamePrefix result = *this;
    result += prefix;
    return result;
}

NamePrefix NamePrefix::operator + (const NamePrefix& prefix) const
{
    NamePrefix result = *this;
    result += prefix;
    return result;
}

NamePrefix& NamePrefix::operator += (const Utf8String& prefix)
{
    if (!prefix.empty())
        this->parts.push_back(prefix);

    return *this;
}

NamePrefix& NamePrefix::operator += (const NamePrefix& other)
{
    for (auto& otherPrefix : other.parts)
        this->parts.push_back(otherPrefix);

    return *this;
}

Utf8String NamePrefix::ToString(char joinChar) const
{
    Utf8String result;

    for (auto& prefix : this->parts)
    {
        result += prefix;
        result += joinChar; //Yes, the final string will have the join character at the end
    }

    return result;
}

Utf8String NamePrefix::ToCommandLineString() const
{
    return ToString('.');
}

Utf8String NamePrefix::ToPathString() const
{
    return ToString('/');
}
