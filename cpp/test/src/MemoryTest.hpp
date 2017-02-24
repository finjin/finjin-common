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
#include "finjin/common/ForwardAllocator.hpp"
#include "finjin/common/GeneralAllocator.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/PassthroughSystemAllocator.hpp"
#include <memory>
#include <boost/thread/null_mutex.hpp>

class MemoryPerson : public AllocatedClass
{
public:
    int age;

    MemoryPerson(Allocator* allocator, int age) : AllocatedClass(allocator)
    {
        this->age = age;
    }
};


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(MemoryTest_allocated_class)
{
    BOOST_TEST_MESSAGE("MemoryTest_allocated_class:");

    FINJIN_DECLARE_ERROR(error);

    PassthroughSystemAllocator systemAllocator;

    auto arena = systemAllocator.AllocateArena(10000, 0, FINJIN_CALLER_ARGUMENTS);
    
    GeneralAllocator::Settings settings;
    
    GeneralAllocator generalAllocator;
    generalAllocator.Create(settings, std::move(arena));

    auto person = AllocatedClass::New<MemoryPerson>(&generalAllocator, FINJIN_CALLER_ARGUMENTS, 123);
    BOOST_CHECK(person != nullptr);
    BOOST_CHECK(person->age == 123);
}

BOOST_AUTO_TEST_CASE(MemoryTest_forward)
{
    BOOST_TEST_MESSAGE("MemoryTest_forward:");
    
    using TestAllocator = ForwardAllocator<boost::null_mutex>;
    
    FINJIN_DECLARE_ERROR(error);
    
    TestAllocator::Settings allocatorSettings;
    
    PassthroughSystemAllocator systemAllocator;

    auto arena = systemAllocator.AllocateArena(10000, 0, FINJIN_CALLER_ARGUMENTS);
    
    TestAllocator allocator;
    allocator.Create(allocatorSettings, std::move(arena), error);
    assert(!error);

    BOOST_CHECK(allocator.GetBytesUsed() == 0);

    allocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
    allocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
    allocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
    allocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);

    allocator.DeallocateAll();

    BOOST_CHECK(allocator.GetBytesUsed() == 0);
}

BOOST_AUTO_TEST_CASE(MemoryTest_general_allocator)
{
    BOOST_TEST_MESSAGE("MemoryTest_general_allocator:");

    FINJIN_DECLARE_ERROR(error);

    PassthroughSystemAllocator systemAllocator;

    auto arena = systemAllocator.AllocateArena(10000, 0, FINJIN_CALLER_ARGUMENTS);

    GeneralAllocator::Settings settings;

    GeneralAllocator generalAllocator;
    generalAllocator.Create(settings, std::move(arena));

    //2 successful allocations and 1 failed allocation, then free all
    {
        auto mem = generalAllocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
        BOOST_CHECK(mem != nullptr);

        auto mem2 = generalAllocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
        BOOST_CHECK(mem2 != nullptr);

        auto mem3 = generalAllocator.Allocate(10000000, FINJIN_CALLER_ARGUMENTS);
        BOOST_CHECK(mem3 == nullptr);

        generalAllocator.DeallocateAll();
        BOOST_CHECK(generalAllocator.GetBytesUsed() == 0);
    }

    //3 successful allocations, then free the one at a time in random order
    {
        auto mem = generalAllocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
        BOOST_CHECK(mem != nullptr);

        auto mem2 = generalAllocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
        BOOST_CHECK(mem2 != nullptr);

        auto mem3 = generalAllocator.Allocate(100, FINJIN_CALLER_ARGUMENTS);
        BOOST_CHECK(mem3 != nullptr);

        generalAllocator.Deallocate(mem2);
        generalAllocator.Deallocate(mem3);
        generalAllocator.Deallocate(mem);

        BOOST_CHECK(generalAllocator.GetBytesUsed() == 0);
    }
}
