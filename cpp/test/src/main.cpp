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
#include "finjin/common/FinjinCommonLibrary.hpp"
using namespace Finjin::Common;


//Test "main"-------------------------------------------------------------------
#define BOOST_TEST_MODULE Finjin Common Library Tests
#include <boost/test/included/unit_test.hpp>


//Tests-------------------------------------------------------------------------
#include "Base64Test.hpp"
#include "BitArrayTest.hpp"
#include "ByteBufferTest.hpp"
#include "TypeDescriptionTest.hpp"
#include "CollectionTest.hpp"
#include "ConfigTest.hpp"
#include "DateTimeTest.hpp"
#include "JobSystemTest.hpp"
#include "LruTest.hpp"
#include "MemoryTest.hpp"
#include "PNGTest.hpp"
#include "PathTest.hpp"
#include "SettingsTest.hpp"
#include "StreamingDataDocumentTest.hpp"
#include "StringFormatTest.hpp"
#include "StringTest.hpp"
#include "TimeDurationParseTest.hpp"
#include "UuidTest.hpp"

//Useful command line: finjin-common-test --log_level=message
