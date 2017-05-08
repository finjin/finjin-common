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
#include "FiberJobQueue.hpp"
#include "finjin/common/FiberException.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
FiberJobQueue::FiberJobQueue()
{
    clear();
}

FiberJobQueue::PushPopResult FiberJobQueue::Push(FiberJob::ptr_t&& newJob)
{
    if (this->isInterrupted)
        return PushPopResult::INTERRUPTED;

    try
    {
        std::unique_lock<FiberMutex> thisLock(this->mutex);

        if (this->isInterrupted)
            return PushPopResult::INTERRUPTED;

    #if FINJIN_DEBUG
        if (!_IsEmpty())
        {
            //The added job's group ID should be greater or equal to the group ID of the last queued job
            auto tail = this->pTail->get();
            assert(newJob->GetGroupID() >= tail->GetGroupID());
        }
    #endif

        *this->pTail = newJob;
        this->pTail = &newJob->next;

        this->notEmptyCondition.NotifyOne();

        return PushPopResult::SUCCESS;
    }
    catch (FiberInterruptedException&)
    {
        this->isInterrupted = true;
        return PushPopResult::INTERRUPTED;
    }
}

FiberJobQueue::PushPopResult FiberJobQueue::Pop(FiberJob::ptr_t& oldHead, size_t maxGroupID)
{
    if (this->isInterrupted)
        return PushPopResult::INTERRUPTED;

    try
    {
        std::unique_lock<FiberMutex> thisLock(this->mutex);

        while (!this->isInterrupted && _IsEmpty())
            this->notEmptyCondition.wait(thisLock);

        if (this->head->GetGroupID() > maxGroupID)
            return FiberJobQueue::PushPopResult::GROUP_CHECK_FAILED;

        oldHead = this->head;

        this->head = oldHead->next;
        if (this->head == nullptr)
            this->pTail = &this->head;
        oldHead->next.reset();

        return PushPopResult::SUCCESS;
    }
    catch (FiberInterruptedException&)
    {
        this->isInterrupted = true;
        return PushPopResult::INTERRUPTED;
    }
}

void FiberJobQueue::clear()
{
    this->isInterrupted = false;

    this->head.reset();
    this->pTail = &this->head;
}

bool FiberJobQueue::empty() const
{
    return _IsEmpty();
}

bool FiberJobQueue::_IsEmpty() const
{
    return this->head == nullptr;
}

bool FiberJobQueue::_IsEmptyOrAboveMaxGroupID(size_t maxGroupID) const
{
    return this->head == nullptr || this->head->GetGroupID() > maxGroupID;
}
