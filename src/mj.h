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

/* majiang suit */
typedef enum majiang_suit {
    mjSuitNone,
    mjSuitBamboo,               /* tiaozi pai */
    mjSuitTiao = mjBamboo,
    mjSuitSuo = mjBamboo,
    mjSuitCharacter,            /* wanzi pai */
    mjSuitWan = mjCharacter,
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
	mjNone,
    mj1,
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
	mjNone,
    mjEast,
    mjSouth,
    mjWest,
    mjNorth
}mjWind;

typedef enum majiang_dragon {
	mjNone,
    mjRed,
    mjZhong = mjRed,
    mjGreen,
    mjFa = mjGreen,
    mjWhite,
    mjBai = mjWhite
}mjDragon;

typedef enum majiang_flower {
	mjNone,
    mjPlum,
    mjMei = mjPlum,
    mjOrchid,
    mjLan = mjOrchid,
    mjChrysan,
    mjJu = mjChrysan,
    mjBamboo,
    mjZhu = mjBamboo
}mjFlower;

typedef enum majiang_season {
	mjNone,
    mjSpring,
    mjCun = mjSpring,
    mjSummer,
    mjXia = mjSummer,
    mjAutumn,
    mjQiu = mjAutumn,
    mjWinter,
    mjDong = mjWinter
}

/* a mj card */
typedef struct mj_s {
    int suit;   /* suit */
    int sign;   /* sign */
}mj_t;

int mj_equal(mj_t* a, mj_t* b);
unsigned char mj_encode(mj_t* card);
void mj_decode(mj_t* card, unsigned char x);

void mj_shuffle(mj_t* cards, int len);
/**
 * print cards to a readable string
 */
const char* mj_print(mj_t cards[], int len, int line_number);
const char* mj_string(mj_t* card);

#ifdef __cplusplus
}
#endif
#endif
