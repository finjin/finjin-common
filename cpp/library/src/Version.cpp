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
#include "finjin/common/Version.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
Version::Version()
{
    for (size_t index = 0; index < MAX_COMPONENTS; index++)
        this->components[index] = 0;
}

Version::Version(int major, int minor, int patch)
{
    this->components[MAJOR_INDEX] = major;
    this->components[MINOR_INDEX] = minor;
    this->components[PATCH_INDEX] = patch;
}

Version::Version(const Utf8String& version)
{
    size_t length = version.length();
    size_t offset = 0;
    size_t foundAt;
    Utf8String component;

    size_t index = 0;
    while (index < MAX_COMPONENTS && offset < length)
    {
        //Extract the current component
        foundAt = version.find('.', offset);
        version.substr(component, offset);
        this->components[index++] = Convert::ToInteger(component, (int)0);

        //Break out if there is no next '.'
        if (foundAt == Utf8String::npos)
            break;

        //Move past the next '.'
        offset = foundAt + 1;
    }

    for (; index < MAX_COMPONENTS; index++)
        this->components[index] = 0;
}

int Version::GetMajor() const
{
    return this->components[MAJOR_INDEX];
}

int Version::GetMinor() const
{
    return this->components[MINOR_INDEX];
}

int Version::GetPatch() const
{
    return this->components[PATCH_INDEX];
}

Utf8String Version::ToString() const
{
    Utf8String result;
    ToString(result);
    return result;
}

ValueOrError<void> Version::ToString(Utf8String& result) const
{
    result.clear();

    //Find the last non-zero component
    size_t lastNonzeroComponent = (size_t)-1;
    for (size_t index = MAX_COMPONENTS - 1; index != (size_t)-1; index--)
    {
        if (this->components[index] != 0)
        {
            lastNonzeroComponent = index;
            break;
        }
    }

    //Output everything up to the last non-zero component
    if (lastNonzeroComponent != (size_t)-1)
    {
        for (size_t index = 0; index <= lastNonzeroComponent; index++)
        {
            if (result.append(Convert::ToString(this->components[index])).HasError())
                return ValueOrError<void>::CreateError();
            if (index < lastNonzeroComponent)
            {
                if (result.append(".").HasError())
                    return ValueOrError<void>::CreateError();
            }
        }
    }
    else
    {
        //All components are zero
        if (result.append("0").HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

int Version::ToInt() const
{
    int version = 0;
    int multiplier = 1;
    for (size_t index = 0; index < MAX_COMPONENTS; index++)
    {
        version += this->components[index] * multiplier;
        multiplier *= 100;
    }
    return version;
}

int Version::Compare(const Version& version1, const Version& version2)
{
    return version1.ToInt() - version2.ToInt();
}
