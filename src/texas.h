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
#define TEXAS_MAX_PLAYER   9

typedef enum texas_player_state_e{
    TPS_START,
    TPS_FOLD,       /* fold */
    TPS_FOLLOW,     /* follow */
    TPS_ALLIN       /* all in */
}TEXAS_PLAYER_STATE;

/* Texas hold'em Poker card type */
typedef enum texas_type_e{
        TEXAS_HIGHCARD = 0,     /* high card 67523 */
        TEXAS_PAIR1,            /* one pair 66893 */
        TEXAS_PAIR2,            /* two pair 66889 */
        TEXAS_THREE,            /* three of a kind 666+89*/
        TEXAS_STRAIGHT,         /* straight (56789) */
        TEXAS_FLUSH,            /* double straight 3+ (S6,S8,S9,SA,SQ) */
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
    TEXAS_GAME_RIVER
}TEXAS_GAMESTATE;

typedef struct texas_s{
    int debug;          /* output debug info */
    deck_t* deck;       /* deck */
    int game_state;     /* game state */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
    int b_burn;         /* is burn card */

    int dealer_player_no;   /* button(banker) player no. */
    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    hand_type board_htype;  /* board hand type */
    card_t board[5];        /* five card of board */

    int player_num;
    card_player_t players[TEXAS_MAX_PLAYER];   /* three player */
    int player_state[TEXAS_MAX_PLAYER];
}texas_t;

texas_t* texas_new();
void texas_free(texas_t* texas);
void texas_start(texas_t* texas);      /* start a new game */
void texas_set_burn(texas_t* texas, int burn);
int texas_get_state(texas_t* texas);
void texas_set_state(texas_t* texas, int state);
void texas_sort(hand_t* hand);
void texas_handtype(hand_t* hand, hand_type* htype);
void texas_next_player(texas_t* texas);
int texas_count_notfolded(texas_t* texas);
void texas_get_folp(texas_t* texas, card_t* c1, card_t* c2, card_t* c3);
void texas_get_turn(texas_t* texas, card_t* card);
void texas_get_river(texas_t* texas, card_t* card);
int texas_fold(texas_t* texas, int player_no);

#ifdef __cplusplus
}
#endif
#endif
