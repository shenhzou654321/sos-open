/* 
 * File:   vsys.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef VSYS_H
#define	VSYS_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Config.h"
#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,2)

    /* vsys系统 */
    struct Vsys {

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
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 获取通道状态的客户端链表 */
            FsStructList *__channelStatusDataClientList;
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
            unsigned char nice;
        } rw;

        struct {
            /* 通道状态数据,为空应返回错误 */
            FsEbml *channelStatusData;
            /* 通道状态数据ebml数据 */
            FsString *channelStatusData_ebml;
            /* 通道状态数据xml数据 */
            FsString *channelStatusData_xml;
            /* 通道状态数据json数据 */
            FsString *channelStatusData_json;
        } p;
    };

#pragma pack(pop)
    /* 
     * 创建一个新的Vsys对象;
     * 返回Vsys指针.
     */
    struct Vsys *vsys_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
            /* 配置管理对象 */ void *pConfigManager);
    /* 删除pVsys指向的实例对象 */
    void vsys_delete__OI(struct Vsys * const pVsys);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void vsys_startThread(struct Vsys * const pVsys,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void vsys_stopThread(struct Vsys * const pVsys);
#ifdef FsDebug
    void vsys_test(void *pMonitor);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* VSYS_H */

