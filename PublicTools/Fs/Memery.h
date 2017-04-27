/* 
 * File:   Memery.h
 * Author: fslib
 *
 * Created on 2013年8月5日, 上午11:34
 */

#ifndef MEMERY_H
#define	MEMERY_H
#include "../publicDefine.h"
#include "StructList.h"
#ifdef	__cplusplus
extern "C" {
#endif
#define FsMemery_disable
    /* 一个管理多个内存的对象,支持批量删除内存,删除顺序是倒序删除的 */
    typedef struct {
        FsStructList list;
    } FsMemeryFree;
    /* 
     * 请求分配不小于requestSize大小的内存,至少多分配sizeof(unsigned long)大小的空间;
     * requestSize:请求分配内存的大小,不能为0;
     * 返回分配的内存,可以用free释放空间;
     * 只有在可能多次申请相同或相近大小内存时,使用此函数才能提高效率.
     */
    void *fs_memery_malloc(unsigned long requestSize);
    /* 
     * 重新分配不小于requestSize大小的内存,至少多分配sizeof(unsigned long)大小的空间;
     * requestSize:请求分配内存的大小,不能为0;
     * 返回分配的内存,可以用free释放空间;
     * 只有在可能多次申请相同或相近大小内存时,使用此函数才能提高效率.
     */
    void *fs_memery_realloc(/* 不能为空 */void *p, /* 原来申请的大小 */const unsigned long sizeOld, /* 现在申请的大小 */const unsigned long requestSize);
        /* 
     * 重新分配不小于requestSize大小的内存,不保存原有数据,至少多分配sizeof(unsigned long)大小的空间;
     * requestSize:请求分配内存的大小,不能为0;
     * 返回分配的内存,可以用free释放空间;
     * 只有在可能多次申请相同或相近大小内存时,使用此函数才能提高效率.
     */
    void *fs_memery_realloc_nodata(/* 不能为空 */void *p, /* 原来申请的大小 */const unsigned long sizeOld, /* 现在申请的大小 */const unsigned long requestSize);
    /* 
     * 只能释放用fs_memery_free申请的内存;
     * p:用fs_memery_free申请内存指针;
     * size:用fs_memery_free申请时请求的内存大小.
     */
    void fs_memery_free(void *p, unsigned long size);

#ifdef FsDebug
    void fs_memery_test();
#endif   
    /*
     * 创建一个FsMemeryFree实例;
     * 返回创建的实例指针.
     */
    FsMemeryFree* fs_memeryFree_new__IO();
    /* 删除pMemeryFree指向的实例对象 */
    void fs_memeryFree_delete__OI(FsMemeryFree *pMemeryFree);
    void fs_memeryFree_clear(FsMemeryFree* pMemeryFree);
    /* 添加一个对象的删除 */
    void fs_memeryFree_add_object(FsMemeryFree *pMemeryFree, /* 释放内存的函数指针 */void (*pDelete)(void *p), /* 对象指针 */ void *pData);
    /* 添加一个线程对象的删除 */
    void fs_memeryFree_add_thread(FsMemeryFree *pMemeryFree, /* 停止线程的函数指针 */void (*pStopThread)(void *p), /* 释放内存的函数指针 */void (*pDelete)(void *p), /* 对象指针 */ void *pData);
#ifdef	__cplusplus
}
#endif

#endif	/* MEMERY_H */

