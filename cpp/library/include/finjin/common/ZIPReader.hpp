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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/FileOperationTypes.hpp"
#include "finjin/common/FileSystemEntry.hpp"
#include "finjin/common/Path.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API ZIPReader
    {
    public:
        ZIPReader(const ZIPReader& other) = delete;
        ZIPReader& operator = (const ZIPReader& other) = delete;

    public:
        ZIPReader();
        ZIPReader(ZIPReader&& other);
        ZIPReader& operator = (ZIPReader&& other);
        ~ZIPReader();

        void Open(const Path& path, Error& error);
        void Rewind(Error& error);
        void Close();

        struct Entry
        {
            int compressionMethod;
            FileSystemEntryType type;
            size_t compressedSize;
            size_t decompressedSize;
            Path path;

            Entry()
            {
                this->type = FileSystemEntryType::NONE;
                this->compressedSize = 0;
                this->decompressedSize = 0;
            }

            size_t GetDepth() const
            {
                size_t depth = 0;

                if (!this->path.empty())
                {
                    for (size_t i = 0; i < this->path.length() - 1; i++)
                    {
                        if (path[i] == '/')
                            depth++;
                    }
                }

                return depth;
            }

            bool IsDirectory() const { return this->type == FileSystemEntryType::DIRECTORY; }
            bool IsFile() const { return this->type == FileSystemEntryType::FILE; }
            FileSystemEntryType GetType() const { return this->type; }
        };

        bool Next(Entry& entry, FileSystemEntryType findTypes);

        FileOperationResult Inflate(ByteBuffer& buffer, const Entry& entry); //Decompresses entire file/entry
        FileOperationResult Inflate(ByteBuffer& buffer, const Path& path); //Decompresses entire file/entry
        FileOperationResult Inflate(void* buffer, size_t& bytesDecompressed, const Entry& entry); //Decompresses entire file/entry, treating 'buffer' as if it's large enough

        FileOperationResult TestStartInflate(const Path& path);
        FileOperationResult TestStartInflate(const Path& path, size_t& decompressedSize);
        FileOperationResult StartInflate(const Path& path, size_t& decompressedSize); //Starts a partial inflate.
        FileOperationResult PartialInflate(ByteBuffer& buffer, size_t byteCount); //Partially decompresses file/entry. Can be called multiple times
        FileOperationResult PartialInflate(void* buffer, size_t& bytesDecompressed, size_t byteCount); //Partially decompresses file/entry, treating 'buffer' as if it's large enough. Can be called multiple times
        FileOperationResult Skip(size_t& bytesSkipped, size_t byteCount); //Partially decompresses file/entry (discarding decompressed data)
        void StopInflate(); //Stops a partial decompression

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };

} }
