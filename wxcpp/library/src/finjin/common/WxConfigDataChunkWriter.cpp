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
#include "WxConfigDataChunkWriter.hpp"
#include "finjin/common/WxBase64.hpp"

using namespace Finjin::Common;

template <typename T>
struct TextDataChunkBlobWrapper
{
    template <typename ValuesType>
    TextDataChunkBlobWrapper(ValuesType values, size_t _count) : bytes((T)values), count(_count)
    {
    }

    T bytes;
    size_t count;
};


//Macros-----------------------------------------------------------------------
#define WRITE_VALUE_LINE(_this, propertyName, value) \
    WriteValueLine(_this->documentWriter, *_this->settings.output, _this->lineBuffer, propertyName, value, error); \
    if (error) \
    { \
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) \
    WriteValuesLine(_this->documentWriter, *_this->settings.output, _this->lineBuffer, propertyName, values, count, valueStride, error); \
    if (error) \
    { \
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_CHUNK_START_LINE(_this, name) \
    auto utf8Name = ConvertToUtf8String(name); \
    if (utf8Name.length() > WxChunkName::MAX_LENGTH) \
    { \
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Chunk name length '%d' for '%s' exceeds maximum '%d'."), name.length(), name.wx_str(), WxChunkName::MAX_LENGTH)); \
        return; \
    } \
    _this->documentWriter.WriteScopeStart(name)

#define WRITE_CHUNK_END_LINE(_this) _this->documentWriter.WriteScopeEnd()


//Local functions--------------------------------------------------------------
static bool ContainsNewline(const wxString& s)
{
    return s.find('\n') != wxString::npos;
}

static const char* BoolToString(bool value)
{
    auto t = "t";
    auto f = "f";
    return value ? t : f;
}

//WritePropertyName
static WxByteBuffer& WritePropertyName(WxByteBuffer& out, const WxChunkPropertyName& propertyName)
{
    if (propertyName.index != (WxChunkPropertyName::Index)-1)
        out.Write(ConvertToUtf8String(propertyName.index));
    else
        out.Write(propertyName.name);
    
    return out;
}

//WriteTextValue
template <typename T>
void WriteTextValue(WxByteBuffer& out, const T& value, WxDocumentWriterOutput* writerOutput)
{
    out.Write("=");
    out.Write(ConvertToUtf8String(value));
}

static void WriteTextValue(WxByteBuffer& out, bool value, WxDocumentWriterOutput* writerOutput)
{
    out.Write("=");
    out.Write(BoolToString(value));
}

static void WriteTextValue(WxByteBuffer& out, TextDataChunkBlobWrapper<const uint8_t*>& value, WxDocumentWriterOutput* writerOutput)
{   
    out.Write("=");
    out.WriteBase64(value.bytes, value.count);
}

static void WriteTextValue(WxByteBuffer& out, const wxString& value, WxDocumentWriterOutput* writerOutput)
{
    auto containsNewline = ContainsNewline(value);

    if (containsNewline)
        out.Write("^=");
    else
        out.Write("=");

    out.Write(value);

    if (containsNewline)
        out.Write("\n^");
}

static void WriteTextValue(WxByteBuffer& out, const WxTimeDuration& value, WxDocumentWriterOutput* writerOutput)
{
    out.Write("=");

    out.Write(value.ToString());
}

static void WriteTextValue(WxByteBuffer& out, float value, WxDocumentWriterOutput* writerOutput)
{
    out.Write("=");
    
    if (std::abs(value) < writerOutput->GetMinFloat())
        value = 0;
    out.Write(ConvertToUtf8String(value));
}

static void WriteTextValue(WxByteBuffer& out, double value, WxDocumentWriterOutput* writerOutput)
{
    out.Write("=");
    
    if (std::abs(value) < writerOutput->GetMinDouble())
        value = 0;
    out.Write(ConvertToUtf8String(value));
}

//WriteTextValues
static void WriteTextValues(WxByteBuffer& out, const bool* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
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

static void WriteTextValues(WxByteBuffer& out, const wxString* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
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

static void WriteTextValues(WxByteBuffer& out, const WxTimeDuration* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
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
void WriteTextValues(WxByteBuffer& out, const T* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        if (writtenCount++ > 0)
            out.Write(" ");            
        
        auto& value = GetStridedValue(values, offset + startIndex, valueStride);
        out.Write(ConvertToUtf8String(value));
    }
}

static void WriteTextValues(WxByteBuffer& out, const float* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, offset + startIndex, valueStride);
        if (std::abs(value) < writerOutput->GetMinFloat())
            value = 0;
        
        if (writtenCount++ > 0)
            out.Write(" ");
        
        out.Write(ConvertToUtf8String(value));
    }
}

static void WriteTextValues(WxByteBuffer& out, const double* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
{
    size_t writtenCount = 0;
    for (size_t startIndex = 0; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, offset + startIndex, valueStride);
        if (std::abs(value) < writerOutput->GetMinDouble())
            value = 0;

        if (writtenCount++ > 0)
            out.Write(" ");
        
        out.Write(ConvertToUtf8String(value));
    }
}

static void WriteTextValues(WxByteBuffer& out, const uint8_t* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput)
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
void WriteValueLine(WxConfigDocumentWriter& documentWriter, WxDocumentWriterOutput& out, WxByteBuffer& lineBuffer, const Property& propertyName, T& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    lineBuffer.clear();

    //Write propertyName to line buffer
    WritePropertyName(lineBuffer, propertyName);
    
    //Write value to line buffer
    WriteTextValue(lineBuffer, value, &out);

    documentWriter.WriteLine(reinterpret_cast<char*>(lineBuffer.data()), lineBuffer.size());
}

template <typename Property, typename T>
void WriteValuesLine(WxConfigDocumentWriter& documentWriter, WxDocumentWriterOutput& out, WxByteBuffer& lineBuffer, const Property& propertyName, const T* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    lineBuffer.clear();

    //Write propertyName to line buffer
    WritePropertyName(lineBuffer, propertyName);
    lineBuffer.Write("=");

    //Write values to line buffer
    WriteTextValues(lineBuffer, values, 0, count, valueStride, &out);

    documentWriter.WriteLine(reinterpret_cast<char*>(lineBuffer.data()), lineBuffer.size());
}


//Implementation---------------------------------------------------------------
WxConfigDataChunkWriter::WxConfigDataChunkWriter()
{
}

WxConfigDataChunkWriter::~WxConfigDataChunkWriter()
{        
    if (AnySet(this->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(this->style & DataChunkWriterStyle::ROOT))
    {
        FINJIN_WX_DECLARE_ERROR(error);

        if (this->settings.customFooter != nullptr)
        {
            this->settings.customFooter(*this, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write custom footer."));
                return;
            }
        }
    }
}

void WxConfigDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, WxDataChunkWriter* parentSection, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Max bytes per line '%d' must be %d or greater."), settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    this->style = style;    
    this->documentWriter.Create(*this->settings.output);
    this->lineBuffer.Create(this->settings.maxBytesPerLine); //The maximum is allowed to be exceeded
}

WxDataChunkWriterController& WxConfigDataChunkWriter::GetWriterController()
{
    return *this->settings.controller;
}

void WxConfigDataChunkWriter::WriteWriterHeader(WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WriteString(WxStandardChunkPropertyNames::BLOB_TEXT_FORMAT, WxDataChunkBlobTextFormatUtilities::ToString(this->settings.blobTextFormat), error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write blob text format."));
        return;
    }

    if (this->settings.customHeader != nullptr)
    {
        this->settings.customHeader(*this, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write custom header."));
            return;
        }
    }
}

void WxConfigDataChunkWriter::WriteChunk(const WxChunkName& name, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (this->settings.controller->RequiresNewOutput(*this, name))
    {
        //Create new chunk output 
        std::shared_ptr<WxDocumentWriterOutput> sharedNewOutput = this->settings.controller->AddOutput(*this, name, error);
        if (error || sharedNewOutput == nullptr)
        {
            FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to create new output for chunk '%s'."), name.ToString().wx_str()));
            return;
        }        
        
        //Create new writer 
        auto textChunkWriter = new WxConfigDataChunkWriter();
        auto newSettings = this->settings;
        newSettings.Create(sharedNewOutput, *this->settings.controller);
        textChunkWriter->Create(newSettings, DataChunkWriterStyle::ALL, nullptr, error);
        if (error)
        {
            delete textChunkWriter;
            
            FINJIN_WX_SET_ERROR(error, wxT("Failed to create new writer."));
            return;
        }

        std::shared_ptr<WxDataChunkWriter> chunkWriter(textChunkWriter);
        chunkWriter->InheritContextStrings(*this);

        //Write chunk to new writer
        chunkWriter->WriteWriterHeader(error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write writer header."));
            return;
        }

        auto nameString = name.ToString();
        WRITE_CHUNK_START_LINE(textChunkWriter, nameString);
        
        auto scheduled = this->settings.controller->ScheduleWriteChunk(chunkWriter, chunkFunc, error);
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
        wxString nameString;
        if (name.index != (WxChunkName::Index)-1)
            nameString << name.index;
        else
            nameString = name.ToString();
        WRITE_CHUNK_START_LINE(this, nameString);
        
        chunkFunc(*this, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk."));
            return;
        }

        WRITE_CHUNK_END_LINE(this);
    }
}

void WxConfigDataChunkWriter::WriteFooter()
{    
}

void WxConfigDataChunkWriter::WriteBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);
    
    switch (this->settings.blobTextFormat)
    {
        case WxDataChunkBlobTextFormat::BYTE_ARRAY:
        {
            this->lineBuffer.resize(count * 2);
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
            TextDataChunkBlobWrapper<const uint8_t*> blob(values, count);
            WRITE_VALUE_LINE(this, propertyName, blob);

            break;
        }
    }    
}

void WxConfigDataChunkWriter::WriteString(const WxChunkPropertyName& propertyName, const wxString& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteDateTime(const WxChunkPropertyName& propertyName, const wxDateTime& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    auto s = FormatIso8601(value);
    WRITE_VALUE_LINE(this, propertyName, s);
}

void WxConfigDataChunkWriter::WriteTimeDuration(const WxChunkPropertyName& propertyName, const WxTimeDuration& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteUuid(const WxChunkPropertyName& propertyName, const WxUuid& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    auto s = value.ToString();
    WRITE_VALUE_LINE(this, propertyName, s);
}

void WxConfigDataChunkWriter::WriteBool(const WxChunkPropertyName& propertyName, bool value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteCount(const WxChunkPropertyName& propertyName, size_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteInt8(const WxChunkPropertyName& propertyName, int8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteInt16(const WxChunkPropertyName& propertyName, int16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteInt32(const WxChunkPropertyName& propertyName, int32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteInt64(const WxChunkPropertyName& propertyName, int64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteUInt8(const WxChunkPropertyName& propertyName, uint8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteUInt16(const WxChunkPropertyName& propertyName, uint16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteUInt32(const WxChunkPropertyName& propertyName, uint32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteUInt64(const WxChunkPropertyName& propertyName, uint64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteFloat(const WxChunkPropertyName& propertyName, float value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteDouble(const WxChunkPropertyName& propertyName, double value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxConfigDataChunkWriter::WriteStridedStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxConfigDataChunkWriter::WriteStridedDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
