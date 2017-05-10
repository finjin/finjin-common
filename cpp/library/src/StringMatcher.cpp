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

#if FINJIN_TARGET_PLATFORM_IS_DESKTOP_OR_SERVER

#include "finjin/common/StringMatcher.hpp"
#include <boost/regex.hpp>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static void EscapeWildcardPattern(Utf8String& wildcardPattern)
{
    //Escapes a string such as "*.html" so that the the "*" part can be treated as a matcher in a Boost regular expression

    //Escape all regex special characters
    wildcardPattern.ReplaceAll("\\", "\\\\");
    wildcardPattern.ReplaceAll("^", "\\^");
    wildcardPattern.ReplaceAll(".", "\\.");
    wildcardPattern.ReplaceAll("$", "\\$");
    wildcardPattern.ReplaceAll("|", "\\|");
    wildcardPattern.ReplaceAll("(", "\\(");
    wildcardPattern.ReplaceAll(")", "\\)");
    wildcardPattern.ReplaceAll("[", "\\[");
    wildcardPattern.ReplaceAll("]", "\\]");
    wildcardPattern.ReplaceAll("*", "\\*");
    wildcardPattern.ReplaceAll("+", "\\+");
    wildcardPattern.ReplaceAll("?", "\\?");
    wildcardPattern.ReplaceAll("/", "\\/");

    //Convert * back to regex equivalent
    wildcardPattern.ReplaceAll("\\*", ".*");
}

template <typename T>
bool DoWildcardMatch(const T& src, const Utf8String& _wildcardPattern, bool caseSensitive)
{
    auto wildcardPattern = _wildcardPattern;
    EscapeWildcardPattern(wildcardPattern);

    boost::regex pattern(wildcardPattern.c_str(), caseSensitive ? boost::regex::normal : boost::regex::icase);
    return boost::regex_match(src.c_str(), pattern);
}

template <typename T>
bool DoWildcardSearch(Utf8String& found, const T& src, const Utf8String& _wildcardPattern, bool caseSensitive)
{
    auto wildcardPattern = _wildcardPattern;
    EscapeWildcardPattern(wildcardPattern);

    boost::match_results<typename T::const_iterator> match;
    boost::regex pattern(wildcardPattern.c_str(), caseSensitive ? boost::regex::normal : boost::regex::icase);
    if (boost::regex_search(src.begin(), src.end(), match, pattern))
    {
        found.assign(match[0].first, match[0].second);
        return true;
    }
    else
        return false;
}


//Implementation----------------------------------------------------------------
bool StringMatcher::WildcardMatch(const Utf8String& src, const Utf8String& wildcardPattern, bool caseSensitive)
{
    return DoWildcardMatch(src, wildcardPattern, caseSensitive);
}

bool StringMatcher::WildcardSearch(Utf8String& found, const Utf8String& src, const Utf8String& wildcardPattern, bool caseSensitive)
{
    return DoWildcardSearch(found, src, wildcardPattern, caseSensitive);
}

bool StringMatcher::WildcardMatch(const Path& src, const Utf8String& wildcardPattern, bool caseSensitive)
{
    return DoWildcardMatch(src, wildcardPattern, caseSensitive);
}

bool StringMatcher::WildcardSearch(Utf8String& found, const Path& src, const Utf8String& wildcardPattern, bool caseSensitive)
{
    return DoWildcardSearch(found, src, wildcardPattern, caseSensitive);
}

#endif
