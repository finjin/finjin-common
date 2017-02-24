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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Utf8String.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API Base64
    {
    public:        
        /**
        * Gets the number of '=' characters that pad the end of the Base64 string.
        * @param base64 [in] - The Base64 string being checked for padding characters.
        * @return The number of padding '=' characters at the end of the Base64 string.
        * This number will be 0, 1, or 2.
        */
        static size_t GetPaddingCount(const Utf8String& base64);
        static size_t GetPaddingCount(const char* base64, size_t length);

        static size_t GetPaddingCount(size_t byteCount);

        /**
        * Converts a byte count to a Base64 count.
        * The Base64 count represents the number of Base64 characters that will be
        * generated for a byte string of byteCount length.
        * @param byteCount [in] - The number of bytes to be converted to a Base64
        * string. No error checking is performed on this parameter.
        * @return The number of Base64 characters that are generated from the given
        * number of bytes.
        */
        static size_t ToBase64Count(size_t byteCount);

        /**
        * Converts a Base64 character count to a byte count.
        * The byte count represents the number of bytes that will be generated for
        * a Base64 string of characterCount length. The character count includes
        * any padding characters that are contained in a string. To calculate the true
        * byte count for a given string, the result from GetPaddingCount() must
        * be subtracted from the value returned by this function.
        * @param characterCount [in] - The number of Base64 characters to be
        * converted to bytes. No error checking is performed on this parameter.
        * @return The number of bytes that are generated from the given number of
        * characters.
        */
        static size_t ToByteCount(size_t characterCount);

        /**
        * Converts an array of bytes to a Base64 string.
        * @param bytes [in] - Source from which the Base64-encoded data is
        * generated.
        * @param base64 [out] - Destination for the generated Base64 string.
        */
        static void ToBase64(const ByteBuffer& bytes, Utf8String& base64, Error& error);

        /**
        * Converts an array of bytes to a Base64 string.
        * @param bytes [in] - Source from which the Base64-encoded data is
        * generated.
        * @param byteCount [in] - The number of bytes in the input byte array.
        * @param base64 [out] - Destination for the generated Base64 string.
        */
        static void ToBase64(const void* bytes, size_t byteCount, Utf8String& base64, Error& error);

        static size_t ToBase64(const void* bytes, size_t byteCount, uint8_t* output);
        
        /**
        * Converts a Base64 string to an array of bytes.
        * @throws ArgumentExceptionException is thrown if the Base64 string's
        * length is not a multiple of 4.
        * @throws CharacterException is thrown if any of the
        * characters in the Base64 string are not Base64 characters.
        * @param bytes [out] - Destination for the bytes.
        */
        static void ToBytes(const Utf8String& base64, ByteBuffer& bytes, Error& error);
        static void ToBytes(const char* base64Begin, const char* base64End, ByteBuffer& bytes, Error& error);
        static void ToBytes(const char* base64, size_t base64Length, ByteBuffer& bytes, Error& error);

        static size_t ToBytes(const char* base64, size_t base64Length, void* bytes, size_t maxByteCount, Error& error);
    };

} }
