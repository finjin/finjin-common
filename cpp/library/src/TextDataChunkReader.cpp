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
#include "finjin/common/TextDataChunkReader.hpp"
#include "finjin/common/Base64.hpp"
#include "finjin/common/Convert.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static bool StringValueRequiresUnescape(const char* value, size_t length = (size_t)-1)
{
    //Need to unescape:
    //A string of two or more "{"s
    //A string of two or more "}"s

    if (value == nullptr || value[0] == 0 || value[1] == 0)
        return false;

    if (length == (size_t)-1)
        length = strlen(value);

    auto firstChar = value[0];
    for (size_t i = 0; i < length; i++)
    {
        auto c = value[i];
        if (c != '{' && c != '}')
            return false;
        if (c != firstChar)
            return false;
    }

    return true;
}

static size_t UnescapeStringValue(char* value, size_t length)
{
    auto c = value[0];

    for (size_t i = 0; i < length / 2; i++)
        value[i] = c;

    return length / 2;
}

template <typename T>
void ReadNumber(TextDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    reader->ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    else
    {
        Convert::ToNumber(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, tempValueString));
    }
}

template <typename T>
size_t ReadNumbers(TextDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    auto nonNewlineLength = dataHeader.length - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex < dataHeader.length; charIndex++)
    {
        if (charIndex == nonNewlineLength || lineBuffer[charIndex] == ' ')
        {
            if (count >= maxCount)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% numbers to be parsed.", maxCount));
                return 0;
            }

            tempValueString.assign(&lineBuffer[previousCharIndex + 1], &lineBuffer[charIndex]);

            T value;
            Convert::ToNumber(value, tempValueString, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, tempValueString));
                return 0;
            }
            else
                GetStridedValue(values, count++, valueStride) = value;

            previousCharIndex = charIndex;
        }
    }

    return count;
}

template <typename T>
void ReadInteger(TextDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    reader->ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    else
    {
        Convert::ToInteger(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, tempValueString));
    }
}

template <typename T>
size_t ReadIntegers(TextDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    auto nonNewlineLength = dataHeader.length - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex < dataHeader.length; charIndex++)
    {
        if (charIndex == nonNewlineLength || lineBuffer[charIndex] == ' ')
        {
            if (count >= maxCount)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% integers to be parsed.", maxCount));
                return 0;
            }

            tempValueString.assign(&lineBuffer[previousCharIndex + 1], &lineBuffer[charIndex]);

            T value;
            Convert::ToInteger(value, tempValueString, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, tempValueString));
                return 0;
            }
            else
                GetStridedValue(values, count++, valueStride) = value;

            previousCharIndex = charIndex;
        }
    }

    return count;
}


//Implementation----------------------------------------------------------------
TextDataChunkReader::TextDataChunkReader()
{
    this->settings.input = nullptr;
    this->settings.controller = nullptr;
}

TextDataChunkReader::~TextDataChunkReader()
{
}

void TextDataChunkReader::Create(const Settings& settings, TextDataChunkReader* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    if (parentSection != nullptr)
        this->readerHeader = parentSection->readerHeader;
    this->lineBuffer.Create(this->settings.maxBytesPerLine, FINJIN_ALLOCATOR_NULL);
}

DataChunkReaderController& TextDataChunkReader::GetReaderController()
{
    return *this->settings.controller;
}

DataChunkReaderInput* TextDataChunkReader::GetReaderInput()
{
    return this->settings.input;
}

void TextDataChunkReader::ReadReaderHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParsedChunkPropertyName key;

    //Read format---------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::PROPERTY)
    {
        FINJIN_SET_ERROR(error, "Format header was in an unexpected format.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format header key.");
        return;
    }
    if (key != StandardChunkPropertyNames::FORMAT)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected 'format' key, read '%1%' instead.", key.ToString()));
        return;
    }

    ReadString(dataHeader, this->readerHeader.format, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format header value.");
        return;
    }

    //Read version---------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read version header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::PROPERTY)
    {
        FINJIN_SET_ERROR(error, "Version header was in an unexpected format.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format version header key.");
        return;
    }
    if (key != StandardChunkPropertyNames::FORMAT_VERSION)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected 'format-version' key, read '%1%' instead.", key.ToString()));
        return;
    }

    ReadUInt32(dataHeader, this->readerHeader.version, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format version header value.");
        return;
    }
    if (this->readerHeader.version != 1)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported version number '%1%'.", this->readerHeader.version));
        return;
    }

    //Read max bytes per line---------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read max bytes per line header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::PROPERTY)
    {
        FINJIN_SET_ERROR(error, "Max bytes per line header was in an unexpected format.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read max bytes per line header key.");
        return;
    }
    if (key != StandardChunkPropertyNames::MAX_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected 'max-bytes-per-line' key, read '%1%' instead.", key.ToString()));
        return;
    }

    ReadCount(dataHeader, this->readerHeader.maxBytesPerLine, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read max bytes per line header value.");
        return;
    }

    //Read blob text format---------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::PROPERTY)
    {
        FINJIN_SET_ERROR(error, "Blob text format header was in an unexpected format.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format key.");
        return;
    }
    if (key != StandardChunkPropertyNames::BLOB_TEXT_FORMAT)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected 'blob-text-format' key, read '%1%' instead.", key.ToString()));
        return;
    }

    Utf8String blobTextFormat;
    ReadString(dataHeader, blobTextFormat, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format value.");
        return;
    }
    this->readerHeader.blobTextFormat = DataChunkBlobTextFormatUtilities::Parse(blobTextFormat);
}

void TextDataChunkReader::ReadChunk(std::unique_ptr<DataChunkReader> reader, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->settings.controller->ScheduleReadChunk(chunkFunc, std::move(reader), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to schedule/execute chunk reader.");
        return;
    }
}

void TextDataChunkReader::ReadDataHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    dataHeader.Reset();

    //Read length, but be ready for the end
    char c = 0;
    while (this->settings.input->IsValid() && dataHeader.currentOffset < this->lineBuffer.size())
    {
        this->settings.input->Read(&c, 1);
        this->lineBuffer[dataHeader.currentOffset++] = c;

        if (c == '-' || c == ' ' || c == '+' || c == '~' || c == '!')
            break;
    }
    if (dataHeader.currentOffset == this->lineBuffer.size())
    {
        FINJIN_SET_ERROR(error, "Failed to read data header. Reached end of line buffer while reading size.");
        return;
    }

    if (c == '-')
    {
        //The end
        dataHeader.type = DataHeaderType::END;
    }
    else
    {
        Utf8String tempValueString(&this->lineBuffer[0], &this->lineBuffer[dataHeader.currentOffset - 1]);
        dataHeader.length = Convert::ToInteger(tempValueString, dataHeader.length);
        if (dataHeader.currentOffset + dataHeader.length > this->lineBuffer.size())
        {
            FINJIN_SET_ERROR(error, "Failed to read data header line. Line buffer is too small.");
            return;
        }
        this->settings.input->Read(&this->lineBuffer[dataHeader.currentOffset], dataHeader.length);

        dataHeader.length += dataHeader.currentOffset;

        if (dataHeader.length >= 4 && //Needs to be at least 4 bytes to contain minimum-1-character name, ' ', '{', and '\n'
            this->lineBuffer[dataHeader.length - 3] == ' ' &&
            this->lineBuffer[dataHeader.length - 2] == '{' &&
            this->lineBuffer[dataHeader.length - 1] == '\n')
        {
            dataHeader.type = DataHeaderType::CHUNK_START;
            dataHeader.name.assign(&this->lineBuffer[dataHeader.currentOffset], &this->lineBuffer[dataHeader.length - 3]);
            Skip(dataHeader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Successfully read chunk start but failed to skip past it.");
                return;
            }
        }
        else if (dataHeader.length == 4 && //Needs four bytes to be "2 }\n"
            this->lineBuffer[dataHeader.currentOffset] == '}' &&
            this->lineBuffer[dataHeader.currentOffset + 1] == '\n')
        {
            dataHeader.type = DataHeaderType::CHUNK_END;
            Skip(dataHeader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Successfully read chunk end but failed to skip past it.");
                return;
            }
        }
        else
        {
            dataHeader.type = DataHeaderType::PROPERTY;

            switch (c)
            {
                case ' ': dataHeader.occurrence = DataHeader::Occurrence::ONE; break;
                case '+': dataHeader.occurrence = DataHeader::Occurrence::FIRST; break;
                case '~': dataHeader.occurrence = DataHeader::Occurrence::MORE; break;
                case '!': dataHeader.occurrence = DataHeader::Occurrence::LAST; break;
            }
        }
    }
}

void TextDataChunkReader::Skip(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    dataHeader.currentOffset = dataHeader.length;
}

const DataChunkReaderHeader& TextDataChunkReader::GetHeader() const
{
    return this->readerHeader;
}

void TextDataChunkReader::ReadPropertyName(DataHeader& dataHeader, ParsedChunkPropertyName& result, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String readString;
    char c = 0;
    while (dataHeader.currentOffset < dataHeader.length)
    {
        c = this->lineBuffer[dataHeader.currentOffset++];

        if (c == ' ')
            break;

        readString += c;
    }

    if (dataHeader.currentOffset >= dataHeader.length)
    {
        FINJIN_SET_ERROR(error, "Failed to read key.");
        return;
    }

    result.Parse(readString);
}

size_t TextDataChunkReader::ReadBlob(DataHeader& dataHeader, void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t result = 0;

    switch (this->readerHeader.blobTextFormat)
    {
        case DataChunkBlobTextFormat::BYTE_ARRAY:
        {
            result = ReadUInt8s(dataHeader, static_cast<uint8_t*>(values), count, error);
            if (error)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error);
                return 0;
            }

            break;
        }
        case DataChunkBlobTextFormat::BASE64:
        {
            auto base64ByteCount = dataHeader.length - dataHeader.currentOffset - 1;
            result = Base64::ToBytes(reinterpret_cast<const char*>(&this->lineBuffer[dataHeader.currentOffset]), base64ByteCount, values, count, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read value for blob.");
                return 0;
            }

            break;
        }
    }

    return result;
}

void TextDataChunkReader::ReadString(DataHeader& dataHeader, Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto length = dataHeader.length - dataHeader.currentOffset - 1; //Leave off the trailing newline

    if (dataHeader.IsOnlyOrFirstOccurrence())
        value.clear();

    auto offset = value.length();
    if (value.append(reinterpret_cast<const char*>(&this->lineBuffer[dataHeader.currentOffset]), length).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
        return;
    }
    dataHeader.currentOffset += length + 1; //Skip past newline

    if (StringValueRequiresUnescape(&value[offset], length))
    {
        auto newLength = UnescapeStringValue(&value[offset], length);
        value.RepairLength(value.length() - (length - newLength));
    }
}

void TextDataChunkReader::ReadDateTime(DataHeader& dataHeader, DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for ISO-8601 date/time.");
    else
    {
        DateTime::ParseIso8601(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse ISO-8601 date/time value for %1%.", tempValueString));
    }
}

void TextDataChunkReader::ReadTimeDuration(DataHeader& dataHeader, TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for time duration.");
    else
    {
        TimeDuration::Parse(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse time duration value for %1%.", tempValueString));
    }
}

void TextDataChunkReader::ReadUuid(DataHeader& dataHeader, Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for UUID.");
    else
    {
        Uuid::Parse(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse UUID value for %1%.", tempValueString));
    }
}

void TextDataChunkReader::ReadBool(DataHeader& dataHeader, bool& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for boolean.");
    else
        value = Convert::ToBool(tempValueString);
}

void TextDataChunkReader::ReadCount(DataHeader& dataHeader, size_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "size_t", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadInt8(DataHeader& dataHeader, int8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadInt16(DataHeader& dataHeader, int16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadInt32(DataHeader& dataHeader, int32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadInt64(DataHeader& dataHeader, int64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadUInt8(DataHeader& dataHeader, uint8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint8", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadUInt16(DataHeader& dataHeader, uint16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadUInt32(DataHeader& dataHeader, uint32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadUInt64(DataHeader& dataHeader, uint64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadFloat(DataHeader& dataHeader, float& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this, this->lineBuffer, dataHeader, value, "float", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TextDataChunkReader::ReadDouble(DataHeader& dataHeader, double& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this, this->lineBuffer, dataHeader, value, "double", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

size_t TextDataChunkReader::ReadStridedStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    auto nonNewlineLength = dataHeader.length - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex < dataHeader.length; charIndex++)
    {
        if (charIndex == nonNewlineLength || this->lineBuffer[charIndex] == ' ')
        {
            if (count >= maxCount)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% strings to be parsed.", maxCount));
                return 0;
            }

            auto& value = GetStridedValue(values, count++, valueStride);
            if (value.assign(&this->lineBuffer[previousCharIndex + 1], &this->lineBuffer[charIndex]).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
                return 0;
            }

            previousCharIndex = charIndex;
        }
    }

    return count;
}

size_t TextDataChunkReader::ReadStridedTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    auto nonNewlineLength = dataHeader.length - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex < dataHeader.length; charIndex++)
    {
        if (charIndex == nonNewlineLength || this->lineBuffer[charIndex] == ' ')
        {
            if (count >= maxCount)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% time durations to be parsed.", maxCount));
                return 0;
            }

            tempValueString.assign(&this->lineBuffer[previousCharIndex + 1], &this->lineBuffer[charIndex]);
            TimeDuration::Parse(GetStridedValue(values, count++, valueStride), tempValueString, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse time duration at index %1%.", count - 1));
                return 0;
            }

            previousCharIndex = charIndex;
        }
    }

    return count;
}

size_t TextDataChunkReader::ReadStridedBools(DataHeader& dataHeader, bool* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    auto nonNewlineLength = dataHeader.length - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex < dataHeader.length; charIndex++)
    {
        if (charIndex == nonNewlineLength || this->lineBuffer[charIndex] == ' ')
        {
            if (count >= maxCount)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% booleans to be parsed.", maxCount));
                return 0;
            }

            tempValueString.assign(&this->lineBuffer[previousCharIndex + 1], &this->lineBuffer[charIndex]);
            GetStridedValue(values, count++, valueStride) = Convert::ToBool(tempValueString);

            previousCharIndex = charIndex;
        }
    }

    return count;
}

size_t TextDataChunkReader::ReadStridedCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "count collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedFloats(DataHeader& dataHeader, float* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "float collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t TextDataChunkReader::ReadStridedDoubles(DataHeader& dataHeader, double* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "double collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}
