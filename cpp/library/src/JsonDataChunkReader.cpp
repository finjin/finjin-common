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
#include "finjin/common/JsonDataChunkReader.hpp"
#include "finjin/common/Base64.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/JsonDocumentImpl.hpp"
#include "DataChunkCommon.hpp"
#include <rapidjson/encodings.h>

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
struct JsonDataChunkReader::Impl
{
    JsonDataChunkReader::Settings settings;

    DataChunkReaderHeader readerHeader;

    Utf8String lineBuffer;
    size_t lineBufferOffset;

    Impl()
    {
        this->lineBufferOffset = 0;
    }

    void Skip(DataHeader& dataHeader, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        dataHeader.currentOffset = dataHeader.length;
        this->lineBufferOffset = this->lineBuffer.size();
    }

    void ReadLine();

    void ReadPropertyName(DataHeader& dataHeader, Utf8String& result, Error& error);
    void ReadValue(DataHeader& dataHeader, Utf8String& value, Error& error);
    void ReadStringBlock(DataHeader& dataHeader, Utf8StringView& value, char startChar, char endChar, Error& error);
    void ReadStringBlock(DataHeader& dataHeader, Utf8String& value, char startChar, char endChar, bool unescape, Error& error);
};


//Local functions---------------------------------------------------------------

//Adapted from RapidJSON reader.h
template<typename Ch, typename InputStream>
unsigned ParseHex4(InputStream& is)
{
    unsigned codepoint = 0;
    for (int i = 0; i < 4; i++)
    {
        Ch c = is.Take();
        codepoint <<= 4;
        codepoint += static_cast<unsigned>(c);
        if (c >= '0' && c <= '9')
            codepoint -= '0';
        else if (c >= 'A' && c <= 'F')
            codepoint -= 'A' - 10;
        else if (c >= 'a' && c <= 'f')
            codepoint -= 'a' - 10;
        else
            return 0;
    }
    return codepoint;
}

template<typename Ch, typename TEncoding, typename InputStream, typename OutputStream>
void ParseStringToStream(InputStream& is, OutputStream& os)
{
    #define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

    static const char escape[256] =
    {
        Z16, Z16, 0, 0, '\"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/',
        Z16, Z16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0,
        0, 0, '\b', 0, 0, 0, '\f', 0, 0, 0, 0, 0, 0, 0, '\n', 0,
        0, 0, '\r', 0, '\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16
    };

    #undef Z16

    while (!is.IsEOF())
    {
        Ch c = is.Peek();
        if (c == '\\')
        {
            is.Take();

            //Escape
            Ch e = is.Take();
            if ((sizeof(Ch) == 1 || unsigned(e) < 256) && escape[(unsigned char)e])
            {
                os.Put(escape[(unsigned char)e]);
            }
            else if (e == 'u')
            {
                //Unicode
                unsigned codepoint = ParseHex4<Ch>(is);
                if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
                {
                    //Handle UTF-16 surrogate pair
                    if (is.Take() != '\\' || is.Take() != 'u')
                    {
                        //RAPIDJSON_PARSE_ERROR(kParseErrorStringUnicodeSurrogateInvalid, is.Tell() - 2);
                    }
                    auto codepoint2 = ParseHex4<Ch>(is);
                    if (codepoint2 < 0xDC00 || codepoint2 > 0xDFFF)
                    {
                        //RAPIDJSON_PARSE_ERROR(kParseErrorStringUnicodeSurrogateInvalid, is.Tell() - 2);
                    }
                    codepoint = (((codepoint - 0xD800) << 10) | (codepoint2 - 0xDC00)) + 0x10000;
                }
                TEncoding::Encode(os, codepoint);
            }
            //else
                //RAPIDJSON_PARSE_ERROR(kParseErrorStringEscapeInvalid, is.Tell() - 1);
        }
        else if ((unsigned)c < 0x20) //RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
        {
            //RAPIDJSON_PARSE_ERROR(kParseErrorStringEscapeInvalid, is.Tell() - 1);
        }
        else
        {
            rapidjson::Transcoder<TEncoding, TEncoding>::Transcode(is, os);
            /*if (parseFlags & kParseValidateEncodingFlag ?
                !Transcoder<SEncoding, TEncoding>::Validate(is, os) :
                !Transcoder<SEncoding, TEncoding>::Transcode(is, os))
                RAPIDJSON_PARSE_ERROR(kParseErrorStringInvalidEncoding, is.Tell());*/
        }
    }
}

class RapidJsonUnescapeInputStream
{
public:
    RapidJsonUnescapeInputStream(const Utf8String& _value) : value(_value)
    {
        this->offset = 0;
    }

    bool IsEOF() const
    {
        return this->offset >= this->value.length();
    }

    char Peek() const
    {
        if (this->offset < this->value.length())
            return this->value[this->offset];
        else
            return 0;
    }

    char Take()
    {
        if (this->offset < this->value.length())
            return this->value[this->offset++];
        else
            return 0;
    }

private:
    const Utf8String& value;
    size_t offset;
};

class RapidJsonUnescapeOutputStream
{
public:
    Utf8String value;

    void Put(char c)
    {
        value += c;
    }
};

static Utf8String UnescapeStringValue(const Utf8String& value)
{
    if (value.empty())
        return value;

    RapidJsonUnescapeInputStream input(value);
    RapidJsonUnescapeOutputStream output;
    ParseStringToStream<char, rapidjson::UTF8<> >(input, output);
    return output.value;
}

static Utf8String RewriteKeyFromJSONStyle(const Utf8String& key)
{
    Utf8String result;

    auto ignoreRemaining = false;
    auto previousCharWasLower = true;
    for (auto c : key)
    {
        if (c == '|')
            ignoreRemaining = true;

        if (!ignoreRemaining && previousCharWasLower && isupper(c))
        {
            result += '-';
            result += tolower(c);
        }
        else
            result += c;

        previousCharWasLower = islower(c) != 0;
    }

    return result;
}

template <typename T>
void ReadNumber(JsonDataChunkReader::Impl* reader, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    reader->ReadValue(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    else
    {
        Convert::ToNumber(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, tempValueString));
    }
}

template <typename T>
size_t ReadNumbers(JsonDataChunkReader::Impl* reader, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView tempValueString;
    reader->ReadStringBlock(dataHeader, tempValueString, '[', ']', error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
        return 0;
    }
    else
    {
        size_t count = 0;

        auto previousCharIndex = (size_t)-1;
        Utf8String stringValue;
        for (size_t charIndex = 0; charIndex <= tempValueString.length(); charIndex++)
        {
            if (charIndex == tempValueString.length() || tempValueString[charIndex] == ',')
            {
                stringValue = tempValueString.substr(previousCharIndex + 1, charIndex - previousCharIndex - 1).ToString();

                previousCharIndex = charIndex;

                if (!stringValue.IsWhitespace())
                {
                    if (count >= maxCount)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% numbers to be parsed.", maxCount));
                        return 0;
                    }

                    T value;
                    Convert::ToNumber(value, stringValue, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, stringValue));
                        return 0;
                    }
                    else
                        GetStridedValue(values, count++, valueStride) = value;
                }
            }
        }

        return count;
    }
}

template <typename T>
void ReadInteger(JsonDataChunkReader::Impl* reader, DataHeader& dataHeader, T& value, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    reader->ReadValue(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
    else
    {
        Convert::ToInteger(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% value for %2%.", typeName, tempValueString));
    }
}

template <typename T>
size_t ReadIntegers(JsonDataChunkReader::Impl* reader, DataHeader& dataHeader, T* values, size_t maxCount, DataChunkReadStride valueStride, const char* typeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView tempValueString;
    reader->ReadStringBlock(dataHeader, tempValueString, '[', ']', error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", typeName));
        return 0;
    }
    else
    {
        size_t count = 0;

        auto previousCharIndex = (size_t)-1;
        Utf8String stringValue;
        for (size_t charIndex = 0; charIndex <= tempValueString.length(); charIndex++)
        {
            if (charIndex == tempValueString.length() || tempValueString[charIndex] == ',')
            {
                stringValue = tempValueString.substr(previousCharIndex + 1, charIndex - previousCharIndex - 1).ToString();

                previousCharIndex = charIndex;

                if (!stringValue.IsWhitespace())
                {
                    if (count >= maxCount)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% integers to be parsed.", maxCount));
                        return 0;
                    }

                    T value;
                    Convert::ToNumber(value, stringValue, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse %1% integers for %2%.", typeName, stringValue));
                        return 0;
                    }
                    else
                        GetStridedValue(values, count++, valueStride) = value;
                }
            }
        }

        return count;
    }
}


//Implementation----------------------------------------------------------------

//JsonDataChunkReader::Impl
void JsonDataChunkReader::Impl::ReadLine()
{
    this->lineBuffer.clear();

    char c;
    while (this->settings.input->IsValid())
    {
        auto readCount = this->settings.input->Read(&c, 1);
        if (readCount == 0)
            break;
        if (c == '\n')
            break;
        if (c != '\r')
            this->lineBuffer += c;
    }

    this->lineBufferOffset = 0;
}

void JsonDataChunkReader::Impl::ReadPropertyName(DataHeader& dataHeader, Utf8String& result, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadStringBlock(dataHeader, result, '"', '"', true, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read key.");
    else
    {
        result = RewriteKeyFromJSONStyle(result);

        this->lineBufferOffset += 1; //Skip past ':'
    }
}

void JsonDataChunkReader::Impl::ReadValue(DataHeader& dataHeader, Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Reads the value as it is
    if (this->lineBufferOffset < this->lineBuffer.size() - 1)
    {
        if (value.assign(this->lineBuffer.begin() + this->lineBufferOffset, this->lineBuffer.size() - this->lineBufferOffset - 1).HasError()) //Subtract trailing ','
        {
            FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
            return;
        }
        this->lineBufferOffset = this->lineBuffer.size();
        Skip(dataHeader, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
    else
        FINJIN_SET_ERROR(error, "Failed to read value.");
}

void JsonDataChunkReader::Impl::ReadStringBlock(DataHeader& dataHeader, Utf8StringView& value, char startChar, char endChar, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t foundCount = 0;
    size_t stringStart = (size_t)-1, stringEnd = (size_t)-1;
    char c = 0;
    while (foundCount < 2 && this->lineBufferOffset < this->lineBuffer.size())
    {
        c = this->lineBuffer[this->lineBufferOffset++];
        dataHeader.currentOffset++;

        if (c == startChar && stringStart == (size_t)-1)
        {
            foundCount++;
            stringStart = this->lineBufferOffset; //This is one past startChar
        }
        else if (c == endChar && stringEnd == (size_t)-1)
        {
            foundCount++;
            stringEnd = this->lineBufferOffset - 1; //This is on endChar
        }
    }

    if (foundCount == 2 && stringStart < stringEnd)
        value.assign(this->lineBuffer.begin() + stringStart, this->lineBuffer.begin() + stringEnd);
    else
        FINJIN_SET_ERROR(error, "Failed to read key.");
}

void JsonDataChunkReader::Impl::ReadStringBlock(DataHeader& dataHeader, Utf8String& value, char startChar, char endChar, bool unescape, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t foundCount = 0;
    size_t stringStart = (size_t)-1, stringEnd = (size_t)-1;
    char c = 0;
    while (foundCount < 2 && this->lineBufferOffset < this->lineBuffer.size())
    {
        c = this->lineBuffer[this->lineBufferOffset++];
        dataHeader.currentOffset++;

        if (c == startChar && stringStart == (size_t)-1)
        {
            foundCount++;
            stringStart = this->lineBufferOffset; //This is one past startChar
        }
        else if (c == endChar && stringEnd == (size_t)-1)
        {
            foundCount++;
            stringEnd = this->lineBufferOffset - 1; //This is on endChar
        }
    }

    if (foundCount == 2 && stringStart < stringEnd)
    {
        this->lineBuffer.substr(value, stringStart, stringEnd - stringStart);
        if (unescape)
            value = UnescapeStringValue(value);
    }
    else
        FINJIN_SET_ERROR(error, "Failed to read key.");
}

//JsonDataChunkReader
JsonDataChunkReader::JsonDataChunkReader()
{
}

JsonDataChunkReader::~JsonDataChunkReader()
{
}

void JsonDataChunkReader::Create(const Settings& settings, JsonDataChunkReader* parentSection, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.maxBytesPerLine < Settings::MIN_BYTES_PER_LINE)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Max bytes per line '%1%' must be %2% or greater.", settings.maxBytesPerLine, Settings::MIN_BYTES_PER_LINE));
        return;
    }

    impl.reset(new Impl);
    impl->settings = settings;
    if (parentSection != nullptr)
        impl->readerHeader = parentSection->impl->readerHeader;
}

DataChunkReaderController& JsonDataChunkReader::GetReaderController()
{
    return *impl->settings.controller;
}

DataChunkReaderInput* JsonDataChunkReader::GetReaderInput()
{
    return impl->settings.input;
}

void JsonDataChunkReader::ReadReaderHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParsedChunkPropertyName key;

    //Read chunk start--------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::CHUNK_START)
    {
        FINJIN_SET_ERROR(error, "Format header start could not be found.");
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

    ReadString(dataHeader, impl->readerHeader.format, error);
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

    ReadUInt32(dataHeader, impl->readerHeader.version, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read format version header value.");
        return;
    }
    if (impl->readerHeader.version != 1)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unsupported version number '%1%'.", impl->readerHeader.version));
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

    ReadCount(dataHeader, impl->readerHeader.maxBytesPerLine, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read max bytes per line header value.");
        return;
    }

    //Read blob text format---------------------------
    ReadDataHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format header.");
        return;
    }
    if (dataHeader.type != DataHeaderType::PROPERTY)
    {
        FINJIN_SET_ERROR(error, "Blob text format header was in an unexpected format.");
        return;
    }

    ReadPropertyName(dataHeader, key, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format key.");
        return;
    }
    if (key != StandardChunkPropertyNames::BLOB_TEXT_FORMAT)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Expected 'blob-text-format' key, read '%1%' instead.", key.ToString()));
        return;
    }

    Utf8String blobTextFormat;
    ReadString(dataHeader, blobTextFormat, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read blob text format value.");
        return;
    }
    impl->readerHeader.blobTextFormat = DataChunkBlobTextFormatUtilities::Parse(blobTextFormat);
}

void JsonDataChunkReader::ReadChunk(std::unique_ptr<DataChunkReader> reader, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->settings.controller->ScheduleReadChunk(chunkFunc, std::move(reader), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to schedule/execute chunk reader.");
        return;
    }
}

void JsonDataChunkReader::ReadDataHeader(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Read length, but be ready for the end
    impl->ReadLine();

    dataHeader.Reset();
    dataHeader.length = impl->lineBuffer.size();

    if (dataHeader.length == 0)
    {
        //The end
        dataHeader.type = DataHeaderType::END;
    }
    else
    {
        if (impl->lineBuffer.back() == '{')
        {
            dataHeader.type = DataHeaderType::CHUNK_START;

            if (impl->lineBuffer.length() > 1)
            {
                impl->ReadPropertyName(dataHeader, dataHeader.name, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to read chunk start key.");
                    return;
                }
            }

            if (!error)
            {
                Skip(dataHeader, error);
                if (error)
                    FINJIN_SET_ERROR(error, "Successfully read chunk start but failed to skip past it.");
            }
        }
        else if (impl->lineBuffer.length() > 1 &&
            impl->lineBuffer[impl->lineBuffer.size() - 2] == '}' &&
            impl->lineBuffer[impl->lineBuffer.size() - 1] == ',')
        {
            dataHeader.type = DataHeaderType::CHUNK_END;
            Skip(dataHeader, error);
            if (error)
                FINJIN_SET_ERROR(error, "Successfully read chunk end but failed to skip past it.");
        }
        else if (impl->lineBuffer.back() == '}')
        {
            dataHeader.type = DataHeaderType::END;
            Skip(dataHeader, error);
            if (error)
                FINJIN_SET_ERROR(error, "Successfully read document end but failed to skip past it.");
        }
        else
        {
            dataHeader.occurrence = DataHeader::Occurrence::ONE;
            dataHeader.type = DataHeaderType::PROPERTY;
        }
    }
}

void JsonDataChunkReader::Skip(DataHeader& dataHeader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->Skip(dataHeader, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

const DataChunkReaderHeader& JsonDataChunkReader::GetHeader() const
{
    return impl->readerHeader;
}

void JsonDataChunkReader::ReadPropertyName(DataHeader& dataHeader, ParsedChunkPropertyName& result, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String readString;
    impl->ReadPropertyName(dataHeader, readString, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    result.Parse(readString);
}

size_t JsonDataChunkReader::ReadBlob(DataHeader& dataHeader, void* values, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t result = 0;

    switch (impl->readerHeader.blobTextFormat)
    {
        case DataChunkBlobTextFormat::BYTE_ARRAY:
        {
            result = ReadUInt8s(dataHeader, static_cast<uint8_t*>(values), count, error);
            if (error)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error);
                return 0;
            }

            break;
        }
        case DataChunkBlobTextFormat::BASE64:
        {
            Utf8String base64;
            ReadString(dataHeader, base64, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read blob base64 string.");
                return 0;
            }

            result = Base64::ToBytes(base64.c_str(), base64.length(), values, count, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to decode blob base64 string.");
                return 0;
            }

            break;
        }
    }

    return result;
}

void JsonDataChunkReader::ReadString(DataHeader& dataHeader, Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->lineBufferOffset < impl->lineBuffer.size() && impl->lineBuffer[impl->lineBufferOffset] != '"')
    {
        //Not quoted, take value as it is
        impl->ReadValue(dataHeader, value, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
    else if (impl->lineBufferOffset < impl->lineBuffer.size() - 2 &&
        impl->lineBuffer[impl->lineBufferOffset] == '"' &&
        impl->lineBuffer[impl->lineBuffer.size() - 2] == '"')
    {
        //Quoted
        Utf8String unquoted;
        impl->lineBuffer.substr(unquoted, impl->lineBufferOffset + 1, impl->lineBuffer.size() - impl->lineBufferOffset - 3);
        if (value.assign(UnescapeStringValue(unquoted)).HasError()) //Skip past leading " and subtract trailing '"' and ','
        {
            FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
            return;
        }

        impl->lineBufferOffset = impl->lineBuffer.size();
        Skip(dataHeader, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
    else
        FINJIN_SET_ERROR(error, "Failed to read value.");
}

void JsonDataChunkReader::ReadDateTime(DataHeader& dataHeader, DateTime& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for ISO-8601 date/time.");
    else
    {
        DateTime::ParseIso8601(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse ISO-8601 date/time value for %1%.", tempValueString));
    }
}

void JsonDataChunkReader::ReadTimeDuration(DataHeader& dataHeader, TimeDuration& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for time duration.");
    else
    {
        TimeDuration::Parse(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse time duration value for %1%.", tempValueString));
    }
}

void JsonDataChunkReader::ReadUuid(DataHeader& dataHeader, Uuid& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for UUID.");
    else
    {
        Uuid::Parse(value, tempValueString, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse UUID value for %1%.", tempValueString));
    }
}

void JsonDataChunkReader::ReadBool(DataHeader& dataHeader, bool& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String tempValueString;
    ReadString(dataHeader, tempValueString, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read value for boolean.");
    else
        value = Convert::ToBool(tempValueString);
}

void JsonDataChunkReader::ReadCount(DataHeader& dataHeader, size_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "size_t", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadInt8(DataHeader& dataHeader, int8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "int8", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadInt16(DataHeader& dataHeader, int16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "int16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadInt32(DataHeader& dataHeader, int32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "int32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadInt64(DataHeader& dataHeader, int64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "int64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadUInt8(DataHeader& dataHeader, uint8_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "uint8", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadUInt16(DataHeader& dataHeader, uint16_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "uint16", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadUInt32(DataHeader& dataHeader, uint32_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "uint32", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadUInt64(DataHeader& dataHeader, uint64_t& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadInteger(impl.get(), dataHeader, value, "uint64", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadFloat(DataHeader& dataHeader, float& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(impl.get(), dataHeader, value, "float", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void JsonDataChunkReader::ReadDouble(DataHeader& dataHeader, double& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ReadNumber(impl.get(), dataHeader, value, "double", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

size_t JsonDataChunkReader::ReadStridedStrings(DataHeader& dataHeader, Utf8String* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView tempValueString;
    impl->ReadStringBlock(dataHeader, tempValueString, '[', ']', error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read string values.");
        return 0;
    }
    else
    {
        size_t count = 0;

        auto previousCharIndex = (size_t)-1;
        Utf8String stringValue;
        for (size_t charIndex = 0; charIndex <= tempValueString.length(); charIndex++)
        {
            if (charIndex == tempValueString.length() || tempValueString[charIndex] == ',')
            {
                stringValue = tempValueString.substr(previousCharIndex + 1, charIndex - previousCharIndex - 1).ToString();

                previousCharIndex = charIndex;

                if (!stringValue.IsWhitespace())
                {
                    if (count >= maxCount)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% strings to be parsed.", maxCount));
                        return 0;
                    }

                    Utf8String unquoted;
                    stringValue.substr(unquoted, 1, stringValue.length() - 2);

                    auto& value = GetStridedValue(values, count++, valueStride);
                    if (value.assign(UnescapeStringValue(unquoted)).HasError())
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate memory for string content.");
                        return 0;
                    }
                }
            }
        }

        return count;
    }
}

size_t JsonDataChunkReader::ReadStridedTimeDurations(DataHeader& dataHeader, TimeDuration* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView tempValueString;
    impl->ReadStringBlock(dataHeader, tempValueString, '[', ']', error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", "time duration collection"));
        return 0;
    }
    else
    {
        size_t count = 0;

        auto previousCharIndex = (size_t)-1;
        Utf8String stringValue;
        for (size_t charIndex = 0; charIndex <= tempValueString.length(); charIndex++)
        {
            if (charIndex == tempValueString.length() || tempValueString[charIndex] == ',')
            {
                stringValue = tempValueString.substr(previousCharIndex + 1, charIndex - previousCharIndex - 1).ToString();

                previousCharIndex = charIndex;

                if (!stringValue.IsWhitespace())
                {
                    if (count >= maxCount)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% time durations to be parsed.", maxCount));
                        return 0;
                    }

                    Utf8String unquoted;
                    stringValue.substr(unquoted, 1, stringValue.length() - 2);

                    auto& value = GetStridedValue(values, count++, valueStride);
                    TimeDuration::Parse(value, unquoted, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse time duration at index %1%.", count - 1));
                        return 0;
                    }
                }
            }
        }

        return count;
    }
}

size_t JsonDataChunkReader::ReadStridedBools(DataHeader& dataHeader, bool* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView tempValueString;
    impl->ReadStringBlock(dataHeader, tempValueString, '[', ']', error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read value for %1%.", "boolean collection"));
        return 0;
    }
    else
    {
        size_t count = 0;

        auto previousCharIndex = (size_t)-1;
        Utf8String stringValue;
        for (size_t charIndex = 0; charIndex <= tempValueString.length(); charIndex++)
        {
            if (charIndex == tempValueString.length() || tempValueString[charIndex] == ',')
            {
                stringValue = tempValueString.substr(previousCharIndex + 1, charIndex - previousCharIndex - 1).ToString();

                previousCharIndex = charIndex;

                if (!stringValue.IsWhitespace())
                {
                    if (count >= maxCount)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There are more than %1% booleans to be parsed.", maxCount));
                        return 0;
                    }

                    GetStridedValue(values, count++, valueStride) = Convert::ToBool(stringValue);
                }
            }
        }

        return count;
    }
}

size_t JsonDataChunkReader::ReadStridedCounts(DataHeader& dataHeader, size_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "count collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedInt8s(DataHeader& dataHeader, int8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "int8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedInt16s(DataHeader& dataHeader, int16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "int16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedInt32s(DataHeader& dataHeader, int32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "int32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedInt64s(DataHeader& dataHeader, int64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "int64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedUInt8s(DataHeader& dataHeader, uint8_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "uint8 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedUInt16s(DataHeader& dataHeader, uint16_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "uint16 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedUInt32s(DataHeader& dataHeader, uint32_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "uint32 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedUInt64s(DataHeader& dataHeader, uint64_t* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadIntegers(impl.get(), dataHeader, values, maxCount, valueStride, "uint64 collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedFloats(DataHeader& dataHeader, float* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(impl.get(), dataHeader, values, maxCount, valueStride, "float collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}

size_t JsonDataChunkReader::ReadStridedDoubles(DataHeader& dataHeader, double* values, size_t maxCount, DataChunkReadStride valueStride, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadNumbers(impl.get(), dataHeader, values, maxCount, valueStride, "double collection", error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    return result;
}
