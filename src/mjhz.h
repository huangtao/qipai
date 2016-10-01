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

#define MJHZ_MAX_PLAYERS	4
#define MJHZ_MAX_PAIS		14      /* 手上牌最大数量 */
#define MJHZ_MAX_SETS       6       /* 面子最大数量 */
#define MJHZ_DECK_PAIS      136		/* 杭州麻将麻将牌最大数量 */
#define MJHZ_MAX_PAITYPE	34		/* 杭州麻将使用34种牌(27张序数+7张字牌) */

#define MJHZ_MAX_PLAYED     50      /* 记录打出的牌 */
#define MJHZ_LEN_JS			35      /* 用于计数 */

/* 游戏状态 */
typedef enum mjhz_gamestate_e {
    MJHZ_GAME_END = 0,   /* game end */
    MJHZ_GAME_PLAY       /* playing */
}MJHZ_GAMESTATE;

typedef enum mjhz_mode_e {
    MJHZ_MODE_SERVER = 0,
    MJHZ_MODE_CLIENT
}MJHZ_MODE;

/* mjhz hu info */
typedef struct mjhz_hu_s {
	int fan;		/* 番数 */
    int is_pair7;	/* 7对子 */
    int pair7_h4;	/* 7对子包含4个的数量 */
    int is_baotou;	/* 爆头 */
    int is_gk;      /* 杠开 */
    int cai_piao;	/* 财飘次数 */
}mjhz_hu_t;

typedef struct mjhz_player_s {
    int level;
    int state;
    int position;
    int64_t score;
    uint64_t gold;
    int tiles[MJHZ_MAX_PAIS];
    int tiles_played[MJHZ_MAX_PLAYED];
    mj_melded_t mj_sets[MJHZ_MAX_SETS];
    int tiles_js[MJHZ_LEN_JS];              /* 用于分析麻将 */
    int last_played;
	int can_chi;
	int can_peng;
	int can_gang;
	int can_hu;
    int pai_gang[4];        /* 杠牌信息 */
    mjhz_hu_t hu;           /* 胡牌信息 */
}mjhz_player_t;

typedef struct mjhz_s {
    int debug;              /* output debug info */
    int mode;               /* client or server mode */
    int game_state;         /* game state */
    int turn_time;          /* turn time */
    int curr_turn_time;     /* current turn left time */
    int round;
    int inning;
    int player_num;         /* 2 or 4 player */
    int banker_no;          /* banker no. */
    int first_player_no;    /* first player no. */
    int curr_player_no;     /* current turn player no. */
    int hu_player_no;
    int flag_liu;           /* 流局标记 */

    int dice1;
    int dice2;
    int deck_all_num;
    int deck_deal_index;    /* current deal card index */
    //int deck_deal_end;      /* where deal end position */
    int deck_deal_gang;     /* deal when gang */
    int deck_valid_num;     /* valid number card */

    int last_played_mj;
	int last_played_no;
    int last_takes_no;      /* last takes(摸牌) player no */
    int lao_z;              /* 老庄 */
    int enable_dian_hu;     /* 能否点和(点炮、捉冲) */
    int joker;              /* 百搭(财神) */
    int pai_gang;           /* 当前杠牌 */

    int deck[MJHZ_DECK_PAIS];                   /* deck mj pais */
    mjhz_player_t players[MJHZ_MAX_PLAYERS];    /* players */
}mjhz_t;

/* init a mjhz game object */
void mjhz_init(mjhz_t* mj, int mode, int player_num);

/* start a new game */
void mjhz_start(mjhz_t* mj);

/* sort a hand */
void mjhz_sort(int* pais, int len);

/* 打牌 */
int mjhz_play(mjhz_t* mj, int player_no, int pai_id);

/* 摸牌 */
int mjhz_takes(mjhz_t* mj, int is_gang);

int mjhz_can_chi(mjhz_t* mj, int player_no);
int mjhz_can_peng(mjhz_t* mj, int player_no);

/* 杠判定。返回可以杠的数量。 */
int mjhz_can_gang(mjhz_t* mj, int player_no);

/* 是否全顺子和刻子 */
int mjhz_all_melded(int array[MJHZ_LEN_JS]);

/* 带财神数量 */
int mjhz_all_melded_joker(int array[MJHZ_LEN_JS], int num_joker);

/* 和(胡)判定 */
int mjhz_can_hu(mjhz_t* mj, int player_no);

/* 吃 */
int mjhz_chi(mjhz_t* mj, int player_no, int pai1, int pai2);

/* 碰 */
int mjhz_peng(mjhz_t* mj, int player_no);

/* 杠 */
int mjhz_gang(mjhz_t* mj, int player_no, int pai);

/* 胡(和) */
int mjhz_hu(mjhz_t* mj, int player_no);

void mjhz_next_player(mjhz_t* mj);
int mjhz_get_next(mjhz_t* mj);

/* 帮助函数 */
void mjhz_dump(mjhz_t* mj);
const char* mjhz_hu_name(mjhz_hu_t* hu);

#ifdef __cplusplus
}
#endif
#endif
