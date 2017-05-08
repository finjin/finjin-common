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
static const Utf8String& _GetHostName()
{
    static Utf8String result;

    if (result.empty())
    {
        const int MAX_HOSTNAME_LENGTH = 1024;

    #if FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
        auto hostNames = Windows::Networking::Connectivity::NetworkInformation::GetHostNames();
        if (hostNames->Size == 0)
        {
            //No host names. Exit early since the fallback code at the end of this function will not work
            return result;
        }

        result = hostNames->GetAt(0)->CanonicalName->Data();
        return result;
    #elif defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN8
        wchar_t hostNameW[MAX_HOSTNAME_LENGTH];
        GetHostNameW(hostNameW, MAX_HOSTNAME_LENGTH);
        result = hostNameW;
        return result;
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
                result = hostNameW;
                return result;
            }
        }
    #endif

        //Works on Windows 7/Windows Server 2008/Linux/etc
        char hostName[MAX_HOSTNAME_LENGTH];
        gethostname(hostName, MAX_HOSTNAME_LENGTH);
        result = hostName;
    }

    return result;
}


//Implementation----------------------------------------------------------------
DomainInformation::DomainInformation()
{
    this->hostName = _GetHostName();
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
