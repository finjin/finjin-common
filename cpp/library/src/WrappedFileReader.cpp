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
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/ByteOrder.hpp"
#include "FileAccessor.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
template <typename Reader>
WrappedFileReader::ReadHeaderResult ReadHeader(Reader& inFile, WrappedFileReader::Header& header)
{
    auto startOffset = inFile.GetOffset();

    //uint32_t: Finjin signature
    if (inFile.Read(&header.signature, sizeof(header.signature)) < sizeof(header.signature))
    {
        inFile.SetOffset(startOffset);
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_SIGNATURE_VALUE;
    }

    auto swapBytes = false;
    if (header.signature != FINJIN_SIGNATURE_FOURCC)
    {
        SwapBytes(header.signature);
        if (header.signature == FINJIN_SIGNATURE_FOURCC)
            swapBytes = true;
        else
        {
            inFile.SetOffset(startOffset);
            return WrappedFileReader::ReadHeaderResult::INVALID_SIGNATURE_VALUE;
        }
    }

    //uint32_t: Format
    if (inFile.Read(&header.fileFormat, sizeof(header.fileFormat)) < sizeof(header.fileFormat))
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT;
    if (swapBytes)
        SwapBytes(header.fileFormat);
    if (!WrappedFileReader::Header::IsValidFormat(header.fileFormat))
        return WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT;

    //uint32_t: Format version
    if (inFile.Read(&header.fileFormatVersion, sizeof(header.fileFormatVersion)) < sizeof(header.fileFormatVersion))
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT_VERSION;
    if (swapBytes)
        SwapBytes(header.fileFormatVersion);
    if (header.fileFormatVersion == 0)
        return WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT_VERSION;

    //uint32_t: File format class
    if (inFile.Read(&header.fileFormatClass, sizeof(header.fileFormatClass)) < sizeof(header.fileFormatClass))
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT_CLASS;
    if (swapBytes)
        SwapBytes(header.fileFormatClass);
    if (!WrappedFileReader::Header::IsValidFormatClass(header.fileFormatClass))
        return WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT_CLASS;

    //uint32_t: File format class version
    if (inFile.Read(&header.fileFormatClassVersion, sizeof(header.fileFormatClassVersion)) < sizeof(header.fileFormatClassVersion))
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT_CLASS_VERSION;
    if (swapBytes)
        SwapBytes(header.fileFormatClassVersion);
    if (header.fileFormatClassVersion == 0)
        return WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT_CLASS_VERSION;

    //uint32_t: File extension length
    if (inFile.Read(&header.fileExtensionLength, sizeof(header.fileExtensionLength)) < sizeof(header.fileExtensionLength))
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_EXTENSION_LENGTH;
    if (swapBytes)
        SwapBytes(header.fileExtensionLength);
    if (header.fileExtensionLength == 0)
        return WrappedFileReader::ReadHeaderResult::INVALID_FILE_EXTENSION_LENGTH;

    //UTF-8[File extension length]: File extension (without leading dot)
    header.fileExtension.resize(header.fileExtensionLength);
    if (inFile.Read(&header.fileExtension[0], header.fileExtensionLength) < header.fileExtensionLength)
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_EXTENSION;

    //uint64_t: Embedded file length
    if (inFile.Read(&header.fileLength, sizeof(header.fileLength)) < sizeof(header.fileLength))
        return WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_LENGTH;
    if (swapBytes)
        SwapBytes(header.fileLength);

    return WrappedFileReader::ReadHeaderResult::SUCCESS;
}

static Utf8String GetReadHeaderResultString(const WrappedFileReader::Header& header, WrappedFileReader::ReadHeaderResult result)
{
    switch (result)
    {
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_SIGNATURE_VALUE: return "Failed to read signature.";
        case WrappedFileReader::ReadHeaderResult::INVALID_SIGNATURE_VALUE: return "Invalid signature.";
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT: return "Failed to read file format.";
        case WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT: return Utf8StringFormatter::Format("Invalid file format '%1%'", (uint32_t)header.fileFormat);
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT_VERSION: return "Failed to read file format version.";
        case WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT_VERSION: return Utf8StringFormatter::Format("Invalid file format version '%1%'", header.fileFormatVersion);
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT_CLASS: return "Failed to read file format class.";
        case WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT_CLASS: return Utf8StringFormatter::Format("Invalid file format class '%1%'", (uint32_t)header.fileFormatClass);
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_FORMAT_CLASS_VERSION: return "Failed to read file format class version.";
        case WrappedFileReader::ReadHeaderResult::INVALID_FILE_FORMAT_CLASS_VERSION: return Utf8StringFormatter::Format("Invalid file format class version '%1%'", header.fileFormatClassVersion);
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_EXTENSION_LENGTH: return "Failed to read file extension length.";
        case WrappedFileReader::ReadHeaderResult::INVALID_FILE_EXTENSION_LENGTH: return Utf8StringFormatter::Format("Invalid file extension length '%1%'", header.fileExtensionLength);
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_EXTENSION: return "Failed to read file extension.";
        case WrappedFileReader::ReadHeaderResult::FAILED_TO_READ_FILE_LENGTH: return "Failed to read file length.";
        default: return Utf8String::GetEmpty();
    }
}

template <typename Reader>
void ReadHeader(Reader& inFile, WrappedFileReader::Header& header, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadHeader(inFile, header);
    if (result != WrappedFileReader::ReadHeaderResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadHeaderResultString(header, result));
}


//Implementation----------------------------------------------------------------

//WrappedFileReader::Header
WrappedFileReader::Header::Header()
{
    this->signature = 0;
    this->fileFormat = FileFormat::EMBEDDED;
    this->fileFormatVersion = 0;
    this->fileFormatClass = FileFormatClass::GENERIC;
    this->fileFormatClassVersion = 0;
    this->fileExtensionLength = 0;
    this->fileLength = 0;
}

bool WrappedFileReader::Header::IsValidFormat(FileFormat format)
{
    return format == Header::FileFormat::EMBEDDED;
}

bool WrappedFileReader::Header::IsValidFormatClass(FileFormatClass formatClass)
{
    return
        formatClass == Header::FileFormatClass::GENERIC ||
        formatClass == Header::FileFormatClass::IMAGE ||
        formatClass == Header::FileFormatClass::SOUND
        ;
}

//WrappedFileReader
WrappedFileReader::WrappedFileReader()
{
    this->headerSize = 0;
}

WrappedFileReader::ReadHeaderResult WrappedFileReader::ReadHeader(ByteBufferReader& reader)
{
    this->headerSize = 0;

    auto result = ::ReadHeader(reader, this->header);
    if (result == ReadHeaderResult::SUCCESS)
        this->headerSize = reader.GetOffset();

    return result;
}

Utf8String WrappedFileReader::GetReadHeaderResultString(ReadHeaderResult result) const
{
    return ::GetReadHeaderResultString(this->header, result);
}

void WrappedFileReader::ReadHeader(ByteBufferReader& reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->headerSize = 0;

    ::ReadHeader(reader, this->header, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read header.");
        return;
    }

    this->headerSize = reader.GetOffset();
}

const WrappedFileReader::Header& WrappedFileReader::GetHeader() const
{
    return this->header;
}

size_t WrappedFileReader::GetHeaderSize() const
{
    return this->headerSize;
}

void WrappedFileReader::Unwrap(const Path& inFilePath, const Path& outFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Read input file---------------------------
    FileAccessor inFile;
    if (!inFile.OpenForRead(inFilePath))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open input file '%1%'.", inFilePath));
        return;
    }

    //Read header
    Header header;
    ::ReadHeader(inFile, header, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to reader header.");
        return;
    }

    //Validate file format
    if (header.fileFormat != Header::FileFormat::EMBEDDED)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected file format to be EMBEDDED, but it is '%1%'.", (uint32_t)header.fileFormat));
        return;
    }

    //The remainder of inFile is the embedded file

    //Write output file--------------------------
    auto outFilePathWithExtension = outFilePath;
    outFilePathWithExtension += ".";
    outFilePathWithExtension += header.fileExtension;
    FileAccessor outFile;
    if (!outFile.OpenForWrite(outFilePathWithExtension))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open output file '%1%'.", outFilePathWithExtension));
        return;
    }

    outFile.WriteRemaining(inFile);
}
