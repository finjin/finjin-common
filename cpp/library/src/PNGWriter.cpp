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
#include "finjin/common/PNGWriter.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/ForwardAllocator.hpp"
#include "finjin/common/MemoryArena.hpp"
#include <png.h>
#include <boost/thread/null_mutex.hpp>

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
struct ByteBufferAndInfo
{
    ByteBufferAndInfo(ByteBuffer& pngOutputBuffer, size_t sizeLeft)
    {
        this->pngOutputBuffer = &pngOutputBuffer;
        this->sizeLeft = sizeLeft;
        this->memoryExhausted = false;
    }

    ByteBuffer* pngOutputBuffer;
    size_t sizeLeft;
    bool memoryExhausted;
};


//Local functions---------------------------------------------------------------
static void PngWriteErrorHandler(png_structp png, png_const_charp message)
{
    FINJIN_DEBUG_LOG_ERROR("PNG writer error: %1%", message);
}

static void PngWriteWarningHandler(png_structp png, png_const_charp message)
{
    FINJIN_DEBUG_LOG_WARNING("PNG writer warning: %1%", message);
}

static void PngWriteCallback(png_structp png, png_bytep data, png_size_t length)
{
    auto byteBufferAndInfo = static_cast<ByteBufferAndInfo*>(png_get_io_ptr(png));
    if (!byteBufferAndInfo->memoryExhausted)
    {
        if (length <= byteBufferAndInfo->sizeLeft)
        {
            byteBufferAndInfo->pngOutputBuffer->Write(data, length);
            byteBufferAndInfo->sizeLeft -= length;
        }
        else
            byteBufferAndInfo->memoryExhausted = true;
    }
}


//Implementation----------------------------------------------------------------
PNGWriter::PNGWriter()
{
    this->reverseRGB = false;
    this->swapAlpha = false;
    this->isSRGB = false;
    this->channelCount = 4;
    this->bytesPerChannel = 1;
}

PNGWriter::~PNGWriter()
{
}

bool PNGWriter::GetReverseRGB() const
{
    return this->reverseRGB;
}

void PNGWriter::SetReverseRGB(bool value)
{
    this->reverseRGB = value;
}

bool PNGWriter::GetSRGB() const
{
    return this->isSRGB;
}

void PNGWriter::SetSRGB(bool value)
{
    this->isSRGB = value;
}

bool PNGWriter::GetSwapAlpha() const
{
    return this->swapAlpha;
}

void PNGWriter::SetSwapAlpha(bool value)
{
    this->swapAlpha = value;
}

uint32_t PNGWriter::GetChannelCount() const
{
    return this->channelCount;
}

void PNGWriter::SetChannelCount(uint32_t value)
{
    this->channelCount = value;
}

uint32_t PNGWriter::GetBytesPerChannel() const
{
    return this->bytesPerChannel;
}

void PNGWriter::SetBytesPerChannel(uint32_t value)
{
    this->bytesPerChannel = value;
}

PNGWriter::WriteResult PNGWriter::Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& pngOutputBuffer)
{
    int format = 0;
    switch (this->channelCount)
    {
        case 1: format = PNG_COLOR_TYPE_GRAY; break;
        case 2: format = PNG_COLOR_TYPE_GRAY_ALPHA; break;
        case 3: format = PNG_COLOR_TYPE_RGB; break;
        case 4: format = PNG_COLOR_TYPE_RGB_ALPHA; break;
        default: return INVALID_CHANNEL_COUNT;
    }
    
    if (this->bytesPerChannel != 1 && this->bytesPerChannel != 2)
        return INVALID_BYTES_PER_CHANNEL;
    
    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngWriteErrorHandler, PngWriteWarningHandler);
    if (png == nullptr)
        return WriteResult::FAILED_TO_START_WRITE;

    auto info = png_create_info_struct(png);
    if (info == nullptr)
    {
        png_destroy_write_struct(&png, nullptr);
        return WriteResult::FAILED_TO_START_WRITE;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_write_struct(&png, &info);
        return WriteResult::FAILED_TO_START_WRITE;
    }

    pngOutputBuffer.clear();

    //Create a temporary allocator that uses memory at the end of the specified output buffer
    auto rowPointersSize = sizeof(uint8_t*) * height;
    auto tempAllocatorSize = Allocator::AlignMemoryUp(rowPointersSize, Allocator::GetDefaultAlignment());
    if (pngOutputBuffer.max_size() < tempAllocatorSize)
    {
        png_destroy_write_struct(&png, &info);
        return WriteResult::NOT_ENOUGH_MEMORY;
    }

    ForwardAllocator<boost::null_mutex> tempAllocator;
    {
        ByteMemoryArena arena;

        //Just set 'raw' and 'aligned' blocks to point at the same memory.
        //It doesn't matter since the memory pointer won't be used later to free the memory

        arena.rawMemory.alignment = Allocator::GetDefaultAlignment();
        arena.rawMemory.ptr = Allocator::AlignMemoryUp(pngOutputBuffer.max_end() - tempAllocatorSize, arena.rawMemory.alignment);
        arena.rawMemory.size = pngOutputBuffer.max_end() - arena.rawMemory.ptr;

        arena.alignedMemory.alignment = arena.rawMemory.alignment;
        arena.alignedMemory.ptr = arena.rawMemory.ptr;
        arena.alignedMemory.size = arena.rawMemory.size;

        tempAllocatorSize = arena.alignedMemory.size; //Adjust size to what is effectively available

        tempAllocator.Create(std::move(arena));
    }

    if (rowStride == 0)
        rowStride = width * this->channelCount * this->bytesPerChannel;

    auto rows = reinterpret_cast<uint8_t**>(tempAllocator.Allocate(rowPointersSize, FINJIN_CALLER_ARGUMENTS));
    {
        auto row = static_cast<const uint8_t*>(pixels);

        for (size_t y = 0; y < height; y++)
        {
            rows[y] = const_cast<uint8_t*>(row);
            row += rowStride;
        }
    }
    
    png_set_IHDR
        (
        png,
        info,
        width,
        height,
        this->bytesPerChannel * 8,
        format,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
        );
    
    if (this->isSRGB)
    {
        png_set_gAMA(png, info, 1.0);
        png_set_sRGB(png, info, PNG_sRGB_INTENT_SATURATION);
    }

    png_set_rows(png, info, rows);

    ByteBufferAndInfo byteBufferAndInfo(pngOutputBuffer, pngOutputBuffer.max_size() - tempAllocatorSize);
    png_set_write_fn(png, &byteBufferAndInfo, PngWriteCallback, nullptr);

    int transform = PNG_TRANSFORM_IDENTITY;
    if (this->reverseRGB)
        transform |= PNG_TRANSFORM_BGR;
    if (this->swapAlpha)
        transform |= PNG_TRANSFORM_SWAP_ALPHA;
    png_write_png(png, info, transform, nullptr);

    png_destroy_write_struct(&png, &info);

    if (byteBufferAndInfo.memoryExhausted)
        return WriteResult::NOT_ENOUGH_MEMORY;
    
    return WriteResult::SUCCESS;
}

Utf8String PNGWriter::GetWriteResultString(WriteResult result) const
{
    switch (result)
    {
        case WriteResult::FAILED_TO_START_WRITE: return "Failed to start writing PNG.";
        case WriteResult::INVALID_CHANNEL_COUNT: return Utf8StringFormatter::Format("Invalid channel count '%1%'.", this->channelCount);
        case WriteResult::INVALID_BYTES_PER_CHANNEL: return Utf8StringFormatter::Format("Invalid bytes per channel '%1%'.", this->bytesPerChannel);
        case WriteResult::NOT_ENOUGH_MEMORY: return "Not enough memory to write PNG.";
        default: return Utf8String::GetEmpty();
    }
}

void PNGWriter::Write(const void* pixels, uint32_t width, uint32_t height, uint32_t rowStride, ByteBuffer& pngOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = Write(pixels, width, height, rowStride, pngOutputBuffer);
    if (result != WriteResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetWriteResultString(result));
}
