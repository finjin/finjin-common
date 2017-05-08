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
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API DataElementWriter
    {
    public:
        DataElementWriter(const DataElementWriter& other) = delete;
        DataElementWriter& operator = (const DataElementWriter& other) = delete;

        DataElementWriter(DataElementWriter&& other) = delete;
        DataElementWriter& operator = (DataElementWriter&& other) = delete;

    public:
        DataElementWriter() {}
        virtual ~DataElementWriter() {}

        virtual void SetAttribute(const Utf8String& name, const Utf8String& value) = 0;
        virtual void SetAttribute(const Utf8String& name, const char* value);
        virtual void SetAttribute(const Utf8String& name, int value);
        virtual void SetAttribute(const Utf8String& name, unsigned int value);
        virtual void SetAttribute(const Utf8String& name, int64_t value);
        virtual void SetAttribute(const Utf8String& name, uint64_t value);
        virtual void SetAttribute(const Utf8String& name, double value);
        virtual void SetAttribute(const Utf8String& name, bool value);

        virtual void AddValue(const Utf8String& name, const Utf8String& value) = 0;
        virtual void AddValue(const Utf8String& name, const char* value);
        virtual void AddValue(const Utf8String& name, int value);
        virtual void AddValue(const Utf8String& name, unsigned int value);
        virtual void AddValue(const Utf8String& name, int64_t value);
        virtual void AddValue(const Utf8String& name, uint64_t value);
        virtual void AddValue(const Utf8String& name, double value);
        virtual void AddValue(const Utf8String& name, bool value);

        virtual std::shared_ptr<DataElementWriter> StartChildElement(const Utf8String& name) = 0;
        virtual void EndChildElement() = 0;

        virtual std::shared_ptr<DataElementWriter> StartChildArray(const Utf8String& name) = 0;
        virtual void EndChildArray() = 0;

        virtual std::shared_ptr<DataElementWriter> StartObject(const Utf8String& name) = 0;
        virtual void EndObject() = 0;

        virtual void AddChildText(const Utf8String& name, const Utf8String& value) = 0;
        virtual void AddSimpleChildText(const Utf8String& name, const Utf8String& value) {AddChildText(name, value);}
    };

    /**
     * For JSON: Creates an entry of the form: {
     * For XML: Creates an entry of the form: <name></name>
     */
    class DataElementObjectBlock
    {
    public:
        DataElementObjectBlock(DataElementWriter* writer, const Utf8String& name)
        {
            this->parentWriter = writer;
            this->dataElementWriter = this->parentWriter->StartObject(name);
        }

        DataElementObjectBlock(std::shared_ptr<DataElementWriter> writer, const Utf8String& name)
        {
            this->parentWriterPtr = writer;
            this->parentWriter = writer.get();
            this->dataElementWriter = this->parentWriter->StartObject(name);
        }

        ~DataElementObjectBlock()
        {
            this->parentWriter->EndObject();
        }

        std::shared_ptr<DataElementWriter> operator -> ()
        {
            return this->dataElementWriter;
        }

        operator std::shared_ptr<DataElementWriter> ()
        {
            return this->dataElementWriter;
        }

    private:
        std::shared_ptr<DataElementWriter> parentWriterPtr;
        DataElementWriter* parentWriter;
        std::shared_ptr<DataElementWriter> dataElementWriter;
    };

    /**
     * For JSON: Creates an entry of the form: name: {
     * For XML: Creates an entry of the form: <name></name>
     */
    class DataElementChildObjectBlock
    {
    public:
        DataElementChildObjectBlock(DataElementWriter* writer, const Utf8String& name)
        {
            this->parentWriter = writer;
            this->dataElementWriter = this->parentWriter->StartChildElement(name);
        }

        DataElementChildObjectBlock(std::shared_ptr<DataElementWriter> writer, const Utf8String& name)
        {
            this->parentWriterPtr = writer;
            this->parentWriter = writer.get();
            this->dataElementWriter = this->parentWriter->StartChildElement(name);
        }

        ~DataElementChildObjectBlock()
        {
            this->parentWriter->EndChildElement();
        }

        std::shared_ptr<DataElementWriter> operator -> ()
        {
            return this->dataElementWriter;
        }

        operator std::shared_ptr<DataElementWriter> ()
        {
            return this->dataElementWriter;
        }

    private:
        std::shared_ptr<DataElementWriter> parentWriterPtr;
        DataElementWriter* parentWriter;
        std::shared_ptr<DataElementWriter> dataElementWriter;
    };

    /**
     * For JSON: Creates an entry of the form: name: [
     * For XML: Creates an entry of the form: <name></name>
     */
    class DataElementChildArrayBlock
    {
    public:
        DataElementChildArrayBlock(DataElementWriter* writer, const Utf8String& name)
        {
            this->parentWriter = writer;
            this->dataElementWriter = this->parentWriter->StartChildArray(name);
        }

        DataElementChildArrayBlock(std::shared_ptr<DataElementWriter> writer, const Utf8String& name)
        {
            this->parentWriterPtr = writer;
            this->parentWriter = writer.get();
            this->dataElementWriter = this->parentWriter->StartChildArray(name);
        }

        ~DataElementChildArrayBlock()
        {
            this->parentWriter->EndChildArray();
        }

        std::shared_ptr<DataElementWriter> operator -> ()
        {
            return this->dataElementWriter;
        }

        operator std::shared_ptr<DataElementWriter> ()
        {
            return this->dataElementWriter;
        }

    private:
        std::shared_ptr<DataElementWriter> parentWriterPtr;
        DataElementWriter* parentWriter;
        std::shared_ptr<DataElementWriter> dataElementWriter;
    };

} }
