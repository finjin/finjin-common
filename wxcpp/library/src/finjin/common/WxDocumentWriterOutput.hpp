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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {
    
    class WxDocumentWriterOutput
    {
    public:
        virtual ~WxDocumentWriterOutput() {}
        
        virtual bool IsValid() const = 0;
        
        virtual void Write(const char* s) = 0;
        virtual void Write(const void* s, size_t byteCount) = 0;
        
        virtual uint64_t GetTotalBytesWritten() const = 0;

        virtual float GetMinFloat() const
        {
            return .00001f;
        }

        virtual double GetMinDouble() const
        {
            return GetMinFloat();
        }

        void WriteString(const wxString& s)
        {
            if (!s.empty())
            {
                auto s8 = s.ToUTF8();
                Write(s8.data(), strlen(s8.data()));
            }
        }
    };

    class WxFileDataChunkWriterOutput : public WxDocumentWriterOutput
    {
    public:
        WxFileDataChunkWriterOutput(std::shared_ptr<wxFile> _outFilePtr) : outFilePtr(_outFilePtr)
        {
            this->outFile = this->outFilePtr.get();
            Init();
        }

        WxFileDataChunkWriterOutput(std::shared_ptr<wxFile> _outFilePtr, wxFile& _outFile) : outFilePtr(_outFilePtr), outFile(&_outFile)
        {
            Init();
        }

        WxFileDataChunkWriterOutput(wxFile& _outFile) : outFile(&_outFile)
        {
            Init();
        }

        ~WxFileDataChunkWriterOutput() 
        {
        }
        
        bool IsValid() const override
        {
            return this->outFile != nullptr && this->outFile->IsOpened();
        }

        void Write(const char* s) override
        {
            if (s != nullptr)
                Write(s, strlen(s));
        }

        void Write(const void* s, size_t byteCount) override
        {
            if (this->outFile != nullptr)
            {
                this->outFile->Write(s, byteCount);
                this->totalBytesWritten += byteCount;
            }
        }

        uint64_t GetTotalBytesWritten() const override
        {
            return this->totalBytesWritten;
        }

    private:
        void Init()
        {
            this->totalBytesWritten = 0;;
        }

    private:
        wxFile* outFile;
        std::shared_ptr<wxFile> outFilePtr;
        uint64_t totalBytesWritten;
    };

    class StreamDataChunkWriterOutput : public WxDocumentWriterOutput
    {
    public:
        StreamDataChunkWriterOutput(std::shared_ptr<std::ostream> inPtr) : outStreamPtr(inPtr)
        {
            this->outStream = this->outStreamPtr.get();
            Init();
        }

        StreamDataChunkWriterOutput(std::shared_ptr<std::ostream> inPtr, std::ostream& in) : outStreamPtr(inPtr), outStream(&in)
        {
            Init();
        }

        StreamDataChunkWriterOutput(std::ostream& in) : outStream(&in)
        {
            Init();
        }

        ~StreamDataChunkWriterOutput() 
        {
        }
        
        bool IsValid() const override
        {
            return this->outStream != nullptr && *this->outStream;
        }

        void Write(const char* s) override
        {
            if (s != nullptr)
                Write(s, strlen(s));
        }

        void Write(const void* s, size_t byteCount) override
        {
            if (this->outStream != nullptr)
            {
                this->outStream->write(static_cast<const char*>(s), byteCount);
                this->totalBytesWritten += byteCount;
            }
        }

        uint64_t GetTotalBytesWritten() const override
        {
            return this->totalBytesWritten;
        }

    private:
        void Init()
        {
            this->totalBytesWritten = 0;
        }

    private:
        std::ostream* outStream;
        std::shared_ptr<std::ostream> outStreamPtr;
        uint64_t totalBytesWritten;
    };

} }
