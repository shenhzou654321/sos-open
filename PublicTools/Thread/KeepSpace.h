/* 
 * File:   KeepSpace.h
 * Author: fslib
 *
 * Created on 2013年8月22日, 下午2:55
 */


#define KEEPSPACE_H
#include "../../PublicTools/publicDefine.h"


#define KEEPSPACE_H
#include <pthread.h>
#include "../../PublicTools/Fs/ObjectList.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 硬盘相数据删除,最大支持256TB */
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
            /* 
             * 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出 
             */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 线程名 */
#define KeepSpace_get_threadName(___pKeepSpace) (((char*)((___pKeepSpace)+1))+(___pKeepSpace)->ro._threadName_)
            unsigned char _threadName_;
            /* 配置文件名 */
#define KeepSpace_get_configFileName(___pKeepSpace) (((char*)((___pKeepSpace)+1))+(___pKeepSpace)->ro._configFileName_)
            unsigned short _configFileName_;
            /* 程序会不停检查此文件,有时更新配置并删除此文件 */
#define KeepSpace_get_configUpdateCheckFile(___pKeepSpace) (((char*)((___pKeepSpace)+1))+(___pKeepSpace)->ro._configUpdateCheckFile_)
            unsigned short _configUpdateCheckFile_;
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
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    } KeepSpace;
#define __KeepSpaceLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  configFileName:\"%s\"\n  configUpdateCheckFile:\"%s\"\n",##__VA_ARGS__\
        ,pKeepSpace,((KeepSpace*)(pKeepSpace))->rw.runStatus,((KeepSpace*)(pKeepSpace))->p.stid\
        ,((KeepSpace*)(pKeepSpace))->ro._pMonitor,((KeepSpace*)(pKeepSpace))->ro._externRunStatus,((KeepSpace*)(pKeepSpace))->ro._parentTid,KeepSpace_get_threadName(pKeepSpace)\
        ,KeepSpace_get_configFileName(pKeepSpace),KeepSpace_get_configUpdateCheckFile(pKeepSpace))
#define KeepSpaceLog(___loglevel,___format, ...) __KeepSpaceLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define KeepSpaceLog2(___loglevel,___format, ...) __KeepSpaceLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    //    /* 
    //     * 创建一个新的KeepSpace对象;
    //     * 成功返回KeepSpace指针;
    //     * 失败返回NULL.
    //     */
    //    KeepSpace * keepSpace_new_by_vroadIni__IO(/* 线程名 */const char threadName[],
    //            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
    //            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
    //            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
    //            /* 配置程序文件名 */ const char configFileName[]);

    /* 
     * 创建一个新的KeepSpace对象;
     * 返回KeepSpace指针.
     */

    KeepSpace *keepSpace_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 配置文件名 */ const char configFileName[], /* 程序会不停检查此文件,有时更新配置并删除此文件 */ const char configUpdateCheckFile[]);

    /* 删除pKeepSpace指向的实例对象 */

    void keepSpace_delete__OI(KeepSpace * const pKeepSpace);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void keepSpace_startThread(KeepSpace * const pKeepSpace, /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级 */const unsigned char thisNice);

        /* 结束线程,只发送信号,不回收线程资源 */

    void keepSpace_stopThread_noWait(KeepSpace * const pKeepSpace);

    /* 结束线程 */

    void keepSpace_stopThread(KeepSpace * const pKeepSpace);


#ifdef __cplusplus
}
#endif

 /* KEEPSPACE_H */

