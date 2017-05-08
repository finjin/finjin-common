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
#include "finjin/common/Utf8StringFormatter.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/Uuid.hpp"
#include <boost/format.hpp>

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
struct Utf8StringFormatter::Impl
{
    Impl(const char* format) : boostFormat(format) {}
    Impl(const Utf8String& format) : boostFormat(format.c_str()) {}

    boost::format boostFormat;
};


//Implementation----------------------------------------------------------------
Utf8StringFormatter::Utf8StringFormatter(const char* format) : impl(new Impl(format))
{
}

Utf8StringFormatter::Utf8StringFormatter(const Utf8String& format) : impl(new Impl(format))
{
}

Utf8StringFormatter::~Utf8StringFormatter()
{
    delete impl;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (int8_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (int16_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (int32_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (int64_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (uint8_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (uint16_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (uint32_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (uint64_t value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS || FINJIN_TARGET_PLATFORM_IS_APPLE

Utf8StringFormatter& Utf8StringFormatter::operator % (long value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (unsigned long value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

#endif

Utf8StringFormatter& Utf8StringFormatter::operator % (float value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (double value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const char* value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const Utf8String& value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const Utf8StringView& value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value.ToString();
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const Path& value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value;
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const TimeDuration& value)
{
    if (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % value.ToString();
    return *this;
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const Uuid& value)
{
    return operator % (value.ToString());
}

Utf8StringFormatter& Utf8StringFormatter::operator % (const void* value)
{
    return operator % ((uintptr_t)value);
}

Utf8String Utf8StringFormatter::ToString() const
{
    while (impl->boostFormat.remaining_args() > 0)
        impl->boostFormat % "<?>";

    try
    {
        Utf8String result = boost::str(impl->boostFormat).c_str();
        impl->boostFormat.clear();
        return result;
    }
    catch (...)
    {
        return "<format error>";
    }
}
