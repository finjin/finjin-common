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
#include "finjin/common/PNGReader.hpp"
#include "finjin/common/Allocator.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/ForwardAllocator.hpp"
#include "finjin/common/MemoryArena.hpp"
#include <png.h>
#include <pngpriv.h>
#include <boost/thread/null_mutex.hpp>

using namespace Finjin::Common;


//Static initialization---------------------------------------------------------
uint32_t PNGReader::PixelFormat::GRAY = PNG_FORMAT_GRAY;
uint32_t PNGReader::PixelFormat::GA = PNG_FORMAT_GA;
uint32_t PNGReader::PixelFormat::AG = PNG_FORMAT_AG;
uint32_t PNGReader::PixelFormat::RGB = PNG_FORMAT_RGB;
uint32_t PNGReader::PixelFormat::BGR = PNG_FORMAT_BGR;
uint32_t PNGReader::PixelFormat::RGBA = PNG_FORMAT_RGBA;
uint32_t PNGReader::PixelFormat::ARGB = PNG_FORMAT_ARGB;
uint32_t PNGReader::PixelFormat::BGRA = PNG_FORMAT_BGRA;
uint32_t PNGReader::PixelFormat::ABGR = PNG_FORMAT_ABGR;
uint32_t PNGReader::PixelFormat::LINEAR_RGB = PNG_FORMAT_LINEAR_RGB;
uint32_t PNGReader::PixelFormat::LINEAR_RGBA = PNG_FORMAT_LINEAR_RGB_ALPHA;


//Local functions---------------------------------------------------------------
static void PngReadHandler(png_structp png, png_bytep out, png_size_t need)
{
    auto byteReader = static_cast<ByteBufferReader*>(png->io_ptr);
    byteReader->Read(out, need);
}

static png_voidp PngMallocHandler(png_structp png, png_alloc_size_t need)
{
    auto allocator = static_cast<Allocator*>(png->mem_ptr);
    return allocator->Allocate(need, FINJIN_CALLER_ARGUMENTS);
}

static void PngFreeHandler(png_structp png, png_voidp mem)
{
    //Do nothing
}

static void PngReadErrorHandler(png_structp png, png_const_charp message)
{
    FINJIN_DEBUG_LOG_ERROR("PNG reader error: %1%", message);
}

static void PngReadWarningHandler(png_structp png, png_const_charp message)
{
    FINJIN_DEBUG_LOG_WARNING("PNG reader warning: %1%", message);
}


//Implementation----------------------------------------------------------------
PNGReader::PNGReader()
{
    this->width = 0;
    this->height = 0;
    this->channelCount = 0;
    this->bytesPerChannel = 0;
    this->format = 0;
    this->flags = 0;

    this->convert16bitTo8bit = false;
    this->addAlpha = false;
    this->swapAlpha = false;
}

PNGReader::~PNGReader()
{
}

bool PNGReader::GetConvert16bitTo8bit() const
{
    return this->convert16bitTo8bit;
}

void PNGReader::SetConvert16bitTo8bit(bool value)
{
    this->convert16bitTo8bit = value;
}

bool PNGReader::GetAddAlpha() const
{
    return this->addAlpha;
}

void PNGReader::SetAddAlpha(bool value)
{
    this->addAlpha = value;
}

bool PNGReader::GetSwapAlpha() const
{
    return this->swapAlpha;
}

void PNGReader::SetSwapAlpha(bool value)
{
    this->swapAlpha = value;
}

uint32_t PNGReader::GetPixelFormat() const
{
    return this->format;
}

uint32_t PNGReader::GetWidth() const
{
    return this->width;
}

uint32_t PNGReader::GetHeight() const
{
    return this->height;
}

bool PNGReader::IsSRGB() const
{
    if (this->flags & PNG_IMAGE_FLAG_COLORSPACE_NOT_sRGB)
        return false;
    else
        return true;
}

uint32_t PNGReader::GetChannelCount() const
{
    return this->channelCount;
}

uint32_t PNGReader::GetBytesPerChannel() const
{
    return this->bytesPerChannel;
}

uint32_t PNGReader::GetBytesPerPixel() const
{
    return this->channelCount * this->bytesPerChannel;
}

uint32_t PNGReader::GetBytesPerRow() const
{
    return this->channelCount * this->bytesPerChannel * this->width;
}

PNGReader::ReadResult PNGReader::ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer)
{
    if (reader.size_left() < 8)
        return ReadResult::FAILED_TO_READ_SIGNATURE;

    if (png_sig_cmp(reader.data_left(), 0, 8) != 0)
        return ReadResult::INVALID_SIGNATURE;

    imageOutputBuffer.clear();

    //Create a temporary allocator that uses memory at the end of the specified output buffer
    auto tempAllocatorSize = Allocator::AlignMemoryUp((size_t)125000, Allocator::GetDefaultAlignment());;
    if (imageOutputBuffer.max_size() < tempAllocatorSize)
        return ReadResult::NOT_ENOUGH_MEMORY_TO_START_IMAGE_READ;

    ForwardAllocator<boost::null_mutex> tempAllocator;
    {
        ByteMemoryArena arena;

        //Just set 'raw' and 'aligned' blocks to point at the same memory.
        //It doesn't matter since the memory pointer won't be used later to free the memory

        arena.rawMemory.alignment = Allocator::GetDefaultAlignment();
        arena.rawMemory.ptr = Allocator::AlignMemoryUp(imageOutputBuffer.max_end() - tempAllocatorSize, arena.rawMemory.alignment);
        arena.rawMemory.size = imageOutputBuffer.max_end() - arena.rawMemory.ptr;

        arena.alignedMemory.alignment = arena.rawMemory.alignment;
        arena.alignedMemory.ptr = arena.rawMemory.ptr;
        arena.alignedMemory.size = arena.rawMemory.size;

        tempAllocatorSize = arena.alignedMemory.size; //Adjust size to what is effectively available

        tempAllocator.Create(std::move(arena));
    }

    auto png = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, nullptr, PngReadErrorHandler, PngReadWarningHandler, &tempAllocator, PngMallocHandler, PngFreeHandler);
    if (png == nullptr)
        return ReadResult::FAILED_TO_START_IMAGE_READ;

    auto info = png_create_info_struct(png);
    if (info == nullptr)
    {
        png_destroy_read_struct(&png, nullptr, nullptr);
        return ReadResult::FAILED_TO_START_IMAGE_READ;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, nullptr);
        return ReadResult::FAILED_TO_START_IMAGE_READ;
    }

    png_set_read_fn(png, &reader, PngReadHandler);

    png_read_info(png, info);

    this->width = png_get_image_width(png, info);
    this->height = png_get_image_height(png, info);
    this->channelCount = png_get_channels(png, info);
    auto colorType = png_get_color_type(png, info);
    auto bitDepth = png_get_bit_depth(png, info);

    if (this->convert16bitTo8bit && bitDepth == 16)
    {
        png_set_strip_16(png);
        bitDepth = 8;
    }

    if (colorType == PNG_COLOR_TYPE_PALETTE)
    {
        //Convert paletted to RGB
        png_set_palette_to_rgb(png);
        colorType = PNG_COLOR_TYPE_RGB;
    }

    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
    {
        //Upgrade the bit depth to 8 bits
        png_set_expand_gray_1_2_4_to_8(png);
        bitDepth = 8;
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
        if ((colorType & PNG_COLOR_MASK_ALPHA) == 0)
        {
            colorType |= PNG_COLOR_MASK_ALPHA;
            this->channelCount++;
        }
    }

    if (colorType == PNG_COLOR_TYPE_RGB_ALPHA && this->swapAlpha)
    {
        //RGBA -> ARGB
        png_set_swap_alpha(png);
    }

    if (this->addAlpha && (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY))
    {
        //Add alpha to non-alpha formats
        if (this->swapAlpha)
            png_set_add_alpha(png, 0xff, PNG_FILLER_BEFORE);
        else
            png_set_add_alpha(png, 0xff, PNG_FILLER_AFTER);

        colorType |= PNG_COLOR_MASK_ALPHA;
        this->channelCount++;
    }

    this->bytesPerChannel = bitDepth / 8;

    png_read_update_info(png, info);

    //Allocate memory to decompress image
    auto bytesPerRow = this->width * this->channelCount * this->bytesPerChannel;
    assert(bytesPerRow == png_get_rowbytes(png, info));
    auto decompressedImageSize = bytesPerRow * this->height;
    auto alignedDecompressedImageSize = Allocator::AlignSizeUp(decompressedImageSize, Allocator::GetDefaultAlignment()); //Size aligned so that row pointers at end start on a proper boundary
    auto totalAllocationSize = alignedDecompressedImageSize + sizeof(png_bytep) * height; //Row pointers at end
    if (imageOutputBuffer.max_size() - tempAllocatorSize < totalAllocationSize)
    {
        png_destroy_read_struct(&png, &info, nullptr);
        return ReadResult::NOT_ENOUGH_MEMORY_FOR_IMAGE;
    }

    imageOutputBuffer.resize(totalAllocationSize);

    auto rowPointers = (png_bytep*)(&imageOutputBuffer[alignedDecompressedImageSize]);
    for (uint32_t rowIndex = 0; rowIndex < this->height; rowIndex++)
        rowPointers[rowIndex] = (png_byte*)(imageOutputBuffer.data() + bytesPerRow * rowIndex);

    //Decompress image
    png_read_image(png, rowPointers);

    //Truncate to remove the pointers at the end
    imageOutputBuffer.resize(decompressedImageSize);

    //Determine format
    this->format = 0;
    if (colorType & PNG_COLOR_MASK_COLOR)
        this->format |= PNG_FORMAT_FLAG_COLOR;
    if ((colorType & PNG_COLOR_MASK_ALPHA) || png->num_trans > 0)
        this->format |= PNG_FORMAT_FLAG_ALPHA;
    if (this->swapAlpha && (this->format & PNG_FORMAT_FLAG_ALPHA))
        this->format |= PNG_FORMAT_FLAG_AFIRST;
    if (bitDepth == 16)
        this->format |= PNG_FORMAT_FLAG_LINEAR;

    //Determine flags
    this->flags = 0;
#ifdef PNG_COLORSPACE_SUPPORTED
    //From pngread.c, line 1410
    if ((this->format & PNG_FORMAT_FLAG_COLOR) && ((png->colorspace.flags & (PNG_COLORSPACE_HAVE_ENDPOINTS | PNG_COLORSPACE_ENDPOINTS_MATCH_sRGB | PNG_COLORSPACE_INVALID)) == PNG_COLORSPACE_HAVE_ENDPOINTS))
        this->flags |= PNG_IMAGE_FLAG_COLORSPACE_NOT_sRGB;
#endif

    //Cleanup
    png_destroy_read_struct(&png, &info, nullptr);

    return ReadResult::SUCCESS;
}

Utf8String PNGReader::GetReadResultString(ReadResult result) const
{
    switch (result)
    {
        case ReadResult::FAILED_TO_READ_SIGNATURE: return "Failed to read signature";
        case ReadResult::INVALID_SIGNATURE: return "Invalid signature";
        case ReadResult::NOT_ENOUGH_MEMORY_TO_START_IMAGE_READ: return "Not enough memory to start reading image.";
        case ReadResult::FAILED_TO_START_IMAGE_READ: return "Failed to start reading image.";
        case ReadResult::NOT_ENOUGH_MEMORY_FOR_IMAGE: return "Not enough memory to read image.";
        case ReadResult::FAILED_TO_FINISH_IMAGE_READ: return "Failed to finish reading image.";
        default: return Utf8String::GetEmpty();
    }
}

void PNGReader::ReadImage(ByteBufferReader& reader, ByteBuffer& imageOutputBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadImage(reader, imageOutputBuffer);
    if (result != ReadResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadResultString(result));
}
