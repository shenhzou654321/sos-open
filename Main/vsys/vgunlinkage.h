/* 
 * File:   vgblinkage.h
 * Author: Administrator
 *
 * Created on 2017年4月26日, 下午2:07
 */

#ifndef V_GUN_LINKAGE_H
#define V_GUN_LINKAGE_H

#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Media/VideoMKV.h"
#include "../../PublicTools/Thread/Rtsp.h"
#include "../../PublicTools/Fs/TypeFace.h"
#include "../../Modules/track/vibe_proc.h"
#include "../../Modules/camera_ctrl/HCNetSDK.h"
#include "../../Modules/camera_ctrl/camera_ctrl_tcp.h"
#include "record.h"
#include "../../PublicTools/Thread/SocketPool.h"

#define   EXPORT_CONDITION   5   //目标联动条件

#ifdef __cplusplus
extern "C" {
#endif
#define Gunlinkage_Mask 0x8LLU

    struct Gunlinkage {

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
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 每个线程最多处理的通道数 */
            unsigned short _channelCountPerThread;
            /* 通道链表,节点仅在控制线程可读 */
            FsObjectList *__itemList_;
            /* 在有输出数据时向哪个开方分组写入信号 */
            FsGroupSqrtPthreadSafety * _pGroupSqrtOut;
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
            /* 多线程锁 */
            pthread_mutex_t __mutex;
            /* 线程数 */
            unsigned short threadCount;
            /* 同步计数 */
            unsigned short syncCount;
            /* 集群的通道链表 */
            FsObjectList *clusterItemList;
            /* 本地ip */
            FsArray *ipList;

            SocketPool* pSocketPool;
        } p;
    };

    //设备信息

    struct GunDeviceConfig {
        char m_camera_ip[25]; //相机IP
        char m_szCameraUid[64]; //相机注册用户名
        char m_szCameraPwd[64]; //相机注册密码
        unsigned short m_camera_port; //相机端口

        char m_camera_ctrl_ip[25]; //相机控制IP(云台控制IP)
        unsigned short m_camera_ctrl_port; //相机控制端口(云台控制端口)

        unsigned int CapImageWidth; //相机采集图像宽
        unsigned int CapImageHeight; //相机采集图像高

        unsigned short m_camera_model; //相机型号  0:"白光球机" 1:"红外球机" 2:"白光枪机" 3:"红外枪机"  4:"白光光电仪"  5:"红外光电仪"     
        unsigned short m_camera_brand; //相机品牌：0, "海康威视" 1, "中光学" 2, "上赛"  3，"霍尼韦尔"
        char m_linkage_server_ip[25]; //联动相机所在服务器的ip地址
        unsigned short m_linkage_server_uuid; //联动相机在服务器中的uuid

        unsigned int m_camera_moved_overTime; //单位为秒(相机人为移动后，多长时间不动后，回到预置位，继续检测！)  
    };

    //附加信息

    struct GunParamConfig {
        char deviceno[128];
        char devicename[128];
        char position_jwd[128];
        char area_jwd[512];
    };

    //外部设置 vibe参数

    struct VibeConfig {
        int m_objw; //目标过滤大小
        int m_objh;

        int moveth; //初始过滤阈值

        int obj_min_h; //目标最小高度
        int loop_length;
        int topth;
        int lowth;
        int loop_start_left;
        int loop_start_right;

        int vibe_left; //检测区域
        int vibe_right;
        int vibe_top;
        int vibe_bottom;

        double vibe_start_time; //枪机检测开始时间，整数为小时，小数点后有效为2位(表示分钟)
        double vibe_end_time; //枪机检测结束时间，整数为小时，小数点后有效为2位(表示分钟)
    };

    struct Gunlinkage_item {

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
            /* 掩码 */
            unsigned long moduleMask;
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
            struct Gunlinkage *_pGunlinkage;
            /* 传入的数据帧链表 */
            FsObjectList *__framelistIn;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
#define  Gunlinkage_item_ClusterLen  (FsPointer_to_long(&((struct Gunlinkage_item*) 0)->ro._sum)+sizeof(unsigned long long))
            /* 整个通道依赖的所有数据的校验和 */
            unsigned long long _sum;
            /* 连接和调用外部命令所需要的同步锁 */
            pthread_mutex_t __mutexCmdConnect;
            /* 文字叠加 */
            FsTypeFace *__pTypeFace;
            /*图像数据处理缩小倍数,1920*1080:缩小16倍*/
            unsigned int zoomTimes;
            /*算法处理的帧率的减小倍数*/
            unsigned int m_linkageframeInterval;
            /* 实时视频掩码通信指针 */
            void *__realMask;
            /* rtsp实时视频掩码 */
            unsigned int _maskIndex;

            char m_linkage_rtspURL[512]; //实时录像地址

            SocketPool_item* __pSoketpool_item;

            struct VibeConfig m_config_vibe;
            struct GunDeviceConfig m_config_devinfo; //相关设备信息
            struct GunParamConfig m_param_config; //附加信息
            int m_debug_switch;
            int m_compressRate;
        } ro;

        /*
         * 除结构体之外的变量：
         * 
         *   m_ :表示是通过配置获取的参数，只能通过配置进行修改
         *   v_ :表示是中间变量，不是通过配置获取的。 
         * 
         *   将所欲v_变量都放到resetParam()函数中，每次跟踪结束都重置参数
         */
        struct {
            /* 当有数据输入时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
            unsigned char *_pGroupSqrtIn;
            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
            unsigned char *_pGroupSqrtOut;
            //int m_camera_model;

            int m_linkage_switch;
            unsigned int m_linkage_objNumber; //目标编号，递增加一产生

            int m_linkage_saveVideo; //1:保存录像 0：不保存录像
            char m_linage_videoPath[512]; //录像储存路径


            unsigned int m_linkage_dome2gun; //是否球机当枪机使用(0:不是；1：是)
            double m_linkage_pre_p; //球机检测预置位(如果发现球机移动，停止检测，过5分钟后回到预置位，然后重新开始检测)
            double m_linkage_pre_t;
            double m_linkage_pre_z;

            unsigned int m_linkage_timevalid_flag; //相机是否在有效检测时间内(0：不在；1：在)
            unsigned int m_linkage_camMoved_flag; //相机移动标志(在球机当作枪机使用时，0：相机没有认为移动，1：相机被人为移动了)
            double m_linkage_camMoved_time; //相机移动开始时间
            double m_linkage_camMoved_p; //相机移动后的ptz值                         
            double m_linkage_camMoved_t;
            double m_linkage_camMoved_z;
            double m_linkave_camMoved_checkTime; //相机移动检测时间


            Camera_ctrl_tcp_item* pCamera_ctrl_item; //相机控制

            pthread_mutex_t m_log_mutex; //日志保存互锁

            VibeParam m_vibeparam; //vibe检测船舶[中间变量]
            vibe_inParam m_vibe_inparam; //vibe输入参数

            trigInfo m_trig_out[TRIGGERCOUNT]; //输出目标缓存队列
            int curr_trig_num; //当前处理的触发目标编号


            //调试使用
            int m_debug_aveW;
            int m_debug_aveH;
            int m_debug_aveW1;
            int m_debug_aveflag; //0:干扰，1：小船，2：大船
            int m_debug_c_left; //调试使用
            int m_debug_c_right; //调试使用
            int m_debug_c_top; //调试使用
            int m_debug_c_bottom; //调试使用
            double m_debug_c_speed; //调试使用

            int m_debug_sendpos; //调试使用
            int m_debug_number; //调试使用
            double m_debug_left; //调试使用
            double m_debug_right; //调试使用
            double m_debug_top; //调试使用
            double m_debug_bottom; //调试使用

            char m_reserved[64];
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
            /* 数据记录的指针 */
            struct Record_item *pRecord_item;
            /* 合成视频的函数指针,不为空表示有效 */
            Record_item_add_partVideo_pthreadSafety record_item_add_partVideo;
        } p;
    };


    void gunlinkage_createConfig(FsConfig * const pConfig, void * parent);
    /* 检查pGunlinkage模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
    int gunlinkage_check_channel_changed(struct Gunlinkage * const pGunlinkage, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);
    /* 
     * 创建一个新的Vlinkage对象;
     * 返回Vlinkage指针.
     */
    struct Gunlinkage * gunlinkage_new__IO(/* 线程名 */const char threadName[]
            , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus
            , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* 配置管理对象 */ void *pConfigManager
            , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut);

    /* 删除pVlinkage指向的实例对象 */
    void gunlinkage_delete__OI(struct Gunlinkage* pGunlinkage);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void gunlinkage_startThread(struct Gunlinkage* pGunlinkage,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程 */
    void gunlinkage_stopThread(struct Gunlinkage* pGunlinkage);
    /* 传入数据帧 */
    void gunlinkage_item_frame_in_pthreadSafety__OI_4(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
    unsigned int gunlinkage_item_frame_out_pthreadSafety__IO_4(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index
            , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);
    /* 获取数据帧的数量 */
    unsigned int gunlinkage_item_get_frame_count_pthreadSafety(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index);
    /* 重置模块内通道对应的开方分组 */
    void gunlinkage_item_reset_groupSqrtOut(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index
            , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
#ifdef __cplusplus
}
#endif

#endif /* VGBLINKAGE_H */

