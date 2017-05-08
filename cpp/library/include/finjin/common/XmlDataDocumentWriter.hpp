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
#include "finjin/common/DataDocumentWriter.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API XmlDataDocumentWriter : public DataDocumentWriter
    {
    public:
        XmlDataDocumentWriter(const XmlDataDocumentWriter& other) = delete;
        XmlDataDocumentWriter& operator = (const XmlDataDocumentWriter& other) = delete;

        XmlDataDocumentWriter(XmlDataDocumentWriter&& other) = delete;
        XmlDataDocumentWriter& operator = (XmlDataDocumentWriter&& other) = delete;

    public:
        XmlDataDocumentWriter();
        ~XmlDataDocumentWriter();

        std::shared_ptr<DataElementWriter> StartRootElement(const Utf8String& name) override;
        void EndRootElement() override;

        void SaveFile(const Path& fileName, Error& error) override;

        void GetString(Utf8String& result, Error& error) override;

    private:
        struct Impl;
        Impl* impl;
    };

} }
