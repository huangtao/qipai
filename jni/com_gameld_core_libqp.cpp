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

/* comment this line to release */
#define TAO_DEBUG 1

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

int Java_com_gameld_core_libqp_gpGetState(JNIEnv *, jclass)
{
    return g_gp.game_state;
}

void Java_com_gameld_core_libqp_gpSetState(JNIEnv *, jclass, jint state)
{
    g_gp.game_state = state;
}

void Java_com_gameld_core_libqp_gpStart(JNIEnv *env, jclass,
    jbyteArray jarray0, jbyteArray jarray1, jint fno, jint pno, jbyteArray jarr_played)
{
    card_t* card;

    g_gp.player_num = 2;
    gp_start(&g_gp);
    g_gp.curr_player_no = fno;
    g_gp.first_player_no = fno;

    jbyte* p = env->GetByteArrayElements(jarray0, NULL);
    jsize size = env->GetArrayLength(jarray0);
    card = g_gp.players[0].cards;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], card);
            card++;
        }
    }
    env->ReleaseByteArrayElements(jarray0, p, 0);
    gp_sort(g_gp.players[0].cards, GP_MAX_CARDS);

    p = env->GetByteArrayElements(jarray1, NULL);
    size = env->GetArrayLength(jarray1);
    card = g_gp.players[1].cards;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], card);
            card++;
        }
    }
    env->ReleaseByteArrayElements(jarray1, p, 0);
    gp_sort(g_gp.players[1].cards, GP_MAX_CARDS);

    if (pno >= 0 && pno < g_gp.player_num) {
        p = env->GetByteArrayElements(jarr_played, NULL);
        size = env->GetArrayLength(jarr_played);
        card = g_gp.players[pno].cards_played;
        for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
            if (p[i] > 0) {
                n55_to_card(p[i], card);
                card++;
            }
        }
        env->ReleaseByteArrayElements(jarr_played, p, 0);
        if (size > 0) {
            memcpy(g_gp.last_hand, g_gp.players[pno].cards_played,
                sizeof(card_t) * GP_MAX_CARDS);
            gp_handtype(&g_gp, g_gp.players[pno].cards_played, GP_MAX_CARDS,
                &g_gp.last_hand_type);
            g_gp.largest_player_no = pno;
        }
    }

#ifdef TAO_DEBUG
    __android_log_print(ANDROID_LOG_INFO, "qipai", "cards0:%d,cards1:%d",
        cards_num(g_gp.players[0].cards, GP_MAX_CARDS),
        cards_num(g_gp.players[1].cards, GP_MAX_CARDS));
        //cards_to_string(g_gp.players[my_seat].cards, GP_MAX_CARDS));
#endif
}

void Java_com_gameld_core_libqp_gpSetCards(JNIEnv *env, jclass,
    jint no, jbyteArray jarray)
{
    card_t* card;

    if (no >= g_gp.player_num)
        return;

    memset(g_gp.players[no].cards, 0, sizeof(card_t) * GP_MAX_PLAYER);
    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    card = g_gp.players[no].cards;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], card);
            card++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    gp_sort(g_gp.players[no].cards, GP_MAX_CARDS);

    #ifdef TAO_DEBUG
        __android_log_print(ANDROID_LOG_INFO, "qipai", "gpSetCards:%s",
            cards_to_string(g_gp.players[no].cards, GP_MAX_CARDS));
    #endif
}

void Java_com_gameld_core_libqp_gpSetPlayedCards(JNIEnv *env, jclass,
    jint no, jbyteArray jarray)
{
    card_t* card;

    if (no >= g_gp.player_num)
        return;

    memset(g_gp.last_hand, 0, sizeof(card_t) * GP_MAX_CARDS);
    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    card = g_gp.players[no].cards_played;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], card);
            card++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);
    if (size > 0) {
        memcpy(g_gp.last_hand, g_gp.players[no].cards_played,
            sizeof(card_t) * GP_MAX_CARDS);
        gp_handtype(&g_gp, g_gp.players[no].cards_played, GP_MAX_CARDS,
            &g_gp.last_hand_type);
        g_gp.largest_player_no = no;
    }
}

int Java_com_gameld_core_libqp_gpCanPlay(JNIEnv *env, jclass,jbyteArray jarray)
{
    int ret,n;
    card_t cards[GP_MAX_CARDS];

    memset(cards, 0, sizeof(card_t) * GP_MAX_CARDS);
    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    n = 0;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], cards + n);
            n++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    ret = gp_canplay(&g_gp, cards, GP_MAX_CARDS);

#ifdef TAO_DEBUG
        char dbg_str[512];
        strcpy(dbg_str, cards_to_string(cards, GP_MAX_CARDS));
        __android_log_print(ANDROID_LOG_INFO, "qipai", "play cards:%s", dbg_str);
        __android_log_print(ANDROID_LOG_INFO, "qipai", "last type:%d,num=%d",
            g_gp.last_hand_type.type, g_gp.last_hand_type.num);
#endif

    return ret;
}

void Java_com_gameld_core_libqp_gpPlay(JNIEnv *env, jclass,jbyteArray jarray)
{
    int n;
    card_t cards[GP_MAX_CARDS];

    memset(cards, 0, sizeof(card_t) * GP_MAX_CARDS);
    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    n = 0;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], cards + n);
            n++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);

    gp_play(&g_gp, g_gp.curr_player_no, cards, GP_MAX_CARDS);
}

void Java_com_gameld_core_libqp_gpPass(JNIEnv *env, jclass)
{
    gp_pass(&g_gp, g_gp.curr_player_no);
}

jbyteArray Java_com_gameld_core_libqp_gpHint(JNIEnv *env, jclass, jint flag)
{
    int i,n;
    unsigned char data[GP_MAX_CARDS];
    card_t cards[GP_MAX_CARDS];

    n = gp_hint(&g_gp, cards, GP_MAX_CARDS, flag);
    memset(data, 0, GP_MAX_CARDS);
    if (n > 0) {
        for (i = 0; i < n; ++i) {
            data[i] = cards[i].id;
        }
    }
    jbyteArray array = env->NewByteArray(GP_MAX_CARDS);
    env->SetByteArrayRegion(array, 0, GP_MAX_CARDS, (jbyte*)data);
    return array;
}

int Java_com_gameld_core_libqp_gpGetCurrentNo(JNIEnv *, jclass)
{
    return g_gp.curr_player_no;
}

int Java_com_gameld_core_libqp_gpGetHandType(JNIEnv *env, jclass, jbyteArray jarray)
{
    int n;
    hand_type htype;
    card_t cards[GP_MAX_CARDS];

    memset(cards, 0, sizeof(card_t) * GP_MAX_CARDS);
    jbyte* p = env->GetByteArrayElements(jarray, NULL);
    jsize size = env->GetArrayLength(jarray);
    n = 0;
    for (int i = 0; i < size && i < GP_MAX_CARDS; i++) {
        if (p[i] > 0) {
            n55_to_card(p[i], cards + n);
            n++;
        }
    }
    env->ReleaseByteArrayElements(jarray, p, 0);
    gp_handtype(&g_gp, cards, GP_MAX_CARDS, &htype);
    return htype.type;
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

int Java_com_gameld_core_libqp_gpCardPlayedNum(JNIEnv *env, jclass, int no)
{
    if (no >= g_gp.player_num) {
        return 0;
    }
    return cards_num(g_gp.players[no].cards_played, GP_MAX_CARDS);
}

int Java_com_gameld_core_libqp_gpGetCardPlayed(JNIEnv *env, jclass, int no, int index)
{
    if (no >= g_gp.player_num) {
        return 0;
    }
    if (index >= GP_MAX_CARDS) {
        return 0;
    }
    return card_to_n55(g_gp.players[no].cards_played + index);
}

jbyteArray Java_com_gameld_core_libqp_gpGetCards(JNIEnv *env, jclass, int no)
{
    int i,n;
    unsigned char data[GP_MAX_CARDS];

    if (no >= g_gp.player_num) {
        n = 0;
    } else {
        n = cards_num(g_gp.players[no].cards, GP_MAX_CARDS);
    }

    memset(data, 0, GP_MAX_CARDS);
    if (n > 0) {
        for (i = 0; i < n; ++i) {
            data[i] = card_to_n55(g_gp.players[no].cards + i);
        }
    }
    jbyteArray array = env->NewByteArray(n);
    if (n > 0)
        env->SetByteArrayRegion(array, 0, n, (jbyte*)data);
    return array;
}

jbyteArray Java_com_gameld_core_libqp_gpGetCardsPlayed(JNIEnv *env, jclass, int no)
{
    int i,n;
    unsigned char data[GP_MAX_CARDS];

    if (no >= g_gp.player_num) {
        n = 0;
    } else {
        n = cards_num(g_gp.players[no].cards_played, GP_MAX_CARDS);
    }

    memset(data, 0, GP_MAX_CARDS);
    if (n > 0) {
        for (i = 0; i < n; ++i) {
            data[i] = card_to_n55(g_gp.players[no].cards_played + i);
        }
    }
    jbyteArray array = env->NewByteArray(n);
    if (n > 0)
        env->SetByteArrayRegion(array, 0, n, (jbyte*)data);
    return array;
}
