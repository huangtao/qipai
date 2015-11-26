#include "card_player.h"
#include <stdlib.h>
#include <string.h>
#include "ht_lch.h"

int card_player_init(card_player_t* player, int max_card_num)
{
    int size;

    if (!player)
        return HTERR_PARAM;
    if (max_card_num <= 0)
        max_card_num = 54;
    player->mycards = hand_new(max_card_num);
    if (!player->mycards)
        return HTERR_OUTOFMEMORY;
    player->played_cards = hand_new(max_card_num);
    if (!player->played_cards)
        return HTERR_OUTOFMEMORY;
    player->position = -1;
    player->data = 0;
    return HT_OK;
}

void card_player_clear(card_player_t* player)
{
    if (player) {
        if (player->mycards)
            hand_free(player->mycards);
        if (player->played_cards)
            hand_free(player->played_cards);
    }
}

void card_player_reset(card_player_t* player)
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

