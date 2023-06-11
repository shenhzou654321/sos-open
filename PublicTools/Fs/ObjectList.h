/* 
 * File:   ObjectList.h
 * Author: fslib
 *
 * Created on 2013年1月8日, 上午9:38
 */
#include "../publicDefine.h"
#include <pthread.h>
#include <stdio.h>

#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        void **pNode;
        /* 第一个结点在pNode中开始的索引 */
        unsigned long startIndex;
        /* 节点数量 */
        unsigned long nodeCount;
        /* 当列表头或尾没有剩余空间时，增加modifyNodeCount+1个节点;当头或尾的空节点数等于modifyNodeCount+1时，删除modifyNodeCount+1个节点; */
        unsigned long modifyNodeCount;
#define FsObjectList_unsafeSize FsPointer_to_integer(&((FsObjectList*)0)->mutex);
        /* 线程同步锁 */
        pthread_mutex_t mutex;
    } FsObjectList;
    /*
     * 创建一个FsObjectList实例;
     * 返回创建的实例指针;
     */
    FsObjectList* fs_ObjectList_new__IO(/* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
 *     增加2^modifyNodeBits个节点;
 *     当列表尾的空节点数大于2^modifyNodeBits时,
 *     删除2^modifyNodeBits个节点;
 *     modifyNodeCount=2^modifyNodeBits-1 */const unsigned char modifyNodeBits);
    /*
     * 不安全地创建一个FsObjectList实例,不会分配mutex的空间,不能调用带_pthreadSafety的函数,释放需要使用带_unsafe的函数;
     * 返回创建的实例指针;
     */
    FsObjectList* fs_ObjectList_new_unsafe__IO(/* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
 *     增加2^modifyNodeBits个节点;
 *     当列表尾的空节点数大于2^modifyNodeBits时,
 *     删除2^modifyNodeBits个节点;
 *     modifyNodeCount=2^modifyNodeBits-1 */const unsigned char modifyNodeBits);
    /*
     * 创建一个FsObjectList实例;
     * 返回创建的实例指针.
     */
    FsObjectList* fs_ObjectList_new_with_externData__IO(/* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
     *     增加2^modifyNodeBits个节点;
     *     当列表尾的空节点数大于2^modifyNodeBits时,
     *     删除2^modifyNodeBits个节点;
     *     modifyNodeCount=2^modifyNodeBits-1 */const unsigned char modifyNodeBits, /* 附加数据的长度 */const unsigned int externDataLenth);
    /*
     * 不安全地创建一个FsObjectList实例,不会分配mutex的空间,不能调用带_pthreadSafety的函数,释放需要使用带_unsafe的函数;
     * 返回创建的实例指针.
     */
    FsObjectList* fs_ObjectList_new_unsafe_with_externData__IO(/* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
     *     增加2^modifyNodeBits个节点;
     *     当列表尾的空节点数大于2^modifyNodeBits时,
     *     删除2^modifyNodeBits个节点;
     *     modifyNodeCount=2^modifyNodeBits-1 */const unsigned char modifyNodeBits, /* 附加数据的长度 */const unsigned int externDataLenth);
    /*
     * 创建一个固定长度的FsObjectList实例;
     * 此实例开始节点为0并分配不小于nodeCount*sizeof(void*)长度的内存,nodeCount置为nodeCount;
     * modifyNodeBits:最小值建议不要小于2,对不需要增删和链表建议填0,当列表头或尾没有剩余空间时,
     *     增加2^modifyNodeBits个节点;
     *     当列表尾的空节点数大于2^modifyNodeBits时,
     *     删除2^modifyNodeBits个节点;
     *     modifyNodeCount=2^modifyNodeBits-1.
     * 返回创建的实例指针.
     */
    FsObjectList* fs_ObjectList_new_prefect__IO(/* 此链表会拥有的节点数量,长度是固定的 */const unsigned long nodeCount, const unsigned char modifyNodeBits);
    /*
     * 创建一个FsObjectList实例;
     * 返回创建的实例指针.
     */
    FsObjectList* fs_ObjectList_new_prefect_with_externData__IO(/* 此链表会拥有的节点数量,长度是固定的 */const unsigned long nodeCount,
            /* 最小值建议不要小于2,对不需要增删和链表建议填0,当列表头或尾没有剩余空间时,
             *     增加2^modifyNodeBits个节点;
             *     当列表尾的空节点数大于2^modifyNodeBits时,
             *     删除2^modifyNodeBits个节点;
             *     modifyNodeCount=2^modifyNodeBits-1.
             * 返回创建的实例指针. */const unsigned char modifyNodeBits, /* 附加数据的长度 */const unsigned int externDataLenth);
    /* 删除pObjectList指向的实例对象 */
    void fs_ObjectList_delete__OI(FsObjectList * const pObjectList);
    /* 删除pObjectList指向的实例对象,对象用带_unsafe的函数创建的 */
    void fs_ObjectList_delete_unsafe__OI(FsObjectList * const pObjectList);
    /*
     * 删除pObjectList指向的实例对象;
     * 根据pObjectList->nodeCount释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0，则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_allSequence__OI(FsObjectList * const pObjectList);
    /*
     * 删除pObjectList指向的实例对象,对象用带_unsafe的函数创建的;
     * 根据pObjectList->nodeCount释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0，则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_unsafe_allSequence__OI(FsObjectList * const pObjectList);
    /*
     * 删除pObjectList指向的实例对象,同时减少每个节点的引用计数(节点为FsRefer或其子类),对引用计数为0的对象进行删除;
     * 根据pObjectList->nodeCount释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0，则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_subRefer__OI(FsObjectList * const pObjectList);
    /*
     * 删除pObjectList指向的实例对象,同时减少每个节点的引用计数(节点为FsRefer或其子类),对引用计数为0的对象进行删除,对象用带_unsafe的函数创建的;
     * 根据pObjectList->nodeCount释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0，则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_unsafe_subRefer__OI(FsObjectList * const pObjectList);
    /* 删除pObjectList指向的实例对象,使用析构释放各节点空间,析构是指节点的第一个成员变量为void (*_delete)(void *const p)指针 */
    void fs_ObjectList_delete_destructor__OI(FsObjectList * const pObjectList);
    /* 删除pObjectList指向的实例对象,对象用带_unsafe的函数创建的,使用析构释放各节点空间,析构是指节点的第一个成员变量为void (*_delete)(void *const p)指针 */
    void fs_ObjectList_delete_unsafe_destructor__OI(FsObjectList * const pObjectList);
    /*
     * 删除pObjectList指向的实例对象;
     * 根据pObjectList->nodeCount用deleteNode释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0,则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_custom__OI(FsObjectList * const pObjectList, void (*const deleteNode) (void * pNode));

    /*
     * 删除pObjectList指向的实例对象,对象用带_unsafe的函数创建的;
     * 根据pObjectList->nodeCount用deleteNode释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0,则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_unsafe_custom__OI(FsObjectList * const pObjectList, void (*const deleteNode) (void * pNode));
    /*
     * 删除pObjectList指向的实例对象;
     * 根据pObjectList->nodeCount用deleteNode释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0,则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_custom2__OI(FsObjectList * const pObjectList, void (*const deleteNode) (void * pNode, void * extenP), void *const externP);

    /*
     * 删除pObjectList指向的实例对象,对象用带_unsafe的函数创建的;
     * 根据pObjectList->nodeCount用deleteNode释放pObjectList->pNode[i]指向的空间;
     * 如果pObjectList->nodeCount为0,则不释放pObjectList->pNode[i]指向的空间.
     */
    void fs_ObjectList_delete_unsafe_custom2__OI(FsObjectList * const pObjectList, void (*const deleteNode) (void * pNode, void * extenP), void *const externP);
    /* 清空pObjectList中的节点,不会释放各节点的空间 */
    void fs_ObjectList_clear(FsObjectList * const pObjectList);
    /* 线程安全地清空pObjectList中的节点,不会释放各节点的空间 */
    void fs_ObjectList_clear_pthreadSafety(FsObjectList * const pObjectList);
    /* 清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_allSequence(FsObjectList * const pObjectList);
    /* 线程安全地清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_allSequence_pthreadSafety(FsObjectList * const pObjectList);
    /* 清空pObjectList中的节点,使用析构释放各节点空间,析构是指节点的第一个成员变量为void (*_delete)(void *const p)指针 */
    void fs_ObjectList_clear_destructor(FsObjectList * const pObjectList);
    /* 线程安全地清空pObjectList中的节点,使用析构释放各节点空间,析构是指节点的第一个成员变量为void (*_delete)(void *const p)指针 */
    void fs_ObjectList_clear_destructor_pthreadSafety(FsObjectList * const pObjectList);
    /* 用deleteNode清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_custom(FsObjectList * const pObjectList_, void (*const deleteNode) (void *const pNode));
    /* 线程安全地用deleteNode清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_custom_pthreadSafety(FsObjectList * const pObjectList, void (*const deleteNode) (void *const pNode));
    /* 用deleteNode清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_custom2(FsObjectList * const pObjectList_, void (*const deleteNode) (void * pNode, void * extenP), void *const externP);
    /* 线程安全地用deleteNode清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_custom2_pthreadSafety(FsObjectList * const pObjectList, void (*const deleteNode) (void * pNode, void * extenP), void *const externP);
    /* 用deleteNode清空pObjectList中的节点,释放各节点的空间 */
    void fs_ObjectList_clear_custom_check(FsObjectList * const pObjectList, /* 清空返回1,不清空返回0,错误返回-1 */signed char (*const deleteNode) (void *const pNode));

    /* 删除pObjectList中为p的节点并重置pObjectList的长度,从from位置开始,一般用于清空链表内为NULL的节点 */
    void fs_ObjectList_resize(FsObjectList * const pObjectList, const void *const p, unsigned long from);
    /* 把pObjectList中存的指针输出到fd中 */
    void fs_ObjectList_out(FsObjectList * const pObjectList, FILE * const fd);
    /* 遍历链表 */
    void fs_ObjectList_traverse(FsObjectList * const pObjectList, void (*const traverse) (void *const pNode));
    /* 遍历链表 */
    void fs_ObjectList_traverse_destructor(FsObjectList * const pObjectList);
    /* 遍历链表 */
    void fs_ObjectList_traverse_update(FsObjectList * const pObjectList, void* (*const traverse__IO__OI) (void *const pNode));
    /* 遍历链表,支持传入两个参数 */
    void fs_ObjectList_traverse2(FsObjectList * const pObjectList, void (*const traverse) (void *const pNode, void *const p), void *const p);

    /*
     * 把node插到pObjectList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    signed char fs_ObjectList_insert_head(FsObjectList * const pObjectList, void* const node);
    /*
     * 把node线程安全地插到pObjectList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    signed char fs_ObjectList_insert_head_pthreadSafety(FsObjectList * const pObjectList, void* const node);
    /*
     * 一次性插入多个节点到pObjectList_的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_head_array(FsObjectList * const pObjectList_, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */void * ppNode[]);
    /*
     * 一次性线程安全地插入多个节点到pObjectList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_head_array_pthreadSafety(FsObjectList * const pObjectList, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */void * ppNode[]);
    /*
     * 把pInsertList_中的节点插到pObjectList_的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_ObjectList_insert_head_all(FsObjectList * const pObjectList_, /* 不能为空且节点数可为0 */const FsObjectList * const pInsertList_);
    /*
     * 把pInsertList_中的节点线程安全地插到pObjectList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_ObjectList_insert_head_all_pthreadSafety(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */const FsObjectList * const pInsertList_);
    /*
     * 把pInsertList中的所有节点全部插到pObjectList的头部,并清空pInsertList;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_head_all_clean(FsObjectList * const pObjectList_, /* 不能为空且节点数可为0 */ FsObjectList * const pInsertList_);
    /*
     * 把pInsertList中的所有节点全部线程安全地插到pObjectList的头部,并清空pInsertList;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_head_all_clean_pthreadSafety(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */ FsObjectList * const pInsertList_);
    /*
     * 把node插到pObjectList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_ObjectList_insert_tail(FsObjectList * const pObjectList, void* const node);
    /*
     * 把node线程安全地插到pObjectList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    signed char fs_ObjectList_insert_tail_pthreadSafety(FsObjectList * const pObjectList, void* const node);
    /*
     * 一次性插入多个节点到pObjectList_的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_tail_array(FsObjectList * const pObjectList_, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */void * ppNode[]);
    /*
     * 一次性线程安全地插入多个节点到pObjectList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_tail_array_pthreadSafety(FsObjectList * const pObjectList, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */void * ppNode[]);
    /*
     * 把pInsertList_中的所有节点全部插到pObjectList_的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_tail_all(FsObjectList * const pObjectList_, /* 不能为空且节点数可为0 */ const FsObjectList * const pInsertList_);
    /*
     * 把pInsertList_中的所有节点全部线程安全地插到pObjectList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_tail_all_pthreadSafety(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */ const FsObjectList * const pInsertList_);
    /*
     * 把pInsertList中的所有节点全部插到pObjectList的尾部,并清空pInsertList;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_tail_all_clean(FsObjectList * const pObjectList_, /* 不能为空且节点数可为0 */ FsObjectList * const pInsertList_);
    /*
     * 把pInsertList中的所有节点全部线程安全地插到pObjectList的尾部,并清空pInsertList;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_ObjectList_insert_tail_all_clean_pthreadSafety(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */ FsObjectList * const pInsertList_);
    /*
     * 把pInsertList中的所有节点全部插到pObjectList的尾部并拷贝每个节点的内存;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    signed char fs_ObjectList_insert_tail_all_copy(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */const FsObjectList * const pInsertList,
            /* copy的函数指针,返回节点的副本 */void *(*const copy) (void*const pNode));
    /*
     * 把pInsertList中的所有节点添加引用(每个节点的前4字节为引用计数)后全部插到pObjectList的尾部并拷贝每个节点的内存;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    signed char fs_ObjectList_insert_tail_all_addrefer(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */const FsObjectList * const pInsertList);
    /*
     * 把pInsertList中的所有节点全部插到pObjectList的尾部并拷贝每个节点的内存;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    signed char fs_ObjectList_insert_tail_all_copy2(FsObjectList * const pObjectList, /* 不能为空且节点数可为0 */const FsObjectList * const pInsertList,
            /* copy的函数指针,返回节点的副本 */void *(*const copy) (void*const pNode, /* 用户指针 */void *const p), /* 用户指针 */void *const p);
    /*
     * 把node插到pObjectList的index位置;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展,返回2;
     * 如果失败返回-1.
     */
    int fs_ObjectList_insert_at(FsObjectList * const pObjectList, /* 从0开始,必需为有效值,否则程序会异常 */unsigned long index, void* const node);
    /*
     * 把node线程安全地插到pObjectList的index位置;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展,返回2;
     * 如果失败返回-1.
     */
    signed char fs_ObjectList_insert_at_pthreadSafety(FsObjectList * const pObjectList, /* 从0开始,必需为有效值,否则程序会异常 */const unsigned long index, void* const node);
    /* 
     * 在pObjectList中查找是否包含node;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist(FsObjectList * const pObjectList, const void* const node);
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 挨个查找;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist_by_string(const FsObjectList * const pObjectList, /* 关键字符串 */const char string[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));

    /* 
     * 在pObjectList中查找是否包含前sizeof(void*)*2的内存为p1和p2的节点;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist_void2(FsObjectList * const pObjectList, const void* const p1, const void*const p2);
    /*
     * 把node按内存位置大小插到pObjectList中;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 如果有相同的地址返回0.
     */
    int fs_ObjectList_insert_order(FsObjectList * const pObjectList, void*const node);
    /*
     * 把node线程安全地按内存位置大小插到pObjectList中;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 如果有相同的地址返回0.
     */
    signed char fs_ObjectList_insert_order_pthreadSafety(FsObjectList * const pObjectList, void*const node);

    /* 
     * 在pObjectList中查找是否包含node;
     * 只有全部使用fs_ObjectList_check_*_order添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_order(FsObjectList * const pObjectList, void*const node);
    /*
     * 把string按字符串插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    signed char fs_ObjectList_insert_order_string_default(FsObjectList * const pObjectList, const char string[]);
    /*
     * 把node按关键字符串插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    int fs_ObjectList_insert_order_string(FsObjectList * const pObjectList, void* const node, /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));
    /*
     * 把node按关键字符串线程安全地插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    int fs_ObjectList_insert_order_string_pthreadSafety(FsObjectList * const pObjectList, void* const node, /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));
    /*
     * 把node按关键字符串插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    int fs_ObjectList_insert_order_string2(FsObjectList * const pObjectList, void* const node, /* 获取节点中用于比较的字符串1指针 */const char *(*const getString1) (void *const node),
            /* 获取节点中用于比较的字符串2指针 */const char *(*const getString2) (void *const node));

    /*
     * 把node按关键字符串线程安全地插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    int fs_ObjectList_insert_order_string2_pthreadSafety(FsObjectList * const pObjectList, void* const node, /* 获取节点中用于比较的字符串1指针 */const char *(*const getString1) (void *const node),
            /* 获取节点中用于比较的字符串2指针 */const char *(*const getString2) (void *const node));
    /* 
     * 在pObjectList中查找节点内容是string的节点;
     * 只有全部使用fs_ObjectList_*_string_default添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist_string_default(const FsObjectList * const pObjectList, /* 关键字符串 */const char string[]);
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 只有全部使用fs_ObjectList_*_string添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist_string(const FsObjectList * const pObjectList, /* 关键字符串 */const char string[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));
    /*
     * 把node按关键字符串插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    long fs_ObjectList_insert_order_string_offset(FsObjectList * const pObjectList, void* const node, /* 关键字符串相对节点头的偏移量 */const unsigned int keyStringOffset);
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 只有全部使用fs_ObjectList_*_string_offset添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_string_offset(const FsObjectList * const pObjectList, /* 关键字符串 */const char keyString[], /* 关键字符串相对节点头的偏移量 */const unsigned int keyStringOffset);
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 只有全部使用fs_ObjectList_*_string_offset添加节点的pObjectList才能使用此函数;
     * 成功返回节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_get_by_string_offset(const FsObjectList * const pObjectList, /* 关键字符串 */const char keyString[], /* 关键字符串长度 */const unsigned int keyStringLen
            , /* 关键字符串相对节点头的偏移量 */const unsigned int keyStringOffset);
    /*
     * 移除pObjectList中的内存为node的节点;
     * 成功返回移除的节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_remove_order_string_offset(FsObjectList * const pObjectList, /* 关键字符串 */const char keyString[], /* 关键字符串相对节点头的偏移量 */const unsigned int keyStringOffset);
    /* 
     * 在pObjectList中按关键字符串二分查找是否包含关键字符串;
     * 只有全部使用fs_ObjectList_*_string2添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist_string2(FsObjectList * const pObjectList, /* 关键字符串1 */const char string1[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString1) (void *const node)
            , /* 关键字符串2 */const char string2[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString2) (void *const node));
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 只有全部使用fs_ObjectList_*_string添加节点的pObjectList才能使用此函数;
     * 成功返回节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_get_by_string(const FsObjectList * const pObjectList, /* 关键字符串 */const char string[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));
    /*
     * 移除pObjectList中的内存为node的节点,用getString进行快速查找;
     * 成功返回移除的节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_remove_order_string(FsObjectList * const pObjectList, /* 关键字符串 */const char string[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));
    /*
     * 线程安全地移除pObjectList中的内存为node的节点,用getString进行快速查找;
     * 成功返回移除的节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_remove_order_string_pthreadSafety(FsObjectList * const pObjectList, /* 关键字符串 */const char string[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString) (const void *const node));
    /*
     * 在pObjectList中按关键字符串二分查找包含关键字符串的节点并移除;
     * 只有全部使用fs_ObjectList_*_string2添加节点的pObjectList才能使用此函数;
     * 成功返回移除的节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_remove_order_string2(FsObjectList * const pObjectList, /* 关键字符串1 */const char string1[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString1) (void *const node)
            , /* 关键字符串2 */const char string2[], /* 获取节点中用于比较的字符串指针 */const char *(*const getString2) (void *const node));
    /*
     * 把node按关键字符串插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的地址返回0.
     */
    long fs_ObjectList_insert_order_void_offset(FsObjectList * const pObjectList, void* const node, /* 关键字的长度 */ const unsigned int keyLen, /* 关键字符串相对节点头的偏移量 */const unsigned int keyOffset);
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 只有全部使用fs_ObjectList_*_void_offset添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_void_offset(const FsObjectList * const pObjectList, /* 关键字符串 */const char key[], /* 关键字的长度 */ const unsigned int keyLen, /* 关键字符串相对节点头的偏移量 */const unsigned int keyOffset);
    /* 
     * 在pObjectList中按关键字符串查找是否包含node;
     * 只有全部使用fs_ObjectList_*_void_offset添加节点的pObjectList才能使用此函数;
     * 成功返回节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_get_by_void_offset(const FsObjectList * const pObjectList, /* 关键字符串 */const char key[], /* 关键字的长度 */ const unsigned int keyLen, /* 关键字符串相对节点头的偏移量 */const unsigned int keyOffset);
    /*
     * 移除pObjectList中的内存为node的节点;
     * 只有全部使用fs_ObjectList_*_void_offset添加节点的pObjectList才能使用此函数;
     * 成功返回移除的节点指针;
     * 失败返回NULL.
     */
    void* fs_ObjectList_remove_order_void_offset(FsObjectList * const pObjectList, /* 关键字符串 */const char key[], /* 关键字的长度 */ const unsigned int keyLen, /* 关键字符串相对节点头的偏移量 */const unsigned int keyOffset);
    /*
     * 把node按自定义的方式插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的值返回0.
     */
    int fs_ObjectList_insert_custom(FsObjectList * const pObjectList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ int (*const compare) (const void *const node1, const void* const node2));
    /*
     * 把node按自定义的方式插到pObjectList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 如果有相同的值返回-(位置+1).
     */
    long fs_ObjectList_insert_custom_getIndex(FsObjectList * const pObjectList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ int (*const compare) (const void *const node1, const void* const node2));

    /*
     * 把node按自定义的方式线程安全地插到pObjectList中,按升序排序;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的值返回0.
     */
    signed char fs_ObjectList_insert_custom_pthreadSafety(FsObjectList * const pObjectList, void* const node
            , /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ int (*const compare) (const void *const node1, const void*const node2));
    /* 
     * 在pObjectList中按自定义的方式查找是否包含node;
     * 只有全部使用fs_ObjectList_*_custom添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_custom(FsObjectList * const pObjectList, const void* const node
            , /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ int (*const compare) (const void *const node1, const void* const node2));
    /* 
     * 在pObjectList中按关键int查找是否包含node;
     * 只有全部使用fs_ObjectList_*_int添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_int(FsObjectList * const pObjectList, const unsigned int key, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    /*
     * 把node按内存位置大小插到pObjectList中;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 如果有相同的地址返回0.
     */
    int fs_ObjectList_insert_order_int(FsObjectList * const pObjectList, void*const node, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    /* 
     * 在pObjectList中按关键double查找是否包含node;
     * 只有全部使用fs_ObjectList_*_double添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_double(FsObjectList * const pObjectList, const double key, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    /*
     * 移动pObjectList中的结点,从下标from移动到to;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void fs_ObjectList_move(FsObjectList * const pObjectList, unsigned long from, unsigned long to);
    /* 
     * 在pObjectList中按自定义数据大小的方式查找是否包含值为data;
     * 只有全部使用fs_ObjectList_*_order_custom添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_order_custom(FsObjectList * const pObjectList, /* 节点开始的数据内容,非节点 */const void* const data
            , /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    /* 
     * 在pObjectList中按自定义数据大小的方式查找是否包含值为data;
     * 只有全部使用fs_ObjectList_*_order_custom_offset添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_ObjectList_check_exist_order_custom_offset(FsObjectList * const pObjectList, /* 要比较的关键字的偏移量 */ const unsigned int offset, /* 节点开始的数据内容,非节点 */const void* const data
            , /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    /*
     * 把node按自定义的方式插到pObjectList中,在关键字相同时,按内存地址排序,按升序排序,移除可以使用fs_ObjectList_remove_custom;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1;
     * 有相同的值返回0.
     */
    int fs_ObjectList_insert_custom_addr(FsObjectList * const pObjectList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ signed char (*const compare) (const void *const node1, const void* const node2));
    /* 
     * 在pObjectList中按自定义的方式查找是否包含node,并进行内存匹配;
     * 只有全部使用fs_ObjectList_*_custom添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_check_exist_custom_addr(FsObjectList * const pObjectList, const void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ signed char (*const compare) (const void *const node1, const void* const node2));
    /*
     * 移除pObjectList中的内存为node的节点,用自定义方式查找,并进行内存匹配;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove_custom_addr(FsObjectList * const pObjectList, const void*const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ signed char (*const compare) (const void *const node1, const void*const node2));

    /*
     * 移除pObjectList中的[from:to]之间结点(以下标表示);
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void fs_ObjectList_remove_between(FsObjectList * const pObjectList, unsigned long from, unsigned long to);
    /*
     * 移除pObjectList中的第index个结点,从0开始返回移除了的节点指针;
     * 如果i大于pObjectList的长度,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void * fs_ObjectList_remove_at(FsObjectList * const pObjectList, unsigned long index);
    /*
     * 线程安全地移除pObjectList中的第index个结点,从0开始返回移除了的节点指针;
     * 如果i大于pObjectList的长度,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void * fs_ObjectList_remove_at_pthreadSafety(FsObjectList * const pObjectList, const unsigned long index);
    /*
     * 移除pObjectList中的内存为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove(FsObjectList * const pObjectList, const void* const node);
    /*
     * 线程安全地移除pObjectList中的内存为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove_pthreadSafety(FsObjectList * const pObjectList, const void* const node);
    /*
     * 移除pObjectList中的内存为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove_order(FsObjectList * const pObjectList, void*const node);
    /*
     * 线程安全地移除pObjectList中的内存为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove_order_pthreadSafety(FsObjectList * const pObjectList, void* const node);
    /*
     * 移除pObjectList中的内存为node的节点,用自定义方式查找;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove_custom(FsObjectList * const pObjectList, const void*const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ int (*const compare) (const void *const node1, const void*const node2));
    /*
     * 线程安全地移除pObjectList中的内存为node的节点,用自定义方式查找;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_ObjectList_remove_custom_pthreadSafety(FsObjectList * const pObjectList, const void*const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */ int (*const compare) (const void *const node1, const void*const node2));
    /*
     * 移除pObjectList中的内存为list链表内的节点;
     * 全成功返回移除的个数;
     * 失败返回失败个数的取反值.
     */
    long fs_ObjectList_remove_order_all(FsObjectList * const pObjectList, const FsObjectList * const list);
    /*
     * 移除pObjectList中的前n个节点;
     * 如果n大于pObjectList的长度为0,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void fs_ObjectList_remove_head_n(FsObjectList * const pObjectList, const unsigned long n);

    /*
     * 移除pObjectList中的前n个节点,对每个移除的节点调用deleteNode函数;
     * 如果n大于pObjectList的长度为0,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void fs_ObjectList_remove_head_n_custom(FsObjectList * const pObjectList, unsigned long n, void (*const deleteNode) (void *const pNode));
    /*
     * 移除pObjectList中的第1个节点,返回移除了的节点指针;
     * 如果pObjectList的长度为0,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void *fs_ObjectList_remove_head(FsObjectList * const pObjectList);
    /*
     * 线程安全地移除pObjectList中的第1个结点,返回移除了的节点指针;
     * 如果pObjectList的长度为0,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void *fs_ObjectList_remove_head_pthreadSafety(FsObjectList * const pObjectList);
    /*
     * 移除pObjectList中的最后1个结点,返回移除了的节点指针;
     * 如果pObjectList的长度为0,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    void *fs_ObjectList_remove_tail(FsObjectList * const pObjectList);
    /*
     * 移除pObjectList中的最后n个结点,可以是0;
     * 调用此函数前务保证pObjectList的长度不小于n.
     */
    void fs_ObjectList_remove_tail_n(FsObjectList * const pObjectList, /* 移除的数量,可以是0 */const unsigned long n);
    /*
     * 移除pObjectList中的最后n个结点,可以是0,对每个移除的节点调用deleteNode函数;;
     * 调用此函数前务保证pObjectList的长度不小于n.
     */
    void fs_ObjectList_remove_tail_n_custom(FsObjectList * const pObjectList, /* 移除的数量,可以是0 */ unsigned long n, void (*const deleteNode) (void *const pNode));
    /*
     * 移除pObjectList中的最后n个结点,可以是0,对每个移除的节点进行析构;
     * 调用此函数前务保证pObjectList的长度不小于n.
     */
    void fs_ObjectList_remove_tail_n_destructor(FsObjectList * const pObjectList, /* 移除的数量,可以是0 */ unsigned long n);
    /*
     * 移除pObjectList中的所有结点,返回移除了的节点组成的新链表的指针;
     * 调用此函数前建议检查pObjectList的长度大于1后再运行此函数.
     */
    FsObjectList *fs_ObjectList_remove_all__IO(FsObjectList * const pObjectList);
    /*
     * 线程安全地移除pObjectList中的所有结点,返回移除了的节点组成的新链表的指针;
     * 调用此函数前建议检查pObjectList的长度大于1后再运行此函数.
     */
    FsObjectList * fs_ObjectList_remove_all_pthreadSafety__IO(FsObjectList * const pObjectList);
#ifdef FsDebug
    void fs_ObjectList_test();
#endif
#ifdef __cplusplus
}
#endif

#endif /* OBJECTLIST_H */

