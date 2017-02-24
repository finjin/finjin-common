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
#include "JobSharedState.hpp"
#include <boost/intrusive_ptr.hpp>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common { namespace detail {

    template <typename R, typename ... Args>
    struct task_base : public shared_state<R> 
    {
        typedef boost::intrusive_ptr<task_base> ptr_t;

        virtual ~task_base() noexcept 
        {
        }

        virtual void run(Args&& ... args) = 0;
    };

} } }
