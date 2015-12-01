#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ht_lch.h"

static char c_suit[] = {'0','D','C','H','S','*','?'};
static char c_rank[] = {'0','A','2','3','4','5','6','7','8','9','T','J','Q','K','S','B','?'};

int card_equal(card_t* a, card_t* b)
{
    if (!a || !b)
        return 0;

    if (a->suit == b->suit &&
            a->rank == b->rank) {
        return 1;
    }

    return 0;
}

char card_encode(card_t* cd)
{
    char x;

    if (cd)
        x = (cd->suit << 4) | cd->rank;
    else
        x = 0;

    return x;
}

void card_decode(card_t* cd, char x)
{
    if (cd) {
        cd->suit = x >> 4;
        cd->rank = x & 0xf;
    }
}

const char* card_to_string(card_t* card)
{
    static char readable[8];

    memset(readable, 0, 8);
    if(!card)
        return 0;

    readable[2] = 0;
    sprintf(readable, "%c%c", c_suit[card->suit], c_rank[card->rank]);

    return readable;
}

/* string's format must be "DA","D2",... */
void card_from_string(card_t* card, const char* str)
{
    int i;

    if (!card || !str)
        return;
    if (strlen(str) < 2)
        return;

    card->rank = card->suit = 0;
    for (i = 0; i < 7; i++) {
        if (*str == c_suit[i]) {
            card->suit = i;
            break;
        }
    }
    for (i = 0; i < 17; i++) {
        if (*(str+1) == c_rank[i]) {
            card->rank = i;
            break;
        }
    }
}

