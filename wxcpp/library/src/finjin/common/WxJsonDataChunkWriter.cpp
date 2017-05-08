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
#include "WxJsonDataChunkWriter.hpp"
#include "finjin/common/WxBase64.hpp"

using namespace Finjin::Common;

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

typedef rapidjson::StringBuffer JsonStringBuffer;
typedef rapidjson::Reader JsonReader;
typedef rapidjson::Writer<JsonStringBuffer> JsonStringWriter;
typedef rapidjson::PrettyWriter<JsonStringBuffer> JsonStringPrettyWriter;
typedef rapidjson::Value JsonValue;
typedef rapidjson::Value JsonElement;


//Macros------------------------------------------------------------------------
#define WRITE_VALUE_QUOTED_LINE(_this, propertyName, stringValue) WriteQuotedLine(*_this->impl->settings.output, _this->impl->escaper, propertyName, stringValue)

#define WRITE_VALUE_LINE(_this, propertyName, value) WriteValueLine(*_this->impl->settings.output, _this->impl->escaper, propertyName, value)

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) WriteValuesLine(*_this->impl->settings.output, _this->impl->escaper, propertyName, values, count, valueStride)

#define WRITE_CHUNK_START_LINE_STRING(_this, name) WriteChunkStartLineWithString(*_this->impl->settings.output, _this->impl->escaper, name)
#define WRITE_CHUNK_START_LINE_INDEX_OR_STRING(_this, name) WriteChunkStartLineWithIndexOrString(*_this->impl->settings.output, _this->impl->escaper, name)

#define WRITE_CHUNK_END_LINE(_this) *_this->impl->settings.output << "},\n";


//Local functions---------------------------------------------------------------
inline WxDocumentWriterOutput& operator << (WxDocumentWriterOutput& out, const char* value)
{
    out.Write(value);
    return out;
}

inline WxDocumentWriterOutput& operator << (WxDocumentWriterOutput& out, const wxString& value)
{
    out.Write(value.ToUTF8().data());
    return out;
}

inline WxDocumentWriterOutput& operator << (WxDocumentWriterOutput& out, const std::string& value)
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

    const JsonStringBuffer::Ch* Escape(const char* s)
    {
        this->buffer.Clear();

        WriteString(s, static_cast<rapidjson::SizeType>(strlen(s)));
        return this->buffer.GetString();
    }

    const JsonStringBuffer::Ch* Escape(const wxString& s)
    {
        this->buffer.Clear();

        auto sBuffer = s.ToUTF8();
        WriteString(sBuffer.data(), static_cast<rapidjson::SizeType>(strlen(sBuffer.data())));
        return this->buffer.GetString();
    }

private:
    JsonStringBuffer buffer;
};

static wxString RewriteKeyToJSONStyle(const wxString& propertyName)
{
    wxString result;

    wxChar previousChar = 0;
    for (auto c : propertyName)
    {
        if (previousChar == wxT('-') && wxIsalpha(c))
            result += wxToupper(c);
        else if (c != '-')
            result += c;

        previousChar = c;
    }

    return result;
}

static wxString RewriteKeyToJSONStyle(const char* propertyName)
{
    return RewriteKeyToJSONStyle(wxString::FromUTF8(propertyName));
}

static WxDocumentWriterOutput& WritePropertyName(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const wxString& propertyName, bool rewriteKey = true)
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

static WxDocumentWriterOutput& WritePropertyName(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const WxChunkPropertyName& propertyName, bool rewriteKey = true)
{
    if (propertyName.IsValid())
    {
        if (propertyName.index != (WxChunkPropertyName::Index)-1)
            out << ConvertToUtf8String(propertyName.index) << ":";
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

static void WriteJsonValue(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, bool value)
{
    out << BoolToString(value);
}

template <typename T>
void WriteJsonValue(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const T& value)
{
    out << ConvertToUtf8String(value);
}

template <typename T>
void WriteJsonValues(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const T* values, size_t count, WxDataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto& value = GetStridedValue(values, i, valueStride);
        out << ConvertToUtf8String(value);
    }
    out << "]";
}

static void WriteJsonValues(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const bool* values, size_t count, WxDataChunkWriteStride valueStride)
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

static void WriteJsonValues(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const wxString* values, size_t count, WxDataChunkWriteStride valueStride)
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

static void WriteJsonValues(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride)
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

static void WriteJsonValues(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const float* values, size_t count, WxDataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto value = GetStridedValue(values, i, valueStride);
        if (std::abs(value) < out.GetMinFloat())
            value = 0;

        out << ConvertToUtf8String(value);
    }
    out << "]";
}

static void WriteJsonValues(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const double* values, size_t count, WxDataChunkWriteStride valueStride)
{
    out << "[";
    for (size_t i = 0; i < count; i++)
    {
        if (i > 0)
            out << ",";

        auto value = GetStridedValue(values, i, valueStride);
        if (std::abs(value) < out.GetMinDouble())
            value = 0;

        out << ConvertToUtf8String(value);
    }
    out << "]";
}

template <typename Property, typename T>
void WriteQuotedLine(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const T& stringValue)
{
    WritePropertyName(out, escaper, propertyName);
    out << escaper.Escape(stringValue);
    out << ",\n";
}

template <typename Property>
void WriteQuotedLine(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const WxTimeDuration& stringValue)
{
    WritePropertyName(out, escaper, propertyName);
    out << escaper.Escape(stringValue.ToString());
    out << ",\n";
}

template <typename Property, typename T>
void WriteValueLine(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const T& value)
{
    WritePropertyName(out, escaper, propertyName);
    WriteJsonValue(out, escaper, value);
    out << ",\n";
}

template <typename Property, typename T>
void WriteValuesLine(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const Property& propertyName, const T* values, size_t count, WxDataChunkWriteStride valueStride)
{
    WritePropertyName(out, escaper, propertyName);
    WriteJsonValues(out, escaper, values, count, valueStride);
    out << ",\n";
}

static void WriteChunkStartLineWithString(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const WxChunkName& name)
{
    wxString stringName = RewriteKeyToJSONStyle(name.name);
    stringName += wxT("|");
    stringName += name.id.ToString();

    WritePropertyName(out, escaper, stringName, false) << "{\n";
}

static void WriteChunkStartLineWithIndexOrString(WxDocumentWriterOutput& out, JsonStringEscaper& escaper, const WxChunkName& name)
{
    if (name.index != (WxChunkName::Index)-1)
    {
        wxString indexString;
        indexString << name.index;
        WritePropertyName(out, escaper, indexString, false) << "{\n";
    }
    else
        WriteChunkStartLineWithString(out, escaper, name);
}


//Local types-------------------------------------------------------------------
struct WxJsonDataChunkWriter::Impl
{
    Settings settings;
    DataChunkWriterStyle style;
    JsonStringEscaper escaper;
};


//Implementation----------------------------------------------------------------
WxJsonDataChunkWriter::WxJsonDataChunkWriter()
{
    impl = nullptr;
}

WxJsonDataChunkWriter::~WxJsonDataChunkWriter()
{
    if (impl == nullptr)
        return;

    if (AnySet(impl->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(impl->style & DataChunkWriterStyle::ROOT))
        WriteFooter();

    delete impl;
}

void WxJsonDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, WxDataChunkWriter* parentSection, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Max bytes per line '%d' must be %d or greater."), settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    if (impl != nullptr)
        delete impl;

    impl = new Impl;
    impl->settings = settings;
    impl->style = style;
}

WxDataChunkWriterController& WxJsonDataChunkWriter::GetWriterController()
{
    return *impl->settings.controller;
}

void WxJsonDataChunkWriter::WriteWriterHeader(WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    *impl->settings.output << "{\n";

    WriteString(WxStandardChunkPropertyNames::FORMAT, wxT("Finjin Streaming JSON Document"), error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write header format."));
        return;
    }

    WriteUInt32(WxStandardChunkPropertyNames::FORMAT_VERSION, 1, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write header format version."));
        return;
    }

    WriteCount(WxStandardChunkPropertyNames::MAX_BYTES_PER_LINE, 0, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write max bytes per line."));
        return;
    }

    WriteString(WxStandardChunkPropertyNames::BLOB_TEXT_FORMAT, WxDataChunkBlobTextFormatUtilities::ToString(impl->settings.blobTextFormat), error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write blob text format."));
        return;
    }

    if (impl->settings.customHeader != nullptr)
    {
        impl->settings.customHeader(*this, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write custom header."));
            return;
        }
    }
}

void WxJsonDataChunkWriter::WriteChunk(const WxChunkName& name, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (impl->settings.controller->RequiresNewOutput(*this, name))
    {
        //Create new chunk output
        std::shared_ptr<WxDocumentWriterOutput> sharedNewOutput = impl->settings.controller->AddOutput(*this, name, error);
        if (error || sharedNewOutput == nullptr)
        {
            FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to create new output for chunk '%s'."), name.ToString().wx_str()));
            return;
        }

        //Create new writer
        auto jsonChunkWriter = new WxJsonDataChunkWriter();
        auto newSettings = impl->settings;
        newSettings.Create(sharedNewOutput, *impl->settings.controller);
        jsonChunkWriter->Create(newSettings, DataChunkWriterStyle::ALL, nullptr, error);
        if (error)
        {
            delete jsonChunkWriter;

            FINJIN_WX_SET_ERROR(error, wxT("Failed to create new writer."));
            return;
        }

        std::shared_ptr<WxDataChunkWriter> chunkWriter(jsonChunkWriter);
        chunkWriter->InheritContextStrings(*this);

        //Write chunk to new writer
        chunkWriter->WriteWriterHeader(error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write writer header."));
            return;
        }

        WRITE_CHUNK_START_LINE_STRING(jsonChunkWriter, name);

        auto scheduled = impl->settings.controller->ScheduleWriteChunk(chunkWriter, chunkFunc, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to schedule/execute chunk writer."));
            return;
        }

        if (!scheduled)
        {
            //Write chunk to the new writer's output
            chunkFunc(*chunkWriter, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk."));
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
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk."));
            return;
        }

        WRITE_CHUNK_END_LINE(this);
    }
}

void WxJsonDataChunkWriter::WriteFooter()
{
    {
        FINJIN_WX_DECLARE_ERROR(error);

        if (impl->settings.customFooter != nullptr)
        {
            impl->settings.customFooter(*this, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write custom footer."));
                return;
            }
        }
    }

    *impl->settings.output << "}\n";
}

ByteOrder WxJsonDataChunkWriter::GetByteOrder() const
{
    return impl->settings.byteOrder;
}

void WxJsonDataChunkWriter::WriteBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    switch (impl->settings.blobTextFormat)
    {
        case WxDataChunkBlobTextFormat::BYTE_ARRAY:
        {
            WriteUInt8s(propertyName, static_cast<const uint8_t*>(values), count, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
                return;
            }

            break;
        }
        case WxDataChunkBlobTextFormat::BASE64:
        {
            wxString base64;
            WxBase64::ToBase64(values, count, base64, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to convert blob to base64."));
                return;
            }

            WRITE_VALUE_QUOTED_LINE(this, propertyName, base64);

            break;
        }
    }
}

void WxJsonDataChunkWriter::WriteString(const WxChunkPropertyName& propertyName, const wxString& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteDateTime(const WxChunkPropertyName& propertyName, const wxDateTime& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, FormatIso8601(value));
}

void WxJsonDataChunkWriter::WriteTimeDuration(const WxChunkPropertyName& propertyName, const WxTimeDuration& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteUuid(const WxChunkPropertyName& propertyName, const WxUuid& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_QUOTED_LINE(this, propertyName, value.ToString());
}

void WxJsonDataChunkWriter::WriteBool(const WxChunkPropertyName& propertyName, bool value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteCount(const WxChunkPropertyName& propertyName, size_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteInt8(const WxChunkPropertyName& propertyName, int8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteInt16(const WxChunkPropertyName& propertyName, int16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteInt32(const WxChunkPropertyName& propertyName, int32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteInt64(const WxChunkPropertyName& propertyName, int64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteUInt8(const WxChunkPropertyName& propertyName, uint8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteUInt16(const WxChunkPropertyName& propertyName, uint16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteUInt32(const WxChunkPropertyName& propertyName, uint32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteUInt64(const WxChunkPropertyName& propertyName, uint64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteFloat(const WxChunkPropertyName& propertyName, float value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteDouble(const WxChunkPropertyName& propertyName, double value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxJsonDataChunkWriter::WriteStridedStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxJsonDataChunkWriter::WriteStridedDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
