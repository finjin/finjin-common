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

    template <typename EnumType, EnumType count, typename ValueType>
    class EnumArray
    {
    public:
        typedef ValueType* iterator;
        typedef const ValueType* const_iterator;

        EnumArray() {}

        template <typename InitialValue>
        EnumArray(const InitialValue& initialValue)
        {
            for (size_t i = 0; i < (size_t)count; i++)
                this->items[i] = initialValue;
        }
        
        template <typename... Args>
        EnumArray(Args... args) { Construct(args...); }

        size_t size() const { return (size_t)count; }
        size_t max_size() const { return (size_t)count; }

        template <typename Index> const ValueType& operator [] (Index index) const { return this->items[(size_t)index]; }
        template <typename Index> ValueType& operator [] (Index index) { return this->items[(size_t)index]; }

        const ValueType* data() const { return &this->items[0]; }
        ValueType* data() { return &this->items[0]; }

        iterator begin() { return &this->items[0]; }
        iterator end() { return &this->items[(size_t)count]; }

        const_iterator begin() const { return &this->items[0]; }
        const_iterator end() const { return &this->items[(size_t)count]; }
        
    private:
        template <typename... Args>
        void Construct(EnumType index, const ValueType& value, Args... args)
        {
            Construct(index, value);
            
            Construct(args...);
        }
        
        void Construct(EnumType index, const ValueType& value)
        {
            this->items[(size_t)index] = value;
        }
        
        void Construct()
        {
            //Do nothing
        }

    private:
        ValueType items[(size_t)count];
    };

} }
