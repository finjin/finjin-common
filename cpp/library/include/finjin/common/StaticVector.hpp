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

    class Allocator;

    template <typename T, size_t MaxItems>
    class StaticVector
    {
    public:
        enum { MAX_ITEMS = MaxItems };

        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;

        StaticVector() { this->count = 0; }
        ~StaticVector() { clear(); }

    #if defined(FINJIN_EXPORTER)
        bool Create(size_t count)
        {
            if (count > MaxItems)
                return false;

            this->count = count;
            return true;
        }

        bool CreateEmpty()
        {
            this->count = 0;
            return true;
        }
    #else
        template <typename... Args>
        bool Create(size_t count, Allocator* allocator, Args&&... args)
        {
            if (count > MaxItems)
                return false;

            this->count = count;
            return true;
        }

        template <typename... Args>
        bool CreateEmpty(Allocator* allocator, Args&&... args)
        {
            this->count = 0;
            return true;
        }
    #endif

        void Destroy()
        {
        }

        size_t size() const { return this->count; }
        size_t max_size() const { return MAX_ITEMS; }
        size_t size_left() const { return max_size() - size(); }

        size_t resize(size_t newSize) { this->count = std::min(newSize, (size_t)MAX_ITEMS); return this->count; }
        size_t maximize() { this->count = (size_t)MAX_ITEMS; return this->count; }

        bool empty() const { return this->count == 0; }
        bool full() const { return this->count == MAX_ITEMS; }

        template <typename FindValueType>
        bool contains(const FindValueType& value) const
        {
            return find(value) != end();
        }

        template <typename OtherType, size_t otherSize>
        ValueOrError<void> assign(const StaticVector<OtherType, otherSize>& other)
        {
            return assign(other.data(), other.size());
        }

        template <typename Iter>
        ValueOrError<void> assign(Iter otherItems, size_t otherCount)
        {
            this->count = std::min(max_size(), otherCount);

            for (size_t i = 0; i < this->count; i++)
            {
                if (assignValue(this->items[i], *otherItems++).HasError())
                    return ValueOrError<void>::CreateError();
            }

            return ValueOrError<void>();
        }

        template <typename FindValueType>
        T* find(const FindValueType& value)
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (this->items[i] == value)
                    return &this->items[i];
            }
            return end();
        }

        template <typename FindValueType>
        const T* find(const FindValueType& value) const
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (this->items[i] == value)
                    return &this->items[i];
            }
            return end();
        }

        void clear()
        {
            this->count = 0;
        }

        const T& front() const { assert(this->count > 0); return this->items[0]; }
        T& front() { assert(this->count > 0); return this->items[0]; }

        const T& back() const { assert(this->count > 0); return this->items[this->count - 1]; }
        T& back() { assert(this->count > 0); return this->items[this->count - 1]; }

        const T& middle() const { assert(this->count > 0); return this->items[this->count / 2 + this->count % 2]; }
        T& middle() { assert(this->count > 0); return this->items[this->count / 2 + this->count % 2]; }

        ValueOrError<bool> push_front()
        {
            if (this->count < MaxItems)
            {
                this->count++;
                for (size_t i = this->count - 1; i > 0; i--)
                {
                    if (assignValue(this->items[i], std::move(this->items[i - 1])).HasError())
                        return ValueOrError<bool>::CreateError();
                }
                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_front(const T& item)
        {
            if (this->count < MaxItems)
            {
                this->count++;
                for (size_t i = this->count - 1; i > 0; i--)
                {
                    if (assignValue(this->items[i], std::move(this->items[i - 1])).HasError())
                        return ValueOrError<bool>::CreateError();
                }
                if (assignValue(this->items[0], item).HasError())
                    return ValueOrError<bool>::CreateError();
                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_front(T&& item)
        {
            if (this->count < MaxItems)
            {
                this->count++;
                for (size_t i = this->count - 1; i > 0; i--)
                {
                    if (assignValue(this->items[i], std::move(this->items[i - 1])).HasError())
                        return ValueOrError<bool>::CreateError();
                }
                if (assignValue(this->items[0], std::move(item)).HasError())
                    return ValueOrError<bool>::CreateError();
                return true;
            }
            else
                return false;
        }

        bool push_back()
        {
            if (this->count < MaxItems)
            {
                this->count++;
                return true;
            }
            else
                return false;
        }

        bool push_back_count(size_t count)
        {
            if (count <= size_left())
            {
                this->count += count;
                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_back(const T& item)
        {
            if (this->count < MaxItems)
            {
                if (assignValue(this->items[this->count++], item).HasError())
                    return ValueOrError<bool>::CreateError();
                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_back(T&& item)
        {
            if (this->count < MaxItems)
            {
                if (assignValue(this->items[this->count++], std::move(item)).HasError())
                    return ValueOrError<bool>::CreateError();
                return true;
            }
            else
                return false;
        }

        void pop_back()
        {
            if (this->count > 0)
                this->count--;
        }

        ValueOrError<void> erase(const T* iter)
        {
            assert(iter >= begin());
            assert(iter < end());

            size_t itemIndex = iter - begin();

            for (size_t i = itemIndex; i < this->count - 1; i++)
            {
                if (assignValue(this->items[i], std::move(this->items[i + 1])).HasError())
                    return ValueOrError<void>::CreateError();
            }
            this->count--;

            return ValueOrError<void>();
        }

        const T* data() const { return this->count > 0 ? &this->items[0] : nullptr; }
        T* data() { return this->count > 0 ? &this->items[0] : nullptr; }

        const T* data_left() const { return this->count <= this->maxCount ? &this->items[this->count] : nullptr; } //Use <= to allow getting end
        T* data_left() { return this->count <= this->maxCount ? &this->items[this->count] : nullptr; } //Use <= to allow getting end

        const T* begin() const { return &this->items[0]; }
        T* begin() { return &this->items[0]; }

        const T* end() const { return &this->items[this->count]; }
        T* end() { return &this->items[this->count]; }

        const T* max_end() const { return &this->items[MaxItems]; }
        T* max_end() { return &this->items[MaxItems]; }

        const T& operator [] (size_t i) const { assert(this->count > 0); return this->items[i]; }
        T& operator [] (size_t i) { assert(this->count > 0); return this->items[i]; }

        ValueOrError<void> Get(size_t i, T& value) { return assignValue(value, this->items[i]); }
        ValueOrError<void> Set(size_t i, const T& value) { return assignValue(this->items[i], value); }
        ValueOrError<void> Set(size_t i, T&& value) { return assignValue(this->items[i], std::move(value)); }

        template <typename Iter>
        bool Equals(Iter iter, size_t valueCount) const
        {
            if (this->count != valueCount)
                return false;

            for (size_t i = 0; i < valueCount; i++)
            {
                if (this->items[i] != *iter)
                    return false;
                ++iter;
            }
            return true;
        }

    protected:
        T items[MaxItems];
        size_t count;
        AssignOrError<T> assignValue;
    };

} }
