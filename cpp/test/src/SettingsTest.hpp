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
#include "finjin/common/Settings.hpp"


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SettingsTest_verify_set)
{
    BOOST_TEST_MESSAGE("SettingsTest_verify_set:");

    Setting<int> value(0);
    BOOST_CHECK(value.IsSet() == false);

    value = 1;
    BOOST_CHECK(value == 1);
    BOOST_CHECK(value.IsSet() == true);
}
