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
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

using JsonStringBuffer = rapidjson::StringBuffer;
using JsonReader = rapidjson::Reader;
using JsonStringWriter = rapidjson::Writer<JsonStringBuffer>;
using JsonStringPrettyWriter = rapidjson::PrettyWriter<JsonStringBuffer>;
using JsonValue = rapidjson::Value;
using JsonElement = rapidjson::Value;


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    struct JsonDocumentImpl
    {
        rapidjson::Document doc;
    };

} }
