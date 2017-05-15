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
#include "finjin/common/ProcessRuntimeInformation.hpp"
#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #include <Windows.h>
#endif

using namespace Finjin::Common;


//Local functions---------------------------------------------------------------
#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_WINDOWS_WIN32

static bool _IsRunningInteractively()
{
    //From http://marc.info/?l=openssl-dev&m=104401851331452&w=2

    HANDLE hProcessToken = nullptr;
    DWORD groupLength = 50;

    PTOKEN_GROUPS groupInfo = (PTOKEN_GROUPS)LocalAlloc(0, groupLength);

    SID_IDENTIFIER_AUTHORITY siaNt = SECURITY_NT_AUTHORITY;
    PSID InteractiveSid = nullptr;
    PSID ServiceSid = nullptr;
    DWORD i;

    //Start with assumption that process is an EXE, not a Service.
    BOOL fExe = TRUE;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hProcessToken))
        goto ret;

    if (groupInfo == nullptr)
        goto ret;

    if (!GetTokenInformation(hProcessToken, TokenGroups, groupInfo, groupLength, &groupLength))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            goto ret;

        LocalFree(groupInfo);
        groupInfo = (PTOKEN_GROUPS)LocalAlloc(0, groupLength);

        if (groupInfo == nullptr)
            goto ret;

        if (!GetTokenInformation(hProcessToken, TokenGroups, groupInfo, groupLength, &groupLength))
            goto ret;
    }

    //We now know the groups associated with this token.  We want to look to see if the interactive group is active in
    //the token, and if so, we know that this is an interactive process.
    //
    //We also look for the "service" SID, and if it's present, we know we're a service.
    //
    //The service SID will be present if and only if the service is running in a user account (and was invoked by the service controller).

    if (!AllocateAndInitializeSid(&siaNt, 1, SECURITY_INTERACTIVE_RID, 0, 0, 0, 0, 0, 0, 0, &InteractiveSid))
        goto ret;

    if (!AllocateAndInitializeSid(&siaNt, 1, SECURITY_SERVICE_RID, 0, 0, 0, 0, 0, 0, 0, &ServiceSid))
        goto ret;

    for (i = 0; i < groupInfo->GroupCount; i += 1)
    {
        SID_AND_ATTRIBUTES sanda = groupInfo->Groups[i];
        PSID Sid = sanda.Sid;

        //Check to see if the group we're looking at is one of the 2 groups we're interested in.
        if (EqualSid(Sid, InteractiveSid))
        {
            //This process has the Interactive SID in its token.  This means that the process is running as an EXE.
            goto ret;
        }
        else if (EqualSid(Sid, ServiceSid))
        {
            //This process has the Service SID in its token.  This means that the process is running as
            //a service running in a user account.
            fExe = FALSE;
            goto ret;
        }
    }

    //Neither Interactive or Service was present in the current users token,
    //This implies that the process is running as a service, most likely running as LocalSystem.
    fExe = FALSE;

ret:
    if (InteractiveSid)
        FreeSid(InteractiveSid);
    if (ServiceSid)
        FreeSid(ServiceSid);
    if (groupInfo)
        LocalFree(groupInfo);
    if (hProcessToken)
        CloseHandle(hProcessToken);

    return fExe ? true : false;
}

#else

static bool _IsRunningInteractively()
{
    //Always interactive, even if running as a daemon
    return true;
}

#endif


//Implementation----------------------------------------------------------------
bool ProcessRuntimeInformation::IsRunningInteractively()
{
    static bool value = _IsRunningInteractively();
    return value;
}
