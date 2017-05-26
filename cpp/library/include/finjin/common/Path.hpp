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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/common/ValueOrError.hpp"
#include <iostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    //Note: This class borrows a lot of functionality from Utf8String.
    //If you make any changes there, you should probably make similar changes here
    class FINJIN_COMMON_LIBRARY_API Path
    {
    public:
        static const Path& GetEmpty();

        using traits_type = std::char_traits<char>;
        using value_type = char;
        using size_type = size_t;
        using difference_type = intptr_t;
        using pointer = Path*;
        using const_pointer = const Path*;
        using reference = Path&;
        using const_reference = const Path&;

        using iterator = char*;
        using const_iterator = const char*;

        using reverse_iterator = char*;
        using const_reverse_iterator = const char*;

        static const size_t npos = (size_t)-1;

        Path(Allocator* allocator = nullptr);
        Path(const char* other, Allocator* allocator = nullptr);
        Path(const wchar_t* other, Allocator* allocator = nullptr);
        Path(const char* other, size_t len, Allocator* allocator = nullptr);
        Path(const wchar_t* other, size_t len, Allocator* allocator = nullptr);
        Path(const char* first, const char* last, Allocator* allocator = nullptr);
        Path(const wchar_t* first, const wchar_t* last, Allocator* allocator = nullptr);
        Path(const uint8_t* first, const uint8_t* last, Allocator* allocator = nullptr);
        Path(size_t count, char c, Allocator* allocator = nullptr);
        Path(size_t count, Allocator* allocator);
        Path(const Utf8String& other, Allocator* allocator = nullptr);
        Path(Utf8String&& other);
        Path(const Utf8StringView& other, Allocator* allocator = nullptr);
        Path(const Path& other, Allocator* allocator = nullptr);
        Path(Path&& other);
        ~Path();

        bool Create(Allocator* allocator);
        void Destroy();

        Allocator* GetAllocator();
        bool SetAllocator(Allocator* allocator);

        ValueOrError<void> operator = (const char* other);
        ValueOrError<void> operator = (const wchar_t* other);
        ValueOrError<void> operator = (const Utf8String& other);
        ValueOrError<void> operator = (Utf8String&& other);
        ValueOrError<void> operator = (const Utf8StringView& other);
        ValueOrError<void> operator = (const Path& other);
        ValueOrError<void> operator = (Path&& other);

        ValueOrError<void> assign(const char* other);
        ValueOrError<void> assign(const wchar_t* other);
        ValueOrError<void> assign(const char* other, size_t len);
        ValueOrError<void> assign(const wchar_t* other, size_t len);
        ValueOrError<void> assign(const char* first, const char* last);
        ValueOrError<void> assign(const wchar_t* first, const wchar_t* last);
        ValueOrError<void> assign(const uint8_t* first, const uint8_t* last);
        ValueOrError<void> assign(size_t count, char c);
        ValueOrError<void> assign(const Utf8String& other);
        ValueOrError<void> assign(Utf8String&& other);
        ValueOrError<void> assign(const Utf8StringView& other);
        ValueOrError<void> assign(const Path& other);
        ValueOrError<void> assign(Path&& other);

        const char* c_str() const;

        bool empty() const;
        void clear();

        size_t length() const;
        size_t size() const;

        ValueOrError<void> reserve(size_t len);
        size_t GetCharactersLeft() const;

        ValueOrError<void> resize(size_t len);

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        template <typename T>
        ValueOrError<void> substr(T& result, size_t pos, size_t len = npos) const
        {
            result.clear();

            if (!empty() && pos < this->l)
                return result.assign(this->s + pos, std::min(len, this->l - pos));

            return ValueOrError<void>();
        }

        bool operator == (const Path& other) const;
        bool operator != (const Path& other) const;
        bool operator < (const Path& other) const;
        bool operator > (const Path& other) const;

        bool operator == (const Utf8String& other) const;
        bool operator != (const Utf8String& other) const;
        bool operator < (const Utf8String& other) const;
        bool operator > (const Utf8String& other) const;

        bool operator == (const char* other) const;
        bool operator != (const char* other) const;
        bool operator < (const char* other) const;
        bool operator > (const char* other) const;

        ValueOrError<void> operator += (Utf8String&& other);
        ValueOrError<void> operator += (const Utf8String& other);
        ValueOrError<void> operator += (const char* other);
        ValueOrError<void> operator += (const wchar_t* other);
        ValueOrError<void> operator += (char other);

        ValueOrError<void> append(Utf8String&& other);
        ValueOrError<void> append(const Utf8String& other);
        ValueOrError<void> append(const char* other);
        ValueOrError<void> append(const char* other, size_t len);
        ValueOrError<void> append(const wchar_t* other);
        ValueOrError<void> append(const wchar_t* other, size_t len);
        ValueOrError<void> append(char other);
        ValueOrError<void> append(wchar_t other);
        ValueOrError<void> append(size_t len, char c);

        ValueOrError<void> push_back(char c);
        char back() const;

        void pop_front();
        void pop_front(size_t count);
        void pop_back();
        void pop_back(size_t count);
        void pop_back(const char* other);

        size_t find(char c, size_t pos = 0) const;
        size_t find(const char* other, size_t pos = 0) const;
        size_t find(const Utf8String& other, size_t pos = 0) const;

        size_t rfind(char c, size_t pos = npos) const;
        size_t rfind(const char* other, size_t pos = npos) const;
        size_t rfind(const Utf8String& other, size_t pos = npos) const;

        char* erase(const char* at);
        char* erase(const char* from, const char* to);

        template <typename BeginIter, typename EndIter>
        ValueOrError<void> insert(char* at, BeginIter beginIt, EndIter endIt)
        {
            if (at == end())
            {
                for (auto iter = beginIt; iter != endIt; ++iter)
                {
                    if (push_back(*iter).HasError())
                        return ValueOrError<void>::CreateError();
                }
            }
            else if (at >= begin() && at < end() && beginIt != endIt)
            {
                Path newString(this->s, at - this->s, this->allocator); //Portion before 'at'
                for (auto iter = beginIt; iter != endIt; ++iter) //Inserted portion
                {
                    if (newString.append(*iter).HasError())
                        return ValueOrError<void>::CreateError();
                }
                if (newString.append(at).HasError()) //Portion after 'at'
                    return ValueOrError<void>::CreateError();

                assign(std::move(newString));
            }

            return ValueOrError<void>();
        }

        const char& operator [] (size_t i) const;
        char& operator [] (size_t i);

        void ReplaceFirst(char find, char replace);
        void ReplaceAll(char find, char replace);

        void ReplaceFirst(const Utf8String& find, const Utf8String& replace);
        void ReplaceAll(const Utf8String& find, const Utf8String& replace);

        void RemoveAllChars(const char* chars);
        void ReplaceAllChars(const char* chars, char replacement);

        int Compare(const Path& other) const;
        int Compare(const Utf8String& other) const;
        int Compare(const char* other) const;

        bool Equals(const Path& other) const;
        bool Equals(const Utf8String& other) const;
        bool Equals(const char* other) const;

        bool StartsWith(const Path& other) const;
        bool StartsWith(const Utf8String& other) const;
        bool StartsWith(const char* other) const;

        bool EndsWith(const Path& other) const;
        bool EndsWith(const Utf8String& other) const;
        bool EndsWith(const char* other) const;

        void TrimTrailingWhitespace();
        void RemoveWhitespace(size_t offset = 0);

        void EnsureTrailingSeparator();
        void RemoveTrailingSeparator();

        void UnifySeparators();
        void RemoveLeadingSeparators();

        bool IsStatic() const;

        size_t RepairLength(size_t newLength = (size_t)-1);

        bool IterateCodepoint(size_t& iter, uint32_t& codepoint) const;
        bool GetCodepointCount(size_t& count) const;

        /**
         * Converts the UTF8 string to UTF32.
         * @param output [out] - The output codepoints. This should point to an array at least length() + 1.
         * @param outputLength [in/out] - On input, the number of elements pointed to by 'output'. On output, the length of the output string, not counting the terminating character.
         * If this value is the same as the input length, it indicates that the 'output' array was not long enough and contains a truncated result.
         */
        bool GetCodepoints(uint32_t* output, size_t& outputLength) const;

        /**
         * Concatenates the specified path onto this one.
         * @param other [in] The path to concatenate onto this one.
         * @return A reference to this path.
         */
        ValueOrError<void> operator /= (const Path& other);
        ValueOrError<void> operator /= (const Utf8String& other);
        ValueOrError<void> operator /= (const Utf8StringView& other);
        ValueOrError<void> operator /= (const char* other);
        ValueOrError<void> operator /= (const wchar_t* other);

        size_t GetHash() const;

        /**
         * Converts the path to a string.
         * @return The path as a string.
         */
        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

        Path& UniversalNormalize();

        Path& PlatformNormalize();

        ValueOrError<void> NormalizeRelativeComponents(Path& result) const;

        /**
         * Indicates whether the path is absolute.
         * @return If the path is absolute, true is returned. Otherwise, false is returned.
         */
        bool IsAbsolute() const;

        /**
         * Indicates whether the path is relative.
         * @return If the path is relative, true is returned. Otherwise, false is returned.
         */
        bool IsRelative() const;

        bool HasExtension() const;

        size_t GetExtensionOffset(bool includeDot) const;

        /**
         * Gets the path's file extension.
         * @return The path's file extension.
         */
        template <typename T>
        ValueOrError<bool> GetExtension(T& result, bool includeDot) const
        {
            result.clear();

            auto extFoundAt = GetExtensionOffset(includeDot);
            if (extFoundAt != npos)
            {
                if (result.assign(this->s + extFoundAt, this->l - extFoundAt).HasError())
                    return ValueOrError<bool>::CreateError();
                else
                    return !result.empty();
            }

            return false;
        }

        void RemoveExtension();

        /**
         * Gets the path's file name with extension.
         * @return The path's file name with extension.
         */
        template <typename T>
        ValueOrError<bool> GetFileName(T& result) const
        {
            result.clear();

            for (size_t i = length() - 1; i != npos; i--)
            {
                if (IsSeparator(this->s[i]) || this->s[i] == ':')
                {
                    if (result.assign(&this->s[i + 1]).HasError())
                        return ValueOrError<bool>::CreateError();
                    else
                        return !result.empty();
                }
            }

            if (result.assign(this->s).HasError())
                return ValueOrError<bool>::CreateError();
            else
                return !result.empty();
        }

        size_t GetFileNameLength() const;

        /**
         * Gets the path's file name without extension.
         * @return The path's file name without extension.
         */
        template <typename T>
        ValueOrError<bool> GetBaseName(T& result) const
        {
            result.clear();

            size_t extensionIndex = npos;

            for (size_t i = length() - 1; i != npos; i--)
            {
                if (this->s[i] == '.')
                {
                    if (extensionIndex == npos)
                        extensionIndex = i;
                }
                else if (IsSeparator(this->s[i]) || this->s[i] == ':')
                {
                    if (extensionIndex != npos)
                    {
                        if (substr(result, i + 1, extensionIndex - i - 1).HasError())
                            return ValueOrError<bool>::CreateError();
                        else
                            return !result.empty();
                    }
                    else
                    {
                        if (result.assign(&this->s[i + 1]).HasError())
                            return ValueOrError<bool>::CreateError();
                        else
                            return !result.empty();
                    }
                }
            }

            if (extensionIndex != npos)
            {
                if (substr(result, 0, extensionIndex).HasError())
                    return ValueOrError<bool>::CreateError();
                else
                    return !result.empty();
            }
            else
            {
                if (result.assign(this->s).HasError())
                    return ValueOrError<bool>::CreateError();
                else
                    return !result.empty();
            }
        }

        void RemoveFileName();

        size_t GetParentOffset() const;

        /**
         * Indicates whether the path has a parent path.
         * @return If the path has a parent path, true is returned. Otherwise, false is returned.
         */
        bool HasParent() const;

        /**
         * Gets the path's parent.
         * @return The path's parent.
         */
        template <typename T>
        ValueOrError<bool> GetParent(T& result) const
        {
            result.clear();
            
            if (empty())
                return false;
            
            size_t endIndex = npos;
            if (HasLongPathPrefix(this->s))
                endIndex = GetLongPathPrefixLength() - 1;
            
            size_t i = length() - 1;
            if (endIndex != npos && i <= endIndex)
                return false;
            if (IsSeparator(this->s[i]) || this->s[i] == ':')
                i--;
            for (; i != endIndex; i--)
            {
                if (IsSeparator(this->s[i]) || this->s[i] == ':')
                {
                    if (substr(result, 0, i).HasError())
                        return ValueOrError<bool>::CreateError();
                    else
                        return !result.empty();
                }
            }
            
            return false;
        }

        bool GoToParent();
        bool RemoveParent();

        template <typename T>
        ValueOrError<bool> GetRoot(T& result) const
        {
            result.clear();
            
        #if FINJIN_TARGET_PLATFORM_IS_WINDOWS
            size_t index = 0;
            if (HasLongPathPrefix(this->s))
                index += GetLongPathPrefixLength();
            if (::IsAbsolute(begin() + index, end()))
            {
                if (IsSeparator(this->s[index]) && IsSeparator(this->s[index + 1]))
                {
                    size_t startIndex = index;
                    
                    index += 2;
                    while (index < this->l && !IsSeparator(this->s[index]))
                        index++;
                    
                    if (substr(result, startIndex, index - startIndex).HasError())
                        return ValueOrError<bool>::CreateError();
                    
                    return !result.empty();
                }
                else if (this->s[index + 1] == ':')
                {
                    if (substr(result, index, index + 2).HasError())
                        return ValueOrError<bool>::CreateError();
                    
                    return !result.empty();
                }
            }
        #else
            if (IsAbsolute())
            {
                if (result.assign(GetPlatformIndependentSeparatorString()).HasError())
                    return ValueOrError<bool>::CreateError();
                
                return !result.empty();
            }
        #endif
            
            return false;
        }

        ValueOrError<bool> GetInternalVolumeID(Utf8String& result) const;

        ValueOrError<bool> EnsureExtension(const Utf8String& defaultExtension);
        ValueOrError<bool> EnsureExecutableExtension();

        size_t FindFirstDirectoryLength() const;
        size_t FindNextDirectoryLength(size_t previousLength) const;

        /**
         * Determines whether this path contains the specified directory.
         * Note that the containment check is only done logically - no checking of the file system occurs.
         * @return If this path contains the specified directory, true is returned. Otherwise, false is returned.
         */
        ValueOrError<bool> ContainsDirectory(const Path& other) const;

        /**
         * Determines whether this path contains the specified file.
         * Note that the containment check is only done logically - no checking of the file system occurs.
         * @return If this path contains the specified file, true is returned. Otherwise, false is returned.
         */
        ValueOrError<bool> ContainsFile(const Path& other) const;

        static const char* GetPlatformIndependentSeparatorString();
        
        static bool HasLongPathPrefix(const char* s);
        
        static const char* GetLongPathPrefix();
        
        static size_t GetLongPathPrefixLength();
        
        template <typename T>
        static bool IsSeparator(T c)
        {
            return c == '/' || c == '\\';
        }
        
        //File system methods--------------------------------------------------

        ValueOrError<bool> ExpandUserHomeDirectory();

        static ValueOrError<bool> GetUserHomeDirectory(Path& directory);

        uint64_t GetFileLength() const;
        bool GetFileLength(uint64_t& fileLength) const;

        bool WriteBinaryFile(const void* bytes, size_t byteCount) const;

        template <typename VectorType>
        bool ReadBinaryFile(VectorType& buffer) const
        {
            size_t readByteCount;
            buffer.maximize();
            if (ReadBinaryFile(buffer.data(), buffer.max_size(), readByteCount))
            {
                buffer.resize(readByteCount);
                return true;
            }
            else
            {
                buffer.clear();
                return false;
            }
        }

        bool ReadBinaryFile(uint8_t* buffer, size_t maxByteCount, size_t& readByteCount) const;
        bool ReadBinaryFile(ByteBuffer& buffer) const;

        //Deletes the file from the file system
        bool RemoveFile() const;

        //Renames the file to the new name.
        bool RenameFile(const Path& destPath) const;

        /**
         * Indicates whether the path is a file.
         * @return If the path is a file, true is returned. Otherwise, false is returned.
         */
        bool IsFile() const;

        static bool IsFile(const char* path);

        /**
         * Indicates whether the path is a directory.
         * @return If the path is a directory, true is returned. Otherwise, false is returned.
         */
        bool IsDirectory() const;

        static bool IsDirectory(const char* path);

        /**
         * Indicates whether the path exists in the file system.
         * @return If the path exists in the file system, true is returned. Otherwise, false is returned.
         */
        bool Exists() const;

        bool CreateDirectories() const;

        static void InitializeUtf8FileSystemAccess();

        enum { STATIC_STRING_LENGTH = 479 };

    private:
        void Init();

        ValueOrError<void> EnsureLengthAllocated(size_t len, bool oversize);
        void Truncate();
        void Free();
        
        static bool IsAbsolute(const char* begin, const char* end);

        char* _Allocate(size_t charCount, FINJIN_CALLER_PARAMETERS_DECLARATION);
        void _Deallocate(void* mem);
        
    protected:
        Allocator* allocator;
        size_t l;
        size_t allocatedLength;
        char* s;
        char shortS[STATIC_STRING_LENGTH + 1];
    };

} }


namespace std
{
    template <>
    struct hash<Finjin::Common::Path>
    {
        size_t operator () (const Finjin::Common::Path& s) const
        {
            return s.GetHash();
        }
    };
}


//Functions---------------------------------------------------------------------
namespace std
{
    inline bool empty(const Finjin::Common::Path& v)
    {
        return v.empty();
    }

    inline istream& operator >> (istream& is, Finjin::Common::Path& v)
    {
         char c;
         while (is)
         {
             is.get(c);
             v.append(c);
         }
         return is;
    }

    inline ostream& operator << (ostream& os, const Finjin::Common::Path& v)
    {
        os << v.c_str();
        return os;
    }

    inline wistream& operator >> (wistream& is, Finjin::Common::Path& v)
    {
         wchar_t c;
         while (is)
         {
             is.get(c);
             v.append(c);
         }
         return is;
    }

    inline wostream& operator << (wostream& os, const Finjin::Common::Path& v)
    {
        os << v.c_str();
        return os;
    }
}
