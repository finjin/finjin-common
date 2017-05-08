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
#include "finjin/common/Convert.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"
#include <boost/lexical_cast.hpp>

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
template <typename T>
Utf8String FloatToString(T value)
{
    auto result = boost::lexical_cast<Utf8String>(value);
    result.ReplaceFirst(',', '.');
    while (result.length() > 1 && result.back() == '0')
        result.pop_back();
    if (!result.empty() && result.back() == '.')
        result.pop_back();
    return result;
}

template <typename T>
Utf8String ToHexString(T value)
{
    Utf8String result;

    size_t nibbleCount = sizeof(T) * 2;

    T nibbleOffset = ((T)nibbleCount - 1) * 4;

    //This results in all digits of the value being converted, including leading zeroes
    char hexDigitChar = 0;
    for (size_t i = 0; i < nibbleCount; i++)
    {
        auto nibbleValue = static_cast<char>((value >> nibbleOffset) & (T)0xf);
        if (nibbleValue >= 0 && nibbleValue <= 9)
            hexDigitChar = '0' + nibbleValue;
        else
            hexDigitChar = 'a' + (nibbleValue - 10);

        result += hexDigitChar;

        nibbleOffset -= 4;
    }

    return result;
}

//Number
template <typename T>
void ToNumberInRange(T& value, const Utf8String& stringValue, T minValue, T maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");
        return;
    }

    bool performedCast = false;

    try
    {
        value = boost::lexical_cast<T>(stringValue.c_str());
        performedCast = true;
    }
    catch (...)
    {
        //Conversion failed. Try turning ',' to '.', or vice versa
        if (stringValue.find('.') != Utf8String::npos)
        {
            auto alternateStringValue = stringValue;
            alternateStringValue.ReplaceAll('.', ',');
            try
            {
                value = boost::lexical_cast<T>(alternateStringValue.c_str());
                performedCast = true;
            }
            catch (...)
            {
                //Do nothing
            }
        }
        else if (stringValue.find(',') != Utf8String::npos)
        {
            auto alternateStringValue = stringValue;
            alternateStringValue.ReplaceAll(',', '.');
            try
            {
                value = boost::lexical_cast<T>(alternateStringValue.c_str());
                performedCast = true;
            }
            catch (...)
            {
                //Do nothing
            }
        }
    }

    if (!performedCast || value < minValue || value > maxValue)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("'%1%' could not be converted to a number. The value should be within [%2% - %3%]", stringValue, minValue, maxValue));
}

template <>
void ToNumberInRange<int8_t>(int8_t& value, const Utf8String& stringValue, int8_t minValue, int8_t maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    int intValue;
    int intMin = minValue;
    int intMax = maxValue;
    ::ToNumberInRange(intValue, stringValue, intMin, intMax, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    value = static_cast<int8_t>(intValue);
}

template <>
void ToNumberInRange<uint8_t>(uint8_t& value, const Utf8String& stringValue, uint8_t minValue, uint8_t maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    unsigned intValue;
    unsigned intMin = minValue;
    unsigned intMax = maxValue;
    ::ToNumberInRange(intValue, stringValue, intMin, intMax, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    value = static_cast<uint8_t>(intValue);
}

template <typename T>
T ToNumberInRange(const Utf8String& stringValue, T minValue, T maxValue, T defaultValue)
{
    if (stringValue.empty())
        return defaultValue;

    T result = defaultValue;

    try
    {
        result = boost::lexical_cast<T>(stringValue.c_str());
    }
    catch (...)
    {
        //Conversion failed. Try turning ',' to '.', or vice versa
        if (stringValue.find('.') != Utf8String::npos)
        {
            auto alternateStringValue = stringValue;
            alternateStringValue.ReplaceAll('.', ',');
            try
            {
                result = boost::lexical_cast<T>(alternateStringValue.c_str());
            }
            catch (...)
            {
                //Do nothing
            }
        }
        else if (stringValue.find(',') != Utf8String::npos)
        {
            auto alternateStringValue = stringValue;
            alternateStringValue.ReplaceAll(',', '.');
            try
            {
                result = boost::lexical_cast<T>(alternateStringValue.c_str());
            }
            catch (...)
            {
                //Do nothing
            }
        }
    }

    if (result < minValue || result > maxValue)
        result = defaultValue;

    return result;
}

template <>
int8_t ToNumberInRange<int8_t>(const Utf8String& stringValue, int8_t minValue, int8_t maxValue, int8_t defaultValue)
{
    int intMin = minValue;
    int intMax = maxValue;
    return static_cast<int8_t>(::ToNumberInRange(stringValue, intMin, intMax, (int)defaultValue));
}

template <>
uint8_t ToNumberInRange<uint8_t>(const Utf8String& stringValue, uint8_t minValue, uint8_t maxValue, uint8_t defaultValue)
{
    unsigned intMin = minValue;
    unsigned intMax = maxValue;
    return static_cast<uint8_t>(::ToNumberInRange(stringValue, intMin, intMax, (unsigned)defaultValue));
}

template <typename T>
void ToNumber(T& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    ToNumberInRange<T>(value, stringValue, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

template <typename T>
T ToNumber(const Utf8String& stringValue, T defaultValue)
{
    return ToNumberInRange<T>(stringValue, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), defaultValue);
}

//Integer
template <typename T>
void ToIntegerInRange(T& value, const Utf8String& stringValue, T minValue, T maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");
        return;
    }

    bool performedCast = false;

    if (stringValue.length() > 1 && (stringValue[0] == 'h' || stringValue[0] == 'H'))
    {
        //Hexadecimal
        value = static_cast<T>(strtoll(stringValue.begin() + 1, nullptr, 16));
        performedCast = true;
    }
    else if (stringValue.length() > 2 && stringValue[0] == '0' && (stringValue[1] == 'x' || stringValue[1] == 'X'))
    {
        //Hexadecimal
        value = static_cast<T>(strtoll(stringValue.begin() + 2, nullptr, 16));
        performedCast = true;
    }
    else if (stringValue.length() > 2 && stringValue[0] == '0' && stringValue[1] == 'o')
    {
        //Octal
        value = static_cast<T>(strtoll(stringValue.begin() + 2, nullptr, 8));
        performedCast = true;
    }
    else if (stringValue.length() > 1 && stringValue[0] == 'b')
    {
        //Binary
        value = static_cast<T>(strtoll(stringValue.begin() + 1, nullptr, 2));
        performedCast = true;
    }
    else
    {
        ::ToNumberInRange(value, stringValue, minValue, maxValue, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }
        else
            performedCast = true;
    }

    if (!performedCast || value < minValue || value > maxValue)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("'%1%' could not be converted to a number. The value should be within [%2% - %3%]", stringValue, minValue, maxValue));
}

template <typename T>
T ToIntegerInRange(const Utf8String& stringValue, T minValue, T maxValue, T defaultValue)
{
    if (stringValue.empty())
        return defaultValue;

    T result = defaultValue;

    if (stringValue.length() > 1 && (stringValue[0] == 'h' || stringValue[0] == 'H'))
    {
        //Hexadecimal
        result = static_cast<T>(strtoll(stringValue.begin() + 1, nullptr, 16));
    }
    else if (stringValue.length() > 2 && stringValue[0] == '0' && (stringValue[1] == 'x' || stringValue[1] == 'X'))
    {
        //Hexadecimal
        result = static_cast<T>(strtoll(stringValue.begin() + 2, nullptr, 16));
    }
    else if (stringValue.length() > 2 && stringValue[0] == '0' && stringValue[1] == 'o')
    {
        //Octal
        result = static_cast<T>(strtoll(stringValue.begin() + 2, nullptr, 8));
    }
    else if (stringValue.length() > 1 && stringValue[0] == 'b')
    {
        //Binary
        result = static_cast<T>(strtoll(stringValue.begin() + 1, nullptr, 2));
    }
    else
    {
        result = ::ToNumberInRange(stringValue, minValue, maxValue, defaultValue);
    }

    if (result < minValue || result > maxValue)
        result = defaultValue;

    return result;
}

template <typename T>
void ToInteger(T& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
    ToIntegerInRange<T>(value, stringValue, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

template <typename T>
T ToInteger(const Utf8String& stringValue, T defaultValue)
{
    return ToIntegerInRange<T>(stringValue, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), defaultValue);
}


//Implementation----------------------------------------------------------------

//Number
#if FINJIN_TARGET_PLATFORM_IS_APPLE
size_t Convert::ToNumber(const Utf8String& stringValue, size_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}
#endif

int8_t Convert::ToNumber(const Utf8String& stringValue, int8_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

uint8_t Convert::ToNumber(const Utf8String& stringValue, uint8_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

int16_t Convert::ToNumber(const Utf8String& stringValue, int16_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

uint16_t Convert::ToNumber(const Utf8String& stringValue, uint16_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

int32_t Convert::ToNumber(const Utf8String& stringValue, int32_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

uint32_t Convert::ToNumber(const Utf8String& stringValue, uint32_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

int64_t Convert::ToNumber(const Utf8String& stringValue, int64_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

uint64_t Convert::ToNumber(const Utf8String& stringValue, uint64_t defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

float Convert::ToNumber(const Utf8String& stringValue, float defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

double Convert::ToNumber(const Utf8String& stringValue, double defaultValue)
{
    return ::ToNumber(stringValue, defaultValue);
}

#if FINJIN_TARGET_PLATFORM_IS_APPLE
void Convert::ToNumber(size_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<size_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}
#endif

void Convert::ToNumber(int8_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<int8_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(uint8_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<uint8_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(int16_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<int16_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(uint16_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<uint16_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(int32_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<int32_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(uint32_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<uint32_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(int64_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<int64_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(uint64_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<uint64_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(float& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<float>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumber(double& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumber<double>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumberInRange(int& value, const Utf8String& stringValue, const int minValue, const int maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumberInRange<int>(value, stringValue, minValue, maxValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumberInRange(unsigned short& value, const Utf8String& stringValue, const unsigned short minValue, const unsigned short maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumberInRange<unsigned short>(value, stringValue, minValue, maxValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToNumberInRange(float& value, const Utf8String& stringValue, const float minValue, const float maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToNumberInRange<float>(value, stringValue, minValue, maxValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

//Integer
#if FINJIN_TARGET_PLATFORM_IS_APPLE
size_t Convert::ToInteger(const Utf8String& stringValue, size_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}
#endif

int8_t Convert::ToInteger(const Utf8String& stringValue, int8_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

uint8_t Convert::ToInteger(const Utf8String& stringValue, uint8_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

int16_t Convert::ToInteger(const Utf8String& stringValue, int16_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

uint16_t Convert::ToInteger(const Utf8String& stringValue, uint16_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

int32_t Convert::ToInteger(const Utf8String& stringValue, int32_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

uint32_t Convert::ToInteger(const Utf8String& stringValue, uint32_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

int64_t Convert::ToInteger(const Utf8String& stringValue, int64_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

uint64_t Convert::ToInteger(const Utf8String& stringValue, uint64_t defaultValue)
{
    return ::ToInteger(stringValue, defaultValue);
}

#if FINJIN_TARGET_PLATFORM_IS_APPLE
void Convert::ToInteger(size_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<size_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}
#endif

void Convert::ToInteger(int8_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<int8_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(uint8_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<uint8_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(int16_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<int16_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(uint16_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<uint16_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(int32_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<int32_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(uint32_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<uint32_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(int64_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<int64_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToInteger(uint64_t& value, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToInteger<uint64_t>(value, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToIntegerInRange(int& value, const Utf8String& stringValue, const int minValue, const int maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToIntegerInRange<int>(value, stringValue, minValue, maxValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToIntegerInRange(unsigned short& value, const Utf8String& stringValue, const unsigned short minValue, const unsigned short maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToIntegerInRange<unsigned short>(value, stringValue, minValue, maxValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Convert::ToIntegerInRange(float& value, const Utf8String& stringValue, const float minValue, const float maxValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ::ToIntegerInRange<float>(value, stringValue, minValue, maxValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

bool Convert::ToBool(const Utf8String& stringValue, bool defaultValue)
{
    auto result = defaultValue;

    if (!stringValue.empty())
    {
        if (stringValue.EqualsNoCaseAscii("t") ||
            stringValue.EqualsNoCaseAscii("true") ||
            stringValue.EqualsNoCaseAscii("y") ||
            stringValue.EqualsNoCaseAscii("yes") ||
            stringValue.EqualsNoCaseAscii("enable") ||
            stringValue.EqualsNoCaseAscii("enabled"))
        {
            result = true;
        }
        else if (stringValue.EqualsNoCaseAscii("f") ||
            stringValue.EqualsNoCaseAscii("false") ||
            stringValue.EqualsNoCaseAscii("n") ||
            stringValue.EqualsNoCaseAscii("no") ||
            stringValue.EqualsNoCaseAscii("disable") ||
            stringValue.EqualsNoCaseAscii("disabled"))
        {
            result = false;
        }
        else
        {
            try
            {
                result = boost::lexical_cast<int64_t>(stringValue) != 0;
            }
            catch (...)
            {
                //If it's not a 0, assume it's a 'true' boolean
                result = true;
            }
        }
    }

    return result;
}

const Utf8String& Convert::ToString(bool value)
{
    static const Utf8String yesString("yes");
    static const Utf8String noString("no");
    return value ? yesString : noString;
}

Utf8String Convert::ToString(int8_t value)
{
    return boost::lexical_cast<Utf8String>((int)value);
}

Utf8String Convert::ToString(uint8_t value)
{
    return boost::lexical_cast<Utf8String>((unsigned)value);
}

Utf8String Convert::ToString(int16_t value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToString(uint16_t value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToString(int32_t value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToString(uint32_t value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToString(int64_t value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToString(uint64_t value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToHexString(uint8_t value)
{
    return ::ToHexString<uint8_t>(value);
}

Utf8String Convert::ToHexString(uint16_t value)
{
    return ::ToHexString<uint16_t>(value);
}

Utf8String Convert::ToHexString(uint32_t value)
{
    return ::ToHexString<uint32_t>(value);
}

Utf8String Convert::ToHexString(uint64_t value)
{
    return ::ToHexString<uint64_t>(value);
}

Utf8String Convert::ToHexString(int8_t value)
{
    return ::ToHexString<int8_t>(static_cast<int8_t>(value));
}

Utf8String Convert::ToHexString(int16_t value)
{
    return ::ToHexString<int16_t>(static_cast<int16_t>(value));
}

Utf8String Convert::ToHexString(int32_t value)
{
    return ::ToHexString<int32_t>(static_cast<int32_t>(value));
}

Utf8String Convert::ToHexString(int64_t value)
{
    return ::ToHexString<int64_t>(static_cast<int64_t>(value));
}

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS || FINJIN_TARGET_PLATFORM_IS_APPLE
Utf8String Convert::ToString(long value)
{
    return boost::lexical_cast<Utf8String>(value);
}

Utf8String Convert::ToString(unsigned long value)
{
    return boost::lexical_cast<Utf8String>(value);
}
#endif

Utf8String Convert::ToString(float value)
{
    return FloatToString(value);
}

Utf8String Convert::ToString(double value)
{
    return FloatToString(value);
}
