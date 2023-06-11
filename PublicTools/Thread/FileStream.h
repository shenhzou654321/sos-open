/* 
 * File:   FileStream.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef FILESTREAM_H
#define	FILESTREAM_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Thread/Rtsp.h"
#include "../../PublicTools/Thread/Hls.h"

#ifdef	__cplusplus
extern "C" {
#endif

    /* 文件流化,需要RtspServer的支持 */
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
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 线程名 */
#define FileStream_get_threadName(___pFileStream) (((char*)(((FileStream*)(___pFileStream))+1))+((FileStream*)(___pFileStream))->ro._threadName_)
            unsigned char _threadName_;
            /* 根目录,以'/'结尾 */
            char *_rootDir;
            /* rtsp服务对象,为空表示不启用rtsp文件点播 */
            Rtsp *_pRtspServer;
            /* hls服务对象,为空表示不启用hls文件点播 */
            Hls * _pHlsServer;
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
            /* rtsp或hls有请求时的同志管道 */
            int pipe[2];
            /* 管道线程同步锁 */
            pthread_mutex_t mutex_pipe;
        } p;
    } FileStream;
#define __FileStreamPrintf(___Printf,___format, ...)\
    ___Printf(5,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  pRtspServer=%p\n  pHlsServer=%p\n",##__VA_ARGS__\
        ,pFileStream,((FileStream*)(pFileStream))->rw.runStatus,((FileStream*)(pFileStream))->ro.stid\
        ,((FileStream*)(pFileStream))->ro._pMonitor,((FileStream*)(pFileStream))->ro._externRunStatus\
        ,((FileStream*)(pFileStream))->ro._parentTid,FileStream_get_threadName(pFileStream)\
        ,((FileStream*)(pFileStream))->ro._pRtspServer,((FileStream*)(pFileStream))->ro._pHlsServer)
#define FileStreamPrintf(___format, ...) __FileStreamPrintf(FsPrintf,___format,##__VA_ARGS__)
#define FileStreamPrintf2(___format, ...) __FileStreamPrintf(FsPrintf2,___format,##__VA_ARGS__)
    /* 
     * 创建一个新的FileStream对象;
     * 返回FileStream指针.
     */
    FileStream *fileStream_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 根目录,推荐以'/'结尾 */const char rootDir[], /* rtsp服务对象,为空表示不启用rtsp文件点播 */ Rtsp * const pRtspServer, /* hls服务对象,为空表示不启用hls文件点播 */ Hls * const pHlsServer);
    /* 删除pFileStream指向的实例对象 */
    void fileStream_delete__OI(FileStream * const pFileStream);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void fileStream_startThread(FileStream * const pFileStream,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程 */
    void fileStream_stopThread(FileStream* pFileStream);

#ifdef	__cplusplus
}
#endif

#endif	/* FILESTREAM_H */

