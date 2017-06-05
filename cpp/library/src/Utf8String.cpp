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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Allocator.hpp"
#include "finjin/common/Hash.hpp"
#include <nowide/stackstring.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/reversed.hpp>
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include "finjin/common/WindowsUtilities.hpp"
#endif

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
using NarrowingConverter = nowide::basic_stackstring<char, wchar_t, Utf8String::STATIC_STRING_LENGTH + 1>;
using WideningToUtf16Converter = nowide::basic_stackstring<wchar_t, char, Utf8String::STATIC_STRING_LENGTH + 1>;
using WideningToUtf32Converter = nowide::basic_stackstring<char32_t, char, Utf8String::STATIC_STRING_LENGTH + 1>;


//Local functions---------------------------------------------------------------
static size_t EstimateNewAllocatedLength(size_t requestedLength, size_t allocatedLength, bool oversize)
{
    if (oversize)
    {
        //Oversize string
        allocatedLength = 2 * std::max(static_cast<size_t>(Utf8String::STATIC_STRING_LENGTH), std::max(allocatedLength, requestedLength));
    }
    else
    {
        //No oversizing desired, just use the requested length
        allocatedLength = requestedLength;
    }
    return allocatedLength;
}

static bool IsValidCodepoint(uint32_t ch)
{
    return ch < 0xd800 || ch > 0xdfff;
}

static size_t strlen32(const char32_t* s)
{
    size_t count = 0;

    while (s != nullptr && *s != 0)
    {
        count++;
        s++;
    }

    return count;
}

static int strncmp32(const char32_t* a, const char32_t* b)
{
    while (*a && *b)
    {
        auto result = static_cast<int32_t>(*a++) - static_cast<int32_t>(*b++);
        if (result != 0)
            return result;
    }

    if (*b)
        return *b;
    else
        return -static_cast<int>(*a);
}

/*static bool IsComboCodepoint(uint32_t ch)
{
    return (ch >= 0x0300 && ch <= 0x036f) || (ch >= 0x20d0 && ch <= 0x20ff) || (ch >= 0xfe20 && ch <= 0xfe2f);
}*/


//Implementation----------------------------------------------------------------

//Utf8String
const Utf8String& Utf8String::GetEmpty()
{
    static const Utf8String value;
    return value;
}

Utf8String::Utf8String(Allocator* allocator)
{
    Init();
    this->allocator = allocator;
}

Utf8String::Utf8String(const char* other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Utf8String::Utf8String(const wchar_t* other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Utf8String::Utf8String(const char* other, size_t len, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, len);
}

Utf8String::Utf8String(const wchar_t* other, size_t len, Allocator* allocator)
{
    Init();
    this->allocator = nullptr;
    assign(other, len);
}

Utf8String::Utf8String(const char* first, const char* last, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(first, last);
}

Utf8String::Utf8String(const uint8_t* first, const uint8_t* last, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(first, last);
}

Utf8String::Utf8String(const wchar_t* first, const wchar_t* last, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(first, last);
}

Utf8String::Utf8String(const uint8_t* other, size_t len, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, len);
}

Utf8String::Utf8String(size_t count, char c, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(count, c);
}

Utf8String::Utf8String(size_t count, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    EnsureLengthAllocated(count, false);
}

Utf8String::Utf8String(const Utf8String& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Utf8String::Utf8String(const Utf8StringView& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Utf8String::Utf8String(Utf8String&& other)
{
    Init();
    assign(std::move(other));
}

Utf8String::~Utf8String()
{
    Free();
}

bool Utf8String::Create(Allocator* allocator)
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

void Utf8String::Destroy()
{
    Free();

    this->allocator = nullptr;
}

Allocator* Utf8String::GetAllocator()
{
    return this->allocator;
}

bool Utf8String::SetAllocator(Allocator* allocator)
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

ValueOrError<void> Utf8String::operator = (const char* other)
{
    return assign(other);
}

ValueOrError<void> Utf8String::operator = (const wchar_t* other)
{
    return assign(other);
}

ValueOrError<void> Utf8String::operator = (const Utf8String& other)
{
    return assign(other);
}

ValueOrError<void> Utf8String::operator = (const Utf8StringView& other)
{
    return assign(other);
}

ValueOrError<void> Utf8String::operator = (Utf8String&& other)
{
    return assign(std::move(other));
}

ValueOrError<void> Utf8String::assign(const char* other)
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

ValueOrError<void> Utf8String::assign(const wchar_t* other)
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

ValueOrError<void> Utf8String::assign(const char* other, size_t len)
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

ValueOrError<void> Utf8String::assign(const wchar_t* other, size_t len)
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

ValueOrError<void> Utf8String::assign(const char* first, const char* last)
{
    if (last <= first)
    {
        Truncate();
        return ValueOrError<void>();
    }
    else
        return assign(first, last - first);
}

ValueOrError<void> Utf8String::assign(const wchar_t* first, const wchar_t* last)
{
    if (last <= first)
    {
        Truncate();
        return ValueOrError<void>();
    }
    else
        return assign(first, last - first);
}

ValueOrError<void> Utf8String::assign(const uint8_t* first, const uint8_t* last)
{
    return assign(reinterpret_cast<const char*>(first), reinterpret_cast<const char*>(last));
}

ValueOrError<void> Utf8String::assign(const uint8_t* other, size_t len)
{
    return assign(reinterpret_cast<const char*>(other), len);
}

ValueOrError<void> Utf8String::assign(size_t count, char c)
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

ValueOrError<void> Utf8String::assign(const Utf8String& other)
{
    if (&other == this)
        return ValueOrError<void>();

    if (EnsureLengthAllocated(other.length(), false).HasError())
        return ValueOrError<void>::CreateError();

    strcpy(this->s, other.s);
    this->l = other.length();
    return ValueOrError<void>();
}

ValueOrError<void> Utf8String::assign(const Utf8StringView& other)
{
    return assign(other.begin(), other.length());
}

ValueOrError<void> Utf8String::assign(Utf8String&& other)
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

const char* Utf8String::c_str() const
{
    return this->s;
}

bool Utf8String::empty() const
{
    return this->l == 0;
}

void Utf8String::clear()
{
    Truncate();
}

size_t Utf8String::length() const
{
    return this->l;
}

size_t Utf8String::size() const
{
    return this->l;
}

ValueOrError<void> Utf8String::reserve(size_t len)
{
    return EnsureLengthAllocated(len, false);
}

size_t Utf8String::GetCharactersLeft() const
{
    if (IsStatic())
        return STATIC_STRING_LENGTH - this->l;
    else
        return this->allocatedLength - this->l;
}

ValueOrError<void> Utf8String::resize(size_t len)
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

Utf8String::iterator Utf8String::begin()
{
    return this->s;
}

Utf8String::iterator Utf8String::end()
{
    return this->s + this->l;
}

Utf8String::const_iterator Utf8String::begin() const
{
    return this->s;
}

Utf8String::const_iterator Utf8String::end() const
{
    return this->s + this->l;
}

bool Utf8String::operator == (const Utf8String& other) const
{
    return this->l == other.l && strcmp(this->s, other.s) == 0;
}

bool Utf8String::operator != (const Utf8String& other) const
{
    return !(operator == (other));
}

bool Utf8String::operator < (const Utf8String& other) const
{
    return strcmp(this->s, other.s) < 0;
}

bool Utf8String::operator > (const Utf8String& other) const
{
    return strcmp(this->s, other.s) > 0;
}

bool Utf8String::operator == (const Utf8StringView& other) const
{
    return other == *this;
}

bool Utf8String::operator != (const Utf8StringView& other) const
{
    return !(operator == (other));
}

bool Utf8String::operator < (const Utf8StringView& other) const
{
    if (this->l == other.length())
        return strncmp(this->s, other.begin(), this->l) < 0;
    else if (this->l < other.length())
        return strncmp(this->s, other.begin(), this->l) <= 0;
    else
        return strncmp(this->s, other.begin(), other.length()) < 0;
}

bool Utf8String::operator > (const Utf8StringView& other) const
{
    if (this->l == other.length())
        return strncmp(this->s, other.begin(), this->l) > 0;
    else if (this->l < other.length())
        return strncmp(this->s, other.begin(), this->l) > 0;
    else
        return strncmp(this->s, other.begin(), other.length()) >= 0;
}

bool Utf8String::operator == (const char* other) const
{
    return strcmp(this->s, GetNonNull(other)) == 0;
}

bool Utf8String::operator != (const char* other) const
{
    return strcmp(this->s, GetNonNull(other)) != 0;
}

bool Utf8String::operator < (const char* other) const
{
    return strcmp(this->s, GetNonNull(other)) < 0;
}

bool Utf8String::operator > (const char* other) const
{
    return strcmp(this->s, GetNonNull(other)) > 0;
}

ValueOrError<void> Utf8String::operator += (Utf8String&& other)
{
    return append(std::move(other));
}

ValueOrError<void> Utf8String::operator += (const Utf8String& other)
{
    return append(other.s);
}

ValueOrError<void> Utf8String::operator += (const Utf8StringView& other)
{
    return append(other.begin(), other.length());
}

ValueOrError<void> Utf8String::operator += (const char* other)
{
    return append(other);
}

ValueOrError<void> Utf8String::operator += (const wchar_t* other)
{
    return append(other);
}

ValueOrError<void> Utf8String::operator += (char other)
{
    return append(other);
}

ValueOrError<void> Utf8String::append(Utf8String&& other)
{
    if (this->l == 0)
        return assign(std::move(other));
    else
        return append(other.s, other.l);
}

ValueOrError<void> Utf8String::append(const Utf8String& other)
{
    return append(other.s, other.l);
}

ValueOrError<void> Utf8String::append(const Utf8StringView& other)
{
    return append(other.begin(), other.length());
}

ValueOrError<void> Utf8String::append(const char* other)
{
    if (other != nullptr && other[0] != 0)
        return append(other, strlen(other));
    else
        return ValueOrError<void>();
}

ValueOrError<void> Utf8String::append(const char* other, size_t len)
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
            Utf8String newString(this->allocator);
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

ValueOrError<void> Utf8String::append(const wchar_t* other)
{
    NarrowingConverter narrowed;
    if (!narrowed.convert(other))
        return ValueOrError<void>::CreateError();
    return append(narrowed.c_str());
}

ValueOrError<void> Utf8String::append(const wchar_t* other, size_t len)
{
    NarrowingConverter narrowed;
    if (!narrowed.convert(other, other + len))
        return ValueOrError<void>::CreateError();
    return append(narrowed.c_str());
}

ValueOrError<void> Utf8String::append(char other)
{
    char s[2] = {other, 0};
    return append(s, 1);
}

ValueOrError<void> Utf8String::append(wchar_t other)
{
    wchar_t s[2] = {other, 0};
    return append(s, 1);
}

ValueOrError<void> Utf8String::append(size_t len, char c)
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

ValueOrError<void> Utf8String::push_back(char c)
{
    return append(c);
}

char Utf8String::back() const
{
    assert(!empty());

    if (!empty())
        return this->s[this->l - 1];
    else
        return 0;
}

void Utf8String::pop_front()
{
    if (!empty())
        erase(begin());
}

void Utf8String::pop_front(size_t count)
{
    count = std::min(count, this->l);
    for (size_t i = 0; i < this->l - count; i++)
        this->s[i] = this->s[i + count];
    this->l -= count;
    this->s[this->l] = 0;
}

void Utf8String::pop_back()
{
    pop_back(1);
}

void Utf8String::pop_back(size_t count)
{
    this->l -= std::min(count, this->l);
    this->s[this->l] = 0;
}

void Utf8String::pop_back(const char* other)
{
    auto otherLength = other != nullptr ? strlen(other) : (size_t)0;
    while (this->l > 0 && otherLength > 0 && this->s[this->l - 1] == other[otherLength - 1])
    {
        this->s[this->l - 1] = 0;
        this->l--;
        otherLength--;
    }
}

size_t Utf8String::find(char c, size_t pos) const
{
    for (size_t i = pos; i < this->l; i++)
    {
        if (this->s[i] == c)
            return i;
    }
    return npos;
}

size_t Utf8String::find(const char* other, size_t pos) const
{
    auto range = boost::make_iterator_range(this->s + pos, this->s + this->l);

    auto safeOther = GetNonNull(other);
    auto otherRange = boost::make_iterator_range(safeOther, safeOther + strlen(safeOther));

    auto foundAt = boost::algorithm::find_first(range, otherRange);
    if (foundAt)
        return foundAt.begin() - s;
    else
        return npos;
}

size_t Utf8String::find(const Utf8String& other, size_t pos) const
{
    auto range = boost::make_iterator_range(this->s + pos, this->s + this->l);

    auto otherRange = boost::make_iterator_range(other.s, other.s + other.l);

    auto foundAt = boost::algorithm::find_first(range, otherRange);
    if (foundAt)
        return foundAt.begin() - this->s;
    else
        return npos;
}

size_t Utf8String::rfind(char c, size_t pos) const
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

size_t Utf8String::rfind(const char* other, size_t pos) const
{
    if (pos > this->l)
        pos = this->l;

    auto range = boost::make_iterator_range(this->s, this->s + pos);

    auto safeOther = GetNonNull(other);
    auto otherRange = boost::make_iterator_range(safeOther, safeOther + strlen(safeOther));

    auto foundAt = boost::algorithm::find_last(range, otherRange);
    if (foundAt)
        return foundAt.begin() - this->s;
    else
        return npos;
}

size_t Utf8String::rfind(const Utf8String& other, size_t pos) const
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

char* Utf8String::erase(const char* at)
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

char* Utf8String::erase(const char* constFrom, const char* constTo)
{
    char* last = this->s + this->l;

    //Ensure 'from' is within range
    if (constFrom >= this->s && constFrom <= last)
    {
        auto from = this->s + (constFrom - this->s);
        auto to = this->s + (constTo - this->s);

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

const char& Utf8String::operator [] (size_t i) const
{
    return this->s[i];
}

char& Utf8String::operator [] (size_t i)
{
    return this->s[i];
}

bool Utf8String::IsDigits() const
{
    return Utf8String::IsDigits(begin(), end());
}

void Utf8String::ToUpperAscii()
{
    if (empty())
        return;

    size_t outCount = 0;
    for (size_t inIndex = 0; inIndex < this->l; inIndex++)
    {
        if (isascii(this->s[inIndex]))
            this->s[outCount++] = static_cast<char>(toupper(this->s[inIndex]));
    }
    this->l = outCount;
    this->s[this->l] = 0;
}

void Utf8String::ToLowerAscii()
{
    if (empty())
        return;

    size_t outCount = 0;
    for (size_t inIndex = 0; inIndex < this->l; inIndex++)
    {
        if (isascii(this->s[inIndex]))
            this->s[outCount++] = static_cast<char>(tolower(this->s[inIndex]));
    }
    this->l = outCount;
    this->s[this->l] = 0;
}

void Utf8String::ToLowerAlphaNumeric()
{
    if (empty())
        return;

    size_t outCount = 0;
    for (size_t inIndex = 0; inIndex < this->l; inIndex++)
    {
        if (isalnum(this->s[inIndex]))
            this->s[outCount++] = static_cast<char>(tolower(this->s[inIndex]));
    }
    this->l = outCount;
    this->s[this->l] = 0;
}

bool Utf8String::IsStatic() const
{
    return this->s == this->shortS;
}

size_t Utf8String::RepairLength(size_t newLength)
{
    if (newLength == (size_t)-1)
        this->l = strlen(this->s);
    else
        this->l = newLength;
    this->s[this->l] = 0;
    return this->l;
}

void Utf8String::Init()
{
    this->allocator = nullptr;
    this->s = this->shortS;
    this->l = 0;
    this->shortS[0] = 0;
    this->allocatedLength = 0;
}

void Utf8String::Truncate()
{
    if (this->s == nullptr)
        this->s = this->shortS;
    this->s[0] = 0;
    this->shortS[0] = 0;
    this->l = 0;
}

ValueOrError<void> Utf8String::EnsureLengthAllocated(size_t len, bool oversize)
{
    //Ensure the buffer can hold a string of len + 1. This does not preserve the content of the current string
    if (len > STATIC_STRING_LENGTH && len > this->allocatedLength)
    {
        this->allocatedLength = EstimateNewAllocatedLength(len, this->allocatedLength, oversize);

        if (!IsStatic())
            _Deallocate(this->s);
        this->s = _Allocate(this->allocatedLength + 1, FINJIN_CALLER_ARGUMENTS);
    }

    if (this->s == nullptr)
    {
        Truncate();
        return ValueOrError<void>::CreateError();
    }

    this->s[0] = 0;
    this->l = 0;

    return ValueOrError<void>();
}

void Utf8String::Free()
{
    if (!IsStatic())
    {
        _Deallocate(this->s);
        this->allocatedLength = 0;
    }

    this->s = this->shortS;
    this->shortS[0] = 0;
}

char* Utf8String::_Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    char* result;
    if (this->allocator != nullptr)
        result = static_cast<char*>(this->allocator->Allocate(sizeof(char) * charCount, FINJIN_CALLER_PARAMETERS));
    else
        result = static_cast<char*>(Allocator::SystemAllocate(sizeof(char) * charCount, FINJIN_CALLER_PARAMETERS));
    assert(result && "Failed to allocate Utf8String buffer.");
    return result;
}

void Utf8String::_Deallocate(void* mem)
{
    if (this->allocator != nullptr)
        this->allocator->Deallocate(mem);
    else
        Allocator::SystemDeallocate(mem);
}

void Utf8String::ReplaceFirst(char find, char replace)
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

void Utf8String::ReplaceAll(char find, char replace)
{
    for (size_t i = 0; i < this->l; i++)
    {
        if (this->s[i] == find)
            this->s[i] = replace;
    }
}

void Utf8String::ReplaceFirst(const Utf8String& find, const Utf8String& replace)
{
    boost::algorithm::replace_first(*this, find, replace);
}

void Utf8String::ReplaceAll(const Utf8String& find, const Utf8String& replace)
{
    boost::algorithm::replace_all(*this, find, replace);
}

void Utf8String::RemoveAllChars(const char* chars)
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

void Utf8String::ReplaceAllChars(const char* chars, char replacement)
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

int Utf8String::Compare(const Utf8String& other) const
{
    return strcmp(this->s, other.s);
}

int Utf8String::Compare(const char* other) const
{
    return strcmp(this->s, GetNonNull(other));
}

int Utf8String::CompareNoCaseAscii(const Utf8String& other) const
{
    return CompareNoCaseAscii(other.c_str());
}

int Utf8String::CompareNoCaseAscii(const char* other) const
{
    auto nonNullOther = GetNonNull(other);
    auto nonNullOtherLength = strlen(nonNullOther);

    auto minLength = std::min(this->l, nonNullOtherLength);
    for (size_t i = 0; i < minLength; i++)
    {
        auto diff = toupper(this->s[i]) - toupper(other[i]);
        if (diff != 0)
            return diff;
    }

    return static_cast<int>(this->l) - static_cast<int>(nonNullOtherLength);
}

bool Utf8String::Equals(const Utf8String& other) const
{
    return this->l == other.l && strcmp(this->s, other.s) == 0;
}

bool Utf8String::Equals(const char* other) const
{
    return strcmp(this->s, GetNonNull(other)) == 0;
}

bool Utf8String::EqualsNoCaseAscii(const Utf8String& other) const
{
    return CompareNoCaseAscii(other) == 0;
}

bool Utf8String::EqualsNoCaseAscii(const char* other) const
{
    return CompareNoCaseAscii(other) == 0;
}

bool Utf8String::StartsWith(const Utf8String& other) const
{
    if (other.empty())
        return false;
    else if (this->l < other.l)
        return false;

    auto thisS = this->s;
    auto otherS = other.s;
    while (*thisS == *otherS && *thisS && *otherS)
    {
        ++thisS;
        ++otherS;
    }

    //Was end reached?
    return *otherS == 0;
}

bool Utf8String::StartsWith(const char* other) const
{
    auto otherS = GetNonNull(other);
    if (*otherS == 0)
    {
        //Input string is empty
        return false;
    }

    auto thisS = this->s;
    while (*thisS == *otherS && *thisS && *otherS)
    {
        ++thisS;
        ++otherS;
    }

    //Was end reached?
    return *otherS == 0;
}

bool Utf8String::EndsWith(const Utf8String& other) const
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

bool Utf8String::EndsWith(const char* other) const
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

void Utf8String::TrimTrailingWhitespace()
{
    while (this->l > 0)
    {
        if (isspace(this->s[this->l - 1]))
            this->l--;
        else
            break;
    }
}

void Utf8String::RemoveWhitespace(size_t offset)
{
    TrimTrailingWhitespace();

    for (size_t i = offset; i < this->l;)
    {
        if (isspace(this->s[i]))
            erase(&this->s[i]);
        else
            i++;
    }
}

bool Utf8String::IterateCodepoint(size_t& iter, uint32_t& codepoint) const
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

bool Utf8String::GetCodepointCount(size_t& count) const
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

bool Utf8String::GetCodepoints(uint32_t* output, size_t& outputLength) const
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

size_t Utf8String::GetHash() const
{
    return Hash::Bytes(this->s, this->l);
}

size_t Utf8String::Hash(const Utf8String& s)
{
    return Hash::Bytes(s.s, s.l);
}

size_t Utf8String::Hash(const char* s, size_t len)
{
    return Hash::Bytes(s, len);
}

size_t Utf8String::Hash(const char* s)
{
    return Hash::Bytes(s, strlen(s));
}

const Utf8String& Utf8String::ToString() const
{
    return *this;
}

ValueOrError<void> Utf8String::ToString(Utf8String& result) const
{
    return result.assign(this->s, this->l);
}

const char* Utf8String::SkipWhitespace(const char* begin, const char* end)
{
    while (begin != end && isspace(*begin))
        begin++;

    return begin;
}

bool Utf8String::IsWhitespace() const
{
    return IsWhitespace(begin(), end());
}

const char* Utf8String::GetNonNull(const char* c)
{
    static const char* empty = "";
    return c != nullptr ? c : empty;
}

//Utf8StringToWideString
Utf8StringToWideString::Utf8StringToWideString(Allocator* allocator)
{
    Init();
    this->allocator = allocator;
}

Utf8StringToWideString::Utf8StringToWideString(const Utf8String& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Utf8StringToWideString::Utf8StringToWideString(const char* other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, other != nullptr ? strlen(other) : 0);
}

Utf8StringToWideString::Utf8StringToWideString(const char* other, size_t length, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, length);
}

Utf8StringToWideString::~Utf8StringToWideString()
{
    Free();
}

bool Utf8StringToWideString::Create(Allocator* allocator)
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

void Utf8StringToWideString::Destroy()
{
    Free();

    this->allocator = nullptr;
}

Allocator* Utf8StringToWideString::GetAllocator()
{
    return this->allocator;
}

bool Utf8StringToWideString::SetAllocator(Allocator* allocator)
{
    if (this->allocator != nullptr || !IsStatic())
        return false;

    this->allocator = allocator;
    return true;
}

ValueOrError<void> Utf8StringToWideString::assign(const Utf8String& other)
{
    return assign(other.c_str(), other.length());
}

ValueOrError<void> Utf8StringToWideString::assign(const char* other)
{
    return assign(other, other != nullptr ? strlen(other) : 0);
}

ValueOrError<void> Utf8StringToWideString::assign(const char* other, size_t len)
{
    if (other == nullptr || len == 0)
    {
        //Other string is zero length
        Truncate();
    }
    else
    {
        //Convert and store
        WideningToUtf16Converter widened;
        if (!widened.convert(other, other + len))
            return ValueOrError<void>::CreateError();
        auto widenedLength = wcslen(widened.c_str());
        if (EnsureLengthAllocated(widenedLength).HasError())
            return ValueOrError<void>::CreateError();

        wcscpy(this->s, widened.c_str());
        this->s[widenedLength] = 0;
        this->l = widenedLength;
    }
    return ValueOrError<void>();
}

bool Utf8StringToWideString::empty() const
{
    return this->l == 0;
}

size_t Utf8StringToWideString::length() const
{
    return this->l;
}

const wchar_t* Utf8StringToWideString::c_str() const
{
    return this->s;
}

const wchar_t* Utf8StringToWideString::data() const
{
    return this->s;
}

wchar_t* Utf8StringToWideString::data()
{
    return this->s;
}

bool Utf8StringToWideString::operator == (const wchar_t* other) const
{
    if (this->s == nullptr && other == nullptr)
        return true;
    else if (this->s == nullptr || other == nullptr)
        return false;
    else if (this->l != wcslen(other))
        return false;

    //Strings are non-null
    return wcsncmp(this->s, other, this->l) == 0;
}

bool Utf8StringToWideString::operator != (const wchar_t* other) const
{
    return !(*this == other);
}

const wchar_t& Utf8StringToWideString::operator [] (size_t i) const
{
    return this->s[i];
}

wchar_t& Utf8StringToWideString::operator [] (size_t i)
{
    return this->s[i];
}

bool Utf8StringToWideString::IsStatic() const
{
    return this->s == this->shortS;
}

void Utf8StringToWideString::Init()
{
    this->allocator = nullptr;
    this->s = this->shortS;
    this->l = 0;
    this->shortS[0] = 0;
    this->allocatedLength = 0;
}

ValueOrError<void> Utf8StringToWideString::EnsureLengthAllocated(size_t len)
{
    //Ensure the buffer can hold a string of len + 1. This does not preserve the content of the current string
    if (len > Utf8String::STATIC_STRING_LENGTH && len > this->allocatedLength)
    {
        this->allocatedLength = len;

        if (!IsStatic())
            _Deallocate(this->s);
        this->s = _Allocate(this->allocatedLength + 1, FINJIN_CALLER_ARGUMENTS);
    }

    if (this->s == nullptr)
    {
        Truncate();
        return ValueOrError<void>::CreateError();
    }

    this->s[0] = 0;
    this->l = 0;

    return ValueOrError<void>();
}

void Utf8StringToWideString::Truncate()
{
    if (this->s == nullptr)
        this->s = this->shortS;
    this->s[0] = 0;
    this->shortS[0] = 0;
    this->l = 0;
}

void Utf8StringToWideString::Free()
{
    if (!IsStatic())
    {
        _Deallocate(this->s);
        this->allocatedLength = 0;
    }

    this->s = this->shortS;
    this->shortS[0] = 0;
}

wchar_t* Utf8StringToWideString::_Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    wchar_t* result;
    if (this->allocator != nullptr)
        result = static_cast<wchar_t*>(this->allocator->Allocate(sizeof(wchar_t) * charCount, FINJIN_CALLER_PARAMETERS));
    else
        result = static_cast<wchar_t*>(Allocator::SystemAllocate(sizeof(wchar_t) * charCount, FINJIN_CALLER_PARAMETERS));
    assert(result && "Failed to allocate Utf8StringToWideString buffer.");
    return result;
}

void Utf8StringToWideString::_Deallocate(void* mem)
{
    if (this->allocator != nullptr)
        this->allocator->Deallocate(mem);
    else
        Allocator::SystemDeallocate(mem);
}

//Utf8StringToCodepointString
Utf8StringToCodepointString::Utf8StringToCodepointString(Allocator* allocator)
{
    Init();
    this->allocator = allocator;
}

Utf8StringToCodepointString::Utf8StringToCodepointString(const Utf8String& other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other);
}

Utf8StringToCodepointString::Utf8StringToCodepointString(const char* other, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, other != nullptr ? strlen(other) : 0);
}

Utf8StringToCodepointString::Utf8StringToCodepointString(const char* other, size_t length, Allocator* allocator)
{
    Init();
    this->allocator = allocator;
    assign(other, length);
}

Utf8StringToCodepointString::~Utf8StringToCodepointString()
{
    Free();
}

bool Utf8StringToCodepointString::Create(Allocator* allocator)
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

void Utf8StringToCodepointString::Destroy()
{
    Free();

    this->allocator = nullptr;
}

Allocator* Utf8StringToCodepointString::GetAllocator()
{
    return this->allocator;
}

bool Utf8StringToCodepointString::SetAllocator(Allocator* allocator)
{
    if (this->allocator != nullptr || !IsStatic())
        return false;

    this->allocator = allocator;
    return true;
}

ValueOrError<void> Utf8StringToCodepointString::assign(const Utf8String& other)
{
    return assign(other.c_str(), other.length());
}

ValueOrError<void> Utf8StringToCodepointString::assign(const char* other)
{
    return assign(other, other != nullptr ? strlen(other) : 0);
}

ValueOrError<void> Utf8StringToCodepointString::assign(const char* other, size_t len)
{
    if (other == nullptr || len == 0)
    {
        //Other string is zero length
        Truncate();
    }
    else
    {
        //Convert and store
        WideningToUtf32Converter widened;
        if (!widened.convert(other, other + len))
            return ValueOrError<void>::CreateError();
        auto widenedLength = strlen32(widened.c_str());
        if (EnsureLengthAllocated(widenedLength).HasError())
            return ValueOrError<void>::CreateError();

        FINJIN_COPY_MEMORY(this->s, widened.c_str(), sizeof(char32_t) * widenedLength);
        this->s[widenedLength] = 0;
        this->l = widenedLength;
    }
    return ValueOrError<void>();
}

bool Utf8StringToCodepointString::empty() const
{
    return this->l == 0;
}

size_t Utf8StringToCodepointString::length() const
{
    return this->l;
}

const char32_t* Utf8StringToCodepointString::c_str() const
{
    return this->s;
}

const char32_t* Utf8StringToCodepointString::data() const
{
    return this->s;
}

char32_t* Utf8StringToCodepointString::data()
{
    return this->s;
}

bool Utf8StringToCodepointString::operator == (const char32_t* other) const
{
    if (this->s == nullptr && other == nullptr)
        return true;
    else if (this->s == nullptr || other == nullptr)
        return false;
    else if (this->l != strlen32(other))
        return false;

    //Strings are non-null
    return strncmp32(this->s, other) == 0;
}

bool Utf8StringToCodepointString::operator != (const char32_t* other) const
{
    return !(*this == other);
}

const char32_t& Utf8StringToCodepointString::operator [] (size_t i) const
{
    return this->s[i];
}

char32_t& Utf8StringToCodepointString::operator [] (size_t i)
{
    return this->s[i];
}

bool Utf8StringToCodepointString::IsStatic() const
{
    return this->s == this->shortS;
}

void Utf8StringToCodepointString::Init()
{
    this->allocator = nullptr;
    this->s = this->shortS;
    this->l = 0;
    this->shortS[0] = 0;
    this->allocatedLength = 0;
}

ValueOrError<void> Utf8StringToCodepointString::EnsureLengthAllocated(size_t len)
{
    //Ensure the buffer can hold a string of len + 1. This does not preserve the content of the current string
    if (len > Utf8String::STATIC_STRING_LENGTH && len > this->allocatedLength)
    {
        this->allocatedLength = len;

        if (!IsStatic())
            _Deallocate(this->s);
        this->s = _Allocate(this->allocatedLength + 1, FINJIN_CALLER_ARGUMENTS);
    }

    if (this->s == nullptr)
    {
        Truncate();
        return ValueOrError<void>::CreateError();
    }

    this->s[0] = 0;
    this->l = 0;

    return ValueOrError<void>();
}

void Utf8StringToCodepointString::Truncate()
{
    if (this->s == nullptr)
        this->s = this->shortS;
    this->s[0] = 0;
    this->shortS[0] = 0;
    this->l = 0;
}

void Utf8StringToCodepointString::Free()
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

char32_t* Utf8StringToCodepointString::_Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    char32_t* result;
    if (this->allocator != nullptr)
        result = static_cast<char32_t*>(this->allocator->Allocate(sizeof(char32_t) * charCount, FINJIN_CALLER_PARAMETERS));
    else
        result = static_cast<char32_t*>(Allocator::SystemAllocate(sizeof(char32_t) * charCount, FINJIN_CALLER_PARAMETERS));
    assert(result && "Failed to allocate Utf8StringToCodepointString buffer.");
    return result;
}

void Utf8StringToCodepointString::_Deallocate(void* mem)
{
    if (this->allocator != nullptr)
        this->allocator->Deallocate(mem);
    else
        Allocator::SystemDeallocate(mem);
}

//Utf8StringView
const Utf8StringView& Utf8StringView::GetEmpty()
{
    static const Utf8StringView value;
    return value;
}

Utf8StringView::Utf8StringView()
{
    clear();
}

Utf8StringView::Utf8StringView(const char* first)
{
    this->s = first;
    this->l = first != nullptr ? strlen(first) : 0;
}

Utf8StringView::Utf8StringView(const char* first, size_t len)
{
    this->s = first;
    this->l = len;
}

Utf8StringView::Utf8StringView(const Utf8StringView& s)
{
    this->s = s.s;
    this->l = s.l;
}

Utf8StringView::Utf8StringView(const Utf8String& s)
{
    this->s = s.c_str();
    this->l = s.length();
}

ValueOrError<void> Utf8StringView::assign(const char* first)
{
    this->s = first;
    this->l = first != nullptr ? strlen(first) : 0;
    return ValueOrError<void>();
}

ValueOrError<void> Utf8StringView::assign(const char* first, size_t len)
{
    this->s = first;
    this->l = len;
    return ValueOrError<void>();
}

ValueOrError<void> Utf8StringView::assign(const char* first, const char* last)
{
    this->s = first;
    this->l = last - first;
    return ValueOrError<void>();
}

ValueOrError<void> Utf8StringView::assign(const uint8_t* first, const uint8_t* last)
{
    this->s = reinterpret_cast<const char*>(first);
    this->l = last - first;
    return ValueOrError<void>();
}

ValueOrError<void> Utf8StringView::assign(const uint8_t* first, size_t len)
{
    this->s = reinterpret_cast<const char*>(first);
    this->l = len;
    return ValueOrError<void>();
}

void Utf8StringView::clear()
{
    this->s = nullptr;
    this->l = 0;
}

bool Utf8StringView::empty() const
{
    return this->l == 0;
}

bool Utf8StringView::operator == (const char* other) const
{
    auto s = Utf8String::GetNonNull(this->s);
    auto otherS = Utf8String::GetNonNull(other);

    if (this->l != strlen(otherS))
        return false;

    return s == otherS || strncmp(s, otherS, this->l) == 0;
}

bool Utf8StringView::operator != (const char* other) const
{
    return !(*this == other);
}

bool Utf8StringView::operator == (const Utf8String& other) const
{
    if (this->l != other.length())
        return false;

    auto s = Utf8String::GetNonNull(this->s);

    return strncmp(s, other.c_str(), this->l) == 0;
}

bool Utf8StringView::operator != (const Utf8String& other) const
{
    return !(*this == other);
}

bool Utf8StringView::operator == (const Utf8StringView& other) const
{
    if (this->l != other.l)
        return false;

    auto s = Utf8String::GetNonNull(this->s);
    auto otherS = Utf8String::GetNonNull(other.s);

    return s == otherS || strncmp(s, otherS, this->l) == 0;
}

bool Utf8StringView::operator != (const Utf8StringView& other) const
{
    return !(*this == other);
}

const char* Utf8StringView::begin() const
{
    return this->s;
}

const char* Utf8StringView::end() const
{
    return this->s != nullptr ? (this->s + this->l) : nullptr;
}

size_t Utf8StringView::size() const
{
    return this->l;
}

size_t Utf8StringView::length() const
{
    return this->l;
}

void Utf8StringView::pop_back()
{
    pop_back(1);
}

void Utf8StringView::pop_back(size_t count)
{
    this->l -= std::min(count, this->l);
}

void Utf8StringView::pop_back(const char* other)
{
    auto otherLength = other != nullptr ? strlen(other) : (size_t)0;
    while (this->l > 0 && otherLength > 0 && this->s[this->l - 1] == other[otherLength - 1])
    {
        this->l--;
        otherLength--;
    }
}

size_t Utf8StringView::find(char c, size_t pos) const
{
    for (size_t i = pos; i < this->l; i++)
    {
        if (this->s[i] == c)
            return i;
    }
    return Utf8String::npos;
}

size_t Utf8StringView::find(const char* other, size_t pos) const
{
    auto range = boost::make_iterator_range(this->s + pos, this->s + this->l);

    auto safeOther = Utf8String::GetNonNull(other);
    auto otherRange = boost::make_iterator_range(safeOther, safeOther + strlen(safeOther));

    auto foundAt = boost::algorithm::find_first(range, otherRange);
    if (foundAt)
        return foundAt.begin() - s;
    else
        return npos;
}

Utf8StringView Utf8StringView::substr(size_t pos, size_t len) const
{
    if (empty())
        return *this;

    if (pos == 0 && len >= this->l)
        return *this;

    Utf8StringView result;
    if (pos < this->l)
        result.assign(this->s + pos, std::min(len, this->l - pos));
    return result;
}

const char& Utf8StringView::operator [] (size_t i) const
{
    return this->s[i];
}

bool Utf8StringView::IsDigits() const
{
    return Utf8String::IsDigits(begin(), end());
}

int Utf8StringView::CompareNoCaseAscii(const char* other) const
{
    auto nonNullOther = Utf8String::GetNonNull(other);
    auto nonNullOtherLength = strlen(nonNullOther);

    auto minLength = std::min(this->l, nonNullOtherLength);
    for (size_t i = 0; i < minLength; i++)
    {
        auto diff = toupper(this->s[i]) - toupper(other[i]);
        if (diff != 0)
            return diff;
    }

    return static_cast<int>(this->l) - static_cast<int>(nonNullOtherLength);
}

bool Utf8StringView::EqualsNoCaseAscii(const char* other) const
{
    auto nonNullOther = Utf8String::GetNonNull(other);
    auto nonNullOtherLength = strlen(nonNullOther);

    if (this->l != nonNullOtherLength)
        return false;

    for (size_t i = 0; i < this->l; i++)
    {
        auto diff = toupper(this->s[i]) - toupper(other[i]);
        if (diff != 0)
            return false;
    }

    return true;
}

bool Utf8StringView::StartsWith(const char* other) const
{
    if (other != nullptr)
        return StartsWith(other, strlen(other));
    else
        return false;
}

bool Utf8StringView::StartsWith(const char* other, size_t otherLength) const
{
    if (otherLength > this->l)
        return false;

    return strncmp(this->s, other, otherLength) == 0;
}

bool Utf8StringView::EndsWith(const char* other) const
{
    if (other != nullptr)
        return EndsWith(other, strlen(other));
    else
        return false;
}

bool Utf8StringView::EndsWith(const char* other, size_t otherLength) const
{
    if (otherLength > this->l)
        return false;

    return strncmp(this->s + this->l - otherLength, other, otherLength) == 0;
}

void Utf8StringView::TrimTrailingWhitespace()
{
    while (this->l > 0)
    {
        if (isspace(this->s[this->l - 1]))
            this->l--;
        else
            break;
    }
}

bool Utf8StringView::IsWhitespace() const
{
    return Utf8String::IsWhitespace(begin(), end());
}

size_t Utf8StringView::GetHash() const
{
    return Utf8String::Hash(begin(), length());
}

Utf8String Utf8StringView::ToString() const
{
    return Utf8String(this->s, this->l);
}

ValueOrError<void> Utf8StringView::ToString(Utf8String& result) const
{
    return result.assign(this->s, this->l);
}
