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
#include "finjin/common/ConfigDataChunkReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Base64.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define CHECK_DATA_LINE_NOT_NULL() \
    if (this->documentLine == nullptr) \
    { \
        FINJIN_SET_ERROR(error, "Document line is null."); \
        return; \
    }


//Local functions---------------------------------------------------------------
template <typename T>
void ReadNumber(ConfigDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
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
size_t ReadNumbers(ConfigDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex <= dataHeader.length; charIndex++)
    {
        if (charIndex == dataHeader.length || lineBuffer[charIndex] == ' ')
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
void ReadInteger(ConfigDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
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
size_t ReadIntegers(ConfigDataChunkReader* reader, ByteBuffer& lineBuffer, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex <= dataHeader.length; charIndex++)
    {
        if (charIndex == dataHeader.length || lineBuffer[charIndex] == ' ')
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
ConfigDataChunkReader::ConfigDataChunkReader()
{
    this->settings.input = nullptr;
    this->settings.controller = nullptr;
    this->documentLine = nullptr;
}

ConfigDataChunkReader::~ConfigDataChunkReader()
{
}

void ConfigDataChunkReader::Create(const Settings& settings, ConfigDataChunkReader* parentSection, Error& error)
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
    this->lineBuffer.Create(this->settings.maxBytesPerLine * this->settings.estimatedLineCount, FINJIN_ALLOCATOR_NULL); //The maximum is allowed to be exceeded
    this->lineBuffer.clear();

    this->documentLine = nullptr;
}

DataChunkReaderController& ConfigDataChunkReader::GetReaderController()
{
    return *this->settings.controller;
}

DataChunkReaderInput* ConfigDataChunkReader::GetReaderInput()
{
    return this->settings.input;
}

void ConfigDataChunkReader::ReadReaderHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.input->ReadToEnd(this->lineBuffer) == 0)
    {
        FINJIN_SET_ERROR(error, "Failed to read config document or document is empty.");
        return;
    }

    this->documentLine = this->documentReader.Start(this->lineBuffer);
    if (this->documentLine == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to parse config document start.");
        return;
    }

    ParsedChunkPropertyName key;

    //Optionally read blob text format---------------------------
    if (this->documentLine->GetType() != ConfigDocumentLine::Type::KEY_AND_VALUE)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format header.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format key.");
        return;
    }
    if (key == StandardChunkPropertyNames::BLOB_TEXT_FORMAT)
    {
        Utf8String blobTextFormat;
        ReadString(dataHeader, blobTextFormat, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to read blob text format value.");
            return;
        }
        this->readerHeader.blobTextFormat = DataChunkBlobTextFormatUtilities::Parse(blobTextFormat);
        if (this->readerHeader.blobTextFormat == DataChunkBlobTextFormat::COUNT)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse blob text format value '%1%'.", blobTextFormat));
            return;
        }
    }
    else
    {
        //Set line to null to indicate to ReadDataHeader() that this->documentReader.Current() should be used
        this->documentLine = nullptr;
    }
}

void ConfigDataChunkReader::ReadChunk(std::unique_ptr<DataChunkReader> reader, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->settings.controller->ScheduleReadChunk(chunkFunc, std::move(reader), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to schedule/execute chunk reader.");
        return;
    }
}

void ConfigDataChunkReader::ReadDataHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    dataHeader.Reset();

    auto done = false;
    while (!done)
    {
        if (this->documentLine == nullptr)
            this->documentLine = this->documentReader.Current();
        else
            this->documentLine = this->documentReader.Next();

        if (this->documentLine == nullptr)
        {
            //The end
            dataHeader.type = DataHeaderType::END;
            break;
        }

        switch (this->documentLine->GetType())
        {
            case ConfigDocumentLine::Type::COMMENT:
            {
                break;
            }
            case ConfigDocumentLine::Type::SECTION:
            {
                dataHeader.name.assign(this->documentLine->GetSectionName());
                dataHeader.type = DataHeaderType::CHUNK_START;
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                if (dataHeader.type != DataHeaderType::CHUNK_START)
                {
                    FINJIN_SET_ERROR(error, "Unexpectedly encountered a scope start in config document.");
                    return;
                }
                done = true;
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_END:
            {
                dataHeader.type = DataHeaderType::CHUNK_END;
                done = true;
                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                dataHeader.type = DataHeaderType::PROPERTY;
                dataHeader.occurrence = DataHeader::Occurrence::ONE;
                done = true;
                break;
            }
            default: break;
        }
    }
}

void ConfigDataChunkReader::Skip(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Just reset the offset and length. No actual skipping is performed
    dataHeader.currentOffset = dataHeader.length = 0;
}

const DataChunkReaderHeader& ConfigDataChunkReader::GetHeader() const
{
    return this->readerHeader;
}

void ConfigDataChunkReader::ReadPropertyName(DataHeader& dataHeader, ParsedChunkPropertyName& result, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    CHECK_DATA_LINE_NOT_NULL()

    auto readString = this->documentLine->GetKey();
    result.Parse(readString);

    //Set up for subsequent value read
    Utf8StringView valuePointer;
    this->documentLine->GetValue(valuePointer);
    dataHeader.currentOffset = valuePointer.begin() - reinterpret_cast<const char*>(this->lineBuffer.data());
    dataHeader.length = valuePointer.end() - reinterpret_cast<const char*>(this->lineBuffer.data());
}

size_t ConfigDataChunkReader::ReadBlob(DataHeader& dataHeader, void* values, size_t count, Error& error)
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
            auto base64ByteCount = dataHeader.length - dataHeader.currentOffset;
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

void ConfigDataChunkReader::ReadString(DataHeader& dataHeader, Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView valuePointer;
    this->documentLine->GetValue(valuePointer);

    if (value.assign(valuePointer.begin(), valuePointer.end()).HasError())
        FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
}

void ConfigDataChunkReader::ReadDateTime(DataHeader& dataHeader, DateTime& value, Error& error)
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

void ConfigDataChunkReader::ReadTimeDuration(DataHeader& dataHeader, TimeDuration& value, Error& error)
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

void ConfigDataChunkReader::ReadUuid(DataHeader& dataHeader, Uuid& value, Error& error)
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

void ConfigDataChunkReader::ReadBool(DataHeader& dataHeader, bool& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for boolean.");
    else
        value = Convert::ToBool(tempValueString);
}

void ConfigDataChunkReader::ReadCount(DataHeader& dataHeader, size_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "size_t", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadInt8(DataHeader& dataHeader, int8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadInt16(DataHeader& dataHeader, int16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadInt32(DataHeader& dataHeader, int32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadInt64(DataHeader& dataHeader, int64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "int64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadUInt8(DataHeader& dataHeader, uint8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint8", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadUInt16(DataHeader& dataHeader, uint16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadUInt32(DataHeader& dataHeader, uint32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadUInt64(DataHeader& dataHeader, uint64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(this, this->lineBuffer, dataHeader, value, "uint64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadFloat(DataHeader& dataHeader, float& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this, this->lineBuffer, dataHeader, value, "float", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void ConfigDataChunkReader::ReadDouble(DataHeader& dataHeader, double& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this, this->lineBuffer, dataHeader, value, "double", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

size_t ConfigDataChunkReader::ReadStridedStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex <= dataHeader.length; charIndex++)
    {
        if (charIndex == dataHeader.length || this->lineBuffer[charIndex] == ' ')
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

size_t ConfigDataChunkReader::ReadStridedTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex <= dataHeader.length; charIndex++)
    {
        if (charIndex == dataHeader.length || this->lineBuffer[charIndex] == ' ')
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

size_t ConfigDataChunkReader::ReadStridedBools(DataHeader& dataHeader, bool* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    Utf8String tempValueString;

    auto previousCharIndex = dataHeader.currentOffset - 1;
    for (size_t charIndex = dataHeader.currentOffset; charIndex <= dataHeader.length; charIndex++)
    {
        if (charIndex == dataHeader.length || this->lineBuffer[charIndex] == ' ')
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

size_t ConfigDataChunkReader::ReadStridedCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "count collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "int64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "uint64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedFloats(DataHeader& dataHeader, float* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "float collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t ConfigDataChunkReader::ReadStridedDoubles(DataHeader& dataHeader, double* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this, this->lineBuffer, dataHeader, values, maxCount, valueStride, "double collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}
