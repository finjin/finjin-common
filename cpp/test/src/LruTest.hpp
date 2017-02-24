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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/RealTimeLruCache.hpp"
#include "finjin/common/ThisThread.hpp"


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(LruTest_basic)
{
    BOOST_TEST_MESSAGE("LruTest_basic:");

    using Cache = DateTimeLruCache<Utf8String, boost::null_mutex>;
    Cache::Settings settings;
    settings.cacheSize = 3;
    settings.deleteValue = [](LruCacheItemHandle handle, Utf8String& value)
    {
        //std::cout << "Deleting value: " << value << std::endl;
    };

    Cache cache;
    cache.Create(settings);
    auto x = cache.Add("1");
    auto y = cache.Add("2");
    auto z = cache.Add("3");
    ThisThread::SleepFor(TimeDuration::Seconds(1));
    cache.Touch(x);
    auto a = cache.Add("4");

    Utf8String expectedValues[] = { "3", "1", "4" };
    BOOST_CHECK(cache.Test(expectedValues, 3) == true);
}
