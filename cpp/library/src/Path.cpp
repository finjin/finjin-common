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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/Utf8String.hpp"
#include "FileAccessor.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include "finjin/common/WindowsUtilities.hpp"

    #if !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        #include <Shlobj.h>
        #include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
    #endif
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fstream>
#endif

#if FINJIN_TARGET_PLATFORM_IS_APPLE
    #include "AppleUtilities.hpp"
#endif

#include <nowide/stackstring.hpp>
#include <nowide/fstream.hpp>

#if (!FINJIN_TARGET_PLATFORM_IS_ANDROID && FINJIN_TARGET_PLATFORM_IS_LINUX) || FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
    //Non-Android Linux or MacOS

    #define USE_BOOST_MAPPED_FILE 0 //Disabled for now since I ran into an issue on Linux

    #if USE_BOOST_MAPPED_FILE
        #include <boost/iostreams/device/mapped_file.hpp>
        #ifndef O_LARGEFILE
            #define O_LARGEFILE 0
        #endif
        #include <libs/iostreams/src/mapped_file.cpp>
    #endif
#endif

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define LONG_PATH_PREFIX "\\\\?\\"
#define LONG_PATH_PREFIX_LENGTH 4

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #define PLATFORM_SEPARATOR_CHAR '\\'
    #define PLATFORM_SEPARATOR_STRING "\\"
#else
    #define PLATFORM_SEPARATOR_CHAR '/'
    #define PLATFORM_SEPARATOR_STRING "/"
#endif

#define PLATFORM_INDEPENDENT_SEPARATOR_CHAR '/'
#define PLATFORM_INDEPENDENT_SEPARATOR_STRING "/"


//Local types-------------------------------------------------------------------
using NarrowingConverter = nowide::basic_stackstring<char, wchar_t, Path::STATIC_STRING_LENGTH + 1>;
using WideningToUtf16Converter = nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1>;
using WideningToUtf32Converter = nowide::basic_stackstring<char32_t, char, Path::STATIC_STRING_LENGTH + 1>;


//Local functions---------------------------------------------------------------
static size_t EstimateNewAllocatedLength(size_t requestedLength, size_t allocatedLength, bool oversize)
{
    if (oversize)
    {
        //Oversize string
        allocatedLength = 2 * std::max(static_cast<size_t>(Path::STATIC_STRING_LENGTH), std::max(allocatedLength, requestedLength));
    }
    else
    {
        //No oversizing desired, just use the requested length
        allocatedLength = requestedLength;
    }
    return allocatedLength;
}

template <typename T>
bool HasLongPathPrefix(const T* begin)
{
    const char* longPathPrefix = LONG_PATH_PREFIX;
    for (size_t i = 0; i < longPathPrefix[i] != 0; i++)
    {
        if (*begin == 0 || *begin != longPathPrefix[i])
            return false;

        begin++;
    }

    return true;
}

template <typename T>
bool HasLongPathPrefix(const T* begin, const T* end)
{
    const char* longPathPrefix = LONG_PATH_PREFIX;
    for (size_t i = 0; i < longPathPrefix[i] != 0; i++)
    {
        if (begin == end || *begin != longPathPrefix[i])
            return false;

        begin++;
    }

    return true;
}

template <typename T>
bool IsAbsolute(const T* begin)
{
    if (*begin == 0)
        return false;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    const T* index = begin;
    if (HasLongPathPrefix(begin))
    {
        //Skip prefix
        index += LONG_PATH_PREFIX_LENGTH;
    }
    else if (Path::IsSeparator(*index) && *(index + 1) != 0 && Path::IsSeparator(*(index + 1)))
    {
        //Network location
        return true;
    }

    //See if it has a drive letter separator
    if (*(index + 1) != 0 && *(index + 1) == ':' && *(index + 2) != 0 && Path::IsSeparator(*(index + 2)))
        return true;
#else
    if (Path::IsSeparator(*begin))
        return true;
#endif

    return false;
}

template <typename T>
bool IsAbsolute(const T* begin, const T* end)
{
    if (begin == end)
        return false;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    const T* index = begin;
    if (HasLongPathPrefix(begin, end))
    {
        //Skip prefix
        index += LONG_PATH_PREFIX_LENGTH;
    }
    else if (Path::IsSeparator(*index) && (index + 1) != end && Path::IsSeparator(*(index + 1)))
    {
        //Network location
        return true;
    }

    //See if it has a drive letter separator
    if ((index + 1) != end && *(index + 1) == ':' && (index + 2) != end && Path::IsSeparator(*(index + 2)))
        return true;
#else
    if (Path::IsSeparator(*begin))
        return true;
#endif

    return false;
}

static ValueOrError<void> AppendSeparator(Path& path)
{
    if (!path.empty() && !path.EndsWith("/") && !path.EndsWith("\\"))
        return path.append(PLATFORM_INDEPENDENT_SEPARATOR_STRING);
    return ValueOrError<void>();
}

template <typename T>
ValueOrError<void> AppendPath(Path& path, const T* otherBegin)
{
    if (IsAbsolute(otherBegin))
        return path.assign(otherBegin);

    auto otherCopy = otherBegin;

    while (*otherBegin != 0 && Path::IsSeparator(*otherBegin))
        otherBegin++;

    for (auto i = otherBegin; *i != 0;)
    {
        if (*i == '.' && *(i + 1) != 0)
        {
            if (*(i + 1) == '.' && *(i + 2) != 0)
            {
                if (Path::IsSeparator(*(i + 2)))
                {
                    //Found "../"
                    path.GoToParent();
                    otherBegin += 3;
                    i += 3;
                }
                else
                    i += 2;
            }
            else if (i == otherCopy && *(i + 2) == 0)
            {
                //Special case where other is just ".."
                path.GoToParent();
                otherBegin += 2;
                i += 2;
                break;
            }
            else
                i += 1;
        }
        else if (*i == '.' && *(i + 1) != 0)
        {
            if (Path::IsSeparator(*(i + 1)))
            {
                //Found "./"
                i += 2;
                otherBegin += 2;
            }
            else
                i += 1;
        }
        else if (*i == '.' && i == otherCopy && *(i + 1) == 0)
        {
            //Special case where other is just "."
            otherBegin += 1;
            i += 1;
            break;
        }
        else
            break;
    }

    while (*otherBegin != 0 && Path::IsSeparator(*otherBegin))
        otherBegin++;

    if (*otherBegin != 0)
    {
        if (AppendSeparator(path).HasError())
            return ValueOrError<void>::CreateError();
        if (path.append(otherBegin).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

template <typename T>
ValueOrError<void> AppendPath(Path& path, const T* otherBegin, const T* otherEnd)
{
    //Skips past the leading relative specifiers, modifying 'path' as necessary
    //Note: This does NOT account for relative path specifiers that come in the middle of a path component, such as "a/b/../d"

    if (IsAbsolute(otherBegin, otherEnd))
        return path.assign(otherBegin, otherEnd);

    auto otherCopy = otherBegin;

    while (otherBegin != otherEnd && Path::IsSeparator(*otherBegin))
        otherBegin++;

    for (auto i = otherBegin; i != otherEnd;)
    {
        if (*i == '.' && (i + 1) != otherEnd)
        {
            if (*(i + 1) == '.' && (i + 2) != otherEnd)
            {
                if (Path::IsSeparator(*(i + 2)))
                {
                    //Found "../"
                    path.GoToParent();
                    otherBegin += 3;
                    i += 3;
                }
                else
                    i += 2;
            }
            else if (i == otherCopy && (i + 2) == otherEnd)
            {
                //Special case where other is just ".."
                path.GoToParent();
                otherBegin += 2;
                i += 2;
                break;
            }
            else
                i += 1;
        }
        else if (*i == '.' && (i + 1) != otherEnd)
        {
            if (Path::IsSeparator(*(i + 1)))
            {
                //Found "./"
                i += 2;
                otherBegin += 2;
            }
            else
                i += 1;
        }
        else if (*i == '.' && i == otherCopy && (i + 1) == otherEnd)
        {
            //Special case where other is just "."
            otherBegin += 1;
            i += 1;
            break;
        }
        else
            break;
    }

    while (otherBegin != otherEnd && Path::IsSeparator(*otherBegin))
        otherBegin++;

    if (otherBegin != otherEnd)
    {
        if (AppendSeparator(path).HasError())
            return ValueOrError<void>::CreateError();
        if (path.append(otherBegin, otherEnd - otherBegin).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

static bool IsValidCodepoint(uint32_t ch)
{
    return ch < 0xd800 || ch > 0xdfff;
}


//Implementation----------------------------------------------------------------
const Path& Path::Empty()
{
    static const Path value;
    return value;
}

Path::Path(Allocator* allocator)
{
    Init();
    this->allocator = allocator;
}

Path::Path(const char* other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Path::Path(const wchar_t* other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Path::Path(const char* other, size_t len, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, len);
}

Path::Path(const wchar_t* other, size_t len, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, len);
}

Path::Path(const char* first, const char* last, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(first, last);
}

Path::Path(const wchar_t* first, const wchar_t* last, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(first, last);
}

Path::Path(const uint8_t* first, const uint8_t* last, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(first, last);
}

Path::Path(size_t count, char c, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(count, c);
}

Path::Path(size_t count, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    EnsureLengthAllocated(count, false);
}

Path::Path(const Utf8String& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Path::Path(Utf8String&& other)
{
    Init();
    assign(std::move(other));
}

Path::Path(const Utf8StringView& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Path::Path(const Path& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Path::Path(Path&& other)
{
    Init();
    assign(std::move(other));
}

Path::~Path()
{
    Free();
}

bool Path::Create(Allocator* allocator)
{
    if (this->allocator == allocator)
        Truncate();
    else
    {
        Destroy();
        SetAllocator(allocator);
    }

    return true;
}

void Path::Destroy()
{
    Free();

    this->allocator = nullptr;
}

Allocator* Path::GetAllocator()
{
    return this->allocator;
}

bool Path::SetAllocator(Allocator* allocator)
{
    if (allocator == this->allocator)
    {
        //Same allocator
        return true;
    }
    if (this->allocator != nullptr)
    {
        //Already has allocator
        return false;
    }
    if (!IsStatic())
    {
        //Dynamic allocation has been made, implicitly determining default/global allocator
        return false;
    }

    this->allocator = allocator;
    return true;
}

ValueOrError<void> Path::operator = (const char* other)
{
    return assign(other);
}

ValueOrError<void> Path::operator = (const wchar_t* other)
{
    return assign(other);
}

ValueOrError<void> Path::operator = (const Utf8String& other)
{
    return assign(other);
}

ValueOrError<void> Path::operator = (Utf8String&& other)
{
    return assign(std::move(other));
}

ValueOrError<void> Path::operator = (const Utf8StringView& other)
{
    return assign(other);
}

ValueOrError<void> Path::operator = (const Path& other)
{
    return assign(other);
}

ValueOrError<void> Path::operator = (Path&& other)
{
    return assign(std::move(other));
}

ValueOrError<void> Path::assign(const char* other)
{
    if (other == this->s)
    {
        return ValueOrError<void>();
    }
    else if (other == nullptr || other[0] == 0)
    {
        //Other string is zero length
        Truncate();
        return ValueOrError<void>();
    }
    else
        return assign(other, strlen(other));
}

ValueOrError<void> Path::assign(const wchar_t* other)
{
    if (other == nullptr || other[0] == 0)
    {
        //Other string is zero length
        Truncate();
        return ValueOrError<void>();
    }
    else
    {
        NarrowingConverter narrowed;
        if (!narrowed.convert(other))
            return ValueOrError<void>::CreateError();
        return assign(narrowed.c_str());
    }
}

ValueOrError<void> Path::assign(const char* other, size_t len)
{
    if (other == this->s && len == this->l)
    {
        //Do nothing
    }
    else if (other == nullptr || len == 0)
    {
        //Other string is zero length
        Truncate();
    }
    else
    {
        if (EnsureLengthAllocated(len, false).HasError())
            return ValueOrError<void>::CreateError();

        strncpy(this->s, other, len);
        this->s[len] = 0;
        this->l = len;
    }
    return ValueOrError<void>();
}

ValueOrError<void> Path::assign(const wchar_t* other, size_t len)
{
    if (other == nullptr || len == 0)
    {
        //Other string is zero length
        Truncate();
        return ValueOrError<void>();
    }
    else
    {
        if (EnsureLengthAllocated(len, false).HasError())
            return ValueOrError<void>::CreateError();

        NarrowingConverter narrowed;
        if (!narrowed.convert(other, other + len))
            return ValueOrError<void>::CreateError();
        return assign(narrowed.c_str());
    }
}

ValueOrError<void> Path::assign(const char* first, const char* last)
{
    if (last <= first)
    {
        Truncate();
        return ValueOrError<void>();
    }
    else
        return assign(first, last - first);
}

ValueOrError<void> Path::assign(const wchar_t* first, const wchar_t* last)
{
    if (last <= first)
    {
        Truncate();
        return ValueOrError<void>();
    }
    else
        return assign(first, last - first);
}

ValueOrError<void> Path::assign(const uint8_t* first, const uint8_t* last)
{
    return assign(reinterpret_cast<const char*>(first), reinterpret_cast<const char*>(last));
}

ValueOrError<void> Path::assign(size_t count, char c)
{
    if (count == 0)
    {
        Truncate();
    }
    else
    {
        if (EnsureLengthAllocated(count, false).HasError())
            return ValueOrError<void>::CreateError();

        for (size_t i = 0; i < count; i++)
            this->s[i] = c;
        this->s[count] = 0;
        this->l = count;
    }
    return ValueOrError<void>();
}

ValueOrError<void> Path::assign(const Utf8String& other)
{
    if (EnsureLengthAllocated(other.length(), false).HasError())
        return ValueOrError<void>::CreateError();

    strcpy(this->s, other.s);
    this->l = other.length();
    return ValueOrError<void>();
}

ValueOrError<void> Path::assign(Utf8String&& other)
{
    if (other.length() <= STATIC_STRING_LENGTH)
    {
        //Copy
        strcpy(this->s, other.s);
        this->l = other.length();
        other.Truncate();
    }
    else
    {
        //Take other string
        Free();

        this->allocator = other.allocator;
        this->s = other.s;
        this->l = other.length();
        this->allocatedLength = other.allocatedLength;

        other.allocator = nullptr;
        other.s = other.shortS;
        other.shortS[0] = 0;
        other.l = 0;
        other.allocatedLength = 0;
    }

    return ValueOrError<void>();
}

ValueOrError<void> Path::assign(const Utf8StringView& other)
{
    if (EnsureLengthAllocated(other.length(), false).HasError())
        return ValueOrError<void>::CreateError();

    strncpy(this->s, other.s, other.length());
    this->l = other.length();
    return ValueOrError<void>();
}

ValueOrError<void> Path::assign(const Path& other)
{
    if (&other == this)
        return ValueOrError<void>();

    if (EnsureLengthAllocated(other.length(), false).HasError())
        return ValueOrError<void>::CreateError();

    strcpy(this->s, other.s);
    this->l = other.length();
    return ValueOrError<void>();
}

ValueOrError<void> Path::assign(Path&& other)
{
    if (other.IsStatic())
    {
        //Copy
        strcpy(this->s, other.s);
        this->l = other.length();
        other.Truncate();
    }
    else
    {
        //Take other string
        Free();

        this->allocator = other.allocator;
        this->s = other.s;
        this->l = other.length();
        this->allocatedLength = other.allocatedLength;

        other.allocator = nullptr;
        other.s = other.shortS;
        other.shortS[0] = 0;
        other.l = 0;
        other.allocatedLength = 0;
    }
    return ValueOrError<void>();
}

const char* Path::c_str() const
{
    return this->s;
}

bool Path::empty() const
{
    return this->l == 0;
}

void Path::clear()
{
    Truncate();
}

size_t Path::length() const
{
    return this->l;
}

size_t Path::size() const
{
    return this->l;
}

ValueOrError<void> Path::reserve(size_t len)
{
    return EnsureLengthAllocated(len, false);
}

size_t Path::GetCharactersLeft() const
{
    if (IsStatic())
        return STATIC_STRING_LENGTH - this->l;
    else
        return this->allocatedLength - this->l;
}

ValueOrError<void> Path::resize(size_t len)
{
    if (len == 0)
    {
        Truncate();
        return ValueOrError<void>();
    }
    else
    {
        //Ensure the buffer can hold a string of len + 1. This does not preserve the content of the current string
        if (len > STATIC_STRING_LENGTH && len > this->allocatedLength)
        {
            this->allocatedLength = len;

            if (!IsStatic())
                _Deallocate(this->s);
            this->s = _Allocate(this->allocatedLength + 1, FINJIN_CALLER_ARGUMENTS);
        }

        if (this->s == nullptr)
        {
            Truncate();
            return ValueOrError<void>();
        }

        this->l = len;
        this->s[0] = 0;
        this->s[this->l] = 0;
        return ValueOrError<void>();
    }
}

Path::iterator Path::begin()
{
    return this->s;
}

Path::iterator Path::end()
{
    return this->s + this->l;
}

Path::const_iterator Path::begin() const
{
    return this->s;
}

Path::const_iterator Path::end() const
{
    return this->s + this->l;
}

bool Path::operator == (const Path& other) const
{
    return this->l == other.l && strcmp(this->s, other.s) == 0;
}

bool Path::operator != (const Path& other) const
{
    return !(operator == (other));
}

bool Path::operator < (const Path& other) const
{
    return strcmp(this->s, other.s) < 0;
}

bool Path::operator > (const Path& other) const
{
    return strcmp(this->s, other.s) > 0;
}

bool Path::operator == (const Utf8String& other) const
{
    return this->l == other.l && strcmp(this->s, other.s) == 0;
}

bool Path::operator != (const Utf8String& other) const
{
    return !(operator == (other));
}

bool Path::operator < (const Utf8String& other) const
{
    return strcmp(this->s, other.s) < 0;
}

bool Path::operator > (const Utf8String& other) const
{
    return strcmp(this->s, other.s) > 0;
}

bool Path::operator == (const char* other) const
{
    return strcmp(this->s, Utf8String::GetNonNull(other)) == 0;
}

bool Path::operator != (const char* other) const
{
    return strcmp(this->s, Utf8String::GetNonNull(other)) != 0;
}

bool Path::operator < (const char* other) const
{
    return strcmp(this->s, Utf8String::GetNonNull(other)) < 0;
}

bool Path::operator > (const char* other) const
{
    return strcmp(this->s, Utf8String::GetNonNull(other)) > 0;
}

ValueOrError<void> Path::operator += (Utf8String&& other)
{
    return append(std::move(other));
}

ValueOrError<void> Path::operator += (const Utf8String& other)
{
    return append(other.s);
}

ValueOrError<void> Path::operator += (const char* other)
{
    return append(other);
}

ValueOrError<void> Path::operator += (const wchar_t* other)
{
    return append(other);
}

ValueOrError<void> Path::operator += (char other)
{
    return append(other);
}

ValueOrError<void> Path::append(Utf8String&& other)
{
    if (this->l == 0)
        return assign(std::move(other));
    else
        return append(other.s, other.l);
}

ValueOrError<void> Path::append(const Utf8String& other)
{
    return append(other.s, other.l);
}

ValueOrError<void> Path::append(const char* other)
{
    if (other != nullptr && other[0] != 0)
        return append(other, strlen(other));
    else
        return ValueOrError<void>();
}

ValueOrError<void> Path::append(const char* other, size_t len)
{
    if (other != nullptr && other[0] != 0 && len > 0)
    {
        auto newLength = this->l + len;
        if (newLength <= STATIC_STRING_LENGTH || newLength <= this->allocatedLength)
        {
            strncpy(&this->s[this->l], other, len);
            this->s[newLength] = 0;
            this->l = newLength;
        }
        else
        {
            Path newString(this->allocator);
            if (newString.EnsureLengthAllocated(this->l + len, this->allocatedLength > 0).HasError())
                return ValueOrError<void>::CreateError();
            if (newString.append(this->s, this->l).HasError())
                return ValueOrError<void>::CreateError();
            if (newString.append(other, len).HasError())
                return ValueOrError<void>::CreateError();

            return assign(std::move(newString));
        }
    }

    return ValueOrError<void>();
}

ValueOrError<void> Path::append(const wchar_t* other)
{
    NarrowingConverter narrowed;
    if (!narrowed.convert(other))
        return ValueOrError<void>::CreateError();
    return append(narrowed.c_str());
}

ValueOrError<void> Path::append(const wchar_t* other, size_t len)
{
    NarrowingConverter narrowed;
    if (!narrowed.convert(other, other + len))
        return ValueOrError<void>::CreateError();
    return append(narrowed.c_str());
}

ValueOrError<void> Path::append(char other)
{
    char s[2] = {other, 0};
    return append(s, 1);
}

ValueOrError<void> Path::append(wchar_t other)
{
    wchar_t s[2] = {other, 0};
    return append(s, 1);
}

ValueOrError<void> Path::append(size_t len, char c)
{
    if (len > 0)
    {
        auto newLength = this->l + len;
        if (newLength <= STATIC_STRING_LENGTH || newLength <= this->allocatedLength)
        {
            for (size_t i = 0; i < len; i++)
                this->s[this->l + i] = c;
            this->s[newLength] = 0;
            this->l = newLength;
        }
        else
        {
            Utf8String newString(this->allocator);
            if (newString.EnsureLengthAllocated(this->l + len, this->allocatedLength > 0).HasError())
                return ValueOrError<void>::CreateError();
            if (newString.append(this->s, this->l).HasError())
                return ValueOrError<void>::CreateError();
            if (newString.append(len, c).HasError())
                return ValueOrError<void>::CreateError();

            return assign(std::move(newString));
        }
    }

    return ValueOrError<void>();
}

ValueOrError<void> Path::push_back(char c)
{
    return append(c);
}

char Path::back() const
{
    assert(!empty());

    if (!empty())
        return this->s[this->l - 1];
    else
        return 0;
}

void Path::pop_front()
{
    if (!empty())
        erase(begin());
}

void Path::pop_front(size_t count)
{
    count = std::min(count, this->l);
    for (size_t i = 0; i < count; i++)
        pop_front();
}

void Path::pop_back()
{
    pop_back(1);
}

void Path::pop_back(size_t count)
{
    this->l -= std::min(count, this->l);
    this->s[this->l] = 0;
}

void Path::pop_back(const char* other)
{
    auto otherLength = other != nullptr ? strlen(other) : (size_t)0;
    while (this->l > 0 && otherLength > 0 && this->s[this->l - 1] == other[otherLength - 1])
    {
        this->s[this->l - 1] = 0;
        this->l--;
        otherLength--;
    }
}

size_t Path::find(char c, size_t pos) const
{
    for (size_t i = pos; i < this->l; i++)
    {
        if (this->s[i] == c)
            return i;
    }
    return npos;
}

size_t Path::find(const char* other, size_t pos) const
{
    auto range = boost::make_iterator_range(this->s + pos, this->s + this->l);

    auto safeOther = Utf8String::GetNonNull(other);
    auto otherRange = boost::make_iterator_range(safeOther, safeOther + strlen(safeOther));

    auto foundAt = boost::algorithm::find_first(range, otherRange);
    if (foundAt)
        return foundAt.begin() - this->s;
    else
        return npos;
}

size_t Path::find(const Utf8String& other, size_t pos) const
{
    auto range = boost::make_iterator_range(this->s + pos, this->s + this->l);

    auto otherRange = boost::make_iterator_range(other.s, other.s + other.l);

    auto foundAt = boost::algorithm::find_first(range, otherRange);
    if (foundAt)
        return foundAt.begin() - this->s;
    else
        return npos;
}

size_t Path::rfind(char c, size_t pos) const
{
    if (pos > this->l)
        pos = this->l;

    for (size_t i = pos; i != npos; i--)
    {
        if (this->s[i] == c)
            return i;
    }
    return npos;
}

size_t Path::rfind(const char* other, size_t pos) const
{
    if (pos > this->l)
        pos = this->l;

    auto range = boost::make_iterator_range(this->s, this->s + pos);

    const char* safeOther = Utf8String::GetNonNull(other);
    auto otherRange = boost::make_iterator_range(safeOther, safeOther + strlen(safeOther));

    auto foundAt = boost::algorithm::find_last(range, otherRange);
    if (foundAt)
        return foundAt.begin() - this->s;
    else
        return npos;
}

size_t Path::rfind(const Utf8String& other, size_t pos) const
{
    if (pos > this->l)
        pos = this->l;

    auto range = boost::make_iterator_range(this->s, this->s + pos);

    auto otherRange = boost::make_iterator_range(other.begin(), other.end());

    auto foundAt = boost::algorithm::find_last(range, otherRange);
    if (foundAt)
        return foundAt.begin() - this->s;
    else
        return npos;
}

char* Path::erase(char* at)
{
    if (at != nullptr)
    {
        if (at == end())
        {
            if (this->l > 0)
            {
                this->l--;
                this->s[this->l] = 0;
            }
            return end();
        }
        else
            return erase(at, at + 1);
    }
    else
        return end();
}

char* Path::erase(char* from, char* to)
{
    char* last = this->s + this->l;

    //Ensure 'from' is within range
    if (from >= this->s && from <= last)
    {
        //Ensure 'to' is within range
        if (to < from)
            to = from; //We assume from/to is non-decreasing
        else if (to > last)
            to = last;

        if (from == to)
        {
            //Empty range specified. Done
            return from + 1;
        }
        else
        {
            //Shift the characters to the left
            size_t count = to - from;
            for (size_t i = 0; i < this->l - count; i++)
                from[i] = from[i + count];
            this->l -= count;
            this->s[this->l] = 0; //Terminate new string

            return from + 1;
        }
    }
    else
    {
        //Invalid range
        return last + 1;
    }
}

const char& Path::operator [] (size_t i) const
{
    return this->s[i];
}

char& Path::operator [] (size_t i)
{
    return this->s[i];
}

void Path::EnsureTrailingSeparator()
{
    if (!EndsWith("/") && !EndsWith("\\"))
        append(PLATFORM_INDEPENDENT_SEPARATOR_STRING);
}

void Path::RemoveTrailingSeparator()
{
    if (EndsWith("/") || EndsWith("\\"))
        pop_back();
}

void Path::UnifySeparators()
{
    ReplaceAll('\\', '/');
}

void Path::RemoveLeadingSeparators()
{
    size_t i = 0;
    for (; i < this->l; i++)
    {
        if (this->s[i] != '\\' && this->s[i] != '/')
            break;
    }

    if (i > 0 && i < this->l)
    {
        Path sub;
        substr(sub, i);
        *this = sub;
    }
}

bool Path::IsStatic() const
{
    return this->s == this->shortS;
}

size_t Path::RepairLength(size_t newLength)
{
    if (newLength == (size_t)-1)
        this->l = strlen(this->s);
    else
    {
        this->l = newLength;
        this->s[this->l] = 0;
    }
    return this->l;
}

void Path::Init()
{
    this->allocator = nullptr;
    this->s = this->shortS;
    this->l = 0;
    this->shortS[0] = 0;
    this->allocatedLength = 0;
}

void Path::Truncate()
{
    if (this->s == nullptr)
        this->s = this->shortS;
    this->s[0] = 0;
    this->shortS[0] = 0;
    this->l = 0;
}

ValueOrError<void> Path::EnsureLengthAllocated(size_t len, bool oversize)
{
    //Ensure the buffer can hold a string of len + 1. This does not preserve the content of the current string
    if (len > STATIC_STRING_LENGTH && len > this->allocatedLength)
    {
        this->allocatedLength = EstimateNewAllocatedLength(len, this->allocatedLength, oversize);

        if (!IsStatic())
            _Deallocate(this->s);
        this->s = _Allocate(this->allocatedLength + 1, FINJIN_CALLER_ARGUMENTS);
        if (this->s == nullptr)
        {
            Truncate();
            return ValueOrError<void>();
        }
    }

    this->s[0] = 0;
    this->l = 0;

    return ValueOrError<void>();
}

void Path::Free()
{
    if (!IsStatic())
    {
        _Deallocate(this->s);
        this->allocatedLength = 0;
    }

    this->s = this->shortS;
    this->shortS[0] = 0;
    this->l = 0;
}

char* Path::_Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    char* result;
    if (this->allocator != nullptr)
        result = static_cast<char*>(this->allocator->Allocate(sizeof(char) * charCount, FINJIN_CALLER_PARAMETERS));
    else
        result = static_cast<char*>(Allocator::SystemAllocate(sizeof(char) * charCount, FINJIN_CALLER_PARAMETERS));
    assert(result && "Failed to allocate Path buffer.");
    return result;
}

void Path::_Deallocate(void* mem)
{
    if (this->allocator != nullptr)
        this->allocator->Deallocate(mem);
    else
        Allocator::SystemDeallocate(mem);
}

void Path::ReplaceFirst(char find, char replace)
{
    for (size_t i = 0; i < this->l; i++)
    {
        if (this->s[i] == find)
        {
            this->s[i] = replace;
            break;
        }
    }
}

void Path::ReplaceAll(char find, char replace)
{
    for (size_t i = 0; i < this->l; i++)
    {
        if (this->s[i] == find)
            this->s[i] = replace;
    }
}

void Path::ReplaceFirst(const Utf8String& find, const Utf8String& replace)
{
    boost::algorithm::replace_first(*this, find, replace);
}

void Path::ReplaceAll(const Utf8String& find, const Utf8String& replace)
{
    boost::algorithm::replace_all(*this, find, replace);
}

void Path::RemoveAllChars(const char* chars)
{
    //Go backwards in search for matching characters
    for (size_t stringCharIndex = this->l - 1; stringCharIndex != (size_t)-1; stringCharIndex--)
    {
        for (size_t charIndex = 0; chars[charIndex] != 0; charIndex++)
        {
            if (this->s[stringCharIndex] == chars[charIndex])
            {
                erase(&this->s[stringCharIndex]);
                break;
            }
        }
    }
}

void Path::ReplaceAllChars(const char* chars, char replacement)
{
    for (size_t stringCharIndex = 0; stringCharIndex < this->l; stringCharIndex++)
    {
        for (size_t charIndex = 0; chars[charIndex] != 0; charIndex++)
        {
            if (this->s[stringCharIndex] == chars[charIndex])
            {
                this->s[stringCharIndex] = replacement;
                break;
            }
        }
    }
}

int Path::Compare(const Path& other) const
{
    return strcmp(this->s, other.s);
}

int Path::Compare(const Utf8String& other) const
{
    return strcmp(this->s, other.s);
}

int Path::Compare(const char* other) const
{
    return strcmp(this->s, Utf8String::GetNonNull(other));
}

bool Path::Equals(const Path& other) const
{
    return this->l == other.l && strcmp(this->s, other.s) == 0;
}

bool Path::Equals(const Utf8String& other) const
{
    return this->l == other.length() && strcmp(this->s, other.s) == 0;
}

bool Path::Equals(const char* other) const
{
    return strcmp(this->s, Utf8String::GetNonNull(other)) == 0;
}

bool Path::StartsWith(const Path& other) const
{
    if (other.empty())
        return false;
    else if (this->l < other.l)
        return false;

    const char* thisS = this->s;
    const char* otherS = other.s;
    while (*thisS == *otherS && *thisS && *otherS)
    {
        ++thisS;
        ++otherS;
    }

    //Was end reached?
    return *otherS == 0;
}

bool Path::StartsWith(const Utf8String& other) const
{
    if (other.empty())
        return false;
    else if (this->l < other.l)
        return false;

    const char* thisS = this->s;
    const char* otherS = other.s;
    while (*thisS == *otherS && *thisS && *otherS)
    {
        ++thisS;
        ++otherS;
    }

    //Was end reached?
    return *otherS == 0;
}

bool Path::StartsWith(const char* other) const
{
    const char* otherS = Utf8String::GetNonNull(other);
    if (*otherS == 0)
    {
        //Input string is empty
        return false;
    }

    const char* thisS = this->s;
    while (*thisS == *otherS && *thisS && *otherS)
    {
        ++thisS;
        ++otherS;
    }

    //Was end reached?
    return *otherS == 0;
}

bool Path::EndsWith(const Path& other) const
{
    if (other.empty())
        return false;
    else if (this->l < other.l)
        return false;
    else if (this->l == other.l)
        return Equals(other);
    else
        return strcmp(this->s + this->l - other.l, other.s) == 0;
}

bool Path::EndsWith(const Utf8String& other) const
{
    if (other.empty())
        return false;
    else if (this->l < other.l)
        return false;
    else if (this->l == other.l)
        return Equals(other);
    else
        return strcmp(this->s + this->l - other.l, other.s) == 0;
}

bool Path::EndsWith(const char* other) const
{
    if (other == nullptr || other[0] == 0)
        return false;
    else
    {
        auto otherLength = strlen(other);

        if (this->l < otherLength)
            return false;
        else if (this->l == otherLength)
            return Equals(other);
        else
            return strcmp(this->s + this->l - otherLength, other) == 0;
    }
}

void Path::TrimTrailingWhitespace()
{
    while (this->l > 0)
    {
        if (isspace(this->s[this->l - 1]))
            this->l--;
        else
            break;
    }
}

void Path::RemoveWhitespace(size_t offset)
{
    Path result;

    for (size_t i = offset; i < this->l; i++)
    {
        if (!isspace(this->s[i]))
            result.append(this->s[i]);
    }

    *this = result;
}

bool Path::IterateCodepoint(size_t& iter, uint32_t& codepoint) const
{
    if (iter >= this->l)
        return false;

    int remainingUnits = 0;
    uint8_t mask = 0;

    uint8_t next = this->s[iter++];
    if (next & 0x80)
    {
        mask = 0xe0;
        for (remainingUnits = 1; (next & mask) != (mask << 1); ++remainingUnits)
            mask = (mask >> 1) | 0x80;
    }

    codepoint = next ^ mask;

    while (remainingUnits-- > 0)
    {
        codepoint <<= 6;
        if (iter >= this->l)
            return false;
        codepoint |= this->s[iter++] & 0x3f;
    }

    return IsValidCodepoint(codepoint);
}

bool Path::GetCodepointCount(size_t& count) const
{
    uint32_t codepoint;
    size_t iter = 0;
    for (count = 0; iter < this->l; count++)
    {
        if (!IterateCodepoint(iter, codepoint))
            return false;
    }

    return true;
}

bool Path::GetCodepoints(uint32_t* output, size_t& outputLength) const
{
    //Output is UTF-32

    const size_t maxLength = outputLength;

    size_t iter = 0;
    for (outputLength = 0; iter < this->l && outputLength < maxLength; outputLength++)
    {
        if (!IterateCodepoint(iter, output[outputLength]))
            return false;
    }

    if (outputLength < maxLength)
    {
        //Terminate output
        output[outputLength] = 0;
    }
    else
    {
        //Truncate output
        output[maxLength - 1] = 0;
    }

    return true;
}

ValueOrError<void> Path::operator /= (const Path& other)
{
    if (!other.empty())
        return AppendPath(*this, other.begin(), other.end());

    return ValueOrError<void>();
}

ValueOrError<void> Path::operator /= (const Utf8String& other)
{
    if (!other.empty())
        return AppendPath(*this, other.begin(), other.end());

    return ValueOrError<void>();
}

ValueOrError<void> Path::operator /= (const Utf8StringView& other)
{
    if (!other.empty())
        return AppendPath(*this, other.begin(), other.end());

    return ValueOrError<void>();
}

ValueOrError<void> Path::operator /= (const char* other)
{
    if (other != nullptr && other[0] != 0)
        return AppendPath(*this, other);

    return ValueOrError<void>();
}

ValueOrError<void> Path::operator /= (const wchar_t* other)
{
    if (other != nullptr && other[0] != 0)
        return AppendPath(*this, other);

    return ValueOrError<void>();
}

size_t Path::GetHash() const
{
    return Hash::Bytes(this->s, this->l);
}

Utf8String Path::ToString() const
{
    return this->s;
}

ValueOrError<void> Path::ToString(Utf8String& result) const
{
    return result.assign(this->s, this->l);
}

Path& Path::UniversalNormalize()
{
    for (size_t i = 0; i < this->l; i++)
    {
        if (this->s[i] == '\\')
            this->s[i] = '/';
    }

    if (this->l > 1)
    {
        if (this->s[this->l - 1] == '/')
        {
            this->l--;
            this->s[this->l] = 0;
        }
    }

    return *this;
}

Path& Path::PlatformNormalize()
{
    for (size_t i = 0; i < this->l; i++)
    {
        if (IsSeparator(this->s[i]))
            this->s[i] = PLATFORM_SEPARATOR_CHAR;
    }

    if (this->l > 1)
    {
        if (IsSeparator(this->s[this->l - 1]))
        {
            this->l--;
            this->s[this->l] = 0;
        }
    }

    return *this;
}

ValueOrError<void> Path::NormalizeRelativeComponents(Path& result) const
{
    result.clear();

    if (result.assign(*this).HasError())
        return ValueOrError<void>::CreateError();

    size_t previousIndex = npos;
    auto directoryLength = FindFirstDirectoryLength();
    if (directoryLength != npos)
    {
        Path component;
        do
        {
            if (substr(component, previousIndex + 1, directoryLength - previousIndex - 1).HasError())
                return ValueOrError<void>::CreateError();

            if (component == ".")
            {
                //Do nothing
            }
            else if (component == "..")
            {
                //Navigate up
                result.GoToParent();
            }
            else
            {
                if ((result /= component).HasError())
                    return ValueOrError<void>::CreateError();
            }

            previousIndex = directoryLength;
            directoryLength = FindNextDirectoryLength(directoryLength);
        } while (directoryLength != npos);
    }

    return ValueOrError<void>();
}

bool Path::IsAbsolute() const
{
    if (empty())
        return false;

    return ::IsAbsolute(this->s, this->s + this->l);
}

bool Path::IsRelative() const
{
    if (empty())
        return false;

    return !IsAbsolute();
}

bool Path::HasExtension() const
{
    return GetExtensionOffset(false) != npos;
}

size_t Path::GetExtensionOffset(bool includeDot) const
{
    for (size_t i = this->l - 1; i != npos; i--)
    {
        if (IsSeparator(this->s[i]))
            break;
        else if (this->s[i] == '.')
        {
            if (i == this->l - 1 && !includeDot)
            {
                //Oddball case where the dot is the last character
                break;
            }

            return includeDot ? i : i + 1;
        }
    }

    return npos;
}

void Path::RemoveExtension()
{
    auto extFoundAt = GetExtensionOffset(true);
    if (extFoundAt != npos)
    {
        this->s[extFoundAt] = 0;
        this->l = extFoundAt;
    }
}

size_t Path::GetFileNameLength() const
{
    for (size_t i = length() - 1; i != npos; i--)
    {
        if (IsSeparator(this->s[i]) || this->s[i] == ':')
            return this->l - i - 1;
    }

    return this->l;
}

void Path::RemoveFileName()
{
    for (size_t i = length() - 1; i != npos; i--)
    {
        if (IsSeparator(this->s[i]) || this->s[i] == ':')
        {
            this->l = i;
            this->s[i] = 0;
            break;
        }
    }
}

size_t Path::GetParentOffset() const
{
    //Note: This will return a result for something like "c:\file.txt", where "c:" is the parent

    if (empty())
        return (size_t)-1;

    size_t endIndex = npos;
    if (HasLongPathPrefix(this->s))
        endIndex = LONG_PATH_PREFIX_LENGTH - 1;

    size_t i = length() - 1;
    if (endIndex != npos && i <= endIndex)
        return (size_t)-1;
    if (IsSeparator(this->s[i]) || this->s[i] == ':')
        i--;
    for (; i != endIndex; i--)
    {
        if (IsSeparator(this->s[i]) || this->s[i] == ':')
            return i;
    }

    return (size_t)-1;
}

bool Path::HasParent() const
{
    return GetParentOffset() != (size_t)-1;
}

ValueOrError<bool> Path::GetParent(Path& result) const
{
    result.clear();

    if (empty())
        return false;

    size_t endIndex = npos;
    if (HasLongPathPrefix(this->s))
        endIndex = LONG_PATH_PREFIX_LENGTH - 1;

    size_t i = length() - 1;
    if (endIndex != npos && i <= endIndex)
        return false;
    if (IsSeparator(this->s[i]) || this->s[i] == ':')
        i--;
    for (; i != endIndex; i--)
    {
        if (IsSeparator(this->s[i]) || this->s[i] == ':')
        {
            if (substr(result, 0, i).HasError())
                return ValueOrError<bool>::CreateError();
            else
                return !result.empty();
        }
    }

    return false;
}

bool Path::GoToParent()
{
    if (empty())
        return false;

    size_t endIndex = npos;
    if (HasLongPathPrefix(this->s))
        endIndex = LONG_PATH_PREFIX_LENGTH - 1;

    size_t i = length() - 1;
    if (endIndex != npos && i <= endIndex)
    {
        clear();
        return false;
    }
    if (IsSeparator(this->s[i]) || this->s[i] == ':')
        i--;
    for (; i != endIndex; i--)
    {
        if (IsSeparator(this->s[i]) || this->s[i] == ':')
        {
            this->s[i] = 0;
            this->l = i;
            return true;
        }
    }

    clear();
    return false;
}

bool Path::RemoveParent()
{
    auto parentOffset = GetParentOffset();
    if (parentOffset != (size_t)-1)
    {
        pop_front(parentOffset + 1);
        return true;
    }

    return false;
}

ValueOrError<bool> Path::GetRoot(Path& result) const
{
    result.clear();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    size_t index = 0;
    if (HasLongPathPrefix(this->s))
        index += LONG_PATH_PREFIX_LENGTH;
    if (::IsAbsolute(begin() + index, end()))
    {
        if (IsSeparator(this->s[index]) && IsSeparator(this->s[index + 1]))
        {
            size_t startIndex = index;

            index += 2;
            while (index < this->l && !IsSeparator(this->s[index]))
                index++;

            if (substr(result, startIndex, index - startIndex).HasError())
                return ValueOrError<bool>::CreateError();

            return !result.empty();
        }
        else if (this->s[index + 1] == ':')
        {
            if (substr(result, index, index + 2).HasError())
                return ValueOrError<bool>::CreateError();

            return !result.empty();
        }
    }
#else
    if (IsAbsolute())
    {
        if (result.assign(PLATFORM_INDEPENDENT_SEPARATOR_STRING).HasError())
            return ValueOrError<bool>::CreateError();

        return !result.empty();
    }
#endif

    return false;
}

ValueOrError<bool> Path::GetInternalVolumeID(Utf8String& result) const
{
    //Callers should assume this method is slow, so it should be called as seldom as possible

    result.clear();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    //For Windows UWP use the root as the ID
    if (!empty())
    {
        Path root;
        if (GetRoot(root).HasError())
            return ValueOrError<bool>::CreateError();

        if (result.assign(root.c_str()).HasError())
            return ValueOrError<bool>::CreateError();

        return !result.empty();
    }
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    //For Win32 use the volume serial number as the ID
    Path root;
    if (GetRoot(root).HasError())
        return ValueOrError<bool>::CreateError();

    if (!root.empty())
    {
        if (root.append(PLATFORM_SEPARATOR_STRING).HasError())
            return ValueOrError<bool>::CreateError();
    }

    WideningToUtf16Converter rootW;
    if (!rootW.convert(root.begin(), root.end()))
        return ValueOrError<bool>::CreateError();

    DWORD serial = 0;
    if (GetVolumeInformationW(rootW.c_str(), nullptr, 0, &serial, nullptr, nullptr, nullptr, 0))
    {
        if (result.assign(Convert::ToString(serial)).HasError())
            return ValueOrError<bool>::CreateError();

        return !result.empty();
    }

#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    //For Apple platforms use the volume ID hash as the ID
    auto hash = AppleUtilities::GetVolumeIDHash(*this);
    if (hash != 0)
    {
        if (result.assign(Convert::ToString(hash)).HasError())
            return ValueOrError<bool>::CreateError();

        return !result.empty();
    }
#else
    //For Linux and everything else use a / or empty string as the volume ID
    if (IsAbsolute())
    {
        if (result.assign(PLATFORM_INDEPENDENT_SEPARATOR_STRING).HasError())
            return ValueOrError<bool>::CreateError();

        return !result.empty();
    }
#endif

    return false;
}

ValueOrError<bool> Path::EnsureExtension(const Utf8String& defaultExtension)
{
    if (!empty() && !defaultExtension.empty())
    {
        auto defaultStartsWithDot = defaultExtension[0] == '.';

        Utf8StringView ext;
        if (GetExtension(ext, defaultStartsWithDot).HasError())
            return ValueOrError<bool>::CreateError();
        if (ext.empty())
        {
            //No extension, use default extension
            if (!defaultStartsWithDot)
            {
                if (append('.').HasError())
                    return ValueOrError<bool>::CreateError();
            }

            if (append(defaultExtension).HasError())
                return ValueOrError<bool>::CreateError();

            return true;
        }
    }
    return false;
}

ValueOrError<bool> Path::EnsureExecutableExtension()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    return EnsureExtension(".exe");
#else
    return true;
#endif
}

size_t Path::FindFirstDirectoryLength() const
{
    //Skip UNC prefix
    size_t index = 0;
    if (HasLongPathPrefix(this->s))
        index += LONG_PATH_PREFIX_LENGTH;

    if (::IsAbsolute(begin() + index, end()))
    {
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        index += 3; //Skip drive letter and ":\"
    #else
        index += 1; //Skip leading '/'
    #endif
    }

    //Find slash or end
    for (; index < this->l; index++)
    {
        if (IsSeparator(this->s[index]))
            return index;
    }

    return index;
}

size_t Path::FindNextDirectoryLength(size_t previousLength) const
{
    if (previousLength >= this->l)
        return npos;

    auto index = previousLength;

    //Skip slash
    while (index < this->l && IsSeparator(this->s[index]))
        index++;

    //Make sure there's actually something left
    if (index == this->l)
        return npos;

    //Find next slash or end of path
    for (; index < this->l; index++)
    {
        if (IsSeparator(this->s[index]))
            return index;
    }

    //End of path
    return this->l;
}

ValueOrError<bool> Path::ContainsDirectory(const Path& other) const
{
    //Determines whether 'other' is contained within 'parentDirectory'. It does not check the actual file system
    auto parentDirectory = *this;
    parentDirectory.PlatformNormalize();

    //Get the other directory
    auto otherDirectory = other;
    otherDirectory.PlatformNormalize();

    //Keep moving up the 'other' hierarchy until a match is found
    Path tempParent;
    while (!otherDirectory.empty() && otherDirectory.length() >= parentDirectory.length())
    {
        if (parentDirectory == otherDirectory)
            return true;

        if (otherDirectory.GetParent(tempParent).HasError())
            return ValueOrError<bool>::CreateError();
        otherDirectory = tempParent;
    }

    return false;
}

ValueOrError<bool> Path::ContainsFile(const Path& other) const
{
    //Determines whether 'other' is contained within 'parentDirectory'. It does not check the actual file system
    auto parentDirectory = *this;
    parentDirectory.PlatformNormalize();

    //Get the file's directory
    auto otherDirectory = other;
    otherDirectory.PlatformNormalize();
    otherDirectory.RemoveFileName();

    //Keep moving up the 'other' hierarchy until a match is found
    Path tempParent;
    while (!otherDirectory.empty() && otherDirectory.length() >= parentDirectory.length())
    {
        if (parentDirectory == otherDirectory)
            return true;

        if (otherDirectory.GetParent(tempParent).HasError())
            return ValueOrError<bool>::CreateError();
        otherDirectory = tempParent;
    }

    return false;
}

ValueOrError<bool> Path::ExpandUserHomeDirectory()
{
    if (this->l > 0 && this->s[0] == '~')
    {
        Path userHome;
        auto getUserHomeResult = GetUserHomeDirectory(userHome);
        if (getUserHomeResult.HasError())
        {
            //There was an error getting the home directory
            return ValueOrError<bool>::CreateError();
        }
        else if (getUserHomeResult.HasValue(true))
        {
            //The home directory was retrieved successfully

            auto newLength = this->l + userHome.l - 1;
            if (newLength <= STATIC_STRING_LENGTH || newLength <= this->allocatedLength)
            {
                boost::algorithm::replace_first(*this, "~", userHome.c_str());
            }
            else
            {
                Path newString(this->allocator);
                if (newString.EnsureLengthAllocated(newLength, false).HasError())
                    return ValueOrError<bool>::CreateError();
                if (newString.assign(this->s, this->l).HasError())
                    return ValueOrError<bool>::CreateError();

                boost::algorithm::replace_first(newString, "~", userHome.c_str());

                assign(std::move(newString));
            }

            return true;
        }
    }

    //No leading '~' or the home directory could not be obtained
    return false;
}

ValueOrError<bool> Path::GetUserHomeDirectory(Path& directory)
{
    directory.clear();

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    if (directory.assign(Windows::Storage::KnownFolders::DocumentsLibrary->Path->Data()).HasError())
        return ValueOrError<bool>::CreateError();
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    Utf8String homeDrive, homePath;
    if (WindowsUtilities::GetEnv(homeDrive, "HOMEDRIVE").HasError())
        return ValueOrError<bool>::CreateError();
    if (WindowsUtilities::GetEnv(homePath, "HOMEPATH").HasError())
        return ValueOrError<bool>::CreateError();
    if (!homeDrive.empty() && !homePath.empty())
    {
        if (directory.assign(homeDrive).HasError())
            return ValueOrError<bool>::CreateError();
        if (directory.append(homePath).HasError())
            return ValueOrError<bool>::CreateError();
    }
#else
    if (directory.assign(getenv("HOME")).HasError())
        return ValueOrError<bool>::CreateError();
#endif

    directory.PlatformNormalize();

    return !directory.empty();
}

bool Path::GetFileLength(uint64_t& fileLength) const
{
    return FileAccessor::GetFileLength(*this, fileLength);
}

uint64_t Path::GetFileLength() const
{
    uint64_t fileLength;
    if (GetFileLength(fileLength))
        return fileLength;

    return 0;
}

bool Path::WriteBinaryFile(const void* bytes, size_t byteCount) const
{
    FileAccessor file;
    if (!file.OpenForWrite(*this))
        return false;

    file.Write(bytes, byteCount);

    return true;
}

bool Path::ReadBinaryFile(uint8_t* buffer, size_t maxByteCount, size_t& readByteCount) const
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS || !USE_BOOST_MAPPED_FILE
    FileAccessor file;
    if (!file.OpenForRead(*this))
        return false;

    readByteCount = file.Read(buffer, maxByteCount);
    return true;
#elif USE_BOOST_MAPPED_FILE
    if (IsFile())
    {
        boost::iostreams::mapped_file mappedFile(c_str(), boost::iostreams::mapped_file::readonly);

        readByteCount = std::min(maxByteCount, mappedFile.size());
        FINJIN_COPY_MEMORY(buffer, mappedFile.const_data(), readByteCount);

        return true;
    }
    readByteCount = 0;
    return false;
#else
    #error Not implemented!
#endif
}

bool Path::ReadBinaryFile(ByteBuffer& buffer) const
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS || !USE_BOOST_MAPPED_FILE
    uint64_t fileSize;
    FileAccessor file;
    if (!file.OpenForRead(*this, &fileSize))
        return false;

    if (buffer.resize(fileSize) < fileSize)
        return false;
    file.Read(buffer.data(), buffer.size());

    return true;
#elif USE_BOOST_MAPPED_FILE
    if (IsFile())
    {
        boost::iostreams::mapped_file mappedFile(c_str(), boost::iostreams::mapped_file::readonly);

        auto mappedFileSize = mappedFile.size();
        if (buffer.resize(mappedFileSize) < mappedFileSize)
            return false;
        FINJIN_COPY_MEMORY(buffer.data(), mappedFile.const_data(), mappedFile.size());

        return true;
    }
    buffer.clear();
    return false;
#else
    #error Not implemented!
#endif
}

bool Path::RemoveFile() const
{
    if (empty())
        return false;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    WideningToUtf16Converter pathW;
    if (!pathW.convert(begin(), end()))
        return false;

    return ::DeleteFileW(pathW.c_str()) != FALSE;
#else
    return unlink(this->s) == 0;
#endif
}

bool Path::RenameFile(const Path& destPath) const
{
    if (destPath.empty())
        return false;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    WideningToUtf16Converter sourcePathW;
    if (!sourcePathW.convert(begin(), end()))
        return false;

    WideningToUtf16Converter destPathW;
    if (!destPathW.convert(destPath.begin(), destPath.end()))
        return false;

    return MoveFileExW(sourcePathW.c_str(), destPathW.c_str(), MOVEFILE_REPLACE_EXISTING) != FALSE;
#else
    return rename(c_str(), destPath.c_str()) == 0;
#endif
}

bool Path::IsFile() const
{
    return IsFile(this->s);
}

bool Path::IsFile(const char* path)
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    auto attributes = WindowsUtilities::GetPathAttributes(path);
    return WindowsUtilities::IsFileAttribute(attributes);
#else
    struct stat statBuf;
    if (stat(path, &statBuf) == -1)
        return false;

    if (S_ISLNK(statBuf.st_mode) != 0)
    {
        //Follow symbolic link
        auto result = false;
        auto absPath = realpath(path, nullptr);
        if (absPath != nullptr)
        {
            result = IsFile(absPath);
            free(absPath);
        }
        return result;
    }
    else
    {
        //Not a symbolic link
        return S_ISREG(statBuf.st_mode) != 0;
    }
#endif
}

bool Path::IsDirectory() const
{
    return IsDirectory(this->s);
}

bool Path::IsDirectory(const char* path)
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    auto attributes = WindowsUtilities::GetPathAttributes(path);
    return WindowsUtilities::IsDirectoryAttribute(attributes);
#else
    struct stat statBuf;
    if (stat(path, &statBuf) == -1)
        return false;

    if (S_ISLNK(statBuf.st_mode) != 0)
    {
        //Follow symbolic link
        auto result = false;
        auto absPath = realpath(path, nullptr);
        if (absPath != nullptr)
        {
            result = IsDirectory(absPath);
            free(absPath);
        }
        return result;
    }
    else
    {
        //Not a symbolic link
        return S_ISDIR(statBuf.st_mode) != 0;
    }
#endif
}

bool Path::Exists() const
{
    if (empty())
        return false;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    auto attributes = WindowsUtilities::GetPathAttributes(*this);
    return attributes != INVALID_FILE_ATTRIBUTES;
#else
    struct stat statBuf;
    return stat(this->s, &statBuf) != -1;
#endif
}

bool Path::CreateDirectories() const
{
    if (empty())
        return false;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS && !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    WideningToUtf16Converter pathW;
    if (!pathW.convert(begin(), end()))
        return false;

    auto res = SHCreateDirectory(nullptr, pathW.c_str());
    return res == ERROR_SUCCESS || res == ERROR_ALREADY_EXISTS;
#else
    auto directoryLength = FindFirstDirectoryLength();
    if (directoryLength != npos)
    {
        WideningToUtf16Converter pathW;

        do
        {
            Path path;
            substr(path, 0, directoryLength);
        #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
            if (!pathW.convert(path.begin(), path.end()))
                return false;

            auto res = _wmkdir(pathW.c_str());
        #else
            auto res = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        #endif
            if (res != 0 && errno != EEXIST)
            {
                //The failure was due to something other than the directory already existing
                return false;
            }

            directoryLength = FindNextDirectoryLength(directoryLength);
        } while (directoryLength != npos);

        return true;
    }
    return false;
#endif
}

void Path::InitializeUtf8FileSystemAccess()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS && !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    std::locale newLocale(std::locale(), new boost::filesystem::detail::utf8_codecvt_facet);
    boost::filesystem::path::imbue(newLocale);
#endif
}
