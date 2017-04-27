/* 
 * File:   capture.h
 * Author: fslib
 *
 * Created on 2016年3月24日, 上午11:19
 */

#ifndef CAPTURE_H
#define	CAPTURE_H
#include "../../PublicTools/publicDefine.h"
#include <sys/time.h>
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Thread/Rtmp.h"
#include "../../PublicTools/Image/ImageFrame.h"
#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,2)

    /* 采集 */
    struct Capture {

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
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 通道链表,节点仅在控制线程可读 */
            FsObjectList *__itemList;
            /* 管道 */
            int __pipe[2];
            /* 每个线程最多处理的通道数 */
            unsigned short _channelCountPerThread;
            /* rtmp客户端对象 */
            Rtmp *_pRtmpClient;
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
            /* 集群的通道链表 */
            FsObjectList *clusterItemList;
        } p;
    };

    /* 采集项 */
    struct Capture_item {

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
            struct Capture *_pCapture;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
            /* 对应的相机对象链表,为空表示是本地数据推送 */
            FsObjectList *_pCameraList;
            /* 相机项 */
            void *__pCamera_item;
            /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
            void (*_camera_remove_and_delete_item_and_emptyCamera)(void* const pCameraList, void *const pCamera_item);
            /* 控制相机流的函数指针 */
            void (*_streamControl)(void *const pCamera_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
            /* 控制 */
            struct Camera_ctl_function *_ctl_function;


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
            int shot_type; //镜头类型(正对中光学)，0：正比例(z值越大，场景拉得越近，z值越小场景拉得越远)  1：反比例(相反)
            char camera_ip[25]; //相机IP
            unsigned short camera_port; //相机端口
            struct timeval camera_tv; //云台socket等待时间
            int camera_type;
            int camera_socket;

        } p;
    };
#pragma pack(pop)
    /* 创建配置 */
    void capture_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * const parent);
    /* 
     * 创建一个新的Capture对象;
     * 返回Capture指针.
     */
    struct Capture *capture_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
            /* 配置管理对象 */ void *const pConfigManager, /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread, /* rtmp客户端对象 */ Rtmp * const pRtmpClient);
    /* 删除pCapture指向的实例对象 */
    void capture_delete__OI(struct Capture * const pCapture);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void capture_startThread(struct Capture* pCapture,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void capture_stopThread(struct Capture* pCapture);
    /* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
    FsObjectImageFrame *capture_item_frame_out_pthreadSafety_IO(/* 采集项,可为空 */ struct Capture_item* pCapture_item);
    typedef void (*capture_loaclPush_frame_pthreadSafety)(/* 本地推送相机项 */ void *const plocalPush_item, FsObjectImageFrame * const pFrame, /* 上一个采集的索引 */ const void* const src);


#ifdef	__cplusplus
}
#endif

#endif	/* CAPTURE_H */

