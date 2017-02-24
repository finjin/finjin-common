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
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/ThreadID.hpp"


//Macros------------------------------------------------------------------------
#ifndef FINJIN_ERROR_NO_MESSAGES
    //By default, use error messages (0 = use messages, 1 = no messages)
    #define FINJIN_ERROR_NO_MESSAGES 0
#endif

#if !FINJIN_ERROR_NO_MESSAGES
    #include "finjin/common/Utf8StringFormatter.hpp"
#endif

//Used in cases where an error occurred but an error message is not being set,
//most likely because it was known to be set adequately in the method that generated the error.
#if FINJIN_ERROR_NO_MESSAGES
    #define FINJIN_SET_ERROR_NO_MESSAGE(error) error.SetError(false, -1, FINJIN_CALLER_ARGUMENTS)
#else
    #define FINJIN_SET_ERROR_NO_MESSAGE(error) error.SetError(Finjin::Common::Utf8String::Empty(), -1, FINJIN_CALLER_ARGUMENTS)
#endif

//Sets an error message into the Error object
#if FINJIN_ERROR_NO_MESSAGES
    #define FINJIN_SET_ERROR(error, message) error.SetError(true, -1, FINJIN_CALLER_ARGUMENTS)
    #define FINJIN_FORMAT_ERROR_MESSAGE(...) ""
#else
    #define FINJIN_SET_ERROR(error, message) error.SetError(message, -1, FINJIN_CALLER_ARGUMENTS)
    #define FINJIN_FORMAT_ERROR_MESSAGE(...) Finjin::Common::Utf8StringFormatter::Format(__VA_ARGS__)
#endif

//Should be placed at the top of every method that takes a non-const Error object
#define FINJIN_ERROR_METHOD_START(error)  Finjin::Common::ErrorMethodEntry _errorMethodEntry(error, FINJIN_CALLER_ARGUMENTS)

//Declares an Error object
#define FINJIN_DECLARE_ERROR(name) Finjin::Common::Error name; FINJIN_ERROR_METHOD_START(name);

//A popular string to pass to Error::JoinErrorMessages()
#define FINJIN_ERROR_NEWLINE_AND_INDENT "\n<- " 


//Classes-----------------------------------------------------------------------
namespace Finjin { namespace Common {

    struct FINJIN_COMMON_LIBRARY_API ErrorOutputFormat
    {
        /** The order in which errors should be output. */
        enum class Order
        {            
            /**
             * Deepest item in the call stack first. This is the operation that caused the error in the first place.
             * This is the default.
             */
            DEEPEST_FIRST,

            /**
             * Top level, starting from the point where a call was made that eventually caused an error somewhere down the call stack.
             */
            SHALLOWEST_FIRST 
        };

        /** Limits the number of errors. */
        enum class ShowUntil
        {
            /** 
             * Shows all errors up to and including the current call stack item. 
             * This is the default. 
             */
            CURRENT_STACK_ITEM,

            /** Shows the complete call stack. */
            COMPLETE_STACK
        };

        /** The format of the file name. */
        enum class FileName
        {
            /** 
             * The full file name with its path. 
             * Depending on how the compiler handles the __FILE__ macro, this may just be the base file name.
             */
            FULL_FILE_NAME,

            /** 
             * The base file name. 
             * This is the default.
             */
            BASE_FILE_NAME
        };

        /** Default constructor. */
        ErrorOutputFormat();

        /** Full constructor. */
        ErrorOutputFormat(Order order, ShowUntil until, FileName fileName);
        
        /** 
         * Extracts the file name according to the internally set 'file name' display setting.
         * @param fileName [in] The file name to potentially parse.
         * @return The file name according to the internally set 'file name' display setting.
         */
        const char* GetFileName(const char* fileName) const;

        Order order;
        ShowUntil until;
        FileName fileName;
    };
    
    /**
     * Error class.
     * This class captures the hierarchical nature of exceptions with the explicitness
     * of error codes.
     * 
     * Rules for creating a new Error object:
     * 1)Declare it as FINJIN_DECLARE_ERROR(error) or FINJIN_DECLARE_ERROR(name_of_my_error)     
     * 
     * Rules for writing a method that takes a MODIFIABLE Error object:
     * 1)The Error instance must be a non-const reference (Error&)
     * 2)The Error instance must be the last parameter in the parameter list. 
     *     For example: void DoSomething(int a, int b, Error& error)
     *   An exception to this rule occurs when the the parameter list has a variable number of trailing parameters.
     *     For example: void DoSomething(int a, int b, Error& error, ...)
     * 3)The Error instance must be named 'error'.
     * 4)The first line in the method must be: FINJIN_ERROR_METHOD_START(error);
     * 
     * Rules for calling a method that takes a modifiable Error object:
     * 1)After calling the method, the error object MUST be checked before proceeding.
     *   For example:
     *   void MyMethod(Error& error)
     *   {
     *       FINJIN_ERROR_METHOD_START(error);
     * 
     *       DoSomething(1, 2, error);
     *       if (error)
     *       {
     *          //Assume the user will read your error message, so try to use complete sentences, 
     *          //with a period at the end.
     *          FINJIN_SET_ERROR(error, "Failed to compute result."); 
     *          return;
     *       }
     *   }
     * 2)It is recommended that you use FINJIN_SET_ERROR() to add detail to a received error. 
     * If you don't have any detail to add then use FINJIN_SET_ERROR_NO_MESSAGE().
     * 3)Be sure to use smart pointers and other types that automatically destroy themselves
     * so that you don't create memory leaks when exiting a method early.
     * 4)It is critical that if FINJIN_SET_ERROR() or FINJIN_SET_ERROR_NO_MESSAGE() has been used, that it NOT be used again within the same scope.
     * Doing so will overwrite error information.
     */
    class FINJIN_COMMON_LIBRARY_API Error
    {
    public:
        Error(const Error& other) = delete;
        Error& operator = (const Error& other) = delete;

        Error(Error&& Error) = delete;
        Error& operator = (Error&& other) = delete;

    public:
        Error();
        ~Error();

        /** Should be called at the start of a method (when it has been entered). */
        void EnterMethod(FINJIN_CALLER_PARAMETERS_DECLARATION);

        /** Should be called at the ending of a method (when it is about to be exited). */
        void LeaveMethod();

        /**
         * Sets error information. 
         * @param message [in] Error message.
         * @param code [in] Error code.
         */
    #if FINJIN_ERROR_NO_MESSAGES
        void SetError(bool hasErrorMessage, int code, FINJIN_CALLER_PARAMETERS_DECLARATION);
    #else    
        void SetError(const Utf8String& message, int code, FINJIN_CALLER_PARAMETERS_DECLARATION);
    #endif

        /** Boolean operator, used to test whether the error object has an error. */
        operator bool () const;

        //The "GetLast" methods all work from the previous call stack instance, not the current one
        int GetLastErrorCode() const;
        const Utf8String& GetLastErrorMessage() const;
        const Utf8String& GetLastNonEmptyErrorMessage() const;
        
        /** 
         * Joins all non-empty error messages, including from the current call stack instance.
         * @param joinText [in] The text used to join the error messages.
         * @return The non-empty error message, joined as a single string.
         */
        Utf8String JoinErrorMessages(const Utf8String& joinText) const;
        ValueOrError<void> JoinErrorMessages(Utf8String& result, const Utf8String& joinText) const;

        /**
         * Outputs the errors.
         * @param out [out] Output stream.
         * @param format [in] Error output format.
         */
        void Output(std::ostream& out, const ErrorOutputFormat& format) const;

        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

        Utf8String ToStringWithPrefix(const Utf8String& messagePrefix) const;
        ValueOrError<void> ToStringWithPrefix(Utf8String& result, const Utf8String& messagePrefix) const;

        struct CallStackRecord
        {
            CallStackRecord();
            CallStackRecord(bool isUnexpected, FINJIN_CALLER_PARAMETERS_DECLARATION);

            bool HasErrorMessage() const;
            const Utf8String& GetErrorMessage() const;
            
            bool isUnexpected;

            const char* fileName; //Just hold onto the pointer
            const char* functionName; //Just hold onto the pointer
            int line;

        #if FINJIN_ERROR_NO_MESSAGES
            bool hasErrorMessage;
        #else
            Utf8String errorMessage;
        #endif
            int errorCode;
        };

    private:
        bool IsCallerOwnerThread() const;

    private:
        ThreadID ownerThreadID;

        StaticVector<CallStackRecord, CommonConstants::MAX_ERROR_CALL_STACK_DEPTH> callStack;
        int depth;

        bool hasError;
    };
    
    /** Automatically handles the entry and exit of a method. Should be used via the FINJIN_ERROR_METHOD_START macro. */
    class FINJIN_COMMON_LIBRARY_API ErrorMethodEntry
    {
    public:
        ErrorMethodEntry(Error& error, FINJIN_CALLER_PARAMETERS_DECLARATION);
        ~ErrorMethodEntry();

    private:
        Error* error;
    };

    /**
     * A simple alternative to Error. Can only be used to capture one error.
     * Use FINJIN_SET_ERROR() macro to set the error.
     */
    class FINJIN_COMMON_LIBRARY_API ErrorState
    {
    public:
        ErrorState();
        ErrorState(const ErrorState& other);
        ErrorState& operator = (const ErrorState& other);

    #if FINJIN_ERROR_NO_MESSAGES
        void SetError(bool hasErrorMessage, int code, FINJIN_CALLER_PARAMETERS_DECLARATION);
    #else
        void SetError(const Utf8String& message, int code, FINJIN_CALLER_PARAMETERS_DECLARATION);
    #endif

        void Reset();

        operator bool() const;

        bool HasErrorMessage() const;
        const Utf8String& GetErrorMessage() const;

        int GetLastErrorCode() const;
        const Utf8String& GetLastErrorMessage() const;

        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

    private:
        bool hasError;

        const char* fileName; //Just hold onto the pointer
        const char* functionName; //Just hold onto the pointer
        int line;

    #if FINJIN_ERROR_NO_MESSAGES
        bool hasErrorMessage;
    #else
        Utf8String errorMessage;
    #endif
        int errorCode;
    };

} }
