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
#include "finjin/common/DataChunkReaderInput.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class DataChunkReader;

    class FINJIN_COMMON_LIBRARY_API DataChunkReaderController
    {
    public:
        virtual ~DataChunkReaderController();

        virtual std::unique_ptr<DataChunkReader> CreateReader(std::shared_ptr<DataChunkReaderInput> inPtr, Error& error) = 0;

        virtual Path ResolveInputFilePath(const Path& filePath);

        virtual std::unique_ptr<DataChunkReaderInput> CreateInputFileStream(const Path& fullFilePath, Error& error);

        virtual void ReadInputFile(const Path& filePath, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error);

        virtual void ScheduleReadChunk(std::function<void(DataChunkReader&, Error&)> chunkFunc, std::unique_ptr<DataChunkReader> reader, Error& error);

        virtual void FinishScheduledWork();
    };

    class DefaultDataChunkReaderController : public DataChunkReaderController
    {
    public:
        std::unique_ptr<DataChunkReader> CreateReader(std::shared_ptr<DataChunkReaderInput> inPtr, Error& error) override
        {
            return nullptr;
        }

        Path ResolveInputFilePath(const Path& filePath) override
        {
            return filePath;
        }

        std::unique_ptr<DataChunkReaderInput> CreateInputFileStream(const Path& fullFilePath, Error& error) override
        {
            return nullptr;
        }

        void ReadInputFile(const Path& filePath, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error) override
        {
        }

        void ScheduleReadChunk(std::function<void(DataChunkReader&, Error&)> chunkFunc, std::unique_ptr<DataChunkReader> reader, Error& error) override
        {
        }

        void FinishScheduledWork() override
        {
        }

        static DefaultDataChunkReaderController& GetInstance();
    };

} }
