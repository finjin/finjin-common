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
#include "finjin/common/PNGReader.hpp"
#include "finjin/common/PNGWriter.hpp"
#include <vector>


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(PNGTest_write_and_read)
{
    BOOST_TEST_MESSAGE("PNGTest_write_and_read:");

    FINJIN_DECLARE_ERROR(error);

    //Create the image to write
    uint32_t pixelColor = 0xff0000ff;
    uint32_t width = 100, height = 200;
    std::vector<uint32_t> writePixels;
    writePixels.resize(width * height);
    for (size_t i = 0; i < width * height; i++)
        writePixels[i] = pixelColor;

    //Write the PNG to memory
    ByteBuffer pngBuffer;
    pngBuffer.Create(MemorySize::MEBIBYTE * 10, FINJIN_ALLOCATOR_NULL);

    PNGWriter writer;
    writer.WriteRGBA8888(writePixels.data(), width, height, pngBuffer, error);
    BOOST_CHECK(!error);
    if (!error)
    {
        //Read the image from the PNG file in memory
        ByteBuffer readPixelsBytes;
        readPixelsBytes.Create(MemorySize::MEBIBYTE * 10, FINJIN_ALLOCATOR_NULL);

        PNGReader pngReader;
        ByteBufferReader pngBufferByteReader(pngBuffer);
        pngReader.ReadImage(pngBufferByteReader, readPixelsBytes, error);
        BOOST_CHECK(!error);
        if (!error)
        {
            auto readPixels = reinterpret_cast<uint32_t*>(readPixelsBytes.data());
            for (size_t i = 0; i < width * height; i++)
                BOOST_CHECK(readPixels[i] == pixelColor);
        }
    }
}
