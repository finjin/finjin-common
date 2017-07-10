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
#include "finjin/common/DomainInformation.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Winsock2.h>
#else
    #include <unistd.h>
    #include <pwd.h>
    #include <stdlib.h>
#endif

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
static ValueOrError<void> _GetHostName(Utf8String& result)
{
    const int MAX_HOSTNAME_LENGTH = 1024;

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    auto hostNames = Windows::Networking::Connectivity::NetworkInformation::GetHostNames();
    if (hostNames->Size == 0)
    {
        //No host names. Exit early since the fallback code at the end of this function will not work
        return ValueOrError<void>::CreateError();
    }

    return result.assign(hostNames->GetAt(0)->CanonicalName->Data());
#elif defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN8
    wchar_t hostNameW[MAX_HOSTNAME_LENGTH];
    GetHostNameW(hostNameW, MAX_HOSTNAME_LENGTH);
    return result.assign(hostNameW);
#elif FINJIN_TARGET_PLATFORM_IS_WINDOWS
    //Works in Win8/WinServer2012 and up. Not Win7
    typedef int (WSAAPI *GetHostNameWProc)(PWSTR, int);
    HMODULE ws2_32 = LoadLibraryW(L"Ws2_32.dll");
    if (ws2_32 != nullptr)
    {
        auto getHostNameWProc = reinterpret_cast<GetHostNameWProc>(GetProcAddress(ws2_32, "GetHostNameW"));
        if (getHostNameWProc != nullptr)
        {
            wchar_t hostNameW[MAX_HOSTNAME_LENGTH];
            getHostNameWProc(hostNameW, MAX_HOSTNAME_LENGTH);
            return result.assign(hostNameW);
        }
    }
#endif

    //Works on Windows 7/Windows Server 2008/Linux/etc
    char hostName[MAX_HOSTNAME_LENGTH];
    gethostname(hostName, MAX_HOSTNAME_LENGTH);
    return result.assign(hostName);
}


//Implementation----------------------------------------------------------------
DomainInformation::DomainInformation(Allocator* allocator)
{
    this->hostName.value.SetAllocator(allocator);

    if (!_GetHostName(this->hostName).HasError())
        this->hostName.isSet = true;

    this->initializationStatus.SetStatus(OperationStatus::SUCCESS);
}

DomainInformation::~DomainInformation()
{
}

const Setting<Utf8String>& DomainInformation::GetHostName() const
{
    return this->hostName;
}

const OperationStatus& DomainInformation::GetInitializationStatus() const
{
    return this->initializationStatus;
}
