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

/* majiang suit */
typedef enum majiang_suit {
    mjSuitNone,
	mjSuitCharacter,            /* wanzi pai */
    mjSuitWan = mjSuitCharacter,
    mjSuitBamboo,               /* tiaozi pai */
    mjSuitTiao = mjSuitBamboo,
    mjSuitSuo = mjSuitBamboo,
    mjSuitCircle,               /* tongzi pai */
    mjSuitTong = mjSuitCircle,
    mjSuitWind,                 /* feng pai */
	mjSuitFeng = mjSuitWind,
    mjSuitDragon,
	mjSuitZFB = mjSuitDragon,	/* zhong,fa,bai */
    mjSuitFlower,               /* hua pai */
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
    mjRed = 1,
    mjZhong = mjRed,
    mjGreen,
    mjFa = mjGreen,
    mjWhite,
    mjBai = mjWhite
}mjDragon;

typedef enum majiang_flower {
    mjPlum = 1,
    mjMei = mjPlum,
    mjOrchid,
    mjLan = mjOrchid,
    mjChrysan,
    mjJu = mjChrysan,
    mjBamboo,
    mjZhu = mjBamboo
}mjFlower;

typedef enum majiang_season {
    mjSpring = 1,
    mjCun = mjSpring,
    mjSummer,
    mjXia = mjSummer,
    mjAutumn,
    mjQiu = mjAutumn,
    mjWinter,
    mjDong = mjWinter
}mjSeason;

typedef enum mj_melded_sets {
    mjMS_NONE,
    mjMS_KE,        /* 刻子 */
    mjMS_SHUN,      /* 顺子 */
    mjMS_JIANG,     /* 将牌(对子) */
    mjMS_CHI,       /* 吃 */
    mjMS_PENG,      /* 碰 */
    mjMS_GANG,      /* 杠 */
    mjMS_GANG_AN    /* 暗杠 */
}mjMeldedSets;

/* a mj pai */
typedef struct mjpai_s {
    int suit;   /* suit */
    int sign;   /* sign */
}mjpai_t;

/* 描述一组(2,3,4张)牌 */
typedef struct mj_sets_s {
    int type;
    mjpai_t card;
    int player_no;
    int info;
}mjSets_t;

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
