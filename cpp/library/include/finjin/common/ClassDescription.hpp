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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"


//Macros------------------------------------------------------------------------

//Abstract base case
/** Goes into the class declaration of a base class */
#define FINJIN_DECLARE_ABSTRACT_BASE_CLASS_DESCRIPTION(class) \
    using This = class; \
    virtual Finjin::Common::ClassDescription& GetClassDescription() { return ClassGetClassDescription(); } \
    static Finjin::Common::ClassDescription& ClassGetClassDescription(); \
    virtual bool IsTypeOf(const Finjin::Common::ClassDescription& other) const { return ClassGetClassDescription().IsTypeOf(other); } \
    virtual bool IsSubclassOf(const Finjin::Common::ClassDescription& other) const { return ClassGetClassDescription().IsSubclassOf(other); } \
    virtual void* Upcast(const Finjin::Common::ClassDescription& other) \
    { \
        if (&ClassGetClassDescription() == &other) \
            return this; \
        else \
            return nullptr; \
    }

/** Goes into the class implementation file for a base class */
#define FINJIN_IMPLEMENT_ABSTRACT_BASE_CLASS_DESCRIPTION(class, typeName) \
    Finjin::Common::ClassDescription& class::ClassGetClassDescription() \
    { \
        static BaseClassDescription<class> classDescription(typeName); \
        return classDescription; \
    } \
    static const Finjin::Common::ClassDescription& class##StaticInit = FINJIN_CLASS_DESCRIPTION(class);


//Concrete base class
/** Goes into the class declaration of a base class */
#define FINJIN_DECLARE_BASE_CLASS_DESCRIPTION(class) FINJIN_DECLARE_ABSTRACT_BASE_CLASS_DESCRIPTION(class)

/** Goes into the class implementation file for a base class */
#define _FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, classFactory, typeName) \
    Finjin::Common::ClassDescription& class::ClassGetClassDescription() \
    { \
        static ConcreteClassDescription<class, classFactory<class> > classDescription(typeName); \
        return classDescription; \
    } \
    static const Finjin::Common::ClassDescription& class##StaticInit = FINJIN_CLASS_DESCRIPTION(class);

#define FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, DefaultNewFactory, typeName)
#define FINJIN_IMPLEMENT_ALLOCATED_BASE_CLASS_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, AllocatedClassNewFactory, typeName)


//Abstract derived class
/** Goes into the class declaration of a derived abstract class */
#define FINJIN_DECLARE_ABSTRACT_CLASS_DESCRIPTION(class, parentClass) \
    using This = class; \
    using Super = parentClass; \
    virtual Finjin::Common::ClassDescription& GetClassDescription() override { return ClassGetClassDescription(); } \
    static Finjin::Common::ClassDescription& ClassGetClassDescription(); \
    virtual bool IsTypeOf(const Finjin::Common::ClassDescription& other) const override { return ClassGetClassDescription().IsTypeOf(other); } \
    virtual bool IsSubclassOf(const Finjin::Common::ClassDescription& other) const override { return ClassGetClassDescription().IsSubclassOf(other); } \
    virtual void* Upcast(const Finjin::Common::ClassDescription& other) override \
    { \
        if (&ClassGetClassDescription() == &other) \
            return this; \
        else if (parentClass::ClassGetClassDescription().IsTypeOf(other)) \
            return static_cast<parentClass*>(this); \
        else \
            return nullptr; \
    }

/** Goes into the class implementation file for a derived abstract class */
#define FINJIN_IMPLEMENT_ABSTRACT_CLASS_DESCRIPTION(class, typeName) \
    Finjin::Common::ClassDescription& class::ClassGetClassDescription() \
    { \
        static AbstractClassDescription<class> classDescription(typeName, &Super::ClassGetClassDescription()); \
        return classDescription; \
    } \
    static const Finjin::Common::ClassDescription& class##StaticInit = FINJIN_CLASS_DESCRIPTION(class);


//Concrete dervied class
/** Goes into the class declaration of a derived class */
#define FINJIN_DECLARE_CLASS_DESCRIPTION(class, parentClass) \
    using This = class; \
    using Super = parentClass; \
    virtual Finjin::Common::ClassDescription& GetClassDescription() override { return ClassGetClassDescription(); }\
    static Finjin::Common::ClassDescription& ClassGetClassDescription();\
    virtual bool IsTypeOf(const Finjin::Common::ClassDescription& other) const override { return ClassGetClassDescription().IsTypeOf(other); } \
    virtual bool IsSubclassOf(const Finjin::Common::ClassDescription& other) const override { return ClassGetClassDescription().IsSubclassOf(other); } \
    virtual void* Upcast(const Finjin::Common::ClassDescription& other) override \
    { \
        if (&ClassGetClassDescription() == &other) \
            return this; \
        else if (parentClass::ClassGetClassDescription().IsTypeOf(other)) \
            return static_cast<parentClass*>(this); \
        else \
            return nullptr; \
    }

/** Goes into the class implementation file for a derived class */
#define _FINJIN_IMPLEMENT_CLASS_DESCRIPTION(class, classFactory, typeName) \
    Finjin::Common::ClassDescription& class::ClassGetClassDescription() \
    { \
        static ConcreteClassDescription<class, classFactory<class> > classDescription(typeName, &Super::ClassGetClassDescription()); \
        return classDescription; \
    } \
    static const Finjin::Common::ClassDescription& class##StaticInit = FINJIN_CLASS_DESCRIPTION(class);

#define FINJIN_IMPLEMENT_CLASS_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_CLASS_DESCRIPTION(class, Finjin::Common::DefaultNewFactory, typeName)
#define FINJIN_IMPLEMENT_ALLOCATED_CLASS_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_CLASS_DESCRIPTION(class, Finjin::Common::AllocatedClassNewFactory, typeName)


//Concrete derived class
/** 
 * Goes into the class declaration of a derived class. This version has two parent classes 
 * The first parent class can be thought of as the main parent class.
 */
#define FINJIN_DECLARE_CLASS_DESCRIPTION2(class, parentClass, parentClass2) \
    using This = class; \
    using Super = parentClass; \
    using Super1 = parentClass; \
    using Super2 = parentClass2; \
    virtual Finjin::Common::ClassDescription& GetClassDescription() override { return ClassGetClassDescription(); }\
    static Finjin::Common::ClassDescription& ClassGetClassDescription();\
    virtual bool IsTypeOf(const Finjin::Common::ClassDescription& other) const override { return ClassGetClassDescription().IsTypeOf(other); } \
    virtual bool IsSubclassOf(const Finjin::Common::ClassDescription& other) const override { return ClassGetClassDescription().IsSubclassOf(other); } \
    virtual void* Upcast(const Finjin::Common::ClassDescription& other) override \
    {\
        if (&ClassGetClassDescription() == &other) \
            return this; \
        else if (parentClass::ClassGetClassDescription().IsTypeOf(other)) \
            return static_cast<parentClass*>(this); \
        else if (parentClass2::ClassGetClassDescription().IsTypeOf(other)) \
            return static_cast<parentClass2*>(this); \
        else \
            return nullptr; \
    }

/** Goes into the class implementation file for a derived class */
#define _FINJIN_IMPLEMENT_CLASS_DESCRIPTION2(class, classFactory, typeName) \
    Finjin::Common::ClassDescription& class::ClassGetClassDescription() \
    { \
        static ConcreteClassDescription<class, classFactory<class> > classDescription(typeName, &Super1::ClassGetClassDescription(), &Super2::ClassGetClassDescription()); \
        return classDescription; \
    } \
    static const Finjin::Common::ClassDescription& class##StaticInit = FINJIN_CLASS_DESCRIPTION(class);

#define FINJIN_IMPLEMENT_CLASS_DESCRIPTION2(class, typeName) _FINJIN_IMPLEMENT_CLASS_DESCRIPTION2(class, DefaultNewFactory, typeName)
#define FINJIN_IMPLEMENT_ALLOCATED_CLASS_DESCRIPTION2(class, typeName) _FINJIN_IMPLEMENT_CLASS_DESCRIPTION2(class, AllocatedClassNewFactory, typeName)

/** Gets the class description for the specified class. Use of this macro is preferable to calling the class method. */
#define FINJIN_CLASS_DESCRIPTION(class) (class::ClassGetClassDescription())


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Type information for a class. 
     * This information allows classes to be dynamically created by name, ensuring that the created instances are of the appropriate type.
     */
    class FINJIN_COMMON_LIBRARY_API ClassDescription
    {
    public:
        ClassDescription(const ClassDescription& other) = delete;
        ClassDescription& operator = (const ClassDescription& other) = delete;

        ClassDescription(ClassDescription&& other) = delete;
        ClassDescription& operator = (ClassDescription&& other) = delete;

    public:
        ClassDescription(const char* typeName, ClassDescription* parentClass = nullptr, ClassDescription* parentClass2 = nullptr);
        virtual ~ClassDescription();

        static ClassDescription** GetAll(size_t* count = nullptr);

        /** 
         * Gets the name of the class.
         * @return The name of the class.
         */
        const Utf8String& GetName() const;

        /**
         * Determines whether the class is equal to or a subclass of the other class.
         * @param other [in] The other class.
         * @return Whether this class is equal to or a subclass of the other class.
         */
        bool IsTypeOf(const ClassDescription& other) const;
        
        /**
         * Determines whether this class is a strict subclass of the other class.
         * @param other [in] The other class.
         * @return Whether this class is a strict subclass of the other class.
         */
        bool IsSubclassOf(const ClassDescription& other) const;

        /** 
         * Determines whether this class is a root class.
         * @return Whether this class is a root class.
         */
        bool IsRoot() const;

        /**
         * Finds a class description with the specified name. All root class descriptions are searched.
         * @param className [in] The name of the class description to find.
         * @return A pointer to the class description is returned if it could be found. Otherwise, nullptr is returned.
         */
        static ClassDescription* Find(const Utf8String& className);

        /** 
         * Indicates whether the the class is abstract - whether it has pure virtual methods and cannot be instantiated. 
         * @return Whether the the class is abstract.
         */
        virtual bool IsAbstract() const;
        
        /** 
         * Creates an instance of the class description.
         * @return Pointer to an instance of the class description. To destroy this instance, call DestroyInstance().
         */
        virtual void* CreateInstance(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION) const;

        /**
         * Destroys an instance of the class description.
         * @param genericInstance [in] Pointer to an instance of the class description.
         */
        virtual void DestroyInstance(void* genericInstance) const;
        
        /** 
         * Upcasts the specified instance to the specified class description.
         * @param genericInstance [in] Pointer to an instance of the class description.
         * @param classDescription [in] The class description to upcast to.
         * @return If the instance could be upcast, a pointer to the upcast instance is returned. Otherwise, nullptr is returned.
         */
        virtual void* UpcastInstance(void* genericInstance, ClassDescription& classDescription) const;
        
    private:
        Utf8String typeName;

    public:
        ClassDescription* parentClassHead;
        ClassDescription* parentClassNext;

        ClassDescription* subclassHead;
        ClassDescription* subclassNext;
    };

    template <typename ClassType>
    class BaseClassDescription : public ClassDescription
    {
    public:
        BaseClassDescription(const char* typeName) : ClassDescription(typeName) {}
        
        void DestroyInstance(void* genericInstance) const override 
        {
            delete static_cast<ClassType*>(genericInstance);
        }
        
        void* UpcastInstance(void* genericInstance, ClassDescription& classDescription) const override
        {
            auto instance = static_cast<ClassType*>(genericInstance);
            return instance->Upcast(classDescription);
        }
    };
    
    template <typename ClassType>
    class AbstractClassDescription : public ClassDescription
    {
    public:
        AbstractClassDescription(const char* typeName, ClassDescription* parentClass) : ClassDescription(typeName, parentClass) {}
        
        void DestroyInstance(void* genericInstance) const override 
        {
            delete static_cast<ClassType*>(genericInstance);
        }
        
        void* UpcastInstance(void* genericInstance, ClassDescription& classDescription) const override
        {
            auto instance = static_cast<ClassType*>(genericInstance);
            return instance->Upcast(classDescription);
        }
    };
    
    template <typename T>
    struct DefaultNewFactory
    {
        T* operator () (Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            return new T;
        }
    };

    template <typename T>
    struct AllocatedClassNewFactory
    {
        T* operator () (Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            return AllocatedClass::New<T>(allocator, FINJIN_CALLER_PARAMETERS);
        }
    };

    template <typename ClassType, typename InstanceFactory = DefaultNewFactory<ClassType> >
    class ConcreteClassDescription : public ClassDescription
    {
    public:
        ConcreteClassDescription(const char* typeName, ClassDescription* parentClass = nullptr, ClassDescription* parentClass2 = nullptr) : ClassDescription(typeName, parentClass, parentClass2) {}

        bool IsAbstract() const override 
        {
            return false;
        }
        
        void* CreateInstance(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION) const override
        {
            InstanceFactory factory;
            return factory(allocator, FINJIN_CALLER_PARAMETERS);
        }

        void DestroyInstance(void* genericInstance) const override 
        {
            delete static_cast<ClassType*>(genericInstance);
        }
        
        void* UpcastInstance(void* genericInstance, ClassDescription& classDescription) const override
        {
            auto instance = static_cast<ClassType*>(genericInstance);
            return instance->Upcast(classDescription);
        }
    };

    /** Can be used with std::unique_ptr to use the ClassDescription for an object to delete that object. */
    template <typename ClassType>
    struct ClassDescriptionDeleter
    {
        using result_type = void;
    
        void operator () (ClassType* obj) const
        {
            auto& classDescription = obj->GetClassDescription();
            classDescription.DestroyInstance(obj);            
        }
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    ClassDescription* GetAndValidateClass(const Utf8String& nameOfClassToCreate, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        if (nameOfClassToCreate.empty())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Cannot create an instance of '%1%' since no class name was specified.", FINJIN_CLASS_DESCRIPTION(T).GetName()));
            return nullptr;
        }

        auto classToCreate = ClassDescription::Find(nameOfClassToCreate);

        if (classToCreate == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Class '%1%' does not exist.", nameOfClassToCreate));
            return nullptr;
        }

        if (!classToCreate->IsTypeOf(FINJIN_CLASS_DESCRIPTION(T)))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Class '%1%' is not an instance of '%2%'.", nameOfClassToCreate, FINJIN_CLASS_DESCRIPTION(T).GetName()));
            return nullptr;
        }

        if (classToCreate->IsAbstract())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Class '%1%' is abstract and cannot be instantiated.", nameOfClassToCreate));
            return nullptr;
        }

        return classToCreate;
    }

    /**
     * Creates an instance of the specified class.
     * @param classToCreate [in] Class description of the class to create.
     * @return If the specified class is either the same class or a subclass of the specified template parameter, and that class
     * is instantiable, an instance of that class will be returned. Otherwise, nullptr will be returned. 
     */
    template <typename T>
    T* CreateInstanceOf(ClassDescription& classToCreate, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION)
    {
        T* upcastInstance = nullptr;
        
        void* genericInstance = classToCreate.CreateInstance(allocator, FINJIN_CALLER_PARAMETERS);
        if (genericInstance != nullptr)
        {
            upcastInstance = static_cast<T*>(classToCreate.UpcastInstance(genericInstance, FINJIN_CLASS_DESCRIPTION(T)));
            if (upcastInstance == nullptr)
                classToCreate.DestroyInstance(genericInstance);
        }
        
        return upcastInstance;
    }

    /**
     * Creates an instance of the specified class.
     * @param classToCreate [in] Class description of the class to create.
     * @param error [out] Error object.
     * @return If the specified class is either the same class or a subclass of the specified template parameter, and that class
     * is instantiable, an instance of that class will be returned. Otherwise, nullptr will be returned.
     * @see Error
     */
    template <typename T>
    T* CreateInstanceOf(const Utf8String& nameOfClassToCreate, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        auto classToCreate = GetAndValidateClass<T>(nameOfClassToCreate, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return nullptr;
        }
        
        return CreateInstanceOf<T>(*classToCreate, allocator, FINJIN_CALLER_PARAMETERS);
    }

} }
