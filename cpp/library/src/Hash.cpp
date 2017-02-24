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
#include "finjin/common/CpuFeatures.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
size_t Hash::Bytes(const void* vbegin, const void* vend)
{
    if (vbegin == vend)
        return 0;

    size_t initialHash = 0;

    auto begin8 = (const uint8_t*)vbegin;
    auto end8 = (const uint8_t*)vend;
    
#if 0 //FINJIN_SSE //Disabled for the moment. It works but not very well due to the fact that the API uses 32 bits
    if (CpuFeatures::SupportsSSE42())
    {
        auto hash = (uint32_t)initialHash; //Yes, this discards the upper 32-bits on a 64-bit build
        
        auto begin16 = (const uint16_t*)Allocator::AlignMemoryUp(begin8, 2);
        auto begin32 = (const uint32_t*)Allocator::AlignMemoryUp(begin8, 4);
    #if FINJIN_TARGET_CPU_BITS == 64
        auto begin64 = (const uint64_t*)Allocator::AlignMemoryUp(begin8, 8);
    #endif
        
        auto end16 = (const uint16_t*)Allocator::AlignMemoryDown(end8, 2);
        auto end32 = (const uint32_t*)Allocator::AlignMemoryDown(end8, 4);
    #if FINJIN_TARGET_CPU_BITS == 64
        auto end64 = (const uint64_t*)Allocator::AlignMemoryDown(end8, 8);
    #endif
        
        auto iter = begin8;
        
        if (iter != (const uint8_t*)begin16)
        {
            hash = _mm_crc32_u8(hash, *iter);
            iter += 1;
        }
        
        if (iter != (const uint8_t*)begin32)
        {
            hash = _mm_crc32_u16(hash, *(const uint16_t*)begin16);
            iter += 2;
        }
        
    #if FINJIN_TARGET_CPU_BITS == 64
        if (iter != (const uint8_t*)begin64)
        {
            hash = _mm_crc32_u32(hash, *(const uint32_t*)iter);
            iter += 4;
        }
        
        for (; iter < (const uint8_t*)end64; iter += 8)
            hash = (uint32_t)_mm_crc32_u64(hash, *(const uint64_t*)iter); //Inconsistency with definition of _mm_crc32_u64(). Cast return value to u32
        
        if (iter != (const uint8_t*)end32)
        {
            hash = _mm_crc32_u32(hash, *(const uint32_t*)iter);
            iter += 4;
        }
    #else
        for (; iter < (const uint8_t*)end32; iter += 4)
            hash = _mm_crc32_u32(hash, *(const uint32_t*)iter);
    #endif
        
        if (iter != (const uint8_t*)end16)
        {
            hash = _mm_crc32_u16(hash, *(const uint16_t*)iter);
            iter += 2;
        }
        
        if (iter != end8)
        {
            hash = _mm_crc32_u8(hash, *iter);
            iter += 1;
        }
        
        return hash;
    }
#endif
    
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
