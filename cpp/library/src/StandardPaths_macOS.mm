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
#define FINJIN_APPLE_OBJCPP_UTILITIES 1
#include "finjin/common/AppleUtilities.hpp"
#import <Foundation/NSBundle.h>
#import <Foundation/NSString.h>
#import <Foundation/NSPathUtilities.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static ValueOrError<void> GetSystemDirectory(StandardPath& standardPath, NSSearchPathDirectory which,bool tryCreateIfNotPresent = true)
{
    return AppleUtilities::GetSystemDirectory(standardPath.path, standardPath.isSystemCreated, which, tryCreateIfNotPresent);
}


//Implementation----------------------------------------------------------------
void StandardPaths::Create(const Utf8String& _applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto mainBundle = [NSBundle mainBundle];
    Utf8String applicationName;
    NSString* bundleID = [mainBundle objectForInfoDictionaryKey:@"CFBundleIdentifier"];
    if (bundleID != nullptr && bundleID.length > 0)
    {
        if (applicationName.assign(bundleID.UTF8String).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign bundle ID to application name.");
            return;
        }
    }
    else
    {
        if (applicationName.assign(_applicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to copy application name.");
            return;
        }
    }

    if (AppleUtilities::GetProcessFilePath(this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign process file path to application executable file path.");
        return;
    }
    this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].isSystemCreated = true;

    if (this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path.assign(mainBundle.bundlePath.UTF8String).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to bundle path to application bundle directory path.");
        return;
    }
    if ((this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path /= "Contents/Resources").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append 'Contents/Resources' to application bundle directory path.");
        return;
    }
    this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].isSystemCreated = true;

    if (GetSystemDirectory(this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY], NSDocumentDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user documents directory.");
        return;
    }

    if (GetSystemDirectory(this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY], NSMusicDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user music directory.");
        return;
    }

    if (GetSystemDirectory(this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY], NSMoviesDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user movies directory.");
        return;
    }

    if (GetSystemDirectory(this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY], NSPicturesDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user pictures directory.");
        return;
    }

    if (GetSystemDirectory(this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY], NSDownloadsDirectory).HasError())
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
            FINJIN_SET_ERROR(error, "Failed to get application executable file name for best application name.");
            return;
        }
    }

    {
        auto& standardPath = this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY];
        if (GetSystemDirectory(standardPath, NSApplicationSupportDirectory, false).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to get user application support directory.");
            return;
        }
        if ((standardPath.path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to user application settings directory.");
            return;
        }
        if (!standardPath.path.IsDirectory())
        {
            if (!standardPath.path.CreateDirectories())
                standardPath.path.clear();
        }
    }

    auto tempDirectory = NSTemporaryDirectory();
    if (tempDirectory != nullptr)
    {
        auto& standardPath = this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY];
        if (standardPath.path.assign(tempDirectory.UTF8String).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to get assign temporary directory.");
            return;
        }
        if ((standardPath.path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to temporary directory.");
            return;
        }
        if (!standardPath.path.IsDirectory())
        {
            if (!standardPath.path.CreateDirectories())
                standardPath.path.clear();
        }
    }

    if (AppleUtilities::GetWorkingDirectory(this->paths[WhichStandardPath::WORKING_DIRECTORY].path).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign working directory.");
        return;
    }
    this->paths[WhichStandardPath::WORKING_DIRECTORY].isSystemCreated = true;
}
