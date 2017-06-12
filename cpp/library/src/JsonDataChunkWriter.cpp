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
#include "finjin/common/JsonDataChunkWriter.hpp"
#include "finjin/common/Base64.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/JsonDocumentImpl.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define WRITE_VALUE_QUOTED_LINE(_this, propertyName, stringValue) WriteQuotedLine(*_this->impl->settings.output, _this->impl->escaper, propertyName, stringValue)

#define WRITE_VALUE_LINE(_this, propertyName, value) WriteValueLine(*_this->impl->settings.output, _this->impl->escaper, propertyName, value)

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) WriteValuesLine(*_this->impl->settings.output, _this->impl->escaper, propertyName, values, count, valueStride)

#define WRITE_CHUNK_START_LINE_STRING(_this, name) WriteChunkStartLineWithString(*_this->impl->settings.output, _this->impl->escaper, name)
#define WRITE_CHUNK_START_LINE_INDEX_OR_STRING(_this, name) WriteChunkStartLineWithIndexOrString(*_this->impl->settings.output, _this->impl->escaper, name)

#define WRITE_CHUNK_END_LINE(_this) *_this->impl->settings.output << "},\n";


//Local functions---------------------------------------------------------------
inline DocumentWriterOutput& operator << (DocumentWriterOutput& out, const char* value)
{
    out.Write(value);
    return out;
}

inline DocumentWriterOutput& operator << (DocumentWriterOutput& out, const Utf8String& value)
{
    out.Write(value.c_str());
    return out;
}

class JsonStringEscaper : public JsonStringWriter
{
public:
    JsonStringEscaper() : JsonStringWriter(buffer)
    {
    }

    const JsonStringBuffer::Ch* Escape(const Utf8String& s)
    {
        this->buffer.Clear();
        WriteString(s.c_str(), static_cast<rapidjson::SizeType>(s.length()));
        return this->buffer.GetString();
    }

    const JsonStringBuffer::Ch* Escape(const char* s)
    {
        this->buffer.Clear();
        WriteString(s, static_cast<rapidjson::SizeType>(strlen(s)));
        return this->buffer.GetString();
    }

private:
    JsonStringBuffer buffer;
};

static Utf8String RewriteKeyToJSONStyle(const Utf8String& propertyName)
{
    Utf8String result;

    char previousChar = 0;
    for (auto c : propertyName)
    {
        if (previousChar == '-' && isalpha(c))
            result += toupper(c);
        else if (c != '-')
            result += c;

        previousChar = c;
    }

    return result;
}

static DocumentWriterOutput& WritePropertyName(DocumentWriterOutput& out, JsonStringEscaper& escaper, const Utf8String& propertyName, bool rewriteKey = true)
{
    if (!propertyName.empty())
    {
        if (rewriteKey)
            out << escaper.Escape(RewriteKeyToJSONStyle(propertyName)) << ":";
        else
            out << escaper.Escape(propertyName) << ":";
    }
    else
        out << "\"\":";
    return out;
}

static DocumentWriterOutput& WritePropertyName(DocumentWriterOutput& out, JsonStringEscaper& escaper, const ChunkPropertyName& propertyName, bool rewriteKey = true)
{
    if (propertyName.IsValid())
    {
        if (propertyName.index != (ChunkPropertyName::Index)-1)
            out << Convert::ToString(propertyName.index) << ":";
        else
        {
            if (rewriteKey)
                out << escaper.Escape(RewriteKeyToJSONStyle(propertyName.name)) << ":";
            else
                out << escaper.Escape(propertyName.name) << ":";
        }
    }
    else
        out << "\"\":";
    return out;
}

static const char* BoolToString(bool value)
{
    auto t = "true";
    auto f = "false";
    return value ? t : f;
}

static void WriteJsonValue(DocumentWriterOutput& out, JsonStringEscaper& escaper, bool value)
{
    out << BoolToString(value);
}

template <typename T>
void WriteJsonValue(DocumentWriterOutput& out, JsonStringEscaper& escaper, const T& value)
{
    out << Convert::ToString(value);
}

template <typename T>
void WriteJsonValues(DocumentWriterOutput& out, JsonStringEscaper& escaper, const T* values, size_t count, DataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto& value = GetStridedValue(values, i, valueStride);
        out << Convert::ToString(value);
    }
    out << "]";
}

static void WriteJsonValues(DocumentWriterOutput& out, JsonStringEscaper& escaper, const bool* values, size_t count, DataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto& value = GetStridedValue(values, i, valueStride);
        out << BoolToString(value);
    }
    out << "]";
}

static void WriteJsonValues(DocumentWriterOutput& out, JsonStringEscaper& escaper, const Utf8String* values, size_t count, DataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto& value = GetStridedValue(values, i, valueStride);
        out << escaper.Escape(value);
    }
    out << "]";
}

static void WriteJsonValues(DocumentWriterOutput& out, JsonStringEscaper& escaper, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto value = GetStridedValue(values, i, valueStride).ToString();
        out << escaper.Escape(value);
    }
    out << "]";
}

static void WriteJsonValues(DocumentWriterOutput& out, JsonStringEscaper& escaper, const float* values, size_t count, DataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto value = GetStridedValue(values, i, valueStride);
        if (GetAbs(value) < out.GetMinFloat())
            value = 0;
        out << Convert::ToString(value);
    }
    out << "]";
}

static void WriteJsonValues(DocumentWriterOutput& out, JsonStringEscaper& escaper, const double* values, size_t count, DataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto value = GetStridedValue(values, i, valueStride);
        if (GetAbs(value) < out.GetMinDouble())
            value = 0;
        out << Convert::ToString(value);
    }
    out << "]";
}

template <typename Property, typename T>
void WriteQuotedLine(DocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const T& stringValue)
{
    WritePropertyName(out, escaper, propertyName);
    out << escaper.Escape(stringValue);
    out << ",\n";
}

template <typename Property>
void WriteQuotedLine(DocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const TimeDuration& stringValue)
{
    WritePropertyName(out, escaper, propertyName);
    out << escaper.Escape(stringValue.ToString());
    out << ",\n";
}

template <typename Property, typename T>
void WriteValueLine(DocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const T& value)
{
    WritePropertyName(out, escaper, propertyName);
    WriteJsonValue(out, escaper, value);
    out << ",\n";
}

template <typename Property, typename T>
void WriteValuesLine(DocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const T* values, size_t count, DataChunkWriteStride valueStride)
{
    WritePropertyName(out, escaper, propertyName);
    WriteJsonValues(out, escaper, values, count, valueStride);
    out << ",\n";
}

static void WriteChunkStartLineWithString(DocumentWriterOutput& out, JsonStringEscaper& escaper, const ChunkName& name)
{
    Utf8String stringName = RewriteKeyToJSONStyle(name.name);
    stringName += "|";
    stringName += name.id.ToString();

    WritePropertyName(out, escaper, stringName, false) << "{\n";
}

static void WriteChunkStartLineWithIndexOrString(DocumentWriterOutput& out, JsonStringEscaper& escaper, const ChunkName& name)
{
    if (name.index != (ChunkName::Index)-1)
        WritePropertyName(out, escaper, Convert::ToString(name.index), false) << "{\n";
    else
        WriteChunkStartLineWithString(out, escaper, name);
}


//Local types-------------------------------------------------------------------
struct JsonDataChunkWriter::Impl
{
    Settings settings;
    DataChunkWriterStyle style;
    JsonStringEscaper escaper;
};


//Implementation----------------------------------------------------------------
JsonDataChunkWriter::JsonDataChunkWriter()
{
}

JsonDataChunkWriter::~JsonDataChunkWriter()
{
    if (impl == nullptr)
        return;

    if (AnySet(impl->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(impl->style & DataChunkWriterStyle::ROOT))
        WriteFooter();
}

void JsonDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, DataChunkWriter* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    impl.reset(new Impl);
    impl->settings = settings;
    impl->style = style;
}

DataChunkWriterController& JsonDataChunkWriter::GetWriterController()
{
    return *impl->settings.controller;
}

DocumentWriterOutput* JsonDataChunkWriter::GetWriterOutput()
{
    return impl->settings.output;
}

void JsonDataChunkWriter::WriteWriterHeader(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    *impl->settings.output << "{\n";

    WriteString(StandardChunkPropertyNames::FORMAT, "Finjin Streaming JSON Document", error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write header format.");
        return;
    }

    WriteUInt32(StandardChunkPropertyNames::FORMAT_VERSION, 1, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write header format version.");
        return;
    }

    WriteCount(StandardChunkPropertyNames::MAX_BYTES_PER_LINE, 0, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write max bytes per line.");
        return;
    }

    WriteString(StandardChunkPropertyNames::BLOB_TEXT_FORMAT, DataChunkBlobTextFormatUtilities::ToString(impl->settings.blobTextFormat), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write blob text format.");
        return;
    }

    if (impl->settings.customHeader != nullptr)
    {
        impl->settings.customHeader(*this, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write custom header.");
            return;
        }
    }
}

void JsonDataChunkWriter::WriteChunk(const ChunkName& name, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->settings.controller->RequiresNewOutput(*this, name))
    {
        //Create new chunk output
        std::shared_ptr<DocumentWriterOutput> sharedNewOutput = impl->settings.controller->AddOutput(*this, name, error);
        if (error || sharedNewOutput == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create new output for chunk '%1%'.", name.name));
            return;
        }

        //Create new writer
        auto jsonChunkWriter = new JsonDataChunkWriter();
        auto newSettings = impl->settings;
        newSettings.Create(sharedNewOutput, *impl->settings.controller);
        jsonChunkWriter->Create(newSettings, DataChunkWriterStyle::ALL, nullptr, error);
        if (error)
        {
            delete jsonChunkWriter;

            FINJIN_SET_ERROR(error, "Failed to create new writer.");
            return;
        }

        std::shared_ptr<DataChunkWriter> chunkWriter(jsonChunkWriter);
        chunkWriter->InheritContextStrings(*this);

        //Write chunk to new writer
        chunkWriter->WriteWriterHeader(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write writer header.");
            return;
        }

        WRITE_CHUNK_START_LINE_STRING(jsonChunkWriter, name);

        auto scheduled = impl->settings.controller->ScheduleWriteChunk(chunkWriter, chunkFunc, error);
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
        WRITE_CHUNK_START_LINE_INDEX_OR_STRING(this, name);

        chunkFunc(*this, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write chunk.");
            return;
        }

        WRITE_CHUNK_END_LINE(this);
    }
}

void JsonDataChunkWriter::WriteChunkStart(const ChunkName& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_CHUNK_START_LINE_INDEX_OR_STRING(this, name);
}

void JsonDataChunkWriter::WriteChunkEnd(const ChunkName& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_CHUNK_END_LINE(this);
}

void JsonDataChunkWriter::WriteFooter()
{
    {
        FINJIN_DECLARE_ERROR(error); //Any error that occurs never leaves this method

        if (impl->settings.customFooter != nullptr)
        {
            impl->settings.customFooter(*this, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write custom footer.");
                return;
            }
        }
    }

    *impl->settings.output << "}\n";
}

ByteOrder JsonDataChunkWriter::GetByteOrder() const
{
    return impl->settings.byteOrder;
}

void JsonDataChunkWriter::WriteBlob(const ChunkPropertyName& propertyName, const void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (impl->settings.blobTextFormat)
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
            Utf8String base64;
            Base64::ToBase64(values, count, base64, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to convert blob to base64.");
                return;
            }

            WRITE_VALUE_QUOTED_LINE(this, propertyName, base64);

            break;
        }
        default: break;
    }
}

void JsonDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const char* value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteDateTime(const ChunkPropertyName& propertyName, const DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value.ToIso8601());
}

void JsonDataChunkWriter::WriteTimeDuration(const ChunkPropertyName& propertyName, const TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteUuid(const ChunkPropertyName& propertyName, const Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value.ToString());
}

void JsonDataChunkWriter::WriteBool(const ChunkPropertyName& propertyName, bool value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteCount(const ChunkPropertyName& propertyName, size_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteInt8(const ChunkPropertyName& propertyName, int8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteInt16(const ChunkPropertyName& propertyName, int16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteInt32(const ChunkPropertyName& propertyName, int32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteInt64(const ChunkPropertyName& propertyName, int64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteUInt8(const ChunkPropertyName& propertyName, uint8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteUInt16(const ChunkPropertyName& propertyName, uint16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteUInt32(const ChunkPropertyName& propertyName, uint32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteUInt64(const ChunkPropertyName& propertyName, uint64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteFloat(const ChunkPropertyName& propertyName, float value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteDouble(const ChunkPropertyName& propertyName, double value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void JsonDataChunkWriter::WriteStridedStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void JsonDataChunkWriter::WriteStridedDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
