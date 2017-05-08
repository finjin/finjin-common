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
#include <xxHash/xxhash.h>

using namespace Finjin::Common;


//Macros------------------------------------------------------------------------
#define XXHSUM32_DEFAULT_SEED 0
#define XXHSUM64_DEFAULT_SEED 0


//Local functions---------------------------------------------------------------
#if FINJIN_TARGET_CPU_BITS != 32 && FINJIN_TARGET_CPU_BITS != 64
    //Will not occur
    static size_t BasicHash(const void* begin, size_t byteCount)
    {
        //djb2: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx (Bernstein hash)

        auto begin8 = static_cast<const uint8_t*>(begin);
        auto end8 = static_cast<const uint8_t*>(begin + byteCount);

        size_t hash = 0;
        for (auto iter = begin8; iter != end8; iter++)
            hash = 33 * hash + *iter;

        return hash;
    }
#endif


//Implementation----------------------------------------------------------------
size_t Hash::Bytes(const void* begin, const void* end, size_t seed)
{
    assert(begin <= end);
    if (begin >= end)
        return 0;

    auto byteCount = static_cast<const uint8_t*>(end) - static_cast<const uint8_t*>(begin);

#if FINJIN_TARGET_CPU_BITS == 32
    if (seed == 0)
        seed = XXHSUM32_DEFAULT_SEED;
    return XXH32(begin, byteCount, seed);
#elif FINJIN_TARGET_CPU_BITS == 64
    if (seed == 0)
        seed = XXHSUM64_DEFAULT_SEED;
    return XXH64(begin, byteCount, seed);
#else
    return BasicHash(begin, byteCount); //Will not occur
#endif
}

size_t Hash::Bytes(const void* bytes, size_t byteCount, size_t seed)
{
    if (byteCount == 0)
        return 0;

#if FINJIN_TARGET_CPU_BITS == 32
    if (seed == 0)
        seed = XXHSUM32_DEFAULT_SEED;
    return XXH32(bytes, byteCount, seed);
#elif FINJIN_TARGET_CPU_BITS == 64
    if (seed == 0)
        seed = XXHSUM64_DEFAULT_SEED;
    return XXH64(bytes, byteCount, seed);
#else
    return BasicHash(bytes, byteCount); //Will not occur
#endif
}
