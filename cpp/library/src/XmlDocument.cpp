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
#include "finjin/common/XmlDocument.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"
#include "finjin/common/Convert.hpp"
#include <nowide/cstdio.hpp>
#include <tinyxml/tinyxml2.h>

using namespace Finjin::Common;


//Local classes-----------------------------------------------------------------
struct XmlDocument::Impl
{
    tinyxml2::XMLDocument doc;
};


//Implementation----------------------------------------------------------------
//XmlNode
XmlNode::XmlNode(void* _impl) : impl(_impl)
{
}

XmlNode::~XmlNode()
{
}

Utf8String XmlNode::GetName() const
{
    Utf8String name;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        name = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->Name();

    return name;
}

bool XmlNode::HasAttribute(const Utf8String& name) const
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return false;

    for (auto attribute = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->FirstAttribute();
        attribute != nullptr;
        attribute = attribute->Next())
    {
        if (name == attribute->Name())
            return true;
    }

    return false;
}

bool XmlNode::GetAttributeNoCase(const Utf8String& name, Utf8String& value, const char* defaultValue) const
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
    {
        value = defaultValue;
        return false;
    }

    for (auto attribute = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->FirstAttribute(); 
        attribute != nullptr; 
        attribute = attribute->Next()) 
    {
        if (name.EqualsNoCaseAscii(attribute->Name()))
        {                
            if (!attribute->Value())
                break;

            value = attribute->Value();
            return true;
        }
    }
    
    value = defaultValue;
    return false;
}

bool XmlNode::GetAttribute(const Utf8String& name, Utf8String& value, const char* defaultValue) const
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
    {
        value = defaultValue;
        return false;
    }

    for (auto attribute = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->FirstAttribute(); 
        attribute != nullptr; 
        attribute = attribute->Next()) 
    {
        if (name.Equals(attribute->Name()))
        {                
            if (!attribute->Value())
                break;

            value = attribute->Value();
            return true;
        }
    }
    
    value = defaultValue;
    return false;
}

bool XmlNode::GetBoolAttribute(const Utf8String& name, bool& value, bool defaultValue) const
{
    Utf8String stringValue;
    if (GetAttribute(name, stringValue))
    {
        value = Convert::ToBool(stringValue);
        return true;
    }

    value = defaultValue;
    return true;
}

bool XmlNode::GetIntAttribute(const Utf8String& name, int& value, int defaultValue) const
{
    Utf8String stringValue;
    if (GetAttribute(name, stringValue))
    {
        value = Convert::ToInteger(stringValue, defaultValue);
        return true;
    }

    value = defaultValue;
    return false;
}

bool XmlNode::GetIntAttribute(const Utf8String& name, size_t& value, size_t defaultValue) const
{
    Utf8String stringValue;
    if (GetAttribute(name, stringValue))
    {
        value = Convert::ToInteger(stringValue, defaultValue);
        return true;
    }

    value = defaultValue;
    return false;
}

bool XmlNode::GetDoubleAttribute(const Utf8String& name, double& value, double defaultValue) const
{
    Utf8String stringValue;
    if (GetAttribute(name, stringValue))
    {
        value = Convert::ToNumber(stringValue, defaultValue);
        return true;
    }

    value = defaultValue;
    return false;
}

void XmlNode::SetAttribute(const Utf8String& name, const Utf8String& value)
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return;

    static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->SetAttribute(name.c_str(), value.c_str());
}

void XmlNode::SetIntAttribute(const Utf8String& name, int value)
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return;

    static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->SetAttribute(name.c_str(), value);
}

void XmlNode::SetIntAttribute(const Utf8String& name, size_t value)
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return;

    static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->SetAttribute(name.c_str(), (unsigned)value);
}

void XmlNode::SetDoubleAttribute(const Utf8String& name, double value)
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return;

    static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->SetAttribute(name.c_str(), value);
}

Utf8String XmlNode::GetText() const
{    
    Utf8String value;
    
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr)
    {
        const char* elementText = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->GetText();
        if (elementText != nullptr)
            value = elementText;
    }
    
    return value;
}

void XmlNode::AddText(const Utf8String& value)
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return;

    auto newChild = static_cast<tinyxml2::XMLNode*>(impl)->GetDocument()->NewText(value.c_str());
    static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->LinkEndChild(newChild);
    newChild->SetCData(false);
}

void XmlNode::AddCDataText(const Utf8String& value)
{
    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() == nullptr)
        return;

    auto newChild = static_cast<tinyxml2::XMLNode*>(impl)->GetDocument()->NewText(value.c_str());
    static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->LinkEndChild(newChild);
    newChild->SetCData(true);
}

std::shared_ptr<XmlNode> XmlNode::AddChildElement(const Utf8String& name)
{
    std::shared_ptr<XmlNode> child;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr ||
        static_cast<tinyxml2::XMLNode*>(impl)->ToDocument() != nullptr)
    {
        auto newChild = static_cast<tinyxml2::XMLNode*>(impl)->GetDocument()->NewElement(name.c_str());
        static_cast<tinyxml2::XMLNode*>(impl)->LinkEndChild(newChild);
        child.reset(new XmlNode(newChild));
    }

    return child;
}

std::shared_ptr<XmlNode> XmlNode::GetFirstChildElement()
{
    std::shared_ptr<XmlNode> child;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr)
    {
        auto firstChild = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->FirstChildElement();
        if (firstChild != nullptr)
            child.reset(new XmlNode(firstChild));
    }

    return child;
}

std::shared_ptr<XmlNode> XmlNode::GetFirstChildElement(const Utf8String& name)
{
    std::shared_ptr<XmlNode> child;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr)
    {
        auto firstChild = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->FirstChildElement(name.c_str());
        if (firstChild != nullptr)
            child.reset(new XmlNode(firstChild));
    }

    return child;
}

std::shared_ptr<XmlNode> XmlNode::GetNextSiblingElement()
{
    std::shared_ptr<XmlNode> sibling;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr)
    {
        auto nextSibling = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->NextSiblingElement();
        if (nextSibling != nullptr)
            sibling.reset(new XmlNode(nextSibling));
    }

    return sibling;
}

std::shared_ptr<XmlNode> XmlNode::GetNextSiblingElement(const Utf8String& name)
{
    std::shared_ptr<XmlNode> sibling;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr)
    {
        auto nextSibling = static_cast<tinyxml2::XMLNode*>(impl)->ToElement()->NextSiblingElement();
        if (nextSibling != nullptr)
            sibling.reset(new XmlNode(nextSibling));
    }

    return sibling;
}

Utf8String XmlNode::GetValue() const
{
    Utf8String value;

    if (static_cast<tinyxml2::XMLNode*>(impl)->ToElement() != nullptr && static_cast<tinyxml2::XMLNode*>(impl)->Value() != nullptr)
        value = static_cast<tinyxml2::XMLNode*>(impl)->Value();

    return value;
}

//XmlDocument
XmlDocument::XmlDocument() : impl(new Impl)
{
}

XmlDocument::~XmlDocument()
{
    delete impl;
}

void XmlDocument::Load(const Path& fileName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    if (fileName.empty())
    {
        FINJIN_SET_ERROR(error, "Empty file name specified.");
        return;
    }

    if (!fileName.Exists())
    {        
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' does not exist.", fileName));
        return;
    }
    
    auto file = nowide::fopen(fileName.c_str(), "rb");
    if (file == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be opened.", fileName));
        return;
    }

    impl->doc.Clear();
    auto xmlError = impl->doc.LoadFile(file);
    if (xmlError != tinyxml2::XML_NO_ERROR)
    {
        if (impl->doc.GetErrorStr1() != nullptr && impl->doc.GetErrorStr1()[0] != 0)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be parsed: %2%", fileName, impl->doc.GetErrorStr1()));
        else
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be parsed.", fileName));
    }

    fclose(file);
}

void XmlDocument::Save(const Path& fileName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    if (fileName.empty())
    {
        FINJIN_SET_ERROR(error, "Empty file name specified.");
        return;
    }
    
    auto file = nowide::fopen(fileName.c_str(), "w");
    if (file == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be opened/created.", fileName));
        return;
    }

    auto xmlError = impl->doc.SaveFile(file);
    if (xmlError != tinyxml2::XML_NO_ERROR)
    {
        if (impl->doc.GetErrorStr1() != nullptr && impl->doc.GetErrorStr1()[0] != 0)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be saved: %2%", fileName, impl->doc.GetErrorStr1()));
        else
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be saved.", fileName));
    }

    fclose(file);
}

std::shared_ptr<XmlNode> XmlDocument::GetDocumentElement()
{
    std::shared_ptr<XmlNode> node(new XmlNode(&impl->doc));
    return node;
}

std::shared_ptr<XmlNode> XmlDocument::GetRootElement()
{
    std::shared_ptr<XmlNode> node;

    if (impl->doc.RootElement() != nullptr)
        node.reset(new XmlNode(impl->doc.RootElement()));

    return node;
}

void XmlDocument::AddDefaultDeclaration()
{
    impl->doc.InsertEndChild(impl->doc.NewDeclaration());
}

Utf8String XmlDocument::ToString() const
{
    tinyxml2::XMLPrinter printer;
    impl->doc.Accept(&printer);
    return printer.CStr();
}
