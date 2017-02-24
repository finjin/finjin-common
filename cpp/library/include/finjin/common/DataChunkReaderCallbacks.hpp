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
#include "finjin/common/ChunkName.hpp"
#include "finjin/common/DataChunkReader.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class DataChunkReaderCallbacksState
    {
    public:
        DataChunkReaderCallbacksState() { this->currentMapping = nullptr; this->bufferValuesRead = 0; this->propertyLengthHint = 0;}
        ~DataChunkReaderCallbacksState() {}

        size_t GetDepth() const { return this->parsedChunkNameStack.size(); }

        void ResetForChunkStart() { this->bufferValuesRead = 0; this->propertyLengthHint = 0; }

    public:
        StaticVector<ParsedChunkName, CommonConstants::MAX_DATA_CHUNK_DEPTH> parsedChunkNameStack;

        const void* currentMapping;

        size_t bufferValuesRead; //Utility variable to help with reading multiple line values into a buffer
        size_t propertyLengthHint; //Hint that is indicates the full length of the following blob or string property
    };

    template <typename StateType>
    struct DataChunkReaderCallbacksChunkMapping
    {
        struct PatternComponent
        {
            PatternComponent()
            {
                this->chunkName = nullptr;
                this->isIndexed = false;
                this->index = (ChunkName::Index)-1;
            }

            PatternComponent(const ChunkName& chunkName)
            {
                //Note that the specified chunk name is NOT copied. Instead, a pointer to it is maintained
                //For this reason, chunkName must be statically defined somewhere

                this->chunkName = &chunkName;
                this->isIndexed = false;
                this->index = (ChunkName::Index)-1;
            }

            static PatternComponent Indexed(ChunkName::Index index = (ChunkName::Index)-1)
            {
                PatternComponent component;
                component.isIndexed = true;
                component.index = index;
                return component;
            }

            bool Matches(const ParsedChunkName& other) const
            {
                if (this->isIndexed)
                {
                    if (this->index != (ChunkName::Index)-1)
                        return other.index == this->index;
                    else
                        return other.index != (ParsedChunkName::Index)-1;
                }
                else
                {
                    assert(this->chunkName != nullptr);
                    return other == *this->chunkName;
                }
            }

            const ChunkName* chunkName;
            bool isIndexed;
            ChunkName::Index index;
        };

        struct Pattern : StaticVector<PatternComponent, CommonConstants::MAX_DATA_CHUNK_DEPTH>
        {
            using Super = StaticVector<PatternComponent, CommonConstants::MAX_DATA_CHUNK_DEPTH>;

            ValueOrError<bool> AddName(const ChunkName& chunkName)
            {
                return Super::push_back(PatternComponent(chunkName));
            }

            ValueOrError<bool> AddIndex(ChunkName::Index index = (ChunkName::Index)-1)
            {
                return Super::push_back(PatternComponent::Indexed(index));
            }

            template <typename... Args>
            ValueOrError<bool> Add(const ChunkName& chunkName, const Args&... args)
            {
                auto addResult = AddName(chunkName);
                if (addResult.HasErrorOrValue(false))
                    return addResult;

                return Add(args...);
            }

            template <typename... Args>
            ValueOrError<bool> Add(ChunkName::Index index, const Args&... args)
            {
                auto addResult = AddIndex(index);
                if (addResult.HasErrorOrValue(false))
                    return addResult;

                return Add(args...);
            }

            ValueOrError<bool> Add()
            {
                //No-op
                return true;
            }
        };

        bool IsDefault() const
        {
            return this->pattern.empty();
        }

        Pattern pattern;
        std::function<void(bool, const ChunkName*, const ParsedChunkName&, DataChunkReader&, StateType&, Error&)> chunkCallback;
        std::function<void(const ChunkName*, const ParsedChunkName*, ParsedChunkPropertyName&, DataChunkReader&, DataHeader&, StateType&, Error&)> propertyCallback;
    };
    
    template <typename StateType, size_t mappingCount>
    class DataChunkReaderCallbacks
    {
    public:
        using State = StateType;
        
        DataChunkReaderCallbacks() {}
        ~DataChunkReaderCallbacks() {}

        void AddMapping(const DataChunkReaderCallbacksChunkMapping<StateType>& mapping, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->mappings.full())
            {
                FINJIN_SET_ERROR(error, "Failed to add mapping. Maximum number of mappings has been reached.");
                return;
            }

            this->mappings.push_back(mapping);
        }

        const StaticVector<DataChunkReaderCallbacksChunkMapping<StateType>, mappingCount>& GetMappings() const 
        { 
            return this->mappings; 
        }

        bool HandleItem(DataChunkReader& reader, DataHeader& dataHeader, StateType& state, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            auto result = HandleItem(nullptr, reader, dataHeader, state, error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
            return result;
        }

        bool HandleItem(std::ostream* debugOutput, DataChunkReader& reader, DataHeader& dataHeader, StateType& state, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            auto continueReading = true;

            switch (dataHeader.type)
            {
                case DataHeaderType::CHUNK_START:
                {
                    auto parsedChunk = dataHeader.GetChunkStart();

                    if (debugOutput != nullptr)
                        *debugOutput << "Encountered chunk start: " << parsedChunk.ToString() << std::endl;

                    HandleChunkStart(parsedChunk, reader, state, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to process chunk start '%1%' via callback.", parsedChunk.ToString()));
                        continueReading = false;
                    }
                    break;
                }
                case DataHeaderType::CHUNK_END:
                {
                    if (debugOutput != nullptr)
                        *debugOutput << "Encountered chunk end." << std::endl;

                    HandleChunkEnd(reader, state, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to process chunk end via callback.");
                        continueReading = false;
                    }
                    break;
                }
                case DataHeaderType::PROPERTY:
                {
                    if (debugOutput != nullptr)
                        *debugOutput << DataHeader::OccurrenceUtilities::ToString(dataHeader.occurrence) << ": ";

                    HandleProperty(reader, dataHeader, state, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to process chunk property via callback.");
                        continueReading = false;
                    }

                    break;
                }
                case DataHeaderType::END:
                {
                    if (debugOutput != nullptr)
                        *debugOutput << "Encountered end." << std::endl;

                    continueReading = false;

                    break;
                }
                default: break;
            }

            return continueReading;
        }
        
        void ReadDocument(std::ostream* debugOutput, DataChunkReader& reader, StateType& state, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            DataHeader dataHeader;
            reader.ReadReaderHeader(dataHeader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read reader header.");
                return;
            }
            else
            {
                if (debugOutput != nullptr)
                {
                    *debugOutput << "Format = " << reader.GetHeader().format << std::endl;
                    *debugOutput << "Version = " << reader.GetHeader().version << std::endl;
                    *debugOutput << "Max bytes per line = " << reader.GetHeader().maxBytesPerLine << std::endl;
                    *debugOutput << "Blob text format = " << DataChunkBlobTextFormatUtilities::ToString(reader.GetHeader().blobTextFormat) << std::endl;
                }

                auto continueReading = true;
                while (continueReading)
                {
                    reader.ReadDataHeader(dataHeader, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to read data header.");
                        continueReading = false;
                    }
                    else
                    {
                        continueReading = HandleItem(debugOutput, reader, dataHeader, state, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to handle data item.");
                            continueReading = false;
                        }
                    }
                }
            }
        }

    private:
        void HandleChunkStart(const ParsedChunkName& chunkName, DataChunkReader& reader, StateType& state, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            if (state.parsedChunkNameStack.full())
            {
                FINJIN_SET_ERROR(error, "Failed to process chunk start. Maximum chunk depth has been reached.");
                return;
            }

            state.parsedChunkNameStack.push_back(chunkName);

            UpdateCurrentMapping(state);

            auto currentMapping = static_cast<const DataChunkReaderCallbacksChunkMapping<State>*>(state.currentMapping);
            if (currentMapping != nullptr && currentMapping->chunkCallback != nullptr)
            {
                state.ResetForChunkStart();

                currentMapping->chunkCallback(true, currentMapping->pattern.back().chunkName, chunkName, reader, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to process chunk start in callback.");
                    return;
                }
            }
        }

        void HandleChunkEnd(DataChunkReader& reader, StateType& state, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            if (state.parsedChunkNameStack.empty())
            {
                FINJIN_SET_ERROR(error, "Failed to process chunk end. The chunk stack is already empty.");
                return;
            }

            auto currentMapping = static_cast<const DataChunkReaderCallbacksChunkMapping<State>*>(state.currentMapping);
            if (currentMapping != nullptr && currentMapping->chunkCallback != nullptr)
            {
                currentMapping->chunkCallback(false, currentMapping->pattern.back().chunkName, state.parsedChunkNameStack.back(), reader, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to process chunk end in callback.");
                    return;
                }
            }

            state.parsedChunkNameStack.pop_back();

            UpdateCurrentMapping(state);
        }

        void HandleProperty(DataChunkReader& reader, DataHeader& dataHeader, StateType& state, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            ParsedChunkPropertyName propertyName;
            reader.ReadPropertyName(dataHeader, propertyName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read propertyName.");
                return;
            }

            if (state.currentMapping != nullptr)
            {
                auto currentMapping = static_cast<const DataChunkReaderCallbacksChunkMapping<State>*>(state.currentMapping);
                if (currentMapping->propertyCallback != nullptr)
                {
                    if (this->lengthHintPropertyName.IsValid() && propertyName == this->lengthHintPropertyName)
                    {
                        reader.ReadCount(dataHeader, state.propertyLengthHint, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", this->lengthHintPropertyName.name));
                            return;
                        }
                    }
                    else
                    {
                        currentMapping->propertyCallback(currentMapping->pattern.back().chunkName, &state.parsedChunkNameStack.back(), propertyName, reader, dataHeader, state, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to process property in callback.");
                            return;
                        }
                    }
                }
            }
            else if (state.parsedChunkNameStack.empty() && !this->mappings.empty() && this->mappings[0].IsDefault())
            {
                //Not within a chunk, and there is a default mapping
                this->mappings[0].propertyCallback(nullptr, nullptr, propertyName, reader, dataHeader, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to process value in value callback.");
                    return;
                }
            }

            reader.Skip(dataHeader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to skip property.");
                return;
            }
        }
        
        void UpdateCurrentMapping(StateType& state) const
        {
            state.currentMapping = nullptr;

            if (state.parsedChunkNameStack.empty())
                return;

            for (auto& mapping : this->mappings)
            {
                if (!mapping.IsDefault() && mapping.pattern.size() <= state.parsedChunkNameStack.size())
                {
                    size_t matchCount = 0;
                    auto parsedChunkStackPatternStart = &state.parsedChunkNameStack[state.parsedChunkNameStack.size() - mapping.pattern.size()];
                    for (auto parsedChunk = parsedChunkStackPatternStart; parsedChunk != state.parsedChunkNameStack.end(); parsedChunk++)
                    {
                        if (!mapping.pattern[parsedChunk - parsedChunkStackPatternStart].Matches(*parsedChunk))
                            break;

                        matchCount++;
                    }

                    if (matchCount == mapping.pattern.size())
                    {
                        state.currentMapping = &mapping;
                        break;
                    }
                }
            }
        }

    private:
        StaticVector<DataChunkReaderCallbacksChunkMapping<StateType>, mappingCount> mappings;

    protected:
        ChunkPropertyName lengthHintPropertyName;
    };

} }
