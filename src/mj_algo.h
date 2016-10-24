/*
* for mahjong game algorithm
* code: huangtao117@gmail.com
*
*/
#ifndef _MJ_ALGO_H
#define _MJ_ALGO_H
#ifdef __cplusplus
extern "C" {
#endif

#include "mj.h"

/*
 * 用于判断一门是否全成面子
 * array : 计数数组
 * start : 开始索引
 * num_joker : 财神数量(返回剩余数量)
 * return : 0/1
 */
int mj_range_melded(int* array, int start, int* num_joker);

/*
 * 用于判断字牌是否全成面子
 */
int mj_hornor_melded(int* array, int* num_joker);

void _sign_meld_delete(int* array, int* num_joker);
void _hornor_meld_delete(int* array, int* num_joker);

/* 根据计数索引数组返回对子数量 */
int mj_pair_count(int* array, int len);

/* 断幺九 */
int mj_duan19(mjpai_t* pais, int length);

/* 清一色(Pure hand) */
int mj_qys(mjpai_t* pais, int length);

/* 混一色(Clean hand) */
int mj_hys(mjpai_t* pais, int length);

/* 7对子 */
int mj_pair7(int* array, int len);

/* 十三幺(国士无双 Thirteen orphans hand) */
int mj_y13(int* array, int len);

#ifdef __cplusplus
}
#endif
#endif
