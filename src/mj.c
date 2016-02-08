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

unsigned char mjpai_encode(mjpai_t* card)
{
    unsigned char x;

    if (card) {
        if (card->suit == mjSuitWan) {
            x = card->sign;
        } else if (card->suit == mjSuitSuo) {
            x = 9 + card->sign;
        } else if (card->suit == mjSuitTong) {
            x = 18 + card->sign;
        } else if (card->suit == mjSuitFeng) {
            x = 27 + card->sign;
        } else if (card->suit == mjSuitZFB) {
            x = 31 + card->sign;
        } else if (card->suit == mjSuitFlower) {
            x = 34 + card->sign;
        } else if (card->suit == mjSuitSeason) {
            x = 38 + card->sign;
        } else {
            x = 43;
        }
    } else {
        x = 0;
    }
	return x;
}

void mjpai_decode(mjpai_t* card, unsigned char x)
{
	if (card) {
        if (x == 0) {
            card->suit = card->sign = 0;
        } else if (x >= 1 && x <= 9) {
            card->suit = mjSuitWan;
            card->sign = x;
        } else if (x >= 10 && x <= 18) {
            card->suit = mjSuitSuo;
            card->sign = x - 9;
        } else if (x >= 19 && x <= 27) {
            card->suit = mjSuitTong;
            card->sign = x - 18;
        } else if (x >= 28 && x <= 31) {
            card->suit = mjSuitFeng;
            card->sign = x - 27;
        } else if (x >= 32 && x <= 34) {
            card->suit = mjSuitZFB;
            card->sign = x - 31;
        } else if (x >= 35 && x <= 38) {
            card->suit = mjSuitFlower;
            card->sign = x - 34;
        } else if (x >= 39 && x <= 42) {
            card->suit = mjSuitSeason;
            card->sign = x - 38;
        } else {
            card->suit = mjSuitUnknow;
            card->sign = 0;
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

const char* mjpai_string(mjpai_t* card)
{
	static char str[8];

	memset(str, 0, 8);
	if (card) {
		if (card->suit == mjSuitSuo) {
			sprintf(str, "%dS", card->sign);
		} else if (card->suit == mjSuitWan) {
			sprintf(str, "%dW", card->sign);
		} else if (card->suit == mjSuitTong) {
			sprintf(str, "%dT", card->sign);
		} else if (card->suit == mjSuitFeng) {
			if (card->sign == mjEast) {
				strcpy(str, "DF");
			} else if (card->sign == mjSouth) {
				strcpy(str, "NF");
			} else if (card->sign == mjWest) {
				strcpy(str, "XF");
			} else if (card->sign == mjNorth) {
				strcpy(str, "BF");
			}
		} else if (card->suit == mjSuitZFB) {
			if (card->sign == mjZhong) {
				strcpy(str, "HZ");
			} else if (card->sign == mjFa) {
				strcpy(str, "FC");
			} else if (card->sign == mjBai) {
				strcpy(str, "BB");
			}
		} else if (card->suit == mjSuitFlower) {
			if (card->sign == mjMei) {
				strcpy(str, "MH");
			} else if (card->sign == mjLan) {
				strcpy(str, "LH");
			} else if (card->sign == mjJu) {
				strcpy(str, "JH");
			} else if (card->sign == mjZhu) {
				strcpy(str, "ZH");
			}
		} else if (card->suit == mjSuitSeason) {
			if (card->sign == mjCun) {
				strcpy(str, "CT");
			} else if (card->sign == mjXia) {
				strcpy(str, "XT");
			} else if (card->sign == mjQiu) {
				strcpy(str, "QT");
			} else if (card->sign == mjDong) {
				strcpy(str, "DT");
			}
		}
	}
	return str;
}
