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


//Macros------------------------------------------------------------------------
#define FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(head, next, value) \
    for (auto value = head; value != nullptr; value = value->next)

#define FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(head, next, value) \
    { \
        auto _headTemp = head; \
        head = value; \
        head->next = _headTemp; \
    }

#define FINJIN_INTRUSIVE_SINGLE_LIST_ADD_UNIQUE_ITEM(head, next, value) \
    { \
        auto found = false; \
        FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(head, next, innerValue) \
        { \
            if (value == innerValue) \
            { \
                found = true; \
                break; \
            } \
        } \
        if (!found) \
        { \
            FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(head, next, value) \
        } \
    }

#define FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(head, next, value) \
    if (head == nullptr) \
    { \
    } \
    else if (value == head) \
    { \
        head = head->next; \
    } \
    else \
    { \
        auto previousIter = head; \
        for (auto currentIter = head->next; currentIter != nullptr; currentIter = currentIter->next) \
        { \
            if (value == currentIter) \
            { \
                previousIter->next = currentIter->next; \
                currentIter->next = nullptr; \
                break; \
            } \
            previousIter = currentIter; \
        } \
    }


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Common {

    template <typename T>
    class NextAccessor
    {
    public:
        using value_type = T;

        static inline T* GetNext(T* item) { return item ? item->next : nullptr; }
        static inline const T* GetNext(const T* item) { return item ? item->next : nullptr; }

        static inline void SetNext(T* item, T* next) { item->next = next; }

        static inline value_type& GetReference(T* item) { return *item; }
        static inline value_type* GetPointer(T* item) { return item; }

        static inline const value_type& GetConstReference(const T* item) { return *item; }
        static inline const value_type* GetConstPointer(const T* item) { return item; }
    };

    /**
     * Wraps access to a class's 'next' pointer, creating a more convenient list interface
     * T is to be the class itself, though nodes in list are stored as pointers, and then exposed through the iterator interface as a reference type (& not *)
     * For example, for a struct defined as:
     *   struct Node
     *   {
     *      Node* next;
     *   };
     * The IntrusiveList would be defined as:
     *   IntrusiveList<Node, NextAccessor<Node>> nodeList;
     * Then to iterate over this list (note the reference type 'node'):
     *   for (auto& node : nodeList)
     *   {
     *   }
     */
    template <typename T, typename Accessor>
    struct IntrusiveSingleList
    {
        using value_type = T*;

        IntrusiveSingleList(IntrusiveSingleList& other) = delete;
        IntrusiveSingleList& operator = (IntrusiveSingleList& other) = delete;

        IntrusiveSingleList() { this->head = this->tail = nullptr; this->deleteOnClear = false; }
        IntrusiveSingleList(bool deleteOnClear) { this->head = this->tail = nullptr; this->deleteOnClear = deleteOnClear; }
        IntrusiveSingleList(T* head, bool deleteOnClear) { this->head = this->tail = head; this->deleteOnClear = deleteOnClear; }
        IntrusiveSingleList(T* head, T* tail, bool deleteOnClear) { this->head = head; this->tail = tail; this->deleteOnClear = deleteOnClear; }
        IntrusiveSingleList(IntrusiveSingleList&& other)
        {
            this->head = other.head;
            this->tail = other.tail;

            this->deleteOnClear = other.deleteOnClear;

            other.head = nullptr;
            other.tail = nullptr;
        }

        IntrusiveSingleList& operator = (IntrusiveSingleList&& other)
        {
            clear();

            this->head = other.head;
            this->tail = other.tail;

            this->deleteOnClear = other.deleteOnClear;

            other.head = nullptr;
            other.tail = nullptr;

            return *this;
        }

        ~IntrusiveSingleList() { clear(); }

        void assign(T* head)
        {
            T* tail = nullptr;

            //Find tail
            if (head != nullptr)
            {
                auto i = head;
                for (; Accessor::GetNext(i) != nullptr; i = Accessor::GetNext(i))
                {
                }
            }

            assign(head, tail);
        }

        void assign(T* head, T* tail)
        {
            clear();

            this->head = head;
            this->tail = tail;
        }

        class iterator
        {
        public:
            iterator() { this->item = nullptr; }
            iterator(T* item) { this->item = item; }

            iterator& operator ++ () { if (this->item != nullptr) this->item = Accessor::GetNext(this->item); return *this; }
            iterator operator ++ (int) { auto result = *this; if (this->item != nullptr) { this->item = Accessor::GetNext(this->item); } return result; }

            bool operator == (iterator other) const { return this->item == other.item; }
            bool operator != (iterator other) const { return this->item != other.item; }

            typename Accessor::value_type& operator * () { return Accessor::GetReference(this->item); }
            typename Accessor::value_type* operator -> () { return Accessor::GetPointer(this->item); }

        private:
            T* item;
        };

        class const_iterator
        {
        public:
            const_iterator() { this->item = nullptr; }
            const_iterator(const T* item) { this->item = item; }

            const_iterator& operator ++ () { if (this->item != nullptr) this->item = Accessor::GetNext(this->item); return *this; }
            const_iterator operator ++ (int) { const_iterator result; if (this->item != nullptr) { result.item = this->item; this->item = Accessor::GetNext(this->item); } return result; }

            bool operator == (const_iterator other) const { return this->item == other.item; }
            bool operator != (const_iterator other) const { return this->item != other.item; }

            const typename Accessor::value_type& operator * () const { return Accessor::GetConstReference(this->item); }
            const typename Accessor::value_type* operator -> () const { return Accessor::GetConstPointer(this->item); }

        private:
            const T* item;
        };

        iterator begin() { return this->head; }
        iterator end() { return iterator(); }

        const_iterator begin() const { return this->head; }
        const_iterator end() const { return const_iterator(); }

        size_t size() const
        {
            size_t count = 0;
            for (auto item = this->head; item != nullptr; item = Accessor::GetNext(item))
                count++;
            return count;
        }

        void push_front(T* item)
        {
            if (this->head == nullptr)
                this->head = this->tail = item;
            else
            {
                auto headTemp = this->head;
                this->head = item;
                Accessor::SetNext(this->head, headTemp);
            }
        }

        size_t push_back(T* item)
        {
            if (item == nullptr)
                return 0;

            size_t count = 1;
            for (auto counter = Accessor::GetNext(item); counter != nullptr; counter = Accessor::GetNext(item))
                count++;

            if (this->head == nullptr)
                this->head = this->tail = item;
            else
            {
                Accessor::SetNext(this->tail, item);
                this->tail = item;
            }

            return count;
        }

        bool empty() const
        {
            return this->head == nullptr;
        }

        void clear()
        {
            if (this->deleteOnClear)
            {
                for (auto item = this->head; item != nullptr;)
                {
                    auto next = Accessor::GetNext(item);
                    delete item;
                    item = next;
                }
            }
            this->head = this->tail = nullptr;
        }

        IntrusiveSingleList SpliceFront(size_t maxCount, size_t* outputCount = nullptr)
        {
            if (maxCount == 0 || this->head == nullptr)
            {
                //Return none
                if (outputCount != nullptr)
                    *outputCount = 0;
                return IntrusiveSingleList(nullptr, nullptr, this->deleteOnClear);
            }
            else if (maxCount == (size_t)-1)
            {
                //Return all
                auto newHead = this->head;
                auto newTail = this->tail;
                this->head = this->tail = nullptr;
                if (outputCount != nullptr)
                    *outputCount = GetCount(newHead);
                return IntrusiveSingleList(newHead, newTail, this->deleteOnClear);
            }
            else
            {
                //Return up to maxCount
                size_t count = 1;
                auto resultTail = this->head;
                for (; count < maxCount && Accessor::GetNext(resultTail) != nullptr; count++)
                    resultTail = Accessor::GetNext(resultTail);

                if (outputCount != nullptr)
                    *outputCount = count;

                if (resultTail == nullptr)
                {
                    //Return all
                    auto newHead = this->head;
                    auto newTail = this->tail;
                    this->head = this->tail = nullptr;
                    return IntrusiveSingleList(newHead, newTail, this->deleteOnClear);
                }
                else
                {
                    //Detach the returned list from the current list
                    auto resultHead = this->head;

                    this->head = Accessor::GetNext(resultTail);
                    Accessor::SetNext(resultTail, nullptr);

                    return IntrusiveSingleList(resultHead, resultTail, this->deleteOnClear);
                }
            }
        }

        template <typename StopPredicate>
        IntrusiveSingleList SpliceFrontUntil(size_t maxCount, StopPredicate stopPred, size_t* outputCount = nullptr)
        {
            if (maxCount == 0 || this->head == nullptr)
            {
                //Return none
                if (outputCount != nullptr)
                    *outputCount = 0;
                return IntrusiveSingleList(nullptr, nullptr, this->deleteOnClear);
            }
            else
            {
                //Return up to maxCount
                size_t count = 1;
                auto resultTail = this->head;
                for (; count < maxCount && Accessor::GetNext(resultTail) != nullptr; count++)
                {
                    if (stopPred(*Accessor::GetNext(resultTail)))
                        break;
                    resultTail = Accessor::GetNext(resultTail);
                }

                if (outputCount != nullptr)
                    *outputCount = count;

                if (resultTail == nullptr)
                {
                    //Return all
                    auto newHead = this->head;
                    auto newTail = this->tail;
                    this->head = this->tail = nullptr;
                    return IntrusiveSingleList(newHead, newTail, this->deleteOnClear);
                }
                else
                {
                    //Detach the returned list from the current list
                    auto resultHead = this->head;

                    this->head = Accessor::GetNext(resultTail);
                    Accessor::SetNext(resultTail, nullptr);

                    return IntrusiveSingleList(resultHead, resultTail, this->deleteOnClear);
                }
            }
        }

        static size_t GetCount(T* head)
        {
            size_t count = 0;
            for (auto item = head; item != nullptr; item = Accessor::GetNext(item))
                count++;
            return count;
        }

        T* head;
        T* tail;
        bool deleteOnClear;
    };

    /**
     * Contains an intrusive list and a element count. It is intended to be used as the result of some type of query, where the query itself
     * calculates the element count, saving the caller the trouble of having to count up the elements in the list.
     */
    template <typename ListType>
    class IntrusiveSingleListResult
    {
    public:
        using value_type = typename ListType::value_type;
        using iterator = typename ListType::iterator;
        using const_iterator = typename ListType::iterator;

        IntrusiveSingleListResult() : list(false) { this->count = 0; }
        IntrusiveSingleListResult(value_type head) : list(head, false) { this->count = this->list.size(); }
        IntrusiveSingleListResult(value_type head, size_t count) : list(head, false) { this->count = count; }

        iterator begin() { return this->list.begin(); }
        iterator end() { return this->list.end(); }

        const_iterator begin() const { return this->list.begin(); }
        const_iterator end() const { return this->list.end(); }

        size_t size() const { return this->count; }
        size_t empty() const { return this->count == 0; }

        void push_back(value_type item) { this->count += this->list.push_back(item); }

        void clear() { this->list.clear(); this->count = 0; }

        ListType& GetList() { return this->list; }

    private:
        ListType list;
        size_t count;
    };

} }
