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
     * Note that unlike PNGWriter, the internal implementation may result in some temporary allocations outside of the output buffer
     */
    class FINJIN_COMMON_LIBRARY_API JPEGWriter
    {
    public:
        JPEGWriter();
        ~JPEGWriter();

        enum class WriteResult
        {
            SUCCESS,
            NOT_ENOUGH_MEMORY,
            INVALID_CHANNEL_COUNT,
            FAILED_TO_WRITE_IMAGE
        };

        uint32_t GetInputChannelCount() const; //Information about input data
        void SetInputChannelCount(uint32_t value);

        uint32_t GetOutputChannelCount() const; //Transforms output data on Write
        void SetOutputChannelCount(uint32_t value);

        uint32_t GetOutputQuality() const; //Transforms output data on Write. Varies between 1 and 100
        void SetOutputQuality(uint32_t value);

        WriteResult Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& jpegOutputBuffer);
        Utf8String GetWriteResultString(WriteResult result) const;

        void Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& jpegOutputBuffer, Error& error);

    private:
        uint32_t inputChannelCount;
        uint32_t outputChannelCount;
        uint32_t outputQuality;
        Path invalidImageErrorMessage; //Use Path to store the message since it has a longer default length
    };

} }
