#ifndef STRUCTLIST_H
#define STRUCTLIST_H
#include "../publicDefine.h"
#include <pthread.h>
#ifdef __cplusplus
extern "C" {
#endif

    /* 结构链表,与FsObjectList的区别是FsObjectList储存的是指针,FsStructList储存的是结构,FsObjectList可看作是FsStructList的一个特例 */
    typedef struct __FsStructList {
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
    FsStructList* fs_StructList_new__IO(const unsigned char modifyNodeBits, /* 要储存的结构大小,不能为0 */const unsigned long structSize);
    FsStructList* fs_StructList_new_with_externData__IO(const unsigned char modifyNodeBits, /* 要储存的结构大小,不能为0 */const unsigned long structSize, /* 附加数据的长度 */const unsigned int externDataLenth);
    /* 删除pStructList指向的实例对象 */
    void fs_StructList_delete__OI(FsStructList * const pStructList);
    void fs_StructList_delete_custom__OI(FsStructList * const pStructList, void (*const deleteNode) (const char *const pNode));
    /* 清空pStructList中的节点,不会释放各节点的空间 */
    void fs_StructList_clear(FsStructList * const pStructList);
    /* 清空pStructList中的节点,不会释放各节点的空间 */
    void fs_StructList_clear_pthreadSafety(FsStructList * const pStructList);
    /* 遍历成员 */
    void fs_StructList_traverse(FsStructList * const pStructList, void (*const traverse) (char *const pNode));
    /* 遍历成员,支持传入两个参数 */
    void fs_StructList_traverse2(FsStructList * const pStructList, void (*const traverse) (char *const pNode, void *const p), void *const p);

    int fs_StructList_insert_head(FsStructList * const pStructList, /* 结构的指针 */void* const node);
    int fs_StructList_insert_head_pthreadSafety(FsStructList * const pStructList, /* 结构的指针 */void* const node);
    int fs_StructList_insert_head_array(FsStructList * const pStructList_, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    int fs_StructList_insert_head_array_pthreadSafety(FsStructList * const pStructList, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    int fs_StructList_insert_head_all(FsStructList * const pStructList_, /* 不能为空且节点数可为0 */const FsStructList * const pInsertList_);
    int fs_StructList_insert_head_all_pthreadSafety(FsStructList * const pStructList, /* 不能为空且节点数可为0 */const FsStructList * const pInsertList_);
    void fs_StructList_remove_head(FsStructList * const pStructList);
    signed char fs_StructList_insert_tail(FsStructList * const pStructList, /* 结构的指针 */const void* const node);
    signed char fs_StructList_insert_tail_pthreadSafety(FsStructList * const pStructList, /* 结构的指针 */const void* node);
    int fs_StructList_insert_tail_array(FsStructList * const pStructList_, /* 插入的节点个数 */ unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    int fs_StructList_insert_tail_array_pthreadSafety(FsStructList * const pStructList, /* 插入的节点个数 */ unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    int fs_StructList_insert_tail_all(FsStructList * const pStructList_, /* 不能为空且节点数可为0 */ const FsStructList * const pInsertList_);
    int fs_StructList_insert_tail_all_pthreadSafety(FsStructList * const pStructList, /* 不能为空且节点数可为0 */ const FsStructList * const pInsertList_);
    void fs_StructList_remove_tail(FsStructList * const pStructList);
    void fs_StructList_remove_tail_n(FsStructList * const pStructList, /* 移除尾部的节点数 */const unsigned long n);
    signed char fs_StructList_insert_at(FsStructList * const pStructList, unsigned long index, const void* const node);

    void fs_StructList_remove_at(FsStructList* pStructList, unsigned long i);
    long fs_StructList_insert_order(FsStructList * const pStructList, const void* const node);
    long fs_StructList_insert_order_pthreadSafety(FsStructList * const pStructList, const void* const node);
    long fs_StructList_insert_order_void(FsStructList * const pStructList, const void* const node);
    long fs_StructList_insert_order_double(FsStructList * const pStructList, const void* const node);
    long fs_StructList_insert_order_int(FsStructList * const pStructList, const void* const node, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    long fs_StructList_insert_order_custom(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    long fs_StructList_insert_order_custom_pthreadSafety(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    long fs_StructList_insert_order_custom_update(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    long fs_StructList_insert_order_custom_update_pthreadSafety(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    long fs_StructList_insert_custom(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数 */int (*const compare) (void *node1, void* node2));
    long fs_StructList_insert_custom_pthreadSafety(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数 */int (*const compare) (void *node1, void* node2));
    long fs_StructList_check_exist(const FsStructList * const pStructList, const void* const node);
    long fs_StructList_check_exist_order(FsStructList * const pStructList, const void* const node);
    long fs_StructList_check_exist_order_void(FsStructList * const pStructList, /* 值为关键的指针数据,非节点 */const void* const voidData);
    long fs_StructList_check_exist_order_int(FsStructList * const pStructList, const unsigned int key, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    long fs_StructList_check_exist_order_custom(FsStructList * const pStructList, /* 节点开始的数据内容,非节点 */const void* const data, /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    long fs_StructList_check_exist_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */int (*compare)(void *node1, void* node2));
    long fs_StructList_remove_order(FsStructList * const pStructList, const void* const node);
    long fs_StructList_remove_order_pthreadSafety(FsStructList * const pStructList, const void* const node);
    long fs_StructList_remove_order_void(FsStructList * const pStructList, /* 值为关键的指针数据,非节点 */const void* const voidData);
    long fs_StructList_remove_order_custom(FsStructList * const pStructList, /* 节点开始的数据内容,非节点 */const void* const data, /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    long fs_StructList_remove_order_custom_pthreadSafety(FsStructList * const pStructList, /* 节点开始的数据内容,非节点 */const void* const data, /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    long fs_StructList_remove_custom(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */int (*const compare) (void *node1, void* node2));
    long fs_StructList_remove_custom_pthreadSafety(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数 */int (*const compare) (void *node1, void* node2));
    long fs_StructList_update_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */int (*const compare) (void *node1, void* node2));
#ifdef FsDebug
    void fs_StructList_test();
#endif
#ifdef __cplusplus
}
#endif

#endif 
