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
#include "WxChunkName.hpp"
#include "finjin/common/WxDocumentWriterOutput.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class WxDataChunkWriter;

    class WxDataChunkWriterController
    {
    public:
        virtual ~WxDataChunkWriterController();

        virtual std::unique_ptr<WxDocumentWriterOutput> CreateOutputFileStream(const wxString& fullFilePath, WxError& error);

        virtual bool RequiresNewOutput(const WxDataChunkWriter& writer, const WxChunkName& chunkName) const;
        virtual std::unique_ptr<WxDocumentWriterOutput> AddOutput(WxDataChunkWriter& writer, const WxChunkName& chunkName, WxError& error);
        virtual bool ScheduleWriteChunk(std::shared_ptr<WxDataChunkWriter> writer, std::function<void(WxDataChunkWriter&, WxError&)> chunkFunc, WxError& error);

        virtual void FinishScheduledWork();
    };

    class DefaultDataChunkWriterController : public WxDataChunkWriterController
    {
    public:
        DefaultDataChunkWriterController()
        {
        }

        static DefaultDataChunkWriterController& GetInstance();
    };

} }
