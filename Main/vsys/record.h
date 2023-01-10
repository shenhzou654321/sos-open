/* 
 * File:   record.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef RECORD_H
#define RECORD_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Media/VideoMKV.h"
#include "../../PublicTools/Media/VideoAVI.h"

#include "../../PublicTools/Thread/Rtsp.h"
#include "../../PublicTools/Thread/Rtmp.h"


#include "../../PublicTools/Thread/Hls.h"


#include "../../PublicTools/Thread/GB28181.h"

#include "capture.h"
#ifdef __cplusplus
extern "C" {
#endif
#define Record_BaseLine 20000
#define Record_Mask 0x0LLU

    /* 卡口,_规定为创建者填写,其他为本模块填写,空间申请使用malloc,并使用memset全置0 */
    struct Record_item_Watch {
        /* 目标编号,使用时间*1000+随机码表示 */
        unsigned long long _objIndex;
        /* 被记录的次数,从1开始 */
        unsigned int _recordIndex;
        /* 目标位置 */
        short x1, y1, x2, y2;
        /* 图片信息,没有图片的必须置空 */
        FsObjectImageFrame *_ppFrame[1];
        /* 车牌识别对象,可为空 */
        struct Recognition_item_Plate *_pPlate;
    };

    /* 事件记录,_规定为创建者填写,其他为本模块填写,空间申请使用malloc,并使用memset全置0 */
    struct Record_item_Inc {
        /* 事件类型,0x10-枪球联动过船信息 */
        unsigned char _type;
        /* 事件开始时间,开机时间 */
        double _starttime;
        /* 事件开始时间,开始时间,小于0表示无结束时间 */
        double _endtime;
        /* 图片信息,没有图片的必须置空 */
        FsObjectImageFrame *_ppFrame[5];

        /* 触发数据 */
        union {

            /* 枪球联动 */
            struct {
                /* 枪机的ip地址 */
                unsigned int ipaddr;
                /* 设备编号 */
                const char *deviceno;
                /* 设备名 */
                const char *devicename;
                /* 经纬度位置,经度与纬度之间用逗号隔开如 10.1,20.3 */
                const char *position_jwd;
                /* 枪机的实时视频地址 */
                const char *realurl;
                /* 枪机的录像视频 */
                const char *incVideo;
                /* 触发的区域,用经纬度表示,如(10,10)(20,20) 每个点用括号括起来,点的个数不定 */
                const char *area_jwd;
                /* 目标在画面中的运动方向,0:表示从左往右运动;1表示从右往左运动 */
                char direction;
            } qqld;
        } trigger;
    };

    /* 数据记录 */
    struct Record {

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
            /* 线程的tid号 */
            pthread_t stid;
            /* 线程名 */
#define Record_get_threadName(pRecord) (((char*)((pRecord)+1))+(pRecord)->ro._threadName_)
            unsigned char _threadName_;
            /* 数据保存的目录,以'/'结尾 */
            char *_dir;
            /* rtsp服务器对象 */
            Rtsp *_pRtspServer;
            /* hls服务器对象 */
            Hls *_pHlsServer;
            /* GB28181服务器对象 */
            GB28181 *_pGB28181Server;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 每个线程最多处理的通道数 */
            unsigned short _channelCountPerThread;
            /* 通道链表,节点仅在控制线程可读 */
            FsObjectList *__itemList;
            /* 在有输出数据时向哪个开方分组写入信号 */
            FsGroupSqrtPthreadSafety * _pGroupSqrtOut;
            /* 获取录像信息数据的客户端链表 */
            FsStructList *__videoInfoDataClientList;
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
            /* 录像信息数据是否发生改变 */
            unsigned char videoInfoChange : 1;
            /* 重置通道,负数表示重置完成,初始化为-1 */
            int _resetChannel;
            /* 快照缓存有效期,单位秒,0不表示不限制 */
            float _snapbuffertimeout;
        } rw;

        struct {
            /* 线程数 */
            unsigned short threadCount;
            /* 同步计数,改为非0时必须锁住 */
            unsigned short syncCount;
            /* 集群的通道链表 */
            FsObjectList *clusterItemList;
            /* 录像信息数据改变的编号 */
            unsigned long long videoInfoIndex;
            /* 录像信息数据,为空应返回错误,使用videoInfoDataClientList的线程锁 */
            FsEbml *videoInfoData__videoInfoDataClientList;
            /* 录像信息数据ebml数据,使用videoInfoDataClientList的线程锁 */
            FsObjectBasePthreadSafety *videoInfoData_ebml__videoInfoDataClientList;
            /* 录像信息数据xml数据,使用videoInfoDataClientList的线程锁 */
            FsObjectBasePthreadSafety *videoInfoData_xml__videoInfoDataClientList;
            /* 录像信息数据json数据,使用videoInfoDataClientList的线程锁 */
            FsObjectBasePthreadSafety *videoInfoData_json__videoInfoDataClientList;
            /* 使用struct ConfigManager_connectNode_useOnce结构缓存一些请求,用于报告一些线程参数,使用videoInfoDataClientList的线程锁 */
            void * pConnectNode__videoInfoDataClientList;
            ////////////////////////////////////////////////////////////////////
            /* sdk配置是否更新,如Record_sdkConfigUpdate_index_dh为0,则sdkConfigUpdate[0]&0x1表示配置文件有更新,sdkConfigUpdate[0]&0x2表示点播客户端信息有更新 */
#define Record_sdkConfigUpdate_get(___sdkConfigUpdate,___index) ((___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]>>((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1)))&0x3)
            //#define Record_sdkConfigUpdate_set_0_0(___sdkConfigUpdate,___index) ___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]&=~(0x1LLU<<((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1)))
#define Record_sdkConfigUpdate_set_0_1(___sdkConfigUpdate,___index) ___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]|=(0x1LLU<<((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1)))
            //#define Record_sdkConfigUpdate_set_1_0(___sdkConfigUpdate,___index) ___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]&=~(0x1LLU<<(((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1))+1))
#define Record_sdkConfigUpdate_set_1_1(___sdkConfigUpdate,___index) ___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]|=(0x1LLU<<(((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1))+1))
#define Record_sdkConfigUpdate_set_01_0(___sdkConfigUpdate,___index) ___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]&=~(0x3LLU<<((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1)))
#define Record_sdkConfigUpdate_set_01_1(___sdkConfigUpdate,___index) ___sdkConfigUpdate[(___index*2)/(sizeof(___sdkConfigUpdate[0])*8)]|=(0x3LLU<<((___index*2)&((sizeof(___sdkConfigUpdate[0])*8)-1)))
#define Record_sdkConfigUpdate_check_not0(___sdkConfigUpdate) (___sdkConfigUpdate[0]!=0)
#define Record_sdkConfig_comment
            unsigned long long _sdkConfigUpdate[1];
            /* 大华sdk的配置信息,使用videoInfoDataClientList的线程锁 */
#define Record_sdkConfigUpdate_index_dh 0
            FsEbml *dhsdkConfig__videoInfoDataClientList;
            /* GB28181的配置信息,使用videoInfoDataClientList的线程锁 */
#define Record_sdkConfigUpdate_index_gb 1
            FsEbml *gbsdkConfig__videoInfoDataClientList;
            ////////////////////////////////////////////////////////////////////
        } p;
    };
#define __RecordLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,1,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  dir:\"%s\"\n  pRtspServer=%p\n\
  pHlsServer=%p\n  pGB28181Server=%p\n  pConfigManager=%p\n\
  channelCountPerThread=%hu\n  itemList=%p\n\
  dhsdkConfig__videoInfoDataClientList=%p/%llx\n\
  gb28181Config__videoInfoDataClientList=%p/%llx\n",##__VA_ARGS__\
        ,pRecord,((struct Record*)(pRecord))->rw.runStatus,((struct Record*)(pRecord))->ro.stid\
        ,((struct Record*)(pRecord))->ro._pMonitor,((struct Record*)(pRecord))->ro._externRunStatus,((struct Record*)(pRecord))->ro._parentTid, Record_get_threadName(pRecord)\
        ,((struct Record*)(pRecord))->ro._dir,((struct Record*)(pRecord))->ro._pRtspServer\
        ,((struct Record*)(pRecord))->ro._pHlsServer,((struct Record*)(pRecord))->ro._pGB28181Server,((struct Record*)(pRecord))->ro._pConfigManager\
        ,((struct Record*)(pRecord))->ro._channelCountPerThread,((struct Record*)(pRecord))->ro.__itemList\
        ,((struct Record*)(pRecord))->p.dhsdkConfig__videoInfoDataClientList,Record_sdkConfigUpdate_get(((struct Record*)(pRecord))->p._sdkConfigUpdate,Record_sdkConfigUpdate_index_dh)\
        ,((struct Record*)(pRecord))->p.gbsdkConfig__videoInfoDataClientList,Record_sdkConfigUpdate_get(((struct Record*)(pRecord))->p._sdkConfigUpdate,Record_sdkConfigUpdate_index_gb))
#define RecordLog(___loglevel,___format, ...) __RecordLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RecordLog2(___loglevel,___format, ...) __RecordLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 记录项 */
    struct Record_item {

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
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 所属主机的ip地址,用于集群,本机为0 */
            unsigned int _ipv4;
            /* 本对象的uuid值,一般是从配置中读入的 */
            char *_uuid;
            /* 所属的记录对象 */
            struct Record *_pRecord;
            /* rtsp视频转发+历史点播+集群 */
            struct RtspServer_item *__pRtspServer_item;
            /* rtmp视频转发+集群 */
            struct RtmpServer_item *__pRtmpServer_item;

            /* hls视频转发+集群 */
            struct HlsServer_item *__pHlsServer_item;
            /* gb28181视频转发 */
            struct GB28181Server_item *__pGB28181Server_item;
#define  Record_item_ClusterLen  (FsPointer_to_long(&((struct Record_item*) 0)->ro._sum)+sizeof(((struct Record_item*) 0)->ro._sum))
            /* 整个通道依赖的所有数据的校验和 */
            unsigned long long _sum;
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 连接和调用外部命令所需要的同步锁 */
            pthread_mutex_t __mutexCmdConnect;
            /* 实时视频掩码通信指针 */
            void *_realMask;
            /* rtsp实时视频掩码 */
            unsigned int _maskIndex_rtsp;
            /* hls实时视频掩码 */
            unsigned int _maskIndex_hls;
            /* gb28181实时视频掩码 */
            unsigned int _maskIndex_gb28181;
            /* 录像的掩码 */
            unsigned int _maskIndex_record;
            /* 外部储存的节点(Record_item_is_remoteVideoStorage(recordVideoMode)为真时有效),为空表示不使用外部储存,比如_recordVideoMode为3时,此值指向pRecord->p.dhsdkConfig__videoInfoDataClientList中的节点,删除时需要同步删除 */
            struct FsEbml_node *__storageConfigNode;
            /* 视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-在有直播请求时记录,3-大华sdk,4-GB28181,15-取流但不记录 */
#define Record_item_is_pullStream(___recordVideoMode) (1==(___recordVideoMode)||15==(___recordVideoMode)) // 记录模块在拉流
#define Record_item_is_localVideoStorage(___recordVideoMode) ((___recordVideoMode)>=1&&(___recordVideoMode)<=2) // 使用本地储存
#define Record_item_is_remoteVideoStorage(___recordVideoMode) ((___recordVideoMode)>=3&&(___recordVideoMode)<=4) // 使用第三方储存
#define Record_item_is_videoStorage(___recordVideoMode) ((___recordVideoMode)>=1&&(___recordVideoMode)<=4) // 使用储存
            unsigned char _recordVideoMode : 4;
            /* 数据记录类型,0-ebml数据,1-xml数据,2-xml含注释数据,3-json数据 */
            unsigned char _recordType : 2;
            /* 实时视频保存的最长时间,单位秒,0表示不限制 */
            unsigned int _realVideoSaveTime;
            /* 设备编号 */
            char *_deviceno;
            /* 设备名 */
            char *_devicename;
            /* 安装位置 */
            char *_positionname;
            /* 经纬度位置,经度与纬度之间用逗号隔开如 10.1,20.3 */
            char *_position_jwd;
            /* 传入的数据帧链表 */
            FsObjectList *__framelistIn;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
            /* 视频缓存链表 */
            FsObjectList *__videoCacheList;
            /* 视频合成任务链表,任务项按结束时间排序 */
            FsObjectList *__videoPartList;
            /* 卡口任务链表,储存struct Record_item_Watch指针 */
            FsObjectList *__watchList;
            /* 事件任务链表,储存struct Record_item_Inc指针 */
            FsObjectList *__incList;
            /* 录像信息在内存中保存的位置指针 */
            struct FsEbml_node *_videoInfo_pos;
            /* 录像文件的周期,单位秒 */
            unsigned int _recordVideoFileInterval;
        } ro;

        struct {
            /* 所在写入层数 */
            unsigned short layer;
            /* 录像信息更新的基础时间,在有变动时置0 */
            unsigned char videoRecordCheckInterval0;
            /* 录像信息更新,为0表示检查并把值置为videoRecordCheckInterval0,否则减1 */
            unsigned char _videoRecordCheckInterval;
            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
            unsigned char *_pGroupSqrtOut;
        } rw;

        struct {
            /* 上一次录像的时间,小于等于0.0表示未录像 */
            double videoLastTime;
            /* 在录像开始时记录录像开始时间,在录像持续时间超过一定时间(如1min)后,再把录像开始时间写入记录,以避免快照或视频质量差导致录像频繁中断的问题,在记录开始时间后取原值的相反数 */
#define Record_Continue_time 60.0
            double videoStartTime;
            /* 在录像开始时记录录像开始时间(逻辑时间) */
            double videoStartTimeGmt;
            /* 缓存的快照数据 */
            FsObjectImageFrame *__snap;
            /* 当前的录像文件句柄 */
            FILE *_recordVideoFile_fd;
            /* mkv录像 */
            VideoMKV *_pMKV;
            /* 当前录像文件流的掩码 */
            unsigned int recordMask;
            /* 实时转发已处理的数据帧帧号 */
            unsigned int realFrameIndex;
            /* 录像已处理的数据帧帧号 */
            unsigned int recordVideoFrameIndex;
            /* 保存数据的编号,0-99 */
            unsigned char recordIndex : 7;
            /* 最新保存视频信息的文件(Record_item_is_localVideoStorage(pRecord_item)为真时有效),0-表示使用__info.txt,1-表示使用__inf0.txt */
            unsigned char _videoInfo_file : 1;
            /* 录像信息是否已加载,0-表示未加载,1-表示已加载 */
            unsigned char videoInfoHasLoad : 1;
            /* 当前录像文件的开始时间(录像文件中的所有帧按一定算法归一化后都是此值)(Record_item_is_localVideoStorage(pRecord_item)为真videoInfoHasLoad为1时且时有效) */
            unsigned int recordVideoFile_startTime;
            /* 两个录像信息文件的结束时间(Record_item_is_localVideoStorage(recordVideoMode)为真且videoInfoHasLoad为1时有效) */
            double videoTimeEnd[2];
        } p;
    };
    #define __Record_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,1,___format",p=%p/%p,storageConfigNode=%p,recordVideoMode=%hhu,recordType=%hhu\
,videoInfo_pos=%p,recordIndex=%hhu,videoInfo_file=%hhu,recordVideoFileInterval=%u\
,videoInfoHasLoad=%hhu,recordVideoFile_startTime=%u,videoTimeEnd=%lf/%lf.\n",##__VA_ARGS__\
        ,pRecord_item,pRecord_item->ro._pRecord\
        ,pRecord_item->ro.__storageConfigNode,pRecord_item->ro._recordVideoMode,pRecord_item->ro._recordType\
        ,pRecord_item->ro._videoInfo_pos,pRecord_item->ro._recordVideoFileInterval\
        ,pRecord_item->p.recordIndex,pRecord_item->p._videoInfo_file,pRecord_item->p.videoInfoHasLoad\
        ,pRecord_item->p.recordVideoFile_startTime,pRecord_item->p.videoTimeEnd[0],pRecord_item->p.videoTimeEnd[1])
#define Record_itemLog(___loglevel,___format, ...) __Record_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Record_itemLog2(___loglevel,___format, ...) __Record_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    /* 创建配置 */
    void record_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent);
    /* 检查pRecord模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
    int record_check_channel_changed(struct Record * const pRecord, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);
    /* 
     * 创建一个新的Record对象;
     * 返回Record指针.
     */
    struct Record * record_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* rtsp服务器对象 */ Rtsp * const pRtspServer
            , /* hls服务器对象 */ Hls * const pHlsServer, /* GB28181服务器对象 */ GB28181 * const pGB28181Server, /* 配置管理对象 */ void *const pConfigManager
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread
            , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut);
    /* 删除pRecord指向的实例对象 */
    void record_delete__OI(struct Record * const pRecord);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void record_startThread(struct Record * const pRecord,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程 */
    void record_stopThread(struct Record * const pRecord);
    /* 传入数据帧,如果数据可作为快照则返回1,否则返回-1 */
    int record_item_frame_in_pthreadSafety__OI_4(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
    unsigned int record_item_frame_out_pthreadSafety__IO_4(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index, /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);
    /* 获取数据帧的数量 */
    unsigned int record_item_get_frame_count_pthreadSafety(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index);
    /* 重置模块内通道对应的开方分组 */
    void record_item_reset_groupSqrtOut(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index
            , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
    /* 添加视频合成任务 */
    void record_item_add_partVideo_pthreadSafety(/* 记录项 */ struct Record_item * const pRecord_item, /* 必需为全路径 */const char fileName[],
            /* 开始时间,推荐使用CPU时钟 */const double startiTime, /* 结束时间,推荐使用CPU时钟 */const double endTime, /* 字幕,可为空 */const char subtitle[]);
    typedef void (*Record_item_add_partVideo_pthreadSafety)(/* 记录项 */ struct Record_item * const pRecord_item, /* 必需为全路径 */const char fileName[],
            /* 开始时间,推荐使用CPU时钟 */const double startiTime, /* 结束时间,推荐使用CPU时钟 */const double endTime, /* 字幕,可为空 */const char subtitle[]);
    /* 删除pWatch的空间 */
    void record_item_watch_delete__OI(struct Record_item_Watch * const pWatch);
    /* 添加卡口记录任务 */
    void record_item_add_watch_pthreadSafety__OI_2(/* 记录项 */ struct Record_item * const pRecord_item, struct Record_item_Watch * const pWatch);
    typedef void (*Record_item_add_watch_pthreadSafety__OI_2)(/* 记录项 */ struct Record_item * const pRecord_item, struct Record_item_Watch * const pWatch);
    /* 删除pInc的空间 */
    void record_item_inc_delete__OI(struct Record_item_Inc * const pInc);
    /* 添加事件记录任务 */
    void record_item_add_inc_pthreadSafety__OI_2(/* 记录项 */ struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc);
    typedef void (*Record_item_add_inc_pthreadSafety__OI_2)(/* 记录项 */ struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc);
    /* 用以检测相机是否在线调用的控制取流的接口,只能在vsys中调用 */
    void record_item_realStream_for_checkStatus(/* 记录项 */ struct Record_item * const pRecord_item, /* 状态,0-停止取流,1-表示取流 */const unsigned char status);
    /* 获取快照,成功返回FsObjectImageFrame指针,失败返回NULL */
    FsObjectImageFrame *record_item_snap_get_pthreadSafety__IO(/* 采集项,不可为空 */ struct Record_item * const pRecord_item);
#ifdef __cplusplus
}
#endif

#endif /* RECORD_H */

