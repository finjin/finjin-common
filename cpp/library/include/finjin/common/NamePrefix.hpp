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
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * A prefix used when naming things, such as command line options or file paths.
     */
    class FINJIN_COMMON_LIBRARY_API NamePrefix 
    {
    public:
        NamePrefix();        
        NamePrefix(const NamePrefix& other);        
        NamePrefix(NamePrefix&& other);
        NamePrefix(const Utf8String& prefix);
        
        NamePrefix& operator = (const NamePrefix& other);        
        NamePrefix& operator = (NamePrefix&& other);

        /**
         * Resets the internal prefixes collection to a single prefix.
         * @param prefix [in] The new prefix.
         * @return A reference to this name prefix.
         */
        NamePrefix& operator = (const Utf8String& prefix);
        
        /**
         * Appends a prefix component.
         */
        NamePrefix operator + (const Utf8String& prefix) const;
        NamePrefix operator + (const NamePrefix& prefix) const;
        
        /**
         * Appends a prefix component.
         * @param prefix [in] The prefix to append.
         * @preturn A reference to this name prefix.
         */
        NamePrefix& operator += (const Utf8String& prefix);        

        /**
         * Appends a prefix component.
         * @param prefix [in] The prefix to append.
         * @preturn A reference to this name prefix.
         */
        NamePrefix& operator += (const NamePrefix& prefix);
        
        /**
         * Formats the prefix as a string.
         * @param joinChar [in] The character to use when joining prefix components.
         * @return The prefix as a string.
         */
        Utf8String ToString(char joinChar) const;
        
        /**
         * Formats the prefix as a command line string. The prefix components are joined with the '.' character.
         * @return The prefix as a command line string. The prefix components are joined with the '.' character.
         */
        Utf8String ToCommandLineString() const;

        /**
         * Formats the prefix as a path string. The prefix components are joined with the '/' character.
         * @return The prefix as a path string. The prefix components are joined with the '/' character.
        */
        Utf8String ToPathString() const;
        
    private:
        StaticVector<Utf8String, 8> parts;
    };

} }
