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
#include "finjin/common/ZIPReader.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"
#include <zzip/zzip.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
#if FINJIN_TARGET_PLATFORM_IS_ANDROID
    //Hack: The Android NDK doesn't define these but zziplib calls them.
    //The trick is that this library doesn't do anything that requires these functions to be called.
    //They just need to exist in order for the application to link.
    #include <dirent.h>
    extern "C" {
        long telldir(DIR* dirp)
        {
            return 0;
        }

        void seekdir(DIR* dirp, long loc)
        {
        }
    }
#endif


//Local types-------------------------------------------------------------------
struct WorkingZipFile
{
    ZZIP_FILE* zzipFile;
    Path zzipFilePath;
    zzip_ssize_t zzipFileBytesRead;
    zzip_ssize_t zzipFileBytesReadMax;

    WorkingZipFile()
    {
        this->zzipFile = nullptr;
        this->zzipFileBytesRead = 0;
        this->zzipFileBytesReadMax = 0;
    }

    ~WorkingZipFile()
    {
        Close();
    }

    bool IsOpen()
    {
        return this->zzipFile != nullptr;
    }

    void Close()
    {
        if (this->zzipFile != nullptr)
        {
            zzip_file_close(this->zzipFile);
            this->zzipFile = nullptr;

            this->zzipFilePath.clear();

            this->zzipFileBytesRead = 0;
            this->zzipFileBytesReadMax = 0;
        }
    }

    size_t GetBytesLeft() const
    {
        if (this->zzipFileBytesRead <= this->zzipFileBytesReadMax)
            return this->zzipFileBytesReadMax - this->zzipFileBytesRead;
        else
            return 0;
    }
};

struct ZIPReader::Impl
{
    Impl()
    {
        this->zzipDir = nullptr;

        this->currentFileIndex = 0;
        this->testFileIndex = 1;
    }

    ~Impl()
    {
        Close();
    }

    void Close()
    {
        for (auto& workingFile : this->workingFiles)
            workingFile.Close();

        if (this->zzipDir != nullptr)
        {
            zzip_dir_close(this->zzipDir);
            this->zzipDir = nullptr;
        }
    }

    FileOperationResult StartInflate(WorkingZipFile& workingFile, const Path& path)
    {
        workingFile.Close();

        workingFile.zzipFile = zzip_file_open(this->zzipDir, path.c_str(), 0);
        if (workingFile.zzipFile == nullptr)
        {
            auto zzipErrorCode = zzip_error(this->zzipDir);
            if (zzipErrorCode == ZZIP_ENOENT)
                return FileOperationResult::NOT_FOUND;
            return FileOperationResult::FAILURE;
        }

        workingFile.zzipFilePath = path;

        ZZIP_STAT zipStat;
        zzip_fstat(workingFile.zzipFile, &zipStat);
        workingFile.zzipFileBytesReadMax = zipStat.st_size; //Decompressed size

        return FileOperationResult::SUCCESS;
    }

    FileOperationResult PartialInflate(WorkingZipFile& workingFile, void* buffer, size_t& bytesDecompressed, size_t byteCount)
    {
        assert(!workingFile.zzipFilePath.empty());

        bytesDecompressed = zzip_file_read(workingFile.zzipFile, buffer, static_cast<zzip_ssize_t>(byteCount));
        workingFile.zzipFileBytesRead += static_cast<zzip_ssize_t>(bytesDecompressed);

        if (workingFile.zzipFileBytesRead >= workingFile.zzipFileBytesReadMax)
            workingFile.Close();

        return FileOperationResult::SUCCESS;
    }

    FileOperationResult Skip(WorkingZipFile& workingFile, size_t& bytesSkipped, size_t byteCount)
    {
        assert(!workingFile.zzipFilePath.empty());

        bytesSkipped = 0;

        //There's no way to skip without reading, so just read into a temp buffer
        size_t bytesDecompressed;
        uint8_t buffer[CommonConstants::FILE_TEMP_BUFFER_SIZE];
        while (true)
        {
            PartialInflate(workingFile, buffer, bytesDecompressed, sizeof(buffer));
            bytesSkipped += bytesDecompressed;
            if (bytesDecompressed < sizeof(buffer))
                break;
        }

        return FileOperationResult::SUCCESS;
    }

    ZZIP_DIR* zzipDir;

    std::array<WorkingZipFile, 2> workingFiles; //Used for iterative/partial decompression
    size_t currentFileIndex;
    size_t testFileIndex;
};


//Local functions---------------------------------------------------------------
static Utf8String ZipErrorToString(zzip_error_t e)
{
    return zzip_strerror((int)e);
}


//Implementation----------------------------------------------------------------
ZIPReader::ZIPReader() : impl(new Impl)
{
}

ZIPReader::ZIPReader(ZIPReader&& other) : impl(std::move(other.impl))
{
    other.impl.reset(new Impl);
}

ZIPReader& ZIPReader::operator = (ZIPReader&& other)
{
    impl = std::move(other.impl);

    other.impl.reset(new Impl);

    return *this;
}

ZIPReader::~ZIPReader()
{
    Close();
}

void ZIPReader::Open(const Path& path, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Close();

    zzip_error_t zipError;
    impl->zzipDir = zzip_dir_open(path.c_str(), &zipError);
    if (impl->zzipDir == nullptr)
    {
        auto zipErrorString = ZipErrorToString(zipError);
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open zip archive '%1%': %2%.", path.c_str(), zipErrorString.c_str()));
    }
}

void ZIPReader::Rewind(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->zzipDir == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to rewind zip archive since it was already closed.");
        return;
    }

    for (auto& workingFile : impl->workingFiles)
        workingFile.Close();

    zzip_rewinddir(impl->zzipDir);
}

void ZIPReader::Close()
{
    impl->Close();
}

bool ZIPReader::Next(Entry& entry)
{
    ZZIP_DIRENT dirent;

    if (!zzip_dir_read(impl->zzipDir, &dirent))
    {
        //Not an error. There's nothing left to read
        return false;
    }

    entry.compressionMethod = dirent.d_compr;
    entry.compressedSize = static_cast<size_t>(dirent.d_csize);
    entry.decompressedSize = static_cast<size_t>(dirent.st_size);
    entry.path = dirent.d_name;

    return true;
}

FileOperationResult ZIPReader::Inflate(ByteBuffer& buffer, const Entry& entry)
{
    if (buffer.resize(entry.decompressedSize) < entry.decompressedSize)
        return FileOperationResult::NOT_ENOUGH_MEMORY;

    size_t bytesDecompressed;
    return Inflate(buffer.data(), bytesDecompressed, entry);
}

FileOperationResult ZIPReader::Inflate(ByteBuffer& buffer, const Path& path)
{
    WorkingZipFile workingFile;
    auto openResult = impl->StartInflate(workingFile, path);
    if (openResult != FileOperationResult::SUCCESS)
        return openResult;

    if (buffer.resize(workingFile.zzipFileBytesReadMax) < workingFile.zzipFileBytesReadMax)
        return FileOperationResult::NOT_ENOUGH_MEMORY;

    size_t bytesRead = 0;
    return impl->PartialInflate(workingFile, buffer.data(), bytesRead, (size_t)-1);
}

FileOperationResult ZIPReader::Inflate(void* buffer, size_t& bytesDecompressed, const Entry& entry)
{
    WorkingZipFile workingFile;
    auto openResult = impl->StartInflate(workingFile, entry.path);
    if (openResult != FileOperationResult::SUCCESS)
        return openResult;

    return impl->PartialInflate(workingFile, buffer, bytesDecompressed, entry.decompressedSize);
}

FileOperationResult ZIPReader::TestStartInflate(const Path& path)
{
    size_t decompressedSize;
    return TestStartInflate(path, decompressedSize);
}

FileOperationResult ZIPReader::TestStartInflate(const Path& path, size_t& decompressedSize)
{
    decompressedSize = 0;

    if (impl->workingFiles[impl->testFileIndex].zzipFilePath == path)
    {
        if (impl->workingFiles[impl->testFileIndex].zzipFileBytesRead != 0)
        {
            zzip_rewind(impl->workingFiles[impl->testFileIndex].zzipFile);
            impl->workingFiles[impl->testFileIndex].zzipFileBytesRead = 0;

            decompressedSize = impl->workingFiles[impl->testFileIndex].zzipFileBytesReadMax;
        }

        return FileOperationResult::SUCCESS;
    }

    auto& workingFile = impl->workingFiles[impl->testFileIndex];
    auto result = impl->StartInflate(workingFile, path);
    if (result == FileOperationResult::SUCCESS)
        decompressedSize = workingFile.zzipFileBytesReadMax;
    return result;
}

FileOperationResult ZIPReader::StartInflate(const Path& path, size_t& decompressedSize)
{
    decompressedSize = 0;

    if (impl->workingFiles[impl->currentFileIndex].zzipFilePath == path)
    {
        if (impl->workingFiles[impl->currentFileIndex].zzipFileBytesRead != 0)
        {
            zzip_rewind(impl->workingFiles[impl->currentFileIndex].zzipFile);
            impl->workingFiles[impl->currentFileIndex].zzipFileBytesRead = 0;

            decompressedSize = impl->workingFiles[impl->currentFileIndex].zzipFileBytesReadMax;
        }

        return FileOperationResult::SUCCESS;
    }
    else if (impl->workingFiles[impl->testFileIndex].zzipFilePath == path)
    {
        if (impl->workingFiles[impl->testFileIndex].zzipFileBytesRead != 0)
        {
            zzip_rewind(impl->workingFiles[impl->testFileIndex].zzipFile);
            impl->workingFiles[impl->testFileIndex].zzipFileBytesRead = 0;

            decompressedSize = impl->workingFiles[impl->testFileIndex].zzipFileBytesReadMax;
        }

        std::swap(impl->currentFileIndex, impl->testFileIndex);

        return FileOperationResult::SUCCESS;
    }

    auto& workingFile = impl->workingFiles[impl->currentFileIndex];
    auto result = impl->StartInflate(workingFile, path);
    if (result == FileOperationResult::SUCCESS)
        decompressedSize = workingFile.zzipFileBytesReadMax;
    return result;
}

FileOperationResult ZIPReader::PartialInflate(ByteBuffer& buffer, size_t byteCount)
{
    auto& workingFile = impl->workingFiles[impl->currentFileIndex];
    byteCount = std::min(byteCount, workingFile.GetBytesLeft());

    if (buffer.resize(byteCount) < byteCount)
        return FileOperationResult::NOT_ENOUGH_MEMORY;

    size_t bytesRead;
    return impl->PartialInflate(workingFile, buffer.data(), bytesRead, byteCount);
}

FileOperationResult ZIPReader::PartialInflate(void* buffer, size_t& bytesDecompressed, size_t byteCount)
{
    auto& workingFile = impl->workingFiles[impl->currentFileIndex];
    byteCount = std::min(byteCount, workingFile.GetBytesLeft());
    return impl->PartialInflate(workingFile, buffer, bytesDecompressed, byteCount);
}

FileOperationResult ZIPReader::Skip(size_t& bytesSkipped, size_t byteCount)
{
    auto& workingFile = impl->workingFiles[impl->currentFileIndex];
    byteCount = std::min(byteCount, workingFile.GetBytesLeft());
    return impl->Skip(workingFile, bytesSkipped, byteCount);
}

void ZIPReader::StopInflate()
{
    impl->workingFiles[impl->currentFileIndex].Close();
}
