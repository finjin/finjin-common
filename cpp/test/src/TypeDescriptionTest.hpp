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
#include "finjin/common/TypeDescription.hpp"
#include <memory>


//Setup-------------------------------------------------------------------------
class SomeBaseClass
{
public:
    FINJIN_DECLARE_ABSTRACT_BASE_TYPE_DESCRIPTION(SomeBaseClass)
};
FINJIN_IMPLEMENT_ABSTRACT_BASE_TYPE_DESCRIPTION(SomeBaseClass, "test.SomeBaseClass")

class SomeDerivedClass : public SomeBaseClass
{
public:
    FINJIN_DECLARE_TYPE_DESCRIPTION(SomeDerivedClass, SomeBaseClass)
};
FINJIN_IMPLEMENT_TYPE_DESCRIPTION(SomeDerivedClass, "test.SomeDerivedClass")

class SomeUnrelatedBaseClass
{
public:
    FINJIN_DECLARE_ABSTRACT_BASE_TYPE_DESCRIPTION(SomeUnrelatedBaseClass)
};
FINJIN_IMPLEMENT_ABSTRACT_BASE_TYPE_DESCRIPTION(SomeUnrelatedBaseClass, "test.SomeUnrelatedBaseClass")

void TypeDescriptionTestSetup()
{
    static bool initialized = false;
    if (initialized)
        return;

    initialized = true;

    FINJIN_TYPE_DESCRIPTION(SomeBaseClass);
    FINJIN_TYPE_DESCRIPTION(SomeDerivedClass);
    FINJIN_TYPE_DESCRIPTION(SomeUnrelatedBaseClass);
}

//Tests-------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(TypeDescriptionTest_create_instance_of)
{
    BOOST_TEST_MESSAGE("TypeDescriptionTest_create_instance_of:");

    TypeDescriptionTestSetup();

    FINJIN_DECLARE_ERROR(error);
    std::unique_ptr<SomeBaseClass> instance(CreateInstanceOf<SomeBaseClass>("test.SomeDerivedClass", FINJIN_ALLOCATOR_NULL, FINJIN_CALLER_ARGUMENTS, error));
    BOOST_CHECK(instance != nullptr);
    BOOST_CHECK(error == false);
}

BOOST_AUTO_TEST_CASE(TypeDescriptionTest_failed_create_instance_of)
{
    BOOST_TEST_MESSAGE("TypeDescriptionTest_failed_create_instance_of:");

    TypeDescriptionTestSetup();

    {
        //Doesn't exist
        FINJIN_DECLARE_ERROR(error);
        std::unique_ptr<SomeBaseClass> instance(CreateInstanceOf<SomeBaseClass>("test.SomeDerivedClass2", FINJIN_ALLOCATOR_NULL, FINJIN_CALLER_ARGUMENTS, error));
        BOOST_CHECK(instance == nullptr);
        BOOST_CHECK(error != false);
    }

    {
        //Exists, but not an instance of the specified template class
        FINJIN_DECLARE_ERROR(error);
        std::unique_ptr<SomeBaseClass> instance(CreateInstanceOf<SomeBaseClass>("test.SomeUnrelatedBaseClass", FINJIN_ALLOCATOR_NULL, FINJIN_CALLER_ARGUMENTS, error));
        BOOST_CHECK(instance == nullptr);
        BOOST_CHECK(error != false);
    }
}
