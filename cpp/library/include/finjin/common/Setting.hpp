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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Container for one setting.
     * This class is similar to boost::optional, except that it always has a value.
     */
    template <typename T>
    class Setting
    {
    public:
        /** Default constructor. The setting will be considered to be 'not set'. */
        explicit Setting() {this->isSet = false;}

        /**
         * Constructs setting from default values. The setting will be considered to be 'not set'.
         * @param defaultValue [in] The setting default value.
         */
    #if defined(FINJIN_EXPORTER)
        explicit Setting(const T& _value) : value(_value) {this->isSet = false;}
    #else
        template <typename... Args>
        Setting(Args&&... args) : value(std::forward<Args>(args)...) {this->isSet = false;}
    #endif

        /**
         * Copy constructor.
         * The setting's value and 'is set' state will be copied from the setting.
         * @param other [in] The setting to copy.
         */
        explicit Setting(const Setting<T>& other) : value(other.value), isSet(other.IsSet()) {}

        /**
         * Gets a reference to the setting value.
         * @return A reference to the setting value.
         */
        operator T& () {return this->value;}

        /**
         * Gets a reference to the setting value.
         * @return A reference to the setting value.
         */
        operator const T& () const {return this->value;}

        /**
         * Equality operator.
         * @param other [in] The setting to test. Only the value is tested, not the 'is set' status.
         * @return If the two settings values are equal, true is returned. Otherwise, false is returned.
         */
        template <typename Value>
        bool operator == (const Value& other) const {return this->value == other;}

        template <typename Value>
        bool operator != (const Value& other) const {return this->value != other;}

        /**
         * Indicates whether the setting value has been explicitly set/changed.
         * @return Whether the setting value has been explicitly set/changed.
         */
        bool IsSet() const {return this->isSet;}

        void Reset() {this->isSet = false;}

        template <typename Value>
        void Reset(const Value& defaultValue) {this->value = defaultValue; this->isSet = false;}

        /**
         * Value assignment operator. Use of this operator causes the the state of the setting to be 'set'.
         * @param value [in] The value to assign.
         * @return Reference to 'this'.
         */
        template <typename Value>
        T& operator = (const Value& value) {this->value = value; this->isSet = true; return this->value;}

        /**
         * Setting copy operator. Use of this operator causes the the state of the setting to take on the state of 'other'.
         * @param other [in] The setting to copy.
         * @return Reference to 'this'.
         */
        Setting<T>& operator = (const Setting<T>& other) {this->isSet = other.IsSet(); this->value = other.value; return *this;}

        /**
         * Setting 'combine' operator.
         * @param other [in] The other value to combine into 'this'. If the status of the other setting is not 'set', nothing is done.
         * @return Reference to 'this'.
         */
        Setting<T>& operator += (const Setting<T>& other)
        {
            if (other.IsSet())
                *this = other.value;
            return *this;
        }

        /**
         * Setting 'combine' operator.
         * @param other [in] The other value to combine into a new setting. If the status of the other setting is not 'set', nothing is done.
         * @return Copy of newly combined setting.
        */
        Setting<T> operator + (const Setting<T>& other) const
        {
            Setting<T> combined(*this);
            combined += other;
            return combined;
        }

    public:
        /** The setting value. To change this value, it is preferable to use the assignment operator. */
        T value;

        bool isSet;
    };

} }
