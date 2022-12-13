/* 
 * File:   output-ebml.h
 * Author: fslib
 *
 * Created on 2017年6月19日, 上午11:19
 */

#ifndef OUTPUTEBML_H
#define	OUTPUTEBML_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Thread/monitor.h"
#include "record.h"
#ifdef	__cplusplus
extern "C" {
#endif
#define OUTPUTEBML_BaseLine 30000
    void output_embl_inc(struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc, char **pBuffer, unsigned int *pBufferLen, MonitorItem * const pMonitorItem);
#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUTEBML_H */

