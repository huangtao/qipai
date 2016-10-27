/*
 * Majang game algorithm library
 * This file is distributed under the BSD License.
 * Copyright (C) 2015 Huang Tao(huangtao117@gmail.com)
 */
#ifndef _MAJANG_H
#define _MAJANG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "qp_comm.h"
#include <time.h>

/*
 * 1914年出版的《绘图麻雀牌谱》中对东南西北的规定
 * ===========
 * =   南    =
 * =西     东=
 * =   北    =
 * ===========
 */

/*
 * 麻将常用术语
 * 完整的麻将牌共152张
 * 字牌(28张)+花牌(8张)+序数牌(108张)+百搭牌(8张)
 * 常用的不含百搭144张，若没有花牌为136张。
 * 顺子：由同花色三个连续的牌组成。
 * 刻子：又称“坎”，由三张或四张（又称杠子）相同的牌组成。
 * 面子：顺子和刻子的统称。
 * 将头：又称“眼”、雀頭（吊头），胡牌需要的对子。
 * 胡牌：一副牌必须凑满四（五）副面子及一组将，才可以胡牌。
 * 庄家：每局掷骰子决定开牌位置的人。
 * 连庄：庄家胡牌或者流局即可连庄。
 * 番：也称台头，日语里面为役。
 * 自摸: Winning from the wall，摸牌起和（胡）。
 * 流局: drawn。
 */

#define WAITTIME_DISCARD   20       /* 摸牌打牌时间 */
#define WAITTIME_TAKE       8       /* 吃碰杠时间 */

/* majiang tile suit */
typedef enum majiang_suit {
    mjSuitNone,
    mjSuitWan,                      /* 万子 */
    mjSuitCharacter = mjSuitWan,
    mjSuitSuo,                      /* 索子 */
    mjSuitBamboo = mjSuitSuo,
    mjSuitTong,                     /* 筒子 */
    mjSuitCircle = mjSuitTong,
    mjSuitFeng,                     /* 风牌 */
    mjSuitWind = mjSuitFeng,
    mjSuitSanY,                     /* 三元牌(中发白) */
    mjSuitZFB = mjSuitSanY,
    mjSuitDragon = mjSuitSanY,
    mjSuitHua,                      /* 花牌 */
    mjSuitFlower = mjSuitHua,
    mjSuitSeason,
    mjSuitUnknow
}mjSuit;

/* majiang ordinal */
typedef enum majiang_ordinal {
    mj1 = 1,
    mj2,
    mj3,
    mj4,
    mj5,
    mj6,
    mj7,
    mj8,
    mj9
}mjOrdinal;

typedef enum majiang_wind {
    mjEast = 1,
    mjSouth,
    mjWest,
    mjNorth
}mjWind;

typedef enum majiang_dragon {
    mjZhong = 1,
    mjRed = mjZhong,
    mjFa,
    mjGreen = mjFa,
    mjBai,
    mjWhite = mjBai
}mjDragon;

typedef enum majiang_flower {
    mjMei = 1,
    mjPlum = mjMei,
    mjLan,
    mjOrchid = mjLan,
    mjZhu,
    mjBamboo = mjZhu,
    mjJu,
    mjChrysan = mjJu
}mjFlower;

typedef enum majiang_season {
    mjCun = 1,
    mjSpring = mjCun,
    mjXia,
    mjSummer = mjXia,
    mjQiu,
    mjAutumn = mjQiu,
    mjDong,
    mjWinter = mjDong
}mjSeason;

/* 麻将牌值定义 */
typedef enum pai_id {
    PAI_EMPTY,
    /* 万子 */
    PAI_1W, PAI_2W, PAI_3W, PAI_4W, PAI_5W, PAI_6W, PAI_7W, PAI_8W, PAI_9W,
    /* 索子 */
    PAI_1S, PAI_2S, PAI_3S, PAI_4S, PAI_5S, PAI_6S, PAI_7S, PAI_8S, PAI_9S,
    /* 筒子 */
    PAI_1T, PAI_2T, PAI_3T, PAI_4T, PAI_5T, PAI_6T, PAI_7T, PAI_8T, PAI_9T,
    /* 字牌 */
    PAI_DF, PAI_NF, PAI_XF, PAI_BF, PAI_ZHONG, PAI_FA, PAI_BAI,
    /* 花牌 */
    PAI_MEI, PAI_LAN, PAI_ZHU, PAI_JU, PAI_CUN, PAI_XIA, PAI_QIU, PAI_DONG,
    /* 特殊 */
    PAI_UNKNOW = 50
}paiID;

/* 面子类型 */
typedef enum meld_type {
    meldNone,
    meldShun,           /* 顺子 */
    meldKe,             /* 刻子 */
    meldJiang,          /* 将牌(对子) */
    meldChiLow,         /* 吃最小的一张 */
    meldChiMiddle,      /* 吃中间的一张 */
    meldChiUpper,       /* 吃最大的一张 */
    meldPengLeft,       /* 碰上家 */
    meldPengOpposit,    /* 碰对家 */
    meldPengRight,      /* 碰下家 */
    meldGang,           /* 暗杠 */
    meldGangLeft,       /* 明杠上家 */
    meldGangOpposit,    /* 明杠对家 */
    meldGangRight,      /* 明杠下家 */
    meldGangAddLeft,    /* 加杠上家 */
    meldGangAddOpposit, /* 加杠对家 */
    meldGangAddRight    /* 加杠下家 */
}meldType;

/* 游戏开始后的逻辑状态 */
typedef enum mj_logic_state {
    lsDiscard,  /* 弃牌(打出牌) */
    lsTake      /* 抓牌(包括玩家吃碰杠胡) */
}mjLogicState;

/* called (claimed by other players) */
typedef enum mj_call_type {
    callChi,
    callPeng,
    callGang,
    callHu
}mjCallType;

/* a mj pai */
typedef struct mjpai_s {
    int id;     /* start with 1 */
    int suit;   /* suit */
    int sign;   /* sign */
}mjpai_t;

/* 面子 */
typedef struct mj_meld_s {
    int type;       /* 面子类型 */
    int pai_id;     /* 特征牌 */
}mj_meld_t;

void mjpai_init_id(mjpai_t* pai, int id);
void mjpai_init_ss(mjpai_t* pai, int suit, int sign);
void mjpai_zero(mjpai_t* pai);

void mjpai_copy(mjpai_t* dest, mjpai_t* src);

int mjpai_sign(int pai_id);

/* 洗牌 */
void mj_shuffle(int* pais, int len);

/* 删除一张牌 */
void mj_delete(int* pais, int len, int id);

/* 整理，返回有效数量 */
int mj_trim(int* pais, int len);

/* 返回有效数量 */
int mj_length(int* pais, int len);

/**
 * print cards to a readable string
 */
const char* mj_string(int* pais, int len, int line_number);
const char* mjpai_string(int id);

#ifdef __cplusplus
}
#endif
#endif
