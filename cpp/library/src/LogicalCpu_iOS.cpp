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
#include <sys/sysctl.h>

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//LogicalCpu
uint64_t LogicalCpu::GetGroup() const
{
    return 0;
}

uint64_t LogicalCpu::GetID() const
{
    return this->processorID;
}

bool LogicalCpu::AssociateCurrentThread() const
{
    return true;
}

void LogicalCpu::AssociateCurrentThread(Error& error) const
{
    //Do nothing
}

bool LogicalCpu::AssociateThread(ThreadHandle threadHandle) const
{
    return true;
}

void LogicalCpu::AssociateThread(ThreadHandle threadHandle, Error& error) const
{
    //Do nothing
}

//LogicalCpus
void LogicalCpus::AssociateCurrentThreadAndRemove(LogicalCpu* removed)
{
    //Just use the first cpu
    auto cpuFoundAt = begin();
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
    clear();

    unsigned int cpuCount;
    size_t cpuCountSize = sizeof(cpuCount);
    sysctlbyname("hw.ncpu", &cpuCount, &cpuCountSize, nullptr, 0); //hw.physicalcpu

    LogicalCpu logicalCpu;
    for (size_t i = 0; i < cpuCount; i++)
    {
        logicalCpu.processorID = i;
        logicalCpu.nodeID = 0;

        push_back(logicalCpu);
    }
}
