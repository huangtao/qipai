#include "card_player.h"
#include <stdlib.h>
#include <string.h>
#include "ht_lch.h"

int card_player_init(card_player_t* player, int max_card_num)
{
    int size;

    if(!player)
        return HTERR_PARAM;

    if(max_card_num <= 0)
        player->max_card_num = 54;
    else
        player->max_card_num = max_card_num;

    size = player->max_card_num * sizeof(card_t);
    player->hand_cards = (card_t*)malloc(size);

    if(!player->hand_cards)
        return HTERR_OUTOFMEMORY;

    memset(player->hand_cards, 0, size);
    player->position = -1;
    player->play_count = 0;

    return HT_OK;
}

void card_player_clear(card_player_t* player)
{
    if(player){
        if(player->hand_cards)
            free(player->hand_cards);
    }
}

void card_player_reset(card_player_t* player)
{
    int size;

    if(player){
        if(player->hand_cards){
            size = player->max_card_num * sizeof(card_t);
            memset(player->hand_cards, 0, size);
        }
    }
}

int card_player_draw(card_player_t* player, card_t* card)
{
    int i;
    card_t* p;

    if(!player || !card)
        return HTERR_PARAM;

    if(player->max_card_num == 0)
        return HTERR_NOINIT;

    for(i = 0; i < player->max_card_num; ++i){
        p = (player->hand_cards + i);
        if(!p->rank){
            p->rank = card->rank;
            p->suit = card->suit;
            break;
        }
    }

    return HT_OK;
}

int card_player_have(card_player_t* player, card_t* card)
{
    int i;
    card_t *p;

    if(!player || !card)
        return HTERR_PARAM;

    for(i = 0; i < player->max_card_num; ++i){
        p = player->hand_cards + i;
        if(p->rank == card->rank && p->suit == card->suit)
            return 1;
    }

    return 0;
}

int card_player_play(card_player_t* player, card_t* card)
{
    int i;
    card_t *p;

    if(!player || !card)
        return HTERR_PARAM;

    for(i = 0; i < player->max_card_num; ++i){
        p = player->hand_cards + i;
        if(p->rank == card->rank && p->suit == card->suit){
            p->rank = 0;
            p->suit = 0;
            return 1;
        }
    }

    return HTERR_NOCARD;
}

/*
namespace silinan
{

namespace game
{

CardPlayer::CardPlayer()
    :position_(0),
    play_count_(0)
{
}

CardPlayer::~CardPlayer()
{
}

size_t CardPlayer::GetPosition()
{
    return position_;
}

void CardPlayer::SetPosition(size_t pos)
{
    position_ = pos;
}

void CardPlayer::DrawCard(Card& card)
{
    cards_.push_back(card);
}

bool CardPlayer::HasCard(Card& card)
{
    for(size_t i = 0; i < cards_.size(); i++){
        if(card == cards_[i])
            return true;
    }
    return false;
}

bool CardPlayer::HasCards(std::vector<Card>& cards)
{
    std::vector<Card> tmp;
    tmp = cards;
    for(size_t i = 0; i < cards.size(); i++){
        bool find = false;
        for(size_t j = 0; j < tmp.size(); j++){
            if(cards[i] == tmp[j]){
                tmp[j].Clear();
                find = true;
                break;
            }
        }
        if(!find)
            return false;
    }
    return true;
}

std::vector<Card>& CardPlayer::GetAllCards()
{
	return cards_;
}

void CardPlayer::ClearAllCards()
{
	cards_.clear();
    play_count_ = 0;
}

void CardPlayer::PlayCards(std::vector<Card>& cards)
{
    if(cards_.size() == 0)
        return;
    if(!cards_[0].Valid())
    {
        // 占位牌(牌背)
        for(size_t i = 0; i < cards.size(); ++i)
        {
            cards_.pop_back();
            if(cards_.size() == 0)
                return;
        }
        return;
    }

    std::vector<Card> tmp;
    tmp = cards_;
    for(size_t i = 0; i < cards.size(); i++){
        for(size_t j = 0; j < tmp.size(); j++){
            if(cards[i] == tmp[j]){
                tmp[j].Clear();
                break;
            }
        }
    }
    cards_.clear();
    for(size_t i = 0; i < tmp.size(); i++){
        if(tmp[i].Valid())
            cards_.push_back(tmp[i]);
    }
    play_count_++;
}

void CardPlayer::PrintCard(int line_number)
{
	std::cout << "start print " << position_ << " player's cards:" << std::endl;
	print_cards(cards_, line_number);
    std::cout << "end print " << position_ << " player's cards." << std::endl;
}

size_t CardPlayer::GetPlayCount() const
{
    return play_count_;
}

}

}
*/