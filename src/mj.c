#include "mj.h"
#include "ht_lch.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int mjpai_equal(mjpai_t* a, mjpai_t* b)
{
    if (a && b) {
        if (a->suit == b->suit &&
                a->sign == b->sign) {
            return 0;
        }
    }
    return -1;
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
            }
        } else if (pai->suit == mjSuitZFB) {
            if (pai->sign == mjZhong) {
                strcpy(str, "HZ");
            } else if (pai->sign == mjFa) {
                strcpy(str, "FC");
            } else if (pai->sign == mjBai) {
                strcpy(str, "BB");
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
            }
        }
    }
    return str;
}

int mj_pair7(int* array, int len)
{
    int i,n;

    if (!array || len < 4)
        return 0;

    n = 0;
    for (i = 0; i < len; ++i) {
        if (*array == 2)
            n++;
        else if (*array == 4)
            n += 2;
    }
    return n;
}

