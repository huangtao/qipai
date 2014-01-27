#include "ddz.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sort_card.h"

#define DECK_FU     1
#define MAX_CARDS   20

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

ddz_t* ddz_new()
{
    int i;
    ddz_t* ddz;

    ddz = (ddz_t*)malloc(sizeof(ddz_t));
    if(!ddz)
        return 0;
    ddz->deck = deck_new(DECK_FU, 1);
    if(!ddz->deck){
        ddz_free(ddz);
        return 0;
    }
    ddz->last_hand = hand_new(MAX_CARDS);
    if(!ddz->last_hand){
        ddz_free(ddz);
        return 0;
    }
    ddz->debug = 0;
    ddz->game_state = DDZ_GAME_END;
    ddz->inning = 0;
    ddz->turn_time = 30;

    for(i = 0; i < DDZ_MAX_PLAYER; i++){
        card_player_init(&(ddz->players[i]), MAX_CARDS);
        ddz->players[i].position = i;
        ddz->call[i] = 0;
    }

    return ddz;
}

void ddz_free(ddz_t* ddz)
{
    int i;
    if(!ddz)
        return;
    if(ddz->last_hand)
        hand_free(ddz->last_hand);
    if(ddz->deck)
        deck_free(ddz->deck);
    for(i = 0; i < DDZ_MAX_PLAYER; i++){
        card_player_clear(&(ddz->players[i]));
    }
    free(ddz);
}

void ddz_start(ddz_t* ddz)
{
    int i;
    card_t card;

    if(!ddz)
        return;
    deck_shuffle(ddz->deck);
    ddz->round = 0;
    ddz->landlord_no = 0;
    ddz->curr_player_no = 0;
    ddz->game_state = DDZ_GAME_CALL;
    for(i = 0; i < DDZ_MAX_PLAYER; ++i){
        ddz->call[i] = 0;
        card_player_reset(&(ddz->players[i]));
        ddz->di[i].rank = 0;
        ddz->di[i].suit = 0;
    }
    hand_zero(ddz->last_hand);
    ddz->last_htype.type = 0;

    /* draw 17 cards for every player */
    for(i = 0; i < 17; ++i){
        deck_deal(ddz->deck, &card);
        card_player_draw(&(ddz->players[0]), &card);

        deck_deal(ddz->deck, &card);
        card_player_draw(&(ddz->players[1]), &card);

        deck_deal(ddz->deck, &card);
        card_player_draw(&(ddz->players[2]), &card);
    }

    /* draw 3 cards for di */
    for(i = 0; i < 3; ++i){
        deck_deal(ddz->deck, &card);
        ddz->di[i].rank = card.rank;
        ddz->di[i].suit = card.suit;
    }

    ddz->game_state = DDZ_GAME_CALL;
    ddz->inning++;
    /* the first inning call random */
	if(ddz->inning == 1)
        ddz->first_player_no = rand() % DDZ_MAX_PLAYER;
	else{
        ddz->first_player_no = ddz->landlord_no + 1;
        if(ddz->first_player_no >= DDZ_MAX_PLAYER)
            ddz->first_player_no = 0;
    }
    ddz->curr_player_no = ddz->first_player_no; 
}

int ddz_get_state(ddz_t* ddz)
{
    if(ddz)
        return ddz->game_state;

    return 0;
}

void ddz_set_state(ddz_t* ddz, int state)
{
    if(ddz){
        ddz->game_state = state;
    }
}

void ddz_sort(hand_t* coll)
{
    cards_sort(coll);
}

void ddz_analyse(hand_t* coll, analyse_r* ar)
{
    int x[16];
    int i;

    if(!coll || !ar)
        return;

    memset(x, 0, sizeof(int)*16);
    cards_bucket(coll, x);
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

void ddz_cardtype(hand_t* coll, hand_type* cdtype)
{
    int flag,i;
    analyse_r ar;
    card_t *p;

    if(!coll || !cdtype)
        return;
    cdtype->type = DDZ_ERROR;
    p = coll->cards;
    switch(coll->num){
    case 0:
        return;
    case 1:
        cdtype->type = DDZ_SINGLE;
        cdtype->logic_value = card_logicvalue(p);
        return;
    case 2:
        if(p->rank == (p + 1)->rank){
            cdtype->type = DDZ_DOUBLE;
            cdtype->logic_value = card_logicvalue(p);
            return;
        }
        if(p->suit == cdSuitJoker && p->suit == (p + 1)->suit){
            cdtype->type = DDZ_ATOM;
            cdtype->logic_value = card_logicvalue(p);
            return;
        }
        return;
    }

    memset(&ar, 0, sizeof(analyse_r));
    ddz_analyse(coll, &ar);

    /* for bomb */
    if(ar.n4 > 0){
        if(ar.n4 == 1 && coll->num == 4){
            cdtype->type = DDZ_BOMB;
            cdtype->logic_value = ar.v4[0];
            return;
        }
        if(ar.n4 == 1 && coll->num == 6){
            cdtype->type = DDZ_FOUR_2S;
            cdtype->logic_value = ar.v4[0];
            return;
        }
        if(ar.n4 == 1 && ar.n2 == 2 && coll->num == 8){
            cdtype->type = DDZ_FOUR_2D;
            cdtype->logic_value = ar.v4[0];
            return;
        }
        return;
    }

    /* for three */
    if(ar.n3 > 0){
        if(ar.n3 == 1 && coll->num == 3){
            cdtype->type = DDZ_THREE;
            cdtype->logic_value = ar.v3[0];
            return;
        }
        if(ar.n3 > 1){
            /* not include rank 2 */
            flag = cards_have_rank(cdRank2, ar.v3, MAX_CARDS);
            if(flag)
                return;
            for(i = 0; i < (ar.n3 - 1); ++i){
                if((ar.v3[i+1] - ar.v3[i]) != 1)
                    return;
            }
            if(ar.n3 * 3 == coll->num){
                cdtype->type = DDZ_T_STRAIGHT;
                cdtype->logic_value = ar.v3[0];
                return;
            }
            if(ar.n3 * 4 == coll->num){
                cdtype->type = DDZ_THREE_P1;
                cdtype->logic_value = ar.v3[0];
                return;
            }
            if(ar.n3 * 5 == coll->num){
                cdtype->type = DDZ_THREE_P2;
                cdtype->logic_value = ar.v3[0];
                return;
            }
        }

        return;
    }

    /* for 2 */
    if(ar.n2 >= 3){
        flag = cards_have_rank(cdRank2, ar.v2, MAX_CARDS);
        if(flag)
            return;
        for(i = 0; i < (ar.n2 - 1); ++i){
            if((ar.v2[i+1] - ar.v2[i]) != 1)
                return;
        }
        if(ar.n2 * 2 == coll->num){
            cdtype->type = DDZ_D_STRAIGHT;
            cdtype->logic_value = ar.v2[0];
            return;
        }

        return;
    }

    /* for straight */
    if(ar.n1 >= 5 && ar.n1 == coll->num){
        flag = cards_have_rank(cdRank2, ar.v1, MAX_CARDS);
        if(flag)
            return;
        flag = cards_have_rank(cdRankZJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;
        flag = cards_have_rank(cdRankFJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;
        for(i = 0; i < (ar.n1 - 1); ++i){
            if((ar.v1[i+1] - ar.v1[i]) != 1)
                return;
        }
        cdtype->type = DDZ_STRAIGHT;
        cdtype->logic_value = ar.v1[0];
        return;
    }

    return;
}

int ddz_playcards(ddz_t* ddz, int player_no, hand_t* coll)
{
    int i;
    hand_type htype;
    card_t* card;
    card_t* plast;

    if(!coll)
        return 0;

    if(ddz->game_state != DDZ_GAME_PLAY){
        if(ddz->debug)
            printf("play cards but game state not play.\n");
        return 0;
    }
    if(player_no != ddz->curr_player_no){
        if(ddz->debug)
            printf("play cards but not this no.\n");
        return 0;
    }

    if(coll->num == 0){
        if(ddz->debug)
            printf("play zero cards.\n");
        return 0;
    }

    for(i = 0; i < coll->num; ++i){
        card = coll->cards + i;
        if(!card_player_have(&ddz->players[player_no], card)){
            if(ddz->debug){
                printf("play cards but player hasn't this card(%s).\n",
                    card_text(card));
            }
            return 0;
        }
    }

    ddz_cardtype(coll, &htype);

    /* can play out these cards */
    if(!ddz_canplay(ddz, coll, &htype)){
        if(ddz->debug)
            printf("cann't play these cards(smaller).\n");
        return 0;
    }

    /* player play these cards */
    plast = ddz->last_hand->cards;
    ddz->last_hand->num = 0;
    for(i = 0; i < coll->num; ++i){
        card = coll->cards + i;
        card_player_play(&ddz->players[player_no], card);
        plast->rank = card->rank;
        plast->suit = card->suit;
        plast++;
        ddz->last_hand->num++;
    }
    ddz->last_htype.type = htype.type;
    ddz->last_htype.logic_value = htype.logic_value;
    ddz->last_htype.num = htype.num;
    ddz->largest_player_no = player_no;

    ddz_next_player(ddz);

    return 1;
}

int ddz_canplay(ddz_t* ddz, hand_t* coll, hand_type* htype)
{
    if(!ddz || !coll || !htype)
        return 0;

    if(ddz->last_hand->num == 0)
        return 1;
    if(htype->type == DDZ_ERROR)
        return 0;
    if(htype->type == DDZ_ATOM)
        return 1;

    if(ddz->last_htype.type == DDZ_BOMB && htype->type != DDZ_BOMB)
        return 1;
    if(ddz->last_htype.type != DDZ_BOMB && htype->type == DDZ_BOMB)
        return 0;

    if(ddz->last_htype.type != htype->type ||
        ddz->last_hand->num != coll->num)
        return 0;

    if(htype->logic_value > ddz->last_htype.logic_value)
        return 1;

    return 0;
}

void ddz_next_player(ddz_t* ddz)
{
    if(!ddz)
        return;
    ddz->curr_player_no++;
    if(ddz->curr_player_no >= DDZ_MAX_PLAYER)
        ddz->curr_player_no = 0;
}

/*
* 0 - error
* 1 - restart game
* 2 - keep call
* 3 - enter play state
*/
int ddz_call(ddz_t* ddz, int call_no, int fen)
{
    int i,ret;
    card_t card;

    ret = 0;
    if(fen > 3 || fen < 0){
        if(ddz->debug)
            printf("invalid call fen:%d", fen);
        return ret;
    }
    if(call_no != ddz->curr_player_no){
        if(ddz->debug)
            printf("invalid player no.");
        return ret;
    }

    ddz->call[call_no] = fen;
    if(fen == 3){
        ddz->landlord_no = ddz->curr_player_no;
        /* deal three di */
        for(i = 0; i < 3; ++i){
            deck_deal(ddz->deck, &card);
            card_player_draw(&ddz->players[call_no], &card);
            ddz->di[i].rank = card.rank;
            ddz->di[i].suit = card.suit;
        }
        ddz->game_state = DDZ_GAME_PLAY;
        ddz->curr_player_no = ddz->landlord_no;
        ret = 3;
    }
    else{
        ddz_next_player(ddz);
        if(ddz->curr_player_no == ddz->first_player_no && fen == 0){
            /* nobody call,restart game */
            ddz->inning = 0;
            ddz_start(ddz);
            ret = 1;
        }
        else
            ret = 2;
    }
    ddz->curr_turn_time = ddz->turn_time;

    return ret;
}

int ddz_pass(ddz_t* ddz, int player_no)
{
    if(!ddz)
        return 0;
    if(player_no != ddz->curr_player_no)
        return 0;
    if(ddz->game_state == DDZ_GAME_END)
        return 0;

    ddz_next_player(ddz);

    return 1;
}
