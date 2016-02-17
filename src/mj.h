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
 * 顺子：由同花色三个连续的牌组成。
 * 刻子：又称“坎”，由三张或四张（又称杠子）相同的牌组成。
 * 面子：顺子和刻子的统称。
 * 将头：又称“眼”，胡牌需要的对子。
 * 胡牌：一副牌必须凑满四（五）副面子及一组将，才可以胡牌。
 * 庄家：每局掷骰子决定开牌位置的人。
 * 连庄：庄家胡牌或者流局即可连庄。
 */

/* majiang tile suit */
typedef enum majiang_suit {
    mjSuitNone,
	mjSuitWan,                      /* 万子 */
    mjSuitCharacters = mjSuitWan,
    mjSuitTiao,                     /* 条子 */
    mjSuitSuo = mjSuitTiao,
    mjSuitBamboo = mjSuitTiao,
    mjSuitTong,                     /* 筒子 */
    mjSuitDots = mjSuitTong,
    mjSuitFeng,                     /* 风牌 */
	mjSuitWind = mjSuitFeng,
    mjSuitZFB,                      /* 中发白 */
	mjSuitDragon = mjSuitZFB,
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
    mjJu,
    mjChrysan = mjJu,
    mjZhu,
    mjBamboo = mjZhu
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

int mjpai_equal(mjpai_t* a, mjpai_t* b);
unsigned char mjpai_encode(mjpai_t* card);
void mjpai_decode(mjpai_t* card, unsigned char x);

void mj_shuffle(mjpai_t* cards, int len);
void mj_trim(mjpai_t* cards, int len);
/**
 * print cards to a readable string
 */
const char* mj_string(mjpai_t* cards, int len, int line_number);
const char* mjpai_string(mjpai_t* card);

/* 7对子判定 */
int mj_pair7(int* array, int len);

#ifdef __cplusplus
}
#endif
#endif
