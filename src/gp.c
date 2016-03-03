#include "gp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "card_algo.h"

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

        /* sort cards */
        for (i = 0; i < gp->player_num; ++i) {
            gp_sort(gp->players[i].cards, GP_MAX_CARDS);
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
            ht->param1 = card_logic(p);
            return;
        case 2:
            if (p->rank == (p + 1)->rank) {
                ht->type = GP_DOUBLE;
                memcpy(&ht->type_card, p, sizeof(card_t));
                ht->param1 = card_logic(p);
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
            ht->param1 = ar.v4[0];
            return;
        }
        if (ar.n4 == 1 && ht->num == 5) {
            ht->type = GP_BOMB;
            ht->type_card.rank = x[ar.v4[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param1 = ar.v4[0];
            return;
        }
        if (gp->game_rule == GP_RULE_ZHUJI) {
            if (ar.n4 == 1 && ht->num == 7){
                ht->type = GP_FOUR_P3;
                ht->type_card.rank = x[ar.v4[0]].rank;
                ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
                ht->param1 = ar.v4[0];
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
            ht->param1 = ar.v3[0];
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
            ht->param1 = ar.v3[0];
            return;
        }
        if (ar.n3 == 1 && ar.n2 == 1 && ht->num == 5) {
            ht->type = GP_THREE_P2;
            ht->type_card.rank = x[ar.v3[0]].rank;
            ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
            ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
            ht->param1 = ar.v3[0];
            return;
        }
        if (ar.n3 == 1 && ht->num == 6) {
            if (x[ar.v3[0]].rank == cdRankK) {
                ht->type = GP_FOUR_P3;
                ht->type_card.rank = x[ar.v3[0]].rank;
                ht->type_card.suit = get_bucket_suit(&x[ar.v3[0]]);
                ht->type_card.id = (ht->type_card.suit - 1) * 13 + ht->type_card.rank;
                ht->param1 = ar.v3[0];
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
                ht->param1 = ar.v3[0];
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
                ht->param1 = ar.v3[0];
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
            ht->param1 = ar.v2[0];
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
        ht->param1 = ar.v1[0];
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
    if (ht.type == GP_ERROR)
        return 0;
    if (gp->game_rule == GP_RULE_ZHUJI) {
        if (ht.type == GP_THREE_P1 || ht.type == GP_FOUR) {
            /* 3带1最后一手可以出 */
            if (gp->last_hand_type.type == GP_ERROR) {
                remain_num = cards_num(gp->players[gp->curr_player_no].cards, GP_MAX_CARDS);
                if (remain_num == 4)
                    return 1;
                else
                    return 0;
            } else {
                return 0;
            }
        }
    } else {
        if (ht.type == GP_THREE_P1)
            return 0;
    }

    if (gp->last_hand_type.type == GP_ERROR) {
        /* 先手 */
        return 1;
    }

    /* 跟牌 */
    if (gp->last_hand_type.type == GP_BOMB && ht.type != GP_BOMB)
        return 0;
    if (gp->last_hand_type.type != GP_BOMB && ht.type == GP_BOMB)
        return 1;

    if(gp->last_hand_type.type != ht.type ||
            gp->last_hand_type.num != ht.num)
        return 0;

    if(card_logic(&(ht.type_card)) > card_logic(&(gp->last_hand_type.type_card)))
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

/* 出牌提示,返回可以出牌的张数 */
/* 专业AI请联系作者 */
int gp_hint(gp_t* gp, card_t* cards, int len)
{
    int i,j,n,ret;
	hand_type htype;
    cd_analyse result;

    if (!gp || !cards || len < GP_MAX_CARDS)
        return 0;
    
	ret = 0;
    /* 全部可以打出吗 */
    memcpy(cards, gp->players[gp->curr_player_no].cards,
            sizeof(card_t) * GP_MAX_CARDS);
    if (gp_canplay(gp, cards, GP_MAX_CARDS)) {
        ret = cards_num(cards, GP_MAX_CARDS);
        return ret;
    }

	/* 分析扑克 */
    memset(cards, 0, sizeof(card_t) * len);
    cards_analyse(gp->players[gp->curr_player_no].cards, GP_MAX_CARDS, &result);
    
    if (gp->last_hand_type.type == GP_ERROR) {
        /* 先出牌 */
		/* 根据最小一张的数量来出 */
		for (i = 3; i <= 13; ++i) {
			if (result.count[i] == 1) {
				n = 1;
				for (j = i + 1; j <= 13; ++j) {
					if (result.count[j] == 0)
						break;
					n++;
				}
				if (n >= 5) {
					/* 有顺子 */
					for (j = i; j < (i + n); ++j) {
						gp_copy_cards(result.raw_cards, cards, j - i,
								card_logic2rank(j), 1);
						ret = n;
					}
					break;
				}
			} else if (result.count[i] == 2) {
				gp_copy_cards(result.raw_cards, cards, 0,
						card_logic2rank(i), 2);
				ret = 2;
				break;
			} else if (result.count[i] == 3) {
				gp_copy_cards(result.raw_cards, cards, 0,
						card_logic2rank(i), 3);
				ret = 3;
				break;
			}
		}
		if (ret == 0) {
			/* 得到最小的一张 */
			for (i = GP_MAX_CARDS - 1; i >= 0; --i) {
				if (gp->players[gp->curr_player_no].cards[i].id != 0) {
					memcpy(cards, 
							gp->players[gp->curr_player_no].cards + i,
							sizeof(card_t));
                    ret = 1;
                    break;
				}
			}
		}
    } else {
        /* 跟牌 */
        memcpy(&htype, &gp->last_hand_type, sizeof(hand_type));
        ret = gp_analyse_search(&result, &htype, cards, len);
        if (ret == 0) {
            /* 有炸弹吗 */
            memset(&htype, 0, sizeof(hand_type));
            htype.type = GP_BOMB;
            ret = gp_analyse_search(&result, &htype, cards, len);
        }
	}
    return ret;
}

/*
 * 查找和ht_in同牌型且比ht_in打的牌组,没有返回0
 * 注意不找炸弹
 */
int gp_analyse_search(cd_analyse* analyse, hand_type* ht_in, card_t* cards, int len)
{
	int i,j,ret,n;
    int n_left,n_right;
    int b_straight;

	if (!analyse || !ht_in || !cards)
		return 0;

	if (ht_in->type == GP_ERROR)
		return 0;

	if (len < GP_MAX_CARDS)
		return 0;

	ret = 0;
	if (ht_in->type == GP_SINGLE) {
		/* logic value 3~15 */
		ret = 0;
        for (i = 3; i <= 15; ++i) {
            if (analyse->count[i] == 1 && i > card_logic(&ht_in->type_card)) {
                /* 是单张 */
                n_left = n_right = 0;
                for (j = i - 1; j >= 3; j--) {
                    if (analyse->count[j] > 0)
                        n_right++;
                }
                for (j = i + 1; j <= 13; j++) {
                    if (analyse->count[j] > 0)
                        n_left++;
                }
                if ((n_left + n_right + 1) < 5) {
                    gp_copy_cards(analyse->raw_cards, cards, 0,
                                  card_logic2rank(i), 1);
                    ret = 1;
                    break;
                }
			}
		}
        if (ret == 0) {
            /* 选张大的 */
            for (i = 3; i <= 15; ++i) {
                if (analyse->count[i] > 0 && i > card_logic(&ht_in->type_card)) {
                    gp_copy_cards(analyse->raw_cards, cards, 0,
                                  card_logic2rank(i), 1);
                    ret = 1;
                    break;
                }
            }
        }
    } else if (ht_in->type == GP_DOUBLE) {
		if (analyse->num_2 == 0)
			return 0;
		for (i = 3; i <= 13; ++i) {
			if (analyse->count[i] == 2 && i > card_logic(&ht_in->type_card)) {
                gp_copy_cards(analyse->raw_cards, cards, 0,
                        card_logic2rank(i), 2);
                ret = 2;
				break;
			}
		}
    } else if (ht_in->type == GP_THREE) {
		if (analyse->num_3 == 0)
			return 0;
		for (i = 3; i <= 12; ++i) {
			if (analyse->count[i] == 3 && i > card_logic(&ht_in->type_card)) {
                gp_copy_cards(analyse->raw_cards, cards, 0,
                        card_logic2rank(i), 3);
                ret = 3;
				break;
			}
		}
    } else if (ht_in->type == GP_STRAIGHT) {
        if (analyse->valid_num < ht_in->num)
            return 0;
        for (i = 3; i<= 10; ++i) {
			if (analyse->count[i] == 0)
				continue;
            if ((i + ht_in->num -1) <= card_logic(&ht_in->type_card))
                continue;
			b_straight = 1;
			for (j = i + 1; j < (i + ht_in->num); ++j) {
				if (analyse->count[j] == 0) {
					b_straight = 0;
					break;
				}
			}
            if (b_straight) {
                for (j = i; j < (i + ht_in->num); ++j) {
                    gp_copy_cards(analyse->raw_cards, cards, j - i,
                            card_logic2rank(j), 1);
                }
                ret = ht_in->num;
				break;
			}
		}
    } else if (ht_in->type == GP_D_STRAIGHT) {
        if ((analyse->num_2 * 2) < ht_in->num)
            return 0;
        for (i = 3; i<= (14 - ht_in->num / 2); ++i) {
            if (analyse->count[i] < 2)
				continue;
            if ((i + ht_in->num - 1) <= card_logic(&ht_in->type_card))
                continue;
			b_straight = 1;
            for (j = i + 1; j < (i + ht_in->num / 2); ++j) {
				if (analyse->count[j] < 2) {
					b_straight = 0;
					break;
				}
			}
			if (b_straight) {
                for (j = i; j < (i + ht_in->num / 2); ++j) {
                    gp_copy_cards(analyse->raw_cards, cards, (j - i) * 2,
                            card_logic2rank(j), 2);
                }
                ret = ht_in->num;
				break;
			}
		}
    } else if (ht_in->type == GP_T_STRAIGHT) {
        if ((analyse->num_3 * 3) < ht_in->num)
			return 0;
        for (i = 3; i<= (14 - ht_in->num / 3); ++i) {
            if (analyse->count[i] < 3)
				continue;
            if ((i + ht_in->num - 1) <= card_logic(&ht_in->type_card))
                continue;
            b_straight = 1;
            for (j = i + 1; j < (i + ht_in->num / 3); ++j) {
                if (analyse->count[j] < 3) {
					b_straight = 0;
                    break;
                }
            }
			if (b_straight) {
                for (j = i; j < (i + ht_in->num / 3); ++j) {
                    gp_copy_cards(analyse->raw_cards, cards, (j - i) * 3,
                            card_logic2rank(j), 3);
                }
                ret = ht_in->num;
				break;
			}
		}
    } else if (ht_in->type == GP_THREE_P2) {
		if (analyse->num_3 == 0 || analyse->num_2 == 0)
			return 0;
		/* 先找到3个 */
        for (i = 3; i <= 12; ++i) {
            if (analyse->count[i] < 3)
				continue;
            if (i > card_logic(&ht_in->type_card)) {
                gp_copy_cards(analyse->raw_cards, cards, 0,
                        card_logic2rank(i), 3);
				/* 找一对 */
				for (j = 3; j <= 13; ++j) {
					if (analyse->count[i] == 2) {
                        gp_copy_cards(analyse->raw_cards, cards, 3,
                                card_logic2rank(j), 2);
                        ret = ht_in->num;
						break;
					}
				}
            }
		}
    } else if (ht_in->type == GP_PLANE) {
        n = ht_in->num / 5;
        if (analyse->num_3 < n || analyse->num_2 < n)
            return 0;
        for (i = 3; i < (13 - n); ++i) {
            if (analyse->count[i] < 3)
                continue;
            if ((i + ht_in->num - 1) <= card_logic(&ht_in->type_card))
                continue;
            b_straight = 1;
            /* 判断3个连续数量 */
            for (j = i + 1; j < (i + n); ++j) {
                if (analyse->count[j] != 3) {
                    b_straight = 0;
                    break;
                }
            }
            if (b_straight) {
                for (j = i; j < (i + n); ++j) {
                    gp_copy_cards(analyse->raw_cards, cards, (j - i) * 3,
                            card_logic2rank(i), 3);
                }
                break;
            }
        }
        if (b_straight) {
            /* 有带的2对吗 */
            for (i = 3; i < (13 - n); ++i) {
                if (analyse->count[i] != 2)
                    continue;
                b_straight = 1;
                for (j = i + 1; j < (i + n); ++j) {
                    if (analyse->count[j] != 2) {
                        b_straight = 0;
                        break;
                    }
                }
                if (b_straight) {
                    for (j = i; j < (i + n); ++j) {
                        gp_copy_cards(analyse->raw_cards, cards,
                                n * 3 + (j - i) * 2,
                                card_logic2rank(j), 2);
                    }
                    ret = ht_in->num;
                    break;
                }
            }
        }
    } else if (ht_in->type == GP_FOUR_P3) {
        if (analyse->num_4 == 0)
            return 0;
        if (analyse->valid_num < 7)
            return 0;
        for (i = 3; i < 13; ++i) {
            if (analyse->count[i] != 4)
                continue;
            if (i > card_logic(&ht_in->type_card)) {
                gp_copy_cards(analyse->raw_cards, cards, 0,
                        card_logic2rank(i), 4);
                n = 0;
                for (j = 3; j <= 13 && j != i; ++j) {
                    if (analyse->count[j] > 0 && analyse->count[j] < 4) {
                        gp_copy_cards(analyse->raw_cards, cards, 4 + n,
                                card_logic2rank(j), analyse->count[j]);
                        n += analyse->count[j];
                        if (n >= 3) {
                            break;
                        }
                    }
                }
                ret = 7;
                break;
            }
        }
    } else if (ht_in->type == GP_BOMB) {
        if (analyse->valid_num < 5)
            return 0;
        n = analyse->num_4 + analyse->num_3;
        if (n == 0)
            return 0;
        for (i = 3; i <= 13; ++i) {
            if (analyse->count[i] != 4) {
                if (!(analyse->count[i] == 3 && i == cdRankK))
                    continue;
            }
            if (i > card_logic(&ht_in->type_card)) {
                gp_copy_cards(analyse->raw_cards, cards, 0,
                        card_logic2rank(i), 4);
                for (j = 3; j <= 15 && j != i; ++j) {
                    if (analyse->count[j] > 0) {
                        gp_copy_cards(analyse->raw_cards, cards, 4,
                                card_logic2rank(j), 1);
                        break;
                    }
                }
                if (i == cdRankK)
                    ret = 4;
                else
                    ret = 5;
                break;
            }
        }
    } else {
        return 0;
    }

    return ret;
}

int gp_copy_cards(card_t* src, card_t* dest, int offset, int rank, int num)
{
    int i,n;

    if (!src || !dest || num == 0)
        return 0;
    if (offset < 0 || offset >= GP_MAX_CARDS)
        return 0;
    if (rank == 0)
        return 0;

    n = 0;
    /* 从小到大查找 */
    for (i = GP_MAX_CARDS - 1; i >= 0; --i) {
        if (src[i].rank == rank) {
            memcpy(dest+offset+n, src+i, sizeof(card_t));
            n++;
            if (n >= num)
                break;
        }
    }
	return n;
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
