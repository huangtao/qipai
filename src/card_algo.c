#include "card_algo.h"
#include <stdlib.h>
#include <string.h>

//static int table_suit[7] = { 0, 1, 2, 3, 4, 5, 6 };
static int rank_to_logic[17] = { 0, 14, 15, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 18 };
static int logic_to_rank[19] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1, 2, 14, 15, 16 };

void cards_sort(card_t* cards, int len)
{
    int i,j;
    int exchange;
    card_t temp;
    card_t *pa, *pb;

    if (!cards || len < 2)
        return;

    /* select sort */
    for (i = 0; i < len - 1; ++i) {
        exchange = 0;
        pa = cards + i;
        for (j = i + 1; j < len; ++j) {
            pb = cards + j;
            if (rank_to_logic[pa->rank] < rank_to_logic[pb->rank]) {
                exchange = 1;
                pa = pb;
            } else if (rank_to_logic[pa->rank] == rank_to_logic[pb->rank]) {
                if (pa->suit < pb->suit) {
                    exchange = 1;
                    pa = pb;
                }
            }
        }
        if (exchange) {
            pb = cards + i;
            memcpy(&temp, pb, sizeof(card_t));
            memcpy(pb, pa, sizeof(card_t));
            memcpy(pa, &temp, sizeof(card_t));
        }
    }
}
/*
int card_compare(const void* a, const void* b)
{
    card_t *card1, *card2;

    card1 = (card_t*)a;
    card2 = (card_t*)b;
    if(!card1 || !card2)
        return 0;

    if (card1->rank > 16 || card1->suit > 6)
        return 0;
    if (card2->rank > 16 || card2->suit > 6)
        return 0;

    if(table_rank[card1->rank] < table_rank[card2->rank])
        return 1;

    if(table_rank[card1->rank] > table_rank[card2->rank])
        return -1;

    if(table_rank[card1->rank] == table_rank[card2->rank]){
        if(table_suit[card1->suit] < table_suit[card2->suit])
            return 1;
        if(table_suit[card1->suit] > table_suit[card2->suit])
            return -1;

        return 0;
    }

    return 0;
}

void cards_sort(card_t* cards, int len)
{
    if(!cards || len <= 0)
        return;

    qsort(cards, len, sizeof(card_t), card_compare);
}
*/
void cards_analyse(card_t* cards, int len, cd_analyse* result)
{
	int i,v;

	if (!cards || !result || len == 0)
		return;

	memset(result, 0, sizeof(cd_analyse));
    result->raw_cards = cards;
    result->raw_max = len;
	result->valid_num = cards_num(cards, len);
	for (i = 0; i < len; ++i) {
		if (cards->id > 0) {
			v = card_logic(cards);
			result->count[v]++;
		}
		cards++;
	}
	for (i = 0; i < (CD_MAX_LOGIC + 1); ++i) {
		switch (result->count[i]) {
			case 1:
				result->num_1++;
				break;
			case 2:
				result->num_2++;
				break;
			case 3:
				result->num_3++;
				break;
			case 4:
				result->num_4++;
				break;
			case 5:
				result->num_5++;
				break;
			case 6:
				result->num_6++;
				break;
			case 7:
				result->num_7++;
				break;
			case 8:
				result->num_8++;
				break;
		}
	}
}

/*
void rank_bucket(hand_t* hand, int x[])
{
    int i,v;
    card_t* p;

    if(!hand || !hand->cards)
        return;
    for(i = 0; i < hand->num; ++i){
        p = hand->cards + i;
        v = card_logicvalue(p);
        x[v]++;
    }
}
*/

/**
 * bucket a hand
 * return hand suit's number
 */
void cards_bucket(card_t* cards, int len, cd_bucket x[])
{
    int i,v;

    if (!cards || len <= 0)
        return;
    for (i = 0; i < len; ++i) {
        v = rank2logic(cards->rank);
        x[v].rank = cards->rank;
        if (cards->suit == cdSuitSpade) {
            x[v].num_spade++;
        }
        else if (cards->suit == cdSuitHeart) {
            x[v].num_heart++;
        }
        else if (cards->suit == cdSuitClub) {
            x[v].num_club++;
        }
        else if (cards->suit == cdSuitDiamond) {
            x[v].num_diamond++;
        } else {
            x[v].num_joker++;
        }
        cards++;
    }
}

int get_bucket_suit(cd_bucket* item)
{
    if (item) {
        if (item->num_spade)
            return cdSuitSpade;
        else if (item->num_heart)
            return cdSuitHeart;
        else if (item->num_club)
            return cdSuitClub;
        else if (item->num_diamond)
            return cdSuitDiamond;
        else if (item->num_joker)
            return cdSuitJoker;
    }

    return cdSuitNone;
}

int cards_have_rank(int rank, int x[], int size)
{
    int i;

    for (i = 0; i < size; ++i) {
        if (x[i] == rank)
            return 1;
    }

    return 0;
}

int card_logicvalue(card_t* card)
{
    if (!card)
        return 0;

    return rank2logic(card->rank);
}

int rank2logic(int rank)
{
    if (rank >= 17) return 0;

    return rank_to_logic[rank];
}

int logic2rank(int logic)
{
    if (logic >= 19)
        return 0;

    return logic_to_rank[logic];
}
