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


//Types/functions--------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class ByteOrder
    {
        LITTLE,
        BIG
    };
    
    inline ByteOrder GetByteOrder()
    {
        uint16_t lowByteSet = 0x00ff;
        uint8_t* someByte = (uint8_t*)&lowByteSet;
        return *someByte != 0 ? ByteOrder::LITTLE : ByteOrder::BIG;
    }

    inline bool IsLittleEndian()
    {
        return GetByteOrder() == ByteOrder::LITTLE;
    }

    inline bool IsBigEndian()
    {
        return GetByteOrder() == ByteOrder::BIG;
    }

    template <typename T>
    void SwapBytes(T& value)
    {
        auto copy = value;

        auto inByte = reinterpret_cast<int8_t*>(&copy) + sizeof(T) - 1;
        auto outByte = reinterpret_cast<int8_t*>(&value);

        for (size_t i = 0; i < sizeof(T); i++)
            *outByte++ = *inByte--;
    }

} }
