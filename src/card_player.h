#ifndef _CARD_PLAYER_H
#define _CARD_PLAYER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "card.h"

/* define a card player */
typedef struct card_player_s{
    int position;
    int play_count;

    /* player's hand cards */
    int max_card_num;
    card_t* hand_cards;
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

/*
class CardPlayer
{
public:
    CardPlayer();
    ~CardPlayer();
    
    size_t GetPosition();
    void SetPosition(size_t pos);
    
    // 抓牌
    void DrawCard(Card& card);
    
    // 有这些牌吗
    bool HasCard(Card& card);
    bool HasCards(std::vector<Card>& cards);

	std::vector<Card>& GetAllCards();

	void ClearAllCards();
    
    //// 判断可以出吗
    //bool CanPlayCards(std::vector<Card>& cards);
    
    // 出牌
    void PlayCards(std::vector<Card>& cards);
    
    // 过
    void Pass();

	void PrintCard(int line_number = 10);

    size_t GetPlayCount() const;
private:
    size_t position_;           // 逻辑位置
    size_t play_count_;         // 出牌次数
    std::vector<Card> cards_;   // 手上的牌
};

}

}
*/
#ifdef __cplusplus
}
#endif
#endif