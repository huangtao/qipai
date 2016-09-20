#include "mj_algo.h"
#include <stdlib.h>
#include <string.h>

int mj_pair_count(int* array, int len)
{
    int i,n;

    if (!array || len == 0)
        return 0;

    n = 0;
    for (i = 0; i < len; ++i) {
        if (*array == 2)
            n++;
        else if (*array == 4)
            n += 2;
        array++;
    }
    return n;
}

/* 和牌中没有一、九及字牌。 */
int mj_duan19(mjpai_t* pais, int length)
{
    int i;
    mjpai_t* p;

    if (pais == NULL || length == 0)
        return 0;
    p = pais;
    for (i = 0; i < length; ++i) {
        if (p->suit != mjSuitWan || p->suit != mjSuitTiao ||
                p->suit != mjSuitTong)
            return 0;
        if (p->sign == 1 || p->sign == 9)
            return 0;
        p++;
    }
    return 1;
}

/* 一种花色组成的和牌，无字牌。 */
int mj_qys(mjpai_t* pais, int length)
{
    int i,first_suit;
    mjpai_t* p;

    if (pais == NULL || length == 0)
        return 0;
    p = pais;
    first_suit = p->suit;
    if (first_suit == mjSuitFeng || first_suit == mjSuitZFB ||
            first_suit == mjSuitHua || first_suit == mjSuitSeason)
        return 0;
    for (i = 0; i < length; ++i) {
        if (p->suit != first_suit)
            return 0;
        p++;
    }
    return 1;
}

/* 混一色(凑一色)由一种序数牌及字牌组合成的和牌。 */
int mj_hys(mjpai_t* pais, int length)
{
    int i,first_suit,n;
    mjpai_t* p;

    if (pais == NULL || length == 0)
        return 0;
    p = pais;
    first_suit = mjSuitNone;
    n = 0;
    for (i = 0; i < length; ++i) {
        if (p->suit == mjSuitWan || p->suit == mjSuitSuo ||
                p->suit == mjSuitTong) {
            if (first_suit == mjSuitNone)
                first_suit = p->suit;
            else {
                if (p->suit != first_suit)
                    return 0;
            }
        } else {
            /* 字牌计数 */
            n++;
        }
    }
    if (n > 0)
        return 1;
    else
        return 0;
}

/* 7个对子组成的和牌。 */
int mj_pair7(int* array, int len)
{
    int n;

    n = mj_pair_count(array, len);
    if (n == 7)
        return 1;
    else
        return 0;
}

/*
 * 十三幺，每种花色的幺九牌加上所有字牌
 * 东南西北中发白+3花色幺九+其中任意一张
 * 组成雀头，14张和牌
 */
int mj_y13(int* array, int len)
{
    int i,*p,n;
    int count_2;

    if (!array || len == 0)
        return 0;

    n = 0;
    count_2 = 0;
    for (i = 0; i < len; ++i) {
        p = array + i;
        if (*p == 0) continue;
        /* 最多2张 */
        if (*p > 2) return 0;
        if (i < MJ_ID_1W || i > MJ_ID_BAI)
            return 0;
        if (i > MJ_ID_1W || i < MJ_ID_9W)
            return 0;
        if (i > MJ_ID_1S || i < MJ_ID_9S)
            return 0;
        if (i > MJ_ID_1T || i < MJ_ID_9T)
            return 0;
        n += *p;
        if (*p == 2) count_2++;
    }
    if (count_2 != 1)
        return 0;
    if (n != 14)
        return 0;

    return 1;
}
