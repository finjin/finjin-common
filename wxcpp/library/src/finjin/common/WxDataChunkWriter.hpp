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


//Includes---------------------------------------------------------------------
#include "WxChunkName.hpp"
#include "WxUuid.hpp"
#include "WxTimeDuration.hpp"
//#include "Logger.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class WxDocumentWriterOutput;
    class WxDataChunkWriter;
    class WxDataChunkWriterController;
        
    enum class DataChunkWriterStyle
    {
        ROOT = 1 << 0,
        NESTED = 1 << 1,
        ALL = ROOT | NESTED
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(DataChunkWriterStyle)

    struct WxDataChunkWriteStride
    {
        WxDataChunkWriteStride()
        {
            this->tupleSize = 0;
            this->tupleStride = 0;
        }

        WxDataChunkWriteStride(size_t tupleSize, size_t tupleStride)
        {
            this->tupleSize = tupleSize;
            this->tupleStride = tupleStride;
        }

        size_t tupleSize; //Number of values (not bytes) next to each other
        size_t tupleStride; //Distance in bytes between a set of tupleSize items, and the next set of tupleSize items
    };

    class WxDataChunkWriter
    {
    public:        
        struct Settings
        {
            enum {MIN_BYTES_PER_LINE = 128 };
            enum {DEFAULT_MAX_BYTES_PER_LINE = 1024 * 10 };

            Settings();

            void Create(std::shared_ptr<WxDocumentWriterOutput> outPtr, WxDataChunkWriterController& controller);
            void Create(WxDocumentWriterOutput& out, WxDataChunkWriterController& controller);
            void Create(std::shared_ptr<WxDocumentWriterOutput> outPtr, WxDocumentWriterOutput& out, WxDataChunkWriterController& controller);

            ByteOrder byteOrder; //Byte ordering when writing binary files
            size_t maxBytesPerLine;
            WxDataChunkBlobTextFormat blobTextFormat;
            WxChunkPropertyName lengthHintPropertyName;
            std::shared_ptr<WxDocumentWriterOutput> outputPtr;
            WxDocumentWriterOutput* output;
            WxDataChunkWriterController* controller;
            std::function<void(WxDataChunkWriter&, WxError& error)> customHeader;
            std::function<void(WxDataChunkWriter&, WxError& error)> customFooter;
        };

        virtual ~WxDataChunkWriter();

        void CreateRoot(const Settings& settings, WxError& error); //Equivalent to Create(settings, DataChunkWriterStyle::ROOT, nullptr, error)

        virtual void Create(const Settings& settings, DataChunkWriterStyle style, WxDataChunkWriter* parentSection, WxError& error) = 0;

        virtual WxDataChunkWriterController& GetWriterController() = 0;

        virtual void WriteWriterHeader(WxError& error) = 0;
        virtual void WriteChunk(const WxChunkName& name, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error) = 0;        
        virtual void WriteFooter() = 0;

        virtual bool IsBinaryFormat() const = 0;
        virtual ByteOrder GetByteOrder() const = 0;

        virtual bool WillSplitBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count) { return false; }
        virtual bool WillSplitString(const WxChunkPropertyName& propertyName, const wxString& value) { return false; }

        virtual void WriteBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count, WxError& error) = 0;
        virtual void WriteString(const WxChunkPropertyName& propertyName, const wxString& value, WxError& error) = 0;
        virtual void WriteDateTime(const WxChunkPropertyName& propertyName, const wxDateTime& value, WxError& error) = 0;
        virtual void WriteTimeDuration(const WxChunkPropertyName& propertyName, const WxTimeDuration& value, WxError& error) = 0;
        virtual void WriteUuid(const WxChunkPropertyName& propertyName, const WxUuid& value, WxError& error) = 0;
        virtual void WriteBool(const WxChunkPropertyName& propertyName, bool value, WxError& error) = 0;
        virtual void WriteCount(const WxChunkPropertyName& propertyName, size_t value, WxError& error) = 0;
        virtual void WriteInt8(const WxChunkPropertyName& propertyName, int8_t value, WxError& error) = 0;
        virtual void WriteInt16(const WxChunkPropertyName& propertyName, int16_t value, WxError& error) = 0;
        virtual void WriteInt32(const WxChunkPropertyName& propertyName, int32_t value, WxError& error) = 0;
        virtual void WriteInt64(const WxChunkPropertyName& propertyName, int64_t value, WxError& error) = 0;
        virtual void WriteUInt8(const WxChunkPropertyName& propertyName, uint8_t value, WxError& error) = 0;
        virtual void WriteUInt16(const WxChunkPropertyName& propertyName, uint16_t value, WxError& error) = 0;
        virtual void WriteUInt32(const WxChunkPropertyName& propertyName, uint32_t value, WxError& error) = 0;
        virtual void WriteUInt64(const WxChunkPropertyName& propertyName, uint64_t value, WxError& error) = 0;
        virtual void WriteFloat(const WxChunkPropertyName& propertyName, float value, WxError& error) = 0;
        virtual void WriteDouble(const WxChunkPropertyName& propertyName, double value, WxError& error) = 0;
        
        virtual void WriteStridedStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;
        virtual void WriteStridedDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error) = 0;

        void WriteStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxError& error);
        void WriteTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxError& error);
        void WriteBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxError& error);
        void WriteCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxError& error);
        void WriteInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxError& error);
        void WriteInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxError& error);
        void WriteInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxError& error);
        void WriteInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxError& error);
        void WriteUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxError& error);
        void WriteUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxError& error);
        void WriteUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxError& error);
        void WriteUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxError& error);
        void WriteFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxError& error);
        void WriteDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxError& error);

        enum class ContextIndex
        {
            OBJECT_NAME,
            EMBED_OBJECT,
            LINK_TO_MAIN_OBJECT,
            
            COUNT
        };

        const wxString& GetContextString(ContextIndex index) const;
        void SetContextString(ContextIndex index, const wxString& value);
        void SetContextStringProcessor(ContextIndex index, std::function<void(wxString&)> value);
        
        void InheritContextStrings(WxDataChunkWriter& other);

    protected:
        EnumValues<ContextIndex, ContextIndex::COUNT, wxString> contextStrings;
        EnumValues<ContextIndex, ContextIndex::COUNT, std::function<void(wxString&)> > contextStringProcessors;
    };

    template <typename T>
    const T& GetStridedValue(const T* values, size_t index, WxDataChunkWriteStride stride)
    {
        auto setIndex = index / stride.tupleSize;
        auto indexInSet = index % stride.tupleSize;
        return *reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(values) + setIndex * stride.tupleStride + indexInSet * sizeof(T));
    }

} }
