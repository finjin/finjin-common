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
#include <wx/string.h>
#if FINJIN_TARGET_OS_IS_WINDOWS
    #include <guiddef.h>
#endif


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /** 128-bit universally unique identifier. */
    class WxUuid
    {
        friend class WxRandomUuidCreator;

    public:
        static WxUuid CreateRandom();

        static const WxUuid NIL;

        /** Default constructor. By default the UUID is "nil". */
        WxUuid();

        WxUuid(const wxString& stringValue);
        WxUuid(const char* stringValue);
        WxUuid(const uint8_t* bytes);
        WxUuid(uint32_t data1, uint16_t data2, uint16_t data3, uint8_t data4_0, uint8_t data4_1, uint8_t data4_2, uint8_t data4_3, uint8_t data4_4, uint8_t data4_5, uint8_t data4_6, uint8_t data4_7);

    #if FINJIN_TARGET_OS_IS_WINDOWS
        WxUuid(const GUID& guid);

        operator GUID () const;
    #endif

        /** Indicates whether the UUID is "nil". */
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
        wxString ToString() const;

        bool operator == (const WxUuid& other) const;
        bool operator != (const WxUuid& other) const;
        bool operator < (const WxUuid& other) const;

        /**
         * Parses a UUID from a string.
         * @param value [out] The parsed UUID.
         * @param stringValue [in] String to parse.
         * @param error [out] Error object.
         * @see Error
         */
        static bool Parse(WxUuid& value, const wxString& stringValue);

        /**
         * Parses a UUID from a string.
         * @param stringValue [in] String to parse.
         * @return The parsed UUID. If the UUID could not be parsed, a "nil" UUID is returned.
         */
        static WxUuid Parse(const wxString& stringValue);
    
    private:
        struct CpuOrderedParts 
        {
            uint32_t data1;
            uint16_t data2;
            uint16_t data3;
            uint8_t data4[8];
        };
        union
        {
            CpuOrderedParts cpuOrderedParts;
            uint8_t data[16];
            uint32_t data32[4];
        #if FINJIN_TARGET_OS_IS_WINDOWS
            GUID guid;
        #endif
        };        
    };

    class WxRandomUuidCreator
    {
    public:
        WxRandomUuidCreator();
        ~WxRandomUuidCreator();

        WxUuid NewUuid();

    private:        
        std::mt19937 randomGenerator;
    };

} }

namespace std
{
    //Allows Finjin::Exporter::WxUuid to be used as the key in a std::unordered_map
    template<>
    struct hash<Finjin::Common::WxUuid>
    {
        size_t operator () (const Finjin::Common::WxUuid& id) const
        {
            return id.GetHash();
        }
    };
}
