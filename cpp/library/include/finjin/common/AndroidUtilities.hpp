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


#if FINJIN_TARGET_PLATFORM_IS_ANDROID

//Includes----------------------------------------------------------------------
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/common/ScreenOrientation.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API AndroidUtilities
    {
    public:
        static bool HasRightToLeftLayout(android_app* androidApp);
        static bool HasTouchScreen(android_app* androidApp);
        static float GetScreenDensity(android_app* androidApp);
        static GenericScreenOrientation GetGenericScreenOrientation(android_app* androidApp);
        static bool IsAndroidMobile(android_app* androidApp);
        static bool IsAndroidTV(android_app* androidApp);
    };

} }

#endif
