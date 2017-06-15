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
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticUnorderedSet.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API WrappedFileReader
    {
    public:
        struct Header
        {
            enum class FileFormat : uint32_t
            {
                EMBEDDED = 0
            };

            enum class FileFormatClass : uint32_t
            {
                GENERIC = 0,
                IMAGE = 1,
                SOUND = 2
            };

            Header();
            
            static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_SET(10)& GetImageExtensionLookup();
            static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_SET(3)& GetSoundExtensionLookup();

            template <typename T>
            static FileFormatClass GetFileFormatClass(const T& ext)
            {
                if (GetImageExtensionLookup().contains(ext))
                    return Header::FileFormatClass::IMAGE;
                else if (GetSoundExtensionLookup().contains(ext))
                    return Header::FileFormatClass::SOUND;
                else
                    return Header::FileFormatClass::GENERIC;
            }

            static bool IsValidFormatClass(FileFormatClass formatClass);
            static bool IsValidFormat(FileFormat format);

            uint32_t signature;
            FileFormat fileFormat;
            uint32_t fileFormatVersion;
            FileFormatClass fileFormatClass;
            uint32_t fileFormatClassVersion;
            uint32_t fileExtensionLength;
            Utf8String fileExtension;
            uint64_t fileLength;
        };

        WrappedFileReader();

        enum class ReadHeaderResult
        {
            SUCCESS,
            FAILED_TO_READ_SIGNATURE_VALUE,
            INVALID_SIGNATURE_VALUE,
            FAILED_TO_READ_FILE_FORMAT,
            INVALID_FILE_FORMAT,
            FAILED_TO_READ_FILE_FORMAT_VERSION,
            INVALID_FILE_FORMAT_VERSION,
            FAILED_TO_READ_FILE_FORMAT_CLASS,
            INVALID_FILE_FORMAT_CLASS,
            FAILED_TO_READ_FILE_FORMAT_CLASS_VERSION,
            INVALID_FILE_FORMAT_CLASS_VERSION,
            FAILED_TO_READ_FILE_EXTENSION_LENGTH,
            INVALID_FILE_EXTENSION_LENGTH,
            FAILED_TO_READ_FILE_EXTENSION,
            FAILED_TO_READ_FILE_LENGTH
        };
        ReadHeaderResult ReadHeader(ByteBufferReader& byteReader);
        Utf8String GetReadHeaderResultString(ReadHeaderResult result) const;

        void ReadHeader(ByteBufferReader& byteReader, Error& error);

        const Header& GetHeader() const;

        size_t GetHeaderSize() const;

        static void Unwrap(const Path& inFilePath, const Path& outFilePath, Error& error);

    private:
        Header header;
        size_t headerSize;
    };

} }
