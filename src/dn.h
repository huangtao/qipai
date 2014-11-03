/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _DN_H
#define _DN_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "deck.h"
#include "card_player.h"

#define DN_MIN_PLAYER   2
#define DN_MAX_PLAYER   5

/* douniu rule */
typedef enum dn_rule_e{
    DN_RULE_DEFAULT,     /* default */
    DN_RULE_SUOHA        /* like suoha */
}DN_RULE;

/* dn hand type */
typedef enum dn_type_e{
        DN_NONIU = 0,   /* no niu */
        DN_NIUX,        /* have niu */
        DN_THREE_P2,    /* three plus double (333+44) */
        DN_BOMB,        /* bomb (6666+3), (KKK+3)*/       
        DN_5HUA,        /* all > J (JJQQK) */        
        DN_5XIAO        /* all < 5 (21334) */
}DN_TYPE;

typedef enum dn_gamestate_e{
    DN_GAME_END = 0,   /* game end */
    DN_GAME_PLAY       /* playing card */
}DN_GAMESTATE;

typedef struct dn_s{
    int debug;          /* output debug info */
    deck_t* deck;       /* deck */
    int game_state;     /* game state */
    int game_rule;      /* rule */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
    int have_side_pot;  /* side-pot mode */

    uint64_t pot;

    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int largest_player_no;  /* current round largest player no. */

    int player_num;
    unsigned int pot_turn[DN_MAX_PLAYER];
    card_player_t players[DN_MAX_PLAYER];   /* three player */
}dn_t;

dn_t* dn_new(int rule);
void dn_free(dn_t* dn);
void dn_start(dn_t* dn);      /* start a new game */
int dn_get_state(dn_t* dn);
void dn_set_state(dn_t* dn, int state);
const char* dn_htype_name(int htype);
void dn_handtype(dn_t* dn, hand_t* hand);
int dn_bet(dn_t* dn, int player_no, unsigned int chip);
int dn_call(dn_t* dn, int player_no);
int dn_fold(dn_t* dn, int player_no);
int dn_allin(dn_t* dn, int player_no);
void dn_next_player(dn_t* dn);
void dn_dump(dn_t* dn);
int dn_logicvalue(card_t* card);

#ifdef __cplusplus
}
#endif
#endif
