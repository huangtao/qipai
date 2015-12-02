/*
* code:huangtao117@gmail.com
*/

#ifndef _CARD_PLAYER_H
#define _CARD_PLAYER_H
#ifdef __cplusplus
extern "C" {
#endif

/* define a card player */
typedef struct card_player_s{
    int level;
    int state;
    int position;
    uint64_t score;
    uint64_t gold;
    int param1;
    int param2;

    /* player's private data */
    void* data;
}card_player_t;

#ifdef __cplusplus
}
#endif
#endif
