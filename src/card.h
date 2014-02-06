/* code: huangtao117@gmail.com */
#ifndef _CARD_H
#define _CARD_H
#ifdef __cplusplus
extern "C" {
#endif

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

/* represent a collection of cards, for example a hand or a deck */
//typedef struct card_coll_s{
//    int num;        /* current valid data number of array */
//    int max_size;   /* maximum number of array */
//    card_t* cards;  /* card array */
//}card_coll;

/* for card type calc */
//typedef struct card_type_s{
//    int type;           /* type enum */
//    int num;            /* number */
//    int logic_value;    /* logic value */
//}card_type;

/* card oprate */
void card_init(card_t* card, const char* sn);
int card_equal(card_t* a, card_t* b);
char card_encode(card_t* card);
void card_decode(card_t* card, char x);

///* card collection oprate */
//card_coll* card_coll_new(int max_size);
//void card_coll_free(card_coll* coll);
//void card_coll_zero(card_coll* coll);
//int card_coll_num(card_coll* coll);
//card_coll* card_coll_clone(card_coll* coll);
//card_t* card_coll_get(card_coll* coll, int n);
//int card_coll_push(card_coll* coll, card_t* card);
//int card_coll_pop(card_coll* coll, card_t* card);
//int card_coll_del(card_coll* coll, card_t* card);
//int card_cool_trim(card_coll* coll);
///* print readable format */
//void card_cool_print(card_coll* coll, int line_number);
//const char* card_text(card_t* card);

#ifdef __cplusplus
}
#endif
#endif
