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
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Setting.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API DomainInformation
    {
    public:
        DomainInformation(Allocator* allocator);
        ~DomainInformation();

        const Setting<Utf8String>& GetHostName() const;

        const OperationStatus& GetInitializationStatus() const;

    private:
        Setting<Utf8String> hostName;
        OperationStatus initializationStatus;
    };

} }
