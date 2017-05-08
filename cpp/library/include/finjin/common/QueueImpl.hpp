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
#include "finjin/common/ValueOrError.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename Owner, typename ValueCollectionType>
    struct QueueImpl
    {
        using value_type = typename Owner::value_type;

        struct iterator
        {
            iterator()
            {
                this->impl = nullptr;
                this->index = nullptr;
                this->count = 0;
            }

            iterator(QueueImpl* impl, size_t index, size_t count)
            {
                this->impl = impl;
                this->index = index;
                this->count = count;
            }

            bool operator == (const iterator& other) const { return this->impl == other.impl && this->index == other.index; }
            bool operator != (const iterator& other) const { return this->impl != other.impl || this->index != other.index; }

            iterator& operator ++ ()
            {
                //Prefix operator
                if (this->index == this->count)
                    return *this;

                this->index++;
                return *this;
            }

            iterator operator ++ (int)
            {
                auto result = *this;

                //Postfix operator
                if (this->index == this->count)
                    return result;

                this->index++;
                return result;
            }

            value_type& operator * ()
            {
                assert(this->impl != nullptr);
                return this->impl->valueEntries[(this->impl->headIndex + this->index) % this->impl->valueEntries.size()];
            }

            value_type* operator -> ()
            {
                assert(this->valueEntry != nullptr);
                return &this->impl->valueEntries[(this->impl->headIndex + this->index) % this->impl->valueEntries.size()];
            }

            QueueImpl* impl;
            size_t index;
            size_t count;
        };

        struct const_iterator
        {
            const_iterator()
            {
                this->impl = nullptr;
                this->index = nullptr;
                this->count = 0;
            }

            const_iterator(QueueImpl* impl, size_t index, size_t count)
            {
                this->impl = impl;
                this->index = index;
                this->count = count;
            }

            bool operator == (const const_iterator& other) const { return this->impl == other.impl && this->index == other.index; }
            bool operator != (const const_iterator& other) const { return this->impl != other.impl || this->index != other.index; }

            const_iterator& operator ++ ()
            {
                //Prefix operator
                if (this->index == this->count)
                    return *this;

                this->index++;
                return *this;
            }

            const_iterator operator ++ (int)
            {
                auto result = *this;

                //Postfix operator
                if (this->index == this->count)
                    return result;

                this->index++;
                return result;
            }

            const value_type& operator * () const
            {
                assert(this->impl != nullptr);
                return this->impl->valueEntries[(this->impl->headIndex + this->index) % this->impl->valueEntries.size()];
            }

            const value_type* operator -> () const
            {
                assert(this->valueEntry != nullptr);
                return &this->impl->valueEntries[(this->impl->headIndex + this->index) % this->impl->valueEntries.size()];
            }

            QueueImpl* impl;
            size_t index;
            size_t count;
        };

        QueueImpl()
        {
            clear();
        }

        void clear()
        {
            this->headIndex = this->tailIndex = 0;
            this->count = 0;
        }

        iterator begin() { return iterator(this, 0, this->count); }
        iterator end() { return iterator(this, this->count, this->count); }

        const_iterator begin() const { return const_iterator(this, 0, this->count); }
        const_iterator end() const { return const_iterator(this, this->count, this->count); }

        bool push() { return push_back(); }
        ValueOrError<bool> push(const value_type& value) { return push_back(value); }
        ValueOrError<bool> push(value_type&& value) { return push_back(std::move(value)); }

        bool pop() { return pop_front(); }
        ValueOrError<bool> pop(value_type& value)
        {
            if (this->count == 0)
                return false;

            return pop_front(value);
        }

        value_type& current() { return front(); }

        value_type& front() { assert(this->count > 0); return this->valueEntries[this->headIndex]; }
        const value_type& front() const { assert(this->count > 0); return this->valueEntries[this->headIndex]; }

        value_type& back() { assert(this->count > 0); return this->valueEntries[(this->tailIndex + this->valueEntries.size() - 1) % this->valueEntries.size()]; }
        const value_type& back() const { assert(this->count > 0); return this->valueEntries[(this->tailIndex + this->valueEntries.size() - 1) % this->valueEntries.size()]; }

        ValueOrError<bool> push_front()
        {
            if (this->count < this->valueEntries.size())
            {
                this->headIndex = (this->headIndex + this->valueEntries.size() - 1) % this->valueEntries.size();
                this->count++;

                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_front(const value_type& value)
        {
            if (this->count < this->valueEntries.size())
            {
                auto index = (this->headIndex + this->valueEntries.size() - 1) % this->valueEntries.size();

                auto setResult = this->valueEntries.Set(index, value);
                if (setResult.HasError())
                    return ValueOrError<bool>::CreateError();

                this->headIndex = index;
                this->count++;

                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_front(value_type&& value)
        {
            if (this->count < this->valueEntries.size())
            {
                auto index = (this->headIndex + this->valueEntries.size() - 1) % this->valueEntries.size();

                auto setResult = this->valueEntries.Set(index, std::move(value));
                if (setResult.HasError())
                    return ValueOrError<bool>::CreateError();

                this->headIndex = index;
                this->count++;

                return true;
            }
            else
                return false;
        }

        bool push_back()
        {
            if (this->count < this->valueEntries.size())
            {
                this->tailIndex = (this->tailIndex + 1) % this->valueEntries.size();
                this->count++;

                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_back(const value_type& value)
        {
            if (this->count < this->valueEntries.size())
            {
                auto setResult = this->valueEntries.Set(this->tailIndex, value);
                if (setResult.HasError())
                    return ValueOrError<bool>::CreateError();

                this->tailIndex = (this->tailIndex + 1) % this->valueEntries.size();
                this->count++;

                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> push_back(value_type&& value)
        {
            if (this->count < this->valueEntries.size())
            {
                auto setResult = this->valueEntries.Set(this->tailIndex, std::move(value));
                if (setResult.HasError())
                    return ValueOrError<bool>::CreateError();

                this->tailIndex = (this->tailIndex + 1) % this->valueEntries.size();
                this->count++;

                return true;
            }
            else
                return false;
        }

        bool pop_front()
        {
            if (this->count > 0)
            {
                this->headIndex = (this->headIndex + 1) % this->valueEntries.size();
                this->count--;
                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> pop_front(value_type& value)
        {
            if (this->count > 0)
            {
                auto getResult = this->valueEntries.Get(this->headIndex, value);

                this->headIndex = (this->headIndex + 1) % this->valueEntries.size();
                this->count--;

                if (getResult.HasError())
                    return ValueOrError<bool>::CreateError();

                return true;
            }
            else
                return false;
        }

        bool pop_back()
        {
            if (this->count > 0)
            {
                this->tailIndex = (this->tailIndex + this->valueEntries.size() - 1) % this->valueEntries.size();
                this->count--;
                return true;
            }
            else
                return false;
        }

        ValueOrError<bool> pop_back(value_type& value)
        {
            if (this->count > 0)
            {
                auto index = (this->tailIndex + this->valueEntries.size() - 1) % this->valueEntries.size();

                auto getResult = this->valueEntries.Get(index, value);

                this->tailIndex = index;
                this->count--;

                if (getResult.HasError())
                    return ValueOrError<bool>::CreateError();

                return true;
            }
            else
                return false;
        }

        ValueCollectionType valueEntries;
        size_t headIndex, tailIndex;
        size_t count;
    };

} }
