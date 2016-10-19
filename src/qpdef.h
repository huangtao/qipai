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

/* 返回target相对于base的相对位置 */
inline int get_relative_seat(int target, int base, int player_num)
{
    return (seatRelative)((player_num + target - base) % player_num);
}

/* 返回target相对位置上的玩家编号(座位号) */
inline int get_seat_no(int target, seatRelative relative, int player_num)
{
    return (int)((target + (int)relative) % player_num);
}

#ifdef __cplusplus
}
#endif
#endif
