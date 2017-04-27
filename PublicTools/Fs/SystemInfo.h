/* 
 * File:   SystemInfo.h
 * Author: RuiXue
 *
 * Created on 2013年5月12日, 下午4:09
 */

#ifndef SYSTEMINFO_H
#define	SYSTEMINFO_H
#include "../publicDefine.h"
#ifdef	__cplusplus
extern "C" {
#endif
 
    /* 线程延时,延时到toUpTime,单位秒,有睡眠返回1,无睡眠返回-1 */
    const signed char fs_thread_sleepTo(double toUpTime);
 
#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEMINFO_H */

