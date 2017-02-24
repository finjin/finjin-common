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
#include "finjin/common/Win32PnpDeviceEnumerator.hpp"
#include <wbemidl.h>
#include <ObjIdl.h>

using namespace Finjin::Common;


//Implementation---------------------------------------------------------------
void Win32PnpDeviceEnumerator::Enumerate()
{
    IWbemLocator* pIWbemLocator = nullptr;
    IEnumWbemClassObject* pEnumDevices = nullptr;
    IWbemClassObject* pDevices[20] = { 0 };
    IWbemServices* pIWbemServices = nullptr;
    BSTR bstrNamespace = nullptr;
    BSTR bstrDeviceID = nullptr;
    BSTR bstrClassName = nullptr;
    VARIANT var;

    //CoInit if needed
    HRESULT hr = CoInitialize(nullptr);
    bool bCleanupCOM = SUCCEEDED(hr);

    //Create WMI
    hr = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (void**)&pIWbemLocator);
    if (FAILED(hr) || pIWbemLocator == nullptr)
        goto LCleanup;

    bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");
    if (bstrNamespace == nullptr)
        goto LCleanup;

    bstrClassName = SysAllocString(L"Win32_PNPEntity");
    if (bstrClassName == nullptr)
        goto LCleanup;

    bstrDeviceID = SysAllocString(L"DeviceID");
    if (bstrDeviceID == nullptr)
        goto LCleanup;

    //Connect to WMI 
    hr = pIWbemLocator->ConnectServer(bstrNamespace, nullptr, nullptr, 0, 0, nullptr, nullptr, &pIWbemServices);
    if (FAILED(hr) || pIWbemServices == nullptr)
        goto LCleanup;

    //Switch security level to IMPERSONATE. 
    CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

    hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &pEnumDevices);
    if (FAILED(hr) || pEnumDevices == nullptr)
        goto LCleanup;

    //Loop over all devices
    for (;;)
    {
        //Get some of them at a time
        DWORD uReturned = 0;
        hr = pEnumDevices->Next(10000, (ULONG)std::size(pDevices), pDevices, &uReturned);
        if (FAILED(hr))
            goto LCleanup;
        if (uReturned == 0)
            break;

        for (UINT deviceIndex = 0; deviceIndex < uReturned; deviceIndex++)
        {
            //For each device, get its device ID
            hr = pDevices[deviceIndex]->Get(bstrDeviceID, 0, &var, nullptr, nullptr);
            if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
            {
                if (!HandleDevice(var.bstrVal))
                    goto LCleanup;
            }

            FINJIN_SAFE_RELEASE(pDevices[deviceIndex]);
        }
    }

LCleanup:
    if (bstrNamespace != nullptr)
        SysFreeString(bstrNamespace);
    if (bstrDeviceID != nullptr)
        SysFreeString(bstrDeviceID);
    if (bstrClassName != nullptr)
        SysFreeString(bstrClassName);
    for (UINT deviceIndex = 0; deviceIndex < std::size(pDevices); deviceIndex++)
        FINJIN_SAFE_RELEASE(pDevices[deviceIndex]);
    FINJIN_SAFE_RELEASE(pEnumDevices);
    FINJIN_SAFE_RELEASE(pIWbemLocator);
    FINJIN_SAFE_RELEASE(pIWbemServices);

    if (bCleanupCOM)
        CoUninitialize();
}
