/*
*
* code:huangtao117@gmail.com
* QQ:409577078
*
*/
#ifndef _TEXAS_H
#define _TEXAS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "card.h"

/* 2+ usually 2-9 */
#define TEXAS_MAX_PLAYER    8
/* deck card number */
#define TEXAS_DECK_NUM      52
#define TEXAS_MAX_CARDS     7

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

/* texas type */
typedef struct texas_type_s{
    int name;           /* type enum */
    int param1;	        /* logic value */
    int param2;
}texas_type;

/* define a texas player */
typedef struct texas_player_s{
    int state;
    uint64_t score;
    uint64_t gold;
    int param1;
    int param2;
   
    /* player's cards */
    int card_num;
    card_t mycards[TEXAS_MAX_CARDS];

    /* result */
    texas_type mytype;
    card_t mybest[5];
}texas_player_t;

typedef struct texas_pot_s{
    uint64_t total_chip;
    uint64_t player_chip[TEXAS_MAX_PLAYER];
    int win_flag[TEXAS_MAX_PLAYER];
}texas_pot_t;

/**
 * texas game core 
 */
typedef struct texas_s{
    int debug;          /* output debug info */
    int game_state;     /* game state */
    int last_state;     /* last game state */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
    int b_burn;         /* is burn card */
    int b_jump_end;     /* live player all allin need jump to end */
    int player_num;
    int curr_poti;

    /* texas not include joker */
    int deal_index;
    card_t deck[TEXAS_DECK_NUM];

    int dealer_player_no;   /* button(banker) player no. */
    int small_blind_no;
    int big_blind_no;
    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    uint64_t turn_max_chip;
    unsigned int small_blind;    /* small blind */
    unsigned int min_raise;      /* minimum raise(big blind or last bet) */

    card_t board[5];        /* five card of board */

    texas_pot_t pots[TEXAS_MAX_PLAYER];         /* pot and side-pot */
    texas_player_t players[TEXAS_MAX_PLAYER];
}texas_t;

/**
 * initialize a texas game core
 */
void texas_init(texas_t* texas);

/**
 * start game
 */
void texas_start(texas_t* texas);

/**
 * end game 
 */
void texas_end(texas_t* texas);

/**
 * set game smalll blind
 */
void texas_set_blind(texas_t* texas, unsigned int chip);

/**
 * get player's bet chip from pot
 */
uint64_t texas_player_bet(texas_t* texas, int player_no);

/**
 * get player total win
 * @texas The texas game handle
 * @player_no The player no
 */
uint64_t texas_player_win(texas_t* texas, int player_no);

/**
 * get player win from special pot
 * @texas The texas game handle
 * @pot_index The chip pot index
 * @player_no The player no
 */
uint64_t texas_pot_win(texas_t* texas, int pot_index, int player_no);

/**
 * get player call need chip
 */
uint64_t texas_call_need_chip(texas_t* texas, int player_no);

/**
 * fold action
 */
int texas_fold(texas_t* texas, int player_no);

/**
 * bet action
 * return bet chip
 */
uint64_t texas_bet(texas_t* texas, int player_no, unsigned int chip);

/**
 * call action
 * return call chip
 */
uint64_t texas_call(texas_t* texas, int player_no);

/**
 * check action
 */
int texas_check(texas_t* texas, int player_no);

/**
 * raiseto action
 * return out chip (call+raise)
 * @chip : raise to this value
 */
uint64_t texas_raiseto(texas_t* texas, int player_no, unsigned int chip);

/**
 * raise action
 * return out chip (call+raise)
 * @chip : raise value
 */
uint64_t texas_raise(texas_t* texas, int player_no, unsigned int chip);

/**
 * allin action
 * return out chip
 */
uint64_t texas_allin(texas_t* texas, int player_no);

/**
 * calc player's texas type and best hand
 */
void texas_calc_type(texas_t* texas, int player_no);

/**
 * game process
 */
void texas_next_step(texas_t* texas);

/**
 * when a step start, init some data
 */
void texas_step_init(texas_t* texas);

/**
 * get live player(not fold)
 */
int texas_live_num(texas_t* texas);

/**
 * split pot(if need)
 */
int texas_pot_split(texas_t* texas);

/**
 * get card logic value
 */
int texas_logicvalue(card_t* card);

/**
 * get card rank from logic value
 */
int texas_rankvalue(int logic_value);

#ifdef __cplusplus
}
#endif
#endif
