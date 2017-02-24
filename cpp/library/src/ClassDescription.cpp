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
#include "finjin/common/ClassDescription.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/common/StaticQueue.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "finjin/common/StaticVector.hpp"

using namespace Finjin::Common;


//Local types------------------------------------------------------------------
using GlobalClassDescriptions = StaticVector<ClassDescription*, CommonConstants::MAX_CLASS_DESCRIPTION_CLASSES>;
using GlobalClassDescriptionsByName = StaticUnorderedMap<size_t, ClassDescription*, CommonConstants::MAX_CLASS_DESCRIPTION_CLASSES, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(CommonConstants::MAX_CLASS_DESCRIPTION_CLASSES), MapPairConstructNone<size_t, ClassDescription*>, PassthroughHash>;
using ClassDescriptionSearchQueue = StaticQueue<ClassDescription*, CommonConstants::MAX_CLASS_DESCRIPTION_CLASSES * 2>;

static GlobalClassDescriptions& GetAllClasses()
{
    static GlobalClassDescriptions allClasses;
    return allClasses;
}

static GlobalClassDescriptionsByName& GetClassesByName()
{
    static GlobalClassDescriptionsByName classesByName;
    return classesByName;
}

static GlobalClassDescriptions& GetRootClasses()
{
    static GlobalClassDescriptions rootClasses;
    return rootClasses;
}


//Implementation----------------------------------------------------------------
ClassDescription::ClassDescription(const char* typeName, ClassDescription* parentClass, ClassDescription* parentClass2)
{
    this->typeName = typeName;

    this->parentClassHead = this->parentClassNext = nullptr;
    this->subclassHead = this->subclassNext = nullptr;

    //Add class description to local lookups
    if (parentClass != nullptr)
    {   
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(parentClass->subclassHead, subclassNext, this);
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->parentClassHead, parentClassNext, parentClass);
    }
    
    if (parentClass2 != nullptr)
    {
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(parentClass2->subclassHead, subclassNext, this);
        FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->parentClassHead, parentClassNext, parentClass2);
    }

    //Add class description to global lookups
    auto& all = GetAllClasses();
    assert(!all.full());
    if (all.push_back(this).HasErrorOrValue(false))
        return;

    auto& allByName = GetClassesByName();
    assert(!allByName.full());
    if (allByName.insert(GetName().GetHash(), this, false).HasErrorOrValue(false))
        return;

    if (IsRoot())
    {
        assert(!GetRootClasses().full());
        if (GetRootClasses().push_back(this).HasErrorOrValue(false))
            return;
    }
}

ClassDescription::~ClassDescription() 
{
}

ClassDescription** ClassDescription::GetAll(size_t* count)
{
    if (count != nullptr)
        *count = GetAllClasses().size();
    return GetAllClasses().data();
}

const Utf8String& ClassDescription::GetName() const 
{ 
    return this->typeName; 
}    

bool ClassDescription::IsTypeOf(const ClassDescription& other) const
{
    if (this == &other)
        return true;

    return IsSubclassOf(other);    
}

bool ClassDescription::IsSubclassOf(const ClassDescription& other) const
{
    if (this == &other)
        return false;

    //Try to find a parent ClassDescription that matches 'other'

    ClassDescriptionSearchQueue searchQueue; //Breadth-first-search queue
    FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->parentClassHead, parentClassNext, parentClass)
    {
        if (searchQueue.push(parentClass).HasValue(false))
        {
            assert(0 && "The class description search queue became full while adding parents. The search cannot be completed.");
            break;
        }
    }

    ClassDescription* classDescription;
    while (!searchQueue.empty())
    {
        searchQueue.pop(classDescription);

        if (classDescription == &other)
            return true;

        FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(classDescription->parentClassHead, parentClassNext, parentClass)
        {
            if (searchQueue.push(parentClass).HasErrorOrValue(false))
            {
                assert(0 && "The class description search queue became full while searching parents. The search cannot be completed.");
                break;
            }
        }
    }
    
    return false;
}

bool ClassDescription::IsRoot() const
{
    return this->parentClassHead == nullptr;
}

ClassDescription* ClassDescription::Find(const Utf8String& className)
{
    auto& allByName = GetClassesByName();
    auto nameToClassDescription = allByName.find(className.GetHash());
    if (nameToClassDescription != allByName.end())
    {
        assert(nameToClassDescription->second->GetName() == className); //It is EXTREMELY unlikely that this will fail, but test just in case
        return nameToClassDescription->second;
    }
    else
        return nullptr;
}

bool ClassDescription::IsAbstract() const 
{
    return true;
}

void* ClassDescription::CreateInstance(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION) const
{
    assert(0 && "CreateInstance() called on base class.");
    return nullptr;
}

void ClassDescription::DestroyInstance(void* genericInstance) const
{
    assert(0 && "DestroyInstance() called on base class.");
}

void* ClassDescription::UpcastInstance(void* genericInstance, ClassDescription& classDescription) const
{
    assert(0 && "UpcastInstance() called on base class.");
    return nullptr; 
}
