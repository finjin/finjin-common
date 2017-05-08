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


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/Chrono.hpp"
#include "finjin/common/LruCache.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename ValueType, typename MutexType>
    class DateTimeLruCache : public LruCache<ValueType, DateTime, MutexType>
    {
    public:
        using Super = LruCache<ValueType, DateTime, MutexType>;
        using Settings = typename Super::Settings;

        void Create(const Settings& settings)
        {
            auto newSettings = settings;
            newSettings.timeNow = DateTime::NowUtc;
            Super::Create(newSettings);
        }
    };

    template <typename ValueType, typename MutexType>
    class HighResolutionTimeStampLruCache : public LruCache<ValueType, HighResolutionTimeStamp, MutexType>
    {
    public:
        using Super = LruCache<ValueType, HighResolutionTimeStamp, MutexType>;
        using Settings = typename Super::Settings;

        void Create(const Settings& settings)
        {
            auto newSettings = settings;
            newSettings.timeNow = [this]() { return this->clock.Now(); };
            Super::Create(newSettings);
        }

    private:
        HighResolutionClock clock;
    };

} }
