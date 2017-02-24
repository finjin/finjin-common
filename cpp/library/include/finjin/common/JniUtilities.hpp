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
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include <jni.h>


//Macros-----------------------------------------------------------------------
#define JNI_EXCEPTION_RETURN_VALUE(env, value) \
    if (env->ExceptionOccurred()) { \
        env->ExceptionDescribe(); \
        env->ExceptionClear(); \
        return value; \
    }

#define JNI_EXCEPTION_RETURN(env) \
    if (env->ExceptionOccurred()) { \
        env->ExceptionDescribe(); \
        env->ExceptionClear(); \
        return; \
    }


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API JniUtilities
    {
    public:
        JniUtilities(JNIEnv* env, jobject thiz);
                
        bool GetBoolField(bool& value, const char* name, bool defaultValue = false);
        bool GetIntField(int32_t& value, const char* name, int32_t defaultValue = 0);
        bool GetIntArrayFieldElement(int32_t& value, const char* name, int index, int32_t defaultValue = 0);
        bool GetLongField(int64_t& value, const char* name, int64_t defaultValue = 0);
        bool GetLongArrayFieldElement(int64_t& value, const char* name, int index, int64_t defaultValue = 0);
        bool GetStringField(Utf8String& value, const char* name, const char* defaultValue = "");
        bool GetStringField(Path& value, const char* name, const Path::value_type* defaultValue = "");
        bool GetStringArrayFieldElement(Utf8String& value, const char* name, int index, const char* defaultValue = "");
        bool GetStringArrayFieldElement(Path& value, const char* name, int index, const Path::value_type* defaultValue = "");
        bool GetIntArrayField(int32_t* arr, int& arrayLength, int arrayMaxLength, const char* name);
        bool GetFloatArrayField(float* arr, int& arrayLength, int arrayMaxLength, const char* name);

        template <size_t maxSize>
        bool GetIntArrayField(StaticVector<int, maxSize>& arr, const char* name)
        {
            int len;
            arr.resize(maxSize);
            auto result = GetIntArrayField(arr.data(), len, static_cast<int>(arr.max_size()), name);
            arr.resize(static_cast<size_t>(result ? len : 0));
            return result;
        }
        template <size_t maxSize>
        bool GetFloatArrayField(StaticVector<float, maxSize>& arr, const char* name)
        {
            int len;
            arr.resize(maxSize);
            auto result = GetFloatArrayField(arr.data(), len, static_cast<int>(arr.max_size()), name);
            arr.resize(static_cast<size_t>(result ? len : 0));
            return result;
        }
                
        bool CallVoidMethod(const char* name);
        bool CallVoidMethod_String(const char* name, const char* v0);
        bool CallVoidMethod_String_String(const char* name, const char* v0, const char* v1);
        bool CallVoidMethod_String_String_String_String(const char* name, const char* v0, const char* v1, const char* v2, const char* v3);
        bool CallVoidMethod_Int_String_String_String_String(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4);
        bool CallVoidMethod_Int_String_String_String_String_Int_Int_Float_Float(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4, int v5, int v6, float v7, float v8);
        bool CallVoidMethod_Int_String_String_String_String_Int_Int_Float_Float_Int(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4, int v5, int v6, float v7, float v8, int v9);
        bool CallVoidMethod_Int_String_String_String_String_Int_int_Int_Float_Float(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4, int v5, int v6, int v7, float v8, float v9);
        bool CallVoidMethod_Int(const char* name, int v0);

        bool CallBoolMethod(bool& result, const char* name);
        bool CallStringMethod_Int(Utf8String& result, const char* name, int v0);
        bool CallClassStringMethod_Int(Utf8String& result, const char* name, int v0);

    protected:
        JNIEnv* env;
        jobject thiz;
    };

    /**
     * Instances of this class delete the specified JNI resource when the instance goes out of scope.
     * This is helpful for preventing resource leaks in the event that JNI exceptions occur and a function needs to exit early.
     */
    template <typename T> 
    class JniAutoDeleteLocalRef
    {
    public:
        JniAutoDeleteLocalRef(JNIEnv* e, T& o) : env(e), obj(o) {}
        ~JniAutoDeleteLocalRef() {env->DeleteLocalRef(obj);}

    private:
        JNIEnv* env;
        T& obj;
    };

    using JNIAutoDeleteLocalObjectRef = JniAutoDeleteLocalRef<jobject>;
    using JNIAutoDeleteLocalClassRef = JniAutoDeleteLocalRef<jclass>;

    class JniAutoReleaseStringChars
    {
    public:
        JniAutoReleaseStringChars(JNIEnv* e, jstring& t);
        ~JniAutoReleaseStringChars();

        const char* GetChars() const {return textChars;}

    private:
        JNIEnv* env;
        jstring& jniText;
        const char* textChars;
    };

} }
