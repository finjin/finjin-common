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

    class FINJIN_COMMON_LIBRARY_API PNGWriter
    {
    public:
        PNGWriter();
        ~PNGWriter();

        enum class WriteResult
        {
            SUCCESS,
            FAILED_TO_START_WRITE,
            INVALID_CHANNEL_COUNT,
            INVALID_BYTES_PER_CHANNEL,
            NOT_ENOUGH_MEMORY
        };

        uint32_t GetInputChannelCount() const; //Information about input data
        void SetInputChannelCount(uint32_t value);

        uint32_t GetInputBytesPerChannel() const; //Information about input data
        void SetInputBytesPerChannel(uint32_t value);

        bool GetInputSRGB() const; //Information about input data
        void SetInputSRGB(bool value);

        bool GetOutputReverseRGB() const; //Transforms output data on Write
        void SetOutputReverseRGB(bool value);

        bool GetOutputSwapAlpha() const; //Transforms output data on Write
        void SetOutputSwapAlpha(bool value);

        WriteResult Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& pngOutputBuffer);
        Utf8String GetWriteResultString(WriteResult result) const;

        void Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& pngOutputBuffer, Error& error);

    private:
        uint32_t inputChannelCount;
        uint32_t inputBytesPerChannel;
        bool inputIsSRGB;
        bool outputReverseRGB; //false = RGB, true = BGR
        bool outputSwapAlpha;
    };

} }
