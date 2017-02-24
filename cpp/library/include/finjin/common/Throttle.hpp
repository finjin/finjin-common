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


//Includes---------------------------------------------------------------------
#include "finjin/common/Chrono.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class Throttle
    {
    public:
        Throttle()
        {
            Reset();
        }

        void Reset()
        {
            this->timeAccumulator = 0;
            this->updateInterval = 0;
            this->triggered = false;
            this->pressing = false;
        }

        SimpleTimeDelta GetTimeout() const
        {
            return this->updateInterval;
        }

        void SetTimeout(SimpleTimeDelta value)
        {
            this->updateInterval = value;
        }

        SimpleTimeCounter GetAccumulator() const
        {
            return this->timeAccumulator;
        }

        void Update(SimpleTimeDelta elapsedTimeSeconds)
        {
            if (this->pressing && this->updateInterval > 0)
            {
                this->timeAccumulator += elapsedTimeSeconds;

                if (this->timeAccumulator >= this->updateInterval)
                {
                    this->triggered = true;

                    while (this->timeAccumulator >= this->updateInterval)
                        this->timeAccumulator -= this->updateInterval;
                }
                else
                    this->triggered = false;
            }
        }

        void Press()
        {
            this->pressing = true;

            this->triggered = false;
            this->timeAccumulator = 0;
        }

        void Release()
        {
            this->pressing = false;

            this->triggered = false;
            this->timeAccumulator = 0;
        }

        bool Triggered() const
        {
            return this->pressing && (this->triggered || this->updateInterval == 0);
        }
        
    private:
        SimpleTimeCounter timeAccumulator;
        SimpleTimeDelta updateInterval;
        bool triggered;
        bool pressing;
    };
    
} }
