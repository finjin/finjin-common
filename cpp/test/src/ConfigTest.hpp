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
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/ConfigDocumentWriter.hpp"
#include <sstream>


//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ConfigTest_write)
{
    BOOST_TEST_MESSAGE("ConfigTest_write:");

    std::stringstream outStream;
    ConfigDocumentWriter writer;
    writer.Create(outStream);
    writer.WriteSection("a");
    writer.WriteKeyAndValue("x", "1");
    writer.WriteCommentedSection("This is a comment", "b");
    auto result = outStream.str();

    const char* expected =
        "\n"
        "[a]\n"
        "x=1\n"
        "\n"
        "#This is a comment\n"
        "[b]\n"
        ;
    BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(ConfigTest_read)
{
    BOOST_TEST_MESSAGE("ConfigTest_read:");

    ConfigDocumentReader reader;
    const char* configText =
        "[section]\n"
        "this is text\n"
        "key=value\n"
        "#comment\n"
        "\n"
        "[another section]\n"
        ;
    auto line = reader.Start(configText, strlen(configText));
    BOOST_CHECK(line != nullptr);
    BOOST_CHECK(line->GetType() == ConfigDocumentLine::Type::SECTION);
    BOOST_CHECK(line->GetSectionName() == "section");

    line = reader.Next();
    BOOST_CHECK(line != nullptr);
    BOOST_CHECK(line->GetType() == ConfigDocumentLine::Type::PLAIN_LINE);
    BOOST_CHECK(line->GetLine() == "this is text");

    line = reader.Next();
    BOOST_CHECK(line != nullptr);
    BOOST_CHECK(line->GetType() == ConfigDocumentLine::Type::KEY_AND_VALUE);
    BOOST_CHECK(line->GetKey() == "key");
    BOOST_CHECK(line->GetValue() == "value");

    line = reader.Next();
    BOOST_CHECK(line != nullptr);
    BOOST_CHECK(line->GetType() == ConfigDocumentLine::Type::COMMENT);
    BOOST_CHECK(line->GetComment() == "comment");

    line = reader.Next();
    BOOST_CHECK(line != nullptr);
    BOOST_CHECK(line->GetType() == ConfigDocumentLine::Type::SECTION);
    BOOST_CHECK(line->GetSectionName() == "another section");

    line = reader.Next();
    BOOST_CHECK(line == nullptr);
}
