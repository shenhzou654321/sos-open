/* 
 * File:   camera_push.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */
#if !defined CAMERA_PUSH_H && !defined JAVA_IN_C

#define CAMERA_PUSH_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/CycleList.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Image/ImageFrame.h"
#include "../../PublicTools/Thread/monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* Push相机 */
    struct Camera_push {

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
#define Camera_push_get_threadName(___pCamera_push) (((char*)((___pCamera_push)+1))+(___pCamera_push)->ro._threadName_)
            unsigned char _threadName_;
            /* 工作线程数量 */
            unsigned short _threadCount;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 通道链表 */
            FsCycleList __itemList;
            /* 互斥锁,用于任务链表操作 */
            pthread_mutex_t __taskListMutex;
            /* 处理的任务链表,成员为camera_push_P_task,使用__taskListMutex互斥锁 */
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
            /* 外部和内部线程异步通信的链表,结构为Camera_push_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 工作线程编号 */
            unsigned short threadIndex;
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
            /* 互斥锁,用于pObjectBaseBuffer操作 */
            pthread_mutex_t pObjectBaseBufferMutex;
            /* 可跨线程的缓存buffer */
            FsObjectBaseBuffer *pObjectBaseBuffer;
        } p;
    };
#define __Camera_pushLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  threadCount=%hu\n  pConfigManager=%p\n itemListClient.nodeCount=%lu,taskList_taskListMutex.nodeCount=%lu,pObjectBaseBuffer=%p\n",##__VA_ARGS__\
        ,pCamera_push,((struct Camera_push*)(pCamera_push))->rw.runStatus,((struct Camera_push*)(pCamera_push))->p.inRun,((struct Camera_push*)(pCamera_push))->p.stid\
        ,((struct Camera_push*)(pCamera_push))->ro._pMonitor,((struct Camera_push*)(pCamera_push))->ro._externRunStatus\
        ,((struct Camera_push*)(pCamera_push))->ro._parentTid\
        ,((struct Camera_push*)(pCamera_push))->ro.__pipe[0],((struct Camera_push*)(pCamera_push))->ro.__pipe[1]\
        ,Camera_push_get_threadName(pCamera_push)\
        ,((struct Camera_push*)(pCamera_push))->ro._threadCount\
        ,((struct Camera_push*)(pCamera_push))->ro._pConfigManager\
        ,((struct Camera_push*)(pCamera_push))->ro.__itemList.nodeCount,((struct Camera_push*)(pCamera_push))->ro._taskList_taskListMutex.nodeCount,((struct Camera_push*)(pCamera_push))->p.pObjectBaseBuffer)
#define Camera_pushLog(___loglevel,___format, ...) __Camera_pushLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_pushLog2(___loglevel,___format, ...) __Camera_pushLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* Push相机项 */
    struct Camera_push_item {
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
            /* uuid */
            char *_uuid;
            /* 是否启用相机控制,0-不启用,1-启用 */
            unsigned char _ctrlEnable : 1;
            /* 拼接方式,0-不拼接,1-从左至右拼接,2-从右至左拼接,3-从上至下拼接,4-从下至上拼接,5-从左至右切片,6-从上至下切片 */
            unsigned char _stitch : 3;
            /* 收到数据后的回调函数,成功返回1,失败返回-1 */
            int (*_cb)(FsObjectImageFrame *pFrame, /* 外部传入的指针 */ void *p, MonitorItem * const pMonitorItem);
            /* 外部的传入指针 */
            void *_externP;
        } ro;

        struct {
        } rw;

        struct {
            /* 所属的push相机对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            struct Camera_push *pCamera_push;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char controlStatus : 1;
            /* 备用的数据来源计数器,为0时才可替换当前数据源 */
            unsigned char spareCounter;
            /* 当前的数据来源指针 */
            const void *src_current;
            /* 备用的数据来源指针 */
            const void *src_spare;
            /* 数据源的uuid,可为空,在_ctrlEnable为0时且stitch为0时不分配此变量空间 */
            char *__uuidSrc;

            union {

                /* 拼接需要的数据,stitch为时1-4时分配此变量空间 */
                struct {
                    /* 只使用3字节 */
                    unsigned int frameIndex;
                    /* 方向储存弧度,逆时针为正 */
                    float _direction;
                    /* 拼接的开始位置 */
                    unsigned short _start0;
                    /* 单张图缩放后的宽度,为0表示忽略 */
                    unsigned short _width;
                    /* 单张图缩缩放后的高度,为0表示忽略 */
                    unsigned short _height;
                    /* 目标图的宽度 */
                    unsigned short _dstWidth;
                    /* 目标图的高度 */
                    unsigned short _dstHeight;
                    /* 帧率,小于0.00001为无效值 */
                    float _frameRate;
                    /* 上一次输出帧的采集时间 */
                    double capture_gmtTime;
                    /* 对称方式,0-不对称,1-左右对称,2-上下对称 */
                    unsigned char _symmetry : 2;
                    /* 图像拼接的左裁剪像素,-1表示使用相机传过来的值 */
                    short _cutLeft;
                    /* 图像拼接的右裁剪像素,-1表示使用相机传过来的值 */
                    short _cutRight;
                    /* 图像拼接的上裁剪像素,-1表示使用相机传过来的值 */
                    short _cutTop;
                    /* 图像拼接的下裁剪像素,-1表示使用相机传过来的值 */
                    short _cutBottom;
                    /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化,使用taskListMutex的互斥锁 */
                    unsigned short classIndex_taskListMutex;
                    /* __ppFrame分配的可储存指针的个数 */
                    unsigned short ppFrameCount;
                    /* __ppFrame已使用的空间数量 */
                    unsigned short ppFrameUseCount;
                    /* 在stitch不为0时,此处需要储存拼接需要的缓存帧 */
                    FsObjectImageFrame **__ppFrame;
                } stitchData1_4;

                /* 切片需要的数据,stitch为时5-6时分配此变量空间 */
                struct {
                    /* 只使用3字节 */
                    unsigned int frameIndex;
                    /* 方向储存弧度,逆时针为正 */
                    float _direction;
                    /* 单张图缩放后的宽度,为0表示忽略 */
                    unsigned short _width;
                    /* 单张图缩缩放后的高度,为0表示忽略 */
                    unsigned short _height;
                    /* 目标图的宽度 */
                    unsigned short _dstWidth;
                    /* 目标图的高度 */
                    unsigned short _dstHeight;
                    /* 切片数量 */
                    unsigned char _cutCount;
                    /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化,使用taskListMutex的互斥锁 */
                    unsigned short classIndex_taskListMutex;
                } stitchData5_6;
            } stitchData[0];
        } p;
    };
#define __Camera_push_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",item(=%p/%p,%p,%p/%hhu,%hhx),uuid:\"%s\",ctrlEnable=%hhu,stitch=%hhu,spareCounter=%hhu,src_current=%p/%p.\n",##__VA_ARGS__\
        ,pCamera_push_item,pCamera_push_item->p.pCamera_push,pCamera_push_item->ro._cb,pCamera_push_item->ro._externP,pCamera_push_item->p.busy,pCamera_push_item->p.controlStatus,pCamera_push_item->ro._uuid\
        ,pCamera_push_item->ro._ctrlEnable,pCamera_push_item->ro._stitch,pCamera_push_item->p.spareCounter,pCamera_push_item->p.src_current,pCamera_push_item->p.src_spare)
#define Camera_push_itemLog(___loglevel,___format, ...) __Camera_push_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Camera_push_itemLog2(___loglevel,___format, ...) __Camera_push_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的Camera_push对象;
     * 返回Camera_push指针.
     */

    struct Camera_push *camera_push_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 工作线程数量 */const unsigned short threadCount, /* 配置管理对象 */ void *const pConfigManager);

    /* 删除pCamera_push指向的实例对象 */

    void camera_push_delete__OI(struct Camera_push * const pCamera_push);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void camera_push_startThread(struct Camera_push * const pCamera_push
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程 */

    void camera_push_stopThread(struct Camera_push * const pCamera_push);

    /* 
     * 创建一个新的Camera_push_item对象;
     * 返回Camera_push_item指针.
     */

    struct Camera_push_item * camera_push_item_new__IO(/* 本通道的uuid,不为空 */ const char uuid[], /* 是否启用相机控制,0-不启用,1-启用 */ const unsigned char ctrlEnable
            , /* 拼接方式,0-不拼接,1-从左至右拼接,2-从右至左拼接,3-从上至下拼接,4-从下至上拼接,5-从左至右切片,6-从上至下切片 */const unsigned char stitch, /* 切片数量,stitch为5-6时有效 */ unsigned char cutCount, /* 方向储存弧度,逆时针为正 */ const float direction
            , /* 拼接的开始位置 */ const unsigned short start0
            , /* 单张图缩放后的宽度,为0表示忽略 */ const unsigned short width, /* 单张图缩缩放后的高度,为0表示忽略 */const unsigned short height
            , /* 帧率,小于0.00001为无效值 */ const float frameRate, /* 对称方式,0-不对称,1-左右对称,2-上下对称 */ const unsigned char symmetry
            , /* 图像拼接的左裁剪像素,-1表示使用相机传过来的值 */ const short cutLeft, /* 图像拼接的右裁剪像素,-1表示使用相机传过来的值 */const short cutRight
            , /* 图像拼接的上裁剪像素,-1表示使用相机传过来的值 */ const short cutTop, /* 图像拼接的下裁剪像素,-1表示使用相机传过来的值 */ const short cutBottom
            , /* 收到数据后的回调函数,成功返回1,失败返回-1 */ int (*const cb) (FsObjectImageFrame *pFrame, /* 外部传入的指针 */ void *p, MonitorItem * const pMonitorItem)
            , /* 外部传入的指针 */void *const externP);

    /* 删除pCamera_push_item指向的实例对象 */

    void camera_push_item_delete__OI(struct Camera_push_item * const pCamera_push_item);

    /* 控制流开始停止,只能在configManager中的mask的回调中使用 */

    void camera_push_item_streamControl_for_configManagerMask(struct Camera_push_item * const pCamera_push_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pCamera_push中添加一个连接项 */

    void camera_push_add_item__OI_2(struct Camera_push * const pCamera_push, struct Camera_push_item * const pCamera_push_item);

    /* 从pCamera_push中移除一个连接项,未等待线程真正移除 */

    void camera_push_remove_item_noWait__IO_2(struct Camera_push * const pCamera_push, struct Camera_push_item * const pCamera_push_item);

    /* 从pCamera_push中移除一个连接项 */

    void camera_push_remove_item__IO_2(struct Camera_push * const pCamera_push, struct Camera_push_item * const pCamera_push_item);

    /* 从*ppCamera_push中移除一个相机项并删除,在所属项被删除后,把相机停止并删除相机再把*ppCamera_pull置空*/

    /* 从*ppCamera_push中移除一个相机项并删除,在所属项被删除后,把相机停止并删除相机再把*ppCamera_pull置空*/
    void camera_push_delete_item_and_camera(/* 相机二级指针,在所属项被删除后,把相机停止并删除相机再把*ppCamera_push置空 */struct Camera_push * * const ppCamera_push
            , /* 删除的相机项 */ struct Camera_push_item * const pCamera_push_item
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


#ifdef __cplusplus
}
#endif

#endif /* CAMERA_PUSH_H */

