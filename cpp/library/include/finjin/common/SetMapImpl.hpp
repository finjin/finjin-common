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
#include "finjin/common/AssignOrError.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename HashValueType, typename ValueType>
    struct SetMapValueEntryImpl
    {
        template <typename... Args>
        SetMapValueEntryImpl(Args&&... args) : value(std::forward<Args>(args)...)
        {
            hashValue = 0;
            this->next = nullptr;
        }

        SetMapValueEntryImpl* next;
        HashValueType hashValue;
        ValueType value;
    };

    template <typename HashValueType, typename ValueType>
    struct SetMapBucketEntryImpl
    {
        SetMapBucketEntryImpl()
        {
            this->valueEntriesHead = nullptr;
        }

        SetMapValueEntryImpl<HashValueType, ValueType>* valueEntriesHead;
    };

    template <typename FirstType, typename SecondType>
    struct MapPairConstructFirst
    {
        using first_type = FirstType;
        using second_type = SecondType;

        template <typename... Args>
        MapPairConstructFirst(Args&&... args) : first(std::forward<Args>(args)...)
        {
        }

        first_type first; //The key
        second_type second; //The value
    };

    template <typename FirstType, typename SecondType>
    struct MapPairConstructSecond
    {
        using first_type = FirstType;
        using second_type = SecondType;

        template <typename... Args>
        MapPairConstructSecond(Args&&... args) : second(std::forward<Args>(args)...)
        {
        }

        first_type first; //The key
        second_type second; //The value
    };

    template <typename FirstType, typename SecondType>
    struct MapPairConstructBoth
    {
        using first_type = FirstType;
        using second_type = SecondType;

        template <typename... Args>
        MapPairConstructBoth(Args&&... args) : first(std::forward<Args>(args)...), second(std::forward<Args>(args)...)
        {
        }

        first_type first; //The key
        second_type second; //The value
    };

    template <typename FirstType, typename SecondType>
    struct MapPairConstructNone
    {
        using first_type = FirstType;
        using second_type = SecondType;

        template <typename... Args>
        MapPairConstructNone(Args&&... args)
        {
        }

        first_type first; //The key
        second_type second; //The value
    };

    template <typename PairType, typename KeyEqual = std::equal_to<typename PairType::first_type> >
    struct MapPairEqualTo
    {
        using first_argument_type = typename PairType::first_type;
        using second_argument_type = typename PairType::second_type;
        typedef bool result_type;

        template <typename Other>
        result_type operator () (const PairType& a, const Other& b) const
        {
            return equals(a.first, b);
        }

        KeyEqual equals;
    };

    template <typename Owner, typename ValueCollectionType, typename BucketCollectionType, typename Hash, typename KeyType, typename KeyEqual>
    struct UnorderedSetMapImpl
    {
        using value_type = typename Owner::value_type;
        using hash_value_type = typename Owner::hash_value_type;

        using ValueEntry = typename ValueCollectionType::value_type;
        using BucketEntry = typename BucketCollectionType::value_type;

        using ValueCollection = ValueCollectionType;
        using BucketCollection = BucketCollectionType;

        struct iterator
        {
            iterator()
            {
                this->impl = nullptr;
                this->bucketEntry = nullptr;
                this->previousValueEntry = nullptr;
                this->valueEntry = nullptr;
                this->stepCount = 0;
            }

            iterator(UnorderedSetMapImpl* impl, BucketEntry* bucketEntry, ValueEntry* previousValueEntry, ValueEntry* valueEntry, size_t stepCount)
            {
                this->impl = impl;
                this->bucketEntry = bucketEntry;
                this->previousValueEntry = previousValueEntry;
                this->valueEntry = valueEntry;
                this->stepCount = stepCount;
            }

            bool operator == (const iterator& other) const { return this->bucketEntry == other.bucketEntry && this->valueEntry == other.valueEntry; }
            bool operator != (const iterator& other) const { return this->bucketEntry != other.bucketEntry || this->valueEntry != other.valueEntry; }

            iterator& operator ++ ()
            {
                //Prefix operator
                if (this->impl == nullptr || this->bucketEntry == nullptr || this->valueEntry == nullptr)
                {
                    *this = iterator();
                    return *this;
                }

                this->impl->advance(*this);
                return *this;
            }

            iterator operator ++ (int)
            {
                //Postfix operator
                if (this->impl == nullptr || this->bucketEntry == nullptr || this->valueEntry == nullptr)
                    return iterator();

                auto result = *this;
                this->impl->advance(*this);
                return result;
            }

            value_type& operator * ()
            {
                assert(this->valueEntry != nullptr);
                return this->valueEntry->value;
            }

            value_type* operator -> ()
            {
                assert(this->valueEntry != nullptr);
                return &this->valueEntry->value;
            }

            UnorderedSetMapImpl* impl;
            BucketEntry* bucketEntry;
            ValueEntry* previousValueEntry;
            ValueEntry* valueEntry;
            size_t stepCount;
        };

        struct const_iterator
        {
            const_iterator()
            {
                this->impl = nullptr;
                this->bucketEntry = nullptr;
                this->previousValueEntry = nullptr;
                this->valueEntry = nullptr;
                this->stepCount = 0;
            }

            const_iterator(const UnorderedSetMapImpl* impl, const BucketEntry* bucketEntry, const ValueEntry* previousValueEntry, const ValueEntry* valueEntry, size_t stepCount)
            {
                this->impl = impl;
                this->bucketEntry = bucketEntry;
                this->previousValueEntry = previousValueEntry;
                this->valueEntry = valueEntry;
                this->stepCount = stepCount;
            }

            bool operator == (const const_iterator& other) const { return this->bucketEntry == other.bucketEntry && this->valueEntry == other.valueEntry; }
            bool operator != (const const_iterator& other) const { return this->bucketEntry != other.bucketEntry || this->valueEntry != other.valueEntry; }

            const_iterator& operator ++ ()
            {
                //Prefix operator
                if (this->impl == nullptr || this->bucketEntry == nullptr || this->valueEntry == nullptr)
                {
                    *this = const_iterator();
                    return *this;
                }

                this->impl->advance(*this);
                return *this;
            }

            const_iterator operator ++ (int)
            {
                //Postfix operator
                if (this->impl == nullptr || this->bucketEntry == nullptr || this->valueEntry == nullptr)
                    return const_iterator();

                auto result = *this;
                this->impl->advance(*this);
                return result;
            }

            const value_type& operator * () const
            {
                assert(this->valueEntry != nullptr);
                return this->valueEntry->value;
            }

            const value_type* operator -> () const
            {
                assert(this->valueEntry != nullptr);
                return &this->valueEntry->value;
            }

            const UnorderedSetMapImpl* impl;
            const BucketEntry* bucketEntry;
            const ValueEntry* previousValueEntry;
            const ValueEntry* valueEntry;
            size_t stepCount;
        };

        UnorderedSetMapImpl()
        {
            clear();
        }

        void clear()
        {
            for (auto& bucketEntry : this->bucketEntries)
                bucketEntry.valueEntriesHead = nullptr;

            if (!this->valueEntries.empty())
            {
                for (size_t i = 0; i < this->valueEntries.size() - 1; i++)
                    this->valueEntries[i].next = &this->valueEntries[i + 1];
                this->valueEntries[this->valueEntries.size() - 1].next = nullptr;

                this->unusedValueEntries = &this->valueEntries[0];
            }
            else
                this->unusedValueEntries = nullptr;

            this->count = 0;
            this->collisionCount = 0;
        }

        iterator begin()
        {
            if (this->count > 0)
            {
                for (auto& bucketEntry : this->bucketEntries)
                {
                    if (bucketEntry.valueEntriesHead != nullptr)
                        return iterator(this, &bucketEntry, nullptr, bucketEntry.valueEntriesHead, 1);
                }
            }

            return iterator();
        }

        iterator end()
        {
            return iterator();
        }

        const_iterator begin() const
        {
            if (this->count > 0)
            {
                for (auto& bucketEntry : this->bucketEntries)
                {
                    if (bucketEntry.valueEntriesHead != nullptr)
                        return const_iterator(this, &bucketEntry, nullptr, bucketEntry.valueEntriesHead, 1);
                }
            }

            return const_iterator();
        }

        const_iterator end() const
        {
            return const_iterator();
        }

        iterator erase(iterator iter)
        {
            if (iter == end())
                return iter;

            auto next = iter;
            ++next;
            next.previousValueEntry = iter.previousValueEntry;

            //Decrement collision count
            if (iter.previousValueEntry != nullptr || iter.valueEntry->next != nullptr)
                this->collisionCount--;

            if (iter.previousValueEntry == nullptr)
            {
                //First value entry in table entry. Update head
                iter.bucketEntry->valueEntriesHead = iter.valueEntry->next;
            }
            else
            {
                //Non-first value entry. Detach from list
                iter.previousValueEntry->next = iter.valueEntry->next;
            }

            //Add value entry back to unused list
            iter.valueEntry->next = this->unusedValueEntries;
            this->unusedValueEntries = iter.valueEntry;

            //Decrement count
            this->count--;

            return next;
        }

        template <typename FindKeyType>
        bool remove(const FindKeyType& key)
        {
            auto foundAt = find(key);
            if (foundAt != end())
            {
                erase(foundAt);
                return true;
            }
            else
                return false;
        }

        template <typename FindKeyType>
        iterator find(const FindKeyType& key)
        {
            if (!this->valueEntries.empty())
            {
                auto hashValue = hash(key);
                auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
                auto& bucketEntry = this->bucketEntries[bucketEntryIndex];

                if (bucketEntry.valueEntriesHead != nullptr)
                {
                    size_t stepCount = 1;
                    decltype(bucketEntry.valueEntriesHead) previousValueEntry = nullptr;
                    auto currentValueEntry = bucketEntry.valueEntriesHead;
                    for (; currentValueEntry != nullptr; previousValueEntry = currentValueEntry, currentValueEntry = currentValueEntry->next)
                    {
                        if (keyEqual(currentValueEntry->value, key))
                        {
                            //Found an entry with the key
                            return iterator(this, &bucketEntry, previousValueEntry, currentValueEntry, stepCount);
                        }
                        stepCount++;
                    }
                }
            }

            return end();
        }

        template <typename FindKeyType>
        const_iterator find(const FindKeyType& key) const
        {
            if (!this->valueEntries.empty())
            {
                auto hashValue = hash(key);
                auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
                auto& bucketEntry = this->bucketEntries[bucketEntryIndex];

                if (bucketEntry.valueEntriesHead != nullptr)
                {
                    size_t stepCount = 1;
                    decltype(bucketEntry.valueEntriesHead) previousValueEntry = nullptr;
                    auto currentValueEntry = bucketEntry.valueEntriesHead;
                    for (; currentValueEntry != nullptr; previousValueEntry = currentValueEntry, currentValueEntry = currentValueEntry->next)
                    {
                        if (keyEqual(currentValueEntry->value, key))
                        {
                            //Found an entry with the key
                            return const_iterator(this, &bucketEntry, previousValueEntry, currentValueEntry, stepCount);
                        }
                        stepCount++;
                    }
                }
            }

            return end();
        }

        size_t GetCollisionCount() const
        {
            return this->collisionCount;
        }

        float GetCollisionRatio() const
        {
            if (this->count > 0)
                return RoundToFloat(this->collisionCount) / RoundToFloat(this->count);
            else
                return 0;
        }

        template <typename Iter>
        void advance(Iter& it) const
        {
            if (it.valueEntry->next != nullptr)
            {
                it.previousValueEntry = it.valueEntry;
                it.valueEntry = it.valueEntry->next;
            }
            else
            {
                for (size_t bucketEntryIndex = (it.bucketEntry - &this->bucketEntries[0]) + 1; bucketEntryIndex < this->bucketEntries.size(); bucketEntryIndex++)
                {
                    auto& bucketEntry = this->bucketEntries[bucketEntryIndex];
                    if (bucketEntry.valueEntriesHead != nullptr)
                    {
                        it = Iter(it.impl, &bucketEntry, nullptr, bucketEntry.valueEntriesHead, 1);
                        return;
                    }
                }

                it = Iter();
            }
        }

        template <typename Iter>
        void advance(Iter& it)
        {
            if (it.valueEntry->next != nullptr)
            {
                it.previousValueEntry = it.valueEntry;
                it.valueEntry = it.valueEntry->next;
            }
            else
            {
                for (size_t bucketEntryIndex = (it.bucketEntry - &this->bucketEntries[0]) + 1; bucketEntryIndex < this->bucketEntries.size(); bucketEntryIndex++)
                {
                    auto& bucketEntry = this->bucketEntries[bucketEntryIndex];
                    if (bucketEntry.valueEntriesHead != nullptr)
                    {
                        it = Iter(it.impl, &bucketEntry, nullptr, bucketEntry.valueEntriesHead, 1);
                        return;
                    }
                }

                it = Iter();
            }
        }

        BucketCollectionType bucketEntries;
        ValueCollectionType valueEntries;
        ValueEntry* unusedValueEntries;
        size_t count;
        size_t collisionCount;
        Hash hash;
        KeyEqual keyEqual;
    };

    template <typename Owner, typename ValueCollectionType, typename BucketCollectionType, typename Hash, typename KeyType, typename KeyEqual = std::equal_to<typename Owner::value_type> >
    struct UnorderedSetImpl : UnorderedSetMapImpl<Owner, ValueCollectionType, BucketCollectionType, Hash, KeyType, KeyEqual>
    {
        using Super = UnorderedSetMapImpl<Owner, ValueCollectionType, BucketCollectionType, Hash, KeyType, KeyEqual>;

        template <typename Other>
        ValueOrError<void> assign(const Other& other)
        {
            Super::clear();

            for (auto& value : other)
            {
                if (insert(value).HasError())
                    return ValueOrError<void>::CreateError();
            }

            return ValueOrError<void>();
        }

        template <typename Other>
        ValueOrError<void> assign(Other&& other)
        {
            Super::clear();

            for (auto& value : other)
            {
                if (insert(std::move(value)).HasError())
                    return ValueOrError<void>::CreateError();
            }

            other.clear();

            return ValueOrError<void>();
        }

        ValueOrError<bool> insert(const KeyType& value)
        {
            if (this->valueEntries.empty())
                return false;

            auto hashValue = Super::hash(value);
            auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
            auto& bucketEntry = this->bucketEntries[bucketEntryIndex];
            if (bucketEntry.valueEntriesHead != nullptr)
            {
                //Collision - Hash mapped to an entry in use

                //Make sure there's no value already
                for (auto valueEntry = bucketEntry.valueEntriesHead; valueEntry != nullptr; valueEntry = valueEntry->next)
                {
                    if (valueEntry->value == value)
                    {
                        //Found match
                        return false;
                    }
                }
            }

            //Find an unused value entry
            if (this->unusedValueEntries == nullptr)
            {
                //No free value in valueEntries
                return false;
            }

            //Take first unused value entry
            auto freeValueEntry = this->unusedValueEntries;
            freeValueEntry->hashValue = hashValue;
            if (assignValue(freeValueEntry->value, value).HasError())
                return ValueOrError<bool>::CreateError();

            //Move unused to next
            this->unusedValueEntries = this->unusedValueEntries->next;

            //Add to head of table entry values list
            freeValueEntry->next = bucketEntry.valueEntriesHead;
            bucketEntry.valueEntriesHead = freeValueEntry;

            //Increment counts
            this->count++;
            if (bucketEntry.valueEntriesHead->next != nullptr)
                this->collisionCount++;

            return true;
        }

        ValueOrError<bool> insert(KeyType&& value)
        {
            if (this->valueEntries.empty())
                return false;

            auto hashValue = Super::hash(value);
            auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
            auto& bucketEntry = this->bucketEntries[bucketEntryIndex];
            if (bucketEntry.valueEntriesHead != nullptr)
            {
                //Collision - Hash mapped to an entry in use

                //Make sure there's no value already
                for (auto valueEntry = bucketEntry.valueEntriesHead; valueEntry != nullptr; valueEntry = valueEntry->next)
                {
                    if (valueEntry->value == value)
                    {
                        //Found match
                        return false;
                    }
                }
            }

            //Find an unused value entry
            if (this->unusedValueEntries == nullptr)
            {
                //No free value in valueEntries
                return false;
            }

            //Take first unused value entry
            auto freeValueEntry = this->unusedValueEntries;
            freeValueEntry->hashValue = hashValue;
            if (assignValue(freeValueEntry->value, std::move(value)).HasError())
                return ValueOrError<bool>::CreateError();

            //Move unused to next
            this->unusedValueEntries = this->unusedValueEntries->next;

            //Add to head of table entry values list
            freeValueEntry->next = bucketEntry.valueEntriesHead;
            bucketEntry.valueEntriesHead = freeValueEntry;

            //Increment counts
            this->count++;
            if (bucketEntry.valueEntriesHead->next != nullptr)
                this->collisionCount++;

            return true;
        }

        AssignOrError<KeyType> assignValue;
    };

    template <typename Owner, typename ValueCollectionType, typename BucketCollectionType, typename Hash, typename KeyType, typename ValueType, typename MapPairType = MapPairConstructFirst<KeyType, ValueType>, typename KeyEqual = std::equal_to<KeyType> >
    struct UnorderedMapImpl : UnorderedSetMapImpl<Owner, ValueCollectionType, BucketCollectionType, Hash, KeyType, MapPairEqualTo<typename Owner::value_type, KeyEqual> >
    {
        using Super = UnorderedSetMapImpl<Owner, ValueCollectionType, BucketCollectionType, Hash, KeyType, MapPairEqualTo<typename Owner::value_type, KeyEqual> >;

        template <typename Other>
        ValueOrError<void> assign(const Other& other)
        {
            Super::clear();

            for (auto& value : other)
            {
                if (insert(value.first, value.second).HasError())
                    return ValueOrError<void>::CreateError();
            }

            return ValueOrError<void>();
        }

        template <typename Other>
        ValueOrError<void> operator = (Other&& other)
        {
            Super::clear();

            for (auto& value : other)
            {
                if (insert(std::move(value.first), std::move(value.second)).HasError())
                    return ValueOrError<void>::CreateError();
            }

            other.clear();

            return ValueOrError<void>();
        }

        ValueOrError<bool> insert(const KeyType& key, const ValueType& value, bool allowOverwrite)
        {
            if (this->valueEntries.empty())
                return false;

            auto hashValue = Super::hash(key);
            auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
            auto& bucketEntry = this->bucketEntries[bucketEntryIndex];
            if (bucketEntry.valueEntriesHead != nullptr)
            {
                //Collision - Hash mapped to an entry in use

                //Make sure there's no key already
                for (auto valueEntry = bucketEntry.valueEntriesHead; valueEntry != nullptr; valueEntry = valueEntry->next)
                {
                    if (valueEntry->value.first == key)
                    {
                        //Found match
                        if (allowOverwrite)
                        {
                            if (assignValue(valueEntry->value.second, value).HasError())
                                return ValueOrError<bool>::CreateError();
                            return true;
                        }
                        else
                            return false;
                    }
                }
            }

            //Find an unused value entry
            if (this->unusedValueEntries == nullptr)
            {
                //No free value in valueEntries
                return false;
            }

            //Take first unused value entry
            auto freeValueEntry = this->unusedValueEntries;
            freeValueEntry->hashValue = hashValue;
            if (assignKey(freeValueEntry->value.first, key).HasError())
                return ValueOrError<bool>::CreateError();
            if (assignValue(freeValueEntry->value.second, value).HasError())
                return ValueOrError<bool>::CreateError();

            //Move unused to next
            this->unusedValueEntries = this->unusedValueEntries->next;

            //Add to head of table entry values list
            freeValueEntry->next = bucketEntry.valueEntriesHead;
            bucketEntry.valueEntriesHead = freeValueEntry;

            //Increment counts
            this->count++;
            if (bucketEntry.valueEntriesHead->next != nullptr)
                this->collisionCount++;

            return true;
        }

        ValueOrError<bool> insert(KeyType&& key, ValueType&& value, bool allowOverwrite)
        {
            if (this->valueEntries.empty())
                return false;

            auto hashValue = Super::hash(key);
            auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
            auto& bucketEntry = this->bucketEntries[bucketEntryIndex];
            if (bucketEntry.valueEntriesHead != nullptr)
            {
                //Collision - Hash mapped to an entry in use

                //Make sure there's no key already
                for (auto valueEntry = bucketEntry.valueEntriesHead; valueEntry != nullptr; valueEntry = valueEntry->next)
                {
                    if (valueEntry->value.first == key)
                    {
                        //Found match
                        if (allowOverwrite)
                        {
                            if (assignValue(valueEntry->value.second, std::move(value)).HasError())
                                return ValueOrError<bool>::CreateError();
                            return true;
                        }
                        else
                            return false;
                    }
                }
            }

            //Find an unused value entry
            if (this->unusedValueEntries == nullptr)
            {
                //No free value in valueEntries
                return false;
            }

            //Take first unused value entry
            auto freeValueEntry = this->unusedValueEntries;
            freeValueEntry->hashValue = hashValue;
            if (assignKey(freeValueEntry->value.first, std::move(key)).HasError())
                return ValueOrError<bool>::CreateError();
            if (assignValue(freeValueEntry->value.second, std::move(value)).HasError())
                return ValueOrError<bool>::CreateError();

            //Move unused to next
            this->unusedValueEntries = this->unusedValueEntries->next;

            //Add to head of table entry values list
            freeValueEntry->next = bucketEntry.valueEntriesHead;
            bucketEntry.valueEntriesHead = freeValueEntry;

            //Increment counts
            this->count++;
            if (bucketEntry.valueEntriesHead->next != nullptr)
                this->collisionCount++;

            return true;
        }

        ValueOrError<ValueType*> GetOrAdd(const KeyType& key)
        {
            if (this->valueEntries.empty())
                return nullptr;

            auto hashValue = Super::hash(key);
            auto bucketEntryIndex = static_cast<size_t>(hashValue % this->bucketEntries.size());
            auto& bucketEntry = this->bucketEntries[bucketEntryIndex];

            if (bucketEntry.valueEntriesHead != nullptr)
            {
                decltype(bucketEntry.valueEntriesHead) previousValueEntry = nullptr;
                auto currentValueEntry = bucketEntry.valueEntriesHead;
                for (; currentValueEntry != nullptr; previousValueEntry = currentValueEntry, currentValueEntry = currentValueEntry->next)
                {
                    if (currentValueEntry->value.first == key)
                    {
                        //Found an entry with the key
                        return &currentValueEntry->value.second;
                    }
                }
            }

            //Find an unused value entry
            if (this->unusedValueEntries == nullptr)
            {
                //No free value in valueEntries
                return nullptr;
            }

            //Take first unused value entry
            auto freeValueEntry = this->unusedValueEntries;

            //Move unused to next
            this->unusedValueEntries = this->unusedValueEntries->next;

            //Add to head of table entry values list
            freeValueEntry->next = bucketEntry.valueEntriesHead;
            freeValueEntry->hashValue = hashValue;
            if (assignKey(freeValueEntry->value.first, key).HasError())
                return ValueOrError<ValueType*>::CreateError();
            bucketEntry.valueEntriesHead = freeValueEntry;

            //Increment counts
            this->count++;
            if (bucketEntry.valueEntriesHead->next != nullptr)
                this->collisionCount++;

            //The returned pointer can be held and used to modify the value at any time
            return &freeValueEntry->value.second;
        }

        template <typename FindKeyType>
        const ValueType& GetOrDefault(const FindKeyType& key, const ValueType& defaultValue) const
        {
            auto foundAt = Super::find(key);
            if (foundAt != Super::end())
                return foundAt->second;
            else
                return defaultValue;
        }

        AssignOrError<KeyType> assignKey;
        AssignOrError<ValueType> assignValue;
    };

} }
