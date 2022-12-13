/* 
 * File:   gbsdk.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined GBSDK_H && !defined JAVA_IN_C

#define GBSDK_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Thread/GB28181.h"
#include "../../Modules/sdkPlayBack/sdkPlayBackTask.h"
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* gbsdk */
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
#define GBSDK_get_threadName(___pGBSDK) (((char*)((___pGBSDK)+1))+(___pGBSDK)->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* GB28181对象,为空表示需要内部创建(程序使用独立方式运行) */
            GB28181 *_pGB28181;
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
            /* 外部和内部线程异步通信的链表,结构为GBSDK_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
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
            /* 本地地址映射,拉流时使用的地址 */
            FsRouter * __pAddrMap;
            /* pAddrMap的校验和 */
            unsigned int pAddrMapSum;
            /* GB28181对象,此对象可能通过外部传入,外部未传入时内部新建 */
            GB28181 *__pGB28181;
            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* 线程异步通信的链表,结构为HKSDK_P_control */
            FsStructList *controlListClient;
            /* 客户端线程对象链表(可接受新任务) */
            FsObjectList *threadListClient;
            //            /* 储存DHSDKPlayBack_P_Connect的链表 */
            //            FsObjectList *dHSDKPlayBack_P_ConnectList__newItemListClient;
            //            /* 运行一些阻塞任务 */
            //            Tasks *pTasks;
        } p;
    } GBSDK;
#define __GBSDKKLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd,tid=%ld,pMonitor=%p,externRunStatus=%p,parentTid=%ld,pipe=%d/%d,threadName:\"%s\"\n\
  channelCountPerThread=%u,itemListClient=%p/%lu,pGB28181=%p/%p,pAddrMap=%p/%x.\n",##__VA_ARGS__\
        ,pGBSDK,((GBSDK*)(pGBSDK))->rw.runStatus,((GBSDK*)(pGBSDK))->p.stid\
        ,((GBSDK*)(pGBSDK))->ro._pMonitor,((GBSDK*)(pGBSDK))->ro._externRunStatus,((GBSDK*)(pGBSDK))->ro._parentTid,((GBSDK*)(pGBSDK))->ro.__pipe[0], ((GBSDK*)(pGBSDK))->ro.__pipe[1],GBSDK_get_threadName(pGBSDK)\
        ,((GBSDK*)(pGBSDK))->ro._channelCountPerThread,((GBSDK*)(pGBSDK))->ro.__itemListClient_, ((GBSDK*)(pGBSDK))->ro.__itemListClient_->nodeCount\
        ,((GBSDK*)(pGBSDK))->ro._pGB28181,((GBSDK*)(pGBSDK))->p.__pGB28181,((GBSDK*)(pGBSDK))->p.__pAddrMap,((GBSDK*)(pGBSDK))->p.pAddrMapSum)
#define GBSDKLog(___loglevel,___format, ...) __GBSDKKLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define GBSDKLog2(___loglevel,___format, ...) __GBSDKKLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* GBSDK客户端项,是一个虚拟结构,需要根据_delete的值确定其到底是哪一种客户端项 */
    struct GBSDKClient_item {
        FsDelete *_delete;

        /* p放前面以使pipe_w的内存位置与HKSDKClient_item中的相同 */
        struct {
            /* 所属的pGBSDK对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            GBSDK *pGBSDK;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
        } p;
    };

    /* GBSDK,视频回放项 */
    struct GBSDK_PlayBack_item {
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         *     无锁的定义;
         */
        FsDelete *_delete;

        /* p放前面以使pipe_w的内存位置与HKSDKClient_item中的相同 */
        struct {
            /* 所属的pGBSDK对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            GBSDK *pGBSDK;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            //________________________________________________________________//
            unsigned char : 5;
            /* SIP服务器端口 */
            unsigned short _ipv4Port;
            /* SIP服务器地址,为0表示只接受被注册 */
            unsigned int _ipv4;
            /* 状态:
             * 0:正常;
             * -1:标记为出错(回放出错,完成后才能重新把状态置为0);
             */
            signed char status;
            /* 录像信息 */
            struct GB28181Client_item_recordInfo recordInfo;
            /* 回放管理对象,在第一次调用时初始化,为空表示未初始化,只能在单线程中运行,线程退出时必须删除 */
            struct SdkPlayBackTask *pSdkPlayBackTask;
#define GBSDK_PlayBack_item_PlayBackHandleCountMax 1 // 最大的句柄数
            /* 播放句柄链表 */
            FsObjectList *pPlayBackHandleList__recordVideoList;
            ///* 标记是否有通道的录像信息有更新 */
            //unsigned char *pVideoInfoHasUpdate;
            /* 绑定的pGB28181Client_item,用于获取时间轴信息 */
            struct GB28181Client_item *pGB28181Client_item;
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
            /* 数据目录,如/vsys/aa/video/ */
#define GBSDK_PlayBack_item_get_dirname(___pGBSDK_PlayBack_item) (((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._dirname_)
            unsigned short _dirname_;
            /* 域名,可为空 */
#define GBSDK_PlayBack_item_get_domain(___pGBSDK_PlayBack_item,___nullValue) (!FsNumberIsMax((___pGBSDK_PlayBack_item)->ro._domain_)\
    ?(((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._domain_):(___nullValue))
            unsigned short _domain_;
            /* 用户名 */
#define GBSDK_PlayBack_item_get_username(___pGBSDK_PlayBack_item,___nullValue) (!FsNumberIsMax((___pGBSDK_PlayBack_item)->ro._username_)\
    ?(((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._username_):(___nullValue))
            unsigned short _username_;
            /* 密码 */
#define GBSDK_PlayBack_item_get_password(___pGBSDK_PlayBack_item,___nullValue) (!FsNumberIsMax((___pGBSDK_PlayBack_item)->ro._password_)\
    ?(((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._password_):(___nullValue))
            unsigned short _password_;
            /* url地址,不可为空此处为通道号如34020000001320000002 */
#define GBSDK_PlayBack_item_get_url(___pGBSDK_PlayBack_item) (!FsNumberIsMax((___pGBSDK_PlayBack_item)->ro._url_)\
    ?(((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._url_):((char*)"NULL"))
            unsigned short _url_;
            /* 本地id,不能为空 */
#define GBSDK_PlayBack_item_get_id(___pGBSDK_PlayBack_item) (((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._id_)
            unsigned char _id_;
            /* SIP服务器ID,不能为空 */
#define GBSDK_PlayBack_item_get_sip_id(___pGBSDK_PlayBack_item) (((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._sip_id_)
            unsigned char _sip_id_;
            /* SIP服务器域,可为空 */
#define GBSDK_PlayBack_item_get_sip_domain(___pGBSDK_PlayBack_item,___nullValue) (!FsNumberIsMax((___pGBSDK_PlayBack_item)->ro._sip_domain_)\
    ?(((char*)((___pGBSDK_PlayBack_item)+1))+(___pGBSDK_PlayBack_item)->ro._sip_domain_):(___nullValue))
            unsigned char _sip_domain_;
            /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */
            unsigned char _sipOver : 2;
            /* 流的承载协议(isForSip为0时有效),&0x1-tcp,&0x2-udp,&0x4-kcp */
            unsigned char _streamOver : 3;
            /* 回放时是否需要进行录像查询,0-不需要,1-仅发送,2-需要等待回执,3-等到回执后发送二次查询,4-需要等待2次回执,以此类推 */
            unsigned char _playBackQueryRecord : 3;
        } ro;

        struct {
        } rw;
    };
#define __GBSDK_PlayBack_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,server(%hhu/%hhu/%hhu/%hhu/%hhu/%hhu/%d):\"%s%s%s@%s%s%hhu.%hhu.%hhu.%hhu%s:%hu/%s|%s|%s|%s\",dirname:\"%s\",recordCheckType=%hhu/%hhu\
,lastCheckVideoInfoTime=%lf,videoFrameRate=%f,recordVideoList=%p/%lu,pSdkPlayBackTask=%p,pPlayBackHandleList__recordVideoList=%p/%lu/%d,pGB28181Client_item=%p\
,sipOver=%hhx,_streamOver=%hhx,playBackQueryRecord=%hhu.\n",##__VA_ARGS__\
        ,pGBSDK_PlayBack_item,pGBSDK_PlayBack_item->p.pGBSDK,pGBSDK_PlayBack_item->p.status,pGBSDK_PlayBack_item->p.busy,pGBSDK_PlayBack_item->p._controlStatus\
        ,pGBSDK_PlayBack_item->p.recordInfo.infoLoad,pGBSDK_PlayBack_item->p.recordInfo.lastVideoInfoStatus__recordVideoList,pGBSDK_PlayBack_item->p.recordInfo.lastVideoInfoStatusCheckV,pGBSDK_PlayBack_item->p.pipe_w\
        ,GBSDK_PlayBack_item_get_username(pGBSDK_PlayBack_item,""),GBSDK_PlayBack_item_get_username(pGBSDK_PlayBack_item,NULL)?":":"", GBSDK_PlayBack_item_get_password(pGBSDK_PlayBack_item,"")\
        ,GBSDK_PlayBack_item_get_domain(pGBSDK_PlayBack_item,""),GBSDK_PlayBack_item_get_domain(pGBSDK_PlayBack_item,NULL)?"(":""\
        ,Fs_Network_Ipv4_print(pGBSDK_PlayBack_item->p._ipv4),GBSDK_PlayBack_item_get_domain(pGBSDK_PlayBack_item,NULL)?")":"",pGBSDK_PlayBack_item->p._ipv4Port\
        ,GBSDK_PlayBack_item_get_url(pGBSDK_PlayBack_item),GBSDK_PlayBack_item_get_id(pGBSDK_PlayBack_item),GBSDK_PlayBack_item_get_sip_id(pGBSDK_PlayBack_item)\
        ,GBSDK_PlayBack_item_get_sip_domain(pGBSDK_PlayBack_item,""),GBSDK_PlayBack_item_get_dirname(pGBSDK_PlayBack_item)\
        ,pGBSDK_PlayBack_item->p.recordInfo.recordCheckType,pGBSDK_PlayBack_item->p.recordInfo.recordStateUpdate\
        ,pGBSDK_PlayBack_item->p.recordInfo.lastCheckVideoInfoTime,pGBSDK_PlayBack_item->p.recordInfo.videoFrameRate\
        ,pGBSDK_PlayBack_item->p.recordInfo.__recordVideoList,pGBSDK_PlayBack_item->p.recordInfo.__recordVideoList->nodeCount,pGBSDK_PlayBack_item->p.pSdkPlayBackTask\
        ,pGBSDK_PlayBack_item->p.pPlayBackHandleList__recordVideoList,pGBSDK_PlayBack_item->p.pPlayBackHandleList__recordVideoList?pGBSDK_PlayBack_item->p.pPlayBackHandleList__recordVideoList->nodeCount:0\
        ,GBSDK_PlayBack_item_PlayBackHandleCountMax\
        ,pGBSDK_PlayBack_item->p.pGB28181Client_item,pGBSDK_PlayBack_item->ro._sipOver,pGBSDK_PlayBack_item->ro._streamOver,pGBSDK_PlayBack_item->ro._playBackQueryRecord)
#define GBSDK_PlayBack_itemLog(___loglevel,___format, ...) __GBSDK_PlayBack_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define GBSDK_PlayBack_itemLog2(___loglevel,___format, ...) __GBSDK_PlayBack_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)


    /* 
     * 创建一个新的GBSDK对象;
     * 返回GBSDK指针.
     */

    GBSDK *gbsdk_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread, /* 线程倍频因子,最小值为1 */ unsigned int const threadMulti
            , /* GB28181对象,为空表示需要内部创建(程序使用独立方式运行) */ GB28181 * const pGB28181
            , /* 本地地址映射,拉流时使用的地址,为空内部创建(独立运行时应为空) */ FsRouter * const pAddrMap
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 更新pGBSDK参数,pGBSDK->ro._pGB28181应为空才能调用此函数 */
    void gbsdk_update_parameter(GBSDK * const pGBSDK, /* 存放参数的ebml文件 */FsEbml * const pEbml
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 删除pGBSDK指向的实例对象 */

    void gbsdk_delete__OI(GBSDK * const pGBSDK);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void gbsdk_startThread(GBSDK * const pGBSDK
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void gbsdk_stopThread_noWait(GBSDK * const pGBSDK);

    /* 结束线程 */

    void gbsdk_stopThread(GBSDK * const pGBSDK);

    /* 
     * 创建一个新的GBSDK_PlayBack_item对象;
     * 返回GBSDK_PlayBack_item指针.
     */

    struct GBSDK_PlayBack_item * gbsdk_PlayBack_item_new__IO(/* 数据目录,如/vsys/aa/video/ */ const char dirname[]
            , /* 域名,可为空 */ const char domain[], /* ipv4地址 */const unsigned int ipv4, /* ipv4地址端口号,此值为0表示是推流 */const unsigned short ipv4Port
            , /* 用户名,不能为空 */ const char username[], /* 密码,不能为空 */const char password[], /* url地址,不可为空此处为通道号如/1 */const char url[]
            , /* 本地id,不能为空 */ const char id[], /* SIP服务器ID,不能为空 */ const char sip_id[]
            , /* SIP服务器域,可为空,不做校验 */const char sip_domain[]
            , /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */ const unsigned char sipOver
            , /* 流的承载协议,0x1-tcp被动模式,&0x2-udp,&0x4-kcp,&0x1-tcp主动模式 */ const unsigned char streamOver
            , /* 回放时是否需要进行录像查询,0-不需要,1-仅发送,2-需要等待回执,3-等到回执后发送二次查询,4-需要等待2次回执,以此类推 */const unsigned char playBackQueryRecord);

    /* 
     * 创建一个新的GBSDK_PlayBack_item对象;
     * 成功返回GBSDK_PlayBack_item指针;
     * 失败返回NULL.
     */

    struct GBSDK_PlayBack_item * gbsdk_PlayBack_item_new_byUrl__IO(/* 数据目录,如/vsys/aa/video/ */ const char dirname[]
            , /* gb28181://34020000001320000001:12345678@192.168.1.161:5080/34020000001320000002|34020000001320000002|34020000001320000001|3402000000?sipOver=1&streamOver=1&playBackQueryRecord=1 */const char url[]);

    /* 删除pGBSDK_PlayBack_item指向的实例对象 */

    void gbsdk_PlayBack_item_delete__OI(struct GBSDK_PlayBack_item * const pGBSDK_PlayBack_item);

    /* 向pGBSDK中添加一个pGBSDK_PlayBack_item客户端条目 */

    void gbsdk_PlayBack_add_item__OI_2(GBSDK * const pGBSDK, struct GBSDK_PlayBack_item * const pGBSDK_PlayBack_item);

    /* 从pGBSDK中移除一个pGBSDK_PlayBack_item客户端条目,未等待线程真正移除 */

    void gbsdk_PlayBack_remove_item_noWait__IO_2(GBSDK * const pGBSDK, struct GBSDK_PlayBack_item * const pGBSDK_PlayBack_item);

    /* 从pGBSDK中移除一个pGBSDK_PlayBack_item客户端条目 */

    void gbsdk_PlayBack_remove_item__IO_2(GBSDK * const pGBSDK, struct GBSDK_PlayBack_item * const pGBSDK_PlayBack_item);
  
    /* 创建一个回放任务,内部应检查dirName是否重复,相同的dirName应视为相同的回放任务,更新参数,必须在pDHSDK_PlayBack_item加入到pDHSDK后才能调用 */

    void gbsdk_PlayBack_item_updatePlayBackTask_pthreadSafety(GBSDK * const pGBSDK, struct GBSDK_PlayBack_item * const pGBSDK_PlayBack_item
            , /* 回放task */FsObjectSdkPlayBackTask * const pTask);
  
    /* 删除一个回放任务,内部会先检查pTask是否在内部,在内部会删除,必须在pGBSDK_PlayBack_item加入到pGBSDK后才能调用 */

    void gbsdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2(GBSDK * const pGBSDK, struct GBSDK_PlayBack_item * const pGBSDK_PlayBack_item
            , /* 回放task */FsObjectSdkPlayBackTask * const pTask);
  

    void gbsdk_test(void *pMonitor);

#ifdef __cplusplus
}
#endif

#endif /* GBSDK_H */


