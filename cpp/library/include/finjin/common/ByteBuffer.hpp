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
#include "finjin/common/Utf8String.hpp"


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API ByteBuffer
    {
    public:
        typedef uint8_t value_type;
        typedef uint8_t* iterator;
        typedef const uint8_t* const_iterator;
        
        ByteBuffer()
        {
            this->items = nullptr;
            this->count = 0;
            this->maxCount = 0;
            this->allocator = nullptr;
            this->resizeStrategy = MemoryResizeStrategy::LIMIT;
        }
        
        ByteBuffer(ByteBuffer&& other)
        {
            this->items = other.items;
            this->count = other.count;
            this->maxCount = other.maxCount;
            this->allocator = other.allocator;
            this->resizeStrategy = other.resizeStrategy;
            
            other.items = nullptr;
            other.count = 0;
            other.maxCount = 0;
            other.allocator = nullptr;
        }
        
        ByteBuffer& operator = (ByteBuffer&& other)
        {
            Destroy();
            
            this->items = other.items;
            this->count = other.count;
            this->maxCount = other.maxCount;
            this->allocator = other.allocator;
            this->resizeStrategy = other.resizeStrategy;
            
            other.items = nullptr;
            other.count = 0;
            other.maxCount = 0;
            other.allocator = nullptr;
            
            return *this;
        }
        
        ByteBuffer(const ByteBuffer& other) = delete;
        
        ~ByteBuffer()
        {
            Destroy();
        }
        
        bool Create(size_t count, Allocator* allocator, MemoryResizeStrategy resizeStrategy = MemoryResizeStrategy::LIMIT);
        bool Create(const void* bytes, size_t count, Allocator* allocator, MemoryResizeStrategy resizeStrategy = MemoryResizeStrategy::LIMIT);
        bool CreateEmpty(size_t count, Allocator* allocator, MemoryResizeStrategy resizeStrategy = MemoryResizeStrategy::LIMIT);

        void Destroy();

        Allocator* GetAllocator();

        MemoryResizeStrategy GetResizeStrategy() const;
        void SetResizeStrategy(MemoryResizeStrategy value);
        
        size_t size() const { return this->count; }
        size_t max_size() const { return this->maxCount; }
        size_t size_left() const { return max_size() - size(); }
        
        size_t resize(size_t newSize) 
        { 
            if (this->resizeStrategy == MemoryResizeStrategy::REALLOCATE && full())
                Reallocate(newSize);
            
            this->count = std::min(newSize, this->maxCount);

            return this->count;
        }
        size_t maximize() { this->count = this->maxCount; return this->count; }
        
        bool empty() const { return this->count == 0; }
        bool full() const { return this->count == this->maxCount; }
        
        bool contains(uint8_t value) const
        {
            return find(value) != end();
        }
        
        ValueOrError<void> assign(const ByteBuffer& other)
        {
            return assign(other.data(), other.size());
        }
        
        ValueOrError<void> assign(const uint8_t* otherItems, size_t otherCount)
        {
            if (this->resizeStrategy == MemoryResizeStrategy::LIMIT || otherCount <= this->maxCount)
                this->count = std::min(max_size(), otherCount);
            else
            {
                Reallocate(otherCount);
                if (this->items == nullptr)
                {
                    this->count = 0;
                    return ValueOrError<void>::CreateError();
                }

                this->count = otherCount;
            }

            FINJIN_COPY_MEMORY(this->items, otherItems, this->count);
            return ValueOrError<void>();
        }
        
        uint8_t* find(uint8_t value)
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (this->items[i] == value)
                    return &this->items[i];
            }
            return end();
        }
        
        const uint8_t* find(uint8_t value) const
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (this->items[i] == value)
                    return &this->items[i];
            }
            return end();
        }
        
        void clear()
        {
            this->count = 0;
        }
        
        const uint8_t& front() const { assert(this->count > 0); return this->items[0]; }
        uint8_t& front() { assert(this->count > 0); return this->items[0]; }
        
        const uint8_t& back() const { assert(this->count > 0); return this->items[this->count - 1]; }
        uint8_t& back() { assert(this->count > 0); return this->items[this->count - 1]; }
        
        const uint8_t& middle() const { assert(this->count > 0); return this->items[this->count / 2 + this->count % 2]; }
        uint8_t& middle() { assert(this->count > 0); return this->items[this->count / 2 + this->count % 2]; }
        
        ValueOrError<bool> push_front()
        {
            if (this->resizeStrategy == MemoryResizeStrategy::REALLOCATE && full())
                Reallocate(this->count + 1);                
            
            if (this->count < this->maxCount)
            {
                this->count++;
                for (size_t i = this->count - 1; i > 0; i--)
                    this->items[i] = this->items[i - 1];
                return true;
            }
            else
                return false;
        }
        
        ValueOrError<bool> push_front(uint8_t item)
        {
            if (this->resizeStrategy == MemoryResizeStrategy::REALLOCATE && full())
                Reallocate(this->count + 1);

            if (this->count < this->maxCount)
            {
                this->count++;
                for (size_t i = this->count - 1; i > 0; i--)
                    this->items[i] = this->items[i - 1];
                this->items[0] = item;
                return true;
            }
            else
                return false;
        }
        
        bool push_back()
        {
            if (this->resizeStrategy == MemoryResizeStrategy::REALLOCATE && full())
                Reallocate(this->count + 1);

            if (this->count < this->maxCount)
            {
                this->count++;
                return true;
            }
            else
                return false;
        }
        
        bool push_back_count(size_t count)
        {
            if (this->resizeStrategy == MemoryResizeStrategy::REALLOCATE && full())
                Reallocate(this->count + count);

            if (count <= size_left())
            {
                this->count += count;
                return true;
            }
            else
                return false;
        }
        
        ValueOrError<bool> push_back(uint8_t item)
        {
            if (this->resizeStrategy == MemoryResizeStrategy::REALLOCATE && full())
                Reallocate(this->count + count);

            if (this->count < this->maxCount)
            {
                this->items[this->count++] = item;
                return true;
            }
            else
                return false;
        }
        
        void pop_back()
        {
            if (this->count > 0)
                this->count--;
        }
        
        ValueOrError<void> erase(const uint8_t* iter)
        {
            assert(iter >= begin());
            assert(iter < end());
            
            size_t itemIndex = iter - begin();
            
            for (size_t i = itemIndex; i < this->count - 1; i++)
                this->items[i] = this->items[i + 1];
            this->count--;
            
            return ValueOrError<void>();
        }
        
        const uint8_t* data() const { return this->count > 0 ? &this->items[0] : nullptr; }
        uint8_t* data() { return this->count > 0 ? &this->items[0] : nullptr; }

        const uint8_t* data_left() const { return this->count < this->maxCount ? &this->items[this->count] : nullptr; }
        uint8_t* data_left() { return this->count < this->maxCount ? &this->items[this->count] : nullptr; }

        const uint8_t* begin() const { return &this->items[0]; }
        uint8_t* begin() { return &this->items[0]; }
        
        const uint8_t* end() const { return &this->items[this->count]; }
        uint8_t* end() { return &this->items[this->count]; }
        
        const uint8_t* max_end() const { return &this->items[this->maxCount]; }
        uint8_t* max_end() { return &this->items[this->maxCount]; }
        
        const uint8_t& operator [] (size_t i) const { assert(this->count > 0); return this->items[i]; }
        uint8_t& operator [] (size_t i) { assert(this->count > 0); return this->items[i]; }
        
        ValueOrError<void> Get(size_t i, uint8_t& value) { assert(i < this->count); value = this->items[i]; return ValueOrError<void>(); }
        ValueOrError<void> Set(size_t i, uint8_t value) { assert(i < this->count); this->items[i] = value; return ValueOrError<void>(); }
        
        void Fill(uint8_t value);
        void Fill(uint8_t value, uint8_t* start, uint8_t* end);
        
        //These act as though memoryStrategy = MemoryResizeStrategy::REALLOCATE
        void EnsureCanAppend(size_t byteCount);
        
        ByteBuffer& Write(const void* bytes, size_t byteCount);
        ByteBuffer& Write(const Utf8String& s);
        ByteBuffer& Write(const char* s);
        ByteBuffer& Write(uint8_t b);
        ByteBuffer& WriteBase64(const void* bytes, size_t byteCount);
        
        //Acts as though memoryStrategy = MemoryResizeStrategy::STAY_WITHIN_LIMIT
        //Returns the number of input bytes that were processed (NOT the number of base64 characters appended).
        size_t WriteBase64(const void* bytes, size_t byteCount, size_t maxBase64Bytes);
        
        void Truncate(size_t count);
        
    private:
        void Reallocate(size_t count);
        
    protected:
        uint8_t* items;
        size_t count;
        size_t maxCount;
        Allocator* allocator;
        MemoryResizeStrategy resizeStrategy;
    };
    
    class FINJIN_COMMON_LIBRARY_API ByteBufferReader
    {
    public:
        ByteBufferReader();
        ByteBufferReader(const ByteBuffer& data);
        ByteBufferReader(const void* bytes, size_t byteCount);

        const uint8_t* data_start() const;
        const uint8_t* data_left() const;
        
        size_t size_left() const;
        size_t max_size() const;
        
        bool empty() const;
        
        bool IsEnd() const;
        
        void ResetOffset(size_t offsetFromStart = 0);
        size_t GetOffset() const;
        const uint8_t* GetOffsetBytes(size_t length) const;
        bool Skip(size_t numBytesToSkip);
        
        size_t Read(void* ptr, size_t numBytesToRead);
        size_t ReadAndSwap(void* ptr, size_t numBytesToRead, bool swapBytes);
        
        bool ReadOrFail(void* ptr, size_t numBytesToRead);
        bool ReadOrFail(ByteBuffer& readInto, size_t length);
        
        template <typename T>
        bool ReadLittleEndian16(T& value)
        {
            //Read a 16 bit integer value that is known to have its lowest byte first

            if (size_left() < 2)
                return false;

            auto buf = this->dataPointer;
            this->dataPointer += 2;
            this->remainingLength -= 2;

            value = ((T)buf[1] << 8) | ((T)buf[0]);
            return true;
        }

        template <typename T>
        bool ReadBigEndian16(T& value)
        {
            //Read a 16 bit integer value that is known to have its highest byte first

            if (size_left() < 2)
                return false;

            auto buf = this->dataPointer;
            this->dataPointer += 2;
            this->remainingLength -= 2;
            
            value = ((T)buf[0] << 8) | ((T)buf[1]);
            return true;
        }

        template <typename T>
        bool ReadLittleEndian32(T& value)
        {
            //Read a 32 bit integer value that is known to have its lowest byte first

            if (size_left() < 4)
                return false;

            auto buf = this->dataPointer;
            this->dataPointer += 4;
            this->remainingLength -= 4;

            value = ((T)buf[3] << 24) | ((T)buf[2] << 16) | ((T)buf[1] << 8) | ((T)buf[0]);
            return true;
        }

        template <typename T>
        bool ReadBigEndian32(T& value)
        {
            //Read a 32 bit integer value that is known to have its highest byte first

            if (size_left() < 4)
                return false;

            auto buf = this->dataPointer;
            this->dataPointer += 4;
            this->remainingLength -= 4;

            value = ((T)buf[0] << 24) | ((T)buf[1] << 16) | ((T)buf[2] << 8) | ((T)buf[3]);
            return true;
        }

        template <typename T>
        bool ReadLittleEndian64(T& value)
        {
            //Read a 64 bit integer value that is known to have its lowest byte first
            
            if (size_left() < 8)
                return false;
            
            auto buf = this->dataPointer;
            this->dataPointer += 8;
            this->remainingLength -= 8;
            
            value = ((T)buf[7] << 56) | ((T)buf[6] << 48) | ((T)buf[5] << 40) | ((T)buf[4] << 32) | ((T)buf[3] << 24) | ((T)buf[2] << 16) | ((T)buf[1] << 8) | ((T)buf[0]);
            return true;
        }
        
        template <typename T>
        bool ReadBigEndian64(T& value)
        {
            //Read a 64 bit integer value that is known to have its highest byte first
            
            if (size_left() < 8)
                return false;
            
            auto buf = this->dataPointer;
            this->dataPointer += 8;
            this->remainingLength -= 8;
            
            value = ((T)buf[0] << 56) | ((T)buf[1] << 48) | ((T)buf[2] << 40) | ((T)buf[3] << 32) | ((T)buf[4] << 24) | ((T)buf[5] << 16) | ((T)buf[6] << 8) | ((T)buf[7]);
            return true;
        }
        
    private:
        Allocator* allocator;
        size_t byteCount;
        size_t remainingLength;
        const uint8_t* dataPointerStart;
        const uint8_t* dataPointer;
    };

} }
