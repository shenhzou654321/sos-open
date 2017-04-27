/* 
 * File:   Object.h
 * Author: fslib
 *
 * Created on 2015年4月24日, 下午4:07
 */
#include "../publicDefine.h"
#include "StructList.h"
#include "ObjectList.h"
#ifndef OBJECT_H
#define	OBJECT_H
#ifdef FsDebug
//#define FsObjectDebug
#endif
#ifdef	__cplusplus
extern "C" {
#endif
#define FsObject_init_referList fs_ObjectList_new__IO(4)
#define FsObject_init_deleteList fs_StructList_new__IO(4, sizeof (void*) << 1)

#define FsObject_type_unknown 0
#define FsObject_type_h264 1
#define FsObject_type_imageFrame 2
#define FsObject_type_aac 3
#define FsObject_type_imageRAW 4
#define FsObject_type_imageRGB 5

#define FsObject_type_String 0x8001

    /* 
     * 支持多次引用的对象,继承此类的子类不实现删除函数,必需使用本类的删除函数;
     * 本对象的实例被其它对象的实例引用后,除此基类的成员可写外,其它子类的变量只读;
     * 注:此基类内的变量不用于具体业务,只用于数据管理和调试;
     */
    typedef struct {
        /* 
         * 通道号:
         * 对视频,音频,字幕这类数据可能存在多通道处理的任务
         */
        unsigned char channelID : 5;
        /* 数据类型 */
        unsigned short type;
        /* 引用次数,为0时应该删除 */
        unsigned int referCount;
        /* 线程同步锁 */
        pthread_mutex_t mutex;
        /* 本结构分配的内存大小 */
        unsigned long mallocLenth;
#ifdef FsObjectDebug
        /* 引用信息,用于调试 */
        FsStructList *referInfo;
#endif
        /* 引用的其它FsObject对象 */
        FsObjectList *referList;
        /* 当本对象要被删除时,要删除的数据链表,结构为void(*)(void*)的函数指针和(void*)要删除的对象指针 */
        FsStructList *deleteList;
    } FsObject;
    /* 初始化FsObject或它的继承对象,只能调用一次,不对referList,deleteList进行初始化 */
    void fs_Object_init(void* pObject, /* 数据类型,用FsObject_type开头的宏 */const unsigned short type, /* 本结构分配的内存大小 */const unsigned long mallocLenth);
    /* 删除FsObject或它的继承对象,会把referCount减1,仅当referCount为0时删除 */
    void fs_Object_delete__OI(void* pObject);
    /* 删除FsObject或它的继承对象,会把referCount减1,仅当referCount为0时删除 */
    void fs_Object_delete_pthreadSafety__OI(void* pObject);
    /* pObject引用+1 */
#ifdef FsObjectDebug
    void fs_Object_addRefer_pthreadSafety(void* pObject, /* 引用的行数,0表示不执行检查 */const unsigned int line);
#else
#define fs_Object_addRefer_pthreadSafety(pObject,line) fs_Object_addRefer_pthreadSafety_(pObject)
    void fs_Object_addRefer_pthreadSafety_(void* pObject);
#endif
    /* pObject引用+referCount */
#ifdef FsObjectDebug
    void fs_Object_addRefer_custom_pthreadSafety(void* const pObject, /* 引用的次数 */const unsigned int referCount, /* 引用的行数,0表示不执行检查 */const unsigned int line);
#else
#define fs_Object_addRefer_custom_pthreadSafety(pObject,referCount,line) fs_Object_addRefer_custom_pthreadSafety_(pObject,referCount)
    void fs_Object_addRefer_custom_pthreadSafety_(void* const pObject, /* 引用的次数 */const unsigned int referCount);
#endif
#ifdef FsDebug
    void fs_Object_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* OBJECT_H */

