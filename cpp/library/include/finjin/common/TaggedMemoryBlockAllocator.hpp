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


//Includes----------------------------------------------------------------------
#include "finjin/common/Allocator.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/SimpleSpinLockMutex.hpp"
#include <mutex>
#include <ostream>


//Macros------------------------------------------------------------------------
#define FINJIN_TAGGED_MEMORY_BLOCK_ALLOCATOR_SET_INDEX 0 //1 = useful for debugging


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    //TagType should be an integer type
    template <typename ArenaType, typename TagType, TagType TagNullValue, typename MutexType>
    class TaggedMemoryBlockPool
    {
    public:
        TaggedMemoryBlockPool(const TaggedMemoryBlockPool& other) = delete;
        TaggedMemoryBlockPool& operator = (const TaggedMemoryBlockPool& other) = delete;

        TaggedMemoryBlockPool(TaggedMemoryBlockPool&& other) = delete;
        TaggedMemoryBlockPool& operator = (TaggedMemoryBlockPool&& other) = delete;

    public:
        using MemoryType = typename ArenaType::MemoryType;
        using Tag = TagType;

        TaggedMemoryBlockPool()
        {
            _Init();
        }

        ~TaggedMemoryBlockPool()
        {
        }

        struct Settings
        {
            Settings()
            {
                this->tagCount = 0;

                this->blockCount = 0;

                this->allocator = nullptr;
            }

            size_t tagCount;

            size_t blockCount;

            Allocator* allocator; //Used to create the usedBlocksByTag array
        };

        void Create(const Settings& settings, ArenaType&& arena, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            //Validate
            if (settings.tagCount == 0)
            {
                FINJIN_SET_ERROR(error, "The tag count must be greater than 0.");
                return;
            }

            if (settings.blockCount == 0)
            {
                FINJIN_SET_ERROR(error, "The block count must be greater than 0.");
                return;
            }

            if (arena.IsNull())
            {
                FINJIN_SET_ERROR(error, "The initial memory was not specified.");
                return;
            }

            Create(settings, std::move(arena));

            if (this->blocks.data() == nullptr)
            {
                FINJIN_SET_ERROR(error, "Failed to allocate memory for blocks.");
                return;
            }
        }

        void Create(const Settings& settings, ArenaType&& arena)
        {
            //This really should not be called while in use, but put a lock here just in case
            std::lock_guard<MutexType> thisLock(this->mutex);

            //Free existing memory
            this->arena.Destroy();
            _FreeBlocksBuffer();
            _Init();

            this->settings = settings;

            this->arena = std::move(arena);

            //Calculate block size
            this->blockSize = this->arena.alignedMemory.size / this->settings.blockCount;

            //Ensure block size is properly aligned so that successive block start addresses are aligned
            if (!Allocator::IsSizeAligned(this->blockSize, this->arena.alignedMemory.alignment))
                this->blockSize = Allocator::AlignSizeUp(this->blockSize, this->arena.alignedMemory.alignment) - this->arena.alignedMemory.alignment;

            //Allocate blocks
            this->blocks.Create(this->settings.blockCount, this->settings.allocator);
            if (this->blocks.data() == nullptr)
            {
                this->arena.Destroy();
                _Init();
                return;
            }

            //Allocate used blocks lookup
            this->usedBlocksByTag.Create(this->settings.tagCount, this->settings.allocator);
            if (this->usedBlocksByTag.data() == nullptr)
            {
                this->blocks.Destroy();
                this->arena.Destroy();
                _Init();
                return;
            }

            //Reset state
            _FreeAll();
        }

        size_t GetAlignment() const
        {
            return this->arena.alignedMemory.alignment;
        }

        size_t GetBlockCount() const
        {
            return this->blocks.size();
        }

        size_t GetBlockSize() const
        {
            return this->blockSize;
        }

        bool IsValidTag(TagType tag) const
        {
            if (tag == TagNullValue || tag >= this->usedBlocksByTag.size())
                return false;

            return true;
        }

        MemoryType Allocate(TagType tag, size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            byteCount = Allocator::UpgradeSizeForOffsetAlignment(byteCount, this->arena.alignedMemory.alignment);

            auto consecutiveCount = (byteCount + this->blockSize - 1) / this->blockSize;
            auto result = _AllocateBlocks(tag, consecutiveCount, FINJIN_CALLER_PARAMETERS);
            if (result != this->arena.memoryNull)
                result = Allocator::AlignMemoryUp(result, this->arena.alignedMemory.alignment);
            return result;
        }

        void Free(TagType tag)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            if (tag == TagNullValue || tag >= this->usedBlocksByTag.size() || this->usedBlocksByTag[tag].empty())
                return;

            auto& blockList = this->usedBlocksByTag[tag];
            this->freeBlocks.Merge(blockList);
            blockList.clear();
        }

        void DeallocateAll()
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            _FreeAll();
        }

        size_t GetBytesUsed() const
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            return this->bytesUsed;
        }

        size_t GetBytesFree() const
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            return this->arena.alignedMemory.size - this->bytesUsed;
        }

        size_t GetBytesUsed(Tag tag) const
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            auto& usedBlock = this->usedBlocksByTag[tag];

            size_t total = 0;
            for (auto item = usedBlock.head; item != nullptr && item <= usedBlock.tail; item = item->HeadGetNextRangeHead())
                total += this->blockSize * item->consecutiveCount;

            return this->bytesUsed;
        }

        size_t GetBytesFree(Tag tag) const
        {
            //No need for lock since GetBytesUsed(tag) has a lock
            return this->arena.alignedMemory.size - GetBytesUsed(tag);
        }

        void Output(std::ostream& out)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            //Used
            for (TagType tag = 0; tag < static_cast<TagType>(this->usedBlocksByTag.size()); tag++)
            {
                auto& usedBlock = this->usedBlocksByTag[tag];
                if (!usedBlock.empty())
                {
                    for (auto item = usedBlock.head; item != nullptr && item <= usedBlock.tail; item = item->HeadGetNextRangeHead())
                    {
                        out << "Used block: tag=" << tag << ", index=" << _GetBlockIndex(item) << ", range=" << item->consecutiveCount << std::endl;
                    }
                }
            }

            //Free
            if (!this->freeBlocks.empty())
            {
                for (auto item = this->freeBlocks.head; item != nullptr && item <= this->freeBlocks.tail; item = item->HeadGetNextRangeHead())
                {
                    out << "Free block: index=" << _GetBlockIndex(item) << ", range=" << item->consecutiveCount << std::endl;
                }
            }
        }

    private:
        void _FreeAll()
        {
            //Add all the blocks to the freeBlocks list. List is constructed so that head is initially at index 0
            this->blocks[0].consecutiveCount = this->blocks[this->blocks.size() - 1].consecutiveCount = this->blocks.size();

            for (size_t i = 0; i < this->blocks.size(); i++)
            {
            #if FINJIN_TAGGED_MEMORY_BLOCK_ALLOCATOR_SET_INDEX
                this->blocks[i].index = i;
            #endif
                this->blocks[i].previous = i > 0 ? &this->blocks[i - 1] : nullptr;
                this->blocks[i].next = (i < this->blocks.size() - 1) ? &this->blocks[i + 1] : nullptr;
            }
            this->freeBlocks.head = &this->blocks[0];
            this->freeBlocks.tail = &this->blocks[this->blocks.size() - 1];

            //Zero out used blocks
            FINJIN_ZERO_MEMORY(this->usedBlocksByTag.data(), this->usedBlocksByTag.size() * sizeof(BlockList));

            this->bytesUsed = 0;
        }

        MemoryType _AllocateBlocks(TagType tag, size_t consecutiveCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            if (tag == TagNullValue || tag >= this->usedBlocksByTag.size())
                return this->arena.memoryNull;

            auto blockRange = this->freeBlocks.SpliceFirstConsecutiveRange(consecutiveCount);
            if (blockRange.empty())
                return this->arena.memoryNull;

            this->usedBlocksByTag[tag].MergeConsecutive(blockRange);

            this->bytesUsed += this->blockSize * consecutiveCount;

            //Convert block index to block address
            auto result = this->arena.alignedMemory.ptr + (this->blockSize * _GetBlockIndex(blockRange.head));
            assert(result != this->arena.memoryNull);
            return result;
        }

        struct Block
        {
        #if FINJIN_TAGGED_MEMORY_BLOCK_ALLOCATOR_SET_INDEX
            size_t index;
        #endif
            size_t consecutiveCount; //Valid on the head and tail of a range of blocks. Indicates the number of consecutive blocks.
            Block* previous;
            Block* next;

            void Remove()
            {
                if (this->previous != nullptr)
                    this->previous->next = next;
                if (this->next != nullptr)
                    this->next->previous = this->previous;
            }

            Block* HeadGetRangeTail()
            {
                //This assumes that the call is occurring on the head
                return this + this->consecutiveCount - 1;
            }

            Block* HeadGetNextRangeHead()
            {
                //This assumes that the call is occurring on the head
                return HeadGetRangeTail()->next;
            }

            Block* TailGetRangeHead()
            {
                //This assumes that the call is occurring on the tail
                return this - (this->consecutiveCount - 1);
            }

            Block* TailGetPreviousRangeTail()
            {
                //This assumes that the call is occurring on the tail
                return TailGetRangeHead()->previous;
            }
        };

        struct BlockList
        {
            Block* head;
            Block* tail;

            BlockList()
            {
                this->head = this->tail = nullptr;
            }

            BlockList(Block* first, Block* second)
            {
                this->head = first;
                this->tail = second;
            }

            bool IsConsecutive() const
            {
                return !empty() && (this->head + this->head->consecutiveCount - 1) == this->tail;
            }

            bool empty() const
            {
                return this->head == nullptr;
            }

            void clear()
            {
                this->head = this->tail = nullptr;
            }

            BlockList SpliceFirstConsecutiveRange()
            {
                BlockList result;

                if (this->head != nullptr)
                {
                    result.head = this->head;
                    result.tail = this->head + this->head->consecutiveCount - 1;

                    this->head = result.tail->next;
                    if (this->head != nullptr)
                        this->head->previous = nullptr;
                    else
                        this->tail = nullptr;
                }

                return result;
            }

            BlockList SpliceFirstConsecutiveRange(size_t consecutiveCount)
            {
                BlockList result;

                for (auto head = this->head; head != nullptr && head <= this->tail; head = head->HeadGetNextRangeHead())
                {
                    if (head->consecutiveCount >= consecutiveCount)
                    {
                        auto tail = head + consecutiveCount - 1;

                        if (head->consecutiveCount > consecutiveCount)
                        {
                            auto nextHead = tail->next;

                            nextHead->consecutiveCount = head->consecutiveCount - consecutiveCount;
                            nextHead->HeadGetRangeTail()->consecutiveCount = nextHead->consecutiveCount;
                        }

                        if (head->previous != nullptr)
                            head->previous->next = tail->next;
                        if (tail->next != nullptr)
                            tail->next->previous = head->previous;

                        if (head == this->head)
                            this->head = tail->next;
                        if (tail == this->tail)
                            this->tail = head->previous;

                        head->previous = nullptr;
                        tail->next = nullptr;

                        head->consecutiveCount = tail->consecutiveCount = consecutiveCount;

                        result.head = head;
                        result.tail = tail;

                        break;
                    }
                }

                return result;
            }

            void Merge(BlockList blockList)
            {
                auto startHead = this->head;
                for (auto consecutive = blockList.SpliceFirstConsecutiveRange(); !consecutive.empty(); consecutive = blockList.SpliceFirstConsecutiveRange())
                    startHead = MergeConsecutive(consecutive, startHead);
            }

            Block* MergeConsecutive(BlockList consecutiveRange, Block* startHead = nullptr)
            {
                assert(consecutiveRange.IsConsecutive());

                if (empty())
                {
                    //The specified block becomes the list
                    *this = consecutiveRange;

                    return this->head;
                }
                else if (consecutiveRange < *this)
                {
                    //The specified block goes before the current head, becoming the new head

                    if (consecutiveRange.tail + 1 == this->head)
                    {
                        consecutiveRange.head->consecutiveCount += this->head->consecutiveCount;
                        this->tail->consecutiveCount = consecutiveRange.head->consecutiveCount;
                    }

                    this->head->previous = consecutiveRange.tail;
                    consecutiveRange.tail->next = this->head;

                    this->head = consecutiveRange.head;

                    return this->head;
                }
                else if (consecutiveRange > *this)
                {
                    //The specified block goes after the current tail, becoming the new tail

                    auto consecutiveHead = this->tail->TailGetRangeHead();

                    if (this->tail + 1 == consecutiveRange.head)
                    {
                        consecutiveHead->consecutiveCount += consecutiveRange.head->consecutiveCount;
                        consecutiveRange.tail->consecutiveCount = consecutiveHead->consecutiveCount;
                    }

                    this->tail->next = consecutiveRange.head;
                    consecutiveRange.head->previous = this->tail;

                    this->tail = consecutiveRange.tail;

                    return consecutiveHead;
                }
                else
                {
                    //The specified block goes somewhere in the middle of the list
                    if (startHead == nullptr)
                        startHead = this->head;
                    auto previousHead = startHead;
                    auto previousTail = startHead->HeadGetRangeTail();
                    for (auto currentHead = startHead->HeadGetNextRangeHead(); currentHead != nullptr; currentHead = currentHead->HeadGetNextRangeHead())
                    {
                        if (consecutiveRange.tail < currentHead)
                        {
                            //Add before the current item

                            if (previousTail + 1 == consecutiveRange.head)
                            {
                                previousHead->consecutiveCount += consecutiveRange.head->consecutiveCount;

                                if (consecutiveRange.tail + 1 == currentHead)
                                {
                                    previousHead->consecutiveCount += currentHead->consecutiveCount;
                                    currentHead->HeadGetRangeTail()->consecutiveCount = previousHead->consecutiveCount;
                                }
                                else
                                    consecutiveRange.tail->consecutiveCount = previousHead->consecutiveCount;
                            }
                            else if (consecutiveRange.tail + 1 == currentHead)
                            {
                                consecutiveRange.head->consecutiveCount += currentHead->consecutiveCount;
                                currentHead->HeadGetRangeTail()->consecutiveCount = consecutiveRange.head->consecutiveCount;
                            }

                            previousTail->next = consecutiveRange.head;
                            consecutiveRange.head->previous = previousTail;

                            consecutiveRange.tail->next = currentHead;
                            currentHead->previous = consecutiveRange.tail;

                            return previousHead;
                        }

                        previousHead = currentHead;
                        previousTail = currentHead->HeadGetRangeTail();
                    }
                }

                assert(0 && "The block list is in an unexpected state during a MergeConsecutive() call.");
                return nullptr;
            }

            bool operator < (const BlockList& other) const { return this->tail < other.head; }
            bool operator > (const BlockList& other) const { return this->head > other.tail; }
        };

        void _Init()
        {
            this->settings = Settings();

            this->blockSize = 0;

            this->freeBlocks.clear();

            this->bytesUsed = 0;
        }

        size_t _GetBlockIndex(Block* block)
        {
            return block - this->blocks.data();
        }

        void _FreeBlocksBuffer()
        {
            this->blocks.Destroy();
        }

    private:
        mutable MutexType mutex;

        Settings settings;

        ArenaType arena;

        DynamicVector<Block> blocks;
        size_t blockSize;

        BlockList freeBlocks;
        DynamicVector<BlockList> usedBlocksByTag;

        size_t bytesUsed;
    };

    template <typename PoolType>
    class TaggedMemoryBlockAllocator : public Allocator
    {
    public:
        using Pool = PoolType;
        using Tag = typename Pool::Tag;

        TaggedMemoryBlockAllocator()
        {
            this->tag = (Tag)-1;
            this->pool = nullptr;
        }

        TaggedMemoryBlockAllocator(Tag tag, Pool* pool)
        {
            this->tag = tag;
            this->pool = pool;
        }

        ~TaggedMemoryBlockAllocator()
        {
        }

        void Create(Tag tag, Pool* pool)
        {
            this->tag = tag;
            this->pool = pool;
        }

        void Create(Tag tag, Pool* pool, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (pool == nullptr)
            {
                FINJIN_SET_ERROR(error, "No pool was specified.");
                return;
            }

            if (!pool->IsValidTag(tag))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid tag '%1%'.", tag));
                return;
            }

            this->tag = tag;
            this->pool = pool;
        }

        void* Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION) override
        {
            if (this->pool == nullptr)
                return nullptr;

            return this->pool->Allocate(this->tag, byteCount, FINJIN_CALLER_PARAMETERS);
        }

        bool CanDeallocateBlock() const override
        {
            return false;
        }

        void Deallocate(void* vmem) override
        {
        }

        bool CanDeallocateAll() const override
        {
            return true;
        }

        void DeallocateAll() override
        {
            if (this->pool != nullptr)
                this->pool->Free(this->tag);
        }

        size_t GetBytesUsed() const override
        {
            if (this->pool != nullptr)
                return this->pool->GetBytesUsed(this->tag);
            else
                return 0;
        }

        size_t GetBytesFree() const override
        {
            if (this->pool != nullptr)
                return this->pool->GetBytesFree(this->tag);
            else
                return 0;
        }

        size_t GetAlignment() const override
        {
            return this->pool->GetAlignment();
        }

    private:
        Tag tag;
        Pool* pool;
    };

} }
