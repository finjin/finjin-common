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
#include "finjin/common/VirtualFileHandle.hpp"
#include "finjin/common/VirtualFileSystem.hpp"
#include "finjin/common/VirtualFileSystemRoot.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
VirtualFileHandle::VirtualFileHandle()
{
    Clear();
}

VirtualFileHandle::~VirtualFileHandle()
{
    Close();
}

bool VirtualFileHandle::operator == (const VirtualFileHandle& other) const
{
    return this->ptr == other.ptr;
}

void VirtualFileHandle::Clear()
{
    this->fileSystem = nullptr;
    this->fileSystemRoot = nullptr;
    this->fileSize = 0;
    this->ptr = nullptr;
}

bool VirtualFileHandle::IsOpen() const
{
    return this->ptr != nullptr;
}

uint64_t VirtualFileHandle::Skip(uint64_t byteCount)
{
    if (this->fileSystemRoot != nullptr)
        return this->fileSystemRoot->Skip(*this, byteCount);

    return 0;
}

size_t VirtualFileHandle::Read(void* bytes, size_t byteCount)
{
    if (this->fileSystemRoot != nullptr)
        return this->fileSystemRoot->Read(*this, bytes, byteCount);

    return 0;
}

size_t VirtualFileHandle::Write(const void* bytes, size_t byteCount)
{
    if (this->fileSystemRoot != nullptr)
        return this->fileSystemRoot->Write(*this, bytes, byteCount);

    return 0;
}

void VirtualFileHandle::Close()
{
    if (this->ptr != nullptr)
    {
        assert(this->fileSystem != nullptr);
        this->fileSystem->Close(*this);

        Clear();
    }
}
