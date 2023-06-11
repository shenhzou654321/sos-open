/* 
 * File:   Router.h
 * Author: fslib
 *
 * Created on 2018年6月13日, 下午3:00
 */

#ifndef ROUTER_H
#define ROUTER_H
#include "../publicDefine.h"
#include <stdio.h>
#include "ObjectList.h"
#include "StructList.h"
#include "Ebml.h"

#ifdef __cplusplus
extern "C" {
#endif
#define FsRouter_callTimes

    /* 路由项 */
    struct FsRouter_item {
        /* 互斥锁指针 */
        pthread_mutex_t *pMutex;
        /* 引用次数,为0时删除 */
        unsigned int referCount;
        /* 网关 */
        unsigned int gw;
        /* 目的地,按广播域方式储存 */
        unsigned int dst;
#ifdef FsRouter_callTimes
        /* 命中次数 */
        unsigned short callTimes;
#endif
        /* 权重初始值 */
#define FsRouter_item_weight_type short
        FsRouter_item_weight_type weight0;
        /* 权重实时值,可以使用一个线程做域名检测或ip通断探测,域名无效或ip不通时设置其权重为0 */
        FsRouter_item_weight_type weight;
        /* 掩码 */
        unsigned char mask;
        /* 域名 */
#define FsRouter_item_get_domain(___pRouter_item,___nullValue) (!FsNumberIsMax(((struct FsRouter_item*)(___pRouter_item))->_domain_)?(((char*)(((struct FsRouter_item*)(___pRouter_item))+1))+((struct FsRouter_item*)(___pRouter_item))->_domain_):(___nullValue))
        unsigned char _domain_;
    };

    /* 路由 */
    typedef struct {
        /* 路由项链表,按路由排序,节点大小为三个指针大小,第一个指针储存FsRouter_item指针,第二个指针储存用户关联指针,第三个指针用于权重计算 */
        FsStructList itemList;
        /* 路由中路由项的掩码或的结果 */
        unsigned int mask;
        /* 引用计数 */
        unsigned int referCount;
        //        /* 从FsRouter中的itemList成员中获取struct FsRouter_item的方法 */
        //        struct FsRouter_item *(*getItem)(void * pItem);
    } FsRouter;
    /* 创建一个路由 */
    FsRouter *fs_Router_new__IO(///* 从FsRouter中的itemList成员中获取struct FsRouter_item的方法 */struct FsRouter_item *(*getItem)(void * pItem),
            /* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
             *     增加2^modifyNodeBits个节点;
             *     当列表尾的空节点数大于2^modifyNodeBits时,
             *     删除2^modifyNodeBits个节点;
             *     modifyNodeCount=2^modifyNodeBits-1 */const unsigned char modifyNodeBits);
    /* 清空pRouter中的节点 */
    void fs_Router_clear_pthreadSafety(FsRouter * const pRouter);
    /* 删除pRouter指向的对象 */
    void fs_Router_delete_pthreadSafety__IO(FsRouter * const pRouter);
    /* 增加pRouter的引用计数 */
    void fs_Router_addRefer_pthreadSafety(FsRouter * const pRouter);
    /* 输出pRouter的信息 */
    void fs_Router_out(FsRouter * const pRouter, FILE * const fd);
    /* pRouter转EBML输出 */
    FsEbml * fs_Router_to_ebml_pthreadSafety__IO(FsRouter * const pRouter);
    /* 在pRouter中查找匹配dst的路由信息,找到返回FsRouter_item对象,失败返回NULL */
    struct FsRouter_item * fs_Router_match(FsRouter * const pRouter, const unsigned int dst);
    /* 在pRouter中查找匹配dst的路由信息,找到返回FsRouter_item对象,失败返回NULL */
    struct FsRouter_item * fs_Router_match_pthreadSafety__IO(FsRouter * const pRouter, const unsigned int dst);
    /* 在pRouter中查找匹配dst的路由信息,找到返回路由网关地址,同时把用户指针写到*rst_user中,失败返回FS_INVALID_IPV4 */
    unsigned int fs_Router_match_ip_pthreadSafety(/* 为空,不返回用户指针 */void * * const rst_user, FsRouter * const pRouter, const unsigned int dst);
    /* 创建一个FsRouter_item对象,成功返回FsRouter_item指针,失败返回NULL */
    struct FsRouter_item *fs_Router_item_new__IO(/* 互斥锁指针 */ pthread_mutex_t * const pMutex
            , /* 目的地,如192.168.11.0/24 192.168.30.1或192.168.11.0/24 aa.com或192.168.11.0/24 aa.com 10,权重空时值为1 */const char route[]);
    /* 创建一个FsRouter_item对象,成功返回FsRouter_item指针,失败返回NULL */
    struct FsRouter_item *fs_Router_item_new_with_externData__IO(/* 互斥锁指针 */ pthread_mutex_t * const pMutex
            , /* 目的地,如192.168.11.0/24 192.168.30.1或192.168.11.0/24 aa.com或192.168.11.0/24 aa.com 10,权重空时值为1 */const char route[], /* 附加数据的长度 */const unsigned int externDataLenth);
    /* 创建一个FsRouter_item对象,成功返回FsRouter_item指针,失败返回NULL */
    struct FsRouter_item *fs_Router_item_new_ip__IO(/* 互斥锁指针 */ pthread_mutex_t * const pMutex
            , /* 目的地,网关只能是ip,如192.168.11.0/24 192.168.30.1 或192.168.11.0/24 192.168.30.1 1,权重空时值为1 */const char route[]);
    /* 创建一个FsRouter_item对象,成功返回FsRouter_item指针,失败返回NULL */
    struct FsRouter_item *fs_Router_item_new_ip_with_externData__IO(/* 互斥锁指针 */ pthread_mutex_t * const pMutex
            , /* 目的地,网关只能是ip,如192.168.11.0/24 192.168.30.1 或192.168.11.0/24 192.168.30.1 1,权重空时值为1 */const char route[], /* 附加数据的长度 */const unsigned int externDataLenth);
    /* 删除pRouter指向的对象 */
    void fs_Router_item_delete_pthreadSafety__IO(struct FsRouter_item * const pRouter_item);
    /* 删除pRouter指向的对象,内部会检查是否引用pMutex锁,未引用时加锁 */
    void fs_Router_item_delete_checkLocked__IO(struct FsRouter_item * const pRouter_item, /* 此锁已被锁住,如pRouter_item也使用此锁,则不需要加锁 */pthread_mutex_t * const pMutex);
    /* 增加pRouter_item的引用计数 */
    void fs_Router_item_addRefer_pthreadSafety(struct FsRouter_item * const pRouter_item);
    /* 向pRouter中添加一个FsRouter_item条目 */
    void fs_Router_add_item_pthreadSafety__OI_2(FsRouter * const pRouter, struct FsRouter_item * const pRouter_item, /* 关联的用户指针 */void *const userData);

#ifdef FsDebug
    void fs_Router_test();
#endif
#ifdef __cplusplus
}
#endif

#endif /* ROUTER_H */

