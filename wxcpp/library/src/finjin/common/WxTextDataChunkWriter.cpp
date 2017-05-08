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
#include "WxTextDataChunkWriter.hpp"
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


//Macros------------------------------------------------------------------------
#define WRITE_VALUE_LINE(_this, propertyName, value) \
    WriteValueLine(*_this->settings.output, _this->lineBuffer, propertyName, value, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) \
    WriteValuesLine(*_this->settings.output, _this->lineBuffer, propertyName, values, count, valueStride, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_CHUNK_START_LINE(_this, name) \
    auto utf8Name = ConvertToUtf8String(name); \
    if (utf8Name.length() > WxChunkName::MAX_LENGTH) \
    { \
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Chunk UTF-8 name length '%d' for '%s' exceeds maximum '%d'."), utf8Name.length(), name.wx_str(), WxChunkName::MAX_LENGTH)); \
        return; \
    } \
    WRITE_VALUE_LINE(_this, name, "{")

#define WRITE_CHUNK_END_LINE(_this) _this->settings.output->Write("2 }\n")


//Local functions---------------------------------------------------------------

//SizeOfValue
template <typename T>
size_t SizeOfValue(const T& value, WxDocumentWriterOutput& writerOutput)
{
    return ConvertToUtf8String(value).length();
}

static size_t SizeOfValue(bool value, WxDocumentWriterOutput& writerOutput)
{
    return 1; //t or f
}

static size_t SizeOfValue(float value, WxDocumentWriterOutput& writerOutput)
{
    if (std::abs(value) < writerOutput.GetMinFloat())
        value = 0;

    return ConvertToUtf8String(value).length();
}

static size_t SizeOfValue(double value, WxDocumentWriterOutput& writerOutput)
{
    if (std::abs(value) < writerOutput.GetMinDouble())
        value = 0;

    return ConvertToUtf8String(value).length();
}

template <typename T>
size_t SizeOfValue(const TextDataChunkBlobWrapper<T>& value, WxDocumentWriterOutput& writerOutput)
{
    return WxBase64::ToBase64Count(value.count);
}

static size_t SizeOfValue(const wxString& value, WxDocumentWriterOutput& writerOutput)
{
    return strlen(value.ToUTF8().data());
}

static size_t SizeOfValue(const char* value, WxDocumentWriterOutput& writerOutput)
{
    return strlen(value);
}

static size_t SizeOfValue(const WxTimeDuration& value, WxDocumentWriterOutput& writerOutput)
{
    return strlen(value.ToString().ToUTF8().data());
}

static size_t SizeOfValue(const WxChunkPropertyName& value)
{
    if (value.index != (WxChunkPropertyName::Index)-1)
        return ConvertToUtf8String(value.index).length();
    else
        return strlen(value.name);
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

static WxByteBuffer& WritePropertyName(WxByteBuffer& out, const wxString& propertyName)
{
    if (propertyName[0] != 0)
        out.Write(propertyName);

    return out;
}

//WriteTextValue
static void WriteTextValue(WxByteBuffer& out, bool value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    out.Write(BoolToString(value));
}

static void WriteTextValue(WxByteBuffer& out, TextDataChunkBlobWrapper<const uint8_t*>& value, WxDocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    auto bytesProcessed = out.WriteBase64(value.bytes, value.count, maxBytesToWrite);
    value.bytes += bytesProcessed;
    value.count -= bytesProcessed;
}

static void WriteTextValue(WxByteBuffer& out, const char* value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    auto length = strlen(value) - byteOffset;
    if (length > maxBytesToWrite)
        length = maxBytesToWrite;

    out.Write(value + byteOffset, length);
}

static void WriteTextValue(WxByteBuffer& out, const wxString& value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    auto valueData = value.ToUTF8();
    auto length = strlen(valueData.data()) - byteOffset;
    if (length > maxBytesToWrite)
        length = maxBytesToWrite;

    out.Write(valueData.data() + byteOffset, length);
}

static void WriteTextValue(WxByteBuffer& out, const WxTimeDuration& value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    auto valueData = value.ToString().ToUTF8();
    auto length = strlen(valueData.data()) - byteOffset;
    if (length > maxBytesToWrite)
        length = maxBytesToWrite;

    out.Write(valueData.data() + byteOffset, length);
}

template <typename T>
void WriteTextValue(WxByteBuffer& out, const T& value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    if (sizeof(T) <= maxBytesToWrite)
        out.Write(ConvertToUtf8String(value));
}

static void WriteTextValue(WxByteBuffer& out, float value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    if (sizeof(value) <= maxBytesToWrite)
    {
        if (std::abs(value) < writerOutput->GetMinFloat())
            value = 0;

        out.Write(ConvertToUtf8String(value));
    }
}

static void WriteTextValue(WxByteBuffer& out, double value, WxDocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    if (sizeof(value) <= maxBytesToWrite)
    {
        if (std::abs(value) < writerOutput->GetMinDouble())
            value = 0;

        out.Write(ConvertToUtf8String(value));
    }
}

//WriteTextValues
static void WriteTextValues(WxByteBuffer& out, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto& value = GetStridedValue(values, startIndex, valueStride);

        auto valueOutputByteSize = SizeOfValue(value, *writerOutput);

        if (writtenCount++ > 0)
        {
            //Need enough space for separator and value
            if ((1 + valueOutputByteSize) > maxBytesToWrite)
                break;

            out.Write(" ");
            maxBytesToWrite--;
        }
        else
        {
            //Need enough space for value
            if (valueOutputByteSize > maxBytesToWrite)
                break;
        }

        out.Write(BoolToString(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

static void WriteTextValues(WxByteBuffer& out, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto& value = GetStridedValue(values, startIndex, valueStride);

        auto valueOutputByteSize = SizeOfValue(value, *writerOutput);

        if (writtenCount++ > 0)
        {
            //Need enough space for separator and value
            if ((1 + valueOutputByteSize) > maxBytesToWrite)
                break;

            out.Write(" ");
            maxBytesToWrite--;
        }
        else
        {
            //Need enough space for value
            if (valueOutputByteSize > maxBytesToWrite)
                break;
        }

        out.Write(value);
        maxBytesToWrite -= valueOutputByteSize;
    }
}

static void WriteTextValues(WxByteBuffer& out, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, startIndex, valueStride).ToString();

        auto valueOutputByteSize = SizeOfValue(value, *writerOutput);

        if (writtenCount++ > 0)
        {
            //Need enough space for separator and value
            if ((1 + valueOutputByteSize) > maxBytesToWrite)
                break;

            out.Write(" ");
            maxBytesToWrite--;
        }
        else
        {
            //Need enough space for value
            if (valueOutputByteSize > maxBytesToWrite)
                break;
        }

        out.Write(value);
        maxBytesToWrite -= valueOutputByteSize;
    }
}

template <typename T>
void WriteTextValues(WxByteBuffer& out, const T* values, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto& value = GetStridedValue(values, startIndex, valueStride);

        auto valueOutputByteSize = SizeOfValue(value, *writerOutput);

        if (writtenCount++ > 0)
        {
            //Need enough space for separator and value
            if ((1 + valueOutputByteSize) > maxBytesToWrite)
                break;

            out.Write(" ");
            maxBytesToWrite--;
        }
        else
        {
            //Need enough space for value
            if (valueOutputByteSize > maxBytesToWrite)
                break;
        }

        out.Write(ConvertToUtf8String(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

static void WriteTextValues(WxByteBuffer& out, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, startIndex, valueStride);

        if (std::abs(value) < writerOutput->GetMinFloat())
            value = 0;

        auto valueOutputByteSize = SizeOfValue(value, *writerOutput);

        if (writtenCount++ > 0)
        {
            //Need enough space for separator and value
            if ((1 + valueOutputByteSize) > maxBytesToWrite)
                break;

            out.Write(" ");
            maxBytesToWrite--;
        }
        else
        {
            //Need enough space for value
            if (valueOutputByteSize > maxBytesToWrite)
                break;
        }

        out.Write(ConvertToUtf8String(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

static void WriteTextValues(WxByteBuffer& out, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxDocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, startIndex, valueStride);

        if (std::abs(value) < writerOutput->GetMinDouble())
            value = 0;

        auto valueOutputByteSize = SizeOfValue(value, *writerOutput);

        if (writtenCount++ > 0)
        {
            //Need enough space for separator and value
            if ((1 + valueOutputByteSize) > maxBytesToWrite)
                break;

            out.Write(" ");
            maxBytesToWrite--;
        }
        else
        {
            //Need enough space for value
            if (valueOutputByteSize > maxBytesToWrite)
                break;
        }

        out.Write(ConvertToUtf8String(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

//WriteValueLine
template <typename Property, typename T>
void WriteValueLine(WxDocumentWriterOutput& out, WxByteBuffer& lineBuffer, const Property& propertyName, T& value, size_t maxBytesPerLine, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    auto maxBytesPerLineString = ConvertToUtf8String(maxBytesPerLine);
    if (maxBytesPerLineString.length() >= maxBytesPerLine)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Unable to write value line length for property. The line buffer is too small."));
        return;
    }

    //Subtract worst case scenario of line length header being the maximum bytes, plus the following ' '
    auto maxNonHeaderSizeBytes = maxBytesPerLine - maxBytesPerLineString.length() - 1;

    size_t linesWritten = 0;
    size_t valueByteCountWritten = 0;
    size_t valueByteCountToWrite = SizeOfValue(value, out);
    while (valueByteCountWritten < valueByteCountToWrite)
    {
        lineBuffer.clear();

        //Write propertyName to line buffer
        WritePropertyName(lineBuffer, propertyName);
        auto maxValueBytes = maxNonHeaderSizeBytes - lineBuffer.size() - 1 - 1; //Subtract one for next ' ' and one for the trailing newline
        lineBuffer.Write(" ");

        //Write as much of value as possible to line buffer
        char occurrenceChar = ' '; //' ' indicates a single occurrence. This may be overwritten later
        auto before = lineBuffer.size();
        WriteTextValue(lineBuffer, value, &out, valueByteCountWritten, maxValueBytes);
        auto amountWritten = lineBuffer.size() - before;
        valueByteCountWritten += amountWritten;
        if (valueByteCountWritten != valueByteCountToWrite)
        {
            if (amountWritten == 0)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Unable to write value for property. The line buffer is too small."));
                return;
            }

            if (linesWritten == 0)
            {
                //'+' indicates first of multiple occurrences
                occurrenceChar = '+';
            }
            else
            {
                //'~' indicates multiple occurrences
                occurrenceChar = '~';
            }
        }
        else if (linesWritten > 0)
        {
            //'!' indicates last of multiple occurrences
            occurrenceChar = '!';
        }

        lineBuffer.Write("\n");

        //Finally output line length + ' ' + line buffer
        wxString lineBufferLengthString;
        lineBufferLengthString << lineBuffer.size();
        out.Write(lineBufferLengthString.ToUTF8().data());
        out.Write(&occurrenceChar, 1);
        out.Write(lineBuffer.data(), lineBuffer.size());

        linesWritten++;
    }
}

template <typename Property, typename T>
void WriteValuesLine(WxDocumentWriterOutput& out, WxByteBuffer& lineBuffer, const Property& propertyName, const T* values, size_t count, WxDataChunkWriteStride valueStride, size_t maxBytesPerLine, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    auto maxBytesPerLineString = ConvertToUtf8String(maxBytesPerLine);
    if (maxBytesPerLineString.length() >= maxBytesPerLine)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Unable to write value line length for property. The line buffer is too small."));
        return;
    }

    //Subtract worst case scenario of line length header being the maximum bytes, plus the following ' '
    auto maxNonHeaderSizeBytes = maxBytesPerLine - maxBytesPerLineString.length() - 1;

    size_t startIndex = 0;

    size_t linesWritten = 0;
    size_t valueByteCountWritten = 0;
    while (startIndex < count)
    {
        lineBuffer.clear();

        //Write propertyName to line buffer
        WritePropertyName(lineBuffer, propertyName);
        auto maxValueBytes = maxNonHeaderSizeBytes - lineBuffer.size() - 1 - 1; //Subtract one for next ' ' and one for the trailing newline
        lineBuffer.Write(" ");

        //Write as much of value as possible to line buffer
        char occurrenceChar = ' '; //' ' indicates a single occurrence. This may be overwritten later
        auto before = lineBuffer.size();
        WriteTextValues(lineBuffer, values, count, valueStride, &out, startIndex, maxValueBytes);
        auto amountWritten = lineBuffer.size() - before;
        valueByteCountWritten += amountWritten;
        if (startIndex != count)
        {
            if (amountWritten == 0)
            {
                //auto& value = GetStridedValue(values, startIndex, valueStride);
                FINJIN_WX_SET_ERROR(error, wxT("Unable to write value for property. The line buffer is too small for value"));
                return;
            }

            if (linesWritten == 0)
            {
                //'+' indicates first of multiple occurrences
                occurrenceChar = '+';
            }
            else
            {
                //'~' indicates multiple occurrences
                occurrenceChar = '~';
            }
        }
        else if (linesWritten > 0)
        {
            //'!' indicates last of multiple occurrences
            occurrenceChar = '!';
        }

        lineBuffer.Write("\n");

        //Finally output line length + ' ' + line buffer
        wxString lineBufferLengthString;
        lineBufferLengthString << lineBuffer.size();
        out.Write(lineBufferLengthString.ToUTF8().data());
        out.Write(&occurrenceChar, 1);
        out.Write(lineBuffer.data(), lineBuffer.size());

        linesWritten++;
    }
}

//StringValueRequiresEscape
static bool StringValueRequiresEscape(const char* value)
{
    //Need to escape:
    //A string of "{"s
    //A string of "}"s

    if (value == nullptr || value[0] == 0)
        return false;

    auto firstChar = value[0];
    for (size_t i = 0; value[i] != 0; i++)
    {
        auto c = value[i];
        if (c != '{' && c != '}')
            return false;
        if (c != firstChar)
            return false;
    }

    return true;
}

static bool StringValueRequiresEscape(const wxString& value)
{
    auto valueData = value.ToUTF8();
    return StringValueRequiresEscape(valueData.data());
}

//EscapeStringValue
static wxString EscapeStringValue(const wxString& value)
{
    return wxString(value.length() * 2, value[0]);
}


//Implementation----------------------------------------------------------------
WxTextDataChunkWriter::WxTextDataChunkWriter()
{
}

WxTextDataChunkWriter::~WxTextDataChunkWriter()
{
    if (AnySet(this->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(this->style & DataChunkWriterStyle::ROOT))
        WriteFooter();
}

void WxTextDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, WxDataChunkWriter* parentSection, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Max bytes per line '%d' must be %d or greater."), settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    this->style = style;
    this->lineBuffer.Create(this->settings.maxBytesPerLine);
}

WxDataChunkWriterController& WxTextDataChunkWriter::GetWriterController()
{
    return *this->settings.controller;
}

void WxTextDataChunkWriter::WriteWriterHeader(WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WriteString(WxStandardChunkPropertyNames::FORMAT, wxT("Finjin Streaming Text Document"), error);
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

    WriteCount(WxStandardChunkPropertyNames::MAX_BYTES_PER_LINE, this->settings.maxBytesPerLine, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write max bytes per line."));
        return;
    }

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

void WxTextDataChunkWriter::WriteChunk(const WxChunkName& name, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error)
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
        auto textChunkWriter = new WxTextDataChunkWriter();
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

void WxTextDataChunkWriter::WriteFooter()
{
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

    this->settings.output->Write("-");
}

bool WxTextDataChunkWriter::WillSplitBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count)
{
    size_t valueLength = 0;

    switch (this->settings.blobTextFormat)
    {
        case WxDataChunkBlobTextFormat::BYTE_ARRAY:
        {
            auto bytes = static_cast<const uint8_t*>(values);

            for (size_t i = 0; i < count; i++)
            {
                if (i > 0)
                    valueLength += 1; //strlen(" ")

                if (bytes[i] <= 9)
                    valueLength += 1; //1 digit
                else if (bytes[i] <= 99)
                    valueLength += 2; //2 digits
                else
                    valueLength += 3; //3 digits

                if (valueLength > this->settings.maxBytesPerLine)
                    return true;
            }

            break;
        }
        case WxDataChunkBlobTextFormat::BASE64:
        {
            valueLength = WxBase64::ToBase64Count(count);
            break;
        }
    }

    auto nonSizeHeaderLength = SizeOfValue(propertyName) + 1 + valueLength + 1; //Trailing +1 is for newline
    auto sizeHeaderLength = ConvertToUtf8String(nonSizeHeaderLength).length() + 1;
    auto proposedLength = sizeHeaderLength + nonSizeHeaderLength;
    return proposedLength > this->settings.maxBytesPerLine;
}

bool WxTextDataChunkWriter::WillSplitString(const WxChunkPropertyName& propertyName, const wxString& value)
{
    size_t valueLength;
    if (StringValueRequiresEscape(value))
        valueLength = EscapeStringValue(value).ToUTF8().length();
    else
        valueLength = value.ToUTF8().length();

    auto nonSizeHeaderLength = SizeOfValue(propertyName) + 1 + valueLength + 1; //Trailing +1 is for newline
    auto sizeHeaderLength = ConvertToUtf8String(nonSizeHeaderLength).length() + 1;
    auto proposedLength = sizeHeaderLength + nonSizeHeaderLength;
    return proposedLength > this->settings.maxBytesPerLine;
}

void WxTextDataChunkWriter::WriteBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitBlob(propertyName, values, count))
    {
        WriteCount(this->settings.lengthHintPropertyName, count, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write length hint."));
            return;
        }
    }

    switch (this->settings.blobTextFormat)
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
            TextDataChunkBlobWrapper<const uint8_t*> blob(values, count);
            WRITE_VALUE_LINE(this, propertyName, blob);

            break;
        }
    }
}

void WxTextDataChunkWriter::WriteString(const WxChunkPropertyName& propertyName, const wxString& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitString(propertyName, value))
    {
        WriteCount(this->settings.lengthHintPropertyName, value.ToUTF8().length(), error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write length hint."));
            return;
        }
    }

    if (StringValueRequiresEscape(value))
    {
        auto s = EscapeStringValue(value);
        WRITE_VALUE_LINE(this, propertyName, s);
    }
    else
    {
        WRITE_VALUE_LINE(this, propertyName, value);
    }
}

void WxTextDataChunkWriter::WriteDateTime(const WxChunkPropertyName& propertyName, const wxDateTime& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    auto s = FormatIso8601(value);
    WRITE_VALUE_LINE(this, propertyName, s);
}

void WxTextDataChunkWriter::WriteTimeDuration(const WxChunkPropertyName& propertyName, const WxTimeDuration& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteUuid(const WxChunkPropertyName& propertyName, const WxUuid& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    auto s = value.ToString();
    WRITE_VALUE_LINE(this, propertyName, s);
}

void WxTextDataChunkWriter::WriteBool(const WxChunkPropertyName& propertyName, bool value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteCount(const WxChunkPropertyName& propertyName, size_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteInt8(const WxChunkPropertyName& propertyName, int8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteInt16(const WxChunkPropertyName& propertyName, int16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteInt32(const WxChunkPropertyName& propertyName, int32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteInt64(const WxChunkPropertyName& propertyName, int64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteUInt8(const WxChunkPropertyName& propertyName, uint8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteUInt16(const WxChunkPropertyName& propertyName, uint16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteUInt32(const WxChunkPropertyName& propertyName, uint32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteUInt64(const WxChunkPropertyName& propertyName, uint64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteFloat(const WxChunkPropertyName& propertyName, float value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteDouble(const WxChunkPropertyName& propertyName, double value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxTextDataChunkWriter::WriteStridedStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxTextDataChunkWriter::WriteStridedDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
