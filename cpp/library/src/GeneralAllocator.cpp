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
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/GeneralAllocator.hpp"

#define ALLOCATED_PADDING_VALUE 11111
#define FREED_PADDING_VALUE 22222
#define EXTENDED_PADDING_VALUE 99999
#define ADD_FREED_TO_END_OF_FREE_LIST 0

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------

//GeneralAllocator::Settings
GeneralAllocator::Settings::Settings()
{
}

void GeneralAllocator::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, Error& error)
{   
}

//GeneralAllocator
GeneralAllocator::GeneralAllocator()
{
}

GeneralAllocator::~GeneralAllocator()
{   
}

void GeneralAllocator::Create(const Settings& settings, ByteMemoryArena&& arena)
{
    //Validate settings
    if (arena.IsNull())
        return;
    
    //Initialize
    std::lock_guard<MutexType> lock(this->mutex);

    this->arena.Destroy();
    _Init();
    
    this->arena = std::move(arena);
    
    _FreeAllBlocks();
}

void GeneralAllocator::Create(const Settings& settings, ByteMemoryArena&& arena, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Validate settings
    if (arena.IsNull())
    {
        FINJIN_SET_ERROR(error, "The initial memory was not specified.");
        return;
    }

    //Initialize
    std::lock_guard<MutexType> lock(this->mutex);

    this->arena.Destroy();
    _Init();

    this->arena = std::move(arena);
    
    _FreeAllBlocks();
}

void* GeneralAllocator::Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    std::lock_guard<MutexType> lock(this->mutex);

    if (this->arena.IsNull())
        return nullptr;
    
    //_DebugCheck();

    //Add to the requested number of bytes the number of bytes required by the memory block data structure plus any bytes for alignment
    byteCount = GetAlignedMemberBlockHeaderSize() + AlignSizeUp(byteCount, this->arena.alignedMemory.alignment);

    auto foundFreeBlockHeader = FindFirstFreeFit(byteCount);
    if (foundFreeBlockHeader == nullptr)
        return nullptr;

    //Found a block of memory that will fit the requested number of bytes plus the size of the allocation structure.
    //If the found block is big enough to hold an additional allocation structure plus a little bit of data, we'll split the block in two, 
    //placing one  block in the free list and the other in the allocated list.
    //Otherwise we remove the block from the free list and move it into the allocated list
    if (foundFreeBlockHeader->size >= byteCount + GetAlignedMemberBlockHeaderSize() + 32)
    {
        //Split
        
        //The memory on the left side of the split is put into the allocation list,
        //and the memory on the right side of the split is put into the free list.  
        //Actually, the memory on the right side STAYS in the free list.  
        //In other words, its relative location in the free list does not change

        auto right = reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<uint8_t*>(foundFreeBlockHeader) + byteCount);
        auto previous = foundFreeBlockHeader->previous;
        auto next = foundFreeBlockHeader->next;

        //Reconfigure the right side of the split, removing the left side
        right->size = foundFreeBlockHeader->size - byteCount;
        right->previous = previous;
        right->next = next;
        right->padding = FREED_PADDING_VALUE;
            
        if (previous != nullptr)
            previous->next = right;
        if (next != nullptr)
            next->previous = right;

        //Adjust free list head/tail if necessary
        if (foundFreeBlockHeader == this->freeList.head)
            this->freeList.head = right;
        if (foundFreeBlockHeader == this->freeList.tail)
            this->freeList.tail = right;

        //Add the left side of the end of the allocated list
        foundFreeBlockHeader->size = byteCount;
        foundFreeBlockHeader->previous = foundFreeBlockHeader->next = nullptr;
        foundFreeBlockHeader->padding = ALLOCATED_PADDING_VALUE;

        if (this->allocList.tail != nullptr)
        {
            this->allocList.tail->next = foundFreeBlockHeader;
            foundFreeBlockHeader->previous = this->allocList.tail;
            
            this->allocList.tail = foundFreeBlockHeader;
        }
        else
        {
            this->allocList.head = this->allocList.tail = foundFreeBlockHeader;
        }
    }
    else
    {
        //Remove from free list
        auto previous = foundFreeBlockHeader->previous;
        auto next = foundFreeBlockHeader->next;

        if (previous != nullptr)
            previous->next = next;
        if (next != nullptr)
            next->previous = previous;

        //Adjust free list head/tail if necessary
        if (foundFreeBlockHeader == this->freeList.head)
            this->freeList.head = next;
        if (foundFreeBlockHeader == this->freeList.tail)
            this->freeList.tail = previous;

        //Add to the end of the allocated list
        foundFreeBlockHeader->previous = foundFreeBlockHeader->next = nullptr;
        foundFreeBlockHeader->padding = ALLOCATED_PADDING_VALUE;
        if (this->allocList.tail != nullptr)
        {
            this->allocList.tail->next = foundFreeBlockHeader;
            foundFreeBlockHeader->previous = this->allocList.tail;

            this->allocList.tail = foundFreeBlockHeader;
        }
        else
            this->allocList.head = this->allocList.tail = foundFreeBlockHeader;
    }

#if FINJIN_DEBUG
    foundFreeBlockHeader->fileName = fileName;
    foundFreeBlockHeader->functionName = functionName;
    foundFreeBlockHeader->line = line;
    foundFreeBlockHeader->padding2 = EXTENDED_PADDING_VALUE;
#endif
    
    //_DebugCheck();
    
    return GetMemory(foundFreeBlockHeader);
}        

bool GeneralAllocator::CanDeallocateBlock() const
{
    return true;
}

void GeneralAllocator::Deallocate(void* mem)
{
    if (mem == nullptr)
        return;

    std::lock_guard<MutexType> lock(this->mutex);

    //_DebugCheck();
    
    auto blockHeader = GetHeader(mem);
    
    assert(blockHeader->padding == ALLOCATED_PADDING_VALUE);
#if FINJIN_DEBUG
    //std::cout << "GeneralAllocator::Deallocate(): Freeing block (" <<   blockHeader->size << " bytes) allocated @ " << blockHeader->fileName << " (" << blockHeader->line << "): " << blockHeader->functionName << std::endl;
#endif
    assert(blockHeader->padding2 = EXTENDED_PADDING_VALUE);

    auto previous = blockHeader->previous;
    auto next = blockHeader->next;

    //Remove the memory from the allocation list
    if (previous != nullptr)
        previous->next = next;
    if (next != nullptr)
        next->previous = previous;

    //Adjust allocated list head/tail if necessary
    if (blockHeader == this->allocList.head)
        this->allocList.head = next;
    if (blockHeader == this->allocList.tail)
        this->allocList.tail = previous;
        
    //Put freed memory into the freed list
    blockHeader->previous = blockHeader->next = nullptr;
    blockHeader->padding = FREED_PADDING_VALUE;
#if ADD_FREED_TO_END_OF_FREE_LIST
    //This will leave the heap fragmented over time
    this->freeList.AddOrCombineWithTail(blockHeader);
#else
    //This will minimize fragmentation, but at a performance cost
    //Ideally, the application will be using specialized allocators for time critical operations, so this shouldn't be an issue
    if (this->freeList.empty())
    {
        //Free list is empty, so just set the freed block as the list
        this->freeList.head = this->freeList.tail = blockHeader;
    }
    else if (this->freeList.tail->GetByteEnd() == blockHeader->GetByteStart())
    {
        //Combine the free block into the end
        this->freeList.tail->size += blockHeader->size;
    }
    else if (blockHeader->GetByteEnd() == this->freeList.head->GetByteStart())
    {
        //The free block should consume the head and become the new head
        blockHeader->size += this->freeList.head->size;
        if (this->freeList.head->next != nullptr)
        {
            blockHeader->next = this->freeList.head->next;
            this->freeList.head->next->previous = blockHeader;
        }
        else
            this->freeList.tail = blockHeader;
        this->freeList.head = blockHeader;
    }
    else if (blockHeader < this->freeList.head)
    {
        //The free block should be the new head
        blockHeader->next = this->freeList.head;
        this->freeList.head->previous = blockHeader;
        this->freeList.head = blockHeader;
    }
    else if (blockHeader > this->freeList.tail)
    {
        //The free block should be the new tail
        blockHeader->previous = this->freeList.tail;
        this->freeList.tail->next = blockHeader;
        this->freeList.tail = blockHeader;
    }
    else
    {
        //The free block can go somewhere in the list
        auto previous = this->freeList.head;
        for (auto node = this->freeList.head->next; node != nullptr; previous = node, node = node->next)
        {
            if (blockHeader > previous && blockHeader < node)
            {
                //Insert between previous and node
                if (previous->GetByteEnd() == blockHeader->GetByteStart())
                {
                    //Add into previous
                    previous->size += blockHeader->size;

                    //See if previous now lines up with its next
                    if (previous->GetByteEnd() == node->GetByteStart())
                    {
                        //It does
                        previous->size += node->size;
                        previous->next = node->next;

                        //If at the end, set new tail
                        if (previous->next == nullptr)
                            this->freeList.tail = previous;
                        else
                            previous->next->previous = previous;
                    }
                }
                else
                {
                    //See if free block should replace node
                    if (blockHeader->GetByteEnd() == node->GetByteStart())
                    {
                        //Replace it
                        blockHeader->size += node->size;

                        blockHeader->previous = previous;
                        blockHeader->next = node->next;
                        
                        previous->next = blockHeader;
                        
                        if (blockHeader->next == nullptr)
                            this->freeList.tail = blockHeader;
                        else
                            blockHeader->next->previous = blockHeader;
                    }
                    else
                    {
                        //Insert it
                        previous->next = blockHeader;
                        blockHeader->previous = previous;

                        blockHeader->next = node;
                        node->previous = blockHeader;
                    }
                }

                break;
            }
        }
    }
#endif
    
    //_DebugCheck();
}

bool GeneralAllocator::CanDeallocateAll() const
{
    return true;
}

void GeneralAllocator::DeallocateAll()
{
    std::lock_guard<MutexType> lock(this->mutex);

    _FreeAllBlocks();
}

size_t GeneralAllocator::GetBytesUsed() const
{
    std::lock_guard<MutexType> lock(this->mutex);

    return this->allocList.GetTotal();
}

size_t GeneralAllocator::GetBytesFree() const
{
    std::lock_guard<MutexType> lock(this->mutex);

    return this->arena.alignedMemory.size - this->allocList.GetTotal();
}

void GeneralAllocator::DebugCheck()
{
    std::lock_guard<MutexType> lock(this->mutex);
    
    _DebugCheck();
}

void GeneralAllocator::Output(std::ostream& out, bool forward)
{
    std::lock_guard<MutexType> lock(this->mutex);
    
    out << "Free:" << std::endl;
    for (auto node = forward ? this->freeList.head : this->freeList.tail; node != nullptr; node = forward ? node->next : node->previous)
    {
        out << "  Address: " << (void*)node
            << "  Total size: " << node->size
            << "  Tag: " << node->padding
            << std::endl;
    }

    out << "Allocated:" << std::endl;
    for (auto node = forward ? this->allocList.head : this->allocList.tail; node != nullptr; node = forward ? node->next : node->previous)
    {
        out << "  Address: " << (void*)node
            << "  Total size: " << node->size
            << "  Tag: " << node->padding
            << std::endl;
    }
}

void GeneralAllocator::_DebugCheck()
{
    size_t index = 0;
    for (auto node = this->freeList.head; node != nullptr; node = node->next)
    {
        assert(node->padding == FREED_PADDING_VALUE);
        index++;
    }
    
    index = 0;
    for (auto node = this->allocList.head; node != nullptr; node = node->next)
    {
        assert(node->padding == ALLOCATED_PADDING_VALUE);
        assert(node->padding2 == EXTENDED_PADDING_VALUE); //This is only set on allocated blocks
        index++;
    }
    
    auto calculatedBytesUsed = this->allocList.GetTotal();
    auto calculatedBytesFree = this->freeList.GetTotal();
    auto total = calculatedBytesUsed + calculatedBytesFree;
    auto expected = this->arena.alignedMemory.size;
    auto diff = (total > expected) ? total - expected : expected - total;
    assert(diff == 0);
}

size_t GeneralAllocator::GetAlignment() const
{
    return this->arena.alignedMemory.alignment;
}

size_t GeneralAllocator::GetAlignedMemberBlockHeaderSize() const
{
    return AlignSizeUp(sizeof(MemoryBlockHeader), this->arena.alignedMemory.alignment);
}

GeneralAllocator::MemoryBlockHeader* GeneralAllocator::FindFirstFreeFit(size_t byteCount)
{        
    for (auto blockHeader = this->freeList.head; blockHeader != nullptr; blockHeader = blockHeader->next)
    {
        assert(blockHeader->padding == FREED_PADDING_VALUE);
        if (blockHeader->size >= byteCount)
            return blockHeader;
    }        

    return nullptr;
}    

void GeneralAllocator::_Init()
{
}

void GeneralAllocator::_FreeAllBlocks()
{
    if (this->arena.IsNull())
        return;

    this->allocList.clear();

    auto freeHead = reinterpret_cast<MemoryBlockHeader*>(this->arena.alignedMemory.ptr);
    freeHead->size = this->arena.alignedMemory.size;
    freeHead->previous = nullptr;
    freeHead->next = nullptr;
    freeHead->padding = FREED_PADDING_VALUE;

    this->freeList.head = this->freeList.tail = freeHead;
}

GeneralAllocator::MemoryBlockHeader* GeneralAllocator::GetHeader(void* mem)
{
    auto blockHeader = reinterpret_cast<MemoryBlockHeader*>(reinterpret_cast<uint8_t*>(mem) - GetAlignedMemberBlockHeaderSize());
    return blockHeader;
}

void* GeneralAllocator::GetMemory(MemoryBlockHeader* header)
{
    auto mem = reinterpret_cast<uint8_t*>(header) + GetAlignedMemberBlockHeaderSize();
    assert(AlignMemoryUp(mem, this->arena.alignedMemory.alignment) == mem);
    return mem;
}
