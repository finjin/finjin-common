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
#include "finjin/common/JsonDocument.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/JsonDocumentImpl.hpp"
#include "finjin/common/Utf8StringFormatter.hpp"
#include <queue>

using namespace Finjin::Common;


//Local types-------------------------------------------------------------------
struct JsonDocumentMember::Impl
{
    JsonElement* member;
};


//Implementation----------------------------------------------------------------

//JsonDocumentMember
JsonDocumentMember::JsonDocumentMember() : impl(new Impl)
{
}

JsonDocumentMember::~JsonDocumentMember()
{
    delete impl;
}

bool JsonDocumentMember::IsObject() const
{
    return impl->member->IsObject();
}

bool JsonDocumentMember::IsString() const
{
    return impl->member->IsString();
}

Utf8String JsonDocumentMember::GetString() const
{
    return impl->member->GetString();
}

bool JsonDocumentMember::IsArray() const
{
    return impl->member->IsArray();
}

size_t JsonDocumentMember::GetSize() const
{
    return impl->member->Size();
}

std::shared_ptr<JsonDocumentMember> JsonDocumentMember::GetItem(size_t index) const
{
    std::shared_ptr<JsonDocumentMember> member;

    if (index < impl->member->Size())
    {
        member.reset(new JsonDocumentMember);
        member->impl->member = &(*impl->member)[rapidjson::SizeType(index)];
    }

    return member;
}

bool JsonDocumentMember::IsNumber() const
{
    return impl->member->IsNumber();
}

double JsonDocumentMember::GetDouble() const
{
    return impl->member->GetDouble();
}

int JsonDocumentMember::GetInt() const
{
    return impl->member->GetInt();
}

bool JsonDocumentMember::IsBool() const
{
    return impl->member->IsBool();
}

bool JsonDocumentMember::GetBool() const
{
    return impl->member->GetBool();
}

bool JsonDocumentMember::HasMember(const Utf8String& name) const
{
    return impl->member->HasMember(name.c_str());
}

std::shared_ptr<JsonDocumentMember> JsonDocumentMember::GetMember(const Utf8String& name) const
{
    std::shared_ptr<JsonDocumentMember> member;

    if (impl->member->HasMember(name.c_str()))
    {
        member.reset(new JsonDocumentMember);
        member->impl->member = &(*impl->member)[name.c_str()];
    }

    return member;
}

std::shared_ptr<JsonDocumentMember> JsonDocument::FindMemberWithChildMemberNameAndValue(const Utf8String& name, const Utf8String& value) const
{
    std::shared_ptr<JsonDocumentMember> result;

    std::queue<JsonElement*> memberQueue;
    memberQueue.push(&impl->doc);

    while (!memberQueue.empty())
    {
        auto currentMember = memberQueue.front();
        memberQueue.pop();

        if (currentMember->IsObject())
        {
            //It's an object. It might have what we're looking for
            auto childMember = currentMember->FindMember(name.c_str());
            if (childMember != currentMember->MemberEnd() && childMember->value.IsString() && value == childMember->value.GetString())
            {
                //Found it
                result.reset(new JsonDocumentMember);
                result->impl->member = currentMember;
                break;
            }

            //Not found. Enqueue all children
            for (childMember = currentMember->MemberBegin(); childMember != currentMember->MemberEnd(); ++childMember)
                memberQueue.push(&(*currentMember)[childMember->name]);
        }
        else if (currentMember->IsArray())
        {
            //It's an array. Enqueue elements of the array
            for (rapidjson::SizeType i = 0; i < currentMember->Size(); i++)
                memberQueue.push(&(*currentMember)[i]);
        }
    }

    return result;
}

int JsonDocumentMember::GetMemberCount() const
{
    return impl->member->MemberCount();
}

//JsonDocument
JsonDocument::JsonDocument() : impl(new JsonDocumentImpl)
{
}

JsonDocument::~JsonDocument()
{
    delete impl;
}

JsonDocument& JsonDocument::SetObject()
{
    impl->doc.SetObject();
    return *this;
}

bool JsonDocument::HasMember(const Utf8String& name) const
{
    return impl->doc.HasMember(name.c_str());
}

JsonDocument& JsonDocument::AddMember(const Utf8String& name, const Utf8String& value)
{
    rapidjson::Document::AllocatorType& allocator = impl->doc.GetAllocator();

    JsonValue jsonName(name.c_str(), allocator);
    JsonValue jsonValue(value.c_str(), allocator);
    impl->doc.AddMember(jsonName, jsonValue, allocator);

    return *this;
}

JsonDocument& JsonDocument::RemoveMember(const Utf8String& name)
{
    impl->doc.RemoveMember(name.c_str());
    return *this;
}

std::shared_ptr<JsonDocumentMember> JsonDocument::GetMember(const Utf8String& name) const
{
    std::shared_ptr<JsonDocumentMember> member;

    if (impl->doc.HasMember(name.c_str()))
    {
        member.reset(new JsonDocumentMember);
        member->impl->member = &impl->doc[name.c_str()];
    }

    return member;
}

int JsonDocument::GetMemberCount() const
{
    return impl->doc.MemberCount();
}

Utf8String JsonDocument::GetStringMember(const Utf8String& name) const
{
    if (impl->doc.HasMember(name.c_str()))
        return impl->doc[name.c_str()].GetString();
    else
        return Utf8String::GetEmpty();
}

void JsonDocument::Parse(const Utf8String& text, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Parse(text);
    if (HasParseError())
        FINJIN_SET_ERROR(error, GetParseError());
}

JsonDocument& JsonDocument::Parse(const Utf8String& text)
{
    impl->doc.Parse<0>(text.c_str());
    return *this;
}

bool JsonDocument::HasParseError() const
{
    return impl->doc.HasParseError();
}

Utf8String JsonDocument::GetParseError() const
{
    if (impl->doc.HasParseError())
        return Utf8StringFormatter::Format("JSON parse error: %1% (%2%)", GetParseError_En(impl->doc.GetParseError()), impl->doc.GetErrorOffset());
    else
        return Utf8String::GetEmpty();
}

Utf8String JsonDocument::ToString() const
{
    JsonStringBuffer buffer;
    JsonStringWriter writer(buffer);
    impl->doc.Accept(writer);
    return buffer.GetString();
}

JsonDocumentImpl* JsonDocument::GetImpl()
{
    return impl;
}
