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
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/ByteBuffer.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//ConfigDocumentLine
ConfigDocumentLine::ConfigDocumentLine()
{
    this->type = Type::NONE;
    this->depth = 0;
    this->lineBegin = this->lineEnd = nullptr;
    this->keyBegin = this->keyEnd = nullptr;
    this->valueBegin = this->valueEnd = nullptr;
}

ConfigDocumentLine::Type ConfigDocumentLine::GetType() const
{
    return this->type;
}

int ConfigDocumentLine::GetDepth() const
{
    return this->depth;
}

void ConfigDocumentLine::GetComment(Utf8String& comment) const
{
    if (this->type == Type::COMMENT)
        comment.assign(this->keyBegin, this->keyEnd);
    else
        comment.clear();
}

Utf8String ConfigDocumentLine::GetComment() const
{
    Utf8String comment;
    GetComment(comment);
    return comment;
}

void ConfigDocumentLine::GetSectionName(Utf8String& name) const
{
    if (this->type == Type::SECTION)
        name.assign(this->keyBegin, this->keyEnd);
    else
        name.clear();
}

Utf8String ConfigDocumentLine::GetSectionName() const
{
    Utf8String name;
    GetSectionName(name);
    return name;
}

Utf8String ConfigDocumentLine::GetKey() const
{
    Utf8String key;
    if (this->type == Type::KEY_AND_VALUE)
        key.assign(this->keyBegin, this->keyEnd);
    return key;
}

Utf8String ConfigDocumentLine::GetValue() const
{
    Utf8String value;
    if (this->type == Type::KEY_AND_VALUE)
        value.assign(this->valueBegin, this->valueEnd);
    value.TrimTrailingWhitespace();
    return value;
}

void ConfigDocumentLine::GetKeyAndValue(Utf8String& key, Utf8String& value) const
{
    if (this->type == Type::KEY_AND_VALUE)
    {
        key.assign(this->keyBegin, this->keyEnd);
        value.assign(this->valueBegin, this->valueEnd);
        value.TrimTrailingWhitespace();
    }
    else
    {
        key.clear();
        value.clear();
    }
}

void ConfigDocumentLine::GetKeyAndValue(Utf8StringView& key, Utf8StringView& value) const
{
    if (this->type == Type::KEY_AND_VALUE)
    {
        key.assign(this->keyBegin, this->keyEnd);
        value.assign(this->valueBegin, this->valueEnd);
        value.TrimTrailingWhitespace();
    }
    else
    {
        key.clear();
        value.clear();
    }
}

void ConfigDocumentLine::GetLine(Utf8String& line) const
{
    if (this->type != Type::NONE)
        line.assign(this->lineBegin, this->lineEnd);
    else
        line.clear();
}

Utf8String ConfigDocumentLine::GetLine() const
{
    Utf8String line;
    GetLine(line);
    return line;
}

Utf8StringView& ConfigDocumentLine::GetComment(Utf8StringView& comment) const
{
    size_t len;
    auto beginPointer = GetComment(len);
    comment.assign(beginPointer, len);
    return comment;
}

Utf8StringView& ConfigDocumentLine::GetSectionName(Utf8StringView& name) const
{
    size_t len;
    auto beginPointer = GetSectionName(len);
    name.assign(beginPointer, len);
    return name;
}

Utf8StringView& ConfigDocumentLine::GetKey(Utf8StringView& key) const
{
    size_t len;
    auto beginPointer = GetKey(len);
    key.assign(beginPointer, len);
    return key;
}

Utf8StringView& ConfigDocumentLine::GetValue(Utf8StringView& value) const
{
    size_t len;
    auto beginPointer = GetValue(len);
    value.assign(beginPointer, len);
    value.TrimTrailingWhitespace();
    return value;
}

Utf8StringView& ConfigDocumentLine::GetLine(Utf8StringView& line) const
{
    size_t len;
    auto beginPointer = GetLine(len);
    line.assign(beginPointer, len);
    return line;
}

const char* ConfigDocumentLine::GetComment(size_t& length) const
{
    if (this->type == Type::COMMENT)
    {
        length = this->keyEnd - this->keyBegin;
        return this->keyBegin;
    }
    else
    {
        length = 0;
        return nullptr;
    }
}

const char* ConfigDocumentLine::GetScope(size_t& length) const
{
    if (this->type == Type::SCOPE_START || this->type == Type::SCOPE_END)
    {
        length = this->keyEnd - this->keyBegin;
        return this->keyBegin;
    }
    else
    {
        length = 0;
        return nullptr;
    }
}

const char* ConfigDocumentLine::GetSectionName(size_t& length) const
{
    if (this->type == Type::SECTION)
    {
        length = this->keyEnd - this->keyBegin;
        return this->keyBegin;
    }
    else
    {
        length = 0;
        return nullptr;
    }
}

const char* ConfigDocumentLine::GetKey(size_t& length) const
{
    if (this->type == Type::KEY_AND_VALUE)
    {
        length = this->keyEnd - this->keyBegin;
        return this->keyBegin;
    }
    else
    {
        length = 0;
        return nullptr;
    }
}

const char* ConfigDocumentLine::GetValue(size_t& length) const
{
    if (this->type == Type::KEY_AND_VALUE)
    {
        length = this->valueEnd - this->valueBegin;
        return this->valueBegin;
    }
    else
    {
        length = 0;
        return nullptr;
    }
}

const char* ConfigDocumentLine::GetLine(size_t& length) const
{
    if (this->type != Type::NONE)
    {
        length = this->lineEnd - this->lineBegin;
        return this->lineBegin;
    }
    else
    {
        length = 0;
        return nullptr;
    }
}

//ConfigDocumentReader
ConfigDocumentReader::ConfigDocumentReader()
{
    Reset();
}

void ConfigDocumentReader::Reset()
{
    this->line.reader = this;

    this->textBegin = this->textEnd = nullptr;
}

ConfigDocumentLine* ConfigDocumentReader::Initialize(const char* begin, const char* end)
{
    this->textBegin = begin;
    this->textEnd = end;

    this->line.lineBegin = this->line.lineEnd = this->textBegin;
    this->line.lineBegin = Utf8String::SkipWhitespace(begin, end);

    return ProcessLine();
}

ConfigDocumentLine* ConfigDocumentReader::Reinitialize(const ConfigDocumentLine& line)
{
    this->line = line;
    return Current();
}

ConfigDocumentLine* ConfigDocumentReader::Start(const ByteBufferReader& buffer)
{
    return Start(reinterpret_cast<const char*>(buffer.data_start()), buffer.max_size());
}

ConfigDocumentLine* ConfigDocumentReader::Start(const Utf8String& s)
{
    return Start(s.begin(), s.end());
}

ConfigDocumentLine* ConfigDocumentReader::Start(const char* text, size_t length)
{
    return Start(text, text + length);
}

ConfigDocumentLine* ConfigDocumentReader::Start(const char* begin, const char* end)
{
    return Initialize(begin, end);
}

ConfigDocumentLine* ConfigDocumentReader::Restart(const ConfigDocumentLine& line)
{
    return Reinitialize(line);
}

ConfigDocumentLine* ConfigDocumentReader::Current()
{
    if (this->line.GetType() != ConfigDocumentLine::Type::NONE)
        return &this->line;
    else
        return nullptr;
}

ConfigDocumentLine* ConfigDocumentReader::Next()
{
    this->line.lineBegin = this->line.lineEnd;
    if (this->line.lineBegin != this->textEnd)
        this->line.lineBegin = Utf8String::SkipWhitespace(this->line.lineBegin, this->textEnd);

    return ProcessLine();
}

void ConfigDocumentReader::SkipScope()
{
    int depth = 0;

    for (auto line = Current(); line != nullptr; line = Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                depth++;
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_END:
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

ConfigDocumentLine* ConfigDocumentReader::ProcessLine()
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
                this->line.type = ConfigDocumentLine::Type::COMMENT;
                this->line.keyBegin = this->line.lineBegin + 1;
                this->line.keyEnd = this->line.lineEnd;
            }
            else if (sectionStartBracket == this->line.lineBegin && sectionEndBracket != nullptr)
            {
                this->line.type = ConfigDocumentLine::Type::SECTION;
                this->line.keyBegin = this->line.lineBegin + 1;
                this->line.keyEnd = sectionEndBracket;
            }
            else if (scopeStartBracket == line.lineBegin && (scopeStartBracket + 1 == this->line.lineEnd || Utf8String::IsWhitespace(scopeStartBracket + 1, this->line.lineEnd)))
            {
                this->line.type = ConfigDocumentLine::Type::SCOPE_START;
                this->line.keyBegin = this->line.lineBegin;
                this->line.keyEnd = this->line.lineBegin + 1;
                this->line.depth++;
            }
            else if (scopeEndBracket == line.lineBegin && (scopeEndBracket + 1 == this->line.lineEnd || Utf8String::IsWhitespace(scopeEndBracket + 1, this->line.lineEnd)))
            {
                this->line.type = ConfigDocumentLine::Type::SCOPE_END;
                this->line.keyBegin = this->line.lineBegin;
                this->line.keyEnd = this->line.lineBegin + 1;
                this->line.depth--;
            }
            else if (equal != nullptr)
            {
                auto isMultiline = carat != nullptr && carat + 1 == equal;

                this->line.type = ConfigDocumentLine::Type::KEY_AND_VALUE;
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
                this->line.type = ConfigDocumentLine::Type::PLAIN_LINE;
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

    this->line.type = ConfigDocumentLine::Type::NONE;
    return nullptr;
}
