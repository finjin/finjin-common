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
#include "finjin/common/AndroidUtilities.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
bool AndroidUtilities::HasRightToLeftLayout(android_app* androidApp)
{
    return AConfiguration_getLayoutDirection(androidApp->config) == ACONFIGURATION_LAYOUTDIR_RTL;
}

bool AndroidUtilities::HasTouchScreen(android_app* androidApp)
{
    return AConfiguration_getTouchscreen(androidApp->config) != 1;
}

float AndroidUtilities::GetScreenDensity(android_app* androidApp)
{
    return AConfiguration_getDensity(androidApp->config) / 160.0;
}

GenericScreenOrientation AndroidUtilities::GetGenericScreenOrientation(android_app* androidApp)
{
    return AConfiguration_getOrientation(androidApp->config) == 1 ? GenericScreenOrientation::PORTRAIT : GenericScreenOrientation::LANDSCAPE;
}

bool AndroidUtilities::IsAndroidMobile(android_app* androidApp)
{
    return AConfiguration_getUiModeType(androidApp->config) == ACONFIGURATION_UI_MODE_TYPE_NORMAL;
}

bool AndroidUtilities::IsAndroidTV(android_app* androidApp)
{
    return AConfiguration_getUiModeType(androidApp->config) == ACONFIGURATION_UI_MODE_TYPE_TELEVISION;
}
