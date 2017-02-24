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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/DataChunkReaderController.hpp"
#include "finjin/common/DataChunkReader.hpp"
#include <nowide/fstream.hpp>

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
DataChunkReaderController::~DataChunkReaderController()
{
}

Path DataChunkReaderController::ResolveInputFilePath(const Path& filePath)
{
    return filePath;
}

std::unique_ptr<DataChunkReaderInput> DataChunkReaderController::CreateInputFileStream(const Path& fullFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto resolvedFilePath = ResolveInputFilePath(fullFilePath);

    std::unique_ptr<DataChunkReaderInput> result;

    auto openedFile = std::make_unique<nowide::ifstream>(resolvedFilePath.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!openedFile->is_open())
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open file '%1%'.", resolvedFilePath));
    else
        result.reset(new StreamDataChunkReaderInput(std::move(openedFile)));
    
    return result;
}

void DataChunkReaderController::ReadInputFile(const Path& filePath, std::function<void(DataChunkReader&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto newInput = CreateInputFileStream(filePath, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to open input stream.");
        return;
    }

    std::shared_ptr<DataChunkReaderInput> sharedNewInput(newInput.release());
    std::unique_ptr<DataChunkReader> newReader(CreateReader(sharedNewInput, error));
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create new reader instance.");
        return;
    }
    
    ScheduleReadChunk(chunkFunc, std::move(newReader), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

void DataChunkReaderController::ScheduleReadChunk(std::function<void(DataChunkReader&, Error&)> chunkFunc, std::unique_ptr<DataChunkReader> reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    chunkFunc(*reader, error); 
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read chunk.");
        return;
    }
}

void DataChunkReaderController::FinishScheduledWork()
{
}

//DefaultDataChunkReaderController
DefaultDataChunkReaderController& DefaultDataChunkReaderController::GetInstance()
{
    static DefaultDataChunkReaderController instance;
    return instance;
}
