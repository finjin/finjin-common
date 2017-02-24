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
#include "finjin/common/ByteBuffer.hpp"
#include <istream>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class DataChunkReaderInput
    {
    public:
        virtual ~DataChunkReaderInput() {}
        
        virtual bool IsValid() const = 0;
        
        virtual uint64_t Skip(uint64_t count) = 0;
        
        virtual size_t Read(void* bytes, size_t count) = 0;

        virtual size_t ReadToEnd(ByteBuffer& bytes)
        {
            size_t amountRead = 0;
            
            uint8_t tempBuffer[CommonConstants::FILE_TEMP_BUFFER_SIZE];
            size_t readCount = 0;
            while ((readCount = Read(tempBuffer, sizeof(tempBuffer))) > 0)
            {
                bytes.Write(tempBuffer, readCount);
                amountRead += readCount;
            }
            
            return amountRead;
        }

        virtual uint64_t GetTotalBytesRead() const = 0;
    };

    class StreamDataChunkReaderInput : public DataChunkReaderInput
    {
    public:
        StreamDataChunkReaderInput(std::shared_ptr<std::istream> inPtr) : inStreamPtr(inPtr)
        {
            this->inStream = this->inStreamPtr.get();
            this->totalBytesRead = 0;
        }

        StreamDataChunkReaderInput(std::shared_ptr<std::istream> inPtr, std::istream& in) : inStreamPtr(inPtr), inStream(&in)
        {
            this->totalBytesRead = 0;
        }

        StreamDataChunkReaderInput(std::istream& in) : inStream(&in)
        {
            this->totalBytesRead = 0;
        }

        ~StreamDataChunkReaderInput() 
        {
        }
        
        bool IsValid() const override
        {
            return this->inStream != nullptr && *this->inStream;
        }

        uint64_t Skip(uint64_t count) override
        {
            if (this->inStream != nullptr)
            {
                auto before = this->inStream->tellg();
                this->inStream->seekg(count, std::ios_base::cur);
                auto after = this->inStream->tellg();
                auto amountRead = after - before;
                this->totalBytesRead += amountRead;
                return amountRead;
            }
            else
                return 0;
        }

        size_t Read(void* bytes, size_t count) override
        {
            if (this->inStream != nullptr)
            {
                auto amountRead = this->inStream->readsome(static_cast<char*>(bytes), count);
                this->totalBytesRead += amountRead;
                return amountRead;
            }
            else
                return 0;
        }

        uint64_t GetTotalBytesRead() const override
        {
            return this->totalBytesRead;
        }

    private:
        std::istream* inStream;
        std::shared_ptr<std::istream> inStreamPtr;
        uint64_t totalBytesRead;
    };

} }
