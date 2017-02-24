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
#include "finjin/common/RangedNameGenerator.hpp"
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace Finjin::Common;


//Local functions--------------------------------------------------------------
inline int CharToDigit(int c)
{
    int charValue = tolower(c);

    int value = 0;
    if (charValue >= 'a' && charValue <= 'f')
        value = 10 + (charValue - 'a');
    else if (charValue >= '0' && charValue <= '9')
        value = charValue - '0';

    return value;
}

inline int DetermineBase(const Utf8String& s)
{
    int minValue = std::numeric_limits<int>::max();
    int maxValue = std::numeric_limits<int>::lowest();

    for (size_t i = 0; i < s.length(); i++)
    {
        int value = CharToDigit(s[i]);
        if (value < minValue)
            minValue = value;
        if (value > maxValue)
            maxValue = value;
    }

    if (minValue >= 0 && maxValue <= 9)
        return 10;
    else if (minValue >= 0 && maxValue <= 15)
        return 16;
    else
        return 0;
}

inline ValueOrError<void> FormatStringForBase(Utf8String& s, int value, int base, size_t digitCount, bool padDigits)
{
    s.clear();
    
    //Convert the value to a string. This string will end up in the reverse order
    do
    {
        int digit = value % base;

        if (digit >= 0 && digit <= 9)
            s.push_back('0' + digit);
        else if (digit >= 10 && digit <= 15)
            s.push_back('a' + digit - 10);

        value /= base;
    } while (value > 0);
    
    //Pad out the string to the specified length
    if (padDigits)
    {
        while (s.length() < digitCount)
            s.push_back('0');
    }

    //Reverse the string to get the correct result
    Utf8String result;
    BOOST_REVERSE_FOREACH(char c, s)
    {
        result.push_back(c);
    }

    return ValueOrError<void>();
}


//Implementation----------------------------------------------------------------

//NumberRangeMatch::Part
void NumberRangeMatch::Part::ParseFrom(const Utf8String& s, int base)
{
    this->digitCount = s.length();
    this->value = 0;

    int scale = 1;

    for (size_t i = this->digitCount - 1; i != (size_t)-1; i--)
    {
        this->value += CharToDigit(s[i]) * scale;

        scale *= base;
    }
}

//NumberRangeMatch::iterator
NumberRangeMatch::iterator::iterator() 
{
    this->value = 0; 
    this->displayBase = 0; 
    this->displayDigitCount = 0; 
    this->padDisplayDigits = false; 
}

NumberRangeMatch::iterator::iterator(int value, int displayBase, size_t displayDigitCount, bool padDisplayDigits) 
{
    this->value = value; 
    this->displayBase = displayBase; 
    this->displayDigitCount = displayDigitCount; 
    this->padDisplayDigits = padDisplayDigits; 
}

NumberRangeMatch::iterator& NumberRangeMatch::iterator::operator ++ ()
{
    this->value++; 
    return *this; 
}

bool NumberRangeMatch::iterator::operator == (const iterator& other) const 
{
    return this->value == other.value; 
}

bool NumberRangeMatch::iterator::operator != (const iterator& other) const 
{
    return !(this->value == other.value); 
}

Utf8String NumberRangeMatch::iterator::ToString() const 
{ 
    Utf8String result;
    FormatStringForBase(result, this->value, this->displayBase, this->displayDigitCount, this->padDisplayDigits); 
    return result;
}

ValueOrError<void> NumberRangeMatch::iterator::ToString(Utf8String& result) const
{
    return FormatStringForBase(result, this->value, this->displayBase, this->displayDigitCount, this->padDisplayDigits);
}

int NumberRangeMatch::iterator::operator * () const 
{ 
    return this->value; 
}

//NumberRangeMatch
NumberRangeMatch::const_iterator NumberRangeMatch::begin() const
{
    return const_iterator(this->rangeStart.value, this->base, this->digitCount, this->padDigits);
}

NumberRangeMatch::const_iterator NumberRangeMatch::end() const
{
    return const_iterator(this->rangeEnd.value + 1, this->base, this->digitCount, this->padDigits);
}

//NumberRangeMatcher
NumberRangeMatcher::NumberRangeMatcher(const Utf8String& _text) : text(_text)
{
    boost::regex regex("\\[[0-9a-fA-F]+h?-([0-9a-fA-F]+h?)?\\]");
    boost::regex_token_iterator<Utf8String::const_iterator> iter(text.begin(), text.end(), regex, 0);
    boost::regex_token_iterator<Utf8String::const_iterator> end;

    for (; iter != end; ++iter)
    {
        NumberRangeMatch match;
        
        match.text = (*iter).str().c_str();

        Utf8String noBrackets;
        match.text.substr(noBrackets, 1, match.text.length() - 2);
        size_t separatorIndex = noBrackets.find('-');
        
        Utf8String start;
        noBrackets.substr(start, 0, separatorIndex);
        int startBase = 0;
        if (start[start.length() - 1] == 'h')
        {
            start.erase(start.end());
            startBase = 16;
        }            
        else
            startBase = DetermineBase(start);

        Utf8String end;
        noBrackets.substr(end, separatorIndex + 1);
        int endBase = 0;
        if (!end.empty())
        {
            if (end[end.length() - 1] == 'h')
            {
                end.erase(end.end());
                endBase = 16;
            }
            else
                endBase = DetermineBase(end);
        }
        else
            endBase = startBase;

        match.base = std::max(startBase, endBase);        

        if (match.base == 10 || match.base == 16)
        {
            match.digitCount = std::max(start.length(), end.length());
            
            match.padDigits = start.length() == end.length() || end.empty();

            match.rangeStart.ParseFrom(start, match.base);
            
            if (!end.empty())                
                match.rangeEnd.ParseFrom(end, match.base);
            else
            {
                match.rangeEnd.digitCount = match.rangeStart.digitCount;
                match.rangeEnd.value = std::numeric_limits<int>::max();
            }

            this->matches.push_back(match);
        }        
    }
}

const Utf8String& NumberRangeMatcher::GetText() const
{
    return this->text;
}

const NumberRangeMatcher::Matches& NumberRangeMatcher::GetMatches() const
{
    return this->matches;
}

//RangedNameGenerator::MatchIterators
bool RangedNameGenerator::MatchIterators::operator == (const std::vector<NumberRangeMatchIterator>& other) const
{
    if (size() != other.size())
        return false;

    for (size_t i = 0; i < size(); i++)
    {
        if ((*this)[i].iter != other[i].iter)
            return false;
    }

    return true;
}

bool RangedNameGenerator::MatchIterators::operator != (const std::vector<NumberRangeMatchIterator>& other) const
{
    return !(operator == (other));
}

//RangedNameGenerator::iterator
RangedNameGenerator::iterator::iterator() 
{
    this->isEnd = false; 
}

RangedNameGenerator::iterator& RangedNameGenerator::iterator::operator ++ ()
{
    if (!this->isEnd)
    {
        auto carry = true;

        if (!this->matchIterators.empty())
        {
            for (auto i = this->matchIterators.size() - 1; i != (size_t)-1 && carry; i--)
            {
                if (carry)
                    ++this->matchIterators[i].iter;

                carry = this->matchIterators[i].iter == this->matchIterators[i].match.end();
                if (carry)
                {
                    this->matchIterators[i].iter = this->matchIterators[i].match.begin();
                    if (i == 0)
                        this->isEnd = true;
                }
            }
        }
        else
            this->isEnd = true;
    }

    return *this;
}

bool RangedNameGenerator::iterator::operator == (const iterator& other) const 
{
    return this->isEnd == other.isEnd && this->matchIterators == other.matchIterators; 
}

bool RangedNameGenerator::iterator::operator != (const iterator& other) const 
{
    return !(operator == (other)); 
}

Utf8String RangedNameGenerator::iterator::operator * () const
{
    auto result = this->text;

    for (size_t i = 0; i < this->matchIterators.size(); i++)
    {
        auto replaceWith = this->matchIterators[i].iter.ToString();
        boost::algorithm::replace_first(result, this->matchIterators[i].match.text, replaceWith);
    }

    return result;
}

//RangedNameGenerator
RangedNameGenerator::RangedNameGenerator(const Utf8String& text) : matcher(text)
{
    this->matchIterators.resize(matcher.GetMatches().size());
    
    this->_begin.matchIterators.resize(matcher.GetMatches().size());
    this->_begin.text = text;
    
    this->_end.matchIterators.resize(matcher.GetMatches().size());
    this->_end.text = text;
    this->_end.isEnd = true;
    
    size_t matchIndex = 0;
    for (auto& match : this->matcher.GetMatches())
    {
        this->matchIterators[matchIndex].match = match;
        this->matchIterators[matchIndex].iter = match.begin();

        matchIndex++;
    }

    this->_begin.matchIterators = this->_end.matchIterators = this->matchIterators;
}

RangedNameGenerator::const_iterator RangedNameGenerator::begin() const
{
    return this->_begin;
}

RangedNameGenerator::const_iterator RangedNameGenerator::end() const
{
    return this->_end;
}
