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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/SimpleSpinLockMutex.hpp"
#include <mutex>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * It works by allocating a big chunk of memory initially, slicing off memory as necessary
     * This allocator is thread safe. However, it should not be used within the job system, since it uses a
     * a standard mutex for synchronization, which will cause problems with the job system's threads/fibers.
     */
    class FINJIN_COMMON_LIBRARY_API GeneralAllocator : public Allocator
    {        
    public:
        GeneralAllocator(const GeneralAllocator& other) = delete;
        GeneralAllocator& operator = (const GeneralAllocator& other) = delete;

        GeneralAllocator(GeneralAllocator&& GeneralAllocator) = delete;
        GeneralAllocator& operator = (GeneralAllocator&& other) = delete;

    public:    
        GeneralAllocator();
        ~GeneralAllocator();    

        struct Settings
        {
            Settings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, Error& error);
        };

        void Create(const Settings& settings, ByteMemoryArena&& arena);

        /** 
         * Sets the number of bytes the allocator is to manage.
         * Note that calling this wipes out whatever data the allocator
         * had previously allocated.
         */
        void Create(const Settings& settings, ByteMemoryArena&& arena, Error& error);

        /**
         * Allocates a chunk of memory of the specified number of bytes.
         * @param byteCount [in] - The number of bytes to allocate.
         * @return A pointer to the allocated memory is returned if it was possible
         * to allocate the specified number of bytes. Otherwise, null is returned.
         */
        void* Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION) override;

        bool CanDeallocateBlock() const override;
        
        /**
         * Frees the memory that was previously allocated by the allocator.
         * @param mem [in] - Pointer to memory that was previously allocated by the 
         * allocator.
         */
        void Deallocate(void* mem) override;

        bool CanDeallocateAll() const override;
        
        /**
         * Frees all the memory allocated by the allocator.
         */
        void DeallocateAll() override;

        /** Gets the number of bytes allocated. */
        size_t GetBytesUsed() const override;

        /** Gets the number of bytes free. */
        size_t GetBytesFree() const override;

        size_t GetAlignment() const override;
        
        void DebugCheck();

        void Output(std::ostream& out, bool forward = true);
        
    private:
        void _DebugCheck();
        
        struct MemoryBlockHeader
        {
            //This struct is 16 bytes on 32-bit, 32 bytes on 64-bit

            size_t size; //Number of bytes allocated including the size of this structure
            MemoryBlockHeader* previous;
            MemoryBlockHeader* next;
            size_t padding; //Pads the struct
            
        #if FINJIN_DEBUG
            const char* fileName;
            const char* functionName;
            size_t line;
            size_t padding2;
        #endif

            uint8_t* GetByteStart() { return reinterpret_cast<uint8_t*>(this); }
            uint8_t* GetByteEnd() { return reinterpret_cast<uint8_t*>(this) + this->size; }
        };

        size_t GetAlignedMemberBlockHeaderSize() const;

        MemoryBlockHeader* FindFirstFreeFit(size_t byteCount);
        
        void _Init();
        void _FreeAllBlocks();

        MemoryBlockHeader* GetHeader(void* mem);
        void* GetMemory(MemoryBlockHeader* header);

    private:    
        ByteMemoryArena arena;
        
        struct BlockRange
        {
            MemoryBlockHeader* head;
            MemoryBlockHeader* tail;

            BlockRange()
            {
                this->head = this->tail = nullptr;
            }

            BlockRange(MemoryBlockHeader* first, MemoryBlockHeader* second)
            {
                this->head = first;
                this->tail = second;
            }

            void AddOrCombineWithTail(MemoryBlockHeader* blockHeader)
            {
                if (this->tail != nullptr)
                {
                    if (this->tail->GetByteEnd() == blockHeader->GetByteStart())
                        this->tail->size += blockHeader->size;
                    else
                    {
                        this->tail->next = blockHeader;
                        blockHeader->previous = this->tail;

                        this->tail = blockHeader;
                    }
                }
                else
                    this->head = this->tail = blockHeader;
            }

            bool empty() const
            {
                return this->head == nullptr;
            }

            void clear()
            {
                this->head = this->tail = nullptr;
            }
            
            size_t GetTotal() const
            {
                size_t total = 0;
                for (auto node = this->head; node != nullptr; node = node->next)
                    total += node->size;
                return total;
            }
        };

        BlockRange allocList; //Beginning/end of list of allocated blocks
        
        BlockRange freeList; //Beginning/end of list of free blocks
        
        using MutexType = SimpleSpinLockMutex;
        mutable MutexType mutex;
    };

} }
