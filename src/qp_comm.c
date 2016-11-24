#include "qp_comm.h"

/* 根据座位号获取相对位置(上家、对家、下家) */
int p4_relative_seat(int base, int target)
{
    return (seatRelative)((4 + target - base) % 4);
}

/* 获取上家、对家、下家的选手编号(座位号) */
int p4_seat_no(int base, seatRelative relative)
{
    return (int)((base + (int)relative) % 4);
}

int p3_relative_seat(int base, int target)
{
    // 上家、下家
    int st;

    st = (seatRelative)((3 + target - base) % 3);
    if (st == stOpposit)
        st = stRight;
    return st;
}

int p3_seat_no(int base, seatRelative relative)
{
    int no;
    if (relative > 2)
        relative = 2;
    no = (int)((base + (int)relative) % 3);
    return no;
}

int p2_relative_seat(int base, int target)
{
    return (seatRelative)(target == base ? stSelf : stOpposit);
}

int p2_seat_no(int base, seatRelative relative)
{
    if (relative == stSelf)
        return base;
    else {
        return ((base + 1) % 2);
    }
}
