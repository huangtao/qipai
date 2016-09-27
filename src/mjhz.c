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

    if (start != MJ_ID_1W || start != MJ_ID_1S ||
            start != MJ_ID_1T)
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
    memcpy(js, array + MJ_ID_DONG, sizeof(int) * 7);
    for (i = 0; i < 7; ++i) {
        if (js[i] % 3 == 1) {
            if (left_joker < 2)
                return 0;
        } else if (js[i] % 3 == 2) {
            if (left_joker == 0)
                return 0;
        }
        js[i] = 0;
    }
    *num_joker = left_joker;
    return 1;
}

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
        /*
         * 序数牌 1W~9T 27张
         * 字牌7张
         */
        for (j = 0; j < 34; j++) {
            mj->deck[n] = MJ_ID_1W + j;
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
    mj->game_state = MJHZ_GAME_PLAY;
    mj->inning++;

    mj->dice1 = rand() % 6 + 1;
    mj->dice2 = rand() % 6 + 1;

    /* 白板是财神 */
    mj->joker = MJ_ID_BAI;
    mj->last_played_mj = MJ_ID_EMPTY;
    mj->pai_gang = 0;

    if (mj->mode == MJHZ_MODE_SERVER) {
        /* 洗牌 */
        mj_shuffle(mj->deck, mj->deck_all_num);
        /* 计算起手牌位置 */
        direct = (mj->banker_no + MJHZ_MAX_PLAYERS
                - (mj->dice1 + mj->dice2) % MJHZ_MAX_PLAYERS)
            % MJHZ_MAX_PLAYERS;
        mj->deck_deal_index = direct * 17 * 2
            + (mj->dice1 + mj->dice2) * 2;
        /* 将牌直接交换好 */
        memcpy(temp, mj->deck + mj->deck_deal_index,
               sizeof(int) * (mj->deck_all_num - mj->deck_deal_index));
        memcpy(temp + mj->deck_deal_index, mj->deck,
               sizeof(int) * mj->deck_deal_index);
        mj->deck_deal_index = 0;
        mj->deck_deal_gang = mj->deck_all_num - 1; /* 杠抓牌 */

        /* 顺时针,每人抓12张,庄家先抓,一次4张 */
        m = 0;
        n = mj->deck_deal_index;
        for (j = 0; j < 3; ++j) {
            for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
                direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
                memcpy(mj->players[direct].tiles + m,
                       mj->deck + n, 4 * sizeof(int));
                n += 4;
            }
            m += 4;
        }

        /* 庄家跳牌2张,其他人一张 */
        mj->players[mj->banker_no].tiles[m] =  mj->deck[n];
        mj->players[mj->banker_no].tiles[m + 1] = mj->deck[n + 4];
        n++;
        for (i = 0; i < MJHZ_MAX_PLAYERS; ++i) {
            if (i == mj->banker_no) continue;
            direct = (mj->banker_no + i) % MJHZ_MAX_PLAYERS;
            mj->players[direct].tiles[m] = mj->deck[n++];
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
                n = mj->players[i].tiles[j];
                if (n <= MJ_ID_EMPTY || n > MJ_ID_BAI) {
                    printf("!!!error:find mjpai index > MJ_ID_BAI!\n");
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
            memset(mj->players[i].tiles, 0,
                   sizeof(int) * MJHZ_MAX_PAIS);
            memset(mj->players[i].tiles_played, 0,
                   sizeof(int) * MJHZ_MAX_PLAYED);
        }
        mj->banker_no = 0;
        mj->last_takes_no = 0;
        mj->first_player_no = 0;
        mj->curr_player_no = 0;
        mj->last_played_no = 0;
    }
}

/* 财神 万 索 筒 风 */
void mjhz_sort(int pais[MJHZ_MAX_PAIS])
{
    int i,j;
    int exchange;
    int temp;

    /* 选择排序 */
    for (i = 0; i < MJHZ_MAX_PAIS - 1; ++i) {
        for (j = i + 1; j < MJHZ_MAX_PAIS; ++j) {
            exchange = 0;
            if (pais[j] == MJ_ID_BAI && pais[i] != MJ_ID_BAI) {
                exchange = 1;
            } else if (pais[j] < pais[i]) {
                exchange = 1;
            }
            if (exchange) {
                temp = pais[i];
                pais[i] = pais[j];
                pais[j] = temp;
            }
        }
    }
}

/*
 * 摸牌
 * 摸到的牌规定放在数组的最后，等打出后排序
 * 无牌可以摸返回0
 */
int mjhz_takes(mjhz_t* mj, int is_gang)
{
    int pai;

    if (!mj)
        return 0;
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
    mj->players[mj->curr_player_no].tiles[MJHZ_MAX_PAIS-1] = pai;
    mj->players[mj->curr_player_no].tiles_js[pai]++;

    /* 杠胡判定 */
    mjhz_can_hu(mj, mj->curr_player_no);
    mjhz_can_gang(mj, mj->curr_player_no);

    return 1;
}

/* 打牌 */
int mjhz_play(mjhz_t* mj, int player_no, int pai_id)
{
    int i,n,flag;

    if(!mj)
        return -1;
    if (pai_id < MJ_ID_1W || pai_id > MJ_ID_BAI)
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
    n = -1;
    for (i = 0; i < MJHZ_MAX_PAIS; ++i){
        if (mj->players[player_no].tiles[i] == pai_id) {
            n = i;
            break;
        }
    }
    if (n == -1) {
        if (mj->debug) {
            printf("play pai but player hasn't this card.\n");
        }
        return -4;
    } else {
        mj->players[player_no].tiles[n] = 0;
        mj->players[player_no].tiles_js[pai_id]--;
    }
    if (pai_id == mj->joker)
        mj->players[player_no].hu.cai_piao++;
    else
        mj->players[player_no].hu.cai_piao = 0;
    mj->last_played_mj = pai_id;
    mj->last_played_no = player_no;
    mj_trim(mj->players[player_no].tiles, MJHZ_MAX_PAIS);
    /* 判定吃碰杠胡 */
    for (i = 0; i < mj->player_num; ++i) {
        if (i != player_no) {
            mjhz_can_hu(mj, i);
            mjhz_can_gang(mj, i);
            mjhz_can_peng(mj, i);
            mjhz_can_chi(mj, i);
        } else {
            mj->players[i].can_hu = 0;
            mj->players[i].can_gang = 0;
            mj->players[i].can_peng = 0;
            mj->players[i].can_chi = 0;
        }
    }

    /* 无人能吃碰胡则摸牌,否则等待 */
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
        mjhz_takes(mj, 0);
    }

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
    int chi_info;
    mjpai_t pai;
    int pos1,pos2,pos3;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (mj->curr_player_no == player_no)
        return 0;
    mj->players[player_no].can_chi = 0;

    mjpai_init_id(&pai, mj->last_played_mj);
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
        if (mj->players[player_no].tiles_js[pai.sign+1] > 0 &&
                mj->players[player_no].tiles_js[pai.sign+2] > 0) {
            chi_info &= mjChiLeft;
        }
    }
    if (pos2) {
        if (mj->players[player_no].tiles_js[pai.sign-1] > 0 &&
                mj->players[player_no].tiles_js[pai.sign+1] >0) {
            chi_info &= mjChiMiddle;
        }
    }
    if (pos3) {
        if (mj->players[player_no].tiles_js[pai.sign-1] > 0 &&
                mj->players[player_no].tiles_js[pai.sign-2] >0) {
            chi_info &= mjChiRight;
        }
    }
    mj->players[player_no].can_chi = chi_info;
    return chi_info;
}

int mjhz_can_peng(mjhz_t* mj, int player_no)
{
    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;

    mj->players[player_no].can_peng = 0;
    if (mj->curr_player_no == player_no)
        return 0;
    if (mj->last_played_mj == mj->joker) {
        return 0;
    }

    if (mj->players[player_no].tiles_js[mj->last_played_mj] >= 2) {
        mj->players[player_no].can_peng = 1;
        return 1;
    }
    else
        return 0;
}

/* 返回可以杠牌的数量 */
int mjhz_can_gang(mjhz_t* mj, int player_no)
{
    int i,num,x;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    num = 0;
    memset(mj->players[player_no].pai_gang, 0, sizeof(int) * 4);

    if (mj->curr_player_no == player_no && mj->last_takes_no == player_no) {
        /* 轮到我且已经摸过牌了，暗杠或者加杠 */
        /* 有没有暗杠 */
        for (i = 1; i < MJHZ_LEN_JS; i++) {
            if (mj->players[player_no].tiles_js[i] == 4) {
                mj->players[player_no].pai_gang[num++] = i;
            }
        }
        /* 加杠(已经碰了，再摸一张) */
        for (i = 0; i < MJHZ_MAX_SETS; ++i) {
            if (mj->players[player_no].mj_sets[i].type == mjMeldPeng) {
                x = mj->players[player_no].mj_sets[i].pai_id;
                if (mj->players[player_no].tiles_js[x] > 0) {
                    mj->players[player_no].pai_gang[num++] = x;
                }
            }
        }
    } else {
        /* 明杠(杠打出的牌) */
        if (mj->last_played_mj == mj->joker) {
            return 0;
        }
        x = mj->last_played_mj;
        if (mj->players[player_no].tiles_js[x] == 3) {
            mj->players[player_no].pai_gang[num++] = x;
        }
    }
    if (num > 0)
        mj->players[player_no].can_gang = 1;
    else
        mj->players[player_no].can_gang = 0;
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
            index = MJ_ID_1W + i * 9 + j;
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
    for (i = MJ_ID_DONG; i < MJ_ID_BAI; ++i) {
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
    if (_range_melded(array, MJ_ID_1W, &left_joker) == 0)
        return 0;
    if (_range_melded(array, MJ_ID_1S, &left_joker) == 0)
        return 0;
    if (_range_melded(array, MJ_ID_1T, &left_joker) == 0)
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

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;

    mj->players[player_no].can_hu = 0;
    memset(&mj->players[player_no].hu, 0,
           sizeof(mjhz_hu_t));

    n_joker = 0;
    memcpy(js, mj->players[player_no].tiles_js,
           sizeof(int) * MJHZ_LEN_JS);
    if (mj->last_takes_no != player_no &&
            mj->last_played_mj != MJ_ID_EMPTY) {
        /* 杭州麻将老庄才能捉冲。*/
        if (mj->lao_z == 0)
            return 0;
        /* 配置开关 */
        if (mj->enable_dian_hu == 0)
            return 0;
        /* 捉冲，财神不能捉。 */
        if (mj->last_played_mj == mj->joker) {
            return 0;
        }
        js[mj->last_played_mj]++;
    } else {
        pai_takes = mj->players[player_no].tiles[MJHZ_MAX_PAIS-1];
    }
    n_joker = left_joker = js[MJ_ID_BAI];
    js[MJ_ID_BAI] = 0;
    memset(&mj->players[player_no].hu, 0, sizeof(mjhz_hu_t));

    /* 是否七对子 */
    memcpy(js_joker, js, sizeof(int) * MJHZ_LEN_JS);
    n_pai = n_4 = 0;
    mj->players[player_no].hu.is_pair7 = 1;
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
                mj->players[player_no].hu.is_pair7 = 0;
                break;
            }
        } else if (js_joker[i] == 1) {
            if (left_joker > 0) {
                left_joker--;
            } else {
                mj->players[player_no].hu.is_pair7 = 0;
                break;
            }
        }
    }
    if (mj->players[player_no].hu.is_pair7 &&
            (n_pai + n_joker) == MJHZ_MAX_PAIS) {
        /* 是7对子 */
        mj->players[player_no].hu.pair7_h4 = n_4;

        /* 判定爆头 */
        if (n_joker > 0) {
            left_joker = n_joker - 1;
            js_joker[pai_takes]--;
            mj->players[player_no].hu.is_baotou = 1;
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
        mj->players[player_no].can_hu = 1;
        return 1;
    }

	/* 暴力枚举，将每张牌作为将牌(1张的用财神+1) 来判定是否全成面子 */
	for (i = 1; i < MJHZ_LEN_JS; ++i) {
		if (js[i] == 0) continue;
		memcpy(js_joker, js, sizeof(int) * MJHZ_LEN_JS);
		if (js_joker[i] >= 2) {
			js_joker[i] -= 2;
			/* 判断去掉将头后是否都是面子 */
            if (mjhz_all_melded_joker(js_joker, n_joker) > 0) {
                mj->players[player_no].can_hu = 1;
				return 1;
            }
		} else if (js_joker[i] == 1) {
			if (n_joker > 0) {
				js_joker[i] = 0;
                if (mjhz_all_melded_joker(js_joker, n_joker - 1) > 0) {
                    mj->players[player_no].can_hu = 1;
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
    int i,set_idx;
    mjpai_t t1,t2,t3;

    if (!mj)
        return 0;
    if (player_no >= mj->player_num)
        return 0;
    if (mj->last_played_mj == mj->joker)
        return 0;
    if (mj->last_played_mj < MJ_ID_1W ||
            mj->last_played_mj > MJ_ID_9T) {
        return 0;
    }
    if (mj->players[player_no].tiles_js[pai1] == 0)
        return 0;
    if (mj->players[player_no].tiles_js[pai2] == 0)
        return 0;

    m1 = mj->last_played_mj;
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
    for (i = 0; i < MJHZ_MAX_SETS; ++i) {
        if (mj->players[player_no].mj_sets[i].type == 0) {
            /* unused */
            set_idx = i;
            break;
        }
    }
    if (set_idx == -1) {
        if (mj->debug)
            printf("players mj_sets overflowed!\n");
    }
    mj->players[player_no].mj_sets[set_idx].type = mjMeldChi;
    mj->players[player_no].mj_sets[set_idx].pai_id = mj->last_played_mj;
    mj->players[player_no].mj_sets[set_idx].player_no = mj->curr_player_no;
    if (m1 == mj->last_played_mj) {
        mj->players[player_no].mj_sets[set_idx].extra_info = mjChiLeft;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, m2);
        mj->players[player_no].tiles_js[m2]--;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, m3);
        mj->players[player_no].tiles_js[m3]--;
    }
    else if (m2 == mj->last_played_mj) {
        mj->players[player_no].mj_sets[set_idx].extra_info = mjChiMiddle;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, m1);
        mj->players[player_no].tiles_js[m1]--;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, m3);
        mj->players[player_no].tiles_js[m3]--;
    }
    else {
        mj->players[player_no].mj_sets[set_idx].extra_info = mjChiRight;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, m1);
        mj->players[player_no].tiles_js[m1]--;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, m2);
        mj->players[player_no].tiles_js[m2]--;
    }
    mj_trim(mj->players[player_no].tiles, MJHZ_MAX_PAIS);

    mj->curr_player_no = player_no;
    mj->last_played_mj = 0;

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
    if (mj->last_played_mj == mj->joker)
        return 0;

    if (mj->players[player_no].tiles_js[mj->last_played_mj] < 2)
        return 0;

    set_idx = -1;
    for (i = 0; i < MJHZ_MAX_SETS; ++i) {
        if (mj->players[player_no].mj_sets[i].type == 0) {
            /* unused */
            set_idx = i;
            break;
        }
    }
    if (set_idx == -1) {
        if (mj->debug)
            printf("players mj_sets overflowed!\n");
    }
    mj->players[player_no].mj_sets[set_idx].type = mjMeldPeng;
    mj->players[player_no].mj_sets[set_idx].pai_id = mj->last_played_mj;
    mj->players[player_no].mj_sets[set_idx].player_no = mj->curr_player_no;
    mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS,
              mj->last_played_mj);
    mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS,
              mj->last_played_mj);
    mj->players[player_no].tiles_js[mj->last_played_mj] -= 2;
    mj_trim(mj->players[player_no].tiles, MJHZ_MAX_PAIS);

    mj->last_played_mj = 0;
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
    for (i = 0; i < MJHZ_MAX_SETS; ++i) {
        if (mj->players[player_no].mj_sets[i].type == 0) {
            /* unused */
            set_idx = i;
            break;
        }
    }
    if (set_idx == -1) {
        if (mj->debug)
            printf("players mj_sets overflowed!\n");
    }

    if (player_no == mj->curr_player_no) {
        /* 暗杠或加杠 */
        if (mj->players[player_no].tiles_js[pai] == 4) {
            mj->players[player_no].mj_sets[set_idx].type = mjMeldGang;
            mj->players[player_no].mj_sets[set_idx].pai_id = mj->last_played_mj;
            mj->players[player_no].mj_sets[set_idx].player_no = mj->curr_player_no;
            mj->players[player_no].mj_sets[set_idx].extra_info = mjGangAn;
            mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
            mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
            mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
            mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
            mj->players[player_no].tiles_js[pai] = 0;
        } else if (mj->players[player_no].tiles_js[pai] == 1) {
            /* 找到面子 */
            peng_idx = -1;
            for (i = 0; i < MJHZ_MAX_SETS; ++i) {
                if (mj->players[player_no].mj_sets[i].type == mjMeldPeng &&
                        mj->players[player_no].mj_sets[i].pai_id == pai) {
                    peng_idx = i;
                    break;
                }
            }
            if (peng_idx == -1) {
                if (mj->debug)
                    printf("players jia gang but not found this peng!\n");
                return 0;
            }
            mj->players[player_no].mj_sets[peng_idx].type = mjMeldGang;
            mj->players[player_no].mj_sets[peng_idx].extra_info = mjGangJia;
            mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
            mj->players[player_no].tiles_js[pai] = 0;
        } else {
            if (mj->debug)
                printf("players gang unknow situation!\n");
            return 0;
        }
    } else {
        /* 明杠 */
        if (mj->players[player_no].tiles_js[pai] != 3)
            return 0;
        if (pai != mj->last_played_mj)
            return 0;
        mj->players[player_no].mj_sets[set_idx].type = mjMeldGang;
        mj->players[player_no].mj_sets[set_idx].pai_id = pai;
        mj->players[player_no].mj_sets[set_idx].player_no = mj->curr_player_no;
        mj->players[player_no].mj_sets[set_idx].extra_info = mjGangMing;
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
        mj_delete(mj->players[player_no].tiles, MJHZ_MAX_PAIS, pai);
        mj->players[player_no].tiles_js[pai] = 0;

        mj->curr_player_no = player_no;
    }
    mj_trim(mj->players[player_no].tiles, MJHZ_MAX_PAIS);

    mj->last_played_mj = 0;

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
        mj->players[player_no].tiles[MJHZ_MAX_PAIS-1] = mj->last_played_mj;
        mj->players[player_no].tiles_js[mj->last_played_mj]++;
    }
    mj->hu_player_no = player_no;
    mj->game_state = MJHZ_GAME_END;

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
            mj_string(mj->players[0].tiles, MJHZ_MAX_PAIS, 10));
    printf("%s\n",
            mj_string(mj->players[1].tiles, MJHZ_MAX_PAIS, 10));
    if (mj->player_num > 2) {
        printf("%s\n",
                mj_string(mj->players[2].tiles, MJHZ_MAX_PAIS, 10));
        printf("%s\n",
                mj_string(mj->players[3].tiles, MJHZ_MAX_PAIS, 10));
    }

    printf("last mj pai:\n");
    printf("%s\n", mjpai_string(mj->last_played_mj));

    printf("current player no is %d\n", mj->curr_player_no);
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
