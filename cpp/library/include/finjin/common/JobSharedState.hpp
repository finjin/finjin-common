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

//          Copyright Oliver Kowalke 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


//Includes----------------------------------------------------------------------
#include "finjin/common/FiberException.hpp"
#include "finjin/common/FiberMutex.hpp"
#include "finjin/common/FiberCondition.hpp"

#include <cstddef>
#include <exception>
#include <mutex>

#include <boost/intrusive_ptr.hpp>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common { namespace detail {

    template <typename R>
    class shared_state
    {
    private:
        void mark_ready_and_notify_(std::unique_lock<FiberMutex>& lk)
        {
            this->ready = true;

            lk.unlock();

            this->waiters.NotifyAll();
        }

        void owner_destroyed_(std::unique_lock<FiberMutex>& lk)
        {
            if (!this->ready)
            {
                set_exception_(std::make_exception_ptr(broken_promise()), lk);
            }
        }

        void set_value_(const R& value, std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            this->value = value;

            mark_ready_and_notify_(lk);
        }

        void set_value_(R&& value, std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            this->value = std::move(value);

            mark_ready_and_notify_(lk);
        }

        void set_exception_(std::exception_ptr except, std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            this->exceptionPtr = except;

            mark_ready_and_notify_(lk);
        }

        const R& get_(std::unique_lock<FiberMutex>& lk)
        {
            wait_(lk);

            if (this->exceptionPtr)
                std::rethrow_exception(this->exceptionPtr);

            return this->value;
        }

        std::exception_ptr get_exception_ptr_(std::unique_lock<FiberMutex>& lk)
        {
            wait_(lk);
            return exceptionPtr;
        }

        void wait_(std::unique_lock<FiberMutex>& lk) const
        {
            while (!this->ready)
                this->waiters.wait(lk);
        }

    protected:
        virtual void deallocate_future() = 0;

    public:
        shared_state(const shared_state&) = delete;
        shared_state& operator = (const shared_state&) = delete;

    public:
        typedef boost::intrusive_ptr<shared_state> ptr_t;

        shared_state() : useCount(0), mutex(), ready(false), value(), exceptionPtr()
        {
        }

        virtual ~shared_state() noexcept
        {
        }

        void owner_destroyed()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            owner_destroyed_(lk);
        }

        void set_value(const R& value)
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_value_(value, lk);
        }

        void set_value(R&& value)
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_value_(std::move(value), lk);
        }

        void set_exception(std::exception_ptr except)
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_exception_(except, lk);
        }

        const R& get()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            return get_(lk);
        }

        std::exception_ptr get_exception_ptr()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            return get_exception_ptr_(lk);
        }

        void wait() const
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            wait_(lk);
        }

        void reset()
        {
            this->ready = false;
        }

        friend inline void intrusive_ptr_add_ref(shared_state* p) noexcept
        {
            ++p->useCount;
        }

        friend inline void intrusive_ptr_release(shared_state* p)
        {
            if (--p->useCount == 0)
               p->deallocate_future();
        }

    private:
        std::atomic<size_t> useCount;
        mutable FiberMutex mutex;
        mutable FiberCondition waiters;
        bool ready;
        R value;
        std::exception_ptr exceptionPtr;
    };

    template <typename R>
    class shared_state<R&>
    {
    private:
        void mark_ready_and_notify_(std::unique_lock<FiberMutex>& lk)
        {
            this->ready = true;

            lk.unlock();

            this->waiters.NotifyAll();
        }

        void owner_destroyed_(std::unique_lock<FiberMutex>& lk)
        {
            if (!this->ready)
                set_exception_(std::make_exception_ptr(broken_promise()), lk);
        }

        void set_value_(R& value, std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            this->value = &value;

            mark_ready_and_notify_(lk);
        }

        void set_exception_(std::exception_ptr except, std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            this->exceptionPtr = except;

            mark_ready_and_notify_(lk);
        }

        R& get_(std::unique_lock<FiberMutex>& lk)
        {
            wait_(lk);
            if (this->exceptionPtr)
                std::rethrow_exception(this->exceptionPtr);

            return *value;
        }

        std::exception_ptr get_exception_ptr_(std::unique_lock<FiberMutex>& lk)
        {
            wait_(lk);
            return exceptionPtr;
        }

        void wait_(std::unique_lock<FiberMutex>& lk) const
        {
            while (!this->ready)
                this->waiters.wait(lk);
        }

    protected:
        virtual void deallocate_future() = 0;

    public:
        shared_state(const shared_state&) = delete;
        shared_state& operator = (const shared_state&) = delete;

    public:
        typedef boost::intrusive_ptr<shared_state> ptr_t;

        shared_state() : useCount(0), mutex(), ready(false), value(0), exceptionPtr()
        {
        }

        virtual ~shared_state() noexcept
        {
        }

        void owner_destroyed()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            owner_destroyed_(lk);
        }

        void set_value(R& value)
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_value_(value, lk);
        }

        void set_exception(std::exception_ptr except)
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_exception_(except, lk);
        }

        R& get()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            return get_(lk);
        }

        std::exception_ptr get_exception_ptr()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            return get_exception_ptr_(lk);
        }

        void wait() const
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            wait_(lk);
        }

        void reset()
        {
            this->ready = false;
        }

        friend inline void intrusive_ptr_add_ref(shared_state* p) noexcept
        {
            ++p->useCount;
        }

        friend inline void intrusive_ptr_release(shared_state* p)
        {
            if (--p->useCount == 0)
                p->deallocate_future();
        }

    private:
        std::atomic<size_t> useCount;
        mutable FiberMutex mutex;
        mutable FiberCondition waiters;
        bool ready;
        R* value;
        std::exception_ptr exceptionPtr;
    };

    template <>
    class shared_state<void>
    {
    private:
        inline void mark_ready_and_notify_(std::unique_lock<FiberMutex>& lk)
        {
            this->ready = true;

            lk.unlock();

            this->waiters.NotifyAll();
        }

        inline void owner_destroyed_(std::unique_lock<FiberMutex>& lk)
        {
            if (!this->ready)
                set_exception_(std::make_exception_ptr(broken_promise()), lk);
        }

        inline void set_value_(std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            mark_ready_and_notify_(lk);
        }

        inline void set_exception_(std::exception_ptr except, std::unique_lock<FiberMutex>& lk)
        {
            if (this->ready)
                throw promise_already_satisfied();

            this->exceptionPtr = except;
            mark_ready_and_notify_(lk);
        }

        inline void get_(std::unique_lock<FiberMutex>& lk)
        {
            wait_(lk);

            if (this->exceptionPtr)
                std::rethrow_exception(this->exceptionPtr);
        }

        inline std::exception_ptr get_exception_ptr_(std::unique_lock<FiberMutex>& lk)
        {
            wait_(lk);
            return exceptionPtr;
        }

        inline void wait_(std::unique_lock<FiberMutex>& lk) const
        {
            while (!this->ready)
                this->waiters.wait(lk);
        }

    protected:
        virtual void deallocate_future() = 0;

    public:
        shared_state(const shared_state&) = delete;
        shared_state& operator = (const shared_state&) = delete;

    public:
        typedef boost::intrusive_ptr<shared_state> ptr_t;

        shared_state() : useCount(0), mutex(), ready(false), exceptionPtr()
        {
        }

        virtual ~shared_state() noexcept
        {
        }

        inline void owner_destroyed()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            owner_destroyed_(lk);
        }

        inline void set_value()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_value_(lk);
        }

        inline void set_exception(std::exception_ptr except)
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            set_exception_(except, lk);
        }

        inline void get()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            get_(lk);
        }

        inline std::exception_ptr get_exception_ptr()
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            return get_exception_ptr_(lk);
        }

        inline void wait() const
        {
            std::unique_lock<FiberMutex> lk(this->mutex);
            wait_(lk);
        }

        inline void reset()
        {
            this->ready = false;
        }

        friend inline void intrusive_ptr_add_ref(shared_state* p) noexcept
        {
            ++p->useCount;
        }

        friend inline void intrusive_ptr_release(shared_state* p)
        {
            if (--p->useCount == 0)
               p->deallocate_future();
        }

    private:
        std::atomic<size_t> useCount;
        mutable FiberMutex mutex;
        mutable FiberCondition waiters;
        bool ready;
        std::exception_ptr exceptionPtr;
    };

} } }
