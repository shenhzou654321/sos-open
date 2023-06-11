/* 
 * File:   sdkPlayBack.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined SDKPLAYBACK_H && !defined JAVA_IN_C

#define SDKPLAYBACK_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Ebml.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../Modules/sdkPlayBack/sdkPlayBackTask.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* 使用sdk回放的管理对象 */
    typedef struct {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 管道,用于添加和删除通道或发送数据 */
            int __pipe[2];
            /* 线程名 */
#define SdkPlayBack_get_threadName(___pSdkPlayBack) (((char*)((___pSdkPlayBack)+1))+(___pSdkPlayBack)->ro._threadName_)
            unsigned char _threadName_;
            /* 管理的sdk,按配置文件名及指针排序 */
            FsObjectList *__itemListClientOrder_;
        } ro;

        struct {
            /* 运行状态:
             *     与_externRunStatus指向值相同-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             * 引用的指针类型必须是(char*);
             * 引用此值的其他线程应实现当自己的运行状态值[0-255]与此值[-128-127]不相等时退出.
             */
            unsigned char runStatus;
            //            /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级,此值会影响到子线程,再开始线程时会设置此值为用本线程的优先级 */
            //            signed char thisNice;
            /* 外部和内部线程异步通信的链表,结构为SdkPlayBack_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    } SdkPlayBack;
#define __SdkPlayBackLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  itemListClient=%p/%lu\n",##__VA_ARGS__\
        ,pSdkPlayBack,((SdkPlayBack*)(pSdkPlayBack))->rw.runStatus,((SdkPlayBack*)(pSdkPlayBack))->p.inRun,((SdkPlayBack*)(pSdkPlayBack))->p.stid,((SdkPlayBack*)(pSdkPlayBack))->ro._pMonitor\
        ,((SdkPlayBack*)(pSdkPlayBack))->ro._externRunStatus,((SdkPlayBack*)(pSdkPlayBack))->ro._parentTid,((SdkPlayBack*)(pSdkPlayBack))->ro.__pipe[0], ((SdkPlayBack*)(pSdkPlayBack))->ro.__pipe[1],SdkPlayBack_get_threadName(pSdkPlayBack)\
        ,((SdkPlayBack*)(pSdkPlayBack))->ro.__itemListClientOrder_, ((SdkPlayBack*)(pSdkPlayBack))->ro.__itemListClientOrder_->nodeCount)
#define SdkPlayBackLog(___loglevel,___format, ...) __SdkPlayBackLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define SdkPlayBackLog2(___loglevel,___format, ...) __SdkPlayBackLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* SdkPlayBack项,对应某类具体的sdk */
    struct SdkPlayBack_item {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 某项具体的sdk */
            void *__pSdk;
            /* 更新pSdk参数的方法,可为空 */
            void (*_sdk_update_parameter)(void *const pSdk, /* 存放参数的ebml文件 */FsEbml * const pEbml, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 删除pSdk的方法 */
            void (*_sdk_delete__OI)(void *const pSdk);
            /* 开始pSdk线程的方法 */
            void (*_sdk_startThread)(void *const pSdk
                    , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
            /* 停止pSdk线程的方法,只发送信号,不回收线程资源 */
            void (*_sdk_stopThread_noWait)(void *const pSdk);
            /* 停止pSdk线程的方法 */
            void (*_sdk_stopThread)(void *const pSdk);
            /* 创建一个回放item的方法 */
            void *(*_sdk_PlayBack_item_new_byUrl__IO)(/* 数据目录,如/vsys/aa/video/ */ const char dirname[], const char url[]);
            /* 删除创建回放item的方法 */
            void (*_sdk_PlayBack_item_delete__OI)(void *const pPlayBack_item);
            /* 把回放item添加到pSdk的方法 */
            void (*_sdk_PlayBack_add_item__OI_2)(void *const pSdk, void *const pPlayBack_item);
            /* 从pSdk移除回放item的方法 */
            void (*_sdk_PlayBack_remove_item_noWait__IO_2)(void *const pSdk, void *const pPlayBack_item);
            /* 创建一个回放任务,内部应检查dirName是否重复,相同的dirName应视为相同的回放任务,更新参数,必须在pPlayBack_item加入到pSdk后才能调用 */
            void (*_sdk_PlayBack_item_updatePlayBackTask_pthreadSafety)(void * const pSdk, void *const pPlayBack_item, /* 回放task */FsObjectSdkPlayBackTask * const pTask);
            /* 删除一个回放任务,内部会先检查pTask是否在内部,在内部会删除,必须在pPlayBack_item加入到pSdk后才能调用 */
            void (*_sdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2)(void * const pSdk, void *const pPlayBack_item, /* 回放task */FsObjectSdkPlayBackTask * const pTask);
        } ro;

        struct {
        } rw;

        struct {
            /* 所属的pSdkPlayBack对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            SdkPlayBack *pSdkPlayBack;
            /* 配置文件标记,不为空以_结尾,如文件为a,则配置文件为a_0.cfg和a_1.cfg,应检测到a后更新配置文件 */
            char *_configFileTag;
            /* 按监控目录排序的通道,存放SdkPlayBack_P_itemChannel指针 */
            FsObjectList *__itemListOrder_;
            /* 按配置中描述的顺序通道,存放SdkPlayBack_P_itemChannel指针,存在某项为空(如有两项的参数相同) */
            FsObjectList *__itemList_;
            /* 等待删除的通道,存放SdkPlayBack_P_itemChannel指针 */
            FsObjectList *__itemListWaitDelete_;
            /* 配置文件对应的目录的监控对象 */
            void *pDirMonitor;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            unsigned char : 5;
            /* pSdk的运行状态,0-未运行,1-运行中,2-停止中(未回收线程资源) */
            unsigned char pSdkRunStatus : 2;
        } p;
    };
#define __SdkPlayBack_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,server(%hhu/%hhu),pSdk=%p/%hhu,sdk_update_parameter=%p,sdk_delete__OI=%p,sdk_startThread=%p,sdk_stopThread=%p,sdk_PlayBack_item_new_byUrl__IO=%p,sdk_PlayBack_item_delete__OI=%p\
sdk_PlayBack_add_item__OI_2=%p,sdk_PlayBack_remove_item_noWait__IO_2=%p,configFileTag:\"%s\",itemListOrder_=%p,itemListWaitDelete_=%p,itemList_=%p,pDirMonitor=%p.\n",##__VA_ARGS__\
        ,pSdkPlayBack_item,pSdkPlayBack_item->p.pSdkPlayBack,pSdkPlayBack_item->p.busy,pSdkPlayBack_item->p._controlStatus\
        ,pSdkPlayBack_item->ro.__pSdk,pSdkPlayBack_item->p.pSdkRunStatus,pSdkPlayBack_item->ro._sdk_update_parameter,pSdkPlayBack_item->ro._sdk_delete__OI\
        ,pSdkPlayBack_item->ro._sdk_startThread,pSdkPlayBack_item->ro._sdk_stopThread\
        ,pSdkPlayBack_item->ro._sdk_PlayBack_item_new_byUrl__IO,pSdkPlayBack_item->ro._sdk_PlayBack_item_delete__OI\
        ,pSdkPlayBack_item->ro._sdk_PlayBack_add_item__OI_2,pSdkPlayBack_item->ro._sdk_PlayBack_remove_item_noWait__IO_2\
        ,pSdkPlayBack_item->p._configFileTag,pSdkPlayBack_item->p.__itemListOrder_\
        ,pSdkPlayBack_item->p.__itemList_,pSdkPlayBack_item->p.__itemListWaitDelete_,pSdkPlayBack_item->p.pDirMonitor)
#define SdkPlayBack_itemLog(___loglevel,___format, ...) __SdkPlayBack_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define SdkPlayBack_itemLog2(___loglevel,___format, ...) __SdkPlayBack_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)


    /* 
     * 创建一个新的SdkPlayBack对象;
     * 返回SdkPlayBack指针.
     */

    SdkPlayBack *sdkPlayBack_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]);

    /* 删除pSdkPlayBack指向的实例对象 */

    void sdkPlayBack_delete__OI(SdkPlayBack * const pSdkPlayBack);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void sdkPlayBack_startThread(SdkPlayBack * const pSdkPlayBack
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void sdkPlayBack_stopThread_noWait(SdkPlayBack * const pSdkPlayBack);

    /* 结束线程 */

    void sdkPlayBack_stopThread(SdkPlayBack * const pSdkPlayBack);

    /* 
     * 创建一个新的SdkPlayBack_item对象;
     * 返回SdkPlayBack_item指针.
     */

    struct SdkPlayBack_item * sdkPlayBack_item_new__IO__OI_2(/* 配置文件标记,不为空以_结尾,如文件为a,则配置文件为a_0.cfg和a_1.cfg,应检测到a后更新配置文件 */ const char configFileTag[]
            , /* 某项具体的sdk */ void *const pSdk
            , /* 更新pSdk参数的方法,可为空 */ void (*const sdk_update_parameter) (void *const pSdk, /* 存放参数的ebml文件 */FsEbml * const pEbml, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 删除pSdk的方法 */ void (*const sdk_delete__OI) (void *const pSdk)
            , /* 开始pSdk线程的方法 */ void (*const sdk_startThread) (void *const pSdk
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice)
            , /* 停止pSdk线程的方法,只发送信号,不回收线程资源 */ void (*const sdk_stopThread_noWait) (void *const pSdk)
            , /* 停止pSdk线程的方法 */ void (*const sdk_stopThread) (void *const pSdk)
            , /* 创建一个回放item的方法 */ void *(*const sdk_PlayBack_item_new_byUrl__IO) (/* 数据目录,如/vsys/aa/video/ */ const char dirname[], const char url[])
            , /* 删除创建回放item的方法 */ void (*const sdk_PlayBack_item_delete__OI) (void *const pPlayBack_item)
            , /* 把回放item添加到pSdk的方法 */ void (*const sdk_PlayBack_add_item__OI_2) (void *const pSdk, void *const pPlayBack_item)
            , /* 从pSdk移除回放item的方法 */ void (*const sdk_PlayBack_remove_item_noWait__IO_2) (void *const pSdk, void *const pPlayBack_item)
            , /* 创建一个回放任务,内部应检查dirName是否重复,相同的dirName应视为相同的回放任务,更新参数,必须在pPlayBack_item加入到pDHSDK后才能调用 */
            void (*const sdk_PlayBack_item_updatePlayBackTask_pthreadSafety) (void * const pSdk, void *const pPlayBack_item, /* 回放task */FsObjectSdkPlayBackTask * const pTask)
            , /* 删除一个回放任务,内部会先检查pTask是否在内部,在内部会删除,必须在pPlayBack_item加入到pDHSDK后才能调用 */
            void (*const sdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2) (void * const pSdk, void *const pPlayBack_item, /* 回放task */FsObjectSdkPlayBackTask * const pTask)
            );

    /* 删除pSdkPlayBack_item指向的实例对象 */

    void sdkPlayBack_item_delete__OI(struct SdkPlayBack_item * const pSdkPlayBack_item);

    /* 向pSdkPlayBack中添加一个pSdkPlayBack_item客户端条目 */

    void sdkPlayBack_add_item__OI_2(SdkPlayBack * const pSdkPlayBack, struct SdkPlayBack_item * const pSdkPlayBack_item);

    /* 从pSdkPlayBack中移除一个pSdkPlayBack_item客户端条目,未等待线程真正移除 */

    void sdkPlayBack_remove_item_noWait__IO_2(SdkPlayBack * const pSdkPlayBack, struct SdkPlayBack_item * const pSdkPlayBack_item);

    /* 从pSdkPlayBack中移除一个pSdkPlayBack_item客户端条目 */

    void sdkPlayBack_remove_item__IO_2(SdkPlayBack * const pSdkPlayBack, struct SdkPlayBack_item * const pSdkPlayBack_item);
  

#ifdef __cplusplus
}
#endif

#endif /* SDKPLAYBACK_H */


