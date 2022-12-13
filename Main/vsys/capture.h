/* 
 * File:   capture.h
 * Author: fslib
 *
 * Created on 2016年3月24日, 上午11:19
 */

#ifndef CAPTURE_H
#define CAPTURE_H
#include "../../PublicTools/publicDefine.h"
#include <sys/time.h>
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/GroupSqrt.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Thread/GB28181.h"
#include "../../PublicTools/Thread/Rtmp.h"


#include "../../PublicTools/Thread/Rtsp.h"
#ifdef __cplusplus
extern "C" {
#endif
#define Capture_Mask 0x0LLU

    /* 采集 */
    struct Capture {

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
            /* 管道,用于添加和删除文件或发送数据 */
            int __pipe[2];
            /* 线程名 */
#define Capture_get_threadName(pCapture) (((char*)((pCapture)+1))+(pCapture)->ro._threadName_)
            unsigned char _threadName_;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 通道链表,节点仅在控制线程可读 */
            FsObjectList *__itemList_;
            /* 每个线程最多处理的通道数 */
            unsigned short _channelCountPerThread;
            /* 在有输出数据时向哪个开方分组写入信号 */
            FsGroupSqrtPthreadSafety * _pGroupSqrtOut;
            /* rtsp客户端对象 */
            Rtsp *_pRtspClient;
            /* GB28181客户端对象 */
            GB28181 *_pGB28181Client;
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
            /* 重置通道,负数表示重置完成,初始化为-1 */
            int _resetChannel;
        } rw;

        struct {
            /* 线程的tid号 */
            pthread_t stid;
            /* 集群的通道链表 */
            FsObjectList *clusterItemList_;
            /* 获取拉流状态的客户端链表 */
            FsStructList *streamCtrlStatusClientList;
            /* 储存已开始流的相机指针 */
            FsObjectList *streamCtrlStartItemListOrder__streamCtrlStatusClientList;
            /* 标记网络客户端是否有连接出错,0-没有,1-有 */
            unsigned char netClientHasError : 1;
        } p;
    };
#define __CaptureLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,1,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n  pipe=%d/%d\n\
  pConfigManager=%p\n  itemList=%p\n  channelCountPerThread=%hu\n  pGroupSqrtOut=%p\n  pRtspClient=%p\n  pGB28181Client=%p\n  netClientHasError=%hhu",##__VA_ARGS__\
        ,pCapture,(( struct Capture*)(pCapture))->rw.runStatus,(( struct Capture*)(pCapture))->p.stid\
        ,(( struct Capture*)(pCapture))->ro._pMonitor,(( struct Capture*)(pCapture))->ro._externRunStatus,(( struct Capture*)(pCapture))->ro._parentTid,Capture_get_threadName(pCapture),(( struct Capture*)(pCapture))->ro.__pipe[0], (( struct Capture*)(pCapture))->ro.__pipe[1]\
        ,(( struct Capture*)(pCapture))->ro._pConfigManager,(( struct Capture*)(pCapture))->ro.__itemList_, (( struct Capture*)(pCapture))->ro._channelCountPerThread\
        ,(( struct Capture*)(pCapture))->ro._pGroupSqrtOut\
        ,(( struct Capture*)(pCapture))->ro._pRtspClient\
        ,(( struct Capture*)(pCapture))->ro._pGB28181Client\
        ,(( struct Capture*)(pCapture))->p.netClientHasError)
#define CaptureLog(___loglevel,___format, ...) __CaptureLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define CaptureLog2(___loglevel,___format, ...) __CaptureLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 采集项中的存在子流时的用的对象 */
    struct Capture_item_SubStream {
        /* 图像宽度,0为无效值 */
        unsigned int imageWidth;
        /* 图像高度,0为无效值 */
        unsigned int imageHeight;
        /* 对应的相机对象链表,为空表示是本地数据推送 */
        FsObjectList *_pCameraList_;
        /* 相机项 */
        void *__pCamera_item;
        /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
        void (*_camera_remove_and_delete_item_and_emptyCamera)(void* const pCameraList, void *const pCamera_item);
        /* 控制相机流的函数指针 */
        void (*_streamControl)(void *const pCamera_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
        /* 缓存的未输出的数据帧 */
        FsObject *__pData[4];
        /* 缓存的数据帧个数,负数表示缓存主码流,正数表示缓存子码流 */
        signed char dataCount;
    };

    /* 采集项 */
    struct Capture_item {

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
            /* 本对象的uuid值,一般是从配置中读入的 */
            char *_uuid;
            /* 所属主机的ip地址,用于集群,本机为0 */
            unsigned int _ipv4;
            /* 所属的记录对象 */
            struct Capture *_pCapture;
#define  Capture_item_ClusterLen  (FsPointer_to_long(&((struct Capture_item*) 0)->ro._sum)+sizeof(((struct Capture_item*) 0)->ro._sum))
            /* 整个通道依赖的所有数据的校验和 */
            unsigned long long _sum;
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
            /* 分发错误信息的对象 */
            void *_pTransfer_error;
            /* 内部控制的函数指针,可用于集群,通知收到的相机控制指令,连接其他本系统的平台进行相机控制 */
            struct Camera_ctrl_item *__pCamera_ctrl_item;
            /* 对应的相机的二级指针,为空表示是本地数据推送,老版本表示对应的相机对象链表 */
            void **_ppCamera;
            /* 相机项 */
            void *__pCamera_item;
            /* 从*ppCamera中移除一个相机项并删除,在所属项被删除后,把相机停止并删除相机再把*ppCamera置空*/
            void (*_camera_remove_and_delete_item_and_camera)(void* * const ppCamera, void *const pCamera_item
                    , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 控制相机流的函数指针 */
            void (*_streamControl)(void *const pCamera_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
            /* 
             * 相机控制分外部控制和内部控制,如通过onvif,gb28181等协议操作相机为外部控制,本程序之间的级联使用的控制为内部控制,内部控制也可用与通知本系统收到了相机控制指令
             */
            /* 外部控制的函数指针 */
            struct Camera_ctrl_functionUser *_ctrl_function;
            /* 子码流,为空表示不存在子码流 */
            struct Capture_item_SubStream *__pSubStream;



            int (*_Redcamera_close)(void *pCamera_item, unsigned int speed, double time);
            int (*_Redcamera_open)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_left)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_right)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_up)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_down)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_leftup)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_leftdown)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_rightup)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_rightdown)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_stopturn)(void *pCamera_item, unsigned int speed, double time);

            int (*_camera_iris_close)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_iris_open)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_focusfar)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_focusnear)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_zoomfar)(void *pCamera_item, unsigned int speed, double time);
            int (*_camera_zoomnear)(void *pCamera_item, unsigned int speed, double time);

            int (*_camera_throughfrog)(void *pCamera_item, unsigned int speed, double time);

            int (*_camera_gotoptz)(void *pCamera_item, unsigned int speed, float p, float t, float z, double time);
            int (*_camera_getptz)(void *pCamera_item, float* p, float* t, float* z);

            int (*_camera_autofocus)(void *pCamera_item, unsigned int speed, double time);

        } ro;

        struct {
            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
            unsigned char *_pGroupSqrtOut;

            unsigned int m_action; //指令编号
            unsigned int camera_speed; //云台控制速度
            unsigned int camera_time; //云台指令持续时间
            float goto_p; //期望转向的ptz值
            float goto_t;
            float goto_z;

            float camera_c_p; //云台当前ptz值
            float camera_c_t;
            float camera_c_z;

            unsigned int m_auto_focus; //自动聚焦功能        
            unsigned int m_target_id; //雷达目标ID

            unsigned int m_reply_type; //回复方式： 1：json 2：xml  3：其他

            int m_stopAction; //停止指令  

        } rw;

        struct {
            /* 缓存的控制客户端连接 */
            FsStructList *__ctrlClientList;
            /* 获取错误数据的客户端 */
            FsStructList * __errorInfoClientList;
            /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
             *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
             *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */
            unsigned short ctrlPriority;
            int shot_type; //镜头类型(正对中光学)，0：正比例(z值越大，场景拉得越近，z值越小场景拉得越远)  1：反比例(相反)
            char camera_ip[25]; //相机IP
            unsigned short camera_port; //相机端口
            struct timeval camera_tv; //云台socket等待时间
            int camera_type;
            int camera_socket;

        } p;
    };
#define __Capture_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,1,___format",p=%p,uuid:\"%s\",pTransfer_error=%p,ctrlClientList=%p/%lu,errorInfoClientList=%p/%lu.\n",##__VA_ARGS__\
        ,pCapture_item,(( struct Capture_item*)(pCapture_item))->ro._uuid,(( struct Capture_item*)(pCapture_item))->ro._pTransfer_error\
        ,((struct Capture_item*)(pCapture_item))->p.__ctrlClientList,((struct Capture_item*)(pCapture_item))->p.__ctrlClientList?(( struct Capture_item*)(pCapture_item))->p.__ctrlClientList->nodeCount:0\
        ,((struct Capture_item*)(pCapture_item))->p.__errorInfoClientList,(( struct Capture_item*)(pCapture_item))->p.__errorInfoClientList?(( struct Capture_item*)(pCapture_item))->p.__errorInfoClientList->nodeCount:0)
#define Capture_itemLog(___loglevel,___format, ...) __Capture_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Capture_itemLog2(___loglevel,___format, ...) __Capture_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    /* 创建配置 */
    void capture_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * const parent);
    /* 检查pCapture模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
    int capture_check_channel_changed(struct Capture * const pCapture, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);
    /* 
     * 创建一个新的Capture对象;
     * 返回Capture指针.
     */
    struct Capture * capture_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 配置管理对象 */ void *const pConfigManager
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread
            , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut
            , /* rtsp客户端对象 */ Rtsp * const pRtspClient
            , /* GB28181客户端对象 */ GB28181 * const pGB28181Client);
    /* 删除pCapture指向的实例对象 */
    void capture_delete__OI(struct Capture * const pCapture);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void capture_startThread(struct Capture * const pCapture
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程,只发送信号,不回收线程资源 */
    void capture_stopThread_noWait(struct Capture * const pCapture);
    /* 结束线程 */
    void capture_stopThread(struct Capture * const pCapture);

    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
    unsigned int capture_item_frame_out_pthreadSafety__IO_4(struct Capture * const pCapture, /* 获取的通道编号,从0开始 */unsigned int index
            , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);
    /* 获取数据帧的数量 */
    unsigned int capture_item_get_frame_count_pthreadSafety(struct Capture * const pCapture, /* 获取的通道编号,从0开始 */unsigned int index);
    typedef void (*capture_loaclPush_frame_pthreadSafety)(/* 本地推送相机项 */ void *const plocalPush_item, FsObjectImageFrame * const pFrame, /* 推送源的uuid */const char uuid[], /* 上一个采集的索引 */ const void* const src
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 重置模块内通道对应的开方分组 */
    void capture_item_reset_groupSqrtOut(struct Capture * const pCapture, /* 获取的通道编号,从0开始 */unsigned int index
            , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
    int capture_Redcamera_close(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_Redcamera_open(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_left(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_right(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_up(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_down(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_leftup(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_leftdown(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_rightup(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_rightdown(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_stopturn(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);

    int capture_camera_iris_close(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_iris_open(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_focusfar(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_focusnear(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_zoomfar(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);
    int capture_camera_zoomnear(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);

    int capture_camera_throughfrog(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);

    int capture_camera_gotoPTZ(struct Capture_item *pCamera_rtsp_item, unsigned int speed, float p, float t, float z, double time);
    int capture_camera_getPTZ(struct Capture_item *pCamera_rtsp_item, float* p, float* t, float* z);
    int capture_camera_autofocus(struct Capture_item *pCamera_rtsp_item, unsigned int speed, double time);

#ifdef __cplusplus
}
#endif

#endif /* CAPTURE_H */

