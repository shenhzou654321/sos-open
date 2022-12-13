/* 
 * File:   vgblinkage.h
 * Author: Administrator
 *
 * Created on 2017年4月26日, 下午2:07
 */

#ifndef V_BALL_LINKAGE_H
#define V_BALL_LINKAGE_H

#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Media/VideoMKV.h"
#include "../../PublicTools/Thread/Rtsp.h"
#include "../../PublicTools/Fs/TypeFace.h"
#include "../../Modules/track/vibe_proc.h"
#include "../../Modules/camera_ctrl/HCNetSDK.h"
#include "../../Modules/camera_ctrl/camera_ctrl_tcp.h"
#include "record.h"

#define MAX_PRESET_COUNT  4
#define MAX_GUN_COUNT     3
#define SNAP_PIC_COUNT    5

#ifdef __cplusplus
extern "C" {
#endif
#define Domelinkage_Mask 0x8LLU

    struct Domelinkage {

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
        } p;
    };

    //设备信息

    struct DomeDeviceConfig {
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
    };

    //标定参数

    struct calibrationParam {
        int valid; //是否有效

        double p;
        double t;
        double z;

        double gunSet[8];
        double domeSet[8];

        double left;
        double right;
        double top;
        double bottom;
    };

    struct calibrationConfig {
        int valid; //是否有效
        char gun_uuid[56]; //关联枪机uuid  
        int calcount; //标定个数
        calibrationParam m_calParam[MAX_PRESET_COUNT]; //标定参数 
    };

    //附加信息

    struct domeParamConfig {
        char deviceno[128];
        char devicename[128];
        char position_jwd[128];
        char area_jwd[512];
        char video_path[512];
        char real_url[128];
        int gun_camera_ip;
        int obj_dir;
    };

    //算法相关参数

    struct domeAlgConfig {
        unsigned short m_linkage_saveVideo; //1:保存录像 0：不保存录像
        unsigned short m_linkage_snapNumber; //抓拍张数
        double m_linkage_sampleTime; //图片抓拍间隔时间(默认为1.0秒；  >0.0 表示启用此条件，==0.0表示不用此间隔时间，使用其他方法) 
        double m_linkage_focusRect_delayTime; //相机发送转向预置位指令到发送居中放大指令 之间最大延迟时间
        double m_linkage_snap_delayTime; //相机发送居中放大指令到拍照 之间 最大延迟时间(让相机操作完框选放大)

        double m_linkage_left_speed_rate; //同一目标在枪球两个画面当中的速度比例(球机在预置位处)
        double m_linkage_right_speed_rate; //同一目标在枪球两个画面当中的速度比例(球机在预置位处)

        double m_linkage_left_speed_rate_1; //同一目标，在抢球两个画面中的速度比例(球机是第二次放大)
        double m_linkage_right_speed_rate_1; //同一目标，在抢球两个画面中的速度比例(球机是第二次放大)

        int m_linkage_left_zoom_dif; //同一目标，第一次居中放大时，放大倍数偏移量
        int m_linkage_right_zoom_dif; //同一目标，第一次居中放大时，放大倍数偏移量

        int m_linkage_left_zoom_dif_1; //同一目标，第二次居中放大时，放大倍数偏移量
        int m_linkage_right_zoom_dif_1; //同一目标，第二次居中放大时，放大倍数偏移量

        double m_linkage_left_boathead_l; //船在左侧时，第一次放大居中后，船舱的相对位置
        double m_linkage_left_boathead_t;
        double m_linkage_left_boathead_r;
        double m_linkage_left_boathead_b;

        double m_linkage_right_boathead_l; //船在右侧时，第一次放大居中后，船舱的相对位置
        double m_linkage_right_boathead_t;
        double m_linkage_right_boathead_r;
        double m_linkage_right_boathead_b;
    };

    //获取配置文件的相关参数

    struct configurationFile {
        unsigned char get_p_done;
        unsigned char get_p_init;

        long get_start;
        long get_end;
        long get_index;
        long get_delay;

        unsigned char get_t_u_done;
        unsigned char get_t_u_init;
        unsigned char get_t_d_done;
        unsigned char get_t_d_init;
        unsigned char get_z_done;
        unsigned char get_z_init;
    };

    struct Domelinkage_item {

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
            /* 所属主机的ip地址,用于集群,本机为0 */
            unsigned int _ipv4;
            /* 本对象的uuid值,一般是从配置中读入的 */
            char *_uuid;
            /* 所属的记录对象 */
            struct Domelinkage *_pDomelinkage;
            /* 传入的数据帧链表 */
            FsObjectList *__framelistIn;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
#define  Domelinkage_item_ClusterLen  (FsPointer_to_long(&((struct Domelinkage_item*) 0)->ro._sum)+sizeof(((struct Domelinkage_item*) 0)->ro._sum))
            /* 整个通道依赖的所有数据的校验和 */
            unsigned long long _sum;
            /* 连接和调用外部命令所需要的同步锁 */
            pthread_mutex_t __mutexCmdConnect;
            /*图像数据处理缩小倍数,1920*1080:缩小16倍*/
            unsigned int zoomTimes;
            /*算法处理帧率：原帧率的1/1;1/2;1/3;*/
            int m_linkageframeInterval; //

            FsObjectList* __gunDataClientList;
            /* 实时视频掩码通信指针 */
            void *__realMask;
            /* rtsp实时视频掩码 */
            unsigned int _maskIndex;

            /* 文字叠加 */
            FsTypeFace *__pTypeFace;

            domeAlgConfig m_dome_alg_config; //算法处理相关参数 

            calibrationConfig m_calconfig[MAX_GUN_COUNT]; //标定参数
            struct DomeDeviceConfig m_config_devinfo; //相关设备信息
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

            double v_linkage_incStartTime; //卡口事件开始时间
            double v_linkage_incEndTime; //卡口事件结束时间    

            unsigned int v_linkage_checkPos_ready; //检查相机是否转到预置位处(>3)
            unsigned int v_linkage_checkPos_unready; //连续多少次检测都没有回到预置位，建议再发送一次回到预置位指令

            char v_linkage_curr_gunuuid[64]; //记录当前联动枪机的uuid
            unsigned int v_linkage_curr_gunIndex; //记录当前关联枪机的序号
            unsigned int v_linkage_curr_calIndex; //记录相机当前转向的是哪个标定的位置[球机多个标定位置情况]

            unsigned int v_linkage_gotoPTZ_flag[SNAP_PIC_COUNT]; //调用gotoPTZ指令标志，==1表示已调用该指令；==0表示没有调用该指令
            double v_linkage_gotoPTZ_startTime[SNAP_PIC_COUNT]; //记录上一次调用gotoPTZ指令的时间；用于调用居中放大指令的时间
            double v_linkage_checkPTZ_startTime; //记录上一次获取相机ptz的时间，程序固定隔0.3秒获取一次相机ptz值

            unsigned int v_linkage_focusRect_flag[SNAP_PIC_COUNT]; //调用居中放大指令标志，==1表示已调用该指令；==0表示没有调用该指令
            double v_linkage_focusRect_startTime[SNAP_PIC_COUNT]; //记录上一次调用gotoPTZ指令的时间；用于调用拍照指令的时间

            unsigned int v_linkage_snap_finish[SNAP_PIC_COUNT]; //图片抓拍结束标记

            unsigned int v_linkage_times; //联动次数,初始值为配置参数，然后拍一张就递减1

            unsigned int v_linkage_snap_times; //抓拍张数,初始值为配置参数，然后拍一张就递减1

            unsigned int v_linkage_gobackPTZ_flag; //拍照结束，返回预置位
            double v_linkage_snap_lastTime; //上一次拍照时间
            double v_linkage_snap_intervalTime; //拍照间隔时间


            Camera_ctrl_tcp_item* pCamera_ctrl_item; //相机控制

            FsObjectImageFrame *_ppFrame[SNAP_PIC_COUNT]; //缓存的数据帧指针
            domeParamConfig m_domeparamcfg; //附加信息由枪机传递过来的

            configurationFile m_configuration_file;

            int m_debug_snapflag[SNAP_PIC_COUNT]; //调试使用
            int m_debug_number; //调试使用
            double m_debug_left; //调试使用
            double m_debug_right; //调试使用
            double m_debug_top; //调试使用
            double m_debug_bottom; //调试使用

            double m_debug_adjust_left; //调试使用
            double m_debug_adjust_right; //调试使用
            double m_debug_adjust_top; //调试使用
            double m_debug_adjust_bottom; //调试使用

            int m_debug_boat_height; //调试使用    
            int m_debug_boat_fouce_height; //调试使用
            int m_debug_boat_width; //调试使用    
            int m_debug_boat_fouce_width; //调试使用

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

            /* 数据记录的指针 */
            struct Record_item *pRecord_inc_item;
            Record_item_add_inc_pthreadSafety__OI_2 record_inc_item_add_partVideo__OI_2;

            /* 采集对象，不为空表示有效 */
            struct Capture_item* pCapture_item;

            struct GunData *pGunData;
            double gunDataTimeout;
        } p;
    };



    /* 
     * 创建一个新的Balllinkage对象;
     * 返回Balllinkage指针.
     */
    struct Domelinkage * domelinkage_new__IO(/* 线程名 */const char threadName[]
            , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus
            , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* 配置管理对象 */ void *pConfigManager
            , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut);

    /* 删除pVlinkage指向的实例对象 */
    void domelinkage_delete__OI(struct Domelinkage* pDomelinkage);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void domelinkage_startThread(struct Domelinkage* pDomelinkage,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程 */
    void domelinkage_stopThread(struct Domelinkage* pDomelinkage);
    /* 传入数据帧 */
    void domelinkage_item_frame_in_pthreadSafety__OI_4(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
    unsigned int domelinkage_item_frame_out_pthreadSafety__IO_4(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index
            , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);
    /* 获取数据帧的数量 */
    unsigned int domelinkage_item_get_frame_count_pthreadSafety(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index);
    void domelinkage_createConfig(FsConfig * const pConfig, void * parent);
    /* 检查pDomelinkage模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
    int domelinkage_check_channel_changed(struct Domelinkage * const pDomelinkage, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);
    /* 重置模块内通道对应的开方分组 */
    void domelinkage_item_reset_groupSqrtOut(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index
            , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
#ifdef __cplusplus
}
#endif

#endif /* VGBLINKAGE_H */

