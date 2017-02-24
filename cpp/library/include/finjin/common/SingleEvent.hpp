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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class SingleEvent
    {
    public:
        SingleEvent(const SingleEvent& other) = delete;
        SingleEvent& operator = (const SingleEvent& other) = delete;

        SingleEvent(SingleEvent&& other) = delete;
        SingleEvent& operator = (SingleEvent&& other) = delete;

    public:
        SingleEvent();
        ~SingleEvent();

        void Notify();
        bool TryWait();
        void Wait();
        void WaitIf(std::function<bool()> pred);

        void* GetNativeHandle();

    private:
        struct Impl;
        Impl* impl;
    };

} }
