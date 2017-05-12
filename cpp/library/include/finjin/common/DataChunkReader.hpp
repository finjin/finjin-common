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


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/ChunkName.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/Uuid.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class DataHeaderType
    {
        NONE,
        CHUNK_START,
        CHUNK_END,
        PROPERTY,
        END
    };

    class FINJIN_COMMON_LIBRARY_API DataHeader
    {
    public:
        DataHeader();

        void Reset();

        ParsedChunkName GetChunkStart();

        bool IsOnlyOrFirstOccurrence() const;
        bool IsOnlyOrLastOccurrence() const;
        bool IsSecondOrLaterOccurrence() const;

        enum class Occurrence
        {
            NONE,
            ONE,
            FIRST,
            MORE,
            LAST
        };

        struct OccurrenceUtilities
        {
            static const char* ToString(Occurrence value);
        };

        uint8_t headerByte; //Only used with binary files
        Occurrence occurrence; //Only used on properties/values
        DataHeaderType type; //Data item type
        size_t length; //Data item length in bytes
        size_t currentOffset; //Current offset within the data item
        Utf8String name; //Chunk name. Only used with text files. May be a name, name + ID ("name|ID"), or index
        Uuid id; //Chunk id. Only used with binary files
        ParsedChunkName::Index index; //Chunk index. Only used with binary files
    };

    class DataChunkReaderController;
    class DataChunkReaderInput;

    struct DataChunkReaderHeader
    {
        DataChunkReaderHeader()
        {
            this->signature = 0;
            this->version = 0;
            this->maxBytesPerLine = 0;
            this->blobTextFormat = DataChunkBlobTextFormat::BYTE_ARRAY;
        }

        uint32_t signature; //Only used with binary files
        Utf8String format;
        uint32_t version;
        size_t maxBytesPerLine; //The maximum bytes per line. This value may exceed the amount of content in the largest line. If 0 (as with JSON reader/writer), then the maximum is unknown
        DataChunkBlobTextFormat blobTextFormat;
    };

    struct DataChunkReadStride
    {
        DataChunkReadStride()
        {
            this->tupleSize = 0;
            this->tupleStride = 0;
        }

        DataChunkReadStride(size_t tupleSize, size_t tupleStride)
        {
            this->tupleSize = tupleSize;
            this->tupleStride = tupleStride;
        }

        size_t tupleSize; //Number of values (not bytes) next to each other
        size_t tupleStride; //Distance in bytes between a set of tupleSize items, and the next set of tupleSize items
    };

    class FINJIN_COMMON_LIBRARY_API DataChunkReader
    {
    public:
        struct Settings
        {
            enum { MIN_BYTES_PER_LINE = 768 };
            enum { DEFAULT_MAX_BYTES_PER_LINE = MemorySize::KIBIBYTE * 10 };

            Settings()
            {
                this->maxBytesPerLine = DEFAULT_MAX_BYTES_PER_LINE;
                this->input = nullptr;
                this->controller = nullptr;
                this->estimatedLineCount = 1;
            }

            void Create(std::shared_ptr<DataChunkReaderInput> inPtr, DataChunkReaderController& controller)
            {
                this->inputPtr = inPtr;
                this->input = inPtr.get();
                this->controller = &controller;
            }

            void Create(DataChunkReaderInput& in, DataChunkReaderController& controller)
            {
                this->inputPtr = nullptr;
                this->input = &in;
                this->controller = &controller;
            }

            void Create(std::shared_ptr<DataChunkReaderInput> inPtr, DataChunkReaderInput& in, DataChunkReaderController& controller)
            {
                this->inputPtr = inPtr;
                this->input = &in;
                this->controller = &controller;
            }

            size_t maxBytesPerLine;
            size_t estimatedLineCount;
            std::shared_ptr<DataChunkReaderInput> inputPtr;
            DataChunkReaderInput* input;
            DataChunkReaderController* controller;
        };

        virtual ~DataChunkReader();

        virtual DataChunkReaderController& GetReaderController() = 0;

        virtual DataChunkReaderInput* GetReaderInput() = 0;

        virtual void ReadReaderHeader(DataHeader& dataHeader, Error& error) = 0;
        virtual void ReadChunk(std::unique_ptr<DataChunkReader> reader, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error) = 0;
        virtual void ReadDataHeader(DataHeader& dataHeader, Error& error) = 0;
        virtual void Skip(DataHeader& dataHeader, Error& error) = 0;

        virtual const DataChunkReaderHeader& GetHeader() const = 0;

        virtual bool IsBinaryFormat() const = 0;

        virtual void ReadPropertyName(DataHeader& dataHeader, ParsedChunkPropertyName& result, Error& error) = 0;

        virtual size_t ReadBlob(DataHeader& dataHeader, void* values, size_t count, Error& error) = 0;
        virtual void ReadString(DataHeader& dataHeader, Utf8String& value, Error& error) = 0;
        virtual void ReadDateTime(DataHeader& dataHeader, DateTime& value, Error& error) = 0;
        virtual void ReadTimeDuration(DataHeader& dataHeader, TimeDuration& value, Error& error) = 0;
        virtual void ReadUuid(DataHeader& dataHeader, Uuid& value, Error& error) = 0;
        virtual void ReadBool(DataHeader& dataHeader, bool& value, Error& error) = 0;
        virtual void ReadCount(DataHeader& dataHeader, size_t& value, Error& error) = 0;
        virtual void ReadInt8(DataHeader& dataHeader, int8_t& value, Error& error) = 0;
        virtual void ReadInt16(DataHeader& dataHeader, int16_t& value, Error& error) = 0;
        virtual void ReadInt32(DataHeader& dataHeader, int32_t& value, Error& error) = 0;
        virtual void ReadInt64(DataHeader& dataHeader, int64_t& value, Error& error) = 0;
        virtual void ReadUInt8(DataHeader& dataHeader, uint8_t& value, Error& error) = 0;
        virtual void ReadUInt16(DataHeader& dataHeader, uint16_t& value, Error& error) = 0;
        virtual void ReadUInt32(DataHeader& dataHeader, uint32_t& value, Error& error) = 0;
        virtual void ReadUInt64(DataHeader& dataHeader, uint64_t& value, Error& error) = 0;
        virtual void ReadFloat(DataHeader& dataHeader, float& value, Error& error) = 0;
        virtual void ReadDouble(DataHeader& dataHeader, double& value, Error& error) = 0;

        virtual size_t ReadStridedStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedBools(DataHeader& dataHeader, bool* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedFloats(DataHeader& dataHeader, float* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;
        virtual size_t ReadStridedDoubles(DataHeader& dataHeader, double* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) = 0;

        size_t ReadStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, Error& error);
        size_t ReadTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, Error& error);
        size_t ReadBools(DataHeader& dataHeader, bool* values, size_t maxCount, Error& error);
        size_t ReadCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, Error& error);
        size_t ReadInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, Error& error);
        size_t ReadInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, Error& error);
        size_t ReadInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, Error& error);
        size_t ReadInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, Error& error);
        size_t ReadUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, Error& error);
        size_t ReadUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, Error& error);
        size_t ReadUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, Error& error);
        size_t ReadUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, Error& error);
        size_t ReadFloats(DataHeader& dataHeader, float* values, size_t maxCount, Error& error);
        size_t ReadDoubles(DataHeader& dataHeader, double* values, size_t maxCount, Error& error);

        void DebugRead(std::ostream& out, DataHeader& dataHeader, Error& error);
    };

    template <typename T>
    T& GetStridedValue(T* values, size_t index, DataChunkReadStride stride)
    {
        auto tupleIndex = index / stride.tupleSize;
        auto indexInTuple = index % stride.tupleSize;
        return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(values) + tupleIndex * stride.tupleStride + indexInTuple * sizeof(T));
    }

} }
