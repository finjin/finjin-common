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
#include "finjin/common/LibraryLoader.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
    #include <dlfcn.h>
    #define USE_DLOPEN 1
#endif
#include <nowide/stackstring.hpp>

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
LibraryLoader::LibraryLoader()
{
    this->module = nullptr;
}

LibraryLoader::~LibraryLoader()
{
    Unload();
}

bool LibraryLoader::Load(const Path& fileName)
{
    return _Load(fileName) == LoadResult::SUCCESS;
}

void LibraryLoader::Load(const Path& fileName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (_Load(fileName))
    {
        case LoadResult::SUCCESS: break;
        case LoadResult::FAILED_TO_WIDEN_FILE_NAME: FINJIN_SET_ERROR(error, "Failed to widen file name."); break;
        case LoadResult::FAILED_TO_LOAD_LIBRARY:
        {
        #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to load DLL '%1%'.", fileName.c_str()));
        #elif USE_DLOPEN
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to load library '%1%': %2%", fileName.c_str(), dlerror()));
        #else
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to load library '%1%'.", fileName.c_str()));
        #endif
            break;
        }
    }
}

LibraryLoader::LoadResult LibraryLoader::_Load(const Path& fileName)
{
    Unload();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> fileNameW;
    if (!fileNameW.convert(fileName.begin(), fileName.end()))
        return LoadResult::FAILED_TO_WIDEN_FILE_NAME;

    auto module = LoadLibraryW(fileNameW.c_str());
    if (module == nullptr)
        return LoadResult::FAILED_TO_LOAD_LIBRARY;

    this->module = module;
#elif USE_DLOPEN
    auto module = dlopen(fileName.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (module == nullptr)
        return LoadResult::FAILED_TO_LOAD_LIBRARY;

    this->module = module;
#else
    #error Implement this!
#endif

    return LoadResult::SUCCESS;
}

void LibraryLoader::Unload()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    if (this->module != nullptr)
    {
        FreeLibrary((HMODULE)this->module);
        this->module = nullptr;
    }
#elif USE_DLOPEN
    if (this->module != nullptr)
    {
        dlclose(this->module);
        this->module = nullptr;
    }
#else
    #error Implement this!
#endif
}

void* LibraryLoader::GetProcPointer(const char* procName)
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    return GetProcAddress((HMODULE)this->module, procName);
#elif USE_DLOPEN
    return dlsym(this->module, procName);
#else
    #error Implement this!
#endif
}
