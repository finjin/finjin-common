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
#include "finjin/common/JPEGWriter.hpp"
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
struct CustomJPEGWriteError
{
    CustomJPEGWriteError()
    {
        FINJIN_ZERO_ITEM(*this);
    }

    struct jpeg_error_mgr pub; //Public fields. Must be first
    jmp_buf setjmp_buffer; //For return to caller
    char invalidImageErrorMessage[JMSG_LENGTH_MAX + 1];
};


//Local functions---------------------------------------------------------------
static void CustomJPEGWriteErrorExit(j_common_ptr cinfo)
{
    auto customError = reinterpret_cast<CustomJPEGWriteError*>(cinfo->err);

    (*cinfo->err->format_message)(cinfo, customError->invalidImageErrorMessage);

    //Return control to the setjmp point
    longjmp(customError->setjmp_buffer, 1);
}


//Implementation----------------------------------------------------------------
JPEGWriter::JPEGWriter()
{
    this->inputChannelCount = 4;
    this->outputChannelCount = 0;
    this->outputQuality = 75;
}

JPEGWriter::~JPEGWriter()
{
}

uint32_t JPEGWriter::GetInputChannelCount() const
{
    return this->inputChannelCount;
}

void JPEGWriter::SetInputChannelCount(uint32_t value)
{
    this->inputChannelCount = value;
}

uint32_t JPEGWriter::GetOutputChannelCount() const
{
    return this->outputChannelCount;
}

void JPEGWriter::SetOutputChannelCount(uint32_t value)
{
    this->outputChannelCount = value;
}

uint32_t JPEGWriter::GetOutputQuality() const
{
    return this->outputQuality;
}

void JPEGWriter::SetOutputQuality(uint32_t value)
{
    this->outputQuality = value;
}

JPEGWriter::WriteResult JPEGWriter::Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& jpegOutputBuffer)
{
    struct jpeg_compress_struct cinfo;
    CustomJPEGWriteError customError;

    switch (this->inputChannelCount)
    {
        case 1: break;
        case 2: break;
        case 3: break;
        case 4: break;
        default: return WriteResult::INVALID_CHANNEL_COUNT;
    }

    //Step 1: Allocate and initialize JPEG compression object-------------------
    cinfo.err = jpeg_std_error(&customError.pub);
    customError.pub.error_exit = CustomJPEGWriteErrorExit; //Override error_exit

    //Establish the setjmp return context for CustomJPEGReadErrorExit to use
    if (setjmp(customError.setjmp_buffer))
    {
        this->invalidImageErrorMessage = customError.invalidImageErrorMessage;

        jpeg_destroy_compress(&cinfo);
        return WriteResult::FAILED_TO_WRITE_IMAGE;
    }

    //Initialize the JPEG compression object
    cinfo.client_data = this; //Not yet used for anything
    jpeg_create_compress(&cinfo);
    
    //Step 2: Specify data destination-------------------
    jpegOutputBuffer.maximize();
    auto outBuffer = static_cast<unsigned char*>(jpegOutputBuffer.data());
    auto outSize = static_cast<unsigned long>(jpegOutputBuffer.size());
    jpeg_mem_dest(&cinfo, &outBuffer, &outSize);

    //Step 3: Set parameters for compression-------------------
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = static_cast<int>(this->inputChannelCount);
    if (this->outputChannelCount == 0)
    {
        switch (this->inputChannelCount)
        {
            case 1:
            case 2:
            case 3: cinfo.num_components = this->inputChannelCount; break;
            case 4: cinfo.num_components = 3; break;
        }
    }
    jpeg_set_defaults(&cinfo);

    //Set custom settings
    jpeg_set_quality(&cinfo, static_cast<int>(Limited(this->outputQuality, 1, 100)), TRUE);

    //Step 4: Start compressor-------------------
    jpeg_start_compress(&cinfo, TRUE);

    //Step 5: Write scanlines-------------------
    auto rowBuffer = const_cast<uint8_t*>(static_cast<const uint8_t*>(pixels));
    while (cinfo.next_scanline < cinfo.image_height)
    {
        jpeg_write_scanlines(&cinfo, &rowBuffer, 1);
        rowBuffer += rowStride;
    }

    //Step 6: Finish compression-------------------
    jpeg_finish_compress(&cinfo);
    jpegOutputBuffer.resize(outSize);

    //Step 7: Release JPEG compression object-------------------
    jpeg_destroy_compress(&cinfo);

    return WriteResult::SUCCESS;
}

Utf8String JPEGWriter::GetWriteResultString(WriteResult result) const
{
    switch (result)
    {
        case WriteResult::NOT_ENOUGH_MEMORY: return "Not enough memory to write JPEG.";
        case WriteResult::INVALID_CHANNEL_COUNT: return Utf8StringFormatter::Format("Invalid channel count '%1%'.", this->inputChannelCount);
        case WriteResult::FAILED_TO_WRITE_IMAGE:
        {
            if (!this->invalidImageErrorMessage.empty())
                return FINJIN_FORMAT_ERROR_MESSAGE("Failed to write image: %1%.", this->invalidImageErrorMessage);
            else
                return "Failed to write image.";
        }
        default: return Utf8String::GetEmpty();
    }
}

void JPEGWriter::Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& jpegOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = Write(pixels, width, height, rowStride, jpegOutputBuffer);
    if (result != WriteResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetWriteResultString(result));
}
