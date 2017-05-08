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


#if FINJIN_TARGET_PLATFORM_IS_APPLE

//Includes----------------------------------------------------------------------
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#if FINJIN_APPLE_OBJCPP_UTILITIES //Define this before including AppleUtilites.hpp
    #import <Foundation/NSPathUtilities.h>
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API AppleUtilities
    {
    public:
        static ValueOrError<void> GetProcessFilePath(Path& path);

        static ValueOrError<void> GetWorkingDirectory(Path& path);

    #if FINJIN_APPLE_OBJCPP_UTILITIES
        static ValueOrError<void> GetSystemDirectory(Path& path, bool& isSystemCreated, NSSearchPathDirectory which, bool tryCreateIfNotPresent = true);
    #endif

        static size_t GetVolumeIDHash(const Path& path);

        static void GetCommandLineArgs(CommandLineArgsProcessor& argsProcessor);

        static void SetApplicationMultithreaded();
    };

} }

#endif
