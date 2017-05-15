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
#include "finjin/common/TextDataChunkWriter.hpp"
#include "finjin/common/Base64.hpp"
#include "finjin/common/Convert.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define WRITE_VALUE_LINE(_this, propertyName, value) \
    WriteValueLine(*_this->settings.output, _this->lineBuffer, propertyName, value, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) \
    WriteValuesLine(*_this->settings.output, _this->lineBuffer, propertyName, values, count, valueStride, _this->settings.maxBytesPerLine, error); \
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
    WRITE_VALUE_LINE(_this, name, "{")

#define WRITE_CHUNK_END_LINE(_this) _this->settings.output->Write("2 }\n")


//Local functions---------------------------------------------------------------

//SizeOfValue
template <typename T>
static size_t SizeOfValue(const T& value, DocumentWriterOutput& writerOutput)
{
    return Convert::ToString(value).length();
}

static size_t SizeOfValue(bool value, DocumentWriterOutput& writerOutput)
{
    return 1; //t or f
}

static size_t SizeOfValue(float value, DocumentWriterOutput& writerOutput)
{
    if (GetAbs(value) < writerOutput.GetMinFloat())
        value = 0;
    return Convert::ToString(value).length();
}

static size_t SizeOfValue(double value, DocumentWriterOutput& writerOutput)
{
    if (GetAbs(value) < writerOutput.GetMinDouble())
        value = 0;
    return Convert::ToString(value).length();
}

template <typename T>
size_t SizeOfValue(const TextDataChunkBlobWrapper<T>& value, DocumentWriterOutput& writerOutput)
{
    return Base64::ToBase64Count(value.count);
}

static size_t SizeOfValue(const Utf8String& value, DocumentWriterOutput& writerOutput)
{
    return value.length();
}

static size_t SizeOfValue(const char* value, DocumentWriterOutput& writerOutput)
{
    return strlen(value);
}

static size_t SizeOfValue(const TimeDuration& value, DocumentWriterOutput& writerOutput)
{
    return value.ToString().length();
}

static size_t SizeOfValue(const ChunkPropertyName& value)
{
    if (value.index != (ChunkPropertyName::Index)-1)
        return Convert::ToString(value.index).length();
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
static ByteBuffer& WritePropertyName(ByteBuffer& out, const ChunkPropertyName& propertyName)
{
    if (propertyName.index != (ChunkPropertyName::Index)-1)
        out.Write(Convert::ToString(propertyName.index));
    else
        out.Write(propertyName.name);

    return out;
}

static ByteBuffer& WritePropertyName(ByteBuffer& out, const Utf8String& propertyName)
{
    if (propertyName[0] != 0)
        out.Write(propertyName);

    return out;
}

//WriteTextValue
static void WriteTextValue(ByteBuffer& out, bool value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    out.Write(BoolToString(value));
}

static void WriteTextValue(ByteBuffer& out, TextDataChunkBlobWrapper<const uint8_t*>& value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    auto bytesProcessed = out.WriteBase64(value.bytes, value.count, maxBytesToWrite);
    value.bytes += bytesProcessed;
    value.count -= bytesProcessed;
}

static void WriteTextValue(ByteBuffer& out, const char* value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    auto length = strlen(value) - outputByteOffset;
    if (length > maxBytesToWrite)
        length = maxBytesToWrite;

    out.Write(value + outputByteOffset, length);
}

static void WriteTextValue(ByteBuffer& out, const Utf8String& value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    auto length = value.length() - outputByteOffset;
    if (length > maxBytesToWrite)
        length = maxBytesToWrite;

    out.Write(value.c_str() + outputByteOffset, length);
}

static void WriteTextValue(ByteBuffer& out, const TimeDuration& value, DocumentWriterOutput* writerOutput, size_t byteOffset, size_t maxBytesToWrite)
{
    auto valueData = value.ToString();
    auto length = valueData.length() - byteOffset;
    if (length > maxBytesToWrite)
        length = maxBytesToWrite;

    out.Write(valueData.c_str() + byteOffset, length);
}

template <typename T>
void WriteTextValue(ByteBuffer& out, const T& value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    if (sizeof(T) <= maxBytesToWrite)
        out.Write(Convert::ToString(value));
}

static void WriteTextValue(ByteBuffer& out, float value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    if (sizeof(value) <= maxBytesToWrite)
    {
        if (GetAbs(value) < writerOutput->GetMinFloat())
            value = 0;
        out.Write(Convert::ToString(value));
    }
}

static void WriteTextValue(ByteBuffer& out, double value, DocumentWriterOutput* writerOutput, size_t outputByteOffset, size_t maxBytesToWrite)
{
    if (sizeof(value) <= maxBytesToWrite)
    {
        if (GetAbs(value) < writerOutput->GetMinDouble())
            value = 0;
        out.Write(Convert::ToString(value));
    }
}

//WriteTextValues
static void WriteTextValues(ByteBuffer& out, const bool* values, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
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

static void WriteTextValues(ByteBuffer& out, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
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

static void WriteTextValues(ByteBuffer& out, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
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
void WriteTextValues(ByteBuffer& out, const T* values, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
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

        out.Write(Convert::ToString(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

static void WriteTextValues(ByteBuffer& out, const float* values, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, startIndex, valueStride);

        if (GetAbs(value) < writerOutput->GetMinFloat())
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

        out.Write(Convert::ToString(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

static void WriteTextValues(ByteBuffer& out, const double* values, size_t count, DataChunkWriteStride valueStride, DocumentWriterOutput* writerOutput, size_t& startIndex, size_t maxBytesToWrite)
{
    size_t writtenCount = 0;
    for (; startIndex < count; startIndex++)
    {
        auto value = GetStridedValue(values, startIndex, valueStride);

        if (GetAbs(value) < writerOutput->GetMinDouble())
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

        out.Write(Convert::ToString(value));
        maxBytesToWrite -= valueOutputByteSize;
    }
}

//WriteValueLine
template <typename Property, typename T>
void WriteValueLine(DocumentWriterOutput& out, ByteBuffer& lineBuffer, const Property& propertyName, T& value, size_t maxBytesPerLine, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto maxBytesPerLineString = Convert::ToString(maxBytesPerLine);
    if (maxBytesPerLineString.length() >= maxBytesPerLine)
    {
        FINJIN_SET_ERROR(error, "Unable to write value line length for property. The line buffer is too small.");
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
                FINJIN_SET_ERROR(error, "Unable to write value for property. The line buffer is too small.");
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
        out.Write(Convert::ToString(lineBuffer.size()).c_str());
        out.Write(&occurrenceChar, 1);
        out.Write(lineBuffer.data(), lineBuffer.size());

        linesWritten++;
    }
}

template <typename Property, typename T>
void WriteValuesLine(DocumentWriterOutput& out, ByteBuffer& lineBuffer, const Property& propertyName, const T* values, size_t count, DataChunkWriteStride valueStride, size_t maxBytesPerLine, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto maxBytesPerLineString = Convert::ToString(maxBytesPerLine);
    if (maxBytesPerLineString.length() >= maxBytesPerLine)
    {
        FINJIN_SET_ERROR(error, "Unable to write value line length for property. The line buffer is too small.");
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
                auto& value = GetStridedValue(values, startIndex, valueStride);
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to write value for property. The line buffer is too small for value '%1%'.", value));
                return;
            }

            if (linesWritten == 0)
            {
                //'+' indicates first of multiple occurrences
                occurrenceChar = '+';
            }
            else
            {
                //'~' indicates continuation of multiple occurrences
                occurrenceChar = '~';
            }
        }
        else if (linesWritten > 0)
        {
            //'!' indicates last of multiple occurrences
            occurrenceChar = '!';
        }

        lineBuffer.Write("\n");

        //Finally output line length + occurrence char + line buffer
        out.Write(Convert::ToString(lineBuffer.size()).c_str());
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

static bool StringValueRequiresEscape(const Utf8String& value)
{
    return StringValueRequiresEscape(value.c_str());
}

//EscapeStringValue
static Utf8String EscapeStringValue(const char* value)
{
    return Utf8String(strlen(value) * 2, value[0]);
}

static Utf8String EscapeStringValue(const Utf8String& value)
{
    return Utf8String(value.length() * 2, value[0]);
}


//Implementation----------------------------------------------------------------
TextDataChunkWriter::TextDataChunkWriter()
{
}

TextDataChunkWriter::~TextDataChunkWriter()
{
    if (AnySet(this->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(this->style & DataChunkWriterStyle::ROOT))
        WriteFooter();
}

void TextDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, DataChunkWriter* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    this->style = style;
    this->lineBuffer.Create(this->settings.maxBytesPerLine, FINJIN_ALLOCATOR_NULL);
}

DataChunkWriterController& TextDataChunkWriter::GetWriterController()
{
    return *this->settings.controller;
}

DocumentWriterOutput* TextDataChunkWriter::GetWriterOutput()
{
    return this->settings.output;
}

void TextDataChunkWriter::WriteWriterHeader(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WriteString(StandardChunkPropertyNames::FORMAT, "Finjin Streaming Text Document", error);
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

    WriteCount(StandardChunkPropertyNames::MAX_BYTES_PER_LINE, this->settings.maxBytesPerLine, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write max bytes per line.");
        return;
    }

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

void TextDataChunkWriter::WriteChunk(const ChunkName& name, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
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
        auto textChunkWriter = new TextDataChunkWriter();
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

void TextDataChunkWriter::WriteChunkStart(const ChunkName& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto nameString = (name.index != (ChunkName::Index)-1) ? Convert::ToString(name.index) : name.ToString();
    WRITE_CHUNK_START_LINE(this, nameString);
}

void TextDataChunkWriter::WriteChunkEnd(const ChunkName& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_CHUNK_END_LINE(this);
}

void TextDataChunkWriter::WriteFooter()
{
    {
        FINJIN_DECLARE_ERROR(error);

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

    this->settings.output->Write("-");
}

bool TextDataChunkWriter::WillSplitBlob(const ChunkPropertyName& propertyName, const void* values, size_t count)
{
    size_t valueLength = 0;

    switch (this->settings.blobTextFormat)
    {
        case DataChunkBlobTextFormat::BYTE_ARRAY:
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
        case DataChunkBlobTextFormat::BASE64:
        {
            valueLength = Base64::ToBase64Count(count);
            break;
        }
    }

    auto nonSizeHeaderLength = SizeOfValue(propertyName) + 1 + valueLength + 1; //Trailing +1 is for newline
    auto sizeHeaderLength = Convert::ToString(nonSizeHeaderLength).length() + 1;
    auto proposedLength = sizeHeaderLength + nonSizeHeaderLength;
    return proposedLength > this->settings.maxBytesPerLine;
}

bool TextDataChunkWriter::WillSplitString(const ChunkPropertyName& propertyName, const Utf8String& value)
{
    size_t valueLength;
    if (StringValueRequiresEscape(value))
        valueLength = EscapeStringValue(value).length();
    else
        valueLength = value.length();

    auto nonSizeHeaderLength = SizeOfValue(propertyName) + 1 + valueLength + 1; //Trailing +1 is for newline
    auto sizeHeaderLength = Convert::ToString(nonSizeHeaderLength).length() + 1;
    auto proposedLength = sizeHeaderLength + nonSizeHeaderLength;
    return proposedLength > this->settings.maxBytesPerLine;
}

void TextDataChunkWriter::WriteBlob(const ChunkPropertyName& propertyName, const void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitBlob(propertyName, values, count))
    {
        WriteCount(this->settings.lengthHintPropertyName, count, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write length hint.");
            return;
        }
    }

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
    }
}

void TextDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitString(propertyName, value))
    {
        WriteCount(this->settings.lengthHintPropertyName, value.length(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write length hint.");
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

void TextDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const char* value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

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

void TextDataChunkWriter::WriteDateTime(const ChunkPropertyName& propertyName, const DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto s = value.ToIso8601();
    WRITE_VALUE_LINE(this, propertyName, s);
}

void TextDataChunkWriter::WriteTimeDuration(const ChunkPropertyName& propertyName, const TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteUuid(const ChunkPropertyName& propertyName, const Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto s = value.ToString();
    WRITE_VALUE_LINE(this, propertyName, s);
}

void TextDataChunkWriter::WriteBool(const ChunkPropertyName& propertyName, bool value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteCount(const ChunkPropertyName& propertyName, size_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteInt8(const ChunkPropertyName& propertyName, int8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteInt16(const ChunkPropertyName& propertyName, int16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteInt32(const ChunkPropertyName& propertyName, int32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteInt64(const ChunkPropertyName& propertyName, int64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteUInt8(const ChunkPropertyName& propertyName, uint8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteUInt16(const ChunkPropertyName& propertyName, uint16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteUInt32(const ChunkPropertyName& propertyName, uint32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteUInt64(const ChunkPropertyName& propertyName, uint64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteFloat(const ChunkPropertyName& propertyName, float value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteDouble(const ChunkPropertyName& propertyName, double value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void TextDataChunkWriter::WriteStridedStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void TextDataChunkWriter::WriteStridedDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
