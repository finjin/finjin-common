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
    
    template <typename EnumType, EnumType count, typename ValueType>
    class EnumValues
    {
    public:
        typedef ValueType* iterator;
        typedef const ValueType* const_iterator;
        
        size_t size() const { return (size_t)count; }
        size_t max_size() const { return (size_t)count; }

        const ValueType& operator [] (EnumType enumValue) const { return this->items[(size_t)enumValue]; }
        ValueType& operator [] (EnumType enumValue) { return this->items[(size_t)enumValue]; }

        const ValueType& operator [] (int i) const { return this->items[i]; }
        ValueType& operator [] (int i) { return this->items[i]; }

        const ValueType& operator [] (size_t i) const { return this->items[i]; }
        ValueType& operator [] (size_t i) { return this->items[i]; }

        iterator begin() { return &this->items[0]; }
        iterator end() { return &this->items[(size_t)count]; }

        const_iterator begin() const { return &this->items[0]; }
        const_iterator end() const { return &this->items[(size_t)count]; }
        
    private:
        ValueType items[(size_t)count];
    };

} }
