#include "card_player.h"
#include <stdlib.h>
#include <string.h>
#include "qp_error.h"
/*
int card_player_init(card_player_t* player, int max_card_num)
{
    if (!player)
        return QIPAI_PARAM;
    if (max_card_num <= 0)
        max_card_num = 54;
    player->mycards = hand_new(max_card_num);
    if (!player->mycards)
        return QIPAI_OUTOFMEMORY;
    player->played_cards = hand_new(max_card_num);
    if (!player->played_cards)
        return QIPAI_OUTOFMEMORY;
    player->position = -1;
    player->data = 0;
    return QIPAI_OK;
}

void card_player_clear(qp_player_t* player)
{
    if (player) {
        if (player->mycards)
            hand_free(player->mycards);
        if (player->played_cards)
            hand_free(player->played_cards);
    }
}

void card_player_reset(qp_player_t* player)
{
    int size;

    if(player){
        if(player->mycards){
            player->mycards->num = 0;
            hand_zero(player->mycards);
        }
    }
}

int card_player_draw(card_player_t* player, card_t* card)
{
    int i;
    card_t* p;

    if(!player || !card)
        return HTERR_PARAM;

    if(!player->mycards)
        return HTERR_NOINIT;

    return hand_push(player->mycards, card);
}

int card_player_have(card_player_t* player, card_t* card)
{
    int i;
    card_t *p;

    if(!player || !card)
        return HTERR_PARAM;

    return hand_have(player->mycards, card);
}

int card_player_play(card_player_t* player, card_t* card)
{
    int i;
    card_t *p;

    if(!player || !card)
        return HTERR_PARAM;

    return hand_del(player->mycards, card);
}
*/
