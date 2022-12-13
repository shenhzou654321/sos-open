/* 
 * File:   output-xml-comment.h
 * Author: fslib
 *
 * Created on 2017年6月19日, 上午11:19
 */

#ifndef OUTPUTXMLCOMMENT_H
#define	OUTPUTXMLCOMMENT_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Thread/monitor.h"
#include "record.h"
#ifdef	__cplusplus
extern "C" {
#endif
#define OUTPUTXMLCOMMENT_BaseLine 30000
    /* 记录事件,成功返回保存的文件名,文件名的内存空间在*pBuffer内,失败返回NULL */
    char * output_xml_comment_watch(struct Record_item * const pRecord_item, struct Record_item_Watch * const pWatch
            , /* 共享buffer,不为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem);
    /* 记录事件,成功返回保存的文件名,文件名的内存空间在*pBuffer内,失败返回NULL */
    char * output_xml_comment_inc(struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc
            , /* 共享buffer,不为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem);
#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUTXMLCOMMENT_H */

