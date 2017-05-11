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


#if FINJIN_TARGET_PLATFORM_IS_DESKTOP_OR_SERVER

//Includes----------------------------------------------------------------------
#include "finjin/common/Path.hpp"
#include "finjin/common/Utf8String.hpp"
#include <locale>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API StringMatcher
    {
    public:
        static bool WildcardMatch(const Utf8String& src, const Utf8String& wildcardPattern, bool caseSensitive = true);
        static bool WildcardSearch(Utf8String& found, const Utf8String& src, const Utf8String& wildcardPattern, bool caseSensitive = true);

        static bool WildcardMatch(const Path& src, const Utf8String& wildcardPattern, bool caseSensitive = true);
        static bool WildcardSearch(Utf8String& found, const Path& src, const Utf8String& wildcardPattern, bool caseSensitive = true);
    };

} }

#endif
