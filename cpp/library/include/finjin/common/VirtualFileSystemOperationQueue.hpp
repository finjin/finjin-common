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
#include "finjin/common/DynamicQueue.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Uri.hpp"
#include "finjin/common/VirtualFileOperationHandle.hpp"
#include "finjin/common/VirtualFileSystem.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class VirtualFileSystemOperationQueue;

    /**
     * Passed as an input to VirtualFileSystemOperationQueue::AddRequest(). Use one of the factory methods to create a request.
     */
    class FINJIN_COMMON_LIBRARY_API VirtualFileOperationRequest
    {
    public:
        VirtualFileOperationRequest(Allocator* allocator = nullptr);

        bool Create(Allocator* allocator);
        void Destroy();

        using ReadCallback = std::function<size_t(VirtualFileOperationHandle& operationHandle, VirtualFileHandle& fileHandle, uint8_t* readBuffer, size_t maxReadByteCount, Error& error)>;
        using PostReadCallback = std::function<void(VirtualFileOperationHandle& operationHandle, const uint8_t* bytes, size_t byteCount, bool isFinished, Error& error)>;
        using WriteCallback = std::function<size_t(VirtualFileOperationHandle& operationHandle, VirtualFileHandle& fileHandle, size_t maxWriteByteCount, Error& error)>;
        using PostWriteCallback = std::function<void(VirtualFileOperationHandle& operationHandle, const uint8_t* writeBuffer, const uint8_t* bytes, size_t byteCount, bool isFinished, Error& error)>;
        using CancelCallback = std::function<void(VirtualFileOperationHandle& operationHandle)>;

        //Read requests----------------------------------

        //In this mode, the queue iteratively reads into an internal buffer and passes a pointer to the data that was read.
        VirtualFileOperationRequest& ReadRequest(PostReadCallback postReadCallback)
        {
            Reset();
            this->mode = FileOpenMode::READ;
            this->postReadCallback = postReadCallback;
            return *this;
        }

        //In this mode, 'readCallback' performs custom reading from the file handle.
        VirtualFileOperationRequest& ReadRequest(ReadCallback readCallback, PostReadCallback postReadCallback = nullptr)
        {
            Reset();
            this->mode = FileOpenMode::READ;
            this->readCallback = readCallback;
            this->postReadCallback = postReadCallback;
            return *this;
        }

        VirtualFileOperationRequest& SetReadUri(const SimpleUri& relativeFileUri, VirtualFileSystem& fileSystem)
        {
            this->fileUri = relativeFileUri;
            this->fileSystem = &fileSystem;
            return *this;
        }

        //Write requests----------------------------------

        //In this mode, the queue iteratively writes from 'writeBuffer'.
        VirtualFileOperationRequest& WriteRequest(const void* writeBuffer, size_t writeBufferByteCount, PostWriteCallback postWriteCallback = nullptr)
        {
            Reset();
            this->mode = FileOpenMode::WRITE;
            this->writeBuffer = static_cast<const uint8_t*>(writeBuffer);
            this->writeBufferByteCount = writeBufferByteCount;
            this->postWriteCallback = postWriteCallback;
            return *this;
        }

        //In this mode, the caller's callback performs custom writing to the file handle.
        VirtualFileOperationRequest& WriteRequest(WriteCallback writeCallback, PostWriteCallback postWriteCallback = nullptr)
        {
            Reset();
            this->mode = FileOpenMode::WRITE;
            this->writeCallback = writeCallback;
            this->postWriteCallback = postWriteCallback;
            return *this;
        }

        VirtualFileOperationRequest& SetWritePath(const Path& relativeFilePath, VirtualFileSystem& fileSystem)
        {
            SimpleUri relativeFileUri;
            relativeFileUri.SetPath(relativeFilePath);
            return SetWriteUri(relativeFileUri, fileSystem);
        }

        VirtualFileOperationRequest& SetWriteUri(const SimpleUri& relativeFileUri, VirtualFileSystem& fileSystem)
        {
            this->fileUri = relativeFileUri;
            this->fileSystem = &fileSystem;
            return *this;
        }

    private:
        void Reset()
        {
            this->fileSystem = nullptr;
            this->mode = FileOpenMode::READ;
            this->writeBuffer = nullptr;
            this->writeBufferByteCount = 0;
            this->estimatedFileSize = 0;
            this->fileUri.clear();

            this->readCallback = nullptr;
            this->postReadCallback = nullptr;
            this->writeCallback = nullptr;
            this->postWriteCallback = nullptr;
            this->cancelCallback = nullptr;
        }

    public:
        VirtualFileSystem* fileSystem;
        FileOpenMode mode;
        const uint8_t* writeBuffer;
        size_t writeBufferByteCount;
        uint64_t estimatedFileSize;
        SimpleUri fileUri;

        ReadCallback readCallback; //Optionally called to perform a read, in place of the default read behavior
        PostReadCallback postReadCallback; //Called after a read takes place
        WriteCallback writeCallback; //Called to perform a write
        PostWriteCallback postWriteCallback; //Called after a write takes place
        CancelCallback cancelCallback; //Called when an read or write is cancelled
    };

    class FINJIN_COMMON_LIBRARY_API VirtualFileSystemOperationQueue
    {
    public:
        struct Settings
        {
            Settings();

            void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

            Allocator* allocator;
            size_t queueSize;
            size_t requestedReadByteCountPerSecond, requestedWriteByteCountPerSecond;
            size_t readBufferSize;
            bool createReadBuffer;
            std::function<void(VirtualFileSystemOperationQueue& operationQueue, FileOperationResult result, const Path& path)> openErrorHandler;
            std::function<void(VirtualFileSystemOperationQueue& operationQueue, const Path& path, const Error& error)> callbackOperationErrorHandler; //Error handler that can be used by a callback
        };

        VirtualFileSystemOperationQueue();

        void Create(const Settings& settings, Error& error);
        void Destroy();

        const Settings& GetSettings() const;

        struct UpdateSettings
        {
            UpdateSettings() : elapsedTime(0), minByteCountPerRead(0)
            {
            }

            SimpleTimeDelta elapsedTime;
            size_t minByteCountPerRead;
        };
        void Update(const UpdateSettings& updateSettings, Error& error);

        void AddRequest(VirtualFileOperationHandle& operationHandle, const VirtualFileOperationRequest& request, Error& error);

        uint64_t GetTotalBytesProcessed(const VirtualFileOperationHandle& operationHandle) const;
        float GetProgress(const VirtualFileOperationHandle& operationHandle) const;
        bool IsQueuedAndFinished(const VirtualFileOperationHandle& operationHandle) const;

        void Cancel(const VirtualFileOperationHandle& operationHandle);

        bool HasPendingOperations() const;

        void CancelAndFinishPendingOperations();

    private:
        Settings settings;

        struct InternalFileOperationRequest : VirtualFileOperationRequest
        {
            InternalFileOperationRequest(Allocator* allocator);

            void Start(const VirtualFileOperationRequest& request, const VirtualFileOperationHandle& operationHandle);

            void Finish();

            bool isFinished;
            bool cancel;
            uint64_t totalBytesProcessed;
            VirtualFileOperationHandle operationHandle;
            VirtualFileHandle fileHandle;
        };

        DynamicQueue<InternalFileOperationRequest> operations;

        size_t sequenceID;

        Path workingPath;

        ByteBuffer readBuffer;
    };

} }
