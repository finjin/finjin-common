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
#include "finjin/common/Path.hpp"
#include <Windows.h>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class FINJIN_COMMON_LIBRARY_API Win32FileFinder
    {
    public:
        Win32FileFinder(Allocator* allocator = nullptr);
        ~Win32FileFinder();

    #if FINJIN_TARGET_OS_IS_WINDOWS_UWP
        bool Start(Windows::Storage::StorageFolder^ storageFolder);
    #endif
        bool Start(const Path& path);
        bool Next();
        ValueOrError<void> GetCurrentName(Path& result) const;
        ValueOrError<void> GetCurrentPath(Path& result) const;
        ValueOrError<bool> IsCurrentFile() const;
        ValueOrError<bool> IsCurrentDirectory() const;
        void Stop();

        const Path& GetStartPath() const;

    private:
        Path path;
        mutable Path testPath;

        HANDLE handle;
        WIN32_FIND_DATAW foundData;

    };

} }
