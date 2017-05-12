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
#include "finjin/common/SetMapImpl.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T, size_t ValueCount, size_t BucketCount, typename Hash = std::hash<T>, typename ValueEqual = std::equal_to<T> >
    class StaticUnorderedSet
    {
    public:
        using value_type = T;
        using hash_value_type = size_t;

        using ValueEntry = SetMapValueEntryImpl<hash_value_type, value_type>;
        using BucketEntry = SetMapBucketEntryImpl<hash_value_type, value_type>;

        using ValueCollection = std::array<ValueEntry, ValueCount>;
        using BucketCollection = std::array<BucketEntry, BucketCount>;

        using This = StaticUnorderedSet<T, ValueCount, BucketCount, Hash, ValueEqual>;
        using Impl = UnorderedSetImpl<This, ValueCollection, BucketCollection, Hash, T, ValueEqual>;

        using iterator = typename Impl::iterator;
        using const_iterator = typename Impl::const_iterator;

        StaticUnorderedSet() { }
        StaticUnorderedSet(const StaticUnorderedSet& other) { operator = (other); }
        StaticUnorderedSet(StaticUnorderedSet&& other) { operator = (std::move(other)); }

        ValueOrError<void> operator = (const StaticUnorderedSet& other) { return impl.assign(other); }
        ValueOrError<void> operator = (StaticUnorderedSet&& other) { return impl.assign(std::move(other)); }

        ValueOrError<bool> insert(const value_type& value) { return impl.insert(value); }
        ValueOrError<bool> insert(value_type&& value) { return impl.insert(std::move(value)); }

        template <typename FindValueType>
        bool contains(const FindValueType& value) const { return find(value) != end(); }

        template <typename FindValueType>
        iterator find(const FindValueType& value) { return impl.find(value); }
        template <typename FindValueType>
        const_iterator find(const FindValueType& value) const { return impl.find(value); }

        void clear() { impl.clear(); }

        iterator erase(iterator iter) { return impl.erase(iter); }

        template <typename FindValueType>
        bool remove(const FindValueType& value) { return impl.remove(value); }

        size_t size() const { return impl.count; }
        size_t max_size() const { return impl.valueEntries.size(); }
        size_t size_left() const { return max_size() - size(); }

        bool empty() const { return impl.count == 0; }
        bool full() const { return impl.count == impl.valueEntries.size(); }

        iterator begin() { return impl.begin(); }
        iterator end() { return impl.end(); }

        const_iterator begin() const { return impl.begin(); }
        const_iterator end() const { return impl.end(); }

        ValueCollection& GetValueEntries() { return impl.valueEntries; }

        size_t GetCollisionCount() const { return impl.GetCollisionCount(); }
        float GetCollisionRatio() const { return impl.GetCollisionRatio(); }

    private:
        Impl impl;
    };

} }
