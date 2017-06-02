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


//Includes----------------------------------------------------------------------
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class WhichStandardPath
    {
        APPLICATION_EXECUTABLE_FILE, //The actual application .exe, if applicable
        APPLICATION_BUNDLE_DIRECTORY,

        //On Windows Win32: C:\Users\All Users\Application Data\(applicationName passed to Initialize() or application executable name)
        APPLICATION_SETTINGS_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\Documents
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Documents
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Documents
        //On macOS: /Users/(username)/Documents
        //On iOS: /var/mobile/Containers/Data/Application/(application UUID)/Documents
        USER_DOCUMENTS_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\Music
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Music
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Music
        //On macOS: /Users/(username)/Music
        USER_MUSIC_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\Video
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Movies
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Videos
        //On macOS: /Users/(username)/Movies
        USER_VIDEOS_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\Pictures
        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/Pictures
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Pictures
        //On macOS: /Users/(username)/Pictures
        USER_PICTURES_DIRECTORY,

        USER_SAVED_PICTURES_DIRECTORY,

        //On Android 6 (as tested on Nexus 6): /storage/emulated/(userid)/DCIM
        USER_CAMERA_ROLL_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\Downloads
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/Downloads
        //On macOS: /Users/(username)/Downloads
        USER_DOWNLOADS_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\AppData\Local\(applicationName passed to Initialize() or application executable name)
        //On Windows UWP: C:\Users\(username)\AppData\Local\Packages\(app guid)\LocalState
        //On Android 6 (as tested on Nexus 6): androidApp->activity->internalDataPath: /data/user/(userid)/(package name in AndroidManifest.xml)/files
        //On Android 6 (final fallback behavior): androidApp->activity->externalDataPath: /storage/emulated/(userid)/Android/data/(package name in AndroidManifest.xml)/files
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/(applicationName passed to Initialize() or application executable name)
        //On macOS: /Users/(username)/Library/Application Support/(applicationName passed to Initialize() or application executable name)
        //On iOS: /var/mobile/Containers/Data/Application/(application UUID)/Library/Application Support
        USER_APPLICATION_SETTINGS_DIRECTORY,

        //On Windows Win32: C:\Users\(username)\AppData\Local\Temp\(applicationName passed to Initialize() or application executable name)
        //On Windows Win32 (fallback): C:\Users\(username)\AppData\Local\(applicationName passed to Initialize() or application executable name)\Temp
        //On Windows UWP: C:\Users\(username)\AppData\Local\Temp\(applicationName passed to Initialize())
        //On Android 6 (as tested on Nexus 6): /data/user/(userid)/(package name in AndroidManifest.xml)/cache
        //On Linux (as tested on Ubuntu 15.x): /home/(username)/(applicationName passed to Initialize() or application executable name)/temp
        //On macOS: /var/folders/(random numbers)/(applicationName passed to Initialize() or application executable name)
        //On iOS: /private/var/mobile/Containers/Data/Application/(application UUID)/tmp
        USER_APPLICATION_TEMPORARY_DIRECTORY,

        WORKING_DIRECTORY,

        COUNT
    };

    class StandardPath
    {
    public:
        enum { MAX_USER_PATHS = 9 }; //The number of the above paths that start with "USER_"

        StandardPath(Allocator* allocator = nullptr);

        void Create(WhichStandardPath which, const char* _internalDisplayName, Allocator* allocator = nullptr);

        const Utf8String& GetDisplayName() const;

        enum class CreateDirectoriesResult
        {
            SUCCESS,
            DIRECTORY_EMPTY,
            SYSTEM_CREATED,
            SYSTEM_CREATED_DOESNT_EXIST,
            FAILURE
        };
        CreateDirectoriesResult CreateDirectories() const;

    public:
        WhichStandardPath which;
        Path path;
        Utf8String internalDisplayName; //Internal name
        Utf8String defaultDisplayName; //System defined name. If empty, a standard name should be used, such as "Video"
        bool isSystemCreated; //Indicates whether the path is defined by the system. If false, it's specific to this application and may need to be created
    };

    class StandardPaths
    {
    public:
        StandardPaths(Allocator* allocator = nullptr);

        void Create(const Utf8String& applicationName, void* applicationHandle, Error& error);

        const StandardPath* GetBestSavedScreenCapturePath() const;

        template <typename T>
        bool ForEach(T callback, Error& error) const //std::function<bool(const StandardPath&, Error&)>
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto standardPath : {
                &this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE],
                &this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY],
                &this->paths[WhichStandardPath::APPLICATION_SETTINGS_DIRECTORY]
                })
            {
                auto result = callback(*standardPath, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error occurred while iterating on application standard path '%1%' at '%2%'.", standardPath->GetDisplayName(), standardPath->path));
                    return false;
                }
                if (!result)
                    return false;
            }

            {
                auto result = ForEachUserPath(callback, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to process a user path.");
                    return false;
                }
                if (!result)
                    return false;
            }

            for (auto standardPath : { &this->paths[WhichStandardPath::WORKING_DIRECTORY] })
            {
                auto result = callback(*standardPath, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error occurred while iterating on working directory path '%1%' at '%2%'.", standardPath->GetDisplayName(), standardPath->path));
                    return false;
                }
                if (!result)
                    return false;
            }

            return true;
        }

        template <typename T>
        bool ForEachUserPath(T callback, Error& error) const //std::function<bool(const StandardPath&, Error& error)>
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto standardPath : {
                &this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY],
                &this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY],
                &this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY],
                &this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY],
                &this->paths[WhichStandardPath::USER_SAVED_PICTURES_DIRECTORY],
                &this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY],
                &this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY],
                &this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY],
                &this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY]
                })
            {
                if (!standardPath->path.empty())
                {
                    auto result = callback(*standardPath, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error occurred while iterating on standard user path '%1%' at '%2%'.", standardPath->GetDisplayName(), standardPath->path));
                        return false;
                    }
                    if (!result)
                        return false;
                }
            }

            return true;
        }

        template <typename Index> const StandardPath& operator [] (Index index) const { return this->paths[index]; }
        template <typename Index> StandardPath& operator [] (Index index) { return this->paths[index]; }

    public:
        EnumArray<WhichStandardPath, WhichStandardPath::COUNT, StandardPath> paths;

    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_WINDOWS_WIN32
        struct LogicalDrive
        {
            char name[3];

            LogicalDrive() { this->name[0] = 0; }
            LogicalDrive(const char* other) { strcpy(this->name, other); }

            operator const char* () const { return this->name; }
        };
        StaticVector<LogicalDrive, 26> logicalDrives; //Of the form "C:", "D:", etc.
    #endif
    };

} }
