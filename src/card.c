#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ht_lch.h"

static char c_suit[] = {'?','D','C','H','S','*'};
static char c_rank[] = {'?','A','2','3','4','5','6','7','8','9','T','J','Q','K','F','Z'};

void card_init(card_t* card, const char* sn)
{
}

int card_equal(card_t* a, card_t* b)
{
    if(!a || !b)
        return 0;

    if(a->suit == b->suit &&
        a->rank == b->rank)
    {
        return 1;
    }

    return 0;
}

char card_encode(card_t* cd)
{
    char x;

    if(cd)
        x = (cd->suit << 4) | cd->rank;
    else
        x = 0;

    return x;
}

void card_decode(card_t* cd, char x)
{
    if(cd){
        cd->suit = x >> 4;
        cd->rank = x & 0xf;
    }
}

const char* cards_print(card_t cards[], int len, int line_number)
{
    int i;
    char buf[256];
    static char readable[256];

    if (!cards || len > 128) {
        strcpy(buf, "");
        return readable;
    }

    memset(readable, 0, 256 * sizeof(char));
    for(i = 0; i < len; ++i){
        if(cards[i].suit > 5 || cards[i].rank > 15)
            sprintf(buf, "%c%c ", '-', '-');
        else {
            if (cards[i].rank == 10)
                sprintf(buf, "%c10 ", c_suit[cards[i].suit]);
            else
                sprintf(buf, "%c%c ", c_suit[cards[i].suit], c_rank[cards[i].rank]);
        }
        if((i+1) % line_number == 0){
            strcat(buf, "\n");
        }
        strcat(readable, buf);
    }

    return readable;
}

char card_suit_char(card_t* card)
{
	char c;

	c = c_suit[0];
	if(card){
		if(card->suit >=0 || card->suit <= 5){
			c = c_suit[card->suit];
		}
	}

	return c;
}

const char* card_rank_str(card_t* card)
{
    static char str[4];

    if(card){
        if(card->rank >=0 || card->rank <= 15){
            if (card->rank == 10)
                strcpy(str, "10");
            else
                sprintf(str, "%c", c_rank[card->rank]);
        }
    }
    else {
        strcpy(str, "?");
    }

    return str;
}
