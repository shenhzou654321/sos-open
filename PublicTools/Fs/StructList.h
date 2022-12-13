/* 
 * File:   StructList.h
 * Author: fslib
 *
 * Created on 2015年4月24日, 下午4:07
 */
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
    /*
     * 创建一个FsStructList实例;
     * 返回创建的实例指针.
     */
    FsStructList* fs_StructList_new_with_externData__IO(/* 最小值建议不要小于2,当列表头或尾没有剩余空间时,
     *     增加2^modifyNodeBits个节点;
     *     当列表尾的空节点数大于2^modifyNodeBits时,
     *     删除2^modifyNodeBits个节点;
     *     modifyNodeCount=2^modifyNodeBits-1;
     */const unsigned char modifyNodeBits, /* 要储存的结构大小,不能为0 */const unsigned long structSize, /* 附加数据的长度 */const unsigned int externDataLenth);
    /* 删除pStructList指向的实例对象 */
    void fs_StructList_delete__OI(FsStructList * const pStructList);
    /*
     * 删除pStructList指向的实例对象;
     * 根据pStructList->nodeCount用deleteNode释放pStructList->pNode[i]指向的空间;
     * 如果pStructList->nodeCount为0，则不释放pStructList->pNode[i]指向的空间.
     */
    void fs_StructList_delete_custom__OI(FsStructList * const pStructList, void (*const deleteNode) (const char *const pNode));
    /* 清空pStructList中的节点,不会释放各节点的空间 */
    void fs_StructList_clear(FsStructList * const pStructList);
    /* 清空pStructList中的节点,不会释放各节点的空间 */
    void fs_StructList_clear_pthreadSafety(FsStructList * const pStructList);
    /* 遍历成员 */
    void fs_StructList_traverse(FsStructList * const pStructList, void (*const traverse) (char *const pNode));
    /* 遍历成员,支持传入两个参数 */
    void fs_StructList_traverse2(FsStructList * const pStructList, void (*const traverse) (char *const pNode, void *const p), void *const p);

    /*
     * 把node的数据拷贝插到pStructList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_StructList_insert_head(FsStructList * const pStructList, /* 结构的指针 */void* const node);
    /*
     * 把node的数据线程安全地拷贝插到pStructList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_StructList_insert_head_pthreadSafety(FsStructList * const pStructList, /* 结构的指针 */void* const node);
    /*
     * 一次性插入多个节点到pStructList_的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_StructList_insert_head_array(FsStructList * const pStructList_, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    /*
     * 一次性线程安全地插入多个节点到pStructList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_StructList_insert_head_array_pthreadSafety(FsStructList * const pStructList, /* 插入的节点个数 */ const unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    /*
     * 把pInsertList_中的节点插到pStructList_的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_StructList_insert_head_all(FsStructList * const pStructList_, /* 不能为空且节点数可为0 */const FsStructList * const pInsertList_);
    /*
     * 把pInsertList_中的节点线程安全地插到pStructList的头部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2;
     * 如果失败返回-1.
     */
    int fs_StructList_insert_head_all_pthreadSafety(FsStructList * const pStructList, /* 不能为空且节点数可为0 */const FsStructList * const pInsertList_);
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
     * 一次性插入多个节点到pStructList_的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_StructList_insert_tail_array(FsStructList * const pStructList_, /* 插入的节点个数 */ unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    /*
     * 一次性线程安全地插入多个节点到pStructList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_StructList_insert_tail_array_pthreadSafety(FsStructList * const pStructList, /* 插入的节点个数 */ unsigned long nodeCount, /* 节点数组指针 */const char * const ppNode);
    /*
     * 把pInsertList_中的所有节点全部插到pStructList_的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_StructList_insert_tail_all(FsStructList * const pStructList_, /* 不能为空且节点数可为0 */ const FsStructList * const pInsertList_);
    /*
     * 把pInsertList_中的所有节点全部线程安全地插到pStructList的尾部;
     * 如果成功返回1;
     * 如果列表不够长进行了空间扩展，返回2.
     */
    int fs_StructList_insert_tail_all_pthreadSafety(FsStructList * const pStructList, /* 不能为空且节点数可为0 */ const FsStructList * const pInsertList_);
    /*
     * 移除pStructList中的尾节点;
     * 如果pStructList的长度为0,程序会出错;
     * 调用此函数前务必对pStructList的长度进行检查.
     */
    void fs_StructList_remove_tail(FsStructList * const pStructList);
    /*
     * 移除pStructList中的尾部的n个节点;
     * 如果pStructList的长度小于n,程序会出错;
     * 调用此函数前务必对pStructList的长度进行检查.
     */
    void fs_StructList_remove_tail_n(FsStructList * const pStructList, /* 移除尾部的节点数 */const unsigned long n);
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
     * 把node的数据按double的排序方式插到pStructList中,按升序排序,插入对象的大小必须是double大小的倍数;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_double(FsStructList * const pStructList, const void* const node);
    /*
     * 把node的数据按unsigned int的排序方式插到pStructList中,按升序排序,插入对象的大小必须是unsigned int大小的倍数;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_int(FsStructList * const pStructList, const void* const node, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    /*
     * 把node的数据按自定义的比较长度排序插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_custom(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    /*
     * 把node的数据线程安全地按自定义的比较长度排序插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_order_custom_pthreadSafety(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    /*
     * 把node的数据按自定义的比较长度排序插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1,同时更新原有的数据.
     */
    long fs_StructList_insert_order_custom_update(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
        /*
     * 把node的数据线程安全地按自定义的比较长度排序插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1,同时更新原有的数据.
     */
    long fs_StructList_insert_order_custom_update_pthreadSafety(FsStructList * const pStructList, const void* const node, /* 比较的数据长度,应小于节点大小 */const unsigned int keyLen);
    /*
     * 把node的数据按自定义的方式插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_custom(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数 */int (*const compare) (void *node1, void* node2));
    /*
     * 把node的数据线程安全地按自定义的方式插到pStructList中,按升序排序;
     * 如果成功返回插入的位置,从0开始;
     * 有相同的值返回-1.
     */
    long fs_StructList_insert_custom_pthreadSafety(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数 */int (*const compare) (void *node1, void* node2));
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
     * 失败返回插入位置的相反数-1.
     */
    long fs_StructList_check_exist_order(FsStructList * const pStructList, const void* const node);
    /* 
     * 在pStructList中按指针数据大小的方式查找是否包含值为voidData,对象的大小必须是指针大小的倍数;
     * 只有全部使用fs_StructList_*_order_void添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_StructList_check_exist_order_void(FsStructList * const pStructList, /* 值为关键的指针数据,非节点 */const void* const voidData);
    /* 
     * 在pStructList中按unsigned int数据大小的方式查找是否包含unsigned int 值,对象的大小必须是unsigned int大小的倍数;
     * 只有全部使用fs_StructList_*_order_int添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_StructList_check_exist_order_int(FsStructList * const pStructList, const unsigned int key, /* 在节点中获取比较用的关键字距离节点指针偏移的字节数 */const unsigned int offset);
    /* 
     * 在pStructList中按自定义数据大小的方式查找是否包含值为data;
     * 只有全部使用fs_StructList_*_order_custom添加节点的pStructList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_StructList_check_exist_order_custom(FsStructList * const pStructList, /* 节点开始的数据内容,非节点 */const void* const data, /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    /* 
     * 在pStructList中按自定义的方式查找是否包含node;
     * 只有全部使用fs_StructList_*_custom添加节点的pObjectList才能使用此函数;
     * 成功返回index,从0开始;
     * 失败返回插入位置的相反数-1.
     */
    long fs_StructList_check_exist_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */int (*compare)(void *node1, void* node2));
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
     * 移除pStructList中的关键数据为voidData的节点,对象的大小必须是指针大小的倍数;
     * 只有全部使用fs_StructList_*_order_void添加节点的pStructList才能使用此函数;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_order_void(FsStructList * const pStructList, /* 值为关键的指针数据,非节点 */const void* const voidData);
    /*
     * 移除pStructList中的关键数据为data的节点;
     * 只有全部使用fs_StructList_*_order_custom添加节点的pStructList才能使用此函数;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_order_custom(FsStructList * const pStructList, /* 节点开始的数据内容,非节点 */const void* const data, /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    /*
     * 线程安全地移除pStructList中的关键数据为data的节点;
     * 只有全部使用fs_StructList_*_order_custom添加节点的pStructList才能使用此函数;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_order_custom_pthreadSafety(FsStructList * const pStructList, /* 节点开始的数据内容,非节点 */const void* const data, /* 比较的数据长度,应小于节点大小 */const unsigned int datalen);
    /*
     * 移除pStructList中的数据为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_custom(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */int (*const compare) (void *node1, void* node2));
    /*
     * 线程安全地移除pStructList中的数据为node的节点;
     * 成功返回移除的节点位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_remove_custom_pthreadSafety(FsStructList * const pStructList, void* const node,
            /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数 */int (*const compare) (void *node1, void* node2));
    /*
     * 更新pStructList中的数据为node的节点的值;
     * 成功返回节点的位置,从0开始;
     * 失败返回-1.
     */
    long fs_StructList_update_custom(FsStructList* pStructList, void* node, /* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */int (*const compare) (void *node1, void* node2));
#ifdef FsDebug
    void fs_StructList_test();
#endif
#ifdef __cplusplus
}
#endif

#endif /* STRUCTLIST_H */

