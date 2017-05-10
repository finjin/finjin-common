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


//Preliminary macros------------------------------------------------------------
#if defined(_MSC_VER)
    #pragma warning(disable: 4521) //The class has multiple copy constructors of a single type
    #pragma warning(disable: 4355) //'this' : used in base member initializer list

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN 1
    #endif
#endif


//Includes----------------------------------------------------------------------
#include <cstddef>
#include <cassert>
#include <cmath>
#include <math.h>
#include <cstdint>
#include <memory>
#include <memory.h>
#include <string.h>
#include <array>
#include <algorithm>
#include <limits>
#include <atomic>
#include <functional>
#include <random>


//Macros------------------------------------------------------------------------
#define FINJIN_COMMON_LIBRARY_API

//Platform identifiers---------------

//For FINJIN_TARGET_CPU
#define FINJIN_TARGET_CPU_ARM 1
#define FINJIN_TARGET_CPU_INTEL 2

//For FINJIN_TARGET_CPU_BITS
//FINJIN_TARGET_CPU_BITS will be defined to be 32 or 64

//For FINJIN_TARGET_PLATFORM
#define FINJIN_TARGET_PLATFORM_WINDOWS_WIN32 1
#define FINJIN_TARGET_PLATFORM_WINDOWS_UWP 2
#define FINJIN_TARGET_PLATFORM_IOS 4
#define FINJIN_TARGET_PLATFORM_WATCHOS 5
#define FINJIN_TARGET_PLATFORM_TVOS 6
#define FINJIN_TARGET_PLATFORM_MACOS 7
#define FINJIN_TARGET_PLATFORM_LINUX 8


//Detect target CPU/platform---------------
#if defined(_WIN32)
    #define FINJIN_TARGET_PLATFORM_IS_WINDOWS 1

    #ifdef _M_ARM
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_ARM
    #else
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_INTEL
    #endif

    #if defined(_WIN64) || defined(__x86_64__) || defined(_M_X64)
        #define FINJIN_TARGET_CPU_BITS 64
    #else
        #define FINJIN_TARGET_CPU_BITS 32
    #endif

    #if defined(WINAPI_FAMILY_PARTITION)
        #define FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP 1
        #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_WINDOWS_UWP
    #else
        #define FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP 0
        #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_WINDOWS_WIN32
    #endif
#elif __APPLE__
    #include "TargetConditionals.h"

    #define FINJIN_TARGET_PLATFORM_IS_APPLE 1

    #if TARGET_OS_SIMULATOR
        #define FINJIN_TARGET_PLATFORM_SIMULATED 1
    #endif

    #if __LP64__
        #define FINJIN_TARGET_CPU_BITS 64
    #else
        #define FINJIN_TARGET_CPU_BITS 32
    #endif

    #if TARGET_OS_TV
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_ARM
        #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_TVOS
    #elif TARGET_OS_IPHONE
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_ARM
        #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_IOS
    #elif TARGET_OS_WATCH
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_ARM
        #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_WATCHOS
    #elif TARGET_OS_OSX
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_INTEL
        #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_MACOS
    #else
        #error Unknown platform
    #endif

    #if __has_feature(objc_arc)
        #define FINJIN_APPLE_WEAK __weak
    #else
        #define FINJIN_APPLE_WEAK
    #endif
#elif defined(__linux)
    #define FINJIN_TARGET_PLATFORM_IS_LINUX 1

    #if _LP64
        #define FINJIN_TARGET_CPU_BITS 64
    #else
        #define FINJIN_TARGET_CPU_BITS 32
    #endif

    #if __arm__ || __APCS_32__
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_ARM
    #elif __i386__ || __x86_64
        #define FINJIN_TARGET_CPU FINJIN_TARGET_CPU_INTEL
    #else
        #error Unknown CPU type
    #endif

    #define FINJIN_TARGET_PLATFORM FINJIN_TARGET_PLATFORM_LINUX
#else
    #error Unknown compiler
#endif


//Set up platform defaults---------------

//Android?
#if defined(__ANDROID__)
    #define FINJIN_TARGET_PLATFORM_IS_ANDROID 1 //Note: FINJIN_TARGET_PLATFORM_IS_LINUX will also be 1
#else
    #define FINJIN_TARGET_PLATFORM_IS_ANDROID 0
#endif

//Simulated environment?
#if !defined(FINJIN_TARGET_PLATFORM_SIMULATED)
    #define FINJIN_TARGET_PLATFORM_SIMULATED 0
#endif

//Windows/Universal Windows Platform?
#if !defined(FINJIN_TARGET_PLATFORM_IS_WINDOWS)
    #define FINJIN_TARGET_PLATFORM_IS_WINDOWS 0
    #define FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP 0
#endif

//Apple?
#if !defined(FINJIN_TARGET_PLATFORM_IS_APPLE)
    #define FINJIN_TARGET_PLATFORM_IS_APPLE 0
#endif

//Linux?
#if !defined(FINJIN_TARGET_PLATFORM_IS_LINUX)
    #define FINJIN_TARGET_PLATFORM_IS_LINUX 0
#endif

//Desktop/server/mobile?
//This is just a hint, since there is some overlap with respect to what's considered desktop/server/mobile
#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_WINDOWS_WIN32 || \
    FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS || \
    (FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_LINUX && !FINJIN_TARGET_PLATFORM_IS_ANDROID)
    #define FINJIN_TARGET_PLATFORM_IS_DESKTOP_OR_SERVER 1
    #define FINJIN_TARGET_PLATFORM_IS_MOBILE 0
#else
    #define FINJIN_TARGET_PLATFORM_IS_DESKTOP_OR_SERVER 0
    #define FINJIN_TARGET_PLATFORM_IS_MOBILE 1
#endif


//Thread-local---------------
//To ensure cross-platform compatibility, use only plain data types with FINJIN_THREAD_LOCAL
#if __APPLE__
    #define FINJIN_THREAD_LOCAL __thread
#else
    #define FINJIN_THREAD_LOCAL thread_local
#endif


//Debug macro---------------
#if defined(_DEBUG)
    #define FINJIN_DEBUG 1 //_DEBUG is typically defined by itself to indicate a debug build
#elif defined(DEBUG)
    #define FINJIN_DEBUG DEBUG //DEBUG is typically defined with a 0 or 1 to indicate a debug build
#elif !defined(NDEBUG)
    #define FINJIN_DEBUG 1
#else
    #define FINJIN_DEBUG 0
#endif


//Function name------------
#ifndef __FUNCTION_NAME__
    #if defined(_MSC_VER)
        #define __FUNCTION_NAME__ __FUNCTION__
    #else
        #define __FUNCTION_NAME__ __func__
    #endif
#endif


//Compiler class--------------
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    #define FINJIN_OLD_COMPILER 0
#else
    #define FINJIN_OLD_COMPILER 1
#endif


//SSE functionality-------------
#if !defined(FINJIN_ENABLE_SSE)
    #define FINJIN_ENABLE_SSE 1
#endif


//Some utilities------------
#define FINJIN_OFFSET_OF(theClass, theMember) ((size_t)(&((theClass*)0)->theMember))

#define FINJIN_COUNT_OF(x) (sizeof(x)/sizeof((x)[0]))

#define FINJIN_SAFE_RELEASE(p) {if (p) {(p)->Release(); (p) = nullptr;}}

#define FINJIN_ZERO_ITEM(item) memset(&(item), 0, sizeof(item))
#define FINJIN_ZERO_MEMORY(mem, byteCount) memset(mem, 0, byteCount)
#define FINJIN_COPY_MEMORY(dest, source, byteCount) memcpy(dest, source, byteCount)

#define FINJIN_FOURCC(a, b, c, d) ((static_cast<uint32_t>(d) << 24) | (static_cast<uint32_t>(c) << 16) | (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a))
#define FINJIN_FOURCC_BIG_ENDIAN(a, b, c, d) FINJIN_FOURCC(d, c, b, a)

#define FINJIN_SIGNATURE_FOURCC FINJIN_FOURCC('f', 'i', 'n', 'j')

#define FINJIN_CALLER_PARAMETERS_DECLARATION const char* fileName, const char* functionName, int line //In the signature for Error::SetError() and other methods
#define FINJIN_CALLER_PARAMETERS fileName, functionName, line //Allows error parameters in a method to be passed to another method
#define FINJIN_CALLER_ARGUMENTS __FILE__, __FUNCTION_NAME__, __LINE__ //Passed into Error::SetError() and other methods


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class DomainInformation;
    class Error;
    class JobSystem;
    class StandardPaths;
    class UsageFormatter;
    class UserInformation;
    class VirtualFileSystem;

    typedef int16_t Int16BigEndian;
    typedef int16_t Int16LittleEndian;

    typedef int32_t Int32BigEndian;
    typedef int32_t Int32LittleEndian;

    typedef int64_t Int64BigEndian;
    typedef int64_t Int64LittleEndian;

    typedef uint16_t UInt16BigEndian;
    typedef uint16_t UInt16LittleEndian;

    typedef uint32_t UInt32BigEndian;
    typedef uint32_t UInt32LittleEndian;

    typedef uint64_t UInt64BigEndian;
    typedef uint64_t UInt64LittleEndian;

    struct CommonConstants
    {
        enum { MAX_FIBERS = 128 };

        enum { MAX_CPU_GROUPS = 1 };
        enum { MAX_CPUS_PER_GROUP = 64 };
        enum { MAX_CPUS = MAX_CPU_GROUPS * MAX_CPUS_PER_GROUP };
        enum { MAX_CPU_CACHE_SHARING = 8 };

        enum { MAX_ROOT_FILE_SYSTEM_ENTRIES = 8 };

        enum { MAX_TYPE_DESCRIPTIONS = 1024 }; //Maximum number of classes created with FINJIN_DECLARE_TYPE_DESCRIPTION and friends

        enum { FILE_TEMP_BUFFER_SIZE = 1024 };

        enum { MAX_DATA_CHUNK_DEPTH = 16 };

        enum { MAX_ERROR_CALL_STACK_DEPTH = 50 };
    };

    enum class MemoryResizeStrategy
    {
        LIMIT,
        REALLOCATE
    };

    struct ChunkNameConstants
    {
        enum { NAME_MAX_LENGTH = 43 };
        enum { ID_MAX_LENGTH = 36 };
        enum { MAX_LENGTH = NAME_MAX_LENGTH + 1 + ID_MAX_LENGTH };
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    inline void DoesNothingToAvoidCompilerWarning()
    {
        //Called by some utility macros that expand to "nothing"
    }

    template <typename T, typename V, typename W>
    inline void Limit(T& value, V minValue, W maxValue)
    {
        if (value < static_cast<T>(minValue))
            value = static_cast<T>(minValue);
        if (value > static_cast<T>(maxValue))
            value = static_cast<T>(maxValue);
    }

    template <typename T, typename V, typename W>
    inline T Limited(T value, V minValue, W maxValue)
    {
        if (value < static_cast<T>(minValue))
            return static_cast<T>(minValue);
        if (value > static_cast<T>(maxValue))
            return static_cast<T>(maxValue);
        return value;
    }

    /**
     * Scales a normalized value to fall within a range
     * @param value [in] - Normalized value, from 0 to 1
     * @param rangeStart [in] - The start of the range
     * @param rangeEnd [in] - The end of the range
     * @return The scaled value is returned
     */
    inline float Scaled(float value, float rangeStart, float rangeEnd)
    {
        return rangeStart + (rangeEnd - rangeStart) * value;
    }
    inline double Scaled(double value, double rangeStart, double rangeEnd)
    {
        return rangeStart + (rangeEnd - rangeStart) * value;
    }

    /**
     * Normalizes a value to fall within a range
     * @param value [in] - Non-normalized value. Should be in [rangeStart, rangeEnd]
     * @param rangeStart [in] - The start of the range
     * @param rangeEnd [in] - The end of the range
     * @return The normalized value is returned
     */
    inline float Normalized(float value, float rangeStart, float rangeEnd)
    {
        return (value - rangeStart) / (rangeEnd - rangeStart);
    }
    inline double Normalized(double value, double rangeStart, double rangeEnd)
    {
        return (value - rangeStart) / (rangeEnd - rangeStart);
    }

    template <typename T>
    inline T GetSign(T value)
    {
        return value < 0 ? (T)-1 : (T)1;
    }

    template <typename T>
    inline T GetAbs(T value)
    {
        return value < 0 ? -value : value;
    }

    template <>
    inline uint8_t GetAbs(uint8_t value)
    {
        return value;
    }

    template <>
    inline uint16_t GetAbs(uint16_t value)
    {
        return value;
    }

    template <>
    inline uint32_t GetAbs(uint32_t value)
    {
        return value;
    }

    template <>
    inline uint64_t GetAbs(uint64_t value)
    {
        return value;
    }

    template <typename T>
    inline T GetOdd(T value)
    {
        return value | (T)1;
    }

    template <typename T>
    inline float RoundToFloat(T value)
    {
        return std::floor(value + .5f);
    }

    template <>
    inline float RoundToFloat(float value)
    {
        return value;
    }

    template <typename T>
    inline double RoundToDouble(T value)
    {
        return std::floor(value + .5);
    }

    template <>
    inline double RoundToDouble(double value)
    {
        return value;
    }

    inline int RoundToInt(double value)
    {
        return static_cast<int>(value + .5);
    }

    inline int64_t RoundToInt64(double value)
    {
        return static_cast<int64_t>(value + .5);
    }

    inline uint64_t RoundToUInt64(double value)
    {
        return static_cast<uint64_t>(value + .5);
    }

    template <typename T>
    inline void RoundFractionalInt(T& whole, T& fractional, int maxDigits)
    {
        T maxValue = 1;
        for (int i = 0; i < maxDigits; i++)
            maxValue *= 10;
        maxValue--;

        while (fractional > maxValue)
        {
            auto lowestDigit = fractional % 10;
            auto carry = lowestDigit > 4 ? 1 : 0;
            fractional /= 10;

            if (fractional < maxValue)
                fractional += carry;
            if (fractional == maxValue)
            {
                whole = GetSign(whole) * (GetAbs(whole) + (T)1);
                fractional = 0;
            }
        }
    }

    template <typename T>
    inline bool IsInRange(const T& value, const T& minValue, const T& maxValue)
    {
        return value >= minValue && value <= maxValue;
    }

} }
