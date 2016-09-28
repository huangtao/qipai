#include "mj.h"
#include "qp_error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void mjpai_init_id(mjpai_t* pai, int id)
{
    if (pai == NULL)
        return;
    if (id <= MJ_ID_EMPTY || id > MJ_ID_UNKNOW) {
        pai->id = MJ_ID_EMPTY;
        pai->sign = 0;
        pai->suit = 0;
        return;
    }
    pai->id = id;
    if (id >= MJ_ID_1W && id <= MJ_ID_9W) {
        pai->suit = mjSuitWan;
        pai->sign = id - MJ_ID_1W + 1;
    } else if (id >= MJ_ID_1S && id <= MJ_ID_9S) {
        pai->suit = mjSuitTiao;
        pai->sign = id - MJ_ID_1S + 1;
    } else if (id >= MJ_ID_1T && id <= MJ_ID_9T) {
        pai->suit = mjSuitTong;
        pai->sign = id - MJ_ID_1T + 1;
    } else if (id >= MJ_ID_DONG && id <= MJ_ID_BEI) {
        pai->suit = mjSuitFeng;
        pai->sign = id - MJ_ID_DONG + 1;
    } else if (id >= MJ_ID_ZHONG && id <= MJ_ID_BAI) {
        pai->suit = mjSuitZFB;
        pai->sign = id - MJ_ID_ZHONG + 1;
    } else if (id >= MJ_ID_MEI && id <= MJ_ID_JU) {
        pai->suit = mjSuitHua;
        pai->sign = id - MJ_ID_MEI + 1;
    } else if (id >= MJ_ID_CUN && id <= MJ_ID_SDONG) {
        pai->suit = mjSuitSeason;
        pai->sign = id - MJ_ID_CUN + 1;
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
        pai->id = MJ_ID_1W + sign - 1;
    } else if (suit == mjSuitTiao) {
        pai->id = MJ_ID_1S + sign - 1;
    } else if (suit == mjSuitTong) {
        pai->id = MJ_ID_1T + sign - 1;
    } else if (suit == mjSuitFeng) {
        pai->id = MJ_ID_DONG + sign - 1;
    } else if (suit == mjSuitZFB) {
        pai->id = MJ_ID_ZHONG + sign - 1;
    } else if (suit == mjSuitHua) {
        pai->id = MJ_ID_MEI + sign - 1;
    } else if (suit == mjSuitSeason) {
        pai->id = MJ_ID_CUN + sign - 1;
    } else {
        pai->id = MJ_ID_UNKNOW;
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

void mj_shuffle(int* pais, int len)
{
    int i,n;
    int a,b,temp;

    if(!pais || len <= 2)
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
    int temp[32];

    if (len > 32)
        return 0;
    if (pais && len > 0) {
        memset((void*)temp, 0, sizeof(int) * 32);
        n = 0;
        p = pais;
        for (i = 0; i < len; i++) {
            if (*p != MJ_ID_EMPTY) {
                temp[n] = *p;
                n++;
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
