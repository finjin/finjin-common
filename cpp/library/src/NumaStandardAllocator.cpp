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
#include "finjin/common/NumaStandardAllocator.hpp"
#include <Windows.h>
#include <iostream>

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
NumaStandardAllocator::NumaStandardAllocator()
{
    this->nodeID = 0;
    
    this->DoAllocate = [this](size_t byteCount) 
    {
        //std::cout << this << " numa allocate: " << byteCount << std::endl;

        void* mem = ::VirtualAllocExNuma(::GetCurrentProcess(), nullptr, static_cast<SIZE_T>(byteCount), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE, static_cast<DWORD>(this->nodeID));
        if (mem == nullptr)
            return mem;

        //Force memory into working set by zeroing contents
        ZeroMemory(mem, static_cast<size_t>(byteCount));
        return mem;
    };

    this->DoFree = [this](void* mem) 
    {
        //std::cout << this << " numa free ptr: " << mem << std::endl;
        
        ::VirtualFree(mem, 0, MEM_RELEASE);
    };
}

NumaStandardAllocator::~NumaStandardAllocator()
{    
}

size_t NumaStandardAllocator::GetNodeID() const
{
    return this->nodeID;
}

void NumaStandardAllocator::SetNodeID(size_t value)
{
    this->nodeID = value;
}

size_t NumaStandardAllocator::GetNodeBytesFree() const
{
    ULONGLONG availableBytes = 0;
    ::GetNumaAvailableMemoryNodeEx(static_cast<USHORT>(this->nodeID), &availableBytes);    
    return static_cast<size_t>(availableBytes);
}
