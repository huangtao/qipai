/*
 * Mojang game algorithm library
 * This file is distributed under the BSD License.
 * Copyright (C) 2014-2015 Huang Tao(huangtao117@gmail.com)
 */
#ifndef _MOJANG_H
#define _MOJANG_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* mojang suit define */
typedef enum mj_suit
{
    mjSuitNone,     /* none */
    mjSuitWord,     /* 字牌 */
    mjSuitOrdinal,  /* 序数牌 */
    mjSuitFlower,   /* 花牌 */
    mjSuitAny,      /* 百搭牌 */
    mjSuitUnknow    /* unknow suit(ob) */
}cdSuit;

/* mojiang rank define */
typedef enum mj_rank
{
    mjRankNone = 0,
    mjRank1,
    mjRank2,
    mjRank3,
    mjRank4,
    mjRank5,
    mjRank6,
    mjRank7,
    mjRank8,
    mjRank9,
    mjRankEast,
    mjRankSouth,
    mjRankWeat,
    mjRankNorth,
    mjRankZhong,
    mjRankFa,
    mjRankBai,
    mjRankUnknow
}cdRank;

/* majiang suit */
typedef enum majiang_suit{
    mjSuitNone,
    mjSuitWan,      /* wanzi pai */
    mjSuitTiao,     /* tiaozi pai */
    mjSuitTong,     /* tongzi pai */
    mjSuitZi,       /* zi pai */
    mjSuitHua       /* hua pai */
}mjSuit;

/* majiang rank */
typedef enum majiang_rank{
    mjRankNone,
    mjRank1,
    mjRank2,
    mjRank3,
    mjRank4,
    mjRank5,
    mjRank6,
    mjRank7,
    mjRank8,
    mjRank9
}mjRank;

/* a card */
typedef struct card_s{
    int suit;   /* card suit */
    int rank;   /* card rank */
}card_t;

/* card oprate */
void card_init(card_t* card, const char* sn);
int card_equal(card_t* a, card_t* b);
char card_encode(card_t* card);
void card_decode(card_t* card, char x);

/**
 * print cards to a readable string
 */
const char* cards_print(card_t cards[], int len, int line_number);

char card_suit_char(card_t* card);
const char* card_rank_str(card_t* card);
const char* card_string(card_t* card);

#ifdef __cplusplus
}
#endif
#endif
