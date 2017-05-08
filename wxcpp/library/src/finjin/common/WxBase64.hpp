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
#include "finjin/common/WxError.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class WxBase64
    {
    public:
        static size_t ToBase64Count(size_t byteCount);

        static size_t ToByteCount(size_t characterCount);

        static void ToBase64(const void* bytes, size_t byteCount, wxString& base64, WxError& error);
        static size_t ToBase64(const void* bytes, size_t byteCount, uint8_t* output);
    };

} }
