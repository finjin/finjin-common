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


//Functions--------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename Fn, typename... Args>
    typename std::enable_if<std::is_member_pointer<typename std::decay<Fn>::type>::value, typename std::result_of<Fn&&(Args&&...)>::type>::type
    invoke_(Fn&& fn, Args&&... args) 
    {
        return std::mem_fn(fn)(std::forward<Args>(args)...);
    }

    template <typename Fn, typename... Args>
    typename std::enable_if<!std::is_member_pointer<typename std::decay<Fn>::type>::value, typename std::result_of<Fn&&(Args&&...)>::type>::type
    invoke_(Fn&& fn, Args&&... args) 
    {
        return fn(std::forward<Args>(args)...);
    }

    template <typename Fn, typename Tpl, size_t... I>
    decltype(auto) invoke_helper(Fn&& fn, Tpl&& tpl, std::index_sequence<I...>) 
    {
        return invoke_
            ( 
            std::forward<Fn>(fn),
            std::forward<decltype(std::get<I>(std::declval<Tpl>()))>(std::get<I>(std::forward<Tpl>(tpl)))... 
            );
    }

    template <typename Fn, typename Tpl>
    decltype(auto) invoke_helper(Fn&& fn, Tpl&& tpl) 
    {
        constexpr auto size = std::tuple_size<typename std::decay<Tpl>::type>::value;
        return invoke_helper(std::forward<Fn>(fn), std::forward<Tpl>(tpl), std::make_index_sequence<size>{});
    }

} }
