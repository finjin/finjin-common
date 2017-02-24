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
#include "finjin/common/XmlDataDocumentWriter.hpp"
#include "finjin/common/XmlDataElementWriter.hpp"
#include "finjin/common/XmlDocument.hpp"

using namespace Finjin::Common;


//Local classes----------------------------------------------------------------
struct XmlDataDocumentWriter::Impl
{
    Impl()
    {
        this->xmlDoc.AddDefaultDeclaration();        
        this->xmlRootElement = this->xmlDoc.GetDocumentElement();
    }

    XmlDocument xmlDoc;
    std::shared_ptr<XmlNode> xmlRootElement;
};


//Implementation---------------------------------------------------------------
XmlDataDocumentWriter::XmlDataDocumentWriter() : impl(new Impl)
{    
}

XmlDataDocumentWriter::~XmlDataDocumentWriter()
{
    delete impl;
}

std::shared_ptr<DataElementWriter> XmlDataDocumentWriter::StartRootElement(const Utf8String& name)
{
    std::shared_ptr<DataElementWriter> childElement;

    if (name.empty())
        childElement.reset(new XmlDataElementWriter(impl->xmlDoc, impl->xmlRootElement));
    else
        childElement.reset(new XmlDataElementWriter(impl->xmlDoc, impl->xmlRootElement->AddChildElement(name)));
    
    return childElement;
}

void XmlDataDocumentWriter::EndRootElement()
{
}

void XmlDataDocumentWriter::SaveFile(const Path& fileName, Error& error) 
{
    FINJIN_ERROR_METHOD_START(error);

    impl->xmlDoc.Save(fileName, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void XmlDataDocumentWriter::GetString(Utf8String& result, Error& error) 
{
    FINJIN_ERROR_METHOD_START(error);

    result = impl->xmlDoc.ToString();    
}
