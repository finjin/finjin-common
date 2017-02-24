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
#include "finjin/common/CommandLineSettingsDescription.hpp"
#include "finjin/common/Settings.hpp"
#include <sstream>
#include <boost/program_options.hpp>

using namespace Finjin::Common;


//Local classes-----------------------------------------------------------------
struct CommandLineSettingsDescription::Impl
{
    Impl()
    {        
    }

    boost::program_options::options_description description;
    std::unique_ptr<boost::program_options::options_description_easy_init> options;
    
    Utf8String prefix;
};


//Implementation----------------------------------------------------------------
CommandLineSettingsDescription::CommandLineSettingsDescription() : impl(new Impl)
{
}

CommandLineSettingsDescription::~CommandLineSettingsDescription()
{
    delete impl;
}

CommandLineSettingsDescription& CommandLineSettingsDescription::Start(const Utf8String& prefix)
{
    if (impl->options == nullptr || prefix != impl->prefix)
    {
        impl->prefix = prefix;

        impl->options.reset(new boost::program_options::options_description_easy_init(impl->description.add_options()));
    }

    return *this;
}

CommandLineSettingsDescription& CommandLineSettingsDescription::AddSetting(const Utf8String& longName, const Utf8String& description)
{
    return AddSetting(longName, 0, description);
}

CommandLineSettingsDescription& CommandLineSettingsDescription::AddSetting(const Utf8String& longName, int argCount, const Utf8String& description)
{
    using namespace boost::program_options;

    auto fullName = impl->prefix;
    fullName += longName;

    switch (argCount)
    {
        case 0: (*impl->options.get())(fullName.c_str(), description.c_str()); break;
        default: (*impl->options.get())(fullName.c_str(), value<std::string>(), description.c_str()); break;        
    }
    
    return *this;
}

Utf8String CommandLineSettingsDescription::ToString() const
{    
    std::stringstream descriptionStream;
    descriptionStream << impl->description;
    return descriptionStream.str().c_str();
}
