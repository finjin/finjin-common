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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    enum class NumericStructElementType
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

    class NumericStructElementTypeUtilities
    {
    public:
        static Utf8String ToString(NumericStructElementType value)
        {
            switch (value)
            {
                case NumericStructElementType::NONE: return "<none>";
                case NumericStructElementType::INT1: return "int1";
                case NumericStructElementType::INT2: return "int2";
                case NumericStructElementType::INT3: return "int3";
                case NumericStructElementType::INT4: return "int4";
                case NumericStructElementType::UINT1: return "uint1";
                case NumericStructElementType::UINT2: return "uint2";
                case NumericStructElementType::UINT3: return "uint3";
                case NumericStructElementType::UINT4: return "uint4";
                case NumericStructElementType::FLOAT1: return "float1";
                case NumericStructElementType::FLOAT2: return "float2";
                case NumericStructElementType::FLOAT3: return "float3";
                case NumericStructElementType::FLOAT4: return "float4";
                case NumericStructElementType::FLOAT3x3: return "float3x3";
                case NumericStructElementType::FLOAT4x4: return "float4x4";
                case NumericStructElementType::NESTED_STRUCT: return "<nested struct>";
                default: return "<unknown element type>";
            }
        }

        template <typename T>
        static void ParseSimpleType(NumericStructElementType& result, const T& value, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            result = ParseSimpleType(value);
            if (result == NumericStructElementType::NONE)
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse simple numeric struct element type '%1%'.", value));
        }

        template <typename T>
        static NumericStructElementType ParseSimpleType(const T& value)
        {
            static StaticUnorderedMap<size_t, NumericStructElementType, 20, 21, MapPairConstructNone<size_t, NumericStructElementType>, PassthroughHash> lookup
                (
                Utf8String::Hash("int"), NumericStructElementType::INT1,
                Utf8String::Hash("int1"), NumericStructElementType::INT1,
                Utf8String::Hash("int2"), NumericStructElementType::INT2,
                Utf8String::Hash("int3"), NumericStructElementType::INT3,
                Utf8String::Hash("int4"), NumericStructElementType::INT4,
                Utf8String::Hash("uint"), NumericStructElementType::UINT1,
                Utf8String::Hash("uint1"), NumericStructElementType::UINT1,
                Utf8String::Hash("uint2"), NumericStructElementType::UINT2,
                Utf8String::Hash("uint3"), NumericStructElementType::UINT3,
                Utf8String::Hash("uint4"), NumericStructElementType::UINT4,
                Utf8String::Hash("float"), NumericStructElementType::FLOAT1,
                Utf8String::Hash("float1"), NumericStructElementType::FLOAT1,
                Utf8String::Hash("float2"), NumericStructElementType::FLOAT2,
                Utf8String::Hash("float3"), NumericStructElementType::FLOAT3,
                Utf8String::Hash("float4"), NumericStructElementType::FLOAT4,
                Utf8String::Hash("float3x3"), NumericStructElementType::FLOAT3x3,
                Utf8String::Hash("float4x4"), NumericStructElementType::FLOAT4x4
                );

            Utf8StringHash hash;
            auto foundAt = lookup.find(hash(value));
            if (foundAt != lookup.end())
                return foundAt->second;
            else
                return NumericStructElementType::NONE;
        }

        static bool IsScalar(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::INT1:
                case NumericStructElementType::UINT1:
                case NumericStructElementType::FLOAT1: return true;
                default: return false;
            }
        }

        static bool IsVector(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::INT2:
                case NumericStructElementType::INT3:
                case NumericStructElementType::INT4:
                case NumericStructElementType::UINT2:
                case NumericStructElementType::UINT3:
                case NumericStructElementType::UINT4:
                case NumericStructElementType::FLOAT2:
                case NumericStructElementType::FLOAT3:
                case NumericStructElementType::FLOAT4: return true;
                default: return false;
            }
        }

        static bool IsMatrix(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::FLOAT3x3:
                case NumericStructElementType::FLOAT4x4: return true;
                default: return false;
            }
        }

        static bool IsInt32(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::INT1:
                case NumericStructElementType::INT2:
                case NumericStructElementType::INT3:
                case NumericStructElementType::INT4:
                case NumericStructElementType::UINT1:
                case NumericStructElementType::UINT2:
                case NumericStructElementType::UINT3:
                case NumericStructElementType::UINT4: return true;
                default: return false;
            }
        }

        static bool IsUnsigned(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::UINT1:
                case NumericStructElementType::UINT2:
                case NumericStructElementType::UINT3:
                case NumericStructElementType::UINT4: return true;
                default: return false;
            }
        }

        static bool IsFloat(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::FLOAT1:
                case NumericStructElementType::FLOAT2:
                case NumericStructElementType::FLOAT3:
                case NumericStructElementType::FLOAT4:
                case NumericStructElementType::FLOAT3x3:
                case NumericStructElementType::FLOAT4x4: return true;
                default: return false;
            }
        }

        static size_t GetSimpleTypeSizeInElements(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::NONE: return 0;
                case NumericStructElementType::INT1: return 1;
                case NumericStructElementType::INT2: return 2;
                case NumericStructElementType::INT3: return 3;
                case NumericStructElementType::INT4: return 4;
                case NumericStructElementType::UINT1: return 1;
                case NumericStructElementType::UINT2: return 2;
                case NumericStructElementType::UINT3: return 3;
                case NumericStructElementType::UINT4: return 4;
                case NumericStructElementType::FLOAT1: return 1;
                case NumericStructElementType::FLOAT2: return 2;
                case NumericStructElementType::FLOAT3: return 3;
                case NumericStructElementType::FLOAT4: return 4;
                case NumericStructElementType::FLOAT3x3: return 9;
                case NumericStructElementType::FLOAT4x4: return 16;
                default: return 0;
            }
        }

        static size_t GetSimpleTypeSizeInBytes(NumericStructElementType type)
        {
            switch (type)
            {
                case NumericStructElementType::NONE: return 0;
                case NumericStructElementType::INT1: return sizeof(int32_t);
                case NumericStructElementType::INT2: return sizeof(int32_t) * 2;
                case NumericStructElementType::INT3: return sizeof(int32_t) * 3;
                case NumericStructElementType::INT4: return sizeof(int32_t) * 4;
                case NumericStructElementType::UINT1: return sizeof(uint32_t);
                case NumericStructElementType::UINT2: return sizeof(uint32_t) * 2;
                case NumericStructElementType::UINT3: return sizeof(uint32_t) * 3;
                case NumericStructElementType::UINT4: return sizeof(uint32_t) * 4;
                case NumericStructElementType::FLOAT1: return sizeof(float);
                case NumericStructElementType::FLOAT2: return sizeof(float) * 2;
                case NumericStructElementType::FLOAT3: return sizeof(float) * 3;
                case NumericStructElementType::FLOAT4: return sizeof(float) * 4;
                case NumericStructElementType::FLOAT3x3: return sizeof(float) * 9;
                case NumericStructElementType::FLOAT4x4: return sizeof(float) * 16;
                default: return 0;
            }
        }
    };

    template <typename NumericStruct>
    struct NumericStructElement
    {
        using ElementID = typename NumericStruct::ElementID;

        enum class DefaultValue
        {
            NONE,
            ZERO,
            ONE,
            IDENTITY_MATRIX,
            X_AXIS, //All axes must be sequential in this order
            Y_AXIS,
            Z_AXIS,
            NEGATIVE_X_AXIS,
            NEGATIVE_Y_AXIS,
            NEGATIVE_Z_AXIS
        };

        void Reset()
        {
            this->elementID = ElementID::NONE;
            this->arraySize = 1;
            this->sizeInBytes = 0;
            this->strideInBytes = 0;
            this->gpuPaddedOffset = 0;
            this->type = NumericStructElementType::NONE;
            this->nestedStruct = nullptr;
            this->defaultValue = DefaultValue::NONE;
            this->packArray = false;
        }

        size_t GetHash() const
        {
            size_t elements[] = { (size_t)this->elementID, this->arraySize, this->sizeInBytes, (size_t)this->type };
            return Hash::Bytes(elements, sizeof(elements));
        }

        bool IsInt32() const
        {
            return NumericStructElementTypeUtilities::IsInt32(this->type);
        }

        bool IsFloat() const
        {
            return NumericStructElementTypeUtilities::IsFloat(this->type);
        }

        size_t _GetTypeSizeInBytes() const
        {
            auto& element = *this;

            size_t result = 0;
            if (element.type == NumericStructElementType::NESTED_STRUCT)
            {
                assert(element.nestedStruct != nullptr);
                if (element.nestedStruct != nullptr)
                    result = element.nestedStruct->totalSize;
            }
            else
                result = NumericStructElementTypeUtilities::GetSimpleTypeSizeInBytes(element.type);
            return result;
        }

        bool operator == (const NumericStructElement& other) const
        {
            return
                this->elementID == other.elementID &&
                this->arraySize == other.arraySize &&
                this->sizeInBytes == other.sizeInBytes &&
                this->strideInBytes == other.strideInBytes &&
                this->gpuPaddedOffset == other.gpuPaddedOffset &&
                this->type == other.type &&
                NumericStruct::Equal(this->nestedStruct, other.nestedStruct) &&
                this->defaultValue == other.defaultValue &&
                this->packArray == other.packArray
                ;
        }

        bool IsSubsetOf(const NumericStructElement& other) const
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
        NumericStructElementType type; //The element type
        NumericStruct* nestedStruct; //Pointer to a nested type, if type = NumericStructElementType::NESTED_STRUCT
        DefaultValue defaultValue; //Default value setting
        bool packArray; //Indicates whether array elements should be packed together tightly. By default they are not, which matches typical GPU memory alignment behavior, Packing can be useful to make a configured array map to consecutive non-array values
    };

    template <typename NumericStructMetadata>
    class NumericStruct
    {
    public:
        using ElementID = typename NumericStructMetadata::ElementID;

        using Element = NumericStructElement<NumericStruct>;

        struct Defines
        {
            enum { NONE = 0 };
        };

        enum class PaddedTotalSizeBehavior
        {
            DEFAULT, //Can be used when creating an instance of the numeric struct
            NONE,
            ROUND_TO_ROW_SIZE,
            ROUND_UP_TO_NEAREST_ROW_SIZE_MULTIPLE_OF_TOTAL_SIZE_ALIGNMENT,
            ROUND_UP_TO_TOTAL_SIZE_ALIGNMENT
        };

        NumericStruct(Allocator* allocator) : typeName(allocator)
        {
            this->memoryStartAddressAlignment = 0;
            this->memoryRowSizeInBytes = 0;
            this->memoryTotalSizeAlignment = 0;
            this->totalSize = 0;
            this->paddedTotalSize = 0;
            this->zeroAllElementDefaultValues = false;
            this->elementHash = 0;
            this->paddedTotalSizeBehavior = PaddedTotalSizeBehavior::NONE;
        }

        const Utf8String& GetTypeName() const
        {
            return this->typeName;
        }

        size_t GetElementHash() const
        {
            return this->elementHash;
        }

        size_t GetPaddedTotalSize(PaddedTotalSizeBehavior behavior) const
        {
            switch (behavior)
            {
                case PaddedTotalSizeBehavior::DEFAULT:
                {
                    return this->paddedTotalSize;
                }
                case PaddedTotalSizeBehavior::NONE:
                {
                    return this->totalSize;
                }
                case PaddedTotalSizeBehavior::ROUND_TO_ROW_SIZE:
                {
                    //Pad the size to contain an even multiple of rows
                    return Allocator::AlignSizeUp(this->totalSize, this->memoryRowSizeInBytes);
                }
                case PaddedTotalSizeBehavior::ROUND_UP_TO_NEAREST_ROW_SIZE_MULTIPLE_OF_TOTAL_SIZE_ALIGNMENT:
                {
                    auto result = this->totalSize;
                    if (result < this->memoryTotalSizeAlignment)
                    {
                        //Size is less than memoryTotalSizeAlignment. Find the next highest size that divides evenly into memoryTotalSizeAlignment
                        for (size_t i = 1; this->memoryTotalSizeAlignment % result > 0; i *= 2)
                            result = Allocator::AlignSizeUp(result, this->memoryRowSizeInBytes * i);
                    }
                    else
                        result = Allocator::AlignSizeUp(result, this->memoryTotalSizeAlignment);
                    return result;
                }
                case PaddedTotalSizeBehavior::ROUND_UP_TO_TOTAL_SIZE_ALIGNMENT:
                {
                    return Allocator::AlignSizeUp(this->totalSize, this->memoryTotalSizeAlignment);
                }
            }

            assert(0);
            return this->paddedTotalSize;
        }

        template <typename NumericStructVector, typename PackingRules>
        static void Create
            (
            NumericStructVector& numericStructs,
            Allocator* allocator,
            const ByteBuffer& readBuffer,
            PackingRules& packingRules,
            uint32_t defines,
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            ConfigDocumentReader reader;
            reader.Start(readBuffer);

            Create(numericStructs, allocator, reader, packingRules, defines, error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
        }

        template <typename NumericStructVector, typename PackingRules>
        static void Create
            (
            NumericStructVector& numericStructs,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            PackingRules& packingRules,
            uint32_t defines,
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            if (reader.Current() == nullptr)
            {
                FINJIN_SET_ERROR(error, "The specified reader ended unexpectedly.");
                return;
            }

            auto startLine = *reader.Current();

            auto& configSectionName = NumericStructMetadata::GetConfigSectionName();

            auto structCount = reader.GetSectionCount(configSectionName);
            if (structCount > 0)
            {
                if (!numericStructs.Create(structCount, allocator, allocator))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' numeric structs.", structCount));
                    return;
                }

                structCount = 0;

                for (auto line = reader.Current(); line != nullptr; line = reader.Next())
                {
                    switch (line->GetType())
                    {
                        case ConfigDocumentLine::Type::SECTION:
                        {
                            Utf8StringView sectionName;
                            line->GetSectionName(sectionName);

                            if (sectionName == configSectionName)
                            {
                                auto& desc = numericStructs[structCount++];

                                CreateFromScope
                                    (
                                    desc,
                                    allocator,
                                    reader,
                                    packingRules,
                                    &numericStructs[0],
                                    structCount - 1,
                                    defines,
                                    error
                                    );
                                if (error)
                                {
                                    FINJIN_SET_ERROR(error, "Failed to read buffer numericStruct.");
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

        template <typename PackingRules>
        static void CreateFromScope
            (
            NumericStruct& desc,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            PackingRules& packingRules,
            NumericStruct* otherStructs,
            size_t otherStructCount,
            uint32_t defines,
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            packingRules.GetAlignmentState(desc);
            desc.zeroAllElementDefaultValues = true; //Assume all can be zeroed

            auto& configSectionElementName = NumericStructMetadata::GetConfigElementSectionName();

            auto elementCount = reader.GetSectionCountBeneathCurrent(configSectionElementName);
            if (!desc.elements.Create(elementCount, allocator))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' elements.", elementCount));
                return;
            }

            if (!desc.elements.empty())
            {
                elementCount = 0;

                int depth = 0;
                auto descriptionDone = false;

                Utf8StringView sectionName;
                Utf8StringView key, value;

                for (auto line = reader.Next(); line != nullptr && !descriptionDone; )
                {
                    switch (line->GetType())
                    {
                        case ConfigDocumentLine::Type::SECTION:
                        {
                            line->GetSectionName(sectionName);

                            if (sectionName == configSectionElementName && elementCount < desc.elements.size())
                            {
                                auto& item = desc.elements[elementCount];

                                uint32_t ifDefines = 0;

                                ReadItemFromScope(reader, item, otherStructs, otherStructCount, ifDefines, error);
                                if (error)
                                {
                                    FINJIN_SET_ERROR(error, "Failed to read buffer item.");
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
                        case ConfigDocumentLine::Type::KEY_AND_VALUE:
                        {
                            line->GetKeyAndValue(key, value);

                            if (key == "type")
                                desc.typeName = value;
                            else if (key == "padded-total-size-behavior")
                            {
                                if (value == "default")
                                    desc.paddedTotalSizeBehavior = PaddedTotalSizeBehavior::DEFAULT;
                                else if (value == "none")
                                    desc.paddedTotalSizeBehavior = PaddedTotalSizeBehavior::NONE;
                                else if (value == "round-to-row-size")
                                    desc.paddedTotalSizeBehavior = PaddedTotalSizeBehavior::ROUND_TO_ROW_SIZE;
                                else if (value == "round-up-to-nearest-row-size-multiple-of-total-size-alignment")
                                    desc.paddedTotalSizeBehavior = PaddedTotalSizeBehavior::ROUND_UP_TO_NEAREST_ROW_SIZE_MULTIPLE_OF_TOTAL_SIZE_ALIGNMENT;
                                else if (value == "round-up-to-total-size-alignment")
                                    desc.paddedTotalSizeBehavior = PaddedTotalSizeBehavior::ROUND_UP_TO_TOTAL_SIZE_ALIGNMENT;
                                else
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse 'total-size-pading-behavior' value '%1%'.", value));
                                    return;
                                }
                            }

                            break;
                        }
                        case ConfigDocumentLine::Type::SCOPE_START:
                        {
                            depth++;
                            break;
                        }
                        case ConfigDocumentLine::Type::SCOPE_END:
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

                //Perform a resize in case the number of elements actually created is smaller than what was counted initially
                desc.elements.resize(elementCount);
            }

            _InitializeDescription(desc, packingRules);
        }

        template <typename NumericStructVector, typename Name>
        static NumericStruct* GetByTypeName(NumericStructVector& numericStructs, const Name& name)
        {
            for (auto& desc : numericStructs)
            {
                if (desc.typeName == name)
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

        size_t GetElementCount() const
        {
            return this->elements.size();
        }

        bool operator == (const NumericStruct& other) const
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

        bool IsSubsetOf(const NumericStruct& other) const
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

        static bool Equal(NumericStruct* a, NumericStruct* b)
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
            ConfigDocumentReader& reader,
            Element& element,
            NumericStruct* otherStructs,
            size_t otherStructCount,
            uint32_t& ifDefines,
            Error& error
            )
        {
            FINJIN_ERROR_METHOD_START(error);

            element.Reset();

            Utf8StringView key, value;

            auto depth = 0;

            for (auto line = reader.Next(); line != nullptr; line = reader.Next())
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::KEY_AND_VALUE:
                    {
                        line->GetKeyAndValue(key, value);

                        if (key == "id")
                        {
                            NumericStructMetadata::ParseElementID(element.elementID, value, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR_NO_MESSAGE(error);
                                return;
                            }
                        }
                        else if (key == "type")
                        {
                            element.type = NumericStructElementTypeUtilities::ParseSimpleType(value);
                            if (element.type == NumericStructElementType::NONE)
                            {
                                for (size_t i = 0; i < otherStructCount; i++)
                                {
                                    if (value == otherStructs[i].typeName)
                                    {
                                        element.type = NumericStructElementType::NESTED_STRUCT;
                                        element.nestedStruct = &otherStructs[i];
                                        break;
                                    }
                                }
                                if (element.type == NumericStructElementType::NONE)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse item type '%1%'.", value));
                                    return;
                                }
                            }
                        }
                        else if (key == "array-size")
                        {
                            element.arraySize = Convert::ToInteger(value, element.arraySize);
                            if (element.arraySize == 0)
                                element.arraySize = 1;
                        }
                        else if (key == "pack-array")
                        {
                            element.packArray = Convert::ToBool(value, element.packArray);
                        }
                        else if (key == "ifdef")
                        {
                            ifDefines = NumericStructMetadata::ParseIfdef(value);
                        }
                        else if (key == "default-value")
                        {
                            if (value == "0")
                                element.defaultValue = Element::DefaultValue::ZERO;
                            else if (value == "1")
                                element.defaultValue = Element::DefaultValue::ONE;
                            else if (value == "identity-matrix")
                                element.defaultValue = Element::DefaultValue::IDENTITY_MATRIX;
                            else if (value == "x-axis")
                                element.defaultValue = Element::DefaultValue::X_AXIS;
                            else if (value == "y-axis")
                                element.defaultValue = Element::DefaultValue::Y_AXIS;
                            else if (value == "z-axis")
                                element.defaultValue = Element::DefaultValue::Z_AXIS;
                            else if (value == "negative-x-axis")
                                element.defaultValue = Element::DefaultValue::NEGATIVE_X_AXIS;
                            else if (value == "negative-y-axis")
                                element.defaultValue = Element::DefaultValue::NEGATIVE_Y_AXIS;
                            else if (value == "negative-z-axis")
                                element.defaultValue = Element::DefaultValue::NEGATIVE_Z_AXIS;
                            else
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse default value '%1%'.", value));
                                return;
                            }
                        }
                        break;
                    }
                    case ConfigDocumentLine::Type::SCOPE_START:
                    {
                        depth++;
                        break;
                    }
                    case ConfigDocumentLine::Type::SCOPE_END:
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

        template <typename PackingRules>
        static void _InitializeDescription
            (
            NumericStruct& desc,
            PackingRules& packingRules
            )
        {
            FINJIN_ZERO_MEMORY(&desc.elementIDToElement[0], desc.elementIDToElement.size() * sizeof(void*));
            desc.totalSize = 0;
            desc.paddedTotalSize = 0;

            if (!desc.elements.empty())
            {
                packingRules.StartPack(desc);

                for (size_t i = 0; i < desc.elements.size(); i++)
                {
                    auto previousElement = i > 0 ? &desc.elements[i - 1] : nullptr;
                    auto& element = desc.elements[i];
                    packingRules.PackElement(desc, element, previousElement, i == desc.elements.size() - 1);
                }

                packingRules.FinishPack(desc);

                //Build elementID->element lookup and calculate hash
                for (auto& element : desc.elements)
                {
                    if (element.elementID != ElementID::NONE)
                    {
                        desc.elementIDToElement[element.elementID] = &element;

                        size_t elementHash[] = { element.GetHash(), desc.elementHash };
                        desc.elementHash = Hash::Bytes(elementHash, sizeof(elementHash));
                    }
                }
            }
        }

    public:
        Utf8String typeName; //The internal name of the type. Used when looking up nested types
        DynamicVector<Element> elements;
        size_t memoryStartAddressAlignment; //Start address alignment
        size_t memoryRowSizeInBytes; //Element size alignment
        size_t memoryTotalSizeAlignment; //Alignment on the total size of the buffer
        size_t totalSize;
        size_t paddedTotalSize;
        bool zeroAllElementDefaultValues; //Indicates whether all elements can be zeroed during a call to GpuConstantBuffer::SetDefaults()
        EnumArray<ElementID, ElementID::COUNT, Element*> elementIDToElement; //Look up table to quickly map a elementID to an element
        size_t elementHash; //Cached hash of the elements
        PaddedTotalSizeBehavior paddedTotalSizeBehavior;
    };

    template <typename ConcreteNumericStruct>
    class NumericStructInstance
    {
    public:
        NumericStructInstance()
        {
            this->numericStruct = nullptr;
            this->structInstanceBuffer = nullptr;
            this->instanceCount = 0;
        }

        void SetDefaults()
        {
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            _SetDefaults(0, this->numericStruct, this->instanceCount);
        }

        size_t GetPaddedTotalSize() const
        {
            return this->numericStruct->paddedTotalSize;
        }

        //Allows for an arbitrary depth when setting a value. For example:
        //Simple value: Set(roughnessValue, 0, ConcreteNumericStruct::ElementID::MATERIAL_ROUGHNESS);
        //which can be read as: buffer[instance 0].MATERIAL_ROUGHNESS = roughnessValue
        // or
        //Array value: Set(roughnessValue, 0, ConcreteNumericStruct::ElementID::MATERIAL_ROUGHNESS, 1);
        //which can be read as: buffer[instance 0].MATERIAL_ROUGHNESS[1] = roughnessValue
        // or
        //Nested value: Set(lightDirectionValue, 0, ConcreteNumericStruct::ElementID::LIGHT, 0, ConcreteNumericStruct::ElementID::LIGHT_DIRECTION);
        //which can be read as: buffer[instance 0].LIGHT[instance 0].LIGHT_DIRECTION = lightDirectionValue
        template <typename T, typename IndexType, typename... Args>
        size_t Set(const T& data, size_t instanceIndex, const IndexType& elementIDOrIndexOrName, const Args&... args)
        {
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            return _Set(data, 0, this->numericStruct, instanceIndex, elementIDOrIndexOrName, args...);
        }

        /*template <bool useInternalOrder, typename T, typename IndexType, typename... Args> //T must be MathMatrix4
        void SetMatrix(const T& data, T& dataTransposeTemp, size_t instanceIndex, const IndexType& elementIDOrIndexOrName, const Args&... args)
        {
            static_assert(T::RowsAtCompileTime == 4 && T::ColsAtCompileTime == 4, "Only 4x4 matrices are supported.");
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            _SetMatrix(data, dataTransposeTemp, typename std::integral_constant<bool, useInternalOrder>::type(), instanceIndex, elementIDOrIndexOrName, args...);
        }*/

        template <typename T, typename IndexType, typename... Args>
        void SetArray(const T& data, size_t count, size_t instanceIndex, const IndexType& elementIDOrIndexOrName, const Args&... args)
        {
            //Note: This could be made more efficient by doing the iteration at the lowest level, not the top
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            for (size_t i = 0; i < count; i++)
                _Set(data[i], 0, this->numericStruct, instanceIndex, elementIDOrIndexOrName, args..., i);
        }

        template <typename T, typename IndexType, typename... Args>
        void SetArray(const T& data, size_t instanceIndex, const IndexType& elementIDOrIndexOrName, const Args&... args)
        {
            //Note: This could be made more efficient by doing the iteration at the lowest level, not the top
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            auto count = data.size();
            for (size_t i = 0; i < count; i++)
                _Set(data[i], 0, this->numericStruct, instanceIndex, elementIDOrIndexOrName, args..., i);
        }

        template <typename T>
        size_t Set(const T& data, size_t instanceIndex)
        {
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            //Note that using this is fairly dangerous since T must be defined EXACTLY as it is defined in a shader (.hlsl) file, including padding
            auto elementByteOffset = instanceIndex * this->paddedTotalSize;
            FINJIN_COPY_MEMORY(&this->structInstanceBuffer[elementByteOffset], &data, sizeof(T));
            return elementByteOffset;
        }

        size_t Set(const NumericStructInstance& data, size_t instanceIndex)
        {
            assert(this->structInstanceBuffer != nullptr);
            assert(this->instanceCount > 0);

            //Copies a buffer to this buffer
            auto paddedTotalSize = std::min(this->paddedTotalSize, data.paddedTotalSize);
            auto elementByteOffset = instanceIndex * this->paddedTotalSize;
            FINJIN_COPY_MEMORY(&this->structInstanceBuffer[elementByteOffset], data.structInstanceBuffer, paddedTotalSize);
            return elementByteOffset;
        }

    private:
        /*template <typename T, typename IndexType, typename... Args>
        inline void _SetMatrix(const T& data, T& dataTransposeTemp, std::false_type, size_t instanceIndex, const IndexType& elementIDOrIndexOrName, const Args&... args)
        {
            //useInternalOrder = false, so transpose
            dataTransposeTemp = data;
            dataTransposeTemp.transposeInPlace();
            return _Set(dataTransposeTemp, 0, this->numericStruct, instanceIndex, elementIDOrIndexOrName, args...);
        }

        template <typename T, typename IndexType, typename... Args>
        inline void _SetMatrix(const T& data, T& dataTransposeTemp, std::true_type, size_t instanceIndex, const IndexType& elementIDOrIndexOrName, const Args&... args)
        {
            //useInternalOrder = true, so use internal representation
            return _Set(data, 0, this->numericStruct, instanceIndex, elementIDOrIndexOrName, args...);
        }*/

        template <typename T, typename IndexType>
        inline size_t _Set
            (
            const T& data,
            size_t baseByteOffset,
            const ConcreteNumericStruct* parentStruct,
            size_t parentInstanceIndex,
            const IndexType& elementIDOrName,
            size_t index
            )
        {
            auto element = parentStruct->GetElement(elementIDOrName);
            if (element != nullptr)
            {
                auto elementByteOffset = baseByteOffset + parentInstanceIndex * parentStruct->paddedTotalSize + element->gpuPaddedOffset + (element->strideInBytes * index);
                auto byteCount = std::min(sizeof(T), element->sizeInBytes);
                FINJIN_COPY_MEMORY(&this->structInstanceBuffer[elementByteOffset], &data, byteCount);
                return elementByteOffset;
            }
            else
                return (size_t)-1;
        }

        template <typename T, typename IndexType>
        inline size_t _Set
            (
            const T& data,
            size_t baseByteOffset,
            const ConcreteNumericStruct* parentStruct,
            size_t parentInstanceIndex,
            const IndexType& elementIDOrIndexOrName
            )
        {
            auto element = parentStruct->GetElement(elementIDOrIndexOrName);
            if (element != nullptr)
            {
                auto elementByteOffset = baseByteOffset + parentInstanceIndex * parentStruct->paddedTotalSize + element->gpuPaddedOffset;
                auto byteCount = std::min(sizeof(T), element->arraySize * element->sizeInBytes);
                FINJIN_COPY_MEMORY(&this->structInstanceBuffer[elementByteOffset], &data, byteCount);
                return elementByteOffset;
            }
            else
                return (size_t)-1;
        }

        template <typename T, typename ElementIndexType, typename NestedElementIndexType, typename... Args>
        inline void _Set
            (
            const T& data,
            size_t baseByteOffset,
            const ConcreteNumericStruct* parentStruct,
            size_t parentInstanceIndex,
            const ElementIndexType& elementIDOrIndexOrName,
            size_t nestedInstanceIndex,
            const NestedElementIndexType& nestedElementIDOrIndexOrName,
            const Args&... args
            )
        {
            auto element = parentStruct->GetElement(elementIDOrIndexOrName);
            if (element != nullptr && element->nestedStruct != nullptr)
            {
                _Set
                    (
                    data,
                    baseByteOffset + parentInstanceIndex * parentStruct->paddedTotalSize + element->gpuPaddedOffset,
                    element->nestedStruct,
                    nestedInstanceIndex,
                    nestedElementIDOrIndexOrName,
                    args...
                    );
            }
        }

        void _SetDefaults(size_t baseByteOffset, const ConcreteNumericStruct* parentStruct, size_t parentInstanceCount)
        {
            if (parentStruct->zeroAllElementDefaultValues)
            {
                //Everything can be zeroed
                FINJIN_ZERO_MEMORY(&this->structInstanceBuffer[baseByteOffset], parentStruct->paddedTotalSize * parentInstanceCount);
            }
            else
            {
                //Iterate over instances, setting defaults
                for (size_t parentInstanceIndex = 0; parentInstanceIndex < parentInstanceCount; parentInstanceIndex++)
                    _SetInstanceDefaults(baseByteOffset, parentStruct, parentInstanceIndex);
            }
        }

        void _SetInstanceDefaults(size_t baseByteOffset, const ConcreteNumericStruct* parentStruct, size_t parentInstanceIndex)
        {
            //Offset to the parent instance
            auto parentInstanceByteOffset = baseByteOffset + parentInstanceIndex * parentStruct->paddedTotalSize;

            //Iterate over each element in the struct
            for (size_t elementIndex = 0; elementIndex < parentStruct->elements.size(); elementIndex++)
            {
                auto& element = parentStruct->elements[elementIndex];

                //Offset to the element in the numericStruct, relative to parent instance
                auto elementOffset = parentInstanceByteOffset + element.gpuPaddedOffset;

                //Iterate over each array entry in the numericStruct element
                for (size_t arrayEntryIndex = 0; arrayEntryIndex < element.arraySize; arrayEntryIndex++)
                {
                    //Offset to the array entry, relative to the array in the numericStruct element
                    auto arrayElementByteOffset = elementOffset + arrayEntryIndex * element.strideInBytes;

                    if (element.type == NumericStructElementType::NESTED_STRUCT)
                    {
                        _SetDefaults(arrayElementByteOffset, element.nestedStruct, 1);
                    }
                    else
                    {
                        auto arrayEntryBufferPointer = &this->structInstanceBuffer[arrayElementByteOffset];

                        switch (element.defaultValue)
                        {
                            case ConcreteNumericStruct::Element::DefaultValue::NONE:
                            {
                                break;
                            }
                            case ConcreteNumericStruct::Element::DefaultValue::ZERO:
                            {
                                FINJIN_ZERO_MEMORY(arrayEntryBufferPointer, element.arraySize * element.sizeInBytes);
                                break;
                            }
                            case ConcreteNumericStruct::Element::DefaultValue::ONE:
                            {
                                if (element.IsInt32())
                                {
                                    auto valueCount = (element.arraySize * element.sizeInBytes) / sizeof(int32_t);
                                    auto values = reinterpret_cast<int32_t*>(arrayEntryBufferPointer);
                                    for (size_t valueIndex = 0; valueIndex < valueCount; valueIndex++)
                                        values[valueIndex] = 1;
                                }
                                else if (element.IsFloat())
                                {
                                    auto valueCount = (element.arraySize * element.sizeInBytes) / sizeof(float);
                                    auto values = reinterpret_cast<float*>(arrayEntryBufferPointer);
                                    for (size_t valueIndex = 0; valueIndex < valueCount; valueIndex++)
                                        values[valueIndex] = 1.0f;
                                }
                                else
                                {
                                    //Unsupported. Just zero it out
                                    FINJIN_ZERO_MEMORY(arrayEntryBufferPointer, element.arraySize * element.sizeInBytes);
                                }
                                break;
                            }
                            case ConcreteNumericStruct::Element::DefaultValue::IDENTITY_MATRIX:
                            {
                                switch (element.type)
                                {
                                    case NumericStructElementType::FLOAT3x3:
                                    {
                                        auto values = reinterpret_cast<float*>(arrayEntryBufferPointer);
                                        values[0] = 1.0f; values[1] = 0.0f; values[2] = 0.0f;
                                        values[3] = 0.0f; values[4] = 1.0f; values[5] = 0.0f;
                                        values[6] = 0.0f; values[7] = 0.0f; values[8] = 1.0f;
                                        break;
                                    }
                                    case NumericStructElementType::FLOAT4x4:
                                    {
                                        auto values = reinterpret_cast<float*>(arrayEntryBufferPointer);
                                        values[0] = 1.0f; values[1] = 0.0f; values[2] = 0.0f; values[3] = 0.0f;
                                        values[4] = 0.0f; values[5] = 1.0f; values[6] = 0.0f; values[7] = 0.0f;
                                        values[8] = 0.0f; values[9] = 0.0f; values[10] = 1.0f; values[11] = 0.0f;
                                        values[12] = 0.0f; values[13] = 0.0f; values[14] = 0.0f; values[15] = 1.0f;
                                        break;
                                    }
                                    default:
                                    {
                                        //Unsupported. Just zero it out
                                        FINJIN_ZERO_MEMORY(arrayEntryBufferPointer, element.arraySize * element.sizeInBytes);
                                        break;
                                    }
                                }
                                break;
                            }
                            case ConcreteNumericStruct::Element::DefaultValue::X_AXIS:
                            case ConcreteNumericStruct::Element::DefaultValue::Y_AXIS:
                            case ConcreteNumericStruct::Element::DefaultValue::Z_AXIS:
                            case ConcreteNumericStruct::Element::DefaultValue::NEGATIVE_X_AXIS:
                            case ConcreteNumericStruct::Element::DefaultValue::NEGATIVE_Y_AXIS:
                            case ConcreteNumericStruct::Element::DefaultValue::NEGATIVE_Z_AXIS:
                            {
                                static const int32_t int32Axes[6][4] =
                                {
                                    { 1, 0, 0, 0 },
                                    { 0, 1, 0, 0 },
                                    { 0, 0, 1, 0 },
                                    { -1, 0, 0, 0 },
                                    { 0, -1, 0, 0 },
                                    { 0, 0, -1, 0 }
                                };
                                static const float floatAxes[6][4] =
                                {
                                    { 1, 0, 0, 0 },
                                    { 0, 1, 0, 0 },
                                    { 0, 0, 1, 0 },
                                    { -1, 0, 0, 0 },
                                    { 0, -1, 0, 0 },
                                    { 0, 0, -1, 0 }
                                };

                                auto isSupported = false;

                                if (element.IsInt32())
                                {
                                    auto elementCount = NumericStructElementTypeUtilities::GetSimpleTypeSizeInElements(element.type);
                                    if (elementCount == 3 || elementCount == 4)
                                    {
                                        auto axis = int32Axes[static_cast<size_t>(element.defaultValue) - static_cast<size_t>(ConcreteNumericStruct::Element::DefaultValue::X_AXIS)];
                                        FINJIN_COPY_MEMORY(arrayEntryBufferPointer, axis, (element.arraySize * element.sizeInBytes) / sizeof(int32_t));

                                        isSupported = true;
                                    }
                                }
                                else if (element.IsFloat())
                                {
                                    auto elementCount = NumericStructElementTypeUtilities::GetSimpleTypeSizeInElements(element.type);
                                    if (elementCount == 3 || elementCount == 4)
                                    {
                                        auto axis = floatAxes[static_cast<size_t>(element.defaultValue) - static_cast<size_t>(ConcreteNumericStruct::Element::DefaultValue::X_AXIS)];
                                        FINJIN_COPY_MEMORY(arrayEntryBufferPointer, axis, (element.arraySize * element.sizeInBytes) / sizeof(float));

                                        isSupported = true;
                                    }
                                }

                                if (!isSupported)
                                {
                                    //Unsupported. Just zero it out
                                    FINJIN_ZERO_MEMORY(arrayEntryBufferPointer, element.arraySize * element.sizeInBytes);
                                }

                                break;
                            }
                        }
                    }
                }
            }
        }

    protected:
        const ConcreteNumericStruct* numericStruct; //Pointer to NumericStruct derived class that contains the NumericStruct
        uint8_t* structInstanceBuffer; //Pointer to data representing 'instanceCount' instances of the NumericStruct
        size_t instanceCount; //The number of instances of NumericStruct in the buffer
    };

    /*struct GenericNumericStructMetadata
    {
        enum class ElementID
        {
            NONE,

            COUNT
        };

        static void ParseElementID(ElementID& result, const Utf8StringView& value, Error& error)
        {
            result = ElementID::NONE;
        }

        static const Utf8String& GetConfigSectionName()
        {
            return Utf8String::Empty();
        }

        static const Utf8String& GetConfigElementSectionName()
        {
            return Utf8String::Empty();
        }

        template <typename StringType>
        static uint32_t ParseIfdef(const StringType& value)
        {
            return 0;
        }
    };
    using GenericNumericStruct = NumericStruct<GenericNumericStructMetadata>;*/

    template <typename ConcreteNumericStruct>
    class SystemMemoryNumericStructInstance : public NumericStructInstance<ConcreteNumericStruct>
    {
    public:
        void Create(const ConcreteNumericStruct& numericStruct, size_t instanceCount, Allocator* allocator, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (numericStruct.elements.empty())
            {
                FINJIN_SET_ERROR(error, "Invalid struct. It contains 0 elements.");
                return;
            }

            if (instanceCount == 0)
            {
                FINJIN_SET_ERROR(error, "Invalid instance count. Must be 1 or greater.");
                return;
            }

            Destroy();

            this->numericStruct = &numericStruct;
            this->instanceCount = instanceCount;

            //Allocate buffer
            auto maxByteCount = (instanceCount * this->paddedTotalSize) + numericStruct.memoryStartAddressAlignment;
            if (!this->byteBuffer.Create(maxByteCount, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to initialize system buffer memory.");
                return;
            }

            //Get an appropriate pointer for use
            this->structInstanceBuffer = Allocator::AlignMemoryUp(this->byteBuffer.data(), numericStruct.memoryStartAddressAlignment);

            NumericStructInstance<ConcreteNumericStruct>::SetDefaults();
        }

        void Destroy()
        {
            this->numericStruct = nullptr;
            this->instanceCount = 0;
            this->structInstanceBuffer = nullptr;
            this->byteBuffer.Destroy();
        }

    private:
        ByteBuffer byteBuffer;
    };

        template <typename NumericStruct>
    struct DefaultBufferPackingRules
    {
        DefaultBufferPackingRules(size_t memoryStartAddressAlignment, size_t memoryRowSizeInBytes, size_t memoryTotalSizeAlignment)
        {
            this->memoryStartAddressAlignment = memoryStartAddressAlignment;
            this->memoryRowSizeInBytes = memoryRowSizeInBytes;
            this->memoryTotalSizeAlignment = memoryTotalSizeAlignment;
        }

        void GetAlignmentState(NumericStruct& desc)
        {
            desc.memoryStartAddressAlignment = this->memoryStartAddressAlignment;
            desc.memoryRowSizeInBytes = this->memoryRowSizeInBytes;
            desc.memoryTotalSizeAlignment = this->memoryTotalSizeAlignment;
        }

        void StartPack(NumericStruct& desc)
        {
            this->previousElementType = NumericStructElementType::NONE;
            this->paddedRowElementOffset = 0;
        }

        void PackElement(NumericStruct& desc, typename NumericStruct::Element& element, typename NumericStruct::Element* previousElement, bool isLast)
        {
            element.sizeInBytes = element._GetTypeSizeInBytes();

            if (element.arraySize == 1 || element.packArray)
                element.strideInBytes = element.sizeInBytes;
            else
                element.strideInBytes = Allocator::AlignSizeUp(element.sizeInBytes, desc.memoryRowSizeInBytes);

            //Add all the array items
            for (size_t arrayItemIndex = 0; arrayItemIndex < element.arraySize; arrayItemIndex++)
            {
                if (this->paddedRowElementOffset != 0)
                {
                    if ((element.arraySize > 1 && !element.packArray) || //Non-packed array
                        (element.packArray && arrayItemIndex == 0 && element.arraySize > 1 && (this->paddedRowElementOffset + element.arraySize * element.sizeInBytes) > desc.memoryRowSizeInBytes) || //First element of packed array, and it won't fit into current row
                        ((this->paddedRowElementOffset + element.sizeInBytes) > desc.memoryRowSizeInBytes) || //Element spans multiple rows
                        (element.type == NumericStructElementType::NESTED_STRUCT) || //Element is a nested struct
                        (this->previousElementType == NumericStructElementType::NESTED_STRUCT)) //Previous element is a nested struct
                    {
                        //Element should be at start of row
                        desc.totalSize += desc.memoryRowSizeInBytes - this->paddedRowElementOffset;
                        this->paddedRowElementOffset = 0;
                    }
                }

                //Assign to element if it's first array item
                if (arrayItemIndex == 0)
                    element.gpuPaddedOffset = desc.totalSize;

                //Adjust running total
                if (arrayItemIndex == element.arraySize - 1)
                    desc.totalSize += element.sizeInBytes;
                else
                    desc.totalSize += element.strideInBytes;
                this->paddedRowElementOffset = (this->paddedRowElementOffset + element.strideInBytes) % desc.memoryRowSizeInBytes;

                //Update 'previous' type
                this->previousElementType = element.type;
            }
        }

        void FinishPack(NumericStruct& desc)
        {
            //Calculate padded total size
            desc.paddedTotalSize = desc.totalSize; //Copy in case behavior is DEFAULT
            desc.paddedTotalSize = desc.GetPaddedTotalSize(desc.paddedTotalSizeBehavior);
        }

        size_t memoryStartAddressAlignment;
        size_t memoryRowSizeInBytes;
        size_t memoryTotalSizeAlignment;

        NumericStructElementType previousElementType;
        size_t paddedRowElementOffset;
    };

    template <typename NumericStruct>
    class Std140ConstantBufferPackingRules
    {
        //This packing works for D3D12 and Vulkan constant/uniform buffers
        //https://khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf page 137, section 7.6.2.2 (Standard Uniform Block Layout)
        //This class adds a few tweakables such as a customizable row size as well as the notion of a "total size alignment"

    public:
        Std140ConstantBufferPackingRules()
        {
            this->memoryRowSizeInBytes = sizeof(float) * 4;
            this->memoryTotalSizeAlignment = 1;
        }

        Std140ConstantBufferPackingRules(size_t memoryTotalSizeAlignment)
        {
            this->memoryRowSizeInBytes = sizeof(float) * 4;
            this->memoryTotalSizeAlignment = memoryTotalSizeAlignment;
        }

        Std140ConstantBufferPackingRules(size_t memoryRowSizeInBytes, size_t memoryTotalSizeAlignment)
        {
            this->memoryRowSizeInBytes = memoryRowSizeInBytes;
            this->memoryTotalSizeAlignment = memoryTotalSizeAlignment;
        }

        void GetAlignmentState(NumericStruct& desc)
        {
            desc.memoryStartAddressAlignment = 0;
            desc.memoryRowSizeInBytes = this->memoryRowSizeInBytes;
            desc.memoryTotalSizeAlignment = this->memoryTotalSizeAlignment;
        }

        void StartPack(NumericStruct& desc)
        {
            //std::cout << "Description " << desc.typeName << ": " << std::endl;
        }

        void PackElement
            (
            NumericStruct& desc,
            typename NumericStruct::Element& element,
            typename NumericStruct::Element* previousElement,
            bool isLast
            )
        {
            size_t baseAlignmentInBytes = 0;
            size_t baseSizeInBytes = 0;
            if (NumericStructElementTypeUtilities::IsScalar(element.type))
                baseAlignmentInBytes = baseSizeInBytes = sizeof(float);
            else if (NumericStructElementTypeUtilities::IsVector(element.type))
            {
                switch (NumericStructElementTypeUtilities::GetSimpleTypeSizeInElements(element.type))
                {
                    case 2: baseAlignmentInBytes = baseSizeInBytes = sizeof(float) * 2; break;
                    case 3: //Fall through. A 3-vector is treated like a 4-vector
                    case 4: baseAlignmentInBytes = baseSizeInBytes = sizeof(float) * 4; break;
                    default: break;
                }
            }
            else if (NumericStructElementTypeUtilities::IsMatrix(element.type))
                baseAlignmentInBytes = baseSizeInBytes = this->memoryRowSizeInBytes;
            else if (element.type == NumericStructElementType::NESTED_STRUCT)
            {
                baseAlignmentInBytes = sizeof(float) * 4;
                baseSizeInBytes = Allocator::AlignSizeUp(element.sizeInBytes, desc.memoryRowSizeInBytes);
            }

            auto previousIsArrayOrStruct = false;
            if (previousElement != nullptr)
            {
                auto previousTotalArraySize = previousElement->arraySize * GetTypeArraySize(*previousElement);
                previousIsArrayOrStruct = previousTotalArraySize > 1 || previousElement->type == NumericStructElementType::NESTED_STRUCT;
            }

            auto typeArraySize = GetTypeArraySize(element);
            auto totalArraySize = element.arraySize * typeArraySize;
            auto isArrayOrStruct = totalArraySize > 1 || element.type == NumericStructElementType::NESTED_STRUCT;

            element.sizeInBytes = element._GetTypeSizeInBytes();

            if (totalArraySize == 1)
                element.strideInBytes = element.sizeInBytes;
            else
                element.strideInBytes = Allocator::AlignSizeUp(baseSizeInBytes * typeArraySize, desc.memoryRowSizeInBytes);

            if (previousIsArrayOrStruct || isArrayOrStruct)
                element.gpuPaddedOffset = Allocator::AlignSizeUp(desc.totalSize, desc.memoryRowSizeInBytes);
            else
                element.gpuPaddedOffset = Allocator::AlignSizeUp(desc.totalSize, baseAlignmentInBytes);

            //Add all the array items
            for (size_t arrayItemIndex = 0; arrayItemIndex < totalArraySize; arrayItemIndex++)
            {
                if (totalArraySize > 1)
                {
                    if (arrayItemIndex == totalArraySize - 1)
                    {
                        //Last element of an array
                        desc.totalSize += baseSizeInBytes;
                    }
                    else
                    {
                        //Non-last element of an array
                        desc.totalSize += Allocator::AlignSizeUp(baseSizeInBytes, desc.memoryRowSizeInBytes);
                    }
                }
                else
                {
                    //Not an array
                    desc.totalSize += baseSizeInBytes;
                }
            }

            //std::cout << "  Element " << (int)element.elementID << " is at " << element.gpuPaddedOffset << std::endl;
        }

        void FinishPack(NumericStruct& desc)
        {
            //Calculate padded total size
            desc.paddedTotalSize = desc.totalSize; //Copy in case behavior is DEFAULT
            desc.paddedTotalSize = desc.GetPaddedTotalSize(desc.paddedTotalSizeBehavior);

            //std::cout << "  ->" << desc.typeName << " has total padded size " << desc.paddedTotalSize << std::endl;
            //std::cout << std::endl;
        }

    private:
        static size_t GetTypeArraySize(typename NumericStruct::Element& element)
        {
            size_t typeArraySize = 1; //The number of elements in the element.type

            if (NumericStructElementTypeUtilities::IsMatrix(element.type))
            {
                if (element.type == NumericStructElementType::FLOAT3x3)
                    typeArraySize = 3;
                else if (element.type == NumericStructElementType::FLOAT4x4)
                    typeArraySize = 4;
            }

            return typeArraySize;
        }

    private:
        size_t memoryRowSizeInBytes;
        size_t memoryTotalSizeAlignment;
    };

} }
