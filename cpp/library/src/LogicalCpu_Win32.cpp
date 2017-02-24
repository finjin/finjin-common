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
#include "finjin/common/LogicalCpu.hpp"
#include <Windows.h>

using namespace Finjin::Common;


//Local classes----------------------------------------------------------------
class ProcessorInfoEnumerator
{
public:
    ProcessorInfoEnumerator(LOGICAL_PROCESSOR_RELATIONSHIP relationship) : base(nullptr), current(nullptr), remaining(0)
    {
        DWORD size = 0;
        if (!GetLogicalProcessorInformationEx(relationship, nullptr, &size))
        {
            this->base = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(LocalAlloc(LMEM_FIXED, size));
            if (this->base != nullptr && GetLogicalProcessorInformationEx(relationship, this->base, &size))
            {
                this->current = this->base;
                this->remaining = size;
            }
        }
    }

    ~ProcessorInfoEnumerator()
    {
        LocalFree(this->base);
    }

    void Next()
    {
        if (this->current != nullptr)
        {
            this->remaining -= this->current->Size;
            if (this->remaining != 0)
                this->current = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(reinterpret_cast<uint8_t*>(this->current) + this->current->Size);
            else
                this->current = nullptr;
        }
    }

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* GetCurrent()
    {
        return this->current;
    }

private:
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* base;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* current;
    DWORD remaining;
};


//Local functions--------------------------------------------------------------
template <typename CPU_SET>
void ComputeCpuSet(CPU_SET& cpuSet, uint64_t groupID, KAFFINITY mask)
{
    cpuSet.clear();

    for (size_t i = 0; i < sizeof(mask) * 8; i++)
    {
        if (mask & (static_cast<KAFFINITY>(1) << i))
            cpuSet.insert(MAXIMUM_PROC_PER_GROUP * groupID + i); //We'll calculate processor ID as a single value derived from its group ID and index in affinity mask
    }
}

static bool AssociateThreadWithProcessor(ThreadHandle threadHandle, uint64_t groupID, uint64_t processorID)
{
    GROUP_AFFINITY affinity = {};
    affinity.Group = static_cast<WORD>(groupID);
    affinity.Mask = static_cast<KAFFINITY>(1) << processorID;
    return SetThreadGroupAffinity(threadHandle, &affinity, nullptr) ? true : false;
}


//Implementation---------------------------------------------------------------

//LogicalCpu---------------
uint64_t LogicalCpu::GetGroup() const
{
    return this->processorID / MAXIMUM_PROC_PER_GROUP;
}

uint64_t LogicalCpu::GetID() const
{
    return this->processorID % MAXIMUM_PROC_PER_GROUP;
}

bool LogicalCpu::AssociateCurrentThread() const
{
    return AssociateThreadWithProcessor(GetCurrentThread(), GetGroup(), GetID());
}

void LogicalCpu::AssociateCurrentThread(Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    if (!AssociateThreadWithProcessor(GetCurrentThread(), GetGroup(), GetID()))
    {
        FINJIN_SET_ERROR(error, "Failed to set thread affinity.");
        return;
    }
}

bool LogicalCpu::AssociateThread(ThreadHandle threadHandle) const
{    
    return AssociateThreadWithProcessor(threadHandle, GetGroup(), GetID());
}

void LogicalCpu::AssociateThread(ThreadHandle threadHandle, Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    if (!AssociateThreadWithProcessor(threadHandle, GetGroup(), GetID()))
    {
        FINJIN_SET_ERROR(error, "Failed to set thread affinity for specified thread.");
        return;
    }
}

//LogicalCpus------------------------
void LogicalCpus::AssociateCurrentThreadAndRemove(LogicalCpu* removed)
{
    //Search in a way that keeps same group that it already has
    GROUP_AFFINITY affinity = {};
    GetThreadGroupAffinity(GetCurrentThread(), &affinity);
    auto cpuFoundAt = std::find_if(begin(), end(), [affinity](const LogicalCpu& cpu) { return cpu.GetGroup() == affinity.Group; });
    if (cpuFoundAt != end())
    {
        //Found it
        if (removed != nullptr)
            *removed = *cpuFoundAt;

        //Associate with current thread
        cpuFoundAt->AssociateCurrentThread();

        //Remove the found CPU
        remove(cpuFoundAt);
    }
}

void LogicalCpus::Enumerate()
{
    CpuMap cpuMap;

    //Get initial processor information, keyed by processor ID
    CpuSet cpuSet;
    for (ProcessorInfoEnumerator e(RelationNumaNode); auto pi = e.GetCurrent(); e.Next())
    {
        ComputeCpuSet(cpuSet, pi->NumaNode.GroupMask.Group, pi->NumaNode.GroupMask.Mask);
        for (auto processorID : cpuSet)
        {
            LogicalCpu t;
            t.nodeID = pi->NumaNode.NodeNumber;
            t.processorID = processorID;

            cpuMap.insert(processorID, t);
        }
    }

    //Figure out cache information for each processor
    CacheCpuSet cacheCpuSet;
    for (ProcessorInfoEnumerator e(RelationCache); auto pi = e.GetCurrent(); e.Next())
    {
        //Ignore non-data caches
        if (pi->Cache.Type != CacheUnified && pi->Cache.Type != CacheData)
            continue;

        size_t levelIndex = pi->Cache.Level - 1;
        if (levelIndex < LogicalCpu::MAX_CACHES)
        {
            ComputeCpuSet(cacheCpuSet, pi->Cache.GroupMask.Group, pi->Cache.GroupMask.Mask);
            for (auto processorID : cacheCpuSet)
            {
                auto foundAt = cpuMap.find(processorID);
                if (foundAt != cpuMap.end())
                    foundAt->second.cacheProcessorSharing[levelIndex] = cacheCpuSet;
            }
        }
    }

    //Convert map to array
    this->count = 0;
    for (const auto& p : cpuMap)
    {
        if (push_back(p.second).HasErrorOrValue(false))
            break;
    }    
}
