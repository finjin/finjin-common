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
#include "finjin/common/FiberSpinLock.hpp"
#include <exception>
#include <stdexcept>
#include <string>
#include <system_error>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class JobFiber;

    class fiber_exception : public std::system_error
    {
    public:
        fiber_exception() : std::system_error(0, std::system_category())
        {
        }

        fiber_exception(int sys_error_code) : std::system_error(sys_error_code, std::system_category())
        {
        }

        fiber_exception(int ev, const char* what_arg) : std::system_error(std::error_code(ev, std::system_category()), what_arg)
        {
        }

        fiber_exception(int ev, const std::string& what_arg) : std::system_error(std::error_code(ev, std::system_category()), what_arg)
        {
        }

        virtual ~fiber_exception() throw()
        {
        }
    };

    class condition_error : public fiber_exception
    {
    public:
        condition_error() : fiber_exception(0, "Condition error")
        {
        }

        condition_error(int ev) : fiber_exception(ev, "Condition error")
        {
        }

        condition_error(int ev, const char* what_arg) : fiber_exception(ev, what_arg)
        {
        }

        condition_error(int ev, const std::string& what_arg) : fiber_exception(ev, what_arg)
        {
        }
    };

    class lock_error : public fiber_exception
    {
    public:
        lock_error() : fiber_exception(0, "lock_error")
        {
        }

        lock_error(int ev) : fiber_exception(ev, "lock_error")
        {
        }

        lock_error(int ev, const char* what_arg) : fiber_exception(ev, what_arg)
        {
        }

        lock_error(int ev, const std::string& what_arg) : fiber_exception(ev, what_arg)
        {
        }
    };

    class fiber_resource_error : public fiber_exception
    {
    public:
        fiber_resource_error() : fiber_exception(static_cast<int>(std::errc::resource_unavailable_try_again), "fiber_resource_error")
        {
        }

        fiber_resource_error(int ev) : fiber_exception(ev, "fiber_resource_error")
        {
        }

        fiber_resource_error(int ev, const char* what_arg) : fiber_exception(ev, what_arg)
        {
        }

        fiber_resource_error(int ev, const std::string& what_arg) : fiber_exception(ev, what_arg)
        {
        }
    };

    class invalid_argument : public fiber_exception
    {
    public:
        invalid_argument() : fiber_exception(static_cast<int>(std::errc::invalid_argument), "invalid_argument")
        {
        }

        invalid_argument(int ev) : fiber_exception(ev, "invalid_argument")
        {
        }

        invalid_argument(int ev, const char* what_arg) : fiber_exception(ev, what_arg)
        {
        }

        invalid_argument(int ev, const std::string& what_arg) : fiber_exception(ev, what_arg)
        {
        }
    };

    class logic_error : public fiber_exception
    {
    public:
        logic_error() : fiber_exception(0, "logic_error")
        {
        }

        logic_error(const char* what_arg) : fiber_exception(0, what_arg)
        {
        }

        logic_error(int ev) : fiber_exception(ev, "logic_error")
        {
        }

        logic_error(int ev, const char* what_arg) : fiber_exception(ev, what_arg)
        {
        }

        logic_error(int ev, const std::string& what_arg) : fiber_exception(ev, what_arg)
        {
        }
    };

    class FiberInterruptedException : public fiber_exception
    {
    public:
        FiberInterruptedException(JobFiber* fiber) : fiber_exception(static_cast<int>(std::errc::interrupted), "fiber_interrupted")
        {
            this->fiber = fiber;
        }

        JobFiber* GetFiber()
        {
            return this->fiber;
        }

    private:
        JobFiber* fiber;
    };

    enum class FutureErrorCode
    {
        UNKNOWN = 0,
        BROKEN_PROMISE,
        FUTURE_ALREADY_RETRIEVED,
        PROMISE_ALREADY_SATISFIED,
        NO_STATE
    };

    const std::error_category& FutureCategory() noexcept;

} }

namespace std
{
    template <>
    struct is_error_code_enum<Finjin::Common::FutureErrorCode> : public true_type
    {
    };

    inline std::error_code make_error_code(Finjin::Common::FutureErrorCode e) noexcept
    {
        return std::error_code(static_cast<int>(e), Finjin::Common::FutureCategory());
    }

    inline std::error_condition make_error_condition(Finjin::Common::FutureErrorCode e) noexcept
    {
        return std::error_condition(static_cast<int>(e), Finjin::Common::FutureCategory());
    }
}

namespace Finjin { namespace Common {

    class future_error : public std::logic_error
    {
    public:
        future_error(std::error_code ec) : logic_error(ec.message()), ec_(ec)
        {
        }

        const std::error_code& code() const noexcept
        {
            return ec_;
        }

        const char* what() const throw()
        {
            return code().message().c_str();
        }

    private:
        std::error_code ec_;
    };

    class future_uninitialized : public future_error
    {
    public:
        future_uninitialized() : future_error(std::make_error_code(FutureErrorCode::NO_STATE))
        {
        }
    };

    class future_already_retrieved : public future_error
    {
    public:
        future_already_retrieved() : future_error(std::make_error_code(FutureErrorCode::FUTURE_ALREADY_RETRIEVED))
        {
        }
    };

    class broken_promise : public future_error
    {
    public:
        broken_promise() : future_error(std::make_error_code(FutureErrorCode::BROKEN_PROMISE))
        {
        }
    };

    class promise_already_satisfied : public future_error
    {
    public:
        promise_already_satisfied() : future_error(std::make_error_code(FutureErrorCode::PROMISE_ALREADY_SATISFIED))
        {
        }
    };

    class promise_uninitialized : public future_error
    {
    public:
        promise_uninitialized() : future_error(std::make_error_code(FutureErrorCode::NO_STATE))
        {
        }
    };

    class packaged_task_uninitialized : public future_error
    {
    public:
        packaged_task_uninitialized() : future_error(std::make_error_code(FutureErrorCode::NO_STATE))
        {
        }
    };

} }
