/* 
 * File:   dhsdk.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined DHSDK_H && !defined JAVA_IN_C

#define DHSDK_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/StructList.h"
#include "../../PublicTools/Thread/Tasks.h"
#include "../../Modules/sdkPlayBack/sdkPlayBackTask.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    /* dhsdk */
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
#define DHSDK_get_threadName(pDHSDK) (((char*)((pDHSDK)+1))+(pDHSDK)->ro._threadName_)
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
            /* 外部和内部线程异步通信的链表,结构为DHSDK_P_control */
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
            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* 线程异步通信的链表,结构为HKSDK_P_control */
            FsStructList *controlListClient;
            /* 客户端线程对象链表(可接受新任务) */
            FsObjectList *threadListClient;
            /* 储存DHSDKPlayBack_P_Connect的链表 */
            FsObjectList *dHSDKPlayBack_P_ConnectList__newItemListClient;
            /* 运行一些阻塞任务 */
            Tasks *pTasks;
        } p;
    } DHSDK;
#define __DHSDKKLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  channelCountPerThread=%u\n  itemListClient=%p/%lu\n  pTasks=%p\n",##__VA_ARGS__\
        ,pDHSDK,((DHSDK*)(pDHSDK))->rw.runStatus,((DHSDK*)(pDHSDK))->p.stid\
        ,((DHSDK*)(pDHSDK))->ro._pMonitor,((DHSDK*)(pDHSDK))->ro._externRunStatus,((DHSDK*)(pDHSDK))->ro._parentTid,((DHSDK*)(pDHSDK))->ro.__pipe[0], ((DHSDK*)(pDHSDK))->ro.__pipe[1],DHSDK_get_threadName(pDHSDK)\
        ,((DHSDK*)(pDHSDK))->ro._channelCountPerThread,((DHSDK*)(pDHSDK))->ro.__itemListClient_, ((DHSDK*)(pDHSDK))->ro.__itemListClient_->nodeCount,((DHSDK*)(pDHSDK))->p.pTasks)
#define DHSDKLog(___loglevel,___format, ...) __DHSDKKLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define DHSDKLog2(___loglevel,___format, ...) __DHSDKKLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* DHSDK客户端项,是一个虚拟结构,需要根据_delete的值确定其到底是哪一种客户端项 */
    struct DHSDKClient_item {
        FsDelete *_delete;

        /* p放前面以使pipe_w的内存位置与HKSDKClient_item中的相同 */
        struct {
            /* 所属的pHKSDK对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            DHSDK *pDHSDK;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
        } p;
    };

    /* DHSDK,视频回放项 */
    struct DHSDK_PlayBack_item {
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         *     无锁的定义;
         */
        FsDelete *_delete;

        /* p放前面以使pipe_w的内存位置与HKSDKClient_item中的相同 */
        struct {
            /* 所属的pHKSDK对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            DHSDK *pDHSDK;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步,在加入时设置,即在线程退出时可不设置 */
            int pipe_w;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            //________________________________________________________________//
            unsigned char : 5;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* ipv4地址 */
            unsigned int _ipv4;
            /* 状态:
             * 0:正常;
             * -1:标记为出错(出错后要等待同步任务完成,完成后才能重新把状态置为0);
             */
            signed char status;
            /* 录像查询状态,用于维护时间轴,录像查询为同步接口,在一个独立线程中完成;
             * -1:检查结束,但有错误;
             * 0;检查结束,没有错误;
             * 1:设置其需要检查;
             */
            signed char queryStatus;
            /* 查询使用的层数,在连接成功时应初始化此值为pTasks->rw.layer,在添加查询任务后应设置此值为pTasks->rw.layer+1 */
            unsigned short queryLayer;
            /* 做查询的序号,因多通道共用连接,需要使用序号兼顾公平,初始化为pDHSDKPlayBack_P_Connect中的index */
#define DHSDK_PlayBack_item_doQueryIndex_set_changed_and_playing(___pDHSDK_PlayBack_item,___doCmdItemNextIndex) ___pDHSDK_PlayBack_item->p.doQueryIndex=(((___doCmdItemNextIndex)+1)&FsNumberMax(___pDHSDK_PlayBack_item->p.doQueryIndex))
#define DHSDK_PlayBack_item_doQueryIndex_set_changed(___pDHSDK_PlayBack_item,___doCmdItemNextIndex) ___pDHSDK_PlayBack_item->p.doQueryIndex=(((___doCmdItemNextIndex)+3)&FsNumberMax(___pDHSDK_PlayBack_item->p.doQueryIndex))
#define DHSDK_PlayBack_item_doQueryIndex_set_notChanged_and_playing(___pDHSDK_PlayBack_item,___doCmdItemNextIndex) ___pDHSDK_PlayBack_item->p.doQueryIndex=(((___doCmdItemNextIndex)+2)&FsNumberMax(___pDHSDK_PlayBack_item->p.doQueryIndex))
#define DHSDK_PlayBack_item_doQueryIndex_set_notChanged(___pDHSDK_PlayBack_item,___doCmdItemNextIndex) ___pDHSDK_PlayBack_item->p.doQueryIndex=(((___doCmdItemNextIndex)+7)&FsNumberMax(___pDHSDK_PlayBack_item->p.doQueryIndex))
#define DHSDK_PlayBack_item_doQueryIndex_set_error(___pDHSDK_PlayBack_item,___doCmdItemNextIndex) ___pDHSDK_PlayBack_item->p.doQueryIndex=(((___doCmdItemNextIndex)+17)&FsNumberMax(___pDHSDK_PlayBack_item->p.doQueryIndex))
            unsigned short doQueryIndex;

            struct {
#define DHSDK_PlayBack_item_recordInfo_chunkTime (24*60*60) // 检查的时间分段
                /* 向前检查的最小时间值,初始化为当前时间-GB28181Client_item_recordInfo_chunkTime,在一轮检测结束后根据checkCycleHasDelete进行调整
                 * checkCycleHasDelete为1时,调整previousTimeMin为C模式;为0时减小previousTimeMin以增大检测范围,同时如__recordVideoList为空需要调整nextTime时间为最新时间
                 */
                double previousTimeMin;
                /* 下一次向前探测的开始时间,在收到回执(或超时)后向前更新 */
                double previousTime;
                /* 下一次向后探测的开始时间,在收到回执(或超时)后向后更新 */
                double nextTime;
                /* 最近一次检查录像信息的时间 */
                double lastCheckVideoInfoTime;
                /* 录像的检查方式:
                 *     0-A模式-向后检查(更新最新数据)(检查后设置为4)
                 *     4-A模式检查中
                 *     1-B模式-向前检查(更新历史数据和巡查已缓存的数据区是否有数据删除)(检查后设置为5)
                 *     5-B模式检查中
                 *     2-C模式-向前检查(recordVideoList中第一条数据检查(检查数据是否有被删除))(检查后设置为6)
                 *     6-C模式检查中
                 */
                unsigned char recordCheckType : 3;
#define DHSDK_PlayBack_item_recordCheckType_C_startTime(___recordVideoList) ((((double (*)[2])___recordVideoList->pNode) + ___recordVideoList->startIndex)[0][0] - DHSDK_PlayBack_item_recordInfo_chunkTime / 4)
                /* A切换时应切换为B还是C(在A切换为其他时修改此值),0-表示下次切换为B,1-表示下次切换为C */
                unsigned char switchBC : 1;
                /* 录像信息是否已加载过历史数据(内部只在已加载过录像数据才会从网络更新),0-未加载,1-已加载 */
                unsigned char infoLoad : 1;
                /* 循环检测中是否出现了数据删除(有数据删除时会调整previousTimeMin为C模式,没有会减小previousTimeMin以增大检测范围),0-没有,1-有 */
                unsigned char checkCycleHasDelete : 1;
                /* 是否正在被回放中(在出现与之相关的客户端时,可设置此值,以便可以加快此路视频的录像查询,提高其实时性),0-没有,1-在 */
                unsigned char isPlayBack : 1;
                unsigned char _1 : 1;
                unsigned char _2 : 7;
                /* 最后一段录像的持续状态,0-结束状态,1-视频持续录像中 */
                unsigned char lastVideoInfoStatus__recordVideoList : 1;
                /* 最后一段录像的持续状态切换使用的参数,在lastVideoInfoStatus为1时,数据有增长时把此值置为DHSDK_PlayBack_item_lastVideoInfoStatusCheckV0,每检查一次此值减1,当值为0时设置lastVideoInfoStatus为0 */
#define DHSDK_PlayBack_item_lastVideoInfoStatusCheckV0 3
                unsigned char lastVideoInfoStatusCheckV;
                /* 录像状态是否有更新,0-没有更新,1-有更新 */
                unsigned char recordStateUpdate : 1;
                /* lastVideoInfoStatusCheckV最后一次更新的时间 */
#define DHSDK_PlayBack_item_lastVideoInfoStatusCheckV0Timeout 10 // 最后一次检查到录像持续更新到下一次录像断开的最小时间间隔
                double lastVideoInfoStatusCheckVLastUptime;
                /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */
                FsStructList *__recordVideoList;
            } recordInfo;
            /* 对__recordInfo中需要非0初始化的变量进行初始化 */
#define DHSDK_PlayBack_item_recordInfo_init_not_0(___recordInfo) do{\
    const double t=time(NULL);\
    /* 向前检查的最小时间值,初始化为当前时间-GB28181Client_item_recordInfo_chunkTime,在一轮检测结束后根据checkCycleHasDelete进行调整\
     * checkCycleHasDelete为1时,调整previousTimeMin为C模式;为0时减小previousTimeMin以增大检测范围,同时如__recordVideoList为空需要调整nextTime时间为最新时间\
     */\
    (___recordInfo)->previousTimeMin=t-DHSDK_PlayBack_item_recordInfo_chunkTime;\
    /* 下一次向前探测的开始时间,在收到回执(或超时)后向前更新 */\
    (___recordInfo)->previousTime=t-DHSDK_PlayBack_item_recordInfo_chunkTime;\
    /* 下一次向后探测的开始时间,在收到回执(或超时)后向后更新 */\
    (___recordInfo)->nextTime=t-DHSDK_PlayBack_item_recordInfo_chunkTime/2;\
    /* 最近一次检查录像信息的时间 */\
    /* double lastCheckVideoInfoTime; */\
    /* 录像的检查方式:\
     *     0-A模式-向后检查(更新最新数据)(检查后设置为4)\
     *     4-A模式检查中\
     *     1-B模式-向前检查(更新历史数据和巡查已缓存的数据区是否有数据删除)(检查后设置为5)\
     *     5-B模式检查中\
     *     2-C模式-向前检查(recordVideoList中第一条数据检查(检查数据是否有被删除))(检查后设置为6)\
     *     6-C模式检查中\
     */\
    /* unsigned char recordCheckType : 3; */\
    /* A切换时应切换为B还是C(在A切换为其他时修改此值),0-表示下次切换为B,1-表示下次切换为C */\
    /* unsigned char switchBC : 1; */\
    /* 录像信息是否已加载过历史数据(内部只在已加载过录像数据才会从网络更新),0-未加载,1-已加载 */\
    /* unsigned char infoLoad : 1; */\
    /* 循环检测中是否出现了数据删除(有数据删除时会调整previousTimeMin为C模式,没有会减小previousTimeMin以增大检测范围),0-没有,1-有 */\
    /* unsigned char checkCycleHasDelete : 1; */\
    /* 是否正在被回放中(在出现与之相关的客户端时,可设置此值,以便可以加快此路视频的录像查询,提高其实时性),0-没有,1-在 */\
    /* unsigned char isPlayBack:1; */\
    /* 最后一段录像的持续状态,0-结束状态,1-视频持续录像中 */\
    /* unsigned char lastVideoInfoStatus__recordVideoList : 1; */\
    /* 最后一段录像的持续状态切换使用的参数,在lastVideoInfoStatus为1时,数据有增长时把此值置为GB28181Client_item_recordInfo_lastVideoInfoStatusCheckV0,每检查一次此值减1,当值为0时设置lastVideoInfoStatus为0 */\
    /* unsigned char lastVideoInfoStatusCheckV; */\
    /* 录像状态是否有更新,0-没有更新,1-有更新 */\
    /* unsigned char recordStateUpdate : 1; */\
    /* 视频数据的帧率,用以让接收数据时的帧率可参考之前的帧率 */\
    /* float videoFrameRate; */\
    /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */\
    (___recordInfo)->__recordVideoList= fs_StructList_new__IO(4, sizeof (double)*2);\
    /* 记录录像时间信息(在第一次收到数据时初始化),节点为两个double大小,第一个表示开始时间,第二个表示结束时间,用于缓存一次分段检测的结果,在数据收完整后再与__recordVideoList中的数据进行合并 */\
    /* FsStructList *__recordVideoListBuffer; */\
    /* __recordVideoListBuffer还需要收取的数据量 */\
    /* unsigned int recordVideoListBufferRemainCount; */\
    /* 不为空时在录像信息更新时通过此函数回调 */\
    /*(___recordInfo)->cb_update=___cb_update;*/\
    /* cb_update回调时外部传入的参数 */\
    /*(___recordInfo)->cb_P=___cb_P;*/\
}while(0)
            /* 释放___recordInfo */
#define DHSDK_PlayBack_item_recordInfo_release(___recordInfo) do{\
    /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */\
    fs_StructList_delete__OI((___recordInfo)->__recordVideoList);\
}while(0)
            /* 清理___recordInfo中除recordVideoList中的数据(即不清除recordVideoList中的数据,也不设置硬盘加载标记和录像是否持续更新标记) */
#define DHSDK_PlayBack_item_recordInfo_clean_except_recordVideoList(___recordInfo) do{\
     const double t = time(NULL);\
    /* 向前检查的最小时间值,初始化为当前时间-GB28181Client_item_recordInfo_chunkTime,在一轮检测结束后根据checkCycleHasDelete进行调整\
     * checkCycleHasDelete为1时,调整previousTimeMin为C模式;为0时减小previousTimeMin以增大检测范围,同时如__recordVideoList为空需要调整nextTime时间为最新时间\
     */\
    (___recordInfo)->previousTimeMin=t-DHSDK_PlayBack_item_recordInfo_chunkTime;\
    /* 下一次向前探测的开始时间,在收到回执(或超时)后向前更新 */\
    (___recordInfo)->previousTime=t-DHSDK_PlayBack_item_recordInfo_chunkTime;\
    /* 下一次向后探测的开始时间,在收到回执(或超时)后向后更新 */\
    (___recordInfo)->nextTime=t-DHSDK_PlayBack_item_recordInfo_chunkTime/2;\
    /* 最近一次检查录像信息的时间 */\
    /* double lastCheckVideoInfoTime; */\
    /* 录像的检查方式:\
     *     0-A模式-向后检查(更新最新数据)(检查后设置为4)\
     *     4-A模式检查中\
     *     1-B模式-向前检查(更新历史数据和巡查已缓存的数据区是否有数据删除)(检查后设置为5)\
     *     5-B模式检查中\
     *     2-C模式-向前检查(recordVideoList中第一条数据检查(检查数据是否有被删除))(检查后设置为6)\
     *     6-C模式检查中\
     */\
     (___recordInfo)->recordCheckType=0;\
    /* A切换时应切换为B还是C(在A切换为其他时修改此值),0-表示下次切换为B,1-表示下次切换为C */\
    /* unsigned char switchBC : 1; */\
    /* 检查与sip关联的方法,轮巡检测,0-录像查询方法,1-直播请求方法 */\
    /* unsigned char sipCheckMethod : 1;*/\
    /* 录像信息是否已加载过历史数据(内部只在已加载过录像数据才会从网络更新),0-未加载,1-已加载 */\
    /* unsigned char infoLoad : 1; */\
    /* 循环检测中是否出现了数据删除(有数据删除时会调整previousTimeMin为C模式,没有会减小previousTimeMin以增大检测范围),0-没有,1-有 */\
    (___recordInfo)->checkCycleHasDelete=0;\
    /* 是否正在被回放中(在出现与之相关的客户端时,可设置此值,以便可以加快此路视频的录像查询,提高其实时性),0-没有,1-在 */\
    (___recordInfo)->isPlayBack=0;\
    /* 最后一段录像的持续状态,0-结束状态,1-视频持续录像中 */\
    /* unsigned char lastVideoInfoStatus__recordVideoList : 1; */\
    /* 最后一段录像的持续状态切换使用的参数,在lastVideoInfoStatus为1时,数据有增长时把此值置为GB28181Client_item_recordInfo_lastVideoInfoStatusCheckV0,每检查一次此值减1,当值为0时设置lastVideoInfoStatus为0 */\
    /* unsigned char lastVideoInfoStatusCheckV; */\
    /* 录像状态是否有更新,0-没有更新,1-有更新 */\
    /* unsigned char recordStateUpdate : 1; */\
    /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */\
    /* FsStructList *__recordVideoListBuffer; */\
}while(0)
            /* 回放管理对象,在第一次调用时初始化,为空表示未初始化,只能在单线程中运行,线程退出时必须删除 */
            struct SdkPlayBackTask *pSdkPlayBackTask;
#define DHSDK_PlayBack_item_PlayBackHandleCountMax 1 // 最大的句柄数
            /* 播放句柄链表 */
            FsObjectList *pPlayBackHandleList_;
            /* 连接,相同的ip,端口,用户名,密码应共用一个连接,以避免连接数过多,连接只能在线程中创建,离开线程时连接被清空 */
            void *pDHSDKPlayBack_P_Connect;
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
#define DHSDK_PlayBack_item_get_dirname(___pDHSDK_PlayBack_item) (((char*)((___pDHSDK_PlayBack_item)+1))+(___pDHSDK_PlayBack_item)->ro._dirname_)
            unsigned short _dirname_;
            /* 域名,可为空 */
#define DHSDK_PlayBack_item_get_domain(___pDHSDK_PlayBack_item,___nullValue) (!FsNumberIsMax((___pDHSDK_PlayBack_item)->ro._domain_)\
    ?(((char*)((___pDHSDK_PlayBack_item)+1))+(___pDHSDK_PlayBack_item)->ro._domain_):(___nullValue))
            unsigned short _domain_;
            /* 用户名 */
#define DHSDK_PlayBack_item_get_username(___pDHSDK_PlayBack_item) (!FsNumberIsMax((___pDHSDK_PlayBack_item)->ro._username_)\
    ?(((char*)((___pDHSDK_PlayBack_item)+1))+(___pDHSDK_PlayBack_item)->ro._username_):((char*)"NULL"))
            unsigned short _username_;
            /* 密码 */
#define DHSDK_PlayBack_item_get_password(___pDHSDK_PlayBack_item) (!FsNumberIsMax((___pDHSDK_PlayBack_item)->ro._password_)\
    ?(((char*)((___pDHSDK_PlayBack_item)+1))+(___pDHSDK_PlayBack_item)->ro._password_):((char*)"NULL"))
            unsigned short _password_;
            /* url地址,不可为空此处为通道号如/1 */
#define DHSDK_PlayBack_item_get_url(___pDHSDK_PlayBack_item) (!FsNumberIsMax((___pDHSDK_PlayBack_item)->ro._url_)\
    ?(((char*)((___pDHSDK_PlayBack_item)+1))+(___pDHSDK_PlayBack_item)->ro._url_):((char*)"/1"))
            unsigned short _url_;
        } ro;

        struct {
        } rw;
    };
#define __DHSDK_PlayBack_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,3,___format",p=%p/%p,server(%hhu/%hhu/%hhu/%hhu/%hhu/%hhu/%d):\"%s:%s@%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",dirname:\"%s\",queryStatus=%hhd,queryLayer=%hu,doQueryIndex=%hu,recordCheckType=%hhu/%hhu\
,lastCheckVideoInfoTime=%lf,recordVideoList=%p/%lu,pSdkPlayBackTask=%p,pPlayBackHandleList_=%p/%lu/%d,pDHSDKPlayBack_P_Connect=%p.\n",##__VA_ARGS__\
        ,pDHSDK_PlayBack_item,pDHSDK_PlayBack_item->p.pDHSDK,pDHSDK_PlayBack_item->p.status,pDHSDK_PlayBack_item->p.busy,pDHSDK_PlayBack_item->p._controlStatus\
        ,pDHSDK_PlayBack_item->p.recordInfo.infoLoad,pDHSDK_PlayBack_item->p.recordInfo.lastVideoInfoStatus__recordVideoList,pDHSDK_PlayBack_item->p.recordInfo.lastVideoInfoStatusCheckV,pDHSDK_PlayBack_item->p.pipe_w\
        ,DHSDK_PlayBack_item_get_username(pDHSDK_PlayBack_item), DHSDK_PlayBack_item_get_password(pDHSDK_PlayBack_item)\
        ,DHSDK_PlayBack_item_get_domain(pDHSDK_PlayBack_item,""),DHSDK_PlayBack_item_get_domain(pDHSDK_PlayBack_item,NULL)?"(":""\
        ,Fs_Network_Ipv4_print(pDHSDK_PlayBack_item->p._ipv4),DHSDK_PlayBack_item_get_domain(pDHSDK_PlayBack_item,NULL)?")":"",pDHSDK_PlayBack_item->p._ipv4Port\
        ,DHSDK_PlayBack_item_get_url(pDHSDK_PlayBack_item),DHSDK_PlayBack_item_get_dirname(pDHSDK_PlayBack_item)\
        ,pDHSDK_PlayBack_item->p.queryStatus,pDHSDK_PlayBack_item->p.queryLayer,pDHSDK_PlayBack_item->p.doQueryIndex\
        ,pDHSDK_PlayBack_item->p.recordInfo.recordCheckType,pDHSDK_PlayBack_item->p.recordInfo.recordStateUpdate\
        ,pDHSDK_PlayBack_item->p.recordInfo.lastCheckVideoInfoTime,pDHSDK_PlayBack_item->p.recordInfo.__recordVideoList,pDHSDK_PlayBack_item->p.recordInfo.__recordVideoList->nodeCount,pDHSDK_PlayBack_item->p.pSdkPlayBackTask\
        ,pDHSDK_PlayBack_item->p.pPlayBackHandleList_,pDHSDK_PlayBack_item->p.pPlayBackHandleList_?pDHSDK_PlayBack_item->p.pPlayBackHandleList_->nodeCount:0,DHSDK_PlayBack_item_PlayBackHandleCountMax\
        ,pDHSDK_PlayBack_item->p.pDHSDKPlayBack_P_Connect)
#define DHSDK_PlayBack_itemLog(___loglevel,___format, ...) __DHSDK_PlayBack_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define DHSDK_PlayBack_itemLog2(___loglevel,___format, ...) __DHSDK_PlayBack_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)


    /* 
     * 创建一个新的HKSDK对象;
     * 返回DHSDK指针.
     */

    DHSDK *dhsdk_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread);

    /* 删除pDHSDK指向的实例对象 */

    void dhsdk_delete__OI(DHSDK * const pDHSDK);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void dhsdk_startThread(DHSDK * const pDHSDK
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void dhsdk_stopThread_noWait(DHSDK * const pDHSDK);

    /* 结束线程 */

    void dhsdk_stopThread(DHSDK * const pDHSDK);

    /* 
     * 创建一个新的DHSDK_PlayBack_item对象;
     * 返回DHSDK_PlayBack_item指针.
     */

    struct DHSDK_PlayBack_item * dhsdk_PlayBack_item_new__IO(/* 数据目录,如/vsys/aa/video/ */ const char dirname[]
            , /* 域名,可为空 */ const char domain[], /* ipv4地址 */const unsigned int ipv4, /* ipv4地址端口号,此值为0表示是推流 */const unsigned short ipv4Port
            , /* 用户名,不能为空 */ const char username[], /* 密码,不能为空 */const char password[], /* url地址,不可为空此处为通道号如/1 */const char url[]);

    /* 
     * 创建一个新的DHSDK_PlayBack_item对象;
     * 成功返回DHSDK_PlayBack_item指针;
     * 失败返回NULL.
     */

    struct DHSDK_PlayBack_item * dhsdk_PlayBack_item_new_byUrl__IO(/* 数据目录,如/vsys/aa/video/ */ const char dirname[], /* dhsdk://admin:12345@192.168.1.101:37777/1 */const char url[]);

    /* 删除pDHSDK_PlayBack_item指向的实例对象 */

    void dhsdk_PlayBack_item_delete__OI(struct DHSDK_PlayBack_item * const pDHSDK_PlayBack_item);

    /* 向pDHSDK中添加一个pDHSDK_PlayBack_item客户端条目 */

    void dhsdk_PlayBack_add_item__OI_2(DHSDK * const pDHSDK, struct DHSDK_PlayBack_item * const pDHSDK_PlayBack_item);

    /* 从pDHSDK中移除一个pDHSDK_PlayBack_item客户端条目,未等待线程真正移除 */

    void dhsdk_PlayBack_remove_item_noWait__IO_2(DHSDK * const pDHSDK, struct DHSDK_PlayBack_item * const pDHSDK_PlayBack_item);

    /* 从pDHSDK中移除一个pDHSDK_PlayBack_item客户端条目 */

    void dhsdk_PlayBack_remove_item__IO_2(DHSDK * const pDHSDK, struct DHSDK_PlayBack_item * const pDHSDK_PlayBack_item);
  
    /* 创建一个回放任务,内部应检查dirName是否重复,相同的dirName应视为相同的回放任务,更新参数,必须在pDHSDK_PlayBack_item加入到pDHSDK后才能调用 */

    void dhsdk_PlayBack_item_updatePlayBackTask_pthreadSafety(DHSDK * const pDHSDK, struct DHSDK_PlayBack_item * const pDHSDK_PlayBack_item
            , /* 回放task */FsObjectSdkPlayBackTask * const pTask);
  
    /* 删除一个回放任务,内部会先检查pTask是否在内部,在内部会删除,必须在pDHSDK_PlayBack_item加入到pDHSDK后才能调用 */

    void dhsdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2(DHSDK * const pDHSDK, struct DHSDK_PlayBack_item * const pDHSDK_PlayBack_item
            , /* 回放task */FsObjectSdkPlayBackTask * const pTask);
  

    void dhsdk_test(void *pMonitor);

#ifdef __cplusplus
}
#endif

#endif /* HKSDK_H */


