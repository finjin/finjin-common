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
#include "finjin/common/WxDocumentWriterOutput.hpp"
#include <ostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class ConfigDocumentLine;

    class WxConfigDocumentWriter
    {
    public:
        WxConfigDocumentWriter();

        void Create(WxDocumentWriterOutput& output, int depth = 0);

        WxConfigDocumentWriter& WriteComment(const wxString& comment);
        WxConfigDocumentWriter& WriteCommentedSection(const wxString& comment, const wxString& name);
        WxConfigDocumentWriter& WriteSection(const wxString& name);
        WxConfigDocumentWriter& WriteScopeStart(const wxString& name);
        WxConfigDocumentWriter& WriteScopeEnd();
        WxConfigDocumentWriter& WriteKeyAndValue(const wxString& key, const wxString& value);
        WxConfigDocumentWriter& WriteLine(const char* line, size_t length);
        WxConfigDocumentWriter& WriteLine(const wxString& line);

        WxConfigDocumentWriter& WriteNewline();

        int GetDepth() const;

    private:
        void Indent();
        void Indent(int depth);

    private:
        WxDocumentWriterOutput* output;
        int depth;
    };

} }
