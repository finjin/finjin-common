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
#include "finjin/common/Win32FileFinder.hpp"
#include "finjin/common/WindowsUtilities.hpp"
#include <nowide/stackstring.hpp>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static bool IsDots(const wchar_t* s)
{
    if (s[0] != 0)
    {
        if (s[1] == 0)
            return s[0] == '.';
        else if (s[2] == 0)
            return s[0] == '.' && s[1] == '.';
    }
    return false;
}


//Implementation----------------------------------------------------------------
Win32FileFinder::Win32FileFinder(Allocator* allocator) : path(allocator), testPath(allocator)
{
    this->handle = nullptr;
}

Win32FileFinder::~Win32FileFinder()
{
    Stop();
}

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
bool Win32FileFinder::Start(Windows::Storage::StorageFolder^ storageFolder)
{
    Path path(storageFolder->Path->Data());
    return Start(path);
}
#endif

bool Win32FileFinder::Start(const Path& path)
{
    Stop();

    if (this->path.assign(path).HasError())
        return false;

    if (this->testPath.assign(path).HasError())
        return false;
    if ((this->testPath /= "*").HasError())
        return false;

    nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> searchPathW;
    if (!searchPathW.convert(this->testPath.begin(), this->testPath.end()))
        return false;

    this->handle = FindFirstFileExW(searchPathW.c_str(), FindExInfoStandard, &foundData, FindExSearchNameMatch, 0, 0);
    if (this->handle == INVALID_HANDLE_VALUE)
        this->handle = nullptr;

    if (this->handle != nullptr && ::IsDots(this->foundData.cFileName))
        Next();

    return this->handle != nullptr;
}

bool Win32FileFinder::Next()
{
    auto result = FALSE;
    do
    {
        result = FindNextFileW(this->handle, &this->foundData);
    } while (result && ::IsDots(this->foundData.cFileName));

    if (!result)
        Stop();

    return result ? true : false;
}

ValueOrError<void> Win32FileFinder::GetCurrentName(Path& result) const
{
    return result.assign(this->foundData.cFileName);
}

ValueOrError<void> Win32FileFinder::GetCurrentPath(Path& result) const
{
    if (result.assign(this->path).HasError())
        return ValueOrError<void>::CreateError();
    if (this->handle != nullptr)
    {
        if ((result /= this->foundData.cFileName).HasError())
            return ValueOrError<void>::CreateError();
    }
    return ValueOrError<void>();
}

ValueOrError<bool> Win32FileFinder::IsCurrentFile() const
{
    if (this->handle != nullptr)
        return WindowsUtilities::IsFileAttribute(this->foundData.dwFileAttributes);
    else
        return false;
}

ValueOrError<bool> Win32FileFinder::IsCurrentDirectory() const
{
    if (this->handle != nullptr)
        return WindowsUtilities::IsDirectoryAttribute(this->foundData.dwFileAttributes);
    else
        return false;
}

ValueOrError<FileSystemEntryType> Win32FileFinder::GetCurrentType() const
{
    if (this->handle != nullptr)
    {
        if (WindowsUtilities::IsDirectoryAttribute(this->foundData.dwFileAttributes))
            return FileSystemEntryType::DIRECTORY;
        else if (WindowsUtilities::IsFileAttribute(this->foundData.dwFileAttributes))
            return FileSystemEntryType::FILE;
    }
    
    return FileSystemEntryType::NONE;
}

void Win32FileFinder::Stop()
{
    if (this->handle != nullptr)
    {
        FindClose(this->handle);
        this->handle = nullptr;
    }
}

const Path& Win32FileFinder::GetStartPath() const
{
    return this->path;
}
