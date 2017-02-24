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
#include <mutex>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename ArenaType, typename MutexType>
    class GenericForwardAllocator
    {
    public:
        GenericForwardAllocator(const GenericForwardAllocator& other) = delete;
        GenericForwardAllocator& operator = (const GenericForwardAllocator& other) = delete;

        GenericForwardAllocator(GenericForwardAllocator&& other) = delete;
        GenericForwardAllocator& operator = (GenericForwardAllocator&& other) = delete;

    public:
        using MemoryType = typename ArenaType::MemoryType;

        GenericForwardAllocator()
        {
            _Init();
        }

        ~GenericForwardAllocator()
        {   
        }

        struct Settings
        {
            Settings()
            {
                this->allowWraparound = false;
            }

            bool allowWraparound;
        };

        void Create(const Settings& settings, ArenaType&& arena, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            //Validate
            if (arena.IsNull())
            {
                FINJIN_SET_ERROR(error, "The initial memory was not specified.");
                return;
            }

            Create(settings, std::move(arena));
        }

        void Create(const Settings& settings, ArenaType&& arena)
        {
            //This really should not be called while in use, but put a lock here just in case
            std::lock_guard<MutexType> thisLock(this->mutex);
            
            //Free existing memory
            this->arena.Destroy();
            _Init();

            this->settings = settings;

            this->arena = std::move(arena);

            _FreeAll();
        }

        virtual bool CanDeallocateBlock() const
        {
            return false;
        }

        virtual void Deallocate(MemoryType mem)
        {
        }
        
        virtual MemoryType Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            bool wrappedAround; //This will be ignored
            return _Allocate(byteCount, wrappedAround, FINJIN_CALLER_PARAMETERS);
        }

        MemoryType Allocate(size_t byteCount, bool& wrappedAround, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);
            
            return _Allocate(byteCount, wrappedAround, FINJIN_CALLER_PARAMETERS);
        }

        void FreeCount(size_t byteCount)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);
            
            //Doesn't free any memory, just moves the allocate head        
            this->cursor -= std::min(_GetBytesFree(), byteCount);
        }

        virtual bool CanDeallocateAll() const
        {
            return true;
        }

        virtual void DeallocateAll()
        {
            std::lock_guard<MutexType> thisLock(this->mutex);
            
            _FreeAll();
        }

        virtual size_t GetBytesUsed() const
        {
            std::lock_guard<MutexType> thisLock(this->mutex);
            
            return _GetBytesUsed();
        }

        virtual size_t GetBytesFree() const
        {
            std::lock_guard<MutexType> thisLock(this->mutex);
            
            return _GetBytesFree();
        }

        virtual size_t GetAlignment() const
        {
            return this->arena.alignedMemory.alignment;
        }

    private:
        void _Init()
        {
            this->settings = Settings();

            this->cursor = this->arena.memoryNull;
        }

        size_t _GetBytesUsed() const
        {
            return this->cursor - this->arena.alignedMemory.ptr;
        }

        size_t _GetBytesFree() const
        {
            return this->arena.alignedMemory.size - _GetBytesUsed();
        }

        void _FreeAll()
        {
            this->cursor = this->arena.alignedMemory.ptr;
        }

        MemoryType _Allocate(size_t byteCount, bool& wrappedAround, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            wrappedAround = false;

            //Make sure there's actually memory
            if (this->cursor == this->arena.memoryNull)
                return this->arena.memoryNull;

            //Align byteCount so that this->cursor is always properly aligned
            byteCount = Allocator::AlignSizeUp(byteCount, this->arena.alignedMemory.alignment);

            auto result = this->arena.memoryNull;

            for (int attempt = 0; attempt < 2; attempt++)
            {
                //Determine byte count
                if (!this->arena.alignedMemory.IsValidRange(this->cursor, byteCount))
                {
                    //End address exceeds bounds of memory space
                    if (this->settings.allowWraparound)
                    {
                        //Free everything and try again
                        wrappedAround = true;
                        
                        _FreeAll();
                        
                        continue;
                    }
                    else
                        return this->arena.memoryNull;
                }

                //Success
                result = this->cursor;

                //Update start address
                this->cursor += byteCount;

                break;
            }

            return result;
        }

    private:
        mutable MutexType mutex;

        Settings settings;

        ArenaType arena;
        MemoryType cursor;
    };

    template <typename MutexType>
    class ForwardAllocator : public Allocator
    {
    public:
        using Super = Allocator;
        using Super2 = GenericForwardAllocator<ByteMemoryArena, MutexType>;
        
        void Create(ByteMemoryArena&& arena, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            typename Super2::Settings settings;
            base.Create(settings, std::move(arena), error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
        }

        void Create(ByteMemoryArena&& arena)
        {
            typename Super2::Settings settings;
            base.Create(settings, std::move(arena));            
        }

        void* Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION) override
        {
            return base.Allocate(byteCount, FINJIN_CALLER_PARAMETERS);
        }

        bool CanDeallocateBlock() const override
        {
            return false;
        }

        void Deallocate(void* mem) override
        {            
        }

        bool CanDeallocateAll() const override
        {
            return true;
        }

        void DeallocateAll() override
        {
            base.DeallocateAll();
        }

        size_t GetBytesUsed() const override
        {
            return base.GetBytesUsed();
        }

        size_t GetBytesFree() const override
        {
            return base.GetBytesFree();
        }

        size_t GetAlignment() const override
        {
            return base.GetAlignment();
        }

    private:
        Super2 base;
    };

} }
