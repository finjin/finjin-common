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
#include "finjin/common/StreamingFileFormat.hpp"
#include "finjin/common/FileAccessor.hpp"
#include "finjin/common/BinaryDataChunkWriter.hpp"
#include "finjin/common/ConfigDataChunkWriter.hpp"
#include "finjin/common/JsonDataChunkWriter.hpp"
#include "finjin/common/TextDataChunkWriter.hpp"

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
class FileAccessorDataChunkWriterOutput : public DocumentWriterOutput
{
public:
    FileAccessorDataChunkWriterOutput()
    {
        this->outStream = nullptr;
        Init();
    }

    FileAccessorDataChunkWriterOutput(std::shared_ptr<FileAccessor> inPtr) : outStreamPtr(inPtr)
    {
        this->outStream = this->outStreamPtr.get();
        Init();
    }

    FileAccessorDataChunkWriterOutput(std::shared_ptr<FileAccessor> inPtr, FileAccessor& in) : outStreamPtr(inPtr), outStream(&in)
    {
        Init();
    }

    FileAccessorDataChunkWriterOutput(FileAccessor& in) : outStream(&in)
    {
        Init();
    }

    ~FileAccessorDataChunkWriterOutput()
    {
    }

    bool IsValid() const override
    {
        return this->outStream != nullptr && this->outStream->IsOpen();
    }

    void Write(const char* s) override
    {
        if (s != nullptr)
            Write(s, strlen(s));
    }

    void Write(const void* s, size_t byteCount) override
    {
        if (this->outStream != nullptr)
        {
            this->outStream->Write(static_cast<const char*>(s), byteCount);
            this->totalBytesWritten += byteCount;
        }
    }

    uint64_t GetTotalBytesWritten() const override
    {
        return this->totalBytesWritten;
    }

private:
    void Init()
    {
        this->totalBytesWritten = 0;
    }

private:
    FileAccessor* outStream;
    std::shared_ptr<FileAccessor> outStreamPtr;
    uint64_t totalBytesWritten;
};


//Implementation----------------------------------------------------------------
bool StreamingFileFormatUtilities::IsValid(StreamingFileFormat format)
{
    switch (format)
    {
        case StreamingFileFormat::STREAMING_TEXT:
        case StreamingFileFormat::STREAMING_BINARY:
        case StreamingFileFormat::STREAMING_JSON:
        case StreamingFileFormat::STREAMING_CONFIG: return true;
        default: return false;
    }
}

const char* StreamingFileFormatUtilities::ToString(StreamingFileFormat format)
{
    switch (format)
    {
        case StreamingFileFormat::STREAMING_TEXT: return "fstd";
        case StreamingFileFormat::STREAMING_BINARY: return "fsbd";
        case StreamingFileFormat::STREAMING_JSON: return "json";
        case StreamingFileFormat::STREAMING_CONFIG: return "cfg";
        default: return FINJIN_ENUM_UNKNOWN_STRING;
    }
}

Utf8String StreamingFileFormatUtilities::MakeExtension(const Utf8String& assetClass, StreamingFileFormat format)
{
    Utf8String result = ToString(format);
    result += "-";
    result += assetClass;
    return result;
}

std::unique_ptr<DataChunkWriter> StreamingFileFormatUtilities::CreateFileWriter(const Utf8String& fileName, StreamingFileFormat format, const DataChunkWriter::Settings& settings, DataChunkWriterController& controller, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto exportFile = std::make_shared<FileAccessor>();
    if (!exportFile->OpenForWrite(fileName))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open file for '%1%'.", fileName));
        return nullptr;
    }

    std::unique_ptr<DataChunkWriter> dataChunkWriter;
    switch (format)
    {
        case StreamingFileFormat::STREAMING_TEXT:
        {
            dataChunkWriter.reset(new TextDataChunkWriter);
            break;
        }
        case StreamingFileFormat::STREAMING_BINARY:
        {
            dataChunkWriter.reset(new BinaryDataChunkWriter);
            break;
        }
        case StreamingFileFormat::STREAMING_JSON:
        {
            dataChunkWriter.reset(new JsonDataChunkWriter);
            break;
        }
        case StreamingFileFormat::STREAMING_CONFIG:
        {
            dataChunkWriter.reset(new ConfigDataChunkWriter);
            break;
        }
        default:
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid streaming file format '%1%'.", (int)format));
            return nullptr;
        }
    }

    auto writerSettings = settings;
    writerSettings.Create(std::shared_ptr<FileAccessorDataChunkWriterOutput>(new FileAccessorDataChunkWriterOutput(exportFile)), controller);

    dataChunkWriter->CreateRoot(writerSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create data chunk writer.");
        return nullptr;
    }

    return dataChunkWriter;
}
