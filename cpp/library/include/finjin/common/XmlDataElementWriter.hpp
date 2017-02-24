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
#include "finjin/common/DataElementWriter.hpp"
#include "finjin/common/XmlDocument.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common { 

    class FINJIN_COMMON_LIBRARY_API XmlDataElementWriter : public DataElementWriter
    {
    public:
        XmlDataElementWriter(const XmlDataElementWriter& other) = delete;
        XmlDataElementWriter& operator = (const XmlDataElementWriter& other) = delete;

        XmlDataElementWriter(XmlDataElementWriter&& other) = delete;
        XmlDataElementWriter& operator = (XmlDataElementWriter&& other) = delete;

    public:
        /** 
         * Constructor.
         * @param xmlElement [in] - The XML element that is managed. This element is assumed to be managed by the document, so it is not destroyed.
         */
        XmlDataElementWriter(XmlDocument& xmlDocument, std::shared_ptr<XmlNode> xmlNode);

        void SetAttribute(const Utf8String& name, const Utf8String& value) override;
        
        void AddValue(const Utf8String& name, const Utf8String& value) override;

        std::shared_ptr<DataElementWriter> StartChildElement(const Utf8String& name) override;
        void EndChildElement() override;

        std::shared_ptr<DataElementWriter> StartChildArray(const Utf8String& name) override;
        void EndChildArray() override;

        std::shared_ptr<DataElementWriter> StartObject(const Utf8String& name) override;
        void EndObject() override;

        void AddChildText(const Utf8String& name, const Utf8String& value) override;
        void AddSimpleChildText(const Utf8String& name, const Utf8String& value) override;
        
    private:
        XmlDocument* xmlDocument;
        std::shared_ptr<XmlNode> xmlNode;
    };

} }
