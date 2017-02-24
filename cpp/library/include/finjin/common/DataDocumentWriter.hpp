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
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common { 

    class FINJIN_COMMON_LIBRARY_API DataDocumentWriter
    {
    public:
        virtual ~DataDocumentWriter() {}

        virtual std::shared_ptr<DataElementWriter> StartRootElement(const Utf8String& name) = 0;
        virtual void EndRootElement() = 0;

        virtual void SaveFile(const Path& fileName, Error& error) = 0;

        virtual void GetString(Utf8String& result, Error& error) = 0;
    };

    class DataElementRootBlock
    {
    public:
        DataElementRootBlock(DataDocumentWriter* writer, const Utf8String& name)
        {
            this->parentWriter = writer;
            this->dataElementWriter = this->parentWriter->StartRootElement(name);
        }

        DataElementRootBlock(std::shared_ptr<DataDocumentWriter> writer, const Utf8String& name)
        {
            this->parentWriterPtr = writer;
            this->parentWriter = writer.get();
            this->dataElementWriter = this->parentWriter->StartRootElement(name);
        }

        ~DataElementRootBlock()
        {
            this->parentWriter->EndRootElement();
        }

        std::shared_ptr<DataElementWriter> operator -> ()
        {
            return this->dataElementWriter;
        }

        operator std::shared_ptr<DataElementWriter>()
        {
            return this->dataElementWriter;
        }

    private:
        std::shared_ptr<DataDocumentWriter> parentWriterPtr;
        DataDocumentWriter* parentWriter;
        std::shared_ptr<DataElementWriter> dataElementWriter;
    };

} }
