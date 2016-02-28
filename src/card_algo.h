/*
* for poker game algorithm
* code: huangtao117@gmail.com
*
*/
#ifndef _CARD_ALGO_H
#define _CARD_ALGO_H
#ifdef __cplusplus
extern "C" {
#endif

#include "card.h"

/* bucket item */
typedef struct cd_bucket_s {
    int rank;
    int num_spade;
    int num_heart;
    int num_club;
    int num_diamond;
    int num_joker;
}cd_bucket;

/* used for analyse cards */
typedef struct cd_analyse_s {
    int valid_num;
    int num_1;
    int num_2;
    int num_3;
    int num_4;
	int num_5;
	int num_6;
	int num_7;
	int num_8;
    int count[CD_MAX_LOGIC + 1];
}cd_analyse;

/* sort,compare,etc... */
void cards_sort(card_t* cards, int len);

/* analyse a hand cards */
void cards_analyse(card_t* cards, int len, cd_analyse* result);

void cards_bucket(card_t* cards, int len, cd_bucket x[]);
int get_bucket_suit(cd_bucket* item);
int cards_have_rank(int rank, int x[], int size);
int card_compare(const void* a, const void* b);
int card_logic(card_t* card);
int rank2logic(int rank);
int logic2rank(int logic);

#ifdef __cplusplus
}
#endif
#endif
