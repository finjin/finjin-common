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


//Macros------------------------------------------------------------------------
/**
 * Helps to define a bucket count for static maps that are likely to be full.
 */
#define FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(count) (size_t(count) * 3 | (size_t)1)

/**
 * Defines a lookup table with known number of literal string keys and corresponding values.
 * For this to compile without error, the Utf8String.hpp header must also be included
 */
#define FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(valueType, count) StaticUnorderedMap<const char*, valueType, size_t(count), FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(count), Finjin::Common::MapPairConstructNone<const char*, valueType>, Finjin::Common::Utf8StringHash, Finjin::Common::Utf8StringEqual>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Statically allocated hash map table.
     * Note that default insert() behavior does not update the value as std::unordered_map does.
     * Instead, this needs to be forced by specifying true for the 'allowOverride' parameter.
     */
    template <typename KeyType, typename ValueType, size_t ValueCount, size_t BucketCount, typename MapPairType = MapPairConstructFirst<KeyType, ValueType>, typename Hash = std::hash<KeyType>, typename KeyEqual = std::equal_to<KeyType> >
    class StaticUnorderedMap
    {
    public:
        using value_type = MapPairType;
        using hash_value_type = size_t;

        using ValueEntry = SetMapValueEntryImpl<hash_value_type, value_type>;
        using BucketEntry = SetMapBucketEntryImpl<hash_value_type, value_type>;

        using ValueCollection = std::array<ValueEntry, ValueCount>;
        using BucketCollection = std::array<BucketEntry, BucketCount>;

        using This = StaticUnorderedMap<KeyType, ValueType, ValueCount, BucketCount, MapPairType, Hash, KeyEqual>;
        using Impl = UnorderedMapImpl<This, ValueCollection, BucketCollection, Hash, KeyType, ValueType, MapPairType, KeyEqual>;

        using iterator = typename Impl::iterator;
        using const_iterator = typename Impl::const_iterator;

        StaticUnorderedMap() {}

        template <typename... Args>
        StaticUnorderedMap(Args... args) { ConstructAdd(args...); }

        StaticUnorderedMap(const StaticUnorderedMap& other) { operator = (other); }
        StaticUnorderedMap(StaticUnorderedMap&& other) { operator = (std::move(other)); }

        ValueOrError<void> operator = (const StaticUnorderedMap& other) { return impl.assign(other); }
        ValueOrError<void> operator = (StaticUnorderedMap&& other) { return impl.assign(std::move(other)); }

        ValueOrError<bool> insert(const KeyType& key, const ValueType& value, bool allowOverwrite) { return impl.insert(key, value, allowOverwrite); }
        ValueOrError<bool> insert(KeyType&& key, ValueType&& value, bool allowOverwrite) { return impl.insert(std::move(key), std::move(value), allowOverwrite); }

        ValueOrError<bool> insert_or_assign(const KeyType& key, const ValueType& value, bool allowOverwrite) { return impl.insert(key, value, true); }
        ValueOrError<bool> insert_or_assign(KeyType&& key, ValueType&& value, bool allowOverwrite) { return impl.insert(std::move(key), std::move(value), true); }

        template <typename FindKeyType>
        bool contains(const FindKeyType& key) const { return find(key) != end(); }

        template <typename FindKeyType>
        iterator find(const FindKeyType& key) { return impl.find(key); }
        template <typename FindKeyType>
        const_iterator find(const FindKeyType& key) const { return impl.find(key); }

        void clear() { impl.clear(); }

        iterator erase(iterator iter) { return impl.erase(iter); }

        template <typename FindKeyType>
        bool remove(const FindKeyType& key) { return impl.remove(key); }

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

        ValueOrError<ValueType*> GetOrAdd(const KeyType& key) { return impl.GetOrAdd(key); }

        size_t GetCollisionCount() const { return impl.GetCollisionCount(); }
        float GetCollisionRatio() const { return impl.GetCollisionRatio(); }

    private:
        template <typename... Args>
        void ConstructAdd(const KeyType& key, const ValueType& value, Args... args)
        {
            ConstructAdd(key, value);

            ConstructAdd(args...);
        }

        void ConstructAdd(const KeyType& key, const ValueType& value)
        {
            auto result = insert(key, value, false);
            assert(!result.HasErrorOrValue(false));
        }

        void ConstructAdd()
        {
            //Do nothing
        }

    private:
        Impl impl;
    };

} }
