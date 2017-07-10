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
#include "finjin/common/DirectoryVirtualFileSystemRoot.hpp"
#include "finjin/common/FileFinder.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE
    #include <stdio.h>
    #include <sys/stat.h>
#endif
#include <nowide/stackstring.hpp>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static bool Enumerate(size_t depth, const Path& rootDirectory, const Path& path, FileSystemEntries& items, FileSystemEntryType findTypes, size_t maxDepth)
{
    FileFinder finder;
    if (finder.Start(path, findTypes).HasValue(true))
    {
        Path workingFilePath;
        do
        {
            if (finder.GetCurrentPath(workingFilePath).HasError())
                return false;

            auto fileSystemEntry = items.Add();
            if (fileSystemEntry == nullptr)
                return false;

            if (workingFilePath.substr(fileSystemEntry->relativePath, rootDirectory.length() + 1).HasError())
            {
                items.CancelAdd(fileSystemEntry);
                return false;
            }

            fileSystemEntry->relativePath.UniversalNormalize();

            fileSystemEntry->type = finder.GetCurrentType();

            if (fileSystemEntry->type == FileSystemEntryType::DIRECTORY && depth < maxDepth)
            {
                if (!Enumerate(depth + 1, rootDirectory, workingFilePath, items, findTypes, maxDepth))
                    return false;
            }
        } while (finder.Next().HasValue(true));
    }

    return true;
}


//Implementation----------------------------------------------------------------
DirectoryVirtualFileSystemRoot::DirectoryVirtualFileSystemRoot(Allocator* allocator) :
    VirtualFileSystemRoot(allocator),
    volumeID(allocator),
    directory(allocator),
    workingFilePath(allocator)
{
}

void DirectoryVirtualFileSystemRoot::OpenRoot(const Path& directory, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (directory.GetInternalVolumeID(this->volumeID).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to get internal volume ID.");
        return;
    }

    if (this->directory.assign(directory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign directory.");
        return;
    }
    this->directory.PlatformNormalize();

    if (this->description.assign("File system @ ").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign base description.");
        return;
    }
    if (this->description.append(directory.c_str()).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append directory to base description.");
        return;
    }
}

VirtualFileSystemRoot::EnumerationResult DirectoryVirtualFileSystemRoot::Enumerate(FileSystemEntries& items, FileSystemEntryType findTypes, size_t maxDepth, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!::Enumerate(0, this->directory, this->directory, items, findTypes, maxDepth))
        FINJIN_SET_ERROR(error, "Failed to enumerate items.");

    return EnumerationResult::COMPLETE;
}

const Utf8String& DirectoryVirtualFileSystemRoot::GetInternalVolumeID() const
{
    return this->volumeID;
}

const Path& DirectoryVirtualFileSystemRoot::GetFileSystemPath() const
{
    return this->directory;
}

FileOperationResult DirectoryVirtualFileSystemRoot::Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->workingFilePath.assign(this->directory).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign directory to working directory.");
        return FileOperationResult::FAILURE;
    }
    if ((this->workingFilePath /= relativeFilePath).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append relative file path to working directory.");
        return FileOperationResult::FAILURE;
    }

    if (!this->workingFilePath.ReadBinaryFile(buffer))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read file '%1%'", this->workingFilePath));
        return FileOperationResult::FAILURE;
    }

    return FileOperationResult::SUCCESS;
}

FileOperationResult DirectoryVirtualFileSystemRoot::Read(const Path& relativeFilePath, ByteBuffer& buffer)
{
    if (this->workingFilePath.assign(this->directory).HasError())
        return FileOperationResult::FAILURE;
    if ((this->workingFilePath /= relativeFilePath).HasError())
        return FileOperationResult::FAILURE;

    if (!this->workingFilePath.ReadBinaryFile(buffer))
        return FileOperationResult::NOT_FOUND;

    return FileOperationResult::SUCCESS;
}

bool DirectoryVirtualFileSystemRoot::CanOpen(const Path& relativeFilePath, FileOpenMode mode)
{
    if (this->workingFilePath.assign(this->directory).HasError())
        return false;
    if ((this->workingFilePath /= relativeFilePath).HasError())
        return false;

    if (mode == FileOpenMode::READ)
        return this->workingFilePath.IsFile();
    else
    {
        auto containsFile = this->directory.ContainsFile(this->workingFilePath);
        if (containsFile.HasError())
            return false;

        return containsFile.value && this->workingFilePath.IsFile();
    }
}

FileOperationResult DirectoryVirtualFileSystemRoot::Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle)
{
    assert(!fileHandle.IsOpen());

    fileHandle.fileSize = 0;

    if (this->workingFilePath.assign(this->directory).HasError())
        return FileOperationResult::FAILURE;
    if ((this->workingFilePath /= relativeFilePath).HasError())
        return FileOperationResult::FAILURE;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> fileNameW;
    if (!fileNameW.convert(this->workingFilePath.begin(), this->workingFilePath.end()))
        return FileOperationResult::FAILURE;

    DWORD desiredAccess = mode == FileOpenMode::READ ? GENERIC_READ : GENERIC_WRITE;
    DWORD shareMode = mode == FileOpenMode::READ ? FILE_SHARE_READ : FILE_SHARE_WRITE;
    DWORD creationDisposition = mode == FileOpenMode::READ ? OPEN_EXISTING : CREATE_ALWAYS;
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        auto win32Handle = CreateFile2(fileNameW.c_str(), desiredAccess, shareMode, creationDisposition, nullptr);
    #else
        auto win32Handle = CreateFileW(fileNameW.c_str(), desiredAccess, shareMode, nullptr, creationDisposition, 0, nullptr);
    #endif

    if (win32Handle != INVALID_HANDLE_VALUE)
        fileHandle.ptr = win32Handle;
    if (mode == FileOpenMode::READ)
    {
        LARGE_INTEGER largeInt;
        GetFileSizeEx(win32Handle, &largeInt);
        fileHandle.fileSize = (int64_t)largeInt.QuadPart;
    }
#elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE
    fileHandle.ptr = fopen(this->workingFilePath.c_str(), mode == FileOpenMode::READ ? "rb" : "wb");

    struct stat statBuf;
    if (mode == FileOpenMode::READ && fileHandle.ptr != nullptr && stat(this->workingFilePath.c_str(), &statBuf) != -1)
        fileHandle.fileSize = statBuf.st_size;
#else
    #error Unsupported platform!
#endif

    if (fileHandle.ptr == nullptr)
        return FileOperationResult::NOT_FOUND;

    fileHandle.fileSystemRoot = this;

    return FileOperationResult::SUCCESS;
}

uint64_t DirectoryVirtualFileSystemRoot::Skip(VirtualFileHandle& fileHandle, uint64_t byteCount)
{
    if (fileHandle.IsOpen())
    {
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        auto win32Handle = static_cast<HANDLE>(fileHandle.ptr);

        //Get current offset
        LARGE_INTEGER offset = { 0 };
        LARGE_INTEGER offsetBefore = { 0 };
        if (SetFilePointerEx(win32Handle, offset, &offsetBefore, FILE_CURRENT))
        {
            //Skip
            offset.QuadPart = byteCount;
            LARGE_INTEGER offsetAfter = { 0 };
            SetFilePointerEx(win32Handle, offset, &offsetAfter, FILE_CURRENT);

            return offsetAfter.QuadPart - offsetBefore.QuadPart;
        }
    #elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE
        auto file = static_cast<FILE*>(fileHandle.ptr);

        auto offsetBefore = ftello(file);
        if (offsetBefore != -1)
        {
            fseek(file, byteCount, SEEK_CUR);
            auto offsetAfter = ftello(file);

            return offsetAfter - offsetBefore;
        }
    #else
        #error Unsupported platform!
    #endif
    }

    return 0;
}

size_t DirectoryVirtualFileSystemRoot::Read(VirtualFileHandle& fileHandle, void* bytes, size_t byteCount)
{
    if (fileHandle.IsOpen())
    {
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        auto win32Handle = static_cast<HANDLE>(fileHandle.ptr);

        DWORD bytesRead = 0;
        ReadFile(win32Handle, bytes, static_cast<DWORD>(byteCount), &bytesRead, nullptr);
        return bytesRead;
    #elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE
        auto file = static_cast<FILE*>(fileHandle.ptr);

        return fread(bytes, 1, byteCount, file);
    #else
        #error Unsupported platform!
    #endif
    }

    return 0;
}

size_t DirectoryVirtualFileSystemRoot::Write(VirtualFileHandle& fileHandle, const void* bytes, size_t byteCount)
{
    if (fileHandle.IsOpen())
    {
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        auto win32Handle = static_cast<HANDLE>(fileHandle.ptr);

        DWORD bytesWritten = 0;
        WriteFile(win32Handle, bytes, static_cast<DWORD>(byteCount), &bytesWritten, nullptr);
        return static_cast<size_t>(bytesWritten);
    #elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE
        auto file = static_cast<FILE*>(fileHandle.ptr);

        return fwrite(bytes, 1, byteCount, file);
    #else
        #error Unsupported platform!
    #endif
    }

    return 0;
}

void DirectoryVirtualFileSystemRoot::Close(VirtualFileHandle& fileHandle)
{
    if (fileHandle.IsOpen())
    {
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        auto win32Handle = static_cast<HANDLE>(fileHandle.ptr);

        CloseHandle(win32Handle);
    #elif FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE
        auto file = static_cast<FILE*>(fileHandle.ptr);

        fclose(file);
    #else
        #error Unsupported platform!
    #endif

        fileHandle.ptr = nullptr;
    }
}
