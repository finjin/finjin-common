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
#include "finjin/common/ConfigDocumentWriter.hpp"
#include "finjin/common/DataChunkWriter.hpp"
#include "finjin/common/DataChunkWriterController.hpp"
#include <ostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API ConfigDataChunkWriter : public DataChunkWriter
    {
    public:
        ConfigDataChunkWriter(const ConfigDataChunkWriter& other) = delete;
        ConfigDataChunkWriter& operator = (const ConfigDataChunkWriter& other) = delete;

        ConfigDataChunkWriter(ConfigDataChunkWriter&& other) = delete;
        ConfigDataChunkWriter& operator = (ConfigDataChunkWriter&& other) = delete;

    public:
        ConfigDataChunkWriter();
        ~ConfigDataChunkWriter();

        void Create(const Settings& settings, DataChunkWriterStyle style, DataChunkWriter* parentSection, Error& error) override;

        DataChunkWriterController& GetWriterController() override;

        DocumentWriterOutput* GetWriterOutput() override;

        void WriteWriterHeader(Error& error) override;
        void WriteChunk(const ChunkName& name, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error) override;
        void WriteChunkStart(const ChunkName& name, Error& error) override;
        void WriteChunkEnd(const ChunkName& name, Error& error) override;
        void WriteFooter() override;

        bool IsBinaryFormat() const override { return false; }
        ByteOrder GetByteOrder() const override { return this->settings.byteOrder; }

        void WriteBlob(const ChunkPropertyName& propertyName, const void* values, size_t count, Error& error) override;
        void WriteString(const ChunkPropertyName& propertyName, const Utf8String& value, Error& error) override;
        void WriteString(const ChunkPropertyName& propertyName, const char* value, Error& error) override;
        void WriteDateTime(const ChunkPropertyName& propertyName, const DateTime& value, Error& error) override;
        void WriteTimeDuration(const ChunkPropertyName& propertyName, const TimeDuration& value, Error& error) override;
        void WriteUuid(const ChunkPropertyName& propertyName, const Uuid& value, Error& error) override;
        void WriteBool(const ChunkPropertyName& propertyName, bool value, Error& error) override;
        void WriteCount(const ChunkPropertyName& propertyName, size_t value, Error& error) override;
        void WriteInt8(const ChunkPropertyName& propertyName, int8_t value, Error& error) override;
        void WriteInt16(const ChunkPropertyName& propertyName, int16_t value, Error& error) override;
        void WriteInt32(const ChunkPropertyName& propertyName, int32_t value, Error& error) override;
        void WriteInt64(const ChunkPropertyName& propertyName, int64_t value, Error& error) override;
        void WriteUInt8(const ChunkPropertyName& propertyName, uint8_t value, Error& error) override;
        void WriteUInt16(const ChunkPropertyName& propertyName, uint16_t value, Error& error) override;
        void WriteUInt32(const ChunkPropertyName& propertyName, uint32_t value, Error& error) override;
        void WriteUInt64(const ChunkPropertyName& propertyName, uint64_t value, Error& error) override;
        void WriteFloat(const ChunkPropertyName& propertyName, float value, Error& error) override;
        void WriteDouble(const ChunkPropertyName& propertyName, double value, Error& error) override;

        void WriteStridedStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;
        void WriteStridedDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, DataChunkWriteStride valueStride, Error& error) override;

    private:
        Settings settings;
        DataChunkWriterStyle style;
        ConfigDocumentWriter documentWriter;
        ByteBuffer lineBuffer;
    };

} }
