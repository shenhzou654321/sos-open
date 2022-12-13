/* 
 * File:   camera_gb28181.h
 * Author: fslib
 *
 * Created on 2016年12月25日, 上午11:19
 */

#if !defined CAMERA_GB28181_H && !defined JAVA_IN_C

#define CAMERA_GB28181_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Media/VideoH264.h"
#include "../../PublicTools/Thread/camera_ctrl.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/GB28181.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct Camera_gb28181_item;

    /* GB28181相机 */
    struct Camera_gb28181 {

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
#define Camera_gb28181_get_threadName(___pCamera_gb28181) (((char*)((___pCamera_gb28181)+1))+(___pCamera_gb28181)->ro._threadName_)
            unsigned char _threadName_;
            /* GB28181客户端对象 */
            GB28181 *_pGB28181Client;
            /* 处理的相机链表 */
            FsObjectList *__cameralist_;
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
            /* 外部和内部线程异步通信的链表,结构为Camera_gb28181_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    };
#define __Camera_gb28181Log(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  cameralist=%p/%lu\n  pRtmpClient=%p\n  controlList=%p/%lu\n",##__VA_ARGS__\
        ,pCamera_gb28181,((struct Camera_gb28181 *)(pCamera_gb28181))->rw.runStatus,((struct Camera_gb28181 *)(pCamera_gb28181))->p.inRun,((struct Camera_gb28181 *)(pCamera_gb28181))->p.stid,((struct Camera_gb28181 *)(pCamera_gb28181))->ro._pMonitor,((struct Camera_gb28181 *)(pCamera_gb28181))->ro._externRunStatus,((struct Camera_gb28181 *)(pCamera_gb28181))->ro._parentTid,Camera_gb28181_get_threadName(pCamera_gb28181)\
        ,((struct Camera_gb28181 *)(pCamera_gb28181))->ro.__cameralist_,((struct Camera_gb28181 *)(pCamera_gb28181))->ro.__cameralist_->nodeCount,((struct Camera_gb28181 *)(pCamera_gb28181))->ro._pGB28181Client\
        ,((struct Camera_gb28181 *)(pCamera_gb28181))->rw.__controlList,((struct Camera_gb28181 *)(pCamera_gb28181))->rw.__controlList->nodeCount)
#define Camera_gb28181Log(___loglevel,___format, ...) __Camera_gb28181Log(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_gb28181Log2(___loglevel,___format, ...) __Camera_gb28181Log(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* GB28181相机项 */
    struct Camera_gb28181_item {

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
            /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */
            unsigned long long _controlMaskDefault;
            /* GB28181采集对象 */
            struct GB28181Client_item *__pGB28181Client_item;
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
            /* 所属的GB28181相机客户端对象 */
            struct Camera_gb28181 *pCamera_gb28181;
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
            /* 最近8次的差值 */
            float lastDiff8;
            /* 最近4次的差值 */
            float lastDiff4;
            /* 上一次接收数据的时间,gmt时间,用于校正本次数据的的采集时间,以让数据均匀 */
            double recvTimeGmt_last;

            /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */
            unsigned long long mask;
            /* 控制的检查编号,仅当值为1时才进行控制检测 */
            unsigned int controlCheckIndex;
            /* 状态,0-未知,1-空闲,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位 */
            unsigned char status;
            /* 设置的状态,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位 */
            unsigned char status_set;
            /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
             *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
             *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */
            unsigned short ctrlPriority__ctrlClientList;

            /* 校验和 */
            int sum;
            /* 新的校验和 */
            int sum_set;
            /* 设置的参数1 */
            short parameter1_set;
            /* 设置的参数2 */
            short parameter2_set;
            /* 设置的参数3 */
            short parameter3_set;
            /* 设置的参数4 */
            short parameter4_set;
            /* 可以做下一个动作的时间 */
            double time_next;
            /* 动作最长可以执行的时间 */
            double time_end;
        } p;
    };
#define __Camera_gb28181_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,status=%hhu/%hhu,streamMask=%#x,controlMaskDefault=%#llx,cb=%p,externP=%p,frameList=%p/%lu,ctrlClientListOrder=%p/%lu"\
        ",sipServer:\"%hhu.%hhu.%hhu.%hhu:%hu\",id:\"%s\",sip_id:\"%s\",sip_domain:\"%s\",sip_username:\"%s\",sip_password:\"%s\",url:\"%s\""\
        ",classIndex=%u,size=%u/%u,frameRate=%f,lastMultiFactor=%f,lastDiff8=%f,lastDiff4=%f,recvTimeGmt_last=%lf"\
        "mask=%llx,controlCheckIndex=%u.\n",##__VA_ARGS__\
        ,pCamera_gb28181_item,pCamera_gb28181_item->p.pCamera_gb28181,pCamera_gb28181_item->p.status,pCamera_gb28181_item->p.controlStatus,pCamera_gb28181_item->ro._streamMask,pCamera_gb28181_item->ro._controlMaskDefault,pCamera_gb28181_item->ro._cb, pCamera_gb28181_item->ro._externP\
        ,pCamera_gb28181_item->ro.__frameList,pCamera_gb28181_item->ro.__frameList->nodeCount,pCamera_gb28181_item->ro.__ctrlClientListOrder,pCamera_gb28181_item->ro.__ctrlClientListOrder->nodeCount\
        ,Fs_Network_Ipv4_print(pCamera_gb28181_item->ro.__pGB28181Client_item->ro._sip_ipv4),pCamera_gb28181_item->ro.__pGB28181Client_item->ro._sip_ipv4Port\
        ,GB28181Client_item_get_id(pCamera_gb28181_item->ro.__pGB28181Client_item),GB28181Client_item_get_sip_id(pCamera_gb28181_item->ro.__pGB28181Client_item)\
        ,GB28181Client_item_get_sip_domain(pCamera_gb28181_item->ro.__pGB28181Client_item,""),GB28181Client_item_get_sip_username(pCamera_gb28181_item->ro.__pGB28181Client_item,"")\
        ,GB28181Client_item_get_sip_password(pCamera_gb28181_item->ro.__pGB28181Client_item,""),GB28181Client_item_get_url(pCamera_gb28181_item->ro.__pGB28181Client_item)\
        ,pCamera_gb28181_item->p.classIndex,pCamera_gb28181_item->p.imageWidth, pCamera_gb28181_item->p.imageHeight,pCamera_gb28181_item->p._frameRate\
        ,pCamera_gb28181_item->p.lastMultiFactor,pCamera_gb28181_item->p.lastDiff8,pCamera_gb28181_item->p.lastDiff4,pCamera_gb28181_item->p.recvTimeGmt_last\
        ,pCamera_gb28181_item->p.mask,pCamera_gb28181_item->p.controlCheckIndex)
#define Camera_gb28181_itemLog(___loglevel,___format, ...) __Camera_gb28181_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_gb28181_itemLog2(___loglevel,___format, ...) __Camera_gb28181_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Camera_gb28181对象;
     * 返回Camera_gb28181指针.
     */

    struct Camera_gb28181 *camera_gb28181_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* GB28181客户端对象 */ GB28181 * const pGB28181Client);

    /* 删除pCamera_gb28181指向的实例对象 */

    void camera_gb28181_delete__OI(struct Camera_gb28181 * const pCamera_gb28181);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_gb28181_startThread(struct Camera_gb28181 * const pCamera_gb28181
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void camera_gb28181_stopThread_noWait(struct Camera_gb28181 * const pCamera_gb28181);

    /* 结束线程 */

    void camera_gb28181_stopThread(struct Camera_gb28181 * const pCamera_gb28181);

    /* 
     * 创建一个新的Camera_gb28181_item对象;
     * 返回Camera_gb28181_item指针.
     */

    struct Camera_gb28181_item *camera_gb28181_item_new__IO(/* 数据流掩码,1-视频,2-音频,4-相机控制 */const unsigned int streamMask, /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */const unsigned long long controlMaskDefault
            , /* 本地id,不能为空 */ const char id[], /* SIP服务器ID,不能为空 */ const char sip_id[]
            , /* SIP服务器域,可为空,不做校验 */const char sip_domain[], /* SIP用户认证ID,可为空 */ const char sip_username[], /* SIP用户认证密码,可为空 */ const char sip_password[]
            , /* 标识符,不能为空 */ const char url[], /* 本地地址映射,拉流时使用的地址 */ FsRouter * const pAddrMap
            , /* SIP服务器地址,为0表示只接受被注册 */ const unsigned int sip_ipv4, /* SIP服务器端口 */ const unsigned short sip_ipv4Port, /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */ const unsigned char sipOver
            , /* 流的承载协议,&0x1-tcp被动模式,&0x2-udp,&0x4-kcp,&0x1-tcp主动模式 */const unsigned char streamOver

            , /* 收到数据后的回调函数,成功返回1,失败返回-1 */ signed char (*const cb) (FsObject *pObject, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem)
            , /* 外部的传入指针 */ void *const externP);

    /* 删除pCamera_gb28181_item指向的实例对象 */

    void camera_gb28181_item_delete__OI(struct Camera_gb28181_item * const pCamera_gb28181_item);

    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */

    void camera_gb28181_item_streamControl_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pCamera_gb28181中添加一个连接项 */

    void camera_gb28181_add_item__OI_2(struct Camera_gb28181 * const pCamera_gb28181, struct Camera_gb28181_item * const pCamera_gb28181_item);

    /* 从pCamera_gb28181中移除一个连接项,未等待线程真正移除 */

    void camera_gb28181_remove_item_noWait__IO_2(struct Camera_gb28181 * const pCamera_gb28181, struct Camera_gb28181_item * const pCamera_gb28181_item);

    /* 从pCamera_gb28181中移除一个连接项 */

    void camera_gb28181_remove_item__IO_2(struct Camera_gb28181 * const pCamera_gb28181, struct Camera_gb28181_item * const pCamera_gb28181_item);

    /* 从pCamera_gb28181中移除一个连接项并删除,如果相机链表为空,把此相机也删除,并把相机指针置0 */

    void camera_gb28181_remove_and_delete_item_and_emptyCamera(/* 不为空时,且pCamera_gb28181_item所属的相机对象内没有相机项时,把pCamera_gb28181_item所属的相机对象从此链表中移除 */FsObjectList * const pCamera_gb28181_list_
            , /* 所属的相机对象内没有相机项时,所属的相机删除 */ struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 取消调用者保持信息 */

    void camera_gb28181_item_cancelKey_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,不为空 */void *const key
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */

    unsigned long long camera_gb28181_item_mask_get_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item, /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item
            , /* 不为空时使用*pProtocol_name返回外部控制协议协议名,如onvif */const char * * const pProtocol_name_out
            , /* 内部控制协议协议名,不为空时把内部控制协议协议名拷贝输出,无设备名输出\0 */ char protocol_name_in_cin[], /* protocol_name_in空间大小 */const unsigned int protocol_name_inLen
            , /* 设备型号,不为空时把外部控制协议设备名拷贝输出,无设备名输出\0 */ char device_type_out_cin[], /* 设备型号,不为空时把内部控制协议设备名拷贝输出,无协议名输出\0 */ char device_type_in_cin[], /* device_type空间大小 */const unsigned int device_typeLen
            , /* 设备序列号,不为空时把外部控制协议设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn_out_cin[], /* 设备序列号,不为空时把内部控制协议设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn_in_cin[], /* device_sn空间大小 */const unsigned int device_snLen
            , /* 设备版本号,不为空时把外部控制协议设备版本号拷贝输出,无设备版本号输出\0 */ char device_version_out_cin[], /* 设备版本号,不为空时把内部控制协议设备版本号拷贝输出,无设备版本号输出\0 */ char device_version_in_cin[], /* device_version空间大小 */const unsigned int device_versionLen
            , /* 不为空时使用*mask_out返回外部控制协议的掩码,如onvif的掩码 */ unsigned long long *const mask_out, /* 不为空时使用*mask_in返内外部控制协议的掩码 */ unsigned long long *const mask_in
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,任意转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turn_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数向左,正数向右 */const short speedx, /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnLeft_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnRight_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,上转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnUp_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,下转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnDown_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左上转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnLeftUp_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,左下转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnLeftDown_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右上转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnRightUp_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,右下转,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_turnRightDown_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,变倍,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_zoom_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 转动时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,聚焦,成功返回1,失败返回-1 */

    int camera_gb28181_item_ctrl_focus_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,设置预置位,成功返回1,失败返回-1 */

    int camera_gb28181_item_ctrl_preset_set_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,调用预置位,成功返回1,失败返回-1 */

    int camera_gb28181_item_ctrl_preset_goto_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,清除预置位,成功返回1,失败返回-1 */

    int camera_gb28181_item_ctrl_preset_clean_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 预置位号 */const unsigned short presetIndex
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,3D定位,成功返回1,失败返回-1 */

    int camera_gb28181_item_ctrl_zoom3D_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对图大小参考值23040 */const unsigned short x1, /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,goto PTZ,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_ptz_goto_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对参考值23040 */const unsigned short P, /* 相对参考值23040 */const unsigned short T, /* 相对参考值23040 */const unsigned short Z
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取PTZ,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */

    int camera_gb28181_item_ctrl_ptz_get_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 相对参考值23040 */ unsigned short*const P, /* 相对参考值23040 */ unsigned short* const T, /* 相对参考值23040 */ unsigned short* const Z
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,设置图像参数,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_imageInfo_set_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask, /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,获取图像参数,成功返回1,失败返回-1,对没有优先级不够的客户端会直接读取缓存返回数据 */

    int camera_gb28181_item_ctrl_imageInfo_get_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
         *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
         *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_chipCmd_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 控制,快门重置,成功返回1,失败返回-1,优先级不够返回-2 */

    int camera_gb28181_item_ctrl_shuterReset_pthreadSafety(struct Camera_gb28181_item * const pCamera_gb28181_item
            , /* 为空表示不支持内部控制 */struct Camera_ctrl_item * const pCamera_ctrl_item, /* 调用者的信息,为空表示不使用调用者信息 */void *const key
            , /* 优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,
                 *   偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;
                 *   奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝 */const unsigned short priority
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif


#endif /* CAMERA_GB28181_H */

