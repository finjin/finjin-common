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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Allocator.hpp"

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------

//AllocatedClass
AllocatedClass::AllocatedClass(Allocator* allocator)
{
    this->allocator = allocator;
}

AllocatedClass::~AllocatedClass()
{
}

Allocator* AllocatedClass::GetAllocator()
{
    return this->allocator;
}

void AllocatedClass::SetAllocator(Allocator* allocator)
{
    assert(this->allocator == nullptr);
    if (this->allocator == nullptr)
        this->allocator = allocator;
}

void* AllocatedClass::operator new(size_t byteCount, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    void* mem = nullptr;
    if (allocator != nullptr)
        mem = allocator->Allocate(byteCount, FINJIN_CALLER_PARAMETERS);
    else
        mem = Allocator::SystemAllocate(byteCount, FINJIN_CALLER_PARAMETERS);
    assert(mem != nullptr);
    return mem;
}

void AllocatedClass::operator delete(void* mem)
{
    auto obj = static_cast<AllocatedClass*>(mem);
    auto allocator = obj->GetAllocator();
    if (allocator != nullptr)
        allocator->Deallocate(obj);
    else
        Allocator::SystemDeallocate(obj);
}

void AllocatedClass::operator delete(void* mem, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    if (allocator != nullptr)
        allocator->Deallocate(mem);
    else
        Allocator::SystemDeallocate(mem);
}
