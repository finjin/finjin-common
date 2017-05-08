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
#include "finjin/common/WxByteBuffer.hpp"
#include "finjin/common/WxConfigDocumentReader.hpp"
#include "finjin/common/WxError.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    inline T AlignSizeUp(T size, T alignment)
    {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

    template <typename T>
    inline T AlignMemoryUp(T location, size_t alignment)
    {
        static_assert(sizeof(T) <= sizeof(uintptr_t), "Unsupported memory type.");

        return (T)AlignSizeUp((uintptr_t)location, alignment);
    }

    enum class WxNumericStructElementType
    {
        NONE,
        INT1, //32-bit int
        INT2,
        INT3,
        INT4,
        UINT1, //32-bit unsigned int
        UINT2,
        UINT3,
        UINT4,
        FLOAT1, //32-bit single precision floating point
        FLOAT2,
        FLOAT3,
        FLOAT4,
        FLOAT3x3,
        FLOAT4x4,
        NESTED_STRUCT //Pointer to another numeric struct
    };

    struct WxNumericStructElementTypeUtilities
    {
        static wxString ToString(WxNumericStructElementType value)
        {
            switch (value)
            {
                case WxNumericStructElementType::NONE: return wxT("<none>");
                case WxNumericStructElementType::INT1: return wxT("int1");
                case WxNumericStructElementType::INT2: return wxT("int2");
                case WxNumericStructElementType::INT3: return wxT("int3");
                case WxNumericStructElementType::INT4: return wxT("int4");
                case WxNumericStructElementType::UINT1: return wxT("uint1");
                case WxNumericStructElementType::UINT2: return wxT("uint2");
                case WxNumericStructElementType::UINT3: return wxT("uint3");
                case WxNumericStructElementType::UINT4: return wxT("uint4");
                case WxNumericStructElementType::FLOAT1: return wxT("float1");
                case WxNumericStructElementType::FLOAT2: return wxT("float2");
                case WxNumericStructElementType::FLOAT3: return wxT("float3");
                case WxNumericStructElementType::FLOAT4: return wxT("float4");
                case WxNumericStructElementType::FLOAT3x3: return wxT("float3x3");
                case WxNumericStructElementType::FLOAT4x4: return wxT("float4x4");
                default: return wxT("<unknown element type>");
            }
        }

        template <typename T>
        static void ParseSimpleType(WxNumericStructElementType& result, const T& value, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            result = ParseSimpleType(value);
            if (result == WxNumericStructElementType::NONE)
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse simple numeric struct element type '%1%'.", value));
        }

        template <typename T>
        static WxNumericStructElementType ParseSimpleType(const T& value)
        {
            if (value == wxT("int") || value == wxT("int1"))
                return WxNumericStructElementType::INT1;
            else if (value == wxT("int2"))
                return WxNumericStructElementType::INT2;
            else if (value == wxT("int3"))
                return WxNumericStructElementType::INT3;
            else if (value == wxT("int4"))
                return WxNumericStructElementType::INT4;
            else if (value == wxT("uint") || value == wxT("uint1"))
                return WxNumericStructElementType::UINT1;
            else if (value == wxT("uint2"))
                return WxNumericStructElementType::UINT2;
            else if (value == wxT("uint3"))
                return WxNumericStructElementType::UINT3;
            else if (value == wxT("uint4"))
                return WxNumericStructElementType::UINT4;
            else if (value == wxT("float") || value == wxT("float1"))
                return WxNumericStructElementType::FLOAT1;
            else if (value == wxT("float2"))
                return WxNumericStructElementType::FLOAT2;
            else if (value == wxT("float3"))
                return WxNumericStructElementType::FLOAT3;
            else if (value == wxT("float4"))
                return WxNumericStructElementType::FLOAT4;
            else if (value == wxT("float3x3"))
                return WxNumericStructElementType::FLOAT3x3;
            else if (value == wxT("float4x4"))
                return WxNumericStructElementType::FLOAT4x4;
            else
                return WxNumericStructElementType::NONE;
        }

        static bool IsInt32(WxNumericStructElementType type)
        {
            switch (type)
            {
                case WxNumericStructElementType::INT1:
                case WxNumericStructElementType::INT2:
                case WxNumericStructElementType::INT3:
                case WxNumericStructElementType::INT4:
                case WxNumericStructElementType::UINT1:
                case WxNumericStructElementType::UINT2:
                case WxNumericStructElementType::UINT3:
                case WxNumericStructElementType::UINT4: return true;
                default: return false;
            }
        }

        static bool IsFloat(WxNumericStructElementType type)
        {
            switch (type)
            {
                case WxNumericStructElementType::FLOAT1:
                case WxNumericStructElementType::FLOAT2:
                case WxNumericStructElementType::FLOAT3:
                case WxNumericStructElementType::FLOAT4:
                case WxNumericStructElementType::FLOAT3x3:
                case WxNumericStructElementType::FLOAT4x4: return true;
                default: return false;
            }
        }

        static size_t GetSimpleTypeSizeInElements(WxNumericStructElementType type)
        {
            switch (type)
            {
                case WxNumericStructElementType::NONE: return 0;
                case WxNumericStructElementType::INT1: return 1;
                case WxNumericStructElementType::INT2: return 2;
                case WxNumericStructElementType::INT3: return 3;
                case WxNumericStructElementType::INT4: return 4;
                case WxNumericStructElementType::UINT1: return 1;
                case WxNumericStructElementType::UINT2: return 2;
                case WxNumericStructElementType::UINT3: return 3;
                case WxNumericStructElementType::UINT4: return 4;
                case WxNumericStructElementType::FLOAT1: return 1;
                case WxNumericStructElementType::FLOAT2: return 2;
                case WxNumericStructElementType::FLOAT3: return 3;
                case WxNumericStructElementType::FLOAT4: return 4;
                case WxNumericStructElementType::FLOAT3x3: return 9;
                case WxNumericStructElementType::FLOAT4x4: return 16;
                default: return 0;
            }
        }

        static size_t GetSimpleTypeSizeInBytes(WxNumericStructElementType type)
        {
            switch (type)
            {
                case WxNumericStructElementType::NONE: return 0;
                case WxNumericStructElementType::INT1: return sizeof(int32_t);
                case WxNumericStructElementType::INT2: return sizeof(int32_t) * 2;
                case WxNumericStructElementType::INT3: return sizeof(int32_t) * 3;
                case WxNumericStructElementType::INT4: return sizeof(int32_t) * 4;
                case WxNumericStructElementType::UINT1: return sizeof(uint32_t);
                case WxNumericStructElementType::UINT2: return sizeof(uint32_t) * 2;
                case WxNumericStructElementType::UINT3: return sizeof(uint32_t) * 3;
                case WxNumericStructElementType::UINT4: return sizeof(uint32_t) * 4;
                case WxNumericStructElementType::FLOAT1: return sizeof(float);
                case WxNumericStructElementType::FLOAT2: return sizeof(float) * 2;
                case WxNumericStructElementType::FLOAT3: return sizeof(float) * 3;
                case WxNumericStructElementType::FLOAT4: return sizeof(float) * 4;
                case WxNumericStructElementType::FLOAT3x3: return sizeof(float) * 9;
                case WxNumericStructElementType::FLOAT4x4: return sizeof(float) * 16;
                default: return 0;
            }
        }
    };

    template <typename WxNumericStruct>
    struct WxNumericStructElement
    {
        typedef typename WxNumericStruct::ElementID ElementID;

        typedef WxNumericStructElementType Type;

        enum class DefaultValue
        {
            NONE,
            ZERO,
            ONE,
            IDENTITY_MATRIX
        };

        WxNumericStructElement()
        {
            Reset();
        }

        void Reset()
        {
            this->elementID = ElementID::NONE;
            this->arraySize = 1;
            this->sizeInBytes = 0;
            this->strideInBytes = 0;
            this->gpuPaddedOffset = 0;
            this->type = Type::NONE;
            this->nestedStruct = nullptr;
            this->defaultValue = DefaultValue::NONE;
            this->packArray = false;
        }

        bool IsInt() const
        {
            return WxNumericStructElementTypeUtilities::IsInt(this->type);
        }

        bool IsFloat() const
        {
            return WxNumericStructElementTypeUtilities::IsFloat(this->type);
        }

        size_t _GetTypeSizeInBytes() const
        {
            auto& element = *this;

            auto result = WxNumericStructElementTypeUtilities::GetSimpleTypeSizeInBytes(element.type);
            if (result == 0 && element.type == WxNumericStructElementType::NESTED_STRUCT)
            {
                if (element.nestedStruct != nullptr)
                    result = element.nestedStruct->paddedTotalSize;
            }
            return result;
        }

        bool operator == (const WxNumericStructElement& other) const
        {
            return
                this->elementID == other.elementID &&
                this->arraySize == other.arraySize &&
                this->sizeInBytes == other.sizeInBytes &&
                this->strideInBytes == other.strideInBytes &&
                this->gpuPaddedOffset == other.gpuPaddedOffset &&
                this->type == other.type &&
                WxNumericStruct::Equal(this->nestedStruct, other.nestedStruct) &&
                this->defaultValue == other.defaultValue &&
                this->packArray == other.packArray
                ;
        }

        bool operator != (const WxNumericStructElement& other) const
        {
            return !(operator == (other));
        }

        template <typename A, typename B> //A and B should be some type of vector
        static bool ElementsSame(const A& a, const B& b)
        {
            if (a.size() != b.size())
                return false;

            for (size_t i = 0; i < a.size(); i++)
            {
                if (a[i] != b[i])
                    return false;
            }

            return true;
        }

        bool IsSubsetOf(const WxNumericStructElement& other) const
        {
            if (this->elementID != other.elementID)
                return false;
            if (this->arraySize != other.arraySize)
                return false;
            if (this->type != other.type)
                return false;
            if ((this->nestedStruct == nullptr && other.nestedStruct != nullptr) || (this->nestedStruct != nullptr && other.nestedStruct == nullptr))
                return false;
            if (this->nestedStruct != nullptr)
                return this->nestedStruct->IsSubsetOf(other.nestedStruct); //other.nestedStruct is non-null, as checked a few lines up

            return true;
        }

        ElementID elementID; //Element identifer. ElementID::NONE indicates no known identifier, in which case the name can be used to identify elements
        size_t arraySize; //Number of items in the array. 1 = not an array, >1 = array
        size_t sizeInBytes; //Size of a single element in the array. Total unpadded size is arraySize * sizeInBytes
        size_t strideInBytes; //Distance between elements in the array. If not an array, strideInBytes = sizeInBytes. Total padded/real size is arraySize * strideInBytes
        size_t gpuPaddedOffset; //Padded/real byte offset from the beginning of the buffer
        Type type; //The element type
        WxNumericStruct* nestedStruct; //Pointer to a nested type, if type = Type::NESTED_STRUCT
        DefaultValue defaultValue; //Default value setting
        bool packArray; //Indicates whether array elements should be packed together tightly. By default they are not, which matches typical GPU memory alignment behavior, Packing can be useful to make a configured array map to consecutive non-array values
    };

    template <typename NumericStructMetadata>
    class WxNumericStruct
    {
    public:
        typedef typename NumericStructMetadata::ElementID ElementID;

        typedef WxNumericStructElement<WxNumericStruct> Element;

        struct Defines
        {
            enum { NONE = 0 };
        };

        WxNumericStruct()
        {
            this->memoryStartAddressAlignment = 0;
            this->memoryRowSizeInBytes = 0;
            this->memoryTotalSizeAlignment = 0;
            this->paddedTotalSize = 0;
            this->zeroAllElementDefaultValues = false;
        }

        const wxString& GetTypeName() const
        {
            return this->typeName;
        }

        template <typename NumericStructVector>
        static void Initialize
            (
            NumericStructVector& numericStructs,
            const WxByteBuffer& readBuffer,
            size_t memoryStartAddressAlignment,
            size_t memoryRowSizeInBytes,
            size_t memoryTotalSizeAlignment,
            uint32_t defines,
            WxError& error
            )
        {
            FINJIN_WX_ERROR_METHOD_START(error);

            WxConfigDocumentReader reader;
            reader.Start(readBuffer);

            Initialize(numericStructs, reader, memoryStartAddressAlignment, memoryRowSizeInBytes, memoryTotalSizeAlignment, defines, error);
            if (error)
                FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
        }

        template <typename NumericStructVector>
        static void Initialize
            (
            NumericStructVector& numericStructs,
            WxConfigDocumentReader& reader,
            size_t memoryStartAddressAlignment,
            size_t memoryRowSizeInBytes,
            size_t memoryTotalSizeAlignment,
            uint32_t defines,
            WxError& error
            )
        {
            FINJIN_WX_ERROR_METHOD_START(error);

            if (reader.Current() == nullptr)
            {
                FINJIN_WX_SET_ERROR(error, wxT("The specified reader ended unexpectedly."));
                return;
            }
            auto startLine = *reader.Current();

            auto structCount = reader.GetSectionCount(NumericStructMetadata::GetConfigSectionName());
            numericStructs.resize(structCount);
            if (numericStructs.size() < structCount)
            {
                FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Unable to allocate '%d' numeric structs. Only '%d' were allocated."), structCount, numericStructs.size()));
                return;
            }

            if (!numericStructs.empty())
            {
                structCount = 0;

                for (auto line = reader.Current(); line != nullptr; line = reader.Next())
                {
                    switch (line->GetType())
                    {
                        case WxConfigDocumentLine::Type::SECTION:
                        {
                            wxString sectionName;
                            line->GetSectionName(sectionName);

                            if (sectionName == NumericStructMetadata::GetConfigSectionName() && structCount < numericStructs.size())
                            {
                                auto& desc = numericStructs[structCount++];

                                InitializeFromScope
                                    (
                                    desc,
                                    reader,
                                    memoryStartAddressAlignment,
                                    memoryRowSizeInBytes,
                                    memoryTotalSizeAlignment,
                                    &numericStructs[0],
                                    structCount - 1,
                                    defines,
                                    error
                                    );
                                if (error)
                                {
                                    FINJIN_WX_SET_ERROR(error, wxT("Failed to read buffer numericStruct."));
                                    return;
                                }
                            }

                            break;
                        }
                        default: break;
                    }
                }
            }

            reader.Restart(startLine);
        }

        static void InitializeFromScope
            (
            WxNumericStruct& desc,
            WxConfigDocumentReader& reader,
            size_t memoryStartAddressAlignment,
            size_t memoryRowSizeInBytes,
            size_t memoryTotalSizeAlignment,
            WxNumericStruct* otherStructs,
            size_t otherStructCount,
            uint32_t defines,
            WxError& error
            )
        {
            FINJIN_WX_ERROR_METHOD_START(error);

            desc.memoryStartAddressAlignment = memoryStartAddressAlignment;
            desc.memoryRowSizeInBytes = memoryRowSizeInBytes;
            desc.memoryTotalSizeAlignment = memoryTotalSizeAlignment;
            desc.zeroAllElementDefaultValues = true; //Assume all can be zeroed

            auto elementCount = reader.GetSectionCount(NumericStructMetadata::GetConfigElementSectionName());
            desc.elements.resize(elementCount);
            if (desc.elements.size() < elementCount)
            {
                FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Unable to allocate '%d' elements. Only '%d' were allocated."), elementCount, desc.elements.size()));
                return;
            }

            if (!desc.elements.empty())
            {
                elementCount = 0;

                int depth = 0;
                auto descriptionDone = false;

                wxString sectionName;
                wxString key, value;

                for (auto line = reader.Next(); line != nullptr && !descriptionDone; )
                {
                    switch (line->GetType())
                    {
                        case WxConfigDocumentLine::Type::SECTION:
                        {
                            line->GetSectionName(sectionName);

                            if (sectionName == NumericStructMetadata::GetConfigElementSectionName() && elementCount < desc.elements.size())
                            {
                                auto& item = desc.elements[elementCount];

                                uint32_t ifDefines = 0;

                                ReadItemFromScope(reader, item, otherStructs, otherStructCount, ifDefines, error);
                                if (error)
                                {
                                    FINJIN_WX_SET_ERROR(error, wxT("Failed to read buffer item."));
                                    return;
                                }

                                if (ifDefines == 0 || (defines & ifDefines))
                                {
                                    elementCount++;

                                    if (item.defaultValue != Element::DefaultValue::NONE && item.defaultValue != Element::DefaultValue::ZERO)
                                        desc.zeroAllElementDefaultValues = false;
                                }
                            }
                            else
                                reader.SkipScope();

                            break;
                        }
                        case WxConfigDocumentLine::Type::KEY_AND_VALUE:
                        {
                            line->GetKeyAndValue(key, value);

                            if (key == wxT("type"))
                                desc.typeName = value;

                            break;
                        }
                        case WxConfigDocumentLine::Type::SCOPE_START:
                        {
                            depth++;
                            break;
                        }
                        case WxConfigDocumentLine::Type::SCOPE_END:
                        {
                            depth--;
                            if (depth == 0)
                                descriptionDone = true;
                            break;
                        }
                        default: break;
                    }

                    if (!descriptionDone)
                        line = reader.Next();
                }

                desc.elements.resize(elementCount);
            }

            _InitializeDescription(desc, memoryStartAddressAlignment, memoryRowSizeInBytes, memoryTotalSizeAlignment);
        }

        template <typename NumericStructVector>
        static WxNumericStruct* GetByTypeName(NumericStructVector& numericStructs, const wxString& type)
        {
            for (auto& desc : numericStructs)
            {
                if (desc.typeName == type)
                    return &desc;
            }

            return nullptr;
        }

        const Element* GetElement(ElementID elementID) const
        {
            return this->elementIDToElement[elementID];
        }

        const Element* GetElement(size_t index) const
        {
            return &this->elements[index];
        }

        bool operator == (const WxNumericStruct& other) const
        {
            if (this->elements.size() != other.elements.size())
                return false;

            for (size_t i = 0; i < this->elements.size(); i++)
            {
                auto& element = this->elements[i];
                auto& otherElement = other.elements[i];

                if (element != otherElement)
                    return false;
            }

            return true;
        }

        bool operator != (const WxNumericStruct& other) const
        {
            return !(operator == (other));
        }

        bool IsSubsetOf(const WxNumericStruct& other) const
        {
            for (auto& element : this->elements)
            {
                auto otherElement = other.GetElement(element.elementID);

                if (otherElement == nullptr)
                    return false;

                if (!element.IsSubsetOf(*otherElement))
                    return false;
            }

            return true;
        }

        static bool Equal(WxNumericStruct* a, WxNumericStruct* b)
        {
            if (a == b)
                return true;
            else if (a != nullptr && b != nullptr)
                return *a == *b;
            else
                return false;
        }

    private:
        static void ReadItemFromScope
            (
            WxConfigDocumentReader& reader,
            Element& element,
            WxNumericStruct* otherStructs,
            size_t otherStructCount,
            uint32_t& ifDefines,
            WxError& error
            )
        {
            FINJIN_WX_ERROR_METHOD_START(error);

            element.Reset();

            wxString key, value;

            auto depth = 0;

            for (auto line = reader.Next(); line != nullptr; line = reader.Next())
            {
                switch (line->GetType())
                {
                    case WxConfigDocumentLine::Type::KEY_AND_VALUE:
                    {
                        line->GetKeyAndValue(key, value);

                        if (key == wxT("id"))
                        {
                            NumericStructMetadata::ParseElementID(element.elementID, value, error);
                            if (error)
                            {
                                FINJIN_WX_SET_ERROR_NO_MESSAGE(error);
                                return;
                            }
                        }
                        else if (key == wxT("type"))
                        {
                            element.type = WxNumericStructElementTypeUtilities::ParseSimpleType(value);
                            if (element.type == WxNumericStructElementType::NONE)
                            {
                                for (size_t i = 0; i < otherStructCount; i++)
                                {
                                    if (value == otherStructs[i].typeName)
                                    {
                                        element.type = WxNumericStructElementType::NESTED_STRUCT;
                                        element.nestedStruct = &otherStructs[i];
                                        break;
                                    }
                                }
                                if (element.type == WxNumericStructElementType::NONE)
                                {
                                    FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to parse item type '%s'."), value.wx_str()));
                                    return;
                                }
                            }
                        }
                        else if (key == wxT("array-size"))
                        {
                            unsigned long long arraySize;
                            value.ToULongLong(&arraySize);
                            element.arraySize = static_cast<size_t>(arraySize);
                            if (element.arraySize == 0)
                                element.arraySize = 1;
                        }
                        else if (key == wxT("pack-array"))
                        {
                            element.packArray = value != wxT("f");
                        }
                        else if (key == wxT("ifdef"))
                        {
                            ifDefines = NumericStructMetadata::ParseIfdef(value);
                        }
                        else if (key == wxT("default-value"))
                        {
                            if (value == wxT("0"))
                                element.defaultValue = Element::DefaultValue::ZERO;
                            else if (value == wxT("1"))
                                element.defaultValue = Element::DefaultValue::ONE;
                            else if (value == wxT("identity-matrix"))
                                element.defaultValue = Element::DefaultValue::IDENTITY_MATRIX;
                            else
                            {
                                FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to parse default value '%s'."), value.wx_str()));
                                return;
                            }
                        }
                        break;
                    }
                    case WxConfigDocumentLine::Type::SCOPE_START:
                    {
                        depth++;
                        break;
                    }
                    case WxConfigDocumentLine::Type::SCOPE_END:
                    {
                        depth--;
                        if (depth == 0)
                            return;
                        break;
                    }
                    default: break;
                }
            }
        }

        static void _InitializeDescription
            (
            WxNumericStruct& desc,
            size_t memoryStartAddressAlignment,
            size_t memoryRowSizeInBytes,
            size_t memoryTotalSizeAlignment
            )
        {
            memset(&desc.elementIDToElement[0], 0, desc.elementIDToElement.size() * sizeof(void*));
            desc.paddedTotalSize = 0;
            if (!desc.elements.empty())
            {
                auto previousElementType = WxNumericStructElementType::NONE;
                size_t paddedRowElementOffset = 0;
                for (size_t elementIndex = 0; elementIndex < desc.elements.size(); elementIndex++)
                {
                    auto& element = desc.elements[elementIndex];

                    element.sizeInBytes = element._GetTypeSizeInBytes();

                    if (element.arraySize == 1)
                        element.strideInBytes = element.sizeInBytes;
                    else
                        element.strideInBytes = AlignSizeUp(element.sizeInBytes, desc.memoryRowSizeInBytes);

                    //Add all the array items
                    for (size_t arrayItemIndex = 0; arrayItemIndex < element.arraySize; arrayItemIndex++)
                    {
                        if (paddedRowElementOffset != 0)
                        {
                            if ((element.arraySize > 1 && !element.packArray) || //Non-packed array
                                ((paddedRowElementOffset + element.sizeInBytes) > desc.memoryRowSizeInBytes) || //Element spans multiple rows
                                (element.type == WxNumericStructElementType::NESTED_STRUCT) || //Element is a nested struct
                                (previousElementType == WxNumericStructElementType::NESTED_STRUCT)) //Previous element is a nested struct
                            {
                                //Element should be at start of row
                                desc.paddedTotalSize += desc.memoryRowSizeInBytes - paddedRowElementOffset;
                                paddedRowElementOffset = 0;
                            }
                        }

                        //Assign to current row
                        element.gpuPaddedOffset = desc.paddedTotalSize;

                        //Adjust running total
                        desc.paddedTotalSize += element.strideInBytes;
                        paddedRowElementOffset = (paddedRowElementOffset + element.strideInBytes) % desc.memoryRowSizeInBytes;

                        //Update 'previous' type
                        previousElementType = element.type;
                    }
                }

                //Round up total size
                desc.paddedTotalSize = AlignSizeUp(desc.paddedTotalSize, std::max(memoryRowSizeInBytes, memoryTotalSizeAlignment));

                //Build elementID->element lookup
                for (auto& element : desc.elements)
                {
                    if (element.elementID != ElementID::NONE)
                        desc.elementIDToElement[element.elementID] = &element;
                }
            }
        }

    public:
        wxString typeName; //The internal name of the type. Used when looking up nested types
        StaticVector<Element, 128> elements;
        size_t memoryStartAddressAlignment; //Start address alignment
        size_t memoryRowSizeInBytes; //Element size alignment
        size_t memoryTotalSizeAlignment; //Alignment on the total size of the buffer
        size_t paddedTotalSize;
        bool zeroAllElementDefaultValues; //Indicates whether all elements can be zeroed during a call to GpuConstantBuffer::SetDefaults()
        EnumArray<ElementID, ElementID::COUNT, Element*> elementIDToElement; //Look up table to quickly map a elementID to an element
    };

    struct GenericNumericStructMetadata
    {
        enum class ElementID
        {
            NONE,

            COUNT
        };

        static void ParseElementID(ElementID& result, const wxString& value, WxError& error)
        {
            result = ElementID::NONE;
        }

        static const wxString& GetConfigSectionName()
        {
            static const wxString name(wxT(""));
            return name;
        }

        static const wxString& GetConfigElementSectionName()
        {
            static const wxString name(wxT(""));
            return name;
        }

        static uint32_t ParseIfdef(const wxString& value)
        {
            return 0;
        }
    };
    typedef WxNumericStruct<GenericNumericStructMetadata> GenericNumericStruct;

    //For testing----------------------------------------------------------------------------
    class WxInputFormatStructMetadata
    {
    public:
        static const wxString& GetConfigSectionName()
        {
            static const wxString name(wxT("input-format"));
            return name;
        }

        static const wxString& GetConfigElementSectionName()
        {
            static const wxString name(wxT("element"));
            return name;
        }

        static uint32_t ParseIfdef(const wxString& value)
        {
            return 0;
        }
    };

    //Generic vertex format struct
    class WxGpuVertexFormatStructMetadata : public WxInputFormatStructMetadata
    {
    public:
        enum class ElementID
        {
            NONE,

            BINORMAL, //float4
            BLEND_INDICES_0, //uint
            BLEND_INDICES_1, //uint
            BLEND_INDICES_2, //uint
            BLEND_INDICES_3, //uint
            BLEND_WEIGHT_0, //float
            BLEND_WEIGHT_1, //float
            BLEND_WEIGHT_2, //float
            BLEND_WEIGHT_3, //float
            COLOR_0, //float4
            COLOR_1, //float4
            COLOR_2, //float4
            COLOR_3, //float4
            NORMAL, //float4
            POSITION, //float4
            POSITION_TRANSFORMED, //float4
            POINT_SIZE, //float
            TANGENT, //float4
            TEX_COORD_0, //float4
            TEX_COORD_1, //float4
            TEX_COORD_2, //float4
            TEX_COORD_3, //float4
            TEX_COORD_4, //float4
            TEX_COORD_5, //float4
            TEX_COORD_6, //float4
            TEX_COORD_7, //float4

            COUNT
        };

        static wxString ElementIDToString(ElementID value)
        {
            switch (value)
            {
                case ElementID::BINORMAL: return wxT("binormal");
                case ElementID::BLEND_INDICES_0: return wxT("blend-indices-0");
                case ElementID::BLEND_INDICES_1: return wxT("blend-indices-1");
                case ElementID::BLEND_INDICES_2: return wxT("blend-indices-2");
                case ElementID::BLEND_INDICES_3: return wxT("blend-indices-3");
                case ElementID::BLEND_WEIGHT_0: return wxT("blend-weight-0");
                case ElementID::BLEND_WEIGHT_1: return wxT("blend-weight-1");
                case ElementID::BLEND_WEIGHT_2: return wxT("blend-weight-2");
                case ElementID::BLEND_WEIGHT_3: return wxT("blend-weight-3");
                case ElementID::COLOR_0: return wxT("color-0");
                case ElementID::COLOR_1: return wxT("color-1");
                case ElementID::COLOR_2: return wxT("color-2");
                case ElementID::COLOR_3: return wxT("color-3");
                case ElementID::NORMAL: return wxT("normal");
                case ElementID::POSITION: return wxT("position");
                case ElementID::POSITION_TRANSFORMED: return wxT("positioned-transformed");
                case ElementID::POINT_SIZE: return wxT("point-size");
                case ElementID::TANGENT: return wxT("tangent");
                case ElementID::TEX_COORD_0: return wxT("tex-coord-0");
                case ElementID::TEX_COORD_1: return wxT("tex-coord-1");
                case ElementID::TEX_COORD_2: return wxT("tex-coord-2");
                case ElementID::TEX_COORD_3: return wxT("tex-coord-3");
                case ElementID::TEX_COORD_4: return wxT("tex-coord-4");
                case ElementID::TEX_COORD_5: return wxT("tex-coord-5");
                case ElementID::TEX_COORD_6: return wxT("tex-coord-6");
                case ElementID::TEX_COORD_7: return wxT("tex-coord-7");
                default: return wxEmptyString;
            }
        }

        static void ParseElementID(ElementID& result, const wxString& value, WxError& error)
        {
            FINJIN_WX_ERROR_METHOD_START(error);

            if (value == wxT("binormal"))
                result = ElementID::BINORMAL;
            else if (value == wxT("blend-indices-0"))
                result = ElementID::BLEND_INDICES_0;
            else if (value == wxT("blend-indices-1"))
                result = ElementID::BLEND_INDICES_1;
            else if (value == wxT("blend-indices-2"))
                result = ElementID::BLEND_INDICES_2;
            else if (value == wxT("blend-indices-3"))
                result = ElementID::BLEND_INDICES_3;
            else if (value == wxT("blend-weight-0"))
                result = ElementID::BLEND_WEIGHT_0;
            else if (value == wxT("blend-weight-1"))
                result = ElementID::BLEND_WEIGHT_1;
            else if (value == wxT("blend-weight-2"))
                result = ElementID::BLEND_WEIGHT_2;
            else if (value == wxT("blend-weight-3"))
                result = ElementID::BLEND_WEIGHT_3;
            else if (value == wxT("color") || value == wxT("color-0"))
                result = ElementID::COLOR_0;
            else if (value == wxT("color-1"))
                result = ElementID::COLOR_1;
            else if (value == wxT("color-2"))
                result = ElementID::COLOR_2;
            else if (value == wxT("color-3"))
                result = ElementID::COLOR_3;
            else if (value == wxT("normal"))
                result = ElementID::NORMAL;
            else if (value == wxT("position"))
                result = ElementID::POSITION;
            else if (value == wxT("position-transformed"))
                result = ElementID::POSITION_TRANSFORMED;
            else if (value == wxT("point-size"))
                result = ElementID::POINT_SIZE;
            else if (value == wxT("tangent"))
                result = ElementID::TANGENT;
            else if (value == wxT("tex-coord") || value == wxT("tex-coord-0"))
                result = ElementID::TEX_COORD_0;
            else if (value == wxT("tex-coord-1"))
                result = ElementID::TEX_COORD_1;
            else if (value == wxT("tex-coord-2"))
                result = ElementID::TEX_COORD_2;
            else if (value == wxT("tex-coord-3"))
                result = ElementID::TEX_COORD_3;
            else if (value == wxT("tex-coord-4"))
                result = ElementID::TEX_COORD_4;
            else if (value == wxT("tex-coord-5"))
                result = ElementID::TEX_COORD_5;
            else if (value == wxT("tex-coord-6"))
                result = ElementID::TEX_COORD_6;
            else if (value == wxT("tex-coord-7"))
                result = ElementID::TEX_COORD_7;
            else
            {
                FINJIN_WX_SET_ERROR(error, wxString::Format(wxT("Failed to parse vertex element ID '%s'."), value.wx_str()));
                return;
            }
        }
    };
    typedef WxNumericStruct<WxGpuVertexFormatStructMetadata> WxGpuVertexFormatStruct;
    //typedef WxNumericStructInstance<WxGpuVertexFormatStruct> WxGpuVertexFormatStructInstance;

    struct WxGpuVertexFormatStructUtilities
    {
        static WxGpuVertexFormatStruct::Element CreateElement(WxGpuVertexFormatStruct::Element::ElementID elementID, WxNumericStructElementType type)
        {
            WxGpuVertexFormatStruct::Element element;
            element.elementID = elementID;
            element.type = type;
            return element;
        }

        static WxGpuVertexFormatStruct::Element CreateTextureCoordinate(size_t textureCoordinateSetIndex, size_t componentCount)
        {
            WxGpuVertexFormatStruct::Element element;
            element.elementID = static_cast<WxGpuVertexFormatStruct::Element::ElementID>((size_t)WxGpuVertexFormatStructMetadata::ElementID::TEX_COORD_0 + textureCoordinateSetIndex);
            element.type = static_cast<WxNumericStructElementType>((size_t)WxNumericStructElementType::FLOAT1 + componentCount - 1);
            return element;
        }
    };

} }
