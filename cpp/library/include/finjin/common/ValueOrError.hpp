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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * ValueOrError is used in cases where using the more complex Error or ErrorState is inappropriate.
     * These cases are:
     *   1)Utf8String - Error depends on this Utf8String, so Utf8String shouldn't use Error.
     *   2)Path - This class is modeled after Utf8String, which doesn't use Error.
     *   3)Anywhere recursion is or might be used - The Error class has a fixed size, so using Error in a recursive method is problematic.
     *   4)Simple functions where it might be overkill or inconvenient to be forced to pass in an Error object.
     */
    template <typename T>
    class ValueOrError
    {
    public:
        ValueOrError() : type(NONE)
        {
        }

        ValueOrError(const T& _value) : type(VALUE), value(_value)
        {
        }

        static ValueOrError CreateError()
        {
            ValueOrError result;
            result.type = HAS_ERROR;
            return result;
        }

        bool HasValue() const
        {
            return this->type == VALUE;
        }

        template <typename OtherType>
        bool HasValue(const OtherType& value) const
        {
            return this->type == VALUE && this->value == value;
        }

        bool HasError() const
        {
            return this->type == HAS_ERROR;
        }

        template <typename OtherType>
        bool HasErrorOrValue(const OtherType& value) const
        {
            return this->type == HAS_ERROR || (this->type == VALUE && this->value == value);
        }

        T value;
    
    private:
        enum Type
        {
            NONE,
            VALUE,
            HAS_ERROR
        };
        Type type;
    };

    template <>
    class ValueOrError<void>
    {
    public:
        ValueOrError() : hasError(false)
        {
        }

        static ValueOrError CreateError()
        {
            ValueOrError result;
            result.hasError = true;
            return result;
        }

        bool HasValue() const
        {
            return false;
        }

        template <typename OtherType>
        bool HasValue(const OtherType& value) const
        {
            return false;
        }

        bool HasError() const
        {
            return this->hasError;
        }

        template <typename OtherType>
        bool HasErrorOrValue(const OtherType& value) const
        {
            return this->hasError;
        }

    private:
        bool hasError;
    };

} }
