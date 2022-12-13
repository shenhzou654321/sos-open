/* 
 * File:   CycleList.h
 * Author: fslib
 *
 * Created on 2017年04月25日, 上午9:38
 */
#include "../publicDefine.h"
#include <pthread.h>

#ifndef CYCLELIST_H
#define	CYCLELIST_H

#ifdef	__cplusplus
extern "C" {
#endif

    struct FsCycleListNode {
        /* 上一个 */
        struct FsCycleListNode *privious;
        /* 下一个 */
        struct FsCycleListNode *next;
    };

    typedef struct {
        /* 主节点 */
        struct FsCycleListNode node;
        /* 节点数量 */
        unsigned long nodeCount;
    } FsCycleList;
    /* 获取首节点 */
#define Fs_CycleList_get_head(cycleList) (cycleList).node.next
    /* 获取尾节点 */
#define Fs_CycleList_get_tail(cycleList) (cycleList).node.privious;
    /* 获取节点数量 */
#define Fs_CycleList_get_count(cycleList) (cycleList).nodeCount
    /* 获取节点的下一个节点 */
#define Fs_CycleList_node_get_next(pNode) ((struct FsCycleListNode*)(pNode))->next
    /* 获取节点的上一个节点 */
#define Fs_CycleList_node_get_privious(pNode) ((struct FsCycleListNode*)(pNode))->privious
#ifdef	__cplusplus
}
#endif

#endif	/* CYCLELIST_H */
