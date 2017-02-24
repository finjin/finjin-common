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
#include "finjin/common/SingleEvent.hpp"
#include <condition_variable>
#include <mutex>

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
struct SingleEvent::Impl
{
public:
    Impl() 
    {
        flag.store(false);
    }

    std::mutex mutex;
    std::condition_variable condition;
    std::atomic_bool flag;
};


//Implementation---------------------------------------------------------------
SingleEvent::SingleEvent() : impl(new Impl)
{
}

SingleEvent::~SingleEvent()
{
    delete impl;
}

void SingleEvent::Notify()
{
    std::unique_lock<std::mutex> lock(impl->mutex);
    bool expected = false;
    if (impl->flag.compare_exchange_strong(expected, true))
        impl->condition.notify_one();
}

bool SingleEvent::TryWait()
{
    bool expected = true;
    return impl->flag.compare_exchange_strong(expected, false);
}

void SingleEvent::Wait()
{
    std::unique_lock<std::mutex> lock(impl->mutex);
    impl->condition.wait(lock, [this] { return impl->flag == true; });
    impl->flag.store(false);
}

void SingleEvent::WaitIf(std::function<bool()> pred)
{
    std::unique_lock<std::mutex> lock(impl->mutex);
    if (pred())
    {
        impl->condition.wait(lock, [this] { return impl->flag == true; });
        impl->flag.store(false);        
    }    
}

void* SingleEvent::GetNativeHandle()
{
    return impl->condition.native_handle();
}
