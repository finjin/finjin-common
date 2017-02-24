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
#include "finjin/common/StandardPaths.hpp"
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/LinuxUtilities.hpp"

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
void StandardPaths::Create(const Utf8String& applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(applicationHandle != nullptr);
    auto androidApp = reinterpret_cast<android_app*>(applicationHandle);

    AndroidJniUtilities jniUtils(androidApp);

    //System defined locations------------------------------
    this->applicationExecutableFile.path = LinuxUtilities::GetProcessFilePath(); //This will return something like "/system/bin/app_process32", not really that useful
    this->applicationExecutableFile.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->applicationExecutableFile: %1%", this->applicationExecutableFile.path);

    jniUtils.GetStringField(this->userDocumentsDirectory.path, "userDocumentsDirectory");
    this->userDocumentsDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userDocumentsDirectory: %1%", this->userDocumentsDirectory.path);

    jniUtils.GetStringField(this->userMusicDirectory.path, "userMusicDirectory");
    this->userMusicDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userMusicDirectory: %1%", this->userMusicDirectory.path);

    jniUtils.GetStringField(this->userVideosDirectory.path, "userVideosDirectory");
    this->userVideosDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userVideosDirectory: %1%", this->userVideosDirectory.path);

    jniUtils.GetStringField(this->userPicturesDirectory.path, "userPicturesDirectory");
    this->userPicturesDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userPicturesDirectory: %1%", this->userPicturesDirectory.path);

    jniUtils.GetStringField(this->userCameraRollDirectory.path, "userCameraRollDirectory");
    this->userCameraRollDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userCameraRollDirectory: %1%", this->userCameraRollDirectory.path);

    jniUtils.GetStringField(this->userDownloadsDirectory.path, "userDownloadsDirectory");
    this->userDownloadsDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userDownloadsDirectory: %1%", this->userDownloadsDirectory.path);
        
    this->userApplicationSettingsDirectory.path = androidApp->activity->internalDataPath;
    if (this->userApplicationSettingsDirectory.path.empty())
        this->userApplicationSettingsDirectory.path = androidApp->activity->externalDataPath;
    this->userApplicationSettingsDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userApplicationSettingsDirectory: %1%", this->userApplicationSettingsDirectory.path);

    jniUtils.GetStringField(this->userApplicationTemporaryDirectory.path, "userApplicationTemporaryDirectory");
    this->userApplicationTemporaryDirectory.isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->userApplicationTemporaryDirectory: %1%", this->userApplicationTemporaryDirectory.path);    
    
    this->workingDirectory.path = LinuxUtilities::GetWorkingDirectory();
    this->workingDirectory.isSystemCreated = true;

    //Fallback behavior for missing locations--------------------
    //Utf8String applicationPackageName;
    //jniUtils.GetStringField(applicationPackageName, "packageName");

    //Path systemDataDirectory;
    //jniUtils.GetStringField(systemDataDirectory, "systemDataDirectory"); ////On Android 6 (Nexus 6): /data
    ////FINJIN_DEBUG_LOG_INFO("systemDataDirectory: %1%", systemDataDirectory);

    //Path systemExternalStorageDirectory;
    //jniUtils.GetStringField(systemExternalStorageDirectory, "systemExternalStorageDirectory"); ////On Android 6 (Nexus 6): /storage/emulated/0
    ////FINJIN_DEBUG_LOG_INFO("systemExternalStorageDirectory: %1%", systemExternalStorageDirectory);

    //bool isExternalStorageReadyForRead;
    //jniUtils.CallBoolMethod(isExternalStorageReadyForRead, "isExternalStorageReadyForRead");
    ////FINJIN_DEBUG_LOG_INFO("isExternalStorageReadyForRead: %1%", isExternalStorageReadyForRead);

    //bool isExternalStorageReadyForWrite;
    //jniUtils.CallBoolMethod(isExternalStorageReadyForWrite, "isExternalStorageReadyForWrite");
    ////FINJIN_DEBUG_LOG_INFO("isExternalStorageReadyForWrite: %1%", isExternalStorageReadyForWrite);
}
