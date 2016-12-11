#include "mj.h"
#include "qp_common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void mjpai_init_id(mjpai_t* pai, int id)
{
    if (pai == NULL)
        return;
    if (id <= PAI_EMPTY || id > PAI_UNKNOW) {
        pai->id = PAI_EMPTY;
        pai->sign = 0;
        pai->suit = 0;
        return;
    }
    pai->id = id;
    if (id >= PAI_1W && id <= PAI_9W) {
        pai->suit = mjSuitWan;
        pai->sign = id - PAI_1W + 1;
    } else if (id >= PAI_1S && id <= PAI_9S) {
        pai->suit = mjSuitSuo;
        pai->sign = id - PAI_1S + 1;
    } else if (id >= PAI_1T && id <= PAI_9T) {
        pai->suit = mjSuitTong;
        pai->sign = id - PAI_1T + 1;
    } else if (id >= PAI_DF && id <= PAI_BF) {
        pai->suit = mjSuitFeng;
        pai->sign = id - PAI_DF + 1;
    } else if (id >= PAI_ZHONG && id <= PAI_BAI) {
        pai->suit = mjSuitZFB;
        pai->sign = id - PAI_ZHONG + 1;
    } else if (id >= PAI_MEI && id <= PAI_JU) {
        pai->suit = mjSuitHua;
        pai->sign = id - PAI_MEI + 1;
    } else if (id >= PAI_CUN && id <= PAI_DONG) {
        pai->suit = mjSuitSeason;
        pai->sign = id - PAI_CUN + 1;
    } else {
        pai->suit = 0;
        pai->sign = 0;
    }
}

void mjpai_init_ss(mjpai_t* pai, int suit, int sign)
{
    if (pai == NULL)
        return;
    if (suit <= mjSuitNone || suit > mjSuitUnknow) {
        pai->id = 0;
        pai->suit = 0;
        pai->sign = 0;
        return;
    }
    pai->sign = sign;
    pai->suit = suit;
    if (suit == mjSuitWan) {
        pai->id = PAI_1W + sign - 1;
    } else if (suit == mjSuitSuo) {
        pai->id = PAI_1S + sign - 1;
    } else if (suit == mjSuitTong) {
        pai->id = PAI_1T + sign - 1;
    } else if (suit == mjSuitFeng) {
        pai->id = PAI_DF + sign - 1;
    } else if (suit == mjSuitZFB) {
        pai->id = PAI_ZHONG + sign - 1;
    } else if (suit == mjSuitHua) {
        pai->id = PAI_MEI + sign - 1;
    } else if (suit == mjSuitSeason) {
        pai->id = PAI_CUN + sign - 1;
    } else {
        pai->id = PAI_UNKNOW;
    }
}

void mjpai_zero(mjpai_t* pai)
{
    if (pai) {
        pai->id = 0;
        pai->suit = 0;
        pai->sign = 0;
    }
}

void mjpai_copy(mjpai_t* dest, mjpai_t* src)
{
    if (src && dest) {
        dest->id = src->id;
        dest->suit = src->suit;
        dest->sign = src->sign;
    }
}

int mjpai_sign(int pai_id)
{
    if (pai_id >= PAI_1W && pai_id <= PAI_9T) {
        return ((pai_id - 1) % 9 + 1);
    } else {
        return 0;
    }
}

void mj_shuffle(int* pais, int len)
{
    int i,n;
    int a,b,temp;

    if (!pais || len <= 2)
        return;

    n = 1000 + rand() % 50;
    for (i = 0; i < n; ++i) {
        a = rand() % len;
        b = rand() % len;
        if(a != b){
            temp = *(pais + a);
            *(pais + a) = *(pais + b);
            *(pais + b) = temp;
        }
    }
}

void mj_delete(int* pais, int len, int id)
{
    int i;

    if (!pais || len <= 2)
        return;
    for (i = 0; i < len; ++i) {
        if ((*pais) == id) {
            (*pais) = 0;
            return;
        }
        pais++;
    }
}

int mj_trim(int* pais, int len)
{
    int i,n;
    int* p;
    int temp[len];  /* c99 */

    if (len > 32)
        return 0;
    n = 0;
    if (pais && len > 0) {
        p = pais;
        for (i = 0; i < len; i++) {
            if (*p != 0) {
                temp[n++] = *p;
            }
            p++;
        }
        memset((void*)pais, 0, sizeof(int) * len);
        p = pais;
        for (i = 0; i < n; i++) {
            *p = temp[i];
            p++;
        }
    }
    return n;
}

int mj_length(int *pais, int len)
{
    int i,n;

    if (!pais)
        return 0;
    n = 0;
    for (i = 0; i < len; ++i) {
        if (*pais != 0)
            n++;
        pais++;
    }

    return n;
}

const char* mj_string(int* pais, int len, int line_number)
{
    static char str[256];
    int i,pai_id,n;

    memset(str, 0, 256);
    if (pais && len > 0) {
        n = 0;
        for (i = 0; i < len; i++) {
            pai_id = *(pais + i);
            if (pai_id != 0) {
                strcat(str, mjpai_string(pai_id));
                n++;
                if (n >= line_number) {
                    strcat(str, "\n");
                    n = 0;
                }
            }
        }
    }
    return str;
}

const char* mjpai_string(int id)
{
    static char str[8];
    mjpai_t pai;

    memset(str, 0, 8);
    mjpai_init_id(&pai, id);
    if (pai.suit == mjSuitSuo) {
        sprintf(str, "%dS", pai.sign);
    } else if (pai.suit == mjSuitWan) {
        sprintf(str, "%dW", pai.sign);
    } else if (pai.suit == mjSuitTong) {
        sprintf(str, "%dT", pai.sign);
    } else if (pai.suit == mjSuitFeng) {
        if (pai.sign == mjEast) {
            strcpy(str, "DF");
        } else if (pai.sign == mjSouth) {
            strcpy(str, "NF");
        } else if (pai.sign == mjWest) {
            strcpy(str, "XF");
        } else if (pai.sign == mjNorth) {
            strcpy(str, "BF");
        } else {
            strcpy(str, "@@");
        }
    } else if (pai.suit == mjSuitZFB) {
        if (pai.sign == mjZhong) {
            strcpy(str, "HZ");
        } else if (pai.sign == mjFa) {
            strcpy(str, "FC");
        } else if (pai.sign == mjBai) {
            strcpy(str, "BB");
        } else {
            strcpy(str, "@@");
        }
    } else if (pai.suit == mjSuitFlower) {
        if (pai.sign == mjMei) {
            strcpy(str, "MH");
        } else if (pai.sign == mjLan) {
            strcpy(str, "LH");
        } else if (pai.sign == mjJu) {
            strcpy(str, "JH");
        } else if (pai.sign == mjZhu) {
            strcpy(str, "ZH");
        } else {
            strcpy(str, "@@");
        }
    } else if (pai.suit == mjSuitSeason) {
        if (pai.sign == mjCun) {
            strcpy(str, "CT");
        } else if (pai.sign == mjXia) {
            strcpy(str, "XT");
        } else if (pai.sign == mjQiu) {
            strcpy(str, "QT");
        } else if (pai.sign == mjDong) {
            strcpy(str, "DT");
        } else {
            strcpy(str, "@@");
        }
    } else if (pai.suit == mjSuitUnknow) {
        strcpy(str, "??");
    } else {
        strcpy(str, "@@");
    }
    return str;
}
