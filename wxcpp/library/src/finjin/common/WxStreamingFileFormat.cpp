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
#include "WxStreamingFileFormat.hpp"
#include "WxTextDataChunkWriter.hpp"
#include "WxJsonDataChunkWriter.hpp"
#include "WxBinaryDataChunkWriter.hpp"
#include "WxConfigDataChunkWriter.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
wxString WxStreamingFileFormatUtilities::ToString(WxStreamingFileFormat ext)
{
    wxString extString;

    switch (ext)
    {
        case WxStreamingFileFormat::STREAMING_TEXT: extString = wxT("fstd"); break;
        case WxStreamingFileFormat::STREAMING_BINARY: extString = wxT("fsbd"); break;
        case WxStreamingFileFormat::STREAMING_JSON: extString = wxT("json"); break;
        case WxStreamingFileFormat::STREAMING_CONFIG: extString = wxT("cfg"); break;
        default: break;
    }

    return extString;
}

WxStreamingFileFormat WxStreamingFileFormatUtilities::Parse(const wxString& ext, WxStreamingFileFormat defaultValue)
{
    if (ext.StartsWith(wxT("fstd")))
        return WxStreamingFileFormat::STREAMING_TEXT;
    else if (ext.StartsWith(wxT("fsbd")))
        return WxStreamingFileFormat::STREAMING_BINARY;
    else if (ext.StartsWith(wxT("json")))
        return WxStreamingFileFormat::STREAMING_JSON;
    else if (ext.StartsWith(wxT("cfg")))
        return WxStreamingFileFormat::STREAMING_CONFIG;
    else
        return defaultValue;
}

wxString WxStreamingFileFormatUtilities::MakeExtension(const wxString& assetClass, WxStreamingFileFormat format)
{
    auto result = ToString(format);
    result += wxT("-");
    result += assetClass;
    return result;
}

std::unique_ptr<WxDataChunkWriter> WxStreamingFileFormatUtilities::CreateFileWriter(const wxString& fileName, WxStreamingFileFormat fileFormat, const WxDataChunkWriter::Settings& settings, WxDataChunkWriterController& controller, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    std::unique_ptr<WxDataChunkWriter> dataChunkWriter;

    std::shared_ptr<wxFile> exportFile(new wxFile(fileName, wxFile::write));
    if (!exportFile->IsOpened())
    {
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to open file for '%s'."), fileName.wx_str()));
        return nullptr;
    }

    switch (fileFormat)
    {
        case WxStreamingFileFormat::STREAMING_TEXT:
        {
            dataChunkWriter.reset(new WxTextDataChunkWriter);
            break;
        }
        case WxStreamingFileFormat::STREAMING_BINARY:
        {
            dataChunkWriter.reset(new WxBinaryDataChunkWriter);
            break;
        }
        case WxStreamingFileFormat::STREAMING_JSON:
        {
            dataChunkWriter.reset(new WxJsonDataChunkWriter);
            break;
        }
        case WxStreamingFileFormat::STREAMING_CONFIG:
        {
            dataChunkWriter.reset(new WxConfigDataChunkWriter);
            break;
        }
        default:
        {
            FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Invalid streaming file format '%d'."), (int)fileFormat));
            return nullptr;
        }
    }

    auto writerSettings = settings;
    writerSettings.Create(std::shared_ptr<WxFileDataChunkWriterOutput>(new WxFileDataChunkWriterOutput(exportFile)), controller);

    dataChunkWriter->CreateRoot(writerSettings, error);
    if (error)
    {
        FINJIN_WX_SET_ERROR(error, wxT("Failed to create data chunk writer."));
        return nullptr;
    }

    return dataChunkWriter;
}
