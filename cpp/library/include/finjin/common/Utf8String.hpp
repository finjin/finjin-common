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
#include "finjin/common/AssignOrError.hpp"
#include <iostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class Allocator;
    class Utf8String;

    /**
     * Manages a pointer to UTF8 string characters.
     * The user must take care to ensure that the pointers outlive instances of Utf8StringView
     */
    class FINJIN_COMMON_LIBRARY_API Utf8StringView
    {
        friend class ConfigDocumentLine;
        friend class Path;

    public:
        static const Utf8StringView& GetEmpty();

        using traits_type = std::char_traits<char>;
        using value_type = char;
        using size_type = size_t;
        using difference_type = intptr_t;
        using pointer = Utf8StringView*;
        using const_pointer = const Utf8StringView*;
        using reference = Utf8StringView&;
        using const_reference = const Utf8StringView&;

        using iterator = char*;
        using const_iterator = const char*;

        using reverse_iterator = char*;
        using const_reverse_iterator = const char*;

        static const size_t npos = (size_t)-1;

        Utf8StringView();
        Utf8StringView(const char* first);
        Utf8StringView(const char* first, size_t len);
        Utf8StringView(const Utf8StringView& s);
        explicit Utf8StringView(const Utf8String& s);

        ValueOrError<void> assign(const char* first);
        ValueOrError<void> assign(const char* first, size_t len);
        ValueOrError<void> assign(const char* first, const char* last);
        ValueOrError<void> assign(const uint8_t* first, const uint8_t* last);
        ValueOrError<void> assign(const uint8_t* first, size_t len);

        void clear();

        bool empty() const; //Returns true if empty

        bool operator == (const char* other) const;
        bool operator != (const char* other) const;
        bool operator == (const Utf8String& other) const;
        bool operator != (const Utf8String& other) const;
        bool operator == (const Utf8StringView& other) const;
        bool operator != (const Utf8StringView& other) const;

        const char* begin() const;
        const char* end() const;

        size_t size() const;
        size_t length() const;

        void pop_back();
        void pop_back(size_t count);
        void pop_back(const char* other);

        size_t find(char c, size_t pos = 0) const;
        size_t find(const char* other, size_t pos = 0) const;

        template <typename T>
        ValueOrError<void> substr(T& result, size_t pos, size_t len = npos) const
        {
            result.clear();

            if (!empty() && pos < this->l)
                return result.assign(this->s + pos, std::min(len, this->l - pos));

            return ValueOrError<void>();
        }

        Utf8StringView substr(size_t pos, size_t len = npos) const;

        const char& operator [] (size_t i) const;

        bool IsDigits() const;

        int CompareNoCaseAscii(const char* other) const;

        bool EqualsNoCaseAscii(const char* other) const;

        bool StartsWith(const char* other) const;
        bool StartsWith(const char* other, size_t otherLength) const;

        bool EndsWith(const char* other) const;
        bool EndsWith(const char* other, size_t otherLength) const;

        void TrimTrailingWhitespace();

        bool IsWhitespace() const;

        size_t GetHash() const;

        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

    private:
        const char* s;
        size_t l;
    };

    /**
     * Standard string class.
     * When initializing an instance of this class it is responsibility of the caller to ensure that the "char*" data being used is actually UTF8.
     * When using Windows API's you will most likely be using the "wide" versions of those API's, which use UTF16. Utf8String will perform UTF16->UTF8 conversion.
     */
    class FINJIN_COMMON_LIBRARY_API Utf8String
    {
        friend class Path;

    public:
        static const Utf8String& GetEmpty();

        using traits_type = std::char_traits<char>;
        using value_type = char;
        using size_type = size_t;
        using difference_type = intptr_t;
        using pointer = Utf8String*;
        using const_pointer = const Utf8String*;
        using reference = Utf8String&;
        using const_reference = const Utf8String&;

        using iterator = char*;
        using const_iterator = const char*;

        using reverse_iterator = char*;
        using const_reverse_iterator = const char*;

        static const size_t npos = (size_t)-1;

        Utf8String(Allocator* allocator = nullptr);
        Utf8String(const char* other, Allocator* allocator = nullptr);
        Utf8String(const wchar_t* other, Allocator* allocator = nullptr);
        Utf8String(const char* other, size_t len, Allocator* allocator = nullptr);
        Utf8String(const wchar_t* other, size_t len, Allocator* allocator = nullptr);
        Utf8String(const char* first, const char* last, Allocator* allocator = nullptr);
        Utf8String(const wchar_t* first, const wchar_t* last, Allocator* allocator = nullptr);
        Utf8String(const uint8_t* first, const uint8_t* last, Allocator* allocator = nullptr);
        Utf8String(const uint8_t* other, size_t len, Allocator* allocator = nullptr);
        Utf8String(size_t count, char c, Allocator* allocator = nullptr);
        Utf8String(size_t count, Allocator* allocator);
        Utf8String(const Utf8String& other, Allocator* allocator = nullptr);
        Utf8String(const Utf8StringView& other, Allocator* allocator = nullptr);
        Utf8String(Utf8String&& other);
        ~Utf8String();

        bool Create(Allocator* allocator);
        void Destroy();

        Allocator* GetAllocator();
        bool SetAllocator(Allocator* allocator);

        ValueOrError<void> operator = (const char* other);
        ValueOrError<void> operator = (const wchar_t* other);
        ValueOrError<void> operator = (const Utf8String& other);
        ValueOrError<void> operator = (const Utf8StringView& other);
        ValueOrError<void> operator = (Utf8String&& other);

        ValueOrError<void> assign(const char* other);
        ValueOrError<void> assign(const wchar_t* other);
        ValueOrError<void> assign(const char* other, size_t len);
        ValueOrError<void> assign(const wchar_t* other, size_t len);
        ValueOrError<void> assign(const char* first, const char* last);
        ValueOrError<void> assign(const wchar_t* first, const wchar_t* last);
        ValueOrError<void> assign(const uint8_t* first, const uint8_t* last);
        ValueOrError<void> assign(const uint8_t* other, size_t len);
        ValueOrError<void> assign(size_t count, char c);
        ValueOrError<void> assign(const Utf8String& other);
        ValueOrError<void> assign(const Utf8StringView& other);
        ValueOrError<void> assign(Utf8String&& other);

        const char* c_str() const;

        bool empty() const;
        void clear();

        size_t length() const;
        size_t size() const;

        ValueOrError<void> reserve(size_t len);
        size_t GetCharactersLeft() const;

        ValueOrError<void> resize(size_t len);

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        template <typename T>
        ValueOrError<void> substr(T& result, size_t pos, size_t len = npos) const
        {
            result.clear();

            if (!empty() && pos < this->l)
                return result.assign(this->s + pos, std::min(len, this->l - pos));

            return ValueOrError<void>();
        }

        bool operator == (const Utf8String& other) const;
        bool operator != (const Utf8String& other) const;
        bool operator < (const Utf8String& other) const;
        bool operator > (const Utf8String& other) const;

        bool operator == (const Utf8StringView& other) const;
        bool operator != (const Utf8StringView& other) const;
        bool operator < (const Utf8StringView& other) const;
        bool operator > (const Utf8StringView& other) const;

        bool operator == (const char* other) const;
        bool operator != (const char* other) const;
        bool operator < (const char* other) const;
        bool operator > (const char* other) const;

        ValueOrError<void> operator += (Utf8String&& other);
        ValueOrError<void> operator += (const Utf8String& other);
        ValueOrError<void> operator += (const Utf8StringView& other);
        ValueOrError<void> operator += (const char* other);
        ValueOrError<void> operator += (const wchar_t* other);
        ValueOrError<void> operator += (char other);

        ValueOrError<void> append(Utf8String&& other);
        ValueOrError<void> append(const Utf8String& other);
        ValueOrError<void> append(const Utf8StringView& other);
        ValueOrError<void> append(const char* other);
        ValueOrError<void> append(const char* other, size_t len);
        ValueOrError<void> append(const wchar_t* other);
        ValueOrError<void> append(const wchar_t* other, size_t len);
        ValueOrError<void> append(char other);
        ValueOrError<void> append(wchar_t other);
        ValueOrError<void> append(size_t count, char c);

        ValueOrError<void> push_back(char c);
        char back() const;

        void pop_front();
        void pop_front(size_t count);
        void pop_back();
        void pop_back(size_t count);
        void pop_back(const char* other);

        size_t find(char c, size_t pos = 0) const;
        size_t find(const char* other, size_t pos = 0) const;
        size_t find(const Utf8String& other, size_t pos = 0) const;

        size_t rfind(char c, size_t pos = npos) const;
        size_t rfind(const char* other, size_t pos = npos) const;
        size_t rfind(const Utf8String& other, size_t pos = npos) const;

        char* erase(const char* at);
        char* erase(const char* from, const char* to);

        template <typename BeginIter, typename EndIter>
        ValueOrError<void> insert(char* at, BeginIter beginIt, EndIter endIt)
        {
            if (at == end())
            {
                for (auto iter = beginIt; iter != endIt; ++iter)
                {
                    if (push_back(*iter).HasError())
                        return ValueOrError<void>::CreateError();
                }
            }
            else if (at >= begin() && at < end() && beginIt != endIt)
            {
                Utf8String newString(this->s, at - this->s, this->allocator); //Portion before 'at'
                for (auto iter = beginIt; iter != endIt; ++iter) //Inserted portion
                {
                    if (newString.append(*iter).HasError())
                        return ValueOrError<void>::CreateError();
                }
                if (newString.append(at).HasError()) //Portion after 'at'
                    return ValueOrError<void>::CreateError();

                assign(std::move(newString));
            }

            return ValueOrError<void>();
        }

        const char& operator [] (size_t i) const;
        char& operator [] (size_t i);

        bool IsDigits() const;

        void ToUpperAscii();
        void ToLowerAscii();

        void ToLowerAlphaNumeric();

        void ReplaceFirst(char find, char replace);
        void ReplaceAll(char find, char replace);

        template <typename Find, typename Replace>
        void ReplaceFirst(const Find& find, const Replace& replace)
        {
            Utf8StringView findView(find);
            Utf8StringView replaceView(replace);
            ReplaceFirst(findView, replaceView);
        }
        void ReplaceFirst(const Utf8StringView& find, const Utf8StringView& replace);

        template <typename Find, typename Replace>
        void ReplaceAll(const Find& find, const Replace& replace)
        {
            Utf8StringView findView(find);
            Utf8StringView replaceView(replace);
            ReplaceAll(findView, replaceView);
        }
        void ReplaceAll(const Utf8StringView& find, const Utf8StringView& replace);

        void RemoveAllChars(const char* chars);
        void ReplaceAllChars(const char* chars, char replacement);

        int Compare(const Utf8String& other) const;
        int Compare(const char* other) const;

        int CompareNoCaseAscii(const Utf8String& other) const;
        int CompareNoCaseAscii(const char* other) const;

        bool Equals(const Utf8String& other) const;
        bool Equals(const char* other) const;

        bool EqualsNoCaseAscii(const Utf8String& other) const;
        bool EqualsNoCaseAscii(const char* other) const;

        bool StartsWith(const Utf8String& other) const;
        bool StartsWith(const char* other) const;

        bool EndsWith(const Utf8String& other) const;
        bool EndsWith(const char* other) const;

        void TrimTrailingWhitespace();
        void RemoveWhitespace(size_t offset = 0);

        bool IsStatic() const;

        size_t RepairLength(size_t newLength = (size_t)-1);

        bool IterateCodepoint(size_t& iter, uint32_t& codepoint) const;
        bool GetCodepointCount(size_t& count) const;

        /**
         * Converts the UTF8 string to UTF32.
         * @param output [out] - The output codepoints. This should point to an array at least length() + 1.
         * @param outputLength [in/out] - On input, the number of elements pointed to by 'output'. On output, the length of the output string, not counting the terminating character.
         * If this value is the same as the input length, it indicates that the 'output' array was not long enough and contains a truncated result.
         */
        bool GetCodepoints(uint32_t* output, size_t& outputLength) const;

        size_t GetHash() const;

        static size_t Hash(const Utf8String& s);
        static size_t Hash(const char* s, size_t len);
        static size_t Hash(const char* s);

        const Utf8String& ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

        static const char* SkipWhitespace(const char* begin, const char* end);

        bool IsWhitespace() const;

        template <typename BeginIter, typename EndIter>
        static bool IsWhitespace(BeginIter begin, EndIter end)
        {
            if (begin == end)
                return false;

            for (auto i = begin; i != end; i++)
            {
                if (!isspace(*i))
                    return false;
            }

            return true;
        }

        template <typename BeginIter, typename EndIter>
        static bool IsDigits(BeginIter begin, EndIter end)
        {
            if (begin == end)
                return false;

            for (auto i = begin; i != end; i++)
            {
                if (!isdigit(*i))
                    return false;
            }

            return true;
        }

        static const char* GetNonNull(const char* c);

        enum { STATIC_STRING_LENGTH = 95 };

    private:
        void Init();

        ValueOrError<void> EnsureLengthAllocated(size_t len, bool oversize);
        void Truncate();
        void Free();

        char* _Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION);
        void _Deallocate(void* mem);

    protected:
        Allocator* allocator;
        size_t l;
        size_t allocatedLength;
        char* s;
        char shortS[STATIC_STRING_LENGTH + 1];
    };

    class Utf8StringToWideString
    {
    public:
        Utf8StringToWideString(const Utf8StringToWideString& other) = delete;
        Utf8StringToWideString(Utf8StringToWideString&& other) = delete;

        Utf8StringToWideString& operator = (const Utf8StringToWideString& other) = delete;
        Utf8StringToWideString& operator = (Utf8StringToWideString&& other) = delete;

        Utf8StringToWideString(Allocator* allocator = nullptr);
        Utf8StringToWideString(const Utf8String& other, Allocator* allocator = nullptr);
        Utf8StringToWideString(const char* other, Allocator* allocator = nullptr);
        Utf8StringToWideString(const char* other, size_t length, Allocator* allocator = nullptr);
        ~Utf8StringToWideString();

        bool Create(Allocator* allocator);
        void Destroy();

        Allocator* GetAllocator();
        bool SetAllocator(Allocator* allocator);

        ValueOrError<void> assign(const Utf8String& other);
        ValueOrError<void> assign(const char* other);
        ValueOrError<void> assign(const char* other, size_t length);

        bool empty() const;

        size_t length() const;

        const wchar_t* c_str() const;

        const wchar_t* data() const;
        wchar_t* data();

        bool operator == (const wchar_t* other) const;
        bool operator != (const wchar_t* other) const;

        const wchar_t& operator [] (size_t i) const;
        wchar_t& operator [] (size_t i);

    private:
        bool IsStatic() const;

        void Init();

        ValueOrError<void> EnsureLengthAllocated(size_t len);
        void Truncate();
        void Free();

        wchar_t* _Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION);
        void _Deallocate(void* mem);

    private:
        Allocator* allocator;
        size_t l;
        size_t allocatedLength;
        wchar_t* s;
        wchar_t shortS[Utf8String::STATIC_STRING_LENGTH + 1];
    };

    class Utf8StringToCodepointString
    {
    public:
        Utf8StringToCodepointString(const Utf8StringToCodepointString& other) = delete;
        Utf8StringToCodepointString(Utf8StringToCodepointString&& other) = delete;

        Utf8StringToCodepointString& operator = (const Utf8StringToCodepointString& other) = delete;
        Utf8StringToCodepointString& operator = (Utf8StringToCodepointString&& other) = delete;

        Utf8StringToCodepointString(Allocator* allocator = nullptr);
        Utf8StringToCodepointString(const Utf8String& other, Allocator* allocator = nullptr);
        Utf8StringToCodepointString(const char* other, Allocator* allocator = nullptr);
        Utf8StringToCodepointString(const char* other, size_t length, Allocator* allocator = nullptr);
        ~Utf8StringToCodepointString();

        bool Create(Allocator* allocator);
        void Destroy();

        Allocator* GetAllocator();
        bool SetAllocator(Allocator* allocator);

        ValueOrError<void> assign(const Utf8String& other);
        ValueOrError<void> assign(const char* other);
        ValueOrError<void> assign(const char* other, size_t length);

        bool empty() const;

        size_t length() const;

        const char32_t* c_str() const;

        const char32_t* data() const;
        char32_t* data();

        bool operator == (const char32_t* other) const;
        bool operator != (const char32_t* other) const;

        const char32_t& operator [] (size_t i) const;
        char32_t& operator [] (size_t i);

    private:
        bool IsStatic() const;

        void Init();

        ValueOrError<void> EnsureLengthAllocated(size_t len);
        void Truncate();
        void Free();

        char32_t* _Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION);
        void _Deallocate(void* mem);

    private:
        Allocator* allocator;
        size_t l;
        size_t allocatedLength;
        char32_t* s;
        char32_t shortS[Utf8String::STATIC_STRING_LENGTH + 1];
    };

    template <>
    struct AssignOrError<Utf8String>
    {
        ValueOrError<void> operator () (Utf8String& a, const Utf8String& b)
        {
            return a.assign(b);
        }

        ValueOrError<void> operator () (Utf8String& a, Utf8String&& b)
        {
            return a.assign(std::move(b));
        }
    };

    struct Utf8StringHash
    {
        size_t operator () (const Finjin::Common::Utf8String& s) const
        {
            return s.GetHash();
        }

        size_t operator () (const Finjin::Common::Utf8StringView& s) const
        {
            return s.GetHash();
        }

        size_t operator () (const char* s) const
        {
            return Utf8String::Hash(s);
        }
    };

    struct Utf8StringEqual
    {
        template <typename A, typename B>
        bool operator () (const A& a, const B& b) const
        {
            Utf8StringView aView(a);
            Utf8StringView bView(b);
            return aView == bView;
        }
    };

} }

namespace std
{
    template <>
    struct hash<Finjin::Common::Utf8String>
    {
        size_t operator () (const Finjin::Common::Utf8String& s) const
        {
            return s.GetHash();
        }
    };

    template <>
    struct hash<char*>
    {
        size_t operator () (const char* s) const
        {
            return s ? Finjin::Common::Utf8String::Hash(s, strlen(s)) : 0;
        }
    };

    template <>
    struct hash<Finjin::Common::Utf8StringView>
    {
        size_t operator () (const Finjin::Common::Utf8StringView& s) const
        {
            return Finjin::Common::Utf8String::Hash(s.begin(), s.length());
        }
    };
}


//Functions---------------------------------------------------------------------
namespace std
{
    inline bool empty(const Finjin::Common::Utf8String& v)
    {
        return v.empty();
    }

    inline bool empty(const Finjin::Common::Utf8StringView& v)
    {
        return v.empty();
    }

    inline bool empty(const char* v)
    {
        return v == nullptr || v[0] == 0;
    }

    inline istream& operator >> (istream& is, Finjin::Common::Utf8String& v)
    {
         int c;
         while (is)
         {
             c = is.get();
             if (c > 0)
                 v.append(static_cast<char>(c));
             else if (c == 0)
                 break;
             else
             {
                 //boost::lexical_cast<> requires the stream to be in a non-failed state in order for the parse to succeed
                 //More on this issue at http://www.boost.org/doc/libs/1_49_0/doc/html/boost_lexical_cast/frequently_asked_questions.html
                 auto state = is.rdstate();
                 is.clear(state & ~(std::ios_base::eofbit | std::ios_base::failbit));
                 break;
             }
         }
         return is;
    }

    inline ostream& operator << (ostream& os, const Finjin::Common::Utf8String& v)
    {
        os << v.c_str();
        return os;
    }

    inline ostream& operator << (ostream& os, const Finjin::Common::Utf8StringView& v)
    {
        os.write(v.begin(), v.size());
        return os;
    }

    inline wistream& operator >> (wistream& is, Finjin::Common::Utf8String& v)
    {
         wchar_t c;
         while (is)
         {
             is.get(c);
             v.append(c);
         }
         return is;
    }

    inline wostream& operator << (wostream& os, const Finjin::Common::Utf8String& v)
    {
        os << v.c_str();
        return os;
    }

    inline wostream& operator << (wostream& os, const Finjin::Common::Utf8StringView& v)
    {
        os << v.ToString().c_str();
        return os;
    }
}

namespace Finjin { namespace Common {

    /**
     * Finds all the null terminated strings within the specified text
     * @param text [in] - The text to parse. It is assumed to have a double null terminator at the end
     * @param handler [in] - Callback called for each null terminated string
     */
    template <typename Handler> //std::function<ValueOrError<bool>(const char*)> handler
    ValueOrError<bool> ParseNullTerminatedStrings(const char* text, Handler handler)
    {
        size_t offset = 0;
        size_t foundAt;
        while (true)
        {
            //Find null character
            foundAt = offset;
            while (text[foundAt] != 0)
                foundAt++;

            //If two consecutive null characters were encountered, break out of loop
            if (text[offset] == 0 && text[foundAt] == 0)
                break;

            //Handle the item
            auto handlerResult = handler(&text[offset]);
            if (handlerResult.HasErrorOrValue(false))
                return handlerResult;

            //Update offset
            offset = foundAt + 1;
        }

        return true;
    }

    template <typename ToSplit, typename Handler> //std::function<ValueOrError<bool>(Utf8StringView&)> handler
    ValueOrError<bool> Split(ToSplit& toSplit, char splitChar, Handler handler, bool skipEmpty = true)
    {
        size_t previousIndex = (size_t)-1;
        Utf8StringView sub;

        auto s = toSplit.begin();
        auto l = toSplit.length();

        for (size_t i = 0; i <= l; i++)
        {
            if (i == l || s[i] == splitChar)
            {
                sub.assign(s + (previousIndex + 1), s + i);

                previousIndex = i;

                if (!skipEmpty || !sub.IsWhitespace())
                {
                    auto handlerResult = handler(sub);
                    if (handlerResult.HasErrorOrValue(false))
                        return handlerResult;
                }
            }
        }

        return true;
    }

    template <typename ToSplit, typename Handler> //std::function<ValueOrError<bool>(Utf8StringView&)> handler
    ValueOrError<bool> Split(ToSplit& toSplit, const Utf8String& splitChars, Handler handler, bool skipEmpty = true)
    {
        size_t previousIndex = (size_t)-1;
        Utf8StringView sub;

        auto s = toSplit.begin();
        auto l = toSplit.length();

        for (size_t i = 0; i <= l; i++)
        {
            if (i == l || splitChars.find(s[i]) != Utf8String::npos)
            {
                sub.assign(s + (previousIndex + 1), s + i);

                previousIndex = i;

                if (!skipEmpty || !sub.IsWhitespace())
                {
                    auto handlerResult = handler(sub);
                    if (handlerResult.HasErrorOrValue(false))
                        return handlerResult;
                }
            }
        }

        return true;
    }

    template <typename ResultType, typename JoinerType, typename Iter>
    ValueOrError<void> Join(ResultType& result, const JoinerType& joiner, const Iter& begin, const Iter& end)
    {
        result.clear();

        for (auto iter = begin; iter != end; ++iter)
        {
            if (iter != begin)
            {
                if (result.append(joiner).HasError())
                    return ValueOrError<void>::CreateError();
            }
            if (result.append(*iter).HasError())
                return ValueOrError<void>::CreateError();
        }

        return ValueOrError<void>();
    }

} }
