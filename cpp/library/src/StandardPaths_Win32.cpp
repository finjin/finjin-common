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
#include <Windows.h>
#include <Shlobj.h>

using namespace Finjin::Common;


//Local functions--------------------------------------------------------------
static ValueOrError<bool> GetSystemCreatedDirectory(StandardPath& standardPath, int which)
{
    standardPath.path.clear();
    standardPath.isSystemCreated = false;

    wchar_t pathW[MAX_PATH + 1];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, which, nullptr, SHGFP_TYPE_CURRENT | CSIDL_FLAG_CREATE, pathW)))
    {
        if (standardPath.path.assign(pathW).HasError())
            return ValueOrError<bool>::CreateError();

        if (standardPath.path.IsDirectory())
        {
            standardPath.path.PlatformNormalize();
            standardPath.isSystemCreated = !standardPath.path.empty();
        }
        else
            standardPath.path.clear();
    }

    return !standardPath.path.empty();
}


//Implementation---------------------------------------------------------------
void StandardPaths::Create(const Utf8String& applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (WindowsUtilities::GetProcessFilePath(this->applicationExecutableFile.path, static_cast<HMODULE>(applicationHandle)).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get application executable file path.");
        return;
    }
    this->applicationExecutableFile.isSystemCreated = true;

    if (this->applicationBundleDirectory.path.assign(this->applicationExecutableFile.path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign application bundle directory.");
        return;
    }
    this->applicationBundleDirectory.path.RemoveFileName();
    this->applicationBundleDirectory.isSystemCreated = true;

    if (GetSystemCreatedDirectory(this->userDocumentsDirectory, CSIDL_MYDOCUMENTS).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user documents directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userMusicDirectory, CSIDL_MYMUSIC).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user documents directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userVideosDirectory, CSIDL_MYVIDEO).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user videos directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userPicturesDirectory, CSIDL_MYPICTURES).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user pictures directory.");
        return;
    }
    
    auto userHomeDirectoryResult = Path::GetUserHomeDirectory(this->userDownloadsDirectory.path);
    if (userHomeDirectoryResult.HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user home directory.");
        return;
    }
    else if (userHomeDirectoryResult.value)
    {
        //On Windows Win32: C:\Users\username
        if ((this->userDownloadsDirectory.path /= "Downloads").HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append 'Downloads' to user home directory.");
            return;
        }
    }
    
    Path bestApplicationName;
    if (!applicationName.empty())
        bestApplicationName = applicationName;
    else
    {
        if (this->applicationExecutableFile.path.GetBaseName(bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to get application executable file base name.");
            return;
        }
    }

    auto localAppDataResult = GetSystemCreatedDirectory(this->userApplicationSettingsDirectory, CSIDL_LOCAL_APPDATA);
    if (localAppDataResult.HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user application settings directory.");
        return;
    }
    else if (localAppDataResult.value) 
    {
        //On Windows Win32: C:\Users\(username)\AppData\Local    
        if ((this->userApplicationSettingsDirectory.path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to user application settings directory.");
            return;
        }
        this->userApplicationSettingsDirectory.isSystemCreated = false; //The base part is system created but the part with the application name at the end is not
    }

    wchar_t tempPathW[MAX_PATH + 1]; //On Windows Win32: C:\Users\(username)\AppData\Local\Temp
    if (GetTempPathW(MAX_PATH, tempPathW) > 0)
    {
        if (this->userApplicationTemporaryDirectory.path.assign(tempPathW).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign user application temporary directory from system temporary directory.");
            return;
        }
        if ((this->userApplicationTemporaryDirectory.path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to user application temporary directory.");
            return;
        }
    }
    else
    {
        if (this->userApplicationTemporaryDirectory.path.assign(this->userApplicationSettingsDirectory.path).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign user application temporary directory from user application settings directory.");
            return;
        }
        if ((this->userApplicationTemporaryDirectory.path /= "Temp").HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append 'Temp' to user application temporary directory.");
            return;
        }
    }    

    auto commonAppDataResult = GetSystemCreatedDirectory(this->applicationSettingsDirectory, CSIDL_COMMON_APPDATA);
    if (commonAppDataResult.HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get application settings directory.");
        return;
    }
    else if (commonAppDataResult.value) //On Windows Win32: C:\Users\All Users\Application Data   
    {
        if ((this->applicationSettingsDirectory.path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to application settings directory.");
            return;
        }
        this->applicationSettingsDirectory.isSystemCreated = false; //The base part is system created but the part with the application name at the end is not
    }

    if (WindowsUtilities::GetWorkingDirectory(this->workingDirectory.path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get working directory.");
        return;
    }
    this->workingDirectory.isSystemCreated = true;

    this->logicalDrives.clear();
    DWORD driveFlags = GetLogicalDrives();
    Utf8String driveName;
    for (DWORD driveIndex = 0; driveIndex < MAX_LOGICAL_DRIVES && !this->logicalDrives.full(); driveIndex++)
    { 
        if (driveFlags & (1 << driveIndex))
        {
            wchar_t driveNameW[4] = { static_cast<wchar_t>(L'A' + driveIndex), L':', L'\\', L'\0' };
            switch (GetDriveTypeW(driveNameW))
            {
                case DRIVE_REMOVABLE:
                case DRIVE_FIXED:
                case DRIVE_REMOTE:
                {
                    driveNameW[2] = 0; //Remove the trailing slash
                    driveName = driveNameW;//Convert to UTF8
                    this->logicalDrives.push_back(driveName.c_str()); //Add to collection
                    break;                    
                }
            }
        }
    }
}
