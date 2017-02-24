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

//TODO: Remove this

//Includes---------------------------------------------------------------------
#include "GeneralAllocator.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class FINJIN_COMMON_LIBRARY_API NumaStandardAllocator : public GeneralAllocator
    {        
    public:
        NumaStandardAllocator(const NumaStandardAllocator& other) = delete;
        NumaStandardAllocator& operator = (const NumaStandardAllocator& other) = delete;

        NumaStandardAllocator(NumaStandardAllocator&& NumaStandardAllocator) = delete;
        NumaStandardAllocator& operator = (NumaStandardAllocator&& other) = delete;

        size_t GetNodeBytesFree() const;

    public:
        NumaStandardAllocator();
        ~NumaStandardAllocator();

        size_t GetNodeID() const;
        void SetNodeID(size_t value);
        
    private:
        size_t nodeID;        
    };
} }
