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


//Local functions--------------------------------------------------------------
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


//Implementation---------------------------------------------------------------
DirEntFileFinder::DirEntFileFinder(Allocator* allocator) : path(allocator), testPath(allocator)
{
    this->dir = nullptr;
    this->ent = nullptr;
}

DirEntFileFinder::~DirEntFileFinder()
{
    Stop();
}

bool DirEntFileFinder::Start(const Path& path)
{
    Stop();

    if (this->path.assign(path).HasError())
        return false;

    this->dir = opendir(path.c_str());
    this->ent = this->dir != nullptr ? readdir(this->dir) : nullptr;
    
    if (this->ent != nullptr && ::IsDots(this->ent->d_name))
        Next();
    
    return this->ent != nullptr;
}

bool DirEntFileFinder::Next()
{
    do
    {
        this->ent = readdir(this->dir);
    } while (this->ent != nullptr && ::IsDots(this->ent->d_name));    
    
    return this->ent != nullptr;
}

ValueOrError<void> DirEntFileFinder::GetCurrentName(Path& result) const
{
    return result.assign(this->ent->d_name);
}

ValueOrError<void> DirEntFileFinder::GetCurrentPath(Path& result) const
{
    if (result.assign(this->path).HasError())
        return ValueOrError<void>::CreateError();

    if (this->ent != nullptr)
        return result /= this->ent->d_name;
    else
        return ValueOrError<void>();
}

ValueOrError<bool> DirEntFileFinder::IsCurrentFile() const
{
    if (this->ent != nullptr)
    {
        if (this->testPath.assign(this->path).HasError())
            return ValueOrError<bool>::CreateError();
        if ((this->testPath /= this->ent->d_name).HasError())
            return ValueOrError<bool>::CreateError();
        
        struct stat statBuf;
        if (stat(this->testPath.c_str(), &statBuf) == -1)
            return false;

        return S_ISREG(statBuf.st_mode) != 0;
    }
    else
        return false;
}

ValueOrError<bool> DirEntFileFinder::IsCurrentDirectory() const
{
    if (this->ent != nullptr)
    {
        if (this->testPath.assign(this->path).HasError())
            return ValueOrError<bool>::CreateError();
        if ((this->testPath /= this->ent->d_name).HasError())
            return ValueOrError<bool>::CreateError();

        struct stat statBuf;
        if (stat(this->testPath.c_str(), &statBuf) == -1)
            return false;

        return S_ISDIR(statBuf.st_mode) != 0;
    }
    else
        return false;
}

void DirEntFileFinder::Stop()
{
    if (this->dir != nullptr)
    {
        closedir(this->dir);
        this->dir = nullptr;
        this->ent = nullptr;
    }
}

const Path& DirEntFileFinder::GetStartPath() const 
{ 
    return this->path; 
}
