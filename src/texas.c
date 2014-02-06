#include "texas.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sort_card.h"

#define DECK_FU     1
#define MAX_CARDS   5

typedef struct analyse_r_s{
    int n1;
    int v1[MAX_CARDS];
    int n2;
    int v2[MAX_CARDS];
    int n3;
    int v3[MAX_CARDS];
    int n4;
    int v4[MAX_CARDS];
}analyse_r;

texas_t* texas_new()
{
    int i;
    texas_t* texas;

    texas = (texas_t*)malloc(sizeof(texas_t));
    if(!texas)
        return 0;
    texas->b_burn = 1;
    texas->deck = deck_new(DECK_FU, 0);
    if(!texas->deck){
        texas_free(texas);
        return 0;
    }
    texas->debug = 0;
    texas->game_state = TEXAS_GAME_END;
    texas->inning = 0;
    texas->turn_time = 30;

    for(i = 0; i < TEXAS_MAX_PLAYER; i++){
        card_player_init(&(texas->players[i]), MAX_CARDS);
        texas->players[i].position = i;
    }

    return texas;
}

void texas_free(texas_t* texas)
{
    int i;
    if(!texas)
        return;
    if(texas->deck)
        deck_free(texas->deck);
    for(i = 0; i < TEXAS_MAX_PLAYER; i++){
        card_player_clear(&(texas->players[i]));
    }
    free(texas);
}

void texas_start(texas_t* texas)
{
    int i,j;
    card_t card;

    if(!texas)
        return;
    deck_shuffle(texas->deck);
    texas->round = 0;
    texas->game_state = TEXAS_GAME_PREFLOP;
    for(i = 0; i < TEXAS_MAX_PLAYER; ++i){
        card_player_reset(&(texas->players[i]));
    }

    /* draw two cards for every player */
    for(i = 0; i < 2; ++i){
        for(j = 0; j < texas->player_num; ++j){
            deck_deal(texas->deck, &card);
            card_player_draw(&(texas->players[j]), &card);
        }
    }

    /* draw five board cards */
    if(texas->b_burn){
        deck_deal(texas->deck, &card);
        for(i = 0; i < 3; ++i){
            deck_deal(texas->deck, &texas->board[i]);
        }
        deck_deal(texas->deck, &card);
        deck_deal(texas->deck, &texas->board[3]);
        deck_deal(texas->deck, &card);
        deck_deal(texas->deck, &texas->board[4]);
    }
    else{
        for(i = 0; i < 5; ++i){
            deck_deal(texas->deck, &texas->board[i]);
        }
    }

    /* the first player */
    if(!texas->inning)
        texas->dealer_player_no = rand() % texas->player_num;
    else{
        texas->dealer_player_no++;
        if(texas->dealer_player_no >= texas->player_num)
            texas->dealer_player_no = 0;
    }
    texas->inning++;
    texas->curr_player_no = texas->first_player_no; 
}

void texas_set_burn(texas_t* texas, int burn)
{
    if(!texas)
        return;

    texas->b_burn = burn;
}

int texas_get_state(texas_t* texas)
{
    if(texas)
        return texas->game_state;

    return 0;
}

void texas_set_state(texas_t* texas, int state)
{
    if(texas){
        texas->game_state = state;
    }
}

void texas_sort(hand_t* hand)
{
    cards_sort(hand);
}

void texas_analyse(hand_t* hand, analyse_r* ar)
{
    int x[16];
    int i;

    if(!hand || !ar)
        return;

    memset(x, 0, sizeof(int)*16);
    cards_bucket(hand, x);
    ar->n1 = ar->n2 = ar->n3 = ar->n4 = 0;
    for(i = 0; i < 16; i++){
        switch(x[i]){
        case 1:
            ar->v1[ar->n1] = i;
            ar->n1++;
            break;
        case 2:
            ar->v2[ar->n2] = i;
            ar->n2++;
            break;
        case 3:
            ar->v3[ar->n3] = i;
            ar->n3++;
            break;
        case 4:
            ar->v4[ar->n4] = i;
            ar->n4++;
            break;
        }
    }
}

void texas_handtype(hand_t* hand, hand_type* htype)
{
    int flag,i;
    int same_suit;
    int suit;
    analyse_r ar;
    card_t *p;

    if(!hand || !htype)
        return;
    if(!hand->num < 2)
        return;
    if(!hand->num > 5)
        return;

    htype->type = TEXAS_HIGHCARD;

    memset(&ar, 0, sizeof(analyse_r));
    texas_analyse(hand, &ar);

    /* four of kind */
    if(ar.n4){
        htype->type = TEXAS_FOUR;
        htype->logic_value = ar.v4[0];
        return;
    }

    /* three */
    if(ar.n3){
        if(ar.n2)
            htype->type = TEXAS_FULLHOUSE;
        else
            htype->type = TEXAS_THREE;
        htype->logic_value = ar.v3[0];
    }

    /* pair */
    if(ar.n2 == 1){
        htype->type = TEXAS_PAIR1;
        htype->logic_value = TEXAS_PAIR1;
    }
    else if(ar.n2 == 2){
        htype->type = TEXAS_PAIR2;
        htype->logic_value = TEXAS_PAIR2;
    }

    /* straight */
    same_suit = 1;
    if(ar.n1 == 5){
        flag = cards_have_rank(cdRank2, ar.v1, MAX_CARDS);
        if(flag)
            return;
        for(i = 0; i < (ar.n1 - 1); ++i){
            if((ar.v1[i+1] - ar.v1[i]) != 1)
                return;
        }
        p = hand->cards;
        suit = p->suit;
        for(i = 1; i < hand->num; ++i){
            p++;
            if(suit != p->suit){
                same_suit = 0;
                break;
            }
        }
        if(same_suit)
            htype->type = TEXAS_ROYAL;
        else
            htype->type = TEXAS_STRAIGHT;
        htype->logic_value = ar.v1[0];
        return;
    }

    return;
}

void texas_next_player(texas_t* texas)
{
    if(!texas)
        return;
    texas->curr_player_no++;
    if(texas->curr_player_no >= texas->player_num)
        texas->curr_player_no = 0;
}

int texas_count_notfoled(texas_t* texas)
{
    int i;
    int not_folded = 0;

    for(i = 0; i < texas->player_num; ++i){
        if(texas->player_state[i] != TPS_FOLD)
            not_folded++;
    }

    return not_folded;
}

void texas_get_folp(texas_t* texas, card_t* c1, card_t* c2, card_t* c3)
{
    if(!c1 || !c2 || !c3)
        return;

    c1->suit = texas->board[0].suit;
    c1->rank = texas->board[0].rank;
    c2->suit = texas->board[1].suit;
    c2->rank = texas->board[1].rank;
    c3->suit = texas->board[2].suit;
    c3->rank = texas->board[2].rank;
}

void texas_get_turn(texas_t* texas, card_t* card)
{
    if(!card)
        return;

    card->suit = texas->board[3].suit;
    card->rank = texas->board[3].rank;
}

void texas_get_river(texas_t* texas, card_t* card)
{
    if(!card)
        return;

    card->suit = texas->board[4].suit;
    card->rank = texas->board[4].rank;
}

int texas_fold(texas_t* texas, int player_no)
{
    if(!texas)
        return 0;
    if(player_no >= texas->player_num)
        return 0;
    if(player_no != texas->curr_player_no)
        return 0;

    texas->player_state[player_no] = TPS_FOLD;
    
    /* check game over */
    return 1;
}
