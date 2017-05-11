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
#include "finjin/common/Utf8StringFormatter.hpp"


//Macros------------------------------------------------------------------------
#if !defined(FINJIN_ENABLE_DEBUG_LOG)
    #if FINJIN_DEBUG
        #define FINJIN_ENABLE_DEBUG_LOG 1
    #else
        #define FINJIN_ENABLE_DEBUG_LOG 0 //In some cases it might still be useful to enable this during development
    #endif
#endif

#if FINJIN_ENABLE_DEBUG_LOG && FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
    #define FINJIN_DEBUG_LOG_ERROR(...) \
    { \
        auto __s = Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__); \
        OutputDebugStringW(L"finjin-debug-log (error): "); \
        Finjin::Common::Utf8StringToWideString __sWide(__s); \
        OutputDebugStringW(__sWide.c_str()); \
        OutputDebugStringW(L"\n"); \
        std::cout << __s << std::endl; \
    }
    #define FINJIN_DEBUG_LOG_WARNING(...) \
    { \
        auto __s = Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__); \
        OutputDebugStringW(L"finjin-debug-log (warning): "); \
        Finjin::Common::Utf8StringToWideString __sWide(__s); \
        OutputDebugStringW(__sWide.c_str()); \
        OutputDebugStringW(L"\n"); \
        std::cout << __s << std::endl; \
    }
    #define FINJIN_DEBUG_LOG_INFO(...) \
    { \
        auto __s = Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__); \
        OutputDebugStringW(L"finjin-debug-log (info): "); \
        Finjin::Common::Utf8StringToWideString __sWide(__s); \
        OutputDebugStringW(__sWide.c_str()); \
        OutputDebugStringW(L"\n"); \
        std::cout << __s << std::endl; \
    }
#elif FINJIN_ENABLE_DEBUG_LOG && FINJIN_TARGET_PLATFORM_IS_ANDROID
    #include <android/log.h>
    #define FINJIN_DEBUG_LOG_ERROR(...) (__android_log_print(ANDROID_LOG_ERROR, "finjin-debug-log", Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__).c_str()))
    #define FINJIN_DEBUG_LOG_WARNING(...) (__android_log_print(ANDROID_LOG_WARN, "finjin-debug-log", Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__).c_str()))
    #define FINJIN_DEBUG_LOG_INFO(...) (__android_log_print(ANDROID_LOG_INFO, "finjin-debug-log", Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__).c_str()))
#elif FINJIN_ENABLE_DEBUG_LOG && (FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM_IS_APPLE)
    #define FINJIN_DEBUG_LOG_ERROR(...) { std::cout << Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__) << std::endl; }
    #define FINJIN_DEBUG_LOG_WARNING(...) { std::cout << Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__) << std::endl; }
    #define FINJIN_DEBUG_LOG_INFO(...) { std::cout << Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__) << std::endl; }
#else
    #define FINJIN_DEBUG_LOG_ERROR(...) Finjin::Common::DoesNothingToAvoidCompilerWarning();
    #define FINJIN_DEBUG_LOG_WARNING(...) Finjin::Common::DoesNothingToAvoidCompilerWarning();
    #define FINJIN_DEBUG_LOG_INFO(...) Finjin::Common::DoesNothingToAvoidCompilerWarning();
#endif
