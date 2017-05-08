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
#include "WxDataChunkWriter.hpp"
#include "finjin/common/WxDocumentWriterOutput.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//WxDataChunkWriter::Settings
WxDataChunkWriter::Settings::Settings()
{
    this->byteOrder = ::GetByteOrder();
    this->maxBytesPerLine = DEFAULT_MAX_BYTES_PER_LINE;
    this->blobTextFormat = WxDataChunkBlobTextFormat::BYTE_ARRAY;
    this->output = nullptr;
    this->controller = nullptr;
}

void WxDataChunkWriter::Settings::Create(std::shared_ptr<WxDocumentWriterOutput> outPtr, WxDataChunkWriterController& controller)
{
    this->outputPtr = outPtr;
    this->output = outPtr.get();
    this->controller = &controller;
}

void WxDataChunkWriter::Settings::Create(WxDocumentWriterOutput& out, WxDataChunkWriterController& controller)
{
    this->outputPtr = nullptr;
    this->output = &out;
    this->controller = &controller;
}

void WxDataChunkWriter::Settings::Create(std::shared_ptr<WxDocumentWriterOutput> outPtr, WxDocumentWriterOutput& out, WxDataChunkWriterController& controller)
{
    this->outputPtr = outPtr;
    this->output = &out;
    this->controller = &controller;
}

//WxDataChunkWriter
WxDataChunkWriter::~WxDataChunkWriter()
{
}

void WxDataChunkWriter::CreateRoot(const Settings& settings, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    Create(settings, DataChunkWriterStyle::ROOT, nullptr, error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedStrings(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedTimeDurations(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedBools(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedCounts(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedInt8s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedInt16s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedInt32s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedInt64s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedUInt8s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedUInt16s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedUInt32s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedUInt64s(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedFloats(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

void WxDataChunkWriter::WriteDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    WriteStridedDoubles(propertyName, values, count, WxDataChunkWriteStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
}

const wxString& WxDataChunkWriter::GetContextString(ContextIndex index) const
{
    return this->contextStrings[index];
}

void WxDataChunkWriter::SetContextString(ContextIndex index, const wxString& value)
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

void WxDataChunkWriter::SetContextStringProcessor(ContextIndex index, std::function<void(wxString&)> value)
{
    this->contextStringProcessors[index] = value;
}

void WxDataChunkWriter::InheritContextStrings(WxDataChunkWriter& other)
{
    this->contextStrings = other.contextStrings;
    this->contextStringProcessors = other.contextStringProcessors;
}
