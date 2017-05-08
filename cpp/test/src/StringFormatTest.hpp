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
#include "finjin/common/Utf8StringFormatter.hpp"


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(StringFormatTest_simple)
{
    BOOST_TEST_MESSAGE("StringFormatTest_simple:");

    Utf8StringFormatter format("%1% was here");
    format % "someone";
    BOOST_CHECK(format.ToString() == "someone was here");
}

BOOST_AUTO_TEST_CASE(StringFormatTest_not_enough_args)
{
    BOOST_TEST_MESSAGE("StringFormatTest_not_enough_args:");

    Utf8StringFormatter format("%1% %2% %3%");
    format % "1" % 2; //Note the lack of a third argument
    BOOST_CHECK(format.ToString() == "1 2 <?>");
}

BOOST_AUTO_TEST_CASE(StringFormatTest_reuse)
{
    BOOST_TEST_MESSAGE("StringFormatTest_reuse:");

    Utf8StringFormatter format("%1%");

    BOOST_CHECK((format % "Hello").ToString() == "Hello");
    BOOST_CHECK((format % "Hello2").ToString() == "Hello2");
    BOOST_CHECK((format % "Test").ToString() == "Test");
}

BOOST_AUTO_TEST_CASE(StringFormatTest_direct)
{
    BOOST_TEST_MESSAGE("StringFormatTest_direct:");

    auto expected = "This 1 is the expected result";

    BOOST_CHECK(Utf8StringFormatter::Format("This %1% is the expected result", 1) == expected);
}
