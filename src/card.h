/*
 * Poker game algorithm library
 * This file is distributed under the BSD License.
 * Copyright (C) 2014-2015 Huang Tao(huangtao117@gmail.com)
 */
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

/* a card */
typedef struct card_s{
    int suit;   /* card suit */
    int rank;   /* card rank */
}card_t;

/* one card function */
int card_equal(card_t* a, card_t* b);
char card_encode(card_t* card);
void card_decode(card_t* card, char x);
void n55_to_card(unsigned char cd, card_t* card);
unsigned char card_to_n55(card_t* card);
const char* card_to_string(card_t* card);
void card_from_string(card_t* card, const char* string);

/*
 * cards(hand or deck) function
 */

/* return valid number card form cards */
int cards_num(card_t* cards, int len);

/* hand have this card */
int cards_have(card_t* cards, int len, card_t* card);

int cards_rank_num(card_t* cards, int len, int rank);

int cards_suit_num(card_t* cards, int len, int suit);

/* add a card to hand */
int cards_add(card_t* cards, int len, card_t* card);

/* delete a card from hand */
int cards_del(card_t* cards, int len, card_t* card);

/* trim a card array */
int cards_trim(card_t* cards, int len);

/* print readable format */
void cards_print(card_t* cards, int len, int line_number);
void cards_dump(card_t* cards, int len, int line_number);

void cards_remove_rank(card_t* cards, int len, int rank);
void cards_remove_suit(card_t* cards, int len, int suit);

/* parse a string like "D2,H3,..." to a cards */
int cards_from_string(card_t* cards, int len, const char* string);
/* dump cards to string */
const char* cards_to_string(card_t* cards, int len);

/*
 * deck function
 */
int deck_init(card_t* cards, int len);
void deck_shuffle(card_t* cards, int num);

#ifdef __cplusplus
}
#endif
#endif
