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


//Includes---------------------------------------------------------------------
#include "finjin/common/FiberJob.hpp"
#include "finjin/common/FiberCondition.hpp"
#include "finjin/common/FiberMutex.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FiberJobQueue
    {
    public:
        FiberJobQueue();

        enum class PushPopResult
        {
            SUCCESS,
            INTERRUPTED,
            GROUP_CHECK_FAILED
        };

        PushPopResult Push(FiberJob::ptr_t&& newJob);
        PushPopResult Pop(FiberJob::ptr_t& oldHead, size_t maxGroupID);
        
        bool empty() const;
        
        void clear();

    private:
        bool _IsEmpty() const;
        bool _IsEmptyOrAboveMaxGroupID(size_t maxGroupID) const;

    private:
        bool isInterrupted;
        FiberJob::ptr_t head;
        FiberJob::ptr_t* pTail;
        mutable FiberMutex mutex;
        FiberCondition notEmptyCondition;
    };

} }
