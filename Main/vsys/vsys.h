/* 
 * File:   vsys.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined VSYS_H && !defined JAVA_IN_C

#define VSYS_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Image/ImageFrame.h"
#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(push,2)
#define Vsys_sdkPlayBack_is_process() (access("/usr/lib/systemd/system/sdkPlayBack.service", F_OK) == 0 || access("/fs/bin/sdkPlayBack", F_OK) == 0 || access("./sdkPlayBack", F_OK) == 0) // 检查sdkPlayBack是否以独立进程方式运行

    /* vsys系统 */
    struct Vsys {

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
            /* 线程名 */
#define Vsys_get_threadName(___pVsys) (((char*)((___pVsys)+1))+(___pVsys)->ro._threadName_)
            unsigned char _threadName_;
            /* 线程倍频因子,最小值为1 */
            unsigned int _threadMulti;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 监听分类服务器对象,可为空 */
            void *_pBindClassifyServer;
            /* 回放对象,为空表示内部不创建回放对象 */
            void * _pSdkPlayBack;
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
            /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级,此值会影响到子线程,再开始线程时会设置此值为用本线程的优先级 */
            signed char thisNice;
        } rw;

        struct {
            /* 线程的tid号 */
            pthread_t stid;
#define Vsys_stat_out_enable
#ifdef Vsys_stat_out_enable
            /* ppObjectStats线程同步锁 */
            pthread_mutex_t mutex_ppObjectStats;
            /* 解码跟踪对象,使用mutex_ppObjectStats的互斥锁 */
            FsObjectStats **ppObjectStats;
            /* 解码跟踪对象的个数,使用mutex_ppObjectStats的互斥锁 */
            unsigned int statsCount;
#endif
            /* 通道状态改变的编号 */
            unsigned long long channelStatusIndex;
            /* 获取通道状态的客户端链表 */
            FsStructList *channelStatusDataClientList;
            /* 通道状态数据,为空应返回错误 */
            FsEbml *channelStatusData;
            /* 通道状态数据ebml数据 */
            FsObjectBasePthreadSafety *channelStatusData_ebml;
            /* 通道状态数据xml数据 */
            FsObjectBasePthreadSafety *channelStatusData_xml;
            /* 通道状态数据json数据 */
            FsObjectBasePthreadSafety *channelStatusData_json;
            /* 通道状态改变的编号(集群) */
            unsigned long long channelStatusIndex_all;
            /* 获取通道状态的客户端链表(集群) */
            FsStructList *channelStatusDataClientList_all;
            /* 缓存集群的连接,使用6个int储存一个连接,前3个int为连接标识符,第4个int为集群节点标识符,第5-6个储存收到的ebml转的FsString指针,使用channelStatusDataClientList_all的互斥锁 */
            FsStructList *channelStatusDataclusterClientList_;
            /* 通道状态数据ebml数据(集群) */
            FsObjectBasePthreadSafety *channelStatusData_ebml_all;
            /* 通道状态数据xml数据(集群) */
            FsObjectBasePthreadSafety *channelStatusData_xml_all;
            /* 通道状态数据json数据(集群) */
            FsObjectBasePthreadSafety *channelStatusData_json_all;
            /* 使用struct ConfigManager_connectNode_useOnce结构缓存一些请求,用于报告一些线程参数,使用channelStatusDataClientList的线程锁 */
            void * pConnectNode__channelStatusDataClientList;
        } p;
    };
#define __VsysLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,1,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  threadMulti=%u,pConfigManager=%p,pBindClassifyServer=%p,pSdkPlayBack=%p\n",##__VA_ARGS__\
        ,pVsys,((struct Vsys*)(pVsys))->rw.runStatus,((struct Vsys*)(pVsys))->p.stid,((struct Vsys*)(pVsys))->ro._pMonitor\
        ,((struct Vsys*)(pVsys))->ro._externRunStatus,((struct Vsys*)(pVsys))->ro._parentTid,Vsys_get_threadName(pVsys)\
        ,((struct Vsys*)(pVsys))->ro._threadMulti,((struct Vsys*)(pVsys))->ro._pConfigManager,((struct Vsys*)(pVsys))->ro._pBindClassifyServer\
        ,((struct Vsys*)(pVsys))->ro._pSdkPlayBack)
#define VsysLog(___loglevel,___format, ...) __VsysLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define VsysLog2(___loglevel,___format, ...) __VsysLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
#pragma pack(pop)

    /* 
     * 创建一个新的Vsys对象;
     * 返回Vsys指针.
     */

    struct Vsys *vsys_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 线程倍频因子,最小值为1 */ const unsigned int threadMulti
            , /* 配置管理对象 */ void *const pConfigManager, /* 监听分类服务器对象,可为空 */ void *const pBindClassifyServer
            , /* 回放对象,为空表示内部不创建回放对象 */ void * const pSdkPlayBack);

    /* 删除pVsys指向的实例对象 */

    void vsys_delete__OI(struct Vsys * const pVsys);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void vsys_startThread(struct Vsys * const pVsys
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void vsys_stopThread_noWait(struct Vsys * const pVsys);

    /* 结束线程 */

    void vsys_stopThread(struct Vsys * const pVsys);



#ifdef __cplusplus
}
#endif

#endif /* VSYS_H */

