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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/FileOperationTypes.hpp"
#include "finjin/common/FileSystemEntry.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/VirtualFileSystemRoot.hpp"
#include <ostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API VirtualFileSystem
    {
    public:
        VirtualFileSystem();

        struct Settings : FileSystemEntries::Settings
        {
            Settings()
            {
                this->searchEntryTypes = FileSystemEntryType::DIRECTORY;
                this->maxSearchDepth = 1;
            }

            FileSystemEntryType searchEntryTypes;
            size_t maxSearchDepth; //Example: 'app' depth = 0, 'app/scenes' depth = 1
        };

        void Create(const Settings& settings, Error& error);

        void AddDirectory(const Path& path, Error& error);
        void AddZipArchive(const Path& path, Error& error);
        void AddRoot(std::unique_ptr<VirtualFileSystemRoot>&& root, Error& error);
        size_t GetRootCount() const;

        void RebuildDatabase(Error& error);
        void UpdateDatabase(Error& error);
        void OutputDatabase(std::ostream& out);

        bool CanRead(const Path& relativeFilePath);

        FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer);
        FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error);

        FileOperationResult Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle);
        void Close(VirtualFileHandle& fileHandle);

        bool IsDatabaseComplete() const;

        bool IsDatabasePopulated() const;

        template <typename Paths>
        void GetDirectoriesStartingWith(const Path& relativePath, Paths& paths)
        {
            for (auto entry = this->database.FindEntryStartingWith(FileSystemEntryType::DIRECTORY, relativePath);
                entry != nullptr;
                entry = this->database.FindNextEntryStartingWith(entry, FileSystemEntryType::DIRECTORY, relativePath))
            {
                if (!paths.push_back(entry->GetRelativePath()))
                    break;
            }
        }

        void ForDirectoriesStartingWith(const Path& relativePath, std::function<bool(const Path&)> func)
        {
            for (auto entry = this->database.FindEntryStartingWith(FileSystemEntryType::DIRECTORY, relativePath);
                entry != nullptr;
                entry = this->database.FindNextEntryStartingWith(entry, FileSystemEntryType::DIRECTORY, relativePath))
            {
                if (!func(entry->GetRelativePath()))
                    break;
            }
        }

    private:
        struct VolumeEntry
        {
            bool operator == (const Utf8String& volumeID) const { return this->volumeID == this->volumeID; }

            Utf8String volumeID;

            VirtualFileHandle fileHandle;
        };

        struct RootEntry
        {
            RootEntry()
            {
                this->status = Status::NEW;
                this->volume = nullptr;
            }

            enum class Status
            {
                NEW,
                ENUMERATED
            };
            Status status;

            VolumeEntry* volume;

            std::unique_ptr<VirtualFileSystemRoot> fileSystemRoot;
        };

        Settings settings;

        StaticVector<VolumeEntry, CommonConstants::MAX_ROOT_FILE_SYSTEM_ENTRIES> volumes;

        StaticVector<RootEntry, CommonConstants::MAX_ROOT_FILE_SYSTEM_ENTRIES> roots;

        FileSystemEntries database;
        FileSystemEntryType completeDatabaseTypes;
    };

} }
