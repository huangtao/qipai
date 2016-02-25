#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char c_suit[] = {'0','D','C','H','S','*','?'};
static char c_rank[] = {'0','A','2','3','4','5','6','7','8','9','T','J','Q','K','S','B','?'};

int card_equal(card_t* a, card_t* b)
{
    if (!a || !b)
        return 0;

    if (a->id == b->id)
        return 1;
	else
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

void n55_to_card(unsigned char cd, card_t* card)
{
    if (card && cd > 0) {
        if (cd >= 55) {
            card->id = CD_ID_UNKNOW;
            card->rank = cdRankUnknow;
            card->suit = cdSuitUnknow;
        } else {
            card->id = cd;
            card->rank = cdRankAce + (cd - 1) % 13;
            card->suit = cdSuitDiamond + (cd - 1) / 13;
        }
    }
}

unsigned char card_to_n55(card_t* card)
{
    if (!card)
        return 0;
    return card->id;
}

const char* card_to_string(card_t* card)
{
    static char readable[8];

    memset(readable, 0, 8);
    if (card)
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

int cards_num(card_t* cards, int len)
{
    int i,n;

    n = 0;
    if (cards && len > 0) {
        for (i = 0; i < len; ++i) {
            if (cards->id > 0)
                n++;
			cards++;
        }
    }

    return n;
}

int cards_have(card_t* cards, int len, card_t* card)
{
    int i,n;

    if (!cards || len <= 0 || !card)
        return 0;

	n = 0;
    for (i = 0; i < len; i++) {
        if (cards->id == card->id)
            n++;
		cards++;
    }

    return n;
}

int cards_rank_num(card_t* cards, int len, int rank)
{
    int i,n;

    if(!cards || len <= 0)
        return 0;

    n = 0;
    for (i = 0; i < len; ++i) {
        if (cards->rank == rank)
            n++;
		cards++;
    }

    return n;
}

int cards_suit_num(card_t* cards, int len, int suit)
{
    int i,n;

    if(!cards || len <= 0)
        return 0;

    n = 0;
    for (i = 0; i < len; ++i) {
        if (cards->suit == suit)
            n++;
		cards++;
    }

    return n;
}

int cards_add(card_t* cards, int len, card_t* card)
{
	int i;

	if (!cards || len <= 0 || !card)
		return -1;

	for (i = 0; i < len; ++i) {
		if (cards->id == 0) {
			cards->id = card->id;
			cards->rank = card->rank;
			cards->suit = card->suit;
			return 0;
		}
		cards++;
	}

	return 0;
}

int cards_del(card_t* cards, int len, card_t* card)
{
    int i,uflag;
    card_t* p;

    if (!cards || len <= 0 || !card)
        return -1;

    uflag = 0;
    p = cards;
    for (i = 0; i < len; i++) {
        if (p->id == card->id) {
            p->suit = 0;
            p->rank = 0;
            break;
        }
        if (p->rank == cdRankUnknow)
            uflag = 1;
        p++;
    }
    if (i >= len && uflag > 0) {
        p = cards;
        for (i = 0; i < len; i++) {
            if (p->id == CD_ID_UNKNOW) {
                p->rank = 0;
                p->suit = 0;
                break;
            }
            p++;
        }
    }

    return 0;
}

int cards_trim(card_t* cards, int len)
{
    int i,num;
    int byte_size;
    card_t *tmp_cards,*p1,*p2;

    if (!cards || len <= 1)
        return -1;

    byte_size = len * sizeof(card_t);
    tmp_cards = (card_t*)malloc(byte_size);
    if (!tmp_cards)
        return -2;
    memset((void*)tmp_cards, 0, byte_size);

    num = 0;
    p1 = tmp_cards;
    p2 = cards;
    for (i = 0; i < len; ++i) {
        if (p2->id > 0) {
			p1->id = p2->id;
            p1->suit = p2->suit;
            p1->rank = p2->rank;
            num++;
            p1++;
        }
        p2++;
    }
    memcpy((void*)cards, (const void*)tmp_cards, byte_size);
    free((void*)tmp_cards);

    return num;
}

void cards_print(card_t* cards, int len, int line_number)
{
    int i,n;

    if (!cards || len <= 0)
        return;

	n = 0;
    for (i = 0; i < len; ++i) {
		if (cards->id > 0) {
            printf("%s ", card_to_string(cards));
			n++;
			if (n % line_number == 0) {
				printf("\n");
			}
		}
		cards++;
    }
    if (n % line_number != 0)
		printf("\n");
}

void cards_dump(card_t* cards, int len, int line_number)
{
    int i;

    if (!cards || len <= 0) {
        printf("hand or cards is invalid pointer!\n");
        return;
    }

    for (i = 0; i < len; ++i) {
        printf("%s ", card_to_string(cards));
        if ((i+1) % line_number == 0) {
            printf("\n");
        }
		cards++;
    }
    if ((i+1) % line_number != 0)
        printf("\n");
}

void cards_remove_rank(card_t* cards, int len, int rank)
{
    int i;

    if (!cards || len == 0)
        return;
    for (i = 0; i < len; ++i) {
        if (cards->rank == rank) {
            cards->rank = 0;
            cards->suit = 0;
        }
        cards++;
    }
}

void cards_remove_suit(card_t* cards, int len, int suit)
{
    int i;

    if (!cards || len == 0)
        return;
    for (i = 0; i < len; ++i) {
        if(cards->suit == suit) {
            cards->rank = 0;
            cards->suit = 0;
        }
        cards++;
    }
}

/* string's format must be "DA,D2,..." */
int cards_from_string(card_t* cards, int len, const char* str)
{
    int len_str,n;
	int idx,k;
	const char* c;

    if (!cards || !str)
        return 0;
    len_str = strlen(str);
	if (len_str < 2)
        return 0;
    memset(cards, 0, sizeof(card_t) * len);
	idx = 0;
	n = len_str;
	c = str;
	k = 0;
	do {
		card_from_string(cards, c);
		k++;
		idx += 2;
		if (idx < len_str)
			idx++;
		else
			break;
		c = str + idx;
		cards++;
		if (k >= len)
			break;
		n--;
	} while (n);

	return k;
}

const char* cards_to_string(card_t* cards, int len)
{
    static char str[512];
    int i;
    char temp[32];

    strcpy(str, "");
    if (!cards || len <= 0) {
        return str;
    }

    for (i = 0; i < len; ++i) {
        sprintf(temp, "%s ", card_to_string(cards));
        strcat(str, temp);
        cards++;
    }
    return str;
}

/*
 * for deck operation
 */
int deck_init(card_t* cards, int len)
{
    int fu;
    card_t* p;
    int i,j,k;

    if (!cards || len <= 0)
        return -1;

    fu = len / 54;
    if (fu == 0)
        return -2;

    p = cards;
    for (i = 0; i < fu; ++i) {
        for (j = cdSuitDiamond; j <= cdSuitSpade; ++j) {
            for(k = cdRankAce; k <= cdRankK; ++k) {
				p->id = (j - 1) * 13 + k;
                p->rank = k;
                p->suit = j;
                p++;
            }
        }
		p->id = CD_ID_JS;
        p->rank = cdRankSJoker;
        p->suit = cdSuitJoker;
        p++;
		p->id = CD_ID_JB;
        p->rank = cdRankBJoker;
        p->suit = cdSuitJoker;
        p++;
    }

    return 0;
}

void deck_shuffle(card_t* cards, int num)
{
    int i,n;
    int a,b;
    card_t* pa;
    card_t* pb;
    card_t temp;

    if (!cards || num == 0)
        return;

    n = 1000 + rand() % 50;
    for (i = 0; i < n; ++i) {
        a = rand() % num;
        b = rand() % num;
        if (a != b) {
            pa = cards + a;
            pb = cards + b;

			temp.id = pa->id;
            temp.rank = pa->rank;
            temp.suit = pa->suit;

			pa->id = pb->id;
            pa->rank = pb->rank;
            pa->suit = pb->suit;
            
			pb->id = temp.id;
			pb->rank = temp.rank;
            pb->suit = temp.suit;
        }
    }
}

