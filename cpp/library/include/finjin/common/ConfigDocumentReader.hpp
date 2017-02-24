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
#include "finjin/common/Utf8String.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class ConfigDocumentReader;
    class ByteBufferReader;

    class ConfigDocumentLine
    {
        friend class ConfigDocumentReader;

    public:
        ConfigDocumentLine();

        enum class Type
        {
            NONE,
            COMMENT,
            SECTION,
            SCOPE_START,
            SCOPE_END,
            KEY_AND_VALUE,
            PLAIN_LINE
        };

        Type GetType() const;

        int GetDepth() const;

        //String methods
        void GetComment(Utf8String& comment) const;
        Utf8String GetComment() const;

        void GetSectionName(Utf8String& name) const;
        Utf8String GetSectionName() const;

        Utf8String GetKey() const;
        Utf8String GetValue() const;
        void GetKeyAndValue(Utf8String& key, Utf8String& value) const;
        void GetKeyAndValue(Utf8StringView& key, Utf8StringView& value) const;

        void GetLine(Utf8String& line) const;
        Utf8String GetLine() const;

        Utf8StringView& GetComment(Utf8StringView& comment) const;
        Utf8StringView& GetSectionName(Utf8StringView& name) const;
        Utf8StringView& GetKey(Utf8StringView& key) const;
        Utf8StringView& GetValue(Utf8StringView& value) const;
        Utf8StringView& GetLine(Utf8StringView& line) const;

        //String pointer methods - The returned string is not null terminated
        const char* GetComment(size_t& length) const;
        const char* GetSectionName(size_t& length) const;
        const char* GetScope(size_t& length) const;
        const char* GetKey(size_t& length) const;
        const char* GetValue(size_t& length) const;        
        const char* GetLine(size_t& length) const;

    private:
        ConfigDocumentReader* reader;

        Type type;

        int depth;

        const char* lineBegin;
        const char* lineEnd;

        const char* keyBegin;
        const char* keyEnd;

        const char* valueBegin;
        const char* valueEnd;
    };

    class ConfigDocumentReader
    {
    public:
        ConfigDocumentReader();
        
        void Reset();

        ConfigDocumentLine* Start(const ByteBufferReader& buffer);
        ConfigDocumentLine* Start(const Utf8String& s);
        ConfigDocumentLine* Start(const char* text, size_t length);
        ConfigDocumentLine* Start(const char* begin, const char* end);
        ConfigDocumentLine* Restart(const ConfigDocumentLine& line);
        
        ConfigDocumentLine* Current();
        ConfigDocumentLine* Next();
        
        void SkipScope();

        template <typename T>
        bool SkipToSection(const T& searchName)
        {
            for (auto line = Current(); line != nullptr; line = Next())
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::SECTION:
                    {
                        Utf8StringView sectionName;
                        line->GetSectionName(sectionName);

                        if (sectionName == searchName)
                            return true;
                    }
                    default: break;
                }
            }

            return false;
        }

        /** 
         * Gets the number of sections with the specified name, starting from the current item. All depths at or anywhere beneath the current item are considered.
         * @param name [in] - The name of the section to match. If empty, all sections are matched.
         */
        template <typename T>
        size_t GetSectionCount(const T& name)
        {
            size_t count = 0;
            
            auto startLine = this->line;
            
            auto depth = this->line.depth;
            
            for (auto line = Current(); line != nullptr; line = Next())
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::SECTION:
                    {
                        if (std::empty(name))
                            count++;
                        else
                        {
                            Utf8StringView sectionName;
                            line->GetSectionName(sectionName);
                            if (sectionName == name)
                                count++;
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
                        if (depth < startLine.depth)
                        {
                            Reinitialize(startLine);
                            return count;
                        }
                        break;
                    }
                    default: break;
                }
            }
            
            Reinitialize(startLine);
            return count;
        }

        /** 
         * Gets the number of sections with the specified name, starting from the current item and going only one level beneath the current item.
         * This is generally called directly after encountering a particular section in order to calculate the number of subsections directly beneath it.
         * @param name [in] - The name of the section to match. If empty, all sections are matched.
         */
        template <typename T>
        size_t GetSectionCountBeneathCurrent(const T& name)
        {
            size_t count = 0;
            
            auto startLine = this->line;
            
            auto depth = this->line.depth;
            
            for (auto line = Current(); line != nullptr; line = Next())
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::SECTION:
                    {
                        Utf8StringView sectionName;
                        line->GetSectionName(sectionName);
                        
                        if (depth == startLine.depth + 1)
                        {
                            if (name.empty())
                                count++;
                            else
                            {                                
                                if (sectionName == name)
                                    count++;
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
                        if (depth <= startLine.depth)
                        {
                            Reinitialize(startLine);
                            return count;
                        }
                        break;
                    }
                    default: break;
                }
            }
            
            Reinitialize(startLine);
            return count;
        }

    private:
        ConfigDocumentLine* Initialize(const char* begin, const char* end);
        ConfigDocumentLine* Reinitialize(const ConfigDocumentLine& line);

        ConfigDocumentLine* ProcessLine();
        
    private:
        ConfigDocumentLine line;

        const char* textBegin;
        const char* textEnd;        
    };

} }
