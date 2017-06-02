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
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/Utf8String.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API Luid
    {
    public:
        static const Luid& GetZero();

        Luid(); //Sets to zero
        Luid(const Utf8String& stringValue);
        Luid(const char* stringValue);
        Luid(const uint8_t* bytes);
        Luid(uint32_t data1, int32_t data2);

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        Luid(const LUID& luid);

        operator LUID () const;
    #endif

        bool empty() const { return IsZero(); }

        bool IsZero() const;

        size_t GetHash() const;

        std::array<uint8_t, 8> GetBytes() const;

        Utf8String ToString() const;

        bool operator == (const Luid& other) const;
        bool operator != (const Luid& other) const;
        bool operator < (const Luid& other) const;

        static void Parse(Luid& value, const Utf8String& stringValue, Error& error);

        static Luid Parse(const Utf8String& stringValue);

    private:
        struct CpuOrderedParts
        {
            //This is how the Windows LUID is defined
            UInt32BigEndian data1; //Yes, uint32
            Int32BigEndian data2; //Yes, int32
        };
        union
        {
            CpuOrderedParts cpuOrderedParts;
            uint8_t data[8];
            uint32_t data32[2];
            uint64_t data64;
        #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
            LUID luid;
        #endif
        };
    };

} }

namespace std
{
    //Allows Finjin::Common::Luid to be used as the key in a std::unordered_map
    template <>
    struct hash<Finjin::Common::Luid>
    {
        size_t operator () (const Finjin::Common::Luid& id) const
        {
            return id.GetHash();
        }
    };
}


//Functions---------------------------------------------------------------------
namespace std
{
    inline ostream& operator << (ostream& out, const Finjin::Common::Luid& value)
    {
        out << value.ToString();
        return out;
    }
}
