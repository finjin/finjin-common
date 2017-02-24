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


//Includes---------------------------------------------------------------------
#include "finjin/common/ByteOrder.hpp"


//Macros-----------------------------------------------------------------------
#define BINARY_LINE_TYPE_CHUNK_START 1
#define BINARY_LINE_TYPE_CHUNK_END 2
#define BINARY_LINE_TYPE_DOCUMENT_END 3
#define BINARY_LINE_TYPE_SINGLE_VALUE 4
#define BINARY_LINE_TYPE_MULTI_VALUE 5
#define BINARY_LINE_TYPE_MASK 0x0f
#define BINARY_LINE_TYPE_SHIFT 0

//If there is no flag, then that indicates everything is contained in the encountered occurrence
#define BINARY_LINE_OCCURRENCE_FIRST 1
#define BINARY_LINE_OCCURRENCE_MORE 2
#define BINARY_LINE_OCCURRENCE_LAST 3
#define BINARY_LINE_OCCURRENCE_MASK 0x30
#define BINARY_LINE_OCCURRENCE_SHIFT 4

#define BINARY_LINE_FLAG_INDEXED (1 << 6) //Indicates chunk start or chunk property name has an integer index, not an ID

//#define BINARY_LINE_FLAG_MORE (1 << 7) //In the future, if more than 8 bits of flags are needed, use the 8th bit to indicate the presence of another byte


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    using BinaryMultiValueSize = uint64_t;
    using BinaryLineLength = uint64_t;
    using BinaryTimeValue = uint64_t;

    template <typename T>
    struct BinaryDataChunkBlobWrapper
    {
        template <typename ValuesType>
        BinaryDataChunkBlobWrapper(ValuesType values, size_t _count) : bytes((T)values), count(_count)
        {
        }

        T bytes;
        size_t count;
    };

    template <typename T>
    struct TextDataChunkBlobWrapper
    {
        template <typename ValuesType>
        TextDataChunkBlobWrapper(ValuesType values, size_t _count) : bytes((T)values), count(_count)
        {
        }

        T bytes;
        size_t count;
    };

} }
