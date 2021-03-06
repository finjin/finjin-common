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
#include "finjin/common/WxBase64.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
static const char* BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t WxBase64::ToBase64Count(size_t byteCount)
{
    //3 pairs of 8-bits -> 4 pairs of 6-bits
    return 4 * ((byteCount + 2) / 3);
}

size_t WxBase64::ToByteCount(size_t characterCount)
{
    //4 pairs of 6-bits -> 3 pairs of 8-bits
    return 3 * (characterCount / 4);
}

void WxBase64::ToBase64(const void* voidBytes, size_t byteCount, wxString& base64, WxError& error)
{
    FINJIN_WX_ERROR_METHOD_START(error);

    if (byteCount == 0)
        base64.clear();
    else
    {
        auto base64Count = ToBase64Count(byteCount);

        std::vector<char> base64Utf8;
        base64Utf8.resize(base64Count);

        ToBase64(voidBytes, byteCount, (uint8_t*)&base64Utf8[0]);

        base64 = wxString::FromUTF8(&base64Utf8[0], base64Utf8.size());
    }
}

size_t WxBase64::ToBase64(const void* vbytes, size_t byteCount, uint8_t* base64)
{
    auto bytes = static_cast<const uint8_t*>(vbytes);

    size_t base64Offset = 0;

    int i = 0;
    int j = 0;
    uint8_t charArray3[3];
    uint8_t charArray4[4];

    while (byteCount-- != 0)
    {
        charArray3[i++] = *(bytes++);
        if (i == 3)
        {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (i = 0; (i <4) ; i++)
                base64[base64Offset++] = BASE64_CHARS[charArray4[i]];
            i = 0;
        }
    }

    if (i > 0)
    {
        for (j = i; j < 3; j++)
            charArray3[j] = '\0';

        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
        charArray4[3] = charArray3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            base64[base64Offset++] = BASE64_CHARS[charArray4[j]];

        while (i++ < 3)
            base64[base64Offset++] = '=';
    }

    return base64Offset;
}
