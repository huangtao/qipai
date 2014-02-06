/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _DECK_H
#define _DECK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "hand.h"

/* a deck */
typedef struct deck_s{
    int num_pack;       /* number pack */
    int have_joker;     /* have_joker */
    int card_adv_id;    /* card back image id */
    int deal_index;     /* current deal card index */
    hand_t* poker;      /* all card */
}deck_t;

deck_t* deck_new(int number, int have_joker);
void deck_free(deck_t* deck);
void deck_shuffle(deck_t* deck);
int deck_deal(deck_t* deck, card_t* card);
int deck_get(deck_t* deck, int index, card_t* card);
int deck_num(deck_t* deck);
void deck_remove(deck_t* deck, card_t* card);
void deck_remove_rank(deck_t* deck, int rank);
void deck_remove_suit(deck_t* deck, int suit);

#ifdef __cplusplus
}
#endif
#endif
