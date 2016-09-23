/*
 * Majang game algorithm library
 * This file is distributed under the BSD License.
 * Copyright (C) 2015 Huang Tao(huangtao117@gmail.com)
 */
#ifndef _MAJANG_H
#define _MAJANG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * 麻将常用术语
 * 完整的麻将牌共152张
 * 字牌(28张)+花牌(8张)+序数牌(108张)+百搭牌(8张)
 * 常用的不含百搭144张，若没有花牌为136张。
 * 顺子：由同花色三个连续的牌组成。
 * 刻子：又称“坎”，由三张或四张（又称杠子）相同的牌组成。
 * 面子：顺子和刻子的统称。
 * 将头：又称“眼”、雀頭（吊头），胡牌需要的对子。
 * 胡牌：一副牌必须凑满四（五）副面子及一组将，才可以胡牌。
 * 庄家：每局掷骰子决定开牌位置的人。
 * 连庄：庄家胡牌或者流局即可连庄。
 * 番：也称台头，日语里面为役。
 * 自摸: Winning from the wall，摸牌起和（胡）。
 */

/* majiang tile suit */
typedef enum majiang_suit {
    mjSuitNone,
    mjSuitWan,                      /* 万子 */
    mjSuitCharacter = mjSuitWan,
    mjSuitTiao,                     /* 条子 */
    mjSuitSuo = mjSuitTiao,
    mjSuitBamboo = mjSuitTiao,
    mjSuitTong,                     /* 筒子 */
    mjSuitCircle = mjSuitTong,
    mjSuitFeng,                     /* 风牌 */
    mjSuitWind = mjSuitFeng,
    mjSuitSanY,                     /* 三元牌(中发白) */
    mjSuitZFB = mjSuitSanY,
    mjSuitDragon = mjSuitSanY,
    mjSuitHua,                      /* 花牌 */
    mjSuitFlower = mjSuitHua,
    mjSuitSeason,
    mjSuitUnknow
}mjSuit;

/* majiang ordinal */
typedef enum majiang_ordinal {
    mj1 = 1,
    mj2,
    mj3,
    mj4,
    mj5,
    mj6,
    mj7,
    mj8,
    mj9
}mjOrdinal;

typedef enum majiang_wind {
    mjEast = 1,
    mjSouth,
    mjWest,
    mjNorth
}mjWind;

typedef enum majiang_dragon {
    mjZhong = 1,
    mjRed = mjZhong,
    mjFa,
    mjGreen = mjFa,
    mjBai,
    mjWhite = mjBai
}mjDragon;

typedef enum majiang_flower {
    mjMei = 1,
    mjPlum = mjMei,
    mjLan,
    mjOrchid = mjLan,
    mjZhu,
    mjBamboo = mjZhu,
    mjJu,
    mjChrysan = mjJu
}mjFlower;

typedef enum majiang_season {
    mjCun = 1,
    mjSpring = mjCun,
    mjXia,
    mjSummer = mjXia,
    mjQiu,
    mjAutumn = mjQiu,
    mjDong,
    mjWinter = mjDong
}mjSeason;

typedef enum majiang_id {
    MJ_ID_EMPTY,
    MJ_ID_1W, MJ_ID_2W, MJ_ID_3W, MJ_ID_4W, MJ_ID_5W, MJ_ID_6W, MJ_ID_7W, MJ_ID_8W, MJ_ID_9W,
    MJ_ID_1S, MJ_ID_2S, MJ_ID_3S, MJ_ID_4S, MJ_ID_5S, MJ_ID_6S, MJ_ID_7S, MJ_ID_8S, MJ_ID_9S,
    MJ_ID_1T, MJ_ID_2T, MJ_ID_3T, MJ_ID_4T, MJ_ID_5T, MJ_ID_6T, MJ_ID_7T, MJ_ID_8T, MJ_ID_9T,
    MJ_ID_DONG, MJ_ID_NAN, MJ_ID_XI, MJ_ID_BEI,
    MJ_ID_ZHONG, MJ_ID_FA, MJ_ID_BAI,
    MJ_ID_MEI, MJ_ID_LAN, MJ_ID_ZHU, MJ_ID_JU,
    MJ_ID_CUN, MJ_ID_XIA, MJ_ID_QIU, MJ_ID_SDONG,
    MJ_ID_UNKNOW
}mjID;

/* 面子类型 */
typedef enum mj_meld_type {
    mjMeldNone,
    mjMeldKe,       /* 刻子 */
    mjMeldShun,     /* 顺子 */
    mjMeldJiang,    /* 将牌(对子) */
    mjMeldChi,      /* 吃获取的顺子 */
    mjMeldPeng,     /* 碰获取的刻子 */
    mjMeldGang,     /* 杠 */
    mjMeldAngang    /* 暗杠 */
}mjMeldType;

/* a mj pai */
typedef struct mjpai_s {
    int id;     /* start with 1 */
    int suit;   /* suit */
    int sign;   /* sign */
}mjpai_t;

/* 面子 */
typedef struct mj_melded_s {
    int type;       /* 面子类型 */
    mjpai_t card;   /* 特征牌 */
    int player_no;  /* 吃碰目标玩家 */
}mj_melded_t;

void mjpai_init_id(mjpai_t* pai, int id);
void mjpai_init_ss(mjpai_t* pai, int suit, int sign);
void mjpai_zero(mjpai_t* pai);
/* get id from suit & sign */
int mjpai_ss2id(int suit, int sign);
void mjpai_copy(mjpai_t* dest, mjpai_t* src);
void mjpai_decode(mjpai_t* card, unsigned char x);

void mj_shuffle(mjpai_t* cards, int len);
void mj_trim(mjpai_t* cards, int len);
/**
 * print cards to a readable string
 */
const char* mj_string(mjpai_t* cards, int len, int line_number);
const char* mjpai_string(mjpai_t* card);

#ifdef __cplusplus
}
#endif
#endif
