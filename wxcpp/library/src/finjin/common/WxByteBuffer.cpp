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
#include "finjin/common/WxByteBuffer.hpp"
#include "finjin/common/WxBase64.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//WxByteBuffer
WxByteBuffer::WxByteBuffer()
{
    this->bytes = nullptr;
    this->maxByteCount = 0;
    this->byteCount = 0;
}

WxByteBuffer::WxByteBuffer(const WxByteBuffer& other)
{
    this->bytes = nullptr;
    this->maxByteCount = 0;
    this->byteCount = 0;

    operator = (other);
}

WxByteBuffer::WxByteBuffer(WxByteBuffer&& other)
{
    this->bytes = other.bytes;
    this->maxByteCount = other.maxByteCount;
    this->byteCount = other.byteCount;

    other.bytes = nullptr;
    other.maxByteCount = 0;
    other.byteCount = 0;
}

WxByteBuffer::~WxByteBuffer()
{
    _Deallocate(this->bytes);
}

WxByteBuffer& WxByteBuffer::operator = (const WxByteBuffer& other)
{
    AllocateBytes(other.maxByteCount);
    if (other.byteCount > 0)
        FINJIN_COPY_MEMORY(this->bytes, other.bytes, other.byteCount);
    this->byteCount = other.byteCount;

    return *this;
}

WxByteBuffer& WxByteBuffer::operator = (WxByteBuffer&& other)
{
    _Deallocate(this->bytes);

    this->bytes = other.bytes;
    this->maxByteCount = other.maxByteCount;
    this->byteCount = other.byteCount;

    other.bytes = nullptr;
    other.maxByteCount = 0;
    other.byteCount = 0;

    return *this;
}

void WxByteBuffer::Create(size_t count)
{
    Destroy();

    resize(count);
}

void WxByteBuffer::Destroy()
{
    _Deallocate(this->bytes);
    this->bytes = nullptr;

    this->maxByteCount = 0;
    this->byteCount = 0;
}

void WxByteBuffer::EnsureCanAppend(size_t byteCount)
{
    if (byteCount > GetUnusedByteCount())
        ReallocateData(this->byteCount + byteCount);
}

uint8_t* WxByteBuffer::AllocateBytes(size_t byteCount)
{
    if (byteCount > this->maxByteCount)
    {
        _Deallocate(this->bytes);
        this->bytes = _Allocate(byteCount);
        this->maxByteCount = byteCount;
    }
    this->byteCount = byteCount;
    return this->bytes;
}

uint8_t* WxByteBuffer::resize(size_t byteCount)
{
    return AllocateBytes(byteCount);
}

void WxByteBuffer::TruncateBytes(size_t newByteCount)
{
    //This doesn't actually shorten the length of the bytes vector
    this->byteCount = std::min(this->maxByteCount, newByteCount);
}

void WxByteBuffer::clear()
{
    TruncateBytes(0);
}

size_t WxByteBuffer::GetUnusedByteCount() const
{
    return this->maxByteCount - this->byteCount;
}

bool WxByteBuffer::full() const
{
    return GetUnusedByteCount() == 0;
}

bool WxByteBuffer::empty() const
{
    return this->byteCount == 0;
}

size_t WxByteBuffer::GetByteCount() const
{
    return this->byteCount;
}

size_t WxByteBuffer::size() const
{
    return this->byteCount;
}

const uint8_t* WxByteBuffer::GetBytes() const
{
    return this->byteCount > 0 ? this->bytes : nullptr;
}

uint8_t* WxByteBuffer::GetBytes()
{
    return this->bytes;
}

const uint8_t* WxByteBuffer::data() const
{
    return this->byteCount > 0 ? this->bytes : nullptr;
}

uint8_t* WxByteBuffer::data()
{
    return this->bytes;
}

const uint8_t& WxByteBuffer::operator [] (size_t index) const
{
    static uint8_t dummy = 0;
    assert(index < this->byteCount);
    return this->byteCount > 0 ? this->bytes[index] : dummy;
}

uint8_t& WxByteBuffer::operator [] (size_t index)
{
    static uint8_t dummy = 0;
    assert(index < this->byteCount);
    return this->byteCount > 0 ? this->bytes[index] : dummy;
}

void WxByteBuffer::Fill(uint8_t value)
{
    std::fill_n(this->bytes, this->byteCount, value);
}

void WxByteBuffer::Fill(uint8_t value, uint8_t* start, uint8_t* end)
{
    assert(start <= end && start >= this->bytes && start <= this->bytes + this->byteCount && end >= this->bytes && end <= this->bytes + this->byteCount);

    if (start < end)
        std::fill_n(start, end - start, value);
}

WxByteBuffer& WxByteBuffer::Write(const void* bytes, size_t byteCount)
{
    //Reallocate if incoming bytes cannot fit within the unused area
    if (byteCount > GetUnusedByteCount())
        ReallocateData(this->byteCount + byteCount);

    //Copy and increment byte count
    FINJIN_COPY_MEMORY(&this->bytes[this->byteCount], static_cast<const uint8_t*>(bytes), byteCount);
    this->byteCount += byteCount;

    return *this;
}

WxByteBuffer& WxByteBuffer::Write(const wxString& s)
{
    return Write(s.ToUTF8().data());
}

WxByteBuffer& WxByteBuffer::Write(const std::string& s)
{
    return Write(s.c_str(), s.length());
}

WxByteBuffer& WxByteBuffer::Write(uint8_t b)
{
    return Write(&b, 1);
}

WxByteBuffer& WxByteBuffer::Write(const char* s)
{
    if (s != nullptr && s[0] != 0)
        Write(s, strlen(s));

    return *this;
}

WxByteBuffer& WxByteBuffer::WriteBase64(const void* bytes, size_t byteCount)
{
    auto base64Length = WxBase64::ToBase64Count(byteCount);

    //Reallocate if incoming bytes cannot fit within the unused area
    if (base64Length > GetUnusedByteCount())
        ReallocateData(this->byteCount + base64Length);

    this->byteCount += WxBase64::ToBase64(bytes, byteCount, &this->bytes[this->byteCount]);

    return *this;
}

size_t WxByteBuffer::WriteBase64(const void* bytes, size_t byteCount, size_t maxBase64Bytes)
{
    maxBase64Bytes = std::min(maxBase64Bytes, GetUnusedByteCount());

    auto base64Length = WxBase64::ToBase64Count(byteCount);
    if (base64Length > maxBase64Bytes)
        byteCount = WxBase64::ToByteCount(maxBase64Bytes);

    if (byteCount > 0)
        this->byteCount += WxBase64::ToBase64(bytes, byteCount, &this->bytes[this->byteCount]);

    return byteCount;
}

void WxByteBuffer::ReallocateData(size_t count)
{
    //Store old data
    auto oldBytes = this->bytes;

    //Reallocate enough to store new data
    this->bytes = _Allocate(count);
    this->maxByteCount = count;

    //Copy old data and delete old bytes
    FINJIN_COPY_MEMORY(this->bytes, oldBytes, this->byteCount);
    _Deallocate(oldBytes);
}

uint8_t* WxByteBuffer::_Allocate(size_t byteCount)
{
    auto result = static_cast<uint8_t*>(malloc(byteCount));
    assert(result != nullptr);
    return result;
}

void WxByteBuffer::_Deallocate(void* mem)
{
    free(mem);
}
