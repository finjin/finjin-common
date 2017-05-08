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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Uuid.hpp"


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UuidTest_parse_and_tostring)
{
    BOOST_TEST_MESSAGE("UuidTest_parse_and_tostring:");

    Utf8String uuidStringLower = "0c56fd86-f297-495e-9340-81cc075bed6c";

    //Parsing should work with lower or uppercase
    Uuid uuid = Uuid::Parse("0c56fd86-F297-495E-9340-81cc075bed6C"); //Mixed case
    auto uuidToString = uuid.ToString();
    BOOST_CHECK(uuid.ToString() == uuidStringLower);

    //Formatting as string should always end up as lowercase
    uuid = Uuid::Parse("0C56FD86-F297-495E-9340-81CC075BED6C"); //All upper
    uuidToString = uuid.ToString();
    BOOST_CHECK(uuid.ToString() == uuidStringLower);
}

BOOST_AUTO_TEST_CASE(UuidTest_construct_and_tostring)
{
    Utf8String uuidString("fae1331f-4915-46da-b8ad-0a59cceb0dad");

    Uuid test1(uuidString);
    BOOST_CHECK(test1.ToString() == uuidString);

    Uuid test2(0xfae1331f, 0x4915, 0x46da, 0xb8, 0xad, 0xa, 0x59, 0xcc, 0xeb, 0xd, 0xad);
    BOOST_CHECK(test2.ToString() == uuidString);

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    GUID g = { 0xfae1331f, 0x4915, 0x46da,{ 0xb8, 0xad, 0xa, 0x59, 0xcc, 0xeb, 0xd, 0xad } };
    Uuid test3(g);
    BOOST_CHECK(test3.ToString() == uuidString);
#endif
}
