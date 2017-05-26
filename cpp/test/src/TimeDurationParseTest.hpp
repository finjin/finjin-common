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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(TimeDurationParseTest_parse_microseconds)
{
    BOOST_TEST_MESSAGE("TimeDurationParseTest_parse_microseconds:");

    TimeDuration oneSecond;

    {
        FINJIN_DECLARE_ERROR(error);

        TimeDuration::Parse(oneSecond, "1us", error);
        if (error)
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(oneSecond.ToMicroseconds() == 1);
    }

    {
        FINJIN_DECLARE_ERROR(error);

        TimeDuration::Parse(oneSecond, "1microsecond", error);
        if (error)
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(oneSecond.ToMicroseconds() == 1);
    }

    {
        FINJIN_DECLARE_ERROR(error);

        TimeDuration::Parse(oneSecond, "1microseconds", error);
        if (error)
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(oneSecond.ToMicroseconds() == 1);
    }

    TimeDuration badNegative = TimeDuration::Parse("-1us");
    BOOST_CHECK(badNegative == TimeDuration::GetZero());
}

BOOST_AUTO_TEST_CASE(TimeDurationParseTest_parse_seconds)
{
    BOOST_TEST_MESSAGE("TimeDurationParseTest_parse_seconds:");

    TimeDuration oneSecond;

    {
        FINJIN_DECLARE_ERROR(error);

        TimeDuration::Parse(oneSecond, "1s", error);
        if (error)
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(oneSecond.ToSeconds() == 1);
    }

    {
        FINJIN_DECLARE_ERROR(error);

        TimeDuration::Parse(oneSecond, "1second", error);
        if (error)
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(oneSecond.ToSeconds() == 1);
    }

    {
        FINJIN_DECLARE_ERROR(error);

        TimeDuration::Parse(oneSecond, "1seconds", error);
        if (error)
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(oneSecond.ToSeconds() == 1);
    }

    TimeDuration badNegative = TimeDuration::Parse("-1s");
    BOOST_CHECK(badNegative == TimeDuration::GetZero());
}
