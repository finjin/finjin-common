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
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/StaticVector.hpp"


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(CollectionTest_allocatedvector_pushfront)
{
    BOOST_TEST_MESSAGE("CollectionTest_allocatedvector_pushfront:");

    AllocatedVector<int> v;
    BOOST_CHECK(v.CreateEmpty(3, FINJIN_ALLOCATOR_NULL));

    BOOST_CHECK(v.push_front(1).HasValue(true));
    BOOST_CHECK(v[0] == 1);

    BOOST_CHECK(v.push_front(2).HasValue(true));
    BOOST_CHECK(v[0] == 2);
    BOOST_CHECK(v[1] == 1);
}

BOOST_AUTO_TEST_CASE(CollectionTest_staticvector_pushfront)
{
    BOOST_TEST_MESSAGE("CollectionTest_staticvector_pushfront:");

    StaticVector<int, 3> v;
    
    BOOST_CHECK(v.push_front(1).HasValue(true));
    BOOST_CHECK(v[0] == 1);

    BOOST_CHECK(v.push_front(2).HasValue(true));
    BOOST_CHECK(v[0] == 2);
    BOOST_CHECK(v[1] == 1);
}
