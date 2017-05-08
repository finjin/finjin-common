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
#include "finjin/common/AndroidJniUtilities.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
AndroidJniUtilities::AndroidJniUtilities(android_app* androidApp) : JniUtilities(androidApp->appThreadEnv, androidApp->appThreadThis)
{
}

AndroidJniUtilities::AndroidJniUtilities(JNIEnv* env, jobject thiz) : JniUtilities(env, thiz)
{
}
