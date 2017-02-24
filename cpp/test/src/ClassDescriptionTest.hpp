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
#include "finjin/common/ClassDescription.hpp"
#include <memory>


//Setup------------------------------------------------------------------------
class SomeBaseClass
{
public:
    FINJIN_DECLARE_ABSTRACT_BASE_CLASS_DESCRIPTION(SomeBaseClass)
};
FINJIN_IMPLEMENT_ABSTRACT_BASE_CLASS_DESCRIPTION(SomeBaseClass, "test.SomeBaseClass")

class SomeDerivedClass : public SomeBaseClass
{
public:
    FINJIN_DECLARE_CLASS_DESCRIPTION(SomeDerivedClass, SomeBaseClass)
};
FINJIN_IMPLEMENT_CLASS_DESCRIPTION(SomeDerivedClass, "test.SomeDerivedClass")

class SomeUnrelatedBaseClass
{
public:
    FINJIN_DECLARE_ABSTRACT_BASE_CLASS_DESCRIPTION(SomeUnrelatedBaseClass)
};
FINJIN_IMPLEMENT_ABSTRACT_BASE_CLASS_DESCRIPTION(SomeUnrelatedBaseClass, "test.SomeUnrelatedBaseClass")

void ClassDescriptionTestSetup()
{
    static bool initialized = false;
    if (initialized)
        return;

    initialized = true;

    FINJIN_CLASS_DESCRIPTION(SomeBaseClass);
    FINJIN_CLASS_DESCRIPTION(SomeDerivedClass);
    FINJIN_CLASS_DESCRIPTION(SomeUnrelatedBaseClass);
}

//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ClassDescriptionTest_create_instance_of)
{
    BOOST_TEST_MESSAGE("ClassDescriptionTest_create_instance_of:");

    ClassDescriptionTestSetup();

    FINJIN_DECLARE_ERROR(error);
    std::unique_ptr<SomeBaseClass> instance(CreateInstanceOf<SomeBaseClass>("test.SomeDerivedClass", FINJIN_ALLOCATOR_NULL, FINJIN_CALLER_ARGUMENTS, error));
    BOOST_CHECK(instance != nullptr);
    BOOST_CHECK(error == false);
}

BOOST_AUTO_TEST_CASE(ClassDescriptionTest_failed_create_instance_of)
{
    BOOST_TEST_MESSAGE("ClassDescriptionTest_failed_create_instance_of:");

    ClassDescriptionTestSetup();

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
