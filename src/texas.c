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

int texas_compare(const void* a, const void* b)
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

    qsort(hand->cards, hand->num, sizeof(card_t), texas_compare);
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

int texas_handtype(hand_t* hand, hand_type* htype, hand_t* result)
{
    int flag,i,j,m,v,suit;
    int x[20];
    int su_num[4];
    card_t *p,*p1;
    card_t su_cards[4][7];

    if(!hand || !htype || !result)
        return -1;
    if(hand->num < 2)
        return -2;
    if(hand->num > 7)
        return -3;

    htype->type = TEXAS_HIGHCARD;

    memset(su_cards, 0, sizeof(card_t) * 4 * 7);
    memset(su_num, 0, sizeof(int) * 4);

    p = hand->cards;
    for(i = 0; i < hand->num; ++i){
        su_cards[p->suit-1][su_num[p->suit-1]].rank = p->rank;
        su_cards[p->suit-1][su_num[p->suit-1]].suit = p->suit;
        su_num[p->suit-1]++;
        p++;
    }

    /* Royal Straight Flush */
    /* Straight Flush */
    for(i = 0; i < 4; i++){
        if(su_num[i] < 5) continue;
        memset(x, 0, sizeof(int) * 20);
        for(j = 0; j < su_num[i]; j++){
            v = texas_logicvalue(&su_cards[i][j]);
            x[v]++;
        }
        for(j = 14; j >= 6; j--){
            if(x[j] && x[j-1] && x[j-2] && x[j-3] && x[j-4]){
                if(j == 14)
                    htype->type = TEXAS_ROYAL;
                else
                    htype->type = TEXAS_STRAIGHT_FLUSH;
                htype->logic_value1 = j;
                htype->logic_value2 = i;
                if(hand->num >= 5){
                    p = result->cards;
                    for(m = 0; m < 5; m++){
                        p->rank = texas_rankvalue(j-m);
                        p->suit = i + 1;
                        p++;
                    }  
                    result->num = 5;
                }
                return TEXAS_ROYAL;
            }
        }
    }

    memset(x, 0, sizeof(int) * 20);
    p = hand->cards; 
    for(i = 0; i < hand->num; i++){
        v = texas_logicvalue(p);
        x[v]++;
        p++;
    }

    /* four of kind */
    for(i = 0; i < 20; i++){
        if(x[i] == 4){
            htype->type = TEXAS_FOUR;
            htype->logic_value1 = i;
            htype->logic_value2 = 0;
            for(j = 19; j >= 0; j--){
                if(j != i && x[j] == 1){
                    htype->logic_value2 = j;
                    break;
                }
            }
            if(hand->num >= 5){
                p = hand->cards;
                p1 = result->cards;
                m = 0;
                for(j = 0; j < hand->num; j++){
                    if(htype->logic_value1 == texas_logicvalue(p)){
                        p1->rank = p->rank;
                        p1->suit = p->suit;
                        p1++;
                        m++;
                        if(m == 4) break;
                    }
                    p++;
                }
                p = hand->cards;
                for(j = 0; j < hand->num; j++){
                    if(htype->logic_value2 == texas_logicvalue(p)){
                        p1->rank = p->rank;
                        p1->suit = p->suit;
                        break;
                    }
                    p++;
                }
                result->num = 5;
            }
            return TEXAS_FOUR;
        }
    }

    /* full house */
    htype->logic_value1 = 0;
    htype->logic_value2 = 0;
    for(i = 19; i >= 0; i--){
        if(x[i] == 3){
            htype->logic_value1 = i;
            break;
        }
    }
    for(i = 19; i >= 0; i--){
        if(x[i] == 2){
            htype->logic_value2 = i;
            break;
        }
    }
    if(htype->logic_value1 && htype->logic_value2){
        htype->type = TEXAS_FULLHOUSE;
        if(hand->num >= 5){
            p = hand->cards;
            p1 = result->cards;
            for(j = 0; j < hand->num; j++){
                if(htype->logic_value1 == texas_logicvalue(p)){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                }
                p++;
            }
            p = hand->cards;
            for(j = 0; j < hand->num; j++){
                if(htype->logic_value2 == texas_logicvalue(p)){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                }
                p++;
            }
            result->num = 5;
        }
        return TEXAS_FULLHOUSE;
    }

    /* flush(same suit) */
    for(i = 0; i < 4; i++){
        if(su_num[i] >= 5){
            htype->type = TEXAS_FLUSH;
            htype->logic_value1 = texas_logicvalue(&su_cards[i][0]);
            htype->logic_value2 = 
                (texas_logicvalue(&su_cards[i][1]) << 8) & 
                texas_logicvalue(&su_cards[i][2]);
            htype->logic_value3 = 
                (texas_logicvalue(&su_cards[i][3]) << 8) &
                texas_logicvalue(&su_cards[i][4]);

            if(hand->num >=5){
                p = result->cards;
                for(j = 0; j < 5; j++){
                    p->rank = su_cards[i][j].rank;
                    p->suit = su_cards[i][j].suit;
                    p++;
                }
                result->num = 5;
            }
            return TEXAS_FLUSH;
        }
    }

    /* straight */
    for(j = 14; j >= 6; j--){
        if(x[j] && x[j-1] && x[j-2] && x[j-3] && x[j-4]){
            htype->type = TEXAS_STRAIGHT;
            htype->logic_value1 = j;
            p1 = result->cards;
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
            result->num = 5;
            return TEXAS_STRAIGHT;
        }
    }

    /* three */
    for(i = 19; i >= 0; i--){
        if(x[i] == 3){
            htype->type = TEXAS_THREE;
            htype->logic_value1 = i;
            htype->logic_value2 = 0;
            htype->logic_value3 = 0;
            for(j = 19; j >= 0; j--){
                if(j != i && x[j] == 1){
                    if(htype->logic_value2 == 0)
                        htype->logic_value2 = j;
                    else if(htype->logic_value3 == 0)
                        htype->logic_value3 = j;
                }
            }
            if(hand->num >=5 ){
                p = hand->cards;
                p1 = result->cards;
                m = 0;
                for(j = 0; j < hand->num; j++){
                    if(htype->logic_value1 == texas_logicvalue(p)){
                        m++;
                        p1->rank = p->rank;
                        p1->suit = p->suit;
                        p1++;
                        if(m == 3) break;
                    }
                    p++;
                }
                p = hand->cards;
                for(j = 0; j < hand->num; j++){
                    if(htype->logic_value2 == texas_logicvalue(p)){
                        p1->rank = p->rank;
                        p1->suit = p->suit;
                        p1++;
                        break;
                    }
                    p++;
                }
                p = hand->cards;
                for(j = 0; j < hand->num; j++){
                    if(htype->logic_value3 == texas_logicvalue(p)){
                        p1->rank = p->rank;
                        p1->suit = p->suit;
                        p1++;
                        break;
                    }
                    p++;
                }
                result->num = 5;
            }
            return TEXAS_THREE;
        }
    }    

    /* pair */
    j = 0;
    htype->logic_value1 = 0;
    htype->logic_value2 = 0;
    for(i = 19; i >= 0; i--){
        if(x[i] == 2){
            j++;
            if(htype->logic_value1 == 0)
                htype->logic_value1 = i;
            else if(htype->logic_value2 == 0)
                htype->logic_value2 = i;
        }
    }
    if(j == 2){
        for(i = 19; i >= 0; i--){
            if(x[i] == 1){
                htype->logic_value3 = i;
                break;
            }
        }
        htype->type = TEXAS_PAIR2;
        if(hand->num >= 5){
            p = hand->cards;
            p1 = result->cards;
            v = 0;
            for(m = 0; m < hand->num; m++){
                if(htype->logic_value1 == texas_logicvalue(p)){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                    v++;
                    if(v == 2) break;
                }
                p++;
            }
            p = hand->cards;
            v = 0;
            for(m = 0; m < hand->num; m++){
                if(htype->logic_value2 == texas_logicvalue(p)){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                    v++;
                    if(v == 2) break;
                }
                p++;
            }
            p = hand->cards;
            for(m = 0; m < hand->num; m++){
                v = texas_logicvalue(p);
                if(v != htype->logic_value1 && 
                    v != htype->logic_value2){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    break;
                }
                p++;
            }
            result->num = 5;
        }
        return TEXAS_PAIR2;
    }
    if(j == 1){
        htype->type = TEXAS_PAIR1;
        m = 0;
        for(i = 19; i >= 0; i--){
            if(x[i] == 1){
                m++;
                if(m == 1)
                    htype->logic_value2 = i;
                else if(m == 2)
                    htype->logic_value3 = i << 8;
                else if(m == 3)
                    htype->logic_value3 &= i;

                if(m == 3)
                    break;
            }
        }
        if(hand->num >= 5){
            p = hand->cards;
            p1 = result->cards;
            v = 0;
            for(j = 0; j < hand->num; j++){
                if(htype->logic_value1 == texas_logicvalue(p)){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    v++;
                    p1++;
                    if(v == 2) break;
                }
                p++;
            }
            p = hand->cards;
            v = 0;
            for(j = 0; j < hand->num; j++){
                if(htype->logic_value1 != texas_logicvalue(p)){
                    p1->rank = p->rank;
                    p1->suit = p->suit;
                    p1++;
                    v++;
                    if(v == 3) break;
                }
                p++;
            }
            result->num = 5;
        }
        return TEXAS_PAIR1;
    }

    m = 0;
    for(i = 19; i >= 0; i--){
        if(x[i] == 1){
            m++;
            if(m == 1)
                htype->logic_value1 = i;
            else if(m == 2)
                htype->logic_value2 = i << 8;
            else if(m == 3)
                htype->logic_value2 &= i;
            else if(m == 4)
                htype->logic_value3 = i << 8;
            else if(m == 5)
                htype->logic_value3 &= i;
            
            if(m == 5)
                break;
        }
    }
    if(hand->num >= 5){
        p = hand->cards;
        p1 = result->cards;
        m = 0;
        for(i = 0; i < hand->num; i++){
            p1->rank = p->rank;
            p1->suit = p->suit;
            p++;
            p1++;
            m++;
            if(m == 5) break;
        }
        result->num = 5;
    }

    return TEXAS_HIGHCARD;
}

void texas_next_player(texas_t* texas)
{
    if(!texas)
        return;
    texas->curr_player_no++;
    if(texas->curr_player_no >= texas->player_num)
        texas->curr_player_no = 0;
}

int texas_count_notfolded(texas_t* texas)
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