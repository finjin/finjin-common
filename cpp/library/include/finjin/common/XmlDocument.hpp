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
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    struct XmlDocumentImpl;
    
    /** A single node within an XmlDocument. */
    class FINJIN_COMMON_LIBRARY_API XmlNode
    {
    public:
        XmlNode(const XmlNode& other) = delete;
        XmlNode& operator = (const XmlNode& other) = delete;

        XmlNode(XmlNode&& other) = delete;
        XmlNode& operator = (XmlNode&& other) = delete;

    public:        
        XmlNode(void* impl);
        virtual ~XmlNode();

        Utf8String GetName() const;

        bool HasAttribute(const Utf8String& name) const;
        
        bool GetAttributeNoCase(const Utf8String& name, Utf8String& value, const char* defaultValue = nullptr) const;
        bool GetAttribute(const Utf8String& name, Utf8String& value, const char* defaultValue = nullptr) const;
        bool GetBoolAttribute(const Utf8String& name, bool& value, bool defaultValue = false) const;
        bool GetIntAttribute(const Utf8String& name, int& value, int defaultValue = 0) const;
        bool GetIntAttribute(const Utf8String& name, size_t& value, size_t defaultValue = 0) const;
        bool GetDoubleAttribute(const Utf8String& name, double& value, double defaultValue = 0) const;
        
        void SetAttribute(const Utf8String& name, const Utf8String& value);
        void SetIntAttribute(const Utf8String& name, int value);
        void SetIntAttribute(const Utf8String& name, size_t value);
        void SetDoubleAttribute(const Utf8String& name, double value);

        void AddText(const Utf8String& value);
        void AddCDataText(const Utf8String& value);
        Utf8String GetText() const;

        std::shared_ptr<XmlNode> AddChildElement(const Utf8String& name);
        std::shared_ptr<XmlNode> GetFirstChildElement();
        std::shared_ptr<XmlNode> GetFirstChildElement(const Utf8String& name);
        std::shared_ptr<XmlNode> GetNextSiblingElement();
        std::shared_ptr<XmlNode> GetNextSiblingElement(const Utf8String& name);

        Utf8String GetValue() const;
        
    private:
        void* impl;
    };

    /** An XML document processor. */
    class FINJIN_COMMON_LIBRARY_API XmlDocument
    {
    public:
        XmlDocument(const XmlDocument& other) = delete;
        XmlDocument& operator = (const XmlDocument& other) = delete;

        XmlDocument(XmlDocument&& other) = delete;
        XmlDocument& operator = (XmlDocument&& other) = delete;

    public:
        XmlDocument();
        virtual ~XmlDocument();

        //Serialization
        void Load(const Path& fileName, Error& error);
        void Save(const Path& fileName, Error& error);

        //Data access
        std::shared_ptr<XmlNode> GetDocumentElement();
        std::shared_ptr<XmlNode> GetRootElement();

        void AddDefaultDeclaration();

        //Conversion
        Utf8String ToString() const;

    private:
        struct Impl;
        Impl* impl;
    };

} }
