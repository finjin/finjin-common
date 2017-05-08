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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class ProcessRuntimeInformation
    {
    public:
        /**
         * Determines whether the application is being running 'interactively', the notion of which differs by platform.
         * @return If the application is being run as a Windows service, false is returned. In any other scenario, true is returned.
         */
        static bool IsRunningInteractively();
    };

} }
