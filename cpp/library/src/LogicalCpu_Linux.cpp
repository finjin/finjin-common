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
#include "finjin/common/Path.hpp"
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <sched.h>

using namespace Finjin::Common;


//Local classes----------------------------------------------------------------
class DirectoryIterator : public std::iterator<std::input_iterator_tag, const std::pair<uint32_t, boost::filesystem::path> > 
{
public:
    DirectoryIterator() : i_(), e_(), exp_(), idx_()
    {
    }

    DirectoryIterator(const boost::filesystem::path& dir, const std::string& exp) : i_(dir), e_(), exp_(exp), idx_()
    {
        while (i_ != e_ && ! eval_(*i_))
        {
            ++i_;
        }
    }

    bool operator == (const DirectoryIterator& other)
    {
        return i_ == other.i_;
    }

    bool operator != (const DirectoryIterator& other)
    {
        return i_ != other.i_;
    }

    DirectoryIterator& operator ++ ()
    {
        do 
        {
            ++i_;
        } while (i_ != e_ && ! eval_(*i_));
        
        return * this;
    }

    DirectoryIterator operator ++ (int)
    {
        DirectoryIterator tmp(*this);
        ++*this;
        return tmp;
    }

    reference operator * () const 
    {
        return idx_;
    }

    pointer operator -> () const 
    {
        return &idx_;
    }

private:
    bool eval_(const boost::filesystem::directory_entry& entry) 
    {
        std::string filename(entry.path().filename().string());
        std::smatch what;
        if (!std::regex_search(filename, what, exp_)) 
            return false;
        
        FINJIN_DECLARE_ERROR(error);
        uint32_t id;
        Convert::ToInteger(id, what[1].str().c_str(), error);
        if (error)
            return false;

        idx_ = std::make_pair(id, entry.path());
        return true;
    }

    boost::filesystem::directory_iterator i_;
    boost::filesystem::directory_iterator e_;
    std::regex exp_;
    std::pair<uint32_t, boost::filesystem::path> idx_;
};


//Local functions--------------------------------------------------------------
template <typename T>
static Utf8String GetFirstLine(const T& buffer)
{
    Utf8String result;
    
    for (size_t i = 0; i < buffer.size(); i++)
    {
        if (buffer[i] == '\r' || buffer[i] == '\n')
        {
            result.assign(&buffer[0], i);
            return result;
        }
    }
    
    if (!buffer.empty())
        result.assign(&buffer[0], buffer.size());
    
    return result;
}

static std::set<uint32_t> ParseIDsFromLine(const Utf8String& content) 
{
    std::set<uint32_t> ids;
    std::vector<Utf8StringView> range;
    
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
            FINJIN_DECLARE_ERROR(error);
            if (range.size() == 1)
            {
                //Only one ID
                uint32_t id;
                Convert::ToInteger(id, range[0].ToString(), error);
                if (!error)
                    ids.insert(static_cast<unsigned int>(id));
            }
            else
            {
                //Range of IDs
                uint32_t first;
                Convert::ToInteger(first, range.front().ToString(), error);
                if (!error)
                {
                    uint32_t last;
                    Convert::ToInteger(last, range.back().ToString(), error);
                    if (!error)
                    {
                        for (auto i = first; i <= last; ++i)
                            ids.insert(i);
                    }
                }
            }
        }

        return ValueOrError<bool>(true);
    });
    
    return ids;
}

static bool AssociateThreadWithProcessor(ThreadHandle threadHandle, uint64_t processorID)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(processorID, &mask);
    return sched_setaffinity(threadHandle, sizeof(mask), &mask) == 0;
}


//Implementation---------------------------------------------------------------

//LogicalCpu---------------
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

//LogicalCpus------------------------
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
    
    StaticVector<uint8_t, 1024> tempFileBuffer;
    
    //Parse list of CPUs
    Path possiblePath;
#if FINJIN_TARGET_OS_IS_ANDROID
    possiblePath = "/sys/devices/system/cpu/possible";
#else
    possiblePath = "/sys/devices/system/cpu/present";
#endif
    if (!possiblePath.ReadBinaryFile(tempFileBuffer))
        return;
    
    auto firstLine = GetFirstLine(tempFileBuffer);
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
            DirectoryIterator numaNodeIterator(cpuPath.c_str(), "^node([0-9]+)$");
            DirectoryIterator numaNodeIteratorEnd;
            if (numaNodeIterator != numaNodeIteratorEnd)
                logicalCpu.nodeID = numaNodeIterator->first;

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
                    firstLine = GetFirstLine(tempFileBuffer);
                    
                    logicalCpu.cacheProcessorSharing[cacheIndex] = ParseIDsFromLine(firstLine);
                    logicalCpu.cacheProcessorSharing[cacheIndex].remove(processorID); // remove itself from shared L1 list
                }                
            }
        }    
        
        //Store parsed logical CPU
        push_back(logicalCpu);
    }
}
