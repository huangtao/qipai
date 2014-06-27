#include "dn.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ht_lch.h"
#include "sort_card.h"

#define DECK_FU     1
#define MAX_CARDS   5

static int table_rank[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0 };

typedef struct analyse_r_s{
    int n1;
    int v1[MAX_CARDS];
    int n2;
    int v2[MAX_CARDS/2];
    int n3;
    int v3[MAX_CARDS/3];
    int n4;
    int v4[MAX_CARDS/4];
}analyse_r;

dn_t* dn_new(int rule)
{
    int i;
    card_t card;
    dn_t* dn;

    dn = (dn_t*)malloc(sizeof(dn_t));
    if(!dn)
        return 0;
    dn->deck = deck_new(DECK_FU, 0);
    if(!dn->deck){
        dn_free(dn);
        return 0;
    }
    dn->debug = 0;
    dn->game_state = DN_GAME_END;
    dn->game_rule = rule;
    dn->inning = 0;
    dn->turn_time = 30;
    dn->player_num = 2;

    for(i = 0; i < GP_MAX_PLAYER; i++){
        card_player_init(&(dn->players[i]), MAX_CARDS);
        dn->players[i].position = i;
    }

    return dn;
}

void dn_free(dn_t* dn)
{
    int i;
    if(!dn)
        return;
    if(dn->deck)
        deck_free(dn->deck);
    for(i = 0; i < GP_MAX_PLAYER; i++){
        card_player_clear(&(dn->players[i]));
    }
    free(dn);
}

void dn_start(dn_t* dn)
{
    int i;
    int start_num;
    card_t card;

    if(!dn)
        return;
    deck_shuffle(dn->deck);
    dn->round = 0;
    dn->game_state = DN_GAME_PLAY;
    for(i = 0; i < DN_MAX_PLAYER; ++i){
        card_player_reset(&(dn->players[i]));
        pot_turn[i] = 0;
    }

    /* draw start cards for every player */
    if(gp->game_rule == GP_RULE_SUOHA){
        start_num = 3;
    }
    else{
        start_num = 0;
    }
    for(i = 0; i < start_num; ++i){
        for(j = 0; j < dn->player_num; ++j){
            deck_deal(dn->deck, &card);
            card_player_draw(&(dn->players[j]), &card);
        }
    }
    dn->first_player_no = rand() % dn->player_num;
    dn->game_state = DN_GAME_PLAY;
    dn->inning++;
    dn->curr_player_no = dn->first_player_no; 
}

int dn_get_state(dn_t* gp)
{
    if(dn)
        return dn->game_state;

    return 0;
}

void dn_set_state(dn_t* gp, int state)
{
    if(dn){
        dn->game_state = state;
    }
}

const char* dn_htype_name(int htype)
{
    static char* htype_name[] = {
        "DN_NONIU",
        "DN_NIUX",
        "GP_THREE_P2",
        "GP_BOMB",
        "GP_5HUA",
        "GP_5XIAO"
        };
    
    if(htype <= 5)
        return htype_name[htype];
   
    return htype_name[0];
}

void dn_handtype(dn_t* dn, hand_t* hand)
{
    int flag,i,j,k,n;
    int v1,v2,v3,sum;
    int rank;
    analyse_r ar;
    card_t *p;
    card_t tmpc;
    cd_bucket x[20];

    if(!dn || !hand)
        return;
    hand->type = DN_NONIU;
    p = hand->cards;

    /* is 5 xiao */
    flag = 0;
    for(i = 0; i < MAX_CARDS; ++i){
        if(dn_logicvalue(p) >= 5){
            flag = 1;
            break;
        }
        p++;
    }
    if(!flag){
        hand->type = DN_5XIAO;
        return;
    }

    /* is 5 hua */
    flag = 0;
    p = hand->cards;
    for(i = 0; i < MAX_CARDS; ++i){
        if(dn_logicvalue(p) <= 10){
            flag = 1;
            break;
        }
        p++;
    }
    if(!flag){
        hand->type = DN_5HUA;
        return;
    }

    memset(&ar, 0, sizeof(analyse_r));
    memset(x, 0, sizeof(cd_bucket) * 20);
    cards_bucket(hand, x);
    ar.n1 = ar.n2 = ar.n3 = ar.n4 = 0;
    for(i = 19; i >= 0; i--){
        n = x[i].num_spade + x[i].num_heart + x[i].num_club + x[i].num_diamond;
        switch(n){
        case 1:
            ar.v1[ar.n1] = i;
            ar.n1++;
            break;
        case 2:
            ar.v2[ar.n2] = i;
            ar.n2++;
            break;
        case 3:
            ar.v3[ar.n3] = i;
            ar.n3++;
            break;
        case 4:
            ar.v4[ar.n4] = i;
            ar.n4++;
            break;
        }
    }

    /* for bomb */
    if(ar.n4 > 0){
        hand->type = GP_BOMB;
        hand->type_card.rank = x[ar.v4[0]].rank;
        hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
        hand->param = ar.v4[0];
        return;
    }

    /* for three */
    if(ar.n3 == 1 && ar.n2 == 1){
        hand->type = GP_THREE_P2;
        hand->type_card.rank = x[ar.v3[0]].rank;
        hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
        hand->param = ar.v3[0];
        return;
    }

    /* niu x */
    sum = 0;
    flag = 0;
    for(i = 0; i < 3; ++i){
        v1 = dn_logicvalue(*(p+i));
        if(v1 >= 10) v1 = 10;
        for(j = i + 1; j < 5; ++j){
            v2 = dn_logicvalue(*(p+j));
            if(v2 >= 10) v2 =10;
            for(k = j + 1; k < 5; ++k){
                v3 = dn_logicvalue(*(p+k));
                if(v3 >= 10) v3 = 10;
                sum = v1 + v2 + v3;
                if(sum % 10) == 0{
                    flag = 1;
                    break;
                }
            }
        }
    }
    if(flag){
        /* have niu */
        hand->type = DN_NIUX;
        hand->param = (i << 16) || (j << 8) || k;
        sum = 0;
        for(n = 0; n < 5; ++n){
            if(n != i && n != j && n != k){
                v1 = dn_logic_value(*(p+n));
                if(v1 >= 10) v1 = 10;
                sum += v1;
            }
        }
        sum %= 10;
        hand->type_card.rank = sum;
    }

    return;
}

int dn_bet(dn_t* dn, int player_no, unsigned int chip)
{
    int i;
    hand_type htype;
    card_t* card;
    card_t* plast;

    if(!dn)
        return HTERR_PARAM;

    if(dn->game_state != DN_GAME_PLAY){
        if(dn->debug)
            printf("play cards but game state not play.\n");
        return HTERR_STATE;
    }
    if(player_no != dn->curr_player_no){
        if(dn->debug)
            printf("play cards but not this no.\n");
        return -1001;
    }

    return 1;
}

void dn_next_player(dn_t* dn)
{
    if(!dn)
        return;
    dn->curr_player_no++;
    if(dn->curr_player_no >= dn->player_num)
        dn->curr_player_no = 0;
}

void dn_dump(dn_t* dn)
{
    int i;

    if(!dn)
        return;
    
    printf("player number:%d\n", dn->player_num);
        
    /* dump player's cards */
    printf("players cards:\n");
    for(i = 0; i < dn->player_num; ++i){
        hand_dump(dn->players[0].mycards, 10);
        printf("\n");
    }
    
    printf("current player no is %d\n", dn->curr_player_no);
}

int dn_logicvalue(card_t* card)
{
    if(!card)
        return 0;

    return dn_table_rank[rank];
}
