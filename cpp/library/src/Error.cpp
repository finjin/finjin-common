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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/ThisThread.hpp"

using namespace Finjin::Common;


//Local values------------------------------------------------------------------
static const Utf8String DISABLED_ERROR_MESSAGE("<error messages disabled>");


//Local functions---------------------------------------------------------------
static Utf8String FormatCallStackRecordMetadataParts(bool isUnexpected, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    Utf8String result;

    if (isUnexpected)
        result += "Unexpected call at: ";
    result += functionName;
    result += " (";
    result += fileName;
    result += ", ";
    result += Convert::ToString(line);
    result += ")";

    return result;
}

static Utf8String FormatCallStackRecordMetadata(const Error::CallStackRecord& record)
{
    return FormatCallStackRecordMetadataParts(record.isUnexpected, record.fileName, record.functionName, record.line);
}

static void OutputCallStackRecord(std::ostream& out, const ErrorOutputFormat& format, const Error::CallStackRecord& record)
{
    if (record.isUnexpected)
        out << "Unexpected call at: ";

    const char* fileName = format.GetFileName(record.fileName);
    out << record.functionName << " (" << fileName << ", " << record.line << ")";

    if (!record.isUnexpected)
    {
        out << ": " << (record.HasErrorMessage() ? record.GetErrorMessage().c_str() : "<no error message at this level>");
    }

    out << std::endl;
}


//Implementation----------------------------------------------------------------

//ErrorOutputFormat
ErrorOutputFormat::ErrorOutputFormat()
{
    this->order = Order::DEEPEST_FIRST;
    this->until = ShowUntil::CURRENT_STACK_ITEM;
    this->fileName = FileName::BASE_FILE_NAME;
}

ErrorOutputFormat::ErrorOutputFormat(Order order, ShowUntil until, FileName fileName)
{
    this->order = order;
    this->until = until;
    this->fileName = fileName;
}

const char* ErrorOutputFormat::GetFileName(const char* fileName) const
{
    if (this->fileName == FileName::BASE_FILE_NAME)
    {
        size_t length = strlen(fileName);
        for (size_t i = length - 1; i != (size_t)-1; i--)
        {
            if ((fileName[i] == '/' || fileName[i] == '\\') && i + 1 < length)
            {
                fileName = &fileName[i + 1];
                break;
            }
        }
    }

    return fileName;
}

//Error::CallStackRecord
Error::CallStackRecord::CallStackRecord()
{
    this->isUnexpected = false;

    this->fileName = nullptr;
    this->functionName = nullptr;
    this->line = -1;

#if FINJIN_ERROR_NO_MESSAGES
    this->hasErrorMessage = false;
#endif
    this->errorCode = 0;
}

Error::CallStackRecord::CallStackRecord(bool isUnexpected, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    this->isUnexpected = isUnexpected;

    this->fileName = fileName;
    this->functionName = functionName;
    this->line = line;

#if FINJIN_ERROR_NO_MESSAGES
    this->hasErrorMessage = false;
#endif
    this->errorCode = 0;
}

bool Error::CallStackRecord::HasErrorMessage() const
{
#if FINJIN_ERROR_NO_MESSAGES
    return this->hasErrorMessage;
#else
    return !this->errorMessage.empty();
#endif
}

const Utf8String& Error::CallStackRecord::GetErrorMessage() const
{
#if FINJIN_ERROR_NO_MESSAGES
    return this->hasErrorMessage ? DISABLED_ERROR_MESSAGE : Utf8String::Empty();
#else
    return this->errorMessage;
#endif
}

//Error
Error::Error()
{
    this->ownerThreadID = ThisThread::GetID();

    this->depth = -1;

    this->hasError = false;
}

Error::~Error()
{
}

void Error::EnterMethod(FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    if (IsCallerOwnerThread())
    {
        CallStackRecord record(this->hasError, FINJIN_CALLER_PARAMETERS);
        if (this->callStack.push_back(record).HasErrorOrValue(false))
        {
            assert(0 && "Error call stack is too deep. Try increasing the size of MAX_ERROR_CALL_STACK_DEPTH.");
        }
        this->depth++;
    }
}

void Error::LeaveMethod()
{
    if (IsCallerOwnerThread())
    {
        if (!this->hasError)
            this->callStack.pop_back();

        this->depth--;

        assert(this->depth >= -1);
    }
}

#if FINJIN_ERROR_NO_MESSAGES
void Error::SetError(bool hasErrorMessage, int code, FINJIN_CALLER_PARAMETERS_DECLARATION)
#else
void Error::SetError(const Utf8String& message, int code, FINJIN_CALLER_PARAMETERS_DECLARATION)
#endif
{
    //Note: The error parameters such as line number, file name, are not used here since
    //it's assumed the values captured during EnterMethod() were good enough.

    if (IsCallerOwnerThread() && this->depth >= 0)
    {
        this->hasError = true;

    #if FINJIN_ERROR_NO_MESSAGES
        this->callStack[this->depth].hasErrorMessage = hasErrorMessage;
    #else
        this->callStack[this->depth].errorMessage = message;
    #endif
        this->callStack[this->depth].errorCode = code;
    }
}

Error::operator bool() const
{
    return this->hasError;
}

int Error::GetLastErrorCode() const
{
    if (IsCallerOwnerThread() && this->hasError)
        return this->callStack[this->depth + 1].errorCode;
    else
        return 0;
}

const Utf8String& Error::GetLastErrorMessage() const
{
    if (IsCallerOwnerThread() && this->hasError)
        return this->callStack[this->depth + 1].GetErrorMessage();
    else
        return Utf8String::Empty();
}

const Utf8String& Error::GetLastNonEmptyErrorMessage() const
{
    if (IsCallerOwnerThread() && this->hasError)
    {
        for (int i = this->depth; i < (int)this->callStack.size(); i++)
        {
            if (this->callStack[i].HasErrorMessage())
                return this->callStack[i].GetErrorMessage();
        }
    }

    return Utf8String::Empty();
}

Utf8String Error::JoinErrorMessages(const Utf8String& joinText) const
{
    Utf8String result;
    JoinErrorMessages(result, joinText);
    return result;
}

ValueOrError<void> Error::JoinErrorMessages(Utf8String& result, const Utf8String& joinText) const
{
    result.clear();

    if (IsCallerOwnerThread() && this->hasError)
    {
        for (int i = this->depth; i < (int)this->callStack.size(); i++)
        {
            if (this->callStack[i].isUnexpected || this->callStack[i].HasErrorMessage())
            {
                if (!result.empty())
                {
                    if (result.append(joinText).HasError())
                        return ValueOrError<void>::CreateError();
                }

                if (this->callStack[i].isUnexpected)
                {
                    if (result.append(FormatCallStackRecordMetadata(this->callStack[i])).HasError())
                        return ValueOrError<void>::CreateError();
                }
                else
                {
                    if (result.append(this->callStack[i].GetErrorMessage()).HasError())
                        return ValueOrError<void>::CreateError();
                }
            }
        }
    }

    return ValueOrError<void>();
}

void Error::Output(std::ostream& out, const ErrorOutputFormat& format) const
{
    if (IsCallerOwnerThread() && this->hasError)
    {
        int current = (format.until == ErrorOutputFormat::ShowUntil::COMPLETE_STACK) ? 0 : this->depth;

        int first, last, increment;
        if (format.order == ErrorOutputFormat::Order::DEEPEST_FIRST)
        {
            first = (int)this->callStack.size() - 1;
            last = current - 1;
            increment = -1;
        }
        else
        {
            first = current;
            last = (int)this->callStack.size();
            increment = 1;
        }

        for (int i = first; i != last; i += increment)
            OutputCallStackRecord(out, format, this->callStack[i]);
    }
}

Utf8String Error::ToString() const
{
    return ToStringWithPrefix(Utf8String::Empty());
}

ValueOrError<void> Error::ToString(Utf8String& result) const
{
    return ToStringWithPrefix(result, Utf8String::Empty());
}

Utf8String Error::ToStringWithPrefix(const Utf8String& messagePrefix) const
{
    Utf8String result;
    ToStringWithPrefix(result, messagePrefix);
    return result;
}

ValueOrError<void> Error::ToStringWithPrefix(Utf8String& result, const Utf8String& messagePrefix) const
{
    result = messagePrefix;
    if (!result.empty())
    {
        if (result.append("\n\n").HasError())
            return ValueOrError<void>::CreateError();
    }
    if (result.append(JoinErrorMessages(FINJIN_ERROR_NEWLINE_AND_INDENT)).HasError())
        return ValueOrError<void>::CreateError();

    return ValueOrError<void>();
}

bool Error::IsCallerOwnerThread() const
{
    return this->ownerThreadID == ThisThread::GetID();
}

//ErrorMethodEntry---------------------
ErrorMethodEntry::ErrorMethodEntry(Error& error, FINJIN_CALLER_PARAMETERS_DECLARATION)
{
    this->error = &error;
    this->error->EnterMethod(FINJIN_CALLER_PARAMETERS);
}

ErrorMethodEntry::~ErrorMethodEntry()
{
    this->error->LeaveMethod();
}

//ErrorState----------------------------
ErrorState::ErrorState()
{
    Reset();
}

ErrorState::ErrorState(const ErrorState& other)
{
    this->hasError = other.hasError;

    this->fileName = other.fileName;
    this->functionName = other.functionName;
    this->line = other.line;

#if FINJIN_ERROR_NO_MESSAGES
    this->hasError = other.hasError;
#else
    this->errorMessage = other.errorMessage;
#endif
    this->errorCode = other.errorCode;
}

ErrorState& ErrorState::operator = (const ErrorState& other)
{
    if (!this->hasError && other.hasError)
    {
        this->hasError = true;

        this->fileName = other.fileName;
        this->functionName = other.functionName;
        this->line = other.line;

    #if FINJIN_ERROR_NO_MESSAGES
        this->hasError = other.hasError;
    #else
        this->errorMessage = other.errorMessage;
    #endif
        this->errorCode = other.errorCode;
    }

    return *this;
}

#if FINJIN_ERROR_NO_MESSAGES
void ErrorState::SetError(bool hasErrorMessage, int code, FINJIN_CALLER_PARAMETERS_DECLARATION)
#else
void ErrorState::SetError(const Utf8String& message, int code, FINJIN_CALLER_PARAMETERS_DECLARATION)
#endif
{
    if (!this->hasError)
    {
        this->hasError = true;

        this->fileName = fileName;
        this->functionName = functionName;
        this->line = line;

    #if FINJIN_ERROR_NO_MESSAGES
        this->hasErrorMessage = hasErrorMessage;
    #else
        this->errorMessage = message;
    #endif
        this->errorCode = code;
    }
}

void ErrorState::Reset()
{
    this->hasError = false;

    this->fileName = nullptr;
    this->functionName = nullptr;
    this->line = -1;

#if FINJIN_ERROR_NO_MESSAGES
    this->hasErrorMessage = false;
#else
    this->errorMessage.clear();
#endif
    this->errorCode = 0;
}

ErrorState::operator bool() const
{
    return this->hasError;
}

bool ErrorState::HasErrorMessage() const
{
#if FINJIN_ERROR_NO_MESSAGES
    return this->hasErrorMessage;
#else
    return !this->errorMessage.empty();
#endif
}

const Utf8String& ErrorState::GetErrorMessage() const
{
#if FINJIN_ERROR_NO_MESSAGES
    return this->hasErrorMessage ? DISABLED_ERROR_MESSAGE : Utf8String::Empty();
#else
    return this->errorMessage;
#endif
}

int ErrorState::GetLastErrorCode() const
{
    return this->errorCode;
}

const Utf8String& ErrorState::GetLastErrorMessage() const
{
    return GetErrorMessage();
}

Utf8String ErrorState::ToString() const
{
    Utf8String result;
    ToString(result);
    return result;
}

ValueOrError<void> ErrorState::ToString(Utf8String& result) const
{
    return result.append(FormatCallStackRecordMetadataParts(false, this->fileName, this->functionName, this->line));
}
