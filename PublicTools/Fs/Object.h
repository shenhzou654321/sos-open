/* 
 * File:   Object.h
 * Author: fslib
 *
 * Created on 2015年4月24日, 下午4:07
 */
#if !defined OBJECT_H && !defined JAVA_IN_C

#define OBJECT_H
#include "../../PublicTools/publicDefine.h"

#include <stdio.h>
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#ifdef FsDebug
//#define FsObjectDebug
#endif
#ifdef __cplusplus
extern "C" {
#endif
#define FsObject_init_referList fs_ObjectList_new__IO(4)
#define FsObject_init_deleteList fs_StructList_new__IO(4, sizeof (void*) << 1)

    enum FsObject_type {
        FsObject_type_unknown = 0x0,
#define FsObject_type_unknown FsObject_type_unknown
        FsObject_type_h264 = 0x1,
#define FsObject_type_h264 FsObject_type_h264
        FsObject_type_h265 = 0x2,
#define FsObject_type_h265 FsObject_type_h265
        FsObject_type_aac = 0x3,
#define FsObject_type_aac FsObject_type_aac
        FsObject_type_imageFrame = 0x4,
#define FsObject_type_imageFrame FsObject_type_imageFrame
        FsObject_type_imageRAW = 0x5,
#define FsObject_type_imageRAW FsObject_type_imageRAW
        FsObject_type_imageRGB = 0x6,
#define FsObject_type_imageRGB FsObject_type_imageRGB
        FsObject_type_imageYCC = 0x7,
#define FsObject_type_imageYCC FsObject_type_imageYCC
        FsObject_type_imageYUV420P = 0x8,
#define FsObject_type_imageYUV420P FsObject_type_imageYUV420P
        FsObject_type_imageYCC3 = 0x9,
#define FsObject_type_imageYCC3 FsObject_type_imageYCC3
        FsObject_type_imageJPEG = 0xA,
#define FsObject_type_imageJPEG FsObject_type_imageJPEG
        FsObject_type_imageGray = 0xB,
#define FsObject_type_imageGray FsObject_type_imageGray     
        FsObject_type_imageEdge = 0xC,
#define FsObject_type_imageEdge FsObject_type_imageEdge    
        FsObject_type_imageHedge = 0x100C,
#define FsObject_type_imageHedge FsObject_type_imageHedge   
        FsObject_type_imageVedge = 0x200C,
#define FsObject_type_imageVedge FsObject_type_imageVedge   
        FsObject_type_stats = 0xD,
#define FsObject_type_stats FsObject_type_stats    
        FsObject_type_String = 0x8001,
#define FsObject_type_String FsObject_type_String
    };
#pragma pack(push,8)

    /* 基本的支持引用计数的数据定义,不能改变顺序,继承只须继承前七个变量,对象会在referCount-_unownedCount=0且_unownedTag为0时真正释放空间 */
    typedef struct {
        /* 
         * 对象销毁方法,在多引用计数时不能修改此指针值,当referCount-_unownedCount=0时
         *     如_unownedTag为0释放所有空间;
         *     否则释放派生类空间,并修改本指针为释放FsObjectBase空间的函数;
         */
        void (*_delete)(void *const p);
        /* 获取pObjectBase的内存大小 */
#define fs_ObjectBase_mallocLenth_get(pObjectBase) (((FsObjectBase*)(pObjectBase))->_mallocLenth>0?((FsObjectBase*)(pObjectBase))->_mallocLenth:\
    (((FsObjectBase*)(((char*)(pObjectBase))+((FsObjectBase*)(pObjectBase))->_mallocLenth))->_mallocLenth+((FsObjectBase*)(pObjectBase))->_mallocLenth))
        /* 本结构分配的内存大小,外部不可修改,负数表示本对象的真实空间属于同类型其他对象,本对象对其空间有引用,值表示同真实对象的偏移量 */
        long _mallocLenth;
        /* 引用计数的参考值,不大于0,只减小不增大 */
        volatile int _unownedCount;
        /* 引用次数,真实的引用计数应为referCount-_unownedCount,引用计数是不包含拥有者标记的 */
        volatile int referCount;
        /* 
         * FsObjectBase类型的拥有者标记,_mallocLenth小于0时此值衡为0,主要用于双线程或多线程与一线程无锁共享数据,
         *   0-表示无拥有者,1-表示拥有者持有中,-1-表示拥有者正在放弃拥有,其他线程应等待稍候,-2-表示强制其他线程退出,拥有者增加_unownedCount再判断是否释放最后的空间;
         */
        volatile signed char _unownedTag;
        /* 在引用减少时的引用标记,0-表示未进行引用减少的动作,-1表示本线程正在做引用减少的动作,无引用时由本线程释放空间 */
        volatile signed char _referTag;
        /* 下一个包在使用时忽略的数据长度,主要用于对已有数据使用不同的头进行封装 */
        unsigned short _nextPackageSkipHeadLen;
        /* 获取pObjectBase的引用计数,引用计数是不包含拥有者标记的 */
#define fs_ObjectBase_referCount_get(pObjectBase) (((FsObjectBase*)(pObjectBase))->referCount-((FsObjectBase*)(pObjectBase))->_unownedCount)
        /* 数据区大小 */
        unsigned int len;
        /* 数据指针 */
        char *data;
    } FsObjectBase;

    /* 基本的支持线程安全的引用计数的数据定义,不能改变顺序,对象继承FsObjectBase,对象会在referCount-_unownedCount=0且_unownedTag为0时真正释放空间,如referCount-_unownedCount=0且_unownedTag不为0,则释放除FsObjectBase定义数据的所有空间,并把_delete改为FsObjectBase类型的删除函数 */
    typedef struct {
        /* 
         * 对象销毁方法,在多引用计数时不能修改此指针值,当referCount-_unownedCount=0时
         *     如_unownedTag为0释放所有空间;
         *     否则释放派生类空间,并修改本指针为释放FsObjectBase空间的函数;
         */
        void (*_delete)(void *const p);
        /* 本结构分配的内存大小,外部不可修改,负数表示本对象的真实空间属于同类型其他对象,本对象对其空间有引用,值表示同真实对象的偏移量 */
        long _mallocLenth;
        /* 引用计数的参考值,不大于0,只减小不增大 */
        volatile int _unownedCount;
        /* 引用次数,真实的引用计数应为referCount-_unownedCount,引用计数是不包含拥有者标记的 */
        volatile int referCount;
        /* 
         * FsObjectBase类型的拥有者标记,_mallocLenth小于0时此值衡为0,主要用于双线程或多线程与一线程无锁共享数据,
         *   0-表示无拥有者,1-表示拥有者持有中,-1-表示拥有者正在放弃拥有,其他线程应等待稍候,-2-表示强制其他线程退出,拥有者增加_unownedCount再判断是否释放最后的空间;
         */
        volatile signed char _unownedTag;
        /* 在引用减少时的引用标记,0-表示未进行引用减少的动作,-1表示本线程正在做引用减少的动作,无引用时由本线程释放空间 */
        volatile signed char _referTag;
        /* 下一个包在使用时忽略的数据长度,主要用于对已有数据使用不同的头进行封装 */
        unsigned short _nextPackageSkipHeadLen;
        /* 数据区大小 */
        unsigned int len;
        /* 数据指针 */
        char *data;
        /* 线程同步锁 */
        pthread_mutex_t _mutex;
    } FsObjectBasePthreadSafety;
#pragma pack(pop)

    /* 继承FsObjectBase,且实现双向链表功能,但objectBase的删除方法并不实现链表移除 */
    struct FsObjectBaseListNode {
        FsObjectBase objectBase;
        /* 上一个节点 */
        struct FsObjectBaseListNode *privious;
        /* 下一个节点 */
        struct FsObjectBaseListNode *next;
    };

    typedef struct {
        /* 主节点 */
        struct FsObjectBaseListNode node;
        /* 节点数量 */
        unsigned long nodeCount;
    } FsObjectBaseList;
    /* 初始化 */
#define Fs_ObjectBaseList_init(__objectBaseList) do{/* 主节点 */(__objectBaseList).node.privious=(__objectBaseList).node.next=&(__objectBaseList).node;/* 节点数量 */(__objectBaseList).nodeCount = 0;}while(0)
    /* 头部插入一节点 */
#define Fs_ObjectBaseList_insert_head(__objectBaseList,pNode) do{(__objectBaseList).nodeCount++;((struct FsObjectBaseListNode*)(pNode))->privious=&(__objectBaseList).node;((struct FsObjectBaseListNode*)(pNode))->next=(__objectBaseList).node.next;(__objectBaseList).node.next->privious=(struct FsObjectBaseListNode*)(pNode);(__objectBaseList).node.next=(struct FsObjectBaseListNode*)(pNode);}while(0)
    /* 尾部插入一节点 */
#define Fs_ObjectBaseList_insert_tail(__objectBaseList,pNode) do{(__objectBaseList).nodeCount++;((struct FsObjectBaseListNode*)(pNode))->privious=(__objectBaseList).node.privious;((struct FsObjectBaseListNode*)(pNode))->next=&(__objectBaseList).node;(__objectBaseList).node.privious->next=(struct FsObjectBaseListNode*)(pNode);(__objectBaseList).node.privious=(struct FsObjectBaseListNode*)(pNode);}while(0)
    /* 移除一节点 */
#define Fs_ObjectBaseList_remove(__objectBaseList,pNode) do{(__objectBaseList).nodeCount--;((struct FsObjectBaseListNode*)(pNode))->next->privious = ((struct FsObjectBaseListNode*)(pNode))->privious;((struct FsObjectBaseListNode*)(pNode))->privious->next = ((struct FsObjectBaseListNode*)(pNode))->next;}while(0)
    /* 移除头节点 */
#define Fs_ObjectBaseList_remove_head(__objectBaseList) (__objectBaseList).node.next;(__objectBaseList).nodeCount--;(__objectBaseList).node.next->next->privious= &(__objectBaseList).node;(__objectBaseList).node.next= (__objectBaseList).node.next->next
    /* 移除尾节点 */
#define Fs_ObjectBaseList_remove_tail(__objectBaseList) (__objectBaseList).node.privious;(__objectBaseList).nodeCount--;(__objectBaseList).privious->privious->next= &(__objectBaseList).node;(__objectBaseList).node.privious= (__objectBaseList).node.privious->privious 
    /* 获取首节点 */
#define Fs_ObjectBaseList_get_head(__objectBaseList) (__objectBaseList).node.next
    /* 获取尾节点 */
#define Fs_ObjectBaseList_get_tail(__objectBaseList) (__objectBaseList).node.privious;
    /* 获取节点数量 */
#define Fs_ObjectBaseList_get_count(__objectBaseList) (__objectBaseList).nodeCount
    /* 获取节点的下一个节点 */
#define Fs_ObjectBaseList_node_get_next(__pNode) ((struct FsObjectBaseListNode*)(__pNode))->next
    /* 获取节点的上一个节点 */
#define Fs_ObjectBaseList_node_get_privious(__pNode) ((struct FsObjectBaseListNode*)(__pNode))->privious

    /* 
     * 支持多次引用的对象,继承此类的子类不实现删除函数,必需使用本类的删除函数,对象继承FsObjectBasePthreadSafety,对象会在referCount-_unownedCount=0且_unownedTag为0时真正释放空间,如referCount-_unownedCount=0且_unownedTag不为0,则释放除FsObjectBase定义数据的所有空间,并把_delete改为FsObjectBase类型的删除函数;
     * 本对象的实例被其它对象的实例引用后,除此基类的成员可写外,其它子类的变量只读;
     * 注:此基类内的变量不用于具体业务,只用于数据管理和调试;
     */
    typedef struct {
        /* 
         * 对象销毁方法,在多引用计数时不能修改此指针值,当referCount-_unownedCount=0时
         *     如_unownedTag为0释放所有空间;
         *     否则释放派生类空间,并修改本指针为释放FsObjectBase空间的函数;
         */
        void (*_delete)(void *const p);
        /* 本结构分配的内存大小,外部不可修改,负数表示本对象的真实空间属于同类型其他对象,本对象对其空间有引用,值表示同真实对象的偏移量 */
        long _mallocLenth;
        /* 引用计数的参考值,不大于0,只减小不增大 */
        volatile int _unownedCount;
        /* 引用次数,真实的引用计数应为referCount-_unownedCount,引用计数是不包含拥有者标记的 */
        volatile int referCount;
        /* 
         * FsObjectBase类型的拥有者标记,_mallocLenth小于0时此值衡为0,主要用于双线程或多线程与一线程无锁共享数据,
         *   0-表示无拥有者,1-表示拥有者持有中,-1-表示拥有者正在放弃拥有,其他线程应等待稍候,-2-表示强制其他线程退出,拥有者增加_unownedCount再判断是否释放最后的空间;
         */
        volatile signed char _unownedTag;
        /* 在引用减少时的引用标记,0-表示未进行引用减少的动作,-1表示本线程正在做引用减少的动作,无引用时由本线程释放空间 */
        volatile signed char _referTag;
        /* 下一个包在使用时忽略的数据长度,主要用于对已有数据使用不同的头进行封装 */
        unsigned short _nextPackageSkipHeadLen;
        /* 数据区大小 */
        unsigned int len;
        /* 数据指针 */
        char *data;
        /* 线程同步锁 */
        pthread_mutex_t mutex;
        /* 
         * 通道号:
         * 对视频,音频,字幕这类数据可能存在多通道处理的任务
         */
        unsigned char channelID : 5;
        /* 数据类型 */
        enum FsObject_type type;
#ifdef FsObjectDebug
        /* 引用信息,用于调试 */
        FsStructList *referInfo;
#endif
        /* 引用的其它FsObject对象 */
        FsObjectList *referList_;
        /* 当本对象要被删除时,要删除的数据链表,结构为void(*)(void*)的函数指针和(void*)要删除的对象指针 */
        FsStructList *deleteList;
    } FsObject;
    /* 释放拥有者标记 */
#define fs_ObjectBase_unowned_delete__OI(pObjectBase) do{\
    if (((FsObjectBase*) (pObjectBase))->_mallocLenth < 0||((FsObjectBase*) (pObjectBase))->_unownedTag != 1) {\
        FsPrintf(5, "Invalid ppObjectBase=%p,referCount=%d,unownedCount=%d,unownedTag=%d,mallocLenth=%lu.\n", pObjectBase, ((FsObjectBase*)(pObjectBase))->referCount,\
                ((FsObjectBase*) (pObjectBase))->_unownedCount, ((FsObjectBase*) (pObjectBase))->_unownedTag, ((FsObjectBase*) (pObjectBase))->_mallocLenth);\
        fflush(stdout);\
    }\
    if (0==fs_ObjectBase_referCount_get(pObjectBase)) fsFree(pObjectBase);\
    else {\
        ((FsObjectBase*) (pObjectBase))->_unownedTag = -1;\
        if (-1 == ((FsObjectBase*) (pObjectBase))->_referTag) {\
            ((FsObjectBase*) (pObjectBase))->_unownedTag = -2;\
            while (((FsObjectBase*) (pObjectBase))->_referTag)FsWaitSyncDo();\
        }\
        if (0==fs_ObjectBase_referCount_get(pObjectBase)) fsFree(pObjectBase);\
        else ((FsObjectBase*) (pObjectBase))->_unownedTag = 0;\
    }\
}while(0)


    /* 
     * 一系列的FsObjectBase缓存对象,一般是FsObjectBase或子类的拥有者使用此结构;
     * 术语:把_mallocLenth大于0的FsObjectBase或子类称为元节点;
     *      把_mallocLenth小于0的FsObjectBase或子类称为非元节点;
     */
    typedef struct {
        /* 最近一次使用的节点,可能是元节点,也可能不是元节点,可为空  */
        FsObjectBase *__lastNode;
        /* 缓存的元节点,只能是元节点,有些节点可能为空 */
        FsObjectBase **__buffer;
        /* 每个元节点期望的内存长度 */
        unsigned int perfectLen;
        /* buffer数量 */
        unsigned int bufferCount;
        /* buffer当前取用的编号 */
        unsigned int bufferIndex;
        /* 遍历一次缓存,没有命中的数量 */
        unsigned int busyCount;
    } FsObjectBaseBuffer;

    /* 单线程结构,只是基类,使用者须自行实现派生类 */
    typedef struct {
        /* 获取pObjectSingle的内存大小 */
#define fs_ObjectSingle_mallocLenth_get(pObjectSingle) (((FsObjectSingle*)(pObjectSingle))->_mallocLenth>0?((FsObjectSingle*)(pObjectSingle))->_mallocLenth:\
    (((FsObjectSingle*)(((char*)(pObjectSingle))+((FsObjectSingle*)(pObjectSingle))->_mallocLenth))->_mallocLenth+((FsObjectSingle*)(pObjectSingle))->_mallocLenth))
        /* 本结构分配的内存大小,外部不可修改,负数表示本对象的真实空间属于同类型其他对象,本对象对其空间有引用,值表示同真实对象的偏移量 */
        int _mallocLenth;
        /* 引用次数,为0时应该删除 */
#define fs_ObjectSingle_referCount_get(pObjectBase) ((FsObjectSingle*)(pObjectBase))->referCount
        unsigned int referCount;
    } FsObjectSingle;
#define FsObjectSingleBuffer_enable

    /* 
     * 一系列的FsObjectSingle缓存对象,FsObjectSingle子类的拥有者使用此结构;
     * 术语:把_mallocLenth大于0的FsObjectSingle或子类称为元节点;
     *      把_mallocLenth小于0的FsObjectSingle或子类称为非元节点;
     */
    typedef struct {
        /* 最近一次使用的节点,可能是元节点,也可能不是元节点,可为空  */
        FsObjectSingle *__lastNode;
        /* 缓存的元节点,只能是元节点,有些节点可能为空 */
        FsObjectSingle **__buffer;
        /* 每个元节点期望的内存长度 */
        unsigned int perfectLen;
        /* buffer数量 */
        unsigned int bufferCount;
        /* buffer当前取用的编号 */
        unsigned int bufferIndex;
        /* 遍历一次缓存,没有命中的数量 */
        unsigned int busyCount;
    } FsObjectSingleBuffer;

    /* 析构FsObjectBase的派生类 */
    void fs_ObjectBase_destruct(void* const pObjectBase, /* FsObjectBase到派生类之间多出的变量的删除方法 */void (*const delete_special_var) (void *pObjectBase));
    /* 初始化FsObjectBase或它的继承对象,在引用计数为1时可反复调用,不对数据长度len进行初始化,一般不使用此函数初始化,使用子类的方法初始化此对象 */
    void fs_ObjectBase_init(void* const pObjectBase, /* 本结构分配的内存大小 */const unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 初始化FsObjectBase或它的继承对象,内存使用malloc分配,在引用计数为1时可反复调用 */
    void fs_ObjectBase_init_with_data__OI_4(void* const pObjectBase, /* 本结构分配的内存大小 */const unsigned long mallocLenth, /* 数据长度 */const unsigned int dataLenth, /* 数据 */ char data[]);
    /* 初始化FsObjectBase或它的继承对象,在引用计数和拥有者和为1时可反复调用,不对数据长度len进行初始化,一般不使用此函数初始化,使用子类的方法初始化此对象 */
    void fs_ObjectBase_reSize(void* * const ppObjectBase, /* 本结构分配的内存大小 */const unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 使用缓存申请一个FsObjectBase对象,不对数据长度len进行初始化 */
    FsObjectBase *fs_ObjectBase_new__IO(/* 原缓存,不为空,在(*pObjectBaseBuffer)为空时,会把当前返回的对象的_unowned置1并设置到(*pObjectBaseBuffer)中,在(*pObjectBaseBuffer)->referCount+_unownedCount为1时返回的数据会使用缓存 */
            FsObjectBase* * const pObjectBaseBuffer, /* 本结构期望分配的内存大小 */const unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 获取pObjectBase的数据长度 */

    unsigned long fs_ObjectBase_get_len(void* const pObjectBase);

    /* 获取pObjectBase的数据长度 */

    char* fs_ObjectBase_get_data(void* const pObjectBase);

    /* 初始化pObjectBaseBuffer对象 */
    void fs_ObjectBaseBuffer_init(FsObjectBaseBuffer * const pObjectBaseBuffer, /* buffer数量,1-255 */ const unsigned char bufferCount);
    /* 释放pObjectBaseBuffer内部空间 */
    void fs_ObjectBaseBuffer_release(FsObjectBaseBuffer * const pObjectBaseBuffer);
    /* 设置buffer数量 */
    void fs_ObjectBaseBuffer_bufferCount_set(FsObjectBaseBuffer * const pObjectBaseBuffer, /* buffer数量,1-255 */ const unsigned int bufferCount);
    /* 设置lastNode */
    void fs_ObjectBaseBuffer_lastNode_set__OI_2(FsObjectBaseBuffer * const pObjectBaseBuffer, /* 设置的节点,可为非元节点 */FsObjectBase * const pObjectBase);
    /* 判断pObjectBase后是否有足够的空间,把pObjectBase后的空间缓存到__lastNode中,调用此函数后pObjectBase不能再次调用此函数 */
    void fs_ObjectBaseBuffer_lastNode_add(FsObjectBaseBuffer * const pObjectBaseBuffer, /* 设置的节点,可为非元节点 */FsObjectBase * const pObjectBase,
            /* pObjectBase真实占用的内存大小 */const unsigned long mallocLenth_pObjectBase);
    /* 从buffer中申请空间 */
    FsObjectBase* fs_ObjectBaseBuffer_get__IO(FsObjectBaseBuffer * const pObjectBaseBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */const unsigned char forceBase,
            /* 本结构期望分配的内存大小 */const unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 对pObjectBase重置大小,不管空间够不够都会重新分配内存 */
    FsObjectBase* fs_ObjectBaseBuffer_resize__IO__OI(FsObjectBase * const pObjectBase, FsObjectBaseBuffer * const pObjectBaseBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */const unsigned char forceBase,
            /* 本结构期望分配的内存大小 */ unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 对pObjectSingle重置大小,仅当空间不够时才重新分配内存 */
#define fs_ObjectBaseBuffer_resize(/* void * */___pObjectBase,/* FsObjectSingleBuffer * const */ ___pObjectBaseBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */ /*const unsigned char*/ ___forceBase\
    ,/* 本结构期望分配的内存大小 *//*const unsigned long */ ___mallocLenth, /* data变量的偏移量 *//* const unsigned int*/ ___dataOffset) do{\
    if (fs_ObjectBase_mallocLenth_get(pObjectBase) < (long)(___mallocLenth))\
        *(FsObjectBase**)&___pObjectBase = fs_ObjectBaseBuffer_resize__IO__OI(___pObjectBase, ___pObjectBaseBuffer, ___forceBase, ___mallocLenth, ___dataOffset);\
}while(0)
    /* 输出pObjectBaseBuffer的信息 */
    void fs_ObjectBaseBuffer_debug_out(FsObjectBaseBuffer * const pObjectBaseBuffer, FILE * const fd);
    /* 初始化FsObjectBasePthreadSafety或它的继承对象,只能调用一次,不对数据长度len进行初始化,一般不使用此函数初始化,使用子类的方法初始化此对象 */
    void fs_ObjectBasePthreadSafety_init(void* const pObjectBasePthreadSafety, /* 本结构分配的内存大小 */const unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 初始化FsObjectBasePthreadSafety或它的继承对象,在引用计数为1时可反复调用,不对数据长度len进行初始化,一般不使用此函数初始化,使用子类的方法初始化此对象 */
    void fs_ObjectBasePthreadSafety_reSize(void* * const ppObjectBasePthreadSafety, /* 本结构分配的内存大小 */const unsigned long mallocLenth, /* data变量的偏移量 */const unsigned int dataOffset);
    /* 对已使用fs_ObjectBase_init初始化的数据使用本函数初始化为FsObjectBasePthreadSafety对象,在引用计数为1时可使用 */
    void fs_ObjectBasePthreadSafety_init_from_objectBase(void* const pObjectBase);
    /* 删除FsObjectBasePthreadSafety或它的继承对象,会把referCount减1,在referCount-_unownedCount=0且_unownedTag为0时真正释放空间 */
    void fs_ObjectBasePthreadSafety_delete__OI(void* const pObjectBasePthreadSafety);
    /* 为FsObjectBasePthreadSafety及子类添加引用 */
    void fs_ObjectBasePthreadSafety_addRefer(void* const pObjectBasePthreadSafety);
    /* 为FsObjectBasePthreadSafety及子类添加引用 */
    void fs_ObjectBasePthreadSafety_addRefer_custom(void* const pObjectBasePthreadSafety, /* 引用的次数 */const unsigned int referCount);
    /* 初始化FsObject或它的继承对象,只能调用一次,不对referList,deleteList进行初始化 */
    void fs_Object_init(void* const pObject, /* 数据类型,用FsObject_type开头的宏 */const enum FsObject_type type, /* 本结构分配的内存大小 */const unsigned long mallocLenth
            , /* data变量的偏移量 */const unsigned int dataOffset, /* data长度 */const unsigned int dataLen);
    /* 对已使用fs_ObjectBase_init初始化的数据使用本函数初始化为FsObject对象,仅在pObjectBase的引用计数为1时可调用 */
    void fs_Object_init_from_objectBase(void* const pObjectBase, /* 数据类型,用FsObject_type开头的宏 */const enum FsObject_type type);
    /* 删除FsObject或它的继承对象,会把referCount减1,在referCount-_unownedCount=0且_unownedTag为0时真正释放空间 */
    void fs_Object_delete__OI(void* const pObject);

    /* 删除FsObject或它的继承对象,会把referCount减1,在referCount-_unownedCount=0且_unownedTag为0时真正释放空间 */

    void fs_Object_delete_pthreadSafety__OI(void* const pObject);


    /* 删除FsObject或它的继承对象,会把referCount减1,仅当referCount为0时删除 */
    void fs_Object_delete_for_gtk__OI(unsigned char* a, void* const pObject);
    /* 删除FsObject或它的继承对象,会把referCount减1,仅当referCount为0时删除 */
    void fs_Object_delete_for_gtk_pthreadSafety__OI(unsigned char* a, void* const pObject);

    /* pObject引用+1 */
#ifdef FsObjectDebug
    void fs_Object_addRefer_pthreadSafety(void* pObject, /* 引用的行数,0表示不执行检查 */const unsigned int line);
#else
#define fs_Object_addRefer_pthreadSafety(pObject,line) fs_Object_addRefer_pthreadSafety_(pObject)
    void fs_Object_addRefer_pthreadSafety_(void* const pObject);
#endif
    /* pObject引用+referCount */
#ifdef FsObjectDebug
    void fs_Object_addRefer_custom_pthreadSafety(void* const pObject, /* 引用的次数 */ unsigned int referCount, /* 引用的行数,0表示不执行检查 */const unsigned int line);
#else
#define fs_Object_addRefer_custom_pthreadSafety(pObject,referCount,line) fs_Object_addRefer_custom_pthreadSafety_(pObject,referCount)
    void fs_Object_addRefer_custom_pthreadSafety_(void* const pObject, /* 引用的次数 */const unsigned int referCount);
#endif
    /* 申请一个FsObjectSingle对象 */
    FsObjectSingle* fs_ObjectSingle_new__IO(/* 本结构期望分配的内存大小,不能小于FsObjectSingle的大小 */const unsigned int mallocLenth);
    /* 调整*ppObjectSingle的内存大小,仅当referCount为1且是元数据时才允许被调整 */
    void fs_ObjectSingle_resize(void** ppObjectSingle, /* 本结构期望分配的内存大小,不能小于FsObjectSingle的大小 */const unsigned int mallocLenth);
    /* 删除pObjectSingle或它的继承对象,会把referCount减1,在referCount为0时真正释放空间 */
    void fs_ObjectSingle_delete__OI(void* pObjectSingle);
    /* 强制pObjectSingle为一个只被引用一次的元数据 */
    FsObjectSingle* fs_ObjectSingle_force_unique__IO__OI(void* const pObjectSingle);
    /* 强制pObjectSingleList中的每一个成员为一个只被引用一次的元数据 */
    void fs_ObjectSingle_force_unique(FsObjectList * const pObjectSingleList_);
    /* 为pObjectSingle及子类添加引用 */
#define fs_ObjectSingle_addRefer(/* void* const */ pObjectSingle) ((FsObjectSingle*) (pObjectSingle))->referCount++
    void* fs_ObjectSingle_addRefer__IO(void* const pObjectSingle);
    /* 为pObjectSingle及子类添加引用 */
#define fs_ObjectSingle_addRefer_custom(/* void* const */ pObjectSingle, /* 引用的次数 *//*const unsigned int */ __referCount) ((FsObjectSingle*) (pObjectSingle))->referCount += __referCount
    /* 初始化pObjectSingleBuffer对象 */
    void fs_ObjectSingleBuffer_init(FsObjectSingleBuffer * const pSingleBuffer, /* buffer数量,1-255 */ const unsigned char bufferCount);
    /* 释放pObjectSingleBuffer内部空间 */
    void fs_ObjectSingleBuffer_release(FsObjectSingleBuffer * const pSingleBuffer);
    /* 设置buffer数量 */
    void fs_ObjectSingleBuffer_bufferCount_set(FsObjectSingleBuffer * const pSingleBuffer, /* buffer数量,1-255 */ const unsigned int bufferCount);
    /* 设置lastNode */
    void fs_ObjectSingleBuffer_lastNode_set__OI_2(FsObjectSingleBuffer * const pSingleBuffer, /* 设置的节点,可为非元节点 */FsObjectSingle * const pObjectSingle);
    /* 判断pObjectSingle后是否有足够的空间,把pObjectSingle后的空间缓存到__lastNode中,调用此函数后pObjectSingle不能再次调用此函数 */
    void fs_ObjectSingleBuffer_lastNode_add(FsObjectSingleBuffer * const pSingleBuffer, /* 设置的节点,可为非元节点 */void * const pObjectSingle,
            /* pObjectSingle真实占用的内存大小 */const unsigned long mallocLenth_pObjectSingle);
    /* 从buffer中申请空间 */
    FsObjectSingle* fs_ObjectSingleBuffer_get__IO(FsObjectSingleBuffer * const pSingleBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */const unsigned char forceBase,
            /* 本结构期望分配的内存大小,不能小于FsObjectSingle的大小 */const unsigned int mallocLenth);
    /* 对pObjectSingle重置大小,不管空间够不够都会重新分配内存 */
    FsObjectSingle* fs_ObjectSingleBuffer_resize__IO__OI(FsObjectSingle * const pObjectSingle, FsObjectSingleBuffer * const pSingleBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */const unsigned char forceBase,
            /* 本结构期望分配的内存大小 */ unsigned int mallocLenth);
    /* 对pObjectSingle重置大小,仅当空间不够时才重新分配内存 */
#define fs_ObjectSingleBuffer_resize(/* void * */pObjectSingle,/* FsObjectSingleBuffer * const */ ___pSingleBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */ /*const unsigned char*/ ___forceBase\
,/* 本结构期望分配的内存大小 *//*const unsigned long */ ___mallocLenth) do{\
    if (fs_ObjectSingle_mallocLenth_get(pObjectSingle) < (int)(___mallocLenth))\
        *(FsObjectSingle**)&pObjectSingle= fs_ObjectSingleBuffer_resize__IO__OI((FsObjectSingle*)pObjectSingle, ___pSingleBuffer, ___forceBase, ___mallocLenth);\
}while(0)
    /* 对pObjectSingle重置大小,仅当空间不够时才重新分配内存 */
#define fs_ObjectSingleBuffer_resize2(/* void * */___pObjectSingle,/* pObjectSingle的指针 */___ppObjectSingle,/* FsObjectSingleBuffer * const */ ___pSingleBuffer, /* 是否强制生成的对象为元数据,0-不是,1-必须是元数据 */ /*const unsigned char*/ ___forceBase\
,/* 本结构期望分配的内存大小 *//*const unsigned long */ ___mallocLenth) do{\
    if (fs_ObjectSingle_mallocLenth_get(pObjectSingle) < (int)(___mallocLenth))\
        *(FsObjectSingle**)&___pObjectSingle=*(FsObjectSingle**)___ppObjectSingle= fs_ObjectSingleBuffer_resize__IO__OI((FsObjectSingle*)___pObjectSingle, ___pSingleBuffer, ___forceBase, ___mallocLenth);\
}while(0)


    void fs_Object_test();

#ifdef __cplusplus
}
#endif

#endif /* OBJECT_H */

