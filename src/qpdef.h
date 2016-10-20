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

/* 根据座位号获取相对位置(上家、对家、下家) */
inline int p4_relative_seat(int target, int base)
{
    return (seatRelative)((4 + target - base) % 4);
}

/* 获取上家、对家、下家的玩家编号(座位号) */
inline int p4_seat_no(int target, seatRelative relative)
{
    return (int)((target + (int)relative) % 4);
}

inline int p2_relative_seat(int target, int base)
{
    return (seatRelative)(target == base ? stSelf : stOpposit);
}
inline int p2_seat_no(int target, seatRelative relative)
{
    if (relative == stSelf)
        return target;
    else {
        return ((target + 1) % 2);
    }
}

#ifdef __cplusplus
}
#endif
#endif
