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
#include "finjin/common/DynamicVector.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    struct UsableDynamicVector
    {
        DynamicVector<T> items;
        DynamicVector<T*> usedItems;

        template <typename... Args>
        bool Create(size_t size, Allocator* allocator, Args&&... args)
        {
            auto result = true;
            result &= this->items.Create(size, allocator, std::forward<Args>(args)...);
            result &= this->usedItems.CreateEmpty(size, allocator);
            return result;
        }

        template <typename... Args>
        bool CreateEmpty(size_t size, Allocator* allocator, Args&&... args)
        {
            auto result = true;
            result &= this->items.CreateEmpty(size, allocator, std::forward<Args>(args)...);
            result &= this->usedItems.CreateEmpty(size, allocator);
            return result;
        }

        void Destroy()
        {
            this->items.Destroy();
            this->usedItems.Destroy();
        }

        void clear()
        {
            this->items.clear();
            this->usedItems.clear();
        }

        void ClearUsed()
        {
            this->usedItems.clear();
        }

        T* FindUnused()
        {
            for (auto& item : this->items)
            {
                if (std::find(this->usedItems.begin(), this->usedItems.end(), &item) == this->usedItems.end())
                    return &item;
            }
            return nullptr;
        }

        T* FindUsed(const T& value)
        {
            for (auto item : this->usedItems)
            {
                if (*item == value)
                    return item;
            }
            return nullptr;
        }

        bool IsUsed(const T& value) const
        {
            for (auto item : this->usedItems)
            {
                if (*item == value)
                    return true;
            }
            return false;
        }

        bool IsUsed(const T* value) const
        {
            for (auto item : this->usedItems)
            {
                if (item == value)
                    return true;
            }
            return false;
        }

        T* Use()
        {
            auto unusedItem = FindUnused();
            if (unusedItem != nullptr)
                this->usedItems.push_back(unusedItem);
            return unusedItem;
        }

        void Use(T* unusedItem)
        {
            this->usedItems.push_back(unusedItem);
        }

        void Unuse(T* usedItem)
        {
            auto iter = std::find(this->usedItems.begin(), this->usedItems.end(), usedItem);
            assert(iter != this->usedItems.end());
            if (iter != this->usedItems.end())
                this->usedItems.erase(iter);
        }
    };

    template <typename T>
    struct UsableDynamicVectorOfPointers
    {
        DynamicVector<T> items;
        DynamicVector<T> usedItems;

        bool Create(size_t size, Allocator* allocator)
        {
            auto result = true;
            result &= this->items.Create(size, allocator);
            result &= this->usedItems.CreateEmpty(size, allocator);
            return result;
        }

        bool CreateEmpty(size_t size, Allocator* allocator)
        {
            auto result = true;
            result &= this->items.CreateEmpty(size, allocator);
            result &= this->usedItems.CreateEmpty(size, allocator);
            return result;
        }

        void Destroy()
        {
            this->items.Destroy();
            this->usedItems.Destroy();
        }

        void clear()
        {
            this->items.clear();
            this->usedItems.clear();
        }

        void ClearUsed()
        {
            this->usedItems.clear();
        }

        T FindUnused()
        {
            for (auto item : this->items)
            {
                if (std::find(this->usedItems.begin(), this->usedItems.end(), item) == this->usedItems.end())
                    return item;
            }
            return nullptr;
        }

        T FindUsed(T value)
        {
            for (auto item : this->usedItems)
            {
                if (item == value)
                    return item;
            }
            return nullptr;
        }

        bool IsUsed(T value) const
        {
            for (auto item : this->usedItems)
            {
                if (item == value)
                    return true;
            }
            return false;
        }

        void Use(T unusedItem)
        {
            this->usedItems.push_back(unusedItem);
        }

        void Unuse(T usedItem)
        {
            auto iter = std::find(this->usedItems.begin(), this->usedItems.end(), usedItem);
            assert(iter != this->usedItems.end());
            if (iter != this->usedItems.end())
                this->usedItems.erase(iter);
        }
    };

} }
