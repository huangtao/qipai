#include <stdio.h>
#include <time.h>
#include "card.h"
#include "gp.h"

void test_gp();

int main(int argc, char** argv)
{
    srand((unsigned int)time(NULL));

    test_gp();

    return 0;
}

void test_gp()
{
    gp_t gp;

    gp_init(&gp, GP_RULE_ZHUJI, GP_MODE_SERVER, 2);
    gp_start(&gp);
    cards_dump(gp.deck, 54, 10);
    cards_dump(gp.players[0].cards, GP_MAX_CARDS, 10);
    cards_dump(gp.players[1].cards, GP_MAX_CARDS, 10);
    printf("gp game state:%d\n", gp.game_state);
    printf("first player:%d\n", gp.first_player_no);

    gp_sort(gp.players[0].cards, GP_MAX_CARDS);
    cards_dump(gp.players[0].cards, GP_MAX_CARDS, 10);

	/* 牌型判断 */
	card_t cards[GP_MAX_CARDS];
	cards_from_string(cards, GP_MAX_CARDS,
			"D3,H3");
	hand_type ht;
	gp_handtype(cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));
}
