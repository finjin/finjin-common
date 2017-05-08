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
#include "finjin/common/DataChunkReaderInput.hpp"
#include "finjin/common/VirtualFileHandle.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API VirtualFileHandleDataChunkReaderInput : public DataChunkReaderInput
    {
    public:
        VirtualFileHandleDataChunkReaderInput()
        {
            this->fileHandle = nullptr;
            this->totalBytesRead = 0;
        }

        void SetHandle(VirtualFileHandle* fileHandle)
        {
            this->fileHandle = fileHandle;
            this->totalBytesRead = 0;
        }

        bool IsValid() const override
        {
            return this->fileHandle != nullptr && this->fileHandle->IsOpen();
        }

        uint64_t Skip(uint64_t count) override
        {
            if (this->fileHandle != nullptr && this->fileHandle->IsOpen())
            {
                auto amountSkipped = this->fileHandle->Skip(count);
                this->totalBytesRead += amountSkipped;
                return amountSkipped;
            }
            else
                return 0;
        }

        size_t Read(void* bytes, size_t count) override
        {
            if (this->fileHandle != nullptr && this->fileHandle->IsOpen())
            {
                auto amountRead = this->fileHandle->Read(bytes, count);
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
        VirtualFileHandle* fileHandle;
        uint64_t totalBytesRead;
    };

} }
