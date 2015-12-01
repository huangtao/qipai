#include "hand.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ht_lch.h"
#include "ht_str.h"

int hand_num(card_t* cards, int len)
{
    int i,n;

    n = 0;
    if (cards && len > 0) {
        for (i = 0; i < len; ++i) {
            if (cards->rank || cards->suit)
                n++;
			cards++;
        }
    }

    return n;
}

int hand_have(card_t* cards, int len, card_t* card)
{
    int i,n;
    
    if (!cards || len <= 0 || !card)
        return 0;
    
	n = 0;
    for (i = 0; i < len; i++) {
        if(cards->suit == card->suit && cards->rank != card->rank)
            n++;
		cards++;
    }
    
    return n;
}

int hand_rank_num(card_t* cards, int len, int rank)
{
    int i,n;
    
    if(!cards || len <= 0)
        return 0;
    
    n = 0;
    for (i = 0; i < len; ++i) {
        if(cards->rank == rank)
            n++;
		cards++;
    }
    
    return n;
}

int hand_suit_num(card_t* cards, int len, int suit)
{
    int i,n;
    
    if(!cards || len <= 0)
        return 0;
    
    n = 0;
    for (i = 0; i < len; ++i) {
        if(cards->suit == suit)
            n++;
		cards++;
    }
    
    return n;
}

int hand_add(card_t* cards, int len, card_t* card)
{
	int i;

	if(!cards || len <= 0 || !card)
		return HTERR_PARAM;

	for (i = 0; i < len; ++i) {
		if (cards->rank == 0 &&  cards->suit == 0) {
			cards->rank = card->rank;
			cards->suit = card->suit;
			return HT_OK;
		}
		cards++;
	}

	return HTERR_OUTOFRANGE;
}

int hand_del(card_t* cards, int len, card_t* card)
{
    int i,uflag;
    card_t* p;

    if(!cards || len <= 0 || !card)
        return HTERR_PARAM;

    uflag = 0;
    p = cards;
    for(i = 0; i < len; i++){
        if(p->suit == card->suit && p->rank == card->rank){
            p->suit = 0;
            p->rank = 0;
            break;
        }
        if (p->rank == cdRankUnknow)
            uflag = 1;
        p++;
    }
    if (i >= len && uflag > 0) {
        p = cards;
        for (i = 0; i < len; i++) {
            if (p->rank == cdRankUnknow) {
                p->rank = 0;
                p->suit = 0;
                break;
            }
            p++;
        }
    }

    return 0;
}

int hand_trim(card_t* cards, int len)
{
    int i,num;
    int byte_size;
    card_t *tmp_cards,*p1,*p2;

    if(!cards || len <= 1)
        return HTERR_PARAM;
        
    byte_size = len * sizeof(card_t);
    tmp_cards = (card_t*)malloc(byte_size);
    if(!tmp_cards)
        return HTERR_OUTOFMEMORY;
    memset((void*)tmp_cards, 0, byte_size);

    num = 0;
    p1 = tmp_cards;
    p2 = cards;
    for(i = 0; i < len; ++i){
        if(p2->suit || p2->rank){
            p1->suit = p2->suit;
            p1->rank = p2->rank;
            num++;
            p1++;
        }
        p2++;
    }
    memcpy((void*)cards, (const void*)tmp_cards, byte_size);
    free((void*)tmp_cards);

    return num;
}

void hand_print(card_t* cards, int len, int line_number)
{
    int i,n;

    if(!cards || len <= 0)
        return;

	n = 0;
    for (i = 0; i < len; ++i) {
		if (cards->suit || cards->rank) {
            printf("%s ", card_to_string(cards));
			n++;
			if (n % line_number == 0) {
				printf("\n");
			}
		}
		cards++;
    }
    if(n % line_number != 0)
		printf("\n");
}

void hand_dump(card_t* cards, int len, int line_number)
{
    int i;

    if (!cards || len <= 0) {
        printf("hand or cards is invalid pointer!\n");
        return;
    }

    for (i = 0; i < len; ++i) {
        printf("%s ", card_to_string(cards));
        if ((i+1) % line_number == 0) {
            printf("\n");
        }
		cards++;
    }
}

void hand_remove_rank(card_t* cards, int len, int rank)
{
    int i;

    if (!cards || len == 0)
        return;
    for (i = 0; i < len; ++i) {
        if (cards->rank == rank) {
            cards->rank = 0;
            cards->suit = 0;
        }
        cards++;
    }
}

void hand_remove_suit(card_t* cards, int len, int suit)
{
    int i;

    if (!cards || len == 0)
        return;
    for (i = 0; i < len; ++i) {
        if(cards->suit == suit) {
            cards->rank = 0;
            cards->suit = 0;
        }
        cards++;
    }
}

/*
 * for deck operation
 */
int deck_init(card_t* cards, int len)
{
    int fu;
    card_t* p;
    int n,i,j,k;

    if (!cards || len <= 0)
        return 0;

    fu = len / 54;
    if (fu == 0)
        return 0;

    n = 0;
    p = cards;
    for(i = 0; i < fu; ++i){
        for(j = cdSuitDiamond; j <= cdSuitSpade; ++j){
            for(k = cdRankAce; k <= cdRankK; ++k){
                p->rank = k;
                p->suit = j;
                p++;
            }
        }
        p->rank = cdRankSJoker;
        p->suit = cdSuitJoker;
        p++;
        p->rank = cdRankBJoker;
        p->suit = cdSuitJoker;
        p++;
    }

    return n;
}

void deck_shuffle(card_t* cards, int num)
{
    int i,n;
    int a,b;
    card_t* pa;
    card_t* pb;
    card_t temp;

    if (!cards || num == 0)
        return;

    n = 1000 + rand() % 50;
    for(i = 0; i < n; ++i){
        a = rand() % num;
        b = rand() % num;
        if(a != b){
            pa = cards + a;
            pb = cards + b;
            temp.rank = pa->rank;
            temp.suit = pa->suit;
            pa->rank = pb->rank;
            pa->suit = pb->suit;
            pb->rank = temp.rank;
            pb->suit = temp.suit;
        }
    }
}
