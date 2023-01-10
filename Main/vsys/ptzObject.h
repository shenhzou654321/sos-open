/* 
 * File:   ptzObject.h
 * Author: fslib
 *
 * Created on 2019年09月16日, 上午11:19
 */
#if !defined PtzObject_H && !defined JAVA_IN_C

#define PtzObject_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/GroupSqrt.h"
#include "../../PublicTools/Image/ImageFrame.h"  
#include "capture.h"
#ifdef __cplusplus
extern "C" {
#endif
#define PtzObject_Mask 0x80LLU

    /* 目标检测 */
    struct PtzObject {

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
            /* 线程名 */
#define PtzObject_get_threadName(___pPtzObject) (((char*)(((struct PtzObject*)(___pPtzObject))+1))+((struct PtzObject*)(___pPtzObject))->ro._threadName_)
            unsigned char _threadName_;
            /* 配置管理对象 */
            void *_pConfigManager;
            /* 通道链表 */
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
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
        } p;
    };
#define __PtzObjectLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,1,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  threadName:\"%s\"\n\
  pConfigManager=%p\n  itemList=%p\n  pGroupSqrtOut=%p\n",##__VA_ARGS__\
        ,pPtzObject,(( struct PtzObject*)(pPtzObject))->rw.runStatus,(( struct PtzObject*)(pPtzObject))->p.stid\
        ,(( struct PtzObject*)(pPtzObject))->ro._pMonitor,(( struct PtzObject*)(pPtzObject))->ro._externRunStatus,(( struct PtzObject*)(pPtzObject))->ro._parentTid,PtzObject_get_threadName(pPtzObject)\
        ,(( struct PtzObject*)(pPtzObject))->ro._pConfigManager,(( struct PtzObject*)(pPtzObject))->ro.__itemList_\
        ,(( struct PtzObject*)(pPtzObject))->ro._pGroupSqrtOut)
#define PtzObjectLog(___loglevel,___format, ...) __PtzObjectLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define PtzObjectLog2(___loglevel,___format, ...) __PtzObjectLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 检测项 */
    struct PtzObject_item {

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
            /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
            unsigned short classIndex;
            /* 所属主机的ip地址,用于集群,本机为0 */
            unsigned int _ipv4;
            /* 本对象的uuid值,一般是从配置中读入的 */
            char *_uuid;
            /* 所属的识别对象 */
            struct PtzObject *_pPtzObject;
            /* 整个通道依赖的所有数据的校验和 */
#define  PtzObject_item_ClusterLen  (FsPointer_to_long(&((struct PtzObject_item*) 0)->ro._sum)+sizeof(((struct PtzObject_item*) 0)->ro._sum))
            unsigned long long _sum;
            /* 连接和调用外部命令所需要的同步锁 */
            pthread_mutex_t __mutexCmdConnect;
            /* 图像宽度,0为无效值 */
            unsigned int imageWidth;
            /* 图像高度,0为无效值 */
            unsigned int imageHeight;
            /* 传入的数据帧链表 */
            FsObjectList *__framelistIn;
            /* 传出的数据帧链表 */
            FsObjectList *__framelistOut;
            /* 联动任务链表 */
            FsObjectList *__linkPtzList;
            /* 与之相关的客户端链表 */
            FsStructList *__clientList;
        } ro;

        struct {
            /* 当有数据输入时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
            unsigned char *_pGroupSqrtIn;
            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
            unsigned char *_pGroupSqrtOut;
        } rw;

        struct {  
            /* 设备垂直视场角 */
            unsigned short device_field_angle;
            /* 可见光变倍 */
            unsigned short ptz_zoom;
            /* 方位偏移量 */
            unsigned short ptz_horizontal_offset;
            /* 俯仰偏移量 */
            short ptz_vertical_offset;
            /* PTZ俯仰上限角度 */
            int ptz_up_range;
            /* PTZ俯仰下限角度 */
            int ptz_down_range;
            /* 采集对象，不为空表示有效 */
            struct Capture_item* pCapture_item;
            /* 记录当前联动时间 */
            long curLinkTime;
            /* 联动间隔时间 */
            short linkIntervalSeconds;
        } p;
    };

    /* 创建配置 */
    void ptzObject_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent);

    /* 检查pinflrayObject模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */

    int ptzObject_check_channel_changed(struct PtzObject * const pPtzObject, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);

    /* 
     * 创建一个新的inflrayObject对象;
     * 返回inflrayObject指针.
     */

    struct PtzObject * ptzObject_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 配置管理对象 */ void *const pConfigManager
            , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 删除pinflrayObject指向的实例对象 */

    void ptzObject_delete__OI(struct PtzObject * const pPtzObject);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void ptzObject_startThread(struct PtzObject * const pPtzObject
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程 */

    void ptzObject_stopThread(struct PtzObject * const pPtzObject);

    /* 传入数据帧 */

    void ptzObject_item_frame_in_pthreadSafety__OI_4(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */

    unsigned int ptzObject_item_frame_out_pthreadSafety__IO_4(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index
            , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);

    /* 获取数据帧的数量 */

    unsigned int ptzObject_item_get_frame_count_pthreadSafety(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index);

    /* 重置模块内通道对应的开方分组 */

    void ptzObject_item_reset_groupSqrtOut(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index
            , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
    
    /* 报警联动可见光球机 */

    void ptz_item_linkPtz_pthreadSafety(struct PtzObject_item * const pPtzObject_item, /* 报警点坐标 */unsigned int target_x, unsigned int target_y, /* 每帧全景图宽高 */unsigned frameWidth,
             unsigned frameHeight);
    typedef void (*Ptz_item_linkPtz_pthreadSafety)(struct PtzObject_item * const pPtzObject_item, /* 报警点坐标 */unsigned int target_x, unsigned int target_y, /* 每帧全景图宽高 */unsigned frameWidth,
             unsigned frameHeight);
    
#ifdef __cplusplus
}
#endif

#endif /* PtzObject_H */

