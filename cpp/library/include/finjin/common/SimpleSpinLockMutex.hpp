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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class SimpleSpinLockMutex
    {
    public:
        SimpleSpinLockMutex(const SimpleSpinLockMutex& other) = delete;
        SimpleSpinLockMutex(SimpleSpinLockMutex&& other) = delete;

        SimpleSpinLockMutex& operator = (const SimpleSpinLockMutex& other) = delete;
        SimpleSpinLockMutex& operator = (SimpleSpinLockMutex&& other) = delete;

    public:
        SimpleSpinLockMutex()
        {
        }

        bool try_lock()
        {
            return this->flag.test_and_set(std::memory_order_acquire) == false;
        }

        void lock()
        {
            while (this->flag.test_and_set(std::memory_order_acquire) == true)
            {
            }
        }

        void unlock()
        {
            this->flag.clear(std::memory_order_release);
        }

    private:
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
    };

} }
