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
#include "finjin/common/WindowsUtilities.hpp"

using namespace Finjin::Common;
using namespace Windows::Storage;


//Macros------------------------------------------------------------------------

//This MUST be a macro so that unprivileged access to the 'which' folder can be handled
#define GET_SYSTEM_CREATED_DIRECTORY(standardPath, which) \
    standardPath.path.clear(); \
    standardPath.isSystemCreated = false; \
    try \
    { \
        if (standardPath.path.assign(which->Path->Data()).HasError()) \
        { \
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign path for '%1%'.", standardPath.defaultDisplayName)); \
            return; \
        } \
        if (standardPath.path.IsDirectory()) \
        { \
            standardPath.path.PlatformNormalize(); \
            standardPath.isSystemCreated = true; \
        } \
        else \
            standardPath.path.clear(); \
    } \
    catch (Platform::Exception^ ex) \
    { \
    }


//Implementation----------------------------------------------------------------
void StandardPaths::Create(const Utf8String& applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (WindowsUtilities::GetProcessFilePath(this->applicationExecutableFile.path, static_cast<HMODULE>(applicationHandle)).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get application executable file path.");
        return;
    }
    this->applicationExecutableFile.isSystemCreated = true;

    GET_SYSTEM_CREATED_DIRECTORY(this->applicationBundleDirectory, Windows::ApplicationModel::Package::Current->InstalledLocation);

    GET_SYSTEM_CREATED_DIRECTORY(this->userDocumentsDirectory, KnownFolders::DocumentsLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->userMusicDirectory, KnownFolders::MusicLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->userVideosDirectory, KnownFolders::VideosLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->userPicturesDirectory, KnownFolders::PicturesLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->userCameraRollDirectory, KnownFolders::CameraRoll);

    GET_SYSTEM_CREATED_DIRECTORY(this->userSavedPicturesDirectory, KnownFolders::SavedPictures);

    GET_SYSTEM_CREATED_DIRECTORY(this->userApplicationSettingsDirectory, ApplicationData::Current->LocalFolder);

    GET_SYSTEM_CREATED_DIRECTORY(this->userApplicationTemporaryDirectory, ApplicationData::Current->TemporaryFolder);

    if (WindowsUtilities::GetWorkingDirectory(this->workingDirectory.path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get working directory.");
        return;
    }
    this->workingDirectory.isSystemCreated = true;
}
