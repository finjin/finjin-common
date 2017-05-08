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
BOOST_AUTO_TEST_CASE(DateTimeTest_parse)
{
    BOOST_TEST_MESSAGE("DateTimeTest_parse:");

    FINJIN_DECLARE_ERROR(error);
    DateTime parsed;
    DateTime::ParseIso8601(parsed, "2015-10-19T22:31:53.0Z", error);
    if (error)
        BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());

    Utf8String parsedAsString = parsed.ToIso8601();
    BOOST_CHECK(parsedAsString == "2015-10-19T22:31:53.0Z");
}
