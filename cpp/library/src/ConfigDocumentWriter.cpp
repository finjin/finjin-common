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
#include "finjin/common/ConfigDocumentWriter.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
ConfigDocumentWriter::ConfigDocumentWriter()
{
    this->output = nullptr;
    this->depth = 0;
}

void ConfigDocumentWriter::Create(DocumentWriterOutput& output, int depth)
{
    this->output = &output;
    this->depth = depth;
}

void ConfigDocumentWriter::Create(std::ostream& outStream, int depth)
{
    this->streamOutput = StreamDataChunkWriterOutput(outStream);
    this->output = &this->streamOutput;
    this->depth = depth;
}

ConfigDocumentWriter& ConfigDocumentWriter::WriteScopeEnd()
{
    if (this->depth > 0)
        this->depth--;

    Indent();
    this->output->Write("}");
    this->output->Write("\n");

    return *this;
}

ConfigDocumentWriter& ConfigDocumentWriter::WriteLine(const char* line, size_t length)
{
    Indent();
    this->output->Write(line, length);
    this->output->Write("\n");
    return *this;
}

ConfigDocumentWriter& ConfigDocumentWriter::WriteLine(const ConfigDocumentLine& line)
{
    switch (line.GetType())
    {
        case ConfigDocumentLine::Type::NONE: break;
        case ConfigDocumentLine::Type::COMMENT:
        {
            Utf8StringView comment;
            line.GetComment(comment);

            Indent(line.GetDepth());
            this->output->Write("#");
            this->output->WriteString(comment);
            this->output->Write("\n");

            break;
        }
        case ConfigDocumentLine::Type::SECTION:
        {
            Utf8StringView sectionName;
            line.GetSectionName(sectionName);

            Indent(line.GetDepth());
            this->output->Write("[");
            this->output->WriteString(sectionName);
            this->output->Write("]");
            this->output->Write("\n");

            break;
        }
        case ConfigDocumentLine::Type::SCOPE_START:
        {
            Indent(line.GetDepth());
            this->output->Write("{");
            this->output->Write("\n");

            break;
        }
        case ConfigDocumentLine::Type::SCOPE_END:
        {
            Indent(line.GetDepth());
            this->output->Write("}");
            this->output->Write("\n");

            break;
        }
        case ConfigDocumentLine::Type::KEY_AND_VALUE:
        {
            Utf8StringView key, value;
            line.GetKeyAndValue(key, value);

            Indent(line.GetDepth());
            this->output->WriteString(key);
            this->output->Write("=");
            this->output->WriteString(value);
            this->output->Write("\n");

            break;
        }
        case ConfigDocumentLine::Type::PLAIN_LINE:
        {
            Utf8StringView plainLine;
            line.GetLine(plainLine);

            Indent(line.GetDepth());
            this->output->WriteString(plainLine);
            this->output->Write("\n");

            break;
        }
    }
    return *this;
}

ConfigDocumentWriter& ConfigDocumentWriter::WriteNewline()
{
    this->output->Write("\n");
    return *this;
}

int ConfigDocumentWriter::GetDepth() const
{
    return this->depth;
}

void ConfigDocumentWriter::Indent()
{
    Indent(this->depth);
}

void ConfigDocumentWriter::Indent(int depth)
{
    for (int i = 0; i < depth; i++)
        this->output->Write("\t");
}
