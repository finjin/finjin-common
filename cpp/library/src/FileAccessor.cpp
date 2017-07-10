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
#include "finjin/common/FileAccessor.hpp"
#include <nowide/stackstring.hpp>
#if !FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <sys/stat.h>
#endif

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
using WideningToUtf16Converter = nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1>;


//Implementation----------------------------------------------------------------
FileAccessor::FileAccessor()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    this->fileHandle = INVALID_HANDLE_VALUE;
#else
    this->file = nullptr;
#endif
}

FileAccessor::FileAccessor(FileAccessor&& other)
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    this->fileHandle = other.fileHandle;
    other.fileHandle = INVALID_HANDLE_VALUE;
#else
    this->file = other.file;
    other.file = nullptr;
#endif
}

FileAccessor& FileAccessor::operator = (FileAccessor&& other)
{
    Close();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    this->fileHandle = other.fileHandle;
    other.fileHandle = INVALID_HANDLE_VALUE;
#else
    this->file = other.file;
    other.file = nullptr;
#endif

    return *this;
}

FileAccessor::~FileAccessor()
{
    Close();
}

bool FileAccessor::OpenForRead(const Path& path, uint64_t* fileSize)
{
    Close();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    WideningToUtf16Converter fileNameW;
    if (!fileNameW.convert(path.begin(), path.end()))
        return false;

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        this->fileHandle = CreateFile2(fileNameW.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
    #else
        this->fileHandle = CreateFileW(fileNameW.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    #endif

    if (this->fileHandle != INVALID_HANDLE_VALUE && fileSize != nullptr)
    {
        LARGE_INTEGER largeFileSize;
        GetFileSizeEx(this->fileHandle, &largeFileSize);

        *fileSize = largeFileSize.QuadPart;
    }

    return this->fileHandle != INVALID_HANDLE_VALUE;
#else
    this->file = fopen(path.c_str(), "rb");
    if (this->file != nullptr && fileSize != nullptr)
    {
        fseeko(this->file, 0, SEEK_END);
        *fileSize = ftello(this->file);
        rewind(this->file);
    }

    return this->file != nullptr;
#endif

    return false;
}

bool FileAccessor::OpenForWrite(const Path& path)
{
    Close();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    WideningToUtf16Converter fileNameW;
    if (!fileNameW.convert(path.begin(), path.end()))
        return false;

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        this->fileHandle = CreateFile2(fileNameW.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, CREATE_ALWAYS, nullptr);
    #else
        this->fileHandle = CreateFileW(fileNameW.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, 0, nullptr);
    #endif
    return this->fileHandle != INVALID_HANDLE_VALUE;
#else
    this->file = fopen(path.c_str(), "wb");
    return this->file != nullptr;
#endif

    return false;
}

bool FileAccessor::IsOpen() const
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    return this->fileHandle != INVALID_HANDLE_VALUE;
#else
    return this->file != nullptr;
#endif
}

void FileAccessor::Close()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    if (this->fileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(this->fileHandle);
        this->fileHandle = INVALID_HANDLE_VALUE;
    }
#else
    if (this->file != nullptr)
    {
        fclose(this->file);
        this->file = nullptr;
    }
#endif
}

size_t FileAccessor::Read(void* bytes, size_t byteCount)
{
    if (!IsOpen())
        return 0;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    DWORD bytesRead = 0;
    ReadFile(this->fileHandle, bytes, static_cast<DWORD>(byteCount), &bytesRead, nullptr);
    return static_cast<size_t>(bytesRead);
#else
    auto before = ftello(this->file);
    fread(bytes, 1, byteCount, this->file);
    return ftello(this->file) - before;
#endif
}

size_t FileAccessor::Write(const void* bytes, size_t byteCount)
{
    if (!IsOpen())
        return 0;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    DWORD bytesWritten = 0;
    WriteFile(this->fileHandle, bytes, static_cast<DWORD>(byteCount), &bytesWritten, nullptr);
    return static_cast<size_t>(bytesWritten);
#else
    auto before = ftello(this->file);
    fwrite(bytes, 1, byteCount, this->file);
    return ftello(this->file) - before;
#endif
}

void FileAccessor::WriteRemaining(FileAccessor& inFile)
{
    if (!IsOpen())
        return;

    uint8_t tempBuffer[CommonConstants::FILE_TEMP_BUFFER_SIZE];
    size_t readCount = 0;
    while ((readCount = inFile.Read(tempBuffer, sizeof(tempBuffer))) > 0)
        Write(tempBuffer, readCount);
}

uint64_t FileAccessor::GetOffset()
{
    if (!IsOpen())
        return 0;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    LARGE_INTEGER zeroOffset = {0};
    LARGE_INTEGER result;
    SetFilePointerEx(this->fileHandle, zeroOffset, &result, FILE_CURRENT);
    return result.QuadPart;
#else
    return ftello(this->file);
#endif
}

void FileAccessor::SetOffset(uint64_t offsetFromStart)
{
    if (!IsOpen())
        return;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    LARGE_INTEGER offsetFromStartLI;
    offsetFromStartLI.QuadPart = offsetFromStart;
    SetFilePointerEx(this->fileHandle, offsetFromStartLI, nullptr, FILE_BEGIN);
#else
    fseeko(this->file, offsetFromStart, SEEK_SET);
#endif
}

bool FileAccessor::GetFileLength(const Path& path, uint64_t& fileLength)
{
    fileLength = 0;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    WideningToUtf16Converter pathW;
    if (!pathW.convert(path.begin(), path.end()))
        return false;

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        auto fileHandle = CreateFile2(pathW.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
    #else
        auto fileHandle = CreateFileW(pathW.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    #endif
    if (fileHandle == INVALID_HANDLE_VALUE)
        return false;

    LARGE_INTEGER fileSize;
    GetFileSizeEx(fileHandle, &fileSize);

    CloseHandle(fileHandle);

    fileLength = fileSize.QuadPart;
    return true;
#else
    struct stat statBuf;
    if (stat(path.c_str(), &statBuf) == -1 || !S_ISREG(statBuf.st_mode))
        return false;

    fileLength = statBuf.st_size;
    return true;
#endif
}
