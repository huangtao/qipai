/*
*
* code: huangtao117@gmail.com
*
*/
#ifndef _HAND_H
#define _HAND_H
#ifdef __cplusplus
extern "C" {
#endif

#include "card.h"

/*
 * represent a hand or a deck
 */
typedef struct hand_s{
    int num;        	/* current valid data number of array */
    int max_size;   	/* maximum number of array */
    card_t* cards;  	/* card array */
    int type;			/* hand type */
    card_t type_card;	/* max card */
    int param;          /* user data */
}hand_t;

/* for hand type calc */
typedef struct hand_type_s{
    int type;           /* type enum */
    int num;            /* number */
    int param1;	        /* logic value */
    int param2;
    int param3;
}hand_type;

hand_t* hand_new(int max_size);
void hand_free(hand_t* hand);
void hand_zero(hand_t* hand);
int hand_num(hand_t* hand);
hand_t* hand_clone(hand_t* hand);
void hand_copy(hand_t* src, hand_t* dest);
/* initialize hand from a string */
void hand_from_string(hand_t* hand, char* string);
card_t* hand_get(hand_t* hand, int n);
/* hand have this card */
int hand_have(hand_t* hand, card_t* card);
int hand_rank_num(hand_t* hand, int rank);
int hand_suit_num(hand_t* hand, int suit);
/* push/pop a card to hand and return card num */
int hand_push(hand_t* hand, card_t* card);
int hand_pop(hand_t* hand, card_t* card);
/* delete a card from hand */
int hand_del(hand_t* hand, card_t* card);
int hand_trim(hand_t* hand);
/* print readable format */
void hand_print(hand_t* hand, int line_number);
void hand_dump(hand_t* hand, int line_number);
const char* card_text(card_t* card);

#ifdef __cplusplus
}
#endif
#endif
