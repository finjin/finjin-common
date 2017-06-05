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
#include "finjin/common/SystemAllocator.hpp"
#include "finjin/common/MemorySize.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    #include <sys/sysinfo.h>
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    #import <mach/mach.h>
    #import <mach/mach_host.h>
#endif

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
SystemAllocator::SystemAllocator()
{
}

SystemAllocator::~SystemAllocator()
{
}

void* SystemAllocator::Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    return SystemAllocate(byteCount, FINJIN_CALLER_PARAMETERS);
}

bool SystemAllocator::CanDeallocateBlock() const
{
    return true;
}

void SystemAllocator::Deallocate(void* mem)
{
    SystemDeallocate(mem);
}

bool SystemAllocator::CanDeallocateAll() const
{
    return false;
}

void SystemAllocator::DeallocateAll()
{
}

size_t SystemAllocator::GetBytesUsed() const
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    return Windows::System::MemoryManager::AppMemoryUsage;
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);
    return memoryStatus.ullTotalPhys - memoryStatus.ullAvailPhys;
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram - info.freeram;
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    mach_port_t hostPort = mach_host_self();

    vm_size_t pageSize;
    host_page_size(hostPort, &pageSize);

    vm_statistics_data_t vmStat;
    mach_msg_type_number_t hostSize = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    if (host_statistics(hostPort, HOST_VM_INFO, (host_info_t)&vmStat, &hostSize) != KERN_SUCCESS)
        return MemorySize::UNKNOWN_SIZE;

    return (vmStat.active_count + vmStat.inactive_count + vmStat.wire_count) * pageSize;
#else
    return MemorySize::UNKNOWN_SIZE;
#endif
}

size_t SystemAllocator::GetBytesFree() const
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    auto family = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily;
    if (family == L"Windows.Desktop")
    {
        //Note: On Windows 10 desktop AppMemoryUsageLimit is system's total system virtual memory, not physical memory
        //For example: On my laptop with 8GB RAM (2GB free), AppMemoryUsageLimit reports that 14GB is available
        return MemorySize::GIBIBYTE - Windows::System::MemoryManager::AppMemoryUsage; //Impose an artificial limit
    }
    else
    {
        //TODO: Verify that this works as expected on other devices such as Xbox
        return Windows::System::MemoryManager::AppMemoryUsageLimit - Windows::System::MemoryManager::AppMemoryUsage;
    }
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);
    return memoryStatus.ullAvailPhys;
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram;
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    mach_port_t hostPort = mach_host_self();

    vm_size_t pageSize;
    host_page_size(hostPort, &pageSize);

    vm_statistics_data_t vmStat;
    mach_msg_type_number_t hostSize = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    if (host_statistics(hostPort, HOST_VM_INFO, (host_info_t)&vmStat, &hostSize) != KERN_SUCCESS)
        return MemorySize::UNKNOWN_SIZE;

    return vmStat.free_count * pageSize;
#else
    return MemorySize::UNKNOWN_SIZE;
#endif
}

size_t SystemAllocator::GetAlignment() const
{
    return GetDefaultAlignment();
}
