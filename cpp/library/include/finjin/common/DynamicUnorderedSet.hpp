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

    template <typename T, typename Hash = std::hash<T> >
    class DynamicUnorderedSet
    {
    public:
        using value_type = T;
        using hash_value_type = size_t;

        using ValueEntry = SetMapValueEntryImpl<hash_value_type, value_type>;
        using BucketEntry = SetMapBucketEntryImpl<hash_value_type, value_type>;

        using ValueCollection = DynamicVector<ValueEntry>;
        using BucketCollection = DynamicVector<BucketEntry>;

        using This = DynamicUnorderedSet<T, Hash>;
        using Impl = UnorderedSetImpl<This, ValueCollection, BucketCollection, Hash, T>;

        using iterator = typename Impl::iterator;
        using const_iterator = typename Impl::const_iterator;

        DynamicUnorderedSet() { }
        DynamicUnorderedSet(const DynamicUnorderedSet& other) { operator = (other); }
        DynamicUnorderedSet(DynamicUnorderedSet&& other) { operator = (std::move(other)); }

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

        ValueOrError<void> operator = (const DynamicUnorderedSet& other) { return impl.assign(other); }
        ValueOrError<void> operator = (DynamicUnorderedSet&& other) { return impl.assign(std::move(other)); }

        ValueOrError<bool> insert(const value_type& value) { return impl.insert(value); }
        ValueOrError<bool> insert(value_type&& value) { return impl.insert(std::move(value)); }

        bool contains(const value_type& value) const { return find(value) != end(); }

        iterator find(const value_type& value) { return impl.find(value); }
        const_iterator find(const value_type& value) const { return impl.find(value); }

        void clear() { impl.clear(); }

        iterator erase(iterator iter) { return impl.erase(iter); }

        bool remove(const value_type& value) { return impl.remove(value); }

        size_t size() const { return impl.count; }
        size_t max_size() const { return impl.valueEntries.size(); }

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

    template <typename T>
    class DynamicEncounteredSet
    {
    public:
        bool Create(bool useObjects, size_t maxCount, Allocator* allocator)
        {
            if (useObjects)
                return this->objects.Create(maxCount, maxCount, allocator, allocator);
            else
                return this->hashes.Create(maxCount, maxCount, allocator);
        }

        void Destroy()
        {
            this->objects.Destroy();
            this->hashes.Destroy();
        }

        ValueOrError<bool> insert(const T& filePath)
        {
            if (this->objects.max_size() > 0)
                return this->objects.insert(filePath);
            else
            {
                std::hash<T> hash;
                return this->hashes.insert(hash(filePath));
            }
        }

        size_t size() const { return this->objects.max_size() > 0 ? this->objects.size() : this->hashes.size(); }
        size_t max_size() const { return this->objects.max_size() > 0 ? this->objects.max_size() : this->hashes.max_size(); }
        size_t size_left() const { return max_size() - size(); }

        bool empty() const { return this->objects.max_size() > 0 ? this->objects.empty() : this->hashes.empty(); }
        bool full() const { return this->objects.max_size() > 0 ? this->objects.full() : this->hashes.full(); }

    public:
        DynamicUnorderedSet<T> objects;
        DynamicUnorderedSet<size_t> hashes;
    };

} }
