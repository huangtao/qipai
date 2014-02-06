/*
* code:huangtao117@gmail.com
*/

#ifndef _CARD_PLAYER_H
#define _CARD_PLAYER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "hand.h"

/* define a card player */
typedef struct card_player_s{
    int position;

    /* player's cards */
    hand_t* mycards;
    
    /* player's private data */
    void* data;
}card_player_t;

/* init a card player */
int card_player_init(card_player_t* player, int max_card_num);
/* clear a card player */
void card_player_clear(card_player_t* player);
/* reset a player's cards */
void card_player_reset(card_player_t* player);
/* draw a card */
int card_player_draw(card_player_t* player, card_t* card);
/* have this card */
int card_player_have(card_player_t* player, card_t* card);
/* play a card */
int card_player_play(card_player_t* player, card_t* card);

#ifdef __cplusplus
}
#endif
#endif
