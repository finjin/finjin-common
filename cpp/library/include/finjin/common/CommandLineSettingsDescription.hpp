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
     * A description of command line settings, used to display the application command line settings help.
     */
    class FINJIN_COMMON_LIBRARY_API CommandLineSettingsDescription
    {
    public:
        CommandLineSettingsDescription(const CommandLineSettingsDescription& other) = delete;
        CommandLineSettingsDescription& operator = (const CommandLineSettingsDescription& other) = delete;

        CommandLineSettingsDescription(CommandLineSettingsDescription&& other) = delete;
        CommandLineSettingsDescription& operator = (CommandLineSettingsDescription&& other) = delete;

    public:
        CommandLineSettingsDescription();
        virtual ~CommandLineSettingsDescription();

        /**
         * Starts a group of settings, each of which have the specified prefix.
         * @param prefix [in] The prefix for all the added settings. May be empty.
         */
        CommandLineSettingsDescription& Start(const Utf8String& prefix);

        /**
         * Adds a command line setting that does not have any arguments.
         * @param longName [in] The long name of the command line setting.
         * @param description [in] Textual description of the setting.
         * @return Reference to 'this'. Allows for chaining of AddSetting() calls.
         */
        CommandLineSettingsDescription& AddSetting(const Utf8String& longName, const Utf8String& description);

        /**
         * Adds a command line setting that has a variable number of arguments.
         * @param longName [in] The long name of the command line setting.
         * @param argCount [in] The maximum number of arguments the setting can accept. Should be 0 or 1. Anything other than 0 or 1 is treated as 1.
         * @param description [in] Textual description of the setting.
         * @return Reference to 'this'. Allows for chaining of AddSetting() calls.
         */
        CommandLineSettingsDescription& AddSetting(const Utf8String& longName, int argCount, const Utf8String& description);

        /**
         * Converts the settings description to a string.
         * @return The settings description as a string.
         */
        Utf8String ToString() const;

    private:
        struct Impl;
        Impl* impl;
    };

} }
