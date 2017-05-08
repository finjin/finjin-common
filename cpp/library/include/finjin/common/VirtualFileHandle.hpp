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

    class VirtualFileSystem;
    class VirtualFileSystemRoot;

    class FINJIN_COMMON_LIBRARY_API VirtualFileHandle
    {
    public:
        VirtualFileHandle();
        ~VirtualFileHandle();

        bool operator == (const VirtualFileHandle& other) const;

        void Clear();

        bool IsOpen() const;
        uint64_t Skip(uint64_t byteCount);
        size_t Read(void* bytes, size_t byteCount);
        size_t Write(const void* bytes, size_t byteCount);
        void Close();

    public:
        VirtualFileSystem* fileSystem;
        VirtualFileSystemRoot* fileSystemRoot;
        uint64_t fileSize; //When reading: For a real file system, the size of the file in bytes. For an archived file system, the size of the unarchived file
        void* ptr;
    };

} }
