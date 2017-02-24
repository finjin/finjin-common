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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/CommandLineArgsProcessor.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
CommandLineArgsProcessor::CommandLineArgsProcessor(Allocator* allocator) : AllocatedClass(allocator)
{    
}

CommandLineArgsProcessor::CommandLineArgsProcessor(Allocator* allocator, int argc, char* argv[]) : AllocatedClass(allocator)
{
    this->items.Create(argc - 1, allocator);
    for (int i = 1; i < argc; i++)
        this->items[i - 1] = argv[i];
}

CommandLineArgsProcessor::CommandLineArgsProcessor(Allocator* allocator, int argc, wchar_t* argv[]) : AllocatedClass(allocator)
{
    this->items.Create(argc - 1, allocator);
    for (int i = 1; i < argc; i++)
        this->items[i - 1] = argv[i];
}

CommandLineArgsProcessor::CommandLineArgsProcessor(Allocator* allocator, const CommandLineArgsProcessor& other) : AllocatedClass(allocator)
{
    if (!other.items.empty())
    {
        this->items.Create(other.items.size(), allocator);
        this->items.assign(other.items);
    }
}

CommandLineArgsProcessor::CommandLineArgsProcessor(CommandLineArgsProcessor&& other) : AllocatedClass(other.GetAllocator())
{
    this->items = std::move(other.items);
}

CommandLineArgsProcessor::~CommandLineArgsProcessor()
{    
}
    
CommandLineArgsProcessor& CommandLineArgsProcessor::operator = (const CommandLineArgsProcessor& other)
{
    if (this != &other)
    {
        if (other.items.empty())
            this->items.clear();
        else
        {
            Create(other.items.size());
            this->items.assign(other.items);
        }
    }

    return *this;
}

CommandLineArgsProcessor& CommandLineArgsProcessor::operator = (CommandLineArgsProcessor&& other)
{
    if (this != &other)
        this->items = std::move(other.items);
    
    return *this;
}

void CommandLineArgsProcessor::Create(size_t count)
{
    if (count > 0)
    {
        if (this->items.resize(count) < count)
            this->items.Create(count, GetAllocator());
    }
    else
        this->items.clear();
}

size_t CommandLineArgsProcessor::GetCount() const
{
    return this->items.size();
}

Utf8String CommandLineArgsProcessor::Consume(size_t i)
{
    auto value = this->items[i];
    this->items.erase(&this->items[i]);
    return value;
}

const Utf8String& CommandLineArgsProcessor::operator [] (size_t i) const
{
    return this->items[i];
}

Utf8String& CommandLineArgsProcessor::operator [] (size_t i)
{
    return this->items[i];
}
