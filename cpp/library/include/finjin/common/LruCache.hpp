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
#include "finjin/common/AllocatedClass.hpp"
#include <boost/cstdint.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <mutex>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    using LruCacheItemHandle = void*;

    /** Least-recently-used cache implementation. */
    template <typename ValueType, typename TimeType, typename MutexType>
    class LruCache
    {
    protected:
        using SizeType = boost::uintmax_t;
        using TimeStampFunction = std::function<TimeType()>;
        using DeleteValueFunction = std::function<void(LruCacheItemHandle, ValueType&)>;

        /** Orders heap such that the least recently used item is at the top. */
        template <typename TypedItemPtr>
        struct ItemCompare
        {
            bool operator () (const TypedItemPtr a, const TypedItemPtr b) const
            {
                return a->lastUsed > b->lastUsed;
            }
        };

        /** A single item in the heap. */
        struct Item : public AllocatedClass
        {
            using TypedItem = Item;
            using Heap = boost::heap::fibonacci_heap
                <
                TypedItem*,
                boost::heap::compare<ItemCompare<TypedItem*> >
                >;
            using HeapItemHandle = typename Heap::handle_type;

            ValueType value; //The value
            TimeType lastUsed; //The last used time
            HeapItemHandle heapItemHandle; //The handle that references the item in the heap
            SizeType size; //The size of the value

            Item(Allocator* allocator, const ValueType& _value, SizeType _size, const TimeType& time) : AllocatedClass(allocator), value(_value), size(_size)
            {
                Touch(time);
            }

            Item(Allocator* allocator, ValueType&& _value, SizeType _size, const TimeType& time) : AllocatedClass(allocator), value(_value), size(_size)
            {
                Touch(time);
            }

            void Touch(const TimeType& time)
            {
                this->lastUsed = time;
            }
        };

        using TypedItem = Item;
        using Heap = boost::heap::fibonacci_heap
            <
            TypedItem*,
            boost::heap::compare<ItemCompare<TypedItem*> >
            >;

    public:
        struct Settings
        {
            Settings() = default;

            TimeStampFunction timeNow; //Function that gets the current time
            DeleteValueFunction deleteValue; //Optional function that "deletes" the value
            SizeType cacheSize = std::numeric_limits<SizeType>::max(); //Total amount of user-defined space the cache can use
            Allocator* allocator = nullptr; //Allocator used to allocate new items (boost::heap::fibonacci_heap doesn't use it though)
        };

        LruCache()
        {
            this->cacheUsed = 0;
        }

        ~LruCache()
        {
            ClearQuiet();
        }

        void Create(const Settings& settings)
        {
            ClearQuiet();

            this->settings = settings;
        }

        TimeType GetTimestamp()
        {
            if (this->settings.timeNow != nullptr)
                return this->settings.timeNow();
            else
                return TimeType();
        }

        LruCacheItemHandle Add(const ValueType& value, SizeType size = 1)
        {
            if (size > this->settings.cacheSize)
                return nullptr;

            std::lock_guard<MutexType> thisLock(this->mutex);

            if (this->cacheUsed + size > this->settings.cacheSize)
                _Free(this->cacheUsed + size - this->settings.cacheSize);

            auto item = AllocatedClass::NewUnique<TypedItem>(this->settings.allocator, FINJIN_CALLER_ARGUMENTS, value, size, GetTimestamp());
            item->heapItemHandle = this->heap.push(item.get());
            this->cacheUsed += size;
            return item.release();
        }

        LruCacheItemHandle Add(ValueType&& value, SizeType size = 1)
        {
            if (size > this->settings.cacheSize)
                return nullptr;

            std::lock_guard<MutexType> thisLock(this->mutex);

            if (this->cacheUsed + size > this->settings.cacheSize)
                _Free(this->cacheUsed + size - this->settings.cacheSize);

            auto item = AllocatedClass::NewUnique<TypedItem>(this->settings.allocator, FINJIN_CALLER_ARGUMENTS, std::move(value), size, GetTimestamp());
            item->heapItemHandle = this->heap.push(item.get());
            this->cacheUsed += size;
            return item.release();
        }

        void Touch(LruCacheItemHandle itemHandle)
        {
            Touch(itemHandle, GetTimestamp());
        }

        void Touch(LruCacheItemHandle itemHandle, const TimeType& time)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            auto item = static_cast<TypedItem*>(itemHandle);
            item->Touch(time);
            this->heap.update(item->heapItemHandle);
        }

        void ClearQuiet()
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            while (!this->heap.empty())
            {
                //Just delete item, don't call _DeleteItem() or _DeleteValue()
                delete this->heap.top();
                this->heap.pop();
            }

            this->cacheUsed = 0;
        }

        void Clear()
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            while (!this->heap.empty())
            {
                std::unique_ptr<TypedItem> item(this->heap.top());
                this->heap.pop();
                _DeleteItem(std::move(item));
            }
        }

        void Remove(LruCacheItemHandle itemHandle)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            std::unique_ptr<TypedItem> item(static_cast<TypedItem*>(itemHandle));
            this->heap.erase(item->heapItemHandle);
            _DeleteItem(std::move(item));
        }

        void RemoveOlderThan(const TimeType& time)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            while (!this->heap.empty())
            {
                auto leastRecentItem = this->heap.top();

                //If least recently used item is at least as current as the specified time, we're done
                if (leastRecentItem->lastUsed >= time)
                    break;

                //Remove item from heap and delete
                std::unique_ptr<TypedItem> item(leastRecentItem);
                this->heap.pop();
                _DeleteItem(std::move(item));
            }
        }

        void Output(std::ostream& out) const
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            //This will output from least-recently-used to most-recently-used
            for (auto i = this->heap.ordered_begin(); i != this->heap.ordered_end(); ++i)
                out << "Value: " << (*i)->value << ", Last used: " << (*i)->lastUsed << std::endl;
        }

        bool Test(const ValueType* values, size_t valueCount)
        {
            std::lock_guard<MutexType> thisLock(this->mutex);

            if (this->heap.size() != valueCount)
                return false;

            //This will output from least-recently-used to most-recently-used
            for (auto i = this->heap.ordered_begin(); i != this->heap.ordered_end(); ++i)
            {
                if ((*i)->value != *values)
                    return false;

                values++;
            }

            return true;
        }

    private:
        void _Free(SizeType amount)
        {
            while (amount > 0)
            {
                assert(!this->heap.empty());

                //Get and remove least recently used item from priority queue
                std::unique_ptr<TypedItem> item(this->heap.top());
                this->heap.pop();

                //Subtract the item size from the running amount to be freed, avoiding overflow
                if (amount > item->size)
                    amount -= item->size;
                else
                    amount = 0;

                //Delete item
                _DeleteItem(std::move(item));
            }
        }

        void _DeleteItem(std::unique_ptr<TypedItem>&& item)
        {
            this->cacheUsed -= item->size;
            _DeleteValue(item.get(), item->value);
        }

        void _DeleteValue(LruCacheItemHandle itemHandle, ValueType& value)
        {
            if (this->settings.deleteValue != nullptr)
                this->settings.deleteValue(itemHandle, value);
        }

    private:
        Settings settings;

        Heap heap;

        SizeType cacheUsed; //Current amount of user-defined space used by the cache

        mutable MutexType mutex;
    };

} }
