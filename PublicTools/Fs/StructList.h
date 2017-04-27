/* 
 * File:   StructList.h
 * Author: fslib
 *
 * Created on 2015年4月24日, 下午4:07
 */
#ifndef STRUCTLIST_H
#define	STRUCTLIST_H
#include "../publicDefine.h"
#include <pthread.h>
#ifdef	__cplusplus
extern "C" {
#endif

    /* 结构链表,与FsObjectList的区别是FsObjectList储存的是指针,FsStructList储存的是结构,FsObjectList可看作是FsStructList的一个特例 */
    typedef struct {
        /* 要储存的结构数据,连续储存 */
        char *pNode;
        /* 第一个结点在pNode中开始的索引 */
        unsigned long startIndex;
        /* 节点数量 */
        unsigned long nodeCount;
        /* 当列表头或尾没有剩余空间时，增加modifyNodeCount+1个节点;当头或尾的空节点数等于modifyNodeCount+1时，删除modifyNodeCount+1个节点; */
        unsigned long modifyNodeCount;
        /* 线程同步锁 */
        pthread_mutex_t mutex;
        /* 要储存的结构大小,当此值为sizeof(void*)时可把此对象强制转换为FsObjectList来使用 */
        unsigned long structSize;
    } FsStructList;
    /*
     * 创建一个FsStructList实例;
     * 返回创建的实例指针.
     */
    FsStructList* fs_StructList_new__IO(/* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
     *     增加2^modifyNodeBits个节点;
     *     当列表尾的空节点数大于2^modifyNodeBits时,
     *     删除2^modifyNodeBits个节点;
     *     modifyNodeCount=2^modifyNodeBits-1;
     */const unsigned char modifyNodeBits, /* 要储存的结构大小,不能为0 */const unsigned long structSize);
    /* 删除pStructList指向的实例对象 */
    void fs_StructList_delete__OI(FsStructList * const pStructList);
    /* 清空pStructList中的节点,不会释放各节点的空间 */
    void fs_StructList_clear(FsStructList * const pStructList);
    /*
     * 把node的数据拷贝插到pStructList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    signed char fs_StructList_insert_head(FsStructList * const pStructList, /* 结构的指针 */void* const node);
    /*
     * 把node的数据线程安全地拷贝插到pStructList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    char fs_StructList_insert_head_pthreadSafety(FsStructList* pStructList, /* 结构的指针 */void* node);
    /*
     * 移除pStructList中的首结点;
     * 如果pStructList的长度为0,程序会出错;
     * 调用此函数前务必对pStructList的长度进行检查.
     */
    void fs_StructList_remove_head(FsStructList * const pStructList);
    /*
     * 把node的数据拷贝插到pStructList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    signed char fs_StructList_insert_tail(FsStructList * const pStructList, /* 结构的指针 */const void* const node);
    /*
     * 把node的数据线程安全地拷贝插到pStructList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    signed char fs_StructList_insert_tail_pthreadSafety(FsStructList * const pStructList, /* 结构的指针 */const void* node);
    /*
     * 把node的数据拷贝插到pStructList的index位置;
     * index:从0开始,必需为有效值,否则程序会异常;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展,返回2;
     * 如果失败返回-1.
     */
    signed char fs_StructList_insert_at(FsStructList * const pStructList, unsigned long index, const void* const node);

    /*
     * 移除pStructList中的第i个结点;
     * 如果i大于pStructList的长度,程序会出错;
     * 调用此函数前务必对pStructList的长度进行检查.
     */
    void fs_StructList_remove_at(FsStructList* pStructList, unsigned long i);
    /*
     * 把node的数据按默认的排序方式插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order(FsStructList * const pStructList, const void* const node);
    /*
     * 把node的数据线程安全地按默认的排序方式插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_pthreadSafety(FsStructList * const pStructList, const void* const node);
    /*
     * 把node的数据按指针大小的排序方式插到pStructList中,按升序排序,插入对象的大小必须是指针大小的倍数;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_void(FsStructList * const pStructList, const void* const node);
    /*
     * 把node的数据按自定义的方式插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */signed char (*compare)(void *node1, void* node2));
    /* 
     * 在pStructList中全字段查找是否包含node;
     * 只有全部使用fs_StructList_*添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_check_exist(const FsStructList * const pStructList, const void* const node);
    /* 
     * 在pStructList中按默认排序的方式查找是否包含node;
     * 只有全部使用fs_StructList_*_order添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_check_exist_order(FsStructList * const pStructList, const void* const node);
    /* 
     * 在pStructList中按指针数据大小的方式查找是否包含值为voidData,对象的大小必须是指针大小的倍数;
     * 只有全部使用fs_StructList_*_order_void添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_check_exist_order_void(FsStructList * const pStructList, /* 值为关键的指针数据,非节点 */const void* const voidData);
    /* 
     * 在pStructList中按自定义的方式查找是否包含node;
     * 只有全部使用fs_StructList_*_custom添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_check_exist_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */signed char (*compare)(void *node1, void* node2));
    /*
     * 移除pStructList中的数据为node的节点;
     * 只有全部使用fs_StructList_*_order添加节点的pStructList才能使用此函数;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_order(FsStructList * const pStructList, const void* const node);
    /*
     * 线程安全地移除pStructList中的数据为node的节点;
     * 只有全部使用fs_StructList_*_order添加节点的pStructList才能使用此函数;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_order_pthreadSafety(FsStructList * const pStructList, const void* const node);
    /*
     * 移除pStructList中的数据为node的节点,对象的大小必须是指针大小的倍数;
     * 只有全部使用fs_StructList_*_order_void添加节点的pStructList才能使用此函数;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_order_void(FsStructList * const pStructList, /* 值为关键的指针数据,非节点 */const void* const voidData);
    /*
     * 移除pStructList中的数据为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */signed char (*compare)(void *node1, void* node2));
    /*
     * 更新pStructList中的数据为node的节点的值;
     * 成功返回节点的位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_update_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */signed char (*compare)(void *node1, void* node2));
#ifdef FsDebug
    void fs_StructList_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* STRUCTLIST_H */

