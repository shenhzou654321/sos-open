/* 
 * File:   plateRec.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef PLATEREC_H
#define	PLATEREC_H
#include "../../PublicTools/publicDefine.h"
#ifdef	__cplusplus
extern "C" {
#endif
#if 1
    /* 初始化车牌检测,成功返回1,失败返回-1 */
    int InitLPD1();
    extern void (*UnInitLPD)();
    extern void (*licensePlateDetect)(int imgH, int imgW, int imgS, void* img, int imgLen, char** resultBuf, int* buflen);
    /* 初始化车牌识别,成功返回1,失败返回-1 */
    int InitLPR1();
    extern void (* UnInitLPR)();

    /********************************************************************
     * 车牌识别
     *********************************************************************/
    extern void (* licensePlateRecog)(int imgH, int imgW, int imgS, void* img, int imgLen, char** resultBuf, int* buflen);
#else
    void InitLPD(char* enginFile);


    void UnInitLPD();

    /********************************************************************
     * ���Ƽ��
     *********************************************************************/
    void licensePlateDetect(int imgH, int imgW, int imgS, void* img, int imgLen, char** resultBuf, int* buflen);


    void InitLPR(char* engineFile);

    void UnInitLPR();

    /********************************************************************
     * 车牌识别
     *********************************************************************/
    void licensePlateRecog(int imgH, int imgW, int imgS, void* img, int imgLen, char** resultBuf, int* buflen);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* PLATEREC_H */

