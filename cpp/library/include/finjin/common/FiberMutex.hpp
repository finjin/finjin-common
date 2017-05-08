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
#include "finjin/common/FiberSpinLock.hpp"
#include "finjin/common/FiberWaitingQueue.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class JobFiber;

    class FiberMutex
    {
    public:
        FiberMutex(const FiberMutex&) = delete;
        FiberMutex& operator = (const FiberMutex&) = delete;

    public:
        FiberMutex();
        ~FiberMutex();

        void lock();
        void unlock();

    private:
        enum class Status
        {
            LOCKED,
            UNLOCKED
        };

        FiberSpinLock spinLock;
        FiberWaitingQueue waiting;
        Status state;

        bool LockIfUnlocked();
    };

} }
