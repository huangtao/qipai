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

/* return code */
#define QIPAI_OK                0
#define QIPAI_PARAM             -1
#define QIPAI_OUTOFMEMORY       -2
#define QIPAI_OUTOFRANGE        -3
#define QIPAI_NOINIT            -4
#define QIPAI_NOCARD            -5
#define QIPAI_STATE             -6
#define QIPAI_RULE              -7

/* game state */
typedef enum gamestate_e {
    GAME_END = 0,   /* game end */
    GAME_PLAY       /* playing */
}GAME_STATE;

/* server or client */
typedef enum game_mode_e {
    GAME_MODE_SERVER = 0,
    GAME_MODE_CLIENT
}GAME_MODE;

typedef enum seat_absolute {
    stNo_1,
    stEast = stNo_1,
    stNo_2,
    stSouth = stNo_2,
    stNo_3,
    stWest = stNo_3,
    stNo_4,
    stNorth = stNo_4
}seatAbsolute;

/* seat no <-> array index 0,1,2,3 */
typedef enum seat_relative {
    stSelf, stRight, stOpposit, stLeft
}seatRelative;

/* 根据座位号获取相对位置(上家、对家、下家) */
int p4_relative_seat(int base, int target);
/* 获取上家、对家、下家的选手编号(座位号) */
int p4_seat_no(int base, seatRelative relative);


int p3_relative_seat(int base, int target);
int p3_seat_no(int base, seatRelative relative);

int p2_relative_seat(int base, int target);
int p2_seat_no(int base, seatRelative relative);

/* 函数指针声明 */
typedef int (*fp_relative_seat)(int base, int target);
typedef int (*fp_seat_no)(int base, seatRelative relative);


///* 根据座位号获取相对位置(上家、对家、下家) */
//inline int qp_relative_seat(int num, int target, int base)
//{
//    if (num == 2)
//        return (seatRelative)(target == base ? stSelf : stOpposit);
//    else
//        return (seatRelative)((num + target - base) % num);
//}

///* 获取上家、对家、下家的选手编号(座位号) */
//inline int qp_seat_no(int num, int target, seatRelative relative)
//{
//    return (int)((target + (int)relative) % num);
//}

#ifdef __cplusplus
}
#endif
#endif
