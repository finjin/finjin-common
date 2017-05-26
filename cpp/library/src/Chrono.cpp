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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Convert.hpp"
#include <chrono>

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define PARSE_TIME_DURATION(unitName, generatorFunction) \
    { \
        auto integerString = s; \
        integerString.pop_back(unitName); \
        \
        uint64_t integerValue; \
        Convert::ToNumber(integerValue, integerString, error); \
        if (error) \
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse integer %1% time duration from '%2%'.", unitName, integerString)); \
        else \
            timeDuration = generatorFunction(integerValue); \
    }


//Local functions---------------------------------------------------------------
template <typename T>
void ParseTimeDuration(TimeDuration& timeDuration, const T& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (s.empty())
        FINJIN_SET_ERROR(error, "Failed to parse empty time duration.");

    else if (s.StartsWith("-"))
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse non-negative time duration. Negative specifier in '%1%'.", s));

    else if (s.EndsWith("nanoseconds"))
        PARSE_TIME_DURATION("nanoseconds", TimeDuration::Nanoseconds)
    else if (s.EndsWith("nanosecond"))
        PARSE_TIME_DURATION("nanosecond", TimeDuration::Nanoseconds)

    else if (s.EndsWith("microseconds"))
        PARSE_TIME_DURATION("microseconds", TimeDuration::Microseconds)
    else if (s.EndsWith("microsecond"))
        PARSE_TIME_DURATION("microsecond", TimeDuration::Microseconds)

    else if (s.EndsWith("milliseconds"))
        PARSE_TIME_DURATION("milliseconds", TimeDuration::Milliseconds)
    else if (s.EndsWith("millisecond"))
        PARSE_TIME_DURATION("millisecond", TimeDuration::Milliseconds)

    else if (s.EndsWith("hours"))
        PARSE_TIME_DURATION("hours", TimeDuration::Hours)
    else if (s.EndsWith("hour"))
        PARSE_TIME_DURATION("hour", TimeDuration::Hours)

    else if (s.EndsWith("minutes"))
        PARSE_TIME_DURATION("minutes", TimeDuration::Minutes)
    else if (s.EndsWith("minute"))
        PARSE_TIME_DURATION("minute", TimeDuration::Minutes)

    else if (s.EndsWith("seconds"))
        PARSE_TIME_DURATION("seconds", TimeDuration::Seconds)
    else if (s.EndsWith("second"))
        PARSE_TIME_DURATION("second", TimeDuration::Seconds)

    else if (s.EndsWith("ns"))
        PARSE_TIME_DURATION("ns", TimeDuration::Nanoseconds)
    else if (s.EndsWith("us"))
        PARSE_TIME_DURATION("us", TimeDuration::Microseconds)
    else if (s.EndsWith("ms"))
        PARSE_TIME_DURATION("ms", TimeDuration::Milliseconds)
    else if (s.EndsWith("h"))
        PARSE_TIME_DURATION("h", TimeDuration::Hours)
    else if (s.EndsWith("m"))
        PARSE_TIME_DURATION("m", TimeDuration::Minutes)
    else if (s.EndsWith("s"))
        PARSE_TIME_DURATION("s", TimeDuration::Seconds)

    else
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse non-negative time duration. Unrecognized or unspecified time unit in '%1%'.", s));
}


//Implementation----------------------------------------------------------------

//TimeDuration
bool TimeDuration::operator == (const TimeDuration& other) const
{
    return this->value == other.value;
}

bool TimeDuration::operator != (const TimeDuration& other) const
{
    return this->value != other.value;
}

bool TimeDuration::operator < (const TimeDuration& other) const
{
    return this->value < other.value;
}

bool TimeDuration::operator <= (const TimeDuration& other) const
{
    return this->value <= other.value;
}

bool TimeDuration::operator > (const TimeDuration& other) const
{
    return this->value > other.value;
}

bool TimeDuration::operator >= (const TimeDuration& other) const
{
    return this->value >= other.value;
}

bool TimeDuration::IsZero() const
{
    return *this == GetZero();
}

uint64_t TimeDuration::ToNanoseconds() const
{
    return this->value;
}

uint64_t TimeDuration::ToMicroseconds() const
{
    return this->value / 1000ull;
}

uint64_t TimeDuration::ToMilliseconds() const
{
    return this->value / 1000000ull;
}

uint64_t TimeDuration::ToSeconds() const
{
    return this->value / 1000000000ull;
}

uint64_t TimeDuration::ToMinutes() const
{
    return this->value / (1000000000ull * 60ull);
}

uint64_t TimeDuration::ToHours() const
{
    return this->value / (1000000000ull * 60ull * 60ull);
}

double TimeDuration::ToSecondsDouble() const
{
    return static_cast<double>(this->value) / 1000000000.0;
}

SimpleTimeDelta TimeDuration::ToSimpleTimeDelta() const
{
    return static_cast<SimpleTimeDelta>(ToSecondsDouble());
}

Utf8String TimeDuration::ToString() const
{
    auto result = ToString(TimeDurationUnit::NANOSECONDS);
    if (result.EndsWith("000ns"))
    {
        result.pop_back("000ns");
        result.append("us");
        if (result.EndsWith("000us"))
        {
            result.pop_back("000us");
            result.append("ms");
            if (result.EndsWith("000ms"))
            {
                result.pop_back("000ms");
                result.append("s");
            }
        }
    }
    else if (result == "0ns")
        result = "0s";
    return result;
}

Utf8String TimeDuration::ToString(TimeDurationUnit unit) const
{
    Utf8String s;

    switch (unit)
    {
        case TimeDurationUnit::NANOSECONDS: s = Convert::ToString(ToNanoseconds()); s += "ns"; break;
        case TimeDurationUnit::MICROSECONDS: s = Convert::ToString(ToMicroseconds()); s += "us"; break;
        case TimeDurationUnit::MILLISECONDS: s = Convert::ToString(ToMilliseconds()); s += "ms"; break;
        case TimeDurationUnit::SECONDS: s = Convert::ToString(ToSeconds()); s += "s"; break;
        case TimeDurationUnit::MINUTES: s = Convert::ToString(ToMinutes()); s += "m"; break;
        case TimeDurationUnit::HOURS: s = Convert::ToString(ToHours()); s += "h"; break;
        default: break;
    }

    return s;
}

TimeDuration TimeDuration::operator + (const TimeDuration& other) const
{
    TimeDuration result;
    result.value = this->value + other.value;
    return result;
}

TimeDuration TimeDuration::operator - (const TimeDuration& other) const
{
    TimeDuration result;
    result.value = this->value - other.value;
    return result;
}

TimeDuration TimeDuration::operator * (const TimeDuration& other) const
{
    TimeDuration result;
    result.value = this->value * other.value;
    return result;
}

TimeDuration TimeDuration::operator / (const TimeDuration& other) const
{
    TimeDuration result;
    result.value = this->value / other.value;
    return result;
}

TimeDuration TimeDuration::Nanoseconds(uint64_t value)
{
    return reinterpret_cast<TimeDuration&>(value);
}

TimeDuration TimeDuration::Microseconds(uint64_t value)
{
    return Nanoseconds(value * 1000ull);
}

TimeDuration TimeDuration::Milliseconds(uint64_t value)
{
    return Nanoseconds(value * 1000000ull);
}

TimeDuration TimeDuration::Seconds(uint64_t value)
{
    return Nanoseconds(value * 1000000000ull);
}

TimeDuration TimeDuration::Seconds(double value, TimeDurationUnit resolution)
{
    switch (resolution)
    {
        case TimeDurationUnit::NANOSECONDS: return Nanoseconds(static_cast<uint64_t>(value * 1000000000.0)); //Unlikely
        case TimeDurationUnit::MICROSECONDS: return Microseconds(static_cast<uint64_t>(value * 1000000.0));
        case TimeDurationUnit::MILLISECONDS: return Milliseconds(static_cast<uint64_t>(value * 1000.0));
        case TimeDurationUnit::SECONDS: return Seconds(static_cast<uint64_t>(value));
        default: return GetZero();
    }
}

TimeDuration TimeDuration::Seconds(float value, TimeDurationUnit resolution)
{
    switch (resolution)
    {
        case TimeDurationUnit::NANOSECONDS: return Nanoseconds(static_cast<uint64_t>(value * 1000000000.0f)); //Unlikely
        case TimeDurationUnit::MICROSECONDS: return Microseconds(static_cast<uint64_t>(value * 1000000.0f)); //Unlikely
        case TimeDurationUnit::MILLISECONDS: return Milliseconds(static_cast<uint64_t>(value * 1000.0f));
        case TimeDurationUnit::SECONDS: return Seconds(static_cast<uint64_t>(value));
        default: return GetZero();
    }
}

TimeDuration TimeDuration::Minutes(uint64_t value)
{
    return Nanoseconds(value * 1000000000ull * 60ull);
}

TimeDuration TimeDuration::Hours(uint64_t value)
{
    return Nanoseconds(value * 1000000000ull * 60ull * 60ull);
}

const TimeDuration& TimeDuration::GetZero()
{
    static const TimeDuration zero = Nanoseconds(0);
    return zero;
}

void TimeDuration::Parse(TimeDuration& timeDuration, const char* s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView sView(s);
    ParseTimeDuration(timeDuration, sView, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TimeDuration::Parse(TimeDuration& timeDuration, const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParseTimeDuration(timeDuration, s, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void TimeDuration::Parse(TimeDuration& timeDuration, const Utf8StringView& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParseTimeDuration(timeDuration, s, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

TimeDuration TimeDuration::Parse(const char* s, TimeDuration defaultValue)
{
    Utf8StringView sView(s);

    FINJIN_DECLARE_ERROR(error);
    TimeDuration parsed;
    Parse(parsed, sView, error);
    return error ? defaultValue : parsed;
}

TimeDuration TimeDuration::Parse(const Utf8String& s, TimeDuration defaultValue)
{
    FINJIN_DECLARE_ERROR(error);
    TimeDuration parsed;
    Parse(parsed, s, error);
    return error ? defaultValue : parsed;
}

TimeDuration TimeDuration::Parse(const Utf8StringView& s, TimeDuration defaultValue)
{
    FINJIN_DECLARE_ERROR(error);
    TimeDuration parsed;
    Parse(parsed, s, error);
    return error ? defaultValue : parsed;
}

//DateTime
bool DateTime::operator == (const DateTime& other) const
{
    return this->value == other.value;
}

bool DateTime::operator != (const DateTime& other) const
{
    return this->value != other.value;
}

bool DateTime::operator < (const DateTime& other) const
{
    return this->value < other.value;
}

bool DateTime::operator <= (const DateTime& other) const
{
    return this->value <= other.value;
}

bool DateTime::operator > (const DateTime& other) const
{
    return this->value > other.value;
}

bool DateTime::operator >= (const DateTime& other) const
{
    return this->value >= other.value;
}

TimeDuration DateTime::operator - (const DateTime& other) const
{
    auto result = std::chrono::system_clock::from_time_t(this->value) - std::chrono::system_clock::from_time_t(other.value);
    return TimeDuration::Nanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(result).count());
}

DateTime DateTime::operator + (const TimeDuration& duration) const
{
    auto result = *this;
    return result += duration;
}

DateTime& DateTime::operator += (const TimeDuration& duration)
{
    this->value = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(this->value) + std::chrono::microseconds(duration.ToMicroseconds()));
    return *this;
}

DateTime DateTime::operator - (const TimeDuration& duration) const
{
    auto result = *this;
    return result -= duration;
}

DateTime& DateTime::operator -= (const TimeDuration& duration)
{
    this->value = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(this->value) - std::chrono::microseconds(duration.ToMicroseconds()));
    return *this;
}

DateTime DateTime::NowUtc()
{
    auto nowLocal = std::chrono::system_clock::now();
    auto nowLocalTimeT = std::chrono::system_clock::to_time_t(nowLocal);
    tm nowUtcTm;
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    gmtime_s(&nowUtcTm, &nowLocalTimeT);
#else
    gmtime_r(&nowLocalTimeT, &nowUtcTm);
#endif
    auto nowUtcTimeT = mktime(&nowUtcTm);
    return reinterpret_cast<DateTime&>(nowUtcTimeT);
}

Utf8String DateTime::ToLogFileName() const
{
    tm nowUtcTm;
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    localtime_s(&nowUtcTm, &this->value);
#else
    localtime_r(&this->value, &nowUtcTm);
#endif

    char buff[100];
    strftime(buff, 100, "%Y-%m-%d-%H_%M_%S", &nowUtcTm);
    return buff;
}

Utf8String DateTime::ToIso8601() const
{
    tm nowUtcTm;
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    localtime_s(&nowUtcTm, &this->value);
#else
    localtime_r(&this->value, &nowUtcTm);
#endif

    char buff[100];
    strftime(buff, 100, "%Y-%m-%dT%H:%M:%S.0Z", &nowUtcTm);
    return buff;
}

time_t DateTime::ToUnixTimestamp() const
{
    return this->value;
}

DateTime DateTime::FromUnixTimestap(time_t value)
{
    DateTime result;
    result.value = value;
    return result;
}

void DateTime::ParseIso8601(DateTime& result, const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    int year, month, day, hour, minute, second;
    sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.0Z", &year, &month, &day, &hour, &minute, &second);
    tm timeTm;
    timeTm.tm_sec = second;
    timeTm.tm_min = minute;
    timeTm.tm_hour = hour - 1;
    timeTm.tm_mday = day;
    timeTm.tm_mon = month - 1;
    timeTm.tm_year = year - 1900;
    timeTm.tm_wday = -1;
    timeTm.tm_yday = -1;
    timeTm.tm_isdst = 0;
    result.value = mktime(&timeTm);
    if (result.value == -1)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse year/month/day hour:minute:ssecond date '%1%'.", s));
        return;
    }
}

//HighResolutionTimeStamp
HighResolutionTimeStamp::HighResolutionTimeStamp()
{
    this->timepoint = 0;
}

HighResolutionTimeStamp::HighResolutionTimeStamp(uint64_t timepoint)
{
    this->timepoint = timepoint;
}

bool HighResolutionTimeStamp::IsZero() const
{
    return this->timepoint == 0;
}

TimeDuration HighResolutionTimeStamp::operator - (const HighResolutionTimeStamp& other) const
{
    //This implementation assumes both time stamps were taken from the same clock

    auto elapsedTimeDuration = this->timepoint - other.timepoint;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    return TimeDuration::Nanoseconds(elapsedTimeDuration * 1000000000ull / this->highPerformanceFrequency.QuadPart);
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    return TimeDuration::Nanoseconds(elapsedTimeDuration * this->timebaseInfo.numer / this->timebaseInfo.denom);
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    return TimeDuration::Nanoseconds(elapsedTimeDuration);
#endif
}

//HighResolutionClock
HighResolutionClock::HighResolutionClock()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    QueryPerformanceFrequency(&this->highPerformanceFrequency);
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    mach_timebase_info(&this->timebaseInfo);
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &this->clockResolution);
#endif
}

HighResolutionTimeStamp HighResolutionClock::Now()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    HighResolutionTimeStamp result(0);
    result.highPerformanceFrequency = this->highPerformanceFrequency;
    LARGE_INTEGER largeCount;
    if (QueryPerformanceCounter(&largeCount))
        result.timepoint = largeCount.QuadPart;
    return result;
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    HighResolutionTimeStamp result(mach_absolute_time());
    result.timebaseInfo = this->timebaseInfo;
    return result;
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    timespec currentTime;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &currentTime);
    return HighResolutionTimeStamp(static_cast<uint64_t>(currentTime.tv_sec) * 1000000000ull + static_cast<uint64_t>(currentTime.tv_nsec));
#endif
}
