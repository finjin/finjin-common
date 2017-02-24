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
#include "finjin/common/JsonDataDocumentWriter.hpp"
#include "finjin/common/JsonDataElementWriter.hpp"
#include "finjin/common/JsonDocumentImpl.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"
#include <nowide/fstream.hpp>

using namespace Finjin::Common;


//Local classes----------------------------------------------------------------
struct JsonDataDocumentWriter::Impl
{
    Impl() : writer(buffer)
    {
    }

    Utf8String rootName;
    JsonStringBuffer buffer;
    JsonStringWriter writer;
    bool addExtraObjectAtRoot;
};


//Implementation---------------------------------------------------------------
JsonDataDocumentWriter::JsonDataDocumentWriter(bool addExtraObjectAtRoot) : impl(new Impl)
{    
    impl->addExtraObjectAtRoot = addExtraObjectAtRoot;
}

JsonDataDocumentWriter::~JsonDataDocumentWriter()
{
    delete impl;
}

std::shared_ptr<DataElementWriter> JsonDataDocumentWriter::StartRootElement(const Utf8String& name)
{
    impl->rootName = name;
    
    impl->writer.StartObject();

    impl->writer.Key(impl->rootName.c_str());        

    if (impl->addExtraObjectAtRoot)
        impl->writer.StartObject();
    std::shared_ptr<DataElementWriter> root(new JsonDataElementWriter(&impl->writer));    
    return root;
}

void JsonDataDocumentWriter::EndRootElement()
{
    if (impl->addExtraObjectAtRoot)
        impl->writer.EndObject();

    impl->writer.EndObject();
}

void JsonDataDocumentWriter::SaveFile(const Path& fileName, Error& error) 
{
    FINJIN_ERROR_METHOD_START(error);

    if (fileName.empty())
    {
        FINJIN_SET_ERROR(error, "Empty file name specified.");
        return;
    }

    nowide::ofstream file(fileName.c_str(), nowide::ofstream::out | nowide::ofstream::binary);
    if (!file)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("File '%1%' could not be opened/created.", fileName));
        return;
    }

    file << impl->buffer.GetString();

    if (!file)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There was an error while saving JSON data to '%1%'.", fileName));
        return;
    }
}

void JsonDataDocumentWriter::GetString(Utf8String& result, Error& error) 
{
    FINJIN_ERROR_METHOD_START(error);

    result = impl->buffer.GetString();
}
