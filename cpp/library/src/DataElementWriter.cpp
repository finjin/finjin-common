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
#include "finjin/common/DataElementWriter.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
void DataElementWriter::SetAttribute(const Utf8String& name, const char* value)
{
    Utf8String stringValue;
    if (value != nullptr)
        stringValue = value;
    SetAttribute(name, stringValue);
}

void DataElementWriter::SetAttribute(const Utf8String& name, int value)
{
    SetAttribute(name, Convert::ToString(value));
}

void DataElementWriter::SetAttribute(const Utf8String& name, unsigned int value)
{
    SetAttribute(name, Convert::ToString(value));
}

void DataElementWriter::SetAttribute(const Utf8String& name, int64_t value)
{
    SetAttribute(name, Convert::ToString(value));
}
 
void DataElementWriter::SetAttribute(const Utf8String& name, uint64_t value)
{
    SetAttribute(name, Convert::ToString(value));
}

void DataElementWriter::SetAttribute(const Utf8String& name, double value)
{
    SetAttribute(name, Convert::ToString(value));
}

void DataElementWriter::SetAttribute(const Utf8String& name, bool value)
{
    static const Utf8String trueString("true");
    static const Utf8String falseString("false");    
    SetAttribute(name, value ? trueString : falseString);
}

void DataElementWriter::AddValue(const Utf8String& name, const char* value)
{
    Utf8String stringValue;
    if (value != nullptr)
        stringValue = value;
    AddValue(name, stringValue);
}

void DataElementWriter::AddValue(const Utf8String& name, int value)
{
    AddValue(name, Convert::ToString(value));
}

void DataElementWriter::AddValue(const Utf8String& name, unsigned int value)
{
    AddValue(name, Convert::ToString(value));
}

void DataElementWriter::AddValue(const Utf8String& name, int64_t value)
{
    AddValue(name, Convert::ToString(value));
}
 
void DataElementWriter::AddValue(const Utf8String& name, uint64_t value)
{
    AddValue(name, Convert::ToString(value));
}

void DataElementWriter::AddValue(const Utf8String& name, double value)
{
    AddValue(name, Convert::ToString(value));
}

void DataElementWriter::AddValue(const Utf8String& name, bool value)
{
    static const Utf8String trueString("true");
    static const Utf8String falseString("false");    
    AddValue(name, value ? trueString : falseString);
}
