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
#include "finjin/common/MemorySize.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T, size_t MaxItems, size_t SizeAlignment = std::alignment_of<max_align_t>::value>
    class AlignedElementStaticVector
    {
    public:
        enum { MAX_ITEMS = MaxItems };

        using value_type = T;

        struct iterator
        {
            T* item;
            size_t size = AlignSizeUp(sizeof(T), SizeAlignment);

            iterator()
            {
                this->item = nullptr;
            }

            iterator(T* item)
            {
                this->item = item;
            }

            bool operator == (const iterator& other) const { return this->item == other.item; }
            bool operator != (const iterator& other) const { return this->item != other.item; }
            bool operator < (iterator other) const { return this->item < other.item; }
            bool operator <= (iterator other) const { return this->item <= other.item; }
            bool operator > (iterator other) const { return this->item > other.item; }
            bool operator >= (iterator other) const { return this->item >= other.item; }

            ptrdiff_t operator - (iterator other) const
            {
                return (reinterpret_cast<const uint8_t*>(this->item) - reinterpret_cast<const uint8_t*>(other.item)) / this->size;
            }

            iterator& operator ++ ()
            {
                //Prefix operator
                auto bytes = reinterpret_cast<uint8_t*>(this->item) + this->size;
                this->item = reinterpret_cast<T*>(bytes);
                return *this;
            }

            iterator operator ++ (int)
            {
                //Postfix operator
                auto result = *this;
                this->item = reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(this->item) + this->size);
                return result;
            }

            T& operator * ()
            {
                return *this->item;
            }

            const T& operator * () const
            {
                return *this->item;
            }

            T* operator -> ()
            {
                return this->item;
            }

            const T* operator -> () const
            {
                return this->item;
            }
        };

        using const_iterator = iterator;

        AlignedElementStaticVector()
        {
            this->count = 0;

            for (size_t i = 0; i < MAX_ITEMS; i++)
                new (&this->bytes[i * alignedItemSize]) T;
        }

        ~AlignedElementStaticVector()
        {
            for (size_t i = 0; i < MAX_ITEMS; i++)
                _Get(i).~T();

            clear();
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

        template <typename OtherType, size_t otherSize, size_t otherAlignment>
        void assign(const AlignedElementStaticVector<OtherType, otherSize, otherAlignment>& other)
        {
            this->count = std::min(max_size(), other.size());

            for (size_t i = 0; i < this->count; i++)
                _Get(i) = other._Get(i);
        }

        template <typename Iter>
        void assign(Iter begin, size_t otherCount)
        {
            this->count = std::min(max_size(), otherCount);
            for (int i = 0; i < this->count; i++)
                _Get(i) = *begin++;
        }

        template <typename FindValueType>
        iterator find(const FindValueType& value)
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (_Get(i) == value)
                    return _GetPointer(i);
            }
            return end();
        }

        template <typename FindValueType>
        const const_iterator find(const FindValueType& value) const
        {
            for (size_t i = 0; i < this->count; i++)
            {
                if (_Get(i) == value)
                    return _GetPointer(i);
            }
            return end();
        }

        void clear()
        {
            this->count = 0;
        }

        const_iterator front() const { assert(this->count > 0); return _Get(0); }
        iterator front() { assert(this->count > 0); return _Get(0); }

        const_iterator back() const { assert(this->count > 0); return _Get(this->count - 1); }
        iterator back() { assert(this->count > 0); return _Get(this->count - 1); }

        const_iterator middle() const { assert(this->count > 0); return _Get(this->count / 2 + this->count % 2); }
        iterator middle() { assert(this->count > 0); return _Get(this->count / 2 + this->count % 2); }

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

        bool push_back(const T& item)
        {
            if (this->count < MaxItems)
            {
                _Get(this->count++) = item;
                return true;
            }
            else
                return false;
        }

        bool push_back(T&& item)
        {
            if (this->count < MaxItems)
            {
                _Get(this->count++) = std::move(item);
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

        void erase(const iterator iter)
        {
            assert(iter >= begin());
            assert(iter < end());

            size_t itemIndex = iter - begin();

            for (size_t i = itemIndex; i < this->count - 1; i++)
                _Get(i) = std::move(_Get(i + 1));
            this->count--;
        }

        const_iterator begin() const { return _GetPointer(0); }
        iterator begin() { return _GetPointer(0); }

        const_iterator end() const { return _GetPointer(this->count); }
        iterator end() { return _GetPointer(this->count); }

        const T& operator [] (size_t i) const { assert(this->count > 0); return _Get(i); }
        T& operator [] (size_t i) { assert(this->count > 0); return _Get(i); }

    private:
        const T& _Get(size_t i) const { return *_GetPointer(i); }
        T& _Get(size_t i) { return *_GetPointer(i); }

        const T* _GetPointer(size_t i) const { return reinterpret_cast<const T*>(&this->bytes[i * alignedItemSize]); }
        T* _GetPointer(size_t i) { return reinterpret_cast<T*>(&this->bytes[i * alignedItemSize]); }

    protected:
        const size_t alignedItemSize = AlignSizeUp(sizeof(T), SizeAlignment);
        uint8_t bytes[AlignSizeUp(sizeof(T), SizeAlignment) * MaxItems];
        size_t count;
    };

} }
