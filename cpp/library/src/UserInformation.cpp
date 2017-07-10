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
#include "finjin/common/UserInformation.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        #include <ppl.h>
        #include <ppltasks.h>

        using namespace Windows::Foundation;
        using namespace Windows::Foundation::Collections;
        using namespace Windows::System;
    #else
        #include <Windows.h>
    #endif
#else
    #include <unistd.h>
    #include <pwd.h>
    #include <stdlib.h>
#endif

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
UserInformation::UserInformation(Allocator* allocator)
{
    this->name.value.SetAllocator(allocator);

    this->initializationStatus.SetStatus(OperationStatus::STARTED);

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    this->requestedCancel = false;
    this->findUsersTask = concurrency::create_task(User::FindAllAsync()).then([this](IVectorView<User^>^ users)
    {
        if (this->requestedCancel)
        {
            this->initializationStatus.SetStatus(OperationStatus::FAILURE);

            concurrency::cancel_current_task();
        }
        else
        {
            this->getPropertyTask = concurrency::create_task(users->GetAt(0)->GetPropertyAsync(KnownUserProperties::FirstName)).then([this](Platform::Object^ obj)
            {
                if (this->requestedCancel)
                {
                    this->initializationStatus.SetStatus(OperationStatus::FAILURE);

                    concurrency::cancel_current_task();
                }
                else
                {
                    auto userName = safe_cast<Platform::String^>(obj);
                    if (!this->name.value.assign(userName->Data()).HasError())
                        this->name.isSet = true;

                    this->initializationStatus.SetStatus(OperationStatus::SUCCESS);
                }
            });
        }
    });
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    const int maxUserName = 500;
    wchar_t userName[maxUserName];
    DWORD userNameLength = maxUserName;
    if (GetUserNameW(userName, &userNameLength))
    {
        if (!this->name.value.assign(userName).HasError())
            this->name.isSet = true;
    }
    else
        this->name = Utf8String::GetEmpty(); //Need this for value to be "set"

    this->initializationStatus.SetStatus(OperationStatus::SUCCESS);
#else
    uid_t uid = geteuid();
    struct passwd* pw = getpwuid(uid);
    if (pw != nullptr)
    {
        if (!this->name.value.assign(pw->pw_name).HasError())
            this->name.isSet = true;
    }
    else
        this->name = Utf8String::GetEmpty(); //Need this for value to be "set"

    this->initializationStatus.SetStatus(OperationStatus::SUCCESS);
#endif
}

UserInformation::~UserInformation()
{
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    try
    {
        //if (!this->getPropertyTask.is_done()) //Not correct!?
        {
            this->requestedCancel = true;
            //this->getPropertyTask.wait(); //Not correct!?
        }
    }
    catch (Platform::Exception^ ex)
    {
    }
#endif
}

const Setting<Utf8String>& UserInformation::GetName() const
{
    return this->name;
}

const OperationStatus& UserInformation::GetInitializationStatus() const
{
    return this->initializationStatus;
}
