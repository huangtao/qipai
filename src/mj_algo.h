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

/* 断幺九 */
int mj_duan19(mjpai_t* pais, int length);

/* 清一色(Pure hand) */
int mj_qys(mjpai_t* pais, int length);

#ifdef __cplusplus
}
#endif
#endif
