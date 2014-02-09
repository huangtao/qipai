#include "gp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ht_lch.h"
#include "sort_card.h"

/* this is zhuji's rule */

#define DECK_FU     1
#define MAX_CARDS   15

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

gp_t* gp_new()
{
    int i;
    card_t card;
    gp_t* gp;

    gp = (gp_t*)malloc(sizeof(gp_t));
    if(!gp)
        return 0;
    gp->deck = deck_new(DECK_FU, 0);
    if(!gp->deck){
        gp_free(gp);
        return 0;
    }
    /* del three 2 and three A and one K */
    card.suit = cdSuitDiamond;
    card.rank = cdRank2;
    deck_remove(gp->deck, &card);
    card.rank = cdRankAce;
    deck_remove(gp->deck, &card);
    card.rank = cdRankK;
    deck_remove(gp->deck, &card);

    card.suit = cdSuitClub;
    card.rank = cdRank2;
    deck_remove(gp->deck, &card);
    card.rank = cdRankAce;
    deck_remove(gp->deck, &card);

    card.suit = cdSuitHeart;
    card.rank = cdRank2;
    deck_remove(gp->deck, &card);
    card.rank = cdRankAce;
    deck_remove(gp->deck, &card);

    gp->last_hand = hand_new(MAX_CARDS);
    if(!gp->last_hand){
        gp_free(gp);
        return 0;
    }
    gp->debug = 0;
    gp->game_state = GP_GAME_END;
    gp->inning = 0;
    gp->turn_time = 30;
    gp->player_num = 3;

    for(i = 0; i < GP_MAX_PLAYER; i++){
        card_player_init(&(gp->players[i]), MAX_CARDS);
        gp->players[i].position = i;
    }

    return gp;
}

void gp_free(gp_t* gp)
{
    int i;
    if(!gp)
        return;
    if(gp->last_hand)
        hand_free(gp->last_hand);
    if(gp->deck)
        deck_free(gp->deck);
    for(i = 0; i < GP_MAX_PLAYER; i++){
        card_player_clear(&(gp->players[i]));
    }
    free(gp);
}

void gp_start(gp_t* gp)
{
    int i;
    card_t card;
    card_t d3;
    int d3_no;

    if(!gp)
        return;
    deck_shuffle(gp->deck);
    gp->round = 0;
    gp->game_state = GP_GAME_PLAY;
    for(i = 0; i < GP_MAX_PLAYER; ++i){
        card_player_reset(&(gp->players[i]));
    }
    hand_zero(gp->last_hand);
    gp->last_htype.type = 0;

    /* draw 15 cards for every player */
    d3.suit = cdSuitDiamond;
    d3.rank = cdRankAce;
    d3_no = -1;
    for(i = 0; i < 15; ++i){
        deck_deal(gp->deck, &card);
        card_player_draw(&(gp->players[0]), &card);
        if(card.suit == d3.suit){
            if(card_compare((void*)&d3, (void*)&card) >= 0){
                d3.rank = card.rank;
                d3_no = 0;
            }
        }

        deck_deal(gp->deck, &card);
        card_player_draw(&(gp->players[1]), &card);
        if(card.suit == d3.suit){
            if(card_compare((void*)&d3, (void*)&card) >= 0){
                d3.rank = card.rank;
                d3_no = 1;
            }
        }
        if(gp->player_num > 2){
            deck_deal(gp->deck, &card);
            card_player_draw(&(gp->players[2]), &card);
            if(card.suit == d3.suit){
                if(card_compare((void*)&d3, (void*)&card) >= 0){
                    d3.rank = card.rank;
                    d3_no = 2;
                }
            }
        }
        else
            hand_zero(gp->players[2].mycards);
    }

    /* the first player */
    if(d3_no == -1)
        gp->first_player_no = rand() % gp->player_num;
    else
        gp->first_player_no = d3_no;
    gp->game_state = GP_GAME_PLAY;
    gp->inning++;
    gp->curr_player_no = gp->first_player_no; 
}

int gp_get_state(gp_t* gp)
{
    if(gp)
        return gp->game_state;

    return 0;
}

void gp_set_state(gp_t* gp, int state)
{
    if(gp){
        gp->game_state = state;
    }
}

void gp_sort(hand_t* hand)
{
    cards_sort(hand);
}

void gp_analyse(hand_t* hand, analyse_r* ar)
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

void gp_cardtype(hand_t* hand, hand_type* htype)
{
    int flag,i;
    analyse_r ar;
    card_t *p;
    card_t tmpc;

    if(!hand || !htype)
        return;
    htype->type = GP_ERROR;
    p = hand->cards;
    switch(hand->num){
    case 0:
        return;
    case 1:
        htype->type = GP_SINGLE;
        htype->logic_value = card_logicvalue(p);
        return;
    case 2:
        if(p->rank == (p + 1)->rank){
            htype->type = GP_DOUBLE;
            htype->logic_value = card_logicvalue(p);
            return;
        }
        return;
    }

    memset(&ar, 0, sizeof(analyse_r));
    gp_analyse(hand, &ar);

    /* for bomb */
    if(ar.n4 > 0){
        if(ar.n4 == 1 && hand->num == 5){
            htype->type = GP_BOMB;
            htype->logic_value = ar.v4[0];
            return;
        }
        if(ar.n4 == 1 && hand->num == 7){
            htype->type = GP_FOUR_P3;
            htype->logic_value = ar.v4[0];
            return;
        }
        return;
    }

    /* for three */
    if(ar.n3 > 0){
        if(ar.n3 == 1 && hand->num == 3){
            htype->type = GP_THREE;
            htype->logic_value = ar.v3[0];
            return;
        }
        if(ar.n3 == 1 && hand->num == 4){
            tmpc.rank = cdRankK;
            tmpc.suit = cdSuitDiamond;
            if(ar.v3[0] == card_logicvalue(&tmpc))
                htype->type = GP_BOMB;
            else
                htype->type = GP_THREE_P1;
            htype->logic_value = ar.v3[0];
            return;
        }
        if(ar.n3 == 1 && hand->num == 6){
            tmpc.rank = cdRankK;
            tmpc.suit = cdSuitDiamond;
            if(ar.v3[0] == card_logicvalue(&tmpc)){
                htype->type = GP_FOUR_P3;
                htype->logic_value = ar.v3[0];
                return;
            }
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
                htype->type = GP_T_STRAIGHT;
                htype->logic_value = ar.v3[0];
                return;
            }
            if(ar.n3 * 5 == hand->num &&
                ar.n3 == ar.n2){
                for(i = 0; i < (ar.n2 - 1); ++i){
                    if((ar.v2[i+1] - ar.v2[i]) != 1)
                    return;
                }
                htype->type = GP_PLANE;
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
            htype->type = GP_D_STRAIGHT;
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
        /*flag = cards_have_rank(cdRankZJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;
        flag = cards_have_rank(cdRankFJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;*/
        for(i = 0; i < (ar.n1 - 1); ++i){
            if((ar.v1[i+1] - ar.v1[i]) != 1)
                return;
        }
        htype->type = GP_STRAIGHT;
        htype->logic_value = ar.v1[0];
        return;
    }

    return;
}

int gp_play(gp_t* gp, int player_no, hand_t* hand)
{
    int i;
    hand_type cd_type;
    card_t* card;
    card_t* plast;

    if(!hand)
        return HTERR_PARAM;

    if(gp->game_state != GP_GAME_PLAY){
        if(gp->debug)
            printf("play cards but game state not play.\n");
        return HTERR_STATE;
    }
    if(player_no != gp->curr_player_no){
        if(gp->debug)
            printf("play cards but not this no.\n");
        return -1001;
    }

    if(hand->num == 0){
        if(gp->debug)
            printf("play zero cards.\n");
        return HTERR_PARAM;
    }

    for(i = 0; i < hand->num; ++i){
        card = hand->cards + i;
        if(!card_player_have(&gp->players[player_no], card)){
            if(gp->debug){
                printf("play cards but player hasn't this card(%s).\n",
                    card_text(card));
            }
            return HTERR_NOCARD;
        }
    }

    gp_cardtype(hand, &cd_type);

    /* can play out these cards */
    if(gp->largest_player_no != player_no){
        if(!gp_canplay(gp, hand, &cd_type)){
            if(gp->debug)
                printf("cann't play these cards(smaller).\n");
            return -1002;
        }
    }

    /* player play these cards */
    plast = gp->last_hand->cards;
    gp->last_hand->num = 0;
    for(i = 0; i < hand->num; ++i){
        card = hand->cards + i;
        card_player_play(&gp->players[player_no], card);
        plast->rank = card->rank;
        plast->suit = card->suit;
        plast++;
        gp->last_hand->num++;
    }
    gp->last_htype.type = cd_type.type;
    gp->last_htype.logic_value = cd_type.logic_value;
    gp->last_htype.num = cd_type.num;
    gp->largest_player_no = player_no;

    hand_trim(gp->players[player_no].mycards);
    if(hand_num(gp->players[player_no].mycards))
        gp_next_player(gp);
    else{
        gp->game_state = GP_GAME_END;
    }

    return 1;
}

int gp_canplay(gp_t* gp, hand_t* hand, hand_type* htype)
{
    if(!gp || !hand || !htype)
        return 0;

    if(gp->last_hand->num == 0)
        return 1;
    if(htype->type == GP_ERROR)
        return 0;

    if(gp->last_htype.type == GP_BOMB && htype->type != GP_BOMB)
        return 1;
    if(gp->last_htype.type != GP_BOMB && htype->type == GP_BOMB)
        return 0;

    if(gp->last_htype.type != htype->type ||
        gp->last_hand->num != hand->num)
        return 0;

    if(htype->logic_value > gp->last_htype.logic_value)
        return 1;

    return 0;
}

void gp_next_player(gp_t* gp)
{
    if(!gp)
        return;
    gp->curr_player_no++;
    if(gp->curr_player_no >= gp->player_num)
        gp->curr_player_no = 0;
}

int gp_pass(gp_t* gp, int player_no)
{
    if(!gp)
        return 0;
    if(player_no != gp->curr_player_no)
        return 0;
    if(gp->game_state == GP_GAME_END)
        return 0;

    gp_next_player(gp);

    return 1;
}

void gp_dump(gp_t* gp)
{
    if(!gp)
        return;
    
    printf("player number:%d\n", gp->player_num);
        
    /* dump player's cards */
    printf("players cards:\n");
    hand_dump(gp->players[0].mycards, 10);
    printf("\n");
    hand_dump(gp->players[1].mycards, 10);
    printf("\n");
    if(gp->player_num > 2){
        hand_dump(gp->players[2].mycards, 10);
        printf("\n");
    }
    
    printf("last hand:\n");
    hand_dump(gp->last_hand, 10);
    printf("\n");
    
    printf("current player no is %d\n", gp->curr_player_no);
}

