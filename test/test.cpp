#include <iostream>
#include <stdio.h>
#include <time.h>
#include "card.h"
#include "deck.h"
#include "ddz.h"
#include "qp_misc.h"
#include "ht_log.h"

void test_shuffle();
void test_ddz();
void test_cardtype();
void test_playcard();

using namespace silinan;
using namespace silinan::game;

void log_cards(std::vector<Card>& cards, size_t line_number = 10)
{
    char suit[] = {'?','D','C','H','S','*'};
    char rank[] = {'?','A','2','3','4','5','6','7','8','9','T','J','Q','K','F','Z'};
    for(size_t i = 0; i < cards.size(); ++i){
		g_log << suit[cards[i].suit_] << rank[cards[i].rank_];
        g_log << " ";
        if((i+1) % line_number == 0){
            g_log << std::endl;
        }
    }
	if(cards.size() % line_number != 0)
		g_log << std::endl;
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

    std::string logName = "log.txt";
    g_log.Init(logName, false);

	test_playcard();
	test_cardtype();
	test_ddz();
	test_shuffle();

	return 0;
}

void test_playcard()
{
	silinan::game::DDZ ddz;

	ddz.NewGame();

	silinan::game::Card card;
	silinan::game::CardType cardtype;
	std::vector<Card> cards;
	cards.clear();
	card.rank_ = cdRank4;
	card.suit_ = cdSuitHeart;
	cards.push_back(card);
	cards.push_back(card);
	if(ddz.CanPlayCards(cards, cardtype)){
		std::cout << "play cards:";
		for(size_t i = 0; i < cards.size(); ++i){
			if(i > 0) std::cout << ",";
			std::cout << cards[i].rank_;
		}
		std::cout << std::endl;
	}
}

void test_shuffle()
{
	int count[14][13];

	silinan::game::Deck deck(1, false);

	silinan::game::Card cd;

	deck.RemoveCardOnSuit(silinan::game::cdSuitDiamond);
	deck.RemoveCardOnSuit(silinan::game::cdSuitClub);
	deck.RemoveCardOnSuit(silinan::game::cdSuitHeart);

	std::cout << "Deck's number=" << deck.Number() << std::endl;

	deck.PrintCard();

	memset(count, 0, sizeof(int)*14*13);

	// 循环10000次
	for(int n = 0; n < 10000; ++n){
		for(int i = 0; i < 13; ++i){
			deck.Get(i, cd);
			//std::cout << "cd rank=" << cd.rank_ << std::endl;
			count[cd.rank_][i]++;
		}
		deck.Shuffle();
	}

	for(int i = 1; i <= 13; ++i){
		std::cout << i << " ";
		for(int j = 0; j < 13; ++j){
			std::cout << count[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void test_ddz()
{
	silinan::game::DDZ ddz;

	ddz.NewGame();

	// 发100副牌
	for(size_t n = 0; n < 100; ++n){
		for(size_t i = 0; i < 3; ++i){
			silinan::game::CardPlayer& cp = ddz.GetPlayer(i);
			std::vector<silinan::game::Card>& cds = cp.GetAllCards();
			ddz.Sort(cds);
			cp.PrintCard(17);
			log_cards(cds, 17);
		}
		std::vector<silinan::game::Card>& cds = ddz.GetDZ3();
		silinan::game::print_cards(cds);
		log_cards(cds);
		g_log << std::endl << std::endl;

		ddz.NewGame();
	}
}

// 牌型测试
void test_cardtype()
{
    const char* type_name[] = {"错误","单张","对子","三张","顺子","双顺","三顺",
		"三带一","三带二","四带两单","四带两对","炸弹","天王炸"};

	silinan::game::DDZ ddz;
	silinan::game::Card card;
	silinan::game::CardType cardtype;
	std::vector<Card> cards;

	// 单张测试
	card.rank_ = cdRank4;
	card.suit_ = cdSuitHeart;
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 对子
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 三张
	cards.clear();
	card.rank_ = cdRank5;
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 顺子
	cards.clear();
	card.rank_ = cdRank10;
	cards.push_back(card);
	card.rank_ = cdRankJ;
	cards.push_back(card);
	card.rank_ = cdRankQ;
	cards.push_back(card);
	card.rank_ = cdRankK;
	cards.push_back(card);
	card.rank_ = cdRankAce;
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 炸弹
	cards.clear();
	card.rank_ = cdRank10;
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 双顺
	cards.clear();
	card.rank_ = cdRank10;
	cards.push_back(card);
	cards.push_back(card);
	card.rank_ = cdRank9;
	cards.push_back(card);
	cards.push_back(card);
	card.rank_ = cdRank8;
	cards.push_back(card);
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 三顺
	cards.clear();
	card.rank_ = cdRank10;
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	card.rank_ = cdRank9;
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	//card.rank_ = cdRank5;
	//cards.push_back(card);
	//cards.push_back(card);
	//card.rank_ = cdRank8;
	//cards.push_back(card);
	//cards.push_back(card);

	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 4+2
	cards.clear();
	card.rank_ = cdRankQ;
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	cards.push_back(card);
	card.rank_ = cdRank9;
	cards.push_back(card);
	//cards.push_back(card);
	card.rank_ = cdRank5;
	cards.push_back(card);
	//cards.push_back(card);

	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;

	// 天王炸
	cards.clear();
	card.rank_ = cdRankFJoker;
	card.suit_ = cdSuitJoker;
	cards.push_back(card);
	card.rank_ = cdRankZJoker;
	cards.push_back(card);
	ddz.GetCardType(cards,cardtype);
	std::cout << "cards:";
	for(size_t i = 0; i < cards.size(); ++i){
		if(i > 0) std::cout << ",";
		std::cout << cards[i].rank_;
	}
	std::cout << std::endl;
	std::cout << "card type is " << type_name[cardtype.type] <<
		" and logic value is " << cardtype.logic_value << std::endl;
}
