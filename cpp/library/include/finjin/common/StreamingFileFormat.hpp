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
#include "finjin/common/DataChunkWriter.hpp"
#include "finjin/common/DocumentWriterOutput.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class StreamingFileFormat
    {
        STREAMING_TEXT,
        STREAMING_BINARY,
        STREAMING_JSON,
        STREAMING_CONFIG,

        COUNT
    };

    struct FINJIN_COMMON_LIBRARY_API StreamingFileFormatUtilities
    {
        static bool IsValid(StreamingFileFormat format);

        static const char* ToString(StreamingFileFormat format);

        static void ParseFromExtension(StreamingFileFormat& format, const Utf8String& s, Error& error);
        static void ParseFromExtension(StreamingFileFormat& format, const Utf8StringView& s, Error& error);

        static StreamingFileFormat ParseFromExtension(const Utf8String& format, StreamingFileFormat defaultValue = StreamingFileFormat::STREAMING_TEXT);
        static StreamingFileFormat ParseFromExtension(const Utf8StringView& format, StreamingFileFormat defaultValue = StreamingFileFormat::STREAMING_TEXT);

        static Utf8String MakeExtension(const Utf8String& assetClass, StreamingFileFormat format);

        static std::unique_ptr<DataChunkWriter> CreateFileWriter(const Utf8String& fileName, StreamingFileFormat format, const DataChunkWriter::Settings& writerSettings, DataChunkWriterController& controller, Error& error);
    };

} }
