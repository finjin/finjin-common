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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Utf8String.hpp"


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ByteBufferTest_unused)
{
    BOOST_TEST_MESSAGE("ByteBufferTest_unused:");

    ByteBuffer buffer;
    buffer.Create(100, FINJIN_ALLOCATOR_NULL);

    buffer.Truncate(40);
    BOOST_CHECK(buffer.size_left() == 60);

    buffer.resize(111);
    BOOST_CHECK(buffer.size_left() == 0);

    buffer.Truncate(0);
    BOOST_CHECK(buffer.size_left() == 100);
}

BOOST_AUTO_TEST_CASE(ByteBufferTest_append)
{
    BOOST_TEST_MESSAGE("ByteBufferTest_append:");

    ByteBuffer buffer;
    buffer.Create(50, FINJIN_ALLOCATOR_NULL);
    buffer.Truncate(0);

    const char* s = "123456789"; //9 bytes

    buffer.Write(s).Write(s).Write(s).Write(s).Write(s);
    BOOST_CHECK(buffer.size_left() == 5);

    buffer.Write(s);
    BOOST_CHECK(buffer.size_left() == 0);
    BOOST_CHECK(buffer.size() == 54);

    Utf8String bufferString((const char*)buffer.data(), buffer.size());
    BOOST_CHECK(bufferString == "123456789123456789123456789123456789123456789123456789"); //s duplicated 6 times
}
