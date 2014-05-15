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
        return 1;

    if(table_rank[card1->rank] > table_rank[card2->rank])
        return -1;

    if(table_rank[card1->rank] == table_rank[card2->rank]){
        if(table_suit[card1->suit] < table_suit[card2->suit])
            return 1;
        if(table_suit[card1->suit] > table_suit[card2->suit])
            return -1;

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

/*
void rank_bucket(hand_t* hand, int x[])
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
*/

/**
 * bucket a hand
 * return hand suit's number
 */
int cards_bucket(hand_t* hand, int x[])
{
    int i,v,n;
    int ss,sh,sc,sd;
    card_t* p;

    if(!hand || !hand->cards)
        return 0;
    ss = sh = sc = sd = 0;
    for(i = 0; i < hand->num; ++i){
        p = hand->cards + i;
        v = card_logicvalue(p);
        if(p->suit == cdSuitSpade){
            ss = 1;
            n = (x[v] >> 12) & 0xF;
            n++;
            x[v] &= 0xFFF;
            x[v] |= (n << 12);
        }
        else if(p->suit == cdSuitHeart){
            sh = 1;
            n = (x[v] >> 8) & 0xF;
            n++;
            x[v] &= 0xF0FF;
            x[v] |= (n << 8);
        }
        else if(p->suit == cdSuitClub){
            sc = 1;
            n = (x[v] >> 4) & 0xF;
            n++;
            x[v] &= 0xFF0F;
            x[v] |= (n << 4);
        }
        else if(p->suit == cdSuitDiamond){
            sd = 1;
            n = x[v] & 0xF;
            n++;
            x[v] &= 0xFFF0;
            x[v] |= n;
        }
        else
            x[v]++;
    }

    return (ss + sh + sc + sd);
}

int get_bucket_number(int value, int suit)
{
    int num_s,num_h,num_c,num_d;

    if(value == 0){
        return 0;
    }

    num_s = num_h = num_c = num_d = 0;
    num_s = value >> 12;
    num_h = (value >> 8) & 0xF;
    num_c = (value >> 4) & 0xF;
    num_d = value & 0xF;

    if(suit == cdSuitSpade)
        return num_s;
    else if(suit == cdSuitHeart)
        return num_h;
    else if(suit == cdSuitClub)
        return num_c;
    else if(suit == cdSuitDiamond)
        return num_d;

    return (num_s + num_h + num_c + num_d);
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
