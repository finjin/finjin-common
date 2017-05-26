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

    if (WindowsUtilities::GetProcessFilePath(this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path, static_cast<HMODULE>(applicationHandle)).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get application executable file path.");
        return;
    }
    this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].isSystemCreated = true;

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY], Windows::ApplicationModel::Package::Current->InstalledLocation);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY], KnownFolders::DocumentsLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY], KnownFolders::MusicLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY], KnownFolders::VideosLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY], KnownFolders::PicturesLibrary);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY], KnownFolders::CameraRoll);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_SAVED_PICTURES_DIRECTORY], KnownFolders::SavedPictures);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY], ApplicationData::Current->LocalFolder);

    GET_SYSTEM_CREATED_DIRECTORY(this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY], ApplicationData::Current->TemporaryFolder);

    if (WindowsUtilities::GetWorkingDirectory(this->paths[WhichStandardPath::WORKING_DIRECTORY].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get working directory.");
        return;
    }
    this->paths[WhichStandardPath::WORKING_DIRECTORY].isSystemCreated = true;
}
