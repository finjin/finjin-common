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
#include "finjin/common/Utf8String.hpp"
#include <list>
#include <vector>


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class NumberRangeMatch
    {
    public:
        Utf8String text;

        struct Part
        {
            size_t digitCount;
            int value;

            void ParseFrom(const Utf8String& s, int base);
        };
        Part rangeStart;
        Part rangeEnd;

        int base; //10 or 16
        size_t digitCount; //Number of digits
        bool padDigits; //Indicates whether zeros should pad out the start of generated output strings

    public:
        class iterator
        {
        public:
            iterator();
            iterator(int value, int displayBase, size_t displayDigitCount, bool padDisplayDigits);
            iterator& operator ++ ();
            bool operator == (const iterator& other) const;
            bool operator != (const iterator& other) const;
            Utf8String ToString() const;
            ValueOrError<void> ToString(Utf8String& result) const;
            int operator * () const;

        private:
            int value;
            int displayBase;
            size_t displayDigitCount;
            bool padDisplayDigits;
        };

        using const_iterator = iterator;

        const_iterator begin() const;
        const_iterator end() const;
    };

    class NumberRangeMatcher
    {
    public:
        using Matches = std::list<NumberRangeMatch>;

    public:
        NumberRangeMatcher(const Utf8String& text);

        const Utf8String& GetText() const;

        const Matches& GetMatches() const;

    private:
        Utf8String text;

        Matches matches;
    };

    class RangedNameGenerator
    {
    private:
        struct NumberRangeMatchIterator
        {
            NumberRangeMatch match;
            NumberRangeMatch::iterator iter;
        };

        struct MatchIterators : public std::vector<NumberRangeMatchIterator>
        {
        public:
            bool operator == (const std::vector<NumberRangeMatchIterator>& other) const;
            bool operator != (const std::vector<NumberRangeMatchIterator>& other) const;
        };

    public:
        class iterator
        {
            friend class RangedNameGenerator;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = Utf8String;
            using difference_type = ptrdiff_t;
            using pointer = iterator*;
            using reference = Utf8String;

        public:
            iterator();

            iterator& operator ++ ();

            bool operator == (const iterator& other) const;
            bool operator != (const iterator& other) const;

            Utf8String operator * () const;        

        private:
            Utf8String text;
            MatchIterators matchIterators;
            bool isEnd;
        };

        using const_iterator = iterator;

    public:
        RangedNameGenerator(const Utf8String& text);

        const_iterator begin() const;
        const_iterator end() const;

    private:
        NumberRangeMatcher matcher;

        MatchIterators matchIterators;

        iterator _begin;
        iterator _end;
    };

} }
