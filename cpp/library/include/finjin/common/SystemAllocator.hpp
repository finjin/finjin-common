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
#include "finjin/common/Allocator.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * Allocator that passes calls through to the underlying system.
     * Since it doesn't internally keep track of allocations, the values returned by
     * GetBytesUsed() and GetBytesFree() will be somewhat unstable from call to call.
     */
    class SystemAllocator : public Allocator
    {
    public:
        SystemAllocator();
        ~SystemAllocator();

        void* Allocate(size_t byteCount, FINJIN_CALLER_PARAMETERS_DECLARATION) override;

        bool CanDeallocateBlock() const override;
        void Deallocate(void* mem) override;

        bool CanDeallocateAll() const override;
        void DeallocateAll() override;

        size_t GetBytesUsed() const override;
        size_t GetBytesFree() const override;
        size_t GetAlignment() const override;
    };

} }
