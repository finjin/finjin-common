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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Not an actual allocator. Instead, wraps an allocator and registers it as the default allocator.
    template <typename T>
    class DefaultAllocator
    {
    public:
        template <typename... Args>
        DefaultAllocator(Args&&... args) : allocator(std::forward<Args>(args)...)
        {
            this->allocator.SetName("default");
            Allocator::SetDefaultAllocator(&this->allocator);
        }

        ~DefaultAllocator()
        {
            Allocator::SetDefaultAllocator(nullptr);
        }

        operator Allocator& ()
        {
            return this->allocator;
        }

        operator Allocator* ()
        {
            return &this->allocator;
        }

    private:
        T allocator;
    };

} }
