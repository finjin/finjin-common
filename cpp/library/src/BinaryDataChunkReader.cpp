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
#include "finjin/common/BinaryDataChunkReader.hpp"
#include "finjin/common/Convert.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
template <typename T>
void ReadNumber(DataChunkReaderInput* readerInput, bool swapBytes, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto amountRead = readerInput->Read(&value, sizeof(T));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(T))
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    else if (swapBytes)
        SwapBytes(value);
}

template <typename T>
size_t ReadNumbers(DataChunkReaderInput* readerInput, bool swapBytes, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = readerInput->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read number array length.");
        return 0;
    }
    else if (swapBytes)
        SwapBytes(length);

    if (length > maxCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% numbers to be parsed.", maxCount));
        return 0;
    }

    if (swapBytes || valueStride.tupleSize != 1 || valueStride.tupleStride != sizeof(T))
    {
        for (BinaryMultiValueSize i = 0; i < length; i++)
        {
            auto& value = GetStridedValue(values, i, valueStride);
            ReadNumber(readerInput, swapBytes, dataHeader, value, typeName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read number at index %1%.", i));
                return 0;
            }
        }
    }
    else
    {
        auto amountRead = readerInput->Read(values, sizeof(T) * length);
        dataHeader.currentOffset += amountRead;
        if (amountRead < sizeof(T) * length)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    }

    return static_cast<size_t>(length);
}

static size_t ReadCounts(DataChunkReaderInput* readerInput, bool swapBytes, DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = readerInput->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read count array length.");
        return 0;
    }
    else if (swapBytes)
        SwapBytes(length);

    if (length > maxCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% counts to be parsed.", maxCount));
        return 0;
    }

    if (swapBytes || valueStride.tupleSize != 1 || valueStride.tupleStride != sizeof(BinaryMultiValueSize))
    {
        for (BinaryMultiValueSize i = 0; i < length; i++)
        {
            BinaryMultiValueSize value;
            ReadNumber(readerInput, swapBytes, dataHeader, value, typeName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count at index %1%.", i));
                return 0;
            }

            GetStridedValue(values, i, valueStride) = value;
        }
    }
    else
    {
        auto amountRead = readerInput->Read(values, sizeof(size_t) * length);
        dataHeader.currentOffset += amountRead;
        if (amountRead < sizeof(size_t) * length)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    }

    return static_cast<size_t>(length);
}

//Implementation----------------------------------------------------------------
BinaryDataChunkReader::BinaryDataChunkReader()
{
}

BinaryDataChunkReader::~BinaryDataChunkReader()
{
}

void BinaryDataChunkReader::Create(const Settings& settings, BinaryDataChunkReader* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    if (parentSection != nullptr)
    {
        this->readerHeader = parentSection->readerHeader;
        this->swapBytes = parentSection != nullptr ? parentSection->swapBytes : false;
    }
}

DataChunkReaderController& BinaryDataChunkReader::GetReaderController()
{
    return *this->settings.controller;
}

DataChunkReaderInput* BinaryDataChunkReader::GetReaderInput()
{
    return this->settings.input;
}

void BinaryDataChunkReader::ReadReaderHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParsedChunkPropertyName key;

    //Read signature---------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::PROPERTY)
    {
        FINJIN_SET_ERROR(error, "Header was in an unexpected format.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read signature header key.");
        return;
    }
    if (key != StandardChunkPropertyNames::SIGNATURE)
    {
        auto signatureKey = key.id;
        SwapBytes(signatureKey);
        if (StandardChunkPropertyNames::SIGNATURE != signatureKey)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected 'signature' key, read '%1%' instead.", key.ToString()));
            return;
        }
        else
        {
            //The signature is byte swapped
            this->swapBytes = !this->swapBytes;
        }
    }

    ReadUInt32(dataHeader, this->readerHeader.signature, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read signature value.");
        return;
    }
    if (this->readerHeader.signature != FINJIN_SIGNATURE_FOURCC)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported signature '%1%'.", this->readerHeader.signature));
        return;
    }

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
}

void BinaryDataChunkReader::ReadChunk(std::unique_ptr<DataChunkReader> reader, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->settings.controller->ScheduleReadChunk(chunkFunc, std::move(reader), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to schedule/execute chunk reader.");
        return;
    }
}

void BinaryDataChunkReader::ReadDataHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    dataHeader.Reset();

    this->settings.input->Read(&dataHeader.headerByte, 1);
    switch ((dataHeader.headerByte & BINARY_LINE_TYPE_MASK) >> BINARY_LINE_TYPE_SHIFT)
    {
        case BINARY_LINE_TYPE_CHUNK_START:
        {
            dataHeader.type = DataHeaderType::CHUNK_START;

            BinaryLineLength lineLength;
            this->settings.input->Read(&lineLength, sizeof(lineLength));
            if (this->swapBytes)
                SwapBytes(lineLength);

            dataHeader.length = lineLength;

            if (dataHeader.headerByte & BINARY_LINE_FLAG_INDEXED)
            {
                //Chunk start has an index
                ReadNumber(this->settings.input, this->swapBytes, dataHeader, dataHeader.index, "chunk index", error);
            }
            else
            {
                //Chunk start has a UUID
                ReadNumber(this->settings.input, false, dataHeader, dataHeader.id, "chunk ID", error);
            }

            break;
        }
        case BINARY_LINE_TYPE_CHUNK_END:
        {
            dataHeader.type = DataHeaderType::CHUNK_END;

            break;
        }
        case BINARY_LINE_TYPE_DOCUMENT_END:
        {
            dataHeader.type = DataHeaderType::END;

            break;
        }
        case BINARY_LINE_TYPE_SINGLE_VALUE:
        case BINARY_LINE_TYPE_MULTI_VALUE:
        {
            dataHeader.type = DataHeaderType::PROPERTY;

            BinaryLineLength lineLength;
            this->settings.input->Read(&lineLength, sizeof(lineLength));
            if (this->swapBytes)
                SwapBytes(lineLength);

            dataHeader.length = lineLength;

            break;
        }
    }

    switch ((dataHeader.headerByte & BINARY_LINE_OCCURRENCE_MASK) >> BINARY_LINE_OCCURRENCE_SHIFT)
    {
        case 0: dataHeader.occurrence = DataHeader::Occurrence::ONE; break;
        case BINARY_LINE_OCCURRENCE_FIRST: dataHeader.occurrence = DataHeader::Occurrence::FIRST; break;
        case BINARY_LINE_OCCURRENCE_MORE: dataHeader.occurrence = DataHeader::Occurrence::MORE; break;
        case BINARY_LINE_OCCURRENCE_LAST: dataHeader.occurrence = DataHeader::Occurrence::LAST; break;
        default: dataHeader.occurrence = DataHeader::Occurrence::NONE; break;
    }
}

void BinaryDataChunkReader::Skip(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (dataHeader.currentOffset < dataHeader.length)
    {
        auto amountToSkip = dataHeader.length - dataHeader.currentOffset;
        auto amountSkipped = this->settings.input->Skip(amountToSkip);

        dataHeader.currentOffset += amountSkipped;

        if (dataHeader.currentOffset != dataHeader.length)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Requested to skip '%1%' bytes but actually skipped '%2%'.", amountToSkip, amountSkipped));
    }
}

const DataChunkReaderHeader& BinaryDataChunkReader::GetHeader() const
{
    return this->readerHeader;
}

ByteOrder BinaryDataChunkReader::GetByteOrder() const
{
    if (this->swapBytes)
        return IsLittleEndian() ? ByteOrder::BIG : ByteOrder::LITTLE;
    else
        return ::GetByteOrder();
}

void BinaryDataChunkReader::ReadPropertyName(DataHeader& dataHeader, ParsedChunkPropertyName& result, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    result.id = (ParsedChunkPropertyName::ID)-1;
    result.index = (ParsedChunkPropertyName::Index)-1;

    if (dataHeader.headerByte & BINARY_LINE_FLAG_INDEXED)
    {
        auto amountRead = this->settings.input->Read(&result.index, sizeof(result.index));
        dataHeader.currentOffset += amountRead;

        if (amountRead < sizeof(result.index))
            FINJIN_SET_ERROR(error, "Failed to read key index.");
        else if (this->swapBytes)
            SwapBytes(result.index);
    }
    else
    {
        auto amountRead = this->settings.input->Read(&result.id, sizeof(result.id));
        dataHeader.currentOffset += amountRead;

        if (amountRead < sizeof(result.id))
            FINJIN_SET_ERROR(error, "Failed to read key ID.");
        else if (this->swapBytes)
            SwapBytes(result.id);
    }
}

size_t BinaryDataChunkReader::ReadBlob(DataHeader& dataHeader, void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = this->settings.input->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read blob length.");
        return 0;
    }
    else if (this->swapBytes)
        SwapBytes(length);

    if (count < static_cast<size_t>(length))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read blob content. Blob is %1% bytes in length but buffer is only %2% bytes in length.", length, count));
        return 0;
    }

    amountRead = this->settings.input->Read(values, static_cast<size_t>(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < static_cast<size_t>(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read blob content.");
        return 0;
    }

    return amountRead;
}

void BinaryDataChunkReader::ReadString(DataHeader& dataHeader, Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = this->settings.input->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read string length.");
        value.clear();
        return;
    }
    else if (this->swapBytes)
        SwapBytes(length);

    if (dataHeader.IsOnlyOrFirstOccurrence())
        value.clear();

    auto offset = value.length();
    if (value.append(static_cast<size_t>(length), ' ').HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
        value.clear();
        return;
    }

    amountRead = this->settings.input->Read(&value[offset], static_cast<size_t>(length));
    value.RepairLength(offset + amountRead);
    dataHeader.currentOffset += amountRead;
    if (amountRead < length)
    {
        FINJIN_SET_ERROR(error, "Failed to read string content.");
        value.clear();
        return;
    }
}

void BinaryDataChunkReader::ReadDateTime(DataHeader& dataHeader, DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryTimeValue timeValue;
    auto amountRead = this->settings.input->Read(&timeValue, sizeof(timeValue));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(timeValue))
    {
        FINJIN_SET_ERROR(error, "Failed to read date/time value.");
        return;
    }
    else if (this->swapBytes)
        SwapBytes(timeValue);

    value = DateTime::FromUnixTimestap(timeValue);
}

void BinaryDataChunkReader::ReadTimeDuration(DataHeader& dataHeader, TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryTimeValue timeValue;
    auto amountRead = this->settings.input->Read(&timeValue, sizeof(timeValue));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(timeValue))
    {
        FINJIN_SET_ERROR(error, "Failed to read time duration value.");
        return;
    }
    else if (this->swapBytes)
        SwapBytes(timeValue);

    value = TimeDuration::Nanoseconds(timeValue);
}

void BinaryDataChunkReader::ReadUuid(DataHeader& dataHeader, Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::array<uint8_t, 16> valueBytes;
    auto amountRead = this->settings.input->Read(&valueBytes, sizeof(valueBytes));
    dataHeader.currentOffset += amountRead;
    if (amountRead < 16)
    {
        FINJIN_SET_ERROR(error, "Failed to read time UUID bytes.");
        return;
    }

    value = Uuid(valueBytes.data());
}

void BinaryDataChunkReader::ReadBool(DataHeader& dataHeader, bool& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    uint8_t boolValue;
    auto amountRead = this->settings.input->Read(&boolValue, 1);
    dataHeader.currentOffset += amountRead;
    if (amountRead < 1)
    {
        FINJIN_SET_ERROR(error, "Failed to read time bool value.");
        return;
    }

    value = boolValue ? true : false;
}

void BinaryDataChunkReader::ReadCount(DataHeader& dataHeader, size_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "size_t", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadInt8(DataHeader& dataHeader, int8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadInt16(DataHeader& dataHeader, int16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadInt32(DataHeader& dataHeader, int32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "int32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadInt64(DataHeader& dataHeader, int64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "int64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadUInt8(DataHeader& dataHeader, uint8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "uint8", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadUInt16(DataHeader& dataHeader, uint16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "uint16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadUInt32(DataHeader& dataHeader, uint32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "uint32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadUInt64(DataHeader& dataHeader, uint64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "uint64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadFloat(DataHeader& dataHeader, float& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "float", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void BinaryDataChunkReader::ReadDouble(DataHeader& dataHeader, double& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(this->settings.input, this->swapBytes, dataHeader, value, "double", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

size_t BinaryDataChunkReader::ReadStridedStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = this->settings.input->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read string array length.");
        return 0;
    }
    else if (this->swapBytes)
        SwapBytes(length);

    if (length > maxCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% strings to be parsed.", maxCount));
        return 0;
    }

    for (BinaryMultiValueSize i = 0; i < length; i++)
    {
        auto& value = GetStridedValue(values, i, valueStride);
        ReadString(dataHeader, value, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string at index %1%.", i));
            return 0;
        }
    }

    return static_cast<size_t>(length);
}

size_t BinaryDataChunkReader::ReadStridedTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = this->settings.input->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read time duration array length.");
        return 0;
    }
    else if (this->swapBytes)
        SwapBytes(length);

    if (length > maxCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% time durations to be parsed.", maxCount));
        return 0;
    }

    for (BinaryMultiValueSize i = 0; i < length; i++)
    {
        auto& value = GetStridedValue(values, i, valueStride);
        ReadTimeDuration(dataHeader, value, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration at index %1%.", i));
            return 0;
        }
    }

    return static_cast<size_t>(length);
}

size_t BinaryDataChunkReader::ReadStridedBools(DataHeader& dataHeader, bool* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize length;
    auto amountRead = this->settings.input->Read(&length, sizeof(length));
    dataHeader.currentOffset += amountRead;
    if (amountRead < sizeof(length))
    {
        FINJIN_SET_ERROR(error, "Failed to read boolean array length.");
        return 0;
    }
    else if (this->swapBytes)
        SwapBytes(length);

    if (length > maxCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% boolean to be parsed.", maxCount));
        return 0;
    }

    for (BinaryMultiValueSize i = 0; i < length; i++)
    {
        auto& value = GetStridedValue(values, i, valueStride);
        ReadBool(dataHeader, value, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean at index %1%.", i));
            return 0;
        }
    }

    return static_cast<size_t>(length);
}

size_t BinaryDataChunkReader::ReadStridedCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ::ReadCounts(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "count collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "int8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "int16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "int32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "int64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "uint8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "uint16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "uint32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "uint64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedFloats(DataHeader& dataHeader, float* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "float collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t BinaryDataChunkReader::ReadStridedDoubles(DataHeader& dataHeader, double* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(this->settings.input, this->swapBytes, dataHeader, values, maxCount, valueStride, "double collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}
