/*
*
* code:huangtao117@gmail.com
* 杭州麻将
*
*/
#ifndef _MJHZ_H
#define _MJHZ_H
#ifdef __cplusplus
extern "C" {
#endif

#include "mj.h"
#include "card_player.h"

#define MJHZ_MAX_PLAYER  4

/* 游戏状态 */
typedef enum mjhz_gamestate_e {
    MJHZ_GAME_END = 0,   /* game end */
    MJHZ_GAME_PLAY       /* playing */
}MJHZ_GAMESTATE;

/* mjhz hu info */
typedef enum mjhz_hu_s {
	int canhu;		/* 是否胡牌 */
    int is7dui;		/* 7对子 */
}mjhz_hu;

typedef struct mjhz_s {
    int debug;          /* output debug info */
    int mode;			/* client or server mode */
    int game_state;     /* game state */
    int turn_time;      /* turn time */
    int curr_turn_time; /* current turn left time */
    int round;
    int inning;
	int player_num;		/* 2 or 4 player */
    int banker_no;		/* banker no. */
    int first_player_no;/* first player no. */
    int curr_player_no; /* current turn player no. */
	mj_t deck[136];		/* mj card */
	int deck_deal_index;/* current deal card index */
	int deck_valid_num;	/* valid number card */
	mj_t last_played_mj;
	mj_t mammon;		/* 财神 */
    MJHZ_PLAYER players[MJHZ_MAX_PLAYER];
}mjhz_t;

/* init a mjhz game object */
void mjhz_init(mjhz_t* mj, int mode, int player_num);

/* start a new game */
void mjhz_start(mjhz_t* mj);

void mjhz_sort(mj_t* cards, int len);
const char* mjhz_hu_name(mjhz_hu_t* hu);
void mjhz_play(mjhz_t* mj, int player_no, mj_t card);
int mjhz_chi(mjhz_t* mj, int player_no);
int mjhz_peng(mjhz_t* mj, int player_no);
int mjhz_gang(mjhz_t* mj, int player_no);
int mjhz_hu(mjhz_t* mj, int player_no);
void mjhz_next_player(mjhz_t* mj);
void mjhz_dump(mjhz_t* mj);

#ifdef __cplusplus
}
#endif
#endif
