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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class FINJIN_COMMON_LIBRARY_API LibraryLoader
    {
    public:
        LibraryLoader();
        ~LibraryLoader();

        bool Load(const Path& fileName);
        void Load(const Path& fileName, Error& error);
        void Unload();

        template <typename T>
        T GetProc(const char* procName)
        {
            return reinterpret_cast<T>(GetProcPointer(procName));
        }
        
        void* GetProcPointer(const char* procName);

    private:
        void* module;
    };

} }
