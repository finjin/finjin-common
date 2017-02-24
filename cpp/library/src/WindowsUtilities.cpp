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
#include "finjin/common/WindowsUtilities.hpp"
#include "finjin/common/Path.hpp"
#if FINJIN_TARGET_OS_IS_WINDOWS_UWP
    #include <ppltasks.h>
    #include <MemoryBuffer.h>
#else
    #include "finjin/common/Win32PnpDeviceEnumerator.hpp"
#endif
#include <vector>
#include <nowide/stackstring.hpp>

using namespace Finjin::Common;


//Local classes----------------------------------------------------------------
#if !FINJIN_TARGET_OS_IS_WINDOWS_UWP
class XInputDeviceEnumerator : public Win32PnpDeviceEnumerator
{
public:
    XInputDeviceEnumerator(const GUID& searchGuid)
    {
        this->searchGuid = searchGuid;
        this->isXInputDevice = false;
    }

    bool HandleDevice(wchar_t* bstrVal) override
    {
        //Enum each PNP device using WMI and check each device ID to see if it contains 
        //"IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
        //Unfortunately this information can not be found by just using DirectInput 

        if (wcsstr(bstrVal, L"IG_"))
        {
            //If it does, then get the VID/PID from var.bstrVal
            DWORD dwVid = 0;
            auto strVid = wcsstr(bstrVal, L"VID_");
            if (strVid != nullptr && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
                dwVid = 0;
            
            DWORD dwPid = 0;
            auto strPid = wcsstr(bstrVal, L"PID_");
            if (strPid != nullptr && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
                dwPid = 0;

            //Compare the VID/PID to the DInput device
            DWORD dwVidPid = MAKELONG(dwVid, dwPid);
            if (dwVidPid == this->searchGuid.Data1)
            {
                this->isXInputDevice = true;
                return false;
            }
        }

        return true;
    }

    GUID searchGuid;
    bool isXInputDevice;
};
#endif


//Implementation---------------------------------------------------------------
bool WindowsUtilities::CheckHResultFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        assert(0 && "Result indicates a failure.");
        return true;
    }
    else
        return false;
}

#if FINJIN_TARGET_OS_IS_WINDOWS_UWP
uint8_t* WindowsUtilities::GetPointerToIBufferData(Windows::Storage::Streams::IBuffer^ buffer)
{
    //Cast to Object^, then to its underlying IInspectable interface
    Platform::Object^ obj = buffer;
    Microsoft::WRL::ComPtr<IInspectable> insp(reinterpret_cast<IInspectable*>(obj));

    //Query the IBufferByteAccess interface
    Microsoft::WRL::ComPtr<Windows::Foundation::IMemoryBufferByteAccess> bufferByteAccess;
    insp.As(&bufferByteAccess);

    //Retrieve the buffer data
    uint8_t* data = nullptr;
    bufferByteAccess->GetBuffer(&data, nullptr);
    return data;
}

bool WindowsUtilities::ReadBinaryFile(Windows::Storage::StorageFolder^ storageFolder, const Path& relativeFilePath, ByteBuffer& buffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    try
    {
        nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> relativeFilePathW;
        relativeFilePathW.convert(relativeFilePath.begin(), relativeFilePath.end());

        auto fileTask = concurrency::create_task(storageFolder->GetFileAsync(ref new Platform::String(relativeFilePathW.c_str())));
        auto file = fileTask.get();
        auto fileBufferTask = concurrency::create_task(Windows::Storage::FileIO::ReadBufferAsync(file));
        auto fileBuffer = fileBufferTask.get();
        if (buffer.resize(fileBuffer->Length) < fileBuffer->Length)
        {
            FINJIN_SET_ERROR(error, "Failed to allocate memory for read.");
            return false;
        }
        FINJIN_COPY_MEMORY(buffer.data(), GetPointerToIBufferData(fileBuffer), fileBuffer->Length);
        return true;
    }
    catch (Platform::Exception^ e)
    {
        Utf8String errorString(e->ToString()->Data());
        FINJIN_SET_ERROR(error, errorString);
        return false;
    }
}

bool WindowsUtilities::ReadBinaryFile(Windows::Storage::StorageFolder^ storageFolder, const Path& relativeFilePath, ByteBuffer& buffer)
{
    try
    {
        nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> relativeFilePathW;
        relativeFilePathW.convert(relativeFilePath.begin(), relativeFilePath.end());

        auto fileTask = concurrency::create_task(storageFolder->GetFileAsync(ref new Platform::String(relativeFilePathW.c_str())));
        auto file = fileTask.get();
        auto fileBufferTask = concurrency::create_task(Windows::Storage::FileIO::ReadBufferAsync(file));
        auto fileBuffer = fileBufferTask.get();
        if (buffer.resize(fileBuffer->Length) < fileBuffer->Length)
            return false;
        FINJIN_COPY_MEMORY(buffer.data(), GetPointerToIBufferData(fileBuffer), fileBuffer->Length);
        return true;
    }
    catch (Platform::Exception^ e)
    {
        return false;
    }
}

#else

bool WindowsUtilities::IsXInputDevice(const GUID& guidProductFromDirectInput)
{
    XInputDeviceEnumerator enumerator(guidProductFromDirectInput);
    enumerator.Enumerate();
    return enumerator.isXInputDevice;
}

ValueOrError<void> WindowsUtilities::GetEnv(Utf8String& result, const Utf8String& key)
{
    nowide::basic_stackstring<wchar_t, char, Utf8String::STATIC_STRING_LENGTH + 1> keyW;
    if (!keyW.convert(key.begin(), key.end()))
    {
        result.clear();
        return ValueOrError<void>::CreateError();
    }

    //Try to get the environment variable without dynamically allocating memory
    static const size_t charCount = 1024;
    wchar_t buff[charCount];
    wchar_t* valuePtr = buff;

    auto valueLength = GetEnvironmentVariableW(keyW.c_str(), buff, charCount);
    if (valueLength == 0 || GetLastError() == ERROR_ENVVAR_NOT_FOUND)
    {
        //No environment variable for key
        result.clear();
        return ValueOrError<void>::CreateError();
    }

    std::vector<wchar_t> tempBuffer;
    if (valueLength >= charCount)
    {
        //Value length exceeds the length of the temporary buffer, so dynamically allocate
        tempBuffer.resize(valueLength + 1, L'\0');
        GetEnvironmentVariableW(keyW.c_str(), &tempBuffer[0], static_cast<DWORD>(tempBuffer.size()) - 1);
        valuePtr = &tempBuffer[0];
    }

    return result.assign(valuePtr);
}

Utf8String WindowsUtilities::GetEnv(const Utf8String& key)
{
    Utf8String value;
    GetEnv(value, key);
    return value;
}

#endif

bool WindowsUtilities::ConvertString(Utf8String& converted, DWORD dwMapFlags, const wchar_t* src, size_t srcLength)
{
    converted.clear();
    if (srcLength == 0)
        return true;

    std::array<wchar_t, 500> staticDest;
    auto result = LCMapStringEx(LOCALE_NAME_INVARIANT, dwMapFlags, src, static_cast<int>(srcLength), staticDest.data(), static_cast<int>(staticDest.size()), nullptr, nullptr, 0);
    if (result > 0)
    {
        converted.assign(staticDest.data(), result);
        return true;
    }
    else
    {
        std::vector<wchar_t> dest;
        dest.resize(srcLength * 2 + 1);
        result = LCMapStringEx(LOCALE_NAME_INVARIANT, dwMapFlags, src, static_cast<int>(srcLength), dest.data(), static_cast<int>(dest.size()), nullptr, nullptr, 0);
        if (result > 0)
        {
            converted.assign(dest.data(), result);
            return true;
        }
    }

    return false;
}

DWORD WindowsUtilities::GetPathAttributes(const Path& path)
{
    WIN32_FILE_ATTRIBUTE_DATA info = {};
    
    //Convert to wide string
    nowide::basic_stackstring<wchar_t, char, Path::STATIC_STRING_LENGTH + 1> pathW;
    if (!pathW.convert(path.begin(), path.end()))
        return INVALID_FILE_ATTRIBUTES;
    auto pathWLength = wcslen(pathW.c_str());

    //The slashes must be in Windows format for long paths
    for (size_t i = 0; i < pathWLength; i++)
    {
        auto& c = pathW.c_str()[i];

        if (c == L'/')
            c = L'\\';
    }

    //Convert to long path
    if (4 + pathWLength <= Path::STATIC_STRING_LENGTH)
    {
        wchar_t longPathW[Path::STATIC_STRING_LENGTH + 1];
        wcscpy(longPathW, L"\\\\?\\");
        wcscat(longPathW, pathW.c_str());

        //Get attributes
        GetFileAttributesExW(longPathW, GetFileExInfoStandard, &info);        
    }
    else
    {
        std::wstring longPathW = L"\\\\?\\";
        longPathW += pathW.c_str();

        //Get attributes
        GetFileAttributesExW(longPathW.c_str(), GetFileExInfoStandard, &info);        
    }

    return info.dwFileAttributes;
}

bool WindowsUtilities::IsFileAttribute(DWORD attributes)
{
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        return false;

    return (attributes & (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NORMAL)) != 0;
}

bool WindowsUtilities::IsDirectoryAttribute(DWORD attributes)
{
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Path WindowsUtilities::GetProcessFilePath(HMODULE moduleHandle)
{
    Path result;
    GetProcessFilePath(result, moduleHandle);
    return result;
}

ValueOrError<void> WindowsUtilities::GetProcessFilePath(Path& result, HMODULE moduleHandle)
{
    result.clear();
    
    const int charCount = 1024;
    wchar_t buff[charCount];
    auto charsCopied = GetModuleFileNameW(moduleHandle, buff, charCount);
    if (charsCopied > 0)
        return result.assign(buff);

    return ValueOrError<void>::CreateError();
}

Path WindowsUtilities::GetWorkingDirectory()
{
    Path result;

    const int charCount = 1024;
    wchar_t buff[charCount];
    auto charsCopied = GetCurrentDirectoryW(charCount, buff);
    if (charsCopied > 0)
        result = buff;

    return result;
}

ValueOrError<void> WindowsUtilities::GetWorkingDirectory(Path& result)
{
    result.clear();

    const int charCount = 1024;
    wchar_t buff[charCount];
    auto charsCopied = GetCurrentDirectoryW(charCount, buff);
    if (charsCopied > 0)
        return result.assign(buff);

    return ValueOrError<void>::CreateError();
}
