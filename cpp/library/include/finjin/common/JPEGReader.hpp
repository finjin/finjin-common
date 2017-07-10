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
#include "finjin/common/Path.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Note that unlike PNGReader, the internal implementation may result in some temporary allocations outside of the output buffer
     */
    class FINJIN_COMMON_LIBRARY_API JPEGReader
    {
    public:
        enum class PixelFormat
        {
            NONE,
            GRAY,
            RGB
        };

        JPEGReader();
        ~JPEGReader();

        PixelFormat GetPixelFormat() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        uint32_t GetChannelCount() const;
        uint32_t GetBytesPerChannel() const;
        uint32_t GetBytesPerPixel() const;
        uint32_t GetBytesPerRow() const;

        enum class ReadResult
        {
            SUCCESS,
            INVALID_SIGNATURE,
            NOT_ENOUGH_MEMORY_FOR_IMAGE,
            INVALID_CHANNEL_COUNT,
            FAILED_TO_READ_IMAGE
        };
        ReadResult ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer);
        Utf8String GetReadResultString(ReadResult result) const;

        void ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer, Error& error);

    private:
        uint32_t width, height;
        uint32_t channelCount;
        PixelFormat format;
        Path invalidImageErrorMessage; //Use Path to store the message since it has a longer default length
    };

} }
