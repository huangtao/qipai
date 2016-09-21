#include "mjhz.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mj_algo.h"

void mjhz_init(mjhz_t* mj, int mode, int player_num)
{
    int i,j,n;

    if (!mj)
        return;
    memset(mj, 0, sizeof(mjhz_t));

    if (mode == MJHZ_MODE_SERVER)
        mj->mode = MJHZ_MODE_SERVER;
    else
        mj->mode = MJHZ_MODE_CLIENT;
    mj->player_num = player_num;
    mj->game_state = MJHZ_GAME_END;

    /* 杭州麻将使用136张牌,108张序数+28张字牌 */
    n = 0;
    for (i = 0; i < 4; i++) {
        /* 序数牌 */
        /* 1W~9T 27张 */
        for (j = 0; j < 27; j++) {
            mjpai_init_id(&mj->deck[n], MJ_ID_1W + j);
            n++;
        }
        /* 字牌 */
        for (j = 0; j < 7; j++) {
            mjpai_init_id(&mj->deck[n], MJ_ID_DONG + j);
            n++;
        }
    }
    mj->deck_all_num = n;

    for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
        mj->players[i].position = i;
    }
}

void mjhz_start(mjhz_t* mj)
{
    int i,j,direct,m,n;

    if (!mj)
        return;
    mj->round = 0;
    mj->game_state = MJHZ_GAME_PLAY;
    mj->inning++;

    mj->dice1 = rand() % 6 + 1;
    mj->dice2 = rand() % 6 + 1;

    /* 白板是财神 */
    mjpai_init_id(&mj->joker, MJ_ID_BAI);
    mjpai_zero(&mj->last_played_mj);
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

        /* 顺时针,每人抓12张,庄家先抓,一次4张 */
        m = n = 0;
        for (j = 0; j < 3; ++j) {
            for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
                direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
                memcpy(mj->players[direct].tiles + m, mj->deck + n, 4 * sizeof(mjpai_t));
                n += 4;
            }
            m += 4;
        }

        /* 庄家跳牌2张,其他人一张 */
        mjpai_copy(mj->players[mj->banker_no].tiles + m, mj->deck + n);
        mjpai_copy(mj->players[mj->banker_no].tiles + m + 1, mj->deck + n + 4);
        n++;
        for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
            if (i == mj->banker_no) continue;
            direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
            mjpai_copy(mj->players[direct].tiles + m, mj->deck + n);
        }
        mj->last_draw_no = mj->banker_no;

        /* 初始化分析数据 */
        for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
            if (i == mj->banker_no)
                m = 14;
            else
                m = 13;
            for (j = 0; j < m; ++j) {
                n = mj->players[i].tiles[j].id;
                if (n <= 0 || n > MJHZ_MAX_PAITYPE) {
                    printf("!!!error:find mjpai index > MJHZ_MAX_PAITYPE!\n");
                    continue;
                }
                mj->players[i].tiles_js[n]++;
            }
        }

        /* the first player */
        mj->first_player_no = mj->banker_no;
        mj->curr_player_no = mj->first_player_no;
    } else {
        /* 初始化，真实数据需要收到服务器数据赋值。 */
        for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
            memset(mj->players[i].tiles, 0, sizeof(mjpai_t) * MJHZ_MAX_CARDS);
            memset(mj->players[i].tiles_played, 0, sizeof(mjpai_t) * MJHZ_MAX_CARDS);
        }
        mj->banker_no = 0;
        mj->last_draw_no = 0;
        mj->first_player_no = 0;
        mj->curr_player_no = 0;
        mj->last_played_no = 0;
    }
}

/* 财神 万 索 筒 风 */
void mjhz_sort(mjhz_t* mj, mjpai_t* tiles, int len)
{
    int i,j;
    int exchange;
    mjpai_t temp;
    mjpai_t *pa, *pb;

    if (!mj || !tiles || len < 2)
        return;

    /* 选择排序 */
    for (i = 0; i < len - 1; ++i) {
        exchange = 0;
        pa = tiles + i;
        for (j = i + 1; j < len; ++j) {
            pb = tiles + j;
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
            pb = tiles + i;
            temp.suit = pb->suit;
            temp.sign = pb->sign;
            pb->suit = pa->suit;
            pb->sign = pa->sign;
            pa->suit = temp.suit;
            pa->sign = temp.sign;
        }
    }
}

int mjhz_pai_length(mjhz_t* mj, int player_no)
{
    int i, n;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    n = 0;
    for (i = 0; i < MJHZ_MAX_CARDS; ++i) {
        if (mj->players[player_no].tiles[i].id != MJ_ID_EMPTY)
            n++;
    }
    return n;
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
int mjhz_play(mjhz_t* mj, int player_no, mjpai_t* pai)
{
    int i,n,flag;
    mjpai_t* p;
    int gang_id[4];

    if(!mj || !pai)
        return -1;

    if (mj->game_state != MJHZ_GAME_PLAY) {
        if (mj->debug)
            printf("play pai but game state not play.\n");
        return -2;
    }
    if (player_no != mj->curr_player_no) {
        if (mj->debug)
            printf("play pai but not this no.\n");
        return -3;
    }

    /* 有效检查并删除这张牌 */
    if (mj->mode == MJHZ_MODE_SERVER) {
        n = -1;
        p = mj->players[player_no].tiles;
        for (i = 0; i < MJHZ_MAX_CARDS; ++i){
            if (p->suit == pai->suit && p->sign == pai->sign) {
                n = i;
                break;
            }
            p++;
        }
        if (n == -1) {
            if (mj->debug) {
                printf("play pai but player hasn't this card.\n");
            }
            return -4;
        } else {
            mj->players[player_no].tiles[n].suit = 0;
            mj->players[player_no].tiles[n].sign = 0;
        }
    } else {
        p = mj->players[player_no].tiles;
        for (i = 0; i < MJHZ_MAX_CARDS; ++i) {
            if (p->suit == pai->suit && p->sign == pai->sign) {
                p->suit = 0;
                p->sign = 0;
                break;
            }
            p++;
        }
    }
    mj->last_played_mj.suit = pai->suit;
    mj->last_played_mj.sign = pai->sign;
    mj->last_played_no = player_no;
    mj_trim(mj->players[player_no].tiles, MJHZ_MAX_CARDS);
    /* 判定吃碰杠胡 */
    for (i = 0; i < mj->player_num; ++i) {
        if (i != player_no) {
            mj->players[i].can_hu = mjhz_can_hu(mj, i);
            mj->players[i].can_gang = mjhz_can_gang(mj, i, gang_id);
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
    if (mj->last_played_mj.suit == mj->joker.suit &&
            mj->last_played_mj.sign == mj->joker.sign) {
        return 0;
    }
    if (mj->last_played_mj.suit != mjSuitWan ||
            mj->last_played_mj.suit != mjSuitSuo ||
            mj->last_played_mj.suit != mjSuitTong) {
        return 0;
    }

    /* 同类型序数牌计数 */
    memset((void*)num, 0, sizeof(int) * 9);
    p = mj->players[player_no].tiles;
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
    if (mj->last_played_mj.suit == mj->joker.suit &&
            mj->last_played_mj.sign == mj->joker.sign) {
        return 0;
    }

    num = 0;
    p = mj->players[player_no].tiles;
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

/* 返回可以杠牌的数量 */
int mjhz_can_gang(mjhz_t* mj, int player_no, int pai_gang[4])
{
    int i,num,x;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    num = 0;
    memset(pai_gang, 0, sizeof(int) * 4);

    if (mj->curr_player_no == player_no && mj->last_draw_no == player_no) {
        /* 轮到我且已经抓过牌了，暗杠或者加杠 */
        /* 有没有暗杠 */
        for (i = 1; i < MJHZ_LEN_JS; i++) {
            if (mj->players[player_no].tiles_js[i] == 4) {
                pai_gang[num++] = i;
            }
        }
        /* 加杠(已经碰了，再摸一张) */
        for (i = 0; i < MJHZ_MAX_SETS; ++i) {
            if (mj->players[player_no].mj_sets[i].type == mjMeldPeng) {
                x = mj->players[player_no].mj_sets[i].card.id;
                if (mj->players[player_no].tiles_js[x] > 0) {
                    pai_gang[num++] = x;
                }
            }
        }
    } else {
        /* 明杠(杠打出的牌) */
        if (mj->last_played_mj.id == MJ_ID_BAI) {
            return 0;
        }
        x = mj->last_played_mj.id;
        if (mj->players[player_no].tiles_js[x] == 3) {
            pai_gang[num++] = x;
        }
    }
    return num;
}

/*
 * 基本胡牌公式
 * m * ABC + n * AAA + AA
 * 其中m,n可以为0
 * 特殊牌型另外判断
 */
int mjhz_can_hu(mjhz_t* mj, int player_no)
{
    int i,x,n;
    int n_pai,n_4;
    int n_joker,left_joker;
    int js[MJHZ_LEN_JS];
    int js_joker[MJHZ_LEN_JS];
    mjhz_hu_t hu;
    mjpai_t* p;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;

    n = 0;
    n_joker = 0;
    memcpy(js, mj->players[player_no].tiles_js,
           sizeof(int) * MJHZ_LEN_JS);
    if (mj->last_draw_no != player_no &&
            mj->last_played_mj.id != MJ_ID_EMPTY) {
        /* 杭州麻将老庄才能捉冲。*/
        if (mj->lao_z == 0)
            return 0;
        /* 配置开关 */
        if (mj->enable_dian_hu == 0)
            return 0;
        /* 捉冲，财神不能捉。 */
        if (mj->last_played_mj.id == mj->joker.id) {
            return 0;
        }
        js[mj->last_played_mj.id]++;
    }
    n_joker = js[MJ_ID_BAI];
    js[MJ_ID_BAI] = 0;
    memset(&hu, 0, sizeof(mjhz_hu_t));

    /* 是否七对子 */
    memcpy(js_joker, js, sizeof(int) * MJHZ_LEN_JS);
    n_pai = n_4 = 0;
    hu.is_pair7 = 1;
    for (i = 1; i < MJHZ_LEN_JS; ++i) {
        if (js_joker[i] == 0) continue;
        n_pai += js_joker[i];
        if (js_joker[i] == 2) {
            continue;
        } else if (js_joker[i] == 4) {
            n_4++;
        } else if (js_joker[i] == 3) {
            if (n_joker > 0) {
                n_joker--;
                n_4++;
            } else {
                hu.is_pair7 = 0;
                break;
            }
        } else if (js_joker[i] == 1) {
            if (n_joker > 0) {
                n_joker--;
                /* 是否爆头 */
                if ()
            } else {
                hu.is_pair7 = 0;
                break;
            }
        }
    }
    if (n_joker > 0) {
        /* 财神处理 */
        left_joker = n_joker;
        for (i = 0; i < MJHZ_LEN_JS; ++i) {
            if (js_joker[i] == 0) continue;
            if (js_joker[i] == 1 || js_joker[i] == 3) {
                js_joker[i]++;
                left_joker--;
                if (left_joker == 0)
                    break;
            }
        }
        /* 财神去凑更多的豪华数量 */
        if (left_joker >= 2) {
            for (i = 0; i < MJHZ_LEN_JS; ++i) {
                if (js_joker[i] == 2) {
                    js_joker[i] = 4;
                    left_joker -= 2;
                    if (left_joker == 0)
                        break;
                }
            }
        }
        if (left_joker > 0) {
            js_joker[MJ_ID_BAI] += left_joker;
        }
    }
    if (mj_pair7(js_joker, MJHZ_LEN_JS)) {
        /* 是7对子得到豪华数量 */
        mj->players[player_no].hu.isPair7 = 1;
        mj->players[player_no].hu.pair7H4 = 0;
        for (i = 0; i < MJHZ_LEN_JS; ++i) {
            if (js_joker[i] == 4)
                mj->players[player_no].hu.pair7H4++;
        }
        return 1;
    }

	/* 暴力枚举，将每张牌作为将牌(1张的用财神+1) 来判定是否全成面子 */
	for (i = 1; i < MJHZ_LEN_JS; ++i) {
		if (js[i] == 0) continue;
		memcpy(js_joker, js, sizeof(int) * MJHZ_LEN_JS);
		if (js_joker[i] >= 2) {
			js_joker[i] -= 2;
			/* 判断去掉将头后是否都是面子 */
			if (mj_all_melded_joker(js_joker, MJHZ_LEN_JS, n_joker) > 0)
				return 1;
		} else if (js_joker[i] == 1) {
			if (n_joker > 0) {
				js_joker[i] = 0;
				if (mj_all_melded_joker(js_joker, MJHZ_LEN_JS, n_joker - 1) > 0)
					return 1;
			}
		}
	}

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
    printf("joker:\n");
    printf("%s\n", mjpai_string(&mj->joker));

    /* dump player's mj pai */
    printf("players mj pai:\n");
    printf("%s\n",
            mj_string(mj->players[0].tiles, MJHZ_MAX_CARDS, 10));
    printf("%s\n",
            mj_string(mj->players[1].tiles, MJHZ_MAX_CARDS, 10));
    if (mj->player_num > 2) {
        printf("%s\n",
                mj_string(mj->players[2].tiles, MJHZ_MAX_CARDS, 10));
        printf("%s\n",
                mj_string(mj->players[3].tiles, MJHZ_MAX_CARDS, 10));
    }

    printf("last mj pai:\n");
    printf("%s\n", mjpai_string(&mj->last_played_mj));

    printf("current player no is %d\n", mj->curr_player_no);
}
