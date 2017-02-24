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
#include "finjin/common/VirtualFileOperationHandle.hpp"
#include "finjin/common/VirtualFileSystemOperationQueue.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
VirtualFileOperationHandle::VirtualFileOperationHandle()
{
    Clear();
}

void VirtualFileOperationHandle::Clear()
{
    this->operationQueue = nullptr;
    this->operationIndex = (size_t)-1;
    this->sequenceID = 0;
}

uint64_t VirtualFileOperationHandle::GetTotalBytesProcessed() const
{
    if (this->operationQueue == nullptr)
        return 0;

    return this->operationQueue->GetTotalBytesProcessed(*this);
}

float VirtualFileOperationHandle::GetProgress() const
{
    if (this->operationQueue == nullptr)
        return 0;

    return this->operationQueue->GetProgress(*this);
}

bool VirtualFileOperationHandle::IsQueuedAndFinished() const
{
    if (this->operationQueue == nullptr)
        return false;

    return this->operationQueue->IsQueuedAndFinished(*this);
}

bool VirtualFileOperationHandle::FinishInProgress()
{
    if (this->operationQueue == nullptr)
        return false;
    
    auto result = this->operationQueue->IsQueuedAndFinished(*this);
    if (result)
        Clear();
    return result;
}

void VirtualFileOperationHandle::Cancel()
{
    if (this->operationQueue != nullptr)
        this->operationQueue->Cancel(*this);
}

VirtualFileSystemOperationQueue& VirtualFileOperationHandle::GetOperationQueue()
{
    assert(this->operationQueue != nullptr);
    return *this->operationQueue;
}
