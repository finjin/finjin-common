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
#include "finjin/common/Utf8String.hpp"
#include <unordered_map>
#include <vector>


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    /**
     * A string key/value map that can be addressed by index (over all the entries), or key (for particular entries).
     */
    class FINJIN_COMMON_LIBRARY_API StringKeyValueMap
    {
    public:
        /** Default constructor. */
        StringKeyValueMap();

        /** Copy constructor. */
        StringKeyValueMap(const StringKeyValueMap& other);

        /** Move constructor. */
        StringKeyValueMap(StringKeyValueMap&& other);

        ValueOrError<void> operator = (const StringKeyValueMap& other);
        ValueOrError<void> operator = (StringKeyValueMap&& other);

        StringKeyValueMap operator + (const StringKeyValueMap& other) const;
        ValueOrError<void> operator += (const StringKeyValueMap& other);

        /**
         * Gets the number of elements in the collection.
         * @return The number of elements in the collection.
         */
        size_t size() const;

        /**
         * Indicates whether the collection is empty.
         * @return If the collection is empty, true is returned. Otherwise, false is returned.
         */
        bool empty() const;

        /**
         * Indicates whether the map at least one key. The opposite of calling empty().
         * @return If the map has the specified key, true is returned. Otherwise, false is returned.
         */
        bool IsSet() const;
        
        /**
         * Removes all entries from the collection.
         */
        void clear();

        /** 
         * Gets the key at the specified index.
         * @param i [in] Index of the key to retrieve.
         * @return The key at the specified index.
         */
        const Utf8String& GetKey(size_t i) const;

        /**
         * Gets the value at the specified index.
         * @param i [in] Index of the value to retrieve.
         * @return The value at the specified index.
         */
        const Utf8String& GetValue(size_t i) const;
        
        class Value
        {
        public:            
            ValueOrError<void> operator = (const Value& other) { return map->Set(key, other); }            
            ValueOrError<void> operator = (const Utf8String& value) { return map->Set(key, value); }
            ValueOrError<void> operator += (const Value& other) { return map->Concat(key, other); }
            ValueOrError<void> operator += (const Utf8String& value) { return map->Concat(key, value); }
            operator const Utf8String& () const {return map->Get(key);}

        private:
            friend class StringKeyValueMap;
            Value();
            Value(const Value&) {}
            Value(StringKeyValueMap* _map, const Utf8String& _key) : map(_map), key(_key) {}

            StringKeyValueMap* map;
            Utf8String key;
        };

        /** 
         * Gets a modifiable value at the specified index. 
         * @param key [in] The key of the value to get.
         * @return A value that can be read and modified.
         */
        Value operator [] (const Utf8String& key);

        /**
         * Gets the value (by string) for the specified key.
         * @param key [in] The key of the value to get.
         * @return The value (by string) for the specified key.
         */
        const Utf8String& operator [] (const Utf8String& key) const;

        /**
         * Gets the value (by string) for the specified key.
         * @param key [in] The key of the value to get.
         * @param value [out] The value (by string) for the specified key.
         * @return If the key exists, true is returned. Otherwise, false is returned.
         */
        bool Get(const Utf8String& key, Utf8String& value) const;

        /**
         * Gets the value (by string) for the specified key.
         * @param key [in] The key of the value to get.
         * @return The value (by string) for the specified key.
         */
        const Utf8String& Get(const Utf8String& key) const;

        /**
         * Sets the value for the specified key.
         * @param key [in] The key to set.
         * @param value [in] The value of the key to set.
         */
        ValueOrError<void> Set(const Utf8String& key, const Utf8String& value);

        /**
         * Sets all of the keys/values in specified map into this one. Values for existing keys will be overwritten.
         * @param other [in] The other map whose keys/values will be set into this one.
         */
        ValueOrError<void> Set(const StringKeyValueMap& other);

        /**
         * Concatenates the value onto the specified key's value. If the key does not exist, it is added.
         * @param key [in] The key to set.
         * @param value [in] The value of the key to set.
         */
        ValueOrError<void> Concat(const Utf8String& key, const Utf8String& value);

        /**
         * Removes the specified key and its associated value.
         * @param key [in] The key to remove.
         * @return If the specified key existed, true is returned. Otherwise, false is returned.
         */
        bool Remove(const Utf8String& key);

        /** 
         * Indicates whether the map has the specified key.
         * @return If the map has the specified key, true is returned. Otherwise, false is returned.
         */
        bool HasKey(const Utf8String& key) const;

        /**
         * Joins all the keys of the map into a single string.
         * @param joinText [in] The text used to join the keys.
         * @return All the keys of the map as single string.
         */
        Utf8String JoinKeys(const Utf8String& joinText) const;
        ValueOrError<void> JoinKeys(Utf8String& result, const Utf8String& joinText) const;

        /**
         * Joins all the values of the map into a single string.
         * @param joinText [in] The text used to join the values.
         * @return All the values of the map as single string.
         */
        Utf8String JoinValues(const Utf8String& joinText) const;
        ValueOrError<void> JoinValues(Utf8String& result, const Utf8String& joinText) const;

        /**
         * Formats the key/value map as a string.
         * @return The key/value map as a string.
         */
        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

    private:
        using KeyValue = std::pair<Utf8String, Utf8String>;

        //Overall this implementation isn't very efficient but it's ok for the moment
        std::unordered_map<Utf8String, Utf8String> map;
        std::vector<KeyValue> vect;
    };

} }
