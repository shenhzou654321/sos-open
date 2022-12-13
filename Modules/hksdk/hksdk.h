/* 
 * File:   hksdk.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined HKSDK_H && !defined JAVA_IN_C

#define HKSDK_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Image/ImageJpeg.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* hksdk */
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
#define HKSDK_get_threadName(pHKSDK) (((char*)((pHKSDK)+1))+(pHKSDK)->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* 客户端链表,按指针排序 */
            FsObjectList *__itemListClient_;
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
            /* 外部和内部线程异步通信的链表,结构为HKSDK_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(0:没有运行,1:在运行) */
            unsigned char inRun : 1;
            /* 子线程的运行状态:
             *     1-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             */
            unsigned char childRunStatus;
            /* 当前客户端的工作线程数 */
            unsigned short workThreadClient;
            /* 线程的tid号 */
            pthread_t stid;
            /* 主线程的线程号,由systemThreadTid获得 */
            pthread_t mainTid;
            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* 线程异步通信的链表,结构为HKSDK_P_control */
            FsStructList *controlListClient;
            /* 客户端线程对象链表(可接受新任务) */
            FsObjectList *threadListClient;
        } p;
    } HKSDK;
#define __HKSDKLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  channelCountPerThread=%u\n  itemListClient=%p/%lu\n",##__VA_ARGS__\
        ,pHKSDK,((HKSDK*)(pHKSDK))->rw.runStatus,((HKSDK*)(pHKSDK))->p.stid\
        ,((HKSDK*)(pHKSDK))->ro._pMonitor,((HKSDK*)(pHKSDK))->ro._externRunStatus,((HKSDK*)(pHKSDK))->ro._parentTid,((HKSDK*)(pHKSDK))->ro.__pipe[0], ((HKSDK*)(pHKSDK))->ro.__pipe[1],HKSDK_get_threadName(pHKSDK)\
        ,((HKSDK*)(pHKSDK))->ro._channelCountPerThread,((HKSDK*)(pHKSDK))->ro.__itemListClient_, ((HKSDK*)(pHKSDK))->ro.__itemListClient_->nodeCount)
#define HKSDKLog(___loglevel,___format, ...) __HKSDKLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define HKSDKLog2(___loglevel,___format, ...) __HKSDKLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* HKSDK客户端项,是一个虚拟结构,需要根据_delete的值确定其到底是哪一种客户端项 */
    struct HKSDKClient_item {
        FsDelete *_delete;

        /* p放前面以使pipe_w的内存位置与HKSDKClient_item中的相同 */
        struct {
            /* 所属的pHKSDK对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            HKSDK *pHKSDK;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;

        } p;
    };

    /* HKSDK,门禁项 */
    struct HKSDK_AccessCard_item {
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         *     无锁的定义;
         */
        FsDelete *_delete;

        /* p放前面以使pipe_w的内存位置与HKSDKClient_item中的相同 */
        struct {
            /* 所属的pHKSDK对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            HKSDK *pHKSDK;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            //________________________________________________________________//
            /* 状态:
             * 0:未连接;
             * 1:尝试连接;
             * 2:连接成功;
             * 3:事件监控中;
             * -1:标记为出错(因接收事件出错);
             */
            signed char status;
            /* 用户注册句柄 */
            long lUserID;
            /* 事件的句柄,NET_DVR_SetupAlarmChan_V41的返回值,在status不小于3时有效 */
            long eventHandle;
            /* 正在做的事的结果:
             *   0-未知,等待结果;
             *   1-成功;
             *   -1-失败;
             */
            signed char inDoRst;
            /* 正在做的事:
             *   0-无事可做
             *   3-下发卡信息
             *   4-下发人脸
             */
            unsigned char inDo;
            /* 与正在做的事关联的句柄 */
            long inDoHandle;
        } p;

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
            /* 收到数据后的回调函数,返回1,表示成功,错误返回负数 */
            int (*_cb)(/* 卡号 */ unsigned long long card, /* 类型,1-刷脸信息 */char type, /* jpg长度 */ unsigned int jpgLen, /* 图像数据 */unsigned char jpgData[], /* 外部传入的指针 */ void *const p);
            /* 外部的传入指针 */
            void *_externP;
            /* 用户名 */
#define HKSDK_AccessCard_item_get_username(pHKSDK_AccessCard_item) (((char*)((pHKSDK_AccessCard_item)+1))+(pHKSDK_AccessCard_item)->ro._username_)
            unsigned char _username_;
            /* 密码 */
#define HKSDK_AccessCard_item_get_password(pHKSDK_AccessCard_item) (((char*)((pHKSDK_AccessCard_item)+1))+(pHKSDK_AccessCard_item)->ro._password_)
            unsigned char _password_;
            /* 命令链表,项为strcut HKSDK_AccessCard_item_cmd指针 */
            FsObjectList *__cmdList;
        } ro;

        struct {
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* ipv4地址 */
            unsigned int _ipv4;
        } rw;

    };
#define __HKSDK_AccessCard_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,server(%hhu/%hhu/%hhu/%d):\"%s:%s@%hhu.%hhu.%hhu.%hhu:%hu\",cmdList=%p/%lu,lUserID=%ld,eventHandle=%ld,inDoRst=%hhd,inDo=%hhu,inDoHandle=%lu.\n",##__VA_ARGS__\
        ,pHKSDK_AccessCard_item,pHKSDK_AccessCard_item->p.pHKSDK,pHKSDK_AccessCard_item->p.status,pHKSDK_AccessCard_item->p.busy,pHKSDK_AccessCard_item->p._controlStatus,pHKSDK_AccessCard_item->p.pipe_w\
        ,HKSDK_AccessCard_item_get_username(pHKSDK_AccessCard_item), HKSDK_AccessCard_item_get_password(pHKSDK_AccessCard_item),Fs_Network_Ipv4_print(pHKSDK_AccessCard_item->rw._ipv4),pHKSDK_AccessCard_item->rw._ipv4Port\
        ,pHKSDK_AccessCard_item->ro.__cmdList,pHKSDK_AccessCard_item->ro.__cmdList->nodeCount\
        ,pHKSDK_AccessCard_item->p.lUserID,pHKSDK_AccessCard_item->p.eventHandle,pHKSDK_AccessCard_item->p.inDoRst,pHKSDK_AccessCard_item->p.inDo,pHKSDK_AccessCard_item->p.inDoHandle)
#define HKSDK_AccessCard_itemLog(___loglevel,___format, ...) __HKSDK_AccessCard_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define HKSDK_AccessCard_itemLog2(___loglevel,___format, ...) __HKSDK_AccessCard_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    struct HKSDK_AccessCard_item_cmd {
        FsObject object;
        /* 命令类型:
         *   0-清空所有卡
         *   1-增加卡
         */
        unsigned char type;
    };


    /* 
     * 创建一个新的HKSDK对象;
     * 返回HKSDK指针.
     */

    HKSDK *hksdk_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread);


    /* 删除pHKSDK指向的实例对象 */

    void hksdk_delete__OI(HKSDK * const pHKSDK);


    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void hksdk_startThread(HKSDK * const pHKSDK
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);


    /* 结束线程 */

    void hksdk_stopThread(HKSDK * const pHKSDK);



    void hksdk_out_pthreadSafety(HKSDK * const pHKSDK, FILE * const fd);

    /* 
     * 创建一个新的HKSDK_AccessCard_item对象;
     * 返回HKSDK_AccessCard_item指针.
     */

    struct HKSDK_AccessCard_item * hksdk_AccessCard_item_new__IO(/* ipv4地址 */const unsigned int ipv4, /* ipv4地址端口号,此值为0表示是推流 */const unsigned short ipv4Port
            , /* 用户名,不能为空 */ const char username[], /* 密码,不能为空 */const char password[]
            , /* 收到数据后的回调函数,返回1,表示成功,错误返回负数 */
            int (*const cb) (/* 卡号 */ unsigned long long card, /* 类型,1-刷脸信息 */char type, /* jpg长度 */ unsigned int jpgLen, /* 图像数据 */unsigned char jpgData[], /* 外部传入的指针 */ void *const p)
            , /* 外部传入的指针 */void *externP);


    /* 删除pHKSDK_AccessCard_item指向的实例对象 */

    void hksdk_AccessCard_item_delete__OI(struct HKSDK_AccessCard_item * const pHKSDK_AccessCard_item);


    /* 向pHKSDK中添加一个pHKSDK_AccessCard_item客户端条目 */

    void hksdk_AccessCard_add_item__OI_2(HKSDK * const pHKSDK, struct HKSDK_AccessCard_item * const pHKSDK_AccessCard_item);


    /* 从pHKSDK中移除一个pHKSDK_AccessCard_item客户端条目,未等待线程真正移除 */

    void hksdk_AccessCard_remove_item_noWait__IO_2(HKSDK * const pHKSDK, struct HKSDK_AccessCard_item * const pHKSDK_AccessCard_item);


    /* 从pHKSDK中移除一个pHKSDK_AccessCard_item客户端条目 */

    void hksdk_AccessCard_remove_item__IO_2(HKSDK * const pHKSDK, struct HKSDK_AccessCard_item * const pHKSDK_AccessCard_item);


    /* 创建一个清除所有卡的命令 */

    struct HKSDK_AccessCard_item_cmd * hksdk_AccessCard_item_cmd_new_clean_card__IO();


    /* 创建一个下发卡参数的命令 */

    /* 创建一个下发卡参数的命令 */
    struct HKSDK_AccessCard_item_cmd * hksdk_AccessCard_item_cmd_new_add_card__IO(
            /* 卡号0xFFFFFFFFFFFFFFFF:非法卡号;0xFFFFFFFFFFFFFFFE:胁迫码;0xFFFFFFFFFFFFFFFD:超级码;0xFFFFFFFFFFFFFFFC~0xFFFFFFFFFFFFFFF1:预留的特殊卡;0xFFFFFFFFFFFFFFF0:最大合法卡号 */const unsigned long long byCardNo
            , /* 工号,1~99999999,不能以0开头且不能重复 */ const unsigned long long employeeNo, /* 姓名,最大31字节 */ const char name[], /* 人脸jpg */ FsObjectImageJpeg * const jpg);


    /* 向pHKSDK_AccessCard_item添加一个命令 */

    void hksdk_AccessCard_item_add_cmd_pthreadSafety(struct HKSDK_AccessCard_item * const pHKSDK_AccessCard_item, struct HKSDK_AccessCard_item_cmd * const pCmd);



    void hksdk_test(void *pMonitor);

#ifdef __cplusplus
}
#endif

#endif /* HKSDK_H */


