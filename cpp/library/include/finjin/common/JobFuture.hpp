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
#include "finjin/common/JobSharedState.hpp"
#include <exception>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename R>
    class packaged_task;

    template <typename R>
    class promise;

    template <typename R>
    class shared_future;

    //Future
    template <typename R>
    class future
    {
    public:
        typedef typename detail::shared_state<R>::ptr_t ptr_t;

        future() noexcept : state()
        {
        }

        ~future() noexcept
        {
        }

        explicit future(const ptr_t& p) noexcept : state(p)
        {
        }

        future(const future&) = delete;
        future& operator = (const future&) = delete;

        future(future<R>&& other) noexcept : state(std::move(other.state))
        {
        }

        future& operator = (future<R>&& other) noexcept
        {
            if (this != &other)
                this->state = std::move(other.state);

            return *this;
        }

        bool valid() const noexcept
        {
            return this->state.get() != nullptr;
        }

        shared_future<R> share();

        R get()
        {
            if (!valid())
                throw future_uninitialized();

            ptr_t tmp;
            tmp.swap(this->state);
            return tmp->get();
        }

        std::exception_ptr get_exception_ptr()
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get_exception_ptr();
        }

        void wait() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->wait();
        }

    private:
        friend class shared_future<R>;

        ptr_t state;
    };

    //Future
    template <typename R>
    class future<R&>
    {
    public:
        typedef typename detail::shared_state<R&>::ptr_t ptr_t;

        future() noexcept : state()
        {
        }

        ~future() noexcept
        {
        }

        explicit future(const ptr_t& p) noexcept : state(p)
        {
        }

        future(const future&) = delete;
        future& operator = (const future&) = delete;

        future(future<R&>&& other) noexcept : state(std::move(other.state))
        {
        }

        future& operator = (future<R&>&& other) noexcept
        {
            if (this != &other)
                this->state = std::move(other.state);

            return *this;
        }

        bool valid() const noexcept
        {
            return this->state.get() != nullptr;
        }

        shared_future<R&> share();

        R& get()
        {
            if (!valid())
                throw future_uninitialized();

            ptr_t tmp;
            tmp.swap(this->state);
            return tmp->get();
        }

        std::exception_ptr get_exception_ptr()
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get_exception_ptr();
        }

        void wait() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->wait();
        }

    private:
        friend class shared_future<R&>;

        ptr_t state;
    };

    //Future
    template<>
    class future<void>
    {
    public:
        typedef detail::shared_state<void>::ptr_t ptr_t;

        future(const future&) = delete;
        future& operator = (const future&) = delete;

        future() noexcept : state()
        {
        }

        ~future() noexcept
        {
        }

        explicit future(const ptr_t& p) noexcept : state(p)
        {
        }

        inline future(future<void>&& other) noexcept : state(std::move(other.state))
        {
        }

        inline future& operator = (future<void>&& other) noexcept
        {
            if (this != &other)
                this->state = std::move(other.state);

            return *this;
        }

        inline bool valid() const noexcept
        {
            return this->state.get() != nullptr;
        }

        shared_future<void> share();

        inline void get()
        {
            if (!valid())
                throw future_uninitialized();

            ptr_t tmp;
            tmp.swap(this->state);
            tmp->get();
        }

        inline std::exception_ptr get_exception_ptr()
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get_exception_ptr();
        }

        inline void wait() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->wait();
        }

    private:
        friend class shared_future<void>;

        ptr_t state;
    };

    //SharedFuture
    template <typename R>
    class shared_future
    {
    public:
        typedef typename detail::shared_state<R>::ptr_t ptr_t;

    private:
        explicit shared_future(const ptr_t& p) noexcept : state(p)
        {
        }

    public:
        shared_future() noexcept : state()
        {
        }

        ~shared_future() noexcept
        {
        }

        shared_future(const shared_future& other) : state(other.state)
        {
        }

        shared_future(shared_future&& other) noexcept : state(std::move(other.state))
        {
        }

        shared_future(future<R>&& other) noexcept : state(std::move(other.state))
        {
        }

        shared_future& operator = (const shared_future& other) noexcept
        {
            if (this != &other)
                this->state = other.state;

            return *this;
        }

        shared_future& operator = (shared_future&& other) noexcept
        {
            if (this != &other)
                this->state= std::move(other.state);

            return *this;
        }

        shared_future& operator = (future<R>&& other) noexcept
        {
            this->state = std::move(other.state);
            return *this;
        }

        bool valid() const noexcept
        {
            return this->state.get() != nullptr;
        }

        const R& get() const
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get();
        }

        std::exception_ptr get_exception_ptr()
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get_exception_ptr();
        }

        void wait() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->wait();
        }

    private:
        friend class future<R>;

        ptr_t state;
    };

    //SharedFuture
    template <typename R>
    class shared_future<R&>
    {
    public:
        typedef typename detail::shared_state<R&>::ptr_t ptr_t;

    private:
        explicit shared_future(const ptr_t& p) noexcept : state(p)
        {
        }

    public:
        shared_future() noexcept : state()
        {
        }

        ~shared_future() noexcept
        {
        }

        shared_future(const shared_future& other) : state(other.state)
        {
        }

        shared_future(shared_future&& other) noexcept : state(std::move(other.state))
        {
        }

        shared_future(future<R&>&& other) noexcept : state(std::move(other.state))
        {
        }

        shared_future& operator = (const shared_future& other) noexcept
        {
            if (this != &other)
                this->state = other.state;

            return *this;
        }

        shared_future& operator = (shared_future&& other) noexcept
        {
            if (this != &other)
                this->state = std::move(other.state);

            return *this;
        }

        shared_future& operator = (future<R&>&& other) noexcept
        {
            this->state = std::move(other.state);
            return *this;
        }

        bool valid() const noexcept
        {
            return this->state.get() != nullptr;
        }

        R& get() const
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get();
        }

        std::exception_ptr get_exception_ptr()
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get_exception_ptr();
        }

        void wait() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->wait();
        }

    private:
        friend class future<R&>;

        ptr_t state;
    };

    //SharedFuture
    template<>
    class shared_future<void>
    {
    public:
        typedef detail::shared_state<void>::ptr_t ptr_t;

    private:
        shared_future(const ptr_t& p) noexcept : state(p)
        {
        }

    public:
        shared_future() noexcept : state()
        {
        }

        ~shared_future() noexcept
        {
        }

        inline shared_future(const shared_future& other) : state(other.state)
        {
        }

        inline shared_future(shared_future&& other) noexcept : state(std::move(other.state))
        {
        }

        inline shared_future(future<void>&& other) noexcept : state(std::move(other.state))
        {
        }

        inline shared_future& operator = (const shared_future& other) noexcept
        {
            if (this != &other)
                this->state = other.state;

            return *this;
        }

        inline shared_future& operator = (shared_future&& other) noexcept
        {
            if (this != &other)
                this->state = std::move(other.state);

            return *this;
        }

        inline shared_future& operator = (future<void>&& other) noexcept
        {
            this->state = std::move(other.state);
            return *this;
        }

        inline bool valid() const noexcept
        {
            return this->state.get() != nullptr;
        }

        inline void get() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->get();
        }

        inline std::exception_ptr get_exception_ptr()
        {
            if (!valid())
                throw future_uninitialized();

            return this->state->get_exception_ptr();
        }

        inline void wait() const
        {
            if (!valid())
                throw future_uninitialized();

            this->state->wait();
        }

    private:
        friend class future<void>;

        ptr_t state;
    };

    //SharedFuture
    template <typename R>
    shared_future<R> future<R>::share()
    {
        if (!valid())
            throw future_uninitialized();

        return shared_future<R>(std::move(*this));
    }

    template <typename R>
    shared_future<R&> future<R&>::share()
    {
        if (!valid())
            throw future_uninitialized();

        return shared_future<R&>(std::move(*this));
    }

    inline shared_future<void> future<void>::share()
    {
        if (!valid())
            throw future_uninitialized();

        return shared_future<void>(std::move(*this));
    }

} }
