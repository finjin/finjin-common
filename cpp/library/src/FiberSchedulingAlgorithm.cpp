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
#include "FiberSchedulingAlgorithm.hpp"
#include "finjin/common/JobFiber.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
//RoundRobinFiberSchedulingAlgorithm::FiberQueue
RoundRobinFiberSchedulingAlgorithm::FiberQueue::FiberQueue()
{
    clear();
}

bool RoundRobinFiberSchedulingAlgorithm::FiberQueue::empty() const
{
    return head == nullptr;
}

void RoundRobinFiberSchedulingAlgorithm::FiberQueue::clear()
{
    this->count = 0;
    this->head = nullptr;
    this->pTail = &this->head;
}

size_t RoundRobinFiberSchedulingAlgorithm::FiberQueue::size() const
{
    return this->count;
}

void RoundRobinFiberSchedulingAlgorithm::FiberQueue::push(JobFiber* item)
{
    assert(item != nullptr);
    assert(item->next == nullptr);

    // * this->tail holds the null marking the end of the FiberQueue. So we can extend
    // the FiberQueue by assigning to * this->tail.
    *this->pTail = item;

    // Advance this->tail to point to the new end marker.
    this->pTail = &item->next;
    ++this->count;
}

JobFiber* RoundRobinFiberSchedulingAlgorithm::FiberQueue::pop()
{
    assert(!empty());

    JobFiber* item = this->head;
    this->head = this->head->next;
    if (this->head == nullptr)
        this->pTail = &this->head;

    item->next = nullptr;
    --this->count;

    return item;
}

void RoundRobinFiberSchedulingAlgorithm::FiberQueue::swap(FiberQueue& other)
{
    std::swap(this->head, other.head);
    std::swap(this->pTail, other.pTail);
}

//RoundRobinFiberSchedulingAlgorithm
void RoundRobinFiberSchedulingAlgorithm::OnFiberAwakened(JobFiber* f)
{
    assert(f != nullptr);

    this->fibers.push(f);
}

JobFiber* RoundRobinFiberSchedulingAlgorithm::PickNext()
{
    JobFiber* fiber = nullptr;
    if (!this->fibers.empty())
    {
        fiber = this->fibers.pop();
        assert(fiber != nullptr);
    }
    return fiber;
}

size_t RoundRobinFiberSchedulingAlgorithm::GetScheduledCount() const
{
    return this->fibers.size();
}

void RoundRobinFiberSchedulingAlgorithm::Clear()
{
    this->fibers.clear();
}
