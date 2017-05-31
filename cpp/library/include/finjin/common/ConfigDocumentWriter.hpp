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


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/DocumentWriterOutput.hpp"
#include "finjin/common/Error.hpp"
#include <ostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class ConfigDocumentLine;

    class ConfigDocumentWriter
    {
    public:
        ConfigDocumentWriter();

        void Create(DocumentWriterOutput& output, int depth = 0);
        void Create(std::ostream& outStream, int depth = 0);

        template <typename StringType>
        ConfigDocumentWriter& WriteComment(const StringType& comment)
        {
            Indent();
            this->output->Write("#");
            this->output->WriteString(comment);
            this->output->Write("\n");
            return *this;
        }

        template <typename CommentStringType, typename NameStringType>
        ConfigDocumentWriter& WriteCommentedSection(const CommentStringType& comment, const NameStringType& name)
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

        template <typename StringType>
        ConfigDocumentWriter& WriteSection(const StringType& name)
        {
            this->output->Write("\n");

            Indent();
            this->output->Write("[");
            this->output->WriteString(name);
            this->output->Write("]");
            this->output->Write("\n");

            return *this;
        }

        template <typename StringType>
        ConfigDocumentWriter& WriteScopeStart(const StringType& name)
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

        ConfigDocumentWriter& WriteScopeEnd();

        template <typename KeyType, typename ValueType>
        ConfigDocumentWriter& WriteKeyAndValue(const KeyType& key, const ValueType& _value, bool testForNewline = true)
        {
            Utf8StringView value(_value);
            
            auto containsNewline = false;
            if (testForNewline)
                containsNewline = value.find('\n') != Utf8String::npos;

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

        template <typename KeyType, typename ValueType>
        ConfigDocumentWriter& WriteKeyAndValues(const KeyType& key, const ValueType* values, size_t count, bool testForNewline = true)
        {
            auto containsNewline = false;
            if (testForNewline)
            {
                for (size_t valueIndex = 0; valueIndex < count; valueIndex++)
                {
                    Utf8StringView valueView(values[valueIndex]);
                    if (valueView.find('\n') != Utf8String::npos)
                    {
                        containsNewline = true;
                        break;
                    }
                }
            }
            
            Indent();
            this->output->WriteString(key);
            if (containsNewline)
                this->output->Write("^=");
            else
                this->output->Write("=");
            for (size_t valueIndex = 0; valueIndex < count; valueIndex++)
            {
                if (valueIndex > 0)
                    this->output->WriteString(" ");
                this->output->WriteString(values[valueIndex]);
            }
            this->output->Write("\n");
            if (containsNewline)
                this->output->Write("^\n");
            return *this;
        }

        template <typename StringType>
        ConfigDocumentWriter& WriteLine(const StringType& line)
        {
            Indent();
            this->output->WriteString(line);
            this->output->Write("\n");
            return *this;
        }

        ConfigDocumentWriter& WriteLine(const char* line, size_t length);
        ConfigDocumentWriter& WriteLine(const ConfigDocumentLine& line);

        ConfigDocumentWriter& WriteNewline();

        int GetDepth() const;

    private:
        void Indent();
        void Indent(int depth);

    private:
        StreamDataChunkWriterOutput streamOutput;
        DocumentWriterOutput* output;
        int depth;
    };

} }
