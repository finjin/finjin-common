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
#include "finjin/common/DataChunkWriter.hpp"
#include "finjin/common/DocumentWriterOutput.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//DataChunkWriter::Settings
DataChunkWriter::Settings::Settings()
{
    this->byteOrder = ::GetByteOrder();
    this->maxBytesPerLine = DEFAULT_MAX_BYTES_PER_LINE;
    this->blobTextFormat = DataChunkBlobTextFormat::BYTE_ARRAY;
    this->output = nullptr;
    this->controller = nullptr;
}

void DataChunkWriter::Settings::Create(std::shared_ptr<DocumentWriterOutput> outPtr, DataChunkWriterController& controller)
{
    this->outputPtr = outPtr;
    this->output = outPtr.get();
    this->controller = &controller;
}

void DataChunkWriter::Settings::Create(DocumentWriterOutput& out, DataChunkWriterController& controller)
{
    this->outputPtr = nullptr;
    this->output = &out;
    this->controller = &controller;
}

void DataChunkWriter::Settings::Create(std::shared_ptr<DocumentWriterOutput> outPtr, DocumentWriterOutput& out, DataChunkWriterController& controller)
{
    this->outputPtr = outPtr;
    this->output = &out;
    this->controller = &controller;
}

//DataChunkWriter
DataChunkWriter::~DataChunkWriter()
{
}

void DataChunkWriter::CreateRoot(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    Create(settings, DataChunkWriterStyle::ROOT, nullptr, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedStrings(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedTimeDurations(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedBools(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedCounts(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedInt8s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedInt16s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedInt32s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedInt64s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedUInt8s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedUInt16s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedUInt32s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedUInt64s(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedFloats(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void DataChunkWriter::WriteDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    WriteStridedDoubles(propertyName, values, count, DataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

const Utf8String& DataChunkWriter::GetContextString(ContextIndex index) const
{
    return this->contextStrings[index];
}

void DataChunkWriter::SetContextString(ContextIndex index, const Utf8String& value)
{
    if (this->contextStringProcessors[index] != nullptr)
    {
        auto tempValue = value;
        this->contextStringProcessors[index](tempValue);
        this->contextStrings[index] = tempValue;
    }
    else
        this->contextStrings[index] = value;

    if (index == ContextIndex::OBJECT_NAME)
    {
        for (size_t i = (size_t)ContextIndex::OBJECT_NAME + 1; i < (size_t)ContextIndex::COUNT; i++)
            this->contextStrings[i].clear();
    }
}

void DataChunkWriter::SetContextStringProcessor(ContextIndex index, std::function<void(Utf8String&)> value)
{
    this->contextStringProcessors[index] = value;
}

void DataChunkWriter::InheritContextStrings(DataChunkWriter& other)
{
    this->contextStrings = other.contextStrings;
    this->contextStringProcessors = other.contextStringProcessors;
}
