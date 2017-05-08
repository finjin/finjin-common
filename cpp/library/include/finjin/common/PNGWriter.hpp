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

        enum WriteResult
        {
            SUCCESS,
            FAILED_TO_START_WRITE,
            NOT_ENOUGH_MEMORY
        };
        WriteResult WriteRGBA8888(const void* pixels, uint32_t width, uint32_t height, ByteBuffer& pngOutputBuffer); //Same format in and out (32 bits per pixel)
        Utf8String GetWriteResultString(WriteResult result) const;

        void WriteRGBA8888(const void* pixels, uint32_t width, uint32_t height, ByteBuffer& pngOutputBuffer, Error& error);
    };

} }
