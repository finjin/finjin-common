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
#include "finjin/common/DataChunkReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DataChunkReaderController.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//DataHeader
DataHeader::DataHeader()
{
    Reset();
}

void DataHeader::Reset()
{
    this->headerByte = 0;
    this->occurrence = Occurrence::NONE;
    this->type = DataHeaderType::NONE;
    this->length = 0;
    this->currentOffset = 0;
    this->name.clear();
    this->id = Uuid::Zero();
    this->index = (ParsedChunkName::Index)-1;
}

ParsedChunkName DataHeader::GetChunkStart()
{
    ParsedChunkName result;
    if (!this->id.IsZero())
        result.id = this->id;
    else if (this->index != (ParsedChunkName::Index)-1)
        result.index = this->index;
    else
        result.Parse(this->name);
    return result;
}

bool DataHeader::IsOnlyOrFirstOccurrence() const
{
    return this->occurrence == Occurrence::ONE || this->occurrence == Occurrence::FIRST;
}

bool DataHeader::IsOnlyOrLastOccurrence() const
{
    return this->occurrence == Occurrence::ONE || this->occurrence == Occurrence::LAST;
}

bool DataHeader::IsSecondOrLaterOccurrence() const
{
    return this->occurrence == Occurrence::MORE || this->occurrence == Occurrence::LAST;
}

//DataChunkReader
DataChunkReader::~DataChunkReader()
{
}

size_t DataChunkReader::ReadStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedStrings(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedTimeDurations(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadBools(DataHeader& dataHeader, bool* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedBools(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedCounts(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedInt8s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedInt16s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedInt32s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedInt64s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedUInt8s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedUInt16s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedUInt32s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedUInt64s(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadFloats(DataHeader& dataHeader, float* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedFloats(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t DataChunkReader::ReadDoubles(DataHeader& dataHeader, double* values, size_t maxCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    auto result = ReadStridedDoubles(dataHeader, values, maxCount, DataChunkReadStride(1, sizeof(values[0])), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

void DataChunkReader::DebugRead(std::ostream& out, DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (dataHeader.type)
    {
        case DataHeaderType::CHUNK_START:
        {
            auto parsedChunk = dataHeader.GetChunkStart();
            out << "Encountered chunk start: " << parsedChunk.ToString() << std::endl;
            break;
        }
        case DataHeaderType::CHUNK_END:
        {
            out << "Encountered chunk end." << std::endl;
            break;
        }
        case DataHeaderType::PROPERTY:
        {
            ParsedChunkPropertyName propertyName;
            ReadPropertyName(dataHeader, propertyName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read property name.");
            }
            else
            {
                out << "Property name: " << propertyName.name << std::endl;
                Skip(dataHeader, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to skip property.");
                }
            }

            break;
        }
        default: break;
    }
}
