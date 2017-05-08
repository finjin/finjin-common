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
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * A major/minor/build/revision number collection.
     * Use the Version class to simplify the storage and comparison of version numbers
     */
    class FINJIN_COMMON_LIBRARY_API Version
    {
    public:
        /** Default class constructor. Constructs a Version object with all zeroes. */
        Version();

        /**
         * Constructs a Version from the major, minor, build, and revision numbers.
         * All components should be less than 0
         * @param major [in] - Major version number.
         * @param minor [in] - Minor version number.
         * @param patch [in] - Patch number.
         */
        Version(int major, int minor = 0, int patch = 0);

        /**
         * Constructs a version by parsing it from a string
         * @param version [in] - The string to parse. The string should be of the form: 'xx.xx.xx'.
         * All components are optional.
         */
        Version(const Utf8String& version);

        /**
         * Gets the major version number.
         */
        int GetMajor() const;

        /**
         * Gets the minor version number.
         */
        int GetMinor() const;

        /**
         * Gets the patch number.
         */
        int GetPatch() const;

        /**
         * Converts the version to a string.
         */
        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

        /**
         * Converts the version to an integer.
         * Each component can be no larger than 99 for this to work properly
         */
        int ToInt() const;

        //Comparison operators
        bool operator == (const Version& v) const {return Compare(*this, v) == 0;}
        bool operator != (const Version& v) const {return Compare(*this, v) != 0;}
        bool operator < (const Version& v) const {return Compare(*this, v) < 0;}
        bool operator <= (const Version& v) const {return Compare(*this, v) <= 0;}
        bool operator > (const Version& v) const {return Compare(*this, v) > 0;}
        bool operator >= (const Version& v) const {return Compare(*this, v) >= 0;}

        bool AtLeast(int major, int minor = 0, int patch = 0) const;

        /**
         * Compares two versions.
         * Function for comparing two Version objects, returning an
         * integer to indicate the result of the comparison.
         * @param version1 [in] - The first Version to compare.
         * @param version2 [in] - The second Version to compare to.
         * @return An integer indicating the result of the comparison.
         * This value is less than 0 if version1 is "older" than version2,
         * greater than 0 if version1 is "newer" than version2, and equal to
         * 0 if the two versions are the same.
         */
        static int Compare(const Version& version1, const Version& version2);

    private:
        enum
        {
            MAJOR_INDEX,
            MINOR_INDEX,
            PATCH_INDEX,
            MAX_COMPONENTS
        };

        /** Major, minor, patch */
        int components[MAX_COMPONENTS];
        int padding; //Not used
    };

} }
