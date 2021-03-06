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


#if FINJIN_TARGET_PLATFORM_IS_LINUX

//Includes----------------------------------------------------------------------
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/ValueOrError.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class LinuxUtilities
    {
    public:
        static ValueOrError<bool> GetProcessFilePath(Path& path);
        static Path GetProcessFilePath();

        static ValueOrError<bool> GetWorkingDirectory(Path& path);
        static Path GetWorkingDirectory();
    };

} }

#endif
