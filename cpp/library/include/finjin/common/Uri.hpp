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
#include "finjin/common/Path.hpp"
#include "finjin/common/StringKeyValueMap.hpp"
#include "finjin/common/Utf8String.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class SimpleUri;

    /**
     * Utility methods for dealing with URIs.
     */
    class Uri
    {
    public:
        Uri(Allocator* allocator = nullptr);
        Uri(const Utf8String& s, Allocator* allocator = nullptr);
        Uri(const Uri& other, Allocator* allocator = nullptr);
        Uri(Uri&& other);
        ~Uri();

        void Create(Allocator* allocator);
        void Destroy();

        Allocator* GetAllocator();
        bool SetAllocator(Allocator* allocator);

        void clear();

        ValueOrError<void> operator = (const Utf8String& s);
        ValueOrError<void> operator = (const Path& s);
        ValueOrError<void> operator = (const Uri& other);
        ValueOrError<void> operator = (Uri&& other);

        bool IsValid() const;

        const Utf8String& GetScheme() const;
        const Utf8String& GetHost() const;
        const Utf8String& GetPort() const;
        const Utf8String& GetPath() const;
        const Utf8String& GetQuery() const;
        const Utf8String& GetFragment() const;

        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

        static void DecodeQueryString(Utf8String& result, const char* begin, const char* end);

    public:
        /**
         * Formats the host for use in a URI.
         * @param host [in] The host. Can be a FQDN, IPv4 address, or IPv6 address.
         * @return If the passed in address is an unformatted IPv6 address, it is returned as a new string enclosed in square brackets. Otherwise, 'host' is returned directly.
         */
        static ValueOrError<void> FormatHost(Utf8String& result, const Utf8String& host);
        static ValueOrError<void> FormatHost(Utf8String& result, const Utf8StringView& host);

        /**
         * Unformats the host that was used in a URI.
         * @param host [in] The host. Can be a FQDN, IPv4 address, IPv6 address, or an IPv6 address enclosed in square brackets.
         * @return If the passed in address is enclosed in square brackets, the string within the brackets is returned. Otherwise, 'formattedHost' is returned directly.
         */
        template <typename T>
        static ValueOrError<void> UnformatHost(Utf8String& result, const T& formattedHost)
        {
            if (!formattedHost.empty() && formattedHost[0] == '[' && formattedHost[formattedHost.length() - 1] == ']')
                return formattedHost.substr(result, 1, formattedHost.length() - 2);
            else
                return result.assign(formattedHost.begin(), formattedHost.end());
        }

        /** 
         * Formats a URI based on the specified components.
         * @param scheme [in] The URI scheme. It should not include the trailing "://"
         * @param host [in] The host. Can be a FQDN, IPv4 address, IPv6 address, or an IPv6 address enclosed in square brackets.
         * @param port [in] The port number as a string. If empty, no port will appear in the URI.
         * @param path [in] The path. It is inserted exactly as specified.
         * @return A URI based on the specified components.
         */
        static ValueOrError<void> Format(Utf8String& result, const Utf8String& scheme, const Utf8String& host, const Utf8String& port, const Utf8String& path);

        /**
         * Rewrites the specified URI according to the appropriate pattern.
         * A pattern that rewrites a URI to its  original form is as follows: {URI_SCHEME}://{URI_HOST}:{URI_PORT}{URI_PATH_QUERY_FRAGMENT}. 
         * Definitions:
         * {URI_SCHEME}: The scheme.
         * {URI_HOST}: The host.
         * {URI_PORT}: The port. If no port was specified in the original URI, the default port for the specified scheme will be used, assuming the scheme is http, https, ws, or wss.
         * {URI_PATH}: The path, if there is one. This will be of the form "/some/path"
         * {URI_QUERY}: The query string, if there is one. If there is one, it will include the leading "?". For example: "?key=value"
         * {URI_FRAGMENT}: The fragment, if there is one. If there is one, it will include the leading "#". For example: "#fragment"
         * {URI_PATH_QUERY_FRAGMENT}: The path and query string. Equivalent to using {URI_PATH}{URI_QUERY}{URI_FRAGMENT}
         * @param uri [in/out] The URI to rewrite. If there wasn't an appropriate entry in the table for the URI's scheme, the input is not modified.
         * @param rewritePatterns [in] A lookup table of patterns. Patterns are keyed by URI scheme, so the scheme specified by 'uri' 
         * will decide which entry is used. If the table contains an entry with an empty key, that entry is used as a default in the
         * event that the scheme of 'uri' doesn't have a corresponding entry in the table. 
         */
        static ValueOrError<void> Rewrite(Utf8String& uri, const StringKeyValueMap& rewritePatterns);

        /**
        * Escapes the specified text so that it can be safely inserted into HTML as a URI component.
        * @param text [in] The text to escape.
        * @return The escaped text.
        */
        static ValueOrError<void> EscapeUriComponent(Utf8String& result, const Utf8String& text);

        static ValueOrError<void> UnescapeUriComponent(Utf8String& result, const Utf8String& text);

    public:
        bool isValid;
        Utf8String scheme;
        Utf8String host;
        Utf8String port;
        Utf8String path;
        Utf8String query;
        Utf8String fragment;
    };

    //Only suitable for use with URIs that are known to be "simple"
    //A simple URI has the following forms:
    //path only: /some/path, somefile.txt
    //scheme + host: http://somehost
    //file scheme + path: file:///somefile.txt, file:///some/path/somefile.txt
    //scheme + host + path: http://somehost/some/path/somefile.txt
    //any of the above + fragment: file:///somefile.txt#fragment
    class SimpleUri
    {
    public:
        SimpleUri(Allocator* allocator = nullptr);
        SimpleUri(const Utf8String& s, Allocator* allocator = nullptr);
        SimpleUri(const SimpleUri& other, Allocator* allocator = nullptr);
        SimpleUri(SimpleUri&& other);
        ~SimpleUri();

        bool Create(Allocator* allocator);
        void Destroy();

        Allocator* GetAllocator();
        bool SetAllocator(Allocator* allocator);

        void clear();

        ValueOrError<void> operator = (const Utf8String& s);
        ValueOrError<void> operator = (const Path& s);
        ValueOrError<void> operator = (const SimpleUri& other);
        ValueOrError<void> operator = (SimpleUri&& other);

        bool IsValid() const;

        const Utf8String& GetScheme() const;
        ValueOrError<void> SetScheme(const Utf8String& value);

        const Utf8String& GetHost() const;
        ValueOrError<void> SetHost(const Utf8String& value);

        const Utf8String& GetPath() const;
        ValueOrError<void> SetPath(const Utf8String& value);
        ValueOrError<void> SetPath(const Path& value);

        const Utf8String& GetFragment() const;
        ValueOrError<void> SetFragment(const Utf8String& value);

        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

    public:
        Utf8String scheme;
        Utf8String host;
        Utf8String path;
        Utf8String fragment;
    };

} }
