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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class ReadCommandLineResult
    {
        SUCCESS,
        SHOW_USAGE,
        FAILURE
    };

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
        void Destroy();

        size_t GetCount() const;

        Utf8String& Consume(size_t index);

        const Utf8String& operator [] (size_t index) const;
        Utf8String& operator [] (size_t index);

    private:
        DynamicVector<Utf8String> items;
        Utf8String consumedValue;
    };

} }
