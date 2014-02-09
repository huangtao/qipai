#include "ddz.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sort_card.h"
#include "ht_lch.h"
#include "ht_str.h"

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
    ddz->round = 0;
    ddz->turn_time = 30;
    ddz->curr_turn_time = 0;

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
    int i,r;
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
        r = deck_deal(ddz->deck, &card);
        if(!r)
            card_player_draw(&(ddz->players[0]), &card);

        r = deck_deal(ddz->deck, &card);
        if(!r)
            card_player_draw(&(ddz->players[1]), &card);

        r = deck_deal(ddz->deck, &card);
        if(!r)
            card_player_draw(&(ddz->players[2]), &card);
    }

    /* draw 3 cards for di */
    for(i = 0; i < 3; ++i){
        r = deck_deal(ddz->deck, &card);
        if(!r){
            ddz->di[i].rank = card.rank;
            ddz->di[i].suit = card.suit;
        }
    }

    ddz->game_state = DDZ_GAME_CALL;
    ddz->inning++;
    ddz->landlord_win = 0;
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

const char* ddz_htype_name(hand_type* htype)
{
    static char* htype_name[] = {
        "DDZ_ERROR",
        "DDZ_SINGLE",
        "DDZ_DOUBLE",
        "DDZ_THREE",
        "DDZ_STRAIGHT",
        "DDZ_D_STRAIGHT",
        "DDZ_T_STRAIGHT",
        "DDZ_THREE_P1",
        "DDZ_THREE_P2",
        "DDZ_FOUR_2S",
        "DDZ_FOUR_2D",
        "DDZ_BOMB",
        "DDZ_ATOM"
        };
    
    if(htype){
        if(htype->type <= 12)
            return htype_name[htype->type];
    }
    
    return htype_name[0];
}

void ddz_sort(hand_t* hand)
{
    cards_sort(hand);
}

void ddz_analyse(hand_t* hand, analyse_r* ar)
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

void ddz_handtype(hand_t* hand, hand_type* htype)
{
    int flag,i;
    analyse_r ar;
    card_t *p;

    if(!hand || !htype)
        return;
    htype->type = DDZ_ERROR;
    htype->logic_value = 0;
    p = hand->cards;
    switch(hand->num){
    case 0:
        return;
    case 1:
        htype->type = DDZ_SINGLE;
        htype->logic_value = card_logicvalue(p);
        return;
    case 2:
        if(p->rank == (p + 1)->rank){
            htype->type = DDZ_DOUBLE;
            htype->logic_value = card_logicvalue(p);
            return;
        }
        if(p->suit == cdSuitJoker && p->suit == (p + 1)->suit){
            htype->type = DDZ_ATOM;
            htype->logic_value = card_logicvalue(p);
            return;
        }
        return;
    }

    memset(&ar, 0, sizeof(analyse_r));
    ddz_analyse(hand, &ar);

    /* for bomb */
    if(ar.n4 > 0){
        if(ar.n4 == 1 && hand->num == 4){
            htype->type = DDZ_BOMB;
            htype->logic_value = ar.v4[0];
            return;
        }
        if(ar.n4 == 1 && hand->num == 6){
            htype->type = DDZ_FOUR_2S;
            htype->logic_value = ar.v4[0];
            return;
        }
        if(ar.n4 == 1 && ar.n2 == 2 && hand->num == 8){
            htype->type = DDZ_FOUR_2D;
            htype->logic_value = ar.v4[0];
            return;
        }
        return;
    }

    /* for three */
    if(ar.n3 > 0){
        if(ar.n3 == 1){
            if(hand->num == 3){
                htype->type = DDZ_THREE;
                htype->logic_value = ar.v3[0];
            }
            else if(hand->num == 4){
                htype->type = DDZ_THREE_P1;
                htype->logic_value = ar.v3[0];
            }
            else if(hand->num == 5){
                if(ar.n2 == 1){
                    htype->type = DDZ_THREE_P2;
                    htype->logic_value = ar.v3[0];
                }
            }
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
            if(ar.n3 * 3 == hand->num){
                htype->type = DDZ_T_STRAIGHT;
                htype->logic_value = ar.v3[0];
                return;
            }
            if(ar.n3 * 4 == hand->num){
                htype->type = DDZ_THREE_P1;
                htype->logic_value = ar.v3[0];
                return;
            }
            if(ar.n3 * 5 == hand->num){
                htype->type = DDZ_THREE_P2;
                htype->logic_value = ar.v3[0];
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
        if(ar.n2 * 2 == hand->num){
            htype->type = DDZ_D_STRAIGHT;
            htype->logic_value = ar.v2[0];
            return;
        }

        return;
    }

    /* for straight */
    if(ar.n1 >= 5 && ar.n1 == hand->num){
        flag = cards_have_rank(cdRank2, ar.v1, MAX_CARDS);
        if(flag)
            return;
        flag = cards_have_rank(cdRankBJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;
        flag = cards_have_rank(cdRankSJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;
        for(i = 0; i < (ar.n1 - 1); ++i){
            if((ar.v1[i+1] - ar.v1[i]) != 1)
                return;
        }
        htype->type = DDZ_STRAIGHT;
        htype->logic_value = ar.v1[ar.n1-1];
        return;
    }

    return;
}

int ddz_play(ddz_t* ddz, int player_no, hand_t* hand)
{
    int i;
    hand_type htype;
    card_t* card;
    card_t* plast;

    if(!hand)
        return HTERR_PARAM;

    if(ddz->game_state != DDZ_GAME_PLAY){
        if(ddz->debug)
            printf("play cards but game state not play.\n");
        return HTERR_STATE;
    }
    if(player_no != ddz->curr_player_no){
        if(ddz->debug)
            printf("play cards but not this no.\n");
        return -1001;
    }

    if(hand->num == 0){
        if(ddz->debug)
            printf("play zero cards.\n");
        return HTERR_PARAM;
    }

    for(i = 0; i < hand->num; ++i){
        card = hand->cards + i;
        if(!card_player_have(&ddz->players[player_no], card)){
            if(ddz->debug){
                printf("play cards but player hasn't this card(%s).\n",
                    card_text(card));
            }
            return HTERR_NOCARD;
        }
    }

    ddz_handtype(hand, &htype);

    /* can play out these cards */
    if(ddz->largest_player_no != player_no){
        if(!ddz_canplay(ddz, hand, &htype)){
            if(ddz->debug)
                printf("cann't play these cards(smaller).\n");
            return -1002;
        }
    }

    /* player play these cards */
    plast = ddz->last_hand->cards;
    ddz->last_hand->num = 0;
    for(i = 0; i < hand->num; ++i){
        card = hand->cards + i;
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
    
    hand_trim(ddz->players[player_no].mycards);
    if(hand_num(ddz->players[player_no].mycards))
        ddz_next_player(ddz);
    else{
        ddz->game_state = DDZ_GAME_END;
        if(player_no == ddz->landlord_no)
            ddz->landlord_win = 1;
        else
            ddz->landlord_win = 0;
    }

    return 1;
}

int ddz_canplay(ddz_t* ddz, hand_t* hand, hand_type* htype)
{
    if(!ddz || !hand || !htype)
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
        ddz->last_hand->num != hand->num)
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
            card_player_draw(&ddz->players[ddz->landlord_no], &ddz->di[i]);
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

void ddz_dump(ddz_t* ddz)
{
    if(!ddz)
        return;
    
    /* dump di */
    printf("di:%s ", card_text(&ddz->di[0]));
    printf("%s ", card_text(&ddz->di[1]));
    printf("%s \n", card_text(&ddz->di[2]));
    
    printf("call:%d,%d,%d\n", ddz->call[0], ddz->call[1], ddz->call[2]);
        
    /* dump player's cards */
    hand_dump(ddz->players[0].mycards, 10);
    printf("\n");
    hand_dump(ddz->players[1].mycards, 10);
    printf("\n");
    hand_dump(ddz->players[2].mycards, 10);
    printf("\n");
    
    hand_dump(ddz->last_hand, 10);
    
    printf("current player no is %d\n", ddz->curr_player_no);
}
