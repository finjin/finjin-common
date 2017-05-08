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
#include "finjin/common/SimpleSpinLockMutex.hpp"
#include <boost/intrusive_ptr.hpp>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FiberJob : public AllocatedClass
    {
    public:
        using ptr_t = boost::intrusive_ptr<FiberJob>;

        FiberJob(Allocator* allocator, size_t groupID, SimpleSpinLockMutex* inUseMutex) : AllocatedClass(allocator)
        {
            this->useCount = 0;
            this->groupID = groupID;
            this->inUseMutex = inUseMutex;
        }

        virtual ~FiberJob() {}

        virtual void Execute() = 0;

        friend void intrusive_ptr_add_ref(FiberJob* work)
        {
            ++work->useCount;
        }

        friend void intrusive_ptr_release(FiberJob* work)
        {
            if (--work->useCount == 0)
                delete work;
        }

        size_t GetGroupID() const
        {
            return this->groupID;
        }

        SimpleSpinLockMutex* GetInUseMutex() const
        {
            return this->inUseMutex;
        }

        FiberJob::ptr_t next;

    private:
        size_t useCount;
        size_t groupID;
        SimpleSpinLockMutex* inUseMutex;
    };

    template <typename Fn>
    class WrappedFiberJob : public FiberJob
    {
    public:
        WrappedFiberJob(Allocator* allocator, size_t groupID, SimpleSpinLockMutex* inUseMutex, Fn&& _fn) : FiberJob(allocator, groupID, inUseMutex), fn(std::forward<Fn>(_fn))
        {
        }

        ~WrappedFiberJob()
        {
        }

        void Execute() override
        {
            this->fn();
        }

    private:
        Fn fn;
    };

} }
