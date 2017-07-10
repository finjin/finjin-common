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
#include "finjin/common/JPEGReader.hpp"
#include "finjin/common/Allocator.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/ForwardAllocator.hpp"
#include "finjin/common/MemoryArena.hpp"
extern "C" {
    #include <jpeglib.h>
}
#include <setjmp.h>
#include <boost/thread/null_mutex.hpp>

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
struct CustomJPEGReadError
{
    CustomJPEGReadError()
    {
        FINJIN_ZERO_ITEM(*this);
    }

    struct jpeg_error_mgr pub; //Public fields. Must be first
    jmp_buf setjmp_buffer; //For return to caller
    char invalidImageErrorMessage[JMSG_LENGTH_MAX + 1];
};


//Local functions---------------------------------------------------------------
static void CustomJPEGReadErrorExit(j_common_ptr cinfo)
{
    auto customError = reinterpret_cast<CustomJPEGReadError*>(cinfo->err);

    (*cinfo->err->format_message)(cinfo, customError->invalidImageErrorMessage);

    //Return control to the setjmp point
    longjmp(customError->setjmp_buffer, 1);
}


//Implementation----------------------------------------------------------------
JPEGReader::JPEGReader()
{
    this->width = 0;
    this->height = 0;
    this->channelCount = 0;
    this->format = PixelFormat::NONE;
}

JPEGReader::~JPEGReader()
{
}

JPEGReader::PixelFormat JPEGReader::GetPixelFormat() const
{
    return this->format;
}

uint32_t JPEGReader::GetWidth() const
{
    return this->width;
}

uint32_t JPEGReader::GetHeight() const
{
    return this->height;
}

uint32_t JPEGReader::GetChannelCount() const
{
    return this->channelCount;
}

uint32_t JPEGReader::GetBytesPerChannel() const
{
    return 1;
}

uint32_t JPEGReader::GetBytesPerPixel() const
{
    return this->channelCount;
}

uint32_t JPEGReader::GetBytesPerRow() const
{
    return this->channelCount * this->width;
}

JPEGReader::ReadResult JPEGReader::ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer)
{
    //jpeg_stdio_src

    struct jpeg_decompress_struct cinfo;
    CustomJPEGReadError customError;

    //Step 1: allocate and initialize JPEG decompression object--------------------------

    //Set up the JPEG error routines
    cinfo.err = jpeg_std_error(&customError.pub);
    customError.pub.error_exit = CustomJPEGReadErrorExit; //Override error_exit

    //Establish the setjmp return context for CustomJPEGReadErrorExit to use
    if (setjmp(customError.setjmp_buffer))
    {
        this->invalidImageErrorMessage = customError.invalidImageErrorMessage;

        jpeg_destroy_decompress(&cinfo);
        return ReadResult::FAILED_TO_READ_IMAGE;
    }

    //Initialize the JPEG decompression object
    cinfo.client_data = this; //Not yet used for anything
    jpeg_create_decompress(&cinfo);
    
    //Step 2: Specify data source----------------------------------------------
    jpeg_mem_src(&cinfo, reader.data_left(), static_cast<unsigned long>(reader.size_left()));

    //Step 3: Read file parameters with jpeg_read_header()----------------------
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
    {
        jpeg_destroy_decompress(&cinfo);
        return ReadResult::INVALID_SIGNATURE;
    }

    //Step 4: Set parameters for decompression-------------------------------
    //Accept defaults by doing nothing

    //Step 5: Start decompressor-----------------------------------------
    jpeg_start_decompress(&cinfo);

    auto rowStride = cinfo.output_width * cinfo.output_components;
    auto decompressedImageSize = rowStride * cinfo.output_height;
    if (imageOutputBuffer.max_size() < decompressedImageSize)
    {
        jpeg_destroy_decompress(&cinfo);
        return ReadResult::NOT_ENOUGH_MEMORY_FOR_IMAGE;
    }
    imageOutputBuffer.resize(decompressedImageSize);

    this->width = cinfo.output_width;
    this->height = cinfo.output_height;
    this->channelCount = static_cast<uint32_t>(cinfo.output_components);
    switch (this->channelCount)
    {
        case 1:
        case 2: this->format = PixelFormat::GRAY; break;
        case 3:
        case 4: this->format = PixelFormat::RGB; break;
        default:
        {
            jpeg_destroy_decompress(&cinfo);
            return ReadResult::INVALID_CHANNEL_COUNT;
        }
    }

    //Step 6: Read scanlines----------------------------------------------
    auto rowBuffer = imageOutputBuffer.data();
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, &rowBuffer, 1);
        rowBuffer += rowStride;
    }

    //Step 7: Finish decompression--------------------------------------------
    jpeg_finish_decompress(&cinfo);

    //Step 8: Release JPEG decompression object------------------------------
    jpeg_destroy_decompress(&cinfo);

    return ReadResult::SUCCESS;
}

Utf8String JPEGReader::GetReadResultString(ReadResult result) const
{
    switch (result)
    {
        case ReadResult::INVALID_SIGNATURE: return "Invalid signature";
        case ReadResult::NOT_ENOUGH_MEMORY_FOR_IMAGE: return "Not enough memory to read image.";
        case ReadResult::INVALID_CHANNEL_COUNT: return FINJIN_FORMAT_ERROR_MESSAGE("Invalid channel count '%1%'.", this->channelCount);
        case ReadResult::FAILED_TO_READ_IMAGE:
        {
            if (!this->invalidImageErrorMessage.empty())
                return FINJIN_FORMAT_ERROR_MESSAGE("Failed to read image: %1%.", this->invalidImageErrorMessage);
            else
                return "Failed to read image.";
        }
        default: return Utf8String::GetEmpty();
    }
}

void JPEGReader::ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadImage(reader, imageOutputBuffer);
    if (result != ReadResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadResultString(result));
}
