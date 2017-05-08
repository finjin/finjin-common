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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class WxByteBuffer
    {
    public:
        WxByteBuffer();
        WxByteBuffer(const WxByteBuffer& other);
        WxByteBuffer(WxByteBuffer&& other);
        ~WxByteBuffer();

        WxByteBuffer& operator = (const WxByteBuffer& other);
        WxByteBuffer& operator = (WxByteBuffer&& other);

        void Create(size_t count);
        void Destroy();

        void EnsureCanAppend(size_t byteCount);
        uint8_t* AllocateBytes(size_t byteCount);
        uint8_t* resize(size_t byteCount);

        void TruncateBytes(size_t newByteCount);
        void clear();

        size_t GetUnusedByteCount() const;

        bool full() const;
        bool empty() const;

        size_t GetByteCount() const;
        size_t size() const;

        const uint8_t* GetBytes() const;
        uint8_t* GetBytes();

        const uint8_t* data() const;
        uint8_t* data();

        const uint8_t& operator [] (size_t index) const;
        uint8_t& operator [] (size_t index);

        void Fill(uint8_t value);
        void Fill(uint8_t value, uint8_t* start, uint8_t* end);

        WxByteBuffer& Write(const void* bytes, size_t byteCount);
        WxByteBuffer& Write(const wxString& s);
        WxByteBuffer& Write(const std::string& s);
        WxByteBuffer& Write(const char* s);
        WxByteBuffer& Write(uint8_t b);
        WxByteBuffer& WriteBase64(const void* bytes, size_t byteCount);

        size_t WriteBase64(const void* bytes, size_t byteCount, size_t maxBase64Bytes);

    private:
        void ReallocateData(size_t count);

        uint8_t* _Allocate(size_t byteCount);
        void _Deallocate(void* mem);

    private:
        uint8_t* bytes;
        size_t maxByteCount;
        size_t byteCount;
    };

} }
