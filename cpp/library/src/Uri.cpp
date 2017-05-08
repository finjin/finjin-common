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
#include "finjin/common/Uri.hpp"
#include <uriparser/Uri.h>
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS || FINJIN_TARGET_PLATFORM_IS_LINUX || FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
    #include <boost/asio.hpp>
    #define USE_BOOST_IPV6 1
#else
    #include <arpa/inet.h>
#endif

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static bool IsIPV6(const Utf8String& host)
{
    auto isIPV6 = false;

#if USE_BOOST_IPV6
    boost::system::error_code boostErrorCode;
    auto hostIpAddress = boost::asio::ip::address::from_string(host.c_str(), boostErrorCode);
    if (!boostErrorCode && hostIpAddress.is_v6())
        isIPV6 = true;
#else
    uint8_t binaryHost[16]; //sizeof(struct in6_addr)
    if (inet_pton(AF_INET6, host.c_str(), binaryHost) == 1)
        isIPV6 = true;
#endif

    return isIPV6;
}

template <typename T>
ValueOrError<void> _Init(Uri* _this, const T& s)
{
    _this->scheme.clear();
    _this->host.clear();
    _this->port.clear();
    _this->path.clear();
    _this->query.clear();
    _this->fragment.clear();

    UriParserStateA state;
    UriUriA parsedUri;

    state.uri = &parsedUri;
    _this->isValid = false;
    if (uriParseUriA(&state, s.c_str()) == URI_SUCCESS)
    {
        if (_this->scheme.assign(parsedUri.scheme.first, parsedUri.scheme.afterLast).HasError())
        {
            uriFreeUriMembersA(&parsedUri);
            return ValueOrError<void>::CreateError();
        }
        if (_this->host.assign(parsedUri.hostText.first, parsedUri.hostText.afterLast).HasError())
        {
            uriFreeUriMembersA(&parsedUri);
            return ValueOrError<void>::CreateError();
        }
        if (_this->port.assign(parsedUri.portText.first, parsedUri.portText.afterLast).HasError())
        {
            uriFreeUriMembersA(&parsedUri);
            return ValueOrError<void>::CreateError();
        }
        for (auto node = parsedUri.pathHead; node != nullptr; node = node->next)
        {
            if (_this->path.append('/').HasError())
            {
                uriFreeUriMembersA(&parsedUri);
                return ValueOrError<void>::CreateError();
            }
            if (_this->path.append(node->text.first, node->text.afterLast - node->text.first).HasError())
            {
                uriFreeUriMembersA(&parsedUri);
                return ValueOrError<void>::CreateError();
            }
        }
        if (_this->query.assign(parsedUri.query.first, parsedUri.query.afterLast).HasError())
        {
            uriFreeUriMembersA(&parsedUri);
            return ValueOrError<void>::CreateError();
        }
        if (_this->fragment.assign(parsedUri.fragment.first, parsedUri.fragment.afterLast).HasError())
        {
            uriFreeUriMembersA(&parsedUri);
            return ValueOrError<void>::CreateError();
        }

        _this->isValid = true;
    }
    uriFreeUriMembersA(&parsedUri);

    return ValueOrError<void>();
}

template <typename T>
ValueOrError<void> _Init(SimpleUri* _this, const T& s)
{
    //Reset
    _this->scheme.clear();
    _this->host.clear();
    _this->path.clear();
    _this->fragment.clear();

    //Extract the non-query/non-fragment part of the the passed in string
    auto questionFoundAt = s.find('?');
    auto hashFoundAt = s.find('#');
    auto endIndex = std::min(questionFoundAt, hashFoundAt);

    //Parse
    auto colonIndex = s.find(':');
    if (colonIndex < endIndex)
    {
        Utf8StringView scheme;
        s.substr(scheme, 0, colonIndex);

        auto slashIndex = s.find('/', colonIndex);
        auto slashIndex2 = slashIndex < endIndex ? s.find('/', slashIndex + 1) : endIndex;
        if (slashIndex == colonIndex + 1 && slashIndex2 == slashIndex + 1)
        {
            //Found "://"
            if (_this->scheme.assign(scheme.begin(), scheme.end()).HasError())
                return ValueOrError<void>::CreateError();
            if (_this->scheme.empty())
            {
                //Do nothing
            }
            else
            {
                Utf8StringView formattedHost;
                auto slashIndex3 = slashIndex2 < endIndex ? s.find('/', slashIndex2 + 1) : endIndex;
                if (slashIndex3 < endIndex)
                {
                    //Host + path + maybe hash
                    s.substr(formattedHost, slashIndex2 + 1, slashIndex3 - slashIndex2 - 1);
                    if (Uri::UnformatHost(_this->host, formattedHost).HasError())
                        return ValueOrError<void>::CreateError();
                    s.substr(_this->path, slashIndex3, hashFoundAt - slashIndex3);
                }
                else
                {
                    //Host
                    s.substr(formattedHost, slashIndex2 + 1);
                    if (Uri::UnformatHost(_this->host, formattedHost).HasError())
                        return ValueOrError<void>::CreateError();
                }
            }
        }
    }
    else
    {
        //Assume the entire string is the path
        if (_this->path.assign(s.begin(), s.end()).HasError())
            return ValueOrError<void>::CreateError();
    }

    if (hashFoundAt != (size_t)-1 && _this->IsValid())
    {
        if (s.substr(_this->fragment, hashFoundAt + 1).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}


//Implementation----------------------------------------------------------------

//Uri
Uri::Uri(Allocator* allocator) :
    scheme(allocator),
    host(allocator),
    port(allocator),
    path(allocator),
    query(allocator),
    fragment(allocator)
{
    this->isValid = false;
}

Uri::Uri(const Utf8String& s, Allocator* allocator) :
    scheme(allocator),
    host(allocator),
    port(allocator),
    path(allocator),
    query(allocator),
    fragment(allocator)
{
    _Init(this, s);
}

Uri::Uri(const Uri& other, Allocator* allocator) :
    scheme(allocator),
    host(allocator),
    port(allocator),
    path(allocator),
    query(allocator),
    fragment(allocator)
{
    this->isValid = other.isValid;
    this->scheme = other.scheme;
    this->host = other.host;
    this->port = other.port;
    this->path = other.path;
    this->query = other.query;
    this->fragment = other.fragment;
}

Uri::Uri(Uri&& other)
{
    this->isValid = other.isValid;
    this->scheme = std::move(other.scheme);
    this->host = std::move(other.host);
    this->port = std::move(other.port);
    this->path = std::move(other.path);
    this->query = std::move(other.query);
    this->fragment = std::move(other.fragment);
}

Uri::~Uri()
{
}

void Uri::Create(Allocator* allocator)
{
    Destroy();

    SetAllocator(allocator);
}

void Uri::Destroy()
{
    this->isValid = false;
    this->scheme.Destroy();
    this->host.Destroy();
    this->port.Destroy();
    this->path.Destroy();
    this->query.Destroy();
    this->fragment.Destroy();
}

Allocator* Uri::GetAllocator()
{
    return this->scheme.GetAllocator();
}

bool Uri::SetAllocator(Allocator* allocator)
{
    auto result = true;
    result &= this->scheme.SetAllocator(allocator);
    result &= this->host.SetAllocator(allocator);
    result &= this->port.SetAllocator(allocator);
    result &= this->path.SetAllocator(allocator);
    result &= this->query.SetAllocator(allocator);
    result &= this->fragment.SetAllocator(allocator);
    return result;
}

void Uri::clear()
{
    this->isValid = false;
    this->scheme.clear();
    this->host.clear();
    this->port.clear();
    this->path.clear();
    this->query.clear();
    this->fragment.clear();
}

ValueOrError<void> Uri::operator = (const Utf8String& s)
{
    return _Init(this, s);
}

ValueOrError<void> Uri::operator = (const Path& s)
{
    return _Init(this, s);
}

ValueOrError<void> Uri::operator = (const Uri& other)
{
    if (this != &other)
    {
        this->isValid = false;

        if (this->scheme.assign(other.scheme).HasError())
            return ValueOrError<void>::CreateError();
        if (this->host.assign(other.host).HasError())
            return ValueOrError<void>::CreateError();
        if (this->port.assign(other.port).HasError())
            return ValueOrError<void>::CreateError();
        if (this->path.assign(other.path).HasError())
            return ValueOrError<void>::CreateError();
        if (this->query.assign(other.query).HasError())
            return ValueOrError<void>::CreateError();
        if (this->fragment.assign(other.fragment).HasError())
            return ValueOrError<void>::CreateError();

        this->isValid = other.isValid;

    }

    return ValueOrError<void>();
}

ValueOrError<void> Uri::operator = (Uri&& other)
{
    if (this != &other)
    {
        this->isValid = other.isValid;
        other.isValid = false;

        this->scheme = std::move(other.scheme);
        this->host = std::move(other.host);
        this->port = std::move(other.port);
        this->path = std::move(other.path);
        this->query = std::move(other.query);
        this->fragment = std::move(other.fragment);
    }

    return ValueOrError<void>();
}

bool Uri::IsValid() const
{
    return this->isValid;
}

const Utf8String& Uri::GetScheme() const
{
    return this->scheme;
}

const Utf8String& Uri::GetHost() const
{
    return this->host;
}

const Utf8String& Uri::GetPort() const
{
    return this->port;
}

const Utf8String& Uri::GetPath() const
{
    return this->path;
}

const Utf8String& Uri::GetQuery() const
{
    return this->query;
}

const Utf8String& Uri::GetFragment() const
{
    return this->fragment;
}

Utf8String Uri::ToString() const
{
    Utf8String result;
    ToString(result);
    return result;
}

ValueOrError<void> Uri::ToString(Utf8String& result) const
{
    result.clear();

    if (this->isValid)
    {
        if (!this->scheme.empty())
        {
            if (result.append(this->scheme).HasError())
                return ValueOrError<void>::CreateError();
            if (result.append("://").HasError())
                return ValueOrError<void>::CreateError();
        }
        Utf8String formattedHost;
        FormatHost(formattedHost, this->host);
        if (result.append(formattedHost).HasError())
            return ValueOrError<void>::CreateError();
        if (!this->port.empty())
        {
            if (result.append(":").HasError())
                return ValueOrError<void>::CreateError();
            if (result.append(this->port).HasError())
                return ValueOrError<void>::CreateError();
        }
        if (result.append(this->path).HasError())
            return ValueOrError<void>::CreateError();
        if (!this->query.empty())
        {
            if (result.append("?").HasError())
                return ValueOrError<void>::CreateError();
            if (result.append(this->query).HasError())
                return ValueOrError<void>::CreateError();
        }
        if (!this->fragment.empty())
        {
            if (result.append("#").HasError())
                return ValueOrError<void>::CreateError();
            if (result.append(this->fragment).HasError())
                return ValueOrError<void>::CreateError();
        }
    }

    return ValueOrError<void>();
}

void Uri::DecodeQueryString(Utf8String& result, const char* begin, const char* end)
{
    result.clear();

    if (begin != end)
    {
        result.assign(begin, end);
        uriUnescapeInPlaceA(&result[0]);
        result.RepairLength();
    }
}

ValueOrError<void> Uri::FormatHost(Utf8String& formattedHost, const Utf8String& host)
{
    formattedHost.clear();

    if (host.empty())
        return ValueOrError<void>();

    //Exit early if the host is already formatted
    if (host[0] == '[' && host[host.length() - 1] == ']')
        return formattedHost.assign(host);

    //If the host address is IPV6, it must be enclosed in square brackets to be put into a URI
    if (IsIPV6(host))
    {
        if (formattedHost.assign("[").HasError())
            return ValueOrError<void>::CreateError();
        if (formattedHost.append(host).HasError())
            return ValueOrError<void>::CreateError();
        if (formattedHost.append("]").HasError())
            return ValueOrError<void>::CreateError();
        return ValueOrError<void>();
    }
    else
        return formattedHost.assign(host);
}

ValueOrError<void> Uri::FormatHost(Utf8String& formattedHost, const Utf8StringView& host)
{
    return FormatHost(formattedHost, host.ToString());
}

ValueOrError<void> Uri::Format(Utf8String& uri, const Utf8String& scheme, const Utf8String& host, const Utf8String& port, const Utf8String& path)
{
    //Scheme
    if (uri.assign(scheme).HasError())
        return ValueOrError<void>::CreateError();
    if (uri.append("://").HasError())
        return ValueOrError<void>::CreateError();

    //Host
    Utf8String formattedHost;
    if (FormatHost(formattedHost, host).HasError())
        return ValueOrError<void>::CreateError();
    if (uri.append(formattedHost).HasError())
        return ValueOrError<void>::CreateError();

    //Port
    if (!port.empty())
    {
        if (uri.append(":").HasError())
            return ValueOrError<void>::CreateError();
        if (uri.append(port).HasError())
            return ValueOrError<void>::CreateError();
    }

    //Path
    if (uri.append(path).HasError())
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
}

ValueOrError<void> Uri::EscapeUriComponent(Utf8String& result, const Utf8String& text)
{
    result.clear();

    if (!text.empty())
    {
        if (result.reserve(text.length() * 3).HasError())
            return ValueOrError<void>::CreateError();
        uriEscapeA(text.c_str(), &result[0], URI_TRUE, URI_FALSE);
    }

    return ValueOrError<void>::CreateError();
}

ValueOrError<void> Uri::UnescapeUriComponent(Utf8String& result, const Utf8String& text)
{
    result.clear();

    if (!text.empty())
    {
        if (result.assign(text).HasError())
            return ValueOrError<void>::CreateError();
        uriUnescapeInPlaceA(&result[0]);
        result.RepairLength();
    }

    return ValueOrError<void>::CreateError();
}

//SimpleUri
SimpleUri::SimpleUri(Allocator* allocator) :
    scheme(allocator),
    host(allocator),
    path(allocator),
    fragment(allocator)
{
}

SimpleUri::SimpleUri(const Utf8String& s, Allocator* allocator) :
    scheme(allocator),
    host(allocator),
    path(allocator),
    fragment(allocator)
{
    _Init(this, s);
}

SimpleUri::SimpleUri(const SimpleUri& other, Allocator* allocator) :
    scheme(allocator),
    host(allocator),
    path(allocator),
    fragment(allocator)
{
    this->scheme = other.scheme;
    this->host = other.host;
    this->path = other.path;
    this->fragment = other.fragment;
}

SimpleUri::SimpleUri(SimpleUri&& other)
{
    this->scheme = std::move(other.scheme);
    this->host = std::move(other.host);
    this->path = std::move(other.path);
    this->fragment = std::move(other.fragment);
}

SimpleUri::~SimpleUri()
{
}

bool SimpleUri::Create(Allocator* allocator)
{
    Destroy();

    SetAllocator(allocator);

    return true;
}

void SimpleUri::Destroy()
{
    this->scheme.Destroy();
    this->host.Destroy();
    this->path.Destroy();
    this->fragment.Destroy();
}

Allocator* SimpleUri::GetAllocator()
{
    return this->scheme.GetAllocator();
}

bool SimpleUri::SetAllocator(Allocator* allocator)
{
    auto result = true;
    result &= this->scheme.SetAllocator(allocator);
    result &= this->host.SetAllocator(allocator);
    result &= this->path.SetAllocator(allocator);
    result &= this->fragment.SetAllocator(allocator);
    return result;
}

void SimpleUri::clear()
{
    this->scheme.clear();
    this->host.clear();
    this->path.clear();
    this->fragment.clear();
}

ValueOrError<void> SimpleUri::operator = (const Utf8String& s)
{
    return _Init(this, s);
}

ValueOrError<void> SimpleUri::operator = (const Path& s)
{
    return _Init(this, s);
}

ValueOrError<void> SimpleUri::operator = (const SimpleUri& other)
{
    if (this != &other)
    {
        if (this->scheme.assign(other.scheme).HasError())
            return ValueOrError<void>::CreateError();
        if (this->host.assign(other.host).HasError())
            return ValueOrError<void>::CreateError();
        if (this->path.assign(other.path).HasError())
            return ValueOrError<void>::CreateError();
        if (this->fragment.assign(other.fragment).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

ValueOrError<void> SimpleUri::operator = (SimpleUri&& other)
{
    if (this != &other)
    {
        this->scheme = std::move(other.scheme);
        this->host = std::move(other.host);
        this->path = std::move(other.path);
        this->fragment = std::move(other.fragment);
    }

    return ValueOrError<void>();
}

bool SimpleUri::IsValid() const
{
    if (!this->scheme.empty())
        return !this->host.empty() || !this->path.empty();
    else if (!this->path.empty())
        return this->host.empty() && this->fragment.empty();
    else
        return false;
}

const Utf8String& SimpleUri::GetScheme() const
{
    return this->scheme;
}

ValueOrError<void> SimpleUri::SetScheme(const Utf8String& value)
{
    return this->scheme = value;
}

const Utf8String& SimpleUri::GetHost() const
{
    return this->host;
}

ValueOrError<void> SimpleUri::SetHost(const Utf8String& value)
{
    return this->host = value;
}

const Utf8String& SimpleUri::GetPath() const
{
    return this->path;
}

ValueOrError<void> SimpleUri::SetPath(const Utf8String& value)
{
    return this->path = value;
}

ValueOrError<void> SimpleUri::SetPath(const Path& value)
{
    return this->path = value.ToString();
}

const Utf8String& SimpleUri::GetFragment() const
{
    return this->fragment;
}

ValueOrError<void> SimpleUri::SetFragment(const Utf8String& value)
{
    return this->fragment = value;
}

Utf8String SimpleUri::ToString() const
{
    Utf8String result;
    ToString(result);
    return result;
}

ValueOrError<void> SimpleUri::ToString(Utf8String& result) const
{
    result.clear();

    if (IsValid())
    {
        if (!this->scheme.empty())
        {
            if (result.append(this->scheme).HasError())
                return ValueOrError<void>::CreateError();
            if (result.append("://").HasError())
                return ValueOrError<void>::CreateError();
        }
        Utf8String formattedHost;
        Uri::FormatHost(formattedHost, this->host);
        if (result.append(formattedHost).HasError())
            return ValueOrError<void>::CreateError();
        if (result.append(this->path).HasError())
            return ValueOrError<void>::CreateError();
        if (!this->fragment.empty())
        {
            if (result.append("#").HasError())
                return ValueOrError<void>::CreateError();
            if (result.append(this->fragment).HasError())
                return ValueOrError<void>::CreateError();
        }
    }

    return ValueOrError<void>();
}
