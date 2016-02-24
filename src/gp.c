#include "gp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "card_sort.h"

typedef struct analyse_r_s {
    int n1;
    int v1[GP_MAX_CARDS];
    int n2;
    int v2[GP_MAX_CARDS/2];
    int n3;
    int v3[GP_MAX_CARDS/3];
    int n4;
    int v4[GP_MAX_CARDS/4];
}analyse_r;

void gp_init(gp_t* gp, int rule, int mode, int player_num)
{
    int i,j,n;

    if(!gp)
        return;
    memset(gp, 0, sizeof(gp_t));

    if (mode == GP_MODE_SERVER)
        gp->mode = GP_MODE_SERVER;
    else
        gp->mode = GP_MODE_CLIENT;
    gp->player_num = player_num;
    gp->game_state = GP_GAME_END;
    gp->game_rule = rule;
    gp->inning = 0;
    gp->turn_time = 30;

    n = 54;
    deck_init(gp->deck, n);

    /* initialize gp's deck */
    n = 0;
    for (i = cdSuitDiamond; i <= cdSuitSpade; ++i) {
        for (j = cdRank3; j <= cdRankQ; ++j) {
            gp->deck[n].id = (i - 1) * 13 + j;
            gp->deck[n].suit = i;
            gp->deck[n].rank = j;
            n++;
        }
    }
    if (gp->game_rule == GP_RULE_DEFAULT) {
        /* one 2, three A and four K */
        for (i = cdSuitDiamond; i <= cdSuitSpade; ++i) {
            gp->deck[n].id = (i - 1) * 13 + cdRankK;
            gp->deck[n].suit = i;
            gp->deck[n].rank = cdRankK;
            n++;
            if (i != cdSuitDiamond) {
                gp->deck[n].id = (i - 1) * 13 + cdRankAce;
                gp->deck[n].suit = i;
                gp->deck[n].rank = cdRankAce;
                n++;
            }
        }
        gp->deck[n].id = CD_ID_S2;
        gp->deck[n].suit = cdSuitSpade;
        gp->deck[n].rank = cdRank2;
        gp->deck_all_num = 48; 
    }
    else if (gp->game_rule == GP_RULE_ZHUJI) {
        /* one 2, one A and three K */
        for (i = cdSuitClub; i <= cdSuitSpade; ++i) {
            gp->deck[n].id = (i - 1) * 13 + cdRankK;
            gp->deck[n].suit = i;
            gp->deck[n].rank = cdRankK;
            n++;
        }
        gp->deck[n].id = CD_ID_SA;
        gp->deck[n].suit = cdSuitSpade;
        gp->deck[n].rank = cdRankAce;
        n++;
        gp->deck[n].id = CD_ID_S2;
        gp->deck[n].suit = cdSuitSpade;
        gp->deck[n].rank = cdRank2;
        gp->deck_all_num = 45;
    }
    deck_shuffle(gp->deck, gp->deck_all_num);
    gp->deck_deal_index = 0;
    gp->deck_valid_num = gp->deck_all_num;

    for (i = 0; i < GP_MAX_PLAYER; i++){
        gp->players[i].position = i;
    }
}

int gp_deal(gp_t* gp, card_t* card)
{
    int i;

    if (!gp || !card)
        return -1;

    if (gp->deck_deal_index >= gp->deck_valid_num)
        return -2;

    for (i = gp->deck_deal_index; i < gp->deck_valid_num; ++i) {
        if (gp->deck[i].id > 0) {
            card->id = gp->deck[i].id;
            card->rank = gp->deck[i].rank;
            card->suit = gp->deck[i].suit;
            gp->deck_deal_index++;
            return 0;
        }
        gp->deck_deal_index++;
    }

    return -2;
}

void gp_start(gp_t* gp)
{
    int i,j;
    int start_num;
    card_t card;

    if (!gp)
        return;
    gp->round = 0;
    gp->game_state = GP_GAME_PLAY;
    gp->inning++;

    memset(&gp->last_hand_type, 0, sizeof(hand_type));
    memset(gp->last_hand, 0, sizeof(card_t) * GP_MAX_CARDS);

    if (gp->mode == GP_MODE_SERVER) {
        deck_shuffle(gp->deck, gp->deck_all_num);
        gp->deck_deal_index = 0;
        gp->deck_valid_num = gp->deck_all_num;

        /* draw start cards for every player */
        if (gp->game_rule == GP_RULE_DEFAULT) {
            start_num = 16;
        } else {
            start_num = 15;
        }
        for (i = 0; i < start_num; ++i) {
            for (j = 0; j < gp->player_num; ++j) {
                gp_deal(gp, &card);
                cards_add(gp->players[j].cards, GP_MAX_CARDS, &card);
            }
        }

        /* the first player */
        gp->first_player_no = rand() % gp->player_num;
        gp->curr_player_no = gp->first_player_no;
    } else {
        for (i = 0; i < 3; i++) {
            memset(gp->players[i].cards, 0, sizeof(card_t) * GP_MAX_PLAYER);
            memset(gp->players[i].cards_played, 0, sizeof(card_t) * GP_MAX_PLAYER);
        }
        gp->first_player_no = 0;
        gp->curr_player_no = 0;
    }
}

void gp_sort(card_t* cards, int len)
{
    cards_sort(cards, len);
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
		"GP_FOUR",
        "GP_FOUR_P3",
        "GP_BOMB"
    };

    if (htype <= 12)
        return htype_name[htype];
    else
        return htype_name[0];
}

void gp_handtype(gp_t* gp, card_t* cards, int len, hand_type* ht)
{
    int flag,i,n,have_k,have_3;
    analyse_r ar;
    card_t *p;
    cd_bucket x[20];

    if (!gp || !cards || len <= 0 || !ht)
        return;
    ht->type = GP_ERROR;
    p = cards;
    ht->num = cards_num(cards, len);
    switch (ht->num) {
        case 0:
            return;
        case 1:
            ht->type = GP_SINGLE;
            memcpy(&ht->type_card, p, sizeof(card_t));
            ht->param = card_logicvalue(p);
            return;
        case 2:
            if (p->rank == (p + 1)->rank) {
                ht->type = GP_DOUBLE;
                memcpy(&ht->type_card, p, sizeof(card_t));
                ht->param = card_logicvalue(p);
                return;
            }
            return;
        default:
            break;
    }

    memset(&ar, 0, sizeof(analyse_r));
    memset(x, 0, sizeof(cd_bucket) * 20);
    cards_bucket(cards, len, x);
    ar.n1 = ar.n2 = ar.n3 = ar.n4 = 0;
    for (i = 19; i >= 0; --i) {
        n = x[i].num_spade + x[i].num_heart + x[i].num_club + x[i].num_diamond;
        switch (n) {
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
    if (ar.n4 > 0) {
        if (ar.n4 == 1 && ht->num == 4) {
            if(gp->game_rule == GP_RULE_DEFAULT)
                ht->type = GP_BOMB;
            else
                ht->type = GP_FOUR;
            ht->type_card.rank = x[ar.v4[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param = ar.v4[0];
            return;
        }
        if (ar.n4 == 1 && ht->num == 5) {
            ht->type = GP_BOMB;
            ht->type_card.rank = x[ar.v4[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param = ar.v4[0];
            return;
        }
        if (gp->game_rule == GP_RULE_ZHUJI) {
            if (ar.n4 == 1 && ht->num == 7){
                ht->type = GP_FOUR_P3;
                ht->type_card.rank = x[ar.v4[0]].rank;
                ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
                ht->param = ar.v4[0];
                return;
            }
        }
        return;
    }

    /* for three */
    if (ar.n3 > 0) {
        if (ar.n3 == 1 && ht->num == 3) {
            if (gp->game_rule == GP_RULE_DEFAULT) {
                if (x[ar.v3[0]].rank == cdRankAce)
                    ht->type = GP_BOMB;
            } else {
                ht->type = GP_THREE;
            }
            ht->type_card.rank = x[ar.v3[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param = ar.v3[0];
            return;
        }
        if (ar.n3 == 1 && ht->num == 4) {
            if (gp->game_rule == GP_RULE_DEFAULT) {
                if(x[ar.v3[0]].rank == cdRankAce)
                    ht->type = GP_BOMB;
            } else {
                if (x[ar.v3[0]].rank == cdRankK)
                    ht->type = GP_BOMB;
                else
                    ht->type = GP_THREE_P1;
            }
            ht->type_card.rank = x[ar.v3[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param = ar.v3[0];
            return;
        }
        if (ar.n3 == 1 && ar.n2 == 1 && ht->num == 5) {
            ht->type = GP_THREE_P2;
            ht->type_card.rank = x[ar.v3[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param = ar.v3[0];
            return;
        }
        if (ar.n3 == 1 && ht->num == 6) {
            if (x[ar.v3[0]].rank == cdRankK) {
                ht->type = GP_FOUR_P3;
                ht->type_card.rank = x[ar.v3[0]].rank;
                ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
                ht->param = ar.v3[0];
                return;
            }
        }
        if (ar.n3 > 1) {
            /* not include rank 2 */
            flag = cards_rank_num(cards, len, cdRank2);
            if (flag)
                return;
            for (i = 0; i < (ar.n3 - 1); ++i) {
                if((ar.v3[i] - (ar.v3[i+1])) != 1)
                    return;
            }
            if (ar.n3 * 3 == ht->num) {
                ht->type = GP_T_STRAIGHT;
                ht->type_card.rank = x[ar.v3[0]].rank;
                ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
                ht->param = ar.v3[0];
                return;
            }
            if (ar.n3 * 5 == ht->num &&
                    ar.n3 == ar.n2) {
                for (i = 0; i < (ar.n2 - 1); ++i) {
                    if ((ar.v2[i] - ar.v2[i+1]) != 1)
                        return;
                }
                ht->type = GP_PLANE;
                ht->type_card.rank = x[ar.v3[0]].rank;
                ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
                ht->param = ar.v3[0];
                return;
            }
        }
        return;
    }

    /* for 2 */
    if (ar.n2 >= 2) {
        flag = cards_rank_num(cards, len, cdRank2);
        if (flag)
            return;
        for (i = 0; i < (ar.n2 - 1); ++i){
            if ((ar.v2[i] - ar.v2[i+1]) != 1)
                return;
        }
        if (ar.n2 * 2 == ht->num) {
            ht->type = GP_D_STRAIGHT;
            ht->type_card.rank = x[ar.v2[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v2[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param = ar.v2[0];
            return;
        }
        return;
    }

    /* for straight */
    if (ar.n1 >= 5 && ar.n1 == ht->num) {
        if (gp->game_rule == GP_RULE_DEFAULT) {
            have_k = cards_rank_num(cards, len, cdRankK);
            have_3 = cards_rank_num(cards, len, cdRank3);
            if (have_3) {
                /* 2 to 2 */
                flag = cards_rank_num(cards, len, cdRank2);
                if (flag) {
                    x[2].rank = x[15].rank;
                    x[2].num_spade = x[15].num_spade;
                    x[2].num_heart = x[15].num_heart;
                    x[2].num_club = x[15].num_club;
                    x[2].num_diamond = x[15].num_diamond;
                    x[15].rank = 0;
                    x[15].num_spade = x[15].num_heart = 0;
                    x[15].num_club = x[15].num_diamond = 0;
                    if (!have_k) {
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
                    for (i = 19; i >= 0; i--) {
                        n = x[i].num_spade + x[i].num_heart + x[i].num_club + x[i].num_diamond;
                        switch (n) {
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
            } else {
                flag = cards_rank_num(cards, len, cdRank2);
                if (flag)
                    return;
            }
        } else {
            flag = cards_rank_num(cards, len, cdRank2);
            if (flag)
                return;
        }
        /*flag = cards_have_rank(cdRankZJoker, ar.v1, MAX_CARDS);
          if(flag)
          return;
          flag = cards_have_rank(cdRankFJoker, ar.v1, MAX_CARDS);
          if(flag)
          return;*/
        for (i = 0; i < (ar.n1 - 1); ++i) {
            if ((ar.v1[i] - ar.v1[i+1]) != 1) {
                return;
            }
        }
        ht->type = GP_STRAIGHT;
        ht->type_card.rank = x[ar.v1[0]].rank;
        ht->type_card.suit = get_bucket_suit(&x[ar.v1[0]]);
        ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
        ht->param = ar.v1[0];
        return;
    }

    return;
}

int gp_play(gp_t* gp, int player_no, card_t* cards, int len)
{
    int i,valid_num;
    hand_type htype;
    card_t* card;

    if (!cards || len <= 0)
        return -1;

    if (gp->game_state != GP_GAME_PLAY) {
        if (gp->debug)
            printf("play cards but game state not play.\n");
        return -2;
    }
    if (player_no != gp->curr_player_no) {
        if (gp->debug)
            printf("play cards but not this no.\n");
        return -3;
    }
    valid_num = cards_num(cards, len);
    if (valid_num == 0) {
        if (gp->debug)
            printf("play zero cards.\n");
        return -4;
    }

    if (gp->mode == GP_MODE_SERVER) {
        for (i = 0; i < valid_num; ++i){
            card = cards + i;
            if (!cards_have(gp->players[player_no].cards, GP_MAX_CARDS, card)) {
                if (gp->debug) {
                    printf("play cards but player hasn't this card(%s).\n",
                            card_to_string(card));
                }
                return -5;
            }
        }
    }

    gp_handtype(gp, cards, len, &htype);

    /* can play out these cards */
    if (gp->largest_player_no != player_no) {
        if (!gp_canplay(gp, cards, len)){
            if (gp->debug)
                printf("cann't play these cards(smaller).\n");
            return -6;
        }
    }

    /* player play these cards */
    memset(gp->last_hand, 0, sizeof(card_t) * GP_MAX_CARDS);
    for (i = 0; i < valid_num; ++i) {
        card = cards + i;
        cards_del(gp->players[player_no].cards, GP_MAX_CARDS, card);
        cards_add(gp->last_hand, GP_MAX_CARDS, card);
    }
    memcpy(&gp->last_hand_type, &htype, sizeof(hand_type));
    memcpy(gp->players[player_no].cards_played, gp->last_hand,
           sizeof(card_t) * GP_MAX_CARDS);
    gp->largest_player_no = player_no;
    cards_trim(gp->players[player_no].cards, GP_MAX_CARDS);
    if (cards_num(gp->players[player_no].cards, GP_MAX_CARDS))
        gp_next_player(gp);
    else {
        if (gp->mode == GP_MODE_SERVER)
            gp->game_state = GP_GAME_END;
    }

    return 1;
}

int gp_canplay(gp_t* gp, card_t* cards, int len)
{
    int remain_num;
    hand_type ht;

    if (!gp || !cards || len <= 0)
        return 0;
    gp_handtype(gp, cards, len, &ht);
    if(ht.type == GP_ERROR)
        return 0;
    if (gp->last_hand_type.num == 0) {
        /* first play */
        if (ht.type == GP_THREE_P1)
            return 0;
        else
            return 1;
    }
    if (gp->largest_player_no == gp->curr_player_no) {
        remain_num = cards_num(gp->players[gp->curr_player_no].cards, GP_MAX_CARDS);
        if ((ht.type == GP_THREE_P1 || ht.type == GP_FOUR)) {
            if (remain_num == 4)
                return 1;
            else
                return 0;
          }
        return 1;
    }

    if (gp->last_hand_type.type == GP_BOMB && ht.type != GP_BOMB)
        return 0;
    if (gp->last_hand_type.type != GP_BOMB && ht.type == GP_BOMB)
        return 1;

    if(gp->last_hand_type.type != ht.type ||
            gp->last_hand_type.num != ht.num)
        return 0;

    if(card_logicvalue(&(ht.type_card)) > card_logicvalue(&(gp->last_hand_type.type_card)))
        return 1;

    return 0;
}

void gp_next_player(gp_t* gp)
{
    if (!gp)
        return;
    gp->curr_player_no++;
    if (gp->curr_player_no >= gp->player_num)
        gp->curr_player_no = 0;
}

int gp_pass(gp_t* gp, int player_no)
{
    int i;

    if (!gp)
        return 0;
    if (player_no != gp->curr_player_no)
        return 0;
    if (gp->game_state == GP_GAME_END)
        return 0;
    if (gp->last_hand_type.num == 0)
        return 0;
    if (gp->largest_player_no == player_no)
        return 0;
    memset(&gp->last_hand_type, 0, sizeof(hand_type));
    memset(gp->last_hand, 0, sizeof(card_t) * GP_MAX_CARDS);
    for (i = 0; i < gp->player_num; ++i) {
        memset(gp->players[i].cards_played, 0, sizeof(card_t) * GP_MAX_CARDS);
    }
    gp_next_player(gp);
    gp->first_player_no = gp->curr_player_no;
    return 1;
}

/* 出牌提示 */
int gp_hint(gp_t* gp, card_t* cards, int len)
{
    int i,j,n,rank;
    int ret_n;
    cd_bucket x[20];
    int js[20];

    if (!gp || !cards || len < GP_MAX_CARDS)
        return 0;
    
    memset(cards, 0, sizeof(card_t) * len);
    memset(x, 0, sizeof(cd_bucket) * 20);
    cards_bucket(gp->players[gp->curr_player_no].cards, GP_MAX_CARDS, x);
    for (i = 0; i < 20; i++) {
        js[i] = x[i].num_spade + x[i].num_heart +
            x[i].num_club + x[i].num_diamond;
    }

    if (gp->curr_player_no == gp->first_player_no) {
        /* 必须出牌 */
        ret_n = 0;
        /* 查找单张 */
        for (i = 3; i < 8; i++) {
            if (js[i] > 1) continue;
            if (js[i+1] >= 1 && js[i+2] >=1 && js[i+3] >= 1 &&
                    js[i+4] >= 1) {
                continue;
            }
            ret_n = 1;
            gp_copy_cards(gp, gp->curr_player_no, cards, 0, i, 1);
            break;
        }
        if (ret_n == 0) {
            memcpy(cards, gp->players[gp->curr_player_no].cards, sizeof(card_t));
            ret_n = 1;
        }
    } else {
        /* 跟牌 */
        ret_n = 0;
        if (gp->last_hand_type.type == GP_SINGLE) {
            for (i = 3; i <= 13; ++i) {
                if (js[i] == 1 &&
                        rank2logic(i) > card_logicvalue(&gp->last_hand_type.type_card)) {
                    rank = i;
                    ret_n = 1;
                    break;
                }
            }
            if (ret_n == 0) {
                for (i = 2; i >= 1; --i) {
                    if (js[i] == 1 &&
                            rank2logic(i) > card_logicvalue(&gp->last_hand_type.type_card)) {
                        rank = i;
                        ret_n = 1;
                        break;
                    }
                }
            }
            if (ret_n > 0)
                gp_copy_cards(gp, gp->curr_player_no, cards, 0, rank, 1);
        } else if (gp->last_hand_type.type == GP_DOUBLE) {
            for (i = 4; i <= 13; ++i) {
                if (js[i] == 2 &&
                        rank2logic(i) > card_logicvalue(&gp->last_hand_type.type_card)) {
                    rank = i;
                    ret_n = 2;
                    break;
                }
            }
            if (ret_n > 0)
                gp_copy_cards(gp, gp->curr_player_no, cards, 0, rank, 2);
        } else if(gp->last_hand_type.type == GP_THREE) {
            for (i = 4; i <= 12; ++i) {
                if (js[i] == 3 &&
                        rank2logic(i) > card_logicvalue(&gp->last_hand_type.type_card)) {
                    rank = i;
                    ret_n = 3;
                    break;
                }
            }
            if (ret_n > 0)
                gp_copy_cards(gp, gp->curr_player_no, cards, 0, rank, 3);
        } else if (gp->last_hand_type.type == GP_STRAIGHT) {
			rank = 0;
            for (i = 4; i<= 10; ++i) {
                if (js[i] == 0) continue;
                if (rank2logic(i) <= card_logicvalue(&gp->last_hand_type.type_card))
                    continue;
                ret_n = gp->last_hand_type.num;
                for (j = i; j < (i + gp->last_hand_type.num); ++j) {
                    if (js[j] == 0) {
                        ret_n = 0;
                        break;
                    }
                }
                if (ret_n > 0) {
                    rank = i;
                    break;
                }
            }
			if (ret_n > 0) {
				n = 0;
				for (i = rank; i < (rank + ret_n); ++i) {
					gp_copy_cards(gp, gp->curr_player_no, cards, n++, i, 1);  
				}
			}
        } else if (gp->last_hand_type.type == GP_D_STRAIGHT) {
			for (i = 4; i<= 10; ++i) {
                if (js[i] < 2) continue;
                if (rank2logic(i) <= card_logicvalue(&gp->last_hand_type.type_card))
                    continue;
                ret_n = gp->last_hand_type.num;
                for (j = i; j < (i + gp->last_hand_type.num); ++j) {
                    if (js[j] < 2) {
                        ret_n = 0;
                        break;
                    }
                }
                if (ret_n > 0) {
                    rank = i;
                    break;
                }
            }
			if (ret_n > 0) {
				n = 0;
				for (i = rank; i < (rank + ret_n); ++i) {
					gp_copy_cards(gp, gp->curr_player_no, cards, n, i, 2);
					n += 2;
				}
			}
        } else if (gp->last_hand_type.type == GP_T_STRAIGHT) {
            for (i = 4; i<= 10; ++i) {
                if (js[i] < 3) continue;
                if (rank2logic(i) <= card_logicvalue(&gp->last_hand_type.type_card))
                    continue;
                ret_n = gp->last_hand_type.num;
                for (j = i; j < (i + gp->last_hand_type.num); ++j) {
                    if (js[j] < 3) {
                        ret_n = 0;
                        break;
                    }
                }
                if (ret_n > 0) {
                    rank = i;
                    break;
                }
            }
			if (ret_n > 0) {
				n = 0;
				for (i = rank; i < (rank + ret_n); ++i) {
					gp_copy_cards(gp, gp->curr_player_no, cards, n, i, 2);
					n += 2;
				}
			}
        } else if (gp->last_hand_type.type == GP_THREE_P2) {
            for (i = 4; i<= 10; ++i) {
                if (js[i] < 3) continue;
                if (rank2logic(i) <= card_logicvalue(&gp->last_hand_type.type_card)) {
                    rank = i;
                    ret_n += 3;
                    break;
                }
            }
            if (ret_n > 0) {
                /* 有对子吗*/
                for (i = 3; i <= 13; ++i) {
                    if (js[i] != 2) continue;
                    if (i == rank) continue;
                    ret_n += 2;
                    j = i;
                    break;
                }
            }
            if (ret_n == 5) {
                gp_copy_cards(gp, gp->curr_player_no, cards, 0, rank, 3);
                gp_copy_cards(gp, gp->curr_player_no, cards, 3, j, 2);
            } else {
                ret_n = 0;
            }
        }
        if (ret_n == 0) { 
            for (i = cdRank3; i <= cdRankK; ++i) {
                if (js[i] == 4) {
                    if (gp->last_hand_type.type == GP_BOMB) {
                        if (rank2logic(i) <= card_logicvalue(&gp->last_hand_type.type_card))
                            continue;
                    }
                    rank = i;
                    ret_n = 4;
                    n = 4;
                    break;
                } else if (js[i] == 3 && i == cdRankK) {
                    rank = i;
                    ret_n = 3;
                    n = 3;
                    break;
                }
            }
            if (ret_n > 0) {
                /* 拿单张 */
                for (i = cdRank3; i <= cdRankK; ++i) {
                    if (js[i] > 0 && i != rank) {
                        j = i;
                        ret_n += 1;
                        break;
                    }
                }
            }
            if (ret_n == 5 || (ret_n == 4 && rank == cdRankK)) {
                gp_copy_cards(gp, gp->curr_player_no, cards, 0, rank, n);
                gp_copy_cards(gp, gp->curr_player_no, cards, n, j, 1);
            } else {
                ret_n = 0;
            }
        }
    }

    return ret_n;
}

void gp_copy_cards(gp_t* gp, int player_no, card_t* cards, int offset, int rank, int num)
{
    int i,n;

    if (!gp || !cards || num == 0)
        return;
    if (offset < 0 || offset >= GP_MAX_CARDS)
        return;
    if (rank == 0)
        return;

    n = 0;
    for (i = 0; i < GP_MAX_CARDS; i++) {
        if (gp->players[player_no].cards[i].rank == rank) {
            memcpy(cards+offset+n, gp->players[player_no].cards+i, sizeof(card_t));
            n++;
            if (n >= num)
                return;
        }
    }
}

void gp_dump(gp_t* gp)
{
    if(!gp)
        return;

    printf("player number:%d\n", gp->player_num);

    /* dump player's cards */
    printf("players cards:\n");
    cards_dump(gp->players[0].cards, GP_MAX_CARDS, 10);
    printf("\n");
    cards_dump(gp->players[1].cards, GP_MAX_CARDS, 10);
    printf("\n");
    if (gp->player_num > 2) {
        cards_dump(gp->players[2].cards, GP_MAX_CARDS, 10);
        printf("\n");
    }

    printf("last hand:\n");
    cards_dump(gp->last_hand, GP_MAX_CARDS, 10);
    printf("\n");

    printf("current player no is %d\n", gp->curr_player_no);
    printf("last hand type is %d\n",gp->last_hand_type.type);
}
