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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/ClassDescription.hpp"
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/LogLevel.hpp"
#include "finjin/common/NamePrefix.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/Setting.hpp"
#include "finjin/common/StandardPaths.hpp"
#include "finjin/common/Uuid.hpp"
#include "finjin/common/XmlDocument.hpp"


//Macros------------------------------------------------------------------------

/** Prefix for a short command line argument. */
#define FINJIN_SHORT_COMMAND_LINE_ARG_PREFIX "-"

/** Prefix for a long command line argument. */
#define FINJIN_LONG_COMMAND_LINE_ARG_PREFIX FINJIN_SHORT_COMMAND_LINE_ARG_PREFIX FINJIN_SHORT_COMMAND_LINE_ARG_PREFIX

/** 
 * Sets an error for an invalid command line setting.
 * @param error [out] Error object.
 * @param prefix [in] The command line setting name prefix.
 * @param settingName [in] The command line setting name.
 * @see Error
 */
#define FINJIN_SET_ERROR_FOR_COMMAND_LINE_SETTING(error, prefix, settingName) \
    FINJIN_SET_ERROR(error, Finjin::Common::Utf8StringFormatter::Format("Invalid setting for '%1%%2%%3%'.", FINJIN_LONG_COMMAND_LINE_ARG_PREFIX, prefix.ToCommandLineString(), settingName))

/**
 * Sets an error for an invalid XML element setting.
 * @param error [out] Error object.
 * @param settingName [in] The XML element name.
 * @see Error
 */
#define FINJIN_SET_ERROR_FOR_XML_ELEMENT_SETTING(error, settingName) \
    FINJIN_SET_ERROR(error, Finjin::Common::Utf8StringFormatter::Format("Invalid setting for '%1%'.", settingName))

/**
 * Sets an error for an invalid XML attribute setting.
 * @param error [out] Error object.
 * @param elementName [in] The XML element name.
 * @param settingName [in] The XML attribute name.
 * @see Error
 */
#define FINJIN_SET_ERROR_FOR_XML_ATTRIBUTE_SETTING(error, elementName, settingName) \
    FINJIN_SET_ERROR(error, Finjin::Common::Utf8StringFormatter::Format("Invalid setting for '%1%' in element '%2%'.", settingName, elementName))


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FileLogListener;
    class CommandLineSettingsDescription;
    
    enum class SettingsFlags
    {
        NONE = 0,

        /**
        * Indicates that CompletePath() should be called for all path-based settings.
        */
        COMPLETE_PATHS = 1 << 0,

        /**
        * Indicates LoadXml(...) that it should store the pointer being passed in.
        * This is only used in a few rare cases (when initially building the application structure) and
        * should not generally be used.
        */
        LOAD_XML_STORE_ELEMENT = 1 << 1
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(SettingsFlags)

    /**
     * Settings utility class that other settings classes can derive from.
     * The general idea behind the settings system is for components to be able to
     * configure their settings in terms a hierarchy of settings. A typical 
     * hierarchy: final settings = application file settings + command line settings
     */
    class FINJIN_COMMON_LIBRARY_API Settings
    {    
    public:
        Settings();
        virtual ~Settings();
        
        /** 
         * Parses settings from the command line. This will usually be overridden in derived classes.
         * @param args [in] The command line arguments to parse. Unrecognized options should be skipped.
         * @param prefix [in] The prefix for the settings. These are used when matching options.
         * @param flags [in] Parse flags.
         * @param error [out] Error object.
         * @see Error
         */
        virtual void ParseCommandLine(CommandLineArgsProcessor& args, const NamePrefix& prefix, SettingsFlags flags, const StandardPaths& standardPaths, Error& error) {}

        /**
         * Parses settings from the specified XML file. Will sometimes be overridden.
         * @param fileName [in] Path to the XML file.
         * @param flags [in] Parse flags.
         * @param error [out] Error object.
         * @see Error
         */
        virtual void LoadFile(const Path& fileName, SettingsFlags flags, const StandardPaths& standardPaths, Error& error) {}

        /**
         * Parses settings from the specified XML element. Will usually be overridden.
         * @param rootElement [in] Root XML element.
         * @param flags [in] Parse flags.
         * @param error [out] Error object.
         * @see Error
         */
        virtual void LoadXml(const std::shared_ptr<XmlNode> rootElement, SettingsFlags flags, const StandardPaths& standardPaths, Error& error) {} 
        
        /** 
         * Gets the settings file path from which these settings were loaded.
         * @return The settings file path from which these settings were loaded.
         */
        const Path& GetThisFileName() const;

        /**
         * Sets the settings file path from which these settings were loaded.
         * @param fileName [in] The settings file path from which these settings were loaded.
         */
        void SetThisFileName(const Path& fileName);

        /** Indicates what base path should be used when completing paths. */
        enum class RelativeTo
        {
            /** Relative to the application's current working directory. */
            CURRENT_WORKING_DIRECTORY,

            /** Relative to the application executable. */
            APPLICATION,

            /** Relative to the settings file. */
            SETTINGS
        };
        Path CompletePath(const Path& path, RelativeTo relativeTo, const StandardPaths& standardPaths);

        static bool IsOption(const Utf8String& arg, const Utf8String& name, const NamePrefix& prefix);
        static bool IsOption(const Utf8String& arg, const Utf8String& name, const Utf8String& otherName, const NamePrefix& prefix);
        
        static bool IsElement(const Utf8String& arg, const Utf8String& name);
        static bool IsElement(const Utf8String& arg, const Utf8String& name, const Utf8String& otherName);
        
        static bool ParseOptionalString(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element);
        static bool ParseOptionalString(Setting<Utf8String>& setting, const Utf8String& stringValue);

        static void ParseRequiredString(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element, Error& error);
        static void ParseRequiredString(Setting<Utf8String>& setting, const Utf8String& stringValue, Error& error);
        
        static void ParseRequiredString(Setting<Path>& setting, const std::shared_ptr<XmlNode> element, Error& error);
        static void ParseRequiredString(Setting<Path>& setting, const Utf8String& stringValue, Error& error);
                
        static void ParseRequiredUuid(Setting<Uuid>& setting, const std::shared_ptr<XmlNode> element, Error& error);
        static void ParseRequiredUuid(Setting<Uuid>& setting, const Utf8String& stringValue, Error& error);
        static void ParseOptionalUuidAttribute(Setting<Uuid>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error);
        static bool ParseOptionalUuidAttribute(Setting<Uuid>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName);
        
        static void ParseRequiredLogLevel(Setting<LogLevel>& setting, const std::shared_ptr<XmlNode> element, Error& error);
        static void ParseRequiredLogLevel(Setting<LogLevel>& setting, const Utf8String& stringValue, Error& error);
        static void ParseOptionalLogLevelAttribute(Setting<LogLevel>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error);
        static bool ParseOptionalLogLevelAttribute(Setting<LogLevel>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName);
        
        static void ParseRequiredStringAttribute(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error);
        static bool ParseOptionalStringAttribute(Setting<Utf8String>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName);

        static void ParseRequiredStringAttribute(Setting<Path>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error);
        static bool ParseOptionalStringAttribute(Setting<Path>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName);
        
        static void ParseRequiredBool(Setting<bool>& setting, const std::shared_ptr<XmlNode> element, Error& error);
        static void ParseRequiredBool(Setting<bool>& setting, const Utf8String& stringValue, Error& error);

        static bool ParseOptionalBoolAttribute(Setting<bool>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName);

        static void ParseRequiredTimeDuration(Setting<TimeDuration>& setting, const std::shared_ptr<XmlNode> element, Error& error);
        static void ParseRequiredTimeDuration(Setting<TimeDuration>& setting, const Utf8String& stringValue, Error& error);        
        static bool ParseOptionalTimeDurationAttribute(Setting<TimeDuration>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error);
      
        static void GetElementText(Utf8String& result, const std::shared_ptr<XmlNode> element);

    protected:
        Setting<Path> thisFileName; //Not a true setting, but it is treated as one when combining settings
    };

} }


//Utility functions-------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    void ParseRequiredNumberInRange(Setting<T>& setting, const Utf8String& stringValue, const T& minValue, const T& maxValue, Error& error)
    {   
        FINJIN_ERROR_METHOD_START(error);
        
        T convertedValue;
        Convert::ToNumberInRange(convertedValue, stringValue, minValue, maxValue, error);
        if (error)
            return;
        
        setting = convertedValue;
    }
    
    template <typename T>
    void ParseRequiredNumber(Setting<T>& setting, const Utf8String& stringValue, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);
        
        ParseRequiredNumberInRange(setting, stringValue, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), error);
    }
    
    template <typename T>
    void ParseRequiredNumberInRange(Setting<T>& setting, const std::shared_ptr<XmlNode> element, const T& minValue, const T& maxValue, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);
        
        Utf8String stringValue = element->GetText();  
        if (stringValue.empty())
        {
            FINJIN_SET_ERROR(error, "XML element is empty. You must specify a value.");
            return;
        }

        ParseRequiredNumberInRange(setting, stringValue, minValue, maxValue, error);
    }
    
    template <typename T>
    void ParseRequiredNumber(Setting<T>& setting, const std::shared_ptr<XmlNode> element, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);
        
        ParseRequiredNumberInRange(setting, element, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), error);
    }
    
    template <typename T>
    bool ParseRequiredNumberAttribute(Setting<T>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);
        
        Setting<Utf8String> stringSetting;
        Settings::ParseRequiredStringAttribute(stringSetting, element, attributeName, error);
        if (error)
            return false;
        
        ParseRequiredNumber(setting, stringSetting, error);
        if (error)
            return false;
        
        return true;
    }    
    
    template <typename T>
    bool ParseOptionalNumberAttributeInRange(Setting<T>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, const T& minValue, const T& maxValue, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);
        
        Setting<Utf8String> stringSetting;
        if (Settings::ParseOptionalStringAttribute(stringSetting, element, attributeName))
        {
            ParseRequiredNumberInRange(setting, stringSetting.value, minValue, maxValue, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("'%1%' is an invalid value for '%2%'.", stringSetting.value, attributeName));
                return false;
            }
            else
                return true;
        }
        
        return false;
    }
    
    template <typename T>
    bool ParseOptionalNumberAttribute(Setting<T>& setting, const std::shared_ptr<XmlNode> element, const Utf8String& attributeName, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);
        
        return ParseOptionalNumberAttributeInRange(setting, element, attributeName, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), error);
    }

} }
