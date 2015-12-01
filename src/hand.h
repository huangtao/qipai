/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _HAND_H
#define _HAND_H
#ifdef __cplusplus
extern "C" {
#endif

#include "card.h"

/* return valid number card form cards */
int hand_num(card_t* cards, int len);

/* hand have this card */
int hand_have(card_t* cards, int len, card_t* card);

int hand_rank_num(card_t* cards, int len, int rank);

int hand_suit_num(card_t* cards, int len, int suit);

/* add a card to hand */
int hand_add(card_t* cards, int len, card_t* card);

/* delete a card from hand */
int hand_del(card_t* cards, int len, card_t* card);

/* trim a card array */
int hand_trim(card_t* cards, int len);

/* print readable format */
void hand_print(card_t* cards, int len, int line_number);
void hand_dump(card_t* cards, int len, int line_number);

void hand_remove_rank(card_t* cards, int len, int rank);
void hand_remove_suit(card_t* cards, int len, int suit);

/*
 * deck function
 */
int deck_init(card_t* cards, int len);
void deck_shuffle(card_t* cards, int num);

#ifdef __cplusplus
}
#endif
#endif
