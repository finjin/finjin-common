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
#include "finjin/common/BinaryDataChunkWriter.hpp"
#include "finjin/common/Convert.hpp"
#include "DataChunkCommon.hpp"

using namespace Finjin::Common;


//Macros-----------------------------------------------------------------------
#define WRITE_VALUE_LINE(_this, propertyName, value) \
    WriteValueLine(*_this->settings.output, _this->swapBytes, propertyName, value, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) \
    WriteValuesLine(*_this->settings.output, _this->swapBytes, propertyName, values, count, valueStride, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_CHUNK_START_LINE(_this, idOrIndex, flags) WriteChunkStartLine(*_this->settings.output, _this->swapBytes, idOrIndex, flags)
#define WRITE_CHUNK_END_LINE(_this) WriteByte(*_this->settings.output, BINARY_LINE_TYPE_CHUNK_END << BINARY_LINE_TYPE_SHIFT);


//Local functions-------------------------------------------------------------
struct BinaryDataChunkSizeTWrapper
{
    BinaryDataChunkSizeTWrapper(const size_t* _values) : values(_values)
    {
    }

    const size_t* values;
    BinaryMultiValueSize tempValue;
}; 

static const BinaryMultiValueSize& GetStridedValue(BinaryDataChunkSizeTWrapper& values, size_t index, DataChunkWriteStride stride)
{
    auto setIndex = index / stride.tupleSize;
    auto indexInSet = index % stride.tupleSize;
    values.tempValue = *reinterpret_cast<const size_t*>(reinterpret_cast<const uint8_t*>(values.values) + setIndex * stride.tupleStride + indexInSet * sizeof(size_t));
    return values.tempValue;
}

//SizeOfValue
template <typename T>
static size_t SizeOfValue(T& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    return sizeof(T);
}

static size_t SizeOfValue(bool value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    return 1; //1 or 0
}

template <typename T>
size_t SizeOfValue(const BinaryDataChunkBlobWrapper<T>& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    if (elementCount == (size_t)-1)
        return sizeof(BinaryMultiValueSize) + value.count - offset; //Length header + bytes
    else
        return sizeof(BinaryMultiValueSize) + elementCount;
}

static size_t SizeOfValue(const Utf8String& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    if (elementCount == (size_t)-1)
        return sizeof(BinaryMultiValueSize) + value.length() - offset; //Length header + string chars
    else
        return sizeof(BinaryMultiValueSize) + elementCount;
}

static size_t SizeOfValue(const char* value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    if (elementCount == (size_t)-1)
        return sizeof(BinaryMultiValueSize) + strlen(value) - offset; //Length header + string chars
    else
        return sizeof(BinaryMultiValueSize) + elementCount;
}

static size_t SizeOfValue(const TimeDuration& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    return sizeof(BinaryTimeValue);
}

static size_t SizeOfValue(const ChunkPropertyName& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    if (value.index != (ChunkPropertyName::Index)-1)
        return sizeof(value.index);
    else
        return sizeof(value.id);
}

//SizeOfValues
template <typename T>
size_t SizeOfValues(T& values, size_t offset, size_t count, DataChunkWriteStride valueStride)
{
    size_t total = sizeof(BinaryMultiValueSize); //Length header

    for (size_t i = 0; i < count; i++)
    {
        auto& value = GetStridedValue(values, offset + i, valueStride);
        total += SizeOfValue(value);
    }

    return total;
}

//GetElementCount
template <typename T>
size_t GetElementCount(const T& value, size_t offset = 0)
{
    return 1;
}

static size_t GetElementCount(bool value, size_t offset = 0)
{
    return 1;
}

template <typename T>
size_t GetElementCount(const BinaryDataChunkBlobWrapper<T>& value, size_t offset = 0)
{
    return value.count - offset;
}

static size_t GetElementCount(const Utf8String& value, size_t offset = 0)
{
    return value.length() - offset;
}

static size_t GetElementCount(const char* value, size_t offset = 0)
{
    return strlen(value) - offset;
}

static size_t GetElementCount(const TimeDuration& value, size_t offset = 0)
{
    return 1;
}

//FitValueIntoBytes
template <typename T>
size_t FitValueIntoBytes(const T& value, size_t offset, size_t maxBytes)
{
    if (sizeof(T) > maxBytes)
        return 0;

    return 1;
}

static size_t FitValueIntoBytes(bool value, size_t offset, size_t maxBytes)
{
    if (1 > maxBytes)
        return 0;

    return 1;
}

template <typename T>
size_t FitValueIntoBytes(const BinaryDataChunkBlobWrapper<T>& value, size_t offset, size_t maxBytes)
{
    if (sizeof(BinaryMultiValueSize) >= maxBytes)
        return 0;

    maxBytes -= sizeof(BinaryMultiValueSize);

    auto length = value.count - offset;

    return std::min(maxBytes, length);
}

static size_t FitValueIntoBytes(const Utf8String& value, size_t offset, size_t maxBytes)
{
    if (sizeof(BinaryMultiValueSize) >= maxBytes)
        return 0;

    maxBytes -= sizeof(BinaryMultiValueSize);

    auto length = value.length() - offset;

    return std::min(maxBytes, length);
}

static size_t FitValueIntoBytes(const char* value, size_t offset, size_t maxBytes)
{
    if (sizeof(BinaryMultiValueSize) >= maxBytes)
        return 0;

    maxBytes -= sizeof(BinaryMultiValueSize);

    auto length = strlen(value) - offset;

    return std::min(maxBytes, length);
}

static size_t FitValueIntoBytes(const TimeDuration& value, size_t offset, size_t maxBytes)
{
    if (sizeof(BinaryTimeValue) > maxBytes)
        return 0;

    return sizeof(BinaryTimeValue);
}

//FitValuesIntoBytes
template <typename T>
size_t FitValuesIntoBytes(T& values, size_t offset, size_t count, DataChunkWriteStride valueStride, size_t maxBytes)
{
    size_t fitCount = 0;

    if (sizeof(BinaryMultiValueSize) < maxBytes)
    {
        size_t byteCount = sizeof(BinaryMultiValueSize); //Length header

        for (size_t i = 0; i < count; i++)
        {
            auto& value = GetStridedValue(values, offset + i, valueStride);
            auto valueByteCount = SizeOfValue(value);
            if (byteCount + valueByteCount > maxBytes)
                break;

            fitCount++;
            byteCount += valueByteCount;
        }
    }

    return fitCount;
}

//WritePropertyName
void WritePropertyName(DocumentWriterOutput& out, const ChunkPropertyName& propertyName, bool swapBytes)
{
    if (propertyName.index != (ChunkPropertyName::Index)-1)
    {
        auto reordered = propertyName.index;
        if (swapBytes)
            SwapBytes(reordered);

        out.Write(&reordered, sizeof(reordered));
    }
    else
    {
        auto reordered = propertyName.id;
        if (swapBytes)
            SwapBytes(reordered);

        out.Write(&reordered, sizeof(reordered));
    }
}

//WriteBinaryValue
static void WriteBinaryValue(DocumentWriterOutput& out, bool value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    uint8_t intValue = value ? 1 : 0;
    out.Write(&intValue, 1);
}

template <typename T>
void WriteBinaryValue(DocumentWriterOutput& out, T value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    if (swapBytes)
        SwapBytes(value);

    out.Write(&value, sizeof(value));
}

template <typename T>
void WriteBinaryValue(DocumentWriterOutput& out, const BinaryDataChunkBlobWrapper<T>& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryMultiValueSize length = elementCount;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    out.Write(value.bytes + elementOffset, elementCount);
}

static void WriteBinaryValue(DocumentWriterOutput& out, const Utf8String& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryMultiValueSize length = elementCount;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    out.Write(value.c_str() + elementOffset, elementCount);
}

static void WriteBinaryValue(DocumentWriterOutput& out, const char* value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryMultiValueSize length = elementCount;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    out.Write(value + elementOffset, elementCount);
}

static void WriteBinaryValue(DocumentWriterOutput& out, const TimeDuration& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryTimeValue timeValue = value.ToNanoseconds();
    if (swapBytes)
        SwapBytes(timeValue);
    out.Write(&timeValue, sizeof(timeValue));
}

static void WriteBinaryValue(DocumentWriterOutput& out, const Uuid& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    auto bytes = value.GetBytes();
    out.Write(bytes.data(), bytes.size());
}

static void WriteBinaryValue(DocumentWriterOutput& out, float value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    if (std::abs(value) < out.GetMinFloat())
        value = 0;
    if (swapBytes)
        SwapBytes(value);

    out.Write(&value, sizeof(value));
}

static void WriteBinaryValue(DocumentWriterOutput& out, double value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    if (std::abs(value) < out.GetMinDouble())
        value = 0;
    if (swapBytes)
        SwapBytes(value);
    
    out.Write(&value, sizeof(value));
}

//WriteBinaryValues
static void WriteBinaryValues(DocumentWriterOutput& out, const bool* values, size_t offset, size_t count, DataChunkWriteStride valueStride, bool swapBytes)
{
    BinaryMultiValueSize length = count;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    for (size_t i = 0; i < count; i++)
    {
        auto& value = GetStridedValue(values, offset + i, valueStride);
        uint8_t intValue = value ? 1 : 0;
        out.Write(&intValue, 1);
    }
}

static void WriteBinaryValues(DocumentWriterOutput& out, const Utf8String* values, size_t offset, size_t count, DataChunkWriteStride valueStride, bool swapBytes)
{
    BinaryMultiValueSize length = count;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    for (size_t i = 0; i < count; i++)
    {
        auto& value = GetStridedValue(values, offset + i, valueStride);
        WriteBinaryValue(out, value, swapBytes, 0, value.length());
    }
}

static void WriteBinaryValues(DocumentWriterOutput& out, const TimeDuration* values, size_t offset, size_t count, DataChunkWriteStride valueStride, bool swapBytes)
{
    BinaryMultiValueSize length = count;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    for (size_t i = 0; i < count; i++)
    {
        BinaryTimeValue value = GetStridedValue(values, offset + i, valueStride).ToNanoseconds();
        if (swapBytes)
            SwapBytes(value);
        out.Write(&value, sizeof(value));
    }
}

template <typename T>
void WriteBinaryValues(DocumentWriterOutput& out, T& values, size_t offset, size_t count, DataChunkWriteStride valueStride, bool swapBytes)
{
    BinaryMultiValueSize length = count;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    for (size_t i = 0; i < count; i++)
    {
        auto& value = GetStridedValue(values, offset + i, valueStride);
        WriteBinaryValue(out, value, swapBytes, 0, 1);
    }
}

//Various utilities
static void WriteByte(DocumentWriterOutput& out, uint8_t value)
{
    out.Write(&value, 1);
}

static void WriteLength(DocumentWriterOutput& out, BinaryLineLength lineLength, bool swapBytes)
{
    if (swapBytes)
        SwapBytes(lineLength);

    out.Write(&lineLength, sizeof(lineLength));
}

//WriteValueLine
template <typename T>
void WriteValueLine(DocumentWriterOutput& out, bool swapBytes, const ChunkPropertyName& propertyName, const T& value, size_t maxBytesPerLine, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) >= maxBytesPerLine)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to write binary header + property name for property '%1%'. The line buffer is too small.", propertyName.ToString()));
        return;
    }

    //The only single value that this logic can be break up is a string

    size_t elementOffset = 0;
    auto elementCount = GetElementCount(value);
    while (elementOffset < elementCount)
    {   
        uint8_t headerByte = BINARY_LINE_TYPE_SINGLE_VALUE << BINARY_LINE_TYPE_SHIFT;
        if (propertyName.index != (ChunkPropertyName::Index)-1)
            headerByte |= BINARY_LINE_FLAG_INDEXED;
        
        auto maxValueBytes = maxBytesPerLine - 1 - sizeof(BinaryLineLength) - SizeOfValue(propertyName); //Subtract one for header byte, for total length and propertyName length

        auto fitElements = FitValueIntoBytes(value, elementOffset, maxValueBytes);
        if (fitElements == 0)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to write binary value for property '%1%'. The line buffer is too small.", propertyName.ToString()));
            return;
        }

        if (elementOffset == 0 && fitElements == elementCount)
            ; //One value. Do nothing
        else if (elementOffset == 0 && fitElements < elementCount)
            headerByte |= BINARY_LINE_OCCURRENCE_FIRST << BINARY_LINE_OCCURRENCE_SHIFT;
        else if (elementOffset + fitElements < elementCount)
            headerByte |= BINARY_LINE_OCCURRENCE_MORE << BINARY_LINE_OCCURRENCE_SHIFT;
        else
            headerByte |= BINARY_LINE_OCCURRENCE_LAST << BINARY_LINE_OCCURRENCE_SHIFT;
        
        WriteByte(out, headerByte);

        WriteLength(out, SizeOfValue(propertyName) + SizeOfValue(value, elementOffset, fitElements), swapBytes);

        WritePropertyName(out, propertyName, swapBytes);

        WriteBinaryValue(out, value, swapBytes, elementOffset, fitElements);

        elementOffset += fitElements;
    }
}

template <typename T>
void WriteValuesLine(DocumentWriterOutput& out, bool swapBytes, const ChunkPropertyName& propertyName, T& values, size_t count, DataChunkWriteStride valueStride, size_t maxBytesPerLine, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) >= maxBytesPerLine)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to write binary header + property name for property '%1%'. The line buffer is too small.", propertyName.ToString()));
        return;
    }

    size_t elementOffset = 0;
    auto elementCount = count;
    while (elementOffset < elementCount)
    {
        uint8_t headerByte = BINARY_LINE_TYPE_MULTI_VALUE << BINARY_LINE_TYPE_SHIFT;
        if (propertyName.index != (ChunkPropertyName::Index)-1)
            headerByte |= BINARY_LINE_FLAG_INDEXED;

        auto maxValueBytes = maxBytesPerLine - 1 - sizeof(BinaryLineLength) - SizeOfValue(propertyName); //Subtract one for header byte, for total length and propertyName length

        auto fitElements = FitValuesIntoBytes(values, elementOffset, count - elementOffset, valueStride, maxValueBytes);
        if (fitElements == 0)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to write binary values for property '%1%'. The line buffer is too small.", propertyName.ToString()));
            return;
        }

        if (elementOffset == 0 && fitElements == elementCount)
            ; //One value. Do nothing
        else if (elementOffset == 0 && fitElements < elementCount)
            headerByte |= BINARY_LINE_OCCURRENCE_FIRST << BINARY_LINE_OCCURRENCE_SHIFT;
        else if (elementOffset + fitElements < elementCount)
            headerByte |= BINARY_LINE_OCCURRENCE_MORE << BINARY_LINE_OCCURRENCE_SHIFT;
        else
            headerByte |= BINARY_LINE_OCCURRENCE_LAST << BINARY_LINE_OCCURRENCE_SHIFT;

        WriteByte(out, headerByte);

        WriteLength(out, SizeOfValue(propertyName) + SizeOfValues(values, elementOffset, fitElements, valueStride), swapBytes);

        WritePropertyName(out, propertyName, swapBytes);

        WriteBinaryValues(out, values, elementOffset, fitElements, valueStride, swapBytes);

        elementOffset += fitElements;
    }
}

//WriteChunkStartLine
template <typename IDOrIndex>
void WriteChunkStartLine(DocumentWriterOutput& out, bool swapBytes, IDOrIndex idOrIndex, uint8_t flags)
{
    WriteByte(out, (BINARY_LINE_TYPE_CHUNK_START << BINARY_LINE_TYPE_SHIFT) | flags);
    WriteLength(out, sizeof(idOrIndex), swapBytes);
    WriteBinaryValue(out, idOrIndex, swapBytes, 0, 1);
}


//Implementation---------------------------------------------------------------
BinaryDataChunkWriter::BinaryDataChunkWriter()
{    
}

BinaryDataChunkWriter::~BinaryDataChunkWriter()
{        
    if (AnySet(this->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(this->style & DataChunkWriterStyle::ROOT))
        WriteFooter();
}

void BinaryDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, DataChunkWriter* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    this->style = style;    
    this->swapBytes = settings.byteOrder != ::GetByteOrder();
}

DataChunkWriterController& BinaryDataChunkWriter::GetWriterController()
{
    return *this->settings.controller;
}

void BinaryDataChunkWriter::WriteWriterHeader(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WriteUInt32(StandardChunkPropertyNames::MAGIC, FINJIN_MAGIC_FOURCC, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write header magic value.");
        return;
    }

    WriteString(StandardChunkPropertyNames::FORMAT, "Finjin Streaming Binary Document", error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write header format.");
        return;
    }

    WriteUInt32(StandardChunkPropertyNames::FORMAT_VERSION, 1, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write header format version.");
        return;
    }

    WriteCount(StandardChunkPropertyNames::MAX_BYTES_PER_LINE, this->settings.maxBytesPerLine, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write header max bytes per line.");
        return;
    }

    if (this->settings.customHeader != nullptr)
    {
        this->settings.customHeader(*this, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write custom header.");
            return;
        }
    }
}

void BinaryDataChunkWriter::WriteChunk(const ChunkName& name, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.controller->RequiresNewOutput(*this, name))
    {
        //Create new chunk output 
        std::shared_ptr<DocumentWriterOutput> sharedNewOutput = this->settings.controller->AddOutput(*this, name, error);
        if (error || sharedNewOutput == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create new output for chunk '%1%'.", name.name));
            return;
        }        
        
        //Create new writer 
        auto binaryChunkWriter = new BinaryDataChunkWriter();
        auto newSettings = this->settings;
        newSettings.Create(sharedNewOutput, *this->settings.controller);
        binaryChunkWriter->Create(newSettings, DataChunkWriterStyle::ALL, nullptr, error);
        if (error)
        {
            delete binaryChunkWriter;

            FINJIN_SET_ERROR(error, "Failed to create new writer.");
            return;
        }

        std::shared_ptr<DataChunkWriter> chunkWriter(binaryChunkWriter);
        chunkWriter->InheritContextStrings(*this);

        //Write chunk to new writer
        chunkWriter->WriteWriterHeader(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write writer header.");
            return;
        }

        WRITE_CHUNK_START_LINE(binaryChunkWriter, name.id, 0);
        
        auto scheduled = this->settings.controller->ScheduleWriteChunk(chunkWriter, chunkFunc, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to schedule/execute chunk writer.");
            return;
        }

        if (!scheduled)
        {
            //Write chunk to the new writer's output            
            chunkFunc(*chunkWriter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write chunk.");
                return;
            }
        }
    }    
    else
    {
        //Write chunk to this writer's output
        if (name.index != (ChunkName::Index)-1)
        {
            WRITE_CHUNK_START_LINE(this, static_cast<ChunkName::Index>(name.index), BINARY_LINE_FLAG_INDEXED);
            
            BinaryDataChunkWriter chunkWriter;
            chunkWriter.Create(this->settings, DataChunkWriterStyle::NESTED, this, error);
            chunkWriter.InheritContextStrings(*this);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write chunk writer.");
                return;
            }

            chunkFunc(chunkWriter, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write chunk.");
                return;
            }
            
            //No need to write chunk end since the chunkWriter does it in its destructor
        }
        else
        {
            WRITE_CHUNK_START_LINE(this, name.id, 0);
            
            chunkFunc(*this, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write chunk.");
                return;
            }

            WRITE_CHUNK_END_LINE(this);
        }
    }
}

void BinaryDataChunkWriter::WriteFooter()
{
    {
        FINJIN_DECLARE_ERROR(error);

        if (this->settings.customFooter != nullptr)
        {
            this->settings.customFooter(*this, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to write custom footer.");
                return;
            }
        }
    }

    WriteByte(*this->settings.output, BINARY_LINE_TYPE_DOCUMENT_END << BINARY_LINE_TYPE_SHIFT);
}

bool BinaryDataChunkWriter::WillSplitBlob(const ChunkPropertyName& propertyName, const void* values, size_t count)
{
    //header byte + sizeof(length) + sizeof(propertyName) + count
    auto proposedLength = 1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) + count;
    return proposedLength > this->settings.maxBytesPerLine;
}

bool BinaryDataChunkWriter::WillSplitString(const ChunkPropertyName& propertyName, const Utf8String& value)
{
    //header byte + sizeof(length) + sizeof(propertyName) + value.length()
    auto proposedLength = 1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) + value.length();
    return proposedLength > this->settings.maxBytesPerLine;
}

void BinaryDataChunkWriter::WriteBlob(const ChunkPropertyName& propertyName, const void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitBlob(propertyName, values, count))
    {
        WriteCount(this->settings.lengthHintPropertyName, count, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write length hint.");
            return;
        }
    }

    BinaryDataChunkBlobWrapper<const uint8_t*> blob(values, count);
    WRITE_VALUE_LINE(this, propertyName, blob);
}

void BinaryDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitString(propertyName, value))
    {
        WriteCount(this->settings.lengthHintPropertyName, value.length(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to write length hint.");
            return;
        }
    }

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteString(const ChunkPropertyName& propertyName, const char* value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteDateTime(const ChunkPropertyName& propertyName, const DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryTimeValue timeValue = value.ToUnixTimestamp();
    WRITE_VALUE_LINE(this, propertyName, timeValue);
}

void BinaryDataChunkWriter::WriteTimeDuration(const ChunkPropertyName& propertyName, const TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteUuid(const ChunkPropertyName& propertyName, const Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteBool(const ChunkPropertyName& propertyName, bool value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteCount(const ChunkPropertyName& propertyName, size_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryMultiValueSize countValue = value;
    WRITE_VALUE_LINE(this, propertyName, countValue);
}

void BinaryDataChunkWriter::WriteInt8(const ChunkPropertyName& propertyName, int8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteInt16(const ChunkPropertyName& propertyName, int16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteInt32(const ChunkPropertyName& propertyName, int32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteInt64(const ChunkPropertyName& propertyName, int64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteUInt8(const ChunkPropertyName& propertyName, uint8_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteUInt16(const ChunkPropertyName& propertyName, uint16_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteUInt32(const ChunkPropertyName& propertyName, uint32_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteUInt64(const ChunkPropertyName& propertyName, uint64_t value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteFloat(const ChunkPropertyName& propertyName, float value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteDouble(const ChunkPropertyName& propertyName, double value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void BinaryDataChunkWriter::WriteStridedStrings(const ChunkPropertyName& propertyName, const Utf8String* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedTimeDurations(const ChunkPropertyName& propertyName, const TimeDuration* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedBools(const ChunkPropertyName& propertyName, const bool* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedCounts(const ChunkPropertyName& propertyName, const size_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    BinaryDataChunkSizeTWrapper wrapper(values);
    WRITE_VALUES_LINE(this, propertyName, wrapper, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedInt8s(const ChunkPropertyName& propertyName, const int8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedInt16s(const ChunkPropertyName& propertyName, const int16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedInt32s(const ChunkPropertyName& propertyName, const int32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedInt64s(const ChunkPropertyName& propertyName, const int64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedUInt8s(const ChunkPropertyName& propertyName, const uint8_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedUInt16s(const ChunkPropertyName& propertyName, const uint16_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedUInt32s(const ChunkPropertyName& propertyName, const uint32_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedUInt64s(const ChunkPropertyName& propertyName, const uint64_t* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedFloats(const ChunkPropertyName& propertyName, const float* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void BinaryDataChunkWriter::WriteStridedDoubles(const ChunkPropertyName& propertyName, const double* values, size_t count, DataChunkWriteStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
