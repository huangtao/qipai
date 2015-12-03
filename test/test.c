#include <stdio.h>
#include <stdlib.h>
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
	printf("\n===start hand type test===\n");
	card_t cards[GP_MAX_CARDS];
	hand_type ht;

	/* 对子 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D3,H3");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* 三带二 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D9,H9,S9,DQ,CQ");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* 三带一 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D7,H9,S9,D9");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* 炸弹 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D9,H9,S9,C9,CQ");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* 顺子 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D9,H8,S7,D6,C5");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* 连对 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D9,H9,S8,D8");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* K炸 */
	cards_from_string(cards, GP_MAX_CARDS,
			"DK,HK,SK,D3");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

	/* 飞机 */
	cards_from_string(cards, GP_MAX_CARDS,
			"D9,H9,S9,D8,H8,S8,D3,H3,S4,C4");
	cards_print(cards, GP_MAX_CARDS, 10);
	gp_handtype(&gp, cards, GP_MAX_CARDS, &ht);
	printf("hand type is %s.\n", gp_htype_name(ht.type));

}
