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
#include "finjin/common/Allocator.hpp"

using namespace Finjin::Common;


//Locals------------------------------------------------------------------------
static Allocator* defaultAllocator = nullptr;


//Implementation----------------------------------------------------------------
const Utf8String& Allocator::GetName() const
{
    return this->name;
}

size_t Allocator::GetEnvironmentAlignment()
{
    struct AlignmentStruct { char c; double d; };
    static const size_t alignment = offsetof(AlignmentStruct, d);
    return alignment;
}

size_t Allocator::GetDefaultAlignment()
{
    //The default environment alignment is usually 8, but some SSE types require 16 byte alignment
    return std::max((size_t)16, GetEnvironmentAlignment());
}

size_t Allocator::GetBestAlignment(size_t alignment)
{
    return std::max(alignment, GetDefaultAlignment());
}

Allocator* Allocator::GetDefaultAllocator()
{
    return defaultAllocator;
}

void Allocator::SetDefaultAllocator(Allocator* allocator)
{
    defaultAllocator = allocator;
}

void* Allocator::SystemAllocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    auto mem = malloc(byteCount);
    if (mem != nullptr)
        FINJIN_ZERO_MEMORY(mem, byteCount);
    return mem;
}

void Allocator::SystemDeallocate(void* mem)
{
    free(mem);
}
