/* 
 * File:   monitor.h
 * Author: fslib
 *
 * Created on 2013年2月19日, 下午1:30
 */
#if !defined TASKMONITOR_H && !defined JAVA_IN_C

#include "../../PublicTools/publicDefine.h"
#define TASKMONITOR_H

#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/String.h"
#ifdef __cplusplus
extern "C" {
#endif
    /* 获取数据时的掩码,不能超过8个字节 */
    /* 所有数据 */
#define MonitorMask_all ((unsigned long)(-1))
    /* 包含注释 */
#define MonitorMask_comment 0x1LU
    /* 包含线程名 */
#define MonitorMask_name (0x1LU<<1)
    /* 包含线程创建时间 */
#define MonitorMask_createTime (0x1LU<<2)
    /* 包含线程对象地址 */
#define MonitorMask_memeryAddress (0x1LU<<3)
    /* 包含线程号 */
#define MonitorMask_tid (0x1LU<<4)
    /* 包含cpu占用时间的绝对时间 */
#define MonitorMask_time (0x1LU<<5)
    /* 包含cpu占用时间的占用率 */
#define MonitorMask_rate (0x1LU<<6)
    /* 包含总的cpu占用时间 */
#define MonitorMask_cpuTime (0x1LU<<7)
    /* 包含utime */
#define MonitorMask_utime (0x1LU<<8)
    /* 包含stime */
#define MonitorMask_stime (0x1LU<<9)
    /* 包含cutime */
#define MonitorMask_cutime (0x1LU<<10)
    /* 包含cstime */
#define MonitorMask_cstime (0x1LU<<11)  
    /* 包含堆栈大小信息 */
#define MonitorMask_stackSize (0x1LU<<12)
    /* 包含线程的优先级 */
#define MonitorMask_priority (0x1LU<<13) 
    /* 包含fd数量 */
#define MonitorMask_fdCount (0x1LU<<14)    
    /* 包含fd详细信息 */
#define MonitorMask_fdDetail (0x1LU<<15)
    /* 包含调用堆栈信息 */
#define MonitorMask_callStack (0x1LU<<16)

    typedef struct _MonitorItem {

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
            /* 被监控的线程对象的地址 */
            const void *_monitoredAddress;
            /* 系统线程号,由systemThreadTid获得 */
            pthread_t _stid;
            /* 线程id,可能与stid不同,主要用于线程堆栈获取 */
            pthread_t _tid;
            /* 被监控的线程对象的线程名 */
#define MonitorItem_get_threadName(pMonitorItem) (((char*)((pMonitorItem)+1))+(pMonitorItem)->ro._threadName_)
            unsigned char _threadName_;
            /* 线程创建时间 */
            double createTime;
            /* 线程用户态占用的CPU时间 */
            unsigned long utime;
            /* 线程核心态占用的CPU时间 */
            unsigned long stime;
            /* 进程所有已死线程用户态占用的CPU时间 */
            unsigned long cutime;
            /* 线程所有已死线程核心态占用的CPU时间 */
            unsigned long cstime;
            /* 线程用户态占用的CPU占用率X100 */
            float utimeRate;
            /* 线程核心态占用的CPU占用率X100 */
            float stimeRate;
            /* 线程所有已死线程用户态占用的CPU占用率X100 */
            float cutimeRate;
            /* 线程所有已死线程核心态占用的CPU占用率X100 */
            float cstimeRate;

        } ro;

        struct {
            /* 不为空表示异常时调用的函数,为空表示不调用,返回错误信息 */
            FsString * (* volatile exceptionFun__IO) (struct _MonitorItem * const pMonitorItem);
            /* 异常时的监控参数 */
            volatile long param[8];
            /* 异常时需要保存现场数据的地址 */
            char* volatile saveBuffer;
            /* 异常时需要保存现场数据的长度,为0表示不保存 */
            volatile int saveBufferLen;
            /* 异常时的监控行 */
            volatile unsigned int line;
            /* 循环是否活着:
             * 0:循环正常(一般不使用此值);
             * 1:循环正常;
             * 2:循环阻塞;
             * -1:不检测;
             * 其他值为内部使用,有特殊含义.
             */
            volatile signed char cycleAlive;
        } rw;

        struct {
            /* 此监控项在监控树中的内存地址,在添加时设置 */
            void *treePositon;


        } p;
    } MonitorItem;

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
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出;
             */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 线程名 */
#define Monitor_get_threadName(___pMonitor) (((char*)(((Monitor*)(___pMonitor))+1))+((Monitor*)(___pMonitor))->ro._threadName_)
            unsigned char _threadName_;
            /* 看门狗监控目录,为空表示不写看门狗,此处记录当前要写的进程文件名 */
            char *_feedDogFile;
            /* 统计周期,单位秒,推荐不要超过10S */
            float _detectCycle;
            /* 定时器周期,越小精度越高,但会占用更多的CPU,单位秒,内部会保证循环执行一次的最短时间不小于此值,推荐不要超过10S */
            float _timerCycle;
            /* pTimerList中正在运行的定时器对应的创建线程号 */
            pthread_t timerRunningTid;
            /* 线程倍频阈值,在线程数超过此值时,线程倍频因子会+1,小于此值1/2时倍频因子会-1,不能小于1 */
            unsigned int _threadMultiThreshold;
            /* 线程倍频需要改变时的回调函数,可为空 */
            void (*_cb_threadMulti) (/* 倍频因子修改标识,0-减小,1-增加 */char add, /* 当前线程数 */unsigned int threadCount, void *p
                    , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 线程倍频需要改变时的回调函数的关联变量,可为空 */
            void * _threadMultiExternP;
            /* 定时器队列,只运行一次,按时间排序 */
            FsObjectList *__pTimerList;
            /* 监控的线程队列 */
            FsObjectList *__pThreadList_;
            /* pThreadList的线程同步锁 */
            pthread_mutex_t __pThreadList_mutex;
            /* 监控树 */
            void *__monitorTree;
            /* 添加的线程项添加的根位置 */
            void* _pRootAddPositon;
            /* 线程总数 */
            unsigned short threadCount;
            /* 进程用户态占用的CPU时间 */
            unsigned long utime;
            /* 进程核心态占用的CPU时间 */
            unsigned long stime;
            /* 进程所有已死线程用户态占用的CPU时间 */
            unsigned long cutime;
            /* 进程所有已死线程核心态占用的CPU时间 */
            unsigned long cstime;
            /* 已注册线程的总堆栈大小 */
            unsigned long stackSize;
            /* 进程用户态占用的CPU占用率X100 */
            float utimeRate;
            /* 进程核心态占用的CPU占用率X100 */
            float stimeRate;
            /* 进程所有已死线程用户态占用的CPU占用率X100 */
            float cutimeRate;
            /* 进程所有已死线程核心态占用的CPU占用率X100 */
            float cstimeRate;
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
        } rw;

        struct {
            /* 定时器空项,可做一个缓存 */
            void *__pTimerItem;
            /* 在线程退出时,调用的停止线程的方法,为空表示不调用,第一个void*为停止方法(方法仅发送信号,不能阻塞,为空表示数组结束),第二个void*为停止的对象(为空表示跳过),此数据应以方法名为NULL结尾,如{{fun,p},{NULL,NULL}} */
            void *(*stop_thread_noWait)[2];
        } p;
    } Monitor;
#define __MonitorLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  feedDogFile:\"%s\"\n  detectCycle=%f\n  timerCycle=%f\n  timerRunningTid=%lu\n  threadMultiThreshold=%u\n  cb_threadMulti=%p\n  threadMultiExternP=%p\n\
  TimerList=%p/%lu\n  pThreadList=%p/%lu\n  monitorTree=%p\n  pRootAddPositon=%p\n\
  threadCount=%hu\n  utime=%lu/%f\n  stime=%lu/%f\n  cutime=%lu/%f\n  cstime=%lu/%f\n  stackSize=%lu\n",##__VA_ARGS__\
        ,pMonitor,((Monitor*)(pMonitor))->rw.runStatus,((Monitor*)(pMonitor))->ro.stid\
        ,((Monitor*)(pMonitor))->ro._pMonitor,((Monitor*)(pMonitor))->ro._externRunStatus,((Monitor*)(pMonitor))->ro._parentTid,Monitor_get_threadName(pMonitor)\
        ,((Monitor*)(pMonitor))->ro._feedDogFile,((Monitor*)(pMonitor))->ro._detectCycle,((Monitor*)(pMonitor))->ro._timerCycle,((Monitor*)(pMonitor))->ro.timerRunningTid\
        ,((Monitor*)(pMonitor))->ro._threadMultiThreshold,((Monitor*)(pMonitor))->ro._cb_threadMulti,((Monitor*)(pMonitor))->ro._threadMultiExternP\
        ,((Monitor*)(pMonitor))->ro.__pTimerList,((Monitor*)(pMonitor))->ro.__pTimerList->nodeCount\
        ,((Monitor*)(pMonitor))->ro.__pThreadList_,((Monitor*)(pMonitor))->ro.__pThreadList_->nodeCount,((Monitor*)(pMonitor))->ro.__monitorTree,((Monitor*)(pMonitor))->ro._pRootAddPositon\
        ,((Monitor*)(pMonitor))->ro.threadCount,((Monitor*)(pMonitor))->ro.utime,((Monitor*)(pMonitor))->ro.utimeRate,((Monitor*)(pMonitor))->ro.stime,((Monitor*)(pMonitor))->ro.stimeRate\
        ,((Monitor*)(pMonitor))->ro.cutime,((Monitor*)(pMonitor))->ro.cutimeRate,((Monitor*)(pMonitor))->ro.cstime,((Monitor*)(pMonitor))->ro.cstimeRate,((Monitor*)(pMonitor))->ro.stackSize)
#define MonitorLog(___loglevel,___format, ...) __MonitorLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define MonitorLog2(___loglevel,___format, ...) __MonitorLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的监控线程项,只能在要监控的线程中调用此函数;
     * 返回MonitorItem指针.
     */

    MonitorItem * monitorItem_new__IO(/* 被监控的线程对象的线程名 */const char threadName[]
            , /* 被监控的线程对象的地址,可为空,只能在要监控的线程中调用此函数 */ const void *const monitorAddress);


    /* 
     * 创建一个新的监控线程项,只能在要监控的线程中调用此函数;
     * 返回MonitorItem指针.
     */

    MonitorItem * monitorItem_new_suffix__IO(/* 被监控的线程对象的线程名 */const char threadName[]
            , /* 后缀,加到threadName后组成新的threadName,可为空 */const char suffix[]
            , /* 被监控的线程对象的地址,可为空,只能在要监控的线程中调用此函数 */ const void *const monitorAddress);

    /* 
     * 创建一个新的监控线程项,只能在要监控的线程中调用此函数;
     * 返回MonitorItem指针.
     */

    MonitorItem * monitorItem_new_extern__IO(/* 被监控的线程对象的线程名 */const char threadName[]
            , /* 后缀,加到threadName后组成新的threadName,可为空 */const char suffix[], /* 序号 */ const unsigned int index
            , /* 被监控的线程对象的地址,可为空,只能在要监控的线程中调用此函数 */ const void *const monitorAddress);

    /* 删除pMonitorItem指向的实例对象 */

    void monitorItem_delete__IO(void* const ppMonitorItem);

    /* 
     * 创建一个新的监控线程对象;
     * 返回Monitor指针.
     */

    Monitor * monitor_new__IO(/* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 看门狗监控目录,为空表示不写看门狗 */const char watchdogDir[]
            , /* 写看门狗时是否加入父进程的pid,加入后在父进程退出时会认为本程序已死,0-不加入,1-加入 */const char watchdogParent
            , /* 统计周期,单位秒,推荐不要超过10S */ const float detectCycle
            , /* 定时器周期,越小精度越高,但会占用更多的CPU,单位秒,内部会保证循环执行一次的最短时间不小于此值,推荐不要超过10S */const float timerCycle
            , /* 线程倍频阈值,在线程数超过此值时,线程倍频因子会+1,小于此值1/2时倍频因子会-1,不能小于1,在cb_threadMulti不为空时有效 */ const unsigned int threadMultiThreshold
            , /* 线程倍频需要改变时的回调函数,可为空 */void (*const cb_threadMulti) (/* 倍频因子修改标识,0-减小,1-增加 */char add, /* 当前线程数 */unsigned int threadCount, void *p
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer), /* 线程倍频需要改变时的回调函数的关联变量,可为空 */void *const threadMultiExternP
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pMonitor指向的实例对象 */

    void monitor_delete__OI(void* const pMonitor);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void monitor_startThread(void *const pMonitor
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice
            , /* 在线程退出时,调用的停止线程的方法,为空表示不调用,第一个void*为停止方法(方法仅发送信号,不能阻塞,为空表示数组结束),第二个void*为停止的对象(为空表示跳过),此数据应以方法名为NULL结尾,如{{fun,p},{NULL,NULL}} */
            void *(*const stop_thread_noWait) [2]);

    /* 结束线程 */

    void monitor_stopThread(void *const pMonitor);

    /* 添加任务到队列 */

    void monitorItem_add__OI(Monitor *pMonitor, MonitorItem * pMonitorItem
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ pthread_t parentTid
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 创建一个新的监控线程项,并添加任务到队列;     
     * 返回MonitorItem指针.
     */

    MonitorItem * monitorItem_new_and_add(void *const pMonitor
            , /* 被监控的线程对象的线程名,不能为空 */const char threadName[]
            , /* 后缀,加到threadName后组成新的threadName,可为空 */const char suffix[]
            , /* 被监控的线程对象的地址,可为空 */ void *const monitorAddress
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */const pthread_t parentTid
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 创建一个新的监控线程项,并添加任务到队列;     
     * 返回MonitorItem指针.
     */

    MonitorItem * monitorItem_new_extern_and_add(void *const pMonitor
            , /* 被监控的线程对象的线程名,不能为空 */const char threadName[]
            , /* 后缀,加到threadName后组成新的threadName,可为空 */const char suffix[]
            , /* 序号 */ const unsigned int index
            , /* 被监控的线程对象的地址,可为空 */ void *const monitorAddress
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */const pthread_t parentTid
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 从队列中移除任务.
     */

    void monitorItem_remove__IO(Monitor * const pMonitor, MonitorItem * const pMonitorItem
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 从队列中移除任务,并删除.
     */

    void monitorItem_remove_and_delete(void *const pMonitor, MonitorItem * const pMonitorItem
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 检查pMonitorItem监控线程已使用的堆栈量 */


#define monitorItem_checkStack(pMonitorItem)


    /* 
     * 把pMonitor中的监控树合并为一个字符串输出.
     */

    char* monitor_monitorTree_to_string_pthreadSafety__IO(Monitor * const pMonitor, /* 输出掩码,请使用宏定义 */const unsigned long mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 输出pMonitor监控的所有线程的堆栈,返回要输出堆栈线程数,使用SIGUSR1信号实现 */

    unsigned int monitor_callstack_out_debug_pthreadSafety(Monitor * const pMonitor);


    /* 添加一个定时运行的函数,只运行一次 */

    void monitor_timer_add(Monitor * const pMonitor, /* 创建此定时器的线程号,由systemThreadTid获得 */ const pthread_t stid, /* 运行的时间,只支持系统开机时间 */const double runTime, /* 运行的额外条件,1-代表在清理时强制运行一次 */const signed char externRun
            , /* 要运行的函数 */ void (*const function) (/* 创建此任务的线程号,由systemThreadTid获得 */pthread_t stid, void* parameter1, void* parameter2
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 运行时传给函数的第一个参数 */ void *const parameter1, /* 运行时传给函数的第二个参数 */ void *const parameter2);

    /* 清空本线程创建的所有定时器,不能清除正在运行的定时器 */

    void monitor_timer_thread_clean(Monitor * const pMonitor, /* 指定线程号,由systemThreadTid获得 */const pthread_t stid
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 清空本线程创建的所有定时器,包含正在运行的定时器,不能在运行的定时器中调用 */

    void monitor_timer_thread_clean_all(Monitor * const pMonitor, /* 指定线程号,由systemThreadTid获得 */const pthread_t stid
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 重置pMonitor监控目录中除自已之外的所有监控进程,返回线程句柄 */

    void * monitor_reset_begin(Monitor * const pMonitor);

    /* 结束重置监控目录中除自已之外的所有监控进程 */

    void monitor_reset_end(/* monitor_reset_begin返回的句柄 */void *const handle);





#ifdef __cplusplus
}
#endif

#endif /* TASKMONITOR_H */

