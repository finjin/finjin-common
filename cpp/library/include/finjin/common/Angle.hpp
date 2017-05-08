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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class Radians;

    class FINJIN_COMMON_LIBRARY_API Degrees
    {
    public:
        Degrees();
        Degrees(const Degrees& other);
        Degrees(const Radians& other);
        explicit Degrees(float value);

        Degrees& operator = (const Degrees& other);
        Degrees& operator = (const Radians& other);

        bool operator == (const Degrees& other) const;
        bool operator != (const Degrees& other) const;

        Degrees operator * (float scale) const;
        Degrees operator / (float scale) const;

        float ToDegreesValue() const;
        float ToRadiansValue() const;

    private:
        float value;
    };

    class FINJIN_COMMON_LIBRARY_API Radians
    {
    public:
        Radians();
        Radians(const Degrees& other);
        Radians(const Radians& other);
        explicit Radians(float value);

        Radians& operator = (const Degrees& other);
        Radians& operator = (const Radians& other);

        bool operator == (const Radians& other) const;
        bool operator != (const Radians& other) const;

        Radians operator * (float scale) const;
        Radians operator / (float scale) const;

        float ToDegreesValue() const;
        float ToRadiansValue() const;

    private:
        float value;
    };

    typedef Radians Angle;

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    inline Degrees operator * (float scale, const Degrees& value)
    {
        return value * scale;
    }

    inline Radians operator * (float scale, const Radians& value)
    {
        return value * scale;
    }

} }
