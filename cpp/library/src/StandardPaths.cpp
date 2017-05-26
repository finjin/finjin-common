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

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
StandardPaths::StandardPaths(Allocator* allocator)
{
    this->paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].Create(WhichStandardPath::APPLICATION_EXECUTABLE_FILE, "Application Executable", allocator);
    this->paths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].Create(WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY, "Application Bundle", allocator);
    this->paths[WhichStandardPath::APPLICATION_SETTINGS_DIRECTORY].Create(WhichStandardPath::APPLICATION_SETTINGS_DIRECTORY, "Global Application Settings", allocator);
    this->paths[WhichStandardPath::USER_DOCUMENTS_DIRECTORY].Create(WhichStandardPath::USER_DOCUMENTS_DIRECTORY, "User Documents", allocator);
    this->paths[WhichStandardPath::USER_MUSIC_DIRECTORY].Create(WhichStandardPath::USER_MUSIC_DIRECTORY, "User Music", allocator);
    this->paths[WhichStandardPath::USER_VIDEOS_DIRECTORY].Create(WhichStandardPath::USER_VIDEOS_DIRECTORY, "User Videos", allocator);
    this->paths[WhichStandardPath::USER_PICTURES_DIRECTORY].Create(WhichStandardPath::USER_PICTURES_DIRECTORY, "User Pictures", allocator);
    this->paths[WhichStandardPath::USER_SAVED_PICTURES_DIRECTORY].Create(WhichStandardPath::USER_SAVED_PICTURES_DIRECTORY, "User Saved Pictures", allocator);
    this->paths[WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY].Create(WhichStandardPath::USER_CAMERA_ROLL_DIRECTORY, "User Camera Roll", allocator);
    this->paths[WhichStandardPath::USER_DOWNLOADS_DIRECTORY].Create(WhichStandardPath::USER_DOWNLOADS_DIRECTORY, "User Downloads", allocator);
    this->paths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].Create(WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY, "User Application Settings", allocator);
    this->paths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].Create(WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY, "User Application Temporary Directory", allocator);
    this->paths[WhichStandardPath::WORKING_DIRECTORY].Create(WhichStandardPath::WORKING_DIRECTORY, "Working Directory", allocator);
}
