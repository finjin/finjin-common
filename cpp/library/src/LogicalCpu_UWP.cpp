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


//Local functions--------------------------------------------------------------
static bool AssociateThreadWithProcessor(ThreadHandle threadHandle, uint64_t groupID, uint64_t processorID)
{
    PROCESSOR_NUMBER processorNumber = { static_cast<WORD>(groupID), static_cast<BYTE>(processorID), 0 };
    return SetThreadIdealProcessorEx(threadHandle, &processorNumber, nullptr) ? true : false;
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
    auto cpuFoundAt = end();

    //Getting the current thread's ideal processor is a little hacky. You have to first set a new one, getting the old one in the process
    PROCESSOR_NUMBER oldProcessorNumber = {};
    PROCESSOR_NUMBER newProcessorNumber = {};
    if (SetThreadIdealProcessorEx(GetCurrentThread(), &newProcessorNumber, &oldProcessorNumber))
    {
        //Search in a way that keeps same group that it already has
        cpuFoundAt = std::find_if(begin(), end(), [&oldProcessorNumber](const LogicalCpu& cpu) { return cpu.GetGroup() == oldProcessorNumber.Group; });

        //Restore old processor association
        SetThreadIdealProcessorEx(GetCurrentThread(), &oldProcessorNumber, nullptr);
    }    
    
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
    LogicalCpu logicalCpu;

    SYSTEM_INFO systemInfo = {};
    GetNativeSystemInfo(&systemInfo);

    this->count = 0;
    for (DWORD i = 0; i < systemInfo.dwNumberOfProcessors; i++)
    {
        if (systemInfo.dwActiveProcessorMask & ((DWORD_PTR)1 << i))
        {
            logicalCpu.processorID = i;
            if (push_back(logicalCpu).HasErrorOrValue(false))
                break;
        }
    }
}
