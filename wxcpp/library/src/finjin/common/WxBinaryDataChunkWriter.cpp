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
#include "WxBinaryDataChunkWriter.hpp"

using namespace Finjin::Common;

#define BINARY_LINE_TYPE_CHUNK_START 1
#define BINARY_LINE_TYPE_CHUNK_END 2
#define BINARY_LINE_TYPE_DOCUMENT_END 3
#define BINARY_LINE_TYPE_SINGLE_VALUE 4
#define BINARY_LINE_TYPE_MULTI_VALUE 5
#define BINARY_LINE_TYPE_MASK 0x0f
#define BINARY_LINE_TYPE_SHIFT 0

//If there is no flag, then that indicates everything is contained in the encountered occurrence
#define BINARY_LINE_OCCURRENCE_FIRST 1
#define BINARY_LINE_OCCURRENCE_MORE 2
#define BINARY_LINE_OCCURRENCE_LAST 3
#define BINARY_LINE_OCCURRENCE_MASK 0x30
#define BINARY_LINE_OCCURRENCE_SHIFT 4

#define BINARY_LINE_FLAG_INDEXED (1 << 6) //Indicates chunk start or chunk property name has an integer index, not an ID

typedef uint64_t BinaryMultiValueSize;
typedef uint64_t BinaryLineLength;
typedef uint64_t BinaryTimeValue;

template <typename T>
struct BinaryDataChunkBlobWrapper
{
    template <typename ValuesType>
    BinaryDataChunkBlobWrapper(ValuesType values, size_t _count) : bytes((T)values), count(_count)
    {
    }

    T bytes;
    size_t count;
};


//Macros-----------------------------------------------------------------------
#define WRITE_VALUE_LINE(_this, propertyName, value) \
    WriteValueLine(*_this->settings.output, _this->swapBytes, propertyName, value, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_VALUES_LINE(_this, propertyName, values, count, valueStride) \
    WriteValuesLine(*_this->settings.output, _this->swapBytes, propertyName, values, count, valueStride, _this->settings.maxBytesPerLine, error); \
    if (error) \
    { \
        FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
        return; \
    }

#define WRITE_CHUNK_START_LINE(_this, idOrIndex, flags) WriteChunkStartLine(*_this->settings.output, _this->swapBytes, idOrIndex, flags)
#define WRITE_CHUNK_END_LINE(_this) WriteByte(*_this->settings.output, BINARY_LINE_TYPE_CHUNK_END << BINARY_LINE_TYPE_SHIFT);


//Local functions--------------------------------------------------------------
struct BinaryDataChunkSizeTWrapper
{
    BinaryDataChunkSizeTWrapper(const size_t* _values) : values(_values)
    {
    }

    const size_t* values;   
    BinaryMultiValueSize tempValue;
};

static const BinaryMultiValueSize& GetStridedValue(BinaryDataChunkSizeTWrapper& values, size_t index, WxDataChunkWriteStride stride)
{
    auto setIndex = index / stride.tupleSize;
    auto indexInSet = index % stride.tupleSize;
    values.tempValue = *reinterpret_cast<const size_t*>(reinterpret_cast<const uint8_t*>(values.values) + setIndex * stride.tupleStride + indexInSet * sizeof(size_t));
    return values.tempValue;
}

//SizeOfValue
template <typename T>
size_t SizeOfValue(const T& value, size_t offset = 0, size_t elementCount = (size_t)-1)
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

static size_t SizeOfValue(const wxString& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    if (elementCount == (size_t)-1)
        return sizeof(BinaryMultiValueSize) + value.length() - offset; //Length header + string chars
    else
        return sizeof(BinaryMultiValueSize) + elementCount;
}

static size_t SizeOfValue(const WxTimeDuration& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    return sizeof(BinaryTimeValue);
}

static size_t SizeOfValue(const WxChunkPropertyName& value, size_t offset = 0, size_t elementCount = (size_t)-1)
{
    if (value.index != (WxChunkPropertyName::Index)-1)
        return sizeof(value.index);
    else
        return sizeof(value.id);
}

//SizeOfValues
template <typename T>
size_t SizeOfValues(T& values, size_t offset, size_t count, WxDataChunkWriteStride valueStride)
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

static size_t GetElementCount(const wxString& value, size_t offset = 0)
{
    return value.length() - offset;
}

static size_t GetElementCount(const WxTimeDuration& value, size_t offset = 0)
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

static size_t FitValueIntoBytes(const wxString& value, size_t offset, size_t maxBytes)
{
    if (sizeof(BinaryMultiValueSize) >= maxBytes)
        return 0;

    maxBytes -= sizeof(BinaryMultiValueSize);

    auto length = value.length() - offset;

    return std::min(maxBytes, length);
}

static size_t FitValueIntoBytes(const WxTimeDuration& value, size_t offset, size_t maxBytes)
{
    if (sizeof(BinaryTimeValue) > maxBytes)
        return 0;

    return sizeof(BinaryTimeValue);
}

//FitValuesIntoBytes
template <typename T>
size_t FitValuesIntoBytes(T& values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, size_t maxBytes)
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
void WritePropertyName(WxDocumentWriterOutput& out, const WxChunkPropertyName& propertyName, bool swapBytes)
{
    if (propertyName.index != (WxChunkPropertyName::Index)-1)
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
static void WriteBinaryValue(WxDocumentWriterOutput& out, bool value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    uint8_t intValue = value ? 1 : 0;
    out.Write(&intValue, 1);
}

template <typename T>
void WriteBinaryValue(WxDocumentWriterOutput& out, T value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    if (swapBytes)
        SwapBytes(value);

    out.Write(&value, sizeof(value));
}

template <typename T>
void WriteBinaryValue(WxDocumentWriterOutput& out, const BinaryDataChunkBlobWrapper<T>& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryMultiValueSize length = elementCount;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    out.Write(value.bytes + elementOffset, elementCount);
}

static void WriteBinaryValue(WxDocumentWriterOutput& out, const wxString& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryMultiValueSize length = elementCount;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    out.Write(value.c_str() + elementOffset, elementCount);
}

static void WriteBinaryValue(WxDocumentWriterOutput& out, const WxTimeDuration& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    BinaryTimeValue timeValue = value.ToNanoseconds();
    if (swapBytes)
        SwapBytes(timeValue);
    out.Write(&timeValue, sizeof(timeValue));
}

static void WriteBinaryValue(WxDocumentWriterOutput& out, const WxUuid& value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    auto bytes = value.GetBytes();
    out.Write(bytes.data(), bytes.size());
}

static void WriteBinaryValue(WxDocumentWriterOutput& out, float value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    if (std::abs(value) < out.GetMinFloat())
        value = 0;
    if (swapBytes)
        SwapBytes(value);

    out.Write(&value, sizeof(value));
}

static void WriteBinaryValue(WxDocumentWriterOutput& out, double value, bool swapBytes, size_t elementOffset, size_t elementCount)
{
    if (std::abs(value) < out.GetMinDouble())
        value = 0;
    if (swapBytes)
        SwapBytes(value);
    
    out.Write(&value, sizeof(value));
}

//WriteBinaryValues
static void WriteBinaryValues(WxDocumentWriterOutput& out, const bool* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, bool swapBytes)
{
    BinaryMultiValueSize length = count;
    if (swapBytes)
        SwapBytes(length);
    out.Write(&length, sizeof(length));

    for (size_t i = 0; i < count; i++)
    {
        uint8_t value = GetStridedValue(values, offset + i, valueStride) ? 1 : 0;
        out.Write(&value, sizeof(value));
    }
}

static void WriteBinaryValues(WxDocumentWriterOutput& out, const wxString* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, bool swapBytes)
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

static void WriteBinaryValues(WxDocumentWriterOutput& out, const WxTimeDuration* values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, bool swapBytes)
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
void WriteBinaryValues(WxDocumentWriterOutput& out, T& values, size_t offset, size_t count, WxDataChunkWriteStride valueStride, bool swapBytes)
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
static void WriteByte(WxDocumentWriterOutput& out, uint8_t value)
{
    out.Write(&value, 1);
}

static void WriteLength(WxDocumentWriterOutput& out, BinaryLineLength lineLength, bool swapBytes)
{
    if (swapBytes)
        SwapBytes(lineLength);

    out.Write(&lineLength, sizeof(lineLength));
}

//WriteValueLine
template <typename T>
void WriteValueLine(WxDocumentWriterOutput& out, bool swapBytes, const WxChunkPropertyName& propertyName, const T& value, size_t maxBytesPerLine, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) >= maxBytesPerLine)
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Unable to write binary header + key for property '%s'. The line buffer is too small."), propertyName.ToString().wx_str()));
        return;
    }

    //The only single value that this logic can be break up is a string

    size_t elementOffset = 0;
    auto elementCount = GetElementCount(value);
    while (elementOffset < elementCount)
    {   
        uint8_t headerByte = BINARY_LINE_TYPE_SINGLE_VALUE << BINARY_LINE_TYPE_SHIFT;
        if (propertyName.index != (WxChunkPropertyName::Index)-1)
            headerByte |= BINARY_LINE_FLAG_INDEXED;
        
        auto maxValueBytes = maxBytesPerLine - 1 - sizeof(BinaryLineLength) - SizeOfValue(propertyName); //Subtract one for header byte, for total length and propertyName length

        auto fitElements = FitValueIntoBytes(value, elementOffset, maxValueBytes);
        if (fitElements == 0)
        {
            FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Unable to write binary value for property '%s'. The line buffer is too small."), propertyName.ToString().wx_str()));
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
void WriteValuesLine(WxDocumentWriterOutput& out, bool swapBytes, const WxChunkPropertyName& propertyName, T& values, size_t count, WxDataChunkWriteStride valueStride, size_t maxBytesPerLine, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) >= maxBytesPerLine)
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Unable to write binary header + propertyName for property '%s'. The line buffer is too small."), propertyName.ToString().wx_str()));
        return;
    }

    size_t elementOffset = 0;
    auto elementCount = count;
    while (elementOffset < elementCount)
    {
        uint8_t headerByte = BINARY_LINE_TYPE_MULTI_VALUE << BINARY_LINE_TYPE_SHIFT;
        if (propertyName.index != (WxChunkPropertyName::Index)-1)
            headerByte |= BINARY_LINE_FLAG_INDEXED;

        auto maxValueBytes = maxBytesPerLine - 1 - sizeof(BinaryLineLength) - SizeOfValue(propertyName); //Subtract one for header byte, for total length and propertyName length

        auto fitElements = FitValuesIntoBytes(values, elementOffset, count - elementOffset, valueStride, maxValueBytes);
        if (fitElements == 0)
        {
            FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Unable to write binary values for property '%s'. The line buffer is too small."), propertyName.ToString().wx_str()));
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
void WriteChunkStartLine(WxDocumentWriterOutput& out, bool swapBytes, IDOrIndex idOrIndex, uint8_t flags)
{
    WriteByte(out, (BINARY_LINE_TYPE_CHUNK_START << BINARY_LINE_TYPE_SHIFT) | flags);
    WriteLength(out, sizeof(idOrIndex), swapBytes);
    WriteBinaryValue(out, idOrIndex, swapBytes, 0, 1);
}


//Implementation---------------------------------------------------------------
WxBinaryDataChunkWriter::WxBinaryDataChunkWriter()
{    
}

WxBinaryDataChunkWriter::~WxBinaryDataChunkWriter()
{    
    if (AnySet(this->style & DataChunkWriterStyle::NESTED))
        WRITE_CHUNK_END_LINE(this);

    if (AnySet(this->style & DataChunkWriterStyle::ROOT))
        WriteFooter();
}

void WxBinaryDataChunkWriter::Create(const Settings& settings, DataChunkWriterStyle style, WxDataChunkWriter* parentSection, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Max bytes per line '%d' must be %d or greater."), settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    this->settings = settings;
    this->style = style;
    this->swapBytes = settings.byteOrder != ::GetByteOrder();
}

WxDataChunkWriterController& WxBinaryDataChunkWriter::GetWriterController()
{
    return *this->settings.controller;
}

void WxBinaryDataChunkWriter::WriteWriterHeader(WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WriteUInt32(WxStandardChunkPropertyNames::MAGIC, FINJIN_MAGIC_FOURCC, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write header magic value."));
        return;
    }

    WriteString(WxStandardChunkPropertyNames::FORMAT, wxT("Finjin Streaming Binary Document"), error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write header format."));
        return;
    }

    WriteUInt32(WxStandardChunkPropertyNames::FORMAT_VERSION, 1, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write header format version."));
        return;
    }

    WriteCount(WxStandardChunkPropertyNames::MAX_BYTES_PER_LINE, this->settings.maxBytesPerLine, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to write header max bytes per line."));
        return;
    }

    if (this->settings.customHeader != nullptr)
    {
        this->settings.customHeader(*this, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write custom header."));
            return;
        }
    }
}

void WxBinaryDataChunkWriter::WriteChunk(const WxChunkName& name, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (this->settings.controller->RequiresNewOutput(*this, name))
    {
        //Create new chunk output 
        std::shared_ptr<WxDocumentWriterOutput> sharedNewOutput = this->settings.controller->AddOutput(*this, name, error);
        if (error || sharedNewOutput == nullptr)
        {
            FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to create new output for chunk '%s'."), name.ToString().wx_str()));
            return;
        }        
        
        //Create new writer 
        auto binaryChunkWriter = new WxBinaryDataChunkWriter();
        auto newSettings = this->settings;
        newSettings.Create(sharedNewOutput, *this->settings.controller);
        binaryChunkWriter->Create(newSettings, DataChunkWriterStyle::ALL, nullptr, error);
        if (error)
        {
            delete binaryChunkWriter;

            FINJIN_WX_SET_ERROR(error, wxT("Failed to create new writer."));
            return;
        }

        std::shared_ptr<WxDataChunkWriter> chunkWriter(binaryChunkWriter);
        chunkWriter->InheritContextStrings(*this);

        //Write chunk to new writer
        chunkWriter->WriteWriterHeader(error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write writer header."));
            return;
        }

        WRITE_CHUNK_START_LINE(binaryChunkWriter, name.id, 0);
        
        auto scheduled = this->settings.controller->ScheduleWriteChunk(chunkWriter, chunkFunc, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to schedule/execute chunk writer."));
            return;
        }

        if (!scheduled)
        {
            //Write chunk to the new writer's output            
            chunkFunc(*chunkWriter, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk."));
                return;
            }
        }
    }    
    else
    {
        //Write chunk to this writer's output
        if (name.index != (WxChunkName::Index)-1)
        {
            WRITE_CHUNK_START_LINE(this, static_cast<WxChunkName::Index>(name.index), BINARY_LINE_FLAG_INDEXED);
            WxBinaryDataChunkWriter chunkWriter;
            chunkWriter.Create(this->settings, DataChunkWriterStyle::NESTED, this, error);
            chunkWriter.InheritContextStrings(*this);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk writer."));
                return;
            }

            chunkFunc(chunkWriter, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk."));
                return;
            }
        }
        else
        {
            WRITE_CHUNK_START_LINE(this, name.id, 0);
            
            chunkFunc(*this, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write chunk."));
                return;
            }

            WRITE_CHUNK_END_LINE(this);
        }
    }
}

void WxBinaryDataChunkWriter::WriteFooter()
{
    {
        FINJIN_WX_DECLARE_ERROR(error);

        if (this->settings.customFooter != nullptr)
        {
            this->settings.customFooter(*this, error);
            if (error)
            {
                FINJIN_WX_SET_ERROR(error, wxT("Failed to write custom footer."));
                return;
            }
        }
    }

    WriteByte(*this->settings.output, BINARY_LINE_TYPE_DOCUMENT_END << BINARY_LINE_TYPE_SHIFT);
}

bool WxBinaryDataChunkWriter::WillSplitBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count)
{
    //header byte + sizeof(length) + sizeof(propertyName) + count
    auto proposedLength = 1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) + count;
    return proposedLength > this->settings.maxBytesPerLine;
}

bool WxBinaryDataChunkWriter::WillSplitString(const WxChunkPropertyName& propertyName, const wxString& _value)
{
    auto value = _value.ToUTF8();

    //header byte + sizeof(length) + sizeof(propertyName) + value.length()
    auto proposedLength = 1 + sizeof(BinaryLineLength) + SizeOfValue(propertyName) + value.length();
    return proposedLength > this->settings.maxBytesPerLine;
}

void WxBinaryDataChunkWriter::WriteBlob(const WxChunkPropertyName& propertyName, const void* values, size_t count, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitBlob(propertyName, values, count))
    {
        WriteCount(this->settings.lengthHintPropertyName, count, error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write length hint."));
            return;
        }
    }

    BinaryDataChunkBlobWrapper<const uint8_t*> blob(values, count);
    WRITE_VALUE_LINE(this, propertyName, blob);
}

void WxBinaryDataChunkWriter::WriteString(const WxChunkPropertyName& propertyName, const wxString& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (this->settings.lengthHintPropertyName.IsValid() && WillSplitString(propertyName, value))
    {
        WriteCount(this->settings.lengthHintPropertyName, value.ToUTF8().length(), error);
        if (error)
        {
            FINJIN_WX_SET_ERROR(error, wxT("Failed to write length hint."));
            return;
        }
    }

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteDateTime(const WxChunkPropertyName& propertyName, const wxDateTime& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    BinaryTimeValue timeValue = value.GetTicks();
    WRITE_VALUE_LINE(this, propertyName, timeValue);
}

void WxBinaryDataChunkWriter::WriteTimeDuration(const WxChunkPropertyName& propertyName, const WxTimeDuration& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteUuid(const WxChunkPropertyName& propertyName, const WxUuid& value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteBool(const WxChunkPropertyName& propertyName, bool value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteCount(const WxChunkPropertyName& propertyName, size_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    BinaryMultiValueSize countValue = value;
    WRITE_VALUE_LINE(this, propertyName, countValue);
}

void WxBinaryDataChunkWriter::WriteInt8(const WxChunkPropertyName& propertyName, int8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteInt16(const WxChunkPropertyName& propertyName, int16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteInt32(const WxChunkPropertyName& propertyName, int32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteInt64(const WxChunkPropertyName& propertyName, int64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteUInt8(const WxChunkPropertyName& propertyName, uint8_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteUInt16(const WxChunkPropertyName& propertyName, uint16_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteUInt32(const WxChunkPropertyName& propertyName, uint32_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteUInt64(const WxChunkPropertyName& propertyName, uint64_t value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteFloat(const WxChunkPropertyName& propertyName, float value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteDouble(const WxChunkPropertyName& propertyName, double value, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUE_LINE(this, propertyName, value);
}

void WxBinaryDataChunkWriter::WriteStridedStrings(const WxChunkPropertyName& propertyName, const wxString* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedTimeDurations(const WxChunkPropertyName& propertyName, const WxTimeDuration* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedBools(const WxChunkPropertyName& propertyName, const bool* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedCounts(const WxChunkPropertyName& propertyName, const size_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    BinaryDataChunkSizeTWrapper wrapper(values);
    WRITE_VALUES_LINE(this, propertyName, wrapper, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedInt8s(const WxChunkPropertyName& propertyName, const int8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedInt16s(const WxChunkPropertyName& propertyName, const int16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedInt32s(const WxChunkPropertyName& propertyName, const int32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedInt64s(const WxChunkPropertyName& propertyName, const int64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedUInt8s(const WxChunkPropertyName& propertyName, const uint8_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedUInt16s(const WxChunkPropertyName& propertyName, const uint16_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedUInt32s(const WxChunkPropertyName& propertyName, const uint32_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedUInt64s(const WxChunkPropertyName& propertyName, const uint64_t* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedFloats(const WxChunkPropertyName& propertyName, const float* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}

void WxBinaryDataChunkWriter::WriteStridedDoubles(const WxChunkPropertyName& propertyName, const double* values, size_t count, WxDataChunkWriteStride valueStride, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    WRITE_VALUES_LINE(this, propertyName, values, count, valueStride);
}
