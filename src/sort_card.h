/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _SORT_CARD_H
#define _SORT_CARD_H
#ifdef __cplusplus
extern "C" {
#endif

#include "hand.h"

/* sort,compare,etc... */
void cards_sort(hand_t* hand);
void cards_bucket(hand_t* hand, int x[]);
int cards_have_rank(int rank, int x[], int size);
int card_compare(const void* a, const void* b);
int card_logicvalue(card_t* card);
int card_interval(card_t* card1, card_t* card2);

#ifdef __cplusplus
}
#endif
#endif