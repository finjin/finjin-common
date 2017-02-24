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


#pragma once


//Includes---------------------------------------------------------------------
#include "finjin/common/Utf8String.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class ByteBufferReader;
    class Error;

    struct MemoryArenaSettings
    {
        MemoryArenaSettings();

        void ParseSettings(const ByteBufferReader& configFileBuffer, Error& error);

        size_t byteCount;
        size_t alignment;
    };

    struct NamedMemoryArenaSettings : MemoryArenaSettings
    {
        Utf8String name;
    };

    //Tracks a block of memory
    //_MemoryType will be something like 'uint8_t*' (not void* or any other pointer type), 'uint64_t', 'intptr_t', or some other integer type
    template <typename _MemoryType, _MemoryType MemoryNull>
    struct MemoryArena
    {
        using MemoryType = _MemoryType;

        const MemoryType memoryNull = MemoryNull; //This is an instance variable

        MemoryArena()
        {
        }

        MemoryArena(const MemoryArena& other) = delete;
        MemoryArena& operator = (const MemoryArena& other) = delete;

        MemoryArena(MemoryArena&& other)
        {
            this->rawMemory = std::move(other.rawMemory);
            this->alignedMemory = std::move(other.alignedMemory);

            this->deallocateMemory = std::move(other.deallocateMemory);
        }

        ~MemoryArena()
        {
            if (this->deallocateMemory != nullptr)
                this->deallocateMemory(this->rawMemory.ptr);
        }

        MemoryArena& operator = (MemoryArena&& other)
        {
            this->rawMemory = std::move(other.rawMemory);
            this->alignedMemory = std::move(other.alignedMemory);

            this->deallocateMemory = std::move(other.deallocateMemory);

            return *this;
        }

        bool IsNull() const
        {
            return this->rawMemory.ptr == MemoryNull || this->alignedMemory.ptr == MemoryNull;
        }

        struct Block
        {
            Block()
            {
                Reset();
            }

            Block(Block&& other)
            {
                this->ptr = other.ptr;
                this->size = other.size;
                this->alignment = other.alignment;

                other.ptr = MemoryNull;
                other.size = 0;
                other.alignment = 0;
            }

            Block& operator = (Block&& other)
            {
                this->ptr = other.ptr;
                this->size = other.size;
                this->alignment = other.alignment;

                other.ptr = MemoryNull;
                other.size = 0;
                other.alignment = 0;

                return *this;
            }

            void Reset()
            {
                this->ptr = MemoryNull;
                this->size = 0;
                this->alignment = 0;
            }

            bool IsValidPointer(MemoryType mem) const
            {
                return (mem >= this->ptr) && (mem < this->ptr + this->size);
            }

            bool IsValidRange(MemoryType mem, size_t size) const
            {
                return IsValidPointer(mem) && IsValidPointer(mem + size - 1);
            }

            MemoryType ptr;
            size_t size;
            size_t alignment;
        };

        template <typename DeallocateMemory>
        void Create(Block&& rawMemory, Block&& alignedMemory, DeallocateMemory deallocateMemory)
        {
            Destroy();

            this->rawMemory = std::move(rawMemory);
            this->alignedMemory = std::move(alignedMemory);

            this->deallocateMemory = deallocateMemory;
        }

        void Destroy()
        {
            if (this->deallocateMemory != nullptr)
                this->deallocateMemory(this->rawMemory.ptr);

            this->rawMemory.Reset();
            this->alignedMemory.Reset();
            this->deallocateMemory = nullptr;
        }

        Block rawMemory;
        Block alignedMemory;
        std::function<void(MemoryType&)> deallocateMemory;
    };

    using ByteMemoryArena = MemoryArena<uint8_t*, (uint8_t*)0>;

} }
