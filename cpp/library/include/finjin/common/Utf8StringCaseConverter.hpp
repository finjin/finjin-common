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
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class Utf8StringCaseConverter
    {
    public:
        Utf8StringCaseConverter(const Utf8StringCaseConverter& other) = delete;
        Utf8StringCaseConverter& operator = (const Utf8StringCaseConverter& other) = delete;

        Utf8StringCaseConverter(Utf8StringCaseConverter&& other) = delete;
        Utf8StringCaseConverter& operator = (Utf8StringCaseConverter&& other) = delete;

    public:
        Utf8StringCaseConverter();

        ValueOrError<void> ToLower(Utf8String& result, const char* source) const;
        ValueOrError<void> ToLower(Utf8String& result, const Utf8String& source) const;

        ValueOrError<void> ToUpper(Utf8String& result, const char* source) const;
        ValueOrError<void> ToUpper(Utf8String& result, const Utf8String& source) const;

        int CompareNoCase(const char* a, const char* b) const;

        bool EqualsNoCase(const char* a, const char* b) const;

    private:
    #if !FINJIN_TARGET_PLATFORM_IS_WINDOWS
        std::locale genericUtf8Locale;
    #endif
    };

} }
