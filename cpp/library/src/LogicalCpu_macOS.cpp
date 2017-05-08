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
#include <pthread.h>
#include <mach/mach.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static bool AssociateMachThreadWithProcessor(mach_port_t machThread, uint64_t processorID)
{
    //Code samples:
    //https://developer.apple.com/library/ios/technotes/tn2169/_index.html
    //https://github.com/adobe/chromium/blob/master/base/threading/platform_thread_mac.mm

    //Note: This should be called before the thread has started, otherwise it might not have effect
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = static_cast<integer_t>(processorID + 1);
    auto kernelResult = thread_policy_set(machThread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
    return kernelResult == KERN_SUCCESS;
}

static bool AssociateThreadWithProcessor(ThreadHandle threadHandle, uint64_t processorID)
{
    auto machThread = pthread_mach_thread_np(threadHandle);
    return AssociateMachThreadWithProcessor(machThread, processorID);
}

static bool AssociateCurrentThreadWithProcessor(uint64_t processorID)
{
    auto machThread = mach_thread_self();
    return AssociateMachThreadWithProcessor(machThread, processorID);
}


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
    return AssociateCurrentThreadWithProcessor(this->processorID);
}

void LogicalCpu::AssociateCurrentThread(Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    if (!AssociateCurrentThreadWithProcessor(this->processorID))
        FINJIN_SET_ERROR(error, "Failed to associate current thread with the specified CPU/core.");
}

bool LogicalCpu::AssociateThread(ThreadHandle threadHandle) const
{
    return AssociateThreadWithProcessor(threadHandle, this->processorID);
}

void LogicalCpu::AssociateThread(ThreadHandle threadHandle, Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    if (!AssociateThreadWithProcessor(threadHandle, this->processorID))
        FINJIN_SET_ERROR(error, "Failed to associate thread with the specified CPU/core.");
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

    //https://developer.apple.com/library/mac/releasenotes/Performance/RN-AffinityAPI/#//apple_ref/doc/uid/TP40006635-CH1-DontLinkElementID_1
    //For Leopard, the hw.cacheconfig sysctl reports the number of logical processors sharing caches at various levels in the system. That is:
    //  hw.cacheconfig[0] reports the RAM sharing (the total number of logical processors).
    //  hw.cacheconfig[1] reports the L1 sharing (the number of logical processors sharing a level 1 cache).
    //  hw.cacheconfig[2] reports the L2 sharing.

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
