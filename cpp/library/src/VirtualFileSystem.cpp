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
#include "finjin/common/VirtualFileSystem.hpp"
#include "finjin/common/DirectoryVirtualFileSystemRoot.hpp"
#include "finjin/common/ZipArchiveVirtualFileSystemRoot.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
VirtualFileSystem::VirtualFileSystem()
{
    this->isDatabaseComplete = false;
}

void VirtualFileSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    if (settings.maxEntries == 0)
    {
        FINJIN_SET_ERROR(error, "The maximum number of entries must be greater than 0.");
        return;
    }

    this->database.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Error initializing database.");
        return;
    }

    this->settings = settings;
}

void VirtualFileSystem::AddDirectory(const Path& path, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!path.IsDirectory())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Could not add root directory '%1%' to virtual file system. It either does not exist, is not a directory, or the application does not have the necessary permissions.", path));
        return;
    }

    if (this->roots.full())
    {
        FINJIN_SET_ERROR(error, "Failed to add root. Root collection is full.");
        return;
    }

    auto root = AllocatedClass::NewUnique<DirectoryVirtualFileSystemRoot>(this->settings.allocator, FINJIN_CALLER_ARGUMENTS);
    if (root == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate directory root for '%1%'.", path));
        return;
    }
    
    root->OpenRoot(path, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Could not open directory root at '%1%'.", path));
        return;
    }

    AddRoot(std::move(root), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add directory root.");
        return;
    }
}

void VirtualFileSystem::AddZipArchive(const Path& path, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->roots.full())
    {
        FINJIN_SET_ERROR(error, "Failed to add root. Root collection is full.");
        return;
    }

    auto root = AllocatedClass::NewUnique<ZipArchiveVirtualFileSystemRoot>(this->settings.allocator, FINJIN_CALLER_ARGUMENTS);
    if (root == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate zip root for '%1%'.", path));
        return;
    }

    root->OpenRoot(path, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Could not open zip archive root at '%1%'.", path));
        return;
    }

    AddRoot(std::move(root), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add zip archive root.");
        return;
    }
}

void VirtualFileSystem::AddRoot(std::unique_ptr<VirtualFileSystemRoot>&& root, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->roots.full())
    {
        FINJIN_SET_ERROR(error, "Failed to add root. Root collection is full.");
        return;
    }

    this->roots.push_back();
    auto& rootEntry = this->roots.back();
    
    auto volumeID = root->GetInternalVolumeID();
    auto rootVolume = this->volumes.find(volumeID);
    if (rootVolume == this->volumes.end())
    {
        if (this->volumes.full())
        {
            this->roots.pop_back();

            FINJIN_SET_ERROR(error, "Failed to add volume. Volume collection is full.");
            return;
        }

        this->volumes.push_back();
        rootVolume = &this->volumes.back();
        rootVolume->volumeID = volumeID;
    }

    rootEntry.status = RootEntry::Status::NEW;
    rootEntry.volume = rootVolume;
    rootEntry.fileSystemRoot = std::move(root);

    this->isDatabaseComplete = false;
}

size_t VirtualFileSystem::GetRootCount() const
{
    return this->roots.size();
}

void VirtualFileSystem::RebuildDatabase(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->database.StartRebuild();
    
    if (!this->roots.empty())
    {
        this->isDatabaseComplete = true; //Assume the database is complete
        for (auto& root : this->roots)
        {
            auto result = root.fileSystemRoot->Enumerate(this->database, error);
            if (error)
            {
                this->database.CancelRebuild();
                this->isDatabaseComplete = false; //Database is not complete

                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to enumerate items in '%1%'.", root.fileSystemRoot->GetDescription()));
                return;
            }

            root.status = RootEntry::Status::ENUMERATED;

            if (result != VirtualFileSystemRoot::EnumerationResult::COMPLETE)
                this->isDatabaseComplete = false; //Database is not complete
        }
    }    
    else
        this->isDatabaseComplete = false;

    this->database.FinishRebuild();    
}

void VirtualFileSystem::UpdateDatabase(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->database.StartUpdate();

    if (!this->roots.empty())
    {
        this->isDatabaseComplete = true; //Assume the database is complete
        for (auto& root : this->roots)
        {
            if (root.status == RootEntry::Status::NEW)
            {
                auto result = root.fileSystemRoot->Enumerate(this->database, error);
                if (error)
                {
                    this->database.CancelUpdate();
                    this->isDatabaseComplete = false; //Database is not complete

                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to enumerate items in '%1%'.", root.fileSystemRoot->GetDescription()));
                    return;
                }

                root.status = RootEntry::Status::ENUMERATED;

                if (result != VirtualFileSystemRoot::EnumerationResult::COMPLETE)
                    this->isDatabaseComplete = false; //Database is not complete
            }
        }
    }
    else
        this->isDatabaseComplete = false;

    this->database.FinishUpdate();
}

void VirtualFileSystem::OutputDatabase(std::ostream& out)
{
    this->database.Output(out);
}

bool VirtualFileSystem::CanRead(const Path& relativeFilePath)
{
    if (this->roots.empty())
        return false;

    if (!this->database.empty())
    {
        //Database has been built
        auto entry = this->database.FindEntry(relativeFilePath);
        if (entry != nullptr)
        {
            do
            {
                if (entry->owner->CanOpen(relativeFilePath, FileOpenMode::READ))
                    return true;

                entry = this->database.FindNextEntry(entry);
            } while (entry != nullptr);

            return false;
        }

        if (this->isDatabaseComplete)
        {
            //The database is considered complete and the file could not be found
            return false;
        }
    }

    //Database has not been built or is incomplete. Just test it and see what happens
    for (auto& root : this->roots)
    {
        if (root.fileSystemRoot->CanOpen(relativeFilePath, FileOpenMode::READ))
            return true;
    }

    return false;
}

FileOperationResult VirtualFileSystem::Read(const Path& relativeFilePath, ByteBuffer& buffer)
{
    if (this->roots.empty())
        return FileOperationResult::NOT_INITIALIZED;

    if (!this->database.empty())
    {
        //Database has been built
        auto entry = this->database.FindEntry(relativeFilePath);
        if (entry != nullptr)
        {
            do
            {
                auto readResult = entry->owner->Read(relativeFilePath, buffer);
                if (readResult == FileOperationResult::SUCCESS)
                    return FileOperationResult::SUCCESS;
                else if (readResult != FileOperationResult::NOT_FOUND)
                    return readResult;

                entry = this->database.FindNextEntry(entry);
            } while (entry != nullptr);

            return FileOperationResult::FAILURE;
        }

        if (this->isDatabaseComplete)
        {
            //The database is considered complete and the file could not be found
            return FileOperationResult::NOT_FOUND;
        }
    }

    //Database has not been built or is incomplete. Just try to read it and see what happens
    for (auto& root : this->roots)
    {
        auto readResult = root.fileSystemRoot->Read(relativeFilePath, buffer);
        if (readResult == FileOperationResult::SUCCESS)
            return FileOperationResult::SUCCESS;
        else if (readResult != FileOperationResult::NOT_FOUND)
            return readResult;
    }

    return FileOperationResult::NOT_FOUND;
}

FileOperationResult VirtualFileSystem::Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->roots.empty())
        return FileOperationResult::NOT_INITIALIZED;

    if (!this->database.empty())
    {
        //Database has been built
        auto entry = this->database.FindEntry(relativeFilePath);
        if (entry != nullptr)
        {
            do
            {
                auto readResult = entry->owner->Read(relativeFilePath, buffer);
                if (readResult == FileOperationResult::SUCCESS)
                    return FileOperationResult::SUCCESS;
                else if (readResult != FileOperationResult::NOT_FOUND)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("An error was encountered while reading '%1%' from file system database.", relativeFilePath));
                    return readResult;
                }
                
                entry = this->database.FindNextEntry(entry);
            } while (entry != nullptr);

            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read '%1%' from file system even though it was found in database.", relativeFilePath));
            return FileOperationResult::FAILURE;
        }

        if (this->isDatabaseComplete)
        {
            //The database is considered complete and the file could not be found
            return FileOperationResult::NOT_FOUND;
        }
    }
    
    //Database has not been built or is incomplete. Just try to read it and see what happens
    for (auto& root : this->roots)
    {
        auto readResult = root.fileSystemRoot->Read(relativeFilePath, buffer);
        if (readResult == FileOperationResult::SUCCESS)
            return FileOperationResult::SUCCESS;
        else if (readResult != FileOperationResult::NOT_FOUND)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("An error was encountered while reading '%1%' from file system.", relativeFilePath));
            return readResult;
        }
    }
    
    return FileOperationResult::NOT_FOUND;
}

FileOperationResult VirtualFileSystem::Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle)
{
    if (this->roots.empty())
        return FileOperationResult::NOT_INITIALIZED;

    for (auto& root : this->roots)
    {
        if (root.volume->fileHandle.IsOpen() && root.fileSystemRoot->CanOpen(relativeFilePath, mode))
            return FileOperationResult::TOO_MANY_OPEN_FILES;
        else
        {
            auto openResult = root.fileSystemRoot->Open(relativeFilePath, mode, root.volume->fileHandle);
            if (openResult == FileOperationResult::SUCCESS)
            {
                root.volume->fileHandle.fileSystem = this;

                fileHandle = root.volume->fileHandle;
                return FileOperationResult::SUCCESS;
            }
            else if (openResult != FileOperationResult::NOT_FOUND)
                return openResult;
        }
    }
    
    return FileOperationResult::NOT_FOUND;
}

void VirtualFileSystem::Close(VirtualFileHandle& fileHandle)
{
    if (!fileHandle.IsOpen())
        return;

    for (auto& volume : this->volumes)
    {
        if (volume.fileHandle == fileHandle)
        {
            //Found the volume associated with the file

            //Close the file using the root
            volume.fileHandle.fileSystemRoot->Close(volume.fileHandle);
            
            //Reset volume and caller file handles (they are the same)
            volume.fileHandle.Clear();
            fileHandle.Clear();

            break;
        }
    }
}

bool VirtualFileSystem::IsDatabaseComplete() const
{
    return this->isDatabaseComplete;
}

bool VirtualFileSystem::IsDatabasePopulated() const
{
    return !this->database.empty();
}
