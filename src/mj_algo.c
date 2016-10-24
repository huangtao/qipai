#include "mj_algo.h"
#include <stdlib.h>
#include <string.h>

int mj_range_melded(int array[], int start, int* num_joker)
{
    int i,left_joker;
    int js[9];

    if (start != PAI_1W && start != PAI_1S &&
            start != PAI_1T)
        return 0;
    if (num_joker == NULL)
        return 0;
    left_joker = *num_joker;
    if (left_joker < 0 || left_joker > 4)
        return 0;

    memcpy(js, array + start, sizeof(int) * 9);
    for (i = 0; i < 7; ++i) {
        if (js[i] % 3 == 0) {
            js[i] = 0;
            continue;
        }
        else if (js[i] % 3 == 1) {
            if (js[i+1] > 0 && js[i+2] > 0) {
                /* 111 112 113 121 123 131 133 */
                js[i] = 0;
                js[i+1]--;
                js[i+2]--;
                continue;
            } else {
                if (left_joker == 0)
                    return 0;
                if (js[i+1] > 0) {
                    /* 110 120 130 */
                    js[i] = 0;
                    js[i+1]--;
                    left_joker--;
                    continue;
                } else if (js[i+2] > 0) {
                    /* 101 102 103 */
                    js[i] = 0;
                    js[i+2]--;
                    left_joker--;
                    continue;
                } else {
                    /* 100 */
                    if (left_joker < 2)
                        return 0;
                    js[i] = 0;
                    left_joker -= 2;
                    continue;
                }
            }
        } else if (js[i] % 3 == 2) {
            if (js[i+1] >= 2 && js[i+2] >= 2) {
                /* 222 232 242 223 233 243 224 */
                js[i] = 0;
                js[i+1] -= 2;
                js[i+2] -= 2;
                continue;
            } else {
                if (left_joker == 0)
                    return 0;
                if (js[i+1] == 1 && js[i+2] >= 2) {
                    /* 212 213 214 */
                    js[i] = 0;
                    js[i+1] = 0;
                    left_joker--;
                    js[i+2] -= 2;
                    continue;
                } else if (js[i+2] == 1 && js[i+1] >= 2) {
                    /* 221 231 241 */
                    js[i] = 0;
                    js[i+1] -= 2;
                    js[i+2] = 0;
                    left_joker--;
                    continue;
                } else {
                    /*
                     * 201 202 203 204
                     * 210 220 230 240
                     * 这种情况凑刻子
                     */
                    left_joker--;
                    js[i] = 0;
                    continue;
                }
            }
        }
    }
    /* 8 */
    if (js[7] % 3 == 1) {
        if (js[8] % 3 == 1 && left_joker > 0) {
            js[8] -= 1;
            left_joker--;
            js[7] = 0;
        } else if (left_joker >= 2) {
            js[7] = 0;
            left_joker -= 2;
        } else {
            return 0;
        }
    } else if (js[7] % 3 == 2) {
        if (left_joker == 0)
            return 0;
        left_joker--;
        js[7] = 0;
    } else {
        js[7] = 0;
    }
    /* 9 */
    if (js[8] % 3 == 1) {
        if (left_joker >= 2) {
            left_joker -= 2;
            js[8] = 0;
        } else {
            return 0;
        }
    } else if (js[8] % 3 == 2) {
        if (left_joker == 0)
            return 0;
        left_joker--;
        js[8] = 0;
    } else {
        js[8] = 0;
    }
    /* now js[9] is all zero */
    *num_joker = left_joker;
    return 1;
}

int mj_hornor_melded(int* array, int* num_joker)
{
    int i,left_joker;
    int js[7];

    if (num_joker == NULL)
        return 0;
    left_joker = *num_joker;
    if (left_joker < 0 || left_joker > 4)
        return 0;
    memcpy(js, array + PAI_DF, sizeof(int) * 7);
    for (i = 0; i < 7; ++i) {
        if (js[i] == 0) continue;
        if (js[i] % 3 == 1) {
            if (left_joker < 2)
                return 0;
        } else if (js[i] % 3 == 2) {
            if (left_joker == 0)
                return 0;
        }
        js[i] = 0;
    }
    /* now js[7] is all zero */
    *num_joker = left_joker;
    return 1;
}

void _sign_meld_delete(int* js, int* num_joker)
{
    int i,left_joker;

    if (!js || !num_joker)
        return;
    left_joker = *num_joker;
    if (left_joker < 0)
        left_joker = 0;
    for (i = 0; i < 7; ++i) {
        if (js[i] % 3 == 0) {
            js[i] = 0;
            continue;
        }
        else if (js[i] % 3 == 1) {
            if (js[i+1] > 0 && js[i+2] > 0) {
                /* 111 112 113 121 123 131 133 */
                js[i] = 0;
                js[i+1]--;
                js[i+2]--;
                continue;
            } else {
                if (left_joker == 0)
                    continue;
                if (js[i+1] > 0) {
                    /* 110 120 130 */
                    js[i] = 0;
                    js[i+1]--;
                    left_joker--;
                    continue;
                } else if (js[i+2] > 0) {
                    /* 101 102 103 */
                    js[i] = 0;
                    js[i+2]--;
                    left_joker--;
                    continue;
                } else {
                    /* 100 */
                    if (left_joker < 2)
                        continue;
                    js[i] = 0;
                    left_joker -= 2;
                    continue;
                }
            }
        } else if (js[i] % 3 == 2) {
            if (js[i+1] >= 2 && js[i+2] >= 2) {
                /* 222 232 242 223 233 243 224 */
                js[i] = 0;
                js[i+1] -= 2;
                js[i+2] -= 2;
                continue;
            } else {
                if (left_joker == 0)
                    continue;
                if (js[i+1] == 1 && js[i+2] >= 2) {
                    /* 212 213 214 */
                    js[i] = 0;
                    js[i+1] = 0;
                    left_joker--;
                    js[i+2] -= 2;
                    continue;
                } else if (js[i+2] == 1 && js[i+1] >= 2) {
                    /* 221 231 241 */
                    js[i] = 0;
                    js[i+1] -= 2;
                    js[i+2] = 0;
                    left_joker--;
                    continue;
                } else {
                    /*
                     * 201 202 203 204
                     * 210 220 230 240
                     * 这种情况凑刻子
                     */
                    left_joker--;
                    js[i] = 0;
                    continue;
                }
            }
        }
    }
    /* 8 */
    if (js[7] % 3 == 1) {
        if (js[8] % 3 == 1 && left_joker > 0) {
            js[8] -= 1;
            left_joker--;
            js[7] = 0;
        } else if (left_joker >= 2) {
            js[7] = 0;
            left_joker -= 2;
        } else {
        }
    } else if (js[7] % 3 == 2) {
        if (left_joker > 0) {
            left_joker--;
            js[7] = 0;
        }
    } else {
        js[7] = 0;
    }
    /* 9 */
    if (js[8] % 3 == 1) {
        if (left_joker >= 2) {
            left_joker -= 2;
            js[8] = 0;
        } else {
        }
    } else if (js[8] % 3 == 2) {
        if (left_joker > 0) {
            left_joker--;
            js[8] = 0;
        }
    } else {
        js[8] = 0;
    }
    /* now js[9] is all zero */
    *num_joker = left_joker;
}

void _hornor_meld_delete(int* array, int* num_joker)
{
    int i,n,left_joker;
    int* js;

    if (!array || !num_joker)
        return;
    left_joker = *num_joker;
    if (left_joker < 0)
        left_joker = 0;
    js = array + PAI_DF;
    for (i = 0; i < 7; ++i) {
        n = *(js + i);
        if (n == 0) continue;
        if (n % 3 == 1) {
            if (left_joker >= 2) {
                left_joker -= 2;
                *(js + i) = 0;
            }
        } else if (n % 3 == 2) {
            if (left_joker > 0) {
                left_joker--;
                *(js + i) = 0;
            }
        } else {
            *(js + i) = 0;
        }
    }
    *num_joker = left_joker;
}

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
        if (p->suit != mjSuitWan || p->suit != mjSuitSuo ||
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
        if (i < PAI_1W || i > PAI_BAI)
            return 0;
        if (i > PAI_1W || i < PAI_9W)
            return 0;
        if (i > PAI_1S || i < PAI_9S)
            return 0;
        if (i > PAI_1T || i < PAI_9T)
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
