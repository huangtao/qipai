#include "hand.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ht_lch.h"

static char c_suit[] = {'?','D','C','H','S','*'};
static char c_rank[] = {'?','A','2','3','4','5','6','7','8','9','T','J','Q','K','F','Z'};

hand_t* hand_new(int max_size)
{
    int byte_size;
    hand_t* p;

    if(max_size <= 0)
        return 0;

    byte_size = sizeof(hand_t) + sizeof(card_t) * max_size;
    p = (hand_t*)malloc(byte_size);
    if(p){
        memset(p, 0, byte_size);
        p->max_size = max_size;
    }

    return p;
}

void hand_free(hand_t* hand)
{
    free(hand);
}

void hand_zero(hand_t* hand)
{
    int i;
    card_t *p;

    if(!hand || !hand->cards)
        return;

    p = hand->cards;
    for(i = 0; i < hand->max_size; ++i){
        p->rank = 0;
        p->suit = 0;
        p++;
    }
    hand->num = 0;
}

int hand_num(hand_t* hand)
{
    int i,n;
    card_t* p;

    n = 0;
    if(hand){
        for(i = 0; i < hand->max_size; ++i){
            p = hand->cards + i;
            if(p->rank || p->suit)
                n++;
        }
    }

    return n;
}

hand_t* hand_clone(hand_t* hand)
{
    hand_t* p;
    int byte_size;

    if(!hand)
        return 0;

    byte_size = sizeof(hand_t) + hand->max_size * sizeof(card_t);
    p = (hand_t*)malloc(byte_size);
    if(p){
        memcpy(p, hand, byte_size);
    }

    return p;
}

card_t* hand_get(hand_t* hand, int n)
{
    card_t* p;

    if(!hand)
        return 0;

    if(n >= hand->max_size)
        return 0;

    p = hand->cards + n;

    return p;
}

int hand_push(hand_t* hand, card_t* card)
{
    card_t* p;

    if(!hand || !card)
        return HTERR_PARAM;

    if(hand->num == hand->max_size)
        return HTERR_OUTOFRANGE;

    p = hand->cards + hand->num;
    p->rank = card->rank;
    p->suit = card->suit;
    hand->num++;

    return 0;
}

int hand_pop(hand_t* hand, card_t* card)
{
    card_t* p;

    if(!hand || !card)
        return HTERR_PARAM;

    if(hand->num < 1)
        return HTERR_OUTOFRANGE;

    p = hand->cards + (hand->num - 1);
    card->suit = p->suit;
    card->rank = p->rank;
    hand->num--;

    return 0;
}

int hand_del(hand_t* hand, card_t* card)
{
    int i;
    card_t* p;

    if(!hand || !card)
        return HTERR_PARAM;

    for(i = 0; i < hand->num; i++){
        p = hand->cards + i;
        if(p->suit == card->suit && p->rank == card->rank){
            p->suit = 0;
            p->rank = 0;
        }
    }

    return 0;
}

int hand_trim(hand_t* hand)
{
    int i,j,num;
    card_t *p1,*p2;

    if(!hand)
        return HTERR_PARAM;
    if(hand->num < 1)
        return 0;

    num = 0;
    for(i = 0; i < hand->num; ++i){
        p1 = hand->cards + i;
        if(p1->suit == 0 && p1->rank == 0){
            for(j = i + 1; j < hand->num; ++j){
                p2 = hand->cards + j;
                if(p2->suit || p2->rank){
                    p1->rank = p2->rank;
                    p1->suit = p2->suit;
                    num++;
                    p2->rank = p2->suit = 0;
                    break;
                }
            }
        }
        else
            num++;
    }

    return 0;
}

void hand_print(hand_t* hand, int line_number)
{
    int i;
    card_t* p;

    if(!hand || !hand->cards)
        return;

    for(i = 0; i < hand->num; ++i){
        p = hand->cards + i;
        printf("%c%c ", c_suit[p->suit], c_rank[p->rank]);
        if((i+1) % line_number == 0){
            printf("\n");
        }
    }
    if(hand->num % line_number != 0)
		printf("\n");
}

const char* card_text(card_t* card)
{
    static char readable[8];
    if(!card)
        return 0;

    readable[2] = 0;
    sprintf(readable, "%c%c", c_suit[card->suit], c_rank[card->rank]);

    return readable;
}
