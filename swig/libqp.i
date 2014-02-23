/* file : libqp.i */
%module libqp
%{
#include "../src/ht_str.h"
#include "../src/card.h"
#include "../src/hand.h"
#include "../src/deck.h"
#include "../src/sort_card.h"
#include "../src/card_player.h"
#include "../src/ddz.h"
#include "../src/gp.h"
%}

%include "../src/ht_str.h"
%include "../src/card.h"
%include "../src/hand.h"
%include "../src/deck.h"
%include "../src/sort_card.h"
%include "../src/card_player.h"
%include "../src/ddz.h"
%include "../src/gp.h"

/* helper function */
%inline %{
card_player_t* ddz_get_player(ddz_t* ddz, int player_no){
    if(ddz && player_no >= 0 && player_no < 3)
        return &(ddz->players[player_no]);
    else
        return 0;
}
hand_t* ddz_get_player_hand(card_player_t* player){
    if(player)
        return player->mycards;
    else
        return 0;
}
card_t* ddz_get_di(ddz_t* ddz, int index){
    if(ddz && index >= 0 && index < 3)
        return &(ddz->di[index]);
    else
        return 0;
}
int ddz_landlord_call(ddz_t* ddz){
    if(ddz)
        return ddz->call[ddz->landlord_no];
    else
        return 0;
}
%}


