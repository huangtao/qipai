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
static gp_t* g_gp = NULL;
static hand_t* g_hand_select = NULL;
static hand_t* g_hand_out = NULL;

inline void ldcard_to_card(unsigned char cd, card_t* card)
{
    if (card && cd > 0) {
        card->rank = cdRankAce + (cd - 1) % 13;
        card->suit = cdSuitDiamond + (cd - 1) / 13;
    }
}

inline unsigned char card_to_ldcard(card_t* card)
{
    unsigned char cd;
    cd = ((card->suit - 1) * 13) + card->rank;
    return cd;
}

int Java_com_gameld_core_libqp_gpInit(JNIEnv *, jclass, jint type)
{
    g_gp = gp_new(type, GP_MODE_CLIENT);
    if (g_gp == NULL) {
        return 0;
    }
    g_hand_select = hand_new(15);
    g_hand_out = hand_new(15);
    return 1;
}

void Java_com_gameld_core_libqp_gpClear(JNIEnv *, jclass)
{
    hand_free(g_hand_select);
    hand_free(g_hand_out);
    gp_free(g_gp);
}

void Java_com_gameld_core_libqp_gpStart(JNIEnv *env, jclass,
    jint player_num, jint my_seat, jbyteArray jarray, jint fno)
{
    card_t* card;

    g_gp->player_num = player_num;
    gp_start(g_gp);
    g_gp->curr_player_no = fno;
    g_gp->first_player_no = fno;

    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    hand_t* hand = g_gp->players[my_seat].mycards;
    for (int i = 0; i < 15; i++) {
        if (p[i] > 0) {
            hand->num++;
            card = hand_get(hand, i);
            ldcard_to_card(p[i], card);
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    gp_sort(hand);

    int st = my_seat + 1;
    if (st >= g_gp->player_num)
        st = 0;
    hand = g_gp->players[st].mycards;
    for (int i = 0; i < 15; i++) {
        hand->num++;
        card = hand_get(hand, i);
        card->rank = cdRankUnknow;
        card->suit = 0;
    }
}

int Java_com_gameld_core_libqp_gpCanPlay(JNIEnv *env, jclass,jbyteArray jarray)
{
    card_t* card;

    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    g_hand_select->num = 0;
    for (int i = 0; i < 15; i++) {
        if (p[i] > 0) {
            g_hand_select->num++;
            card = hand_get(g_hand_select, i);
            ldcard_to_card(p[i], card);
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    return gp_canplay(g_gp, g_hand_select);
}

void Java_com_gameld_core_libqp_gpPlay(JNIEnv *env, jclass,jbyteArray jarray)
{
    card_t* card;

    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    g_hand_out->num = 0;
    for (int i = 0; i < 15; i++) {
        if (p[i] > 0) {
            g_hand_select->num++;
            card = hand_get(g_hand_out, i);
            ldcard_to_card(p[i], card);
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    gp_play(g_gp, g_gp->curr_player_no, g_hand_out);
}

int Java_com_gameld_core_libqp_gpCardNum(JNIEnv *env, jclass, int no)
{
    if (no >= g_gp->player_num) {
        return 0;
    }
    hand_t* hand = g_gp->players[no].mycards;
    return hand_num(hand);
}

int Java_com_gameld_core_libqp_gpGetCard(JNIEnv *env, jclass, int no, int index)
{
    if (no >= g_gp->player_num) {
        return 0;
    }
    hand_t* hand = g_gp->players[no].mycards;
    card_t* card = hand_get(hand, index);
    if (card == NULL) {
        return 0;
    }
    return card_to_ldcard(card);
}
