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
#include "finjin/common/DataElementWriter.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common { 

    class FINJIN_COMMON_LIBRARY_API JsonDataElementWriter : public DataElementWriter
    {
    public:
        JsonDataElementWriter(void* jsonStringWriter);
        ~JsonDataElementWriter();

        void SetAttribute(const Utf8String& name, const Utf8String& value) override;
        void SetAttribute(const Utf8String& name, const char* value) override;
        void SetAttribute(const Utf8String& name, int value) override;
        void SetAttribute(const Utf8String& name, unsigned int value) override;
        void SetAttribute(const Utf8String& name, int64_t value) override;
        void SetAttribute(const Utf8String& name, uint64_t value) override;
        void SetAttribute(const Utf8String& name, double value) override;
        void SetAttribute(const Utf8String& name, bool value) override;

        void AddValue(const Utf8String& name, const Utf8String& value) override;
        void AddValue(const Utf8String& name, const char* value) override;
        void AddValue(const Utf8String& name, int value) override;
        void AddValue(const Utf8String& name, unsigned int value) override;
        void AddValue(const Utf8String& name, int64_t value) override;
        void AddValue(const Utf8String& name, uint64_t value) override;
        void AddValue(const Utf8String& name, double value) override;
        void AddValue(const Utf8String& name, bool value) override;

        std::shared_ptr<DataElementWriter> StartChildElement(const Utf8String& name) override;
        void EndChildElement() override;

        std::shared_ptr<DataElementWriter> StartChildArray(const Utf8String& name) override;
        void EndChildArray() override;

        std::shared_ptr<DataElementWriter> StartObject(const Utf8String& name) override;
        void EndObject() override;

        void AddChildText(const Utf8String& name, const Utf8String& value) override;
        void AddSimpleChildText(const Utf8String& name, const Utf8String& value) override;

    private:
        void* writer;
    };

} }
