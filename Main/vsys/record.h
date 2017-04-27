/* 
 * File:   record.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef RECORD_H
#define	RECORD_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Media/VideoMKV.h"
#include "../../PublicTools/Thread/Rtmp.h"
#include "capture.h"
#ifdef	__cplusplus
extern "C" {
#endif
#define Record_BaseLine 20000
#pragma pack(push,2)

    /* 数据记录 */
    struct Record {

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
            /* 数据保存的目录,以'/'结尾 */
            char *_dir;
            /* 主线程的tid号 */
            pthread_t stid;
            /* rtsp服务器对象 */
            void *_pRtspServer;
            /* rtmp服务器对象 */
            Rtmp *_pRtmp;
            /* hls服务器对象 */
            void *_pHls;
            /* GB28181服务器对象 */
            void *_pGB28181Server;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 每个线程最多处理的通道数 */
            unsigned short _channelCountPerThread;
            /* 通道链表,节点仅在控制线程可读 */
            FsObjectList *__itemList;
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
            /* 重置通道,负数表示重置完成 */
            int resetChannel;
        } rw;

        struct {
            /* 多线程锁 */
            pthread_mutex_t __mutex;
            /* 线程数 */
            unsigned short threadCount;
            /* 同步计数,改为非0时必须锁住 */
            unsigned short syncCount;
            /* 集群的通道链表 */
            FsObjectList *clusterItemList;
            /* 本地ip */
            FsArray *ipList;
        } p;
    };

    /* 记录项 */
    struct Record_item {

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
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 本对象的uuid值,一般是从配置中读入的 */
            char *_uuid;
            /* 所属主机的ip地址,用于集群,本机为0 */
            unsigned int _ipv4;
            /* 所属的记录对象 */
            struct Record *_pRecord;
            /* 视频转发+历史点播+集群 */
            struct RtspServer_item *__pRtspServer_item;
            /* 视频转发+集群 */
            struct RtmpServer_item *__pRtmpServer_item;
            /* hls视频转发+集群 */
            struct HlsServer_item *__pHlsServer_item;
            /* 连接和调用外部命令所需要的同步锁 */
            pthread_mutex_t __mutexCmdConnect;
            /* 实时视频掩码通信指针 */
            void *__realMask;
            /* rtsp实时视频掩码 */
            unsigned int _maskIndex_rtsp;
            /* rtmp实时视频掩码 */
            unsigned int _maskIndex_rtmp;
            /* hls实时视频掩码 */
            unsigned int _maskIndex_hls;
            /* 录像的掩码 */
            unsigned int _maskIndex_record;
            /* 注册推送实时视频掩码通信 */
            unsigned int _maskIndex_localPush;
            /* gb28181视频转发 */
            struct GB28181Server_item *__pGB28181Server_item;
            /* 内部数据推送,为空表示不推送 */
            char *_localPush;
            /* 视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-不记录,但保留已储存的数据 */
            unsigned char _recordVideoMode : 2;
            /* 实时视频保存的最长时间,单位秒,0表示不限制 */
            unsigned int _realVideoSaveTime;
            /* 传入的数据帧链表 */
            FsObjectList *__framelistIn;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
            /* 视频缓存链表 */
            FsObjectList *__videoCacheList;
            /* 视频合成任务链表,任务项按结束时间排序 */
            FsObjectList *__videoPartList;
            /* 录像文件的周期,单位秒 */
            unsigned int _recordVideoFileInterval;
            /* 录像文件的开始时间 */
            unsigned int recordVideoFile_startTime;
            /* 本线程函数要用到的临时空间 */
            char *_tmpBufferForThis;
            /* 配置线程函数要用到的临时空间1 */
            char *_tmpBufferForConfig1;
            /* 配置线程函数要用到的临时空间2 */
            char *_tmpBufferForConfig2;
            /* 录像信息,为空表示无录像 */
            FILE *__videoInfo;
        } ro;

        struct {
            /* 所在写入层数 */
            unsigned short layer;
            /* 录像信息更新,0-表示更新录像信息,非0-不更新 */
            unsigned short videoRecordCheck;
            /* 上一次录像的时间,小于0.00001表示未录像 */
            double videoLastTime;
            /* 录像开始时间 */
            double videoStartTime;
        } rw;

        struct {
            /* 当前的录像文件句柄 */
            FILE *_recordVideoFile_fd;
            /* mkv录像 */
            VideoMKV *_pMKV;
            /* 实时转发已处理的数据帧帧号 */
            unsigned int realFrameIndex;
            /* 录像已处理的数据帧帧号 */
            unsigned int recordVideoFrameIndex;
            /* 本地推送数据的对象 */
            void *pLocalPushCamera;
            /* 本地推送的函数指针,不为空表示有效 */
            capture_loaclPush_frame_pthreadSafety localPushCameraFun;
        } p;
    };
#pragma pack(pop)
    /* 创建配置 */
    void record_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent);
    /* 
     * 创建一个新的Record对象;
     * 返回Record指针.
     */
    struct Record *record_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
            /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* rtsp服务器对象 */ void * const pRtspServer, /* rtmp服务器对象 */ Rtmp * const pRtmp, /* hls服务器对象 */ void * const pHls,
            /* GB28181服务器对象 */ void * const pGB28181Server, /* 配置管理对象 */ void *const pConfigManager,
            /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread);
    /* 删除pRecord指向的实例对象 */
    void record_delete__OI(struct Record * const pRecord);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void record_startThread(struct Record* pRecord,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void record_stopThread(struct Record* pRecord);
    /* 传入数据帧,如果数据可作为快照则返回1,否则返回-1 */
    signed char record_item_frame_in_pthreadSafety_OI(/* 采集项,可为空 */ struct Record_item* pRecord_item, FsObjectImageFrame *pFrame);
    /* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
    FsObjectImageFrame *record_item_frame_out_pthreadSafety_IO(/* 采集项,可为空 */ struct Record_item* pRecord_item);
    /* 添加视频合成任务 */
    void record_item_add_partVideo(/* 记录项 */ struct Record_item * const pRecord_item, /* 必需为全路径 */const char fileName[],
            /* 开始时间,推荐使用CPU时钟 */const double startiTime, /* 结束时间,推荐使用CPU时钟 */const double endTime, /* 字幕,可为空 */const char subtitle[]);
    typedef void (*Record_item_add_partVideo)(/* 记录项 */ struct Record_item * const pRecord_item, /* 必需为全路径 */const char fileName[],
            /* 开始时间,推荐使用CPU时钟 */const double startiTime, /* 结束时间,推荐使用CPU时钟 */const double endTime, /* 字幕,可为空 */const char subtitle[]);
    /* 用以检测相机是否在线调用的控制取流的接口,只能在vsys中调用 */
    void record_item_realStream_for_checkStatus(/* 记录项 */ struct Record_item * const pRecord_item, /* 状态,0-停止取流,1-表示取流 */const unsigned char status);
#ifdef	__cplusplus
}
#endif

#endif	/* RECORD_H */

