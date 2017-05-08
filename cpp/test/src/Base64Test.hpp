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
#include "finjin/common/Base64.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/ByteBuffer.hpp"


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Base64Test_empty)
{
    BOOST_TEST_MESSAGE("Base64Test_empty:");

    ByteBuffer bytes;
    BOOST_CHECK(Base64::ToBase64Count(bytes.size()) == 0);
}

BOOST_AUTO_TEST_CASE(Base64Test_one_two_three)
{
    BOOST_TEST_MESSAGE("Base64Test_one_two_three:");

    ByteBuffer bytes;

    bytes.Create(1, FINJIN_ALLOCATOR_NULL);
    BOOST_CHECK(Base64::ToBase64Count(bytes.size()) == 4);

    bytes.Create(2, FINJIN_ALLOCATOR_NULL);
    BOOST_CHECK(Base64::ToBase64Count(bytes.size()) == 4);

    bytes.Create(3, FINJIN_ALLOCATOR_NULL);
    BOOST_CHECK(Base64::ToBase64Count(bytes.size()) == 4);
}

BOOST_AUTO_TEST_CASE(Base64Test_four)
{
    BOOST_TEST_MESSAGE("Base64Test_four:");

    ByteBuffer bytes;

    bytes.Create(4, FINJIN_ALLOCATOR_NULL);
    BOOST_CHECK(Base64::ToBase64Count(bytes.size()) == 8);
}

BOOST_AUTO_TEST_CASE(Base64Test_encode)
{
    BOOST_TEST_MESSAGE("Base64Test_encode:");

    FINJIN_DECLARE_ERROR(error);

    ByteBuffer bytes;
    bytes.Create(3, FINJIN_ALLOCATOR_NULL);
    bytes[0] = 1;
    bytes[1] = 2;
    bytes[2] = 3;

    Utf8String base64;
    Base64::ToBase64(bytes, base64, error);
    if (error)
    {
        BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
    }
    else
    {
        BOOST_CHECK(base64 == "AQID");
    }
}

BOOST_AUTO_TEST_CASE(Base64Test_decode)
{
    BOOST_TEST_MESSAGE("Base64Test_decode:");

    FINJIN_DECLARE_ERROR(error);

    Utf8String base64 = "AQID";
    ByteBuffer bytes;
    bytes.CreateEmpty(100, FINJIN_ALLOCATOR_NULL);
    Base64::ToBytes(base64, bytes, error);
    if (error)
    {
        BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
    }
    else
    {
        BOOST_CHECK(bytes.size() == 3);
        BOOST_CHECK(bytes[0] == 1);
        BOOST_CHECK(bytes[1] == 2);
        BOOST_CHECK(bytes[2] == 3);
    }
}
