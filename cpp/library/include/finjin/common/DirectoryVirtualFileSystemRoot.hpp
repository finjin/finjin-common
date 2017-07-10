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


//Includes----------------------------------------------------------------------
#include "finjin/common/VirtualFileSystemRoot.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API DirectoryVirtualFileSystemRoot : public VirtualFileSystemRoot
    {
    public:
        DirectoryVirtualFileSystemRoot(Allocator* allocator);

        void OpenRoot(const Path& directory, Error& error);

        EnumerationResult Enumerate(FileSystemEntries& items, FileSystemEntryType findTypes, size_t maxDepth, Error& error) override;

        const Utf8String& GetInternalVolumeID() const override;
        const Path& GetFileSystemPath() const override;

        FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error) override;
        FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer) override;

        bool CanOpen(const Path& relativeFilePath, FileOpenMode mode) override;
        FileOperationResult Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle) override;
        uint64_t Skip(VirtualFileHandle& fileHandle, uint64_t byteCount) override;
        size_t Read(VirtualFileHandle& fileHandle, void* bytes, size_t byteCount) override;
        size_t Write(VirtualFileHandle& fileHandle, const void* bytes, size_t byteCount) override;
        void Close(VirtualFileHandle& fileHandle) override;

    private:
        Utf8String volumeID;
        Path directory;
        Path workingFilePath;
    };

} }
