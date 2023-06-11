/* 
 * File:   Inflray.h
 * Author: fslib
 *
 * Created on 2021年12月06日, 上午11:19
 */

#if !defined INFLRAY_H && !defined JAVA_IN_C

#define INFLRAY_H
#include "../../PublicTools/publicDefine.h"
#include <pthread.h>
#include <sys/epoll.h>

#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 
     * Inflray相机
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
#define Inflray_get_threadName(___pInflray) (((char*)((___pInflray)+1))+(___pInflray)->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* Inflray客户端条目链表,按指针排序 */
            FsObjectList *__itemListClient_;
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
            /* 外部和内部线程异步通信的链表,结构为Inflray_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 子线程的运行状态:
             *     1-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             */
            unsigned char childRunStatus;
            /* 当前客户端的工作线程数 */
            unsigned short workThreadClient;
            /* 被控制速度的rtsp客户端数量 */
            unsigned int controlSpeedClientCount;
            /* 线程的tid号 */
            pthread_t stid;
            /* 主线程的线程号,由systemThreadTid获得 */
            pthread_t mainTid;
            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* 线程异步通信的链表,结构为Inflray_P_control */
            FsStructList *controlListClient;
            /* 客户端线程对象链表 */
            FsObjectList *threadListClient;
        } p;
    } Inflray;
#define __InflrayLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  channelCountPerThread=%hu\n  itemListClient=%p/%lu\n  controlList=%p/%lu\n  childRunStatus=%hhu\n  workThreadClient=%hu\n  controlSpeedClientCount=%u\
  mainTid=%ld\n  newItemListClient=%p\n  controlListClient=%p\n  threadListClient=%p\n",##__VA_ARGS__\
        ,pInflray,((Inflray*)(pInflray))->rw.runStatus,((Inflray*)(pInflray))->p.inRun,((Inflray*)(pInflray))->p.stid,((Inflray*)(pInflray))->ro._pMonitor,((Inflray*)(pInflray))->ro._externRunStatus,((Inflray*)(pInflray))->ro._parentTid,Inflray_get_threadName(pInflray)\
        ,((Inflray*)(pInflray))->ro._channelCountPerThread,((Inflray*)(pInflray))->ro.__itemListClient_,((Inflray*)(pInflray))->ro.__itemListClient_->nodeCount\
        ,((Inflray*)(pInflray))->rw.__controlList,((Inflray*)(pInflray))->rw.__controlList->nodeCount,((Inflray*)(pInflray))->p.childRunStatus,((Inflray*)(pInflray))->p.workThreadClient\
        ,((Inflray*)(pInflray))->p.controlSpeedClientCount\
        ,((Inflray*)(pInflray))->p.mainTid,((Inflray*)(pInflray))->p.newItemListClient,((Inflray*)(pInflray))->p.controlListClient,((Inflray*)(pInflray))->p.threadListClient)
#define InflrayLog(___loglevel,___format, ...) __InflrayLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define InflrayLog2(___loglevel,___format, ...) __InflrayLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* Inflray客户端项 */
    struct InflrayClient_item {
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         * 无锁的定义;
         */
        FsDelete *_delete;

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
            /* 域名,可为空 */
#define InflrayClient_item_get_domain(___pInflrayClient_item,___nullValue) (!FsNumberIsMax((___pInflrayClient_item)->ro._domain_)?(((char*)((___pInflrayClient_item)+1))+(___pInflrayClient_item)->ro._domain_):(___nullValue))
            unsigned char _domain_;
            /* 视频帧传出时前面预留大小 */
            unsigned int _videoPrefix;
            /* 视频帧传出时后面预留大小 */
            unsigned int _videoSuffix;
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            int (*_cb)(/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
                    , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
                    , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac,4-yuv420p */unsigned char type
                    , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
                    , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
                    , /* 数据的来源ip */unsigned int ip
                    , /* 外部传入的指针 */ void *const p
                    , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 外部的传入指针 */
            void *_externP;
            /* 分发错误信息的对象,用configManager_transfer_out函数分发,为空表示不分发 */
            void *_pTransfer_error;
        } ro;

        struct {
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* 最后一次收数据的时间(CPU的uptime时间),在没有连接时表示上一次尝试的时间 */
            double lastRecvTime;
        } rw;

        struct {
            /* 所属的pInflray对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            Inflray *pInflray;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除,(controlStatus&0x2)-表示是否开始实时流 */
            unsigned char _controlStatus : 2;
            unsigned char : 5;
            /* 此变量标识实时流的开始与结束(实现开始停止不使用busy变量),0-停止,1-开始,底层是否开始由(_controlStatus&0x2)判断 */
            unsigned char _streamStatus_set : 1;
            //________________________________________________________________//
            unsigned char : 7;
            /* 状态:
             * 0:未连接;
             * 1:尝试连接;
             * 2:正在接收视频数据;
             */
            unsigned char status : 2;
            /* socket状态在执行epoll_ctl时修改,&0x1==0x1表示收数据状态,&0x2==0x2表示发数据状态 */
            unsigned char socketStatus : 2;
            /* 接收状态,0-收数据头,1-收数据 */
            unsigned char recvStatus : 2;
            /* 视频通道正在控制速度(0:未控制,1:控制中) */
            unsigned char videoControlSpeed : 1;
            /* 是否在控制播放速度,(0:未控制,1:控制中) */
            unsigned char isControlSpeed : 1;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 帧号,仅使用后三字节 */
            unsigned int frameIndex;
            /* 连接的soket,是否有效使用status判断 */
            int __sock;
            /* 图像的宽高 */
            unsigned int width, height;
            /* 拼接编号 */
            unsigned char stitchIndex;
            /* 接收数据的空间;
             *     第一个int表示数据长度;
             */
            char *__recvData_Hi;
            /* 接收数据的buffer,可为空 */
            FsObjectBase *__pObjectBase_recv;
        } p;
    };
#define __InflrayClient_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p/%p,server(sock=%d/%hhu/%hhu):\"%s%s%hhu.%hhu.%hhu.%hhu%s:%hu\",videoPrefix=%u,videoSuffix=%u,cb=%p,externP=%p,pTransfer_error=%p\
,lastRecvTime=%lf,recvStatus=%hhu,videoControlSpeed=%hhu,isControlSpeed=%hhu,pipe_w=%d,frameIndex=%u,size=%hu/%hu,stitchIndex=%hhu,recvData_Hi=%p,pObjectBase_recv=%p\n",##__VA_ARGS__\
        ,pInflrayClient_item,pInflrayClient_item->p.pInflray,pInflrayClient_item->p.__sock,pInflrayClient_item->p.status,pInflrayClient_item->p._controlStatus\
        ,InflrayClient_item_get_domain(pInflrayClient_item,""),InflrayClient_item_get_domain(pInflrayClient_item,NULL) ? "(" : "",Fs_Network_Ipv4_print(pInflrayClient_item->rw._ipv4)\
        ,InflrayClient_item_get_domain(pInflrayClient_item,NULL) ? ")" : "",pInflrayClient_item->rw._ipv4Port\
        ,pInflrayClient_item->ro._videoPrefix,pInflrayClient_item->ro._videoSuffix,pInflrayClient_item->ro._cb,pInflrayClient_item->ro._externP,pInflrayClient_item->ro._pTransfer_error\
        ,pInflrayClient_item->rw.lastRecvTime,pInflrayClient_item->p.recvStatus,pInflrayClient_item->p.videoControlSpeed,pInflrayClient_item->p.isControlSpeed\
        ,pInflrayClient_item->p.pipe_w,pInflrayClient_item->p.frameIndex,pInflrayClient_item->p.width,pInflrayClient_item->p.height,pInflrayClient_item->p.stitchIndex\
        ,pInflrayClient_item->p.__recvData_Hi,pInflrayClient_item->p.__pObjectBase_recv)  
#define InflrayClient_itemLog(___loglevel,___format, ...) __InflrayClient_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define InflrayClient_itemLog2(___loglevel,___format, ...) __InflrayClient_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Inflray对象;
     * 返回Inflray指针.
     */

    Inflray * inflray_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 每个线程最多处理的通道数 */ const unsigned int channelCountPerThread
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pInflray指向的实例对象 */

    void inflray_delete__OI(Inflray * const pInflray);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void inflray_startThread(Inflray * const pInflray
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void inflray_stopThread_noWait(Inflray * const pInflray);

    /* 结束线程 */

    void inflray_stopThread(Inflray * const pInflray);

    /* 
     * 创建一个新的InflrayClient_item对象;
     * 返回InflrayClient_item指针.
     */

    struct InflrayClient_item * inflrayClient_item_new__IO(/* 域名,可为空 */ const char domain[], /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4
            , /* ipv4地址端口号,以此值判断是否是推流 */const unsigned short ipv4Port
            , /* 视频帧传出时前面预留大小 */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小 */ const unsigned int videoSuffix
            , /* 取流状态,0-停止取流,1-开始取流 */const unsigned char streamControl
            , /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */ int (*const cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
            , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
            , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac,4-yuv420p */unsigned char type
            , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
            , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
            , /* 数据的来源ip */unsigned int ip
            , /* 外部传入的指针 */ void *const p
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer), /* 外部传入的指针 */void *externP
            , /* 分发错误信息的对象,用configManager_transfer_out函数分发,为空表示不分发 */ void *const pTransfer_error);

    /* 删除pInflrayClient_item指向的实例对象 */

    void inflrayClient_item_delete__OI(struct InflrayClient_item * const pInflrayClient_item);

    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */

    void inflrayClient_item_streamControl_pthreadSafety(struct InflrayClient_item * const pInflrayClient_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pInflray中添加一个Inflray客户端条目 */

    void inflrayClient_add_item__OI_2(Inflray * const pInflray, struct InflrayClient_item * const pInflrayClient_item);

    /* 向pInflray中添加一个Inflray客户端条目,未等待线程真正移除 */

    void inflrayClient_remove_item_noWait__IO_2(Inflray * const pInflray, struct InflrayClient_item * const pInflrayClient_item);

    /* 从pInflray中移除一个Inflray客户端条目 */

    void inflrayClient_remove_item__IO_2(Inflray * const pInflray, struct InflrayClient_item * const pInflrayClient_item);
     


#ifdef __cplusplus
}
#endif

#endif /* INFLRAY_H */

