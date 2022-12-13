/* 
 * File:   camera_rtmp.h
 * Author: fslib
 *
 * Created on 2013年01月04日, 上午11:19
 */
#if !defined CAMERA_RTMP_H && !defined JAVA_IN_C

#define CAMERA_RTMP_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Media/VideoH264.h"
#include "../../PublicTools/Thread/Rtmp.h"

#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/camera_ctrl.h"


#ifdef __cplusplus
extern "C" {
#endif
    //#pragma pack(push,4)

    struct Camera_rtmp_item;

    /* Rtmp相机 */
    struct Camera_rtmp {

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
#define Camera_rtmp_get_threadName(___pCamera_rtmp) (((char*)((___pCamera_rtmp)+1))+(___pCamera_rtmp)->ro._threadName_)
            unsigned char _threadName_;
            /* 处理的相机链表 */
            FsObjectList *__cameralist_;
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
            /* 外部和内部线程异步通信的链表,结构为Camera_rtmp_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    };
#define __Camera_rtmpLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  cameralist=%p/%lu\n  pRtmpClient=%p\n  controlList=%p/%lu\n",##__VA_ARGS__\
        ,pCamera_rtmp,((struct Camera_rtmp *)(pCamera_rtmp))->rw.runStatus,((struct Camera_rtmp *)(pCamera_rtmp))->p.inRun,((struct Camera_rtmp *)(pCamera_rtmp))->p.stid,((struct Camera_rtmp *)(pCamera_rtmp))->ro._pMonitor,((struct Camera_rtmp *)(pCamera_rtmp))->ro._externRunStatus,((struct Camera_rtmp *)(pCamera_rtmp))->ro._parentTid,Camera_rtmp_get_threadName(pCamera_rtmp)\
        ,((struct Camera_rtmp *)(pCamera_rtmp))->ro.__cameralist_,((struct Camera_rtmp *)(pCamera_rtmp))->ro.__cameralist_->nodeCount,((struct Camera_rtmp *)(pCamera_rtmp))->ro._pRtmpClient\
        ,((struct Camera_rtmp *)(pCamera_rtmp))->rw.__controlList,((struct Camera_rtmp *)(pCamera_rtmp))->rw.__controlList->nodeCount)
#define Camera_rtmpLog(___loglevel,___format, ...) __Camera_rtmpLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_rtmpLog2(___loglevel,___format, ...) __Camera_rtmpLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* Rtmp相机项 */
    struct Camera_rtmp_item {

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
            /* 数据流掩码,1-视频,2-音频,4-相机控制 */
            unsigned int _streamMask;
            /* 默认的控制类型,0-表示自动探测,0x1表示探测结果最多只能包含任意转 */
            unsigned long long _controlMaskDefault;
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
            int (*_cb)(FsObjectH264 *pH264, /* 帧率 */const float frameRate, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem);
            /* 外部的传入指针 */
            void *_externP;
            /* 收到的数据链表 */
            FsObjectList *__frameList;
            /* 互斥锁 */
            pthread_mutex_t __mutex;
        } ro;

        struct {
        } rw;

        struct {
            /* 所属的rtmp相机对象 */
            struct Camera_rtmp *pCamera_rtmp;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char controlStatus : 1;
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 帧率,小于0.001为无效值 */
            float _frameRate;
            /* 上一次的乘数因子 */
            float lastMultiFactor;
            /* 最近16次的差值 */
            float lastDiff8;
            /* 最近4次的差值 */
            float lastDiff4;
            /* 上一次接收数据的时间,gmt时间,用于校正本次数据的的采集时间,以让数据均匀 */
            double recvTimeGmt_last;
            /* 控制掩码,指向真实的控制掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */
            unsigned long long *ctrl_mask;
            /* 状态,指向真实的控制状态 */
            unsigned char *ctrl_status;
            /* 相机类型是否已确认,0-表示未确认,1-已确认 */
            unsigned char ctrl_type_checked;
            /* 控制相机类型:1-为海康http相机 */
            unsigned char ctrl_type : 7;
            /* 下一次的类型检查时间 */
            double ctrl_typeCheckTime;
            /* 控制句柄 */
            void *ctrl_p;
            /* 删除控制句柄的函数指针 */
            void (*delCtrl_p)(void *ctrl_p);
            /* 控制函数 */
            struct Camera_ctrl_function *ctl_function;
        } p;
    };
#define __Camera_rtmp_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,controlMaskDefault=%#llx,busy=%hhu/%hhu,streamMask=%#x,cb=%p,externP=%p,frameList=%p/%lu,"\
        "server:\"%hhu.%hhu.%hhu.%hhu:%hu\",src:\"%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",uuid:\"%s\",classIndex=%u,size=%u/%u,frameRate=%f,lastMultiFactor=%f,lastDiff8=%f,lastDiff4=%f,recvTimeGmt_last=%lf.\n",##__VA_ARGS__\
        ,pCamera_rtmp_item,pCamera_rtmp_item->p.pCamera_rtmp,pCamera_rtmp_item->ro._controlMaskDefault,pCamera_rtmp_item->p.busy,pCamera_rtmp_item->p.controlStatus,pCamera_rtmp_item->ro._streamMask\
        ,pCamera_rtmp_item->ro._cb,pCamera_rtmp_item->ro._externP,pCamera_rtmp_item->ro.__frameList,pCamera_rtmp_item->ro.__frameList->nodeCount\
        ,Fs_Network_Ipv4_print(pCamera_rtmp_item->ro.__pRtmpClient_item->rw._ipv4),pCamera_rtmp_item->ro.__pRtmpClient_item->rw._ipv4Port\
        ,FsStrprint(pCamera_rtmp_item->ro._domain,""),pCamera_rtmp_item->ro._domain ? "(" : ""\
        ,Fs_Network_Ipv4_print(pCamera_rtmp_item->ro._ipv4), pCamera_rtmp_item->ro._domain ? ")" : "", pCamera_rtmp_item->ro._ipv4Port, FsStrprint(pCamera_rtmp_item->ro._url, ""),pCamera_rtmp_item->ro._uuid\
        ,pCamera_rtmp_item->p.classIndex,pCamera_rtmp_item->p.imageWidth, pCamera_rtmp_item->p.imageHeight,pCamera_rtmp_item->p._frameRate\
        ,pCamera_rtmp_item->p.lastMultiFactor,pCamera_rtmp_item->p.lastDiff8,pCamera_rtmp_item->p.lastDiff4,pCamera_rtmp_item->p.recvTimeGmt_last)
#define Camera_rtmp_itemLog(___loglevel,___format, ...) __Camera_rtmp_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_rtmp_itemLog2(___loglevel,___format, ...) __Camera_rtmp_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    //#pragma pack(pop)

    /* 
     * 创建一个新的Camera_rtmp对象;
     * 返回Camera_rtmp指针.
     */

    struct Camera_rtmp *camera_rtmp_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* rtmp客户端对象 */ Rtmp * const pRtmpClient);

    /* 删除pCamera_rtmp指向的实例对象 */

    void camera_rtmp_delete__OI(struct Camera_rtmp * const pCamera_rtmp);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_rtmp_startThread(struct Camera_rtmp * const pCamera_rtmp
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void camera_rtmp_stopThread_noWait(struct Camera_rtmp * const pCamera_rtmp);

    /* 结束线程 */

    void camera_rtmp_stopThread(struct Camera_rtmp * const pCamera_rtmp);

    /* 
     * 创建一个新的Camera_rtmp_item对象;
     * 返回Camera_rtmp_item指针.
     */

    struct Camera_rtmp_item *camera_rtmp_item_new__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask, /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */ const unsigned long long controlMaskDefault, /* 域名,可为空 */ const char domain[]
            , /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port
            , /* 用户名 */const char username[], /* 密码 */const char password[], /* url地址 */const char url[], /* uuid,通道标识,可为空 */const char uuid[]
            , /* 收到数据后的回调函数,成功返回1,失败返回-1 */ int (*const cb) (FsObjectH264 *pH264, /* 帧率(小于0.001表示未知) */const float frameRate, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem)
            , /* 外部传入的指针 */void *externP);

    /* 
     * 创建一个新的Camera_rtmp_item对象;
     * 成功返回Camera_rtmp_item指针;
     * 失败返回NULL.
     */

    struct Camera_rtmp_item *camera_rtmp_item_new_byURL__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask, /* url地址,如rtsp://admin:admin@127.0.0.1:554/video */const char url[]
            , /* uuid,通道标识,可为空 */const char uuid[]
            , /* 收到数据后的回调函数,成功返回1,失败返回-1 */int (*const cb) (FsObjectH264 *pH264, /* 帧率 */const float frameRate, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem)
            , /* 外部传入的指针 */void *externP);

    /* 删除pCamera_rtmp_item指向的实例对象 */

    void camera_rtmp_item_delete__OI(struct Camera_rtmp_item * const pCamera_rtmp_item);

    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */

    void camera_rtmp_item_streamControl_pthreadSafety(struct Camera_rtmp_item * const pCamera_rtmp_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pCamera_rtmp中添加一个连接项 */

    void camera_rtmp_add_item__OI_2(struct Camera_rtmp * const pCamera_rtmp, struct Camera_rtmp_item * const pCamera_rtmp_item);

    /* 从pCamera_rtmp中移除一个连接项,未等待线程真正移除 */

    void camera_rtmp_remove_item_noWait__IO_2(struct Camera_rtmp * const pCamera_rtmp, struct Camera_rtmp_item * const pCamera_rtmp_item);

    /* 从pCamera_rtmp中移除一个连接项 */

    void camera_rtmp_remove_item__IO_2(struct Camera_rtmp * const pCamera_rtmp, struct Camera_rtmp_item * const pCamera_rtmp_item);

    /* 从pCamera_rtmp中移除一个连接项并删除,如果相机链表为空,把此相机也删除,并把相机指针置0 */

    void camera_rtmp_remove_and_delete_item_and_emptyCamera(/* 不为空时,且pCamera_rtmp_item所属的相机对象内没有相机项时,把pCamera_rtmp_item所属的相机对象从此链表中移除 */FsObjectList * const pCamera_rtmp_list
            , /* 所属的相机对象内没有相机项时,所属的相机删除 */ struct Camera_rtmp_item * const pCamera_rtmp_item
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif

#endif /* CAMERA_RTMP_H */

