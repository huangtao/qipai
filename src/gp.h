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

#include "card.h"

#define GP_MIN_PLAYER   2
#define GP_MAX_PLAYER   3
#define GP_MAX_CARDS    20
#define GP_DECK_FU      1

/* gp rule */
typedef enum gp_rule_e {
    GP_RULE_DEFAULT,     /* like QQ game's gp */
    GP_RULE_ZHUJI        /* zhejiang zhuji rule */
}GP_RULE;

/* gp card type */
typedef enum gp_type_e {
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

/* for hand type calc */
typedef struct hand_type_s {
    int type;           /* type enum */
    card_t type_card;   /* type card */
    int num;            /* number */
    int param1;	        /* logic value */
    int param2;
    int param3;
}hand_type;

typedef enum gp_gamestate_e {
    GP_GAME_END = 0,   /* game end */
    GP_GAME_PLAY       /* playing card */
}GP_GAMESTATE;

typedef enum gp_mode_e {
    GP_MODE_SERVER = 0,
    GP_MODE_CLIENT
}GP_MODE;

typedef struct gp_player_s {
    int level;
    int state;
    int position;
    int64_t score;
    uint64_t gold;
    card_t cards[GP_MAX_CARDS];
    card_t cards_played[GP_MAX_CARDS];
    int num_valid_card;
}GP_PLAYER;

typedef struct gp_s {
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

    hand_type last_hand_type;
    card_t last_hand[GP_MAX_CARDS];     /* last play out cards */
    GP_PLAYER players[GP_MAX_PLAYER];   /* three player */
}gp_t;

/* init a guanpai game object */
void gp_init(gp_t* gp, int rule, int mode, int player_num);

/* start a new game */
void gp_start(gp_t* gp);

void gp_sort(card_t* cards, int len);
const char* gp_htype_name(int htype);
void gp_handtype(gp_t* gp, card_t* cards, int len, hand_type* ht);
int gp_play(gp_t* gp, int player_no, card_t* cards, int len);
int gp_canplay(gp_t* gp, card_t* cards, int len);
void gp_next_player(gp_t* gp);
int gp_pass(gp_t* gp, int player_no);

/* simple hint for play */
int gp_hint(gp_t* gp, card_t* cards, int len);

/* 出牌搜索 */
int gp_analyse_search(cd_analyse* analyse, hand_type* ht_in, card_t* cards, int len);

/*
 * 将特定的rank值牌从src拷贝num张到dest中
 * 此函数不检查数组溢出
 * return: number of copyed
 */
int gp_copy_cards(card_t* src, card_t* dest, int offset, int rank, int num);

/* dump a gp */
void gp_dump(gp_t* gp);

#ifdef __cplusplus
}
#endif
#endif
