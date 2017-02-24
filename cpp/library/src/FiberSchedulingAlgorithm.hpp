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
#include "FinjinPrecompiled.hpp"
#include "finjin/common/FiberSpinLock.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class JobFiber;

    //FiberSchedulingAlgorithm---------------------------------------------
    class FiberSchedulingAlgorithm
    {
    public:
        virtual ~FiberSchedulingAlgorithm() {}

        virtual void OnFiberAwakened(JobFiber* f) = 0;

        virtual JobFiber* PickNext() = 0;

        virtual size_t GetScheduledCount() const = 0;
        
        virtual void Clear() = 0;
    };


    //RoundRobinFiberSchedulingAlgorithm---------------------------------------------
    class RoundRobinFiberSchedulingAlgorithm : public FiberSchedulingAlgorithm
    {
    public:
        void OnFiberAwakened(JobFiber* f) override;

        JobFiber* PickNext() override;

        size_t GetScheduledCount() const override;
        
        void Clear() override;

    private:
        class FiberQueue
        {
        public:
            FiberQueue(const FiberQueue&) = delete;
            FiberQueue& operator = (const FiberQueue&) = delete;

        public:
            FiberQueue();

            bool empty() const;

            void clear();
            
            size_t size() const;
            
            void push(JobFiber* item);

            JobFiber* pop();

            void swap(FiberQueue& other);

        private:
            size_t count;

            JobFiber* head;

            JobFiber** pTail; //Never null. On empty, points to head
        };
        FiberQueue fibers;
    };

} }
