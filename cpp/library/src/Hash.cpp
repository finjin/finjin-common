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
#include "finjin/common/Hash.hpp"
#include "finjin/common/Allocator.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
size_t Hash::Bytes(const void* vbegin, const void* vend)
{
    if (vbegin == vend)
        return 0;

    size_t initialHash = 0;

    auto begin8 = (const uint8_t*)vbegin;
    auto end8 = (const uint8_t*)vend;
    
    //djb2: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx (Bernstein hash)
    auto hash = initialHash;
    for (auto iter = begin8; iter != end8; iter++)
        hash = 33 * hash + *iter;
    
    return hash;
}

size_t Hash::Bytes(const void* bytes, size_t byteCount)
{
    if (byteCount > 0)
        return Bytes(bytes, (const uint8_t*)bytes + byteCount);
    else
        return 0;
}
