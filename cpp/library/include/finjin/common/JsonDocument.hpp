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
#include "finjin/common/Utf8String.hpp"


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /** A single member/element within an XmlDocument. */
    class FINJIN_COMMON_LIBRARY_API JsonDocumentMember
    {
    public:
        JsonDocumentMember(const JsonDocumentMember& other) = delete;
        JsonDocumentMember& operator = (const JsonDocumentMember& other) = delete;

        JsonDocumentMember(JsonDocumentMember&& other) = delete;
        JsonDocumentMember& operator = (JsonDocumentMember&& other) = delete;

    public:        
        JsonDocumentMember();
        virtual ~JsonDocumentMember();

        bool IsObject() const;
        
        bool IsString() const;
        Utf8String GetString() const;

        bool IsArray() const;
        int GetSize() const;
        std::shared_ptr<JsonDocumentMember> GetItem(int index) const;
        
        bool IsNumber() const;
        double GetDouble() const;
        int GetInt() const;
                
        bool IsBool() const;
        bool GetBool() const;

        bool HasMember(const Utf8String& name) const;

        std::shared_ptr<JsonDocumentMember> GetMember(const Utf8String& name) const;

        int GetMemberCount() const;
        
    private:
        friend class JsonDocument;

        struct Impl;
        Impl* impl;
    };

    struct JsonDocumentImpl;

    /** A JSON document processor. */
    class FINJIN_COMMON_LIBRARY_API JsonDocument
    {
    public:
        JsonDocument(const JsonDocument& other) = delete;
        JsonDocument& operator = (const JsonDocument& other) = delete;

        JsonDocument(JsonDocument&& JsonDocument) = delete;
        JsonDocument& operator = (JsonDocument&& other) = delete;

    public:
        JsonDocument();
        virtual ~JsonDocument();

        //Data access
        JsonDocument& SetObject();

        bool HasMember(const Utf8String& name) const;
        JsonDocument& AddMember(const Utf8String& name, const Utf8String& value);
        JsonDocument& RemoveMember(const Utf8String& name);
        
        std::shared_ptr<JsonDocumentMember> GetMember(const Utf8String& name) const;

        std::shared_ptr<JsonDocumentMember> FindMemberWithChildMemberNameAndValue(const Utf8String& name, const Utf8String& value) const;

        int GetMemberCount() const;
        
        //Utilities
        Utf8String GetStringMember(const Utf8String& name) const;
        
        //Parsing
        void Parse(const Utf8String& text, Error& error);

        JsonDocument& Parse(const Utf8String& text);
        bool HasParseError() const;
        Utf8String GetParseError() const;

        //Conversion
        Utf8String ToString() const;

        JsonDocumentImpl* GetImpl();

    private:
        JsonDocumentImpl* impl;
    };

} }
