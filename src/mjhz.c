#include "mjhz.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "mj_algo.h"

void _reset_wait_req(mjhz_t* mj)
{
    int i;
    for (i = 0; i < mj->player_num; ++i) {
        mj->players[i].wait_chi = 0;
        mj->players[i].wait_peng = 0;
        mj->players[i].wait_gang = 0;
        mj->players[i].wait_hu = 0;
        mj->players[i].req_chi = 0;
        mj->players[i].req_chi2 = 0;
        mj->players[i].req_peng = 0;
        mj->players[i].req_gang = 0;
        mj->players[i].req_hu = 0;
        mj->players[i].req_pass = 0;
    }
}

/* 其他玩家都选择了吗 */
int _is_all_select(mjhz_t* mj)
{
    int i,all_select,have_hu;
    int have_pg;

    if (!mj)
        return 0;
    if (mj->logic_state != lsTake)
        return 0;
    have_hu = 0;
    have_pg = 0;
    for (i = 0; i < mj->player_num; ++i) {
        if (mj->players[i].req_hu > 0)
            have_hu = 1;
        if (mj->players[i].req_peng > 0 ||
                mj->players[i].req_gang > 0) {
            have_pg = 1;
        }
    }
    /* 检查其他玩家是否已选择操作 */
    all_select = 1;
    for (i = 0; i < mj->player_num; ++i) {
        if (i == mj->discarded_no) {
            /* 他弃的牌 */
            continue;
        }
        if (mj->gang_pai != 0 && i == mj->curr_player_no) {
            /* 此玩家正在杠 */
            continue;
        }
        /* 已请求 */
        if (mj->players[i].req_chi > 0)
            continue;
        if (mj->players[i].req_peng > 0)
            continue;
        if (mj->players[i].req_gang > 0)
            continue;
        if (mj->players[i].req_hu > 0)
            continue;
        if (mj->players[i].req_pass > 0)
            continue;
        /* 没有选择操作 */
        if (mj->players[i].wait_hu > 0) {
            /* 可以胡 */
            all_select = 0;
            break;
        }
        if (have_hu)
            continue;   /* 有人要胡,不需要看吃碰杠了 */
        if (mj->players[i].wait_gang ||
                mj->players[i].wait_peng) {
            all_select = 0;
            break;
        }
        if (have_pg)
            continue;
        if (mj->players[i].wait_chi) {
            all_select = 0;
            break;
        }
    }

    /* 所有其他玩家已选择 */
    if (all_select) {
        mj->sec_wait = 0;
    }
    return all_select;
}

int _check_chi(mjhz_t* mj, int player_no, int pai1, int pai2)
{
    int i,st,n;
    mjhz_player_t* player;

    if (!mj)
        return -1;
    if (mj->game_state != GAME_PLAY)
        return -2;
    if (mj->enable_chi == 0)
        return -3;
    if (player_no >= mj->player_num)
        return -4;
    if (mj->discard_pai < PAI_1W ||
            mj->discard_pai > PAI_9T) {
        return -5;
    }
    if (mj->discarded_no == player_no)
        return -6;
    if (mj->discard_pai == mj->joker)
        return -7;
    if (mj->cai_piao_no != -1) {
        /* 有人财飘 */
        if (mj->cai_piao_no != player_no)
            return -8;
    }

    player = &mj->players[player_no];
    st = mj->pf_seat_no(player_no, stLeft);
    if (st != mj->discarded_no)
        return -9;
    if (mj->enable_3tan) {
        /* 3摊处理 */
        n = 0;
        for (i = 0; i < player->meld_index; ++i) {
            if (player->meld[i].type >= meldChiLow &&
                    player->meld[i].type <= meldChiUpper) {
                n++;
            }
        }
        if (mj->lao_z >= 3) {
            if (n == 2 && player_no != mj->dealer_no &&
                    mj->discarded_no != mj->dealer_no) {
                /* 3老庄闲家之间不能吃3摊 */
                return -10;
            }
        }
    }
    if (pai1 != 0 && pai2 != 0) {
        if (player->hand_js[pai1] == 0)
            return -11;
        if (player->hand_js[pai2] == 0)
            return -11;
    }

    return 1;
}

int _check_peng(mjhz_t* mj, int player_no)
{
    if (!mj)
        return -1;
    if (mj->game_state != GAME_PLAY)
        return -2;
    if (player_no >= mj->player_num)
        return -3;
    if (mj->discard_pai == 0)
        return -4;
    if (mj->discarded_no == player_no)
        return -5;
    if (mj->discard_pai == mj->joker)
        return -6;
    if (mj->cai_piao_no != -1) {
        if (mj->cai_piao_no != player_no)
            return -7;
    }

    return 1;
}

void mjhz_init(mjhz_t* mj, int mode, int player_num)
{
    int i,j,n;

    if (!mj)
        return;

    memset(mj, 0, sizeof(mjhz_t));
    mj->lao_z = 1;
    mj->enable_chi = 1;
    mj->enable_qg = 0;          /* 现在没有抢杠了 */
    mj->enable_dian_hu = 0;     /* 现在没有捉冲了 */
    mj->enable_3tan = 1;
    if (mode == GAME_MODE_SERVER)
        mj->mode = GAME_MODE_SERVER;
    else
        mj->mode = GAME_MODE_CLIENT;
    mj->player_num = player_num;
    if (player_num == 2) {
        mj->pf_relative_seat = p2_relative_seat;
        mj->pf_seat_no = p2_seat_no;
    } else {
        mj->pf_relative_seat = p4_relative_seat;
        mj->pf_seat_no = p4_seat_no;
    }
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
    int i,j,k,seat,n;

    if (!mj)
        return;
    mj->round = 0;
    mj->game_state = GAME_PLAY;
    mj->logic_state = lsDiscard;
    time(&mj->time_start);
    mj->inning++;

    mj->dice[0] = rand() % 6 + 1;
    mj->dice[1] = rand() % 6 + 1;

    if (mj->enable_dl) {
        /* 笃老 */
        if (mj->dice[0] == mj->dice[1] ||
                mj->dice[0] + mj->dice[1] > 9) {
            if (mj->lao_z < 3)
                mj->lao_z = 3;
        }
    }

    /* 白板是财神 */
    mj->joker = PAI_BAI;
    mj->discard_pai = 0;
    mj->discarded_no = -1;
    mj->cai_piao_no = -1;
    mj->gang_pai = 0;

    _reset_wait_req(mj);
    for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
        memset(mj->players[i].hand, 0,
               sizeof(mj->players[i].hand));
        memset(mj->players[i].hand_js, 0,
               sizeof(mj->players[i].hand_js));
        mj->players[i].meld_index = 0;
        memset(&mj->players[i].meld, 0,
               sizeof(mj->players[i].meld));
        mj->players[i].keep_gang = 0;
        mj->players[i].pass_hu = 0;
        memset(&mj->players[i].hu, 0,
               sizeof(mjhz_hu_t));
        memset(mj->players[i].pai_peng, 0,
               sizeof(mj->players[i].pai_peng));
        memset(mj->players[i].pai_gang, 0,
               sizeof(mj->players[i].pai_gang));
        mj->players[i].discard_index = 0;
        memset(mj->players[i].discard, 0,
               sizeof(mj->players[i].discard));
    }

    if (mj->mode == GAME_MODE_SERVER) {
        /* 洗牌 */
        mj_shuffle(mj->deck, mj->deck_all_num);
        /* 计算起手牌位置 */
        seat = (mj->dealer_no + mj->player_num
                - (mj->dice[0] + mj->dice[1]) % mj->player_num)
            % mj->player_num;
        mj->deck_deal_index = seat * 17 * 2
            + (mj->dice[0] + mj->dice[1]) * 2;
        mj->deck_deal_gang = mj->deck_deal_index - 1; /* 杠抓牌 */
        mj->deck_deal_end = (mj->deck_deal_index + mj->deck_all_num - 20)
                % mj->deck_all_num;

        /* 逆时针,每人抓12张,庄家先抓,一次4张 */
        for (j = 0; j < 3; ++j) {
            for (i = 0; i < mj->player_num; ++i) {
                seat = mj->pf_seat_no(mj->dealer_no, i);
                for (k = 0; k < 4; ++k) {
                    mj->players[seat].hand[j*4+k] =
                            mj->deck[mj->deck_deal_index++];
                    if (mj->deck_deal_index == mj->deck_all_num)
                        mj->deck_deal_index = 0;
                }
            }
        }
        /* 每人一张 */
        for (i = 0; i < mj->player_num; ++i) {
            seat = mj->pf_seat_no(mj->dealer_no, i);
            mj->players[seat].hand[12] =
                    mj->deck[mj->deck_deal_index++];
            if (mj->deck_deal_index == mj->deck_all_num)
                mj->deck_deal_index = 0;
        }

        /* 庄家14张 */
        mj->players[mj->dealer_no].hand[13] =
                mj->deck[mj->deck_deal_index++];
        if (mj->deck_deal_index == mj->deck_all_num)
            mj->deck_deal_index = 0;
        mj->deck_valid_num = mj->deck_all_num -
                13 * (mj->player_num - 1) + 14;

        /* 初始化分析数据 */
        for (i = 0; i < mj->player_num; ++i) {
            if (i == mj->dealer_no)
                k = 14;
            else
                k = 13;
            for (j = 0; j < k; ++j) {
                n = mj->players[i].hand[j];
                if (n < PAI_1W || n > PAI_BAI) {
                    printf("!!!error:find mjpai index > PAI_BAI!\n");
                    continue;
                }
                mj->players[i].hand_js[n]++;
            }
        }
        mj->first_player_no = mj->dealer_no;
        mj->curr_player_no = mj->first_player_no;
        time(&mj->time_turn);
        mj->sec_wait = WAITTIME_DISCARD;
        /* 庄家能杠胡吗 */
        mjhz_can_gang(mj, mj->curr_player_no);
        mjhz_can_hu(mj, mj->curr_player_no);
    } else {
        /* 初始化，真实数据需要收到服务器数据赋值。 */
        for (i = 0; i < MJHZ_MAX_PLAYERS; i++) {
            memset(mj->players[i].hand, 0,
                   sizeof(mj->players[i].hand));
            memset(mj->players[i].discard, 0,
                   sizeof(mj->players[i].discard));
        }
        mj->dealer_no = -1;
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
 * 抓牌
 * 抓到的牌规定放在数组的最后，等打出后排序
 */
int mjhz_pickup(mjhz_t* mj, int is_gang)
{
    int pai;
    mjhz_player_t* player;

    if (!mj)
        return -1;
    if (mj->discard_pai != 0) {
        player = &mj->players[mj->discarded_no];
        if (player->discard_index >= MJHZ_MAX_DISCARDED)
            player->discard_index = 0;
        player->discard[player->discard_index] =
                mj->discard_pai;
        player->discard_index++;
        mj->discard_pai = 0;
        mj->discarded_no = -1;
    }
    if (mj->deck_valid_num <= 20) {
        /* 流局 */
        return -2;
    }
    _reset_wait_req(mj);
    mj->gang_pai = 0;
    if (is_gang) {
        pai = mj->deck[mj->deck_deal_gang];
        mj->deck_deal_gang--;
        if (mj->deck_deal_gang < 0)
            mj->deck_deal_gang += mj->deck_all_num;
    } else {
        pai = mj->deck[mj->deck_deal_index];
        mj->deck_deal_index++;
        if (mj->deck_deal_index >= mj->deck_all_num)
            mj->deck_deal_index = 0;
        /* 重置弃胡弃碰 */
        mj->players[mj->curr_player_no].pass_hu = 0;
        memset(mj->players[mj->curr_player_no].pai_peng, 0,
               sizeof(mj->players[mj->curr_player_no].pai_peng));
    }
    mj->deck_valid_num--;
    mj->players[mj->curr_player_no].hand[MJHZ_MAX_HAND-1] = pai;
    mj->players[mj->curr_player_no].hand_js[pai]++;
    mj->logic_state = lsDiscard;
    mj->discard_pai = 0;
    mj->discarded_no = -1;
    mj->sec_wait = WAITTIME_DISCARD;

    /* 杠胡判定 */
    mjhz_can_hu(mj, mj->curr_player_no);
    mjhz_can_gang(mj, mj->curr_player_no);
    if (mj->pf_event)
        mj->pf_event(mjEventPickup, mj->curr_player_no, is_gang);

    return 1;
}

int mjhz_discard(mjhz_t* mj, int player_no, int pai_id)
{
    int i,n,flag;

    if (!mj)
        return -1;
    if (mj->game_state != GAME_PLAY) {
        if (mj->debug)
            printf("discard pai but game state not play.\n");
        return -2;
    }
    if (mj->logic_state != lsDiscard)
        return -3;
    if (player_no != mj->curr_player_no)
        return -4;
    if (pai_id < PAI_1W || pai_id > PAI_BAI)
        return -5;

    /* 有效检查并删除这张牌 */
    n = -1;
    for (i = 0; i < MJHZ_MAX_HAND; ++i){
        if (mj->players[player_no].hand[i] == pai_id) {
            n = i;
            break;
        }
    }
    if (n == -1) {
        if (mj->debug) {
            printf("discard pai but player hasn't %d.\n", pai_id);
        }
        return -4;
    } else {
        mj->players[player_no].hand[n] = 0;
        mj->players[player_no].hand_js[pai_id]--;
    }
    /* 财飘处理 */
    if (mj->cai_piao_no != -1 && player_no == mj->cai_piao_no) {
        mj->cai_piao_no = -1;
    }
    if (pai_id == mj->joker) {
        mj->cai_piao_no = player_no;
        mj->players[player_no].hu.cai_piao++;
    } else {
        mj->players[player_no].hu.cai_piao = 0;
    }
    mj->discard_pai = pai_id;
    mj->discarded_no = player_no;
    mj_trim(mj->players[player_no].hand, MJHZ_MAX_HAND);
    mj->logic_state = lsTake;

    /* 判定吃碰杠胡 */
    flag = 0;
    for (i = 0; i < mj->player_num; ++i) {
        if (i != player_no) {
            flag |= mjhz_can_hu(mj, i);
            flag |= mjhz_can_gang(mj, i);
            flag |= mjhz_can_peng(mj, i);
            flag |= mjhz_can_chi(mj, i);
        } else {
            mj->players[i].keep_gang = 0;
            mj->players[i].wait_hu = 0;
            mj->players[i].wait_gang = 0;
            mj->players[i].wait_peng = 0;
            mj->players[i].wait_chi = 0;
            memset(mj->players[i].pai_gang, 0,
                   sizeof(mj->players[i].pai_gang));
        }
    }
    if (flag) {
        mj->sec_wait = WAITTIME_TAKE;
        time(&mj->time_turn);
    }
    else
        mj->sec_wait = 0;

    return 1;
}

void mjhz_referee(mjhz_t *mj)
{
    int i;

    if (!mj)
        return;
    if (mj->game_state != GAME_PLAY)
        return;

    /* 有人请求胡吗 */
    for (i = 0; i < mj->player_num; ++i) {
        if (mj->players[i].req_hu > 0) {
            mjhz_hu(mj, i);
            return;
        }
    }

    /* 杠 */
    for (i = 0; i < mj->player_num; ++i) {
        if (mj->players[i].req_gang > 0) {
            mjhz_gang(mj, i, mj->discard_pai);
            return;
        }
    }
    /* 碰 */
    for (i = 0; i < mj->player_num; ++i) {
        if (mj->players[i].req_peng > 0) {
            mjhz_peng(mj, i);
            return;
        }
    }
    /* 吃 */
    for (i = 0; i < mj->player_num; ++i) {
        if (mj->players[i].req_chi > 0) {
            mjhz_chi(mj, i, mj->players[i].req_chi,
                     mj->players[i].req_chi2);
            return;
        }
    }

    if (mj->step_by_step)
        return;

    /* 下家抓牌 */
    mjhz_next_player(mj);
    if (mjhz_pickup(mj, 0) == -2) {
        /* 流局 */
        mj->game_state = GAME_END;
        if (mj->pf_event)
            mj->pf_event(mjEventLiu, 0, 0);
    }
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
    int n,chi_info;
    mjpai_t pai;
    int pos1,pos2,pos3;
    mjhz_player_t* player;

    n = _check_chi(mj, player_no, 0, 0);
    if (n < 0)
        return 0;
    player = &mj->players[player_no];
    player->wait_chi = 0;

    chi_info = 0;
    pos1 = pos2 = pos3 = 0;
    mjpai_init_id(&pai, mj->discard_pai);
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
            chi_info &= 0x01;
        }
    }
    if (pos2) {
        if (player->hand_js[pai.sign-1] > 0 &&
                player->hand_js[pai.sign+1] >0) {
            chi_info &= 0x02;
        }
    }
    if (pos3) {
        if (player->hand_js[pai.sign-1] > 0 &&
                player->hand_js[pai.sign-2] >0) {
            chi_info &= 0x04;
        }
    }
    player->wait_chi = chi_info;
    return chi_info;
}

/*
 * 0 - 不能碰
 * 1 - 能碰
 * 2 - 财碰
 */
int mjhz_can_peng(mjhz_t* mj, int player_no)
{
    int r;
    mjhz_player_t* player;

    r = _check_peng(mj, player_no);
    if (r < 0)
        return 0;

    player = &mj->players[player_no];
    if (player->hand_js[mj->discard_pai] >= 2)
        player->pai_peng[mj->discard_pai]++;

    if (player->pai_peng[mj->discard_pai] == 1)
        player->wait_peng = 1;
    else
        player->wait_peng = 0;

    return player->wait_peng;
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
    if (mj->cai_piao_no != -1) {
        if (!mj->enable_cp_gang) {
            if (mj->cai_piao_no != player_no)
                return 0;
        }
    }

    if (mj->curr_player_no == player_no &&
            mj->logic_state == lsDiscard) {
        /* 轮到我且弃牌阶段，暗杠或者加杠 */
        /* 有没有暗杠 */
        for (i = 1; i < MJHZ_LEN_JS; i++) {
            if (player->hand_js[i] == 4 && i != mj->joker) {
                player->pai_gang[num++] = i;
            }
        }
        /* 加杠(已经碰了，再摸一张) */
        for (i = 0; i < MJHZ_MAX_MELD; ++i) {
            if (player->meld[i].type == meldPengLeft ||
                    player->meld[i].type == meldPengOpposit ||
                    player->meld[i].type == meldPengRight) {
                x = player->meld[i].pai_id;
                if (player->hand_js[x] > 0) {
                    player->pai_gang[num++] = x;
                }
            }
        }
    } else {
        /* 明杠(杠打出的牌) */
        if (mj->discard_pai == 0)
            return -3;
        if (mj->discarded_no == player_no)
            return -4;
        if (mj->discard_pai == mj->joker) {
            return -5;
        }
        x = mj->discard_pai;
        if (player->hand_js[x] == 3) {
            player->pai_gang[num++] = x;
        }
    }
    if (num > 0)
        player->wait_gang = 1;
    else
        player->wait_gang = 0;

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
    for (i = PAI_DF; i < PAI_BAI; ++i) {
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
    if (mj_hornor_melded(array, &left_joker) == 0)
        return 0;

    /* 序数牌 */
    if (mj_range_melded(array, PAI_1W, &left_joker) == 0)
        return 0;
    if (mj_range_melded(array, PAI_1S, &left_joker) == 0)
        return 0;
    if (mj_range_melded(array, PAI_1T, &left_joker) == 0)
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
    int pai_pickup; /* 刚刚摸到的牌 */
    int js[MJHZ_LEN_JS];
    int js_joker[MJHZ_LEN_JS];
    mjhz_player_t *player;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;

    player = &mj->players[player_no];
    player->wait_hu = 0;
    player->hu.fan = 0;
    player->hu.is_baotou = 0;
    player->hu.is_pair7 = 0;
    player->hu.is_qg = 0;
    player->hu.is_tianhu = 0;
    player->hu.pair7_h4 = 0;
    /* 漏胡后不能胡 */
    if (player->pass_hu > 0)
        return 0;
    n_joker = 0;
    pai_pickup = PAI_EMPTY;
    memcpy(js, player->hand_js, sizeof(js));
    if (mj->discard_pai != 0) {
        /* 配置开关 */
        if (!mj->enable_dian_hu)
            return 0;
        /* 捉冲，财神不能捉。 */
        if (mj->discard_pai == mj->joker)
            return 0;
        /* 三老庄才能捉冲 */
        if (mj->lao_z < 3)
            return 0;
        /* 闲家之间不能捉 */
        if (player_no != mj->dealer_no &&
                mj->discarded_no != mj->dealer_no)
            return 0;
        /* 非财飘选手不能捉冲 */
        if (mj->cai_piao_no != -1) {
            if (mj->cai_piao_no != player_no)
                return 0;
        }
        js[mj->discard_pai]++;
    } else if (mj->gang_pai != 0) {
        /* 抢杠 */
        if (mj->gang_pai == mj->joker)
            return 0;
        js[mj->gang_pai]++;
    } else {
        /* 自摸 */
        pai_pickup = player->hand[MJHZ_MAX_HAND-1];
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
    if (n_pai == 0)
        return 0;
    if (player->hu.is_pair7 &&
            (n_pai + n_joker) == MJHZ_MAX_HAND) {
        /* 是7对子 */
        player->hu.pair7_h4 = n_4;

        /* 判定爆头 */
        if (n_joker > 0) {
            left_joker = n_joker - 1;
            if (pai_pickup != PAI_EMPTY)
                js_joker[pai_pickup]--;
            player->hu.is_baotou = 1;
            for (i = 1; i < MJHZ_LEN_JS; ++i) {
                if (js_joker[i] % 2 == 0)
                    continue;
                else {
                    if (left_joker > 0) {
                        left_joker--;
                    } else {
                        player->hu.is_baotou = 0;
                        break;
                    }
                }
            }
        }
        if (mj->discard_pai != 0 && player->hu.is_baotou) {
            /* 爆头不能捉冲 */
            player->wait_hu = 0;
        } else {
            player->wait_hu = 1;
        }
        return player->wait_hu;
    } else {
        player->hu.is_pair7 = 0;
    }

    /* 是否爆头 */
    if (n_joker > 0 && pai_pickup != PAI_EMPTY) {
        memcpy(js_joker, js, sizeof(js_joker));
        left_joker = n_joker - 1;
        if (pai_pickup == mj->joker)
            left_joker--;
        else
            js_joker[pai_pickup]--;
        if (mjhz_all_melded_joker(js_joker, left_joker)) {
            player->wait_hu = 1;
            player->hu.is_baotou = 1;
            return 1;
        }
    }

	/* 暴力枚举，将每张牌作为将牌(1张的用财神+1) 来判定是否全成面子 */
	for (i = 1; i < MJHZ_LEN_JS; ++i) {
		if (js[i] == 0) continue;
        memcpy(js_joker, js, sizeof(js_joker));
		if (js_joker[i] >= 2) {
			js_joker[i] -= 2;
			/* 判断去掉将头后是否都是面子 */
            if (mjhz_all_melded_joker(js_joker, n_joker) > 0) {
                player->wait_hu = 1;
				return 1;
            }
		} else if (js_joker[i] == 1) {
			if (n_joker > 0) {
				js_joker[i] = 0;
                if (mjhz_all_melded_joker(js_joker, n_joker - 1) > 0) {
                    player->wait_hu = 1;
					return 1;
                }
            } else {
                if (mjhz_all_melded(js_joker)) {
                    player->wait_hu = 1;
                    return 1;
                }
            }
		}
	}

    return 0;
}

/* 请求 */
int mjhz_req_chi(mjhz_t* mj, int player_no, int pai1, int pai2)
{
    int r,temp;
    mjhz_player_t* player;

    if (pai1 == 0 || pai2 == 0)
        return 0;
    r = _check_chi(mj, player_no, pai1, pai2);
    if (r < 0)
        return r;
    player = &mj->players[player_no];
    if (player->wait_chi == 0)
        return -100;
    if (pai1 == mj->joker || pai2 == mj->joker)
        return -101;
    if (pai1 > pai2) {
        temp = pai1;
        pai1 = pai2;
        pai2 = temp;
    }
    if (player->wait_chi & 0x01) {
        if (pai1 != (mj->discard_pai + 1) ||
                pai2 != (mj->discard_pai + 2))
            return -102;
    }
    if (player->wait_chi & 0x02) {
        if (pai1 != (mj->discard_pai - 1) ||
                pai2 != (mj->discard_pai + 1))
            return -102;
    }
    if (player->wait_chi & 0x04) {
        if (pai1 != (mj->discard_pai - 2) ||
                pai2 != (mj->discard_pai - 1))
            return -102;
    }
    player->req_chi = pai1;
    player->req_chi2 = pai2;
    if (_is_all_select(mj))
        mjhz_referee(mj);

    return 1;
}

int mjhz_req_peng(mjhz_t* mj, int player_no)
{
    int r;
    mjhz_player_t* player;

    r = _check_peng(mj, player_no);
    if (r < 0)
        return r;

    player = &mj->players[player_no];
    if (player->wait_peng == 0)
        return -100;
    if (player->hand_js[mj->discard_pai] < 2)
        return -101;
    player->req_peng = 1;
    if (_is_all_select(mj))
        mjhz_referee(mj);

    return 1;
}

int mjhz_req_gang(mjhz_t* mj, int player_no, int pai)
{
    int r;
    mjhz_player_t* player;

    if (!mj)
        return -1;
    if (mj->game_state != GAME_PLAY)
        return -2;
    if (player_no >= mj->player_num)
        return -3;
    if (pai == mj->joker)
        return -4;
    if (pai < PAI_1W && pai >= PAI_BAI)
        return -5;
    player = &mj->players[player_no];
    if (player->wait_gang == 0)
        return -6;
    if (mj->logic_state == lsDiscard) {
        r = mjhz_gang(mj, player_no, pai);
        if (r < 0)
            return (r-100);
    } else {
        player->req_gang = 1;
        if (_is_all_select(mj))
            mjhz_referee(mj);
    }

    return 1;
}

int mjhz_req_hu(mjhz_t* mj, int player_no)
{
    int r;
    mjhz_player_t* player;

    if (!mj)
        return -1;
    if (mj->game_state != GAME_PLAY)
        return -2;
    if (player_no >= mj->player_num)
        return -3;
    if (mj->players[player_no].wait_hu == 0)
        return -4;
    player = &mj->players[player_no];
    if (mj->logic_state == lsDiscard) {
        r = mjhz_hu(mj, player_no);
        if (r < 0)
            return r;
    } else {
        player->req_hu = 1;
        if (_is_all_select(mj))
            mjhz_referee(mj);
    }

    return 1;
}

/* 执行 */
int mjhz_chi(mjhz_t* mj, int player_no, int pai1, int pai2)
{
    int m1,m2,m3,temp;
    int i,r;
    mjpai_t t1,t2,t3;
    mjhz_player_t* player;

    r = _check_chi(mj, player_no, pai1, pai2);
    if (r < 0)
        return r;
    player = &mj->players[player_no];

    m1 = mj->discard_pai;
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
        return -100;
    if ((m3 - m2) != 1)
        return -100;
    mjpai_init_id(&t1, m1);
    mjpai_init_id(&t2, m2);
    mjpai_init_id(&t3, m3);
    if (!(t1.suit == t2.suit && t1.suit == t3.suit)) {
        return -101;
    }
    if (player->meld_index >= MJHZ_MAX_MELD)
        return -102;

    i = player->meld_index;
    player->meld[i].pai_id = mj->discard_pai;
    if (m1 == mj->discard_pai) {
        player->meld[i].type = meldChiLow;
        mj_delete(player->hand, MJHZ_MAX_HAND, m2);
        player->hand_js[m2]--;
        mj_delete(player->hand, MJHZ_MAX_HAND, m3);
        player->hand_js[m3]--;
    }
    else if (m2 == mj->discard_pai) {
        player->meld[i].type = meldChiMiddle;
        mj_delete(player->hand, MJHZ_MAX_HAND, m1);
        player->hand_js[m1]--;
        mj_delete(player->hand, MJHZ_MAX_HAND, m3);
        player->hand_js[m3]--;
    }
    else {
        player->meld[i].type = meldChiUpper;
        mj_delete(player->hand, MJHZ_MAX_HAND, m1);
        player->hand_js[m1]--;
        mj_delete(player->hand, MJHZ_MAX_HAND, m2);
        player->hand_js[m2]--;
    }
    mj_trim(player->hand, MJHZ_MAX_HAND);
    _reset_wait_req(mj);
    mj->curr_player_no = player_no;
    mj->discard_pai = 0;
    if (mj->pf_event)
        mj->pf_event(mjEventChi, player_no, 0);

    return 1;
}

int mjhz_peng(mjhz_t* mj, int player_no)
{
    int r_no;
    mjhz_player_t* player;

    r_no = _check_peng(mj, player_no);
    if (r_no < 0)
        return r_no;
    player = &mj->players[player_no];
    if (player->hand_js[mj->discard_pai] < 2)
        return -101;
    if (player->meld_index >= MJHZ_MAX_MELD) {
        if (mj->debug)
            printf("players meld overflowed!\n");
    }

    /* take the discarded tile */
    if (mj->player_num == 2) {
        player->meld[player->meld_index].type = meldPengOpposit;
    } else {
        r_no = mj->pf_relative_seat(
                    player_no,
                    mj->discarded_no);
        if (r_no == stLeft)
            player->meld[player->meld_index].type = meldPengLeft;
        else if (r_no == stOpposit)
            player->meld[player->meld_index].type = meldPengOpposit;
        else
            player->meld[player->meld_index].type = meldPengRight;
    }

    player->meld[player->meld_index].pai_id = mj->discard_pai;
    player->meld_index++;
    mj_delete(player->hand, MJHZ_MAX_HAND,
              mj->discard_pai);
    mj_delete(player->hand, MJHZ_MAX_HAND,
              mj->discard_pai);
    player->hand_js[mj->discard_pai] -= 2;
    mj_trim(player->hand, MJHZ_MAX_HAND);
    _reset_wait_req(mj);

    mj->curr_player_no = player_no;
    mj->logic_state = lsDiscard;
    mj->discard_pai = 0;
    mj->discarded_no = -1;
    mj->sec_wait = WAITTIME_DISCARD;
    if (mj->pf_event)
        mj->pf_event(mjEventPeng, player_no, 0);

    return 1;
}

/* 杠 */
int mjhz_gang(mjhz_t* mj, int player_no, int pai)
{
    int i,can_qg,peng_idx;
    mjhz_player_t* player;
    int relative,have_qg;

    if (!mj)
        return -1;
    if (player_no >= mj->player_num)
        return -2;
    if (pai == mj->joker)
        return -3;
    if (mj->cai_piao_no != -1) {
        if (!mj->enable_cp_gang) {
            if (mj->cai_piao_no != player_no)
                return 0;
        }
    }

    player = &mj->players[player_no];
    if (player->meld_index >= MJHZ_MAX_MELD) {
        if (mj->debug)
            printf("players meld overflowed!\n");
    }

    can_qg = 0;
    if (mj->logic_state == lsDiscard) {
        if (mj->curr_player_no != player_no)
            return -4;
        /* 暗杠或加杠 */
        if (player->hand_js[pai] == 4) {
            player->meld[player->meld_index].type = meldGang;
            player->meld[player->meld_index].pai_id = pai;
            player->meld_index++;
            mj_delete(player->hand, MJHZ_MAX_HAND, pai);
            mj_delete(player->hand, MJHZ_MAX_HAND, pai);
            mj_delete(player->hand, MJHZ_MAX_HAND, pai);
            mj_delete(player->hand, MJHZ_MAX_HAND, pai);
            player->hand_js[pai] = 0;
            /* 不能抢杠 */
        } else if (player->hand_js[pai] == 1) {
            /* 找到面子 */
            peng_idx = -1;
            for (i = 0; i < MJHZ_MAX_MELD; ++i) {
                if (player->meld[i].pai_id != pai)
                    continue;
                if (player->meld[i].type == meldPengLeft ||
                        player->meld[i].type == meldPengOpposit ||
                        player->meld[i].type == meldPengRight) {
                    peng_idx = i;
                    break;
                }
            }
            if (peng_idx == -1) {
                if (mj->debug)
                    printf("players jia gang but not found this peng!\n");
                return -5;
            }
            if (player->meld[peng_idx].type == meldPengLeft)
                player->meld[peng_idx].type = meldGangAddLeft;
            else if (player->meld[peng_idx].type == meldPengOpposit)
                player->meld[peng_idx].type = meldGangAddOpposit;
            else if (player->meld[peng_idx].type == meldPengRight)
                player->meld[peng_idx].type = meldGangAddRight;
            mj_delete(player->hand, MJHZ_MAX_HAND, pai);
            player->hand_js[pai] = 0;
            can_qg = 1;
        } else {
            if (mj->debug)
                printf("players gang unknow situation!\n");
            return -6;
        }
    } else {
        /* 明杠 */
        if (mj->players[player_no].hand_js[pai] != 3)
            return -7;
        if (pai != mj->discard_pai)
            return -8;
        relative = mj->pf_relative_seat(
                    player_no, mj->discarded_no);
        if (relative == stLeft)
            player->meld[player->meld_index].type = meldGangLeft;
        else if (relative == stRight)
            player->meld[player->meld_index].type = meldGangRight;
        else
            player->meld[player->meld_index].type = meldGangOpposit;
        player->meld[player->meld_index].pai_id = pai;
        player->meld_index++;
        mj_delete(player->hand, MJHZ_MAX_HAND, pai);
        mj_delete(player->hand, MJHZ_MAX_HAND, pai);
        mj_delete(player->hand, MJHZ_MAX_HAND, pai);
        mj->players[player_no].hand_js[pai] = 0;
        can_qg = 1;
    }
    mj_trim(player->hand, MJHZ_MAX_HAND);
    player->keep_gang++;
    mj->gang_pai = pai;
    mj->discard_pai = 0;
    mj->discarded_no = -1;
    mj->curr_player_no = player_no;
    _reset_wait_req(mj);
    if (mj->pf_event)
        mj->pf_event(mjEventGang, player_no, 0);
    have_qg = 0;
    if (can_qg && mj->enable_qg) {
        /* 抢杠判断 */
        for (i = 0; i < mj->player_num; ++i) {
            if (i == player_no) continue;
            mjhz_can_hu(mj, i);
            if (mj->players[i].wait_hu &&
                    !mj->players[i].hu.is_baotou) {
                /* 爆头不能抢 */
                mj->players[i].hu.is_qg = 1;
                have_qg = 1;
            }
        }
    }
    if (!have_qg) {
        mjhz_pickup(mj, 1);
        if (mj->game_state == GAME_PLAY) {
            mj->logic_state = lsDiscard;
            mj->sec_wait = WAITTIME_DISCARD;
        }
    } else {
        mj->sec_wait = WAITTIME_TAKE;
    }
    time(&mj->time_turn);

    return 1;
}

int mjhz_hu(mjhz_t* mj, int player_no)
{
    int i,bei,x_bei;
    mjhz_player_t* player;

    if (!mj)
        return -1;
    if (player_no >= mj->player_num)
        return -2;
    if (mj->players[player_no].wait_hu == 0)
        return -3;
    player = &mj->players[player_no];
    if (mj->curr_player_no == player_no) {
        /* 自摸、杠开 */
        player->hu.pao_no = -1;
        player->hu.gang = player->keep_gang;
    } else {
        /* 点炮 */
        if (!mj->enable_dian_hu)
            return -4;
        player->hand[MJHZ_MAX_HAND-1] = mj->discard_pai;
        player->hand_js[mj->discard_pai]++;
        player->hu.pao_no = mj->discarded_no;
    }
    mj->hu_player_no = player_no;
    mj->game_state = GAME_END;
    _reset_wait_req(mj);

    /* 番计算 */
    player->hu.fan = 0;
    player->hu.fan += mj->lao_z;
    if (player->hu.is_pair7) {
        player->hu.fan++;
        player->hu.fan += player->hu.pair7_h4;
    }
    if (player->hu.is_baotou)
        player->hu.fan++;
    if (player->hu.gang)
        player->hu.fan += player->hu.gang;
    if (player->hu.cai_piao)
        player->hu.fan += player->hu.cai_piao;

    bei = pow(2, player->hu.fan);
    if (player_no == mj->dealer_no) {
        mj->lao_z++;
        if (mj->lao_z > 3)
            mj->lao_z = 3;
        for (i = 0; i < mj->player_num; ++i) {
            if (i == mj->dealer_no) {
                mj->players[i].win_lose =
                        (mj->player_num - 1) * bei;
            } else {
                mj->players[i].win_lose = -1 * bei;
            }
        }
    } else {
        mj->lao_z = 1;
        x_bei = bei / 2;
        for (i = 0; i < mj->player_num; ++i) {
            if (i == mj->dealer_no)
                mj->players[i].win_lose = -1 * bei;
            else if (i != player_no)
                mj->players[i].win_lose = -1 * x_bei;
        }
        mj->players[player_no].win_lose =
                bei + (mj->player_num - 2) * x_bei;
    }

    if (mj->pf_event)
        mj->pf_event(mjEventHu, player_no, 0);

    return 1;
}

/*
 * 过判定
 * 弃牌时,其他玩家可以吃碰杠胡
 * 杠牌时,其他玩家可以胡
 */
void mjhz_pass(mjhz_t *mj, int player_no)
{
    if (!mj)
        return;
    if (player_no >= mj->player_num)
        return;

    mj->players[player_no].req_pass = 1;
    if (mj->players[player_no].wait_hu)
        mj->players[player_no].pass_hu = 1;
    if (_is_all_select(mj))
        mjhz_referee(mj);
}

/*
 * 逆时针
 * stSelf(0)->stRight(1)->stOpposit(2)->stLeft(3)
 */
void mjhz_next_player(mjhz_t* mj)
{
    if (!mj)
        return;
    mj->curr_player_no++;
    if (mj->curr_player_no >= mj->player_num)
        mj->curr_player_no = 0;
}

int mjhz_get_next(mjhz_t* mj)
{
    int next_no;

    if (!mj)
        return -1;
    next_no = mj->curr_player_no + 1;
    if (next_no >= mj->player_num)
        next_no = 0;

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
    printf("%s\n", mjpai_string(mj->discard_pai));
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
