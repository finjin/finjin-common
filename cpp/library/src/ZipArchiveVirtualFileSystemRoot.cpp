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
#include "finjin/common/ZipArchiveVirtualFileSystemRoot.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
ZipArchiveVirtualFileSystemRoot::ZipArchiveVirtualFileSystemRoot(Allocator* allocator) :
    VirtualFileSystemRoot(allocator),
    volumeID(allocator),
    zipFilePath(allocator)
{
}

void ZipArchiveVirtualFileSystemRoot::OpenRoot(const Path& zipFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (zipFilePath.GetInternalVolumeID(this->volumeID).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get internal volume ID.");
        return;
    }

    if (this->zipFilePath.assign(zipFilePath).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to zip path.");
        return;
    }

    if (this->description.assign("Zip archive @ ").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign base description.");
        return;
    }
    if (this->description.append(zipFilePath.c_str()).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append zip path to base description.");
        return;
    }

    this->zipArchive.Open(zipFilePath, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to open zip archive - it can't be added as a file system root.");
        return;
    }
}

VirtualFileSystemRoot::EnumerationResult ZipArchiveVirtualFileSystemRoot::Enumerate(FileSystemEntries& items, FileSystemEntryType findTypes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->zipArchive.Rewind(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to rewind zip archive.");
        return EnumerationResult::INCOMPLETE;
    }

    ZIPReader::Entry zipEntry;
    while (this->zipArchive.Next(zipEntry, findTypes))
    {
        auto fileSystemEntry = items.Add();
        if (fileSystemEntry == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to get free database entry for zip path.");
            return EnumerationResult::INCOMPLETE;
        }

        if (fileSystemEntry->relativePath.assign(zipEntry.path).HasError())
        {
            items.CancelAdd(fileSystemEntry);

            FINJIN_SET_ERROR(error, "Failed to assign relative zip path.");
            return EnumerationResult::INCOMPLETE;
        }
        fileSystemEntry->relativePath.UniversalNormalize();

        fileSystemEntry->type = zipEntry.type;

        fileSystemEntry->decompressedSize = zipEntry.decompressedSize;
    }

    return EnumerationResult::COMPLETE;
}

const Utf8String& ZipArchiveVirtualFileSystemRoot::GetInternalVolumeID() const
{
    return this->volumeID;
}

const Path& ZipArchiveVirtualFileSystemRoot::GetFileSystemPath() const
{
    return this->zipFilePath;
}

FileOperationResult ZipArchiveVirtualFileSystemRoot::Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto readResult = this->zipArchive.Inflate(buffer, relativeFilePath);
    if (readResult != FileOperationResult::SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to decompress file '%1%' from the zip archive.", relativeFilePath));
        return readResult;
    }

    return FileOperationResult::SUCCESS;
}

FileOperationResult ZipArchiveVirtualFileSystemRoot::Read(const Path& relativeFilePath, ByteBuffer& buffer)
{
    return this->zipArchive.Inflate(buffer, relativeFilePath);
}

bool ZipArchiveVirtualFileSystemRoot::CanOpen(const Path& relativeFilePath, FileOpenMode mode)
{
    if (mode == FileOpenMode::READ)
        return this->zipArchive.TestStartInflate(relativeFilePath) == FileOperationResult::SUCCESS;
    else
        return false;
}

FileOperationResult ZipArchiveVirtualFileSystemRoot::Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle)
{
    assert(!fileHandle.IsOpen());

    size_t inflatedSize;
    auto openResult = this->zipArchive.StartInflate(relativeFilePath, inflatedSize);
    if (openResult != FileOperationResult::SUCCESS)
        return openResult;

    fileHandle.fileSize = inflatedSize;

    fileHandle.ptr = &this->zipArchive;

    fileHandle.fileSystemRoot = this;

    return FileOperationResult::SUCCESS;
}

uint64_t ZipArchiveVirtualFileSystemRoot::Skip(VirtualFileHandle& fileHandle, uint64_t byteCount)
{
    assert(fileHandle.ptr == &this->zipArchive);

    if (byteCount > std::numeric_limits<size_t>::max())
        byteCount = std::numeric_limits<size_t>::max();

    size_t bytesSkipped = 0;
    if (fileHandle.ptr == &this->zipArchive)
        this->zipArchive.Skip(bytesSkipped, byteCount);
    return bytesSkipped;
}

size_t ZipArchiveVirtualFileSystemRoot::Read(VirtualFileHandle& fileHandle, void* bytes, size_t byteCount)
{
    assert(fileHandle.ptr == &this->zipArchive);

    size_t bytesDecompressed = 0;
    if (fileHandle.ptr == &this->zipArchive)
        this->zipArchive.PartialInflate(bytes, bytesDecompressed, byteCount);
    return bytesDecompressed;
}

size_t ZipArchiveVirtualFileSystemRoot::Write(VirtualFileHandle& fileHandle, const void* bytes, size_t byteCount)
{
    return 0;
}

void ZipArchiveVirtualFileSystemRoot::Close(VirtualFileHandle& fileHandle)
{
    assert(fileHandle.ptr == &this->zipArchive);

    if (fileHandle.ptr == &this->zipArchive)
        this->zipArchive.StopInflate();

    fileHandle.ptr = nullptr;
}
