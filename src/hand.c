#include "hand.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ht_lch.h"
#include "ht_str.h"

static char c_suit[] = {'0','D','C','H','S','*','?'};
static char c_rank[] = {'0','A','2','3','4','5','6','7','8','9','T','J','Q','K','S','B','?'};

hand_t* hand_new(int max_size)
{
    int byte_size;
    hand_t* p;
    card_t* cd;

    if(max_size <= 0)
        return 0;

    p = (hand_t*)malloc(sizeof(hand_t));
    p->num = 0;
    p->max_size = 0;
    byte_size = sizeof(card_t) * max_size;
    cd = (card_t*)malloc(byte_size);
    if(cd){
        memset(cd, 0, byte_size);
        p->max_size = max_size;
    }
    p->cards = cd;

    return p;
}

void hand_free(hand_t* hand)
{
    if(hand){
        if(hand->cards){
            free(hand->cards);
            hand->cards = 0;
        }
        hand->num = 0;
        free(hand);
        hand = 0;
    }
}

void hand_zero(hand_t* hand)
{
    int i;
    card_t *p;

    if(!hand)
        return;

    p = hand->cards;
    for(i = 0; i < hand->max_size; ++i){
        p->rank = 0;
        p->suit = 0;
        p++;
    }
    hand->num = 0;
    hand->type = 0;
    hand->type_card.rank = 0;
    hand->type_card.suit = 0;
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

void hand_copy(hand_t* src, hand_t* dest)
{
    int i,num;
    card_t *p1,*p2;

    if(!src || !dest)
        return;

    num = src->num;
    if(num > dest->max_size)
        num = dest->max_size;
    dest->num = num;
    p1 = src->cards;
    p2 = dest->cards;
    for(i = 0; i < num; i++)
    {
        p2->rank = p1->rank;
        p2->suit = p1->suit;
        p1++;
        p2++;
    }
    dest->type = src->type;
    dest->type_card.rank = src->type_card.rank;
    dest->type_card.suit = src->type_card.suit;
}

hand_t* hand_clone(hand_t* hand)
{
    hand_t* p;
    int byte_size;

    if(!hand)
        return 0;

    byte_size = hand->max_size * sizeof(card_t);
    p = (hand_t*)malloc(sizeof(hand_t));
    if(p){
        p->cards = (card_t*)malloc(byte_size);
        p->max_size = hand->max_size;
        p->num = hand->num;
        p->type = hand->type;
        p->type_card.rank = hand->type_card.rank;
        p->type_card.suit = hand->type_card.suit;
        memcpy(p->cards, hand->cards, byte_size);
    }

    return p;
}

/* note:string's format must be "DA,D2,..." */
void hand_from_string(hand_t* hand, char* string)
{
    int i,j,len,num;
    int realnum;
    char **v;
    char *p;
    card_t card;
    
    if(!hand || !string)
        return;
    
    len = strlen(string);
    if(len < 2 || (len + 1) % 3 != 0)
        return;
    num = (len + 1) / 3;
    v = (char**)malloc(sizeof(char*) * num);
    memset(v, 0, sizeof(char*) * num);
    
    realnum = ht_string_split(string, v, num, ",");
    if(realnum > 0)
        hand_zero(hand);
    for(i = 0; i < realnum; i++){
        p = v[i];
        card.suit = card.rank = 0;
        for(j = 0; j < 7; j++){
            if(*p == c_suit[j]){
                card.suit = j;
                break;
            }
        }
        for(j = 0; j < 17; j++){
            if(*(p+1) == c_rank[j]){
                card.rank = j;
                break;
            }
        }
        hand_push(hand, &card);
    }
    
    free(v);
}

int hand_have(hand_t* hand, card_t* card)
{
    int i;
    card_t* p;
    
    if(!hand || !card)
        return 0;
    
    p = hand->cards;
    for(i = 0; i < hand->num; i++,p++){
        if(p->suit != card->suit || p->rank != card->rank)
            continue;
        return 1;
    }
    
    return 0;
}

int hand_rank_num(hand_t* hand, int rank)
{
    int i,n;
    card_t* p;
    
    if(!hand)
        return 0;
    
    n = 0;
    p = hand->cards;
    for(i = 0; i < hand->num; i++,p++){
        if(p->rank == rank)
            n++;
    }
    
    return n;
}

int hand_suit_num(hand_t* hand, int suit)
{
    int i,n;
    card_t* p;
    
    if(!hand)
        return 0;
    
    n = 0;
    p = hand->cards;
    for(i = 0; i < hand->num; i++,p++){
        if(p->suit == suit)
            n++;
    }
    
    return n;
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

    return hand->num;
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
    p->suit = 0;
    p->rank = 0;
    hand->num--;

    return hand->num;
}

int hand_add(card_t* cards, int len, card_t* card)
{
    int i;
    card_t* p;

    if(!cards || len <= 0 || !card)
        return HTERR_PARAM;

    p = cards;
    for (i = 0; i < len; ++i) {
        if (cards->rank == 0 && cards->suit == 0) {
            cards->rank = card->rank;
            cards->suit = cards->suit;
        }
        cards++;
    }

    return HT_OK;
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

void hand_dump(hand_t* hand, int line_number)
{
    int i;
    card_t* p;

    if(!hand || !hand->cards){
        printf("hand or cards is invalid pointer!\n");
        return;
    }

    for(i = 0; i < hand->max_size; ++i){
        p = hand->cards + i;
        printf("%c%c ", c_suit[p->suit], c_rank[p->rank]);
        if((i+1) % line_number == 0){
            printf("\n");
        }
    }
}

const char* card_text(card_t* card)
{
    static char readable[8];
    
    memset(readable, 0, 8);
    if(!card)
        return 0;

    readable[2] = 0;
    sprintf(readable, "%c%c", c_suit[card->suit], c_rank[card->rank]);

    return readable;
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
