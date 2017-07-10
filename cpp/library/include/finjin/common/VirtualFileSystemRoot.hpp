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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FileOperationTypes.hpp"
#include "finjin/common/FileSystemEntry.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/VirtualFileHandle.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class VirtualFileSystem;
    class VirtualFileSystemRoot;

    class FINJIN_COMMON_LIBRARY_API VirtualFileSystemRoot : public AllocatedClass
    {
    public:
        VirtualFileSystemRoot(Allocator* allocator);
        ~VirtualFileSystemRoot();

        enum class EnumerationResult
        {
            COMPLETE,
            INCOMPLETE
        };
        virtual EnumerationResult Enumerate(FileSystemEntries& items, FileSystemEntryType findTypes, size_t maxDepth, Error& error) = 0;

        virtual const Utf8String& GetInternalVolumeID() const = 0;
        virtual const Path& GetFileSystemPath() const = 0;

        virtual FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error) = 0;
        virtual FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer) = 0;

        virtual bool CanOpen(const Path& relativeFilePath, FileOpenMode mode) = 0;
        virtual FileOperationResult Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle) = 0;
        virtual uint64_t Skip(VirtualFileHandle& fileHandle, uint64_t byteCount) = 0;
        virtual size_t Read(VirtualFileHandle& fileHandle, void* bytes, size_t byteCount) = 0;
        virtual size_t Write(VirtualFileHandle& fileHandle, const void* bytes, size_t byteCount) = 0;
        virtual void Close(VirtualFileHandle& fileHandle) = 0;

        const Utf8String& GetDescription() const { return this->description; }

    protected:
        Utf8String description;
    };

} }
