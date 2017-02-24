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


//Includes---------------------------------------------------------------------
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/JobSystem.hpp"
#include "finjin/common/MemorySize.hpp"
#include <memory>


//Test support functions-------------------------------------------------------
//Somehow causes a memory leak on Windows?
/*template <uint64_t n>
decltype(auto) SubmitFibonacciJob(JobSystem& s)
{
    return s.Submit(
        JobThreadIndex::AnyPublic(),
        []() {
        uint64_t first = 1, second = 1, third = -1;
        for (uint64_t i = 2; i < n; ++i)
        {
            third = first + second;
            first = second;
            second = third;
        }

        return third;});
}*/


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(JobSystemTest_submit)
{
    BOOST_TEST_MESSAGE("JobSystemTest_submit:");

    FINJIN_DECLARE_ERROR(error);

    LogicalCpus logicalCpus;
    
    logicalCpus.Enumerate();
    BOOST_CHECK(logicalCpus.size() != 0);

    if (logicalCpus.size() > 1)
    {
        logicalCpus.AssociateCurrentThreadAndRemove();

        JobSystem::Settings settings;
        settings.fiberCount.requested = 100;
        settings.threadStackByteCount = MemorySize::MEBIBYTE * 2;
        settings.threadStackReserveByteCount = MemorySize::MEBIBYTE * 2;
        settings.Finalize(logicalCpus);
        
        std::unique_ptr<JobSystem> jobSystem(new JobSystem);
        jobSystem->Create(settings, error);
        if (error)
        {
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
            return;
        }
        
        jobSystem->Start(error);
        if (error)
        {
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
            return;
        }

        auto f = jobSystem->Submit(0, [](int a, int b) {return a + b; }, 1, 2);
        auto result = f.get();
        BOOST_CHECK(result == 3); 

        /*auto f2 = SubmitFibonacciJob<83>(*jobSystem.get());
        auto result2 = f.get();
        BOOST_CHECK(result2 == 99194853094755497UL);*/

        jobSystem->Stop();

        jobSystem->Destroy();
    }
}
