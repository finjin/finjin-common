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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/SetMapImpl.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Dynamically allocated hash map table.
     * Note that default insert() behavior does not update the value as std::unordered_map does.
     * Instead, this needs to be forced by specifying true for the 'allowOverride' parameter.
     */
    template <typename KeyType, typename ValueType, typename MapPairType = MapPairConstructFirst<KeyType, ValueType>, typename Hash = std::hash<KeyType> >
    class DynamicUnorderedMap
    {
    public:
        using value_type = MapPairType;
        using hash_value_type = size_t;

        using ValueEntry = SetMapValueEntryImpl<hash_value_type, value_type>;
        using BucketEntry = SetMapBucketEntryImpl<hash_value_type, value_type>;

        using ValueCollection = DynamicVector<ValueEntry>;
        using BucketCollection = DynamicVector<BucketEntry>;

        using This = DynamicUnorderedMap<KeyType, ValueType, MapPairType, Hash>;
        using Impl = UnorderedMapImpl<This, ValueCollection, BucketCollection, Hash, KeyType, ValueType, MapPairType>;

        using iterator = typename Impl::iterator;
        using const_iterator = typename Impl::const_iterator;

        DynamicUnorderedMap() { }
        DynamicUnorderedMap(const DynamicUnorderedMap& other) { operator = (other); }
        DynamicUnorderedMap(DynamicUnorderedMap&& other) { operator = (std::move(other)); }

        template <typename... Args>
        bool Create(size_t valueCount, size_t bucketCount, Allocator* allocator, Args&&... args)
        {
            Destroy();

            bucketCount = GetOdd(bucketCount);

            auto result = true;
            result &= impl.bucketEntries.Create(bucketCount, allocator);
            result &= impl.valueEntries.Create(valueCount, allocator, std::forward<Args>(args)...);

            clear();

            return result;
        }

        void Destroy()
        {
            impl.bucketEntries.Destroy();
            impl.valueEntries.Destroy();

            clear();
        }

        ValueOrError<void> operator = (const DynamicUnorderedMap& other) { return impl.assign(other); }
        ValueOrError<void> operator = (DynamicUnorderedMap&& other) { return impl.assign(std::move(other)); }

        ValueOrError<bool> insert(const KeyType& key, const ValueType& value, bool allowOverwrite) { return impl.insert(key, value, allowOverwrite); }
        ValueOrError<bool> insert(KeyType&& key, ValueType&& value, bool allowOverwrite) { return impl.insert(std::move(key), std::move(value), allowOverwrite); }

        ValueOrError<bool> insert_or_assign(const KeyType& key, const ValueType& value) { return impl.insert(key, value, true); }
        ValueOrError<bool> insert_or_assign(KeyType&& key, ValueType&& value) { return impl.insert(std::move(key), std::move(value), true); }

        bool contains(const KeyType& key) const { return find(key) != end(); }

        iterator find(const KeyType& key) { return impl.find(key); }
        const_iterator find(const KeyType& key) const { return impl.find(key); }

        void clear() { impl.clear(); }

        iterator erase(iterator iter) { return impl.erase(iter); }

        bool remove(const KeyType& key) { return impl.remove(key); }

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
        Impl impl;
    };

} }
