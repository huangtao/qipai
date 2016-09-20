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
