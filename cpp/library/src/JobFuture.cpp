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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/FiberException.hpp"


//Implementation---------------------------------------------------------------
namespace Finjin { namespace Common {

    class FutureErrorCategory : public std::error_category 
    {
    public:
        const char* name() const noexcept override
        {
            return "future";
        }

        std::error_condition default_error_condition(int ev) const noexcept override
        {
            switch (ev) 
            {
                case 1: return std::error_condition(static_cast<int>(FutureErrorCode::BROKEN_PROMISE), FutureCategory());
                case 2: return std::error_condition(static_cast<int>(FutureErrorCode::FUTURE_ALREADY_RETRIEVED), FutureCategory());
                case 3: return std::error_condition(static_cast<int>(FutureErrorCode::PROMISE_ALREADY_SATISFIED), FutureCategory());
                case 4: return std::error_condition(static_cast<int>(FutureErrorCode::NO_STATE), FutureCategory());
                default: return std::error_condition(static_cast<int>(FutureErrorCode::UNKNOWN), FutureCategory());
            }
        }

        bool equivalent(const std::error_code& code, int condition) const noexcept override
        {
            return *this == code.category() && static_cast<int>(default_error_condition(code.value()).value()) == condition;
        }

        std::string message(int ev) const override
        {
            switch (static_cast<FutureErrorCode>(ev)) 
            {
                case FutureErrorCode::UNKNOWN: return std::string("Unknown error");
                case FutureErrorCode::BROKEN_PROMISE: return std::string("The associated promise has been destructed prior to the associated state becoming ready.");
                case FutureErrorCode::FUTURE_ALREADY_RETRIEVED: return std::string("The future has already been retrieved from the promise or packaged_task.");
                case FutureErrorCode::PROMISE_ALREADY_SATISFIED: return std::string("The state of the promise has already been set.");
                case FutureErrorCode::NO_STATE: return std::string("Operation not permitted on an object without an associated state.");
            }
            return std::string("unspecified FutureErrorCode value\n");
        }
    };

    const std::error_category& FutureCategory() noexcept
    {
        static FutureErrorCategory cat;
        return cat;
    }

} }
