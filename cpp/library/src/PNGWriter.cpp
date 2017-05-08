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
}

PNGWriter::~PNGWriter()
{
}

PNGWriter::WriteResult PNGWriter::WriteRGBA8888(const void* pixels, uint32_t width, uint32_t height, ByteBuffer& pngOutputBuffer)
{
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
    auto tempAllocatorSize = rowPointersSize;
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

    uint32_t componentCount = 4;
    uint32_t bitsPerComponent = 8;
    auto bytesPerPixel = componentCount * bitsPerComponent / 8;

    auto rows = reinterpret_cast<uint8_t**>(tempAllocator.Allocate(rowPointersSize, FINJIN_CALLER_ARGUMENTS));
    {
        auto rgbaPixels = static_cast<const uint8_t*>(pixels);

        size_t rowOffset = 0;
        for (size_t y = 0; y < height; ++y)
        {
            rows[y] = const_cast<uint8_t*>(&rgbaPixels[rowOffset]);
            rowOffset += width * bytesPerPixel;
        }
    }

    png_set_IHDR
        (
        png,
        info,
        width,
        height,
        bitsPerComponent,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
        );

    png_set_rows(png, info, rows);

    ByteBufferAndInfo byteBufferAndInfo(pngOutputBuffer, pngOutputBuffer.max_size() - tempAllocatorSize);
    png_set_write_fn(png, &byteBufferAndInfo, PngWriteCallback, nullptr);

    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);

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
        case WriteResult::NOT_ENOUGH_MEMORY: return "Not enough memory to write PNG.";
        default: return Utf8String::Empty();
    }
}

void PNGWriter::WriteRGBA8888(const void* pixels, uint32_t width, uint32_t height, ByteBuffer& pngOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = WriteRGBA8888(pixels, width, height, pngOutputBuffer);
    if (result != WriteResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetWriteResultString(result));
}
