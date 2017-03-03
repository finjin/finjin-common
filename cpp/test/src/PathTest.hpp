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


//Includes---------------------------------------------------------------------
#include "finjin/common/Path.hpp"


//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(PathTest_extension)
{
    BOOST_TEST_MESSAGE("PathTest_extension:");

    Path p("somefile.txt");
    Utf8String ext;

    BOOST_CHECK(p.GetExtension(ext, false).HasValue(true));
    BOOST_CHECK(ext == "txt");
    
    BOOST_CHECK(p.GetExtension(ext, true).HasValue(true));
    BOOST_CHECK(ext == ".txt");
}

BOOST_AUTO_TEST_CASE(PathTest_absolute)
{
    BOOST_TEST_MESSAGE("PathTest_absolute:");

#if FINJIN_TARGET_OS_IS_WINDOWS
    Path path("c:\\somefile.txt");
#else
    Path path("/somefile.txt");
#endif
    
    BOOST_CHECK(path.IsAbsolute() == true);
    BOOST_CHECK(path.IsRelative() == false);
}

BOOST_AUTO_TEST_CASE(PathTest_relative)
{
    BOOST_TEST_MESSAGE("PathTest_relative:");

    Path path("./somefile.txt");
    BOOST_CHECK(path.IsRelative() == true);
    BOOST_CHECK(path.IsAbsolute() == false);    
}

BOOST_AUTO_TEST_CASE(PathTest_parent)
{
    BOOST_TEST_MESSAGE("PathTest_parent:");

#if FINJIN_TARGET_OS_IS_WINDOWS
    Path path("c:/users/someone/somefile.txt");
    Path parentPath;
    BOOST_CHECK(!path.GetParent(parentPath).HasError());
    BOOST_CHECK(parentPath == "c:/users/someone");
#else
    Path path("/users/someone/somefile.txt");
    Path parentPath;
    BOOST_CHECK(!path.GetParent(parentPath).HasError() && parentPath == "/users/someone");
#endif
}

BOOST_AUTO_TEST_CASE(PathTest_contains_directory)
{
    BOOST_TEST_MESSAGE("PathTest_contains_directory:");

#if FINJIN_TARGET_OS_IS_WINDOWS
    Path path("c:/users/someone");
    Path goodSubpath("c:/users/someone/subdir/somefile.txt");
#else
    Path path("/users/someone");
    Path goodSubpath("/users/someone/subdir/somefile.txt");
#endif
    BOOST_CHECK(path.ContainsDirectory(goodSubpath).HasValue(true));
    BOOST_CHECK(path.ContainsFile(goodSubpath).HasValue(true));
}

BOOST_AUTO_TEST_CASE(PathTest_just_file_name)
{
    BOOST_TEST_MESSAGE("PathTest_just_file_name:");

    Path path("somefile.txt");

    Path baseName;
    BOOST_CHECK(path.GetBaseName(baseName).HasValue(true));
    BOOST_CHECK(baseName == "somefile");
    Utf8String ext;
    BOOST_CHECK(path.GetExtension(ext, true).HasValue(true));
    BOOST_CHECK(ext == ".txt");
}

BOOST_AUTO_TEST_CASE(PathTest_goto_parent)
{
    BOOST_TEST_MESSAGE("PathTest_goto_parent:");
        
    Path path;

    //Windows-style
    path = "c:/root/file.txt";
    
    path.GoToParent();
    BOOST_CHECK(path == "c:/root");

    path.GoToParent();
    BOOST_CHECK(path == "c:");

    path.GoToParent();
    BOOST_CHECK(path.empty());

    //Linux-style
    path = "/root/file.txt";

    path.GoToParent();
    BOOST_CHECK(path == "/root");

    path.GoToParent();
    BOOST_CHECK(path.empty());
}

BOOST_AUTO_TEST_CASE(PathTest_append_path)
{
    BOOST_TEST_MESSAGE("PathTest_append_path:");

    Path path("c:/somedir/a/b/c");

    Path newPath = path;
    newPath /= "../d";
    newPath.UniversalNormalize();
    BOOST_CHECK(newPath == "c:/somedir/a/b/d");
}

BOOST_AUTO_TEST_CASE(PathTest_absolute_append_test)
{
    BOOST_TEST_MESSAGE("PathTest_absolute_append_test:");
    
#if FINJIN_TARGET_OS_IS_WINDOWS
    Path path("c:/somedir/a/b/c");
    Path newPath = path;
    newPath /= "d:/d";
    BOOST_CHECK(newPath == "d:/d");
#else
    Path path("/somedir/a/b/c");
    Path newPath = path / "/d";
    BOOST_CHECK(newPath == "/d");
#endif
}

BOOST_AUTO_TEST_CASE(PathTest_unc_test)
{
    BOOST_TEST_MESSAGE("PathTest_unc_test:");

    Path path("\\\\?\\c:/somedir/a/b/c");

    BOOST_CHECK(path.IsAbsolute() == true);

    path.GoToParent();
    BOOST_CHECK(path == "\\\\?\\c:/somedir/a/b");
}

BOOST_AUTO_TEST_CASE(PathTest_ensure_extension_test)
{
    BOOST_TEST_MESSAGE("PathTest_ensure_extension_test:");
        
    {
        Path path("/somefile");
        path.EnsureExtension("txt");

        BOOST_CHECK(path == "/somefile.txt");
    }

    {
        Path path("/somefile");
        path.EnsureExtension(".txt");

        BOOST_CHECK(path == "/somefile.txt");
    }

    {
        Path path("/somefile.txt");
        path.EnsureExtension(".jpg");

        BOOST_CHECK(path == "/somefile.txt");
    }
}
