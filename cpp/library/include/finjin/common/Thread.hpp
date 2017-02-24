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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/LogicalCpu.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API Thread
    {
    public:
        Thread(const Thread& other) = delete;
        Thread& operator = (const Thread& other) = delete;
        
    public:
        Thread();
        Thread(Thread&& other);
        Thread& operator = (Thread&& other);
        ~Thread();
        
        void Create(Allocator* allocator, const Utf8String& name, std::function<void()> threadProc);
        void Create(Allocator* allocator, const Utf8String& name, const LogicalCpu& logicalCpu, std::function<void()> threadProc);
        
        void Start(Error& error);
        void Stop();

        const Utf8String& GetName() const;

        const LogicalCpu* GetLogicalCpu() const;

        ThreadHandle GetNativeHandle();
        
        struct Impl;
        Impl* GetImpl();
        
    private:        
        Impl* impl;        
    };

} }
