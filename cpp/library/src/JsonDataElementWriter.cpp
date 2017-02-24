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
#include "finjin/common/JsonDataElementWriter.hpp"
#include "finjin/common/JsonDocumentImpl.hpp"

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
JsonDataElementWriter::JsonDataElementWriter(void* _writer) : writer(_writer)
{   
}

JsonDataElementWriter::~JsonDataElementWriter()
{    
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, const Utf8String& value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->String(value.c_str());
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, const char* value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->String(value);
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, int value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->Int(value);
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, unsigned int value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->Uint(value);
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, int64_t value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->Int64(value);
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, uint64_t value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->Uint64(value);
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, double value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->Double(value);
}

void JsonDataElementWriter::SetAttribute(const Utf8String& name, bool value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->Bool(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, const Utf8String& value)
{
    static_cast<JsonStringWriter*>(this->writer)->String(value.c_str());
}

void JsonDataElementWriter::AddValue(const Utf8String& name, const char* value)
{
    static_cast<JsonStringWriter*>(this->writer)->String(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, int value)
{
    static_cast<JsonStringWriter*>(this->writer)->Int(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, unsigned int value)
{
    static_cast<JsonStringWriter*>(this->writer)->Uint(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, int64_t value)
{
    static_cast<JsonStringWriter*>(this->writer)->Int64(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, uint64_t value)
{
    static_cast<JsonStringWriter*>(this->writer)->Uint64(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, double value)
{
    static_cast<JsonStringWriter*>(this->writer)->Double(value);
}

void JsonDataElementWriter::AddValue(const Utf8String& name, bool value)
{
    static_cast<JsonStringWriter*>(this->writer)->Bool(value);
}

std::shared_ptr<DataElementWriter> JsonDataElementWriter::StartChildElement(const Utf8String& name)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->StartObject();
    return std::shared_ptr<DataElementWriter>(new JsonDataElementWriter(this->writer));
}

void JsonDataElementWriter::EndChildElement()
{
    static_cast<JsonStringWriter*>(this->writer)->EndObject();
}

std::shared_ptr<DataElementWriter> JsonDataElementWriter::StartChildArray(const Utf8String& name)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->StartArray();
    return std::shared_ptr<DataElementWriter>(new JsonDataElementWriter(this->writer));
}

void JsonDataElementWriter::EndChildArray()
{
    static_cast<JsonStringWriter*>(this->writer)->EndArray();
}

std::shared_ptr<DataElementWriter> JsonDataElementWriter::StartObject(const Utf8String& name)
{
    //Ignore the name
    static_cast<JsonStringWriter*>(this->writer)->StartObject();
    return std::shared_ptr<DataElementWriter>(new JsonDataElementWriter(this->writer));
}

void JsonDataElementWriter::EndObject()
{
    static_cast<JsonStringWriter*>(this->writer)->EndObject();
}

void JsonDataElementWriter::AddChildText(const Utf8String& name, const Utf8String& value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->String(value.c_str());    
}

void JsonDataElementWriter::AddSimpleChildText(const Utf8String& name, const Utf8String& value)
{
    static_cast<JsonStringWriter*>(this->writer)->Key(name.c_str());
    static_cast<JsonStringWriter*>(this->writer)->String(value.c_str());    
}
