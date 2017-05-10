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
#include "finjin/common/Error.hpp"
#include <time.h>

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    #include <mach/mach.h>
    #include <mach/mach_time.h>
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
#else
    #error Platform not defined!
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class TimeDurationUnit
    {
        NANOSECONDS,
        MICROSECONDS,
        MILLISECONDS,
        SECONDS,
        MINUTES,
        HOURS,

        COUNT
    };

    using SimpleTimeDelta = float;
    using SimpleTimeCounter = double;

    /** A time interval. */
    class FINJIN_COMMON_LIBRARY_API TimeDuration
    {
    public:
        bool operator == (const TimeDuration& other) const;
        bool operator != (const TimeDuration& other) const;
        bool operator < (const TimeDuration& other) const;
        bool operator <= (const TimeDuration& other) const;
        bool operator > (const TimeDuration& other) const;
        bool operator >= (const TimeDuration& other) const;

        bool IsZero() const;

        int64_t ToNanoseconds() const;
        int64_t ToMicroseconds() const;
        int64_t ToMilliseconds() const;
        int64_t ToSeconds() const;
        int64_t ToMinutes() const;
        int64_t ToHours() const;

        double ToSecondsDouble() const;
        SimpleTimeDelta ToSimpleTimeDelta() const;

        TimeDuration operator + (const TimeDuration& other) const;
        TimeDuration operator - (const TimeDuration& other) const;
        TimeDuration operator * (const TimeDuration& other) const;
        TimeDuration operator / (const TimeDuration& other) const;

        static TimeDuration Nanoseconds(int64_t value);
        static TimeDuration Microseconds(int64_t value);
        static TimeDuration Milliseconds(int64_t value);
        static TimeDuration Seconds(int64_t value);
        static TimeDuration Seconds(int value);
        static TimeDuration Seconds(double value, TimeDurationUnit resolution);
        static TimeDuration Seconds(float value, TimeDurationUnit resolution);
        static TimeDuration Minutes(int64_t value);
        static TimeDuration Hours(int64_t value);

        static const TimeDuration& Zero();

        Utf8String ToString() const;
        Utf8String ToString(TimeDurationUnit unit) const;

        static void Parse(TimeDuration& timeDuration, const char* s, Error& error);
        static void Parse(TimeDuration& timeDuration, const Utf8String& s, Error& error);
        static void Parse(TimeDuration& timeDuration, const Utf8StringView& s, Error& error);

        static TimeDuration Parse(const char* s, TimeDuration defaultValue = TimeDuration::Zero());
        static TimeDuration Parse(const Utf8String& s, TimeDuration defaultValue = TimeDuration::Zero());
        static TimeDuration Parse(const Utf8StringView& s, TimeDuration defaultValue = TimeDuration::Zero());

    private:
        int64_t value; //Nanoseconds
    };

    /** A date/time. */
    class FINJIN_COMMON_LIBRARY_API DateTime
    {
    public:
        bool operator == (const DateTime& other) const;
        bool operator != (const DateTime& other) const;
        bool operator < (const DateTime& other) const;
        bool operator <= (const DateTime& other) const;
        bool operator > (const DateTime& other) const;
        bool operator >= (const DateTime& other) const;

        TimeDuration operator - (const DateTime& other) const;

        DateTime operator + (const TimeDuration& duration) const;
        DateTime& operator += (const TimeDuration& duration);

        DateTime operator - (const TimeDuration& duration) const;
        DateTime& operator -= (const TimeDuration& duration);

        /**
         * Gets the date/time now.
         */
        static DateTime NowUtc();

        /**
         * Formats the date/time as a file name suitable for use as a log file name.
         * @return The date/time as a file name suitable for use as a log file name.
         */
        Utf8String ToLogFileName() const;

        /**
         * Formats the date/time as an ISO-8601 string.
         * @return The date/time as an ISO-8601 string.
         */
        Utf8String ToIso8601() const;

        time_t ToUnixTimestamp() const;

        static DateTime FromUnixTimestap(time_t value);

        /**
         * Parses the date/time from an ISO-8601 string.
         * @param result [out] The date/time parsed from an ISO-8601 string.
         * @param s [in] The date/time as an ISO-8601 string.
         * @error [out] Error object.
         * @see Error
         */
        static void ParseIso8601(DateTime& result, const Utf8String& s, Error& error);

    private:
        time_t value;
    };

    class FINJIN_COMMON_LIBRARY_API HighResolutionTimeStamp
    {
        friend class HighResolutionClock;

    public:
        HighResolutionTimeStamp();
        HighResolutionTimeStamp(int64_t timepoint);

        bool IsZero() const;

        TimeDuration operator - (const HighResolutionTimeStamp& other) const;

    private:
        int64_t timepoint;
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        LARGE_INTEGER highPerformanceFrequency;
    #elif FINJIN_TARGET_PLATFORM_IS_APPLE
        mach_timebase_info_data_t timebaseInfo;
    #endif
    };

    class FINJIN_COMMON_LIBRARY_API HighResolutionClock
    {
    public:
        HighResolutionClock();

        HighResolutionTimeStamp Now();

    private:
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        LARGE_INTEGER highPerformanceFrequency;
    #elif FINJIN_TARGET_PLATFORM_IS_APPLE
        mach_timebase_info_data_t timebaseInfo;
    #elif FINJIN_TARGET_PLATFORM_IS_LINUX
        timespec clockResolution;
    #endif
    };

} }


//Functions---------------------------------------------------------------------
namespace std
{
    inline ostream& operator << (ostream& os, const Finjin::Common::DateTime& v)
    {
        os << v.ToIso8601();
        return os;
    }
}

namespace Finjin { namespace Common {

    inline float SimpleTimeDeltaToFloat(SimpleTimeDelta value)
    {
        return value;
    }

    inline float SimpleTimeCounterToFloat(SimpleTimeCounter value)
    {
        return static_cast<float>(value);
    }

    template <typename T>
    T SecondsToNanoseconds(int64_t in)
    {
        return static_cast<T>(in) * (T)1000000000;
    }

    template <typename T>
    T SecondsToMicroseconds(int64_t in)
    {
        return static_cast<T>(in) * (T)1000000;
    }

    template <typename T>
    T SecondsToMilliseconds(int64_t in)
    {
        return static_cast<T>(in) * (T)1000;
    }

    template <typename T>
    T NanosecondsToSeconds(int64_t in)
    {
        return static_cast<T>(in) / (T)1000000000;
    }

    template <typename T>
    T MicrosecondsToSeconds(int64_t in)
    {
        return static_cast<T>(in) / (T)1000000;
    }

    template <typename T>
    T MillisecondsToSeconds(int64_t in)
    {
        return static_cast<T>(in) / (T)1000;
    }

} }
