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
#include "FinjinPrecompiled.hpp"
#include "finjin/common/Angle.hpp"
#include "finjin/common/Math.hpp"

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define RADIANS_PER_DEGREE (FINJIN_PI / 180.0f)


//Implementation----------------------------------------------------------------

//Degrees
Degrees::Degrees()
{
    this->value = 0;
}

Degrees::Degrees(const Degrees& other)
{
    this->value = other.value;
}

Degrees::Degrees(const Radians& other)
{
    this->value = other.ToRadiansValue();
}

Degrees::Degrees(float value)
{
    this->value = value * RADIANS_PER_DEGREE;
}

Degrees& Degrees::operator = (const Degrees& other)
{
    this->value = other.value;
    return *this;
}

Degrees& Degrees::operator = (const Radians& other)
{
    this->value = other.ToRadiansValue();
    return *this;
}

bool Degrees::operator == (const Degrees& other) const
{
    return this->value == other.value;
}

bool Degrees::operator != (const Degrees& other) const
{
    return this->value != other.value;
}

Degrees Degrees::operator * (float scale) const
{
    return Degrees(this->value * scale);
}

Degrees Degrees::operator / (float scale) const
{
    return Degrees(this->value / scale);
}

float Degrees::ToDegreesValue() const
{
    return this->value / RADIANS_PER_DEGREE;
}

float Degrees::ToRadiansValue() const
{
    return this->value;
}

//Radians
Radians::Radians()
{
    this->value = 0;
}

Radians::Radians(const Degrees& other)
{
    this->value = other.ToRadiansValue();
}

Radians::Radians(const Radians& other)
{
    this->value = other.value;
}

Radians::Radians(float value)
{
    this->value = value;
}

Radians& Radians::operator = (const Degrees& other)
{
    this->value = other.ToRadiansValue();
    return *this;
}

Radians& Radians::operator = (const Radians& other)
{
    this->value = other.value;
    return *this;
}

bool Radians::operator == (const Radians& other) const
{
    return this->value == other.value;
}

bool Radians::operator != (const Radians& other) const
{
    return this->value != other.value;
}

Radians Radians::operator * (float scale) const
{
    return Radians(this->value * scale);
}

Radians Radians::operator / (float scale) const
{
    return Radians(this->value / scale);
}

float Radians::ToDegreesValue() const
{
    return this->value / RADIANS_PER_DEGREE;
}

float Radians::ToRadiansValue() const
{
    return this->value;
}
