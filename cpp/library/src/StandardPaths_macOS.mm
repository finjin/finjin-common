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
#include "finjin/common/AppleUtilities.hpp"
#import <Foundation/NSString.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSBundle.h>

using namespace Finjin::Common;


//Local functions--------------------------------------------------------------
static ValueOrError<void> GetSystemCreatedDirectory(StandardPath& standardPath, NSSearchPathDirectory which)
{
    auto foundAt = [NSSearchPathForDirectoriesInDomains(which, NSUserDomainMask, YES) lastObject];
    if (foundAt != nil)
    {
        if (standardPath.path.assign(foundAt.UTF8String).HasError())
            return ValueOrError<void>::CreateError();
        standardPath.path.PlatformNormalize();
        standardPath.isSystemCreated = true;
    }
    else
        standardPath.path.clear();
    
    return ValueOrError<void>();
}


//Implementation---------------------------------------------------------------
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
    
    if (this->applicationExecutableFile.path.assign(AppleUtilities::GetProcessFilePath()).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign process file path to application executable file path.");
        return;
    }
    this->applicationExecutableFile.isSystemCreated = true;

    if (this->applicationBundleDirectory.path.assign(mainBundle.bundlePath.UTF8String).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to bundle path to application bundle directory path.");
        return;
    }
    if ((this->applicationBundleDirectory.path /= "Contents/Resources").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append 'Contents/Resources' to application bundle directory path.");
        return;
    }
    this->applicationBundleDirectory.isSystemCreated = true;

    if (GetSystemCreatedDirectory(this->userDocumentsDirectory, NSDocumentDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user documents directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userMusicDirectory, NSMusicDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user music directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userVideosDirectory, NSMoviesDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user movies directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userPicturesDirectory, NSPicturesDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user pictures directory.");
        return;
    }
    
    if (GetSystemCreatedDirectory(this->userDownloadsDirectory, NSDownloadsDirectory).HasError())
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
        if (this->applicationExecutableFile.path.GetBaseName(bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to get application executable file name for best application name.");
            return;
        }
    }
    
    if (GetSystemCreatedDirectory(this->userApplicationSettingsDirectory, NSApplicationSupportDirectory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get user application support directory.");
        return;
    }
    if ((this->userApplicationSettingsDirectory.path /= bestApplicationName).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append application name to user application settings directory.");
        return;
    }
    this->userApplicationSettingsDirectory.isSystemCreated = false;
    
    auto tempDirectory = NSTemporaryDirectory();
    if (tempDirectory != nil)
    {
        if (this->userApplicationTemporaryDirectory.path.assign(tempDirectory.UTF8String).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to get assign temporary directory.");
            return;
        }
        if ((this->userApplicationTemporaryDirectory.path /= bestApplicationName).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append application name to temporary directory.");
            return;
        }
    }
    
    if (this->workingDirectory.path.assign(AppleUtilities::GetWorkingDirectory()).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign working directory.");
        return;
    }
    this->workingDirectory.isSystemCreated = true;
}
