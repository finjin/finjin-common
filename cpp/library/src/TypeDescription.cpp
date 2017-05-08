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
#include "finjin/common/TypeDescription.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/common/StaticQueue.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "finjin/common/StaticVector.hpp"

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
using GlobalTypeDescriptions = StaticVector<TypeDescription*, CommonConstants::MAX_TYPE_DESCRIPTIONS>;
using GlobalTypeDescriptionsByName = StaticUnorderedMap<size_t, TypeDescription*, CommonConstants::MAX_TYPE_DESCRIPTIONS, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(CommonConstants::MAX_TYPE_DESCRIPTIONS), MapPairConstructNone<size_t, TypeDescription*>, PassthroughHash>;
using TypeDescriptionSearchQueue = StaticQueue<TypeDescription*, CommonConstants::MAX_TYPE_DESCRIPTIONS * 2>;


//Local functions---------------------------------------------------------------
static GlobalTypeDescriptions& GetAllTypes()
{
    static GlobalTypeDescriptions allTypes;
    return allTypes;
}

static GlobalTypeDescriptionsByName& GetTypesByName()
{
    static GlobalTypeDescriptionsByName typesByName;
    return typesByName;
}

static GlobalTypeDescriptions& GetRootTypes()
{
    static GlobalTypeDescriptions rootClasses;
    return rootClasses;
}


//Implementation----------------------------------------------------------------
TypeDescription::TypeDescription(const char* typeName, TypeDescription* parentType, TypeDescription* parentType2)
{
    this->typeName = typeName;

    this->superTypeHead = this->superTypeNext = nullptr;
    this->subTypeHead = this->subTypeNext = nullptr;

    //Add type description to local lookups
    if (parentType != nullptr)
    {
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(parentType->subTypeHead, subTypeNext, this);
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->superTypeHead, superTypeNext, parentType);
    }

    if (parentType2 != nullptr)
    {
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(parentType2->subTypeHead, subTypeNext, this);
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->superTypeHead, superTypeNext, parentType2);
    }

    //Add type description to global lookups
    auto& all = GetAllTypes();
    assert(!all.full());
    if (all.push_back(this).HasErrorOrValue(false))
        return;

    auto& allByName = GetTypesByName();
    assert(!allByName.full());
    if (allByName.insert(GetName().GetHash(), this, false).HasErrorOrValue(false))
        return;

    if (IsRoot())
    {
        assert(!GetRootTypes().full());
        if (GetRootTypes().push_back(this).HasErrorOrValue(false))
            return;
    }
}

TypeDescription::~TypeDescription()
{
}

TypeDescription** TypeDescription::GetAll(size_t* count)
{
    if (count != nullptr)
        *count = GetAllTypes().size();
    return GetAllTypes().data();
}

const Utf8String& TypeDescription::GetName() const
{
    return this->typeName;
}

bool TypeDescription::IsTypeOf(const TypeDescription& other) const
{
    if (this == &other)
        return true;

    return IsSubTypeOf(other);
}

bool TypeDescription::IsSubTypeOf(const TypeDescription& other) const
{
    if (this == &other)
        return false;

    //Try to find a parent TypeDescription that matches 'other'

    TypeDescriptionSearchQueue searchQueue; //Breadth-first-search queue
    FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->superTypeHead, superTypeNext, parentType)
    {
        if (searchQueue.push(parentType).HasValue(false))
        {
            assert(0 && "The type description search queue became full while adding parents. The search cannot be completed.");
            break;
        }
    }

    TypeDescription* typeDescription;
    while (!searchQueue.empty())
    {
        searchQueue.pop(typeDescription);

        if (typeDescription == &other)
            return true;

        FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(typeDescription->superTypeHead, superTypeNext, parentType)
        {
            if (searchQueue.push(parentType).HasErrorOrValue(false))
            {
                assert(0 && "The type description search queue became full while searching parents. The search cannot be completed.");
                break;
            }
        }
    }

    return false;
}

bool TypeDescription::IsRoot() const
{
    return this->superTypeHead == nullptr;
}

TypeDescription* TypeDescription::Find(const Utf8String& typeName)
{
    auto& allByName = GetTypesByName();
    auto nameToTypeDescription = allByName.find(typeName.GetHash());
    if (nameToTypeDescription != allByName.end())
    {
        assert(nameToTypeDescription->second->GetName() == typeName); //It is EXTREMELY unlikely that this will fail, but test just in case
        return nameToTypeDescription->second;
    }
    else
        return nullptr;
}

bool TypeDescription::IsAbstract() const
{
    return true;
}

void* TypeDescription::CreateInstance(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION) const
{
    assert(0 && "CreateInstance() called on base type.");
    return nullptr;
}

void TypeDescription::DestroyInstance(void* genericInstance) const
{
    assert(0 && "DestroyInstance() called on base type.");
}

void* TypeDescription::UpcastInstance(void* genericInstance, TypeDescription& typeDescription) const
{
    assert(0 && "UpcastInstance() called on base type.");
    return nullptr;
}
