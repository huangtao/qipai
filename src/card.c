#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ht_lch.h"

static char c_suit[] = {'?','D','C','H','S','*'};
static char c_rank[] = {'?','A','2','3','4','5','6','7','8','9','T','J','Q','K','F','Z'};

void card_init(card_t* card, const char* sn)
{
}

int card_equal(card_t* a, card_t* b)
{
    if(!a || !b)
        return 0;

    if(a->suit == b->suit &&
        a->rank == b->rank)
    {
        return 1;
    }

    return 0;
}

char card_encode(card_t* cd)
{
    char x;

    if(cd)
        x = (cd->suit << 4) | cd->rank;
    else
        x = 0;
    
    return x;
}

void card_decode(card_t* cd, char x)
{
    if(cd){
        cd->suit = x >> 4;
        cd->rank = x & 0xf;
    }
}

const char* cards_print(card_t cards[], int len, int line_number)
{
    int i;
    char buf[256];
    static char readable[256];

    if (!cards || len > 128) {
        sprintf(buf, "");
        return readable;
    }

    memset(readable, 0, 256 * sizeof(char));
    for(i = 0; i < len; ++i){
        if(cards[i].suit > 5 || cards[i].rank > 15)
            sprintf(buf, "%c%c ", '-', '-');
        else
            sprintf(buf, "%c%c ", c_suit[cards[i].suit], c_rank[cards[i].rank]);
        if((i+1) % line_number == 0){
            strcat(buf, "\n");
        }
        strcat(readable, buf);
    }

    return readable;
}

/*
card_coll* card_coll_new(int max_size)
{
    int byte_size;
    card_coll* p;

    if(max_size <= 0)
        return 0;

    byte_size = sizeof(card_coll) + sizeof(card_t) * max_size;
    p = (card_coll*)malloc(byte_size);
    if(p){
        memset(p, 0, byte_size);
        p->max_size = max_size;
    }

    return p;
}

void card_coll_free(card_coll* coll)
{
    free(coll);
}

void card_coll_zero(card_coll* coll)
{
    int i;
    card_t *p;

    if(!coll || !coll->cards)
        return;

    p = coll->cards;
    for(i = 0; i < coll->max_size; ++i){
        p->rank = 0;
        p->suit = 0;
        p++;
    }
    coll->num = 0;
}

int card_coll_num(card_coll* coll)
{
    int i,n;
    card_t* p;

    n = 0;
    if(coll){
        for(i = 0; i < coll->max_size; ++i){
            p = coll->cards + i;
            if(p->rank || p->suit)
                n++;
        }
    }

    return n;
}

card_coll* card_coll_clone(card_coll* coll)
{
    card_coll* p;
    int byte_size;

    if(!coll)
        return 0;

    byte_size = sizeof(card_coll) + coll->max_size * sizeof(card_t);
    p = (card_coll*)malloc(byte_size);
    if(p){
        memcpy(p, coll, byte_size);
    }

    return p;
}

card_t* card_coll_get(card_coll* coll, int n)
{
    card_t* p;

    if(!coll)
        return 0;

    if(n >= coll->max_size)
        return 0;

    p = coll->cards + n;

    return p;
}

int card_coll_push(card_coll* coll, card_t* card)
{
    card_t* p;

    if(!coll || !card)
        return HTERR_PARAM;

    if(coll->num == coll->max_size)
        return HTERR_OUTOFRANGE;

    p = coll->cards + coll->num;
    p->rank = card->rank;
    p->suit = card->suit;
    coll->num++;

    return 0;
}

int card_coll_pop(card_coll* coll, card_t* card)
{
    card_t* p;

    if(!coll || !card)
        return HTERR_PARAM;

    if(coll->num < 1)
        return HTERR_OUTOFRANGE;

    p = coll->cards + (coll->num - 1);
    card->suit = p->suit;
    card->rank = p->rank;
    coll->num--;

    return 0;
}

int card_coll_del(card_coll* coll, card_t* card)
{
    int i;
    card_t* p;

    if(!coll || !card)
        return HTERR_PARAM;

    for(i = 0; i < coll->num; i++){
        p = coll->cards + i;
        if(p->suit == card->suit && p->rank == card->rank){
            p->suit = 0;
            p->rank = 0;
        }
    }

    return 0;
}

int card_coll_trim(card_coll* coll)
{
    int i,j,num;
    card_t *p1,*p2;

    if(!coll)
        return HTERR_PARAM;
    if(coll->num < 1)
        return 0;

    num = 0;
    for(i = 0; i < coll->num; ++i){
        p1 = coll->cards + i;
        if(p1->suit == 0 && p1->rank == 0){
            for(j = i + 1; j < coll->num; ++j){
                p2 = coll->cards + j;
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

void card_cool_print(card_coll* coll, int line_number)
{
    int i;
    card_t* p;

    if(!coll || !coll->cards)
        return;

    for(i = 0; i < coll->num; ++i){
        p = coll->cards + i;
        printf("%c%c ", c_suit[p->suit], c_rank[p->rank]);
        if((i+1) % line_number == 0){
            printf("\n");
        }
    }
    if(coll->num % line_number != 0)
		printf("\n");
}

const char* card_text(card_t* card)
{
    static char readable[3];
    if(!card)
        return 0;

    readable[2] = 0;
    sprintf(readable, "%c%c", c_suit[card->suit], c_rank[card->rank]);

    return readable;
}*/
