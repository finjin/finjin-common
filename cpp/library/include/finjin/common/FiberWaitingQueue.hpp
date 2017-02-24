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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class JobFiber;
    
    class FiberWaitingQueue
    {
    public:
        enum { MAX_ITEMS = CommonConstants::MAX_FIBERS };

        FiberWaitingQueue()
        {
            this->count = 0;
            this->frontIndex = 0;
        }

        JobFiber* front()
        {
            assert(this->count > 0);
            if (this->count > 0)
                return this->items[this->frontIndex];
            else
                return nullptr;
        }

        void pop_front()
        {
            assert(this->count > 0);
            if (this->count > 0)
            {
                this->frontIndex = (this->frontIndex + 1) % MAX_ITEMS;
                this->count--;
            }
        }

        bool push_back(JobFiber* item)
        {
            assert(this->count < MAX_ITEMS);
            if (this->count < MAX_ITEMS)
            {
                this->items[(this->frontIndex + this->count) % MAX_ITEMS] = item;
                this->count++;
                return true;
            }
            else
                return false;
        }

        void swap(FiberWaitingQueue& other)
        {
            FiberWaitingQueue temp = *this;
            *this = other;
            other = temp;
        }

        bool empty() const
        {
            return this->count == 0;
        }

        bool Contains(JobFiber* item) const
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (this->items[(this->frontIndex + i) % MAX_ITEMS] == item)
                    return true;
            }

            return false;
        }

        void RemoveOnly(JobFiber* item)
        {
            if (this->count > 0)
            {
                if (item == this->items[this->frontIndex])
                    pop_front();
                else if (item == this->items[(this->frontIndex + this->count - 1) % MAX_ITEMS])
                    this->count--;
                else
                {
                    for (size_t i = 1; i < this->count - 1; i++)
                    {
                        auto itemIndex = (this->frontIndex + i) % MAX_ITEMS;
                        if (this->items[itemIndex] == item)
                        {
                            RemoveAtIndex(itemIndex);
                            return;
                        }
                    }
                }
            }
        }
        
    private:
        void RemoveAtIndex(size_t itemIndex)
        {
            for (size_t i = 0; i < this->count - 1; i++)
                this->items[(itemIndex + i) % MAX_ITEMS] = this->items[(itemIndex + i + 1) % MAX_ITEMS];
            this->count--;
        }

    private:
        JobFiber* items[MAX_ITEMS];
        size_t count;
        size_t frontIndex;
    };

} }
