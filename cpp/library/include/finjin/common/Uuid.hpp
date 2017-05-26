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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Utf8String.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <guiddef.h>
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /** 128-bit universally unique identifier. */
    class FINJIN_COMMON_LIBRARY_API Uuid
    {
        friend class RandomUuidCreator;

    public:
        static Uuid CreateRandom();

        static const Uuid& GetZero();

        Uuid(); //Sets to zero
        Uuid(const Utf8String& stringValue);
        Uuid(const char* stringValue);
        Uuid(const uint8_t* bytes);
        Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint8_t data4_0, uint8_t data4_1, uint8_t data4_2, uint8_t data4_3, uint8_t data4_4, uint8_t data4_5, uint8_t data4_6, uint8_t data4_7);

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
        Uuid(const GUID& guid);

        operator GUID () const;
    #endif

        bool empty() const { return IsZero(); }

        bool IsZero() const;

        /**
         * Gets a hash of the UUID.
         * @return A hash of the UUID.
         */
        size_t GetHash() const;

        std::array<uint8_t, 16> GetBytes() const;

        /**
         * Formats a string from the UUID.
         * @return A string from the UUID.
         */
        Utf8String ToString() const;

        bool operator == (const Uuid& other) const;
        bool operator != (const Uuid& other) const;
        bool operator < (const Uuid& other) const;

        /**
         * Parses a UUID from a string.
         * @param value [out] The parsed UUID.
         * @param stringValue [in] String to parse.
         * @param error [out] Error object.
         * @see Error
         */
        static void Parse(Uuid& value, const Utf8String& stringValue, Error& error);

        /**
         * Parses a UUID from a string.
         * @param stringValue [in] String to parse.
         * @return The parsed UUID. If the UUID could not be parsed, a "nil" UUID is returned.
         */
        static Uuid Parse(const Utf8String& stringValue);

    private:
        struct CpuOrderedParts
        {
            UInt32BigEndian data1;
            UInt16BigEndian data2;
            UInt16BigEndian data3;
            uint8_t data4[8];
        };
        union
        {
            CpuOrderedParts cpuOrderedParts;
            uint8_t data[16];
            uint32_t data32[4];
        #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
            GUID guid;
        #endif
        };
    };

    class FINJIN_COMMON_LIBRARY_API RandomUuidCreator
    {
    public:
        RandomUuidCreator();
        ~RandomUuidCreator();

        Uuid NewUuid();

    private:
        std::mt19937 randomGenerator;
    };

} }

namespace std
{
    //Allows Finjin::Common::Uuid to be used as the key in a std::unordered_map
    template <>
    struct hash<Finjin::Common::Uuid>
    {
        size_t operator () (const Finjin::Common::Uuid& id) const
        {
            return id.GetHash();
        }
    };
}


//Functions---------------------------------------------------------------------
namespace std
{
    inline ostream& operator << (ostream& out, const Finjin::Common::Uuid& value)
    {
        out << value.ToString();
        return out;
    }
}
