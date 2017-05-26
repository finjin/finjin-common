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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class WxTimeDurationUnit
    {
        NANOSECONDS,
        MICROSECONDS,
        MILLISECONDS,
        SECONDS,
        MINUTES,
        HOURS,

        COUNT
    };

    /** A time interval. */
    class WxTimeDuration
    {
    public:
        bool operator == (const WxTimeDuration& other) const;
        bool operator != (const WxTimeDuration& other) const;
        bool operator < (const WxTimeDuration& other) const;
        bool operator <= (const WxTimeDuration& other) const;
        bool operator > (const WxTimeDuration& other) const;
        bool operator >= (const WxTimeDuration& other) const;

        bool IsZero() const;

        int64_t ToNanoseconds() const;
        int64_t ToMicroseconds() const;
        int64_t ToMilliseconds() const;
        int64_t ToSeconds() const;
        int64_t ToMinutes() const;
        int64_t ToHours() const;

        double ToSecondsDouble() const;

        WxTimeDuration operator + (const WxTimeDuration& other) const;
        WxTimeDuration operator - (const WxTimeDuration& other) const;
        WxTimeDuration operator * (const WxTimeDuration& other) const;
        WxTimeDuration operator / (const WxTimeDuration& other) const;

        static WxTimeDuration Nanoseconds(int64_t value);
        static WxTimeDuration Microseconds(int64_t value);
        static WxTimeDuration Milliseconds(int64_t value);
        static WxTimeDuration Seconds(int64_t value);
        static WxTimeDuration Seconds(int value);
        static WxTimeDuration Seconds(double value, WxTimeDurationUnit resolution);
        static WxTimeDuration Seconds(float value, WxTimeDurationUnit resolution);
        static WxTimeDuration Minutes(int64_t value);
        static WxTimeDuration Hours(int64_t value);

        static const WxTimeDuration& GetZero();

        wxString ToString() const;
        wxString ToString(WxTimeDurationUnit unit) const;

        static void Parse(WxTimeDuration& timeDuration, const wxString& s, WxError& error);
        static WxTimeDuration Parse(const wxString& s, WxTimeDuration defaultValue = WxTimeDuration::GetZero());

    private:
        int64_t value; //Nanoseconds
    };

} }
