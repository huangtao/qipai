#include "mjhz.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mj_algo.h"

/*
 * 内部用于判断一门是否全成面子
 * array : 计数数组
 * start : 开始索引
 * num_joker : 财神数量(返回剩余数量)
 * return : 0/1
 */
static int _range_melded(int array[MJHZ_LEN_JS], int start, int* num_joker)
{
    int i,left_joker;
    int js[9];

    if (start != PAI_1W && start != PAI_1S &&
            start != PAI_1T)
        return 0;
    if (num_joker == NULL)
        return 0;
    left_joker = *num_joker;
    if (left_joker < 0 || left_joker > 4)
        return 0;

    memcpy(js, array + start, sizeof(int) * 9);
    for (i = 0; i < 7; ++i) {
        if (js[i] % 3 == 0) {
            js[i] = 0;
            continue;
        }
        else if (js[i] % 3 == 1) {
            if (js[i+1] > 0 && js[i+2] > 0) {
                /* 111 112 113 121 123 131 133 */
                js[i] = 0;
                js[i+1]--;
                js[i+2]--;
                continue;
            } else {
                if (left_joker == 0)
                    return 0;
                if (js[i+1] > 0) {
                    /* 110 120 130 */
                    js[i] = 0;
                    js[i+1]--;
                    left_joker--;
                    continue;
                } else if (js[i+2] > 0) {
                    /* 101 102 103 */
                    js[i] = 0;
                    js[i+2]--;
                    left_joker--;
                    continue;
                } else {
                    /* 100 */
                    if (left_joker < 2)
                        return 0;
                    js[i] = 0;
                    left_joker -= 2;
                    continue;
                }
            }
        } else if (js[i] % 3 == 2) {
            if (js[i+1] >= 2 && js[i+2] >= 2) {
                /* 222 232 242 223 233 243 224 */
                js[i] = 0;
                js[i+1] -= 2;
                js[i+2] -= 2;
                continue;
            } else {
                if (left_joker == 0)
                    return 0;
                if (js[i+1] == 1 && js[i+2] >= 2) {
                    /* 212 213 214 */
                    js[i] = 0;
                    js[i+1] = 0;
                    left_joker--;
                    js[i+2] -= 2;
                    continue;
                } else if (js[i+2] == 1 && js[i+1] >= 2) {
                    /* 221 231 241 */
                    js[i] = 0;
                    js[i+1] -= 2;
                    js[i+2] = 0;
                    left_joker--;
                    continue;
                } else {
                    /*
                     * 201 202 203 204
                     * 210 220 230 240
                     * 这种情况凑刻子
                     */
                    left_joker--;
                    js[i] = 0;
                    continue;
                }
            }
        }
    }
    /* 8 */
    if (js[7] % 3 == 1) {
        if (js[8] % 3 == 1 && left_joker > 0) {
            js[8] -= 1;
            left_joker--;
            js[7] = 0;
        } else if (left_joker >= 2) {
            js[7] = 0;
            left_joker -= 2;
        } else {
            return 0;
        }
    } else if (js[7] % 3 == 2) {
        if (left_joker == 0)
            return 0;
        left_joker--;
        js[7] = 0;
    } else {
        js[7] = 0;
    }
    /* 9 */
    if (js[8] % 3 == 1) {
        if (left_joker >= 2) {
            left_joker -= 2;
            js[8] = 0;
        } else {
            return 0;
        }
    } else if (js[8] % 3 == 2) {
        if (left_joker == 0)
            return 0;
        left_joker--;
        js[8] = 0;
    } else {
        js[8] = 0;
    }
    /* now js[9] is all zero */
    *num_joker = left_joker;
    return 1;
}

/*
 * 内部用于判断字牌是否全成面子
 */
static int _hornor_melded(int array[MJHZ_LEN_JS], int* num_joker)
{
    int i,left_joker;
    int js[7];

    if (num_joker == NULL)
        return 0;
    left_joker = *num_joker;
    if (left_joker < 0 || left_joker > 4)
        return 0;
    memcpy(js, array + PAI_DONG, sizeof(int) * 7);
    for (i = 0; i < 7; ++i) {
        if (js[i] == 0) continue;
        if (js[i] % 3 == 1) {
            if (left_joker < 2)
                return 0;
        } else if (js[i] % 3 == 2) {
            if (left_joker == 0)
                return 0;
        }
        js[i] = 0;
    }
    /* now js[7] is all zero */
    *num_joker = left_joker;
    return 1;
}

void mjhz_init(mjhz_t* mj, int mode, int player_num)
{
    int i,j,n;

    if (!mj)
        return;
    memset(mj, 0, sizeof(mjhz_t));

    if (mode == GAME_MODE_SERVER)
        mj->mode = GAME_MODE_SERVER;
    else
        mj->mode = GAME_MODE_CLIENT;
    mj->player_num = player_num;
    mj->game_state = GAME_END;
    mj->curr_player_no = -1;
    mj->first_player_no = -1;

    /* 杭州麻将使用136张牌,108张序数+28张字牌 */
    n = 0;
    for (i = 0; i < 4; i++) {
        /*
         * 序数牌 1W~9T 27张
         * 字牌7张
         */
        for (j = 0; j < PAI_BAI; j++) {
            mj->deck[n] = PAI_1W + j;
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
    int temp[MJHZ_DECK_PAIS];

    if (!mj)
        return;
    mj->round = 0;
    mj->game_state = GAME_PLAY;
    mj->logic_state = lsDiscard;
    time(&mj->time_start);
    mj->inning++;

    mj->dice[0] = rand() % 6 + 1;
    mj->dice[1] = rand() % 6 + 1;

    /* 白板是财神 */
    mj->joker = PAI_BAI;
    mj->current_discard = 0;
    mj->pai_gang = 0;

    for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
        mj->players[i].can_hu = 0;
        memset(mj->players[i].can_peng, 0,
               sizeof(mj->players[i].can_peng));
        memset(mj->players[i].discard, 0,
               sizeof(mj->players[i].discard));
    }

    if (mj->mode == GAME_MODE_SERVER) {
        /* 洗牌 */
        mj_shuffle(mj->deck, mj->deck_all_num);
        /* 计算起手牌位置 */
        direct = (mj->banker_no + MJHZ_MAX_PLAYERS
                - (mj->dice[0] + mj->dice[1]) % MJHZ_MAX_PLAYERS)
            % MJHZ_MAX_PLAYERS;
        mj->deck_deal_index = direct * 17 * 2
            + (mj->dice[0] + mj->dice[1]) * 2;
        /* 将牌直接交换好 */
        m = mj->deck_all_num - mj->deck_deal_index;
        memcpy(temp, mj->deck + mj->deck_deal_index,
               sizeof(int) * m);
        memcpy(temp + m, mj->deck,
               sizeof(int) * mj->deck_deal_index);
        mj->deck_deal_index = 0;
        mj->deck_deal_gang = mj->deck_all_num - 1; /* 杠抓牌 */

        /* 顺时针,每人抓12张,庄家先抓,一次4张 */
        m = 0;
        n = mj->deck_deal_index;
        for (j = 0; j < 3; ++j) {
            for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
                direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
                memcpy(mj->players[direct].hand + m,
                       mj->deck + n, 4 * sizeof(int));
                n += 4;
            }
            m += 4;
        }

        /* 庄家跳牌2张,其他人一张 */
        mj->players[mj->banker_no].hand[m] =  mj->deck[n];
        mj->players[mj->banker_no].hand[m + 1] = mj->deck[n + 4];
        n++;
        for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
            if (i == mj->banker_no) continue;
            direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
            mj->players[direct].hand[m] = mj->deck[n++];
        }
        mj->last_takes_no = mj->banker_no;
        mj->deck_deal_index += 13 * (MJHZ_MAX_PLAYERS - 1) + 14;
        mj->deck_valid_num = mj->deck_all_num -
                13 * (MJHZ_MAX_PLAYERS - 1) + 14;

        /* 初始化分析数据 */
        for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
            if (i == mj->banker_no)
                m = 14;
            else
                m = 13;
            for (j = 0; j < m; ++j) {
                n = mj->players[i].hand[j];
                if (n <= 0 || n > PAI_BAI) {
                    printf("!!!error:find mjpai index > PAI_BAI!\n");
                    continue;
                }
                mj->players[i].hand_js[n]++;
            }
        }

        /* the first player */
        mj->first_player_no = mj->banker_no;
        mj->curr_player_no = mj->first_player_no;
        time(&mj->time_turn);
        mj->sec_wait = WAITTIME_TURN;

        /* 庄家能杠胡吗 */
        mjhz_can_gang(mj, mj->curr_player_no);
        mjhz_can_hu(mj, mj->curr_player_no);
    } else {
        /* 初始化，真实数据需要收到服务器数据赋值。 */
        for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
            memset(mj->players[i].hand, 0,
                   sizeof(int) * MJHZ_MAX_HAND);
            memset(mj->players[i].discard, 0,
                   sizeof(int) * MJHZ_MAX_DISCARDED);
        }
        mj->banker_no = -1;
        mj->last_takes_no = -1;
        mj->first_player_no = -1;
        mj->curr_player_no = -1;
        mj->discarded_no = -1;
    }
}

/* 财神 万 索 筒 风 */
void mjhz_sort(int* pais, int len)
{
    int i,j;
    int *pi,*pj;
    int exchange;
    int temp;

    if (!pais || len < 2)
        return;

    /* 选择排序 */
    pi = pais;
    for (i = 0; i < MJHZ_MAX_HAND - 1; ++i) {
        for (j = i + 1; j < MJHZ_MAX_HAND; ++j) {
            pj = pais + j;
            exchange = 0;
            if (*pj == PAI_BAI && *pi != PAI_BAI) {
                exchange = 1;
            } else if (*pi > *pj) {
                if (*pi == PAI_BAI)
                    continue;
                if (*pj <= PAI_EMPTY)
                    continue;
                exchange = 1;
            } else if (*pi == PAI_EMPTY &&
                       *pj != PAI_EMPTY) {
                exchange = 1;
            }
            if (exchange) {
                temp = *pi;
                *pi = *pj;
                *pj = temp;
            }
        }
        pi++;
    }
}

/*
 * 摸牌
 * 摸到的牌规定放在数组的最后，等打出后排序
 * 无牌可以摸返回0
 */
int mjhz_draw(mjhz_t* mj, int is_gang)
{
    int i,pai;

    if (!mj)
        return 0;
    if (mj->current_discard != 0) {
        for (i = 0; i < MJHZ_MAX_DISCARDED; ++i) {
            if (mj->players[mj->discarded_no].discard[i] != 0)
                continue;
            mj->players[mj->discarded_no].discard[i] =
                    mj->current_discard;
            break;
        }
        mj->current_discard = 0;
    }
    if (mj->deck_valid_num <= 20) {
        /* 流局 */
        return 0;
    }
    mj->pai_gang = 0;
    if (is_gang) {
        pai = mj->deck[mj->deck_deal_gang];
        mj->deck_deal_gang--;
    } else {
        pai = mj->deck[mj->deck_deal_index];
        mj->deck_deal_index++;
    }
    mj->deck_valid_num--;
    mj->players[mj->curr_player_no].hand[MJHZ_MAX_HAND-1] = pai;
    mj->players[mj->curr_player_no].hand_js[pai]++;

    /* 杠胡判定 */
    mjhz_can_hu(mj, mj->curr_player_no);
    mjhz_can_gang(mj, mj->curr_player_no);

    return 1;
}

int mjhz_discard(mjhz_t* mj, int pai_id)
{
    int i,n,flag;
    int no;

    if (!mj)
        return -1;
    if (pai_id < PAI_1W || pai_id > PAI_BAI)
        return -1;
    if (mj->game_state != GAME_PLAY) {
        if (mj->debug)
            printf("discard pai but game state not play.\n");
        return -2;
    }
    no = mj->curr_player_no;

    /* 有效检查并删除这张牌 */
    n = -1;
    for (i = 0; i < MJHZ_MAX_HAND; ++i){
        if (mj->players[no].hand[i] == pai_id) {
            n = i;
            break;
        }
    }
    if (n == -1) {
        if (mj->debug) {
            printf("discard pai but player hasn't this pai.\n");
        }
        return -4;
    } else {
        mj->players[no].hand[n] = 0;
        mj->players[no].hand_js[pai_id]--;
    }
    /* 财飘处理 */
    if (pai_id == mj->joker)
        mj->players[no].hu.cai_piao++;
    else
        mj->players[no].hu.cai_piao = 0;
    mj->current_discard = pai_id;
    mj->discarded_no = no;
    mj_trim(mj->players[no].hand, MJHZ_MAX_HAND);

    /* 判定吃碰杠胡 */
    flag = 0;
    for (i = 0; i < mj->player_num; ++i) {
        if (i != no) {
            flag |= mjhz_can_hu(mj, i);
            flag |= mjhz_can_gang(mj, i);
            flag |= mjhz_can_peng(mj, i);
            flag |= mjhz_can_chi(mj, i);
        } else {
            mj->players[i].can_hu = 0;
            mj->players[i].can_gang = 0;
            mj->players[i].can_chi = 0;
            memset(mj->players[i].can_peng, 0,
                   sizeof(mj->players[i].can_peng));
        }
    }
    mj->logic_state = lsTake;
    if (flag)
        mj->sec_wait = WAITTIME_CALL;
    else
        mj->sec_wait = 0;

    return 1;
}

/*
 * 返回吃牌信息
 * 0 : 不能吃
 * 1 : o** 左吃
 * 2 : *o* 中吃
 * 4 : **o 右吃
 */
int mjhz_can_chi(mjhz_t* mj, int player_no)
{
    int i,n,chi_info;
    mjpai_t pai;
    int st_left;
    int pos1,pos2,pos3;
    mjhz_player_t* player;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (mj->curr_player_no == player_no)
        return 0;
    player = &mj->players[player_no];
    player->can_chi = 0;
    if (mj->current_discard == mj->joker)
        return 0;
    st_left = (player_no + mj->player_num - 1) %
            mj->player_num;
    if (st_left != mj->discarded_no)
        return 0;

    n = 0;
    for (i = 0; i < MJHZ_MAX_MELD; ++i) {
        if (player->meld[i].type == mjMeldChi) {
            n++;
        }
    }
    /* 三老庄不能吃三摊，庄闲之间可以 */
    if (mj->lao_z >= 3 && n == 2) {
        if (player_no != mj->banker_no &&
                mj->discarded_no != mj->banker_no)
            return 0;
    }

    mjpai_init_id(&pai, mj->current_discard);
    /* 只能吃万、索、筒子 */
    if (pai.suit != mjSuitWan || pai.suit != mjSuitSuo ||
            pai.suit != mjSuitTong) {
        return 0;
    }
    chi_info = 0;
    pos1 = pos2 = pos3 = 0;
    /* 允许吃的位置 */
    if (pai.sign == 1) {
        pos1 = 1;
    } else if (pai.sign == 2) {
        pos1 = pos2 = 1;
    } else if (pai.sign == 8) {
        pos2 = pos3 = 1;
    } else if (pai.sign == 9) {
        pos3 = 1;
    } else {
        pos1 = pos2 = pos3 = 1;
    }
    if (pos1) {
        if (player->hand_js[pai.sign+1] > 0 &&
                player->hand_js[pai.sign+2] > 0) {
            chi_info &= mjChiLeft;
        }
    }
    if (pos2) {
        if (player->hand_js[pai.sign-1] > 0 &&
                player->hand_js[pai.sign+1] >0) {
            chi_info &= mjChiMiddle;
        }
    }
    if (pos3) {
        if (player->hand_js[pai.sign-1] > 0 &&
                player->hand_js[pai.sign-2] >0) {
            chi_info &= mjChiRight;
        }
    }
    player->can_chi = chi_info;
    return chi_info;
}

int mjhz_can_peng(mjhz_t* mj, int player_no)
{
    mjhz_player_t* player;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (mj->curr_player_no == player_no)
        return 0;
    if (mj->current_discard == 0)
        return 0;
    if (mj->current_discard == mj->joker)
        return 0;
    player = &mj->players[player_no];

    if (player->hand_js[mj->current_discard] >= 2)
        player->can_peng[mj->current_discard]++;

    if (player->can_peng[mj->current_discard] == 1)
        return 1;
    else
        return 0;
}

/* 返回可以杠牌的数量 */
int mjhz_can_gang(mjhz_t* mj, int player_no)
{
    int i,num,x;
    mjhz_player_t* player;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    player = &mj->players[player_no];
    num = 0;
    memset(player->pai_gang, 0, sizeof(int) * 4);

    if (mj->curr_player_no == player_no && mj->last_takes_no == player_no) {
        /* 轮到我且已经摸过牌了，暗杠或者加杠 */
        /* 有没有暗杠 */
        for (i = 1; i < MJHZ_LEN_JS; i++) {
            if (player->hand_js[i] == 4) {
                player->pai_gang[num++] = i;
            }
        }
        /* 加杠(已经碰了，再摸一张) */
        for (i = 0; i < MJHZ_MAX_MELD; ++i) {
            if (player->meld[i].type == mjMeldPeng) {
                x = player->meld[i].pai_id;
                if (player->hand_js[x] > 0) {
                    player->pai_gang[num++] = x;
                }
            }
        }
    } else {
        /* 明杠(杠打出的牌) */
        if (mj->current_discard == mj->joker) {
            return 0;
        }
        x = mj->current_discard;
        if (player->hand_js[x] == 3) {
            player->pai_gang[num++] = x;
        }
    }
    if (num > 0)
        player->can_gang = 1;
    else
        player->can_gang = 0;
    return num;
}

int mjhz_all_melded(int array[MJHZ_LEN_JS])
{
    int i,j,index;
    int js[MJHZ_LEN_JS];

    memcpy(js, array, sizeof(int) * MJHZ_LEN_JS);
    /* 序数牌 */
    for (i = 0; i < 3; ++i) {
        for (j = 0; i < 7; ++j) {
            index = PAI_1W + i * 9 + j;
            if (js[index] % 3 == 0)
                continue;
            else if (js[index] % 3 == 1) {
                if (js[index+1] > 0 && js[index+2] > 0) {
                    js[index+1]--;
                    js[index+2]--;
                } else {
                    return 0;
                }
            } else if (js[index] % 3 == 2) {
                if (js[index+1] >= 2 && js[index+2] >= 2) {
                    js[index+1] -= 2;
                    js[index+2] -= 2;
                } else {
                    return 0;
                }
            }
        }
        /* 8,9 */
        if (js[i*9+7] % 3 == 0 && js[i*9+8] % 3 == 0)
            continue;
        else
            return 0;
    }
    /* 字牌 */
    for (i = PAI_DONG; i < PAI_BAI; ++i) {
        if (js[i] % 3 == 0)
            continue;
        else
            return 0;
    }

    return 1;
}

/*
 * 此版本适用没有花牌的判定
 */
int mjhz_all_melded_joker(int array[MJHZ_LEN_JS], int num_joker)
{
    int left_joker;

    /* 先判定字牌 */
    left_joker = num_joker;
    if (_hornor_melded(array, &left_joker) == 0)
        return 0;

    /* 序数牌 */
    if (_range_melded(array, PAI_1W, &left_joker) == 0)
        return 0;
    if (_range_melded(array, PAI_1S, &left_joker) == 0)
        return 0;
    if (_range_melded(array, PAI_1T, &left_joker) == 0)
        return 0;

    return 1;
}

/*
 * 基本胡牌公式
 * m * ABC + n * AAA + AA
 * 其中m,n可以为0
 * 特殊牌型另外判断
 */
int mjhz_can_hu(mjhz_t* mj, int player_no)
{
    int i;
    int n_pai,n_4;
    int n_joker,left_joker;
    int pai_takes; /* 刚刚摸到的牌 */
    int js[MJHZ_LEN_JS];
    int js_joker[MJHZ_LEN_JS];
    mjhz_player_t *player;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    player = &mj->players[player_no];
    player->can_hu = 0;
    memset(&player->hu, 0, sizeof(mjhz_hu_t));

    n_joker = 0;
    memcpy(js, player->hand_js, sizeof(js));
    if (mj->current_discard != 0) {
        /* 配置开关 */
        if (mj->enable_dian_hu)
            return 0;
        /* 捉冲，财神不能捉。 */
        if (mj->current_discard == mj->joker)
            return 0;
        /* 三老庄才能捉冲 */
        if (mj->lao_z < 3)
            return 0;
        /* 漏胡后不能胡 */
        if (player->pass_hu > 0)
            return 0;
        /* 闲家之间不能捉 */
        if (player_no != mj->banker_no &&
                mj->discarded_no != mj->banker_no)
            return 0;
        js[mj->current_discard]++;
    } else {
        /* 自摸 */
        pai_takes = player->hand[MJHZ_MAX_HAND-1];
    }
    n_joker = left_joker = js[PAI_BAI];
    js[PAI_BAI] = 0;

    /* 是否七对子 */
    memcpy(js_joker, js, sizeof(js_joker));
    n_pai = n_4 = 0;
    player->hu.is_pair7 = 1;
    for (i = 1; i < MJHZ_LEN_JS; ++i) {
        if (js_joker[i] == 0) continue;
        n_pai += js_joker[i];
        if (js_joker[i] == 2) {
            continue;
        } else if (js_joker[i] == 4) {
            n_4++;
        } else if (js_joker[i] == 3) {
            if (left_joker > 0) {
                left_joker--;
            } else {
                player->hu.is_pair7 = 0;
                break;
            }
        } else if (js_joker[i] == 1) {
            if (left_joker > 0) {
                left_joker--;
            } else {
                player->hu.is_pair7 = 0;
                break;
            }
        }
    }
    if (player->hu.is_pair7 &&
            (n_pai + n_joker) == MJHZ_MAX_HAND) {
        /* 是7对子 */
        player->hu.pair7_h4 = n_4;

        /* 判定爆头 */
        if (n_joker > 0) {
            left_joker = n_joker - 1;
            js_joker[pai_takes]--;
            player->hu.is_baotou = 1;
            for (i = 1; i < MJHZ_LEN_JS; ++i) {
                if (js_joker[i] % 2 == 0)
                    continue;
                else {
                    if (left_joker > 0) {
                        left_joker--;
                    } else {
                        mj->players[player_no].hu.is_baotou = 0;
                        break;
                    }
                }
            }
        }
        if (mj->current_discard != 0 && player->hu.is_baotou) {
            /* 爆头不能捉冲 */
            player->can_hu = 0;
        } else {
            player->can_hu = 1;
        }
        return player->can_hu;
    }

	/* 暴力枚举，将每张牌作为将牌(1张的用财神+1) 来判定是否全成面子 */
	for (i = 1; i < MJHZ_LEN_JS; ++i) {
		if (js[i] == 0) continue;
        memcpy(js_joker, js, sizeof(js_joker));
		if (js_joker[i] >= 2) {
			js_joker[i] -= 2;
			/* 判断去掉将头后是否都是面子 */
            if (mjhz_all_melded_joker(js_joker, n_joker) > 0) {
                player->can_hu = 1;
				return 1;
            }
		} else if (js_joker[i] == 1) {
			if (n_joker > 0) {
				js_joker[i] = 0;
                if (mjhz_all_melded_joker(js_joker, n_joker - 1) > 0) {
                    player->can_hu = 1;
					return 1;
                }
			}
		}
	}

    return 0;
}

int mjhz_chi(mjhz_t* mj, int player_no, int pai1, int pai2)
{
    int m1,m2,m3,temp;
    int i,st,set_idx;
    mjpai_t t1,t2,t3;

    if (!mj)
        return -1;
    if (player_no >= mj->player_num)
        return -2;
    if (mj->logic_state != lsTake)
        return -3;
    if (mj->current_discard == mj->joker)
        return -4;
    if (mj->current_discard < PAI_1W ||
            mj->current_discard > PAI_9T) {
        return -5;
    }
    if (mj->players[player_no].hand_js[pai1] == 0)
        return -6;
    if (mj->players[player_no].hand_js[pai2] == 0)
        return -6;
    st = get_relative_seat(mj->discarded_no, player_no, mj->player_num);
    if (st != stLeft)
        return -7;

    m1 = mj->current_discard;
    m2 = pai1;
    m3 = pai2;
    if (m1 > m2) {
        temp = m2;
        m2 = m1;
        m1 = temp;
    }
    if (m1 > m3) {
        temp = m3;
        m3 = m1;
        m1 = temp;
    }
    if (m2 > m3) {
        temp = m3;
        m3 = m2;
        m2 = temp;
    }
    if ((m2 - m1) != 1)
        return 0;
    if ((m3 - m2) != 1)
        return 0;
    mjpai_init_id(&t1, m1);
    mjpai_init_id(&t2, m2);
    mjpai_init_id(&t3, m3);
    if (!(t1.suit == t2.suit && t1.suit == t3.suit)) {
        return 0;
    }
    set_idx = -1;
    for (i = 0; i < MJHZ_MAX_MELD; ++i) {
        if (mj->players[player_no].meld[i].type == 0) {
            /* unused */
            set_idx = i;
            break;
        }
    }
    if (set_idx == -1) {
        if (mj->debug)
            printf("players meld overflowed!\n");
    }
    mj->players[player_no].meld[set_idx].type = mjMeldChi;
    mj->players[player_no].meld[set_idx].pai_id = mj->current_discard;
    mj->players[player_no].meld[set_idx].player_no = mj->curr_player_no;
    if (m1 == mj->current_discard) {
        mj->players[player_no].meld[set_idx].extra_info = mjChiLeft;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, m2);
        mj->players[player_no].hand_js[m2]--;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, m3);
        mj->players[player_no].hand_js[m3]--;
    }
    else if (m2 == mj->current_discard) {
        mj->players[player_no].meld[set_idx].extra_info = mjChiMiddle;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, m1);
        mj->players[player_no].hand_js[m1]--;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, m3);
        mj->players[player_no].hand_js[m3]--;
    }
    else {
        mj->players[player_no].meld[set_idx].extra_info = mjChiRight;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, m1);
        mj->players[player_no].hand_js[m1]--;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, m2);
        mj->players[player_no].hand_js[m2]--;
    }
    mj_trim(mj->players[player_no].hand, MJHZ_MAX_HAND);
    mj->curr_player_no = player_no;
    mj->current_discard = 0;

    return 1;
}

int mjhz_peng(mjhz_t* mj, int player_no)
{
    int i,set_idx;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (player_no == mj->curr_player_no)
        return 0;
    if (mj->current_discard == mj->joker)
        return 0;

    if (mj->players[player_no].hand_js[mj->current_discard] < 2)
        return 0;

    set_idx = -1;
    for (i = 0; i < MJHZ_MAX_MELD; ++i) {
        if (mj->players[player_no].meld[i].type == 0) {
            /* unused */
            set_idx = i;
            break;
        }
    }
    if (set_idx == -1) {
        if (mj->debug)
            printf("players meld overflowed!\n");
    }
    /* take the discarded tile */
    mj->players[player_no].meld[set_idx].type = mjMeldPeng;
    mj->players[player_no].meld[set_idx].pai_id = mj->current_discard;
    mj->players[player_no].meld[set_idx].player_no = mj->curr_player_no;
    mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND,
              mj->current_discard);
    mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND,
              mj->current_discard);
    mj->players[player_no].hand_js[mj->current_discard] -= 2;
    mj_trim(mj->players[player_no].hand, MJHZ_MAX_HAND);
    mj->current_discard = 0;
    mj->curr_player_no = player_no;

    return 1;
}

int mjhz_gang(mjhz_t* mj, int player_no, int pai)
{
    int i,set_idx;
    int peng_idx;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (pai == mj->joker)
        return 0;

    set_idx = -1;
    for (i = 0; i < MJHZ_MAX_MELD; ++i) {
        if (mj->players[player_no].meld[i].type == 0) {
            /* unused */
            set_idx = i;
            break;
        }
    }
    if (set_idx == -1) {
        if (mj->debug)
            printf("players meld overflowed!\n");
    }

    if (player_no == mj->curr_player_no) {
        /* 暗杠或加杠 */
        if (mj->players[player_no].hand_js[pai] == 4) {
            mj->players[player_no].meld[set_idx].type = mjMeldGang;
            mj->players[player_no].meld[set_idx].pai_id = mj->current_discard;
            mj->players[player_no].meld[set_idx].player_no = mj->curr_player_no;
            mj->players[player_no].meld[set_idx].extra_info = mjGangAn;
            mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
            mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
            mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
            mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
            mj->players[player_no].hand_js[pai] = 0;
        } else if (mj->players[player_no].hand_js[pai] == 1) {
            /* 找到面子 */
            peng_idx = -1;
            for (i = 0; i < MJHZ_MAX_MELD; ++i) {
                if (mj->players[player_no].meld[i].type == mjMeldPeng &&
                        mj->players[player_no].meld[i].pai_id == pai) {
                    peng_idx = i;
                    break;
                }
            }
            if (peng_idx == -1) {
                if (mj->debug)
                    printf("players jia gang but not found this peng!\n");
                return 0;
            }
            mj->players[player_no].meld[peng_idx].type = mjMeldGang;
            mj->players[player_no].meld[peng_idx].extra_info = mjGangJia;
            mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
            mj->players[player_no].hand_js[pai] = 0;
        } else {
            if (mj->debug)
                printf("players gang unknow situation!\n");
            return 0;
        }
    } else {
        /* 明杠 */
        if (mj->players[player_no].hand_js[pai] != 3)
            return 0;
        if (pai != mj->current_discard)
            return 0;
        mj->players[player_no].meld[set_idx].type = mjMeldGang;
        mj->players[player_no].meld[set_idx].pai_id = pai;
        mj->players[player_no].meld[set_idx].player_no = mj->curr_player_no;
        mj->players[player_no].meld[set_idx].extra_info = mjGangMing;
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
        mj_delete(mj->players[player_no].hand, MJHZ_MAX_HAND, pai);
        mj->players[player_no].hand_js[pai] = 0;

        mj->curr_player_no = player_no;
    }
    mj_trim(mj->players[player_no].hand, MJHZ_MAX_HAND);
    mj->current_discard = 0;

    return 1;
}

int mjhz_hu(mjhz_t* mj, int player_no)
{
    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (mj->players[player_no].can_hu == 0)
        return 0;

    if (mj->curr_player_no == player_no) {
        /* 自摸、杠开 */
    } else {
        /* 点炮 */
        if (!mj->enable_dian_hu)
            return 0;
        mj->players[player_no].hand[MJHZ_MAX_HAND-1] = mj->current_discard;
        mj->players[player_no].hand_js[mj->current_discard]++;
    }
    mj->hu_player_no = player_no;
    mj->game_state = GAME_END;

    /* 番计算 */
    mj->players[player_no].hu.fan = 0;
    if (mj->players[player_no].hu.is_pair7) {
        mj->players[player_no].hu.fan++;
        mj->players[player_no].hu.fan += mj->players[player_no].hu.pair7_h4;
    }
    if (mj->players[player_no].hu.is_baotou)
        mj->players[player_no].hu.fan++;
    if (mj->players[player_no].hu.is_gk) {
        if (mj->players[player_no].hu.cai_piao) {
            /* 飘杠(3番) */
            mj->players[player_no].hu.fan += 3;
            if (mj->players[player_no].hu.cai_piao > 1) {
                mj->players[player_no].hu.fan +=
                        mj->players[player_no].hu.cai_piao - 1;
            }
        } else {
            mj->players[player_no].hu.fan++;
        }
    } else {
        mj->players[player_no].hu.fan += mj->players[player_no].hu.cai_piao;
    }
    mj->players[player_no].hu.fan += mj->lao_z;

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

int mjhz_get_next(mjhz_t* mj)
{
    int next_no;

    if (!mj)
        return 0;
    next_no = mj->curr_player_no - 1;
    if (next_no < 0)
        next_no += mj->player_num;

    return next_no;
}

void mjhz_dump(mjhz_t* mj)
{
    if(!mj)
        return;

    printf("player number:%d\n", mj->player_num);
    printf("joker:\n");
    printf("%s\n", mjpai_string(mj->joker));

    /* dump player's mj pai */
    printf("players mj pai:\n");
    printf("%s\n",
            mj_string(mj->players[0].hand, MJHZ_MAX_HAND, 10));
    printf("%s\n",
            mj_string(mj->players[1].hand, MJHZ_MAX_HAND, 10));
    if (mj->player_num > 2) {
        printf("%s\n",
                mj_string(mj->players[2].hand, MJHZ_MAX_HAND, 10));
        printf("%s\n",
                mj_string(mj->players[3].hand, MJHZ_MAX_HAND, 10));
    }
    printf("last mj pai:\n");
    printf("%s\n", mjpai_string(mj->current_discard));
    printf("current player no is %d\n", mj->curr_player_no);
}

const char* mjhz_hu_name(mjhz_hu_t* hu)
{
    static char* hu_name[] = {
        "MJHZ_ERROR",
        "MJHZ_PING",    /* 平胡 */
        "MJHZ_DUI7"     /* 7对子 */
    };

    if (hu == NULL) {
        return hu_name[0];
    }
    if (hu->is_pair7)
        return hu_name[2];
    else
        return hu_name[1];
}
