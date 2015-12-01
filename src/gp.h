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

#define GP_MIN_PLAYER   2
#define GP_MAX_PLAYER   3
#define GP_MAX_CARDS    20
#define GP_DECK_FU      1

/* gp rule */
typedef enum gp_rule_e{
    GP_RULE_DEFAULT,     /* like QQ game's gp */
    GP_RULE_ZHUJI        /* zhejiang zhuji rule */
}GP_RULE;

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
        GP_FOUR,        /* four (6666) */
        GP_FOUR_P3,     /* four plus three (6666+883) */
        GP_BOMB         /* bomb (6666+3), (KKK+3)*/
}GP_TYPE;

typedef enum gp_gamestate_e{
    GP_GAME_END = 0,   /* game end */
    GP_GAME_PLAY       /* playing card */
}GP_GAMESTATE;

typedef enum gp_mode_e {
    GP_MODE_SERVER = 0,
    GP_MODE_CLIENT
}GP_MODE;

typedef struct gp_player_s {
    card_player_t data;
    card_t cards[GP_MAX_CARDS];
    int num_valid_card;
}GP_PLAYER;

typedef struct gp_s{
    int debug;          /* output debug info */
    int mode;           /* client or server mode */
    int game_state;     /* game state */
    int game_rule;      /* rule */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
    int player_num;
    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    card_t deck[54];        /* deck */
    int deck_all_num;
    int deck_deal_index;    /* current deal card index */
    int deck_valid_num;     /* valid number card */

    card_t last_hand[GP_MAX_CARDS];     /* last play out cards */
    GP_PLAYER players[GP_MAX_PLAYER];   /* three player */
}gp_t;

/* init a guanpai game object */
void gp_init(gp_t* gp, int rule, int mode, int player_num);

/* start a new game */
void gp_start(gp_t* gp);

int gp_get_state(gp_t* gp);
void gp_set_state(gp_t* gp, int state);
void gp_sort(hand_t* hand);
const char* gp_htype_name(int htype);
void gp_handtype(gp_t* gp, hand_t* hand);
int gp_play(gp_t* gp, int player_no, hand_t* hand);
int gp_canplay(gp_t* gp, hand_t* hand);
void gp_next_player(gp_t* gp);
int gp_pass(gp_t* gp, int player_no);
void gp_dump(gp_t* gp);

#ifdef __cplusplus
}
#endif
#endif
