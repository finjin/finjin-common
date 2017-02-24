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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class DistanceUnitType
    {
        CENTIMETERS,
        METERS,
        KILOMETERS
    };

    template <typename T>
    struct Distance
    {
        Distance()
        {
            this->metersPerUnit = 1.0f;
        }

        T ToUnit(DistanceUnitType type) const
        {
            switch (type)
            {
                case DistanceUnitType::CENTIMETERS: return ToCentimeters();
                case DistanceUnitType::METERS: return ToMeters(); 
                case DistanceUnitType::KILOMETERS: return ToKilometers();
                default: return this->value;
            }
        }

        T ToMeters() const
        {
            return this->value * this->metersPerUnit;
        }

        void SetMeters(const T& value)
        {
            this->value = value;
            this->metersPerUnit = 1.0f;
        }

        T ToCentimeters() const
        {
            return this->value * (this->metersPerUnit * 100.0f);
        }

        void SetCentimeters(const T& value)
        {
            this->value = value;
            this->metersPerUnit = 1.0f/100.0f;
        }

        T ToKilometers() const
        {
            return this->value * (this->metersPerUnit / 1000.0f);
        }

        void SetKilometers(const T& value)
        {
            this->value = value;
            this->metersPerUnit = 1000.0f;
        }

        T value;
        float metersPerUnit;
    };

} }
