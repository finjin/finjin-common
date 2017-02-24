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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Path.hpp"
#if FINJIN_TARGET_OS_IS_WINDOWS
    #include <Windows.h>
#else
    #include <nowide/fstream.hpp>
#endif


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Internal utility class for reading/writing to a file
    struct FileAccessor
    {
        FileAccessor();
        ~FileAccessor();

        bool OpenForRead(const Path& path, uint64_t* fileSize = nullptr);
        bool OpenForWrite(const Path& path);

        bool IsOpen() const;

        void Close();

        size_t Read(void* bytes, size_t byteCount);
        size_t Write(const void* bytes, size_t byteCount);
        void WriteRemaining(FileAccessor& inFile);

        static bool GetFileLength(const Path& path, uint64_t& fileLength);

    #if FINJIN_TARGET_OS_IS_WINDOWS
        HANDLE fileHandle = INVALID_HANDLE_VALUE;
    #else
        nowide::fstream fileStream;
    #endif
    };

} }
