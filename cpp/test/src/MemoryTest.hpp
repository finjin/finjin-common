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
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/ForwardAllocator.hpp"
#include "finjin/common/GeneralAllocator.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/PassthroughSystemAllocator.hpp"
#include <memory>
#include <boost/thread/null_mutex.hpp>


//Types-------------------------------------------------------------------------
class MemoryPerson : public AllocatedClass
{
public:
    int age;

    MemoryPerson(Allocator* allocator, int age) : AllocatedClass(allocator)
    {
        this->age = age;
    }
};


//Tests-------------------------------------------------------------------------
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

    PassthroughSystemAllocator systemAllocator;

    auto arena = systemAllocator.AllocateArena(10000, 0, FINJIN_CALLER_ARGUMENTS);

    TestAllocator allocator;
    allocator.Create(std::move(arena), error);
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

BOOST_AUTO_TEST_CASE(MemoryTest_size_parse)
{
    BOOST_TEST_MESSAGE("MemoryTest_size_parse:");

    auto result = MemorySize::Parse64("12.5MB");
    BOOST_CHECK(result == 12500000);

    result = MemorySize::Parse64("12MB");
    BOOST_CHECK(result == 12000000);
}

BOOST_AUTO_TEST_CASE(MemoryTest_swap_bytes)
{
    BOOST_TEST_MESSAGE("MemoryTest_swap_bytes:");

    {
        std::array<uint8_t, 4> value;
        value[0] = 0;
        value[1] = 1;
        value[2] = 2;
        value[3] = 3;
        SwapBytes(value);
        BOOST_CHECK(value[0] == 3);
        BOOST_CHECK(value[1] == 2);
        BOOST_CHECK(value[2] == 1);
        BOOST_CHECK(value[3] == 0);
    }

    {
        std::array<uint8_t, 5> value;
        value[0] = 0;
        value[1] = 1;
        value[2] = 2;
        value[3] = 3;
        value[4] = 4;
        SwapBytes(value);
        BOOST_CHECK(value[0] == 4);
        BOOST_CHECK(value[1] == 3);
        BOOST_CHECK(value[2] == 2);
        BOOST_CHECK(value[3] == 1);
        BOOST_CHECK(value[4] == 0);
    }
}
