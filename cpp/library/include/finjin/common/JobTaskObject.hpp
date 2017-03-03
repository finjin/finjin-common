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


//Includes---------------------------------------------------------------------
#include "JobInvokeHelper.hpp"
#include "JobTaskBase.hpp"
#include <exception>
#include <utility>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common { namespace detail {

    template <typename Fn, typename Allocator, typename R, typename... Args>
    class task_object : public task_base<R, Args...> 
    {
    public:
        typedef typename Allocator::template rebind<task_object<Fn, Allocator, R, Args...>>::other allocator_t;

        explicit task_object(const allocator_t& alloc, Fn&& fn) : task_base<R, Args...>(), fn_(std::forward<Fn>(fn)), alloc_(alloc)
        {
        }

        void run(Args&&... args)
        {
            try 
            {
                this->set_value(invoke_helper(std::move(fn_), std::make_tuple(std::forward<Args>(args)...)));
            } 
            catch (...)
            {
                this->set_exception(std::current_exception());
            }
        }

    protected:
        void deallocate_future()
        {
            destroy_(alloc_, this);
        }

    private:
        static void destroy_(allocator_t& alloc, task_object* p)
        {
            alloc.destroy( p);
            alloc.deallocate( p, 1);
        }

        Fn fn_;
        allocator_t alloc_;
    };

    template <typename Fn, typename Allocator, typename... Args>
    class task_object<Fn, Allocator, void, Args...> : public task_base<void, Args...> 
    {
    public:
        typedef typename Allocator::template rebind<task_object<Fn, Allocator, void, Args...> >::other allocator_t;

        explicit task_object(const allocator_t& alloc, Fn&& fn): task_base<void, Args...>(), fn_(std::forward<Fn>(fn)), alloc_(alloc)
        {
        }

        void run(Args&&... args)
        {
            try 
            {
                invoke_helper(std::move(fn_), std::make_tuple(std::forward<Args>(args)...));
                this->set_value();
            } 
            catch (...)
            {
                this->set_exception(std::current_exception());
            }
        }

    protected:
        void deallocate_future()
        {
            destroy_(alloc_, this);
        }

    private:
        static void destroy_(allocator_t& alloc, task_object* p)
        {
            alloc.destroy( p);
            alloc.deallocate( p, 1);
        }

        Fn fn_;
        allocator_t alloc_;
    };

} } }
