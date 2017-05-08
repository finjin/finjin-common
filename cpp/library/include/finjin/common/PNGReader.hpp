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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API PNGReader
    {
    public:
        struct PixelFormat
        {
            static uint32_t GRAY;
            static uint32_t GA;
            static uint32_t AG;
            static uint32_t RGB;
            static uint32_t BGR;
            static uint32_t RGBA;
            static uint32_t ARGB;
            static uint32_t BGRA;
            static uint32_t ABGR;
            static uint32_t LINEAR_RGB;
            static uint32_t LINEAR_RGBA;
        };

        PNGReader();
        ~PNGReader();

        bool GetConvert16bitTo8bit() const;
        void SetConvert16bitTo8bit(bool value);

        bool GetAddAlpha() const;
        void SetAddAlpha(bool value);

        bool GetSwapAlpha() const;
        void SetSwapAlpha(bool value);

        uint32_t GetPixelFormat() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        bool IsSRGB() const;

        uint32_t GetChannelCount() const;
        uint32_t GetBytesPerChannel() const;
        uint32_t GetBytesPerPixel() const;
        uint32_t GetBytesPerRow() const;

        enum class ReadResult
        {
            SUCCESS,
            FAILED_TO_READ_SIGNATURE,
            INVALID_SIGNATURE,
            NOT_ENOUGH_MEMORY_TO_START_IMAGE_READ,
            FAILED_TO_START_IMAGE_READ,
            NOT_ENOUGH_MEMORY_FOR_IMAGE,
            FAILED_TO_FINISH_IMAGE_READ
        };
        ReadResult ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer);
        Utf8String GetReadResultString(ReadResult result) const;

        void ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer, Error& error);

    private:
        uint32_t width, height;
        uint32_t channelCount;
        uint32_t bytesPerChannel;
        uint32_t format;
        uint32_t flags;

        bool convert16bitTo8bit;
        bool addAlpha;
        bool swapAlpha;
    };

} }
