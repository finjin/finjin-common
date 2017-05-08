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
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Uuid.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class DataChunkWriter;
    class DataChunkWriterController;
    class DocumentWriterOutput;

    enum class DataChunkWriterStyle
    {
        ROOT = 1 << 0,
        NESTED = 1 << 1,
        ALL = ROOT | NESTED
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(DataChunkWriterStyle);

    struct DataChunkWriteStride
    {
        DataChunkWriteStride()
        {
            this->tupleSize = 0;
            this->tupleStride = 0;
        }

        DataChunkWriteStride(size_t tupleSize, size_t tupleStride)
        {
            this->tupleSize = tupleSize;
            this->tupleStride = tupleStride;
        }

        size_t tupleSize; //Number of values (not bytes) next to each other
        size_t tupleStride; //Distance in bytes between a set of tupleSize items, and the next set of tupleSize items
    };

    class FINJIN_COMMON_LIBRARY_API DataChunkWriter
    {
    public:
        struct Settings
        {
            enum { MIN_BYTES_PER_LINE = 128 };
            enum { DEFAULT_MAX_BYTES_PER_LINE = MemorySize::KIBIBYTE * 10 };

            Settings();

            void Create(std::shared_ptr<DocumentWriterOutput> outPtr, DataChunkWriterController& controller);
            void Create(DocumentWriterOutput& out, DataChunkWriterController& controller);
            void Create(std::shared_ptr<DocumentWriterOutput> outPtr, DocumentWriterOutput& out, DataChunkWriterController& controller);

            ByteOrder byteOrder; //Byte ordering when writing binary files
            size_t maxBytesPerLine;
            DataChunkBlobTextFormat blobTextFormat;
            ChunkPropertyName lengthHintPropertyName;
            std::shared_ptr<DocumentWriterOutput> outputPtr;
            DocumentWriterOutput* output;
            DataChunkWriterController* controller;
            std::function<void(DataChunkWriter&, Error& error)> customHeader;
            std::function<void(DataChunkWriter&, Error& error)> customFooter;
        };

        virtual ~DataChunkWriter();

        void CreateRoot(const Settings& settings, Error& error); //Equivalent to Create(settings, DataChunkWriterStyle::ROOT, nullptr, error)

        virtual void Create(const Settings& settings, DataChunkWriterStyle style, DataChunkWriter* parentSection, Error& error) = 0;

        virtual DataChunkWriterController& GetWriterController() = 0;

        virtual DocumentWriterOutput* GetWriterOutput() = 0;

        virtual void WriteWriterHeader(Error& error) = 0;
        virtual void WriteChunk(const ChunkName& name, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error) = 0;
        virtual void WriteChunkStart(const ChunkName& name, Error& error) = 0;
        virtual void WriteChunkEnd(const ChunkName& name, Error& error) = 0;
        virtual void WriteFooter() = 0;

        virtual bool IsBinaryFormat() const = 0;
        virtual ByteOrder GetByteOrder() const = 0;

        virtual bool WillSplitBlob(const ChunkPropertyName& propertyName, const void* values, size_t count) { return false; }
        virtual bool WillSplitString(const ChunkPropertyName& propertyName, const Utf8String& value) { return false; }

        virtual void WriteBlob(const ChunkPropertyName& propertyName, const void* values, size_t count, Error& error) = 0;
        virtual void WriteString(const ChunkPropertyName& propertyName, const Utf8String& value, Error& error) = 0;
        virtual void WriteString(const ChunkPropertyName& propertyName, const char* value, Error& error) = 0;
        virtual void WriteDateTime(const ChunkPropertyName& propertyName, const DateTime& value, Error& error) = 0;
        virtual void WriteTimeDuration(const ChunkPropertyName& propertyName, const TimeDuration& value, Error& error) = 0;
        virtual void WriteUuid(const ChunkPropertyName& propertyName, const Uuid& value, Error& error) = 0;
        virtual void WriteBool(const ChunkPropertyName& propertyName, bool value, Error& error) = 0;
        virtual void WriteCount(const ChunkPropertyName& propertyName, size_t value, Error& error) = 0;
        virtual void WriteInt8(const ChunkPropertyName& propertyName, int8_t value, Error& error) = 0;
        virtual void WriteInt16(const ChunkPropertyName& propertyName, int16_t value, Error& error) = 0;
        virtual void WriteInt32(const ChunkPropertyName& propertyName, int32_t value, Error& error) = 0;
        virtual void WriteInt64(const ChunkPropertyName& propertyName, int64_t value, Error& error) = 0;
        virtual void WriteUInt8(const ChunkPropertyName& propertyName, uint8_t value, Error& error) = 0;
        virtual void WriteUInt16(const ChunkPropertyName& propertyName, uint16_t value, Error& error) = 0;
        virtual void WriteUInt32(const ChunkPropertyName& propertyName, uint32_t value, Error& error) = 0;
        virtual void WriteUInt64(const ChunkPropertyName& propertyName, uint64_t value, Error& error) = 0;
        virtual void WriteFloat(const ChunkPropertyName& propertyName, float value, Error& error) = 0;
        virtual void WriteDouble(const ChunkPropertyName& propertyName, double value, Error& error) = 0;

        //Note: WriteStridedStrings() and WriteStrings() are provided for writing arrays of simple alphanumeric strings. It should not be used for generic free text strings.

        virtual void WriteStridedStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;
        virtual void WriteStridedDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, DataChunkWriteStride valueStride, Error& error) = 0;

        void WriteStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, Error& error);
        void WriteTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, Error& error);
        void WriteBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, Error& error);
        void WriteCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, Error& error);
        void WriteInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, Error& error);
        void WriteInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, Error& error);
        void WriteInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, Error& error);
        void WriteInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, Error& error);
        void WriteUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, Error& error);
        void WriteUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, Error& error);
        void WriteUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, Error& error);
        void WriteUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, Error& error);
        void WriteFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, Error& error);
        void WriteDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, Error& error);

        enum class ContextIndex
        {
            OBJECT_NAME, //When setting this, it resets the other strings
            EMBED_OBJECT,
            LINK_TO_MAIN_OBJECT,

            COUNT
        };

        const Utf8String& GetContextString(ContextIndex index) const;
        void SetContextString(ContextIndex index, const Utf8String& value);
        void SetContextStringProcessor(ContextIndex index, std::function<void(Utf8String&)> value);

        void InheritContextStrings(DataChunkWriter& other);

    protected:
        EnumArray<ContextIndex, ContextIndex::COUNT, Utf8String> contextStrings;
        EnumArray<ContextIndex, ContextIndex::COUNT, std::function<void(Utf8String&)> > contextStringProcessors;
    };

    template <typename T>
    const T& GetStridedValue(const T* values, size_t index, DataChunkWriteStride stride)
    {
        auto tupleIndex = index / stride.tupleSize;
        auto indexInTuple = index % stride.tupleSize;
        return *reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(values) + tupleIndex * stride.tupleStride + indexInTuple * sizeof(T));
    }

} }
