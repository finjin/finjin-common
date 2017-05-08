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
#include "finjin/common/Convert.hpp"
#include "finjin/common/DirEntFileFinder.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include <sched.h>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
template <typename T>
static void GetFirstLine(Utf8StringView& result, const T& buffer)
{
    result.clear();

    for (size_t i = 0; i < buffer.size(); i++)
    {
        if (buffer[i] == '\r' || buffer[i] == '\n')
        {
            result.assign(&buffer[0], i);
            return;
        }
    }

    if (!buffer.empty())
        result.assign(&buffer[0], buffer.size());
}

static StaticVector<uint32_t, CommonConstants::MAX_CPUS> ParseIDsFromLine(const Utf8StringView& content)
{
    StaticVector<uint32_t, CommonConstants::MAX_CPUS> ids;
    StaticVector<Utf8StringView, 2> range;
    Split(content, ',', [&ids, &range](Utf8StringView& entry)
    {
        entry.TrimTrailingWhitespace();

        range.clear();
        auto splitResult = Split(entry, '-', [&range](Utf8StringView& item)
        {
            range.push_back(item);
            return ValueOrError<bool>(true);
        });

        if (!range.empty())
        {
            if (range.size() == 1)
            {
                //Only one ID
                if (Utf8String::IsDigits(range[0].begin(), range[0].end()))
                {
                    uint32_t id = 0;
                    id = Convert::ToInteger(range[0].ToString(), id);
                    if (!ids.contains(id))
                        ids.push_back(id);
                }
            }
            else
            {
                //Range of IDs
                if (Utf8String::IsDigits(range[0].begin(), range[0].end()) &&
                    Utf8String::IsDigits(range[1].begin(), range[1].end()))
                {
                    uint32_t first = 0;
                    first = Convert::ToInteger(range[0].ToString(), first);

                    uint32_t last = 0;
                    last = Convert::ToInteger(range[1].ToString(), last);

                    for (auto i = first; i <= last; ++i)
                    {
                        if (!ids.contains(i))
                            ids.push_back(i);
                    }
                }
            }
        }

        return ValueOrError<bool>(true);
    });

    std::sort(ids.begin(), ids.end());
    return ids;
}

static bool AssociateThreadWithProcessor(ThreadHandle threadHandle, uint64_t processorID)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(processorID, &mask);
    return sched_setaffinity(threadHandle, sizeof(mask), &mask) == 0;
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
    return AssociateThreadWithProcessor(pthread_self(), this->processorID);
}

void LogicalCpu::AssociateCurrentThread(Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    if (!AssociateThreadWithProcessor(pthread_self(), this->processorID))
    {
        FINJIN_SET_ERROR(error, "Failed to set thread affinity.");
        return;
    }
}

bool LogicalCpu::AssociateThread(ThreadHandle threadHandle) const
{
    return AssociateThreadWithProcessor(threadHandle, this->processorID);
}

void LogicalCpu::AssociateThread(ThreadHandle threadHandle, Error& error) const
{
    FINJIN_ERROR_METHOD_START(error);

    if (!AssociateThreadWithProcessor(threadHandle, this->processorID))
    {
        FINJIN_SET_ERROR(error, "Failed to set thread affinity for specified thread.");
        return;
    }
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

    //Info on these file paths can be found at: https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-devices-system-cpu

    StaticVector<uint8_t, 1024> tempFileBuffer; //This is way more space than is necessary for the file

    //Parse list of CPUs
    Path possiblePath;
#if FINJIN_TARGET_PLATFORM_IS_ANDROID
    possiblePath = "/sys/devices/system/cpu/possible";
#else
    possiblePath = "/sys/devices/system/cpu/present";
#endif
    if (!possiblePath.ReadBinaryFile(tempFileBuffer))
        return;

    Utf8StringView firstLine;
    GetFirstLine(firstLine, tempFileBuffer);

    auto onlineProcessorIDs = ParseIDsFromLine(firstLine);
    if (onlineProcessorIDs.empty())
        return;

    //Iterate list of CPU IDs
    LogicalCpu logicalCpu;
    Path cpuPath, cachePath, fullCachePath;
    for (auto processorID : onlineProcessorIDs)
    {
        logicalCpu.Reset();

        logicalCpu.processorID = processorID;

        //Determine which NUMA node the CPU belongs to
        cpuPath = "/sys/devices/system/cpu/cpu";
        cpuPath += Convert::ToString(processorID);
        cpuPath += "/";
        if (cpuPath.IsDirectory())
        {
            DirEntFileFinder fileFinder;
            if (fileFinder.Start(cpuPath))
            {
                Path maybeNodePath;
                do
                {
                    if (!fileFinder.GetCurrentName(maybeNodePath).HasError() && maybeNodePath.StartsWith("node"))
                    {
                        //It's a file starting with 'node'
                        logicalCpu.nodeID = Convert::ToInteger(&maybeNodePath[4], logicalCpu.nodeID);
                        break;
                    }
                } while (fileFinder.Next());
            }

            //Determine which CPUs this CPU shares its caches with
            for (size_t cacheIndex = 0; cacheIndex < LogicalCpu::MAX_CACHES; cacheIndex++)
            {
                cachePath = "cache/index";
                cachePath += Convert::ToString(cacheIndex + 1);
                cachePath += "/shared_cpu_list";

                fullCachePath = cpuPath;
                fullCachePath /= cachePath;
                if (fullCachePath.ReadBinaryFile(tempFileBuffer))
                {
                    GetFirstLine(firstLine, tempFileBuffer);

                    logicalCpu.cacheProcessorSharing[cacheIndex] = ParseIDsFromLine(firstLine);
                    logicalCpu.cacheProcessorSharing[cacheIndex].remove(processorID); //Remove itself from shared L1 list
                }
            }
        }

        //Store parsed logical CPU
        push_back(logicalCpu);
    }
}
