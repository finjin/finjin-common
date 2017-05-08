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
#include "finjin/common/WxConfigDocumentWriter.hpp"
#include <ctype.h>

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
WxConfigDocumentWriter::WxConfigDocumentWriter()
{
    this->output = nullptr;
    this->depth = 0;
}

void WxConfigDocumentWriter::Create(WxDocumentWriterOutput& output, int depth)
{
    this->output = &output;
    this->depth = depth;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteComment(const wxString& comment)
{
    Indent();
    this->output->Write("#");
    this->output->WriteString(comment);
    this->output->Write("\n");
    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteCommentedSection(const wxString& comment, const wxString& name)
{
    Indent();
    this->output->Write("\n");
    this->output->Write("#");
    this->output->WriteString(comment);
    this->output->Write("\n");

    Indent();
    this->output->Write("[");
    this->output->WriteString(name);
    this->output->Write("]");
    this->output->Write("\n");

    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteSection(const wxString& name)
{
    this->output->Write("\n");

    Indent();
    this->output->Write("[");
    this->output->WriteString(name);
    this->output->Write("]");
    this->output->Write("\n");

    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteScopeStart(const wxString& name)
{
    Indent();
    this->output->Write("[");
    this->output->WriteString(name);
    this->output->Write("]");
    this->output->Write("\n");

    Indent();
    this->output->Write("{");
    this->output->Write("\n");

    this->depth++;

    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteScopeEnd()
{
    if (this->depth > 0)
        this->depth--;

    Indent();
    this->output->Write("}");
    this->output->Write("\n");

    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteKeyAndValue(const wxString& key, const wxString& value)
{
    auto containsNewline = value.find('\n') != wxString::npos;

    Indent();
    this->output->WriteString(key);
    if (containsNewline)
        this->output->Write("^=");
    else
        this->output->Write("=");
    this->output->WriteString(value);
    this->output->Write("\n");
    if (containsNewline)
        this->output->Write("^\n");
    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteLine(const char* line, size_t length)
{
    Indent();
    this->output->Write(line, length);
    this->output->Write("\n");
    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteLine(const wxString& line)
{
    Indent();
    this->output->WriteString(line);
    this->output->Write("\n");
    return *this;
}

WxConfigDocumentWriter& WxConfigDocumentWriter::WriteNewline()
{
    this->output->Write("\n");
    return *this;
}

int WxConfigDocumentWriter::GetDepth() const
{
    return this->depth;
}

void WxConfigDocumentWriter::Indent()
{
    Indent(this->depth);
}

void WxConfigDocumentWriter::Indent(int depth)
{
    for (int i = 0; i < depth; i++)
        this->output->Write("\t");
}
