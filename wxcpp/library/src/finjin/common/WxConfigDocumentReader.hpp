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
#include "finjin/common/WxByteBuffer.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class WxConfigDocumentReader;
    class WxByteBuffer;

    class WxConfigDocumentLine
    {
        friend class WxConfigDocumentReader;

    public:
        WxConfigDocumentLine();

        enum class Type
        {
            NONE,
            COMMENT,
            SECTION,
            SCOPE_START,
            SCOPE_END,
            KEY_AND_VALUE,
            PLAIN_LINE
        };

        Type GetType() const;

        int GetDepth() const;

        //String methods
        void GetComment(wxString& comment) const;
        wxString GetComment() const;

        void GetSectionName(wxString& name) const;
        wxString GetSectionName() const;

        wxString GetKey() const;
        wxString GetValue() const;
        void GetKeyAndValue(wxString& key, wxString& value) const;

        void GetLine(wxString& line) const;
        wxString GetLine() const;

    private:
        WxConfigDocumentReader* reader;

        Type type;

        int depth;

        const char* lineBegin;
        const char* lineEnd;

        const char* keyBegin;
        const char* keyEnd;

        const char* valueBegin;
        const char* valueEnd;
    };

    class WxConfigDocumentReader
    {
    public:
        WxConfigDocumentReader();

        void Reset();

        WxConfigDocumentLine* Start(const WxByteBuffer& buffer);
        WxConfigDocumentLine* Start(const std::vector<uint8_t>& buffer);
        WxConfigDocumentLine* Start(const char* text, size_t length);
        WxConfigDocumentLine* Start(const char* begin, const char* end);
        WxConfigDocumentLine* Restart(const WxConfigDocumentLine& line);

        WxConfigDocumentLine* Current();
        WxConfigDocumentLine* Next();

        void SkipScope();

        size_t GetSectionCount(const wxString& name);
        size_t GetItemCount(const wxString& name);

    private:
        WxConfigDocumentLine* Initialize(const char* begin, const char* end);
        WxConfigDocumentLine* Reinitialize(const WxConfigDocumentLine& line);

        WxConfigDocumentLine* ProcessLine();

    private:
        WxConfigDocumentLine line;

        const char* textBegin;
        const char* textEnd;
    };

} }
