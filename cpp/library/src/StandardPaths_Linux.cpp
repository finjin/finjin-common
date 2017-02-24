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


//Local functions--------------------------------------------------------------
static void GetSystemCreatedDirectory(StandardPath& standardPath, const Path& userHomeDirectory, const char* standardName)
{    
    standardPath.path = userHomeDirectory;
    standardPath.path /= standardName;
    standardPath.isSystemCreated = standardPath.path.IsDirectory();
    if (!standardPath.isSystemCreated)
        standardPath.path.clear();
}


//Implementation---------------------------------------------------------------
void StandardPaths::Create(const Utf8String& applicationName, void* applicationHandle, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    
    this->applicationExecutableFile.path = LinuxUtilities::GetProcessFilePath();
    this->applicationExecutableFile.isSystemCreated = true;
    
    this->applicationBundleDirectory.path = this->applicationExecutableFile.path;
    this->applicationBundleDirectory.path.RemoveFileName();
    this->applicationBundleDirectory.isSystemCreated = true;
    
    Path userHomeDirectory;
    Path::GetUserHomeDirectory(userHomeDirectory);
    
    GetSystemCreatedDirectory(this->userDocumentsDirectory, userHomeDirectory, "Documents");    
    GetSystemCreatedDirectory(this->userMusicDirectory, userHomeDirectory, "Music");
    GetSystemCreatedDirectory(this->userVideosDirectory, userHomeDirectory, "Videos");
    GetSystemCreatedDirectory(this->userPicturesDirectory, userHomeDirectory, "Pictures");
    GetSystemCreatedDirectory(this->userDownloadsDirectory, userHomeDirectory, "Downloads");
    
    Path bestApplicationName;
    if (!applicationName.empty())
        bestApplicationName = applicationName;
    else
        this->applicationExecutableFile.path.GetBaseName(bestApplicationName);
    
    this->userApplicationSettingsDirectory.path = userHomeDirectory;
    this->userApplicationSettingsDirectory.path /= bestApplicationName;
    
    auto tmpDir = getenv("TMPDIR");
    if (tmpDir != nullptr && tmpDir[0] != 0)
    {
        this->userApplicationTemporaryDirectory.path = tmpDir;
        this->userApplicationTemporaryDirectory.path /= bestApplicationName;
    }
    else
    {
        this->userApplicationTemporaryDirectory.path = userHomeDirectory;
        this->userApplicationTemporaryDirectory.path /= bestApplicationName;
        this->userApplicationTemporaryDirectory.path /= "temp";    
    }
    
    this->workingDirectory.path = LinuxUtilities::GetWorkingDirectory();
    this->workingDirectory.isSystemCreated = true;
}
