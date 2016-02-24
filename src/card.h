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

/* poker id */
typedef enum card_id {
	CD_ID_EMPTY,
	
	/* diamond A~K */
	CD_ID_DA, CD_ID_D2, CD_ID_D3, CD_ID_D4, CD_ID_D5,
	CD_ID_D6, CD_ID_D7, CD_ID_D8, CD_ID_D9, CD_ID_D10,
	CD_ID_DJ, CD_ID_DQ, CD_ID_DK,

	/* club A~K */
	CD_ID_CA, CD_ID_C2, CD_ID_C3, CD_ID_C4, CD_ID_C5,
	CD_ID_C6, CD_ID_C7, CD_ID_C8, CD_ID_C9, CD_ID_C10,
	CD_ID_CJ, CD_ID_CQ, CD_ID_CK,
	
	/* heart A~K */
	CD_ID_HA, CD_ID_H2, CD_ID_H3, CD_ID_H4, CD_ID_H5,
	CD_ID_H6, CD_ID_H7, CD_ID_H8, CD_ID_H9, CD_ID_H10,
	CD_ID_HJ, CD_ID_HQ, CD_ID_HK,
	
	/* spade A~K */
	CD_ID_SA, CD_ID_S2, CD_ID_S3, CD_ID_S4, CD_ID_S5,
	CD_ID_S6, CD_ID_S7, CD_ID_S8, CD_ID_S9, CD_ID_S10,
	CD_ID_SJ, CD_ID_SQ, CD_ID_SK,
	
	/* joker */
	CD_ID_JS, CD_ID_JB,
	
	/* for ob */
	CD_ID_UNKNOW
}cdID;

/* a card */
typedef struct card_s {
	int id;		/* card id */
    int suit;   /* card suit */
    int rank;   /* card rank */
}card_t;

/* card function */
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
