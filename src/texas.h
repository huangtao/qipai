/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _TEXAS_H
#define _TEXAS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "deck.h"
#include "card_player.h"

/* 2+ usually 2-9 */
#define TEXAS_MAX_PLAYER   8

typedef enum texas_player_action_e{
    PLAYER_ACTION_WAIT = 0,
    PLAYER_ACTION_FOLD,     /* give up */
    PLAYER_ACTION_CHECK,    /* pass(no add) */
    PLAYER_ACTION_CALL,     /* follow */
    PLAYER_ACTION_BET,
    PLAYER_ACTION_RAISE,
    PLAYER_ACTION_ALLIN
}TEXAS_PLAYER_ACTION;

/* Texas hold'em Poker card type */
typedef enum texas_type_e{
        TEXAS_HIGHCARD = 0,     /* high card 67523 */
        TEXAS_PAIR1,            /* one pair 66893 */
        TEXAS_PAIR2,            /* two pair 66889 */
        TEXAS_THREE,            /* three of a kind 666+89*/
        TEXAS_STRAIGHT,         /* straight (56789) */
        TEXAS_FLUSH,            /* same suit (S6,S8,S9,SA,SQ) */
        TEXAS_FULLHOUSE,        /* three plus double (666+88) */
		TEXAS_FOUR,             /* four of a kind (6666+8) */
        TEXAS_STRAIGHT_FLUSH,   /* strright flush (S6,S7,S8,S9,S10) */
        TEXAS_ROYAL             /* royal straight flush (SA,SK,SQ,SJ,S10) */       
}TEXAS_TYPE;

typedef enum texas_gamestate_e{
    TEXAS_GAME_END = 0,     /* game end */
    TEXAS_GAME_PREFLOP,
    TEXAS_GAME_FLOP,
    TEXAS_GAME_TURN,
    TEXAS_GAME_RIVER,
    TEXAS_GAME_POST_RIVER
}TEXAS_GAMESTATE;

typedef struct texas_pot_s{
    int locked;
    uint64_t total_chip;
    uint64_t player_chip[TEXAS_MAX_PLAYER];
    int win_flag[TEXAS_MAX_PLAYER];
}texas_pot_t;

typedef struct texas_s{
    int debug;          /* output debug info */
    deck_t* deck;       /* deck */
    int game_state;     /* game state */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
    int b_burn;         /* is burn card */
    int player_num;
    int curr_poti;

    int dealer_player_no;   /* button(banker) player no. */
    int small_blind_no;
    int big_blind_no;
    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    uint64_t turn_max_chip;
    unsigned int small_blind;    /* small blind */
    unsigned int min_raise;      /* minimum raise(big blind or last bet) */

    hand_type board_htype;  /* board hand type */
    card_t board[5];        /* five card of board */

    texas_pot_t pots[TEXAS_MAX_PLAYER];         /* pot and side-pot */
    card_player_t players[TEXAS_MAX_PLAYER];
    card_t best[TEXAS_MAX_PLAYER][5];
}texas_t;

texas_t* texas_new();
void texas_free(texas_t* texas);
void texas_init(texas_t* texas);
void texas_clear(texas_t* texas);
void texas_start(texas_t* texas);      /* start a new game */
void texas_end(texas_t* texas);
void texas_set_burn(texas_t* texas, int burn);
void texas_set_blind(texas_t* texas, unsigned int chip);
uint64_t texas_get_chip(texas_t* texas, int player_no);
int texas_fold(texas_t* texas, int player_no);
int texas_bet(texas_t* texas, int player_no, unsigned int chip);
int texas_call(texas_t* texas, int player_no);
int texas_check(texas_t* texas, int player_no);
int texas_raise(texas_t* texas, int player_no, unsigned int chip);
int texas_allin(texas_t* texas, int player_no);
int texas_get_state(texas_t* texas);
void texas_set_state(texas_t* texas, int state);
int texas_card_compare(const void* a, const void* b);
void texas_sort(hand_t* hand);
void texas_group(texas_t* texas, int player_no, hand_t* hand);
int texas_handtype(hand_t* hand, hand_type* htype, hand_t* best_hand);
int texas_compare(hand_type* a, hand_type* b);
void texas_next_step(texas_t* texas);
int texas_count_notfolded(texas_t* texas);
int texas_pot_split(texas_t* texas);
void texas_get_folp(texas_t* texas, card_t* c1, card_t* c2, card_t* c3);
void texas_get_turn(texas_t* texas, card_t* card);
void texas_get_river(texas_t* texas, card_t* card);
int texas_logicvalue(card_t* card);
int texas_rankvalue(int logic_value);

#ifdef __cplusplus
}
#endif
#endif
