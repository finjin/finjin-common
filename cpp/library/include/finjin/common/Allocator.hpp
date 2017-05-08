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
#include "finjin/common/MemoryArena.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Base class for type that allocates and frees memory.
     */
    class FINJIN_COMMON_LIBRARY_API Allocator
    {
    public:
        virtual ~Allocator() {}

        const Utf8String& GetName() const;
        ValueOrError<void> SetName(const Utf8String& name);

        ByteMemoryArena AllocateArena(const MemoryArenaSettings& settings, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            return AllocateArena(settings.byteCount, settings.alignment, FINJIN_CALLER_PARAMETERS);
        }

        ByteMemoryArena AllocateArena(size_t byteCount, size_t alignment, FINJIN_CALLER_PARAMETERS_DECLARATION)
        {
            if (alignment == 0)
                alignment = GetAlignment();

            ByteMemoryArena arena;

            if (alignment > GetAlignment())
                arena.rawMemory.size = UpgradeSizeForOffsetAlignment(byteCount, alignment);
            else
                arena.rawMemory.size = byteCount;
            arena.rawMemory.ptr = static_cast<uint8_t*>(Allocate(arena.rawMemory.size, FINJIN_CALLER_PARAMETERS));
            assert(arena.rawMemory.ptr != nullptr);
            arena.rawMemory.alignment = GetAlignment();

            arena.alignedMemory.ptr = AlignMemoryUp(arena.rawMemory.ptr, alignment);
            arena.alignedMemory.size = GetByteCountAfterMemoryAlignment(arena.rawMemory.size, arena.rawMemory.ptr, alignment);
            arena.alignedMemory.alignment = alignment;

            arena.deallocateMemory = [this](void* mem)
            {
                Deallocate(mem);
            };

            return arena;
        }

        /**
         * Allocates a chunk of memory of the specified number of bytes.
         * @param byteCount [in] - The number of bytes to allocate.
         * @return A pointer to the allocated memory is returned if it was possible
         * to allocate the specified number of bytes. Otherwise, null is returned.
         */
        virtual void* Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION) = 0;

        virtual bool CanDeallocateBlock() const = 0;

        /**
         * Frees the memory that was previously allocated by the allocator.
         * @param mem [in] - Pointer to memory that was previously allocated by the
         * allocator.
         */
        virtual void Deallocate(void* mem) = 0;

        virtual bool CanDeallocateAll() const = 0;

        /**
         * Frees all the memory allocated by the allocator.
         */
        virtual void DeallocateAll() = 0;

        /** Gets the number of bytes allocated. */
        virtual size_t GetBytesUsed() const = 0;

        /** Gets the number of bytes free. */
        virtual size_t GetBytesFree() const = 0;

        virtual size_t GetAlignment() const = 0;

        static size_t GetEnvironmentAlignment();
        static size_t GetDefaultAlignment();
        static size_t GetBestAlignment(size_t alignment);

        template <typename T>
        static T AlignMemoryDown(T location, size_t alignment)
        {
            //If necessary, aligns memory location downwards toward nearest aligned location
            static_assert(sizeof(T) <= sizeof(uintptr_t), "Unsupported memory type.");

            return T((uintptr_t)location & ~(alignment - 1));
        }

        template <typename T>
        static T AlignMemoryUp(T location, size_t alignment)
        {
            //If necessary, aligns memory location upwards toward nearest aligned location
            static_assert(sizeof(T) <= sizeof(uintptr_t), "Unsupported memory type.");

            return T(AlignSizeUp((uintptr_t)location, alignment));
        }

        template <typename T>
        static T AlignOffsetUp(T offset, size_t alignment)
        {
            //If necessary, aligns offset upwards toward nearest aligned offset
            return AlignSizeUp(offset, alignment);
        }

        template <typename T>
        static T AlignSizeUp(T size, size_t alignment)
        {
            //If necessary, aligns size upwards toward nearest aligned size
            assert(alignment != 0);
            assert((alignment & (alignment - 1)) == 0);
            return T((size + (alignment - 1)) & ~(alignment - 1));
        }

        template <typename T>
        static bool IsSizeAligned(T size, size_t alignment)
        {
            assert(alignment != 0);
            assert((alignment & (alignment - 1)) == 0);

            return (size & (alignment - 1)) == 0;
        }

        //Gets the minimum size required to ensure the specified size is available after aligning a memory start address
        //This only needs to be used when calling on an allocator that isn't known to align addresses in the desired way
        template <typename T>
        static T UpgradeSizeForOffsetAlignment(T size, size_t alignment)
        {
            assert(alignment != 0);
            assert((alignment & (alignment - 1)) == 0);

            return T(size + alignment - 1);
        }

        template <typename T>
        static size_t GetByteCountAfterMemoryAlignment(size_t byteCount, T location, size_t alignment)
        {
            static_assert(sizeof(T) <= sizeof(uintptr_t), "Unsupported memory type.");

            auto alignmentLoss = AlignOffsetUp((uintptr_t)location, alignment) - (uintptr_t)location;
            return byteCount - alignmentLoss;
        }

        template <typename T>
        static size_t GetByteCountAfterOffsetAlignment(size_t byteCount, T location, size_t alignment)
        {
            auto alignmentLoss = AlignOffsetUp(location, alignment) - location;
            return byteCount - alignmentLoss;
        }

        static Allocator* GetDefaultAllocator();
        static void SetDefaultAllocator(Allocator* allocator);

        static void* SystemAllocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION);
        static void SystemDeallocate(void* mem);

    private:
        Utf8String name;
    };

    /**
     * Wraps access to a Allocator, making it usable with std:: types that have allocators.
     *
     * For example:
     * AllocatorWrapper<Person> stdalloc(&allocator); //'allocator' is an instance of an Allocator class
     * std::vector<Person, AllocatorWrapper<Person>> people(stdalloc);
     * people.resize(3);
    */
    template <typename T>
    class AllocatorWrapper
    {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;

        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        AllocatorWrapper()
        {
            this->allocator = nullptr;
        }

        AllocatorWrapper(Allocator* allocator) throw()
        {
            this->allocator = allocator;
        }

        AllocatorWrapper(const AllocatorWrapper& other) throw()
        {
            this->allocator = other.allocator;
        }

        template <typename U>
        AllocatorWrapper(const AllocatorWrapper<U>& other) throw()
        {
            this->allocator = other.GetAllocator();
        }

        ~AllocatorWrapper() throw()
        {
        }

        template <typename U>
        struct rebind
        {
            using other = AllocatorWrapper<U>;
        };

        pointer address(reference value) const
        {
            return std::addressof(value);
        }

        const_pointer address(const_reference value) const
        {
            return std::addressof(value);
        }

        size_type max_size() const throw()
        {
            //Maximum number of elements that can be allocated
            return (size_t)-1 / sizeof(T);
        }

        pointer allocate(size_type num, const void* prev = nullptr)
        {
            assert(this->allocator != nullptr);

            //Allocate but don't initialize num elements of type T
            return static_cast<pointer>(this->allocator->Allocate(num * sizeof(T), FINJIN_CALLER_ARGUMENTS));
        }

        void destroy(pointer p)
        {
            if (p != nullptr)
                p->~T();
        }

        void deallocate(pointer p, size_type num)
        {
            assert(this->allocator != nullptr);

            this->allocator->Deallocate(p);
        }

        Allocator* GetAllocator() const
        {
            return this->allocator;
        }

        void SetAllocator(Allocator* allocator)
        {
            assert(this->allocator == nullptr);
            if (this->allocator == nullptr)
                this->allocator = allocator;
        }

    private:
        Allocator* allocator;
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T1, class T2>
    bool operator == (const AllocatorWrapper<T1>& a, const AllocatorWrapper<T2>& b) throw()
    {
        return a.GetAllocator() == b.GetAllocator();
    }

    template <typename T1, class T2>
    bool operator != (const AllocatorWrapper<T1>& a, const AllocatorWrapper<T2>& b) throw()
    {
        return a.GetAllocator() != b.GetAllocator();
    }

} }


//Macros------------------------------------------------------------------------
#define FINJIN_ALLOCATOR_NULL (Finjin::Common::Allocator*)0 //For readability/search-ability, prefer passing this into methods as a null pointer instead of 'nullptr'
