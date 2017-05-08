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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class FileOpenMode
    {
        READ,
        WRITE
    };

    enum class FileOperationResult
    {
        SUCCESS, //File was successfully read or opened
        NOT_INITIALIZED, //File system is in an uninitialized state
        TOO_MANY_OPEN_FILES, //There are too many files already open. Occurs during an open
        NOT_ENOUGH_MEMORY, //Not enough memory to read
        FAILURE, //File was found but could not be accessed for whatever reason
        NOT_FOUND //File was not found
    };

} }
