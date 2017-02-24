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
#include "finjin/common/StringKeyValueMap.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
StringKeyValueMap::StringKeyValueMap()
{
}

StringKeyValueMap::StringKeyValueMap(const StringKeyValueMap& other)
{
    this->map = other.map;
    this->vect = other.vect;
}

StringKeyValueMap::StringKeyValueMap(StringKeyValueMap&& other)
{
    this->map = std::move(other.map);
    this->vect = std::move(other.vect);
}

ValueOrError<void> StringKeyValueMap::operator = (const StringKeyValueMap& other)
{
    if (this != &other)
    {
        this->map = other.map;
        this->vect = other.vect;
    }

    return ValueOrError<void>();
}

ValueOrError<void> StringKeyValueMap::operator = (StringKeyValueMap&& other)
{
    if (this != &other)
    {
        this->map = std::move(other.map);
        this->vect = std::move(other.vect);
    }

    return ValueOrError<void>();
}

StringKeyValueMap StringKeyValueMap::operator + (const StringKeyValueMap& other) const
{
    StringKeyValueMap combined = *this;
    combined += other;
    return combined;
}

ValueOrError<void> StringKeyValueMap::operator += (const StringKeyValueMap& other)
{
    for (const auto& keyValue : other.vect)
    {
        if (Set(keyValue.first, keyValue.second).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

size_t StringKeyValueMap::size() const
{
    return this->vect.size();
}

bool StringKeyValueMap::empty() const
{
    return this->vect.empty();
}

bool StringKeyValueMap::IsSet() const
{
    return !this->map.empty();
}

void StringKeyValueMap::clear()
{
    this->map.clear();
    this->vect.clear();
}

const Utf8String& StringKeyValueMap::GetKey(size_t i) const
{
    return this->vect[i].first;
}

const Utf8String& StringKeyValueMap::GetValue(size_t i) const
{
    return this->vect[i].second;
}

StringKeyValueMap::Value StringKeyValueMap::operator [] (const Utf8String& key)
{
    return Value(this, key);
}

const Utf8String& StringKeyValueMap::operator [] (const Utf8String& key) const
{
    return Get(key);
}

bool StringKeyValueMap::Get(const Utf8String& key, Utf8String& value) const
{
    auto foundAt = this->map.find(key);
    if (foundAt != this->map.end())
    {
        value = foundAt->second;
        return true;
    }
    else
        return false;
}

const Utf8String& StringKeyValueMap::Get(const Utf8String& key) const
{
    auto foundAt = this->map.find(key);    
    if (foundAt != this->map.end())
        return foundAt->second;
    else
        return Utf8String::Empty();
}

ValueOrError<void> StringKeyValueMap::Set(const Utf8String& key, const Utf8String& value)
{
    auto foundAt = this->map.find(key);
    if (foundAt == this->map.end())
    {
        if (this->map[key].assign(value).HasError())
            return ValueOrError<void>::CreateError();
    
        this->vect.push_back(std::make_pair(key, value));
    }
    else
    {
        if (foundAt->second.assign(value).HasError())
            return ValueOrError<void>::CreateError();

        for (auto& item : this->vect)
        {
            if (item.first == key)
            {
                if (item.second.assign(value).HasError())
                    return ValueOrError<void>::CreateError();
                break;
            }
        } 
    }

    return ValueOrError<void>();
}

ValueOrError<void> StringKeyValueMap::Set(const StringKeyValueMap& other)
{
    for (size_t i = 0; i < other.size(); i++)
    {
        if (Set(other.GetKey(i), other.GetValue(i)).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

ValueOrError<void> StringKeyValueMap::Concat(const Utf8String& key, const Utf8String& value)
{
    auto foundAt = this->map.find(key);
    if (foundAt == this->map.end())
    {
        if (this->map[key].assign(value).HasError())
            return ValueOrError<void>::CreateError();
    
        this->vect.push_back(std::make_pair(key, value));
    }
    else
    {
        if (foundAt->second.append(value).HasError())
            return ValueOrError<void>::CreateError();

        for (auto& item : this->vect)
        {
            if (item.first == key)
            {
                if (item.second.append(value).HasError())
                    return ValueOrError<void>::CreateError();
                break;
            }
        } 
    }

    return ValueOrError<void>();
}
    
bool StringKeyValueMap::Remove(const Utf8String& key)
{
    auto foundAt = this->map.find(key);    
    if (foundAt != this->map.end())
    {
        this->map.erase(foundAt);
        this->vect.erase(std::find_if(this->vect.begin(), this->vect.end(), [&key] (const KeyValue& item) {return item.first == key;}));
        return true;
    }
    else
        return false;
}

bool StringKeyValueMap::HasKey(const Utf8String& key) const
{
    return this->map.find(key) != this->map.end();
}

Utf8String StringKeyValueMap::JoinKeys(const Utf8String& joinText) const
{
    Utf8String result;
    JoinKeys(result, joinText);
    return result;
}

ValueOrError<void> StringKeyValueMap::JoinKeys(Utf8String& result, const Utf8String& joinText) const
{
    result.clear();

    for (size_t i = 0; i < this->vect.size(); i++)
    {
        if (i > 0)
        {
            if (result.append(joinText).HasError())
                return ValueOrError<void>::CreateError();
        }

        if (result.append(this->vect[i].first).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

Utf8String StringKeyValueMap::JoinValues(const Utf8String& joinText) const
{
    Utf8String result;
    JoinValues(result, joinText);
    return result;
}

ValueOrError<void> StringKeyValueMap::JoinValues(Utf8String& result, const Utf8String& joinText) const
{
    result.clear();

    for (size_t i = 0; i < this->vect.size(); i++)
    {
        if (i > 0)
        {
            if (result.append(joinText).HasError())
                return ValueOrError<void>::CreateError();
        }

        if (result.append(this->vect[i].second).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

Utf8String StringKeyValueMap::ToString() const
{
    Utf8String result;
    ToString(result);
    return result;
}

ValueOrError<void> StringKeyValueMap::ToString(Utf8String& result) const
{
    result.clear();

    for (size_t i = 0; i < this->vect.size(); i++)
    {
        if (i > 0)
        {
            if (result.append(", ").HasError())
                return ValueOrError<void>::CreateError();
        }

        if (result.append(this->vect[i].first).HasError())
            return ValueOrError<void>::CreateError();
        if (result.append("=").HasError())
            return ValueOrError<void>::CreateError();
        if (result.append(this->vect[i].second).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}
