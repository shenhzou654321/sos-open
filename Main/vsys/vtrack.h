/* 
 * File:   vtrack.h
 * Author: leo
 *
 * Created on 2016年6月15日, 上午11:19
 * 
 * 
 * 
 */

#ifndef TRACK_H
#define TRACK_H

#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Media/VideoMKV.h"
#include "../../Modules/track/v_search.h"
#include "../../Modules/track/v_Track.h"
#include "../../Modules/track/v_findLine.h"
#include "../../Modules/camera_ctrl/HCNetSDK.h"
#include "../../PublicTools/Fs/TypeFace.h"
#include "../../Modules/trackcar/capturevehicle.h"
#include "../../Modules/trackcar/v_trackcar.h"
#include "../../Modules/recon/wave.h"
#include "record.h"
#include "../../Modules/track/vibe_proc.h"
#include "../../Modules/derelict/CamDetect.h"
#include "../../Modules/derelict/MetroDLL.h"
#include "../../Modules/camera_ctrl/camera_ctrl_tcp.h"

#define  Hiki_ENABLE  

///////////////////////////////
//是否打开调试信息：
//#define    PRINT_DetectionDir

//#define  PRINT_DEBUG  
//#define  PRINT_DEBUG_1

#define  SHORE_INC_COUNT   5//靠岸检测的预置位个数
#define  VIDEO_DETECT_COUNT   5//视频检测支持最大个数的预置位
/////////////////////////////


#ifdef __cplusplus
extern "C" {
#endif
#define Vtrack_Mask (/* 雷达跟踪 */0x1llU|/* 视频自动跟踪 */0x4llU)

    /* 数据记录 */
    struct Vtrack {

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

    //云台控制相关变量

    struct HolderConfig {
        //云台socket连接
        //    char m_holder_ip[32]; //云台控制端口
        //    int m_holder_port; //云台控制端口
        //    int m_holder_port_1; //云台控制端口
        //    int m_holder_type; //云台类型(海康球机：2，光电仪：1)  
        //    char m_holder_name[64]; //用户名和密码：海康使用
        //    char m_holder_pwd[64];

        int m_holder_speed; //云台转动速度(上下左右都使用最慢的速度：1)
        int m_holder_time; //云台转动持续时间(暂时不用)

        int m_p; //云台回到预置的ptz位置                    
        int m_t;
        int m_z;

        //x轴方向上：
        int m_holder_forceStopX; //当发送云台转动命令后，强制在几帧内发送停止命令
        int m_holder_forceStopX_1; //两次云台转动命令的间隔帧数(至少)

        //y轴方向上：
        int m_holder_forceStopY; //当发送云台转动命令后，强制在几帧内发送停止命令
        int m_holder_forceStopY_1; //两次云台转动命令的间隔帧数(至少)

        int m_holder_keeptouchFrames; //与云台保持连接的最少帧数，在间隔多少帧内，必须向云台发送一个信号(用停止转动命令来代替心跳信号) 

        int m_cond_dir; //运动方向判断条件: 当跟踪当前坐标与初始坐标之间位移大于此阈值时再判断方向
        int m_cond_start; //相机联动，转动条件：当跟踪当前坐标与初始坐标之间位移大于此阈值时，发送转动命令
        int m_cond_stop; //暂不用 //相机联动，停止条件：当跟踪当前坐标与初始坐标之间位移大于此阈值时，发送停止转动命令
    };

    struct TrackConfig {
        int m_trackWidth; //跟踪框的大小
        int m_trackHeight; //跟踪框的大小

        int m_trackSearchWidth; //跟踪框搜索范围的大小
        int m_trackSearchHeight; //跟踪框搜索范围的大小

        int m_trackdelay; //延迟跟踪时间

        int m_trackOverFlag1; //根据结束条件1：轮船在水平方向上反方向移动距离
        int m_trackOverFlag2; //根据结束条件2：轮船在垂直方向上反方向移动距离
        int m_trackOverFlag3; //根据结束条件3：云台在阈值时间内不转动(10秒)
        int m_trackOverFlag4; //根据结束条件4：云台在阈值时间内不转动(30秒)
        float m_trackOverTime; //根据结束条件4：跟踪强制结束时间

        int m_trackOverPTZ; //跟踪结束后，是否回到预置位： 0：不回到预置位  1：回到预置位      
        int m_trackAgainTimes; //跟踪结束后，是否需要再次跟踪(次数)


        //        int m_boat_linewidth; //目标跟踪框的边框宽度
        //        int m_boat_linecolor; //跟踪框的颜色：1：红色，2：蓝色 3：绿色
        //        int m_boat_linerate; //比例     
    };

    struct OverlyConfig {
        int m_overly_line; //是否叠加跟踪框
        int m_overly_tracknum; //是否叠加目标编号

        int m_overly_linewidth; //目标跟踪框的边框宽度
        int m_overly_linecolor; //跟踪框的颜色：1：红色，2：蓝色 3：绿色
        int m_overly_linerate; //比例     

        int m_overly_tracknum_width;
        int m_overly_tracknum_height;
        int m_overly_tracknum_color;
        int m_overly_tracknum_pos;

    };


    //输出结果

    struct DebugConfig {
        int savelog_flag; //0：不保存打印日志   1：保存打印日志
        char savelog_path[256]; //日志存储路径(绝对路径)

        int savepic_flag; //0：不保存调试图片 1：保存调试图片
        char savepic_path[256]; //保存调试图片路径(绝对路径)  

        int m_change_z; //可以通过此参数调整控制器传过来的ptz参数
        int m_change_p;
        int m_change_t;

        int m_opendebug; //是否开启调试模式[隔帧叠加跟踪框，叠加跟踪框]
    };

    //输出结果

    struct Result_item {
        int m_flag;

        double incStartTime; //跟踪开始，结束时间
        double incEndTime;
    };

    struct HIKI_item {
        char m_szCameraIP[64]; //IP��
        char m_szControlIP[64];
        char m_szCameraUid[64];
        char m_szCameraPwd[64];
        int m_nUserID;
        int m_nControlChannel;
        unsigned int m_nCameraPort;
        NET_DVR_DEVICEINFO_V30 m_sDeviceInfo;
    };

    struct ObjInfo {
        long long tracknum; //雷达目标编号
        double lat; //雷达目标第一次经纬度
        double lon;
        double p;
        double t;
        double z;
        double time; //雷达目标第一次检测时间

        int m_false_alarm_count; //判定一个雷达目标为虚景的次数(跟踪结束后对该值进行赋值)
    };

    //---------------------------------------------------------
    //视频检测船只位置

    struct videoDetectionPos {
        int isused; //是否在使用(0:未使用；1：在使用，但可以中断 2：锁定，无法中断)
        double startTime; //跟踪开始时间
        int isready; //是否转向到位(0：初始值，>50：为转向到位的值)

        int search_p; //球机进行视频检测的ptz位置
        int search_t;
        int search_z;

        int left; //在该ptz位置下，画面检测的范围	
        int right;
        int top;
        int bottom;

        int modw; //在该ptz位置下，画面最小目标尺寸
        int modh;
    };

    //白昼检测参数

    struct checkdayParam {
        int daynight; //0:白天 1：夜间
        int daycount; //检测白天计数
        int nightcount; //检测夜间计数       
    };

    //视频检测船只参数

    struct videoDetectionConfig {
        int m_camtpye; //0：枪机，1：球机
        int m_valid; //视频检测是否有效，1 有效 0 无效
        int m_upload; //是否上传数据至控制器
        int m_devnumber; //设备编号(三位：第一位是区域，第二，三位是相机编号)
        int m_ptz_count; //预置位检测个数
        int m_polltype; //是否需要轮循
        int m_polltime; //轮循间隔时间
        int m_avegray; //图像平均灰度阈值，小于此阈值，不进行视频自动检测(== 0：不做检测，> 0; 作检测)
        int m_close_starttime; //关闭视频自动检测开始时间
        int m_close_endtime; //关闭视频自动检测结束时间

        int vibe_obj_keepTime; //vibe检测到目标后，目标存在的最短时间
        int vibe_obj_moveMinDis; //vibe检测到目标，目标移动超过最短距离后才判定为目标

        int vibe_obj_locked_moveMinDis; //vibe检测到目标，开始跟踪后，目标移动超过最短距离后，才发送锁定成功

        int m_objminWidth; //目标大小大于此阈值才输出
        int m_objminHeight;

        int m_objmaxWidth; //目标大小大于此阈值才输出
        int m_objmaxHeight;

        int m_cloud_minWidth; //云阴影导致的错误目标 
        int m_cloud_minHeight;

        int m_keepTime; //目标最短存在时间（秒)
        int m_moveth; //背景差阈值
        int m_camera_movevalid; //相机是否支持预置位检测
        int m_camera_movetime; //相机离开预置位多长时间，自动转回

        int m_floater_valid; //漂浮物检测是否有效(0:无效，1:有效)
        int m_watergrass_factor; //检测水草特征因子(0:不做判断； >0:进行水草判断)
        int m_floater_group_factor; //漂浮物群体特征值(个数多，运动轨迹一致)

        int m_re_light_strength; //反射光线的强度(0-255)
        int m_re_light_rate; //强光点个数占套框的比例

        int m_time_interval; //同一点，前后两次告警时间间隔(单位:秒)

        videoDetectionPos m_pos[VIDEO_DETECT_COUNT]; //检测位置参数[枪机默认取第一个数据]
        //vibeParam  m_vibeparam;
    };



    //相关设置的参数信息

    struct DeviceInfoConfig {
        char m_camera_ip[25]; //相机IP
        unsigned short m_camera_port; //相机端口
        char m_szCameraUid[64]; //相机注册用户名
        char m_szCameraPwd[64]; //相机注册密码

        char m_camera_ctrl_ip[25]; //相机控制IP(云台控制IP)
        unsigned short m_camera_ctrl_port; //相机控制端口(云台控制端口)

        unsigned int CapImageWidth; //相机采集图像宽
        unsigned int CapImageHeight; //相机采集图像高

        unsigned short m_camera_model; //相机型号(0：白光，1：红光) //相机型号  0:"白光球机" 1:"红外球机" 2:"白光枪机" 3:"红外枪机"  4:"白光光电仪"  5:"红外光电仪"     
        unsigned short m_camera_brand; //相机品牌：0, "海康威视" 1, "中光学" 2, "上赛"  3，"霍尼韦尔"

        char m_camctl_serverip[25]; //相机控制器服务器ip
        unsigned short m_camctl_serverport; //相机控制器服务器监听端口(默认:8888)
    };

    //---------------------------------------------------------
    //靠岸检测的相关参数

    struct IncAshore {
        int m_ashore_type; //是否需要靠岸检测，0：不需要 1：需要
        int m_ashore_delay; //延迟标记，初始化为-1，当相机转向预置位后，将标记置为0。当标记为0时，才开始计数自加，计数大于25*50开始视频检测

        int m_ashore_search_p[SHORE_INC_COUNT]; //当球机转到设置位置时(此位置，岸线在画面的左侧边缘)，目标移动到画面左侧，即可代表目标靠岸            
        int m_ashore_search_t[SHORE_INC_COUNT];
        int m_ashore_search_z[SHORE_INC_COUNT];

        int m_ashore_search_left[SHORE_INC_COUNT]; //运动目标检测区域
        int m_ashore_search_right[SHORE_INC_COUNT];
        int m_ashore_search_top[SHORE_INC_COUNT];
        int m_ashore_search_bottom[SHORE_INC_COUNT];

        int m_ashore_p[SHORE_INC_COUNT]; //靠岸时的ptz         
        int m_ashore_t[SHORE_INC_COUNT];
        int m_ashore_z[SHORE_INC_COUNT];

        int m_ashore_objw[SHORE_INC_COUNT]; //岸线目标的尺寸阈值
        int m_ashore_objh[SHORE_INC_COUNT];

        int m_ashore_objnum; //岸线目标的编号(时间+id)
        int m_ashore_path[512]; //岸线目标的录像存储路径

        int m_ashore_dir; //运动方向： 1：向左  2：向右 
    };

    /* 记录项 */
    struct Vtrack_item {

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
            struct Vtrack *_pVtrack;
            /* 传入的数据帧链表 */
            FsObjectList *__framelistIn;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
#define  Vtrack_item_ClusterLen  (FsPointer_to_long(&((struct Vtrack_item*) 0)->ro._sum)+sizeof(((struct Vtrack_item*) 0)->ro._sum))
            /* 整个通道依赖的所有数据的校验和 */
            unsigned long long _sum;
            /* 连接和调用外部命令所需要的同步锁 */
            pthread_mutex_t __mutexCmdConnect;
            /* 文字叠加 */
            FsTypeFace *__pTypeFace;

            /*图像数据处理缩小倍数,1920*1080:缩小16倍*/
            unsigned int zoomTimes;
            //间隔帧处理数据(2：原帧率的一半，3：原帧率的1/3，其他数值：原帧率)
            unsigned int m_trackframeInterval;


            int m_camctrl_flag; //1：表示是建立了链接的
            int m_camcrl_requesID_3[3]; //保存控制器的相关信息(回执信息使用)
            unsigned int m_camcrl_head;
            unsigned int m_camcrl_requesID_data[5]; //保存控制器的相关信息(回执信息使用)

            //系统设置相关结构数据：
            DebugConfig m_config_debug; //调试参数
            OverlyConfig m_config_overly; //图像叠加配置参数
            HolderConfig m_config_holder; //云台相关变量
            TrackConfig m_config_track; //跟踪相关参数
            vSeachConfig m_config_search; //运动检测参数
            videoDetectionConfig m_config_videoDect; //视频检测设置参数
            DeviceInfoConfig m_config_devinfo; //相关设备信息

            int m_compressRate; //获取一张jpg图片，压缩因子(1-100)

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

            int v_gotoptz; //转向ptz
            float v_obj_p; //转向的ptz方向
            float v_obj_t;
            float v_obj_z;
            double v_obj_lat; //雷达目标第一次经纬度
            double v_obj_lon;
            int v_obj_alarmsite; //目标告警地点类型1：防区告警，2：岸线提示，3：岸线告警
            int v_trackmodel; //跟踪模式

            float v_current_p; //当前相机的ptz值
            float v_current_t;
            float v_current_z;

            int v_autofocus; //自动聚焦一次
            int v_waitcount; //等待计数
            int v_iscamready;
            int v_waitforcamera; //等待相机准备好
            int v_findObj; //是否发现目标： 初始值：0 ， 当为1时，表示可以调用算法处理    
            int v_trackingObj; //是否正在跟踪，初始值：0，当跟踪初始化完后，置为1
            int v_locktrackObj; //是否锁定跟踪目标，初始值：-1  当为1时，锁定成功,； 当为0时，锁定失败;
            int v_handleToStop; //手动停止标记，此标记为1，在主线程中去保存跟踪录像操作
            int v_handleToStop_1; //手动停止标记，此标记为1，在主线程中去保存跟踪录像,并将球机转到阈值位操作
            long long v_trackNum; //目标跟踪号(用于叠加)(从控制器获取)
            int v_hiki_unConnect; //海康球机：第一次连接成功后，将其置为0，如果向海康球机发送数据失败后，将其置为1。

            float max_pfs; //最大帧率

            double v_incStartTime; //跟踪开始时间
            double v_incEndTime; //跟踪结束时间
            char v_videoPath[612]; //跟踪录像保存路径  
            char v_videoPath_1[612]; //跟踪录像保存路径  

            double v_lastIncTime; //上一次告警时间(发送结束跟踪时，获取)  

            pthread_mutex_t m_log_mutex; //日志保存互锁

            //目标定位        
            vSeach m_vsearch; //运动检测需要的内部数据结构

            vProc g_vproc; //跟踪中间数据

            int v_movedir; // 0:初始值，1：向左转，2：向右转
            int v_findmove; // 是否发现目标移动，初始值：0， 当为1时，表明定位到合适运动块，可以开始跟踪
            int v_max_x; // 最佳运动块的坐标
            int v_max_y;

            int v_avegray; //平均灰度

            int v_assistfind; //调用了辅助方法的

            int v_litter; //定位运动块是否是垃圾

            int v_effective; //在寻找运动块算法中，定位船的轮廓是否有效：0 无效  1：船体在左侧   2：船体在右侧
            int v_boatheight; //大致定位船的高度
            int v_boat_left; //大致定位船的左边界
            int v_boat_right; //大致定位船的右边界
            int v_boat_bottom_y; //船的底部位置
            int v_boat_enterall; //船已经全部进入图像
            int v_changeBoatpos_flag; //当船的位置不是太好时(太靠近两侧)，会先改变一次初始坐标

            int v_adjustIndex; //当目标初始检测位置太靠近两侧时，用到这两个参数(v_adjustIndex = 0)
            int v_adjustpos; //v_adjustpos 初始值：-1   0：不需要再调整位置了， 1：需要继续调整位置

            //目标跟踪
            TrackParam pm[MAX_OBJ]; //目标跟踪结果数据结构体

            int v_boat_rect_l; //绘制目标跟踪框
            int v_boat_rect_r;
            int v_boat_rect_t;
            int v_boat_rect_b;
            int v_obj_number; //跟踪目标序号
            int v_trackobj_times; //跟踪次数

            int v_rightMoveSteps;
            int v_rightMoveDistance;
            int v_leftMoveSteps;
            int v_leftMoveDistance;

            float v_reduce_rate; //船底长度的减小率

            //云台联动相关参数：     
            int v_holder_lastConnectdx; //上一次向云台发送命令的帧号：为了保持跟云台的连接，当连接上云台后，我们通过向云台发送停止命令来作为心跳信号
            int v_holder_lastctrlDx; //上一次控制云台的帧号，记录上一次转动的帧号
            int v_holder_lastmoveYDx; //上一次转动帧号(上下方向)  
            int v_holder_lastmoveXDx; //上一次转动帧号(左右方向)
            struct timeval v_holder_tv; //云台socket等待时间
            int v_holder_socket; //云台连接socket
            int v_holder_socket1; //云台连接socket1
            int v_holder_forcestop; //强制停止转动，某些时候，即使符合转动条件，也可以这个变量的强制不转动
            int v_holder_incmove; //增加或减少移动时间(机制：通过配置假设每次转动时间为40ms，但船移动太快，相机转动太慢时，可以通过此变量来增加转动时间)
            int v_holder_notmatchCnt;
            int v_holder_notmatch[6];

            float v_holder_p[2]; //缓存前两次获取的云台的ptz值(暂不用)
            float v_holder_t[2];
            float v_holder_z[2];
            int v_holder_getptzcnt; //获取ptz值的次数(暂不用)

            int v_camctrl_dir; //相机转动方向：1：向左 2：向右(发送指令使用)
            int v_camctrl_dirY; //相机转动方向：1：向上 2：向下(发送指令使用)
            int v_camctrl_movingX; //x轴方向上的移动标记： 0：停止移动  1：开始移动
            int v_camctrl_movingY; //y轴方向上的移动标记： 0：停止移动  1：开始移动                   

            ObjInfo m_objInfo[20]; //缓存20个；
            int m_isfalsealarm; //是否为虚景

            HIKI_item m_hikiCamera; //海康球机控制

            int m_autotrack_status; //相机视频自动检测状态
            int m_videotrack_ready; //判定相机是否在预置位上，如果在才进行视频自动检测
            //int         m_videotrack_checkptz_diftime;  //判定相机是否在预置位上的，时间间隔
            boatInfo m_boatinfo; //vibe检测船舶输出信息
            VibeParam m_vibeparam; //vibe检测船舶[中间变量]
            vibe_inParam m_vibe_inparam; //vibe输入参数
            int m_record_vibe_obj;
            int m_autotrack_handle_obj; //告知控制器是否处理(保存到大数据)自动检测到的目标

            //相机移动检测
            float v_cammoved_last_p; //相机移动后，上一次ptz
            float v_cammoved_last_t;
            float v_cammoved_last_z;
            unsigned int m_cammoved_flag; //相机被移动的时间(用帧数来统计)    
            double m_cammoved_start_time; //相机移动开始时间

            int m_autotrack_aveGray;

            int m_water_grass_count; //套框区域水草像素点的数量

            checkdayParam m_checkdayparam;

            Camera_ctrl_tcp_item* pCamera_ctrl_item; //相机控制   
            /////////////////////////////////////////////////////////////
            //for test
            /*
            CAPV theCapture1;           //创建车辆检测对象
            int  theCapInit;            //是否初始化
            TrackCarParam   trackcarparam[30];     //
            
            CWave m_wave;   
            unsigned char waveImage[600*600];
            
            vParam camDetectParam;
            vOutData  camOutData;
             */
            //vWaveParam m_waveparam;     //是否是水波纹
            ///////////////////////////////////////////////////////////////
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

    /* 创建配置 */
    void vsearch_createConfig(FsConfig * const pConfig, void * parent);
    void vtrack_createConfig(FsConfig * const pConfig, void * parent);
    /* 检查pVtrack模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
    int vtrack_check_channel_changed(struct Vtrack * const pVtrack, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);
    void holder_createConfig(FsConfig * const pConfig, void * parent);
    void debug_createConfig(FsConfig * const pConfig, void * parent);
    /* 
     * 创建一个新的vtrack对象;
     * 返回vtrack指针.
     */
    struct Vtrack * vtrack_new__IO(/* 线程名 */const char threadName[]
            , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus
            , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* 配置管理对象 */ void *pConfigManager
            , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut);

    /* 删除pvtrack指向的实例对象 */
    void vtrack_delete__OI(struct Vtrack* pVtrack);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void vtrack_startThread(struct Vtrack* pVtrack,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程 */
    void vtrack_stopThread(struct Vtrack* pVtrack);
    /* 传入数据帧 */
    void vtrack_item_frame_in_pthreadSafety__OI_4(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
    unsigned int vtrack_item_frame_out_pthreadSafety__IO_4(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index
            , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);
    /* 获取数据帧的数量 */
    unsigned int vtrack_item_get_frame_count_pthreadSafety(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index);

    /* 重置模块内通道对应的开方分组 */
    void vtrack_item_reset_groupSqrtOut(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index
            , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
#ifdef __cplusplus
}
#endif

#endif /* RECORD_H */

