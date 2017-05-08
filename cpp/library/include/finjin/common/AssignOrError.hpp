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

    template <typename T>
    struct AssignOrError
    {
        ValueOrError<void> operator () (T& a, const T& b)
        {
            a = b;
            return ValueOrError<void>();
        }

        ValueOrError<void> operator () (T& a, T&& b)
        {
            a = std::move(b);
            return ValueOrError<void>();
        }
    };

} }
