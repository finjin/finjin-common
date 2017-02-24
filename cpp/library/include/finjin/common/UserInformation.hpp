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
#if FINJIN_TARGET_OS_IS_WINDOWS_UWP
    #include <ppltasks.h>
#endif


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class FINJIN_COMMON_LIBRARY_API UserInformation
    {
    public:
        UserInformation();
        ~UserInformation();

        const Setting<Utf8String>& GetName() const;

        const OperationStatus& GetInitializationStatus() const;

    private:
        Setting<Utf8String> name;
    #if FINJIN_TARGET_OS_IS_WINDOWS_UWP
        bool requestedCancel;
        concurrency::task<void> findUsersTask;
        concurrency::task<void> getPropertyTask;
    #endif

        OperationStatus initializationStatus;
    };

} }
