#include "defines.h"

static char * jstring_to_gbk_char(JNIEnv * env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = (*env)->FindClass(env, "java/lang/String");
    jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jstring gbk_encode = (*env)->NewStringUTF(env, "GBK");
    jbyteArray barr = (jbyteArray) (*env)->CallObjectMethod(env, jstr, mid, gbk_encode);
    jsize alen = (*env)->GetArrayLength(env, barr);
    jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
    return rtn;
}
/*
 * Class:     com_fant_util_CompanyExtracter
 * Method:    search
 * Signature: (Ljava/lang/String;Z)Ljava/util/List;
 */
JNIEXPORT jobject JNICALL Java_com_fant_util_CompanyExtracter_search
(JNIEnv *env, jclass cls, jstring jstr, jboolean greedy) {
    char * str = jstring_to_gbk_char(env, jstr);
    match_result * result = cut(str, greedy), *p;
    free(str);
    jclass list_cls = (*env)->FindClass(env, "Ljava/util/ArrayList;");
    jmethodID list_costruct = (*env)->GetMethodID(env, list_cls, "<init>","()V");
    jobject list_obj = (*env)->NewObject(env, list_cls, list_costruct);
    jmethodID list_add  = (*env)->GetMethodID(env, list_cls,"add","(Ljava/lang/Object;)Z");
    jclass string_cls = (*env)->FindClass(env, "java/lang/String");
    jmethodID string_costruct = (*env)->GetMethodID(env, string_cls, "<init>","([BLjava/lang/String;)V");
    jbyteArray data;
    int str_len;
    jstring gbk_encode = (*env)->NewStringUTF(env, "GBK");
    while(result != NULL) {
        p = result->next;
        str_len = strlen(result->match_str);
        data = (*env)->NewByteArray(env, str_len);
        (*env)->SetByteArrayRegion(env, data, 0, str_len, (jbyte*)result->match_str);
        (*env)->CallBooleanMethod(env, list_obj, list_add, (*env)->NewObject(env, string_cls, string_costruct, data, gbk_encode));
        free(result->match_str);
        free(result);
        result = p;
    }
    return list_obj;
}

/*
 * Class:     com_fant_util_CompanyExtracter
 * Method:    init
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_fant_util_CompanyExtracter_init
(JNIEnv * env, jclass cls, jstring jstr, jint cache_lavel) {
    char * str = jstring_to_gbk_char(env, jstr);
    init(str, cache_lavel);
    free(str);
}

/*
 * Class:     com_fant_util_CompanyExtracter
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_fant_util_CompanyExtracter_destroy
(JNIEnv * env, jclass cls) {
    destroy();
}


JNIEXPORT jobject JNICALL Java_com_jzdata_util_CompanyExtracter_search
(JNIEnv *env, jclass cls, jstring jstr, jboolean greedy) {
    return Java_com_fant_util_CompanyExtracter_search(env, cls, jstr, greedy);
}

JNIEXPORT void JNICALL Java_com_jzdata_util_CompanyExtracter_init
(JNIEnv * env, jclass cls, jstring jstr, jint cache_lavel) {
    Java_com_fant_util_CompanyExtracter_init(env, cls, jstr, cache_lavel);
}

JNIEXPORT void JNICALL Java_com_jzdata_util_CompanyExtracter_destroy
(JNIEnv * env, jclass cls) {
    Java_com_fant_util_CompanyExtracter_destroy(env, cls);
}
