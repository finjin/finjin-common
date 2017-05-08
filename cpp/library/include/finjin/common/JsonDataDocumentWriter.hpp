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

    class FINJIN_COMMON_LIBRARY_API JsonDataDocumentWriter : public DataDocumentWriter
    {
    public:
        JsonDataDocumentWriter(const JsonDataDocumentWriter& other) = delete;
        JsonDataDocumentWriter& operator = (const JsonDataDocumentWriter& other) = delete;

        JsonDataDocumentWriter(JsonDataDocumentWriter&& other) = delete;
        JsonDataDocumentWriter& operator = (JsonDataDocumentWriter&& other) = delete;

    public:
        /**
         * This addExtraObjectAtRoot is a little of a hack.
         * It is necessary in cases where you need to start out beneath an object opening when calling StartRootElement("status").
         * If addExtraObjectAtRoot were true the following would result:
         * {"status":{ <--After this you would usually call anything EXCEPT DataElementWriter::StartObject()
         * If addExtraObjectAtRoot were false (the default) the following example would result:
         * {"status":   <--After this you would usually call DataElementWriter::StartObject()
         */
        JsonDataDocumentWriter(bool addExtraObjectAtRoot = false);
        ~JsonDataDocumentWriter();

        std::shared_ptr<DataElementWriter> StartRootElement(const Utf8String& name) override;
        void EndRootElement() override;

        void SaveFile(const Path& fileName, Error& error) override;

        void GetString(Utf8String& result, Error& error) override;

    private:
        struct Impl;
        Impl* impl;
    };

} }
