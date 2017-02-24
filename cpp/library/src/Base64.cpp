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
#include "finjin/common/Base64.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
static const char* BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static bool IsBase64(char character)
{
    return 
        (character >= 'A' && character <= 'Z') ||
        (character >= 'a' && character <= 'z') ||
        (character >= '/' && character <= '9') ||
        (character == '+');
}

static char Base64ToIndex(char character)
{
    if (IsBase64(character))
    {
        auto result = (const char*)memchr(BASE64_CHARS, character, 64) - BASE64_CHARS;
        return (char)result;
    }
    else
        return -1;
}

size_t Base64::GetPaddingCount(const Utf8String& base64)
{
    return GetPaddingCount(base64.c_str(), base64.length());
}

size_t Base64::GetPaddingCount(const char* base64, size_t length)
{
    size_t padCount = 0;

    if (length > 0 && base64[length - 1] == '=')
        padCount++;
    if (length > 1 && base64[length - 2] == '=')
        padCount++;

    return padCount;
}

size_t Base64::GetPaddingCount(size_t byteCount)
{
    auto converted = ToByteCount(ToBase64Count(byteCount));
    return converted - byteCount;
}

size_t Base64::ToBase64Count(size_t byteCount)
{
    //3 pairs of 8-bits -> 4 pairs of 6-bits
    return 4 * ((byteCount + 2) / 3);
}

size_t Base64::ToByteCount(size_t characterCount)
{
    //4 pairs of 6-bits -> 3 pairs of 8-bits
    return 3 * (characterCount / 4);
}

void Base64::ToBase64(const ByteBuffer& bytes, Utf8String& base64, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (bytes.size() == 0)
        base64.clear();
    else
    {
        ToBase64(&bytes[0], bytes.size(), base64, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
}

void Base64::ToBase64(const void* voidBytes, size_t byteCount, Utf8String& base64, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (byteCount == 0)
        base64.clear();
    else
    {
        auto base64Count = ToBase64Count(byteCount);
        base64.resize(base64Count);

        ToBase64(voidBytes, byteCount, (uint8_t*)&base64[0]);
    }
}

size_t Base64::ToBase64(const void* vbytes, size_t byteCount, uint8_t* base64)
{
    //From http://www.adp-gmbh.ch/cpp/common/base64.html

    auto bytes = static_cast<const uint8_t*>(vbytes);

    size_t base64Offset = 0;

    int i = 0;
    int j = 0;
    uint8_t charArray3[3];
    uint8_t charArray4[4];

    while (byteCount-- != 0)
    {
        charArray3[i++] = *(bytes++);
        if (i == 3)
        {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (i = 0; (i <4); i++)
                base64[base64Offset++] = BASE64_CHARS[charArray4[i]];
            i = 0;
        }
    }

    if (i > 0)
    {
        for (j = i; j < 3; j++)
            charArray3[j] = '\0';

        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
        charArray4[3] = charArray3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            base64[base64Offset++] = BASE64_CHARS[charArray4[j]];

        while (i++ < 3)
            base64[base64Offset++] = '=';
    }

    return base64Offset;
}

void Base64::ToBytes(const Utf8String& base64, ByteBuffer& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ToBytes(base64.c_str(), base64.length(), bytes, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

void Base64::ToBytes(const char* base64Begin, const char* base64End, ByteBuffer& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ToBytes(base64Begin, base64End - base64Begin, bytes, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

void Base64::ToBytes(const char* base64, size_t base64Length, ByteBuffer& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if ((base64Length & 0x3) != 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The length of the base64 string must be a multiple of 4. It is %1%.", base64Length));
        return;
    }

    auto padByteCount = GetPaddingCount(base64);
    auto byteCount = ToByteCount(base64Length) - padByteCount;
    if (bytes.resize(byteCount) < byteCount)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate %1% bytes in byte buffer.", byteCount));
        return;
    }

    if (byteCount > 0)
    {
        ToBytes(base64, base64Length, bytes.data(), byteCount, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
}

size_t Base64::ToBytes(const char* base64, size_t base64Length, void* vbytes, size_t maxByteCount, Error& error)
{
    //From http://www.adp-gmbh.ch/cpp/common/base64.html

    FINJIN_ERROR_METHOD_START(error);

    auto bytes = static_cast<uint8_t*>(vbytes);

    if ((base64Length & 0x3) != 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The length of the base64 string must be a multiple of 4. It is %1%.", base64Length));
        return 0;
    }

    size_t base64Offset = 0;
    size_t byteOffset = 0;
    uint8_t tempArray3[3];
    uint8_t tempArray4[4];

    size_t i = 0;
    while (base64Length-- && (base64[base64Offset] != '=') && IsBase64(base64[base64Offset]))
    {
        tempArray4[i++] = base64[base64Offset]; base64Offset++;
        if (i == 4) 
        {
            for (i = 0; i < 4; i++)
                tempArray4[i] = Base64ToIndex(tempArray4[i]);

            tempArray3[0] = (tempArray4[0] << 2) + ((tempArray4[1] & 0x30) >> 4);
            tempArray3[1] = ((tempArray4[1] & 0xf) << 4) + ((tempArray4[2] & 0x3c) >> 2);
            tempArray3[2] = ((tempArray4[2] & 0x3) << 6) + tempArray4[3];

            for (i = 0; i < 3; i++)
                bytes[byteOffset++] = tempArray3[i];
            
            i = 0;
        }
    }

    if (i > 0) 
    {
        for (size_t j = i; j < 4; j++)
            tempArray4[j] = 0;

        for (size_t j = 0; j < 4; j++)
            tempArray4[j] = Base64ToIndex(tempArray4[j]);

        tempArray3[0] = (tempArray4[0] << 2) + ((tempArray4[1] & 0x30) >> 4);
        tempArray3[1] = ((tempArray4[1] & 0xf) << 4) + ((tempArray4[2] & 0x3c) >> 2);
        tempArray3[2] = ((tempArray4[2] & 0x3) << 6) + tempArray4[3];

        for (size_t j = 0; j < i - 1; j++)
            bytes[byteOffset++] = tempArray3[j];
    }

    return byteOffset;
}
