/* 
 * File:   taskMonitor.h
 * Author: fslib
 *
 * Created on 2013年2月19日, 下午1:30
 */
#ifndef TASKMONITOR_H
#define	TASKMONITOR_H

#include "../Fs/ObjectList.h"

#ifdef	__cplusplus
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

    typedef struct {

        /* 
         * 结构约定:
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 被监控的线程对象的线程名 */
            char *_threadName;
            /* 系统线程号,由systemThreadTid获得 */
            pthread_t _stid;
            /* 被监控的线程对象的地址 */
            void *_monitoredAddress;
            /* 线程创建时间 */
            double createTime;
            /* 线程用户态占用的CPU时间 */
            unsigned long utime;
            /* 线程用户态占用的CPU占用率X100 */
            float utimeRate;
            /* 线程核心态占用的CPU时间 */
            unsigned long stime;
            /* 线程核心态占用的CPU占用率X100 */
            float stimeRate;
            /* 进程所有已死线程用户态占用的CPU时间 */
            unsigned long cutime;
            /* 线程所有已死线程用户态占用的CPU占用率X100 */
            float cutimeRate;
            /* 线程所有已死线程核心态占用的CPU时间 */
            unsigned long cstime;
            /* 线程所有已死线程核心态占用的CPU占用率X100 */
            float cstimeRate;
        } ro;

        struct {
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
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */

        struct {
            /* 线程名 */
            char *_threadName;
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出;
             */
            signed char *_externRunStatus;
            /* 看门狗监控目录,为空表示不写看门狗,此处记录当前要写的进程文件名 */
            char *_feedDogFile;
            /* 统计周期,单位秒,推荐不要超过10S */
            float _detectCycle;
            /* 定时器周期,越小精度越高,但会占用更多的CPU,单位秒,内部会保证循环执行一次的最短时间不小于此值,推荐不要超过10S */
            float _timerCycle;
            /* 定时器队列 */
            FsObjectList *__pTimerList;
            /* 监控的线程队列 */
            FsObjectList *__pThreadList;
            /* 监控树 */
            void *__monitorTree;
            /* 添加的线程项添加的根位置 */
            void* _pRootAddPositon;
            /* Monitor线程的tid号 */
            pthread_t stid;
            /* 线程同步锁,外部在读取此对象只读区数据所指向的数据内容时必需加锁(threadName除外) */
            pthread_mutex_t __mutex;
            /* 线程总数 */
            unsigned short threadCount;
            /* 进程用户态占用的CPU时间 */
            unsigned long utime;
            /* 进程用户态占用的CPU占用率X100 */
            float utimeRate;
            /* 进程核心态占用的CPU时间 */
            unsigned long stime;
            /* 进程核心态占用的CPU占用率X100 */
            float stimeRate;
            /* 进程所有已死线程用户态占用的CPU时间 */
            unsigned long cutime;
            /* 进程所有已死线程用户态占用的CPU占用率X100 */
            float cutimeRate;
            /* 进程所有已死线程核心态占用的CPU时间 */
            unsigned long cstime;
            /* 进程所有已死线程核心态占用的CPU占用率X100 */
            float cstimeRate;
            /* 已注册线程的总堆栈大小 */
            unsigned long stackSize;
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
        } p;
    } Monitor;
    /* 
     * 创建一个新的监控线程项,只能在要监控的线程中调用此函数;
     * 返回MonitorItem指针.
     */
    MonitorItem * monitorItem_new__IO(/* 被监控的线程对象的线程名 */const char threadName[],
            /* 被监控的线程对象的地址,可为空,只能在要监控的线程中调用此函数 */ void *monitorAddress);
    /* 删除pMonitorItem指向的实例对象 */
    void monitorItem_delete__IO(void* ppMonitorItem);
    /* 
     * 创建一个新的监控线程对象;
     * 返回Monitor指针.
     */
    Monitor * monitor_new__IO(/* 线程名 */const char threadName[],
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出;
             */signed char *externRunStatus,
            /* 看门狗监控目录,为空表示不写看门狗 */const char watchdogDir[],
            /* 写看门狗时是否加入父进程的pid,加入后在父进程退出时会认为本程序已死,0-不加入,1-加入 */const char watchdogParent,
            /* 统计周期,单位秒,推荐不要超过10S */ const float detectCycle,
            /* 定时器周期,越小精度越高,但会占用更多的CPU,单位秒,内部会保证循环执行一次的最短时间不小于此值,推荐不要超过10S */const float timerCycle);
    /* 删除pMonitor指向的实例对象 */
    void monitor_delete__OI(void* pMonitor);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void monitor_startThread(void *const pMonitor,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void monitor_stopThread(void *pMonitor);
    /* 添加任务到队列 */
    void monitorItem_add__OI(Monitor *pMonitor, MonitorItem * pMonitorItem,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ pthread_t parentTid);
    /* 
     * 创建一个新的监控线程项,并添加任务到队列;     
     * 返回MonitorItem指针.
     */
    MonitorItem* monitorItem_new_and_add(void *pMonitor,
            /* 被监控的线程对象的线程名,不能为空 */const char threadName[],
            /* 后缀,加到threadName后组成新的threadName,可为空 */const char suffix[],
            /* 被监控的线程对象的地址,可为空 */ void *monitorAddress,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */pthread_t parentTid);
    /* 
     * 从队列中移除任务.
     */
    void monitorItem_remove__IO(Monitor *pMonitor, MonitorItem *pMonitorItem);
    /* 
     * 从队列中移除任务,并删除.
     */
    void monitorItem_remove_and_delete(void *pMonitor, MonitorItem * pMonitorItem);
    /* 
     * 把pMonitor中的监控树合并为一个字符串输出.
     */
    char* monitor_monitorTree_to_string__IO(Monitor *pMonitor, /* 输出掩码,请使用宏定义 */const unsigned long mask);
    /* 添加一个定时运行的函数,只运行一次 */
    void monitor_timer_add(Monitor * const pMonitor, /* 创建此定时器的线程号,由systemThreadTid获得 */ const pthread_t stid, /* 运行的时间,只支持系统开机时间 */const double runTime, /* 运行的额外条件,1-代表在清理时强制运行一次 */const signed char externRun,
            /* 要运行的函数 */ void (*const function) (/* 创建此任务的线程号,由systemThreadTid获得 */pthread_t stid, void* parameter1, void* parameter2),
            /* 运行时传给函数的第一个参数 */ void *const parameter1, /* 运行时传给函数的第二个参数 */ void *const parameter2);
    /* 清空本线程创建的所有定时器 */
    void monitor_timer_thread_clean(Monitor * const pMonitor, /* 指定线程号,由systemThreadTid获得 */const pthread_t stid);
#ifdef FsDebug
    void monitor_test(void *pMonitor);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* TASKMONITOR_H */

