#include "texas.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sort_card.h"

#define DECK_FU     1
#define MAX_CARDS   5

static int texas_table_rank[16] = { 0, 14, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0 };
static int texas_table_logic[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1, 0 };
static int table_suit[6] = { 0, 1, 2, 3, 4, 5 };
static const int straight[10] = { 7936, 3968, 1984, 992, 496, 248, 124, 62, 31, 4111 };

texas_t* texas_new()
{
    int i,j;
    texas_t* texas;

    texas = (texas_t*)malloc(sizeof(texas_t));
    if(!texas)
        return 0;
    texas->b_burn = 1;
    texas_set_blind(texas, 1);
    texas->deck = deck_new(DECK_FU, 0);
    if(!texas->deck){
        texas_free(texas);
        return 0;
    }
    texas->debug = 0;
    texas->game_state = TEXAS_GAME_END;
    texas->inning = 0;
    texas->turn_time = 30;
    texas->curr_turn_time = 30;
    texas->dealer_player_no = 0;
    texas->small_blind_no = 0;
    texas->big_blind_no = 0;
    texas->first_player_no = 0;
    texas->curr_player_no = 0;
    texas->largest_player_no = 0;
    texas->curr_poti = 0;

    for(i = 0; i < TEXAS_MAX_PLAYER; i++){
        card_player_init(&(texas->players[i]), MAX_CARDS);
        texas->players[i].valid = 0;
        texas->players[i].state = 0;
        texas->players[i].position = i;
        texas->players[i].gold = 0;
        texas->players[i].level = 0;
        texas->players[i].score = 0;
        texas->players[i].param1 = 0;
        texas->players[i].param2 = 0;
        texas->pots[i].total_chip = 0;
        texas->pots[i].locked = 0;
        for(j = 0; j < TEXAS_MAX_PLAYER; j++){
            texas->pots[i].player_chip[j] = 0;
            texas->pots[i].win_flag[j] = 0;
        }
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
    int i,j,k;
    card_t card;

    if(!texas)
        return;
    if(texas->player_num < 2)
        return;
    if(texas->small_blind <= 0)
        return;
    for(i = 0;i < texas->player_num; i++){
        if(texas->players[i].gold < (texas->small_blind * 2))
            return;
    }

    deck_shuffle(texas->deck);
    texas->round = 0;
    texas->curr_poti = 0;
    texas->game_state = TEXAS_GAME_PREFLOP;
    for(i = 0; i < TEXAS_MAX_PLAYER; ++i){
        if(i < texas->player_num){
            texas->players[i].valid = 1;
            texas->players[i].state = PLAYER_ACTION_WAIT;
        }
        else{
            texas->players[i].valid = 0;
        }
        texas->pots[i].total_chip = 0;
        texas->pots[i].locked = 0;
        for(j = 0; j < TEXAS_MAX_PLAYER; ++j){
            texas->pots[i].player_chip[j] = 0;
            texas->pots[i].win_flag[j] = 0;
        }
        card_player_reset(&(texas->players[i]));
    }

    /* the button position */
    if(!texas->inning)
        texas->dealer_player_no = rand() % texas->player_num;
    else{
        texas->dealer_player_no++;
        if(texas->dealer_player_no >= texas->player_num)
            texas->dealer_player_no = 0;
    }
    if(texas->player_num > 2){
        texas->small_blind_no = texas->dealer_player_no + 1;
        if(texas->small_blind_no >= texas->player_num)
            texas->small_blind_no = 0;
    }
    else{
        /* heads-up(1v1) */
        texas->small_blind_no = texas->dealer_player_no;
    }
    texas->big_blind_no = texas->small_blind_no + 1;
    if(texas->big_blind_no >= texas->player_num)
        texas->big_blind_no = 0;
    if(texas->player_num == 2)
        texas->first_player_no = texas->big_blind_no;
    else{
        texas->first_player_no = texas->big_blind_no + 1;
        if(texas->first_player_no >= texas->player_num)
            texas->first_player_no = 0;
    }
    texas->curr_player_no = texas->first_player_no;

    /* about chip */
    texas->turn_max_chip = texas->small_blind * 2;
    texas->pots[0].total_chip = 3 * texas->small_blind;
    texas->pots[0].player_chip[texas->small_blind_no] = texas->small_blind;
    texas->pots[0].player_chip[texas->big_blind_no] = texas->small_blind * 2;
    texas->turn_max_chip = texas->small_blind * 2;

    /* draw two cards for every player */
    for(i = 0; i < 2; ++i){
        k = texas->dealer_player_no + 1;
        if(k >= texas->player_num)
            k = 0;
        for(j = 0; j < texas->player_num; ++j){
            deck_deal(texas->deck, &card);
            card_player_draw(&(texas->players[k]), &card);
            k++;
            if(k >= texas->player_num)
                k = 0;
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

    texas->inning++;
}

void texas_set_burn(texas_t* texas, int burn)
{
    if(!texas)
        return;

    texas->b_burn = burn;
}

void texas_set_blind(texas_t* texas, unsigned int chip)
{
    if(!texas)
        return;

    texas->small_blind = chip;
    texas->min_raise = 2 * texas->small_blind;
}

uint64_t texas_get_chip(texas_t* texas, int player_no)
{
    int i;
    uint64_t chip;

    if(player_no >= texas->player_num)
        return 0;

    chip = 0;
    for(i = 0; i < texas->curr_poti; i++){
        chip += texas->pots[i].player_chip[player_no];
    }

    return chip;
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

int texas_card_compare(const void* a, const void* b)
{
    card_t *card1, *card2;

    card1 = (card_t*)a;
    card2 = (card_t*)b;
    if(!card1 || !card2)
        return 0;

    if(texas_table_rank[card1->rank] < texas_table_rank[card2->rank])
        return 1;

    if(texas_table_rank[card1->rank] > texas_table_rank[card2->rank])
        return -1;

    if(texas_table_rank[card1->rank] == texas_table_rank[card2->rank]){
        if(table_suit[card1->suit] < table_suit[card2->suit])
            return 1;
        if(table_suit[card1->suit] > table_suit[card2->suit])
            return -1;

        return 0;
    }

    return 0;
}

void texas_sort(hand_t* hand)
{
    if(!hand || !hand->cards)
        return;

    qsort(hand->cards, hand->num, sizeof(card_t), texas_card_compare);
}

void texas_group(texas_t* texas, int player_no, hand_t* hand)
{
    int i,num;
    hand_t* h;
    card_t* p;

    if(!texas || !hand)
        return;
    if(player_no >= TEXAS_MAX_PLAYER)
        return;

    hand_zero(hand);
    h = texas->players[player_no].mycards;
    p = h->cards;
    for(i = 0; i < 2; i++){
        hand_push(hand, p);
        p++;
    }
    num = 0;
    if(texas->game_state == TEXAS_GAME_FLOP)
        num = 3;
    else if(texas->game_state == TEXAS_GAME_TURN)
        num = 4;
    else if(texas->game_state == TEXAS_GAME_RIVER)
        num = 5;
    for(i = 0; i < num; i++){
        hand_push(hand, &texas->board[i]);
    }
    texas_sort(hand);
}

int texas_handtype(hand_t* hand, hand_type* htype, hand_t* best_hand)
{
    int i,j,m,v;
    int sub[5];
    int x[20];
    int su_num[4];
    card_t *p,*p1;
    card_t tempc;
    card_t su_cards[4][7];

    if(!hand || !htype)
        return -1;
    if(hand->num < 2)
        return -2;
    if(hand->num > 7)
        return -3;
    if(best_hand){
        if(best_hand->max_size < 5)
            return -4;
        if(hand->num != 7)
            return -5;
    }

    htype->type = TEXAS_HIGHCARD;
    htype->param1 = 0;

    memset(su_cards, 0, sizeof(card_t) * 4 * 7);
    memset(su_num, 0, sizeof(int) * 4);
    memset(sub, 0, sizeof(int) * 5);

    p = hand->cards;
    for(i = 0; i < hand->num; ++i){
        su_cards[p->suit-1][su_num[p->suit-1]].rank = p->rank;
        su_cards[p->suit-1][su_num[p->suit-1]].suit = p->suit;
        su_num[p->suit-1]++;
        v = texas_logicvalue(p);
        x[v]++;
        p++;
    }

    /* Royal Straight Flush */
    /* Straight Flush */
    for(i = 0; i < 4; i++){
        if(su_num[i] < 5) continue;
        memset(x, 0, sizeof(int) * 20);
        for(j = 0; j < su_num[i]; ++j){
            v = texas_logicvalue(&su_cards[i][j]);
            x[v]++;
        }
        for(j = 14; j >= 6; j--){
            if(x[j] && x[j-1] && x[j-2] && x[j-3] && x[j-4]){
                if(best_hand){
                    p1 = best_hand->cards;
                    for(m = 0; m < 5; m++){
                        p1->rank = x[j+m];
                        p1->suit = i+1;
                        p1++;
                    }
                    best_hand->num = 5;
                }
                if(j == 14){
                    /* Royal Flush */
                    htype->type = TEXAS_ROYAL;
                    return TEXAS_ROYAL;
                }
                else{
                    htype->type = TEXAS_STRAIGHT_FLUSH;
                    htype->param1 = j;
                    return TEXAS_STRAIGHT_FLUSH;
                }
            }
        }
        /* Flush */
        htype->type = TEXAS_FLUSH;
        m = 0;
        for(j = 14; j >= 0; j--){
            if(x[j]){
                sub[m] = j;
                m++;
                if(m >= 5) break;
            }
        }
        htype->param1 = sub[0] << 16 | sub[1] << 12 |
            sub[2] << 8 | sub[3] << 4 | sub[4];
        if(best_hand){
            p1 = best_hand->cards;
            m = 0;
            for(j = 14; j >= 0; j--){
                if(x[j] == 0) continue;
                p1->rank = texas_rankvalue(j);
                p1->suit = i+1;
                m++;
                if(m >= 5) break;
                p1++;
            }
            best_hand->num = 5;
        }
        return TEXAS_FLUSH;
    }

    /* four of kind */
    memset(x, 0, sizeof(int) * 20);
    p = hand->cards;
    for(i = 0; i < hand->num; ++i){
        v = texas_logicvalue(p);
        x[v]++;
        p++;
    }
    for(i = 0; i < 15; i++){
        if(x[i] == 4){
            sub[0] = i;
            htype->type = TEXAS_FOUR;
            p = hand->cards;
            sub[1] = 0;
            for(j = 0; j < hand->num; ++j){
                v = texas_logicvalue(p);
                p++;
                if(v == i) continue;
                if(v > sub[1]){
                    sub[1] = v;
                    tempc.rank = p->rank;
                    tempc.suit = p->suit;
                }
            }
            htype->param1 = sub[0] << 4 | sub[1];
            if(best_hand){
                p1 = best_hand->cards;
                for(j = 0; j < 4; j++){
                    p1->rank = texas_rankvalue(i);
                    p1->suit = j + 1;
                    p1++;
                }
                p1->rank = tempc.rank;
                p1->suit = tempc.suit;
                best_hand->num = 5;
            }
            return TEXAS_FOUR;
        }
    }

    /* straight */
    for(j = 14; j >= 6; j--){
        if(x[j] && x[j-1] && x[j-2] && x[j-3] && x[j-4]){
            htype->type = TEXAS_STRAIGHT;
            htype->param1 = j;
            if(best_hand){
                p1 = best_hand->cards;
                for(m = 0; m < 5; m++){
                    p = hand->cards;
                    for(i = 0; i < hand->num; i++){
                        if((j-m) == texas_logicvalue(p)){
                            p1->rank = p->rank;
                            p1->suit = p->suit;
                            p1++;
                            break;
                        }
                        p++;
                    }
                }
                best_hand->num = 5;
            }
            return TEXAS_STRAIGHT;
        }
    }

    /* full house,three */
    sub[0] = 0;
    for(i = 14; i >= 0; i--){
        if(x[i] == 3){
            sub[0] = i;
            break;
        }
    }
    sub[1] = 0;
    for(i = 14; i >= 0; i--){
        if(x[i] == 2 && i > sub[1]){
            sub[1] = i;
        }
    }
    if(sub[0] && sub[1]){
        htype->type = TEXAS_FULLHOUSE;
        htype->param1 = sub[0] << 4 | sub[1];
        if(best_hand){
            p = hand->cards;
            p1 = best_hand->cards;
            for(j = 0; j < hand->num; j++){
                v = texas_logicvalue(p);
                if(sub[0] == v || sub[1] == v){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                }
                p++;
            }
            best_hand->num = 5;
        }
        return TEXAS_FULLHOUSE;
    }
    if(sub[0] > 0 && sub[1] == 0){
        htype->type = TEXAS_THREE;
        m = 0;
        sub[1] = sub[2] = 0;
        for(i = 14; i >= 0; i--){
            if(x[i] == 1){
                if(m == 0)
                    sub[1] = i;
                else
                    sub[2] = i;
                m++;
                if(m == 2)
                    break;
            }
        }
        htype->param1 = sub[0] << 8 | sub[1] << 4 | sub[2];
        if(best_hand){
            p = hand->cards;
            p1 = best_hand->cards;
            for(j = 0; j < hand->num; j++){
                v = texas_logicvalue(p);
                if(v == sub[0] || v == sub[1] || v == sub[2]){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                }
                p++;
            }
            best_hand->num = 5;
        }
        return TEXAS_THREE;
    }

    /* pair */
    j = 0;
    memset(sub, 0, sizeof(int) * 5);
    for(i = 14; i >= 0; i--){
        if(x[i] == 2){
            j++;
            if(sub[0] == 0) sub[0] = i;
            else sub[1] = i;
            if(j == 2)
                break;
        }
    }
    if(j == 2){
        htype->type = TEXAS_PAIR2;
        sub[2] = 0;
        for(i = 14; i >= 0; i--){
            if(x[i] == 1){
                sub[2] = i;
                break;
            }
        }
        htype->param1 = sub[0] << 8 | sub[1] << 4 | sub[2];
        if(best_hand){
            p = hand->cards;
            p1 = best_hand->cards;
            for(m = 0; m < hand->num; m++){
                v = texas_logicvalue(p);
                if(sub[0] == v || sub[1] == v || sub[2] == v){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                }
                p++;
            }
            best_hand->num = 5;
        }
        return TEXAS_PAIR2;
    }
    if(j == 1){
        htype->type = TEXAS_PAIR1;
        m = 0;
        for(i = 19; i >= 0; i--){
            if(x[i] == 1){
                if(m == 0)
                    sub[1] = i;
                else if(m == 1)
                    sub[2] = i;
                else
                    sub[3] = i; 
                m++;
                if(m == 3)
                    break;
            }
        }
        htype->param1 = sub[0] << 12 | sub[1] << 8 | sub[2] << 4 | sub[3];
        if(best_hand){
            p = hand->cards;
            p1 = best_hand->cards;
            v = 0;
            for(j = 0; j < hand->num; j++){
                v = texas_logicvalue(p);
                if(v == sub[0] || v == sub[1] || v == sub[2] || v == sub[3]){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    v++;
                    p1++;
                    if(v == 2) break;
                }
                p++;
            }
            best_hand->num = 5;
        }
        return TEXAS_PAIR1;
    }

    /* high card */
    htype->type = TEXAS_HIGHCARD;
    m = 0;
    memset(sub, 0, sizeof(int) * 5);
    for(i = 14; i >= 0; i--){
        if(x[i] == 1){
            if(m == 0)
                sub[0] = i;
            else if(m == 1)
                sub[1] = i;
            else if(m == 2)
                sub[2] = i;
            else if(m == 3)
                sub[3] = i;
            else if(m == 4)
                sub[4] = i;
            m++;
            if(m == 5)
                break;
        }
    }
    htype->param1 = sub[0] << 16 | sub[1] << 12 | sub[2] << 8 |
        sub[3] << 4 | sub[4];
    if(best_hand){
        p = hand->cards;
        p1 = best_hand->cards;
        m = 0;
        for(i = 0; i < hand->num; i++){
            v = texas_logicvalue(p);
            if(v == sub[0] || v == sub[1] || v == sub[2]
            || v == sub[3] || v == sub[4]){
                p1->rank = p->rank;
                p1->suit = p->suit;
                v++;
                p1++;
                if(v == 2) break;
            }
            p++;
            m++;
            if(m == 5) break;
        }
        best_hand->num = 5;
    }

    return TEXAS_HIGHCARD;
}

void texas_end(texas_t* texas)
{
    int i,j,n,cmpn,flag;
    hand_type hmax;
    hand_type htype[TEXAS_MAX_PLAYER];
    static hand_t* best;

    if(!texas)
        return;

    if(!best){
        best = hand_new(5);
    }

    cmpn = 0;
    for(i = 0; i < texas->player_num; i++){
        if(texas->players[i].state != PLAYER_ACTION_FOLD)
            cmpn++;
    }

    if(texas->game_state >= TEXAS_GAME_RIVER && cmpn > 1){
        hmax.type = TEXAS_HIGHCARD;
        hmax.param1 = 0;
        for(i = 0; i <= texas->curr_poti; i++){
            /* get pot max */
            for(j = 0; j < texas->player_num; j++){
                if(texas->pots[i].player_chip > 0 &&
                    texas->players[i].state != PLAYER_ACTION_FOLD){
                        texas_handtype(texas->players[i].mycards, &htype[j], best);
                        memcpy(texas->best[j], best, sizeof(card_t) * 5);
                        flag = 0;
                        if(htype[i].type > hmax.type)
                            flag = 1;
                        else if(htype[i].type == hmax.type){
                            if(htype[i].param1 > hmax.param1)
                                flag = 1;
                        }
                        if(flag){
                            memcpy(&hmax, &htype[i], sizeof(hand_type));
                        }
                }
            }
            /* flag winner */
            n = 0;
            for(j = 0; j < texas->player_num; j++){
                if(texas->pots[i].player_chip > 0 &&
                    texas->players[i].state != PLAYER_ACTION_FOLD){
                        if(htype[j].type == hmax.type &&
                            htype[i].param1 == hmax.param1){
                                texas->pots[i].win_flag[j] = 1;
                                n++;
                        }
                }
            }
            for(j = 0; j < texas->player_num; j++){
                if(texas->pots[i].win_flag[j] > 0){
                    texas->players[j].gold += texas->pots[i].total_chip / n;
                }
            }
        }
    }
    else{
        for(i = 0; i < texas->player_num; i++){
            if(texas->players[i].state != PLAYER_ACTION_FOLD){
                texas->pots[0].win_flag[i] = 1;
                texas->players[i].gold += texas->pots[0].total_chip;
            }
        }
    }
}

int texas_compare(hand_type* a, hand_type* b)
{
    int x1,y1;

    if(!a || !b)
        return 0;

    if(a->type > b->type)
        return 1;
    if(a->type < b->type)
        return -1;

    if(a->type == TEXAS_STRAIGHT_FLUSH || a->type == TEXAS_FLUSH ||
        a->type == TEXAS_STRAIGHT){
            if(a->param1 > b->param1)
                return 1;
            else if(a->param1 < b->param1)
                return -1;
            return 0;
    }

    if(a->type == TEXAS_FOUR || a->type == TEXAS_FULLHOUSE){
        if(a->param1 > b->param1)
            return 1;
        else if(a->param1 < b->param1)
            return -1;

        if(a->param2 > b->param2)
            return 1;
        else if(a->param2 > b->param2)
            return -1;
        return 0;
    }
        
    if(a->type == TEXAS_THREE || a->type == TEXAS_PAIR2){
        if(a->param1 > b->param1)
            return 1;
        else if(a->param1 < b->param1)
            return -1;

        if(a->param2 > b->param2)
            return 1;
        else if(a->param2 > b->param2)
            return -1;

        if(a->param3 > b->param3)
            return 1;
        else if(a->param3 > b->param3)
            return -1;
        return 0;
    }
    
    if(a->type == TEXAS_PAIR1){
        if(a->param1 > b->param1)
            return 1;
        else if(a->param1 < b->param1)
            return -1;

        if(a->param2 > b->param2)
            return 1;
        else if(a->param2 < b->param2)
            return -1;

        x1 = a->param3 >> 8;
        y1 = b->param3 >> 8;
        if(x1 > y1)
            return 1;
        else if(x1 < y1)
            return -1;

        x1 = a->param3 & 0xFF;
        y1 = a->param3 & 0xFF;
        if(x1 > y1)
            return 1;
        else if(x1 < y1)
            return -1;

        return 0;
    }
    if(a->type == TEXAS_HIGHCARD){
        if(a->param1 > b->param1)
            return 1;
        else if(a->param1 < b->param1)
            return -1;

        x1 = a->param2 >> 8;
        y1 = b->param2 >> 8;
        if(x1 > y1)
            return 1;
        else if(x1 < y1)
            return -1;

        x1 = a->param2 & 0xFF;
        y1 = a->param2 & 0xFF;
        if(x1 > y1)
            return 1;
        else if(x1 < y1)
            return -1;

        x1 = a->param3 >> 8;
        y1 = b->param3 >> 8;
        if(x1 > y1)
            return 1;
        else if(x1 < y1)
            return -1;

        x1 = a->param3 & 0xFF;
        y1 = a->param3 & 0xFF;
        if(x1 > y1)
            return 1;
        else if(x1 < y1)
            return -1;

        return 0;        
    }

    return 0;
}

void texas_next_step(texas_t* texas)
{
    int i,flag,state,loop;
    int left_num;
    uint64_t chip;

    if(!texas)
        return;

    flag = 1;
    for(i = 0; i < texas->player_num; i++){
        if(texas->players[i].state == PLAYER_ACTION_ALLIN ||
            texas->players[i].state == PLAYER_ACTION_FOLD)
            continue;
        if(texas->players[i].state == PLAYER_ACTION_WAIT){
            flag = 0;
            break;
        }
        chip = texas->pots[texas->curr_poti].player_chip[i];
        if(chip != texas->turn_max_chip){
            flag = 0;
            break;
        }
    }
    if(flag){
        /* goto next game state */
        left_num = 0;
        texas->curr_player_no = texas->small_blind_no;
        for(i = 0; i < texas->player_num; i++){
            if(texas->players[i].state != PLAYER_ACTION_FOLD &&
                texas->players[i].state != PLAYER_ACTION_ALLIN){
                    left_num++;
                    texas->players[i].state = PLAYER_ACTION_WAIT;
            }
            if(texas->players[texas->curr_player_no].state == PLAYER_ACTION_FOLD ||
                texas->players[texas->curr_player_no].state == PLAYER_ACTION_ALLIN){
                    texas->curr_player_no++;
                    if(texas->curr_player_no >= texas->player_num)
                        texas->curr_player_no = 0;
            }
        }
        loop = TEXAS_MAX_PLAYER + 1;
        while(loop && texas_pot_split(texas)) loop--;
        if(left_num == 1 || texas->game_state == TEXAS_GAME_RIVER)
            texas->game_state = TEXAS_GAME_END;
        else
            texas->game_state++;
    }
    else{
        for(i = 0; i < texas->player_num; i++){
            texas->curr_player_no++;
            if(texas->curr_player_no >= texas->player_num)
                texas->curr_player_no = 0;
            state = texas->players[texas->curr_player_no].state;
            if(state == PLAYER_ACTION_FOLD || PLAYER_ACTION_ALLIN)
                continue;
            if(state == PLAYER_ACTION_CHECK &&
                texas->players[texas->curr_player_no].gold == 0)
                continue;
            break;
        }
    }    
}

int texas_count_notfolded(texas_t* texas)
{
    int i;
    int not_folded = 0;

    for(i = 0; i < texas->player_num; ++i){
        if(texas->players[i].state != PLAYER_ACTION_FOLD)
            not_folded++;
    }

    return not_folded;
}

int texas_pot_split(texas_t* texas)
{
    int split,i,j;
    uint64_t chip;
    uint64_t temp[TEXAS_MAX_PLAYER];

    split = 0;
    j = 0;
    chip = texas->pots[texas->curr_poti].player_chip[0];
    for(i = 0; i < texas->player_num; i++){
        if(texas->pots[texas->curr_poti].player_chip[i] > 0)
            j++;
        if(texas->pots[texas->curr_poti].player_chip[i] != chip){
            split = 1;
            if(texas->pots[texas->curr_poti].player_chip[i] < chip)
                chip = texas->pots[texas->curr_poti].player_chip[i];
        }
    }
    if(split){
        for(i = 0; i < TEXAS_MAX_PLAYER; i++){
            temp[i] = texas->pots[texas->curr_poti].player_chip[i];
            temp[i] -= chip;
            if(texas->pots[texas->curr_poti].player_chip[i] > 0)
                texas->pots[texas->curr_poti].player_chip[i] = chip;
        }
        texas->pots[texas->curr_poti].total_chip = j * chip;
        texas->pots[texas->curr_poti].locked = 1;
        texas->curr_poti++;
        if(texas->curr_poti >= TEXAS_MAX_PLAYER){
            printf("pot out of range when split!\n");
            return split;
        }
    }

    return split;
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

    texas->players[player_no].state = PLAYER_ACTION_FOLD;
    texas_next_step(texas);
    
    return 1;
}

int texas_bet(texas_t* texas, int player_no, unsigned int chip)
{
    if(!texas)
        return 0;
    if(player_no >= texas->player_num)
        return 0;
    if(player_no != texas->curr_player_no)
        return 0;
    if(texas->turn_max_chip > 0)
        return 0;
    if(chip > texas->players[player_no].gold)
        return 0;
    if(chip < (texas->small_blind * 2))
        return 0;

    texas->turn_max_chip = chip;
    texas->pots[texas->curr_poti].total_chip += chip;
    texas->pots[texas->curr_poti].player_chip[player_no] += chip;
    texas->players[player_no].gold -= chip;
    if(texas->players[player_no].gold == 0)
        texas->players[player_no].state = PLAYER_ACTION_ALLIN;
    else
        texas->players[player_no].state = PLAYER_ACTION_BET;

    texas_next_step(texas);
    
    return 1;
}

int texas_call(texas_t* texas, int player_no)
{
    if(!texas)
        return 0;
    if(player_no >= texas->player_num)
        return 0;
    if(player_no != texas->curr_player_no)
        return 0;
    if(texas->players[player_no].gold < texas->turn_max_chip)
        return 0;

    texas->pots[texas->curr_poti].total_chip += texas->turn_max_chip;
    texas->pots[texas->curr_poti].player_chip[player_no] += texas->turn_max_chip;
    texas->players[player_no].gold -= texas->turn_max_chip;
    if(texas->players[player_no].gold == 0)
        texas->players[player_no].state = PLAYER_ACTION_ALLIN;
    else
        texas->players[player_no].state = PLAYER_ACTION_CALL;

    texas_next_step(texas);

    return 1;
}

int texas_raise(texas_t* texas, int player_no, unsigned int chip)
{
    if(!texas)
        return 0;
    if(player_no >= texas->player_num)
        return 0;
    if(player_no != texas->curr_player_no)
        return 0;
    if(texas->turn_max_chip == 0)
        return 0;
    if(chip < texas->min_raise)
        return 0;
    if(chip > texas->players[player_no].gold)
        return 0;

    if(chip > texas->min_raise)
        texas->min_raise = chip;
    texas->turn_max_chip += chip;
    texas->pots[texas->curr_poti].total_chip += texas->turn_max_chip;
    texas->pots[texas->curr_poti].player_chip[player_no] += texas->turn_max_chip;
    texas->players[player_no].gold -= texas->turn_max_chip;
    if(texas->players[player_no].gold == 0)
        texas->players[player_no].state = PLAYER_ACTION_ALLIN;
    else
        texas->players[player_no].state = PLAYER_ACTION_RAISE;

    texas_next_step(texas);

    return 1;
}

int texas_allin(texas_t* texas, int player_no)
{
    if(!texas)
        return 0;
    if(player_no >= texas->player_num)
        return 0;
    if(player_no != texas->curr_player_no)
        return 0;
    if(texas->players[player_no].gold == 0)
        return 0;
    if(texas->turn_max_chip >= texas->players[player_no].gold)
        return 0;

    texas->turn_max_chip += texas->players[player_no].gold;
    texas->pots[texas->curr_poti].total_chip += texas->turn_max_chip;
    texas->pots[texas->curr_poti].player_chip[player_no] += texas->turn_max_chip;
    texas->players[player_no].gold = 0;
    texas->players[player_no].state = PLAYER_ACTION_ALLIN;

    texas_next_step(texas);

    return 1;
}

int texas_logicvalue(card_t* card)
{
    if(!card)
        return 0;

    if(card->rank > cdRankK) return 0;

    return texas_table_rank[card->rank];
}

int texas_rankvalue(int logic_value)
{
    if(logic_value > 14 || logic_value < 0)
        return 0;
    return texas_table_logic[logic_value];
}