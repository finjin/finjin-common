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
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/Error.hpp"


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API CommandLineArgsProcessor : public AllocatedClass
    {
    public:
        CommandLineArgsProcessor(Allocator* allocator);
        CommandLineArgsProcessor(Allocator* allocator, int argc, char* argv[]);
        CommandLineArgsProcessor(Allocator* allocator, int argc, wchar_t* argv[]);

        CommandLineArgsProcessor(Allocator* allocator, const CommandLineArgsProcessor& other);
        CommandLineArgsProcessor(const CommandLineArgsProcessor& other) = delete;
        CommandLineArgsProcessor(CommandLineArgsProcessor&& other);

        ~CommandLineArgsProcessor();

        CommandLineArgsProcessor& operator = (const CommandLineArgsProcessor& other);
        CommandLineArgsProcessor& operator = (CommandLineArgsProcessor&& other);

        void Create(size_t count);

        size_t GetCount() const;
        
        Utf8String Consume(size_t i);
                
        const Utf8String& operator [] (size_t i) const;
        Utf8String& operator [] (size_t i);

    private:
        AllocatedVector<Utf8String> items;
    };

} }
