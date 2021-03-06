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
#include "finjin/common/StaticUnorderedMap.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Uuid.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    struct ParsedChunkName;
    struct ParsedChunkPropertyName;

    //Chunk name-----------------------------
    struct FINJIN_COMMON_LIBRARY_API ChunkName
    {
        using Index = uint32_t;

        enum { NAME_MAX_LENGTH = CommonConstants::ChunkName::NAME_MAX_LENGTH };
        enum { ID_MAX_LENGTH = CommonConstants::ChunkName::ID_MAX_LENGTH };
        enum { MAX_LENGTH = CommonConstants::ChunkName::MAX_LENGTH };

        ChunkName();
        ChunkName(const char* name);
        ChunkName(const char* name, const Uuid& id);
        ChunkName(size_t index);
        ChunkName(const ChunkName& other, size_t index);

        bool IsValid() const;

        bool operator == (const ChunkName& other) const;
        bool operator == (const Utf8String& other) const;
        bool operator == (const Uuid& other) const;
        bool operator == (const ParsedChunkName& other) const;

        bool operator != (const ChunkName& other) const;
        bool operator != (const Utf8String& other) const;
        bool operator != (const Uuid& other) const;
        bool operator != (const ParsedChunkName& other) const;

        Utf8String ToString() const;

        const char* name; //Should be unique among all chunk names and point to a string of length no more than NAME_MAX_LENGTH
        Uuid id;
        Index index;
    };

    struct FINJIN_COMMON_LIBRARY_API ParsedChunkName
    {
        using Index = ChunkName::Index;

        ParsedChunkName();
        ParsedChunkName(const Utf8String& s);
        ParsedChunkName(Index index);

        void Parse(const Utf8String& s);

        bool operator == (const ParsedChunkName& other) const;
        bool operator == (const ChunkName& other) const;

        bool operator != (const ParsedChunkName& other) const;
        bool operator != (const ChunkName& other) const;

        bool IsValid() const;

        Utf8String ToString() const;

        Utf8String name;
        Uuid id;
        Index index;
    };

    //Chunk property name-----------------------
    struct FINJIN_COMMON_LIBRARY_API ChunkPropertyName
    {
        using ID = uint32_t;
        using Index = ChunkName::Index;

        ChunkPropertyName();
        explicit ChunkPropertyName(const char* name);
        ChunkPropertyName(const char* name, ID id);
        ChunkPropertyName(size_t index);

        bool IsValid() const;

        bool operator == (const ChunkPropertyName& other) const;
        bool operator == (const Utf8String& other) const;
        bool operator == (ID other) const;
        bool operator == (const ParsedChunkPropertyName& other) const;

        bool operator != (const ChunkPropertyName& other) const;
        bool operator != (const Utf8String& other) const;
        bool operator != (ID other) const;
        bool operator != (const ParsedChunkPropertyName& other) const;

        Utf8String ToString() const;

        const char* name; //Should be unique among all chunk names and point to a string of length no more than NAME_MAX_LENGTH
        ID id;
        Index index;
    };

    struct FINJIN_COMMON_LIBRARY_API ParsedChunkPropertyName
    {
        using ID = ChunkPropertyName::ID;
        using Index = ChunkPropertyName::Index;

        ParsedChunkPropertyName();
        ParsedChunkPropertyName(const Utf8String& s);
        ParsedChunkPropertyName(size_t index);

        void Parse(const Utf8String& s);

        bool operator == (const ChunkPropertyName& other) const;

        bool operator != (const ChunkPropertyName& other) const;

        bool IsValid() const;

        Utf8String ToString() const;

        Utf8String name; //Property name. Only used with text files.
        ID id; //Property ID. When reading a text file, this is a hash of the name. When reading a binary file it is read directly
        Index index; //Property index. When reading a text file this is parsed, when reading a binary file it is read directly
    };

    //Standard names-------------------------------
    struct StandardChunkPropertyNames
    {
        static const ChunkPropertyName SIGNATURE;
        static const ChunkPropertyName FORMAT;
        static const ChunkPropertyName FORMAT_VERSION;
        static const ChunkPropertyName MAX_BYTES_PER_LINE;
        static const ChunkPropertyName BLOB_TEXT_FORMAT;
    };


    //Blob text format (binary data in a text file)-----------
    enum class DataChunkBlobTextFormat
    {
        BYTE_ARRAY,
        BASE64,

        COUNT
    };

    struct DataChunkBlobTextFormatUtilities
    {
        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(DataChunkBlobTextFormat, DataChunkBlobTextFormat::COUNT)& GetLookup()
        {
            static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(DataChunkBlobTextFormat, DataChunkBlobTextFormat::COUNT) lookup
                (
                "byte-array", DataChunkBlobTextFormat::BYTE_ARRAY,
                "base64", DataChunkBlobTextFormat::BASE64
                );
            return lookup;
        }

        static const char* ToString(DataChunkBlobTextFormat value)
        {
            auto& lookup = GetLookup();
            for (auto& item : lookup)
            {
                if (item.second == value)
                    return item.first;
            }

            return FINJIN_ENUM_UNKNOWN_STRING;
        }

        template <typename T>
        static DataChunkBlobTextFormat Parse(const T& value)
        {
            auto& lookup = GetLookup();
            return lookup.GetOrDefault(value, DataChunkBlobTextFormat::COUNT);
        }
    };

} }
