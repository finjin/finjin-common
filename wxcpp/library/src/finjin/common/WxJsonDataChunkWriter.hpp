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
#include "WxDataChunkWriter.hpp"
#include "WxDataChunkWriterController.hpp"
#include <ostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API WxJsonDataChunkWriter : public WxDataChunkWriter
    {
        FINJIN_EXPORTER_NO_COPY_OR_MOVE(WxJsonDataChunkWriter)

    public:
        WxJsonDataChunkWriter();
        ~WxJsonDataChunkWriter();

        void Create(const Settings& settings, DataChunkWriterStyle style, WxDataChunkWriter* parentSection, WxError& error) override;

        WxDataChunkWriterController& GetWriterController() override;

        void WriteWriterHeader(WxError& error) override;
        void WriteChunk(const WxChunkName& name, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error) override;
        void WriteFooter() override;

        bool IsBinaryFormat() const override { return false; }
        ByteOrder GetByteOrder() const override;

        void WriteBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count, WxError& error) override;
        void WriteString(const WxChunkPropertyName& propertyName, const wxString& value, WxError& error) override;
        void WriteDateTime(const WxChunkPropertyName& propertyName, const wxDateTime& value, WxError& error) override;
        void WriteTimeDuration(const WxChunkPropertyName& propertyName, const WxTimeDuration& value, WxError& error) override;
        void WriteUuid(const WxChunkPropertyName& propertyName, const WxUuid& value, WxError& error) override;
        void WriteBool(const WxChunkPropertyName& propertyName, bool value, WxError& error) override;
        void WriteCount(const WxChunkPropertyName& propertyName, size_t value, WxError& error) override;
        void WriteInt8(const WxChunkPropertyName& propertyName, int8_t value, WxError& error) override;
        void WriteInt16(const WxChunkPropertyName& propertyName, int16_t value, WxError& error) override;
        void WriteInt32(const WxChunkPropertyName& propertyName, int32_t value, WxError& error) override;
        void WriteInt64(const WxChunkPropertyName& propertyName, int64_t value, WxError& error) override;
        void WriteUInt8(const WxChunkPropertyName& propertyName, uint8_t value, WxError& error) override;
        void WriteUInt16(const WxChunkPropertyName& propertyName, uint16_t value, WxError& error) override;
        void WriteUInt32(const WxChunkPropertyName& propertyName, uint32_t value, WxError& error) override;
        void WriteUInt64(const WxChunkPropertyName& propertyName, uint64_t value, WxError& error) override;
        void WriteFloat(const WxChunkPropertyName& propertyName, float value, WxError& error) override;
        void WriteDouble(const WxChunkPropertyName& propertyName, double value, WxError& error) override;

        void WriteStridedStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;
        void WriteStridedDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) override;

    private:
        struct Impl;
        Impl* impl;
    };

} }
