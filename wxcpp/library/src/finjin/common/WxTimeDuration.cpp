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
#include "WxTimeDuration.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
bool WxTimeDuration::operator == (const WxTimeDuration& other) const
{
    return this->value == other.value;
}

bool WxTimeDuration::operator != (const WxTimeDuration& other) const
{
    return this->value != other.value;
}

bool WxTimeDuration::operator < (const WxTimeDuration& other) const
{
    return this->value < other.value;
}

bool WxTimeDuration::operator <= (const WxTimeDuration& other) const
{
    return this->value <= other.value;
}

bool WxTimeDuration::operator > (const WxTimeDuration& other) const
{
    return this->value > other.value;
}

bool WxTimeDuration::operator >= (const WxTimeDuration& other) const
{
    return this->value >= other.value;
}

bool WxTimeDuration::IsZero() const
{
    return *this == GetZero();
}

int64_t WxTimeDuration::ToNanoseconds() const
{
    return this->value;
}

int64_t WxTimeDuration::ToMicroseconds() const
{
    return this->value / 1000ll;
}

int64_t WxTimeDuration::ToMilliseconds() const
{
    return this->value / 1000000ll;
}

int64_t WxTimeDuration::ToSeconds() const
{
    return this->value / 1000000000ll;
}

int64_t WxTimeDuration::ToMinutes() const
{
    return this->value / (1000000000ll * 60ll);
}

int64_t WxTimeDuration::ToHours() const
{
    return this->value / (1000000000ll * 60ll * 60ll);
}

double WxTimeDuration::ToSecondsDouble() const
{
    return (double)this->value / 1000000000.0;
}

wxString WxTimeDuration::ToString() const
{
    auto result = ToString(WxTimeDurationUnit::NANOSECONDS);
    if (result.EndsWith(wxT("000ns")))
    {
        result.Replace(wxT("000ns"), wxT("us"), false);
        if (result.EndsWith(wxT("000us")))
        {
            result.Replace(wxT("000us"), wxT("ms"), false);
            if (result.EndsWith(wxT("000ms")))
            {
                result.Replace(wxT("000ms"), wxT("s"), false);
            }
        }
    }
    else if (result == wxT("0ns"))
        result = wxT("0s");
    return result;
}

wxString WxTimeDuration::ToString(WxTimeDurationUnit unit) const
{
    wxString s;

    switch (unit)
    {
        case WxTimeDurationUnit::NANOSECONDS: s << ToNanoseconds(); s += wxT("ns"); break;
        case WxTimeDurationUnit::MICROSECONDS: s << ToMicroseconds(); s += wxT("us"); break;
        case WxTimeDurationUnit::MILLISECONDS: s << ToMilliseconds(); s += wxT("ms"); break;
        case WxTimeDurationUnit::SECONDS: s << ToSeconds(); s += wxT("s"); break;
        case WxTimeDurationUnit::MINUTES: s << ToMinutes(); s += wxT("m"); break;
        case WxTimeDurationUnit::HOURS: s << ToHours(); s += wxT("h"); break;
        default: break;
    }

    return s;
}

WxTimeDuration WxTimeDuration::operator + (const WxTimeDuration& other) const
{
    WxTimeDuration result;
    result.value = this->value + other.value;
    return result;
}

WxTimeDuration WxTimeDuration::operator - (const WxTimeDuration& other) const
{
    WxTimeDuration result;
    result.value = this->value - other.value;
    return result;
}

WxTimeDuration WxTimeDuration::operator * (const WxTimeDuration& other) const
{
    WxTimeDuration result;
    result.value = this->value * other.value;
    return result;
}

WxTimeDuration WxTimeDuration::operator / (const WxTimeDuration& other) const
{
    WxTimeDuration result;
    result.value = this->value / other.value;
    return result;
}

WxTimeDuration WxTimeDuration::Nanoseconds(int64_t value)
{
    return reinterpret_cast<WxTimeDuration&>(value);
}

WxTimeDuration WxTimeDuration::Microseconds(int64_t value)
{
    return Nanoseconds(value * 1000ll);
}

WxTimeDuration WxTimeDuration::Milliseconds(int64_t value)
{
    return Nanoseconds(value * 1000000ll);
}

WxTimeDuration WxTimeDuration::Seconds(int64_t value)
{
    return Nanoseconds(value * 1000000000ll);
}

WxTimeDuration WxTimeDuration::Seconds(int value)
{
    return Seconds(static_cast<int64_t>(value));
}

WxTimeDuration WxTimeDuration::Seconds(double value, WxTimeDurationUnit resolution)
{
    switch (resolution)
    {
        case WxTimeDurationUnit::NANOSECONDS: return Nanoseconds(static_cast<int64_t>(value * 1000000000.0)); //Unlikely
        case WxTimeDurationUnit::MICROSECONDS: return Microseconds(static_cast<int64_t>(value * 1000000.0));
        case WxTimeDurationUnit::MILLISECONDS: return Milliseconds(static_cast<int64_t>(value * 1000.0));
        case WxTimeDurationUnit::SECONDS: return Seconds(static_cast<int64_t>(value));
        default: return GetZero();
    }
}

WxTimeDuration WxTimeDuration::Seconds(float value, WxTimeDurationUnit resolution)
{
    switch (resolution)
    {
        case WxTimeDurationUnit::NANOSECONDS: return Nanoseconds(static_cast<int64_t>(value * 1000000000.0f)); //Unlikely
        case WxTimeDurationUnit::MICROSECONDS: return Microseconds(static_cast<int64_t>(value * 1000000.0f)); //Unlikely
        case WxTimeDurationUnit::MILLISECONDS: return Milliseconds(static_cast<int64_t>(value * 1000.0f));
        case WxTimeDurationUnit::SECONDS: return Seconds(static_cast<int64_t>(value));
        default: return GetZero();
    }
}

WxTimeDuration WxTimeDuration::Minutes(int64_t value)
{
    return Nanoseconds(value * 1000000000ll * 60ll);
}

WxTimeDuration WxTimeDuration::Hours(int64_t value)
{
    return Nanoseconds(value * 1000000000ll * 60ll * 60ll);
}

const WxTimeDuration& WxTimeDuration::GetZero()
{
    static const WxTimeDuration zero = Nanoseconds(0);
    return zero;
}

#define PARSE_DURATION(unitName, generatorFunction) \
    {\
        auto integerString = s;\
        integerString.Replace(unitName, wxEmptyString);\
        \
        int64_t integerValue;\
        s.ToLongLong(&integerValue);\
        timeDuration = generatorFunction(integerValue);\
    }

void WxTimeDuration::Parse(WxTimeDuration& timeDuration, const wxString& s, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (s.empty())
        FINJIN_WX_SET_ERROR(error, wxT("Failed to parse empty time duration."));

    else if (s.StartsWith(wxT("-")))
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to parse non-negative time duration. Negative specifier in '%s'."), s.wx_str()));

    else if (s.EndsWith(wxT("nanoseconds")))
        PARSE_DURATION(wxT("nanoseconds"), Nanoseconds)
    else if (s.EndsWith(wxT("nanosecond")))
        PARSE_DURATION(wxT("nanosecond"), Nanoseconds)

    else if (s.EndsWith(wxT("microseconds")))
        PARSE_DURATION(wxT("microseconds"), Microseconds)
    else if (s.EndsWith(wxT("microsecond")))
        PARSE_DURATION(wxT("microsecond"), Microseconds)

    else if (s.EndsWith(wxT("milliseconds")))
        PARSE_DURATION(wxT("milliseconds"), Milliseconds)
    else if (s.EndsWith(wxT("millisecond")))
        PARSE_DURATION(wxT("millisecond"), Milliseconds)

    else if (s.EndsWith(wxT("hours")))
        PARSE_DURATION(wxT("hours"), Hours)
    else if (s.EndsWith(wxT("hour")))
        PARSE_DURATION(wxT("hour"), Hours)

    else if (s.EndsWith(wxT("minutes")))
        PARSE_DURATION(wxT("minutes"), Minutes)
    else if (s.EndsWith(wxT("minute")))
        PARSE_DURATION(wxT("minute"), Minutes)

    else if (s.EndsWith(wxT("seconds")))
        PARSE_DURATION(wxT("seconds"), Seconds)
    else if (s.EndsWith(wxT("second")))
        PARSE_DURATION(wxT("second"), Seconds)

    else if (s.EndsWith(wxT("ns")))
        PARSE_DURATION(wxT("ns"), Nanoseconds)
    else if (s.EndsWith(wxT("us")))
        PARSE_DURATION(wxT("us"), Microseconds)
    else if (s.EndsWith(wxT("ms")))
        PARSE_DURATION(wxT("ms"), Milliseconds)
    else if (s.EndsWith(wxT("h")))
        PARSE_DURATION(wxT("h"), Hours)
    else if (s.EndsWith(wxT("m")))
        PARSE_DURATION(wxT("m"), Minutes)
    else if (s.EndsWith(wxT("s")))
        PARSE_DURATION(wxT("s"), Seconds)

    else
        FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to parse non-negative time duration. Unrecognized or unspecified time unit in '%s'."), s.wx_str()));
}

WxTimeDuration WxTimeDuration::Parse(const wxString& s, WxTimeDuration defaultValue)
{
    FINJIN_WX_DECLARE_ERROR(error);
    WxTimeDuration parsed;
    Parse(parsed, s, error);
    return error ? defaultValue : parsed;
}
