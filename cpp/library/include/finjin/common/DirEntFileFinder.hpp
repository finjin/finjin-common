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


#if !FINJIN_TARGET_PLATFORM_IS_WINDOWS

//Includes----------------------------------------------------------------------
#include "finjin/common/FileSystemEntry.hpp"
#include "finjin/common/Path.hpp"
#include <dirent.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class DirEntFileFinder
    {
    public:
        DirEntFileFinder(Allocator* allocator = nullptr);
        ~DirEntFileFinder();

        bool Start(const Path& path);
        bool Next();
        ValueOrError<void> GetCurrentName(Path& result) const;
        ValueOrError<void> GetCurrentPath(Path& result) const;
        ValueOrError<bool> IsCurrentFile() const;
        ValueOrError<bool> IsCurrentDirectory() const;
        ValueOrError<FileSystemEntryType> GetCurrentType() const;
        void Stop();

        const Path& GetStartPath() const;

    protected:
        Path path;
        mutable Path testPath;

        DIR* dir;
        struct dirent* ent;
    };

} }

#endif
