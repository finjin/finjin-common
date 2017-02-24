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
#include "finjin/common/JniUtilities.hpp"

using namespace Finjin::Common;


//Macros-----------------------------------------------------------------------
#define GET_CHARS(env, jniText) env->GetStringUTFChars(jniText, 0);    
#define RELEASE_CHARS(env, jniText, chars) env->ReleaseStringUTFChars(jniText, chars);    
#define NEW_STRING(env, chars) env->NewStringUTF(chars);


//Implementation---------------------------------------------------------------

//JniUtilities
JniUtilities::JniUtilities(JNIEnv* env, jobject thiz)
{
    this->env = env;
    this->thiz = thiz;
}

bool JniUtilities::GetBoolField(bool& value, const char* name, bool defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "Z");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    value = env->GetBooleanField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::GetIntField(int32_t& value, const char* name, int32_t defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "I");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    value = env->GetIntField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::GetIntArrayFieldElement(int32_t& value, const char* name, int index, int32_t defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "[I");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jobjectArray fieldObjArray = (jobjectArray)fieldObj;
    if (fieldObjArray == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    env->GetIntArrayRegion((jintArray)fieldObjArray, index, 1, &value);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    
    return true;
}

bool JniUtilities::GetLongField(int64_t& value, const char* name, int64_t defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "J");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    value = env->GetLongField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::GetLongArrayFieldElement(int64_t& value, const char* name, int index, int64_t defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "[J");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jobjectArray fieldObjArray = (jobjectArray)fieldObj;
    if (fieldObjArray == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    env->GetLongArrayRegion((jlongArray)fieldObjArray, index, 1, &value);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::GetStringField(Utf8String& value, const char* name, const char* defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "Ljava/lang/String;");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, fieldId);    
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jstring fieldString = (jstring)fieldObj;
    if (fieldString == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    const char* fieldValueChars = GET_CHARS(env, fieldString);
    value = fieldValueChars;
    RELEASE_CHARS(env, fieldString, fieldValueChars);

    return true;
}

bool JniUtilities::GetStringField(Path& value, const char* name, const Path::value_type* defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "Ljava/lang/String;");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jstring fieldString = (jstring)fieldObj;
    if (fieldString == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    const char* fieldValueChars = GET_CHARS(env, fieldString);
    value = fieldValueChars;
    RELEASE_CHARS(env, fieldString, fieldValueChars);

    return true;
}

bool JniUtilities::GetStringArrayFieldElement(Utf8String& value, const char* name, int index, const char* defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "[Ljava/lang/String;");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, fieldId);    
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jobjectArray fieldObjArray = (jobjectArray)fieldObj;
    if (fieldObjArray == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    jobject elementObj = env->GetObjectArrayElement(fieldObjArray, index);  
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jstring elementString = (jstring)elementObj;
    if (elementString != 0)
    {        
        JNIAutoDeleteLocalObjectRef autoDeleteLocal_elementObj(env, elementObj);

        const char* elementValueChars = GET_CHARS(env, elementString);
        value = elementValueChars;
        RELEASE_CHARS(env, elementString, elementValueChars);
    }
    
    return true;
}

bool JniUtilities::GetStringArrayFieldElement(Path& value, const char* name, int index, const Path::value_type* defaultValue)
{
    value = defaultValue;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID fieldId = env->GetFieldID(thizClass, name, "[Ljava/lang/String;");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, fieldId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jobjectArray fieldObjArray = (jobjectArray)fieldObj;
    if (fieldObjArray == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    jobject elementObj = env->GetObjectArrayElement(fieldObjArray, index);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jstring elementString = (jstring)elementObj;
    if (elementString != 0)
    {
        JNIAutoDeleteLocalObjectRef autoDeleteLocal_elementObj(env, elementObj);

        const char* elementValueChars = GET_CHARS(env, elementString);
        value = elementValueChars;
        RELEASE_CHARS(env, elementString, elementValueChars);
    }

    return true;
}

bool JniUtilities::GetIntArrayField(int32_t* arr, int& arrayLength, int arrayMaxLength, const char* name)
{
    arrayLength = 0;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID arrayId = env->GetFieldID(thizClass, name, "[I");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, arrayId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jintArray arrayObj = (jintArray)fieldObj;
    if (arrayObj == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    arrayLength = env->GetArrayLength(arrayObj);    
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (arrayLength > arrayMaxLength)
        arrayLength = arrayMaxLength;
    
    env->GetIntArrayRegion(arrayObj, 0, arrayLength, arr);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::GetFloatArrayField(float* arr, int& arrayLength, int arrayMaxLength, const char* name)
{
    arrayLength = 0;

    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jfieldID arrayId = env->GetFieldID(thizClass, name, "[F");
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    jobject fieldObj = env->GetObjectField(thiz, arrayId);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jfloatArray arrayObj = (jfloatArray)fieldObj;
    if (arrayObj == 0)
        return false;

    JNIAutoDeleteLocalObjectRef autoDeleteLocal_fieldObj(env, fieldObj);

    arrayLength = env->GetArrayLength(arrayObj);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (arrayLength > arrayMaxLength)
        arrayLength = arrayMaxLength;
    
    env->GetFloatArrayRegion(arrayObj, 0, arrayLength, arr);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod(const char* name)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "()V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    env->CallVoidMethod(thiz, methodID);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_String(const char* name, const char* v0)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(Ljava/lang/String;)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v0 = NEW_STRING(env, v0 ? v0 : "");
    env->CallVoidMethod(thiz, methodID, jni_v0);
    env->DeleteLocalRef(jni_v0);    
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_String_String(const char* name, const char* v0, const char* v1)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(Ljava/lang/String;Ljava/lang/String;)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v0 = NEW_STRING(env, v0 ? v0 : "");
    jstring jni_v1 = NEW_STRING(env, v1 ? v1 : "");
    env->CallVoidMethod(thiz, methodID, jni_v0, jni_v1);
    env->DeleteLocalRef(jni_v0);
    env->DeleteLocalRef(jni_v1);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_String_String_String_String(const char* name, const char* v0, const char* v1, const char* v2, const char* v3)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v0 = NEW_STRING(env, v0 ? v0 : "");
    jstring jni_v1 = NEW_STRING(env, v1 ? v1 : "");
    jstring jni_v2 = NEW_STRING(env, v2 ? v2 : "");
    jstring jni_v3 = NEW_STRING(env, v3 ? v3 : "");
    env->CallVoidMethod(thiz, methodID, jni_v0, jni_v1, jni_v2, jni_v3);
    env->DeleteLocalRef(jni_v0);
    env->DeleteLocalRef(jni_v1);
    env->DeleteLocalRef(jni_v2);
    env->DeleteLocalRef(jni_v3);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_Int_String_String_String_String(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v1 = NEW_STRING(env, v1 ? v1 : "");
    jstring jni_v2 = NEW_STRING(env, v2 ? v2 : "");
    jstring jni_v3 = NEW_STRING(env, v3 ? v3 : "");
    jstring jni_v4 = NEW_STRING(env, v4 ? v4 : "");
    env->CallVoidMethod(thiz, methodID, v0, jni_v1, jni_v2, jni_v3, jni_v4);
    env->DeleteLocalRef(jni_v1);
    env->DeleteLocalRef(jni_v2);
    env->DeleteLocalRef(jni_v3);
    env->DeleteLocalRef(jni_v4);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_Int_String_String_String_String_Int_Int_Float_Float(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4, int v5, int v6, float v7, float v8)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIFF)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v1 = NEW_STRING(env, v1 ? v1 : "");
    jstring jni_v2 = NEW_STRING(env, v2 ? v2 : "");
    jstring jni_v3 = NEW_STRING(env, v3 ? v3 : "");
    jstring jni_v4 = NEW_STRING(env, v4 ? v4 : "");
    env->CallVoidMethod(thiz, methodID, v0, jni_v1, jni_v2, jni_v3, jni_v4, v5, v6, v7, v8);
    env->DeleteLocalRef(jni_v1);
    env->DeleteLocalRef(jni_v2);
    env->DeleteLocalRef(jni_v3);
    env->DeleteLocalRef(jni_v4);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_Int_String_String_String_String_Int_Int_Float_Float_Int(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4, int v5, int v6, float v7, float v8, int v9)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIFFI)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v1 = NEW_STRING(env, v1 ? v1 : "");
    jstring jni_v2 = NEW_STRING(env, v2 ? v2 : "");
    jstring jni_v3 = NEW_STRING(env, v3 ? v3 : "");
    jstring jni_v4 = NEW_STRING(env, v4 ? v4 : "");
    env->CallVoidMethod(thiz, methodID, v0, jni_v1, jni_v2, jni_v3, jni_v4, v5, v6, v7, v8, v9);
    env->DeleteLocalRef(jni_v1);
    env->DeleteLocalRef(jni_v2);
    env->DeleteLocalRef(jni_v3);
    env->DeleteLocalRef(jni_v4);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_Int_String_String_String_String_Int_int_Int_Float_Float(const char* name, int v0, const char* v1, const char* v2, const char* v3, const char* v4, int v5, int v6, int v7, float v8, float v9)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIFF)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jstring jni_v1 = NEW_STRING(env, v1 ? v1 : "");
    jstring jni_v2 = NEW_STRING(env, v2 ? v2 : "");
    jstring jni_v3 = NEW_STRING(env, v3 ? v3 : "");
    jstring jni_v4 = NEW_STRING(env, v4 ? v4 : "");
    env->CallVoidMethod(thiz, methodID, v0, jni_v1, jni_v2, jni_v3, jni_v4, v5, v6, v7, v8, v9);
    env->DeleteLocalRef(jni_v1);
    env->DeleteLocalRef(jni_v2);
    env->DeleteLocalRef(jni_v3);
    env->DeleteLocalRef(jni_v4);
    JNI_EXCEPTION_RETURN_VALUE(env, false);

    return true;
}

bool JniUtilities::CallVoidMethod_Int(const char* name, int v0)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(I)V");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    env->CallVoidMethod(thiz, methodID, v0);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
}

bool JniUtilities::CallBoolMethod(bool& result, const char* name)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "()Z");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0) 
        return false;

    jboolean jniResult = env->CallBooleanMethod(thiz, methodID);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    
    result = jniResult == JNI_TRUE;
    
    return true;
}

bool JniUtilities::CallStringMethod_Int(Utf8String& result, const char* name, int v0)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetMethodID(thizClass, name, "(I)Ljava/lang/String;");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0)
        return false;

    jobject jniResult = env->CallObjectMethod(thiz, methodID, v0);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jstring resultString = (jstring)jniResult;

    if (resultString == 0)
        return false;

    const char* returnChars = GET_CHARS(env, resultString);
    result = returnChars;
    RELEASE_CHARS(env, resultString, returnChars);

    return true;
}

bool JniUtilities::CallClassStringMethod_Int(Utf8String& result, const char* name, int v0)
{
    jclass thizClass = env->GetObjectClass(thiz);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    JNIAutoDeleteLocalClassRef autoDeleteLocal_thizClass(env, thizClass);

    jmethodID methodID = env->GetStaticMethodID(thizClass, name, "(I)Ljava/lang/String;");
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    if (methodID == 0)
        return false;

    jobject jniResult = env->CallStaticObjectMethod(thizClass, methodID, v0);
    JNI_EXCEPTION_RETURN_VALUE(env, false);
    jstring resultString = (jstring)jniResult;

    if (resultString == 0)
        return false;

    const char* returnChars = GET_CHARS(env, resultString);
    result = returnChars;
    RELEASE_CHARS(env, resultString, returnChars);

    return true;
}

//JniAutoReleaseStringChars
JniAutoReleaseStringChars::JniAutoReleaseStringChars(JNIEnv* e, jstring& t) : env(e), jniText(t) 
{
    textChars = GET_CHARS(env, jniText);
}

JniAutoReleaseStringChars::~JniAutoReleaseStringChars() 
{
    RELEASE_CHARS(env, jniText, textChars);
}
