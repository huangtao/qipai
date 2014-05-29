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

/* bucket item */
typedef struct cd_bucket_s{
    int rank;
    int num_spade;
    int num_heart;
    int num_club;
    int num_diamond;
    int num_joker;
}cd_bucket;

/* sort,compare,etc... */
void cards_sort(hand_t* hand);
//void rank_bucket(hand_t* hand, int x[]);
void cards_bucket(hand_t* hand, cd_bucket x[]);
int get_bucket_suit(cd_bucket* item);
int cards_have_rank(int rank, int x[], int size);
int card_compare(const void* a, const void* b);
int card_logicvalue(card_t* card);
int rank2logic(int rank);
int logic2rank(int logic);
int card_interval(card_t* card1, card_t* card2);

#ifdef __cplusplus
}
#endif
#endif
