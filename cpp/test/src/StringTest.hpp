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
#include "finjin/common/Utf8String.hpp"
#include <vector>


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(StringTest_replace_all)
{
    BOOST_TEST_MESSAGE("StringTest_replace_all:");

    Utf8String s("the quick brown fox is quick");
    s.ReplaceAll("quick", "slow");
    BOOST_CHECK(s == "the slow brown fox is slow");
}

BOOST_AUTO_TEST_CASE(StringTest_ends_with)
{
    BOOST_TEST_MESSAGE("StringTest_ends_with_test:");

    Utf8String s("the quick brown fox");
    BOOST_CHECK(s.EndsWith("fox"));
}

BOOST_AUTO_TEST_CASE(StringTest_starts_with)
{
    BOOST_TEST_MESSAGE("StringTest_starts_with:");

    Utf8String s("the quick brown fox");
    BOOST_CHECK(s.StartsWith("the"));
}

BOOST_AUTO_TEST_CASE(StringTest_find)
{
    BOOST_TEST_MESSAGE("StringTest_find:");

    Utf8String s("the quick brown fox");
    BOOST_CHECK(s.find("quick") == 4);
}

BOOST_AUTO_TEST_CASE(StringTest_reverse_find)
{
    BOOST_TEST_MESSAGE("StringTest_reverse_find:");

    Utf8String s("the quick brown fox");
    BOOST_CHECK(s.rfind("quick") == 4);
}

BOOST_AUTO_TEST_CASE(StringTest_ensure_shortness)
{
    BOOST_TEST_MESSAGE("StringTest_ensure_shortness:");

    Utf8String s;

    BOOST_CHECK(s.IsStatic() == true); //It's empty, so it should be short

    for (int i = 0; i < 1000; i++) //This should be WAY MORE than enough to make it not-short
        s += "this is a test this is a test";
    BOOST_CHECK(s.IsStatic() == false);

    s.clear();
    BOOST_CHECK(s.IsStatic() == false); //It's empty, but the memory doesn't get freed

    Utf8String sCopy = s;
    BOOST_CHECK(sCopy.IsStatic() == true); //It's a copy of an empty string (that is still non-short). The copy should be short
}

BOOST_AUTO_TEST_CASE(StringTest_equals_no_case)
{
    BOOST_TEST_MESSAGE("StringTest_equals_no_case:");

    Utf8String a("ThisIsATest");
    Utf8String b("thisisatest");
    BOOST_CHECK(a.EqualsNoCaseAscii(b));
}

BOOST_AUTO_TEST_CASE(StringTest_upper_ascii)
{
    BOOST_TEST_MESSAGE("StringTest_upper_ascii:");

    Utf8String a("ThisIsATest");
    a.ToUpperAscii();
    BOOST_CHECK(a == "THISISATEST");
}

BOOST_AUTO_TEST_CASE(StringTest_lower_ascii)
{
    BOOST_TEST_MESSAGE("StringTest_lower_ascii:");

    Utf8String a("ThisIsATest");
    a.ToLowerAscii();
    BOOST_CHECK(a == "thisisatest");
}

BOOST_AUTO_TEST_CASE(StringTest_codepoint_count)
{
    BOOST_TEST_MESSAGE("StringTest_codepoint_count:");

    Utf8String s("Some");
    size_t count = 0;
    BOOST_CHECK(s.GetCodepointCount(count));
    BOOST_CHECK(count == 4);
}

BOOST_AUTO_TEST_CASE(StringTest_codepoint_get)
{
    BOOST_TEST_MESSAGE("StringTest_codepoint_get:");

    Utf8String s("Some");
    size_t count = 0;
    if (s.GetCodepointCount(count))
    {
        uint32_t codepoint;
        for (size_t i = 0; s.IterateCodepoint(i, codepoint);)
        {
            switch (i - 1)
            {
                case 0: BOOST_CHECK(codepoint == 'S'); break;
                case 1: BOOST_CHECK(codepoint == 'o'); break;
                case 2: BOOST_CHECK(codepoint == 'm'); break;
                case 3: BOOST_CHECK(codepoint == 'e'); break;
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(StringTest_codepoints_get)
{
    BOOST_TEST_MESSAGE("StringTest_codepoints_get:");

    Utf8String s("Some");

    std::vector<uint32_t> codepoints;
    codepoints.resize(s.length() + 1);
    size_t convertedCount = codepoints.size();
    if (s.GetCodepoints(&codepoints[0], convertedCount))
    {
        for (size_t i = 0; i < convertedCount; i++)
        {
            switch (i)
            {
                case 0: BOOST_CHECK(codepoints[i] == 'S'); break;
                case 1: BOOST_CHECK(codepoints[i] == 'o'); break;
                case 2: BOOST_CHECK(codepoints[i] == 'm'); break;
                case 3: BOOST_CHECK(codepoints[i] == 'e'); break;
            }
        }
    }
    else
    {
        BOOST_CHECK(false);
    }
}

BOOST_AUTO_TEST_CASE(StringTest_utf32_codepoint_test)
{
    BOOST_TEST_MESSAGE("StringTest_utf32_codepoint_test:");

    Utf8String s("SomeTest");

    //Convert to u32string
    Utf8StringToCodepointString codepointString(s);

    //Convert to uint32 vector
    std::vector<uint32_t> codepointVector;
    codepointVector.resize(s.length() + 1);
    size_t convertedCount = codepointVector.size();
    if (s.GetCodepoints(&codepointVector[0], convertedCount))
    {
        //Ensure both strings are of the correct length
        BOOST_CHECK(codepointString.length() == convertedCount);
        if (codepointString.length() == convertedCount)
        {
            //Check all characters
            for (size_t i = 0; i < convertedCount; i++)
            {
                BOOST_CHECK(codepointString[i] == codepointVector[i]);
            }
        }
    }
    else
    {
        BOOST_CHECK(false);
    }
}

BOOST_AUTO_TEST_CASE(StringTest_utf8stringpointer_compare)
{
    BOOST_TEST_MESSAGE("StringTest_utf8stringpointer_compare:");

    {
        Utf8String left("same");
        Utf8StringView right("same", 4);
        BOOST_CHECK(left == right);

        BOOST_CHECK(!(left > right));

        BOOST_CHECK(!(left < right));
    }

    {
        Utf8String left("base");
        Utf8StringView right("basen", 5);
        BOOST_CHECK(left < right);

        BOOST_CHECK(!(left == right));

        BOOST_CHECK(!(left > right));
    }

    {
        Utf8String left("basen");
        Utf8StringView right("base", 5);
        BOOST_CHECK(left > right);

        BOOST_CHECK(!(left == right));

        BOOST_CHECK(!(left < right));
    }
}

BOOST_AUTO_TEST_CASE(StringTest_removechars)
{
    BOOST_TEST_MESSAGE("StringTest_removechars:");

    //Check a basic interior replacement
    {
        Utf8String s("there are spaces in this string");
        s.RemoveAllChars(" ");

        BOOST_CHECK(s == "therearespacesinthisstring");
    }

    //Check a beginning/end replacement
    {
        Utf8String s(" there are spaces in this string ");
        s.RemoveAllChars(" ");

        BOOST_CHECK(s == "therearespacesinthisstring");
    }
}

BOOST_AUTO_TEST_CASE(StringTest_replacechars)
{
    BOOST_TEST_MESSAGE("StringTest_replacechars:");

    Utf8String s("abcdefg");
    s.ReplaceAllChars("ad", 'z');

    BOOST_CHECK(s == "zbczefg");
}
