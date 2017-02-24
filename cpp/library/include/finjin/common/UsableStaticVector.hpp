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
#include "finjin/common/StaticVector.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T, size_t maxCount>
    struct UsableStaticVector
    {
        std::array<T, maxCount> items;
        StaticVector<T*, maxCount> usedItems;

        T* FindUnused()
        {
            for (auto& item : this->items)
            {
                if (this->usedItems.find(&item) == this->usedItems.end())
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
            auto iter = this->usedItems.find(usedItem);
            assert(iter != this->usedItems.end());
            if (iter != this->usedItems.end())
                this->usedItems.erase(iter);
        }
    };

} }
