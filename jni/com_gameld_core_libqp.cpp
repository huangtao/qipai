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
#include "com_gameld_core_libqp.h"
#include "qipai.h"

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
static gp_t g_gp;

int Java_com_gameld_core_libqp_gpInit(JNIEnv *, jclass, jint type)
{
    gp_init(&g_gp, type, GP_MODE_CLIENT, 2);
    return 1;
}

void Java_com_gameld_core_libqp_gpClear(JNIEnv *, jclass)
{
}

void Java_com_gameld_core_libqp_gpStart(JNIEnv *env, jclass,
    jint player_num, jint my_seat, jbyteArray jarray, jint fno)
{
    card_t* card;

    g_gp.player_num = player_num;
    gp_start(&g_gp);
    g_gp.curr_player_no = fno;
    g_gp.first_player_no = fno;

    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    card = g_gp.players[my_seat].cards;
    for (int i = 0; i < 15; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], card);
            card++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    gp_sort(g_gp.players[my_seat].cards, GP_MAX_CARDS);

    int st = my_seat + 1;
    if (st >= g_gp.player_num)
        st = 0;
    card = g_gp.players[st].cards;
    for (int i = 0; i < 15; i++) {
        card->rank = cdRankUnknow;
        card->suit = cdSuitUnknow;
        card++;
    }
}

int Java_com_gameld_core_libqp_gpCanPlay(JNIEnv *env, jclass,jbyteArray jarray)
{
    int n;
    card_t cards[GP_MAX_CARDS];

    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    n = 0;
    for (int i = 0; i < 15; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], cards + n);
            n++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    return gp_canplay(&g_gp, cards, GP_MAX_CARDS);
}

void Java_com_gameld_core_libqp_gpPlay(JNIEnv *env, jclass,jbyteArray jarray)
{
    int n;
    card_t cards[GP_MAX_CARDS];

    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    n = 0;
    for (int i = 0; i < 15; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], cards + n);
            n++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    gp_play(&g_gp, g_gp.curr_player_no, cards, GP_MAX_CARDS);
}

int Java_com_gameld_core_libqp_gpCardNum(JNIEnv *env, jclass, int no)
{
    if (no >= g_gp.player_num) {
        return 0;
    }
    return cards_num(g_gp.players[no].cards, GP_MAX_CARDS);
}

int Java_com_gameld_core_libqp_gpGetCard(JNIEnv *env, jclass, int no, int index)
{
    if (no >= g_gp.player_num) {
        return 0;
    }
    if (index >= GP_MAX_CARDS) {
        return 0;
    }
    return card_to_n55(g_gp.players[no].cards + index);
}
