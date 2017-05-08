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
#include "finjin/common/XmlDataElementWriter.hpp"
#include "finjin/common/XmlDocument.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
XmlDataElementWriter::XmlDataElementWriter(XmlDocument& _xmlDocument, std::shared_ptr<XmlNode> _xmlNode) : xmlDocument(&_xmlDocument), xmlNode(_xmlNode)
{
}

void XmlDataElementWriter::SetAttribute(const Utf8String& name, const Utf8String& value)
{
    if (this->xmlNode != nullptr)
        xmlNode->SetAttribute(name, value);
}

void XmlDataElementWriter::AddValue(const Utf8String& name, const Utf8String& value)
{
    AddSimpleChildText(name, value);
}

std::shared_ptr<DataElementWriter> XmlDataElementWriter::StartChildElement(const Utf8String& name)
{
    std::shared_ptr<DataElementWriter> childElement;

    if (this->xmlNode != nullptr)
        childElement.reset(new XmlDataElementWriter(*this->xmlDocument, this->xmlNode->AddChildElement(name)));

    return childElement;
}

void XmlDataElementWriter::EndChildElement()
{
}

std::shared_ptr<DataElementWriter> XmlDataElementWriter::StartChildArray(const Utf8String& name)
{
    return StartChildElement(name);
}

void XmlDataElementWriter::EndChildArray()
{
}

std::shared_ptr<DataElementWriter> XmlDataElementWriter::StartObject(const Utf8String& name)
{
    return StartChildElement(name);
}

void XmlDataElementWriter::EndObject()
{
}

void XmlDataElementWriter::AddChildText(const Utf8String& name, const Utf8String& value)
{
    if (this->xmlNode != nullptr)
    {
        auto childElement = this->xmlNode->AddChildElement(name);
        childElement->AddCDataText(value);
    }
}

void XmlDataElementWriter::AddSimpleChildText(const Utf8String& name, const Utf8String& value)
{
    if (this->xmlNode != nullptr)
    {
        auto childElement = this->xmlNode->AddChildElement(name);
        childElement->AddText(value);
    }
}
