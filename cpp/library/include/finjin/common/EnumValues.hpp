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
    struct EnumValues
    {
        std::array<ValueType, (size_t)count> items;

        size_t size() const { return this->items.size(); }
        size_t max_size() const { return this->items.max_size(); }

        const ValueType& operator [] (EnumType enumValue) const { return this->items[(size_t)enumValue]; }
        ValueType& operator [] (EnumType enumValue) { return this->items[(size_t)enumValue]; }

        const ValueType& operator [] (int i) const { return this->items[i]; }
        ValueType& operator [] (int i) { return this->items[i]; }

        const ValueType& operator [] (size_t i) const { return this->items[i]; }
        ValueType& operator [] (size_t i) { return this->items[i]; }

        const ValueType& Get(EnumType enumValue) const { return this->items[(size_t)enumValue]; }
        ValueType& Get(EnumType enumValue) { return this->items[(size_t)enumValue]; }

        const ValueType& Get(int i) const { return this->items[i]; }
        ValueType& Get(int i) { return this->items[i]; }

        const ValueType& Get(size_t i) const { return this->items[i]; }
        ValueType& Get(size_t i) { return this->items[i]; }

        ValueType* begin() { return this->items.data(); }
        ValueType* end() { return this->items.data() + (size_t)count; }

        const ValueType* begin() const { return this->items.data(); }
        const ValueType* end() const { return this->items.data() + (size_t)count; }
    };

} }
