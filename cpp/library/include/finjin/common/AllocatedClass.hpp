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
#include "Allocator.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    /**
     * Provides custom memory allocation services for a class.
     * To use this, a derived class must be declared. For example:
     * class Person : public AllocatedClass
     * {
     * public:
     *     Person(int a): age(a) {}
     *     int age;
     * };
     *
     * Then, when creating an instance of the class, it is done as as follows:
     * auto person = AllocatedClass::New<Person>(allocator, FINJIN_CALLER_ARGUMENTS, 52);
     *
     * where 'allocator' refers to previously instantiated pointer to some sort of Allocator class.
     *
     * The Person object can be deleted as follows:
     * delete person;
     *
     * Alternatively, the person can be automatically deleted with an smart pointer:
     * std::unique_ptr<Person> smartPerson(person);
     */
    class FINJIN_COMMON_LIBRARY_API AllocatedClass
    {
    public:
        AllocatedClass(Allocator* allocator);
        virtual ~AllocatedClass();

        Allocator* GetAllocator();
        void SetAllocator(Allocator* allocator);

        void* operator new(size_t byteCount, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION);
        void operator delete(void* mem);
        void operator delete(void* mem, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION);

        //Array operators intentionally omitted
        //void* operator new[](size_t byteCount, Allocator* allocator);
        //void operator delete[](void* mem);
        //void operator delete[](void* mem, Allocator* allocator);

        template <typename T, typename... Args>
        static T* New(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION, Args&&... args)
        {
            if (allocator == nullptr)
                allocator = Allocator::GetDefaultAllocator();
            auto p = new (allocator, FINJIN_CALLER_PARAMETERS) T(allocator, std::forward<Args>(args)...);
            assert(p != nullptr);
            return p;
        }

        template <typename T, typename... Args>
        static std::unique_ptr<T> NewUnique(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION, Args&&... args)
        {
            if (allocator == nullptr)
                allocator = Allocator::GetDefaultAllocator();
            std::unique_ptr<T> p(new (allocator, FINJIN_CALLER_PARAMETERS) T(allocator, std::forward<Args>(args)...));
            assert(p != nullptr);
            return p;
        }
        
    private:
        Allocator* allocator;
    };

} }
