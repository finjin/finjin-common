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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/Settings.hpp"
#include "finjin/common/UserInformation.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static void CleanArg(Utf8String& cleanedArg, const Utf8String& arg)
{
    cleanedArg.clear();

    //Keep leading '-' characters
    auto argString = arg.c_str();
    while (*argString && *argString == '-')
    {
        cleanedArg += '-';
        argString++;
    }

    //Remove all internal '-' and '_' characters
    while (*argString)
    {
        if (*argString != '-' && *argString != '_')
            cleanedArg += *argString;

        argString++;
    }
}


//Implementation----------------------------------------------------------------
Settings::Settings()
{
}

Settings::~Settings()
{
}

const Path& Settings::GetThisFileName() const
{
    return this->thisFileName;
}

void Settings::SetThisFileName(const Path& fileName)
{
    this->thisFileName = fileName;
}

Path Settings::CompletePath(const Path& _path, RelativeTo relativeTo, const StandardPaths& standardPaths)
{
    //TODO: This is only really appropriate for desktop/server environments. Fix that?

    Path homeDirectory;
    Path::GetUserHomeDirectory(homeDirectory);

    auto path = _path;
    path.ReplaceFirst("~", homeDirectory.c_str());

    if (path.IsAbsolute())
        return path;
    else
    {
        Path combinedPath;

        if (relativeTo == RelativeTo::CURRENT_WORKING_DIRECTORY)
        {
            combinedPath = standardPaths.paths[WhichStandardPath::WORKING_DIRECTORY].path;
            combinedPath /= path;
        }
        else if (relativeTo == RelativeTo::APPLICATION)
        {
            standardPaths.paths[WhichStandardPath::APPLICATION_EXECUTABLE_FILE].path.GetParent(combinedPath);
            combinedPath /= path;
        }
        else if (!GetThisFileName().empty())
        {
            Path thisFileNamePath(GetThisFileName());
            if (thisFileNamePath.HasParent())
            {
                thisFileNamePath.GetParent(combinedPath);
                combinedPath /= path;
            }
            else
                combinedPath = path;
        }
        else
            combinedPath = path;

        return combinedPath;
    }
}

bool Settings::IsOption(const Utf8String& arg, const Utf8String& name, const NamePrefix& prefix)
{
    Utf8String cleanedArg;
    CleanArg(cleanedArg, arg); //'some_random-arg' becomes 'somerandomarg'
    Utf8String cleanedName;
    CleanArg(cleanedName, name);

    Utf8String prefixString;
    CleanArg(prefixString, prefix.ToCommandLineString());

    //Looks like '-arg'
    Utf8String shortName(FINJIN_SHORT_COMMAND_LINE_ARG_PREFIX);
    shortName += prefixString;
    shortName += cleanedName;

    //Looks like '--arg'
    Utf8String longName(FINJIN_LONG_COMMAND_LINE_ARG_PREFIX);
    longName += prefixString;
    longName += cleanedName;

    return
        cleanedArg.EqualsNoCaseAscii(shortName.c_str()) ||
        cleanedArg.EqualsNoCaseAscii(longName.c_str());
}

bool Settings::IsOption(const Utf8String& arg, const Utf8String& name, const Utf8String& otherName, const NamePrefix& prefix)
{
    if (IsOption(arg, name, prefix))
        return true;
    return IsOption(arg, otherName, prefix);
}

bool Settings::IsElement(const Utf8String& arg, const Utf8String& name)
{
    return arg.EqualsNoCaseAscii(name);
}

bool Settings::IsElement(const Utf8String& arg, const Utf8String& name, const Utf8String& otherName)
{
    return arg.EqualsNoCaseAscii(name) || arg.EqualsNoCaseAscii(otherName);
}

bool Settings::ParseOptionalString(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element)
{
    Utf8String stringValue;
    GetElementText(stringValue, element);
    if (!stringValue.empty())
        setting = stringValue;
    return !stringValue.empty();
}

bool Settings::ParseOptionalString(Setting<Utf8String>& setting, const Utf8String& stringValue)
{
    if (!stringValue.empty())
        setting = stringValue;
    return !stringValue.empty();
}

void Settings::ParseRequiredString(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String stringValue;
    GetElementText(stringValue, element);
    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "XML element is empty or has non-text content. You must specify a non-empty text-only value.");
        return;
    }

    setting = stringValue;
}

void Settings::ParseRequiredString(Setting<Utf8String>& setting, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");
        return;
    }

    setting = stringValue;
}

void Settings::ParseRequiredString(Setting<Path>& setting, const std::shared_ptr<XmlNode> element, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String stringValue;
    GetElementText(stringValue, element);
    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "XML element is empty or has non-text content. You must specify a non-empty text-only path value.");
        return;
    }

    setting = stringValue;
}

void Settings::ParseRequiredString(Setting<Path>& setting, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Path value is empty. You must specify a value.");
        return;
    }

    setting = stringValue;
}

void Settings::ParseRequiredUuid(Setting<Uuid>& setting, const std::shared_ptr<XmlNode> element, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String stringValue;
    GetElementText(stringValue, element);
    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "XML element is empty or has non-text content. You must specify a non-empty text-only value.");
        return;
    }

    ParseRequiredUuid(setting, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Settings::ParseRequiredUuid(Setting<Uuid>& setting, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Uuid value;
    Uuid::Parse(value, stringValue, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to parse the UUID.");
        return;
    }

    setting = value;
}

void Settings::ParseOptionalUuidAttribute(Setting<Uuid>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Setting<Utf8String> stringValue;
    if (ParseOptionalStringAttribute(stringValue, element, attributeName) && !stringValue.value.empty())
    {
        ParseRequiredUuid(setting, stringValue, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
}

bool Settings::ParseOptionalUuidAttribute(Setting<Uuid>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName)
{
    FINJIN_DECLARE_ERROR(error);

    Setting<Utf8String> stringValue;
    if (ParseOptionalStringAttribute(stringValue, element, attributeName) && !stringValue.value.empty())
    {
        ParseRequiredUuid(setting, stringValue, error);
        if (!error)
            return true;
    }

    return false;
}

void Settings::ParseRequiredLogLevel(Setting<LogLevel>& setting, const std::shared_ptr<XmlNode> element, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String stringValue;
    GetElementText(stringValue, element);
    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "XML element is empty or has non-text content. You must specify a non-empty text-only value.");
        return;
    }

    ParseRequiredLogLevel(setting, stringValue, error);
}

void Settings::ParseRequiredLogLevel(Setting<LogLevel>& setting, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
    {
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");
        return;
    }

    LogLevel logLevel;
    LogLevelUtilities::Parse(logLevel, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
    else
        setting = logLevel;
}

void Settings::ParseOptionalLogLevelAttribute(Setting<LogLevel>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Setting<Utf8String> stringValue;
    if (ParseOptionalStringAttribute(stringValue, element, attributeName) && !stringValue.value.empty())
    {
        ParseRequiredLogLevel(setting, stringValue, error);
        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }
}

bool Settings::ParseOptionalLogLevelAttribute(Setting<LogLevel>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName)
{
    FINJIN_DECLARE_ERROR(error);

    Setting<Utf8String> stringValue;
    if (ParseOptionalStringAttribute(stringValue, element, attributeName) && !stringValue.value.empty())
    {
        ParseRequiredLogLevel(setting, stringValue, error);
        if (!error)
            return true;
    }

    return false;
}

void Settings::ParseRequiredStringAttribute(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!ParseOptionalStringAttribute(setting, element, attributeName))
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Required attribute '%1%' is missing.", attributeName));
}

bool Settings::ParseOptionalStringAttribute(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName)
{
    Utf8String value;
    if (element->GetAttributeNoCase(attributeName, value))
    {
        setting = value;
        return true;
    }
    else
        return false;
}

void Settings::ParseRequiredStringAttribute(Setting<Path>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!ParseOptionalStringAttribute(setting, element, attributeName))
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Required attribute '%1%' is missing.", attributeName));
}

bool Settings::ParseOptionalStringAttribute(Setting<Path>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName)
{
    Utf8String value;
    if (element->GetAttributeNoCase(attributeName, value))
    {
        setting = value;
        return true;
    }
    else
        return false;
}

void Settings::ParseRequiredBool(Setting<bool>& setting, const std::shared_ptr<XmlNode> element, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String stringValue = element->GetText();
    if (stringValue.empty())
        FINJIN_SET_ERROR(error, "XML element is empty or has non-text content. You must specify a non-empty text-only value.");

    setting = Convert::ToBool(stringValue);
}

void Settings::ParseRequiredBool(Setting<bool>& setting, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (stringValue.empty())
        FINJIN_SET_ERROR(error, "Value is empty. You must specify a value.");

    setting = Convert::ToBool(stringValue);
}

bool Settings::ParseOptionalBoolAttribute(Setting<bool>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName)
{
    Setting<Utf8String> value;
    if (ParseOptionalStringAttribute(value, element, attributeName))
    {
        setting = Convert::ToBool(value);
        return true;
    }

    return false;
}

void Settings::ParseRequiredTimeDuration(Setting<TimeDuration>& setting, const std::shared_ptr<XmlNode> element, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8String stringValue = element->GetText();
    if (stringValue.empty())
        FINJIN_SET_ERROR(error, "XML element is empty or has non-text content. You must specify a non-empty text-only value.");

    ParseRequiredTimeDuration(setting, stringValue, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void Settings::ParseRequiredTimeDuration(Setting<TimeDuration>& setting, const Utf8String& stringValue, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    TimeDuration timeDuration;
    TimeDuration::Parse(timeDuration, stringValue, error);
    if (error)
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse time duration value '%1%'.", stringValue));
    else
        setting = timeDuration;
}

bool Settings::ParseOptionalTimeDurationAttribute(Setting<TimeDuration>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Setting<Utf8String> stringValue;
    if (ParseOptionalStringAttribute(stringValue, element, attributeName))
    {
        ParseRequiredTimeDuration(setting, stringValue, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse time duration attribute '%1%'.", attributeName));
            return false;
        }
        return true;
    }

    return false;
}

void Settings::GetElementText(Utf8String& result, const std::shared_ptr<XmlNode> element)
{
    if (element == nullptr)
        result.clear();
    else
        result = element->GetText();
}
