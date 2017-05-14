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
#include "finjin/common/Hash.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <size_t bitCount>
    class BitArray
    {
    public:
        using value_type = uint8_t;
        static const size_t arraySize = (bitCount + sizeof(value_type) * 8 - 1) / (sizeof(value_type) * 8);

        template <size_t _bitCount>
        class Accessor
        {
            BitArray<_bitCount>* bitArray;
            size_t bitIndex;

        public:
            Accessor(BitArray<_bitCount>* arr, size_t index) : bitArray(arr), bitIndex(index)
            {
            }

            operator bool () const
            {
                return bitArray->IsBitSet(bitIndex);
            }

            Accessor& operator = (bool value)
            {
                if (value)
                    bitArray->SetBit(bitIndex);
                else
                    bitArray->ClearBit(bitIndex);

                return *this;
            }

            Accessor& operator ^= (bool value)
            {
                value ^= bitArray->IsBitSet(bitIndex);
                if (value)
                    bitArray->SetBit(bitIndex);
                else
                    bitArray->ClearBit(bitIndex);

                return *this;
            }

            Accessor& operator &= (bool value)
            {
                value &= bitArray->IsBitSet(bitIndex);
                if (value)
                    bitArray->SetBit(bitIndex);
                else
                    bitArray->ClearBit(bitIndex);

                return *this;
            }

            Accessor& operator |= (bool value)
            {
                value |= bitArray->IsBitSet(bitIndex);
                if (value)
                    bitArray->SetBit(bitIndex);
                else
                    bitArray->ClearBit(bitIndex);

                return *this;
            }

            bool operator ^ (bool value) const
            {
                return value ^ bitArray->IsBitSet(bitIndex);
            }

            bool operator & (bool value) const
            {
                return value & bitArray->IsBitSet(bitIndex);
            }

            bool operator | (bool value) const
            {
                return value | bitArray->IsBitSet(bitIndex);
            }
        };

    public:
        BitArray()
        {
            clear();
        }

        void clear()
        {
            FINJIN_ZERO_ITEM(bits);
        }

        size_t size() const
        {
            return bitCount;
        }

        bool operator == (const BitArray& other) const
        {
            return memcmp(bits, other.bits, sizeof(bits)) == 0;
        }

        bool operator != (const BitArray& other) const
        {
            return memcmp(bits, other.bits, sizeof(bits)) != 0;
        }

        BitArray& operator = (const BitArray& b)
        {
            FINJIN_COPY_MEMORY(bits, b.bits, sizeof(bits));
            return *this;
        }

        BitArray operator ~ () const
        {
            BitArray result;

            for (size_t i = 0; i < arraySize; i++)
                result.bits[i] = ~bits[i];

            return result;
        }

        BitArray& operator ^= (const BitArray& b)
        {
            for (size_t i = 0; i < arraySize; i++)
                bits[i] ^= b.bits[i];

            return *this;
        }

        BitArray& operator &= (const BitArray& b)
        {
            for (size_t i = 0; i < arraySize; i++)
                bits[i] &= b.bits[i];

            return *this;
        }

        BitArray& operator |= (const BitArray& b)
        {
            for (size_t i = 0; i < arraySize; i++)
                bits[i] |= b.bits[i];

            return *this;
        }

        BitArray operator ^ (const BitArray& b) const
        {
            BitArray result;

            for (size_t i = 0; i < arraySize; i++)
                result.bits[i] = bits[i] ^ b.bits[i];

            return result;
        }

        BitArray operator & (const BitArray& b) const
        {
            BitArray result;

            for (size_t i = 0; i < arraySize; i++)
                result.bits[i] = bits[i] & b.bits[i];

            return result;
        }

        BitArray operator | (const BitArray& b) const
        {
            BitArray result;

            for (size_t i = 0; i < arraySize; i++)
                result.bits[i] = bits[i] | b.bits[i];

            return result;
        }

        bool operator [] (size_t i) const
        {
            return IsBitSet(i);
        }

        Accessor<bitCount> operator [] (size_t index)
        {
            return Accessor<bitCount>(this, index);
        }

        bool IsAnyBitSet() const
        {
            for (size_t i = 0; i < arraySize; i++)
            {
                if (bits[i] != 0)
                    return true;
            }
            return false;
        }

        bool IsBitSet(size_t index) const
        {
            return bits[index / (sizeof(value_type) * 8)] >> (index & (sizeof(value_type) * 8 - 1)) & 1;
        }

        void SetBit(size_t index)
        {
            bits[index / (sizeof(value_type) * 8)] |= (value_type)1 << (index & (sizeof(value_type) * 8 - 1));
        }

        void ClearBit(size_t index)
        {
            bits[index / (sizeof(value_type) * 8)] &= ~((value_type)1 << (index & (sizeof(value_type) * 8 - 1)));
        }

        void FlipBit(size_t index)
        {
            bits[index / (sizeof(value_type) * 8)] ^= (value_type)1 << (index & (sizeof(value_type) * 8 - 1));
        };

        bool TestAndSetBit(size_t index)
        {
            auto result = IsBitSet(index);
            SetBit(index);
            return result;
        }

        bool TestAndClearBit(size_t index)
        {
            auto result = IsBitSet(index);
            ClearBit(index);
            return result;
        }

        size_t GetHash() const
        {
            return Hash::Bytes(bits, sizeof(bits));
        }

        ValueOrError<void> ToString(Utf8String& result) const
        {
            if (result.resize(bitCount).HasError())
                return ValueOrError<void>::CreateError();

            for (size_t i = 0; i < bitCount; i++)
                result[i] = IsBitSet(i) ? '1' : '0';

            return ValueOrError<void>();
        }

        std::array<uint8_t, bitCount> ToBytes() const
        {
            std::array<uint8_t, bitCount> result;
            ToBytes(result);
            return result;
        }

        void ToBytes(std::array<uint8_t, bitCount>& result) const
        {
            ToBytes(result.data(), result.size());
        }

        size_t ToBytes(uint8_t* result, size_t maxBytes) const
        {
            maxBytes = std::min(maxBytes, bitCount);

            for (size_t i = 0; i < maxBytes; i++)
                result[i] = IsBitSet(i) ? 1 : 0;

            return maxBytes;
        }

    private:
        value_type bits[arraySize];
    };

} }

namespace std
{
    template <size_t bitCount>
    struct hash<Finjin::Common::BitArray<bitCount> >
    {
        size_t operator () (const Finjin::Common::BitArray<bitCount>& b) const
        {
            return b.GetHash();
        }
    };
}
