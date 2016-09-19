#include "mj_algo.h"
#include <stdlib.h>
#include <string.h>

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
