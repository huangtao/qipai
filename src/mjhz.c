#include "mjhz.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "card_sort.h"

typedef struct analyse_r_s{
    int n1;
    int v1[MJHZ_MAX_CARDS];
    int n2;
    int v2[MJHZ_MAX_CARDS/2];
    int n3;
    int v3[MJHZ_MAX_CARDS/3];
    int n4;
    int v4[MJHZ_MAX_CARDS/4];
}analyse_r;

void mjhz_init(mjhz_t* mj, int mode, int player_num)
{
    int i,j,n;

    if(!mj)
        return;
    memset(mj, 0, sizeof(mjhz_t));

    if (mode == MJHZ_MODE_SERVER)
        mj->mode = MJHZ_MODE_SERVER;
    else
        mj->mode = MJHZ_MODE_CLIENT;
    mj->player_num = player_num;
    mj->game_state = MJHZ_GAME_END;
    mj->inning = 0;
    mj->turn_time = 30;

	/* 杭州麻将使用136张牌 */
	/* 序数牌 */
	n = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 10; j++) {
			mj->deck[n].suit = mjSuitWan;
			mj->deck[n].sign = mj1 + j;
			n++;
			mj->deck[n].suit = mjSuitSuo;
			mj->deck[n].sign = mj1 + j;
			n++;
			mj->deck[n].suit = mjSuitTong;
			mj->deck[n].sign = mj1 + j;
			n++;
		}
	}
	/* 字牌 */
	for (i = 0; i < 4; i++) {
		mj->deck[n].suit = mjSuitFeng;
		mj->deck[n].sign = mjEast;
		n++;
		mj->deck[n].suit = mjSuitFeng;
		mj->deck[n].sign = mjSouth;
		n++;
		mj->deck[n].suit = mjSuitFeng;
		mj->deck[n].sign = mjWest;
		n++;
		mj->deck[n].suit = mjSuitFeng;
		mj->deck[n].sign = mjNorth;
		n++;
		mj->deck[n].suit = mjSuitZFB;
		mj->deck[n].sign = mjZhong;
		n++;
		mj->deck[n].suit = mjSuitZFB;
		mj->deck[n].sign = mjFa;
		n++;
		mj->deck[n].suit = mjSuitZFB;
		mj->deck[n].sign = mjBai;
		n++;
	}
    mj->deck_all_num = n;

    for(i = 0; i < MJHZ_MAX_PLAYERS; i++){
        mj->players[i].position = i;
    }
}

void mjhz_start(mjhz_t* mj)
{
    int i,direct,m,n;

    if(!mj)
        return;
    mj->round = 0;
    mj->game_state = MJHZ_GAME_PLAY;
    mj->inning++;

	mj->dice1 = rand() % 6 + 1;
	mj->dice2 = rand() % 6 + 1;

	/* 白板是财神 */
	mj->mammon.suit = mjSuitZFB;
	mj->mammon.sign = mjBai;

	mj->last_played_mj.suit = 0;
	mj->last_played_mj.sign = 0;

    if (mj->mode == MJHZ_MODE_SERVER) {
		/* 洗牌 */
		mj_shuffle(mj->deck, mj->deck_all_num);
		/* 计算起手牌位置 */
		direct = (mj->banker_no + MJHZ_MAX_PLAYERS
			- (mj->dice1 + mj->dice2) % MJHZ_MAX_PLAYERS)
			% MJHZ_MAX_PLAYERS;
		mj->deck_deal_index = direct * 17 * 2 
			+ (mj->dice1 + mj->dice2) * 2;
		mj->deck_deal_gang = mj->deck_deal_index - 1; /* 杠抓牌 */
		mj->deck_deal_end = (mj->deck_deal_index + mj->deck_all_num
				- 20) % mj->deck_all_num;
		mj->deck_valid_num = mj->deck_all_num - 20;

		/* 顺时针,每人抓12张,庄家先抓 */
		m = n = 0;
		for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
			direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
			memcpy(mj->players[direct].cards + m, mj->deck + n, 4 * sizeof(mjpai_t));
			n += 4;
		}
		m += 12;
		/* 庄家跳牌2张,其他人一张 */
		mj->players[mj->banker_no].cards[m].suit = mj->deck[n].suit;
		mj->players[mj->banker_no].cards[m].sign = mj->deck[n].sign;
		mj->players[mj->banker_no].cards[m+1].suit = mj->deck[n+4].suit;
		mj->players[mj->banker_no].cards[m+1].sign = mj->deck[n+4].sign;
		n++;
		for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
			if (i == mj->banker_no) continue;
			direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
			mj->players[direct].cards[m].suit = mj->deck[n].suit;
			mj->players[direct].cards[m].suit = mj->deck[n].sign;
		}
        /* the first player */
        mj->first_player_no = mj->banker_no;
        mj->curr_player_no = mj->first_player_no;
    } else {
        for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
            memset(mj->players[i].cards, 0, sizeof(mjpai_t) * MJHZ_MAX_CARDS);
            memset(mj->players[i].cards_played, 0, sizeof(mjpai_t) * MJHZ_MAX_CARDS);
        }
		mj->banker_no = 0;
        mj->first_player_no = 0;
        mj->curr_player_no = 0;
		mj->last_played_no = 0;
    }
}

/* 财神 万 索 筒 风 */
void mjhz_sort(mjhz_t* mj, mjpai_t* cards, int len)
{
	int i,j;
	int exchange;
	mjpai_t temp;
	mjpai_t *pa, *pb;

	if (!mj || !cards || len < 2)
		return;

	/* 选择排序 */
	for (i = 0; i < len - 1; ++i) {
		exchange = 0;
		pa = cards + i;
		for (j = i + 1; j < len; ++j) {
			pb = cards + j;
			if (pb->suit == mjSuitZFB && pb->sign == mjBai) {
				if (pa->suit != mjSuitZFB || pa->sign != mjBai) {
					exchange = 1;
					pa = pb;
				}
			} else if (pb->suit == pa->suit) {
				if (pb->sign < pa->sign) {
					exchange = 1;
					pa = pb;
				}
			} else {
				if (pb->suit < pa->suit) {
					exchange = 1;
					pa = pb;
				}
			}
		}
		if (exchange) {
			pb = cards + i;
			temp.suit = pb->suit;
			temp.sign = pb->sign;
			pb->suit = pa->suit;
			pb->sign = pa->sign;
			pa->suit = temp.suit;
			pb->sign = temp.sign;
		}
	}
}

const char* mjhz_hu_name(mjhz_hu_t* hu)
{
    static char* hu_name[] = {
        "MJHZ_ERROR",
        "MJHZ_PING",	/* 平胡 */
        "MJHZ_DUI7",	/* 7对子 */
        "MJHZ_LONG",	/* 一条龙 */
        "MJHZ_QYS"		/* 清一色*/
    };

    return hu_name[0];
}

/* 抓牌 */
void mjhz_draw(mjhz_t* mj, int is_gang)
{
}

/* 出牌 */
int mjhz_play(mjhz_t* mj, int player_no, mjpai_t* card)
{
    int i,n,flag;
    mjpai_t* cd;

    if(!mj || !card)
        return -1;

    if (mj->game_state != MJHZ_GAME_PLAY) {
        if (mj->debug)
            printf("play cards but game state not play.\n");
        return -2;
    }
    if (player_no != mj->curr_player_no) {
        if (mj->debug)
            printf("play cards but not this no.\n");
        return -3;
    }

	/* 有效检查并删除这张牌 */
    if (mj->mode == MJHZ_MODE_SERVER) {
		n = -1;
		cd = mj->players[player_no].cards;
        for (i = 0; i < MJHZ_MAX_CARDS; ++i){
			if (cd->suit == card->suit && cd->sign == card->sign) {
				n = i;
				break;
			}
			cd++;
		}
		if (n == -1) {
            if (mj->debug) {
                printf("play cards but player hasn't this card.\n");
            }
			return -4;
        } else {
			mj->players[player_no].cards[n].suit = 0;
			mj->players[player_no].cards[n].sign = 0;
		}
    } else {
		cd = mj->players[player_no].cards;
		for (i = 0; i < MJHZ_MAX_CARDS; ++i) {
			if (cd->suit == card->suit && cd->sign == card->sign) {
				cd->suit = 0;
				cd->sign = 0;
				break;
			}
			cd++;
		}
	}
	mj->last_played_mj.suit = card->suit;
	mj->last_played_mj.sign = card->sign;
	mj->last_played_no = player_no;
	mj_trim(mj->players[player_no].cards, MJHZ_MAX_CARDS);
	/* 判定吃碰杠胡 */
	for (i = 0; i < mj->player_num; ++i) {
		if (i != player_no) {
			mj->players[i].can_hu = mjhz_can_hu(mj, i);
			mj->players[i].can_gang = mjhz_can_gang(mj, i);
			mj->players[i].can_peng = mjhz_can_peng(mj, i);
			mj->players[i].can_chi = mjhz_can_chi(mj, i);
		} else {
			mj->players[i].can_hu = 0;
			mj->players[i].can_gang = 0;
			mj->players[i].can_peng = 0;
			mj->players[i].can_chi = 0;
		}
	}

	/* 无人吃碰胡则摸牌 */
	flag = 0;
	for (i = 0; i < mj->player_num; ++i) {
		if (mj->players[i].can_hu ||
				mj->players[i].can_gang ||
				mj->players[i].can_peng ||
				mj->players[i].can_chi) {
			flag = 1;
			break;
		}
	}
	if (!flag) {
		mjhz_next_player(mj);
		mjhz_draw(mj, 0);
	}

    return 1;
}

/*
 * 返回吃牌信息
 * 0 : 不能吃
 * 1 : **o
 * 2 : *o*
 * 4 : o**
 */
int mjhz_can_chi(mjhz_t* mj, int player_no)
{
	int i,chi_info;
	int num[10];
	mjpai_t* p;
	int pos1,pos2,pos3;

	if (!mj)
		return 0;
	if (player_no >= mj->player_num)
		return 0;
	if (mj->curr_player_no == player_no)
		return 0;
	/* 只能吃万、索、筒子 */
	if (mj->last_played_mj.suit == mj->mammon.suit && 
			mj->last_played_mj.sign == mj->mammon.sign) {
		return 0;
	}
	if (mj->last_played_mj.suit != mjSuitWan ||
			mj->last_played_mj.suit != mjSuitSuo ||
			mj->last_played_mj.suit != mjSuitTong) {
		return 0;
	}

	/* 同类型序数牌计数 */
	memset((void*)num, 0, sizeof(int) * 9);
	p = mj->players[player_no].cards;
	for (i = 0; i < MJHZ_MAX_CARDS; ++i) {
		if (p->suit == mj->last_played_mj.suit) {
			num[p->sign]++;
		}
		p++;
	}
	chi_info = 0;

	for (i = 1; i <= 9; ++i) {
		if ( i != mj->last_played_mj.sign)
			continue;
		pos1 = pos2 = pos3 = 0;
		if (i == 1) {
			pos1 = 1;
		} else if (i == 2) {
			pos1 = pos2 = 1;
		} else if (i == 8) {
			pos2 = pos3 = 1;
		} else if (i == 9) {
			pos3 = 1;
		} else {
			pos1 = pos2 = pos3 = 1;
		}
		if (pos1) {
			if (num[i+1] > 0 && num[i+2] > 0)
				chi_info &= 0x01;
		}
		if (pos2) {
			if (num[i-1] > 0 && num[i+1] > 0)
				chi_info &= 0x02;
		}
		if (pos3) {
			if (num[i-1] > 0 && num[i-2] > 0)
				chi_info &= 0x04;
		}
	}
	
    return chi_info;
}

int mjhz_can_peng(mjhz_t* mj, int player_no)
{
    int i;
	int num;
	mjpai_t* p;

	if (!mj)
		return 0;
	if (player_no >= mj->player_num)
		return 0;
	if (mj->curr_player_no == player_no)
		return 0;
	if (mj->last_played_mj.suit == mj->mammon.suit && 
			mj->last_played_mj.sign == mj->mammon.sign) {
		return 0;
	}

	p = mj->players[player_no].cards;
	for (i = 0; i < MJHZ_MAX_CARDS; ++i) {
		if (p->suit == mj->last_played_mj.suit &&
                p->sign == mj->last_played_mj.sign) {
			num++;
		}
		p++;
	}
	if (num >= 2)
        return 1;
    else
        return 0;
}

int mjhz_can_gang(mjhz_t* mj, int player_no)
{
    int i,num,x;
    int js[43];
	mjpai_t* p;

	if (!mj)
		return 0;
	if (player_no >= mj->player_num)
		return 0;
	if (mj->curr_player_no == player_no)
		return 0;

    if (mj->last_played_mj.suit > 0 && 
            mj->last_played_mj.sign > 0) {
        /* 明杠(杠打出的牌) */
        if (mj->last_played_mj.suit == mj->mammon.suit &&
                mj->last_played_mj.sign == mj->mammon.sign) {
            return 0;
        }
        num = 0;
        p = mj->players[player_no].cards;
        for (i = 0; i < MJHZ_MAX_CARDS; ++i) {
            if (p->suit == mj->last_played_mj.suit &&
                    p->sign == mj->last_played_mj.sign) {
                num++;
            }
            p++;
        }
        if (num >= 3)
            return 1;
        else
            return 0;
    } else {
        /* 暗杠或者加杠 */
        memset(js, 0, sizeof(int) * 43);
        p = mj->players[player_no].cards;
        for (i = 0; i < MJHZ_MAX_CARDS; ++i,p++) {
            if (p->suit == 0 || p->sign == 0)
                continue;
            x = mjpai_encode(p);
            if (x >= 43) continue;
            js[x]++;
        }
        /* 有没有暗杠 */
        for (i = 1; i < 43; i++) {
            if (js[i] == 4) {
                return 1;
            }
        }
    }
    return 0;
}

int mjhz_can_hu(mjhz_t* mj, int player_no)
{
    return 0;
}

int mjhz_chi(mjhz_t* mj, int player_no)
{
	return 0;
}

int mjhz_peng(mjhz_t* mj, int player_no)
{
	return 0;
}

int mjhz_gang(mjhz_t* mj, int player_no)
{
	return 0;
}

int mjhz_hu(mjhz_t* mj, int player_no)
{
	return 0;
}

/* 逆时针*/
void mjhz_next_player(mjhz_t* mj)
{
    if (!mj)
        return;
    mj->curr_player_no--;
    if (mj->curr_player_no < 0)
        mj->curr_player_no += mj->player_num;
}

void mjhz_dump(mjhz_t* mj)
{
    if(!mj)
        return;

    printf("player number:%d\n", mj->player_num);
	printf("mamon:\n");
	printf("%s\n", mjpai_string(&mj->mammon));

    /* dump player's mj pai */
    printf("players mj cards:\n");
    printf("%s\n",
		mj_string(mj->players[0].cards, MJHZ_MAX_CARDS, 10));
	printf("%s\n",
		mj_string(mj->players[1].cards, MJHZ_MAX_CARDS, 10));
    if (mj->player_num > 2) {
		printf("%s\n",
			mj_string(mj->players[2].cards, MJHZ_MAX_CARDS, 10));
		printf("%s\n",
			mj_string(mj->players[3].cards, MJHZ_MAX_CARDS, 10));
    }

    printf("last mj pai:\n");
    printf("%s\n", mjpai_string(&mj->last_played_mj));

    printf("current player no is %d\n", mj->curr_player_no);
}
