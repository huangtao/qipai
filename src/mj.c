#include "mj.h"
#include "ht_lch.h"
#include <stdlib.h>

int mj_equal(mj_t* a, mj_t* b)
{
	if (a && b) {
		if (a->suit == b->suit &&
				a->sign == b->sign) {
			return 0;
		}
	}
	return -1;
}

unsigned char mj_encode(mj_t* card)
{
	return 0;
}

void mj_decode(mj_t* card, unsigned char* x)
{
	if (x && card) {
		*x = 0;
	}
}

void mj_shuffle(mj_t* cards, int len)
{
    int i,n;
    int a,b;
    mj_t temp;
    mj_t *pa, *pb;

    if(!mj || len <= 2)
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
