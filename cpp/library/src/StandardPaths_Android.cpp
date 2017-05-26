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
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/LinuxUtilities.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static void VerifyStandardPath(StandardPath& standardPath)
{
    if (!standardPath.path.empty())
    {
        if (!standardPath.path.IsDirectory())
        {
            if (!standardPath.path.CreateDirectories())
            {
                standardPath.path.clear();
                standardPath.isSystemCreated = false;
            }
        }
    }
}


//Implementation----------------------------------------------------------------
void StandardPaths::Create(const Utf8String& applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(applicationHandle != nullptr);
    auto androidApp = reinterpret_cast<android_app*>(applicationHandle);

    AndroidJniUtilities jniUtils(androidApp);

    //System defined locations------------------------------
    if (LinuxUtilities::GetProcessFilePath(this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path).HasError()) //This will return something like "/system/bin/app_process32", not really that useful
    {
        FINJIN_SET_ERROR(error, "Failed to get process path.");
        return;
    }
    this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].isSystemCreated = true;
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE]: %1%", this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY].path, "userDocumentsDirectory");
    this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY].path, "userMusicDirectory");
    this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY].path, "userVideosDirectory");
    this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY].path, "userPicturesDirectory");
    this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY].path, "userCameraRollDirectory");
    this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY].path, "userDownloadsDirectory");
    this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY].path);

    this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path = androidApp->activity->internalDataPath;
    if (this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path.empty())
        this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path = androidApp->activity->externalDataPath;
    this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path);

    jniUtils.GetStringField(this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path, "userApplicationTemporaryDirectory");
    this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY]);
    //FINJIN_DEBUG_LOG_INFO("this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY]: %1%", this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path);

    if (LinuxUtilities::GetWorkingDirectory(this->paths[WhichStandardPath::WORKING_DIRECTORY].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get working directory.");
        return;
    }
    this->paths[WhichStandardPath::WORKING_DIRECTORY].isSystemCreated = true;
    VerifyStandardPath(this->paths[WhichStandardPath::WORKING_DIRECTORY]);

    //Fallback behavior for missing locations--------------------
    //Utf8String applicationPackageName;
    //jniUtils.GetStringField(applicationPackageName, "packageName");

    //Path systemDataDirectory;
    //jniUtils.GetStringField(systemDataDirectory, "systemDataDirectory"); ////On Android 6 (Nexus 6): /data
    //FINJIN_DEBUG_LOG_INFO("systemDataDirectory: %1%", systemDataDirectory);

    //Path systemExternalStorageDirectory;
    //jniUtils.GetStringField(systemExternalStorageDirectory, "systemExternalStorageDirectory"); ////On Android 6 (Nexus 6): /storage/emulated/0
    //FINJIN_DEBUG_LOG_INFO("systemExternalStorageDirectory: %1%", systemExternalStorageDirectory);

    //bool isExternalStorageReadyForRead;
    //jniUtils.CallBoolMethod(isExternalStorageReadyForRead, "isExternalStorageReadyForRead");
    //FINJIN_DEBUG_LOG_INFO("isExternalStorageReadyForRead: %1%", isExternalStorageReadyForRead);

    //bool isExternalStorageReadyForWrite;
    //jniUtils.CallBoolMethod(isExternalStorageReadyForWrite, "isExternalStorageReadyForWrite");
    //FINJIN_DEBUG_LOG_INFO("isExternalStorageReadyForWrite: %1%", isExternalStorageReadyForWrite);
}
