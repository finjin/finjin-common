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


//Includes---------------------------------------------------------------------
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include <Windows.h>


//Macros-----------------------------------------------------------------------
#if FINJIN_DEBUG
    #define FINJIN_CHECK_HRESULT_FAILED(hr) WindowsUtilities::CheckHResultFailed(hr)
#else
    #define FINJIN_CHECK_HRESULT_FAILED(hr) FAILED(hr)
#endif


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class WindowsUtilities
    {
    public:
        static bool CheckHResultFailed(HRESULT hr);

    #if FINJIN_TARGET_OS_IS_WINDOWS_UWP
        static uint8_t* GetPointerToIBufferData(Windows::Storage::Streams::IBuffer^ buffer);

        static bool ReadBinaryFile(Windows::Storage::StorageFolder^ storageFolder, const Path& relativeFilePath, ByteBuffer& buffer, Error& error);
        static bool ReadBinaryFile(Windows::Storage::StorageFolder^ storageFolder, const Path& relativeFilePath, ByteBuffer& buffer);
    #else
        static bool IsXInputDevice(const GUID& guidProductFromDirectInput);

        static ValueOrError<void> GetEnv(Utf8String& result, const Utf8String& key);
        static Utf8String GetEnv(const Utf8String& key);        
    #endif

        static bool ConvertString(Utf8String& dest, DWORD dwMapFlags, const wchar_t* src, size_t srcLength);

        template <typename T>
        static Utf8String BlobToString(T& errorBlob)
        {
            if (errorBlob != nullptr)
                return Utf8String(static_cast<const char*>(errorBlob->GetBufferPointer()), static_cast<size_t>(errorBlob->GetBufferSize()));
            else
                return Utf8String::Empty();
        }

        static DWORD GetPathAttributes(const Path& path);

        static bool IsFileAttribute(DWORD attributes);
        static bool IsDirectoryAttribute(DWORD attributes);

        static Path GetProcessFilePath(HMODULE moduleHandle = nullptr);
        static ValueOrError<void> GetProcessFilePath(Path& result, HMODULE moduleHandle = nullptr);
        
        static Path GetWorkingDirectory();
        static ValueOrError<void> GetWorkingDirectory(Path& result);
    };

} }
