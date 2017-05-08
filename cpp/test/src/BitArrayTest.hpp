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
#include "finjin/common/BitArray.hpp"


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(BitArrayTest_set_bit)
{
    BOOST_TEST_MESSAGE("BitArrayTest_set_bit:");

    using Bits = BitArray<7>;
    Bits bits;

    bits.SetBit(1);
    BOOST_CHECK(bits.IsBitSet(1));
    BOOST_CHECK(bits.IsAnyBitSet());

    bits.FlipBit(1);
    BOOST_CHECK(!bits.IsBitSet(1));
    BOOST_CHECK(!bits.IsAnyBitSet());

    bits.SetBit(1);
    bits.ClearBit(1);
    BOOST_CHECK(!bits.IsBitSet(1));
    BOOST_CHECK(!bits.IsAnyBitSet());
}

BOOST_AUTO_TEST_CASE(BitArrayTest_string)
{
    BOOST_TEST_MESSAGE("BitArrayTest_string:");

    using Bits = BitArray<7>;
    Bits bits;

    bits.SetBit(1);

    Utf8String s;
    bits.ToString(s);
    BOOST_CHECK(s == "0100000");
}

BOOST_AUTO_TEST_CASE(BitArrayTest_compare)
{
    BOOST_TEST_MESSAGE("BitArrayTest_compare:");

    using Bits = BitArray<7>;

    Bits a, b;
    a.SetBit(1);
    b.SetBit(1);

    BOOST_CHECK(a == b);

    b.SetBit(0);
    BOOST_CHECK(a != b);
}
