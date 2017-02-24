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
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/AllocatedVector.hpp"
#include <ostream>


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class VirtualFileSystemRoot;

    class FINJIN_COMMON_LIBRARY_API FileSystemEntry
    {
    public:
        enum class Type
        {
            FILE,
            DIRECTORY
        };

        FileSystemEntry(Allocator* allocator);
        FileSystemEntry(VirtualFileSystemRoot* owner, Allocator* allocator);

        bool operator < (const FileSystemEntry& other) const;
        bool operator < (const Path& other) const;

        bool IsDirectory() const;
        bool IsFile() const;

        const Path& GetRelativePath() const;
        void GetAbsolutePath(Path& path, Error& error) const;

        VirtualFileSystemRoot* owner;
        Type type;
        size_t decompressedSize; //0 = directory, -1 = unknown
        Path relativePath;
    };

    class FileSystemEntries
    {
    public:
        struct Settings
        {
            Settings()
            {
                this->allocator = nullptr;
                this->maxEntries = 500;
            }

            Allocator* allocator;
            size_t maxEntries;
        };

        void Create(const Settings& settings, Error& error);

        void StartRebuild();
        void CancelRebuild();
        void FinishRebuild();

        void StartUpdate();
        void CancelUpdate();
        void FinishUpdate();

        void Add(const FileSystemEntry& entry);

        bool empty() const;
        bool full() const;

        void Output(std::ostream& out) const;

        FileSystemEntry* FindEntry(const Path& relativePath);
        FileSystemEntry* FindEntryStartingWith(const Path& relativePath);

        FileSystemEntry* FindNextEntry(FileSystemEntry* entry);
        FileSystemEntry* FindNextEntryStartingWith(FileSystemEntry* entry, const Path& relativePath);

        FileSystemEntry* FindEntryStartingWith(FileSystemEntry::Type type, const Path& relativePath);
        FileSystemEntry* FindNextEntryStartingWith(FileSystemEntry* entry, FileSystemEntry::Type type, const Path& relativePath);

    private:
        Settings settings;
        AllocatedVector<FileSystemEntry> items;
    };

} }
