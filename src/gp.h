/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _GP_H
#define _GP_H
#ifdef __cplusplus
extern "C" {
#endif

#include "deck.h"
#include "card_player.h"

#define GP_MAX_PLAYER   3

/* gp card type */
typedef enum gp_type_e{
        GP_ERROR = 0,
        GP_SINGLE,      /* single */
        GP_DOUBLE,      /* double */
        GP_THREE,       /* three 666 */
        GP_STRAIGHT,    /* straight 5+ (45678) */
        GP_D_STRAIGHT,  /* double straight 3+ (334455) */
        GP_T_STRAIGHT,  /* three straight 2+ (333444) */
        GP_THREE_P1,    /* three plus single (last hand) */
        GP_THREE_P2,    /* three plus double (333+44) */
		GP_PLANE,       /* three straight plus double straight (333444+7788) */
        GP_FOUR_P3,     /* four plus three (6666+883) */
        GP_BOMB         /* bomb (6666+3), (KKK+3)*/       
}GP_TYPE;

typedef enum gp_gamestate_e{
    GP_GAME_END = 0,   /* game end */
    GP_GAME_PLAY       /* playing card */
}GP_GAMESTATE;

typedef struct gp_s{
    int debug;          /* output debug info */
    deck_t* deck;       /* deck */
    int game_state;     /* game state */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;

    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    hand_t* last_hand;      /* last play out cards */
    hand_type last_htype;   /* last play out cards type */

    int player_num;
    card_player_t players[GP_MAX_PLAYER];   /* three player */
}gp_t;

gp_t* gp_new();
void gp_free(gp_t* gp);
void gp_start(gp_t* gp);      /* start a new game */
int gp_get_state(gp_t* gp);
void gp_set_state(gp_t* gp, int state);
void gp_sort(hand_t* hand);
int gp_play(gp_t* gp, int player_no, hand_t* hand);
int gp_canplay(gp_t* gp, hand_t* hand, hand_type* cdtype);
void gp_next_player(gp_t* gp);
int gp_pass(gp_t* gp, int player_no);
void gp_dump(gp_t* gp);

#ifdef __cplusplus
}
#endif
#endif
