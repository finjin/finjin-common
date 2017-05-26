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
#include "finjin/common/LinuxUtilities.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static ValueOrError<void> GetSystemCreatedDirectory(StandardPath& standardPath, const Path& path, const char* standardName)
{
    if (standardPath.path.assign(path).HasError())
        return ValueOrError<void>::CreateError();
    if ((standardPath.path /= standardName).HasError())
        return ValueOrError<void>::CreateError();

    standardPath.isSystemCreated = standardPath.path.IsDirectory();
    if (!standardPath.isSystemCreated)
        standardPath.path.clear();

    return ValueOrError<void>();
}


//Implementation----------------------------------------------------------------
void StandardPaths::Create(const Utf8String& applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (LinuxUtilities::GetProcessFilePath(this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get application executable path");
        return;
    }
    this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].isSystemCreated = true;

    if (this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path.assign(this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign application bundle directory");
        return;
    }
    this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path.RemoveFileName();
    this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].isSystemCreated = true;

    Path userHomeDirectory;
    if (Path::GetUserHomeDirectory(userHomeDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user home directory.");
        return;
    }

    if (GetSystemCreatedDirectory(this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY], userHomeDirectory, "Documents").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user documents directory.");
        return;
    }

    if (GetSystemCreatedDirectory(this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY], userHomeDirectory, "Music").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user music directory.");
        return;
    }

    if (GetSystemCreatedDirectory(this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY], userHomeDirectory, "Videos").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user videos directory.");
        return;
    }

    if (GetSystemCreatedDirectory(this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY], userHomeDirectory, "Pictures").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user pictures directory.");
        return;
    }

    if (GetSystemCreatedDirectory(this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY], userHomeDirectory, "Downloads").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user downloads directory.");
        return;
    }

    Path bestApplicationName;
    if (!applicationName.empty())
    {
        if (bestApplicationName.assign(applicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign application name to best application name.");
            return;
        }
    }
    else
    {
        if (this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path.GetBaseName(bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to get application base name.");
            return;
        }
    }

    if (this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path.assign(userHomeDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign user application settings directory.");
        return;
    }
    if ((this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path /= bestApplicationName).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append application name to user application settings directory.");
        return;
    }

    auto tmpDir = getenv("TMPDIR");
    if (tmpDir != nullptr && tmpDir[0] != 0)
    {
        if (this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path.assign(tmpDir).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign TMPDIR to user application temporary directory.");
            return;
        }

        if ((this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to user application temporary directory.");
            return;
        }
    }
    else
    {
        if (this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path.assign(userHomeDirectory).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign user home directory to user application temporary directory.");
            return;
        }
        if ((this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to user application temporary directory.");
            return;
        }
        if ((this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path /= "temp").HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to 'temp' to user application temporary directory.");
            return;
        }
    }

    if (LinuxUtilities::GetWorkingDirectory(this->paths[WhichStandardPath::WORKING_DIRECTORY].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get working directory.");
        return;
    }
    this->paths[WhichStandardPath::WORKING_DIRECTORY].isSystemCreated = true;
}
