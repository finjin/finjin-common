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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    struct RequestedValue
    {
    public:
        RequestedValue()
        {
        }

        explicit RequestedValue(const T& value)
        {
            this->requested = this->actual = value;
        }

        RequestedValue& operator = (const T& value)
        {
            this->requested = this->actual = value;
            return *this;
        }

        T requested;
        T actual;
    };

} }
