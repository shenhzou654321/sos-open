/* 
 * File:   camera_ctrl_inflray.h
 * Author: fslib
 *
 * Created on 2020年12月21日, 上午11:19
 */

#if !defined CAMERA_CTRL_INFLRAY_H && !defined JAVA_IN_C

#define CAMERA_CTRL_INFLRAY_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Thread/camera_ctrl.h"
#include "../../PublicTools/Thread/SocketPool.h"
#ifdef __cplusplus
extern "C" {
#endif
#define Camera_ctrl_inflray_Debug

    /* 
     * INFLRAY控制(双光谱云台)
     */
    typedef struct {

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
#define Camera_ctrl_inflray_get_threadName(___pCamera_ctrl_inflray) (((char*)((___pCamera_ctrl_inflray)+1))+(___pCamera_ctrl_inflray)->ro._threadName_)
            unsigned char _threadName_;
            /* 连接池,为空内部使用独立的连接池 */
            SocketPool *_pSocketPool;
            /* 处理的相机链表,按指针排序 */
            FsObjectList *__itemListOrder_;
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
            /* 外部和内部线程异步通信的链表,结构为Camera_ctrl_inflray_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    } Camera_ctrl_inflray;

#define __Camera_ctrl_inflrayLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  pSocketPool=%p,itemList_=%p/%lu,controlList=%p/%lu\n",##__VA_ARGS__\
        ,pCamera_ctrl_inflray,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->rw.runStatus,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->p.inRun,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->p.stid,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro._pMonitor\
        ,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro._externRunStatus,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro._parentTid,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro.__pipe[0], ((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro.__pipe[1],Camera_ctrl_inflray_get_threadName(pCamera_ctrl_inflray)\
        ,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro._pSocketPool,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro.__itemListOrder_,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->ro.__itemListOrder_->nodeCount\
        ,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->rw.__controlList,((Camera_ctrl_inflray*)(pCamera_ctrl_inflray))->rw.__controlList->nodeCount)
#define Camera_ctrl_inflrayLog(___loglevel,___format, ...) __Camera_ctrl_inflrayLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_ctrl_inflrayLog2(___loglevel,___format, ...) __Camera_ctrl_inflrayLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* Camera_ctrl_inflray客户端条目 */
    struct Camera_ctrl_inflray_item {

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
            /* 域名,可为空 */
#define Camera_ctrl_inflray_item_get_domain(___pCamera_ctrl_inflray_item,___nullValue) (!FsNumberIsMax((___pCamera_ctrl_inflray_item)->ro._domain_)?(((char*)((___pCamera_ctrl_inflray_item)+1))+(___pCamera_ctrl_inflray_item)->ro._domain_):(___nullValue))
            unsigned char _domain_;
            /* 默认的控制类型,0-表示自动探测,0x1表示探测结果最多只能包含任意转 */
            unsigned long long _controlMaskDefault;
            /* 数据连接对象 */
#define Camera_ctrl_inflray_item_get_pSocketPool_item(___pCamera_ctrl_inflray_item) ((___pCamera_ctrl_inflray_item)->ro.__pSocketPool_item[(___pCamera_ctrl_inflray_item)->p.index_pSocketPool_item])
#define Camera_ctrl_inflray_item_get_pSocketPool_itemNext(___pCamera_ctrl_inflray_item) ((___pCamera_ctrl_inflray_item)->ro.__pSocketPool_item[Camera_ctrl_inflray_item_get_nextIndex_pSocketPool_item(___pCamera_ctrl_inflray_item)])
            struct SocketPool_item *__pSocketPool_item[2];
            /* 互斥锁 */
            pthread_mutex_t __mutex;
        } ro;

        struct {
            /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* 当前PTZ */
            unsigned short P, T, Z;
            /* 图像拼接的左裁剪像素(储存外部传过来的值),-1表示使用相机传过来的值 */
            short _cutLeft;
            /* 图像拼接的右裁剪像素(储存外部传过来的值),-1表示使用相机传过来的值 */
            short _cutRight;
            /* 重置快门,0-表示不需要重置,1-需要重置,2-表示需要关闭tif */
            unsigned char shutterReset : 2;
            /* imageInfo_set中哪些参数是有效的,按位摆布,如setMask为0,具体定义参看Camera_ctrl_imageInfo定义 */
            unsigned int setMask;
            /* 当前图像信息 */
            struct Camera_ctrl_imageInfo imageInfo;
            /* 设置的图像信息 */
            struct Camera_ctrl_imageInfo imageInfo_set;
        } rw;

        struct {
            /* 所属的pCamera_ctrl_inflray对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            Camera_ctrl_inflray *pCamera_ctrl_inflray;
            /* 掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
            unsigned long long mask;
            /* 当前使用哪个pSocketPool_item */
#define Camera_ctrl_inflray_item_get_nextIndex_pSocketPool_item(___pCamera_ctrl_inflray_item) (((___pCamera_ctrl_inflray_item)->p.index_pSocketPool_item+1)%Fs_Array_len(___pCamera_ctrl_inflray_item->ro.__pSocketPool_item))
#define Camera_ctrl_inflray_item_set_index_pSocketPool_item_next(___pCamera_ctrl_inflray_item) (___pCamera_ctrl_inflray_item)->p.index_pSocketPool_item=Camera_ctrl_inflray_item_get_nextIndex_pSocketPool_item(___pCamera_ctrl_inflray_item)
            unsigned char index_pSocketPool_item : 2;
            /* 心跳类型
             * 0-ptz停止;
             * 1-获取ptz;
             * 2-获取p值;
             */
#define Camera_ctrl_inflray_item_send_heartbeat_interval 0.5 // 发送心跳的间隔时间,主要用于获取相机参数
#define Camera_ctrl_inflray_item_send_heartbeat_cycle 3 // 一个周期的数量
#define Camera_ctrl_inflray_item_send_heartbeat_get_bw(___pCamera_ctrl_inflray_item) (___pCamera_ctrl_inflray_item)->p.heartbeatType=0 // 获取黑白热模式时应设置heartbeatType为多少
#define Camera_ctrl_inflray_item_send_heartbeat_get_cut(___pCamera_ctrl_inflray_item) (___pCamera_ctrl_inflray_item)->p.heartbeatType=1 // 获取裁剪系数时应设置heartbeatType为多少
#define Camera_ctrl_inflray_item_send_heartbeat_get_pt(___pCamera_ctrl_inflray_item) (___pCamera_ctrl_inflray_item)->p.heartbeatType=2 // 获取p值时应设置heartbeatType为多少
//#define Camera_ctrl_inflray_item_send_heartbeat_get_error(___pCamera_ctrl_inflray_item) (___pCamera_ctrl_inflray_item)->p.heartbeatType=3 // 获取error值时应设置heartbeatType为多少
            //#define Camera_ctrl_inflray_item_send_heartbeat_set_bw(___pCamera_ctrl_inflray_item) (___pCamera_ctrl_inflray_item)->p.heartbeatType=1 // 设置黑白热模式时应设置heartbeatType为多少
            unsigned char heartbeatType : 2;
            /* 是否已发送帧率调整命令,0-未发送(在status变为0时设置),1-已发送  1280设备启动的时候，要发一个同步指令下去，帧率才会正常 "AA 06 01 A3 01 03 32 8A EB AA" */
            unsigned char sendFrameChange : 1;
            unsigned char : 3;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
#define Camera_ctrl_inflray_item_wait(___pCamera_ctrl_inflray_item) do{while((___pCamera_ctrl_inflray_item)->p.busy)FsWaitSyncDo();}while(0)
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            /* 状态,0-未知,1-空闲,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位,17-gotoPTZ,18-获取PTZ,19-获取图像参数 */
            unsigned char status;
            /* 设置的状态,2-任意转,3-左转,4-右转,5-上转,6-下转,7-左上,8-左下,9-右上,10-右下,11-变倍,12-聚焦,13-设置预置位,14-调用预置位,15-清除预置位,16-3d定位,17-gotoPTZ,18-获取PTZ,19-获取图像参数 */
            unsigned char status_set;
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
            /* 可以做下一个动作的时间,发送指令后置为下一次可以执行命令的时间(负数),接收数据后置为相反数 */
#define Camera_ctrl_inflray_cmd_interval 0.1
            double time_next;
            /* 动作最长可以执行的时间 */
            double time_end;
            /* 最后一次收到正常数据的时间,发送数据时设置为当前时间(负数),接收数据时设置为当前时间(正数) */
            double lastRecvReturnTime;
            /* 命令链表,储存命令字,第一次使用时初始化,如"直接向机芯发送命令"会存于此链表中,节点类型为 */
            FsObjectList * __cmdList_mutex;
            /* 控制协议协议名,为空表示没有,使用__device_type__mutex的空间 */
            char *device_type;
            /* device_type用的空间,使用mutex互斥锁 */
            char *__device_type__mutex;
        } p;
    };
#define __Camera_ctrl_inflray_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p/%p,controlMaskDefault=%#llx,server[=%hhu](=%p/%p):\"%s%s%hhu.%hhu.%hhu.%hhu%s:%hu\",P=%hu,T=%hu,Z=%hu,cutLeft=%hd,cutRight=%hd,shutterReset=%hhu\
,mask=%llx,index_pSocketPool_item=%hhu,heartbeatType=%hhu,busy=%hhu,controlStatus=%hhu,status=%hhu/%hhu,sum=%x/%x,parameter=%hu/%hu/%hu/%hu,time_next=%lf,time_end=%lf,lastRecvReturnTime=%lf\n",##__VA_ARGS__\
        ,pCamera_ctrl_inflray_item,pCamera_ctrl_inflray_item->p.pCamera_ctrl_inflray,pCamera_ctrl_inflray_item->ro._controlMaskDefault,pCamera_ctrl_inflray_item->p.index_pSocketPool_item\
        ,Camera_ctrl_inflray_item_get_pSocketPool_item(pCamera_ctrl_inflray_item),Camera_ctrl_inflray_item_get_pSocketPool_itemNext(pCamera_ctrl_inflray_item)\
        ,SocketPool_item_get_domain(Camera_ctrl_inflray_item_get_pSocketPool_item(pCamera_ctrl_inflray_item),"")\
        ,(SocketPool_item_get_domain(Camera_ctrl_inflray_item_get_pSocketPool_item(pCamera_ctrl_inflray_item),NULL) ? "(" : "")\
        ,Fs_Network_Ipv4_print(Camera_ctrl_inflray_item_get_pSocketPool_item(pCamera_ctrl_inflray_item)->rw._ipv4)\
        ,(SocketPool_item_get_domain(Camera_ctrl_inflray_item_get_pSocketPool_item(pCamera_ctrl_inflray_item),NULL) ? ")" : "")\
        ,Camera_ctrl_inflray_item_get_pSocketPool_item(pCamera_ctrl_inflray_item)->rw._ipv4Port,pCamera_ctrl_inflray_item->rw.P,pCamera_ctrl_inflray_item->rw.T,pCamera_ctrl_inflray_item->rw.Z\
        ,pCamera_ctrl_inflray_item->rw._cutLeft,pCamera_ctrl_inflray_item->rw._cutRight,pCamera_ctrl_inflray_item->rw.shutterReset\
        ,pCamera_ctrl_inflray_item->p.mask,pCamera_ctrl_inflray_item->p.index_pSocketPool_item,pCamera_ctrl_inflray_item->p.heartbeatType,pCamera_ctrl_inflray_item->p.busy,pCamera_ctrl_inflray_item->p._controlStatus\
        ,pCamera_ctrl_inflray_item->p.status,pCamera_ctrl_inflray_item->p.status_set,pCamera_ctrl_inflray_item->p.sum,pCamera_ctrl_inflray_item->p.sum_set\
        ,pCamera_ctrl_inflray_item->p.parameter1_set,pCamera_ctrl_inflray_item->p.parameter2_set,pCamera_ctrl_inflray_item->p.parameter3_set,pCamera_ctrl_inflray_item->p.parameter4_set\
        ,pCamera_ctrl_inflray_item->p.time_next,pCamera_ctrl_inflray_item->p.time_end,pCamera_ctrl_inflray_item->p.lastRecvReturnTime)
#define Camera_ctrl_inflray_itemLog(___loglevel,___format, ...) __Camera_ctrl_inflray_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_ctrl_inflray_itemLog2(___loglevel,___format, ...) __Camera_ctrl_inflray_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Camera_ctrl_inflray对象;
     * 返回Camera_ctrl_inflray指针.
     */

    Camera_ctrl_inflray *camera_ctrl_inflray_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 连接池,为空内部使用独立的连接池 */ SocketPool * const pSocketPool
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pCamera_ctrl_inflray指向的实例对象 */

    void camera_ctrl_inflray_delete__OI(Camera_ctrl_inflray * const pCamera_ctrl_inflray);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_ctrl_inflray_startThread(Camera_ctrl_inflray * const pCamera_ctrl_inflray
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void camera_ctrl_inflray_stopThread_noWait(Camera_ctrl_inflray * const pCamera_ctrl_inflray);

    /* 结束线程 */

    void camera_ctrl_inflray_stopThread(Camera_ctrl_inflray * const pCamera_ctrl_inflray);

    /* 
     * 创建一个新的Camera_ctrl_inflray_item对象;
     * 返回Camera_ctrl_inflray_item指针.
     */

    struct Camera_ctrl_inflray_item * camera_ctrl_inflray_item_new__IO(/* 域名,可为空 */ const char domain[]
            , /* IPV4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4, /* IPV4地址端口 */ const unsigned short ipv4Port
            , /* 默认的控制类型,0-表示自动探测,0x1表示探测结果最多只能包含任意转 */ const unsigned long long controlMaskDefault);

    /* 删除pCamera_ctrl_inflray_item指向的实例对象 */

    void camera_ctrl_inflray_item_delete__OI(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item);

    /* 向pCamera_ctrl_inflray中添加一个pCamera_ctrl_inflray_item */

    void camera_ctrl_inflray_add_item__OI_2(Camera_ctrl_inflray * const pCamera_ctrl_inflray, struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item);

    /* 从pCamera_ctrl_inflray中移除一个pCamera_ctrl_inflray_item,未等待线程真正移除 */

    void camera_ctrl_inflray_remove_item_noWait__IO_2(Camera_ctrl_inflray * const pCamera_ctrl_inflray, struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item);

    /* 从pCamera_ctrl_inflray中移除一个pCamera_ctrl_inflray_item */

    void camera_ctrl_inflray_remove_item__IO_2(Camera_ctrl_inflray * const pCamera_ctrl_inflray, struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item);

    /* 获取掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ */

    unsigned long long camera_ctrl_inflray_item_mask_get(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 不为空时使用*pProtocol_name返回控制协议协议名,如onvif */const char * * const pProtocol_name
            , /* 设备型号,不为空时把设备名拷贝输出,无设备名输出\0 */ char device_type[], /* device_type空间大小 */const unsigned int device_typeLen
            , /* 设备序列号,不为空时把设备序列号拷贝输出,无设备序列号输出\0 */ char device_sn[], /* device_sn空间大小 */const unsigned int device_snLen
            , /* 设备版本号,不为空时把设备版本号拷贝输出,无设备版本号输出\0 */ char device_version[], /* device_version空间大小 */const unsigned int device_versionLen);

    /* 控制,任意转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turn_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040,负数向左,正数向右 */const short speedx
            , /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 转动时间 */const float time);

    /* 控制,左转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnLeft_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,右转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnRight_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,上转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnUp_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,下转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnDown_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,左上转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnLeftUp_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,左下转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnLeftDown_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,右上转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnRightUp_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,右下转,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_turnRightDown_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040 */const unsigned short speed, /* 转动时间 */const float time);

    /* 控制,变倍,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_zoom_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040,负数缩小,正数放大 */const short speed, /* 变倍时间 */const float time);

    /* 控制,聚焦,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_focus_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 参考值精度为23040,负数-,正数+ */const short speed, /* 聚焦时间 */const float time);

    /* 控制,设置预置位,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_preset_set_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 预置位号 */const unsigned short presetIndex);

    /* 控制,调用预置位,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_preset_goto_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 预置位号 */const unsigned short presetIndex);

    /* 控制,清除预置位,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_preset_clean_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 预置位号 */const unsigned short presetIndex);

    /* 控制,3D定位,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_zoom3D_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 相对图大小参考值23040 */const unsigned short x1,
            /* 相对图大小参考值23040 */const unsigned short y1, /* 相对图大小参考值23040 */const unsigned short x2, /* 相对图大小参考值23040 */const unsigned short y2);

    /* 控制,goto PTZ,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_ptz_goto_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 相对参考值23040 */const unsigned short P
            , /* 相对参考值23040 */const unsigned short T, /* 相对参考值23040 */const unsigned short Z);

    /* 控制,获取PTZ,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_ptz_get_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 相对参考值23040 */ unsigned short*const P
            , /* 相对参考值23040 */ unsigned short* const T, /* 相对参考值23040 */ unsigned short* const Z, /* 是否更新,1-更新,0-直接读取内存 */const unsigned char update);

    /* 控制,设置图像参数,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_imageInfo_set_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 哪些参数是有效的,按位摆布,如controlMode为0 */unsigned int setMask
            , /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo);

    /* 控制,获取图像参数,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_imageInfo_get_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item
            , /* 图像信息 */ struct Camera_ctrl_imageInfo * const imageInfo, /* 是否更新,1-更新,0-直接读取内存 */const unsigned char update);

    /* 控制,向机芯直接发送命令,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_chipCmd_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item, /* 命令的数据类型,0-字符串,1-二进制(cmd以字符串输入时使用base64编码) */ const unsigned char dataType, /* 命令 */const char cmd[], /* 命令长度 */const unsigned int cmdLen);

    /* 控制,快门重置,成功返回1,失败返回-1 */

    int camera_ctrl_inflray_item_ctrl_shutterReset_pthreadSafety(struct Camera_ctrl_inflray_item * const pCamera_ctrl_inflray_item);



#ifdef __cplusplus
}
#endif

#endif /* CAMERA_CTRL_INFLRAY_H */

