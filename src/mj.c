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

int mjpai_ss2id(int suit, int sign)
{
    int id;
    if (suit == 0)
        return MJ_ID_EMPTY;
    else if (suit == mjSuitWan || suit == mjSuitSuo ||
             suit == mjSuitTong) {
        id = (suit - 1) * 9 + sign;
    } else if (suit == mjSuitFeng) {
        id = MJ_ID_DONG + (sign - 1);
    } else if (suit == mjSuitZFB) {
        id = MJ_ID_ZHONG + (sign - 1);
    } else if (suit == mjSuitHua) {
        id = MJ_ID_MEI + (sign - 1);
    } else if (suit == mjSuitSeason) {
        id = MJ_ID_CUN + (sign - 1);
    } else {
        id = MJ_ID_UNKNOW;
    }
    return id;
}

void mjpai_copy(mjpai_t* dest, mjpai_t* src)
{
    if (src && dest) {
        dest->id = src->id;
        dest->suit = src->suit;
        dest->sign = src->sign;
    }
}

unsigned char mjpai_encode(mjpai_t* pai)
{
    unsigned char x;

    if (pai) {
        if (pai->suit == mjSuitWan) {
            x = pai->sign;
        } else if (pai->suit == mjSuitSuo) {
            x = 9 + pai->sign;
        } else if (pai->suit == mjSuitTong) {
            x = 18 + pai->sign;
        } else if (pai->suit == mjSuitFeng) {
            x = 27 + pai->sign;
        } else if (pai->suit == mjSuitZFB) {
            x = 31 + pai->sign;
        } else if (pai->suit == mjSuitFlower) {
            x = 34 + pai->sign;
        } else if (pai->suit == mjSuitSeason) {
            x = 38 + pai->sign;
        } else {
            x = 43;
        }
    } else {
        x = 0;
    }
    return x;
}

void mjpai_decode(mjpai_t* pai, unsigned char x)
{
    if (pai) {
        if (x == 0) {
            pai->suit = pai->sign = 0;
        } else if (x >= 1 && x <= 9) {
            pai->suit = mjSuitWan;
            pai->sign = x;
        } else if (x >= 10 && x <= 18) {
            pai->suit = mjSuitSuo;
            pai->sign = x - 9;
        } else if (x >= 19 && x <= 27) {
            pai->suit = mjSuitTong;
            pai->sign = x - 18;
        } else if (x >= 28 && x <= 31) {
            pai->suit = mjSuitFeng;
            pai->sign = x - 27;
        } else if (x >= 32 && x <= 34) {
            pai->suit = mjSuitZFB;
            pai->sign = x - 31;
        } else if (x >= 35 && x <= 38) {
            pai->suit = mjSuitFlower;
            pai->sign = x - 34;
        } else if (x >= 39 && x <= 42) {
            pai->suit = mjSuitSeason;
            pai->sign = x - 38;
        } else {
            pai->suit = mjSuitUnknow;
            pai->sign = 0;
        }
    } else {
        x = 0;
    }
}

void mj_shuffle(mjpai_t* cards, int len)
{
    int i,n;
    int a,b;
    mjpai_t temp;
    mjpai_t *pa, *pb;

    if(!cards || len <= 2)
        return;

    n = 1000 + rand() % 50;
    for (i = 0; i < n; ++i) {
        a = rand() % len;
        b = rand() % len;
        if(a != b){
            pa = cards + a;
            pb = cards + b;
            temp.sign = pa->sign;
            temp.suit = pa->suit;
            pa->sign = pb->sign;
            pa->suit = pb->suit;
            pb->sign = temp.sign;
            pb->suit = temp.suit;
        }
    }
}

void mj_sort(mjpai_t* cards, int len, mjpai_t* mammon)
{
    if (!cards || len < 2)
        return;
}

void mj_trim(mjpai_t* cards, int len)
{
    int i,n;
    mjpai_t* p;
    static mjpai_t temp[32];

    if (len > 32)
        return;
    if (cards && len > 0) {
        memset((void*)temp, 0, sizeof(mjpai_t) * 32);
        n = 0;
        p = cards;
        for (i = 0; i < len; i++) {
            if (p->suit && p->sign) {
                temp[n].suit = p->suit;
                temp[n].sign = p->sign;
                n++;
            }
            p++;
        }
        memset((void*)cards, 0, sizeof(mjpai_t) * len);
        p = cards;
        for (i = 0; i < n; i++) {
            p->suit = temp[i].suit;
            p->sign = temp[i].sign;
            p++;
        }
    }
}

const char* mj_string(mjpai_t* cards, int len, int line_number)
{
    static char str[256];
    int i;
    mjpai_t* p;

    memset(str, 0, 256);
    if (cards && len > 0) {
        p = cards;
        for (i = 0; i < len; i++) {
            if (p->suit && p->sign) {
                strcat(str, mjpai_string(p));
            }
        }
    }
    return str;
}

const char* mjpai_string(mjpai_t* pai)
{
    static char str[8];

    memset(str, 0, 8);
    if (pai) {
        if (pai->suit == mjSuitSuo) {
            sprintf(str, "%dS", pai->sign);
        } else if (pai->suit == mjSuitWan) {
            sprintf(str, "%dW", pai->sign);
        } else if (pai->suit == mjSuitTong) {
            sprintf(str, "%dT", pai->sign);
        } else if (pai->suit == mjSuitFeng) {
            if (pai->sign == mjEast) {
                strcpy(str, "DF");
            } else if (pai->sign == mjSouth) {
                strcpy(str, "NF");
            } else if (pai->sign == mjWest) {
                strcpy(str, "XF");
            } else if (pai->sign == mjNorth) {
                strcpy(str, "BF");
            } else {
                strcpy(str, "@@");
            }
        } else if (pai->suit == mjSuitZFB) {
            if (pai->sign == mjZhong) {
                strcpy(str, "HZ");
            } else if (pai->sign == mjFa) {
                strcpy(str, "FC");
            } else if (pai->sign == mjBai) {
                strcpy(str, "BB");
            } else {
                strcpy(str, "@@");
            }
        } else if (pai->suit == mjSuitFlower) {
            if (pai->sign == mjMei) {
                strcpy(str, "MH");
            } else if (pai->sign == mjLan) {
                strcpy(str, "LH");
            } else if (pai->sign == mjJu) {
                strcpy(str, "JH");
            } else if (pai->sign == mjZhu) {
                strcpy(str, "ZH");
            } else {
                strcpy(str, "@@");
            }
        } else if (pai->suit == mjSuitSeason) {
            if (pai->sign == mjCun) {
                strcpy(str, "CT");
            } else if (pai->sign == mjXia) {
                strcpy(str, "XT");
            } else if (pai->sign == mjQiu) {
                strcpy(str, "QT");
            } else if (pai->sign == mjDong) {
                strcpy(str, "DT");
            } else {
                strcpy(str, "@@");
            }
        } else if (pai->suit == mjSuitUnknow) {
            strcpy(str, "??");
        } else {
            strcpy(str, "@@");
        }
    }
    return str;
}
