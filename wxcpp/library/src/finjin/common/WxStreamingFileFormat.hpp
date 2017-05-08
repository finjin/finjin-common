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
#include "finjin/common/WxDataChunkWriter.hpp"
#include "finjin/common/WxDocumentWriterOutput.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class WxStreamingFileFormat
    {
        STREAMING_TEXT,
        STREAMING_BINARY,
        STREAMING_JSON,
        STREAMING_CONFIG,

        COUNT
    };

    struct WxStreamingFileFormatUtilities
    {
        static wxString ToString(WxStreamingFileFormat ext);
        static WxStreamingFileFormat Parse(const wxString& ext, WxStreamingFileFormat defaultValue = WxStreamingFileFormat::STREAMING_TEXT);
        static wxString MakeExtension(const wxString& assetClass, WxStreamingFileFormat format);

        static std::unique_ptr<WxDataChunkWriter> CreateFileWriter(const wxString& fileName, WxStreamingFileFormat fileFormat, const WxDataChunkWriter::Settings& writerSettings, WxDataChunkWriterController& controller, WxError& error);
    };

} }
