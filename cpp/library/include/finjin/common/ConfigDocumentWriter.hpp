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


//Includes---------------------------------------------------------------------
#include "finjin/common/Error.hpp"
#include "finjin/common/DocumentWriterOutput.hpp"
#include <ostream>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class ConfigDocumentLine;

    class ConfigDocumentWriter
    {
    public:
        ConfigDocumentWriter();
        
        void Create(DocumentWriterOutput& output, int depth = 0);
        void Create(std::ostream& outStream, int depth = 0);

        ConfigDocumentWriter& WriteComment(const Utf8String& comment);
        ConfigDocumentWriter& WriteCommentedSection(const Utf8String& comment, const Utf8String& name);
        ConfigDocumentWriter& WriteSection(const Utf8String& name);
        ConfigDocumentWriter& WriteScopeStart(const Utf8String& name);
        ConfigDocumentWriter& WriteScopeEnd();
        ConfigDocumentWriter& WriteKeyAndValue(const Utf8String& key, const Utf8String& value);
        ConfigDocumentWriter& WriteLine(const Utf8String& line);
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
