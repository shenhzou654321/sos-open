/* 
 * File:   camera_rtmp.h
 * Author: fslib
 *
 * Created on 2013年01月04日, 上午11:19
 */

#ifndef CAMERA_RTMP_H
#define	CAMERA_RTMP_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Media/VideoH264.h"
#include "../../PublicTools/Thread/Rtmp.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/camera_ctrl.h"


#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,4)

    struct Camera_rtmp_item;

    /* Rtmp相机 */
    struct Camera_rtmp {

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
            /* 处理的相机链表 */
            FsObjectList *__cameralist;
            /* 线程是否在运行(0:没有运行,1:在运行) */
            unsigned char inRun : 1;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 管道,用于添加和删除文件或发送数据 */
            int __pipe[2];
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
            /* 外部和内部线程异步通信的链表,结构为通道指针+参数(首字节低3位建议为:0-client添加,1-client移除,2-client开始,3-client停止,4-server添加,5-server移除),大小固定为两个指针大小 */
            FsStructList *__controlList;
        } rw;

        struct {
        } p;
    };

    /* Rtmp相机项 */
    struct Camera_rtmp_item {

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
            /* 数据流掩码,1-视频,2-音频,4-相机控制 */
            unsigned int _streamMask;
            /* 域名,可为空 */
            char *_domain;
            /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* 用户名,可为空 */
            char* _username;
            /* 密码,可为空 */
            char* _password;
            /* url地址,可为空 */
            char *_url;
            /* uuid,通道标识,可为空 */
            char *_uuid;
            /* rtmp采集对象 */
            struct RtmpClient_item *__pRtmpClient_item;            
            /* 收到数据后的回调函数,成功返回1,失败返回-1 */
            signed char (*_cb)(FsObjectH264 *pH264, /* 帧率 */const float frameRate, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem);
            /* 外部的传入指针 */
            void *_externP;
            /* 收到的数据链表 */
            FsObjectList *__frameList;
            /* 互斥锁 */
            pthread_mutex_t __mutex;
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 帧率,小于0.001为无效值 */
            float frameRate;
            /* 帧率,小于0.001为无效值,统计帧率 */
            float frameRate0;
            /* 用于帧率统计 */
            unsigned short frameRate_count;
            /* 用于帧率统计 */
            double frameRate_lastTime;
        } ro;

        struct {
        } rw;

        struct {
            /* 是否繁忙,1-繁忙,2-空闲 */
            unsigned char busy;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除,(controlStatus&0x2)-表示是否开始实时流 */
            unsigned char controlStatus;
            /* 上一次接收数据的时间,gmt时间,用于校正本次数据的的采集时间,以让数据均匀 */
            double recvTimeGmt_last;
            /* 所属的rtsp客户端对象 */
            struct Camera_rtmp *pCamera_rtmp;
            /* 控制掩码,指向真实的控制掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */
            unsigned long long *ctrl_mask;
            /* 状态,指向真实的控制状态 */
            unsigned char *ctrl_status;
            /* 相机类型是否已确认,0-表示未确认,1-已确认 */
            unsigned char ctrl_type_checked;
            /* 控制相机类型:0-sos相机,1-为海康http相机 */
            unsigned char ctrl_type : 7;
            /* 下一次的类型检查时间 */
            double ctrl_typeCheckTime;
            /* 控制句柄 */
            void *ctrl_p;
            /* 删除控制句柄的函数指针 */
            void (*delCtrl_p)(void *ctrl_p);
            /* 控制函数 */
            struct Camera_ctl_function *ctl_function;
        } p;
    };
#pragma pack(pop)
    /* 
     * 创建一个新的Camera_rtmp对象;
     * 返回Camera_rtmp指针.
     */
    struct Camera_rtmp *camera_rtmp_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
            /* rtmp客户端对象 */ Rtmp * const pRtmpClient);

    /* 删除pCamera_rtmp指向的实例对象 */
    void camera_rtmp_delete__OI(struct Camera_rtmp * const pCamera_rtmp);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void camera_rtmp_startThread(struct Camera_rtmp * const pCamera_rtmp,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
    /* 结束线程 */
    void camera_rtmp_stopThread(struct Camera_rtmp * const pCamera_rtmp);
    /* 
     * 创建一个新的Camera_rtmp_item对象;
     * 返回Camera_rtmp_item指针.
     */
    struct Camera_rtmp_item *camera_rtmp_item_new__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask, /* 域名,可为空 */ const char domain[],
            /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port,
            /* 用户名 */const char username[], /* 密码 */const char password[], /* url地址 */const char url[], /* uuid,通道标识,可为空 */const char uuid[],
            /* 收到数据后的回调函数,成功返回1,失败返回-1 */ signed char (*cb)(FsObjectH264 *pH264, /* 帧率(小于0.001表示未知) */const float frameRate, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem),
            /* 外部传入的指针 */void *externP);
    /* 
     * 创建一个新的Camera_rtmp_item对象;
     * 成功返回Camera_rtmp_item指针;
     * 失败返回NULL.
     */
    struct Camera_rtmp_item *camera_rtmp_item_new_byURL__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask, /* url地址,如rtsp://admin:admin@127.0.0.1:554/video */const char url[],
            /* uuid,通道标识,可为空 */const char uuid[],
            /* 收到数据后的回调函数,成功返回1,失败返回-1 */signed char (*cb)(FsObjectH264 *pH264, /* 帧率 */const float frameRate, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem),
            /* 外部传入的指针 */void *externP);
    /* 删除pCamera_rtmp_item指向的实例对象 */
    void camera_rtmp_item_delete__OI(struct Camera_rtmp_item * const pCamera_rtmp_item);
/* 控制流开始停止,必须在添加到服务后才能调用此函数 */
    void camera_rtmp_item_streamControl_pthreadSafety(struct Camera_rtmp_item * const pCamera_rtmp_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
    /* 向pCamera_rtmp中添加一个连接项 */
    void camera_rtmp_add_item_pthreadSafety__OI_2(struct Camera_rtmp * const pCamera_rtmp, struct Camera_rtmp_item * const pCamera_rtmp_item);
    /* 从pCamera_rtmp中移除一个连接项 */
    void camera_rtmp_remove_item_pthreadSafety__IO_2(struct Camera_rtmp * const pCamera_rtmp, struct Camera_rtmp_item * const pCamera_rtmp_item);
    /* 从pCamera_rtmp中移除一个连接项并删除,如果相机链表为空,把此相机也删除,并把相机指针置0 */
    void camera_rtmp_remove_and_delete_item_and_emptyCamera(/* 不为空时,且pCamera_rtmp_item所属的相机对象内没有相机项时,把pCamera_rtmp_item所属的相机对象从此链表中移除 */FsObjectList * const pCamera_rtmp_list,
            /* 所属的相机对象内没有相机项时,所属的相机删除 */ struct Camera_rtmp_item * const pCamera_rtmp_item);

#ifdef	__cplusplus
}
#endif

#endif	/* CAMERA_RTMP_H */

