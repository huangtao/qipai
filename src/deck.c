#include "deck.h"
#include "ht_lch.h"
#include <stdlib.h>

deck_t* deck_new(int number, int have_joker)
{
    deck_t* deck;
    card_t* p;
    int n,i,j,k;
    int idx;

    if(have_joker)
        n = 54;
    else
        n = 52;

    deck = (deck_t*)malloc(sizeof(deck_t));
    if(!deck)
        return 0;

    deck->poker = hand_new(number * n);
    if(!deck->poker){
        deck_free(deck);
        return 0;
    }
    deck->num_pack = number;
    deck->card_adv_id = 0;
    deck->deal_index = 0;
    deck->have_joker = have_joker;

    idx = 0;
    p = deck->poker->cards;
    for(i = 0; i < number; ++i){
        for(j = cdSuitDiamond; j <= cdSuitSpade; ++j){
            for(k = cdRankAce; k <= cdRankK; ++k){
                p->rank = k;
                p->suit = j;
                p++;
            }
        }
        if(have_joker){
            p->suit = cdSuitJoker;
            p->rank = cdRankSJoker;
            p++;
            p->suit = cdSuitJoker;
            p->rank = cdRankBJoker;
            p++;
        }
    }
    deck->poker->num = number * n;

    deck_shuffle(deck);

    return deck;
}

void deck_free(deck_t* deck)
{
    if(!deck){
        if(!deck->poker)
            hand_free(deck->poker);
        free(deck);
    }
}

void deck_shuffle(deck_t* deck)
{
    int i,n;
    int a,b;
    card_t temp;
    card_t *pa, *pb;

    if(!deck || !deck->poker)
        return;

    deck->deal_index = 0;
    n = 1000 + rand() % 50;
    for(i = 0; i < n; ++i){
        a = rand() % deck->poker->num;
        b = rand() % deck->poker->num;
        if(a != b){
            pa = deck->poker->cards + a;
            pb = deck->poker->cards + b;
            temp.rank = pa->rank;
            temp.suit = pa->suit;
            pa->rank = pb->rank;
            pa->suit = pb->suit;
            pb->rank = temp.rank;
            pb->suit = temp.suit;
        }
    }
}

int deck_deal(deck_t* deck, card_t* card)
{
    int i;
    card_t* p;

    if(!deck || !deck->poker || !card)
        return HTERR_PARAM;

    if(deck->deal_index >= deck->poker->num)
        return HTERR_NOCARD;

    for(i = deck->deal_index; i < deck->poker->num; ++i){
        p = deck->poker->cards + deck->deal_index;
        if(p->rank || p->suit){
            card->rank = p->rank;
            card->suit = p->suit;
            deck->deal_index++;
            return 0;
        }
        deck->deal_index++;
    }

    return HTERR_NOCARD;
}

int deck_get(deck_t* deck, int index, card_t* card)
{
    card_t* p;

    if(!deck || !deck->poker || !card)
        return HTERR_PARAM;

    if(index >= deck->poker->num)
        return HTERR_OUTOFRANGE;

    p = deck->poker->cards + index;
    card->rank = p->rank;
    card->suit = p->suit;

    return 0;
}

int deck_num(deck_t* deck)
{
    if(!deck || !deck->poker)
        return HTERR_PARAM;

    return deck->poker->num;
}

void deck_remove(deck_t* deck, card_t* card)
{
    if(!deck || !deck->poker || !card)
        return;

    hand_del(deck->poker, card);
}

void deck_remove_rank(deck_t* deck, int rank)
{
    int i;
    card_t* p;

    if(!deck || !deck->poker)
        return;
    for(i = 0; i < deck->poker->num; ++i){
        p = deck->poker->cards + i;
        if(p->rank == rank){
            p->rank = 0;
            p->suit = 0;
        }
    }
}

void deck_remove_suit(deck_t* deck, int suit)
{
    int i;
    card_t* p;

    if(!deck || !deck->poker)
        return;
    for(i = 0; i < deck->poker->num; ++i){
        p = deck->poker->cards + i;
        if(p->suit == suit){
            p->rank = 0;
            p->suit = 0;
        }
    }
}
