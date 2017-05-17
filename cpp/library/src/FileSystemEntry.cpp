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
#include "finjin/common/FileSystemEntry.hpp"
#include "finjin/common/VirtualFileSystemRoot.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//FileSystemEntry
FileSystemEntry::FileSystemEntry(Allocator* allocator) : relativePath(allocator)
{
    this->owner = nullptr;
    Reset();
}

FileSystemEntry::FileSystemEntry(VirtualFileSystemRoot* owner, Allocator* allocator) : relativePath(allocator)
{
    this->owner = owner;
    Reset();
}

void FileSystemEntry::Reset()
{
    this->type = FileSystemEntryType::FILE;
    this->decompressedSize = (size_t)-1;
    this->relativePath.clear();
}

bool FileSystemEntry::operator < (const FileSystemEntry& other) const
{
    return this->relativePath < other.relativePath;
}

bool FileSystemEntry::operator < (const Path& other) const
{
    return this->relativePath < other;
}

bool FileSystemEntry::IsDirectory() const
{
    return this->type == FileSystemEntryType::DIRECTORY;
}

bool FileSystemEntry::IsFile() const
{
    return this->type == FileSystemEntryType::FILE;
}

const Path& FileSystemEntry::GetRelativePath() const
{
    return this->relativePath;
}

void FileSystemEntry::GetAbsolutePath(Path& result, Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    assert(this->owner != nullptr);
    if (result.assign(this->owner->GetFileSystemPath()).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign file system path.");
        return;
    }
    if ((result /= this->relativePath).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to append relative path.");
        return;
    }
}

//FileSystemEntries
void FileSystemEntries::Create(const Settings& settings, Error& error)
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

    this->settings = settings;

    this->items.CreateEmpty(settings.maxEntries, settings.allocator, settings.allocator);
}

void FileSystemEntries::StartRebuild()
{
    this->items.clear();
}

void FileSystemEntries::CancelRebuild()
{
    this->items.clear();
}

void FileSystemEntries::FinishRebuild()
{
    if (!this->items.empty())
        std::sort(this->items.begin(), this->items.end());
}

void FileSystemEntries::StartUpdate()
{
    //Do nothing
}

void FileSystemEntries::CancelUpdate()
{
    //Do nothing
}

void FileSystemEntries::FinishUpdate()
{
    if (!this->items.empty())
        std::sort(this->items.begin(), this->items.end());
}

FileSystemEntry* FileSystemEntries::Add()
{
    if (this->items.full())
        return nullptr;
    
    this->items.push_back();
    auto& entry = this->items.back();
    entry.Reset();
    return &entry;
}

void FileSystemEntries::CancelAdd(FileSystemEntry* entry)
{
    if (!empty() && entry == &this->items.back())
        this->items.pop_back();
}

bool FileSystemEntries::empty() const
{
    return this->items.empty();
}

bool FileSystemEntries::full() const
{
    return this->items.full();
}

void FileSystemEntries::Output(std::ostream& out) const
{
    for (const auto& entry : this->items)
    {
        out << entry.GetRelativePath() << std::endl;
    }
}

FileSystemEntry* FileSystemEntries::FindEntry(const Path& relativePath)
{
    auto itemIter = std::lower_bound(this->items.begin(), this->items.end(), relativePath);
    if (itemIter != this->items.end() && itemIter->relativePath == relativePath)
    {
        auto index = itemIter - this->items.begin();
        return this->items.begin() + index;
    }
    else
        return nullptr;
}

FileSystemEntry* FileSystemEntries::FindEntryStartingWith(const Path& relativePath)
{
    auto itemIter = std::lower_bound(this->items.begin(), this->items.end(), relativePath);
    if (itemIter != this->items.end() && itemIter->relativePath.StartsWith(relativePath))
    {
        auto index = itemIter - this->items.begin();
        return this->items.begin() + index;
    }
    else
        return nullptr;
}

FileSystemEntry* FileSystemEntries::FindNextEntry(FileSystemEntry* entry)
{
    if (entry != nullptr)
    {
        auto entryIndex = entry - this->items.begin();

        auto nextEntryIter = this->items.begin() + entryIndex + 1;
        if (nextEntryIter != this->items.end() && nextEntryIter->relativePath == entry->relativePath)
        {
            auto index = nextEntryIter - this->items.begin();
            return this->items.begin() + index;
        }
    }

    return nullptr;
}

FileSystemEntry* FileSystemEntries::FindNextEntryStartingWith(FileSystemEntry* entry, const Path& relativePath)
{
    if (entry != nullptr)
    {
        auto entryIndex = entry - this->items.begin();

        auto nextEntryIter = this->items.begin() + entryIndex + 1;
        if (nextEntryIter != this->items.end() && nextEntryIter->relativePath.StartsWith(relativePath))
        {
            auto index = nextEntryIter - this->items.begin();
            return this->items.begin() + index;
        }
    }

    return nullptr;
}

FileSystemEntry* FileSystemEntries::FindEntryStartingWith(FileSystemEntryType type, const Path& relativePath)
{
    for (auto entry = FindEntryStartingWith(relativePath);
        entry != nullptr;
        entry = FindNextEntryStartingWith(entry, relativePath))
    {
        if (entry->type == type)
            return entry;
    }

    return nullptr;
}

FileSystemEntry* FileSystemEntries::FindNextEntryStartingWith(FileSystemEntry* entry, FileSystemEntryType type, const Path& relativePath)
{
    for (entry = FindNextEntryStartingWith(entry, relativePath);
        entry != nullptr;
        entry = FindNextEntryStartingWith(entry, relativePath))
    {
        if (entry->type == type)
            return entry;
    }

    return nullptr;
}
