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
#include "WxDataChunkWriterController.hpp"
#include "WxDataChunkWriter.hpp"
#include "WxChunkName.hpp"
#include "Logger.hpp"

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------

//WxDataChunkWriterController
WxDataChunkWriterController::~WxDataChunkWriterController() 
{
}

std::unique_ptr<WxDocumentWriterOutput> WxDataChunkWriterController::CreateOutputFileStream(const wxString& fullFilePath, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    std::unique_ptr<WxDocumentWriterOutput> result;

    std::unique_ptr<wxFile> openedFile(new wxFile(fullFilePath.wx_str(), wxFile::write));
    if (!openedFile->IsOpened())
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to open file '%s'."), fullFilePath.wx_str()));
    else
        result.reset(new WxFileDataChunkWriterOutput(std::move(openedFile)));

    return result;
}

bool WxDataChunkWriterController::RequiresNewOutput(const WxDataChunkWriter& writer, const WxChunkName& chunkName) const
{
    return false; 
}

std::unique_ptr<WxDocumentWriterOutput> WxDataChunkWriterController::AddOutput(WxDataChunkWriter& writer, const WxChunkName& chunkName, WxError& error)
{
    return nullptr;
}

bool WxDataChunkWriterController::ScheduleWriteChunk(std::shared_ptr<WxDataChunkWriter> writer, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error)
{
    return false;
}

void WxDataChunkWriterController::FinishScheduledWork()
{
}

//DefaultDataChunkWriterController
DefaultDataChunkWriterController& DefaultDataChunkWriterController::GetInstance()
{
    static DefaultDataChunkWriterController instance;
    return instance;
}
