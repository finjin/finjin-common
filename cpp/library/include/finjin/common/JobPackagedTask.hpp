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
#include <utility>
#include "finjin/common/FiberException.hpp"
#include "finjin/common/JobTaskBase.hpp"
#include "finjin/common/JobTaskObject.hpp"
#include "finjin/common/JobFuture.hpp"
#include "finjin/common/Allocator.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename Signature>
    class packaged_task;

    template <typename R, typename... Args>
    class packaged_task<R(Args...)>
    {
    public:
        typedef typename detail::task_base<R, Args...>::ptr_t ptr_t;

        packaged_task(const packaged_task&) = delete;
        packaged_task& operator = (const packaged_task&) = delete;

    public:
        packaged_task() noexcept : obtained(false), task()
        {
        }

        ~packaged_task()
        {
            if (this->task)
                this->task->owner_destroyed();
        }

        template <typename Fn>
        explicit packaged_task(Allocator* allocator, Fn&& fn) : obtained(false), task()
        {
            typedef detail::task_object<Fn, AllocatorWrapper<packaged_task<R()> >, R, Args...> object_t;

            AllocatorWrapper<packaged_task<R()> > alloc(allocator);
            typename object_t::allocator_t a(alloc);
            this->task = ptr_t(::new(a.allocate(1)) object_t(a, std::forward<Fn>(fn)));
        }

        packaged_task(packaged_task&& other) noexcept : obtained(other.obtained), task(std::move(other.task))
        {
            other.obtained = false;
        }

        packaged_task& operator = (packaged_task&& other) noexcept
        {
            if (this != &other)
            {
                this->obtained = other.obtained;
                other.obtained = false;
                this->task = std::move(other.task);
            }
            return *this;
        }

        void swap(packaged_task& other) noexcept
        {
            std::swap(this->obtained, other.obtained);
            this->task.swap(other.task);
        }

        bool valid() const noexcept
        {
            return nullptr != this->task.get();
        }

        future<R> get_future()
        {
            if (this->obtained)
                throw future_already_retrieved();

            if (!valid())
                throw packaged_task_uninitialized();

            this->obtained = true;

            return future<R>(boost::static_pointer_cast<detail::shared_state<R>>(this->task));
        }

        void operator()(Args&&... args)
        {
            if (!valid())
                throw packaged_task_uninitialized();

            this->task->run(std::forward<Args>(args)...);
        }

        void reset()
        {
            if (!valid())
                throw packaged_task_uninitialized();

            this->obtained = false;

            this->task->reset();
        }

    private:
        bool obtained;
        ptr_t task;
    };

    template <typename Signature>
    void swap(packaged_task<Signature>& l, packaged_task<Signature>& r)
    {
        l.swap(r);
    }

} }
