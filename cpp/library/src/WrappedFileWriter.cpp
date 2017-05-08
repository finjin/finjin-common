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
#include "finjin/common/WrappedFileWriter.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "FileAccessor.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
void WrappedFileWriter::Wrap(const Path& inFilePath, ByteOrder byteOrder, const Path& outFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Get input file extension
    Utf8String inFileExtension;
    inFilePath.GetExtension(inFileExtension, false);
    if (inFileExtension.empty())
    {
        FINJIN_SET_ERROR(error, "Input file must have an extension.");
        return;
    }

    //Get input file length
    uint64_t inFileLength = inFilePath.GetFileLength();
    if (inFileLength == 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read input file '%1%' or it was zero length.", inFilePath));
        return;
    }

    auto swapBytes = byteOrder != ::GetByteOrder();

    //Write output file----------------------
    FileAccessor outFile;
    if (!outFile.OpenForWrite(outFilePath))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open output file '%1%'.", outFilePath));
        return;
    }

    //uint32_t: Finjin signature
    uint32_t signature = FINJIN_SIGNATURE_FOURCC;
    if (swapBytes)
        SwapBytes(signature);
    outFile.Write(&signature, sizeof(signature));

    //uint32_t: Format
    auto fileFormat = WrappedFileReader::Header::FileFormat::EMBEDDED;
    if (swapBytes)
        SwapBytes(fileFormat);
    outFile.Write(&fileFormat, sizeof(fileFormat));

    //uint32_t: Format version
    uint32_t fileFormatVersion = 1;
    if (swapBytes)
        SwapBytes(fileFormatVersion);
    outFile.Write(&fileFormatVersion, sizeof(fileFormatVersion));

    //uint32_t: File format class
    auto fileFormatClass = WrappedFileReader::Header::GetFileFormatClass(inFileExtension);
    if (swapBytes)
        SwapBytes(fileFormatClass);
    outFile.Write(&fileFormatClass, sizeof(fileFormatClass));

    //uint32_t: File format class version
    uint32_t fileFormatClassVersion = 1;
    if (swapBytes)
        SwapBytes(fileFormatClassVersion);
    outFile.Write(&fileFormatClassVersion, sizeof(fileFormatClassVersion));

    //uint32_t: File extension length
    uint32_t fileExtensionLength = static_cast<uint32_t>(inFileExtension.length());
    if (swapBytes)
        SwapBytes(fileExtensionLength);
    outFile.Write(&fileExtensionLength, sizeof(fileExtensionLength));

    //UTF-8[File extension length]: File extension (without leading dot)
    outFile.Write(inFileExtension.c_str(), inFileExtension.length());

    //uint64_t: Embedded file length
    if (swapBytes)
        SwapBytes(inFileLength);
    outFile.Write(&inFileLength, sizeof(inFileLength));

    //Write input file----------------
    FileAccessor inFile;
    if (!inFile.OpenForRead(inFilePath))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open input file '%1%'.", inFilePath));
        return;
    }
    outFile.WriteRemaining(inFile);
}
