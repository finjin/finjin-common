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


//Includes---------------------------------------------------------------------
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Uri.hpp"
#include "finjin/common/StringKeyValueMap.hpp"


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UriTest_parse)
{
    BOOST_TEST_MESSAGE("UriTest_parse:");

    Utf8String original = "http://www.finjin.com:80/the/path?a=b&c=d#fragment_goes_here";
    Uri uri(original);

    BOOST_CHECK(uri.IsValid() == true);
    BOOST_CHECK(uri.GetScheme() == "http");
    BOOST_CHECK(uri.GetHost() == "www.finjin.com");
    BOOST_CHECK(uri.GetPort() == "80");
    BOOST_CHECK(uri.GetPath() == "/the/path");
    BOOST_CHECK(uri.GetQuery() == "a=b&c=d");
    BOOST_CHECK(uri.GetFragment() == "fragment_goes_here");
    BOOST_CHECK(uri.ToString() == original);
}

BOOST_AUTO_TEST_CASE(UriTest_rewrite_to_self)
{
    BOOST_TEST_MESSAGE("UriTest_rewrite_to_self:");

    Utf8String original = "http://www.finjin.com:80/the/path?a=b&c=d#fragment_goes_here";    
    Utf8String uri = original;
    
    StringKeyValueMap rewritePatterns;
    rewritePatterns.Set("http", "{URI_SCHEME}://{URI_HOST}:{URI_PORT}{URI_PATH}?{URI_QUERY}#{URI_FRAGMENT}");
    
    Uri::Rewrite(uri, rewritePatterns);        
    
    BOOST_CHECK(uri == original);
}

BOOST_AUTO_TEST_CASE(UriTest_rewrite_to_secure)
{
    BOOST_TEST_MESSAGE("UriTest_rewrite_to_secure:");

    Utf8String uri = "http://www.finjin.com:80/the/path?a=b&c=d#fragment_goes_here";

    StringKeyValueMap rewritePatterns;
    rewritePatterns.Set("http", "https://{URI_HOST}:{URI_PORT}{URI_PATH}?{URI_QUERY}#{URI_FRAGMENT}");
    
    Uri::Rewrite(uri, rewritePatterns);
    
    BOOST_CHECK(uri == "https://www.finjin.com:80/the/path?a=b&c=d#fragment_goes_here");
}

BOOST_AUTO_TEST_CASE(UriTest_format_unformat_host)
{
    BOOST_TEST_MESSAGE("UriTest_format_host:");

    Utf8String ipv6Address = "FEDC:BA98:7654:3210:FEDC:BA98:7654:3210";
    Utf8String ipv6AddressWithBrackets = "[" + ipv6Address + "]";

    //Formatting
    Utf8String formattedHost = Uri::FormatHost(ipv6Address);
    BOOST_CHECK(formattedHost == ipv6AddressWithBrackets);

    //Redundant formatting
    formattedHost = Uri::FormatHost(formattedHost);
    BOOST_CHECK(formattedHost == ipv6AddressWithBrackets);

    //Unformatting
    Utf8String unformattedHost = Uri::UnformatHost(formattedHost);
    BOOST_CHECK(unformattedHost == ipv6Address);

    //Redundant unformatting
    unformattedHost = Uri::UnformatHost(unformattedHost);
    BOOST_CHECK(unformattedHost == ipv6Address);
}
