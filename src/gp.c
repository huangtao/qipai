#include "gp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ht_lch.h"
#include "sort_card.h"

#define DECK_FU     1
#define MAX_CARDS   20

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

gp_t* gp_new(int rule)
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
    gp->debug = 0;
    gp->game_state = GP_GAME_END;
    gp->game_rule = rule;
    gp->inning = 0;
    gp->turn_time = 30;
    gp->player_num = 3;

    if(gp->game_rule == GP_RULE_DEFAULT){
        /* del three 2 and one A */
        card.suit = cdSuitDiamond;
        card.rank = cdRank2;
        deck_remove(gp->deck, &card);
        card.rank = cdRankAce;
        deck_remove(gp->deck, &card);

        card.suit = cdSuitClub;
        card.rank = cdRank2;
        deck_remove(gp->deck, &card);

        card.suit = cdSuitHeart;
        card.rank = cdRank2;
        deck_remove(gp->deck, &card);
    }
    else if(gp->game_rule == GP_RULE_ZHUJI){
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
    }

    gp->last_hand = hand_new(MAX_CARDS);
    if(!gp->last_hand){
        gp_free(gp);
        return 0;
    }

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
    int start_num;
    card_t card;
    card_t first_cd;
    int first_no;

    if(!gp)
        return;
    deck_shuffle(gp->deck);
    gp->round = 0;
    gp->game_state = GP_GAME_PLAY;
    for(i = 0; i < GP_MAX_PLAYER; ++i){
        card_player_reset(&(gp->players[i]));
    }
    hand_zero(gp->last_hand);

    /* draw start cards for every player */
    if(gp->game_rule == GP_RULE_DEFAULT){
        start_num = 16;
        first_cd.suit = cdSuitSpade;
        first_cd.rank = cdRankAce;
    }
    else{
        start_num = 15;
        first_cd.suit = cdSuitDiamond;
        first_cd.rank = cdRankAce;
    }
    first_no = -1;
    for(i = 0; i < start_num; ++i){
        deck_deal(gp->deck, &card);
        card_player_draw(&(gp->players[0]), &card);
        if(card.suit == first_cd.suit){
            if(card_compare((void*)&first_cd, (void*)&card) >= 0){
                first_cd.rank = card.rank;
                first_no = 0;
            }
        }

        deck_deal(gp->deck, &card);
        card_player_draw(&(gp->players[1]), &card);
        if(card.suit == first_cd.suit){
            if(card_compare((void*)&first_cd, (void*)&card) >= 0){
                first_cd.rank = card.rank;
                first_no = 1;
            }
        }
        if(gp->player_num > 2){
            deck_deal(gp->deck, &card);
            card_player_draw(&(gp->players[2]), &card);
            if(card.suit == first_cd.suit){
                if(card_compare((void*)&first_cd, (void*)&card) >= 0){
                    first_cd.rank = card.rank;
                    first_no = 2;
                }
            }
        }
        else
            hand_zero(gp->players[2].mycards);
    }

    /* the first player */
    if(first_no == -1)
        gp->first_player_no = rand() % gp->player_num;
    else
        gp->first_player_no = first_no;
    gp->inning++;
    gp->curr_player_no = gp->first_player_no;
}

void gp_zero(gp_t* gp)
{
    int i;

    if(!gp)
        return;
    gp->round = 0;
    for(i = 0; i < GP_MAX_PLAYER; ++i){
        card_player_reset(&(gp->players[i]));
    }
    hand_zero(gp->last_hand);
    gp->game_state = 0;
    gp->inning++;
    gp->first_player_no = 0;
    gp->curr_player_no = 0;
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

const char* gp_htype_name(int htype)
{
    static char* htype_name[] = {
        "GP_ERROR",
        "GP_SINGLE",
        "GP_DOUBLE",
        "GP_THREE",
        "GP_STRAIGHT",
        "GP_D_STRAIGHT",
        "GP_T_STRAIGHT",
        "GP_THREE_P1",
        "GP_THREE_P2",
        "GP_PLANE",
        "GP_FOUR_P3",
        "GP_BOMB"
        };

    if(htype <= 11)
        return htype_name[htype];

    return htype_name[0];
}

void gp_handtype(gp_t* gp, hand_t* hand)
{
    int flag,i,n,have_k,have_3;
    int rank;
    analyse_r ar;
    card_t *p;
    card_t tmpc;
    cd_bucket x[20];

    if(!gp || !hand)
        return;
    hand->type = GP_ERROR;
    p = hand->cards;
    switch(hand->num){
    case 0:
        return;
    case 1:
        hand->type = GP_SINGLE;
        hand->type_card.rank = p->rank;
        hand->type_card.suit = p->suit;
        hand->param = card_logicvalue(p);
        return;
    case 2:
        if(p->rank == (p + 1)->rank){
            hand->type = GP_DOUBLE;
            hand->type_card.rank = p->rank;
            hand->type_card.suit = p->suit;
            hand->param = card_logicvalue(p);
            return;
        }
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
        if(ar.n4 == 1 && hand->num == 4){
            if(gp->game_rule == GP_RULE_DEFAULT)
                hand->type = GP_BOMB;
            else
                hand->type = GP_FOUR;
            hand->type_card.rank = x[ar.v4[0]].rank;
            hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            hand->param = ar.v4[0];
            return;
        }
        if(ar.n4 == 1 && hand->num == 5){
            hand->type = GP_BOMB;
            hand->type_card.rank = x[ar.v4[0]].rank;
            hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            hand->param = ar.v4[0];
            return;
        }
        if(gp->game_rule == GP_RULE_ZHUJI){
            if(ar.n4 == 1 && hand->num == 7){
                hand->type = GP_FOUR_P3;
                hand->type_card.rank = x[ar.v4[0]].rank;
                hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                hand->param = ar.v4[0];
                return;
            }
        }
        return;
    }

    /* for three */
    if(ar.n3 > 0){
        if(ar.n3 == 1 && hand->num == 3){
            if(gp->game_rule == GP_RULE_DEFAULT){
                if(x[ar.v3[0]].rank == cdRankAce)
                    hand->type = GP_BOMB;
            }
            else
                hand->type = GP_THREE;
            hand->type_card.rank = x[ar.v3[0]].rank;
            hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            hand->param = ar.v3[0];
            return;
        }
        if(ar.n3 == 1 && hand->num == 4){
            if(gp->game_rule == GP_RULE_DEFAULT){
                if(x[ar.v3[0]].rank == cdRankAce)
                    hand->type = GP_BOMB;
            }
            else{
                if(x[ar.v3[0]].rank == cdRankK)
                    hand->type = GP_BOMB;
                else
                    hand->type = GP_THREE_P1;
            }
            hand->type_card.rank = x[ar.v3[0]].rank;
            hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            hand->param = ar.v3[0];
            return;
        }
        if(ar.n3 == 1 && ar.n2 == 1 && hand->num == 5){
            hand->type = GP_THREE_P2;
            hand->type_card.rank = x[ar.v3[0]].rank;
            hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            hand->param = ar.v3[0];
            return;
        }
        if(ar.n3 == 1 && hand->num == 6){
            if(x[ar.v3[0]].rank == cdRankK){
                hand->type = GP_FOUR_P3;
                hand->type_card.rank = x[ar.v3[0]].rank;
                hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                hand->param = ar.v3[0];
                return;
            }
        }
        if(ar.n3 > 1){
            /* not include rank 2 */
            flag = hand_rank_num(hand, cdRank2);
            if(flag)
                return;
            for(i = 0; i < (ar.n3 - 1); ++i){
                if((ar.v3[i] - (ar.v3[i+1])) != 1)
                    return;
            }
            if(ar.n3 * 3 == hand->num){
                hand->type = GP_T_STRAIGHT;
                hand->type_card.rank = x[ar.v3[0]].rank;
                hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                hand->param = ar.v3[0];
                return;
            }
            if(ar.n3 * 5 == hand->num &&
                ar.n3 == ar.n2){
                for(i = 0; i < (ar.n2 - 1); ++i){
                    if((ar.v2[i] - ar.v2[i+1]) != 1)
                    return;
                }
                hand->type = GP_PLANE;
                hand->type_card.rank = x[ar.v3[0]].rank;
                hand->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                hand->param = ar.v3[0];
                return;
            }
        }

        return;
    }

    /* for 2 */
    if(ar.n2 >= 2){
        flag = hand_rank_num(hand, cdRank2);
        if(flag)
            return;
        for(i = 0; i < (ar.n2 - 1); ++i){
            if((ar.v2[i] - ar.v2[i+1]) != 1)
                return;
        }
        if(ar.n2 * 2 == hand->num){
            hand->type = GP_D_STRAIGHT;
            hand->type_card.rank = x[ar.v2[0]].rank;
            hand->type_card.suit = get_bucket_suit(&x[ar.v2[0]]);
            hand->param = ar.v2[0];
            return;
        }

        return;
    }

    /* for straight */
    if(ar.n1 >= 5 && ar.n1 == hand->num){
        if(gp->game_rule == GP_RULE_DEFAULT){
            have_k = hand_rank_num(hand, cdRankK);
            have_3 = hand_rank_num(hand, cdRank3);
            if(have_3){
                /* 2 to 2 */
                flag = hand_rank_num(hand, cdRank2);
                if(flag){
                    x[2].rank = x[15].rank;
                    x[2].num_spade = x[15].num_spade;
                    x[2].num_heart = x[15].num_heart;
                    x[2].num_club = x[15].num_club;
                    x[2].num_diamond = x[15].num_diamond;
                    x[15].rank = 0;
                    x[15].num_spade = x[15].num_heart = 0;
                    x[15].num_club = x[15].num_diamond = 0;
                    if(!have_k){
                        /* ace to 1 */
                        x[1].rank = x[14].rank;
                        x[1].num_spade = x[14].num_spade;
                        x[1].num_heart = x[14].num_heart;
                        x[1].num_club = x[14].num_club;
                        x[1].num_diamond = x[14].num_diamond;
                        x[14].rank = 0;
                        x[14].num_spade = x[14].num_heart = 0;
                        x[14].num_club = x[14].num_diamond = 0;
                    }

                    /* recount */
                    memset(&ar, 0, sizeof(analyse_r));
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
                }
            }
            else{
                flag = hand_rank_num(hand, cdRank2);
                if(flag)
                    return;
            }
        }
        else{
            flag = hand_rank_num(hand, cdRank2);
            if(flag)
                return;
        }
        /*flag = cards_have_rank(cdRankZJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;
        flag = cards_have_rank(cdRankFJoker, ar.v1, MAX_CARDS);
        if(flag)
            return;*/
        for(i = 0; i < (ar.n1 - 1); ++i){
            if((ar.v1[i] - ar.v1[i+1]) != 1){
                return;
            }
        }
        hand->type = GP_STRAIGHT;
        hand->type_card.rank = x[ar.v1[0]].rank;
        hand->type_card.suit = get_bucket_suit(&x[ar.v1[0]]);
        hand->param = ar.v1[0];
        return;
    }

    return;
}

int gp_play(gp_t* gp, int player_no, hand_t* hand)
{
    int i;
    hand_type htype;
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

    gp_handtype(gp, hand);

    /* can play out these cards */
    if(gp->largest_player_no != player_no){
        if(!gp_canplay(gp, hand)){
            if(gp->debug)
                printf("cann't play these cards(smaller).\n");
            return -1002;
        }
    }

    /* player play these cards */
    gp->last_hand->num = 0;
    for(i = 0; i < hand->num; ++i){
        card = hand->cards + i;
        card_player_play(&gp->players[player_no], card);
    }
    hand_copy(hand, gp->last_hand);
    gp->largest_player_no = player_no;

    hand_trim(gp->players[player_no].mycards);
    if(hand_num(gp->players[player_no].mycards))
        gp_next_player(gp);
    else{
        gp->game_state = GP_GAME_END;
    }

    return 1;
}

int gp_canplay(gp_t* gp, hand_t* hand)
{
    int remain_num;

    if(!gp || !hand)
        return 0;
    gp_handtype(gp, hand);
    if(hand->type == GP_ERROR)
        return 0;
    if(gp->last_hand->num == 0){
        /* first play */
        if(hand->type == GP_THREE_P1)
            return 0;
        else
            return 1;
    }
    if(gp->largest_player_no == gp->curr_player_no){
        remain_num = hand_num(gp->players[gp->curr_player_no].mycards);
        /*if((htype->type == GP_THREE_P1 || htype->type == GP_FOUR)){
            if(remain_num == 4)
                return 1;
            else
                return 0;
        }*/
        return 1;
    }

    if(gp->last_hand->type == GP_BOMB && hand->type != GP_BOMB)
        return 0;
    if(gp->last_hand->type != GP_BOMB && hand->type == GP_BOMB)
        return 1;

    if(gp->last_hand->type != hand->type ||
        gp->last_hand->num != hand->num)
        return 0;

    if(card_logicvalue(&(hand->type_card)) > card_logicvalue(&(gp->last_hand->type_card)))
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
    if(gp->last_hand->num == 0)
        return 0;
    if(gp->largest_player_no == player_no)
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
