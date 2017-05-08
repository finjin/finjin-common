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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/DataChunkReader.hpp"
#include "finjin/common/DataChunkReaderController.hpp"
#include "finjin/common/Error.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API TextDataChunkReader : public DataChunkReader
    {
    public:
        TextDataChunkReader(const TextDataChunkReader& other) = delete;
        TextDataChunkReader& operator = (const TextDataChunkReader& other) = delete;

        TextDataChunkReader(TextDataChunkReader&& other) = delete;
        TextDataChunkReader& operator = (TextDataChunkReader&& other) = delete;

    public:
        TextDataChunkReader();
        ~TextDataChunkReader();

        void Create(const Settings& settings, TextDataChunkReader* parentSection, Error& error);

        DataChunkReaderController& GetReaderController() override;

        DataChunkReaderInput* GetReaderInput() override;

        void ReadReaderHeader(DataHeader& dataHeader, Error& error) override;
        void ReadChunk(std::unique_ptr<DataChunkReader> reader, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error) override;
        void ReadDataHeader(DataHeader& dataHeader, Error& error) override;
        void Skip(DataHeader& dataHeader, Error& error) override;

        const DataChunkReaderHeader& GetHeader() const override;

        bool IsBinaryFormat() const override { return false; }

        void ReadPropertyName(DataHeader& dataHeader, ParsedChunkPropertyName& result, Error& error) override;

        size_t ReadBlob(DataHeader& dataHeader, void* values, size_t count, Error& error) override;
        void ReadString(DataHeader& dataHeader, Utf8String& value, Error& error) override;
        void ReadDateTime(DataHeader& dataHeader, DateTime& value, Error& error) override;
        void ReadTimeDuration(DataHeader& dataHeader, TimeDuration& value, Error& error) override;
        void ReadUuid(DataHeader& dataHeader, Uuid& value, Error& error) override;
        void ReadBool(DataHeader& dataHeader, bool& value, Error& error) override;
        void ReadCount(DataHeader& dataHeader, size_t& value, Error& error) override;
        void ReadInt8(DataHeader& dataHeader, int8_t& value, Error& error) override;
        void ReadInt16(DataHeader& dataHeader, int16_t& value, Error& error) override;
        void ReadInt32(DataHeader& dataHeader, int32_t& value, Error& error) override;
        void ReadInt64(DataHeader& dataHeader, int64_t& value, Error& error) override;
        void ReadUInt8(DataHeader& dataHeader, uint8_t& value, Error& error) override;
        void ReadUInt16(DataHeader& dataHeader, uint16_t& value, Error& error) override;
        void ReadUInt32(DataHeader& dataHeader, uint32_t& value, Error& error) override;
        void ReadUInt64(DataHeader& dataHeader, uint64_t& value, Error& error) override;
        void ReadFloat(DataHeader& dataHeader, float& value, Error& error) override;
        void ReadDouble(DataHeader& dataHeader, double& value, Error& error) override;

        size_t ReadStridedStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedBools(DataHeader& dataHeader, bool* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedFloats(DataHeader& dataHeader, float* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;
        size_t ReadStridedDoubles(DataHeader& dataHeader, double* values, size_t maxCount, DataChunkReadStride valueStride, Error& error) override;

    private:
        Settings settings;

        DataChunkReaderHeader readerHeader;

        ByteBuffer lineBuffer;
    };

} }
