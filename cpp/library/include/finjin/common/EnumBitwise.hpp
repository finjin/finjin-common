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
#include <type_traits>


//Macros-----------------------------------------------------------------------
#define FINJIN_ENUM_BITWISE_OPERATIONS(enumName)\
    inline enumName operator & (enumName a, enumName b) \
    { \
        return enumName(std::underlying_type<enumName>::type(a) & std::underlying_type<enumName>::type(b)); \
    } \
    inline enumName& operator &= (enumName& a, enumName b) \
    { \
        a = a & b; \
        return a; \
    } \
    inline enumName operator | (enumName a, enumName b) \
    { \
        return enumName(std::underlying_type<enumName>::type(a) | std::underlying_type<enumName>::type(b)); \
    } \
    inline enumName& operator |= (enumName& a, enumName b) \
    { \
        a = a | b; \
        return a; \
    }\
    inline enumName operator ^ (enumName a, enumName b) \
    { \
        return enumName(std::underlying_type<enumName>::type(a) ^ std::underlying_type<enumName>::type(b)); \
    } \
    inline enumName& operator ^= (enumName& a, enumName b) \
    { \
        a = a ^ b; \
        return a; \
    } \
    inline void SetAll(enumName& a) \
    { \
        a = static_cast<enumName>(-1); \
    } \
    inline bool AllSet(enumName a) \
    { \
        return a == static_cast<enumName>(-1); \
    } \
    inline bool AnySet(enumName a) \
    { \
        return std::underlying_type<enumName>::type(a) != 0; \
    } \
    inline bool NoneSet(enumName a) \
    { \
        return std::underlying_type<enumName>::type(a) == 0; \
    } \
    inline std::underlying_type<enumName>::type ToNumber(enumName a) \
    { \
        return std::underlying_type<enumName>::type(a); \
    }

//Use this at global scope with the fully qualified name of the enum. 
//For example: FINJIN_ENUM_BITWISE_STD_OPERATIONS(Finjin::Engine::PovDirection)
#define FINJIN_ENUM_BITWISE_STD_OPERATIONS(enumName)\
    namespace std \
    { \
        template <> \
        struct hash<enumName> \
        { \
            size_t operator () (enumName value) const \
            { \
                hash<std::underlying_type<enumName>::type> h; \
                return h(std::underlying_type<enumName>::type(value)); \
            } \
        }; \
    }
