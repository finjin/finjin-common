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
#include "finjin/common/Utf8StringCaseConverter.hpp"
#include <nowide/stackstring.hpp>
#if FINJIN_TARGET_OS_IS_WINDOWS
    #include "finjin/common/WindowsUtilities.hpp"
#endif
#include <boost/locale.hpp>

using namespace Finjin::Common;

using WideningToUtf16Converter = nowide::basic_stackstring<wchar_t, char, Utf8String::STATIC_STRING_LENGTH + 1>;


//Implementation----------------------------------------------------------------
Utf8StringCaseConverter::Utf8StringCaseConverter()
{
#if !FINJIN_TARGET_OS_IS_WINDOWS
    boost::locale::generator gen;
    this->genericUtf8Locale = gen("UTF-8");
#endif
}

ValueOrError<void> Utf8StringCaseConverter::ToLower(Utf8String& result, const char* s) const
{
    result.clear();

    if (s == nullptr || s[0] == 0)
        return ValueOrError<void>();
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    WideningToUtf16Converter widened;
    if (!widened.convert(s))
        return ValueOrError<void>::CreateError();
    
    if (!WindowsUtilities::ConvertString(result, LCMAP_LOWERCASE, widened.c_str(), static_cast<int>(wcslen(widened.c_str()))))
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
#else
    return result.assign(boost::locale::to_lower(s, s + strlen(s), this->genericUtf8Locale).c_str());
#endif
}

ValueOrError<void> Utf8StringCaseConverter::ToLower(Utf8String& result, const Utf8String& s) const
{
    result.clear();

    if (s.empty())
        return ValueOrError<void>();
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    WideningToUtf16Converter widened;
    if (!widened.convert(s.begin(), s.end()))
        return ValueOrError<void>::CreateError();
    
    if (!WindowsUtilities::ConvertString(result, LCMAP_LOWERCASE, widened.c_str(), static_cast<int>(wcslen(widened.c_str()))))
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
#else
    return result.assign(boost::locale::to_lower(s.begin(), s.end(), this->genericUtf8Locale).c_str());
#endif
}

ValueOrError<void> Utf8StringCaseConverter::ToUpper(Utf8String& result, const char* s) const
{
    result.clear();

    if (s == nullptr || s[0] == 0)
        return ValueOrError<void>();
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    WideningToUtf16Converter widened;
    if (!widened.convert(s))
        return ValueOrError<void>::CreateError();
    
    if (!WindowsUtilities::ConvertString(result, LCMAP_UPPERCASE, widened.c_str(), static_cast<int>(wcslen(widened.c_str()))))
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
#else
    return result.assign(boost::locale::to_upper(s, s + strlen(s), this->genericUtf8Locale).c_str());
#endif
}

ValueOrError<void> Utf8StringCaseConverter::ToUpper(Utf8String& result, const Utf8String& s) const
{
    result.clear();

    if (s.empty())
        return ValueOrError<void>();
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    WideningToUtf16Converter widened;
    if (!widened.convert(s.begin(), s.end()))
        return ValueOrError<void>::CreateError();
    
    if (!WindowsUtilities::ConvertString(result, LCMAP_UPPERCASE, widened.c_str(), static_cast<int>(wcslen(widened.c_str()))))
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
#else
    return result.assign(boost::locale::to_upper(s.begin(), s.end(), this->genericUtf8Locale).c_str());
#endif
}

int Utf8StringCaseConverter::CompareNoCase(const char* a, const char* b) const
{
    Utf8String aLower, bLower;    
    ToLower(aLower, a);
    ToLower(bLower, b);
    return aLower.Compare(bLower);
}

bool Utf8StringCaseConverter::EqualsNoCase(const char* a, const char* b) const
{
    return CompareNoCase(a, b) == 0;
}
