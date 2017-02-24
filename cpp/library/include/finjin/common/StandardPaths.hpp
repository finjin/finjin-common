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


#pragma once


//Includes---------------------------------------------------------------------
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class StandardPath
    {
    public:
        StandardPath(Allocator* allocator = nullptr) : path(allocator), internalDisplayName(allocator), displayName(allocator)
        {
            this->isSystemCreated = false;
        }

        StandardPath(const char* _internalDisplayName, Allocator* allocator = nullptr) : path(allocator), internalDisplayName(_internalDisplayName, allocator), displayName(allocator)
        {
            this->isSystemCreated = false;
        }

        const Utf8String& GetDisplayName() const
        {
            return !this->displayName.empty() ? this->displayName : this->internalDisplayName;
        }

        bool CreateDirectories() const
        {
            if (!this->isSystemCreated && !this->path.empty())
                return this->path.CreateDirectories();
            else
                return false;
        }

    public:
        Path path;
        Utf8String internalDisplayName; //Internal name
        Utf8String displayName; //System defined name. If empty, a standard name should be used, such as "Video"
        bool isSystemCreated; //Indicates whether the path is defined by the system. If false, it's specific to this application and may need to be created
    };

    class StandardPaths
    {
    public:
        template <typename T>
        bool ForEach(T callback) const //std::function<bool(const StandardPath&)>
        {
            if (!this->applicationExecutableFile.path.empty())
            {
                if (!callback(this->applicationExecutableFile))
                    return false;
            }

            if (!this->applicationBundleDirectory.path.empty())
            {
                if (!callback(this->applicationBundleDirectory))
                    return false;
            }

            if (!ForEachUserPath(callback))
                return false;

            if (!this->workingDirectory.path.empty())
            {
                if (!callback(this->workingDirectory))
                    return false;
            }

            return true;
        }

        template <typename T>
        bool ForEachUserPath(T callback) const //std::function<bool(const StandardPath&)>
        {
            if (!this->userDocumentsDirectory.path.empty())
            {
                if (!callback(this->userDocumentsDirectory))
                    return false;
            }

            if (!this->userMusicDirectory.path.empty())
            {
                if (!callback(this->userMusicDirectory))
                    return false;
            }

            if (!this->userVideosDirectory.path.empty())
            {
                if (!callback(this->userVideosDirectory))
                    return false;
            }

            if (!this->userPicturesDirectory.path.empty())
            {
                if (!callback(this->userPicturesDirectory))
                    return false;
            }

            if (!this->userSavedPicturesDirectory.path.empty())
            {
                if (!callback(this->userSavedPicturesDirectory))
                    return false;
            }

            if (!this->userCameraRollDirectory.path.empty())
            {
                if (!callback(this->userCameraRollDirectory))
                    return false;
            }

            if (!this->userDownloadsDirectory.path.empty())
            {
                if (!callback(this->userDownloadsDirectory))
                    return false;
            }

            if (!this->userApplicationSettingsDirectory.path.empty())
            {
                if (!callback(this->userApplicationSettingsDirectory))
                    return false;
            }

            if (!this->userApplicationTemporaryDirectory.path.empty())
            {
                if (!callback(this->userApplicationTemporaryDirectory))
                    return false;
            }

            return true;
        }

        StandardPath applicationExecutableFile;
        StandardPath applicationBundleDirectory;

        //On Windows Win32: C:\Users\(username)\Documents
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Documents
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Documents
        StandardPath userDocumentsDirectory; 

        //On Windows Win32: C:\Users\(username)\Music
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Music
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Music
        StandardPath userMusicDirectory; 

        //On Windows Win32: C:\Users\(username)\Video
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Movies
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Videos
        StandardPath userVideosDirectory; 

        //On Windows Win32: C:\Users\(username)\Pictures
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Pictures
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Pictures
        StandardPath userPicturesDirectory; 

        StandardPath userSavedPicturesDirectory;

        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/DCIM
        StandardPath userCameraRollDirectory;        

        //On Windows Win32: C:\Users\(username)\Downloads
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Downloads
        StandardPath userDownloadsDirectory; 

        //On Windows Win32: C:\Users\(username)\AppData\Local\(applicationName passed to Initialize() or application executable name)
        //On Windows UWP: C:\Users\(username)\AppData\Local\Packages\(app guid)\LocalState
        //On Android 6 (as tested on Nexus 6): androidApp->activity->internalDataPath: /data/user/(userid)/(package name in AndroidManifest.xml)/files
        //On Android 6 (final fallback behavior): androidApp->activity->externalDataPath: /storage/emulated/(userid)/Android/data/(package name in AndroidManifest.xml)/files
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/(applicationName passed to Initialize() or application executable name)
        StandardPath userApplicationSettingsDirectory; 
        
        //On Windows Win32: C:\Users\(username)\AppData\Local\Temp\(applicationName passed to Initialize() or application executable name)
        //On Windows Win32 (fallback): C:\Users\(username)\AppData\Local\(applicationName passed to Initialize() or application executable name)\Temp
        //On Windows UWP: C:\Users\(username)\AppData\Local\Temp\(applicationName passed to Initialize())
        //On Android 6 (as tested on Nexus 6): /data/user/(userid)/(package name in AndroidManifest.xml)/cache
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/(applicationName passed to Initialize() or application executable name)/temp
        StandardPath userApplicationTemporaryDirectory; 
        
        //On Windows Win32: C:\Users\All Users\Application Data\(applicationName passed to Initialize() or application executable name)
        StandardPath applicationSettingsDirectory;

        StandardPath workingDirectory;

        enum { MAX_USER_PATHS = 9 }; //The number of above paths that start with 'user'

    #if FINJIN_TARGET_OS == FINJIN_TARGET_OS_WINDOWS_WIN32
        enum { MAX_LOGICAL_DRIVES = 26 }; //A - Z
        struct LogicalDrive
        {
            char name[3];

            LogicalDrive() { this->name[0] = 0; }
            LogicalDrive(const char* other) { strcpy(this->name, other); }

            operator const char* () const { return this->name; }
        };
        StaticVector<LogicalDrive, MAX_LOGICAL_DRIVES> logicalDrives; //Of the form "C:", "D:", etc.
    #endif

    public:
        StandardPaths(Allocator* allocator = nullptr) : 
            applicationExecutableFile("Application Executable", allocator),
            applicationBundleDirectory("Application Bundle", allocator),
            userDocumentsDirectory("Documents", allocator),
            userMusicDirectory("Music", allocator),
            userVideosDirectory("Videos", allocator),
            userPicturesDirectory("Pictures", allocator),
            userSavedPicturesDirectory("Saved Pictures", allocator),
            userCameraRollDirectory("Camera Roll", allocator),
            userDownloadsDirectory("Downloads", allocator),
            userApplicationSettingsDirectory("Application Settings", allocator),
            userApplicationTemporaryDirectory("Application Temporary", allocator),
            workingDirectory("Working Directory", allocator)
        {            
        }

        void Create(const Utf8String& applicationName, void* applicationHandle, Error& error);
    };

} }
