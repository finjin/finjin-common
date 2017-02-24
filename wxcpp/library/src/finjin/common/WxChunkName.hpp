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
#include "WxUuid.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    struct ParsedChunkName;

    struct WxChunkName
    {
        typedef uint32_t Index;

        enum { NAME_MAX_LENGTH = Finjin::Common::ChunkNameConstants::NAME_MAX_LENGTH };
        enum { ID_MAX_LENGTH = Finjin::Common::ChunkNameConstants::ID_MAX_LENGTH };
        enum { MAX_LENGTH = Finjin::Common::ChunkNameConstants::MAX_LENGTH };

    #if wxUSE_UNICODE
        WxChunkName(const char* name);
        WxChunkName(const char* name, const WxUuid& id);
    #endif
        WxChunkName(const wchar_t* name);
        WxChunkName(const wchar_t* name, const WxUuid& id);
        WxChunkName(size_t index);
        WxChunkName(const WxChunkName& other, size_t index);

        bool IsValid() const;

        bool operator == (const WxChunkName& other) const;
        bool operator == (const wxString& other) const;
        bool operator == (const WxUuid& other) const;
        bool operator == (const ParsedChunkName& other) const;

        wxString ToString() const;

        const wxString name; //Should be unique among all chunk names and point to a string of length no more than NAME_MAX_LENGTH
        const WxUuid id;
        const Index index;
    };

    struct WxChunkPropertyName
    {
        typedef uint32_t ID;
        typedef WxChunkName::Index Index;
        
        WxChunkPropertyName();
        WxChunkPropertyName(const char* s);
        WxChunkPropertyName(size_t index);
        
        bool IsValid() const;
        
        bool operator == (const WxChunkPropertyName& other) const;

        bool operator != (const WxChunkPropertyName& other) const;

        wxString ToString() const;

        const char* name;
        ID id;
        Index index;
    };

    //Standard names-------------------------------
    struct WxStandardChunkPropertyNames
    {
        static const WxChunkPropertyName MAGIC;
        static const WxChunkPropertyName FORMAT;
        static const WxChunkPropertyName FORMAT_VERSION;
        static const WxChunkPropertyName MAX_BYTES_PER_LINE;
        static const WxChunkPropertyName BLOB_TEXT_FORMAT;
    };    

    enum class WxDataChunkBlobTextFormat
    {
        BYTE_ARRAY,
        BASE64
    };

    struct WxDataChunkBlobTextFormatUtilities
    {
        static wxString ToString(WxDataChunkBlobTextFormat value)
        {
            switch (value)
            {
                case WxDataChunkBlobTextFormat::BYTE_ARRAY: return wxT("byte-array");
                case WxDataChunkBlobTextFormat::BASE64: return wxT("base64");
                default: return wxT("byte-array");
            }
        }
    };

} }
