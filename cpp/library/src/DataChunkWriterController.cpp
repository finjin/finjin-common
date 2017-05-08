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
#include "finjin/common/DataChunkWriterController.hpp"
#include "finjin/common/DataChunkWriter.hpp"
#include "finjin/common/Path.hpp"
#include <nowide/fstream.hpp>

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//DataChunkWriterController
DataChunkWriterController::~DataChunkWriterController()
{
}

std::unique_ptr<DocumentWriterOutput> DataChunkWriterController::CreateOutputFileStream(const Path& fullFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::unique_ptr<DocumentWriterOutput> result;

    auto openedFile = std::make_unique<nowide::ofstream>(fullFilePath.c_str(), std::ios_base::out | std::ios_base::binary);
    if (!openedFile->is_open())
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open file '%1%'.", fullFilePath));
    else
        result.reset(new StreamDataChunkWriterOutput(std::move(openedFile)));

    return result;
}

void DataChunkWriterController::WriteOutputFile(const Path& filePath, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto outputFileStream = CreateOutputFileStream(filePath, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to open output stream.");
        return;
    }

    std::unique_ptr<DocumentWriterOutput> sharedOutputFileStream(outputFileStream.release());
    std::unique_ptr<DataChunkWriter> writer(CreateWriter(std::move(sharedOutputFileStream), error));
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create new writer instance.");
        return;
    }

    ScheduleWriteChunk(std::move(writer), chunkFunc, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

bool DataChunkWriterController::RequiresNewOutput(const DataChunkWriter& writer, const ChunkName& chunkName) const
{
    return false;
}

std::unique_ptr<DocumentWriterOutput> DataChunkWriterController::AddOutput(DataChunkWriter& writer, const ChunkName& chunkName, Error& error)
{
    return nullptr;
}

bool DataChunkWriterController::ScheduleWriteChunk(std::shared_ptr<DataChunkWriter> writer, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
{
    return false;
}

void DataChunkWriterController::FinishScheduledWork()
{
}

//DefaultDataChunkWriterController
DefaultDataChunkWriterController& DefaultDataChunkWriterController::GetInstance()
{
    static DefaultDataChunkWriterController instance;
    return instance;
}
