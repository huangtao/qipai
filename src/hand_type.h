#ifndef _HAND_TYPE_H
#define _HAND_TYPE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "hand.h"

/*
 * common hand-type estimate 
 * 0 is not hand-type else is true
 */
int ht_single(hand_t* hand);             /* single card type (6) */
int ht_double(hand_t* hand);             /* double card type (66) */
int ht_three(hand_t* hand);              /* three (666) */
int ht_three_p2(hand_t* hand);           /* three plus double (666+77) */
int ht_bomb(hand_t* hand);               /* bomb (6666) */
int ht_straight(hand_t* hand);           /* straight line (56789) 5+ */
int ht_double_straight(hand_t* hand);    /* double straight (6677) 2+ */
int ht_three_straight(hand_t* hand);     /* three straight (666777) 2+ */

#ifdef __cplusplus
}
#endif
#endif
