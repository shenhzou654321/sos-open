/* 
 * File:   Rtmp.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef RTMP_H
#define	RTMP_H
#include "../publicDefine.h"
#include "../Fs/String.h"
#include "../Fs/StructList.h"
#include "../Fs/LinkList.h"
//#define RtmpClientDebug
#define RtmpServerDebug
#ifdef	__cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* Rtmp客户端和服务器,支持客户端和服务器项在不同线程中热插拔 */
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
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 线程是否在运行(0:没有运行,1:在运行) */
            unsigned char inRun : 1;
            /* 管道,用于添加和删除文件或发送数据 */
            int __pipe[2];
            /* 监听有ipv4地址,为0表示监听所有地址 */
            unsigned int _ipv4;
            /* ipv4地址端口号,为0表示不监听仅使用客户端拉流功能 */
            unsigned short _ipv4Port;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* rtmp客户端条目链表 */
            FsObjectList *__itemListClient;
            /* rtmp本地服务器条目链表 */
            FsObjectList *__itemListServer;
            /* 集群的通道链表 */
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
            unsigned char nice;
            /* 外部和内部线程异步通信的链表,结构为通道指针+参数(首字节低3位建议为:0-client添加,1-client移除,2-client开始,3-client停止,4-server添加,5-server移除),大小固定为两个指针大小 */
            FsStructList *__controlList;
        } rw;

        struct {
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
            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* 线程异步通信的链表,结构为通道指针+参数(首字节低3位建议为:0-client添加,1-client移除,2-client开始,3-client停止,4-server添加,5-server移除),大小固定为两个指针大小 */
            FsStructList *controlListClient;
            /* 客户端线程对象链表 */
            FsObjectList *threadListClient;
            /* 新加入的服务通道条目链表 */
            FsObjectList *newItemListServer;
            /* 线程异步通信的链表,结构为通道指针+参数(首字节低3位建议为:0-client添加,1-client移除,2-client开始,3-client停止,4-server添加,5-server移除,6-重置参数),大小固定为两个指针大小 */
            FsStructList *controlListServer;
            /* 服务端端线程对象链表 */
            FsObjectList *threadListServer;
        } p;
    } Rtmp;

    /* Rtmp客户端项 */
    struct RtmpClient_item {

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
            /* 域名,可为空 */
            char *_domain;
            /* url地址,可为空 */
            char *_url;
            /* 视频帧传出时前面预留大小 */
            unsigned int _videoPrefix;
            /* 视频帧传出时后面预留大小 */
            unsigned int _videoSuffix;
            /* 音频帧传出时前面预留大小 */
            unsigned int _audioPrefix;
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            signed char (*_cb)(/* *data指向内存的实际大小,做数据交换时必须设定此值 */unsigned int *const dataMemLenth,
                    /* 数据,是用fs_memery_malloc分配的内存,传二级指针方便实现数据交换 */ char * * const fsData, /* 收到的数据长度 */const unsigned int dataLenth,
                    /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */const double realTime,
                    /* 帧号,最大值只能到3个字节 */const unsigned int frameIndex, /* 是否是关键帧(-1:无法判断;0;不是;1:是) */signed char isKeyFrame, /* 帧率(小于0.001表示未知) */ float frameRate,
                    /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void *const p);
            /* 外部的传入指针 */
            void *_externP;
            /* 连接的soket,不能根据此值判断连接是否有效 */
            int __sock;
            /* 要发送的总长度 */
            unsigned int sendLenAll;
            /* 已发送的长度 */
            unsigned int sendLen;
            /* 已接收的长度 */
            unsigned int recvLen;
            /* 状态(双数发数据,单数收数据):
             * 0:未连接;
             * 1:尝试连接发送C0C1;
             * 2:接收S0S1发送C2;
             * 3:接收S2数据发送connect;
             * 4:接收onBWDone数据发送_checkbw;
             * 5:接收_result数据发送setBuffer;
             * 6:接收数据流;
             */
            unsigned char status;
            /* 接受状态,0-收数据头,1-收数据,2-收忽略数据 */
            unsigned char recvStatus : 2;
            /* 是否丢失数据,(1:丢失,0:未丢失) */
            unsigned char lostData : 1;
            /* socket错误检查,0-表示不能容错,1-表示可以容错 */
            unsigned char socketErrorCheck : 1;
            /* 是否在控制播放速度,(0:未控制,1:控制中) */
            unsigned char isControlSpeed;
            /* 帧号,仅使用后三字节 */
            unsigned int frameIndex;
        } ro;

        struct {
            /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* ipv4地址端口号,以此值判断是否是推流 */
            unsigned short _ipv4Port;
            /* 最后一次收发数据的时间(CPU的uptime时间),在没有连接时表示上一次常识的时间*/
            double lastSendOrRecvTime;
            /* 最后一次发送心跳的时间 */
            double lastSendHeartbeatTime;
        } rw;

        struct {
            /* 是否繁忙,1-繁忙,2-空闲 */
            unsigned char busy;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除,(controlStatus&0x2)-表示是否开始实时流 */
            unsigned char _controlStatus;
            /* 所属的rtmp对象 */
            Rtmp *pRtmp;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步 */
            int pipe_w;
            /* 对象要使用的空间大小 */
            unsigned int _bufferlenth;
            /* 对象要使用的空间,一般用于接收数据 */
            char *__buffer;
            /* 发送数据指向的指针 */
            char *_pSenddata;
            /* 视频的编码信息,对h264就是储存sps和pps */
            unsigned char *__videoCode;
            /* 视频的编码信息长度 */
            unsigned short videoCodeLen;
            /* 剩下的还要接收的长度 */
            unsigned int recvLenRemain;
            /* 对端chunk大小 */
            unsigned int chunkSize;
            /* 当前chunk还可容纳的大小 */
            unsigned int chunkLenRemain;
            /* rtmp头 */
            unsigned char head[28];
            /* 1字节头(0和0xFF都可能为无效值)+3字节长度,小端模式 */
            unsigned int lasthead;
            /* 接收状态,0-收数据头,1-收忽略数据,2-收h264视频数据 */
            unsigned char recvStatus : 2;
        } p;
    };

    /* RTMP服务器通道项 */
    struct RtmpServer_item {

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
            /* 网络地址,为空意义不明确 */
            char *_url;
            /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */
            char _modelType;
            /* 主机ip,用于集群,0表示本机,低四位表示ip */
            unsigned int _ipv4;
            /* 有客户端时的回掉函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*cb_client)(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p);
            /* 有客户端时的回掉函数关联的指针 */
            void *cb_clientP;
            /* 实时视频数据链表,为空表示不支持实时视频 */
            FsObjectList *__realVideoList;
            /* 连接的实时客户端链表,只有当实时视频数据链表不为空时才有效 */
            FsLinkList __realClientList;
            /* 实时客户端的互斥锁,只有当实时视频数据链表不为空时才有效 */
            pthread_mutex_t __mutex_realClientList;
            /* 实时视频数据的索引,只有当实时视频数据链表不为空时才有效 */
            unsigned char *_realVideoIndex;
            /* 发送数据时rtp里面的timestamp值,只有当实时视频数据链表不为空时才有效 */
            unsigned long long *_timestamp;
        } ro;

        struct {
            /* 点播客户端链表,实时视频数据链表不为空时,此链表储存历史点播的客户端,否则储存文件点播客户端,也可能不支持点播 */
            FsObjectList *__demandClientList;
        } rw;

        struct {
            /* 是否繁忙,1-繁忙,0-空闲 */
            unsigned char busy : 1;
            /* 是否有新的实时客户端,1-有,0-没有 */
            unsigned char newRealClient : 1;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus;
            /* 上一次实时视频的数量 */
            unsigned int realVideoListCountLast;
            /* 实时数据的掩码 */
            unsigned int mask_realAll;
            /* 实时数据中不是所有帧都为关键帧的掩码 */
            unsigned int mask_realkey;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程 */
            int pipe_w;
            /* 实时转发时要发送的信息 */
            char *__realVideosendInfo;
            /* 具有相同url的下一个RtmpServer通道 */
            struct RtmpServer_item *next;
            /* 所属的Rtmp对象 */
            Rtmp *pRtmp;
        } p;
    };
    /* 
     * 创建一个新的Rtmp对象;
     * 返回Rtmp指针.
     */
    Rtmp *rtmp_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
            /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号,为0表示不监听不支支持推流 */const unsigned short ipv4Port,
            /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread);
    /* 设置pRtmp的参数 */
    void rtmp_set_parameter_pthreadSafety(Rtmp * const pRtmp, /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port);
    /* 删除pRtmp指向的实例对象 */
    void rtmp_delete__OI(Rtmp * const pRtmp);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void rtmp_startThread(Rtmp * const pRtmp,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void rtmp_stopThread(Rtmp * const pRtmp);
    /* 向pRtmp中添加一个rtmp客户端条目 */
    void rtmpClient_add_item__OI_2(Rtmp * const pRtmp, struct RtmpClient_item * const pRtmpClient_item);
    /* 从pRtmp中移除一个rtmp客户端条目 */
    void rtmpClient_remove_item__IO_2(Rtmp * const pRtmp, struct RtmpClient_item * const pRtmpClient_item);
    /* 
     * 创建一个新的RtmpClient_item对象;
     * 返回RtmpClient_item指针.
     */
    struct RtmpClient_item * rtmpClient_item_new__IO(/* 域名,可为空 */ const char domain[], /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4,
            /* ipv4地址端口号 */const unsigned short ipv4Port,
            /* url地址 */const char url[], /* 视频帧传出时前面预留大小 */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小 */ const unsigned int videoSuffix, /* 音频帧传出时前面预留大小 */ const unsigned int audioPrefix,
            /* 取流状态,0-停止取流,1-开始取流 */const unsigned char streamControl,
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */ signed char (*cb)(/* *data指向内存的实际大小,做数据交换时必须设定此值 */unsigned int *const dataMemLenth,
            /* 数据,是用fs_memery_malloc分配的内存,传二级指针方便实现数据交换 */ char * * const fsData, /* 收到的数据长度 */const unsigned int dataLenth,
            /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */const double realTime,
            /* 帧号,最大值只能到3个字节 */const unsigned int frameIndex, /* 是否是关键帧(-1:无法判断;0;不是;1:是) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate,
            /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void *const p), /* 外部传入的指针 */void *externP);
    /* 
     * 创建一个新的RtmpClient_item对象;
     * 成功返回RtmpClient_item指针;
     * 失败返回NULL.
     */
    struct RtmpClient_item *rtmpClient_item_new_byURL__IO(/* url地址,如rtmp://127.0.0.1:554/video */const char url[],
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */ signed char (*cb)(/* *data指向内存的实际大小,做数据交换时必须设定此值 */unsigned int *const dataMemLenth,
            /* 数据,是用fs_memery_malloc分配的内存,传二级指针方便实现数据交换 */ char * * const fsData, /* 收到的数据长度 */const unsigned int dataLenth,
            /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */const double realTime,
            /* 帧号,最大值只能到3个字节 */const unsigned int frameIndex, /* 是否是关键帧(-1:无法判断;0;不是;1:是) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate,
            /* 图像宽度(0表示未知) */ unsigned int imgWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 外部传入的指针 */ void *const p), /* 外部传入的指针 */void *externP);
    /* 删除pRtmpClient_item指向的实例对象 */
    void rtmpClient_item_delete__OI(struct RtmpClient_item * const pRtmpClient_item);
    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */
    void rtmpClient_item_streamControl_pthreadSafety(struct RtmpClient_item * const pRtmpClient_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
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
#ifdef FsDebug
    void rtmp_test(void *pMonitor);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* RTMP_H */

