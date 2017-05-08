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
#include "finjin/common/WxConfigDocumentReader.hpp"
#include <ctype.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static void TrimTrailingWhitespace(wxString& s)
{
    while (!s.empty() && isspace(s.Last()))
        s.RemoveLast();
}

static const char* SkipWhitespace(const char* begin, const char* end)
{
    while (begin != end && isspace(*begin))
        begin++;

    return begin;
}

static bool IsWhitespace(const char* begin, const char* end)
{
    while (begin != end)
    {
        if (!isspace(*begin))
            return false;
        begin++;
    }

    return true;
}


//Implementation----------------------------------------------------------------

//WxConfigDocumentLine
WxConfigDocumentLine::WxConfigDocumentLine()
{
    this->type = Type::NONE;
    this->depth = 0;
    this->lineBegin = this->lineEnd = nullptr;
    this->keyBegin = this->keyEnd = nullptr;
    this->valueBegin = this->valueEnd = nullptr;
}

WxConfigDocumentLine::Type WxConfigDocumentLine::GetType() const
{
    return this->type;
}

int WxConfigDocumentLine::GetDepth() const
{
    return this->depth;
}

void WxConfigDocumentLine::GetComment(wxString& comment) const
{
    if (this->type == Type::COMMENT)
        comment = wxString::FromUTF8(this->keyBegin, this->keyEnd - this->keyBegin);
    else
        comment.clear();
}

wxString WxConfigDocumentLine::GetComment() const
{
    wxString comment;
    GetComment(comment);
    return comment;
}

void WxConfigDocumentLine::GetSectionName(wxString& name) const
{
    if (this->type == Type::SECTION)
        name = wxString::FromUTF8(this->keyBegin, this->keyEnd - this->keyBegin);
    else
        name.clear();
}

wxString WxConfigDocumentLine::GetSectionName() const
{
    wxString name;
    GetSectionName(name);
    return name;
}

wxString WxConfigDocumentLine::GetKey() const
{
    wxString key;
    if (this->type == Type::KEY_AND_VALUE)
        key = wxString::FromUTF8(this->keyBegin, this->keyEnd - this->keyBegin);
    return key;
}

wxString WxConfigDocumentLine::GetValue() const
{
    wxString value;
    if (this->type == Type::KEY_AND_VALUE)
        value = wxString::FromUTF8(this->valueBegin, this->valueEnd - this->valueBegin);
    TrimTrailingWhitespace(value);
    return value;
}

void WxConfigDocumentLine::GetKeyAndValue(wxString& key, wxString& value) const
{
    if (this->type == Type::KEY_AND_VALUE)
    {
        key = wxString::FromUTF8(this->keyBegin, this->keyEnd - this->keyBegin);
        value = wxString::FromUTF8(this->valueBegin, this->valueEnd - this->valueBegin);
        TrimTrailingWhitespace(value);
    }
    else
    {
        key.clear();
        value.clear();
    }
}

void WxConfigDocumentLine::GetLine(wxString& line) const
{
    if (this->type != Type::NONE)
        line = wxString::FromUTF8(this->lineBegin, this->lineEnd - this->lineBegin);
    else
        line.clear();
}

wxString WxConfigDocumentLine::GetLine() const
{
    wxString line;
    GetLine(line);
    return line;
}

//WxConfigDocumentReader
WxConfigDocumentReader::WxConfigDocumentReader()
{
    Reset();
}

void WxConfigDocumentReader::Reset()
{
    this->line.reader = this;

    this->textBegin = this->textEnd = nullptr;
}

WxConfigDocumentLine* WxConfigDocumentReader::Initialize(const char* begin, const char* end)
{
    this->textBegin = begin;
    this->textEnd = end;

    this->line.lineBegin = this->line.lineEnd = this->textBegin;
    this->line.lineBegin = SkipWhitespace(begin, end);

    return ProcessLine();
}

WxConfigDocumentLine* WxConfigDocumentReader::Reinitialize(const WxConfigDocumentLine& line)
{
    this->line = line;
    return Current();
}

WxConfigDocumentLine* WxConfigDocumentReader::Start(const WxByteBuffer& buffer)
{
    return Start(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

WxConfigDocumentLine* WxConfigDocumentReader::Start(const std::vector<uint8_t>& buffer)
{
    return Start(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

WxConfigDocumentLine* WxConfigDocumentReader::Start(const char* text, size_t length)
{
    return Start(text, text + length);
}

WxConfigDocumentLine* WxConfigDocumentReader::Start(const char* begin, const char* end)
{
    return Initialize(begin, end);
}

WxConfigDocumentLine* WxConfigDocumentReader::Restart(const WxConfigDocumentLine& line)
{
    return Reinitialize(line);
}

WxConfigDocumentLine* WxConfigDocumentReader::Current()
{
    if (this->line.GetType() != WxConfigDocumentLine::Type::NONE)
        return &this->line;
    else
        return nullptr;
}

WxConfigDocumentLine* WxConfigDocumentReader::Next()
{
    this->line.lineBegin = this->line.lineEnd;
    if (this->line.lineBegin != this->textEnd)
        this->line.lineBegin = SkipWhitespace(this->line.lineBegin, this->textEnd);

    return ProcessLine();
}

void WxConfigDocumentReader::SkipScope()
{
    int depth = 0;

    for (auto line = Current(); line != nullptr; line = Next())
    {
        switch (line->GetType())
        {
            case WxConfigDocumentLine::Type::SCOPE_START:
            {
                depth++;
                break;
            }
            case WxConfigDocumentLine::Type::SCOPE_END:
            {
                depth--;
                if (depth == 0)
                    return;
                break;
            }
            default: break;
        }
    }
}

size_t WxConfigDocumentReader::GetSectionCount(const wxString& name)
{
    size_t count = 0;

    auto startLine = this->line;

    auto depth = this->line.depth;

    for (auto line = Current(); line != nullptr; line = Next())
    {
        switch (line->GetType())
        {
            case WxConfigDocumentLine::Type::SECTION:
            {
                if (name.empty())
                    count++;
                else
                {
                    wxString sectionName;
                    line->GetSectionName(sectionName);
                    if (sectionName == name)
                        count++;
                }
                break;
            }
            case WxConfigDocumentLine::Type::SCOPE_START:
            {
                depth++;
                break;
            }
            case WxConfigDocumentLine::Type::SCOPE_END:
            {
                depth--;
                if (depth < startLine.depth)
                {
                    Reinitialize(startLine);
                    return count;
                }
                break;
            }
            default: break;
        }
    }

    Reinitialize(startLine);
    return count;
}

size_t WxConfigDocumentReader::GetItemCount(const wxString& name)
{
    size_t count = 0;

    auto startLine = this->line;

    auto depth = this->line.depth;

    for (auto line = Current(); line != nullptr; line = Next())
    {
        switch (line->GetType())
        {
            case WxConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                if (name.empty())
                    count++;
                else
                {
                    auto key = line->GetKey();
                    if (key == name)
                        count++;
                }
                break;
            }
            case WxConfigDocumentLine::Type::SCOPE_START:
            {
                depth++;
                break;
            }
            case WxConfigDocumentLine::Type::SCOPE_END:
            {
                depth--;
                if (depth < startLine.depth)
                {
                    Reinitialize(startLine);
                    return count;
                }
                break;
            }
            default: break;
        }
    }

    Reinitialize(startLine);
    return count;
}

WxConfigDocumentLine* WxConfigDocumentReader::ProcessLine()
{
    if (this->line.lineBegin == this->textEnd)
        return nullptr;

    const char* hash = nullptr;
    const char* carat = nullptr;
    const char* equal = nullptr;
    const char* sectionStartBracket = nullptr;
    const char* sectionEndBracket = nullptr;
    const char* scopeStartBracket = nullptr;
    const char* scopeEndBracket = nullptr;

    for (auto i = this->line.lineBegin; i <= this->textEnd; i++)
    {
        if (i == this->textEnd || *i == '\r' || *i == '\n')
        {
            this->line.lineEnd = i;

            if (hash == this->line.lineBegin)
            {
                this->line.type = WxConfigDocumentLine::Type::COMMENT;
                this->line.keyBegin = this->line.lineBegin + 1;
                this->line.keyEnd = this->line.lineEnd;
            }
            else if (sectionStartBracket == this->line.lineBegin && sectionEndBracket != nullptr)
            {
                this->line.type = WxConfigDocumentLine::Type::SECTION;
                this->line.keyBegin = this->line.lineBegin + 1;
                this->line.keyEnd = sectionEndBracket;
            }
            else if (scopeStartBracket == line.lineBegin && IsWhitespace(scopeStartBracket + 1, this->line.lineEnd))
            {
                this->line.type = WxConfigDocumentLine::Type::SCOPE_START;
                this->line.keyBegin = this->line.lineBegin;
                this->line.keyEnd = this->line.lineBegin + 1;
                this->line.depth++;
            }
            else if (scopeEndBracket == line.lineBegin && IsWhitespace(scopeEndBracket + 1, this->line.lineEnd))
            {
                this->line.type = WxConfigDocumentLine::Type::SCOPE_END;
                this->line.keyBegin = this->line.lineBegin;
                this->line.keyEnd = this->line.lineBegin + 1;
                this->line.depth--;
            }
            else if (equal != nullptr)
            {
                auto isMultiline = carat != nullptr && carat + 1 == equal;

                this->line.type = WxConfigDocumentLine::Type::KEY_AND_VALUE;
                this->line.keyBegin = this->line.lineBegin;
                this->line.keyEnd = isMultiline ? carat : equal;
                this->line.valueBegin = equal + 1;
                if (isMultiline)
                {
                    for (auto j = i + 1; j < this->textEnd - 3; j++)
                    {
                        if (j[0] == '\n' && j[1] == '^' && (j[2] == '\r' || j[2] == '\n'))
                        {
                            this->line.valueEnd = j;
                            break;
                        }
                    }
                }
                else
                    this->line.valueEnd = this->line.lineEnd;
            }
            else
            {
                this->line.type = WxConfigDocumentLine::Type::PLAIN_LINE;
                this->line.keyBegin = this->line.lineBegin;
                this->line.keyEnd = this->line.lineEnd;
            }

            return &this->line;
        }
        else if (*i == '#')
        {
            if (hash == nullptr)
                hash = i;
        }
        else if (*i == '^')
        {
            if (carat == nullptr)
                carat = i;
        }
        else if (*i == '=')
        {
            if (equal == nullptr)
                equal = i;
        }
        else if (*i == '[')
        {
            if (sectionStartBracket == nullptr)
                sectionStartBracket = i;
        }
        else if (*i == ']')
        {
            if (sectionStartBracket != nullptr && sectionEndBracket == nullptr)
                sectionEndBracket = i;
        }
        else if (*i == '{')
        {
            if (scopeStartBracket == nullptr)
                scopeStartBracket = i;
        }
        else if (*i == '}')
        {
            if (scopeEndBracket == nullptr)
                scopeEndBracket = i;
        }
    }

    this->line.type = WxConfigDocumentLine::Type::NONE;
    return nullptr;
}
