/*
 * Copyright (C) 2014 gameld
 * Write: huangtao117@gmail.com
 * 注意:
 * JNI规定每个JNIEnv对于线程都是本地的
 * 一个线程使用另一个线程的JNIEnv将引起BUG和崩溃
 * 对于c++创建的线程如果需要调用JNI则必须创建JNIEnv
 * 对于CallStaticObjectMethod这类调用来说，异常检测是必须的。
 */
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <android/log.h>
#include "com_gameld_core_MyJni.h"
#include "ld_client.h"

const int CORE_PARAM_LOG = 1;

/*
 * c++ thread cann't FindClass
 * because c++ thread != jvm thread
 * so must cached jobject
 */
static JavaVM* g_jvm = NULL;
static JNIEnv* g_envJava = NULL;
static JNIEnv* g_envRead = NULL;
static JNIEnv* g_envWrite = NULL;
static jclass g_myjni = NULL;
static jmethodID g_connect = NULL;
static jmethodID g_disconnect = NULL;
static jmethodID g_receive = NULL;

void process_xy(char* data, int len)
{
    char str[256];
    short xy_id;
    xy_gameld::EncryptVer ev;
    xy_gameld::RespKey key;

    if (data == NULL || len <= 0)
        return;

#ifdef TAO_DEBUG
    __android_log_print(ANDROID_LOG_INFO, "ldcore", "%s:%d",
        "process_xy start", len);
#endif

    gameld::bistream bis(data, len);
    bis >> xy_id;
    switch (xy_id) {
        case xy_gameld::CMDT_ENCRYPTVER:
            bis >> ev;
            if (ev.m_dwVer == 1) {
                g_client.crypt = 1;
                ht_log_info(&g_log, "crypt => 1.");
            }
            break;
        case xy_gameld::CMDT_RESPKEY:
            bis >> key;
            if (key.m_len != 0) {
                g_client.crypt = 2;
                g_client.key1 = key.GetKey(1);
                g_client.key2 = key.GetKey(2);
                g_client.key3 = key.GetKey(3);
            }
            sprintf(str, "resp key:%d, %d, %d.", g_client.key1,
                g_client.key2,
                g_client.key3);
            ht_log_info(&g_log, str);
            break;
    }
#ifdef TAO_DEBUG
    __android_log_print(ANDROID_LOG_INFO, "ldcore", "%s", "process_xy end");
#endif
}

/*
 * log callback
 */
void log_cb(const char* msg)
{
    if (msg != NULL) {
        __android_log_print(ANDROID_LOG_INFO, "ldcore", "%s", msg);
    }
}

/*
 * callback from ld_client
 */
void net_cb(int eid, int flag, void* data)
{
    char str[512];
    JNIEnv* env;
    jbyte* jb;
    jbyteArray jarray;
    ht_message* msg;

    sprintf(str, "net callback eid=%d, flag=%d.", eid, flag);
    ht_log_info(&g_log, str);
    env = NULL;
    switch (eid) {
        case EVENT_THREAD_START:
            g_jvm->AttachCurrentThread(&env, NULL);
            if (env == NULL) {
                ht_log_error(&g_log, "java env error!");
                return;
            }
            if (flag == THREAD_ID_READ)
                g_envRead = env;
            else if (flag == THREAD_ID_WRITE)
                g_envWrite = env;
            break;
        case EVENT_THREAD_END:
            g_jvm->DetachCurrentThread();
            if (flag == THREAD_ID_READ)
                g_envRead = NULL;
            else if (flag == THREAD_ID_WRITE)
                g_envWrite = NULL;
            break;
        case EVENT_NET_ONCONNECT:
            env = g_envWrite;
            if (env == NULL) {
                ht_log_error(&g_log, "java write env error!");
                return;
            }
            env->CallStaticObjectMethod(g_myjni, g_connect, flag);
            if (env->ExceptionCheck()) {
                ht_log_error(&g_log, "call static method(onConnect) failed!");
                return;
            }
            break;
        case EVENT_NET_ONRECEIVE:
            env = g_envRead;
            if (env == NULL) {
                ht_log_error(&g_log, "java read env error!");
                return;
            }
            msg = (ht_message*)data;
            if (msg == NULL || msg->data == NULL || msg->param1 <= 0) {
                ht_log_error(&g_log, "receive error callback!");
                return;
            }
            /* preprocess xieyi */
            process_xy((char*)msg->data, msg->param1);
            jarray = env->NewByteArray(msg->param1);
            if (jarray == NULL) {
                ht_log_error(&g_log, "java alloc byte array failed!");
                return;
            }
            jb = (jbyte*)msg->data;
            env->SetByteArrayRegion(jarray, 0, msg->param1, jb);
            env->CallStaticObjectMethod(g_myjni, g_receive, jarray);
            if (env->ExceptionCheck()) {
                ht_log_error(&g_log, "call static method(onReceive) failed!");
                return;
            }
            if (msg->data != NULL)
                free(msg->data);
            free(msg);
            break;
        case EVENT_NET_ONDISCONNECT:
            env = g_envRead;
            if (env == NULL) {
                ht_log_error(&g_log, "java env error!");
                return;
            }
            env->CallStaticObjectMethod(g_myjni, g_disconnect);
            if (env->ExceptionCheck()) {
                ht_log_error(&g_log, "call static method(onDisconnect) failed!");
                return;
            }
            break;
        default:
            break;
    }

#ifdef TAO_DEBUG
    ht_log_info(&g_log, "net callback end.");
#endif
}

/*
 * arm,x86 is LittleEndian
 * java is BitEndian
 */

/* This is a trivial JNI example where we use a native method
 * to return a new VM String.
 */
jstring
Java_com_gameld_core_MyJni_stringFromJNI( JNIEnv* env,
                                                  jclass thiz )
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #define ABI "armeabi-v7a/NEON"
    #else
      #define ABI "armeabi-v7a"
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__mips__)
   #define ABI "mips"
#else
   #define ABI "unknown"
#endif

    return env->NewStringUTF("Hello from JNI !  Compiled with ABI " ABI ".");
}

jint Java_com_gameld_core_MyJni_add(JNIEnv *, jclass, jint a, jint b)
{
    jint s = a + b;
    return s;
}

jstring
Java_com_gameld_core_MyJni_errorString(JNIEnv* env, jclass thiz)
{
    return env->NewStringUTF(g_log.errstr);
}

jint Java_com_gameld_core_MyJni_init(JNIEnv *env, jclass, jstring path)
{
    int r;

    env->GetJavaVM(&g_jvm);
    if (g_jvm == NULL)
        return HTERR_JAVA;

    /* java threa env cached */
    g_envJava = env;

    /* get java class */
    g_myjni = env->FindClass("com/gameld/core/MyJni");
    if (env->ExceptionCheck()) {
        ht_log_error(&g_log, "find class MyJni failed!");
        return HTERR_JAVA;
    }
    if (g_myjni == NULL) {
        ht_log_error(&g_log, "find com/gameld/core/MyJni failed!");
        return HTERR_JAVA;
    }
    /*
     * get a method
     * ()V: call param and return
     * no param and return void
     * etc:
     * "(III)Ljava/lang/String;"
     * need there integer(int) param and
     * return java String
     * !!!note: ";"
     */
    g_connect = env->GetStaticMethodID(g_myjni, "onConnect", "(I)V");
    if (env->ExceptionCheck()) {
        ht_log_error(&g_log, "get static method(onConnect) failed!");
        return HTERR_JAVA;
    }
    if (g_connect == NULL) {
        return HTERR_JAVA;
    }
    /*
     * [BV: need a byte array(byte[]) param and return void
     */
    g_receive = env->GetStaticMethodID(g_myjni, "onReceive", "([B)V");
    if (env->ExceptionCheck()) {
        ht_log_error(&g_log, "get static method(onReceive) failed!");
        return HTERR_JAVA;
    }
    if (g_receive == NULL) {
        return HTERR_JAVA;
    }
    g_disconnect = env->GetStaticMethodID(g_myjni, "onDisconnect", "()V");
    if (env->ExceptionCheck()) {
        ht_log_error(&g_log, "get static method(onDisconnect) failed!");
        return HTERR_JAVA;
    }
    if (g_disconnect == NULL) {
        return HTERR_JAVA;
    }

	const char *pstr = env->GetStringUTFChars(path, 0);
	r = ld_core_init(OS_ANDROID, pstr);
    g_client.callback = net_cb;
	env->ReleaseStringUTFChars(path, pstr);
	return r;
}

void Java_com_gameld_core_MyJni_clear(JNIEnv *, jclass)
{
	ld_core_clear();
}

void Java_com_gameld_core_MyJni_log(JNIEnv *env, jclass, jstring msg) {
    const char *pstr = env->GetStringUTFChars(msg, 0);
    if (pstr != NULL) {
        ht_log_info(&g_log, pstr);
    }
    env->ReleaseStringUTFChars(msg, pstr);
}

jint Java_com_gameld_core_MyJni_connect(JNIEnv *env, jclass,
    jstring jhost, jint port)
{
    const char *host = env->GetStringUTFChars(jhost, 0);
    if (host == NULL)
        return HTERR_PARAM;
#ifdef TAO_DEBUG
    char str[256];
    sprintf(str, "connect server:host=%s,port=%d.", host, port);
    ht_log_info(&g_log, str);
#endif
    int r = ld_client_connect(host, port);
    env->ReleaseStringUTFChars(jhost, host);
    return r;
}

jint Java_com_gameld_core_MyJni_send(JNIEnv *env, jclass, jbyteArray jarray)
{
    static jbyte carray[MAX_BUFFER_SIZE];
    char* buffer;
    int buff_len,offset;

#ifdef TAO_DEBUG
    __android_log_print(ANDROID_LOG_INFO,
        "ldcore","MyJni_send start:%p", jarray);
#endif

    if (jarray == NULL) {
        ht_log_error(&g_log, "send null array found!");
        return 0;
    }
    int len_data = env->GetArrayLength(jarray);
    if (len_data == 0 || len_data >= MAX_BUFFER_SIZE) {
        __android_log_print(ANDROID_LOG_ERROR, "ldcore",
                "send data error length=%d", len_data);
        return 0;
    }
    // copy data to c array
    env->GetByteArrayRegion(jarray, 0, len_data, carray);

    // jarray has include xieyi id
    if (len_data >= 0xff)
        buff_len = len_data + 3;
    else
        buff_len = len_data + 1;
    if (buff_len > MAX_BUFFER_SIZE) {
        __android_log_print(ANDROID_LOG_ERROR, "ldcore",
                "send data overflow length=%d", buff_len);
        return 0;
    }
    ht_log_info(&g_log, "3");
    buffer = (char*)malloc(buff_len);
    if (buffer == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "ldcore",
                "alloc %d bytes memory failed!", buff_len);
    }
    memset(buffer, 0, buff_len);

    // package
    gameld::bostream bos;
    bos.attach(buffer, buff_len);
    offset = 0;
    if (len_data >= 0xff) {
        bos << (char)0xff << (short)len_data;
        offset = 3;
    } else {
        bos << (char)len_data;
        offset = 1;
    }
    bos.write((char*)carray, len_data);
    // length head don't crypt
    if (g_client.crypt == 2) {
        ARACrypt ac;
        ac.SetKey(g_client.key1, g_client.key2, g_client.key3);
        ac.TransformString((unsigned char*)buffer + offset, len_data);
    } else if (g_client.crypt == 1) {
        ARACrypt ac;
        ac.SetKey(0, 0, 0);
        ac.TransformString((unsigned char*)buffer + offset, len_data);
    }
    int r = ld_client_send(buffer, buff_len);

#ifdef TAO_DEBUG
    __android_log_print(ANDROID_LOG_INFO,
        "ldcore","MyJni_send end:send return=%d", r);
#endif
    return r;
}

// only for debug
jint Java_com_gameld_core_MyJni_sendXieyi(JNIEnv *env, jclass, jbyteArray jarray)
{
    static jbyte carray[MAX_BUFFER_SIZE];

    //__android_log_print(ANDROID_LOG_INFO,
    //    "ldcore","MyJni_send start:%p", jarray);

    //int len_data = env->GetArrayLength(jarray);
    //__android_log_print(ANDROID_LOG_INFO,
    //    "ldcore","xieyi len:%d", len_data);

    // copy data to c array
    //env->GetByteArrayRegion(jarray, 0, len_data, carray);

    return 0;
}

void Java_com_gameld_core_MyJni_setParam(JNIEnv *, jclass, jint type, jint value)
{
    if (type == CORE_PARAM_LOG) {
        if (value != 0)
            g_log.callback = log_cb;
        else
            g_log.callback = NULL;
    }
}
