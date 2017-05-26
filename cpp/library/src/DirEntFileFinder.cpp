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
#include "finjin/common/DirEntFileFinder.hpp"
#include <sys/stat.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static bool IsDots(const char* s)
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

static ValueOrError<void> GetCurrentTypeAndPath(FileSystemEntryType& currentEntryType, Path& currentPath, const DIR* dir, const struct dirent* ent, const Path& startPath, FileSystemEntryType findTypes)
{
    currentEntryType = FileSystemEntryType::NONE;
    currentPath.clear();

    if (ent != nullptr && !IsDots(ent->d_name))
    {
        if (ent->d_type == DT_DIR)
        {
            if (AnySet(findTypes & FileSystemEntryType::DIRECTORY))
            {
                currentEntryType = FileSystemEntryType::DIRECTORY;

                if (currentPath.assign(startPath).HasError())
                    return ValueOrError<void>::CreateError();
                if ((currentPath /= ent->d_name).HasError())
                    return ValueOrError<void>::CreateError();
            }
        }
        else if (ent->d_type == DT_REG)
        {
            if (AnySet(findTypes & FileSystemEntryType::FILE))
            {
                currentEntryType = FileSystemEntryType::FILE;

                if (currentPath.assign(startPath).HasError())
                    return ValueOrError<void>::CreateError();
                if ((currentPath /= ent->d_name).HasError())
                    return ValueOrError<void>::CreateError();
            }
        }
        else if (ent->d_type == DT_LNK)
        {
            if (currentPath.assign(startPath).HasError())
                return ValueOrError<void>::CreateError();
            if ((currentPath /= ent->d_name).HasError())
                return ValueOrError<void>::CreateError();

            if (currentPath.IsDirectory())
            {
                if (AnySet(findTypes & FileSystemEntryType::DIRECTORY))
                    currentEntryType = FileSystemEntryType::DIRECTORY;
            }
            else if (currentPath.IsFile())
            {
                if (AnySet(findTypes & FileSystemEntryType::FILE))
                    currentEntryType = FileSystemEntryType::FILE;
            }
        }
    }

    return ValueOrError<void>();
}


//Implementation----------------------------------------------------------------
DirEntFileFinder::DirEntFileFinder(Allocator* allocator) : startPath(allocator), currentPath(allocator)
{
    this->findTypes = FileSystemEntryType::ALL;

    this->dir = nullptr;
    this->ent = nullptr;
    this->currentEntryType = FileSystemEntryType::NONE;
}

DirEntFileFinder::~DirEntFileFinder()
{
    Stop();
}

ValueOrError<bool> DirEntFileFinder::Start(const Path& path, FileSystemEntryType findTypes)
{
    Stop();

    if (this->startPath.assign(path).HasError())
        return ValueOrError<bool>::CreateError();

    this->findTypes = findTypes;

    this->dir = opendir(path.c_str());
    this->ent = this->dir != nullptr ? readdir(this->dir) : nullptr;
    this->currentEntryType = FileSystemEntryType::NONE;

    if (this->ent != nullptr)
    {
        GetCurrentTypeAndPath(this->currentEntryType, this->currentPath, this->dir, this->ent, this->startPath, this->findTypes);
        if (NoneSet(this->currentEntryType & this->findTypes))
        {
            if (Next().HasError())
                return ValueOrError<bool>::CreateError(false);
        }
    }

    return this->ent != nullptr;
}

ValueOrError<bool> DirEntFileFinder::Next()
{
    do
    {
        this->ent = readdir(this->dir);
        if (this->ent != nullptr)
        {
            if (GetCurrentTypeAndPath(this->currentEntryType, this->currentPath, this->dir, this->ent, this->startPath, this->findTypes).HasError())
                return ValueOrError<bool>::CreateError();
        }
        else
        {
            this->currentPath.clear();
            this->currentEntryType = FileSystemEntryType::NONE;
        }
    } while (this->ent != nullptr && NoneSet(this->currentEntryType & this->findTypes));

    if (this->ent == nullptr)
        Stop();

    return this->currentEntryType != FileSystemEntryType::NONE;
}

ValueOrError<void> DirEntFileFinder::GetCurrentName(Path& result) const
{
    if (this->ent == nullptr)
    {
        result.clear();
        return ValueOrError<void>();
    }
    else
        return result.assign(this->ent->d_name);
}

ValueOrError<void> DirEntFileFinder::GetCurrentPath(Path& result) const
{
    return result.assign(this->currentPath);
}

bool DirEntFileFinder::IsCurrentFile() const
{
    return this->currentEntryType == FileSystemEntryType::FILE;
}

bool DirEntFileFinder::IsCurrentDirectory() const
{
    return this->currentEntryType == FileSystemEntryType::DIRECTORY;
}

FileSystemEntryType DirEntFileFinder::GetCurrentType() const
{
    return this->currentEntryType;
}

void DirEntFileFinder::Stop()
{
    if (this->dir != nullptr)
    {
        closedir(this->dir);
        this->dir = nullptr;
        this->ent = nullptr;
        this->currentPath.clear();
        this->currentEntryType = FileSystemEntryType::NONE;
    }
}

const Path& DirEntFileFinder::GetStartPath() const
{
    return this->startPath;
}
