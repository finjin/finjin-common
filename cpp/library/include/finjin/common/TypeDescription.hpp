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

//Abstract base type
/** Goes into the class declaration of a base type */
#define FINJIN_DECLARE_ABSTRACT_BASE_TYPE_DESCRIPTION(class) \
    using This = class; \
    virtual Finjin::Common::TypeDescription& GetTypeDescription() { return ClassGetTypeDescription(); } \
    static Finjin::Common::TypeDescription& ClassGetTypeDescription(); \
    virtual bool IsTypeOf(const Finjin::Common::TypeDescription& other) const { return ClassGetTypeDescription().IsTypeOf(other); } \
    virtual bool IsSubTypeOf(const Finjin::Common::TypeDescription& other) const { return ClassGetTypeDescription().IsSubTypeOf(other); } \
    virtual void* Upcast(const Finjin::Common::TypeDescription& other) \
    { \
        if (&ClassGetTypeDescription() == &other) \
            return this; \
        else \
            return nullptr; \
    }

/** Goes into the type implementation file for a base type */
#define FINJIN_IMPLEMENT_ABSTRACT_BASE_TYPE_DESCRIPTION(class, typeName) \
    Finjin::Common::TypeDescription& class::ClassGetTypeDescription() \
    { \
        static BaseTypeDescription<class> typeDescription(typeName); \
        return typeDescription; \
    } \
    static const Finjin::Common::TypeDescription& class##StaticInit = FINJIN_TYPE_DESCRIPTION(class);


//Concrete base type
/** Goes into the class declaration of a base type */
#define FINJIN_DECLARE_BASE_TYPE_DESCRIPTION(class) FINJIN_DECLARE_ABSTRACT_BASE_TYPE_DESCRIPTION(class)

/** Goes into the type implementation file for a base type */
#define _FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, factory, typeName) \
    Finjin::Common::TypeDescription& class::ClassGetTypeDescription() \
    { \
        static ConcreteTypeDescription<class, factory<class> > typeDescription(typeName); \
        return typeDescription; \
    } \
    static const Finjin::Common::TypeDescription& class##StaticInit = FINJIN_TYPE_DESCRIPTION(class);

#define FINJIN_IMPLEMENT_BASE_TYPE_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, DefaultNewFactory, typeName)
#define FINJIN_IMPLEMENT_ALLOCATED_BASE_TYPE_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_BASE_CLASS_DESCRIPTION(class, AllocatedClassNewFactory, typeName)


//Abstract derived type
/** Goes into the class declaration of a derived abstract type */
#define FINJIN_DECLARE_ABSTRACT_TYPE_DESCRIPTION(class, superType) \
    using This = class; \
    using Super = superType; \
    virtual Finjin::Common::TypeDescription& GetTypeDescription() override { return ClassGetTypeDescription(); } \
    static Finjin::Common::TypeDescription& ClassGetTypeDescription(); \
    virtual bool IsTypeOf(const Finjin::Common::TypeDescription& other) const override { return ClassGetTypeDescription().IsTypeOf(other); } \
    virtual bool IsSubTypeOf(const Finjin::Common::TypeDescription& other) const override { return ClassGetTypeDescription().IsSubTypeOf(other); } \
    virtual void* Upcast(const Finjin::Common::TypeDescription& other) override \
    { \
        if (&ClassGetTypeDescription() == &other) \
            return this; \
        else if (superType::ClassGetTypeDescription().IsTypeOf(other)) \
            return static_cast<superType*>(this); \
        else \
            return nullptr; \
    }

/** Goes into the type implementation file for a derived abstract type */
#define FINJIN_IMPLEMENT_ABSTRACT_TYPE_DESCRIPTION(class, typeName) \
    Finjin::Common::TypeDescription& class::ClassGetTypeDescription() \
    { \
        static AbstractTypeDescription<class> typeDescription(typeName, &Super::ClassGetTypeDescription()); \
        return typeDescription; \
    } \
    static const Finjin::Common::TypeDescription& class##StaticInit = FINJIN_TYPE_DESCRIPTION(class);


//Concrete dervied type
/** Goes into the class declaration of a derived type */
#define FINJIN_DECLARE_TYPE_DESCRIPTION(class, superType) \
    using This = class; \
    using Super = superType; \
    virtual Finjin::Common::TypeDescription& GetTypeDescription() override { return ClassGetTypeDescription(); }\
    static Finjin::Common::TypeDescription& ClassGetTypeDescription();\
    virtual bool IsTypeOf(const Finjin::Common::TypeDescription& other) const override { return ClassGetTypeDescription().IsTypeOf(other); } \
    virtual bool IsSubTypeOf(const Finjin::Common::TypeDescription& other) const override { return ClassGetTypeDescription().IsSubTypeOf(other); } \
    virtual void* Upcast(const Finjin::Common::TypeDescription& other) override \
    { \
        if (&ClassGetTypeDescription() == &other) \
            return this; \
        else if (superType::ClassGetTypeDescription().IsTypeOf(other)) \
            return static_cast<superType*>(this); \
        else \
            return nullptr; \
    }

/** Goes into the type implementation file for a derived type */
#define _FINJIN_IMPLEMENT_TYPE_DESCRIPTION(class, factory, typeName) \
    Finjin::Common::TypeDescription& class::ClassGetTypeDescription() \
    { \
        static ConcreteTypeDescription<class, factory<class> > typeDescription(typeName, &Super::ClassGetTypeDescription()); \
        return typeDescription; \
    } \
    static const Finjin::Common::TypeDescription& class##StaticInit = FINJIN_TYPE_DESCRIPTION(class);

#define FINJIN_IMPLEMENT_TYPE_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_TYPE_DESCRIPTION(class, Finjin::Common::DefaultNewFactory, typeName)
#define FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(class, typeName) _FINJIN_IMPLEMENT_TYPE_DESCRIPTION(class, Finjin::Common::AllocatedClassNewFactory, typeName)


//Concrete derived type
/**
 * Goes into the class declaration of a derived type. This version has two parent classes
 * The first parent type can be thought of as the main parent type.
 */
#define FINJIN_DECLARE_CLASS_DESCRIPTION2(class, superType, superType2) \
    using This = class; \
    using Super = superType; \
    using Super1 = superType; \
    using Super2 = superType2; \
    virtual Finjin::Common::TypeDescription& GetTypeDescription() override { return ClassGetTypeDescription(); }\
    static Finjin::Common::TypeDescription& ClassGetTypeDescription();\
    virtual bool IsTypeOf(const Finjin::Common::TypeDescription& other) const override { return ClassGetTypeDescription().IsTypeOf(other); } \
    virtual bool IsSubTypeOf(const Finjin::Common::TypeDescription& other) const override { return ClassGetTypeDescription().IsSubTypeOf(other); } \
    virtual void* Upcast(const Finjin::Common::TypeDescription& other) override \
    {\
        if (&ClassGetTypeDescription() == &other) \
            return this; \
        else if (superType::ClassGetTypeDescription().IsTypeOf(other)) \
            return static_cast<superType*>(this); \
        else if (superType2::ClassGetTypeDescription().IsTypeOf(other)) \
            return static_cast<superType2*>(this); \
        else \
            return nullptr; \
    }

/** Goes into the type implementation file for a derived type */
#define _FINJIN_IMPLEMENT_TYPE_DESCRIPTION2(class, factory, typeName) \
    Finjin::Common::TypeDescription& class::ClassGetTypeDescription() \
    { \
        static ConcreteTypeDescription<class, factory<class> > typeDescription(typeName, &Super1::ClassGetTypeDescription(), &Super2::ClassGetTypeDescription()); \
        return typeDescription; \
    } \
    static const Finjin::Common::TypeDescription& class##StaticInit = FINJIN_TYPE_DESCRIPTION(class);

#define FINJIN_IMPLEMENT_TYPE_DESCRIPTION2(class, typeName) _FINJIN_IMPLEMENT_TYPE_DESCRIPTION2(class, DefaultNewFactory, typeName)
#define FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION2(class, typeName) _FINJIN_IMPLEMENT_TYPE_DESCRIPTION2(class, AllocatedClassNewFactory, typeName)

/** Gets the type description for the specified type. Use of this macro is preferable to calling the type method. */
#define FINJIN_TYPE_DESCRIPTION(class) (class::ClassGetTypeDescription())


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Type information for a class.
     * This information allows classes to be dynamically created by name, ensuring that the created instances are of the appropriate type.
     */
    class FINJIN_COMMON_LIBRARY_API TypeDescription
    {
    public:
        TypeDescription(const TypeDescription& other) = delete;
        TypeDescription& operator = (const TypeDescription& other) = delete;

        TypeDescription(TypeDescription&& other) = delete;
        TypeDescription& operator = (TypeDescription&& other) = delete;

    public:
        TypeDescription(const char* typeName, TypeDescription* superType = nullptr, TypeDescription* superType2 = nullptr);
        virtual ~TypeDescription();

        static TypeDescription** GetAll(size_t* count = nullptr);

        /**
         * Gets the name of the type.
         * @return The name of the type.
         */
        const Utf8String& GetName() const;

        /**
         * Determines whether the type is equal to or a subtype of the other type.
         * @param other [in] The other type.
         * @return Whether this class is equal to or a subtype of the other type.
         */
        bool IsTypeOf(const TypeDescription& other) const;

        /**
         * Determines whether this class is a strict subtype of the other type.
         * @param other [in] The other type.
         * @return Whether this class is a strict subtype of the other type.
         */
        bool IsSubTypeOf(const TypeDescription& other) const;

        /**
         * Determines whether this class is a root class.
         * @return Whether this class is a root class.
         */
        bool IsRoot() const;

        /**
         * Finds a type description with the specified name. All root type descriptions are searched.
         * @param typeName [in] The name of the type description to find.
         * @return A pointer to the type description is returned if it could be found. Otherwise, nullptr is returned.
         */
        static TypeDescription* Find(const Utf8String& typeName);

        /**
         * Indicates whether the the type is abstract - whether it has pure virtual methods and cannot be instantiated.
         * @return Whether the the type is abstract.
         */
        virtual bool IsAbstract() const;

        /**
         * Creates an instance of the type description.
         * @return Pointer to an instance of the type description. To destroy this instance, call DestroyInstance().
         */
        virtual void* CreateInstance(Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION) const;

        /**
         * Destroys an instance of the type description.
         * @param genericInstance [in] Pointer to an instance of the type description.
         */
        virtual void DestroyInstance(void* genericInstance) const;

        /**
         * Upcasts the specified instance to the specified type description.
         * @param genericInstance [in] Pointer to an instance of the type description.
         * @param typeDescription [in] The type description to upcast to.
         * @return If the instance could be upcast, a pointer to the upcast instance is returned. Otherwise, nullptr is returned.
         */
        virtual void* UpcastInstance(void* genericInstance, TypeDescription& typeDescription) const;

    private:
        Utf8String typeName;

    public:
        TypeDescription* superTypeHead;
        TypeDescription* superTypeNext;

        TypeDescription* subTypeHead;
        TypeDescription* subTypeNext;
    };

    template <typename ClassType>
    class BaseTypeDescription : public TypeDescription
    {
    public:
        BaseTypeDescription(const char* typeName) : TypeDescription(typeName) {}

        void DestroyInstance(void* genericInstance) const override
        {
            delete static_cast<ClassType*>(genericInstance);
        }

        void* UpcastInstance(void* genericInstance, TypeDescription& typeDescription) const override
        {
            auto instance = static_cast<ClassType*>(genericInstance);
            return instance->Upcast(typeDescription);
        }
    };

    template <typename ClassType>
    class AbstractTypeDescription : public TypeDescription
    {
    public:
        AbstractTypeDescription(const char* typeName, TypeDescription* superType) : TypeDescription(typeName, superType) {}

        void DestroyInstance(void* genericInstance) const override
        {
            delete static_cast<ClassType*>(genericInstance);
        }

        void* UpcastInstance(void* genericInstance, TypeDescription& typeDescription) const override
        {
            auto instance = static_cast<ClassType*>(genericInstance);
            return instance->Upcast(typeDescription);
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
    class ConcreteTypeDescription : public TypeDescription
    {
    public:
        ConcreteTypeDescription(const char* typeName, TypeDescription* superType = nullptr, TypeDescription* superType2 = nullptr) : TypeDescription(typeName, superType, superType2) {}

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

        void* UpcastInstance(void* genericInstance, TypeDescription& typeDescription) const override
        {
            auto instance = static_cast<ClassType*>(genericInstance);
            return instance->Upcast(typeDescription);
        }
    };

    /** Can be used with std::unique_ptr to use the TypeDescription for an object to delete that object. */
    template <typename ClassType>
    struct TypeDescriptionDeleter
    {
        using result_type = void;

        void operator () (ClassType* obj) const
        {
            auto& typeDescription = obj->GetTypeDescription();
            typeDescription.DestroyInstance(obj);
        }
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    TypeDescription* GetAndValidateClass(const Utf8String& nameOfTypeToCreate, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        if (nameOfTypeToCreate.empty())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Cannot create an instance of '%1%' since no type name was specified.", FINJIN_TYPE_DESCRIPTION(T).GetName()));
            return nullptr;
        }

        auto typeToCreate = TypeDescription::Find(nameOfTypeToCreate);

        if (typeToCreate == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Type '%1%' does not exist.", nameOfTypeToCreate));
            return nullptr;
        }

        if (!typeToCreate->IsTypeOf(FINJIN_TYPE_DESCRIPTION(T)))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Type '%1%' is not an instance of '%2%'.", nameOfTypeToCreate, FINJIN_TYPE_DESCRIPTION(T).GetName()));
            return nullptr;
        }

        if (typeToCreate->IsAbstract())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Type '%1%' is abstract and cannot be instantiated.", nameOfTypeToCreate));
            return nullptr;
        }

        return typeToCreate;
    }

    /**
     * Creates an instance of the specified type.
     * @param typeToCreate [in] type description of the type to create.
     * @return If the specified type is either the same class or a subtype of the specified template parameter, and that type
     * is instantiable, an instance of that type will be returned. Otherwise, nullptr will be returned.
     */
    template <typename T>
    T* CreateInstanceOf(TypeDescription& typeToCreate, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION)
    {
        T* upcastInstance = nullptr;

        void* genericInstance = typeToCreate.CreateInstance(allocator, FINJIN_CALLER_PARAMETERS);
        if (genericInstance != nullptr)
        {
            upcastInstance = static_cast<T*>(typeToCreate.UpcastInstance(genericInstance, FINJIN_TYPE_DESCRIPTION(T)));
            if (upcastInstance == nullptr)
                typeToCreate.DestroyInstance(genericInstance);
        }

        return upcastInstance;
    }

    /**
     * Creates an instance of the specified type.
     * @param typeToCreate [in] type description of the type to create.
     * @param error [out] Error object.
     * @return If the specified type is either the same class or a subtype of the specified template parameter, and that type
     * is instantiable, an instance of that type will be returned. Otherwise, nullptr will be returned.
     * @see Error
     */
    template <typename T>
    T* CreateInstanceOf(const Utf8String& nameOfTypeToCreate, Allocator* allocator, FINJIN_CALLER_PARAMETERS_DECLARATION, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        auto typeToCreate = GetAndValidateClass<T>(nameOfTypeToCreate, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return nullptr;
        }

        return CreateInstanceOf<T>(*typeToCreate, allocator, FINJIN_CALLER_PARAMETERS);
    }

} }
