/* 
 * File:   camera_rtsp.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined CAMERA_RTSP_H && !defined JAVA_IN_C

#define CAMERA_RTSP_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Thread/camera_ctrl.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/Rtsp.h"
#ifdef __cplusplus
extern "C" {
#endif

    struct Camera_rtsp_item;

    /* Rtsp相机 */
    struct Camera_rtsp {

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
#define Camera_rtsp_get_threadName(___pCamera_rtsp) (((char*)((___pCamera_rtsp)+1))+(___pCamera_rtsp)->ro._threadName_)
            unsigned char _threadName_;
            /* 处理的相机链表 */
            FsObjectList *__cameralist_;
            /* rtsp客户端对象 */
            Rtsp *_pRtspClient;
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
            /* 外部和内部线程异步通信的链表,结构为Camera_rtsp_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    };
#define __Camera_rtspLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  cameralist=%p/%lu\n  pRtmpClient=%p\n  controlList=%p/%lu\n",##__VA_ARGS__\
        ,pCamera_rtsp,((struct Camera_rtsp *)(pCamera_rtsp))->rw.runStatus,((struct Camera_rtsp *)(pCamera_rtsp))->p.inRun,((struct Camera_rtsp *)(pCamera_rtsp))->p.stid,((struct Camera_rtsp *)(pCamera_rtsp))->ro._pMonitor,((struct Camera_rtsp *)(pCamera_rtsp))->ro._externRunStatus,((struct Camera_rtsp *)(pCamera_rtsp))->ro._parentTid,Camera_rtsp_get_threadName(pCamera_rtsp)\
        ,((struct Camera_rtsp *)(pCamera_rtsp))->ro.__cameralist_,((struct Camera_rtsp *)(pCamera_rtsp))->ro.__cameralist_->nodeCount,((struct Camera_rtsp *)(pCamera_rtsp))->ro._pRtspClient\
        ,((struct Camera_rtsp *)(pCamera_rtsp))->rw.__controlList,((struct Camera_rtsp *)(pCamera_rtsp))->rw.__controlList->nodeCount)
#define Camera_rtspLog(___loglevel,___format, ...) __Camera_rtspLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_rtspLog2(___loglevel,___format, ...) __Camera_rtspLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* Rtsp相机项 */
    struct Camera_rtsp_item {

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
            /* 控制掩码,为0表示不支持控制,0x1-onvif,0x2-宇视HTTP,0x4-海康Http相机,0x8-Inflray */
            unsigned int _controlMask;
            /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */
            unsigned long long _controlMaskDefault;
            /* 域名,可为空 */
#define Camera_rtsp_item_get_domain(___pCamera_rtsp_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_rtsp_item)->ro._domain_)?(((char*)((___pCamera_rtsp_item)+1))+(___pCamera_rtsp_item)->ro._domain_):(___nullValue))
            unsigned short _domain_;
            /* 控制域名,可为空 */
#define Camera_rtsp_item_get_holderDomain(___pCamera_rtsp_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_rtsp_item)->ro._holderDomain_)?(((char*)((___pCamera_rtsp_item)+1))+(___pCamera_rtsp_item)->ro._holderDomain_):(___nullValue))
            unsigned short _holderDomain_;
            /* ipv4地址 */
            unsigned int _ipv4;
            /* 控制ip */
            unsigned int _holderipv4;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* 控制端口,0表示使用默认值 */
            unsigned short _holderPort;
            /* 用户名,可为空 */
#define Camera_rtsp_item_get_username(___pCamera_rtsp_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_rtsp_item)->ro._username_)?(((char*)((___pCamera_rtsp_item)+1))+(___pCamera_rtsp_item)->ro._username_):(___nullValue))
            unsigned short _username_;
            /* 密码,可为空 */
#define Camera_rtsp_item_get_password(___pCamera_rtsp_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_rtsp_item)->ro._password_)?(((char*)((___pCamera_rtsp_item)+1))+(___pCamera_rtsp_item)->ro._password_):(___nullValue))
            unsigned short _password_;
            /* url地址,可为空 */
#define Camera_rtsp_item_get_url(___pCamera_rtsp_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_rtsp_item)->ro._url_)?(((char*)((___pCamera_rtsp_item)+1))+(___pCamera_rtsp_item)->ro._url_):(___nullValue))
            unsigned short _url_;
            /* 控制标识,可为空 */
#define Camera_rtsp_item_get_channelUUID(___pCamera_rtsp_item,___nullValue) \
    (!FsNumberIsMax((___pCamera_rtsp_item)->ro._channelUUID_)?(((char*)((___pCamera_rtsp_item)+1))+(___pCamera_rtsp_item)->ro._channelUUID_):(___nullValue))
            unsigned short _channelUUID_;
            /* rtsp采集对象 */
            struct RtspClient_item *__pRtspClient_item;
            /* 收到数据后的回调函数,成功返回1,失败返回-1 */
            signed char (*_cb)(FsObject *pObject, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem);
            /* 外部的传入指针 */
            void *_externP;
            /* 收到的数据链表 */
            FsObjectList *__frameList;
            /* 缓存的控制指针 */
            FsObjectList *__ctrlClientListOrder;
        } ro;

        struct {
        } rw;

        struct {
            /* 所属的rtsp相机对象 */
            struct Camera_rtsp *pCamera_rtsp;
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
            /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
            unsigned long long *ctrl_mask;
            /* 状态,指向真实的控制状态 */
            unsigned char *ctrl_status;
            /* 相机类型是否已确认,0-表示未确认,1-已确认,使用ctrlClientList的互斥锁 */
            unsigned char ctrl_type_checked__ctrlClientList : 1;
            /* 控制相机类型:0-为onvif相机,1-为Camera_ctrl_ys_http相机,2-为海康Http相机,3-Inflray相机 */
            unsigned char ctrl_type : 7;
            /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
             *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
             *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */
            unsigned short ctrlPriority__ctrlClientList;
            /* 下一次的类型检查时间 */
            double ctrl_typeCheckTime;
            /* 最近一次重置快门的时间 */
            double lastShutterRestTime;
            /* 控制句柄,为空表示句柄无效 */
            void *ctrl_p;
            /* 删除控制句柄的函数指针 */
            void (*delCtrl_p)(void *ctrl_p);
            /* 控制函数 */
            struct Camera_ctrl_function *ctrl_function;
        } p;
    };
#define __Camera_rtsp_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,_controlMaskDefault=%#llx,busy=%hhu/%hhu,streamMask=%#x,cb=%p,externP=%p,frameList=%p/%lu,ctrlClientListOrder=%p/%lu"\
        ",server:\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",src:\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",holder:\"%s%s%hhu.%hhu.%hhu.%hhu%s:%hu/%s\""\
        "classIndex=%u,size=%u/%u,frameRate=%f,lastMultiFactor=%f,lastDiff8=%f,lastDiff4=%f,recvTimeGmt_last=%lf"\
        ",ctrl_mask=%p,ctrl_status=%p,ctrl_type_checked__ctrlClientList=%hhu/%hhu/%hu/%lf,lastShutterRestTime=%lf,ctrl_p=%p/%p.\n",##__VA_ARGS__\
        ,pCamera_rtsp_item,pCamera_rtsp_item->p.pCamera_rtsp,pCamera_rtsp_item->ro._controlMaskDefault,pCamera_rtsp_item->p.busy,pCamera_rtsp_item->p.controlStatus,pCamera_rtsp_item->ro._streamMask\
        ,pCamera_rtsp_item->ro._cb, pCamera_rtsp_item->ro._externP,pCamera_rtsp_item->ro.__frameList,pCamera_rtsp_item->ro.__frameList->nodeCount,pCamera_rtsp_item->ro.__ctrlClientListOrder,pCamera_rtsp_item->ro.__ctrlClientListOrder->nodeCount\
        ,FsStrprint(pCamera_rtsp_item->ro.__pRtspClient_item->p._username, ""), pCamera_rtsp_item->ro.__pRtspClient_item->p._username ? ":" : ""\
        ,pCamera_rtsp_item->ro.__pRtspClient_item->p._username ? pCamera_rtsp_item->ro.__pRtspClient_item->p._password : "", pCamera_rtsp_item->ro.__pRtspClient_item->p._username ? "@" : ""\
        ,FsStrprint(pCamera_rtsp_item->ro.__pRtspClient_item->p._domain, ""), pCamera_rtsp_item->ro.__pRtspClient_item->p._domain ? "(" : ""\
        ,Fs_Network_Ipv4_print(pCamera_rtsp_item->ro.__pRtspClient_item->p._ipv4), pCamera_rtsp_item->ro.__pRtspClient_item->p._domain ? ")" : ""\
        ,pCamera_rtsp_item->ro.__pRtspClient_item->p._ipv4Port, FsStrprint(pCamera_rtsp_item->ro.__pRtspClient_item->p._url, "")\
        ,Camera_rtsp_item_get_username(pCamera_rtsp_item,""), Camera_rtsp_item_get_username(pCamera_rtsp_item,NULL) ? ":" : ""\
        ,Camera_rtsp_item_get_username(pCamera_rtsp_item,NULL) ? Camera_rtsp_item_get_password(pCamera_rtsp_item,""): "", Camera_rtsp_item_get_username(pCamera_rtsp_item,NULL) ? "@" : ""\
        ,Camera_rtsp_item_get_domain(pCamera_rtsp_item,""), Camera_rtsp_item_get_domain(pCamera_rtsp_item,NULL) ? "(" : ""\
        ,Fs_Network_Ipv4_print(pCamera_rtsp_item->ro._ipv4), Camera_rtsp_item_get_domain(pCamera_rtsp_item,NULL) ? ")" : "", pCamera_rtsp_item->ro._ipv4Port, Camera_rtsp_item_get_url(pCamera_rtsp_item,"")\
        ,Camera_rtsp_item_get_holderDomain(pCamera_rtsp_item,""), Camera_rtsp_item_get_holderDomain(pCamera_rtsp_item,NULL)? "(" : "",Fs_Network_Ipv4_print(pCamera_rtsp_item->ro._holderipv4)\
        ,Camera_rtsp_item_get_holderDomain(pCamera_rtsp_item,NULL) ? ")" : "", pCamera_rtsp_item->ro._holderPort, Camera_rtsp_item_get_channelUUID(pCamera_rtsp_item,"")\
        ,pCamera_rtsp_item->p.classIndex,pCamera_rtsp_item->p.imageWidth,pCamera_rtsp_item->p.imageHeight,pCamera_rtsp_item->p._frameRate\
        ,pCamera_rtsp_item->p.lastMultiFactor,pCamera_rtsp_item->p.lastDiff8,pCamera_rtsp_item->p.lastDiff4,pCamera_rtsp_item->p.recvTimeGmt_last\
        ,pCamera_rtsp_item->p.ctrl_mask,pCamera_rtsp_item->p.ctrl_status,pCamera_rtsp_item->p.ctrl_type_checked__ctrlClientList,pCamera_rtsp_item->p.ctrl_type,pCamera_rtsp_item->p.ctrlPriority__ctrlClientList,pCamera_rtsp_item->p.ctrl_typeCheckTime,pCamera_rtsp_item->p.lastShutterRestTime\
        ,pCamera_rtsp_item->p.ctrl_p,pCamera_rtsp_item->p.delCtrl_p)
#define Camera_rtsp_itemLog(___loglevel,___format, ...) __Camera_rtsp_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_rtsp_itemLog2(___loglevel,___format, ...) __Camera_rtsp_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Camera_rtsp对象;
     * 返回Camera_rtsp指针.
     */

    struct Camera_rtsp *camera_rtsp_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* rtsp客户端对象 */ Rtsp * const pRtspClient);

    /* 删除pCamera_rtsp指向的实例对象 */

    void camera_rtsp_delete__OI(struct Camera_rtsp * const pCamera_rtsp);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_rtsp_startThread(struct Camera_rtsp * const pCamera_rtsp
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void camera_rtsp_stopThread_noWait(struct Camera_rtsp * const pCamera_rtsp);

    /* 结束线程 */

    void camera_rtsp_stopThread(struct Camera_rtsp * const pCamera_rtsp);

    /* 
     * 创建一个新的Camera_rtsp_item对象;
     * 返回Camera_rtsp_item指针.
     */

    struct Camera_rtsp_item * camera_rtsp_item_new__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask
            , /* 控制掩码,为0表示不支持控制,0x1-onvif,0x2-宇视HTTP,0x4-海康Http相机,0x8-Inflray */const unsigned int controlMask, /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */ const unsigned long long controlMaskDefault, /* 域名,可为空 */ const char domain[]
            , /* ipv4地址 */const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port
            , /* 用户名 */const char username[], /* 密码 */const char password[], /* url地址 */const char url[]
            , /* 控制域名,可为空,为空时如流域名不为空则使用流域名 */ const char holderDomain[], /* 控制ip,0表示在控制域名为空时使用流ip */ const unsigned int holderipv4
            , /* 控制端口,0表示使用默认值 */ const unsigned short holderPort, /* 控制标识,可为空 */ const char channelUUID[]
            , /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */const unsigned char streamOver
            , /* 收到数据后的回调函数,成功返回1,失败返回-1 */ signed char (*cb)(FsObject *pObject, /* 外部传入的指针 */ void *p, MonitorItem * pMonitorItem)
            , /* 外部传入的指针 */void *externP);

    //    /* 
    //     * 创建一个新的Camera_rtsp对象;
    //     * 成功返回Camera_rtsp指针;
    //     * 失败返回NULL.
    //     */
    //#ifdef OJAVA
    //#else
    //    struct Camera_rtsp_item *camera_rtsp_item_new_byURL__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask
    //            , /* 控制掩码,为0表示不支持控制,0x1-onvif,0x2-宇视HTTP */const unsigned int controlMask, /* url地址,如rtsp://admin:admin@127.0.0.1:554/video */const char url[]
    //            , /* uuid,通道标识,可为空 */const char uuid[],
    //            /* 收到数据后的回调函数,成功返回1,失败返回-1 */signed char (*cb)(FsObject *pObject, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem)
    //            , /* 外部传入的指针 */void *externP);
    //#endif
    /* 删除pCamera_rtsp_item指向的实例对象 */

    void camera_rtsp_item_delete__OI(struct Camera_rtsp_item * const pCamera_rtsp_item);

    /* 控制流开始停止 */

    void camera_rtsp_item_streamControl_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pCamera_rtsp中添加一个连接项 */

    void camera_rtsp_add_item__OI_2(struct Camera_rtsp * const pCamera_rtsp, struct Camera_rtsp_item * const pCamera_rtsp_item);

    /* 从pCamera_rtsp中移除一个连接项,未等待线程真正移除 */

    void camera_rtsp_remove_item_noWait__IO_2(struct Camera_rtsp * const pCamera_rtsp, struct Camera_rtsp_item * const pCamera_rtsp_item);

    /* 从pCamera_rtsp中移除一个连接项 */

    void camera_rtsp_remove_item__IO_2(struct Camera_rtsp * const pCamera_rtsp, struct Camera_rtsp_item * const pCamera_rtsp_item);

    /* 从pCamera_rtsp中移除一个连接项并删除,如果相机链表为空,把此相机也删除,并把相机指针置0 */

    void camera_rtsp_remove_and_delete_item_and_emptyCamera(/* 不为空时,且pCamera_rtsp_item所属的相机对象内没有相机项时,把pCamera_rtsp_item所属的相机对象从此链表中移除 */FsObjectList * const pCamera_rtsp_list_
            , /* 所属的相机对象内没有相机项时,所属的相机删除 */ struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 取消调用者保持信息 */

    void camera_rtsp_item_cancelKey_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,不为空 */void *const key
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ */

    unsigned long long camera_rtsp_item_mask_get_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item, /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item
            , /* 不为空时使用*pProtocol_name返回外部控制协议协议名,如onvif */const char * * const pProtocol_name_out
            , /* 内部控制协议协议名,不为空时把内部控制协议协议名拷贝输出,无设备名输出\0 */ char protocol_name_in_cin[], /* protocol_name_in空间大小 */const unsigned int protocol_name_inLen
            , /* 设备型号,不为空时把外部控制协议设备名拷贝输出,无设备名输出\0 */ char device_type_out_cin[], /* 设备型号,不为空时把内部控制协议设备名拷贝输出,无协议名输出\0 */ char device_type_in_cin[], /* device_type空间大小 */const unsigned int device_typeLen
            , /* 设备序列号,不为空时把外部控制协议设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn_out_cin[], /* 设备序列号,不为空时把内部控制协议设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn_in_cin[], /* device_sn空间大小 */const unsigned int device_snLen
            , /* 设备版本号,不为空时把外部控制协议设备版本号拷贝输出,无设备版本号输出\0 */ char device_version_out_cin[], /* 设备版本号,不为空时把内部控制协议设备版本号拷贝输出,无设备版本号输出\0 */ char device_version_in_cin[], /* device_version空间大小 */const unsigned int device_versionLen
            , /* 不为空时使用*mask_out返回外部控制协议的掩码,如onvif的掩码 */ unsigned long long *const mask_out, /* 不为空时使用*mask_in返内外部控制协议的掩码 */ unsigned long long *const mask_in
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,任意转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turn_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数向左,正数向右 */const short speedx, /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnLeft_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnRight_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,上转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnUp_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,下转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnDown_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左上转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnLeftUp_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左下转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnLeftDown_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右上转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnRightUp_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右下转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_turnRightDown_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,变倍,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_zoom_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,聚焦,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_focus_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,设置预置位,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_preset_set_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,调用预置位,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_preset_goto_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,清除预置位,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_preset_clean_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,3D定位,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_zoom3D_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对图大小参考值23040 */const unsigned short x1, /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,goto PTZ,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_ptz_goto_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对参考值23040 */const unsigned short P, /* 相对参考值23040 */const unsigned short T, /* 相对参考值23040 */const unsigned short Z
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取PTZ,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */

    int camera_rtsp_item_ctrl_ptz_get_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority, /* 相对参考值23040 */ unsigned short*const P
            , /* 相对参考值23040 */ unsigned short* const T, /* 相对参考值23040 */ unsigned short* const Z
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,设置图像参数,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_imageInfo_set_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */

    int camera_rtsp_item_ctrl_imageInfo_get_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_chipCmd_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,快门重置,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_rtsp_item_ctrl_shuterReset_pthreadSafety(struct Camera_rtsp_item * const pCamera_rtsp_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif

#endif /* CAMERA_RTSP_H */

