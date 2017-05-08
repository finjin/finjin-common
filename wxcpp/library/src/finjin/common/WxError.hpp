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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    #define FINJIN_WX_DECLARE_ERROR(error) WxError error
    #define FINJIN_WX_ERROR_METHOD_START(error)

    #define FINJIN_WX_ERROR_METHOD_START(error)
    #define FINJIN_WX_SET_ERROR(error, msg) error.Set(msg)
    #define FINJIN_WX_SET_ERROR_NO_MESSAGE(error) error.Set()
    #define FINJIN_WX_DEFAULT_ERROR_CHECK(error) \
        if (error) \
        { \
            FINJIN_WX_SET_ERROR_NO_MESSAGE(error); \
            return; \
        }

    class WxError
    {
    public:
        WxError() {this->error = false;}

        void Set() {this->error = true;}
        void Set(const wxString& s) {this->error = true;}
        operator bool () const {return this->error;}

    private:
        bool error;
    };

} }
