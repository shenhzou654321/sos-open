/* 
 * File:   Rtmp.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined RTMP_H && !defined JAVA_IN_C
#include "../../PublicTools/publicDefine.h"
#define RTMP_H
#include "../../PublicTools/Fs/CycleList.h"
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/String.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Fs/Network.h"
/*
 * 定义模块是否依赖ConfigManager
 * 不依赖ConfigManager时,内部信息无法使用ConfigManager输出
 */
#define Rtmp_ConfigManager
#define Rtmp_stats // 开启统计
#define Rtmp_Debug
//#define RtmpClientDebug
#define RtmpServerDebug
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* Rtmp客户端和服务器,支持客户端和服务器项在不同线程中热插拔 */
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
#define Rtmp_get_threadName(pRtmp) (((char*)((pRtmp)+1))+(pRtmp)->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* 配置管理对象,为空表示不支持与之相关的业务 */
            void *_pConfigManager;
            /* rtmp客户端条目链表,按指针排序 */
            FsObjectList *__itemListClient_;
            /* rtmp本地服务器条目链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__itemListServer;
            /* 集群的通道链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__clusterItemListServer;
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
            /* ipv4地址端口号,为0表示不监听仅使用客户端拉流功能 */
            unsigned short _ipv4Port;
            /* 监听有ipv4地址,为0表示监听所有地址 */
            unsigned int _ipv4;
            /* 实时视频缓存的超时时间 */
            float _realBufferTimeOut;
            /* 外部和内部线程异步通信的链表,结构为Rtmp_P_control */
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
            /* 当前服务端的工作线程数 */
            unsigned short workThreadServer;
            /* 线程的tid号 */
            pthread_t stid;
            /* 主线程的线程号,由systemThreadTid获得 */
            pthread_t mainTid;
            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* rtmp推送连接,储存暂不需要流的连接,使用newItemListClient的互斥锁 */
            FsObjectList *clientListPull__newItemListClient;
            /* 线程异步通信的链表,结构为通道指针+参数(首字节低3位建议为:0-client添加,1-client移除,2-client开始,3-client停止,4-server添加,5-server移除),大小固定为两个指针大小 */
            FsStructList *controlListClient;
            /* 客户端线程对象链表(可接受新任务) */
            FsObjectList *threadListClient;
            /* 新加入的服务通道条目链表 */
            FsObjectList *newItemListServer;
            //                   /* 本地的服务端通道链表,按指针排序 */
            //            FsObjectList *localItemList;
            /* 本地的服务端通道数量 */
            unsigned long localItemListCount;
            /* 线程异步通信的链表,结构为通道指针+参数(首字节低3位建议为:0-client添加,1-client移除,2-client开始,3-client停止,4-server添加,5-server移除,6-重置参数),大小固定为两个指针大小 */
            FsStructList *controlListServer;
            /* 服务端端线程对象链表(可接受新任务) */
            FsObjectList *threadListServer;
        } p;
    } Rtmp;
#define __RtmpLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  channelCountPerThread=%u\n  pConfigManager=%p\n  itemListClient=%p/%lu\n  itemListServer=%p/%lu\n  clusterItemListServer=%p/%lu\n  ipv4:\"%hhu.%hhu.%hhu.%hhu:%hu\"\n  realBufferTimeOut=%f\n",##__VA_ARGS__\
        ,pRtmp,((Rtmp*)(pRtmp))->rw.runStatus,((Rtmp*)(pRtmp))->p.stid\
        ,((Rtmp*)(pRtmp))->ro._pMonitor,((Rtmp*)(pRtmp))->ro._externRunStatus,((Rtmp*)(pRtmp))->ro._parentTid,((Rtmp*)(pRtmp))->ro.__pipe[0], ((Rtmp*)(pRtmp))->ro.__pipe[1],Rtmp_get_threadName(pRtmp)\
        ,((Rtmp*)(pRtmp))->ro._channelCountPerThread,((Rtmp*)(pRtmp))->ro._pConfigManager ,((Rtmp*)(pRtmp))->ro.__itemListClient_,((Rtmp*)(pRtmp))->ro.__itemListClient_->nodeCount\
        ,((Rtmp*)(pRtmp))->ro.__itemListServer,((Rtmp*)(pRtmp))->ro.__itemListServer->nodeCount, ((Rtmp*)(pRtmp))->ro.__clusterItemListServer,((Rtmp*)(pRtmp))->ro.__clusterItemListServer->nodeCount\
        ,Fs_Network_Ipv4_print(((Rtmp*)(pRtmp))->rw._ipv4),((Rtmp*)(pRtmp))->rw._ipv4Port,((Rtmp*)(pRtmp))->rw._realBufferTimeOut)
#define RtmpLog(___loglevel,___format, ...) __RtmpLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtmpLog2(___loglevel,___format, ...) __RtmpLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    struct RtmpClient_P_package;

    /* Rtmp客户端项 */
    struct RtmpClient_item {
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
            /* 视频帧传出时前面预留大小 */
            unsigned int _videoPrefix;
            /* 视频帧传出时后面预留大小 */
            unsigned int _videoSuffix;
            /* 音频帧传出时前面预留大小 */
            unsigned int _audioPrefix;
            /* 是否是推流客户端,0-不是推流,1-推流 */
            unsigned char _isPull : 1;
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            int (*_cb)(/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
                    , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
                    , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac */unsigned char type
                    , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
                    , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
                    , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
                    , /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void *const p);
            /* 外部的传入指针 */
            void *_externP;
        } ro;

        struct {
            /* ipv4地址,为0表示无效,在不是推流时不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* 最后一次收数据的时间(CPU的uptime时间),在没有连接时表示上一次尝试的时间 */
            double lastRecvTime;
            /* 最后一次发送心跳的时间 */
            double lastSendHeartbeatTime;
        } rw;

        struct {
            /* 所属的pRtmp对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            Rtmp *pRtmp;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除,(controlStatus&0x2)-表示是否开始实时流 */
            unsigned char _controlStatus : 2;
            unsigned char : 5;
            /* 此变量标识实时流的开始与结束(实现开始停止不使用busy变量),0-停止,1-开始,底层是否开始由(_controlStatus&0x2)判断 */
            unsigned char _streamStatus_set : 1;
            //________________________________________________________________//
            /* 状态:
             * 0:未连接;
             * 1:尝试连接发送C0C1;
             * 2:接收S0S1发送C2和connect;
             * 3:接收S2数据;
             * 4:接收onBWDone数据发送_checkbw;
             * 5:接收_result(_result的编号应为发送需要-1)数据发送setBuffer;
             * 6:接收数据流;
             */
            unsigned char status;
            /* 是否丢失数据,(1:丢失,0:未丢失) */
            unsigned char lostData : 1;
            /* socket状态在执行epoll_ctl时修改,&0x1==0x1表示收数据状态,&0x2==0x2表示发数据状态 */
            unsigned char socketStatus : 2;
            /* 接收状态,0-收数据头,1-收忽略数据,2-收h264视频数据 */
            unsigned char recvStatus : 2;
            /* 视频通道正在控制速度(0:未控制,1:控制中) */
            unsigned char videoControlSpeed : 1;
            /* 是否在控制播放速度,(0:未控制,1:控制中) */
            unsigned char isControlSpeed : 1;
            /* 本地chunk大小 */
            unsigned short _chunkSizeLocal;
            /* 对端的chunkSize */
            unsigned int _chunkSize;
            /* 发送命令的序号,只使用3字节,RtmpClient_item与RtmpServer_P_client中的sendIndex定义相同,在发送cs为3,4,8时需要连续编码 */
            unsigned int sendIndex;
            /* 对csid为0,1,2的缓存8字节 */
            unsigned char chunkHeadBuffer[7][8];
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 帧号,仅使用后三字节 */
            unsigned int frameIndex_24bits;
            /* 连接的soket,负数表示无效 */
            int __sock;
            /* 发送的数据 */
            struct RtmpClient_P_package *__pSend;
            /* 接收数据的空间;
             *     第一个int表示数据长度;
             *     第二个int表示数据区相对于__pObjectBase_recv->data偏移量
             *     第三个int表示已收数据区的长度;
             *     第四个int表示已检查并去除0xC?标记的存于此变量中当前chunk msg的长度;
             *     在没有数据可收时,但数据长度又不足解析时把数据暂存的空间,为空表示无暂存数据 
             */
            char *__recvData_Hiiii;
            /* 接收数据的buffer,可为空 */
            FsObjectBase *__pObjectBase_recv;
            /* 视频的编码信息,第一个int表示数据长度,对h264就是储存sps和pps */
            unsigned char *__videoCode_Hi;
            /* 域名,可为空 */
            char *_domain;
            /* url地址,可为空 */
            char *_url;
            /* 重定向的服务端信息,为空表示未重定向,对推流的客户端用此指针记录有相同url的下一个客户端 */
            void *__serverInfoRedirect;
        } p;
    };
#define __RtmpClient_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,server(sock=%d/%hhu):\"%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",pSend=%p/%u/%u,lastRecvTime=%lf,chunkSize=%hu/%u,sendIndex=%u,videoCode_Hi=%p\n",##__VA_ARGS__\
        ,pRtmpClient_item,pRtmpClient_item->p.__sock,pRtmpClient_item->p.status\
        ,FsStrprint(pRtmpClient_item->p._domain,""),pRtmpClient_item->p._domain ? "(" : "",Fs_Network_Ipv4_print(pRtmpClient_item->rw._ipv4)\
        ,pRtmpClient_item->p._domain ? ")" : "",pRtmpClient_item->rw._ipv4Port,FsStrprint(pRtmpClient_item->p._url,"")\
        ,pRtmpClient_item->p.__pSend,pRtmpClient_item->p.__pSend?pRtmpClient_item->p.__pSend->sendPos:0, pRtmpClient_item->p.__pSend?pRtmpClient_item->p.__pSend->dataLen:0\
        ,pRtmpClient_item->rw.lastRecvTime,pRtmpClient_item->p._chunkSizeLocal,pRtmpClient_item->p._chunkSize,pRtmpClient_item->p.sendIndex,pRtmpClient_item->p.__videoCode_Hi)  
#define RtmpClient_itemLog(___loglevel,___format, ...) __RtmpClient_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtmpClient_itemLog2(___loglevel,___format, ...) __RtmpClient_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 点播的客户端链表结构 */
    struct RtmpServer_item_DemandClient {
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

        /* rw放前面,以便RTSP,gb28181的回放共用 */
        struct {
#ifndef __DemandClient_rw 
            unsigned int allMask;
            /* 外部标记是否出错,0-表示正常,1-表示已结束,-1表示出错 */
            signed char error;
            /* 数据发送类型(0--正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送,其他值-无效) */
            unsigned char sendType;
            /* 内部的发送缓存值 */
            unsigned char sendBufferCountMaxIn;
            /* 外部的发送缓存值 */
            unsigned char sendBufferCountMaxOut;
            /* 外部指针 */
            void *externP;
            /* 删除externP的函数指针,为空表示不删除 */
            void (*deleteExternP) (void *externP);
            /* 时长,为0表示未知,用clock格式一般用于按时点播类应用,不为0为文件点播用npt格式 */
            double duration;
            /* 跳转到的时间,为npt格式时表示距离文件开始的时间,为clock时表示1970年1月1号以来经过的秒数,在此值不为0时,调用者应改变视频流的的位置并把此值置0 */
            double jumpTime;
#endif
        } rw;

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
            /* 数据链表 */
            FsObjectList *__dataList;
            /* 数据的索引 */
            unsigned short index[32];
            /* 发送数据时rtp里面的timestamp值 */
            unsigned int timestamp[32];
            /* url参数 */
            FsUrlParameter *__pUrlParameter;
        } ro;
    };

    /* RTMP服务器通道项 */
    struct RtmpServer_item {

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
            /* 网络地址,为空意义不明确 */
            char *_url;
            /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */
            char _modelType;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            unsigned char : 5;
            /* 是否有新的实时客户端,1-有,0-没有 */
            unsigned char newRealClient : 1;
            unsigned char : 7;
            /* 是否有推流客户端等待发送dataInfo中,设置此标记时需要标记newRealClient为1 */
            unsigned char hasPushDoSendDataInfo : 1;
            /* 主机ip,用于集群,0表示本机,低四位表示ip */
            unsigned int _ipv4;
        } r;

        struct {
#define RtmpServer_item_ClusterLen (FsPointer_to_long(&((struct RtmpServer_item*) 0)->ro._cb_client)+sizeof(void*)*3)
#define RtmpServer_item_Cluster_callTimes(pRtmpServer_item) (*(unsigned int*)fs_pointer_to_void_pointer((void*)(&(pRtmpServer_item)->ro._cb_client)+0))
#define RtmpServer_item_Cluster_next(pRtmpServer_item) (*(struct RtmpServer_item **)fs_pointer_to_void_pointer((void*)((&(pRtmpServer_item)->ro._cb_client)+1)))
#define RtmpServer_item_Cluster_pRtmpServer(pRtmpServer_item) (*(Rtmp**)fs_pointer_to_void_pointer((void*)((&(pRtmpServer_item)->ro._cb_client)+2)))
#define RtmpServer_item_next(pRtmpServer_item) (0==(pRtmpServer_item)->r._ipv4?(pRtmpServer_item)->p.next:RtmpServer_item_Cluster_next(pRtmpServer_item))
#define RtmpServer_item_pRtmpServer(pRtmpServer_item) (0==(pRtmpServer_item)->r._ipv4?(pRtmpServer_item)->p.pRtmp:RtmpServer_item_Cluster_pRtmpServer(pRtmpServer_item))
            /* 有客户端时的回掉函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*_cb_client)(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p);
            /* 有客户端时的回掉函数关联的指针 */
            void *_cb_clientP;
            /* 实时视频数据链表,为空表示不支持实时视频 */
            FsObjectList *__realVideoList;
            /* 连接的实时客户端链表,只有当实时视频数据链表不为空时才有效 */
            FsCycleList __realClientList;
#define  Rtmp_mutex_realClientList(___pRtmp) (___pRtmp)->ro.__mutex_realClientList
            /* 实时客户端的互斥锁,只有当实时视频数据链表不为空时才有效 */
            pthread_mutex_t __mutex_realClientList;
            /* 实时视频数据的索引,只有当实时视频数据链表不为空时才有效 */
            unsigned char *_realVideoIndex;
            /* 发送数据时rtp里面的timestamp值,只有当实时视频数据链表不为空时才有效 */
            unsigned long long *_timestamp;
            /* 实时推流客户端,只有当实时视频数据链表不为空时才有效 */
            FsObjectList *__realPushClient_;
        } ro;

        struct {
            /* 
             * 点播客户端链表,仅当有历史数据功能时有效,实时视频数据链表不为空时,此链表储存历史点播的客户端,否则储存文件点播客户端,也可能不支持点播;
             * 储存类型是继承FsCycleListNode的客户端连接对象,外部使用宏RtmpServer_DemandClient_get(__demandClientList.node->next)可取得第一个(struct RtmpServer_item_DemandClient)类型的点播客户端指针;
             */
            FsCycleList __demandClientList;
            /* 历史客户端的互斥锁,仅当有历史数据功能时有效 */
            pthread_mutex_t __mutex_demandClientList;
        } rw;

        struct {
            /* 上一次实时视频的数量 */
            unsigned int realVideoListCountLast;
            /* 实时数据的掩码 */
            unsigned int mask_realAll;
            /* 实时数据中不是所有帧都为关键帧的掩码 */
            unsigned int mask_realkey;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程 */
            int pipe_w;
            /* 当前所有准备开始实时流的客户端掩码之和 */
            unsigned int newRealStreamMask;
            /* 实时转发时要发送的信息 */
            char *__realVideosendInfo;
            /* 统计实时流中的帧率,结果应更接近低频通道 */
            float _realRate_low;
            /* 用于统计realRate_low的辅助变量 */
            float realRate_low_last;
            /* 具有相同url的下一个RtmpServer通道 */
            struct RtmpServer_item *next;
            /* 所属的Rtmp对象 */
            Rtmp *pRtmp;
        } p;
    };
#define __RtmpServer_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,url:\"%s\",addr:\"%hhu.%hhu.%hhu.%hhu\".\n",##__VA_ARGS__\
        ,pRtmpServer_item,pRtmpServer_item->r._url, Fs_Network_Ipv4_print(pRtmpServer_item->r._ipv4))
#define RtmpServer_itemLog(___loglevel,___format, ...) __RtmpServer_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtmpServer_itemLog2(___loglevel,___format, ...) __RtmpServer_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* rtmp服务端连接socket */
    struct RtmpServer_P_client {
        /* 继承FsCycleListNode */
        struct FsCycleListNode cycleListNode;
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         *     实时客户端指pClient->pRtmpServer_item->ro.__mutex_realClientList
         *     历史客户端指pClient->pRtmpServer_item->ro.__mutex_demandClientList
         */
        FsDelete *_delete;
        /* 点播结构指针,为空表示不为点播客户端,状态为4时,指向推流的url,在小于4时不为空储存连接时的url地址,如rtmp://192.168.4.57/live */
        struct RtspServer_item_DemandClient *pDemandClient;
        /* 状态rtmp:0-接收C0发送S0S1,如收到C0C1则发送S0S1S2;1-接收C1发送S2;2-接收C2,3-查找连接所属通道中,4-推流缓存客户端,5-已确定通道,等待加入到工作线程中(回调后状态变为6),6-等待工作线程响应中,7-推流预处理中,8-等待播放数据,9-播放中 */
        unsigned char status : 4;
        /* socket的检查状态,&0x1表示读,&0x2表示写 */
        unsigned char socketCheckStatus : 2;
        /* 握手方式,0-普通握手,1-代表对端C1中的版本号为0x80,0x00,0x07,0x02 */
        unsigned char handshake : 1;
        /* 客户端的ipv4地址 */
        unsigned int ipv4;
        /* 客户端的ipv4地址端口号 */
        unsigned int ipv4Port;
        /* 连接socket对象 */
        int sock;
        /* 超时时间,对推流客户端来说为下一次发送的数据的时间,对Server_item_push来说为下一次尝试连接的时间,因发送量大,可使用发送时间判断连接通断,否则应使用接收时间判断 */
        double timeout;
        /* 所属的rtmp服务项,在未确定是属于何通道时储存rtmp服务对象 */
        struct RtmpServer_item *pRtmpServer_item;
        /* 发送的数据链表,内容为继承FsObjectSingle的struct Rtmp_P_package指针,不能为空,发送的数据链表 */
        FsObjectList *list_;
        /* 当前使用发送包,可能为空 */
        struct Rtmp_P_package *pSendData_tcp;
#ifdef Rtmp_stats
        /* 已成功发送的数据量 */
        unsigned long long hasSendByte;
        /* 已接收的数据量 */
        unsigned long long hasRecvByte;
#endif
        /* 播放掩码 */
        unsigned int playMask;
        /* 数据发送的位置,需要初始化为0 */
        unsigned int pSendData_tcpSendPos;
        /* 发送命令的序号,只使用3字节,RtmpClient_item与RtmpServer_P_client中的sendIndex定义相同,在发送cs为3,4,8时需要连续编码 */
        unsigned int sendIndex;
        /* 对csid为0,1,2的缓存8字节 */
        unsigned char chunkHeadBuffer[Fs_Array_len(((struct RtmpClient_item*) 0)->p.chunkHeadBuffer)][8];
        /* 对端chunk大小 */
        unsigned int chunkSize;
        /* 本地chunk大小 */
        unsigned short chunkSizeLocal;
        /* 接收数据的空间,第一个int大小节表示数据长度,第二和第三int固定为0为同pRtmpClient_item兼容(未初始化),第四个int表示已检查并去除0xC?标记的存于此变量中当前chunk msg的长度,在没有数据可收时,但数据长度又不足解析时把数据暂存的空间,为空表示无暂存数据 */
        char *__recvData_Hiiii;
#define RtmpServer_video_force0 // 强制视频帧时间从0开始
#ifdef RtmpServer_video_force0
        /* 流开始的参考时间,在流开始时初始化 */
        double baseTime;
#endif
    };
#define __RtmpServer_P_clientLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,FsPrintfIndex,___format",p=%p,client(sock=%d/%hhu/%hhu):\"%hhu.%hhu.%hhu.%hhu:%hu\",url:\"%s\"/\"%s\",pSendData_tcpSendPos=%u/%u/%hhu/%hhu,recvData_Hiiii=%p/%u/%u\n",##__VA_ARGS__\
        ,pClient,pClient->sock, pClient->status,pClient->handshake\
        ,Fs_Network_Ipv4_print(pClient->ipv4),pClient->ipv4Port\
        ,pClient->status<4&&pClient->pDemandClient!=NULL?(char*)pClient->pDemandClient:""\
        ,pClient->status >=5 ? (pClient->pRtmpServer_item->r._url ? pClient->pRtmpServer_item->r._url : "") : ""\
        ,pClient->pSendData_tcpSendPos, pClient->pSendData_tcp ? pClient->pSendData_tcp->dataLen : 0, pClient->pSendData_tcp ? pClient->pSendData_tcp->dataOffset : 0, pClient->pSendData_tcp ? pClient->pSendData_tcp->type : 0\
        ,pClient->__recvData_Hiiii, pClient->__recvData_Hiiii?((unsigned int*)pClient->__recvData_Hiiii)[0]:0,pClient->__recvData_Hiiii?((unsigned int*)pClient->__recvData_Hiiii)[3]:0)
#define RtmpServer_P_clientLog(___loglevel,___format, ...) __RtmpServer_P_clientLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtmpServer_P_clientLog2(___loglevel,___format, ...) __RtmpServer_P_clientLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    /* 
     * 创建一个新的Rtmp对象;
     * 返回Rtmp指针.
     */
    Rtmp *rtmp_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号,为0表示不监听不支支持推流 */const unsigned short ipv4Port
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut
            , /* 配置管理对象,为空表示不支持与之相关的业务 */ void *const pConfigManager);
    /* 设置pRtmp的参数(需要线程同步的部分参数) */
    void rtmp_set_parameter_thread_pthreadSafety(Rtmp * const pRtmp, /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port);
    /* 设置pRtmp的参数(不需要线程同步的部分参数) */
    void rtmp_set_parameter_pthreadSafety(Rtmp * const pRtmp, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut);
    /* 删除pRtmp指向的实例对象 */
    void rtmp_delete__OI(Rtmp * const pRtmp);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void rtmp_startThread(Rtmp * const pRtmp
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程,只发送信号,不回收线程资源 */

    void rtmp_stopThread_noWait(Rtmp * const pRtmp);

    /* 结束线程 */
    void rtmp_stopThread(Rtmp * const pRtmp);
    /* 
     * 创建一个新的RtmpClient_item对象;
     * 返回RtmpClient_item指针.
     */
    struct RtmpClient_item * rtmpClient_item_new__IO(/* 域名,可为空 */ const char domain[], /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4
            , /* ipv4地址端口号,此值为0表示是推流 */const unsigned short ipv4Port, /* url地址 */const char url[]
            , /* 视频帧传出时前面预留大小 */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小 */ const unsigned int videoSuffix, /* 音频帧传出时前面预留大小 */ const unsigned int audioPrefix
            , /* 取流状态,0-停止取流,1-开始取流 */const unsigned char streamControl
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            , int (*const cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
            , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
            , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac */unsigned char type
            , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
            , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
            , /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void *const p)
            , /* 外部传入的指针 */void *externP);
    /* 
     * 创建一个新的RtmpClient_item对象;
     * 成功返回RtmpClient_item指针;
     * 失败返回NULL.
     */
    struct RtmpClient_item *rtmpClient_item_new_byURL__IO(/* url地址,如rtmp://127.0.0.1:554/video */const char url[]
            , /* 视频帧传出时前面预留大小 */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小 */ const unsigned int videoSuffix, /* 音频帧传出时前面预留大小 */ const unsigned int audioPrefix
            , /* 取流状态,0-停止取流,1-开始取流 */const unsigned char streamControl
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            , int (*const cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
            , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
            , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac */unsigned char type
            , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
            , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
            , /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void *const p)
            , /* 外部传入的指针 */void *externP);
    /* 删除pRtmpClient_item指向的实例对象 */
    void rtmpClient_item_delete__OI(struct RtmpClient_item * const pRtmpClient_item);
    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */
    void rtmpClient_item_streamControl_pthreadSafety(struct RtmpClient_item * const pRtmpClient_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
    /* 向pRtmp中添加一个rtmp客户端条目 */
    void rtmpClient_add_item__OI_2(Rtmp * const pRtmp, struct RtmpClient_item * const pRtmpClient_item);
    /* 从pRtmp中移除一个rtmp客户端条目,未等待线程真正移除 */
    void rtmpClient_remove_item_noWait__IO_2(Rtmp * const pRtmp, struct RtmpClient_item * const pRtmpClient_item);
    /* 从pRtmp中移除一个rtmp客户端条目 */
    void rtmpClient_remove_item__IO_2(Rtmp * const pRtmp, struct RtmpClient_item * const pRtmpClient_item);
    /* 
     * 创建一个新的RtmpServer_item对象;
     * 返回RtmpServer_item指针.
     */
    struct RtmpServer_item * rtmpServer_item_new__IO(/* 标识符,不能为空 */ const char url[], /* 主机ip,为0表示本地主机,非0表示远程主机,之后的数据仅对此值为0时有效 */const unsigned int ipv4,
            /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */const char modelType,
            /* 有客户端时的回掉函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*const cb_client) (/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p),
            /* 有客户端时的回掉函数关联的指针 */void *const cb_clientP);
    /* 删除pRtmpServer_item指向的实例对象 */
    void rtmpServer_item_delete__OI(struct RtmpServer_item * const pRtmpServer_item);
    /* 向pRtmp中添加一个rtmp服务端条目 */
    void rtmpServer_add_item__OI_2(Rtmp * const pRtmp, struct RtmpServer_item * const pRtmpServer_item);
    /* 从pRtmp中移除一个rtmp服务端条目 */
    void rtmpServer_remove_item__IO_2(Rtmp * const pRtmp, struct RtmpServer_item * const pRtmpServer_item);
    /* 添加数据到pRtmpServer_item中 */
    void rtmpServer_item_add_frame(struct RtmpServer_item * const pRtmpServer_item, /* 要添加的数据,只支持VideoH264Object */ void *const pObject);
    /* 添加数据到pRtmpServer_item中 */
    void rtmpServer_item_add_frame__OI_2(struct RtmpServer_item * const pRtmpServer_item, /* 要添加的数据,只支持VideoH264Object */ void *const pObject);

#ifdef __cplusplus
}
#endif
#endif /* RTMP_H */

