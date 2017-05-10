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
#include "finjin/common/VirtualFileSystemOperationQueue.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/MemorySize.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
VirtualFileOperationRequest::VirtualFileOperationRequest(Allocator* allocator) : fileUri(allocator)
{
    this->fileSystem = nullptr;
    this->mode = FileOpenMode::READ;
    this->writeBuffer = nullptr;
    this->writeBufferByteCount = 0;
    this->estimatedFileSize = 0;
}

bool VirtualFileOperationRequest::Create(Allocator* allocator)
{
    auto result = true;
    result &= this->fileUri.SetAllocator(allocator);
    return result;
}

void VirtualFileOperationRequest::Destroy()
{
    this->fileUri.Destroy();
    Reset();
}

//VirtualFileSystemOperationQueue::Settings
VirtualFileSystemOperationQueue::Settings::Settings()
{
    this->allocator = nullptr;
    this->queueSize = 100;
    this->requestedReadByteCountPerSecond = MemorySize::MEBIBYTE * 50;
    this->requestedWriteByteCountPerSecond = MemorySize::MEBIBYTE * 50;
    this->readBufferSize = MemorySize::MEBIBYTE;
    this->createReadBuffer = true;
}

void VirtualFileSystemOperationQueue::Settings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (section.empty())
                {
                    if (key == "queue-size")
                    {
                        this->queueSize = Convert::ToInteger(value, this->queueSize);
                        if (this->queueSize < 2)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'queue-size'.", value));
                            return;
                        }
                    }
                    else if (key == "read-bytes-per-second")
                    {
                        MemorySize::Parse(this->requestedReadByteCountPerSecond, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'read-bytes-per-second'.", value));
                            return;
                        }
                    }
                    else if (key == "write-bytes-per-second")
                    {
                        MemorySize::Parse(this->requestedWriteByteCountPerSecond, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'write-bytes-per-second'.", value));
                            return;
                        }
                    }
                    else if (key == "read-buffer-bytes")
                    {
                        MemorySize::Parse(this->readBufferSize, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'read-buffer-bytes'.", value));
                            return;
                        }
                    }
                    else if (key == "create-read-buffer")
                    {
                        this->createReadBuffer = Convert::ToBool(value);
                    }
                }

                break;
            }
            default: break;
        }
    }
}

//VirtualFileSystemOperationQueue::InternalFileOperationRequest
VirtualFileSystemOperationQueue::InternalFileOperationRequest::InternalFileOperationRequest(Allocator* allocator) : VirtualFileOperationRequest(allocator)
{
    this->isFinished = true;
    this->totalBytesProcessed = 0;
}

void VirtualFileSystemOperationQueue::InternalFileOperationRequest::Start(const VirtualFileOperationRequest& request, const VirtualFileOperationHandle& operationHandle)
{
    static_cast<VirtualFileOperationRequest&>(*this) = request;
    this->isFinished = false;
    this->cancel = false;
    this->totalBytesProcessed = 0;
    this->operationHandle = operationHandle;
}

void VirtualFileSystemOperationQueue::InternalFileOperationRequest::Finish()
{
    this->isFinished = true;
    this->cancel = false;

    this->fileHandle.Close();
}

//VirtualFileSystemOperationQueue
VirtualFileSystemOperationQueue::VirtualFileSystemOperationQueue()
{
    this->sequenceID = 0;
}

void VirtualFileSystemOperationQueue::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.queueSize < 2)
    {
        FINJIN_SET_ERROR(error, "The operation queue size must be 2 or more.");
        return;
    }

    if (settings.requestedReadByteCountPerSecond == 0 && settings.requestedWriteByteCountPerSecond == 0)
    {
        FINJIN_SET_ERROR(error, "Read and/or write byte count per second must be greater than 0.");
        return;
    }

    if (settings.createReadBuffer && settings.readBufferSize == 0)
    {
        FINJIN_SET_ERROR(error, "Read buffer size must be greater than 0 when the creation of this buffer is enabled.");
        return;
    }

    this->settings = settings;

    if (!this->operations.Create(this->settings.queueSize, this->settings.allocator, this->settings.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create internal operations queue.");
        return;
    }

    if (!this->workingPath.Create(this->settings.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create internal path object.");
        return;
    }

    if (settings.createReadBuffer)
    {
        if (!this->readBuffer.Create(settings.readBufferSize, this->settings.allocator))
        {
            FINJIN_SET_ERROR(error, "Failed to create read buffer.");
            return;
        }
    }
}

void VirtualFileSystemOperationQueue::Destroy()
{
    CancelAndFinishPendingOperations();

    this->readBuffer.Destroy();

    this->workingPath.Destroy();

    this->operations.Destroy();
}

const VirtualFileSystemOperationQueue::Settings& VirtualFileSystemOperationQueue::GetSettings() const
{
    return this->settings;
}

void VirtualFileSystemOperationQueue::Update(const UpdateSettings& updateSettings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    uint64_t requestedReadByteCountForThisUpdate = 0;
    uint64_t requestedWriteByteCountForThisUpdate = 0;
    if (updateSettings.elapsedTime > 0)
    {
        requestedReadByteCountForThisUpdate = RoundToInt64(this->settings.requestedReadByteCountPerSecond * updateSettings.elapsedTime);
        requestedWriteByteCountForThisUpdate = RoundToInt64(this->settings.requestedWriteByteCountPerSecond * updateSettings.elapsedTime);
    }
    if (requestedReadByteCountForThisUpdate == 0)
        requestedReadByteCountForThisUpdate = this->settings.requestedReadByteCountPerSecond / 30;
    if (requestedWriteByteCountForThisUpdate == 0)
        requestedWriteByteCountForThisUpdate = this->settings.requestedWriteByteCountPerSecond / 30;

    auto minByteCountPerRead = std::max(static_cast<uint64_t>(updateSettings.minByteCountPerRead), requestedReadByteCountForThisUpdate);

    uint64_t totalBytesReadForThisUpdate = 0;
    uint64_t totalBytesWrittenForThisUpdate = 0;

    while (HasPendingOperations() &&
        totalBytesReadForThisUpdate < requestedReadByteCountForThisUpdate &&
        totalBytesWrittenForThisUpdate < requestedWriteByteCountForThisUpdate &&
        !error)
    {
        auto& currentOperation = this->operations.current();

        if (currentOperation.cancel)
        {
            if (currentOperation.cancelCallback != nullptr)
                currentOperation.cancelCallback(currentOperation.operationHandle);

            currentOperation.Finish();
        }
        else
        {
            if (currentOperation.totalBytesProcessed == 0)
            {
                if (currentOperation.fileUri.GetScheme().empty() || !currentOperation.fileUri.GetPath().StartsWith("/"))
                {
                    if (this->workingPath.assign(currentOperation.fileUri.GetPath()).HasError())
                        FINJIN_SET_ERROR(error, "Failed to assign working file path.");
                }
                else
                {
                    if (this->workingPath.assign(currentOperation.fileUri.GetPath().c_str() + 1).HasError())
                        FINJIN_SET_ERROR(error, "Failed to assign working file path.");
                }

                if (!error)
                {
                    auto fileOpenResult = currentOperation.fileSystem->Open(this->workingPath, currentOperation.mode, currentOperation.fileHandle);
                    if (fileOpenResult == FileOperationResult::SUCCESS)
                    {
                        if (currentOperation.mode == FileOpenMode::READ)
                            currentOperation.estimatedFileSize = currentOperation.fileHandle.fileSize;
                    }
                    else
                    {
                        if (this->settings.openErrorHandler != nullptr)
                            this->settings.openErrorHandler(*this, fileOpenResult, this->workingPath);

                        currentOperation.Finish();
                    }
                }
            }

            //std::cout << "Current operation: " << currentOperation.totalBytesProcessed << std::endl;

            size_t bytesProcessed = 0;
            if (currentOperation.fileHandle.IsOpen())
            {
                if (currentOperation.mode == FileOpenMode::READ)
                {
                    //Reading

                    //Get bytes left to read
                    auto requestedReadByteCountForThisRead = requestedReadByteCountForThisUpdate - totalBytesReadForThisUpdate;

                    //Ensure byte count is at least minByteCountPerRead
                    requestedReadByteCountForThisRead = std::max(requestedReadByteCountForThisRead, minByteCountPerRead);

                    if (currentOperation.readCallback != nullptr)
                    {
                        //Use read callback
                        bytesProcessed = currentOperation.readCallback
                            (
                            currentOperation.operationHandle,
                            currentOperation.fileHandle,
                            this->readBuffer.data(),
                            requestedReadByteCountForThisRead,
                            error
                            );
                        if (error)
                            FINJIN_SET_ERROR(error, "Error calling asset read callback.");
                    }
                    else
                    {
                        //Read into internal read buffer

                        //Ensure no larger than internal read buffer
                        requestedReadByteCountForThisRead = std::min(requestedReadByteCountForThisRead, static_cast<decltype(requestedReadByteCountForThisRead)>(this->readBuffer.size()));
                        if (requestedReadByteCountForThisRead > 0)
                        {
                            bytesProcessed = currentOperation.fileHandle.Read
                                (
                                this->readBuffer.data(),
                                requestedReadByteCountForThisRead
                                );
                        }
                        else
                            FINJIN_SET_ERROR(error, "Unable to read into internal buffer. Buffer was explicitly configured to not be created.");
                    }

                    auto isFinished = error || bytesProcessed < requestedReadByteCountForThisRead;

                    //Call post-read callback
                    if (!error && currentOperation.postReadCallback != nullptr)
                    {
                        currentOperation.postReadCallback
                            (
                            currentOperation.operationHandle,
                            this->readBuffer.data(),
                            bytesProcessed,
                            isFinished,
                            error
                            );
                        if (error)
                            FINJIN_SET_ERROR(error, "Error calling asset post read callback.");
                    }

                    //Update total
                    currentOperation.totalBytesProcessed += bytesProcessed;
                    totalBytesReadForThisUpdate += bytesProcessed;

                    //Determine whether the file has been fully read
                    if (error || isFinished)
                        currentOperation.Finish();
                }
                else
                {
                    //Writing
                    if (currentOperation.writeCallback != nullptr)
                    {
                        //Use write callback
                        bytesProcessed = currentOperation.writeCallback
                            (
                            currentOperation.operationHandle,
                            currentOperation.fileHandle,
                            requestedWriteByteCountForThisUpdate,
                            error
                            );
                        if (error)
                            FINJIN_SET_ERROR(error, "Error calling asset write callback.");

                        auto isFinished = error || bytesProcessed == 0;

                        //Call post-write callback
                        if (!error && currentOperation.postWriteCallback != nullptr)
                        {
                            currentOperation.postWriteCallback
                                (
                                currentOperation.operationHandle,
                                nullptr,
                                nullptr,
                                bytesProcessed,
                                isFinished,
                                error
                                );
                            if (error)
                                FINJIN_SET_ERROR(error, "Error calling asset post write callback.");
                        }

                        //Update total
                        currentOperation.totalBytesProcessed += bytesProcessed;
                        totalBytesWrittenForThisUpdate += bytesProcessed;

                        //Determine whether file has been totally written
                        if (error || isFinished)
                            currentOperation.Finish();
                    }
                    else
                    {
                        //Write from interal write buffer
                        auto currentWriteBuffer = &currentOperation.writeBuffer[currentOperation.totalBytesProcessed];

                        //Write no more than is available
                        auto bytesToWrite = std::min
                            (
                            currentOperation.writeBufferByteCount - static_cast<size_t>(currentOperation.totalBytesProcessed),
                            static_cast<size_t>(requestedWriteByteCountForThisUpdate)
                            );
                        bytesProcessed = currentOperation.fileHandle.Write(currentWriteBuffer, bytesToWrite);

                        auto isFinished = (currentOperation.totalBytesProcessed + bytesProcessed) == currentOperation.writeBufferByteCount;

                        //Call post-write callback
                        if (currentOperation.postWriteCallback != nullptr)
                        {
                            currentOperation.postWriteCallback
                                (
                                currentOperation.operationHandle,
                                currentOperation.writeBuffer,
                                currentWriteBuffer,
                                bytesProcessed,
                                isFinished,
                                error
                                );
                            if (error)
                                FINJIN_SET_ERROR(error, "Error calling asset post write callback.");
                        }

                        //Update total
                        currentOperation.totalBytesProcessed += bytesProcessed;
                        totalBytesWrittenForThisUpdate += bytesProcessed;

                        //Determine whether file has been totally written
                        if (error || isFinished)
                            currentOperation.Finish();
                    }
                }

                //It's possible the operation became cancelled during the update
                if (currentOperation.cancel)
                {
                    if (currentOperation.cancelCallback != nullptr)
                        currentOperation.cancelCallback(currentOperation.operationHandle);

                    currentOperation.Finish();
                }
            }
        }

        //Move to next operation
        if (currentOperation.isFinished)
            this->operations.pop();
    }
}

void VirtualFileSystemOperationQueue::AddRequest(VirtualFileOperationHandle& operationHandle, const VirtualFileOperationRequest& request, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    operationHandle.Clear();

    //Validate request
    if (request.fileSystem == nullptr)
    {
        FINJIN_SET_ERROR(error, "No file system was specified.");
        return;
    }

    if (request.mode == FileOpenMode::READ)
    {
        if (!request.fileUri.IsValid())
        {
            FINJIN_SET_ERROR(error, "No read file URI was specified.");
            return;
        }
        else if (request.readCallback == nullptr && request.postReadCallback == nullptr)
        {
            FINJIN_SET_ERROR(error, "No read callback or post-read callback was specified.");
            return;
        }
    }
    else if (request.mode == FileOpenMode::WRITE)
    {
        if (!request.fileUri.IsValid())
        {
            FINJIN_SET_ERROR(error, "No write file URI was specified.");
            return;
        }
        else if (request.writeBuffer != nullptr && request.writeCallback != nullptr)
        {
            FINJIN_SET_ERROR(error, "Both a write buffer and a write callback were specified. Only one should be specified.");
            return;
        }
        else if (request.writeBuffer == nullptr && request.writeCallback == nullptr)
        {
            FINJIN_SET_ERROR(error, "No write buffer or write callback was specified.");
            return;
        }
        else if (request.writeBuffer != nullptr && request.writeBufferByteCount == 0)
        {
            FINJIN_SET_ERROR(error, "The write buffer must have a non-zero length.");
            return;
        }
    }

    if (this->operations.full())
    {
        FINJIN_SET_ERROR(error, "The operations queue is full.");
        return;
    }

    //Set up queue entry
    this->operations.push();
    auto& internalReadRequest = this->operations.back();

    //Fill in result
    operationHandle.operationQueue = this;
    operationHandle.operationIndex = this->operations.GetIndex(internalReadRequest);
    operationHandle.sequenceID = this->sequenceID;

    internalReadRequest.Start(request, operationHandle);

    //Advance
    this->sequenceID++;
}

uint64_t VirtualFileSystemOperationQueue::GetTotalBytesProcessed(const VirtualFileOperationHandle& operationHandle) const
{
    if (operationHandle.operationQueue == this && operationHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->operations[operationHandle.operationIndex];
        if (operation.operationHandle.sequenceID == operationHandle.sequenceID)
            return operation.totalBytesProcessed;
    }

    return 0;
}

float VirtualFileSystemOperationQueue::GetProgress(const VirtualFileOperationHandle& operationHandle) const
{
    if (operationHandle.operationQueue == this && operationHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->operations[operationHandle.operationIndex];
        if (operation.operationHandle.sequenceID == operationHandle.sequenceID)
        {
            if (operation.estimatedFileSize > 0)
                return RoundToFloat(operation.totalBytesProcessed) / RoundToFloat(operation.estimatedFileSize);
            else
                return -1;
        }
    }

    return 0;
}

bool VirtualFileSystemOperationQueue::IsQueuedAndFinished(const VirtualFileOperationHandle& operationHandle) const
{
    if (operationHandle.operationQueue == this && operationHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->operations[operationHandle.operationIndex];
        if (operation.operationHandle.sequenceID == operationHandle.sequenceID)
            return operation.isFinished;
    }

    return false;
}

void VirtualFileSystemOperationQueue::Cancel(const VirtualFileOperationHandle& operationHandle)
{
    if (operationHandle.operationQueue == this && operationHandle.operationIndex != (size_t)-1)
    {
        auto& operation = this->operations[operationHandle.operationIndex];
        if (operation.operationHandle.sequenceID == operationHandle.sequenceID)
            operation.cancel = true;
    }
}

bool VirtualFileSystemOperationQueue::HasPendingOperations() const
{
    return !this->operations.empty();
}

void VirtualFileSystemOperationQueue::CancelAndFinishPendingOperations()
{
    for (auto& operation : this->operations)
        operation.cancel = true;

    VirtualFileSystemOperationQueue::UpdateSettings updateSettings;
    FINJIN_DECLARE_ERROR(error);
    Update(updateSettings, error);
}
