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

static ValueOrError<void> GetCurrentTypeAndPath(FileSystemEntryType& currentEntryType, Path& currentPath, const WIN32_FIND_DATAW& foundData, const Path& startPath, FileSystemEntryType findTypes)
{
    currentEntryType = FileSystemEntryType::NONE;
    currentPath.clear();

    if (!IsDots(foundData.cFileName))
    {
        if (WindowsUtilities::IsDirectoryAttribute(foundData.dwFileAttributes))
        {
            if (AnySet(findTypes & FileSystemEntryType::DIRECTORY))
                currentEntryType = FileSystemEntryType::DIRECTORY;
        }
        else if (WindowsUtilities::IsFileAttribute(foundData.dwFileAttributes))
        {
            if (AnySet(findTypes & FileSystemEntryType::FILE))
                currentEntryType = FileSystemEntryType::FILE;
        }
    }

    if (currentEntryType != FileSystemEntryType::NONE)
    {
        if (currentPath.assign(startPath).HasError())
            return ValueOrError<void>::CreateError();
        if ((currentPath /= foundData.cFileName).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}


//Implementation----------------------------------------------------------------
Win32FileFinder::Win32FileFinder(Allocator* allocator) : startPath(allocator), currentPath(allocator)
{
    this->findEntryTypes = FileSystemEntryType::ALL;

    this->handle = nullptr;
    this->currentEntryType = FileSystemEntryType::NONE;
}

Win32FileFinder::~Win32FileFinder()
{
    Stop();
}

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
ValueOrError<bool> Win32FileFinder::Start(Windows::Storage::StorageFolder^ storageFolder, FileSystemEntryType findTypes)
{
    Path path(storageFolder->Path->Data());
    return Start(path, findTypes);
}
#endif

ValueOrError<bool> Win32FileFinder::Start(const Path& path, FileSystemEntryType findTypes)
{
    Stop();

    if (this->startPath.assign(path).HasError())
        return ValueOrError<bool>::CreateError();

    this->findEntryTypes = findTypes;

    if (this->currentPath.assign(path).HasError())
        return ValueOrError<bool>::CreateError();
    if ((this->currentPath /= "*").HasError())
        return ValueOrError<bool>::CreateError();

    nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> searchPathW;
    if (!searchPathW.convert(this->currentPath.begin(), this->currentPath.end()))
        return ValueOrError<bool>::CreateError();

    auto searchOp = FindExSearchNameMatch;
    if (this->findEntryTypes == FileSystemEntryType::DIRECTORY)
        searchOp = FindExSearchLimitToDirectories;
    this->handle = FindFirstFileExW(searchPathW.c_str(), FindExInfoStandard, &foundData, searchOp, nullptr, FIND_FIRST_EX_CASE_SENSITIVE);
    if (this->handle == INVALID_HANDLE_VALUE)
        this->handle = nullptr;

    if (this->handle != nullptr)
    {
        if (GetCurrentTypeAndPath(this->currentEntryType, this->currentPath, this->foundData, this->startPath, this->findEntryTypes).HasError())
            return ValueOrError<bool>::CreateError();
        if (NoneSet(this->currentEntryType & this->findEntryTypes))
        {
            if (Next().HasError())
                return ValueOrError<bool>::CreateError();
        }
    }

    return this->handle != nullptr;
}

ValueOrError<bool> Win32FileFinder::Next()
{
    auto foundEntry = FALSE;
    do
    {
        foundEntry = FindNextFileW(this->handle, &this->foundData);
        if (foundEntry)
        {
            if (GetCurrentTypeAndPath(this->currentEntryType, this->currentPath, this->foundData, this->startPath, this->findEntryTypes).HasError())
                return ValueOrError<bool>::CreateError();
        }
        else
        {
            this->currentPath.clear();
            this->currentEntryType = FileSystemEntryType::NONE;
        }
    } while (foundEntry && NoneSet(this->currentEntryType & this->findEntryTypes));

    if (!foundEntry)
        Stop();

    return this->currentEntryType != FileSystemEntryType::NONE;
}

ValueOrError<void> Win32FileFinder::GetCurrentName(Path& result) const
{
    if (this->handle == nullptr)
    {
        result.clear();
        return ValueOrError<void>();
    }
    else
        return result.assign(this->foundData.cFileName);
}

ValueOrError<void> Win32FileFinder::GetCurrentPath(Path& result) const
{
    return result.assign(this->currentPath);
}

bool Win32FileFinder::IsCurrentFile() const
{
    return this->currentEntryType == FileSystemEntryType::FILE;
}

bool Win32FileFinder::IsCurrentDirectory() const
{
    return this->currentEntryType == FileSystemEntryType::DIRECTORY;
}

FileSystemEntryType Win32FileFinder::GetCurrentType() const
{
    return this->currentEntryType;
}

void Win32FileFinder::Stop()
{
    if (this->handle != nullptr)
    {
        FindClose(this->handle);
        this->handle = nullptr;

        this->currentPath.clear();
        this->currentEntryType = FileSystemEntryType::NONE;
    }
}

const Path& Win32FileFinder::GetStartPath() const
{
    return this->startPath;
}
