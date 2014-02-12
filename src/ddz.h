/*
*
* code:huangtao117@gmail.com
*
*/
#ifndef _DDZ_H
#define _DDZ_H
#ifdef __cplusplus
extern "C" {
#endif

#include "deck.h"
#include "card_player.h"

#define DDZ_MAX_PLAYER  3

/* ddz card type */
typedef enum ddz_type_e{
    DDZ_ERROR = 0,
    DDZ_SINGLE,     /* single */
    DDZ_DOUBLE,     /* double */
    DDZ_THREE,      /* three 666 */
    DDZ_STRAIGHT,   /* straight 5+ (45678) */
    DDZ_D_STRAIGHT, /* double straight 3+ (334455) */
    DDZ_T_STRAIGHT, /* three straight 2+ (333444) */
    DDZ_THREE_P1,   /* three plus single (333+4) */
    DDZ_THREE_P2,   /* three plus double (333+44) */
    DDZ_FOUR_2S,    /* four plsu two single (3333+4+5) */
    DDZ_FOUR_2D,    /* four plus two double (3333+44+55) */
    DDZ_BOMB,       /* bomb */
    DDZ_ATOM        /* atom bomb */         
}DDZ_TYPE;

typedef enum ddz_gamestate_e{
    DDZ_GAME_END = 0,   /* game end */
    DDZ_GAME_CALL,      /* call landlord */
    DDZ_GAME_PLAY       /* playing card */
}DDZ_GAMESTATE;

typedef struct ddz_s{
    int debug;          /* output debug info */
    deck_t* deck;       /* deck */
    int game_state;     /* game state */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
    int landlord_win;   /* is landlord win */

    int landlord_no;        /* landloard no. */
    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    hand_t* last_hand;      /* last play out cards */
    hand_type last_htype;   /* last play out cards type */
    card_t di[3];           /* three card of landloard */

    int call[DDZ_MAX_PLAYER];                /* call fen */
    card_player_t players[DDZ_MAX_PLAYER];   /* three player */
}ddz_t;

ddz_t* ddz_new();
void ddz_free(ddz_t* ddz);
void ddz_start(ddz_t* ddz);      /* start a new game */
int ddz_get_state(ddz_t* ddz);
void ddz_set_state(ddz_t* ddz, int state);
void ddz_sort(hand_t* coll);
const char* ddz_htype_name(hand_type* htype);
void ddz_handtype(hand_t* hand, hand_type* htype);
int ddz_play(ddz_t* ddz, int player_no, hand_t* hand);
int ddz_canplay(ddz_t* ddz, hand_t* hand, hand_type* htype);
void ddz_next_player(ddz_t* ddz);
int ddz_call(ddz_t* ddz, int call_no, int fen);
int ddz_pass(ddz_t* ddz, int player_no);
void ddz_dump(ddz_t* ddz);

#ifdef __cplusplus
}
#endif
#endif
