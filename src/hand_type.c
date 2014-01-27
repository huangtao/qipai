#include "hand_type.h"
#include <stdlib.h>
#include <string.h>
#include "sort_card.h"

int ht_single(hand_t* hand)
{
    if(!hand || !hand->cards)
        return 0;
    if(hand->num != 1)
        return 0;
    else
        return 1;
}

int ht_double(hand_t* hand)
{
    card_t *p1,*p2;

    if(!hand || !hand->cards)
        return 0;
    if(hand->num != 2)
        return 0;

    p1 = hand->cards;
    p2 = hand->cards + 1;
    if(p1->rank != p2->rank)
        return 0;

    return 1;
}

int ht_three(hand_t* hand)
{
    int i;
    card_t *p1,*p2;

    if(!hand || !hand->cards)
        return 0;
    if(hand->num != 3)
        return 0;

    p1 = hand->cards;
    for(i = 1; i < hand->num; ++i){
        p2 = hand->cards + i;
        if(p1->rank != p2->rank)
            return 0;
    }

    return 1;
}

int ht_three_p2(hand_t* hand)
{
    int i,n,max;
    int x[16];

    if(!hand || !hand->cards)
        return 0;
    if(hand->num != 5)
        return 0;

    memset(x, 0, sizeof(int)*16);
    cards_bucket(hand, x);
    n = max = 0;
    for(i = 0; i < 16; ++i){
        if(x[i] == 0) continue;
        if(x[i] != 2 || x[i] != 3)
            return 0;
        if(x[i] == 3)
            max = i;
    }
    if(n != 2)
        return 0;

    return max;
}

int ht_bomb(hand_t* hand)
{
    int i;
    card_t *p1,*p2;

    if(!hand || !hand->cards)
        return 0;
    if(hand->num != 4)
        return 0;

    p1 = hand->cards;
    for(i = 1; i < hand->num; ++i){
        p2 = hand->cards + i;
        if(p1->rank != p2->rank)
            return 0;
    }

    return 1;
}

int ht_straight(hand_t* hand)
{
    int i,j,k,n,v;
    int x[16];

    if(!hand || !hand->cards)
        return 0;

    if(!hand || !hand->cards)
        return 0;
    if(hand->num < 5)
        return 0;

    memset(x, 0, sizeof(int)*16);
    cards_bucket(hand, x);
    n = v = 0;
    for(i = 0; i < 16; ++i){
        if(x[i] == 0) continue;
        break;
    }
    for(j = i; j < 16; ++j){
        if(x[j] != 1)
            return 0;
        if(x[j] == 0)
            break;
        if(j > v)
            v = j;
    }
    for(k = j; k < 16; ++k){
        if(x[k] != 0)
            return 0;
    }

    return v;
}

int ht_double_straight(hand_t* hand)
{
    int i,j,k,n,v;
    int x[16];

    if(!hand || !hand->cards)
        return 0;

    if(!hand || !hand->cards)
        return 0;
    if(hand->num < 4)
        return 0;
    if(hand->num % 2 != 0)
        return 0;

    memset(x, 0, sizeof(int)*16);
    cards_bucket(hand, x);
    n = v = 0;
    for(i = 0; i < 16; ++i){
        if(x[i] == 0) continue;
        break;
    }
    for(j = i; j < 16; ++j){
        if(x[j] != 2)
            return 0;
        if(x[j] == 0)
            break;
        if(j > v)
            v = j;
    }
    for(k = j; k < 16; ++k){
        if(x[k] != 0)
            return 0;
    }

    return v;
}

int ht_three_straight(hand_t* hand)
{
    int i,j,k,n,v;
    int x[16];

    if(!hand || !hand->cards)
        return 0;

    if(!hand || !hand->cards)
        return 0;
    if(hand->num < 6)
        return 0;
    if(hand->num % 3 != 0)
        return 0;

    memset(x, 0, sizeof(int)*16);
    cards_bucket(hand, x);
    n = v = 0;
    for(i = 0; i < 16; ++i){
        if(x[i] == 0) continue;
        break;
    }
    for(j = i; j < 16; ++j){
        if(x[j] != 3)
            return 0;
        if(x[j] == 0)
            break;
        if(j > v)
            v = j;
    }
    for(k = j; k < 16; ++k){
        if(x[k] != 0)
            return 0;
    }

    return v;
}