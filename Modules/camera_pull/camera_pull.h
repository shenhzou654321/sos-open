/* 
 * File:   camera_pull.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */
#if !defined CAMERA_PULL_H && !defined JAVA_IN_C

#define CAMERA_PULL_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/CycleList.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Thread/monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* Pull相机 */
    struct Camera_pull {

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
#define Camera_pull_get_threadName(___pCamera_pull) (((char*)((___pCamera_pull)+1))+(___pCamera_pull)->ro._threadName_)
            unsigned char _threadName_;
            /* 工作线程数量 */
            unsigned short _threadCount;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 通道链表 */
            FsCycleList __itemList;
            /* 互斥锁,用于任务链表操作 */
            pthread_mutex_t __taskListMutex;
            /* 处理的任务链表,成员为camera_pull_P_task,使用__taskListMutex互斥锁 */
            FsObjectBaseList _taskList_taskListMutex;
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
            /* 外部和内部线程异步通信的链表,结构为Camera_pull_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 工作线程编号 */
            unsigned short threadIndex;
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    };
#define __Camera_pullLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  threadCount=%hu\n  pConfigManager=%p\n  itemListClient.nodeCount=%lu\n",##__VA_ARGS__\
        ,pCamera_pull,((struct Camera_pull*)(pCamera_pull))->rw.runStatus,((struct Camera_pull*)(pCamera_pull))->p.inRun,((struct Camera_pull*)(pCamera_pull))->p.stid\
        ,((struct Camera_pull*)(pCamera_pull))->ro._pMonitor,((struct Camera_pull*)(pCamera_pull))->ro._externRunStatus\
        ,((struct Camera_pull*)(pCamera_pull))->ro._parentTid\
        ,((struct Camera_pull*)(pCamera_pull))->ro.__pipe[0],((struct Camera_pull*)(pCamera_pull))->ro.__pipe[1]\
        ,Camera_pull_get_threadName(pCamera_pull)\
        ,((struct Camera_pull*)(pCamera_pull))->ro._threadCount\
        ,((struct Camera_pull*)(pCamera_pull))->ro._pConfigManager\
        ,((struct Camera_pull*)(pCamera_pull))->ro.__itemList.nodeCount)
#define Camera_pullLog(___loglevel,___format, ...) __Camera_pullLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_pullLog2(___loglevel,___format, ...) __Camera_pullLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* Pull相机项的通道 */
    struct Camera_pull_item_channel {
        /* 对应的uuid */
        char *uuid;
        /* 输入的数据帧的链表,使用Camera_pull_item中的__frameMutex互斥锁 */
        FsObjectList * frameListIn_;
        /* 实时视频掩码通信指针,不为空表示可以在camera_pull_item_streamControl_pthreadSafety中调用 */
        void *realMask;
        /* 拉流掩码 */
        unsigned int maskIndex_realStream;
        /* 方向储存弧度,逆时针为正 */
        float direction;
        /* 缩放后的宽度,为0表示忽略 */
        unsigned short width;
        /* 缩放后的高度,为0表示忽略 */
        unsigned short height;
        /* 中心点坐标,相对于目标图像中心点的坐标,向下为y+ */
        short center_x, center_y;
        /* 对称方式,0-不对称,1-左右对称,2-上下对称 */
        unsigned char symmetry : 2;
        /* 在Camera_pull_item中的通道号 */
        unsigned char index;
    };

    /* Pull相机项 */
    struct Camera_pull_item {
        /* 继承FsCycleListNode */
        struct FsCycleListNode cycleListNode;

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
            /* 收到数据后的回调函数,成功返回1,失败返回-1 */
            int (*_cb)(FsObjectImageFrame *pFrame, /* 外部传入的指针 */ void *p, MonitorItem *pMonitorItem);
            /* 外部的传入指针 */
            void *_externP;
            /* uuid */
            char *_uuid;
            /* 互斥锁,用于数据帧操作 */
            pthread_mutex_t __frameMutex;
            /* 互斥锁,用于连接命令字的操作 */
            pthread_mutex_t __cmdMutex;
            /* 掩码图,0-使用黑色,非0代表不同区域,在_imageWidth或_imageWidth为0时不使用掩码,此值应为空 */
            FsObjectBasePthreadSafety *__mask;
            /* 图像宽度,0表示自动,仅拉取一路视频流时才能为0,为0时只能使用旋转参数,忽略区域参数,其余情况为0不工作 */
            unsigned int _imageWidth;
            /* 图像高度,0表示自动,仅拉取一路视频流时才能为0,为0时只能使用旋转参数,忽略区域参数,其余情况为0不工作 */
            unsigned int _imageHeight;
            /* 帧率,小于0.00001为无效值 */
            float _frameRate;
            /* 背景图,在图像宽度不为0时有效 */
            FsObjectImageFrame *__bp;
            /* 背景填充色,rgb,其中0x80000000表示随机值,既不设置,在有背景图时,不使用背景色 */
            unsigned int _color_bg_rgb;
            /* 背景填充色,ycc,其中0x80000000表示随机值,既不设置,在有背景图时,不使用背景色 */
            unsigned int _color_bg_ycc;
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
        } ro;

        struct {
        } rw;

        struct {
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char controlStatus;
            /* 所属的pull相机对象 */
            struct Camera_pull *pCamera_pull;
            /* 只使用3字节 */
            unsigned int frameIndex;
            /* 输出的帧率 */
            float frameRate;
            /* 上一次输出帧的采集时间 */
            double capture_gmtTime;
            /* 流状态,1-表示开始流,0-停止流,-1-初始状态,停止流,此参数用于解决破环调用问题,使用__cmdMutex互斥锁 */
            signed char _streamStatus;
            /* 通道数 */
            unsigned char _channelCount;
#define   Camera_pull_item_get_from_channel(__pCamera_pull_item_channel) \
((struct Camera_pull_item *) (((char*) ((__pCamera_pull_item_channel) - (__pCamera_pull_item_channel)->index)) - FsPointer_to_long(((struct Camera_pull_item*) 0)->p._channel)))
            /* 拉取的通道 */
            struct Camera_pull_item_channel _channel[0];
        } p;
    };
#define __Camera_pull_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",item(=%p/%p,%p,%p/%hhu,%hhx),uuid:\"%s\",imageWidth=%u,imageHeight=%u,frameRate=%f,classIndex=%hu,channelCount=%hu.\n",##__VA_ARGS__\
        ,pCamera_pull_item,pCamera_pull_item->p.pCamera_pull,pCamera_pull_item->ro._cb,pCamera_pull_item->ro._externP,pCamera_pull_item->p.busy,pCamera_pull_item->p.controlStatus,pCamera_pull_item->ro._uuid\
        ,pCamera_pull_item->ro._imageWidth,pCamera_pull_item->ro._imageHeight,pCamera_pull_item->ro._frameRate,pCamera_pull_item->ro.classIndex,pCamera_pull_item->p._channelCount)
#define Camera_pull_itemLog(___loglevel,___format, ...) __Camera_pull_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_pull_itemLog2(___loglevel,___format, ...) __Camera_pull_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Camera_pull对象;
     * 返回Camera_pull指针.
     */

    struct Camera_pull *camera_pull_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 工作线程数量 */const unsigned short threadCount, /* 配置管理对象 */ void *const pConfigManager);

    /* 删除pCamera_pull指向的实例对象 */

    void camera_pull_delete__OI(struct Camera_pull * const pCamera_pull);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_pull_startThread(struct Camera_pull * const pCamera_pull
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程 */

    void camera_pull_stopThread(struct Camera_pull * const pCamera_pull);

    /* 
     * 创建一个新的Camera_pull_item对象;
     * 返回Camera_pull_item指针.
     */

    struct Camera_pull_item * camera_pull_item_new__IO(/* 本通道的uuid,不为空 */ const char uuid_camera[], FsConfig * const pConfig, const void *const cameraConfig0, const void *const cameraConfig
            , /* 收到数据后的回调函数,成功返回1,失败返回-1 */ int (*const cb) (FsObjectImageFrame *pFrame, /* 外部传入的指针 */ void *p, MonitorItem * pMonitorItem)
            , /* 外部传入的指针 */void *const externP
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pCamera_pull_item指向的实例对象 */

    void camera_pull_item_delete__OI(struct Camera_pull_item * const pCamera_pull_item);

    /* 控制流开始停止,只能在configManager中的mask的回调中使用 */

    void camera_pull_item_streamControl_for_configManagerMask(struct Camera_pull_item * const pCamera_pull_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pCamera_pull中添加一个连接项 */

    void camera_pull_add_item__OI_2(struct Camera_pull * const pCamera_pull, struct Camera_pull_item * const pCamera_pull_item);

    /* 从pCamera_pull中移除一个连接项,未等待线程真正移除 */

    void camera_pull_remove_item_noWait__IO_2(struct Camera_pull * const pCamera_pull, struct Camera_pull_item * const pCamera_pull_item);

    /* 从pCamera_pull中移除一个连接项 */

    void camera_pull_remove_item__IO_2(struct Camera_pull * const pCamera_pull, struct Camera_pull_item * const pCamera_pull_item);

    /* 从*ppCamera_pull中移除一个相机项并删除,在所属项被删除后,把相机停止并删除相机再把*ppCamera_pull置空*/

    void camera_pull_delete_item_and_camera(/* 相机二级指针,在所属项被删除后,把相机停止并删除相机再把*ppCamera_pull置空 */struct Camera_pull * * const ppCamera_pull
            , /* 删除的相机项 */ struct Camera_pull_item * const pCamera_pull_item
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


#ifdef __cplusplus
}
#endif

#endif /* CAMERA_PULL_H */

