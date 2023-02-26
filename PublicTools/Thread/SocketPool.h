/* 
 * File:   SocketPool.h
 * Author: fslib.org
 *
 * Created on 2017年8月25日, 上午11:19
 */

#if !defined SOCKETPOOL_H && !defined JAVA_IN_C

#include "../../PublicTools/publicDefine.h"

#define SOCKETPOOL_enable
#define SOCKETPOOL_H
//#define SocketPool_HttpDebug

#include "../../PublicTools/Fs/BindClassify.h"

#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Object.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/String.h"
#include "../../PublicTools/Thread/monitor.h"
#ifdef __cplusplus
extern "C" {
#endif
    struct SocketPool_item;

    /* 
     * socket连接池,外部创建连接后把SocketPool_item添加到本对象,支持连接在线程间无缝插拔;
     */
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
#define SocketPool_get_threadName(___pSocketPool) (((char*)((___pSocketPool)+1))+(___pSocketPool)->ro._threadName_)
            unsigned char _threadName_;
            /* 处理的Socket链表 */
            FsObjectList *__socketList_;
            /* 接收队列的最大容量 */
            unsigned short _recvListMaxCount;
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
            /* 外部和内部线程异步通信的链表,结构为SocketPool_P_control */
            FsStructList *__controlList;
            /* 最长没有发送和接收数据的时间 */
            double _keepTime;
#define SocketPool_ByteCount // 收发数据计数
#ifdef SocketPool_ByteCount
            /* 总发送的字节数 */
            unsigned long toallSendByte;
            /* 总接收的字节数 */
            unsigned long toallRecvByte;
#endif
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    } SocketPool;
#ifdef SocketPool_ByteCount
#define __SocketPoolLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  socketList=%p/%lu,recvListMaxCount=%hu,keepTime=%lf,toallSendByte=%lu/%lu\n",##__VA_ARGS__\
        ,pSocketPool,((SocketPool*)(pSocketPool))->rw.runStatus,((SocketPool*)(pSocketPool))->p.inRun,((SocketPool*)(pSocketPool))->p.stid,((SocketPool*)(pSocketPool))->ro._pMonitor\
        ,((SocketPool*)(pSocketPool))->ro._externRunStatus,((SocketPool*)(pSocketPool))->ro._parentTid,((SocketPool*)(pSocketPool))->ro.__pipe[0], ((SocketPool*)(pSocketPool))->ro.__pipe[1],SocketPool_get_threadName(pSocketPool)\
        ,((SocketPool*)(pSocketPool))->ro.__socketList_, ((SocketPool*)(pSocketPool))->ro.__socketList_->nodeCount,((SocketPool*)(pSocketPool))->ro._recvListMaxCount,((SocketPool*)(pSocketPool))->rw._keepTime\
        ,((SocketPool*)(pSocketPool))->rw.toallSendByte,((SocketPool*)(pSocketPool))->rw.toallRecvByte)
#else
#define __SocketPoolLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  socketList=%p/%lu,recvListMaxCount=%hu,keepTime=%lf\n",##__VA_ARGS__\
        ,pSocketPool,((SocketPool*)(pSocketPool))->rw.runStatus,((SocketPool*)(pSocketPool))->p.inRun,((SocketPool*)(pSocketPool))->p.stid,((SocketPool*)(pSocketPool))->ro._pMonitor\
        ,((SocketPool*)(pSocketPool))->ro._externRunStatus,((SocketPool*)(pSocketPool))->ro._parentTid,((SocketPool*)(pSocketPool))->ro.__pipe[0], ((SocketPool*)(pSocketPool))->ro.__pipe[1],SocketPool_get_threadName(pSocketPool)\
        ,((SocketPool*)(pSocketPool))->ro.__socketList, ((SocketPool*)(pSocketPool))->ro.__socketList->nodeCount,((SocketPool*)(pSocketPool))->ro._recvListMaxCount,((SocketPool*)(pSocketPool))->rw._keepTime)
#endif
#define SocketPoolLog(___loglevel,___format, ...) __SocketPoolLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define SocketPoolLog2(___loglevel,___format, ...) __SocketPoolLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    struct SocketPool_item {

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
            /* 关联的FsBindClassify_item_Connect对象,为空表示不关联,不为空表示本连接来自FsBindClassify_item_Connect分拣,来自FsBindClassify_item_Connect的连接是不支持重连的 */
            struct FsBindClassify_item_Connect *_pBindClassify_item_Connect;
            /* 域名,可为空 */
#define SocketPool_item_get_domain(___pSocketPool_item,___nullValue) (!FsNumberIsMax((___pSocketPool_item)->ro._domain_)?(((char*)((___pSocketPool_item)+1))+(___pSocketPool_item)->ro._domain_):(___nullValue))
            unsigned short _domain_;
            /* 接收时应先接收的长度,用以调回调函数确定此次应接收的长度,在_pGetRecvLenth__IO不为NULL,此处为0,表示收的数据长度是不确定的 */
            unsigned int _recvHeadLenth;
            /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */
            unsigned int _prefix;
            /* 包的最大大小,大于packageSizeMax时,如设置了缓存目录,会把数据写入临时缓存中反回时,数据长度为0,其数据内容为文件名,没有则标记连接出错 */
            unsigned long _packageSizeMax;
            /*
             * 用以确定此次应接收的长度,为空表示收的数据是http数据;
             * 根据head确定此次应接收的长度并初始化一个FsObjectBasePthreadSafety空间用于保存数据;
             * 在包的大小大于_packageSizeMax时,仅分配可储存头大小的空间,但len的值仍然为实际数据内容大小的值;
             * 成功返回FsObjectBasePthreadSafety指针;
             * 失败返回NULL.
             */
            FsObjectBasePthreadSafety* (*_pGetRecvLenth__IO)(struct SocketPool_item * const pSocketPool_item, /* 头数据,内存对齐后的指针 */ char head[]
                    , /* 接收数据的buffer,为空表示不使用缓存 */FsObjectBaseBuffer * const recvBuffer);
            /* 在数据包过大时,缓存buffer的目录,以/结尾,为空时,收到大包时标记连接出错 */
            FsObjectString *__bufferDir;
            /* 
             * 要发送的队列,列表长度由外部控制;
             * 每项必须封装为FsObjectBase或子类传入;
             * 发送后会把此条数据删除.
             */
            FsObjectList *__sendList;
            /* 
             * 接收的队列,列表长度由内部控制;
             * 各项为FsObjectBasePthreadSafety指针;
             * 在单包数据较小时直接存放包数据内容,在包过大时,数据写入文件,FsObjectBasePthreadSafety的len为0,data存放的是文件名;
             * 如果长度达到队列的最大值,则卡住暂不接收.
             */
            FsObjectList *__recvList;
        } ro;

        struct {
            /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* IPV4地址端口 */
            unsigned short _ipv4Port;
            /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */
            unsigned char _timeoutCheckMethod : 2;
            /* 最后一次发数据的时间(CPU的uptime时间) */
            double lastSendTime;
            /* 最后一次收数据的时间(CPU的uptime时间) */
            double lastRecvTime;
            /* 总发送的字节数 */
            unsigned long toallSendByte;
            /* 总发送的字节数 */
            unsigned long toallRecvByte;
        } rw;
#pragma pack(push,8)
        /* 附加数据,本对象不会使用 */
        char externData[(sizeof (unsigned int) << 1) + (sizeof (void*) << 1)];
#pragma pack(pop)

        struct {
            /* 再添加到SocketPool后会把此值赋为pSocketPool,移除时置NULL */
            SocketPool *pSocketPool;
            /* 在收到数据后的回调函数,仅当接收缓存为0时才回调,即边缘触发,在连接成功和连接断开时也会回调 */
            void (*_cb_recvData)(struct SocketPool_item * const pSocketPool_item, void *const p);
            /* 收到数据后的回调函数的关联指针 */
            void *_cb_recvDataP;
            /* 正在发送的数据 */
            FsObjectBase *__sendingData;
            /* 正在接收的数据,在包过大时,数据会写入文件,本变量储存写文件需要的变量,在写入完成后仅储存文件名,其长度为0 */
            FsObjectBasePthreadSafety *__recvdata;
            /* 头数据的缓存空间,结构为SocketPool_P_item_headBuffer */
#define SocketPool_item_check_http_close // 定义后内部会检查http的关闭标记
            void *__pHeadBuffer;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
#define SocketPool_item_wait(___pSocketPool_item) do{while((___pSocketPool_item)->p.busy)FsWaitSyncDo();}while(0)
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            unsigned char _1 : 5;
            /* 连接建立后收发过数据没有,&0x1表示收过数据,&0x2表示发送过数据 */
            unsigned char doData : 2;
            unsigned char _2 : 6;
            /* 连接模式:
             * 0:默认模式,断开后自动连接;
             * 1:断开后不重连;
             * 2:保持断开模式;
             * 3:设定此值为0,如果是连接模式,则断开一次连接;
             * 4:设定此值为0,如果是连接模式且已收过数据,则断开一次连接;
             * 5:设定此值为0,如果是连接模式且已发过数据,则断开一次连接;
             * 6:设定此值为0,如果是连接模式且已收发过数据,则断开一次连接;
             * 7:设定此值为0,如果是连接模式且已收或发过数据,则断开一次连接;
             */
            unsigned char connMode : 3;
            /* 连接的soket,不能根据此值判断连接是否有效 */
            int __sock;
            /* 最后一次成功建立连接的时间(CPU的uptime时间)没有建立连接时为0或负数,为负数表示正在尝试连接 */
            double createTime;
            /* 已发送的长度 */
            unsigned int sendLen;
            /* 已接收的长度 */
            unsigned int recvLen;
        } p;
    };
#define SocketPool_item_sendInfo_update(___sendLen) do{\
    pSocketPool_item->rw.toallSendByte += ___sendLen;\
    pSocketPool->rw.toallSendByte += ___sendLen;\
    pSocketPool_item->rw.lastSendTime = thisUptime;\
}while (0)
#define SocketPool_item_recvInfo_update(___recvLen) do{\
    pSocketPool_item->rw.toallRecvByte += ___recvLen;\
    pSocketPool->rw.toallRecvByte += ___recvLen;\
    pSocketPool_item->rw.lastRecvTime = thisUptime;\
}while (0)
#define __SocketPool_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p/%p/%p,server(%hhu/%hhu/%hhx/%hhx,sock=%d/%lf):\"%s%s%hhu.%hhu.%hhu.%hhu%s:%hu\",prefix=%d,packageSizeMax=%lu,bufferDir=%p/%d/%lu/\"%s\",recvdata(recvList=%p/%lu)=%p/%u\
,senddata(sendList=%p/%lu)=%p/%lu/%ld/%ld/%u,recvHeadLenth=%u,pGetRecvLenth__IO=%p,timeoutCheckMethod=%hhx,lastSendTime=%lf,lastRecvTime=%lf,toallSendByte=%lu,toallRecvByte=%lu,pGetRecvLenth__IO=%p,cb_recvDataP=%p.\n",##__VA_ARGS__\
        ,pSocketPool_item,pSocketPool_item->p.pSocketPool,pSocketPool_item->ro._pBindClassify_item_Connect,pSocketPool_item->p.busy,pSocketPool_item->p._controlStatus,pSocketPool_item->p.doData,pSocketPool_item->p.connMode\
        ,pSocketPool_item->p.__sock, pSocketPool_item->p.createTime,SocketPool_item_get_domain(pSocketPool_item,""), (SocketPool_item_get_domain(pSocketPool_item,NULL) ? "(" : "")\
        ,Fs_Network_Ipv4_print(pSocketPool_item->rw._ipv4),(SocketPool_item_get_domain(pSocketPool_item,NULL) ? ")" : ""), pSocketPool_item->rw._ipv4Port,pSocketPool_item->ro._prefix,pSocketPool_item->ro._packageSizeMax\
        ,pSocketPool_item->ro.__bufferDir,(pSocketPool_item->ro.__bufferDir?fs_ObjectBase_referCount_get(pSocketPool_item->ro.__bufferDir):0)\
        ,(pSocketPool_item->ro.__bufferDir?pSocketPool_item->ro.__bufferDir->lenth:0),(pSocketPool_item->ro.__bufferDir?pSocketPool_item->ro.__bufferDir->buffer:"")\
        ,pSocketPool_item->ro.__recvList,pSocketPool_item->ro.__recvList->nodeCount,pSocketPool_item->p.__recvdata,pSocketPool_item->p.recvLen\
        ,pSocketPool_item->ro.__sendList,pSocketPool_item->ro.__sendList->nodeCount,pSocketPool_item->p.__sendingData,(pSocketPool_item->p.__sendingData?pSocketPool_item->p.__sendingData->len:0L)\
        ,(pSocketPool_item->p.__sendingData?pSocketPool_item->p.__sendingData->_mallocLenth:0L),(pSocketPool_item->p.__sendingData?fs_ObjectBase_referCount_get(pSocketPool_item->p.__sendingData):0L),pSocketPool_item->p.sendLen\
        ,pSocketPool_item->ro._recvHeadLenth,pSocketPool_item->ro._pGetRecvLenth__IO,pSocketPool_item->rw._timeoutCheckMethod,pSocketPool_item->rw.lastSendTime,pSocketPool_item->rw.lastRecvTime\
        ,pSocketPool_item->rw.toallSendByte,pSocketPool_item->rw.toallRecvByte,pSocketPool_item->p._cb_recvData,pSocketPool_item->p._cb_recvDataP)
#define SocketPool_itemLog(___loglevel,___format, ...) __SocketPool_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define SocketPool_itemLog2(___loglevel,___format, ...) __SocketPool_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)


#if defined SOCKETPOOL_enable
    /* 
     * 创建一个新的SocketPool发送与接收线程项;
     * 返回SocketPool指针.
     */

    SocketPool *socketPool_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 接收队列的最大容量 */const unsigned short recvListMaxCount, /* 最长没有发送和接收数据的时间 */ const double keepTime);

    /* 删除pSocketPool指向的实例对象 */

    void socketPool_delete__OI(SocketPool * const pSocketPool);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void socketPool_startThread(SocketPool * const pSocketPool
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数,不使用缓存 */

    void socketPool_startThread_noBuffer(SocketPool * const pSocketPool
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void socketPool_stopThread_noWait(SocketPool * const pSocketPool);

    /* 结束线程 */

    void socketPool_stopThread(SocketPool * const pSocketPool);

    /* 使用ip和端口进行排序,node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */

    int socketPool_item_compare_ip_and_port(struct SocketPool_item * const node1, struct SocketPool_item * const node2);

    /* 使用ip和端口进行排序,node1使用具体的数字代替,node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */

    int socketPool_item_compare_ip_and_port_do(const unsigned int ipv4, const unsigned short ipv4Port, struct SocketPool_item * const node2);

    /* 
     * 创建一个连接项;
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item * socketPool_item_new__IO(/* 域名,可为空 */ const char domain[], /* 域名长度,在domain不为空时有效 */const unsigned int domainLen
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收时应先接收的长度,用以调回调函数确定此次应接收的长度 */const unsigned short recvHeadLenth
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix
            , /*
             * 用以确定此次应接收的长度,为空表示收的数据是http数据;
             * 根据head确定此次应接收的长度并初始化一个FsObjectBasePthreadSafety空间用于保存数据;
             * 在包的大小大于_packageSizeMax时,仅分配可储存头大小的空间,但len的值仍然为实际数据内容大小的值;
             * 成功返回FsObjectBasePthreadSafety指针;
             * 失败返回NULL.
             */ FsObjectBasePthreadSafety* (*pGetRecvLenth__IO)(struct SocketPool_item * const pSocketPool_item, /* 头数据,内存对齐后的指针 */ char head[]
            , /* 接收数据的buffer,为空表示不使用缓存 */FsObjectBaseBuffer * const recvBuffer)
            , /* 包的最大大小,大于packageSizeMax时,如设置了缓存目录,会把数据写入临时缓存中反回时,数据长度为0,其数据内容为文件名,没有则标记连接出错 */ const unsigned long packageSizeMax
            , /* 在数据包过大时,缓存buffer的目录,以/结尾,为空时,收到大包时标记连接出错 */ FsObjectString * const bufferDir);

    /* 
     * 创建一个连接项,连接是外面已成功建立了的;
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item *socketPool_item_new_bySock__IO__OI(/* 已建立好的sock连接 */const int sock
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收时应先接收的长度,用以调回调函数确定此次应接收的长度 */const unsigned short recvHeadLenth
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix
            , /*
             * 用以确定此次应接收的长度,为空表示收的数据是http数据;
             * 根据head确定此次应接收的长度并初始化一个FsObjectBasePthreadSafety空间用于保存数据;
             * 在包的大小大于_packageSizeMax时,仅分配可储存头大小的空间,但len的值仍然为实际数据内容大小的值;
             * 成功返回FsObjectBasePthreadSafety指针;
             * 失败返回NULL.
             */ FsObjectBasePthreadSafety* (*pGetRecvLenth__IO)(struct SocketPool_item * const pSocketPool_item, /* 头数据,内存对齐后的指针 */ char head[]
            , /* 接收数据的buffer,为空表示不使用缓存 */FsObjectBaseBuffer * const recvBuffer)
            , /* 包的最大大小,大于packageSizeMax时,如设置了缓存目录,会把数据写入临时缓存中反回时,数据长度为0,其数据内容为文件名,没有则标记连接出错 */ const unsigned long packageSizeMax
            , /* 在数据包过大时,缓存buffer的目录,以/结尾,为空时,收到大包时标记连接出错 */ FsObjectString * const bufferDir);

    /* 16字节头设计(解码后的原始样式,传输时会因校验方法的不同而不同)
     *     |--4bit流通道--|--4bit数据格式类型--|--3Byte序号--|--4Byte长度--|--2Byte扩充长度--|--2Byte校验和--|--3Byte虚拟连接号--|--3bit的校正位--|--5bit的校验方法--|
     *     第一个字节
     *         .... ...1 :第八位--表示数据是否是二进制数据,1-是,0-不是;
     *         .... ..1. :第七位--表示数据是否是回执数据,1-是,0-不是;
     *         .... .1.. :第六位--表示数据是否是错误数据,1-是(回执数据时,表示发送的命令或请求不支持或错误或需要重定向--重定向的数据肯定是含有格式的数据,包含redirect节点,不是回执数据时,报告服务端的一些错误),0-不是;
     *         .... 1... :第五位--表示数据的解析方式:
     *             当为二进制数据时,为0表示用ebml方式解析数据,为1表示数据序列,有两种格式(两种格式不能混用),数据内容按如下定义:
     *                 0000 .... :第一个字节的高4位为0时--由多个序列组成,每个序列按如下方式定义;
     *                     第1个字节的低4位为预留字段,固定为0;
     *                     第2个到第4个字节(3字节,小端模式)表示数据类型,具体参考数据序列的定义;
     *                     第5个到第7个字节(3字节,小端模式)子头长度(不包含16字节头的头长度);
     *                     第8个字节表示填充数据长度;
     *                     第9个到第14个字节(6字节,小端模式)表示数据长度(包含子头+数据区+填充);
     *                     第15个到第16个字节(2字节,小端模式)表示层数,一般用于显示;
     *                     填充长度为0时,没有填充数据;
     *                     填充长度为1时,填充数据固定为0;
     *                     填充长度为2时,填充数据固定为00;
     *                     填充长度大于2时,填充数据为0+(填充长度-2)字节的自定义数据+0,系统默认此自定义数据为此序列的解释内容;
     *                 1111 .... :第一个字节的高4位为非0时--只有一个序列,数据内存为特殊数据内容,如FFD8表示jpg数据;
     *             当为全文本数据数据时,为0表示数据是无格式数据,打印显示既可,为1表示数据是有格式数据,可能是json数据,也可能是xml数据;
     *         1111 .... :第一个字节的第一到第四位--表示一些快速解析的数据类型,在实际通信中进行字义;
     *             其中全0:表示默认的设置和回执类型;
     *             非0:表示各类流(发送的序号只由客户端改变);
     *             全1:表示默认的流通道(保留,心跳的第一个字节固定为0xF1,心跳回执固定为0xF3,且长度为0);
     *     第2个字节到第4个字节:表示发送的序号(3字节,小端模式),应实现循环使用,应根据不同的流进行分别设置
     *     第5个字节到第8个字节:表示数据长度(4字节,小端模式),如其值小于0x80000000表示头为8字节,否则头为16字节;
     *     第9个字节到第10个字节:为数据长度的扩充长度(2字节,小端模式),值为5个到8个字节值a与本字段值b按公式(a&0x7FFFFFFF)|(b << 31) >计算得到;
     *     第11个字节到第12个字节:表示校验和(2字节),计算方法受校验方法的编号不同而不同:
     *     第13个字节到第15个字节:表示虚拟连接号(3字节,小端模式),0-表示默认的虚拟连接;
     *     第16个字节高3bit:表示校正位,在头按校验方式编码后,本字段设置合适的值后把第16字节同第1字节异或后作为第一字节的传输值,避免第一字节出现一些特殊字符(如H,h,r,R等),不为0,为0表示16字节头是解码状态
     *     第16个字节低5bit:表示16字节头的校验方法,按如下定义(校验的目的:在连接层过滤掉一些非法连接):
     *         分两类预设值[0-15]与自定义[16-31](两类本身没有区别,在使用时一般是0到多个预设值加0到一个自定义值)
     *         其中0表示为解码状态
     *         校验方法1:建议数据按2字节异或校验(不验证,用于测试);
     ***************************************************************************
     *     校验码生成的流程(原则,长度字段,校验字段,校正位,校验方法不能加密及修改):
     *         1,把校正位预设为置0,填充好头;
     *         2,按校验方式生成校验码存于校验位;
     *         3,设置合适的校正位,使用第16字节同第一字节异或得到第一字节的值;
     ***************************************************************************
     *     协议规定数据包单包可达2^47=128TB,但在编程时应限制数据包单包大小,对大数据包
     *     进行回调进入写文件流程,只把限制大小范围内的数据存于内存分析;
     */

    struct SocketPool_item *socketPool_item_new_head16__IO(/* 域名,可为空 */ const char domain[], /* 域名长度,在domain不为空时有效 */const unsigned int domainLen
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix
            , /* 包的最大大小,大于packageSizeMax时,如设置了缓存目录,会把数据写入临时缓存中反回时,数据长度为0,其数据内容为文件名,没有则标记连接出错 */ const unsigned long packageSizeMax
            , /* 在数据包过大时,缓存buffer的目录,为空时,收到大包时标记连接出错 */ FsObjectString * const bufferDir);

    /* 
     * 创建一个新的16字节头的连接项,如果连接断开不重连;
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item * socketPool_item_new_head16_bySock__IO__OI(/* 已建立好的sock连接 */const int sock
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix
            , /* 包的最大大小,大于packageSizeMax时,如设置了缓存目录,会把数据写入临时缓存中反回时,数据长度为0,其数据内容为文件名,没有则标记连接出错 */ const unsigned long packageSizeMax
            , /* 在数据包过大时,缓存buffer的目录,为空时,收到大包时标记连接出错 */ FsObjectString * const bufferDir);

    /* 
     * 创建一个新的标准连接项;
     * 有8个字节的头,其中后4个字节表示该数据包的长度(不包含头,小端模式),前4个字节,本对象不作解析,建议按如下定义:
     *     第一个字节的第八位:表示数据是否是二进制数据,1-是,0-不是;
     *     第一个字节的第七位:表示数据是否是回执数据,1-是,0-不是;
     *     第一个字节的第六位:表示数据是否是错误数据,1-是(回执数据时,表示发送的命令或请求不支持或错误或需要重定向--重定向的数据肯定是含有格式的数据,包含redirect节点,不是回执数据时,报告服务端的一些错误),0-不是;
     *     第一个字节的第五位:表示数据的解析方式:
     *          当为二进制数据时,为0表示用ebml方式解析数据,为1表示数据序列,有两种格式(两种格式不能混用):
     *              第一个字节的高4位为0时:由多个序列组成,每个序列按如下方式定义;
     *                   第1个字节的低4位为预留字段,固定为0;
     *                   第2个到第4个字节(3字节,小端模式)表示数据类型,具体参考数据序列的定义;
     *                   第5个到第8个字节表示本序列数据区+(填充)+1字节(填充长度)的长度;
     *                   填充长度为0时,没有填充数据;
     *                   填充长度为1时,填充数据固定为0;
     *                   填充长度为2时,填充数据固定为00;
     *                   填充长度大于2时,填充数据为0+(填充长度-2)字节的自定义数据+0,系统默认此自定义数据为此序列的解释内容;
     *              第一个字节的高4位为非0时:只有一个序列,数据内存为特殊数据内容,如FFD8表示jpg数据;
     *          当为全文本数据数据时,为0表示数据是无格式数据,打印显示既可,为1表示数据是有格式数据,可能是json数据,也可能是xml数据;
     *     第一个字节的第一到第四位:表示一些快速解析的数据类型,在实际通信中进行字义;
     *          其中全0:表示默认的设置和回执类型;
     *              非0:表示各类流(发送的序号只由客户端改变);
     *              全1:表示默认的流通道(保留,心跳的第一个字节固定为0xF1,心跳回执固定为0xF3,且长度为0);
     *     第二个字节到第四个字节:表示发送的序号,低位在前,应实现循环使用,应根据不同的流进行分别设置.
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item *socketPool_item_new_standard__IO(/* 域名,可为空 */ const char domain[], /* 域名长度,在domain不为空时有效 */const unsigned int domainLen
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix);

    /* 
     * 创建一个新的标准连接项(与socketPool_item_new_standard__IO不同的是长度包含头的长度);
     * 有8个字节的头,其中后4个字节表示该数据包的长度(包含头,小端模式),前4个字节,本对象不作解析,建议按如下定义:
     *     第一个字节的第八位:表示数据是否是二进制数据,1-是,0-不是;
     *     第一个字节的第七位:表示数据是否回执数据,1-是,0-不是;
     *     第一个字节的第六位:表示数据是否是错误数据,1-是(回执数据时,表示发送的命令或请求不支持或错误或需要重定向--重定向的数据肯定是含有格式的数据,不是回执数据时,报告服务端的一些错误),0-不是;
     *     第一个字节的第五位:表示数据的解析方式:
     *          当为二进制数据时,为0表示用ebml方式解析数据,为1表示数据序列,有两种格式(两种格式不能混用):
     *              第一个字节的高4位为0时:由多个序列组成,每个序列按如下方式定义;
     *                   第1个字节的低4位为预留字段,固定为0;
     *                   第2个到第4个字节(3字节,小端模式)表示数据类型,具体参考数据序列的定义;
     *                   第5个到第8个字节表示本序列数据区+(填充)+1字节(填充长度)的长度;
     *                   填充长度为0时,没有填充数据;
     *                   填充长度为1时,填充数据固定为0;
     *                   填充长度为2时,填充数据固定为00;
     *                   填充长度大于2时,填充数据为0+(填充长度-2)字节的自定义数据+0,系统默认此自定义数据为此序列的解释内容;
     *              第一个字节的高4位为非0时:只有一个序列,数据内存为特殊数据内容,如FFD8表示jpg数据;
     *          当为全文本数据数据时,为0表示数据是无格式数据,打印显示既可,为1表示数据是有格式数据,可能是json数据,也可能是xml数据;
     *     第一个字节的第一到第四位:表示一些快速解析的数据类型,在实际通信中进行字义;
     *          其中全0:表示默认的设置和回执类型;
     *              非0:表示各类流(发送的序号只由客户端改变);
     *              全1:表示默认的流通道(保留,心跳的第一个字节固定为0xF1,心跳回执固定为0xF3,且长度为0);
     *     第二个字节到第四个字节:表示发送的序号,低位在前,应实现循环使用,应根据不同的流进行分别设置.
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item *socketPool_item_new_standard2__IO(/* 域名,可为空 */ const char domain[], /* 域名长度,在domain不为空时有效 */const unsigned int domainLen
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix);

    /* 
     * 创建一个新的标准连接项,如果连接断开不重连;
     * 有8个字节的头,其中后4个字节表示该数据包的长度(不包含头,小端模式),前4个字节,本对象不作解析,建议按如下定义:
     *     第一个字节的第八位:表示数据是否是二进制数据,1-是,0-不是;
     *     第一个字节的第七位:表示数据是否回执数据,1-是,0-不是;
     *     第一个字节的第六位:表示数据是否是错误数据,1-是(回执数据时,表示发送的命令或请求不支持或错误或需要重定向--重定向的数据肯定是含有格式的数据,不是回执数据时,报告服务端的一些错误),0-不是;
     *     第一个字节的第五位:表示数据的解析方式:
     *          当为二进制数据时,为0表示用ebml方式解析数据,为1表示数据序列,有两种格式(两种格式不能混用):
     *              第一个字节的高4位为0时:由多个序列组成,每个序列按如下方式定义;
     *                   第1个字节的低4位为预留字段,固定为0;
     *                   第2个到第4个字节(3字节,小端模式)表示数据类型,具体参考数据序列的定义;
     *                   第5个到第8个字节表示本序列数据区+(填充)+1字节(填充长度)的长度;
     *                   填充长度为0时,没有填充数据;
     *                   填充长度为1时,填充数据固定为0;
     *                   填充长度为2时,填充数据固定为00;
     *                   填充长度大于2时,填充数据为0+(填充长度-2)字节的自定义数据+0,系统默认此自定义数据为此序列的解释内容;
     *              第一个字节的高4位为非0时:只有一个序列,数据内存为特殊数据内容,如FFD8表示jpg数据;
     *          当为全文本数据数据时,为0表示数据是无格式数据,打印显示既可,为1表示数据是有格式数据,可能是json数据,也可能是xml数据;
     *     第一个字节的第一到第四位:表示一些快速解析的数据类型,在实际通信中进行字义;
     *          其中全0:表示默认的设置和回执类型;
     *              非0:表示各类流(发送的序号只由客户端改变);
     *              全1:表示默认的流通道(保留,心跳的第一个字节固定为0xF1,心跳回执固定为0xF3,且长度为0);
     *     第二个字节到第四个字节:表示发送的序号,低位在前,应实现循环使用,应根据不同的流进行分别设置.
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item *socketPool_item_new_standard_bySock__IO__OI(/* 已建立好的sock连接 */const int sock
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix);

    /* 
     * 创建一个新的auto连接项(自动连接项),支持(4字节头+4字节长度(长度不含8字节头))和http,rtsp协议,最后读到的数据长度+1经8字节对齐后放4字节的数据头长度,再放4字节的数据类型;
     * 第一个字节的最高位为1表示是(4字节头+4字节长度(长度不含8字节头))数据,前8字节中后4个字节表示该数据包的长度(不包含头,小端模式),其余字节不解析,建议按如下定义:
     *     第一个字节的第八位:表示数据是否是二进制数据,1-是,0-不是;
     *     第一个字节的第七位:表示数据是否回执数据,1-是,0-不是;
     *     第一个字节的第六位:表示数据是否是错误数据,1-是(回执数据时,表示发送的命令或请求不支持或错误或需要重定向--重定向的数据肯定是含有格式的数据,不是回执数据时,报告服务端的一些错误),0-不是;
     *     第一个字节的第五位:表示数据的解析方式:
     *          当为二进制数据时,为0表示用ebml方式解析数据,为1表示数据序列,有两种格式(两种格式不能混用):
     *              第一个字节的高4位为0时:由多个序列组成,每个序列按如下方式定义;
     *                   第1个字节的低4位为预留字段,固定为0;
     *                   第2个到第4个字节(3字节,小端模式)表示数据类型,具体参考数据序列的定义;
     *                   第5个到第8个字节表示本序列数据区+(填充)+1字节(填充长度)的长度;
     *                   填充长度为0时,没有填充数据;
     *                   填充长度为1时,填充数据固定为0;
     *                   填充长度为2时,填充数据固定为00;
     *                   填充长度大于2时,填充数据为0+(填充长度-2)字节的自定义数据+0,系统默认此自定义数据为此序列的解释内容;
     *              第一个字节的高4位为非0时:只有一个序列,数据内存为特殊数据内容,如FFD8表示jpg数据;
     *          当为全文本数据数据时,为0表示数据是无格式数据,打印显示既可,为1表示数据是有格式数据,可能是json数据,也可能是xml数据;
     *     第一个字节的第二到第四位:表示一些快速解析的数据类型,在实际通信中进行字义;
     *          其中全0:表示默认的设置和回执类型;
     *              非0:表示各类流(发送的序号只由客户端改变);
     *              全1:表示默认的流通道(保留,心跳的第一个字节固定为0xF1,心跳回执固定为0xF3,且长度为0);
     *     第二个字节到第四个字节:表示发送的序号,低位在前,应实现循环使用,应根据不同的流进行分别设置.
     * 第一个字节的最高位为0表示一些可以理解的协议,如http,rtsp协议;
     * 返回SocketPool_item指针.
     */




    /* 
     * 创建一个http连接项;
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item * socketPool_item_new_http__OI(/* 域名,可为空 */ const char domain[], /* 域名长度,在domain不为空时有效 */const unsigned int domainLen
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix);

    /* 
     * 创建一个http连接项,如果连接断开不重连;
     * 返回SocketPool_item指针.
     */

    struct SocketPool_item *socketPool_item_new_http_bySock__OI__OI(/* 已建立好的sock连接 */const int sock
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 超时的检查方式,&0x1-表示判断收数据超时,&0x2-表示判断发数据超时,0x3-表示同时判断收数据和发数据都超时才算超时 */ const unsigned char timeoutCheckMethod
            , /* 接收数据的偏移位置,不能小于FsObjectBasePthreadSafety结构大小,必须是8的倍数 */ const unsigned int prefix);

    /* 删除pSocketPool_item指向的实例对象 */

    void socketPool_item_delete__OI(struct SocketPool_item * const pSocketPool_item);

    /* 连接项在添加到连接池之前允许修改的参数 */

    void socketPool_item_set_parameter_before_add(struct SocketPool_item * const pSocketPool_item
            , /* 在收到数据后的回调函数,仅当接收缓存为0时才回调,即边缘触发,在连接成功和连接断开时也会回调 */ void (*const cb_recvData) (struct SocketPool_item * const pSocketPool_item, void *const p),
            /* 收到数据后的回调函数的关联指针 */ void *const cb_recvDataP);

    /* 
     *  设置已接收的数据,必须在添加到socketPool之前设置;
     */

    void socketPool_item_set_has_recv_data_before_add(struct SocketPool_item * const pSocketPool_item, /* 要设置的数据长度,不能为0 */unsigned int dataLen, /* 设置数据的开始指针 */const char data[]);

    /* 向pSocketPool中添加一个连接项 */

    void socketPool_add_item__OI_2(SocketPool * const pSocketPool, struct SocketPool_item * const pSocketPool_item);

    /* 从pSocketPool中移除一个连接项,未等待线程真正移除 */

    void socketPool_remove_item_noWait__IO_2(SocketPool * const pSocketPool, struct SocketPool_item * const pSocketPool_item);

    /* 从pSocketPool中移除一个连接项 */

    void socketPool_remove_item__IO_2(SocketPool * const pSocketPool, struct SocketPool_item * const pSocketPool_item);

    /* 设置pSocketPool_item的连接方式 */

    void socketPool_item_set_connect_mode(struct SocketPool_item * const pSocketPool_item
            , /* 连接模式:
             * 0:默认模式,断开后自动连接;
             * 1:断开后不重连;
             * 2:保持断开模式;
             * 3:设定此值为0,如果是连接模式,则断开一次连接;
             * 4:设定此值为0,如果是连接模式且已收过数据,则断开一次连接;
             * 5:设定此值为0,如果是连接模式且已发过数据,则断开一次连接;
             * 6:设定此值为0,如果是连接模式且已收发过数据,则断开一次连接;
             * 7:设定此值为0,如果是连接模式且已收或发过数据,则断开一次连接;
             */
            const unsigned char connMode);

    /* 
     * 获取一条接收的数据,必须先检查pSocketPool_item->ro.recvList的长度大于0,才能调用此函数;
     * 返回FsString指针,对http返回的数据区在(FsString.lenth+1)按4字节对齐后的位置存储http头的长度.
     */

    __attribute__ ((__deprecated__)) FsString * socketPool_item_recv_get__IO(struct SocketPool_item * const pSocketPool_item);

    /* 
     * 获取一条接收的数据,必须先检查pSocketPool_item->ro.recvList的长度大于0,才能调用此函数;
     * 返回FsObjectBasePthreadSafety指针,对http返回的数据区末尾+1按4字节对齐后的位置存储http头的长度.
     */

    FsObjectBasePthreadSafety * socketPool_item_recv__IO(struct SocketPool_item * const pSocketPool_item);

    /* 
     * 获取所有接收的数据,建议先检查pSocketPool_item->ro.recvList的长度大于1,才调用此函数;
     * 节点指针为FsObjectBasePthreadSafety;
     * 返回链表指针.
     */

    FsObjectList * socketPool_item_recv_all__IO(struct SocketPool_item * const pSocketPool_item);

    /* 清空pSocketPool_item中的所有接收数据  */

    void socketPool_item_recv_clean(struct SocketPool_item * const pSocketPool_item);

    /* 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长 */

    void socketPool_item_send_buffer_pthreadSafety(struct SocketPool_item * pSocketPool_item, const unsigned int sendDataLenth, const char sendData[]);

    /* 添加发送数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长,函数会使用list_成员的一个引用计数,即需要在调用函数前增加引用计数 */

    void socketPool_item_send_list_pthreadSafety__OI_2data(struct SocketPool_item * const pSocketPool_item, /* 要发送的数据,成员全是FsObjectBase,长度不能为0 */FsObjectList * const list_);

    /* 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长 */

    void socketPool_item_send_pthreadSafety__OI_2(struct SocketPool_item * const pSocketPool_item, /* 要发送的数据 */FsObjectBase * const sendData);

    /* 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长 */

    void socketPool_item_send_pthreadSafety__OI_3(struct SocketPool_item * const pSocketPool_item, /* 一次发送的sendData个数 */ unsigned int count, /* 要发送的数据 */FsObjectBase * sendData[]);

    /* 
     * 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长;
     * pSocketPool_item:要添加发送数据到的连接;
     * sendData:要发送的数据.
     */

    void socketPool_item_send_OI_2(struct SocketPool_item * const pSocketPool_item, FsString * const sendData);

    /* 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长 */

    void socketPool_item_send_data_pthreadSafety__OI_3(struct SocketPool_item * const pSocketPool_item, /* 要发送的数据长度 */const unsigned int sendDataLenth, /* 要发送的数据 */ char sendData[]);

    /* 发送错误信息 */

#ifdef FsDebug
    void __socketPool_item_send_errorData_standard(struct SocketPool_item * const pSocketPool_item, /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ const unsigned char headType
            , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31 */ const unsigned char checkMethod
            , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */const unsigned int virtualConnection
            , /* 收到数据的前4字节,发送数据固定为 (head & 0xFFFFFFF0U) | 0x06 */const unsigned int head
            , /* 错误信息,格式参见fs_String_print__IO函数说明 */const char * __restrict __format, ...);
#define socketPool_item_send_errorData_standard(...) __socketPool_item_send_errorData_standard(__VA_ARGS__,~((int)0))
#else
    void socketPool_item_send_errorData_standard(struct SocketPool_item * const pSocketPool_item, /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ const unsigned char headType
            , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31 */ const unsigned char checkMethod
            , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */const unsigned int virtualConnection
            , /* 收到数据的前4字节,发送数据固定为 (head & 0xFFFFFFF0U) | 0x06 */const unsigned int head
            , /* 错误信息,格式参见fs_String_print__IO函数说明 */const char * __restrict __format, ...);
#endif

    /* 清空pSocketPool_item的发送链表,无法清空正在发的数据 */

    void socketPool_item_send_clean_pthreadSafety(struct SocketPool_item * const pSocketPool_item);

    /* 清空pSocketPool_item的发送数据,包含正在发送的数据 */

    void socketPool_item_send_clean_all(struct SocketPool_item * const pSocketPool_item);

#endif

#ifdef SOCKETPOOL_enable


#ifdef __cplusplus
}
#endif
#endif

#endif /* SOCKETPOOL_H */

