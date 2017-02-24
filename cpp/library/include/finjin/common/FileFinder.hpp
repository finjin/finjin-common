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
#if FINJIN_TARGET_OS_IS_WINDOWS
    #include "finjin/common/Win32FileFinder.hpp"
#else
    #include "finjin/common/DirEntFileFinder.hpp"
#endif


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

#if FINJIN_TARGET_OS_IS_WINDOWS
    using FileFinder = Win32FileFinder;
#else
    using FileFinder = DirEntFileFinder;
#endif

} }
