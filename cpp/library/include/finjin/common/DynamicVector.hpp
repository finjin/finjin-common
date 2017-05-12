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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/AssignOrError.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    class DynamicVector
    {
    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;

        DynamicVector()
        {
            this->items = nullptr;
            this->count = 0;
            this->maxCount = 0;
            this->allocator = nullptr;
        }

        DynamicVector(DynamicVector&& other)
        {
            this->items = other.items;
            this->count = other.count;
            this->maxCount = other.maxCount;
            this->allocator = other.allocator;

            other.items = nullptr;
            other.count = 0;
            other.maxCount = 0;
            other.allocator = nullptr;
        }

        DynamicVector& operator = (DynamicVector&& other)
        {
            Destroy();

            this->items = other.items;
            this->count = other.count;
            this->maxCount = other.maxCount;
            this->allocator = other.allocator;

            other.items = nullptr;
            other.count = 0;
            other.maxCount = 0;
            other.allocator = nullptr;

            return *this;
        }

        DynamicVector(const DynamicVector& other) = delete;

        ~DynamicVector()
        {
            Destroy();
        }

        template <typename... Args>
        bool Create(size_t count, Allocator* allocator, Args&&... args)
        {
            Destroy();

            if (allocator == nullptr)
                allocator = Allocator::GetDefaultAllocator();

            void* mem = nullptr;
            if (count > 0)
            {
                if (allocator != nullptr)
                    mem = allocator->Allocate(sizeof(T) * count, FINJIN_CALLER_ARGUMENTS);
                else
                    mem = Allocator::SystemAllocate(sizeof(T) * count, FINJIN_CALLER_ARGUMENTS);
            }
            this->items = static_cast<T*>(mem);
            this->count = count;
            this->maxCount = count;
            this->allocator = allocator;

            //Construct the items
            if (mem != nullptr)
            {
                auto bytes = static_cast<uint8_t*>(mem);
                for (size_t i = 0; i < count; i++)
                {
                    new (bytes) T(std::forward<Args>(args)...);
                    bytes += sizeof(T);
                }

                return true;
            }
            else
            {
                //0 = success, anything else = failure
                return count == 0;
            }
        }

        template <typename... Args>
        bool CreateEmpty(size_t count, Allocator* allocator, Args&&... args)
        {
            auto result = Create(count, allocator, std::forward<Args>(args)...);
            this->count = 0;
            return result;
        }

        void Destroy()
        {
            for (size_t i = 0; i < this->maxCount; i++)
                this->items[i].~T();
            if (this->allocator != nullptr)
                this->allocator->Deallocate(this->items);
            else
                Allocator::SystemDeallocate(this->items);

            this->items = nullptr;
            this->count = 0;
            this->maxCount = 0;
            this->allocator = nullptr;
        }

        size_t size() const { return this->count; }
        size_t max_size() const { return this->maxCount; }
        size_t size_left() const { return max_size() - size(); }

        size_t resize(size_t newSize) { this->count = std::min(newSize, this->maxCount); return this->count; }
        size_t maximize() { this->count = this->maxCount; return this->count; }

        bool empty() const { return this->count == 0; }
        bool full() const { return this->count == this->maxCount; }

        template <typename FindValueType>
        bool contains(const FindValueType& value) const
        {
            return find(value) != end();
        }

        template <typename OtherType>
        ValueOrError<void> assign(const DynamicVector<OtherType>& other)
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
            if (this->count < this->maxCount)
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
            if (this->count < this->maxCount)
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
            if (this->count < this->maxCount)
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
            if (this->count < this->maxCount)
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
            if (this->count < this->maxCount)
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
            if (this->count < this->maxCount)
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

        const T* max_end() const { return &this->items[this->maxCount]; }
        T* max_end() { return &this->items[this->maxCount]; }

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
        T* items;
        size_t count;
        size_t maxCount;
        Allocator* allocator;
        AssignOrError<T> assignValue;
    };

} }
