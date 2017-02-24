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
#include "finjin/common/UsageFormatter.hpp"

using namespace Finjin::Common;


//Local classes-----------------------------------------------------------------
struct UsageFormatter::Impl
{
    Impl()
    {
        this->outputStream = nullptr;
    }

    std::ostream* outputStream;
};


//Implementation----------------------------------------------------------------
UsageFormatter::UsageFormatter(std::ostream& outputStream) : impl(new Impl)
{
    impl->outputStream = &outputStream;
}

UsageFormatter::~UsageFormatter()
{
    delete impl;
}

std::ostream* UsageFormatter::GetStream()
{
    return impl->outputStream;
}
