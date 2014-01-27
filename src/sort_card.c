#include "sort_card.h"
#include <stdlib.h>

static int table_suit[6] = { 0, 1, 2, 3, 4, 5 };
static int table_rank[16] = { 0, 14, 15, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17 };

int card_compare(const void* a, const void* b)
{
    card_t *card1, *card2;

    card1 = (card_t*)a;
    card2 = (card_t*)b;
    if(!card1 || !card2)
        return 0;

    if(table_rank[card1->rank] < table_rank[card2->rank])
        return -1;

    if(table_rank[card1->rank] > table_rank[card2->rank])
        return 1;

    if(table_rank[card1->rank] == table_rank[card2->rank]){
        if(table_suit[card1->suit] < table_suit[card2->suit])
            return -1;
        if(table_suit[card1->suit] > table_suit[card2->suit])
            return 1;

        return 0;
    }

    return 0;
}

void cards_sort(hand_t* hand)
{
    if(!hand || !hand->cards)
        return;

    qsort(hand->cards, hand->num, sizeof(card_t), card_compare); 
}

void cards_bucket(hand_t* hand, int x[])
{
    int i,v;
    card_t* p;

    if(!hand || !hand->cards)
        return;
    for(i = 0; i < hand->num; ++i){
        p = hand->cards + i;
        v = card_logicvalue(p);
        x[v]++;
    }
}

int cards_have_rank(int rank, int x[], int size)
{
    int i,v;
    card_t card;

    card.rank = rank;
    card.suit = cdSuitHeart;
    v = card_logicvalue(&card);
    for(i = 0; i < size; ++i){
        if(x[i] == v)
            return 1;
    }

    return 0;
}

int card_interval(card_t* card1, card_t* card2)
{
    int r;

    if(!card1 || !card2)
        return 0;

	r = (table_rank[card1->rank] - table_rank[card2->rank]) / 10;

	return r;
}

int card_logicvalue(card_t* card)
{
    if(!card)
        return 0;

    if(card->rank > 15) return 0;

    return table_rank[card->rank];
}
