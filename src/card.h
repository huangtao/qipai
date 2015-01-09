/* code: huangtao117@gmail.com */
#ifndef _CARD_H
#define _CARD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* poker suit define */
typedef enum card_suit
{
    cdSuitNone,     /* none */
    cdSuitDiamond,  /* diamond */
    cdSuitClub,     /* club */
    cdSuitHeart,    /* heart */
    cdSuitSpade,    /* spade */
    cdSuitJoker,    /* joker */
    cdSuitUnknow    /* unknow suit(ob) */
}cdSuit;

/* poker rank define */
typedef enum card_rank
{
    cdRankNone = 0,
    cdRankAce,
    cdRank2,
    cdRank3,
    cdRank4,
    cdRank5,
    cdRank6,
    cdRank7,
    cdRank8,
    cdRank9,
    cdRank10,
    cdRankJ,        /* jack */
    cdRankQ,        /* queen */
    cdRankK,        /* king */
    cdRankSJoker,   /* small joker */
    cdRankBJoker,   /* big joker */
    cdRankUnknow    /* unknow rank(ob) */
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
