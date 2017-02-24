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


//Includes---------------------------------------------------------------------
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/QueueUtilities.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    class AllocatedQueue
    {
    public:
        using value_type = T;

        using ValueCollection = AllocatedVector<value_type>;

        using This = AllocatedQueue<T>;
        using Impl = QueueImpl<This, ValueCollection>;

        using iterator = typename Impl::iterator;
        using const_iterator = typename Impl::const_iterator;

        template <typename... Args>
        bool Create(size_t valueCount, Allocator* allocator, Args&&... args)
        {
            Destroy();

            auto result = true;
            result &= impl.valueEntries.Create(valueCount, allocator, std::forward<Args>(args)...);

            clear();

            return result;
        }

        void Destroy()
        {
            impl.valueEntries.Destroy();

            clear();
        }

        void clear() { impl.clear(); }

        iterator begin() { return impl.begin(); }
        iterator end() { return impl.end(); }

        const_iterator begin() const { return impl.begin(); }
        const_iterator end() const { return impl.end(); }

        bool push() { return impl.push(); }
        ValueOrError<bool> push(const T& value) { return impl.push(value); }
        ValueOrError<bool> push(T&& value) { return impl.push(std::move(value)); }

        bool pop() { return impl.pop(); }
        ValueOrError<bool> pop(T& value) { return impl.pop(value); }

        T& current() { return impl.current(); }

        size_t size() const { return impl.count; }
        size_t max_size() const { return impl.valueEntries.size(); }
        size_t size_left() const { return max_size() - size(); }

        bool empty() const { return impl.count == 0; }
        bool full() const { return impl.count == impl.valueEntries.size(); }

        T& front() { return impl.front(); }
        const T& front() const { return impl.front(); }

        T& back() { return impl.back(); }
        const T& back() const { return impl.back(); }

        ValueOrError<bool> push_front() { return impl.push_front(); }
        ValueOrError<bool> push_front(const T& value) { return impl.push_front(value); }
        ValueOrError<bool> push_front(T&& value) { return impl.push_front(std::move(value)); }

        ValueOrError<bool> push_back() { return impl.push_back(); }
        ValueOrError<bool> push_back(const T& value) { return impl.push_back(value); }
        ValueOrError<bool> push_back(T&& value) { return impl.push_back(std::move(value)); }

        bool pop_front() { return impl.pop_front(); }
        ValueOrError<bool> pop_front(T& value) { return impl.pop_front(value); }

        bool pop_back() { return impl.pop_back(); }
        ValueOrError<bool> pop_back(T& value) { return impl.pop_back(value); }

        ValueCollection& GetValueEntries() { return impl.valueEntries; }

        size_t GetIndex(const T& value) const { return &value - impl.valueEntries.data(); }

        T& operator [] (size_t i) { return impl.valueEntries[i]; }
        const T& operator [] (size_t i) const { return impl.valueEntries[i]; }

    private:
        Impl impl;
    };

} }
