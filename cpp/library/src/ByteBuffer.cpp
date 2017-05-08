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
#include "finjin/common/Base64.hpp"
#include "finjin/common/ByteBuffer.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//ByteBuffer
ByteBuffer::ByteBuffer()
{
    this->items = nullptr;
    this->count = 0;
    this->maxCount = 0;
    this->allocator = nullptr;
    this->resizeStrategy = MemoryResizeStrategy::LIMIT;
}

ByteBuffer::ByteBuffer(ByteBuffer&& other)
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

ByteBuffer& ByteBuffer::operator = (ByteBuffer&& other)
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

ByteBuffer::~ByteBuffer()
{
    Destroy();
}

bool ByteBuffer::Create(size_t count, Allocator* allocator, MemoryResizeStrategy resizeStrategy)
{
    Destroy();

    if (allocator == nullptr)
        allocator = Allocator::GetDefaultAllocator();

    void* mem = nullptr;
    if (count > 0)
    {
        if (allocator != nullptr)
            mem = allocator->Allocate(count, FINJIN_CALLER_ARGUMENTS);
        else
            mem = Allocator::SystemAllocate(count, FINJIN_CALLER_ARGUMENTS);
    }
    this->items = static_cast<uint8_t*>(mem);
    this->count = count;
    this->maxCount = count;
    this->allocator = allocator;
    this->resizeStrategy = resizeStrategy;

    if (count > 0)
        return mem != nullptr;
    else
        return true;
}

bool ByteBuffer::Create(const void* bytes, size_t count, Allocator* allocator, MemoryResizeStrategy resizeStrategy)
{
    if (!Create(count, allocator, resizeStrategy))
        return false;

    if (bytes != nullptr)
        FINJIN_COPY_MEMORY(this->items, bytes, count);

    return true;
}

bool ByteBuffer::CreateEmpty(size_t count, Allocator* allocator, MemoryResizeStrategy resizeStrategy)
{
    auto result = Create(count, allocator, resizeStrategy);
    this->count = 0;
    return result;
}

void ByteBuffer::Destroy()
{
    if (this->allocator != nullptr)
        this->allocator->Deallocate(this->items);
    else
        Allocator::SystemDeallocate(this->items);

    this->items = nullptr;
    this->count = 0;
    this->maxCount = 0;
    this->allocator = nullptr;
    this->resizeStrategy = MemoryResizeStrategy::LIMIT;
}

Allocator* ByteBuffer::GetAllocator()
{
    return this->allocator;
}

MemoryResizeStrategy ByteBuffer::GetResizeStrategy() const
{
    return this->resizeStrategy;
}

void ByteBuffer::SetResizeStrategy(MemoryResizeStrategy value)
{
    this->resizeStrategy = value;
}

void ByteBuffer::Fill(uint8_t value)
{
    std::fill_n(this->items, this->count, value);
}

void ByteBuffer::Fill(uint8_t value, uint8_t* start, uint8_t* end)
{
    assert(start <= end && start >= this->items && start <= this->items + this->count && end >= this->items && end <= this->items + this->count);

    if (start < end)
        std::fill_n(start, end - start, value);
}

void ByteBuffer::EnsureCanAppend(size_t byteCount)
{
    if (byteCount > size_left())
        Reallocate(this->count + byteCount);
}

ByteBuffer& ByteBuffer::Write(const void* bytes, size_t byteCount)
{
    //Reallocate if incoming bytes cannot fit within the unused area
    if (byteCount > size_left())
        Reallocate(this->count + byteCount);

    //Copy and increment byte count
    FINJIN_COPY_MEMORY(&this->items[this->count], bytes, byteCount);
    this->count += byteCount;

    return *this;
}

ByteBuffer& ByteBuffer::Write(const Utf8String& s)
{
    return Write(s.c_str(), s.length() * sizeof(char));
}

ByteBuffer& ByteBuffer::Write(const char* s)
{
    if (s != nullptr && s[0] != 0)
        Write(s, strlen(s));

    return *this;
}

ByteBuffer& ByteBuffer::Write(uint8_t b)
{
    return Write(&b, 1);
}

ByteBuffer& ByteBuffer::WriteBase64(const void* bytes, size_t byteCount)
{
    auto base64Length = Base64::ToBase64Count(byteCount);

    //Reallocate if incoming bytes cannot fit within the unused area
    if (base64Length > size_left())
        Reallocate(this->count + base64Length);

    this->count += Base64::ToBase64(bytes, byteCount, &this->items[this->count]);

    return *this;
}

size_t ByteBuffer::WriteBase64(const void* bytes, size_t byteCount, size_t maxBase64Bytes)
{
    maxBase64Bytes = std::min(maxBase64Bytes, size_left());

    auto base64Length = Base64::ToBase64Count(byteCount);
    if (base64Length > maxBase64Bytes)
        byteCount = Base64::ToByteCount(maxBase64Bytes);

    if (byteCount > 0)
        this->count += Base64::ToBase64(bytes, byteCount, &this->items[this->count]);

    return byteCount;
}

void ByteBuffer::Truncate(size_t newCount)
{
    this->count = std::min(this->count, newCount);
}

void ByteBuffer::Reallocate(size_t count)
{
    //Store old data
    auto oldItems = this->items;

    //Reallocate enough to store new data
    if (this->allocator != nullptr)
        this->items = static_cast<uint8_t*>(allocator->Allocate(count, FINJIN_CALLER_ARGUMENTS));
    else
        this->items = static_cast<uint8_t*>(Allocator::SystemAllocate(count, FINJIN_CALLER_ARGUMENTS));

    if (this->items != nullptr)
        this->maxCount = count;
    else
        this->count = this->maxCount = 0;

    //Copy old data and delete old bytes
    if (oldItems != nullptr)
    {
        FINJIN_COPY_MEMORY(this->items, oldItems, this->count);
        if (this->allocator != nullptr)
            allocator->Deallocate(oldItems);
        else
            Allocator::SystemDeallocate(oldItems);
    }
}

//ByteBufferReader
ByteBufferReader::ByteBufferReader()
{
    this->byteCount = this->remainingLength = 0;
    this->dataPointerStart = this->dataPointer = nullptr;
}

ByteBufferReader::ByteBufferReader(const ByteBuffer& data)
{
    this->byteCount = this->remainingLength = data.size();
    this->dataPointerStart = this->dataPointer = data.data();
}

ByteBufferReader::ByteBufferReader(const void* bytes, size_t byteCount)
{
    this->byteCount = this->remainingLength = byteCount;
    this->dataPointerStart = this->dataPointer = static_cast<const uint8_t*>(bytes);
}

const uint8_t* ByteBufferReader::data_start() const
{
    return this->dataPointerStart;
}

const uint8_t* ByteBufferReader::data_left() const
{
    if (this->dataPointerStart != nullptr)
        return this->dataPointerStart + GetOffset();
    else
        return nullptr;
}

size_t ByteBufferReader::size_left() const
{
    return this->remainingLength;
}

size_t ByteBufferReader::max_size() const
{
    return this->byteCount;
}

bool ByteBufferReader::empty() const
{
    return this->byteCount == 0;
}

bool ByteBufferReader::IsEnd() const
{
    return (this->remainingLength == 0) ? true : false;
}

size_t ByteBufferReader::GetOffset() const
{
    return this->dataPointer - this->dataPointerStart;
}

const uint8_t* ByteBufferReader::GetOffsetBytes(size_t length) const
{
    if (this->remainingLength < length)
        return nullptr;

    return this->dataPointer;
}

void ByteBufferReader::SetOffset(size_t offsetFromStart)
{
    this->remainingLength = this->byteCount - offsetFromStart;
    this->dataPointer = this->dataPointerStart + offsetFromStart;
}

bool ByteBufferReader::Skip(size_t numBytesToSkip)
{
    if (this->remainingLength < numBytesToSkip)
        return false;

    this->dataPointer += numBytesToSkip;
    this->remainingLength -= numBytesToSkip;
    return true;
}

size_t ByteBufferReader::Read(void* ptr, size_t numBytesToRead)
{
    numBytesToRead = std::min(numBytesToRead, this->remainingLength);

    FINJIN_COPY_MEMORY(ptr, this->dataPointer, numBytesToRead);
    this->dataPointer += numBytesToRead;
    this->remainingLength -= numBytesToRead;

    return numBytesToRead;
}

size_t ByteBufferReader::ReadAndSwap(void* ptr, size_t numBytesToRead, bool swapBytes)
{
    auto bytesRead = Read(ptr, numBytesToRead);
    if (bytesRead == numBytesToRead && swapBytes)
    {
        auto bytes = static_cast<uint8_t*>(ptr);
        for (size_t i = 0; i < bytesRead / 2; i++)
            std::swap(bytes[i], bytes[bytesRead - i - 1]);
    }
    return bytesRead;
}

bool ByteBufferReader::ReadOrFail(void* ptr, size_t numBytesToRead)
{
    if (this->remainingLength < numBytesToRead)
        return false;

    FINJIN_COPY_MEMORY(ptr, this->dataPointer, numBytesToRead);
    this->dataPointer += numBytesToRead;
    this->remainingLength -= numBytesToRead;
    return true;
}

bool ByteBufferReader::ReadOrFail(ByteBuffer& readInto, size_t length)
{
    if (readInto.resize(length) < length)
        return false;
    return ReadOrFail(readInto.data(), length);
}
