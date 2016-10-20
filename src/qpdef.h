/*
 * qipai game library
 * This file is distributed under the BSD License.
 * Copyright (C) 2015 Huang Tao(huangtao117@gmail.com)
 */
#ifndef _QIPAIDEF_H
#define _QIPAIDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 游戏状态 */
typedef enum gamestate_e {
    GAME_END = 0,   /* game end */
    GAME_PLAY       /* playing */
}GAME_STATE;

/* 服务器/客户端模式 */
typedef enum game_mode_e {
    GAME_MODE_SERVER = 0,
    GAME_MODE_CLIENT
}GAME_MODE;

/* 座位号定义 */
typedef enum seat_absolute {
    stEast, stSouth, stWest, stNorth
}seatAbsolute;

typedef enum seat_relative {
    stSelf, stLeft, stOpposit, stRight
}seatRelative;

#ifdef __cplusplus
}
#endif
#endif
