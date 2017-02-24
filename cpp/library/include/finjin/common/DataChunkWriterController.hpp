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
#include "finjin/common/ChunkName.hpp"
#include "finjin/common/DocumentWriterOutput.hpp"
#include "finjin/common/Error.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class DataChunkWriter;

    class FINJIN_COMMON_LIBRARY_API DataChunkWriterController
    {
    public:
        virtual ~DataChunkWriterController();

        virtual std::unique_ptr<DataChunkWriter> CreateWriter(std::shared_ptr<DocumentWriterOutput> outPtr, Error& error) = 0;

        virtual std::unique_ptr<DocumentWriterOutput> CreateOutputFileStream(const Path& fullFilePath, Error& error);

        virtual void WriteOutputFile(const Path& filePath, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error);

        virtual bool RequiresNewOutput(const DataChunkWriter& writer, const ChunkName& chunkName) const;        
        virtual std::unique_ptr<DocumentWriterOutput> AddOutput(DataChunkWriter& writer, const ChunkName& chunkName, Error& error);
        virtual bool ScheduleWriteChunk(std::shared_ptr<DataChunkWriter> writer, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error);

        virtual void FinishScheduledWork();
    };

    class FINJIN_COMMON_LIBRARY_API DefaultDataChunkWriterController : public DataChunkWriterController
    {
    public:
        DefaultDataChunkWriterController()
        {
        }

        std::unique_ptr<DataChunkWriter> CreateWriter(std::shared_ptr<DocumentWriterOutput> outPtr, Error& error) override
        {
            return nullptr;
        }

        static DefaultDataChunkWriterController& GetInstance();
    };

} }
