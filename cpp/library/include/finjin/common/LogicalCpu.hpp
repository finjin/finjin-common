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
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/ThreadID.hpp"
#include <set>
#include <ostream>
#include <utility>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    using namespace Finjin::Common;

    class CpuSet : public StaticVector<uint64_t, CommonConstants::MAX_CPUS>
    {
    public:
        void insert(uint64_t value)
        {
            if (std::find(begin(), end(), value) == end())
                push_back(value);
        }
    };

    //Set of CPUs that share a cache
    class CacheCpuSet : public StaticVector<uint64_t, CommonConstants::MAX_CPU_CACHE_SHARING>
    {
    public:
        void insert(uint64_t value)
        {
            if (std::find(begin(), end(), value) == end())
                push_back(value);
        }
        
        void remove(uint64_t value)
        {
            for (size_t itemIndex = 0; itemIndex < this->count; itemIndex++)
            {
                if (this->items[itemIndex] == value)
                {
                    for (size_t i = itemIndex; i < this->count - 1; i++)
                        this->items[i] = this->items[i + 1];
                    this->count--;
                }
            }
        }
        
        template <typename T>
        CacheCpuSet& operator = (const std::set<T>& other)
        {
            this->count = 0;
            
            for (auto value : other)
            {
                if (this->count >= MAX_ITEMS)
                    break;
                
                this->items[this->count++] = value;
            }
            
            return *this;
        }

        CacheCpuSet& operator = (const CacheCpuSet& other)
        {
            this->count = 0;

            for (auto value : other)
            {
                if (this->count >= MAX_ITEMS)
                    break;

                this->items[this->count++] = value;
            }

            return *this;
        }
    };

    class FINJIN_COMMON_LIBRARY_API LogicalCpu
    {
    public:
        uint64_t nodeID;
        uint64_t processorID;

        enum {MAX_CACHES = 3};
        CacheCpuSet cacheProcessorSharing[MAX_CACHES]; //0 = level 1 cache, 1 = level 2 cache, etc
    
        LogicalCpu()
        {
            Reset();
        }
        
        void Reset()
        {
            this->nodeID = 0;
            this->processorID = 0;
            
            for (size_t i = 0; i < MAX_CACHES; i++)
                this->cacheProcessorSharing[i].clear();
        }
        
        uint64_t GetGroup() const;
        uint64_t GetID() const;
        
        bool AssociateCurrentThread() const;
        void AssociateCurrentThread(Error& error) const;
        
        bool AssociateThread(ThreadHandle threadHandle) const;
        void AssociateThread(ThreadHandle threadHandle, Error& error) const;

    public:
        static void Output(std::ostream& out, const LogicalCpu* items, size_t count)
        {
            const LogicalCpu* end = items + count;
            for (auto i = items; i != end; ++i)
            {
                const auto& cpu = *i;

                out << "NUMA node: " << cpu.nodeID << "\n";
                out << "CPU ID: " << cpu.processorID << "\n";

                for (int i = 0; i < LogicalCpu::MAX_CACHES; i++)
                {
                    out << "share L" << (i + 1) << " cache with: ";
                    for (auto processorID : cpu.cacheProcessorSharing[i])
                        out << processorID << " ";
                    out << std::endl;
                }

                out << std::endl << std::endl;
            }
        }
    };

    class FINJIN_COMMON_LIBRARY_API LogicalCpus : public StaticVector<LogicalCpu, CpuSet::MAX_ITEMS>
    {
    public:        
        void AssociateCurrentThreadAndRemove(LogicalCpu* removed = nullptr);

        void Truncate(size_t count)
        {
            if (count < this->count)
                this->count = count;
        }
        
        void clear()
        {
            Truncate(0);
        }
        
        void remove(LogicalCpu* iter)
        {
            RemoveAt(iter - begin());
        }
        
        void RemoveAt(size_t index)
        {
            for (size_t i = index; i < this->count - 1; i++)
                this->items[i] = this->items[i + 1];
            this->count--;
        }
        
        void Output(std::ostream& out)
        {
            LogicalCpu::Output(out, begin(), this->count);
        }
        
        void Enumerate();
    };

    class CpuMap : public StaticVector<std::pair<uint64_t, LogicalCpu>, CpuSet::MAX_ITEMS>
    {
    public:
        void insert(uint64_t processorID, const LogicalCpu& cpu)
        {
            for (auto& item : *this)
            {
                if (processorID == item.first)
                {
                    item.second = cpu;
                    return;
                }
            }

            assert(size() < MAX_ITEMS);
            this->items[this->count++].second = cpu;
        }

        std::pair<uint64_t, LogicalCpu>* find(uint64_t processorID)
        {
            for (auto& item : *this)
            {
                if (processorID == item.first)
                    return &item;
            }
            return end();
        }
    };

} }
