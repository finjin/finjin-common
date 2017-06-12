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
#include "finjin/common/ConfigDataChunkWriter.hpp"
#include "finjin/common/Base64.hpp"
#include "finjin/common/Convert.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define WRITE_VALUE_LINE(_this, propertyName, value) \
    WriteValueLine(_this->documentWriter, *_this->settings.output, _this->lineBuffer, propertyName, value, error); \
    if (error) \
    { \
        FINJIN_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) \
    WriteValuesLine(_this->documentWriter, *_this->settings.output, _this->lineBuffer, propertyName, values, count, valueStride, error); \
    if (error) \
    { \
        FINJIN_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_CHUNK_START_LINE(_this, name) \
    if (name.length() > ChunkName::MAX_LENGTH) \
    { \
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Chunk name length '%1%' for '%2%' exceeds maximum '%3%'.", name.length(), name.c_str(), ChunkName::MAX_LENGTH)); \
        return; \
    } \
    _this->documentWriter.WriteScopeStart(name)

#define WRITE_CHUNK_END_LINE(_this) _this->documentWriter.WriteScopeEnd()


//Local functions---------------------------------------------------------------
static bool ContainsNewline(const char* s)
{
    auto end = s + strlen(s);
    return std::find(s, end, '\n') != end;
}

static const char* BoolToString(bool value)
{
    auto t = "t";
    auto f = "f";
    return value ? t : f;
}

//WritePropertyName
static ByteBuffer& WritePropertyName(ByteBuffer& out, const ChunkPropertyName& propertyName)
{
    if (propertyName.index != (ChunkPropertyName::Index)-1)
        out.Write(Convert::ToString(propertyName.index));
    else
        out.Write(propertyName.name);

    return out;
}

//WriteTextValue
static void WriteTextValue(ByteBuffer& out, bool value, DocumentWriterOutput* writerOutput)
{
    out.Write("=");
    out.Write(BoolToString(value));
}

static void WriteTextValue(ByteBuffer& out, TextDataChunkBlobWrapper<const uint8_t*>& value, DocumentWriterOutput* writerOutput)
{
    out.Write("=");
    out.WriteBase64(value.bytes, value.count);
}

static void WriteTextValue(ByteBuffer& out, const char* value, DocumentWriterOutput* writerOutput)
{
    auto containsNewline = ContainsNewline(value);

    if (containsNewline)
        out.Write("^=");
    else
        out.Write("=");

    auto length = strlen(value);
    out.Write(value, length);

    if (containsNewline)
        out.Write("\n^");
}

static void WriteTextValue(ByteBuffer& out, const Utf8String& value, DocumentWriterOutput* writerOutput)
{
    auto containsNewline = ContainsNewline(value.c_str());

    if (containsNewline)
        out.Write("^=");
    else
        out.Write("=");

    auto length = value.length();
    out.Write(value.c_str(), length);

    if (containsNewline)
        out.Write("\n^");
}

static void WriteTextValue(ByteBuffer& out, const TimeDuration& value, DocumentWriterOutput* writerOutput)
{
    out.Write("=");

    out.Write(value.ToString());
}

template <typename T>
void WriteTextValue(ByteBuffer& out, const T& value, DocumentWriterOutput* writerOutput)
{
    out.Write("=");
    out.Write(Convert::ToString(value));
}

static void WriteTextValue(ByteBuffer& out, float value, DocumentWriterOutput* writerOutput)
{
    out.Write("=");

    if (GetAbs(value) < writerOutput->GetMinFloat())
        value = 0;
    out.Write(Convert::ToString(value));
}

static void WriteTextValue(ByteBuffer& out, double value, DocumentWriterOutput* writerOutput)
{
    out.Write("=");

    if (GetAbs(value) < writerOutput->GetMinDouble())
        value = 0;
    out.Write(Convert::ToString(value));
}

//WriteTextValues
static void WriteTextValues(ByteBuffer& out, const bool* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        if (writtenCount++ > 0)
            out.Write(" ");

        auto& value = GetStridedValue(values, offset + startIndex, valueStride);
        out.Write(BoolToString(value));
    }
}

static void WriteTextValues(ByteBuffer& out, const Utf8String* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        if (writtenCount++ > 0)
            out.Write(" ");

        auto& value = GetStridedValue(values, offset + startIndex, valueStride);
        out.Write(value);
    }
}

static void WriteTextValues(ByteBuffer& out, const TimeDuration* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        if (writtenCount++ > 0)
            out.Write(" ");

        auto value = GetStridedValue(values, offset + startIndex, valueStride).ToString();
        out.Write(value);
    }
}

template <typename T>
void WriteTextValues(ByteBuffer& out, const T* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        if (writtenCount++ > 0)
            out.Write(" ");

        auto& value = GetStridedValue(values, offset + startIndex, valueStride);
        out.Write(Convert::ToString(value));
    }
}

static void WriteTextValues(ByteBuffer& out, const float* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, offset + startIndex, valueStride);
        if (GetAbs(value) < writerOutput->GetMinFloat())
            value = 0;

        if (writtenCount++ > 0)
            out.Write(" ");

        out.Write(Convert::ToString(value));
    }
}

static void WriteTextValues(ByteBuffer& out, const double* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, offset + startIndex, valueStride);
        if (GetAbs(value) < writerOutput->GetMinDouble())
            value = 0;

        if (writtenCount++ > 0)
            out.Write(" ");

        out.Write(Convert::ToString(value));
    }
}

static void WriteTextValues(ByteBuffer& out, const uint8_t* values, size_t offset, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput)
{
    out.EnsureCanAppend(count * 4); //Worst case scenario where each value is 3 digits + space

    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        if (writtenCount++ > 0)
            out.Write(' ');

        auto value = GetStridedValue(values, offset + startIndex, valueStride);
        if (value > 99)
        {
            //3 digits
            auto digit = value / 100;
            value %= 100;
            out.Write('0' + digit);

            digit = value / 10;
            value %= 10;
            out.Write('0' + digit);

            out.Write('0' + value);
        }
        else if (value > 9)
        {
            //2 digits
            auto digit = value / 10;
            value %= 10;
            out.Write('0' + digit);

            out.Write('0' + value);
        }
        else
        {
            //1 digit
            out.Write('0' + value);
        }
    }
}

//WriteValueLine
template <typename Property, typename T>
void WriteValueLine(ConfigDocumentWriter& documentWriter, DocumentWriterOutput& out, ByteBuffer& lineBuffer, const Property& propertyName, T& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    lineBuffer.clear();

    //Write propertyName to line buffer
    WritePropertyName(lineBuffer, propertyName);

    //Write value to line buffer
    WriteTextValue(lineBuffer, value, &out);

    documentWriter.WriteLine(reinterpret_cast<char*>(lineBuffer.data()), lineBuffer.size());
}

template <typename Property, typename T>
void WriteValuesLine(ConfigDocumentWriter& documentWriter, DocumentWriterOutput& out, ByteBuffer& lineBuffer, const Property& propertyName, const T* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    lineBuffer.clear();

    //Write propertyName to line buffer
    WritePropertyName(lineBuffer, propertyName);
    lineBuffer.Write("=");

    //Write values to line buffer
    WriteTextValues(lineBuffer, values, 0, count, valueStride, &out);

    documentWriter.WriteLine(reinterpret_cast<char*>(lineBuffer.data()), lineBuffer.size());
}


//Implementation----------------------------------------------------------------
ConfigDataChunkWriter::ConfigDataChunkWriter()
{
}

ConfigDataChunkWriter::~ConfigDataChunkWriter()
{
    if (AnySet(this->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(this->style & DataChunkWriterStyle::ROOT))
    {
        FINJIN_DECLARE_ERROR(error); //Any error that occurs never leaves this method

        if (this->settings.customFooter != nullptr)
        {
            this->settings.customFooter(*this, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write custom footer.");
                return;
            }
        }
    }
}

void ConfigDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, DataChunkWriter* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    this->style = style;
    this->documentWriter.Create(*this->settings.output);
    this->lineBuffer.Create(this->settings.maxBytesPerLine, FINJIN_ALLOCATOR_NULL); //The maximum is allowed to be exceeded
}

DataChunkWriterController& ConfigDataChunkWriter::GetWriterController()
{
    return *this->settings.controller;
}

DocumentWriterOutput* ConfigDataChunkWriter::GetWriterOutput()
{
    return this->settings.output;
}

void ConfigDataChunkWriter::WriteWriterHeader(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WriteString(StandardChunkPropertyNames::BLOB_TEXT_FORMAT, DataChunkBlobTextFormatUtilities::ToString(this->settings.blobTextFormat), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write blob text format.");
        return;
    }

    if (this->settings.customHeader != nullptr)
    {
        this->settings.customHeader(*this, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write custom header.");
            return;
        }
    }
}

void ConfigDataChunkWriter::WriteChunk(const ChunkName& name, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.controller->RequiresNewOutput(*this, name))
    {
        //Create new chunk output
        std::shared_ptr<DocumentWriterOutput> sharedNewOutput = this->settings.controller->AddOutput(*this, name, error);
        if (error || sharedNewOutput == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create new output for chunk '%1%'.", name.name));
            return;
        }

        //Create new writer
        auto textChunkWriter = new ConfigDataChunkWriter();
        auto newSettings = this->settings;
        newSettings.Create(sharedNewOutput, *this->settings.controller);
        textChunkWriter->Create(newSettings, DataChunkWriterStyle::ALL, nullptr, error);
        if (error)
        {
            delete textChunkWriter;

            FINJIN_SET_ERROR(error, "Failed to create new writer.");
            return;
        }

        std::shared_ptr<DataChunkWriter> chunkWriter(textChunkWriter);
        chunkWriter->InheritContextStrings(*this);

        //Write chunk to new writer
        chunkWriter->WriteWriterHeader(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write writer header.");
            return;
        }

        auto nameString = name.ToString();
        WRITE_CHUNK_START_LINE(textChunkWriter, nameString);

        auto scheduled = this->settings.controller->ScheduleWriteChunk(chunkWriter, chunkFunc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to schedule/execute chunk writer.");
            return;
        }

        if (!scheduled)
        {
            //Write chunk to the new writer's output
            chunkFunc(*chunkWriter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write chunk.");
                return;
            }
        }
    }
    else
    {
        //Write chunk to this writer's output
        auto nameString = (name.index != (ChunkName::Index)-1) ? Convert::ToString(name.index) : name.ToString();
        WRITE_CHUNK_START_LINE(this, nameString);

        chunkFunc(*this, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write chunk.");
            return;
        }

        WRITE_CHUNK_END_LINE(this);
    }
}

void ConfigDataChunkWriter::WriteChunkStart(const ChunkName& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto nameString = (name.index != (ChunkName::Index)-1) ? Convert::ToString(name.index) : name.ToString();
    WRITE_CHUNK_START_LINE(this, nameString);
}

void ConfigDataChunkWriter::WriteChunkEnd(const ChunkName& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_CHUNK_END_LINE(this);
}

void ConfigDataChunkWriter::WriteFooter()
{
}

void ConfigDataChunkWriter::WriteBlob(const ChunkPropertyName& propertyName, const void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (this->settings.blobTextFormat)
    {
        case DataChunkBlobTextFormat::BYTE_ARRAY:
        {
            WriteUInt8s(propertyName, static_cast<const uint8_t*>(values), count, error);
            if (error)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error);
                return;
            }

            break;
        }
        case DataChunkBlobTextFormat::BASE64:
        {
            TextDataChunkBlobWrapper<const uint8_t*> blob(values, count);
            WRITE_VALUE_LINE(this, propertyName, blob);

            break;
        }
        default: break;
    }
}

void ConfigDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const char* value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteDateTime(const ChunkPropertyName& propertyName, const DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto s = value.ToIso8601();
    WRITE_VALUE_LINE(this, propertyName, s);
}

void ConfigDataChunkWriter::WriteTimeDuration(const ChunkPropertyName& propertyName, const TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteUuid(const ChunkPropertyName& propertyName, const Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto s = value.ToString();
    WRITE_VALUE_LINE(this, propertyName, s);
}

void ConfigDataChunkWriter::WriteBool(const ChunkPropertyName& propertyName, bool value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteCount(const ChunkPropertyName& propertyName, size_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteInt8(const ChunkPropertyName& propertyName, int8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteInt16(const ChunkPropertyName& propertyName, int16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteInt32(const ChunkPropertyName& propertyName, int32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteInt64(const ChunkPropertyName& propertyName, int64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteUInt8(const ChunkPropertyName& propertyName, uint8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteUInt16(const ChunkPropertyName& propertyName, uint16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteUInt32(const ChunkPropertyName& propertyName, uint32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteUInt64(const ChunkPropertyName& propertyName, uint64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteFloat(const ChunkPropertyName& propertyName, float value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteDouble(const ChunkPropertyName& propertyName, double value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void ConfigDataChunkWriter::WriteStridedStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void ConfigDataChunkWriter::WriteStridedDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
