/* 
 * File:   LinkList.h
 * Author: fslib
 *
 * Created on 2017年04月25日, 上午9:38
 */
#include "../publicDefine.h"
#include <pthread.h>

#ifndef LINKLIST_H
#define	LINKLIST_H

#ifdef	__cplusplus
extern "C" {
#endif

    struct FsLinkListNode {
        /* 上一个 */
        struct FsLinkListNode *privious;
        /* 下一个 */
        struct FsLinkListNode *next;
    };

    typedef struct {
        /* 首节点 */
        struct FsLinkListNode *head;
        /* 尾节点 */
        struct FsLinkListNode *tail;
        /* 节点数量 */
        unsigned long nodeCount;
    } FsLinkList;
#ifdef	__cplusplus
}
#endif

#endif	/* LINKLIST_H */

