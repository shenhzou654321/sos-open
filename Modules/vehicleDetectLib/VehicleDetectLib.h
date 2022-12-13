/* 
 * File:   VehicleDetectLib.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef VEHICLEDETECTLIB_H
#define	VEHICLEDETECTLIB_H
#include "../../PublicTools/publicDefine.h"
#ifdef	__cplusplus
extern "C" {
#endif
#if 1
    /* 初始化车辆检测,成功返回1,失败返回-1 */
    int InitVD1();
    extern void (*UnInitVD)();
    extern void (*vehicleDetect)(int imgH, int imgW, int imgS, void* img, int imgLen, char** resultBuf, int* buflen);
#else
    /********************************************************************
     * 初始化/反初始化
     *********************************************************************/
    void InitVD(char* enginFile);

    void UnInitVD();

    /********************************************************************
     * 车牌检测
     *********************************************************************/
    void vehicleDetect(int imgH, int imgW, int imgS, void* img, int imgLen, char** resultBuf, int* buflen);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* VEHICLEDETECTLIB_H */

