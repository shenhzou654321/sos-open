/* 
 * File:   GB28181.h
 * Author: fslib
 *
 * Created on 2017年10月27日, 上午10:50
 */
#if !defined GB28181_H && !defined JAVA_IN_C

#define GB28181_H
#include "../../PublicTools/publicDefine.h"

#define GB28181_Debug
#define GB28181Client_Debug

#include <pthread.h>
#include <sys/epoll.h>

#include "../../PublicTools/Fs/CycleList.h"
#include "../../PublicTools/Fs/Kcp.h"
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/Router.h"
#include "../../PublicTools/Fs/StructList.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 定义 */
    struct _GB28181Server_P_client {

        /* 使用GB28181Server_P_client的定义 */
        union {

            struct {
                /* 继承FsCycleListNode,此变量仅代表储存的空间,不可直接访问此变量,实际数据在GB28181Sip_P_client的头部 */
                struct FsCycleListNode cycleListNode;
                /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
                 * 锁的定义:
                 *     实时客户端指pClient->pRtspServer_item->ro.__mutex_realClientList
                 *     历史客户端指pClient->pRtspServer_item->ro.__mutex_demandClientList
                 */
                FsDelete *_delete;
            } _real_GB28181Sip_P_client;

            struct {
                /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
                 * 锁的定义:
                 *     实时客户端指pClient->pRtspServer_item->ro.__mutex_realClientList
                 *     历史客户端指pClient->pRtspServer_item->ro.__mutex_demandClientList
                 */
                FsDelete *_delete;
                /* 来源连接标识,虚拟连接时储存标识 */
                unsigned int src_requestID_3[3];
            } _virtual_GB28181Sip_P_client;

            struct {
                /* 继承FsCycleListNode,此变量仅代表储存的空间,不可直接访问此变量,实际数据在GB28181Sip_P_client的头部 */
                struct FsCycleListNode cycleListNode;
                /* 删除sock[0]使用的指针方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
                 * 锁的定义:
                 *     实时客户端指pGB28181->rw.__mutex_realClientList
                 *     历史客户端指pGB28181->rw.__mutex_demandClientList
                 */
                FsDelete *_delete1;
                /* 删除sock[1]使用的指针方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
                 * 锁的定义:
                 *     实时客户端指pGB28181->rw.__mutex_realClientList
                 *     历史客户端指pGB28181->rw.__mutex_demandClientList
                 */
                FsDelete *_delete2;
            } _h_GB28181Server_P_client;
        } _h;
        /* 回放客户端 */
        struct GB28181Server_item_DemandClient *pDemandClient;
    };
#define GB28181Server_DemandClient_get(___pClient) ((struct _GB28181Server_P_client*)___pClient)->pDemandClient

#define GB28181Server_MTU 1416 //1462
    /*
     * 线程规划设计:
     *     一主线程监听端口,通信所有的sip通信;
     *     主线程创建任意数量的客户端线程,用于获取其他平台的数据流;
     *     主线程创建任意数量的服务端线程,用于对外提供视频流;
     * 客户端线程设计:
     *     客户端线程所需的sip信息必须在主线程中存在,不能在其他主机中;
     * 服务端线程设计:
     *     主线程记录了所有服务项的信息,并对外提供,但真正的服务项并不完全存在于本地中,对不在本机中的服务项应转发指令由数据拥有者提供服务(具体细节待设计);
     *     本机各服务项的相机状态统计由外部通过函数通知,其他节点的相机状态由本模块主动获取(具体细节待设计);
     *     服务器发送流使用sip端口;
     */

    /* GB28181服务 */
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
            /* 
             * 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出 
             */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 管道,用于添加和删除通道或发送数据 */
            int __pipe[2];
            /* 线程名 */
#define GB28181_get_threadName(___pGB28181) (((char*)((___pGB28181)+1))+(___pGB28181)->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* 配置管理对象,为空表示不支持与之相关的业务 */
            void *_pConfigManager;
            /* 监听分类服务器对象,可为空 */
            void *_pBindClassifyServer;
            /* 本地地址映射,拉流时使用的地址,用于初始化forSip的GB28181Client_item */
            FsRouter * __pAddrMap;
            /* 获取通道状态信息,为空表示不支持,校验和发生改变时返回FsEbml指针,否则返回NULL,返回NULL可能是出错 */
            FsEbml * (* _channelStatusGet__IO) (/* 调用时缓存的通道最后改变的编号,在发生改变时才返回数据,不为空 */unsigned long long *pChannelStatusIndex, void * channelStatusP, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 获取通道状态信息相关的指针 */
            void * _channelStatusP;
            /* GB28181 Sip条目链表,按指针排序 */
            FsObjectList *__itemListSip_;
            /* GB28181客户端条目链表,按指针排序 */
            FsObjectList *__itemListClient_;
            /* GB28181本地服务器条目链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__itemListServer_;
            /* 集群的通道链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__clusterItemListServer_;
            /* udp端口对,成员为两个usigned short 大小,记录哪些udp端口对被使用了,不管是自动绑定的还是预分配的都应加入端口对中,rtp和rtcp端口都要加入,端口使用invite中标记的端口(实际用到的端口是经过路由nat,如对方取流也不会使用nat后的端口) */
            FsStructList *__udpPortList;
            /* tcp端口对,成员为两个usigned short 大小,只能使用预分配端口,只加入rtp端口,端口使用invite中标记的端口(实际用到的端口是经过路由nat,如对方取流也不会使用nat后的端口) */
            FsStructList *__tcpPortList;
            /* 获取目录查询结果的客户端链表 */
            FsStructList *__catalogDataClientList;
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
            /* 目录查询,启用后会查询并缓存目录查询结果,0-关闭,1-仅生成连接信息,3-启用 */
            unsigned char catalog;
            /* 目录查询数据的更新标记,0-没有更新,1-有更新 */
            unsigned char catalogDataUpdate;
            /* ipv4地址端口号,参数需要取消 */
            unsigned short _ipv4Port;
            /* 监听的ipv4地址,为0表示监听所有地址 */
            unsigned int _ipv4;
            /* 实时视频缓存的超时时间 */
            float _realBufferTimeOut;
            /* 外部和内部线程异步通信的链表,结构为GB28181_P_control */
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
            /* 当前服务端的工作线程数 */
            unsigned short workThreadServer;
            /* 组织数量 */
            unsigned short groupCount;
            /* tcpStreamPortList使用的索引号 */
            unsigned int tcpStreamPortListIndex;
            /* 线程的tid号 */
            pthread_t stid;
            /* 监听分类服务器对象 */
            void *pBindClassifyServer;
            /* GB28181 组织头,按parent排序,父在前,子在后 */
            void *__groupHead;
            /* 端口指针,前2字节表示端口个数,第3个字节表示是否有强制主线程建立连接的任务,第4字节表示是否是第一次监听,第一次监听应不设置等待时间
             * ,第5-6字节表示上一次端口遍历的索引,只在主线程中被访问,使用udpPortList的互斥锁 */
            void *__udpStreamPort__udpPortList;
            /* 缓存Buffer */
            FsObjectBaseBuffer * pObjectBaseBuffer;
            /* 共享buffer */
            FsShareBuffer * pShareBuffer;
            /* tcp端口,记录哪些tcp端口可被使用了,成员为GB28181_P_StreamPort_tcp,使用tcpPortList的互斥锁 */
            FsStructList *tcpStreamPortList__tcpPortList;
            /* 主线程的线程号,由systemThreadTid获得 */
            pthread_t mainTid;
            /* 新加入的客户端服务条目链表,用于主线程向客户端线程分发客户端 */
            FsObjectList *newItemListClient;
            /* 需要检查录像记录的GB28181Client_item,在GB28181Client_item与sip绑定后才加到此链表中,使用pGB28181Client_item->p.GB28181Sip_P_client和pGB28181Client_item的地址排序 */
            FsObjectList *itemListClientCheckRecordInfoOrder_;
            /* 储存正准备收流的但流未初始化完成的客户端,客户端线程断开连接后应再次添加到此链表中,使用newItemListClient的互斥锁 */
            FsObjectList *streamItemListClient__newItemListClient;
            /* 缓存未与sip建立绑定关系的GB28181Client_item */
            FsObjectList *itemListClientCheckSip_;
            /* 线程异步通信的链表,结构为GB28181_private_control */
            FsStructList *controlListClient;
            /* 客户端线程对象链表 */
            FsObjectList *threadListClient;
            /* 新加入的服务通道条目链表 */
            FsObjectList *newItemListServer;
            /* 本地的服务端通道数量 */
            unsigned long localItemListCount;
            /* 线程异步通信的链表,结构为GB28181_private_control */
            FsStructList *controlListServer;
            /* 服务端线程对象链表 */
            FsObjectList *threadListServer;
            /* 缓存要关闭的一些客户端连接 */
            FsStructList * pStructList_socketBuffer;
            /* 虚拟的sip连接,按虚拟的源集群主机节点ID,源ip,源端口,目的ip,目的端口排序,当_pConfigManager为空时此值也为空 */
            FsObjectList *vSipClientList;
            /* 缓存集群cmd信息,当_pConfigManager为空时此值也为空,使用vSipClientList的互斥锁 */
            FsObjectList *clusterCmdList_;
            /* 创建的虚拟流分发连接,未加入epoll前放在此链表中,使用vSipClientList的互斥锁 */
            FsObjectList *vServerClientListA_;
            /* 创建的虚拟流分发连接,按指针地址排序,已加入epoll,使用vSipClientList的互斥锁 */
            FsObjectList *vServerClientList_;
            /* 待删除的虚拟流分发连接,已加入epoll,使用vSipClientList的互斥锁 */
            FsObjectList *vServerClientListD_;
            /* 虚拟sip代理的原理:
             *    收到其他节点的命令时,把命令缓存到clusterCmdList_中,并增加来源连接的引用(直接处理可能会导致处理时间过长或某数据的线程不安全性,导致错误);
             *    接收到其他节点连接错误(其他节点出现故障了)的信息时,清vSipClientList与clusterCmdList_中所有与此节点连接相关的所有连接数据;
             */
            /* 通过pConfigManager创建的代理GB28181Client_item对象用于把sip指令转发出去 */
            FsObjectList *proxyGB28181Client_itemList;
            /* proxyGB28181Client_itemList中需要删除的对象,直接删除可能会产生死锁 */
            FsObjectList *proxyGB28181Client_itemList_delete__proxyGB28181Client_itemList;
            /* 目录查询,为空应返回错误,使用catalogDataClientList的线程锁 */
            FsEbml *catalogData__catalogDataClientList;
            /* 目录查询ebml数据,使用catalogDataClientList的线程锁 */
            FsObjectBasePthreadSafety *catalogData_ebml__catalogDataClientList;
            /* 目录查询xml数据,使用catalogDataClientList的线程锁*/
            FsObjectBasePthreadSafety *catalogData_xml__catalogDataClientList;
            /* 目录查询json数据,使用catalogDataClientList的线程锁 */
            FsObjectBasePthreadSafety *catalogData_json__catalogDataClientList;
            /* 使用struct ConfigManager_connectNode_useOnce结构缓存一些请求,用于报告一些线程参数,使用catalogDataClientList的线程锁 */
            void * pConnectNode__catalogDataClientList;
        } p;
    } GB28181;
#define __GB28181Log(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  channelCountPerThread=%u\n  pConfigManager=%p\n  pBindClassifyServer=%p/%p\n  pAddrMap=%p\n  channelStatusGet__IO=%p/%p\n  itemListSip=%p/%lu\n  itemListClient=%p/%lu\n  itemListServer=%p/%lu\n  clusterItemListServer=%p/%lu\n  udpPortList=%p/%lu\n  tcpPortList=%p/%lu\n\
  catalogDataClientList=%p/%lu\n  catalog=%hhu/%hhu\n  ipv4:\"%hhu.%hhu.%hhu.%hhu:%hu\",realBufferTimeOut=%f\n",##__VA_ARGS__\
        ,pGB28181,((GB28181*)(pGB28181))->rw.runStatus,((GB28181*)(pGB28181))->p.stid\
        ,((GB28181*)(pGB28181))->ro._pMonitor,((GB28181*)(pGB28181))->ro._externRunStatus,((GB28181*)(pGB28181))->ro._parentTid,((GB28181*)(pGB28181))->ro.__pipe[0], ((GB28181*)(pGB28181))->ro.__pipe[1],GB28181_get_threadName(pGB28181)\
        ,((GB28181*)(pGB28181))->ro._channelCountPerThread,((GB28181*)(pGB28181))->ro._pConfigManager,((GB28181*)(pGB28181))->ro._pBindClassifyServer,((GB28181*)(pGB28181))->p.pBindClassifyServer,((GB28181*)(pGB28181))->ro.__pAddrMap\
        ,((GB28181*)(pGB28181))->ro._channelStatusGet__IO,((GB28181*)(pGB28181))->ro._channelStatusP\
        ,((GB28181*)(pGB28181))->ro.__itemListSip_,((GB28181*)(pGB28181))->ro.__itemListSip_->nodeCount, ((GB28181*)(pGB28181))->ro.__itemListClient_, ((GB28181*)(pGB28181))->ro.__itemListClient_->nodeCount\
        ,((GB28181*)(pGB28181))->ro.__itemListServer_ ,((GB28181*)(pGB28181))->ro.__itemListServer_->nodeCount, ((GB28181*)(pGB28181))->ro.__clusterItemListServer_, ((GB28181*)(pGB28181))->ro.__clusterItemListServer_->nodeCount\
        ,((GB28181*)(pGB28181))->ro.__udpPortList,((GB28181*)(pGB28181))->ro.__udpPortList->nodeCount,((GB28181*)(pGB28181))->ro.__tcpPortList,((GB28181*)(pGB28181))->ro.__tcpPortList->nodeCount\
        ,((GB28181*)(pGB28181))->ro.__catalogDataClientList,((GB28181*)(pGB28181))->ro.__catalogDataClientList->nodeCount\
        ,((GB28181*)(pGB28181))->rw.catalog,((GB28181*)(pGB28181))->rw.catalogDataUpdate,Fs_Network_Ipv4_print(((GB28181*)(pGB28181))->rw._ipv4),((GB28181*)(pGB28181))->rw._ipv4Port,((GB28181*)(pGB28181))->rw._realBufferTimeOut)
#define GB28181Log(___loglevel,___format, ...) __GB28181Log(FsLog,___loglevel,___format,##__VA_ARGS__)
#define GB28181Log2(___loglevel,___format, ...) __GB28181Log(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* GB28181Sip对象 */
    struct GB28181Sip_item {

        /* p放在前面,虚拟sip客户端时只分配存储pGB28181的空间 */
        struct {
            /* 所属的GB28181对象,不能根据其是否为空来判断是否加入或移除线程,必须放在第一个,sip虚拟对象要使用 */
            GB28181 *pGB28181;
            /* 下一个拥有相同"本地id"和"SIP服务器ID"的sip对象 */
            struct GB28181Sip_item *next;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char controlStatus : 1;
            /* 最后一次被需要的时间,用于拉流客户端建立的sip对象 */
            double lastGetStreamUsedTime;
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
            /* 代理服务器地址,0表示不使用代理 */
            unsigned int _proxyServer;
            /* 本地id,不能为空 */
#define GB28181Sip_item_get_id(pGB28181Sip_item) (((char*)((pGB28181Sip_item)+1))+(pGB28181Sip_item)->ro._id_)
            unsigned char _id_;
            /* SIP服务器ID,不能为空 */
#define GB28181Sip_item_get_sip_id(pGB28181Sip_item) (((char*)((pGB28181Sip_item)+1))+(pGB28181Sip_item)->ro._sip_id_)
            unsigned char _sip_id_;
            /* 检验和的长度,使用本地id+SIP服务器ID作为连接标示符 */
#define GB28181Sip_item_get_checkSumLen(pGB28181Sip_item) ((pGB28181Sip_item)->ro._sip_id_+strlen(GB28181Sip_item_get_sip_id(pGB28181Sip_item))+1)
            /* SIP服务器域,可为空,不做校验 */
#define GB28181Sip_item_get_sip_domain(pGB28181Sip_item,nullValue) (!FsNumberIsMax((pGB28181Sip_item)->ro._sip_domain_)?(((char*)((pGB28181Sip_item)+1))+(pGB28181Sip_item)->ro._sip_domain_):(nullValue))
            unsigned char _sip_domain_;
            /* 本机使用的地址信息,一般在NAT环境中使用,为空时程序自动计算 */
#define GB28181Sip_item_get_addr_info(pGB28181Sip_item,nullValue) (!FsNumberIsMax((pGB28181Sip_item)->ro._addr_info_)?(((char*)((pGB28181Sip_item)+1))+(pGB28181Sip_item)->ro._addr_info_):(nullValue))
            unsigned char _addr_info_;
            /* SIP用户认证ID,可为空 */
#define GB28181Sip_item_get_sip_username(pGB28181Sip_item,nullValue) (!FsNumberIsMax((pGB28181Sip_item)->ro._sip_username_)?(((char*)((pGB28181Sip_item)+1))+(pGB28181Sip_item)->ro._sip_username_):(nullValue))
            unsigned short _sip_username_;
            /* SIP用户认证密码,可为空 */
#define GB28181Sip_item_get_sip_password(pGB28181Sip_item,nullValue) (!FsNumberIsMax((pGB28181Sip_item)->ro._sip_password_)?(((char*)((pGB28181Sip_item)+1))+(pGB28181Sip_item)->ro._sip_password_):(nullValue))
            unsigned short _sip_password_;
            /* SIP服务器地址,为0表示只接受被注册 */
            unsigned int _sip_ipv4;
            /* SIP服务器端口 */
            unsigned short _sip_ipv4Port;
            /* 自定义配置,根据对方sip的特性开启特殊处理
             * (1 << 0):目录订阅回执中callid增加错误信息
             * (1 << 1):注册即推送目录订阅信息
             * (1 << 2):不进行目录查询
             */
            unsigned char _customConfig;
            /* 是否仅用于客户端拉流 */
            unsigned char useForClientStream : 1;
            /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */
            unsigned char _sipOver : 2;
            /* 当前使用的协议,0-表示udp,1-表示tcp */
#define GB28181Sip_item_sipOver_next(___pClientSip,___pGB28181Sip_item) do{\
    if(0==(___pClientSip)->proxyID){\
        if(0==(___pClientSip)->isTcp){\
            if((___pGB28181Sip_item)->ro._sipOver & 0x2)(___pClientSip)->isTcp=1;\
        }else{\
            if((___pGB28181Sip_item)->ro._sipOver & 0x1)(___pClientSip)->isTcp=0;\
        }\
    }\
}while(0)

            /* 与本sip相关的客户端,允许多次注册,第一个为同本对象关联的客户端 */
            FsCycleList __clientList;
            /* 本地地址映射,主动发起命令时填写的本地地址 */
            FsRouter * __pAddrMap;
        } ro;

        struct {
        } rw;
    };
#define __GB28181Sip_itemLog(___Log,___loglevel,___format, ...) \
    ___Log(___loglevel,5,___format",p=%p,proxyServer:\"%hhu.%hhu.%hhu.%hhu\",status=%hhx/%hhx/%hhu,sipOver=%hhx,clientCount=%lu,server:\"%hhu.%hhu.%hhu.%hhu:%hu\",id:\"%s\",sip_id:\"%s\",sip_domain:\"%s\",addr_info:\"%s\"\
    ,sip_username:\"%s\",sip_password:\"%s\",pAddrMap=%p.\n",##__VA_ARGS__\
        ,pGB28181Sip_item,Fs_Network_Ipv4_print(pGB28181Sip_item->ro._proxyServer),pGB28181Sip_item->p.controlStatus,pGB28181Sip_item->p.busy,pGB28181Sip_item->ro.useForClientStream,pGB28181Sip_item->ro._sipOver\
        ,Fs_CycleList_get_count(pGB28181Sip_item->ro.__clientList)\
        ,Fs_Network_Ipv4_print(pGB28181Sip_item->ro._sip_ipv4),pGB28181Sip_item->ro._sip_ipv4Port\
        ,GB28181Sip_item_get_id(pGB28181Sip_item),GB28181Sip_item_get_sip_id(pGB28181Sip_item)\
        ,GB28181Sip_item_get_sip_domain(pGB28181Sip_item,""), GB28181Sip_item_get_addr_info(pGB28181Sip_item,"")\
        ,GB28181Sip_item_get_sip_username(pGB28181Sip_item,""), GB28181Sip_item_get_sip_password(pGB28181Sip_item,""),pGB28181Sip_item->ro.__pAddrMap)
#define GB28181Sip_itemLog(___loglevel,___format, ...) __GB28181Sip_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define GB28181Sip_itemLog2(___loglevel,___format, ...) __GB28181Sip_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 录像信息,在gb28181模块中不负责信息从硬盘读取,也不负责有变化时写入硬盘 */
    struct GB28181Client_item_recordInfo {
#define GB28181Client_item_recordInfo_chunkTime (6*60*60) // 检查的时间分段
        /* 向前检查的最小时间值,初始化为当前时间-GB28181Client_item_recordInfo_chunkTime,在一轮检测结束后根据checkCycleHasDelete进行调整
         * checkCycleHasDelete为1时,调整previousTimeMin为C模式;为0时减小previousTimeMin以增大检测范围,同时如__recordVideoList为空需要调整nextTime时间为最新时间
         */
        double previousTimeMin;
        /* 下一次向前探测的开始时间,在收到回执(或超时)后向前更新 */
        double previousTime;
        /* 下一次向后探测的开始时间,在收到回执(或超时)后向后更新 */
        double nextTime;
        /* 最近一次检查录像信息的时间 */
#define GB28181Client_item_recordInfo_lastCheckVideoInfoTime_add_changed 3.0
#define GB28181Client_item_recordInfo_lastCheckVideoInfoTime_add_notChanged 7.0
#define GB28181Client_item_recordInfo_lastCheckVideoInfoTime_add_error 17.0
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
#define GB28181Client_item_recordInfo_recordCheckType_C_startTime(___recordVideoList) ((((double (*)[2])___recordVideoList->pNode) + ___recordVideoList->startIndex)[0][0] - GB28181Client_item_recordInfo_chunkTime / 4)
        /* A切换时应切换为B还是C(在A切换为其他时修改此值),0-表示下次切换为B,1-表示下次切换为C */
        unsigned char switchBC : 1;
#define GB28181Client_item_recordInfo_sipCheckMethod_get_next(___recordInfo) ((___recordInfo)->sipCheckMethod+1)%2
        /* 检查与sip关联的方法,轮巡检测,0-录像查询方法,1-直播请求方法 */
        unsigned char sipCheckMethod : 1;
        /* 录像信息是否已加载过历史数据(内部只在已加载过录像数据才会从网络更新),0-未加载,1-已加载 */
        unsigned char infoLoad : 1;
        /* 循环检测中是否出现了数据删除(有数据删除时会调整previousTimeMin为C模式,没有会减小previousTimeMin以增大检测范围),0-没有,1-有 */
        unsigned char checkCycleHasDelete : 1;
        /* 是否正在被回放中(在出现与之相关的客户端时,可设置此值,以便可以加快此路视频的录像查询,提高其实时性),0-没有,1-在 */
        unsigned char isPlayBack : 1;
        /* 检查的编号(取双数,单数回放时查询录像使用),用以校验回执数据是否是与本对象相关(只能取6位,proxyID为24位,总共30位,31位可能出现一些平台无法识别的情况) */
        unsigned char checkIndex : 6;
        /* 最后一段录像的持续状态,0-结束状态,1-视频持续录像中 */
        unsigned char lastVideoInfoStatus__recordVideoList : 1;
        /* 最后一段录像的持续状态切换使用的参数,在lastVideoInfoStatus为1时,数据有增长时把此值置为GB28181Client_item_recordInfo_lastVideoInfoStatusCheckV0,每检查一次此值减1,当值为0时设置lastVideoInfoStatus为0 */
#define GB28181Client_item_recordInfo_lastVideoInfoStatusCheckV0 3
        unsigned char lastVideoInfoStatusCheckV;
        /* 录像状态是否有更新,0-没有更新,1-有更新 */
        unsigned char recordStateUpdate : 1;
        /* 视频数据的帧率,用以让接收数据时的帧率可参考之前的帧率 */
        float videoFrameRate;
#define GB28181Client_item_recordInfo_lastVideoInfoStatusCheckV0Timeout 130 // 最后一次检查到录像持续更新到下一次录像断开的最小时间间隔,海康nvr有的120S更新一次
        double lastVideoInfoStatusCheckVLastUptime;
        /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */
        FsStructList *__recordVideoList;
        /* 缓存一次分段检测的结果(在第一次收到数据时初始化),节点为两个double大小,第一个表示开始时间,第二个表示结束时间,在数据收完整后再与__recordVideoList中的数据进行合并 */
        FsStructList *__recordVideoListBuffer;
        /* __recordVideoListBuffer还需要收取的数据量 */
        unsigned int recordVideoListBufferRemainCount;
        //        /* 不为空时在录像信息更新时通过此函数回调 */
        //        void (*cb_update)(void *cb_P);
        //        /* cb_update回调时外部传入的参数 */
        //        void *cb_P;
    };
    /* 对__recordInfo中需要非0初始化的变量进行初始化 */
#define GB28181Client_item_recordInfo_init_not_0(___recordInfo/*,___cb_update,___cb_P*/) do{\
    const double t=time(NULL);\
    /* 向前检查的最小时间值,初始化为当前时间-GB28181Client_item_recordInfo_chunkTime,在一轮检测结束后根据checkCycleHasDelete进行调整\
     * checkCycleHasDelete为1时,调整previousTimeMin为C模式;为0时减小previousTimeMin以增大检测范围,同时如__recordVideoList为空需要调整nextTime时间为最新时间\
     */\
    (___recordInfo)->previousTimeMin=t-GB28181Client_item_recordInfo_chunkTime;\
    /* 下一次向前探测的开始时间,在收到回执(或超时)后向前更新 */\
    (___recordInfo)->previousTime=t-GB28181Client_item_recordInfo_chunkTime;\
    /* 下一次向后探测的开始时间,在收到回执(或超时)后向后更新 */\
    (___recordInfo)->nextTime=t-GB28181Client_item_recordInfo_chunkTime/2;\
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
    /* 检查与sip关联的方法,轮巡检测,0-录像查询方法,1-直播请求方法 */\
    (___recordInfo)->sipCheckMethod=1;\
    /* 录像信息是否已加载过历史数据(内部只在已加载过录像数据才会从网络更新),0-未加载,1-已加载 */\
    /* unsigned char infoLoad : 1; */\
    /* 循环检测中是否出现了数据删除(有数据删除时会调整previousTimeMin为C模式,没有会减小previousTimeMin以增大检测范围),0-没有,1-有 */\
    /* unsigned char checkCycleHasDelete : 1; */\
    /* 是否正在被回放中(在出现与之相关的客户端时,可设置此值,以便可以加快此路视频的录像查询,提高其实时性),0-没有,1-在 */\
    /* unsigned char isPlayBack:1; */\
    /* 检查的编号,用以校验回执数据是否是与本对象相关(只能取6位,proxyID为24位,总共30位,31位可能出现一些平台无法识别的情况) */\
    /* unsigned char checkIndex : 6; */\
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
#define GB28181Client_item_recordInfo_release(___recordInfo) do{\
    /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */\
    fs_StructList_delete__OI((___recordInfo)->__recordVideoList);\
    /* 记录录像时间信息(在第一次收到数据时初始化),节点为两个double大小,第一个表示开始时间,第二个表示结束时间,用于缓存一次分段检测的结果,在数据收完整后再与__recordVideoList中的数据进行合并 */\
    if((___recordInfo)->__recordVideoListBuffer)fs_StructList_delete__OI((___recordInfo)->__recordVideoListBuffer);\
}while(0)
    /* 清理___recordInfo中除recordVideoList中的数据(即不清除recordVideoList中的数据,也不设置硬盘加载标记和录像是否持续更新标记) */
#define GB28181Client_item_recordInfo_clean_except_recordVideoList(___recordInfo) do{\
     const double t = time(NULL);\
    /* 向前检查的最小时间值,初始化为当前时间-GB28181Client_item_recordInfo_chunkTime,在一轮检测结束后根据checkCycleHasDelete进行调整\
     * checkCycleHasDelete为1时,调整previousTimeMin为C模式;为0时减小previousTimeMin以增大检测范围,同时如__recordVideoList为空需要调整nextTime时间为最新时间\
     */\
    (___recordInfo)->previousTimeMin=t-GB28181Client_item_recordInfo_chunkTime;\
    /* 下一次向前探测的开始时间,在收到回执(或超时)后向前更新 */\
    (___recordInfo)->previousTime=t-GB28181Client_item_recordInfo_chunkTime;\
    /* 下一次向后探测的开始时间,在收到回执(或超时)后向后更新 */\
    (___recordInfo)->nextTime=t-GB28181Client_item_recordInfo_chunkTime/2;\
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
    /* 检查的编号,用以校验回执数据是否是与本对象相关(只能取6位,proxyID为24位,总共30位,31位可能出现一些平台无法识别的情况) */\
    (___recordInfo)->checkIndex=((___recordInfo)->checkIndex + 1)&0x3F;\
    /* 最后一段录像的持续状态,0-结束状态,1-视频持续录像中 */\
    /* unsigned char lastVideoInfoStatus__recordVideoList : 1; */\
    /* 最后一段录像的持续状态切换使用的参数,在lastVideoInfoStatus为1时,数据有增长时把此值置为GB28181Client_item_recordInfo_lastVideoInfoStatusCheckV0,每检查一次此值减1,当值为0时设置lastVideoInfoStatus为0 */\
    /* unsigned char lastVideoInfoStatusCheckV; */\
    /* 录像状态是否有更新,0-没有更新,1-有更新 */\
    /* unsigned char recordStateUpdate : 1; */\
    /* 视频数据的帧率,用以让接收数据时的帧率可参考之前的帧率 */\
    /* float videoFrameRate; */\
    /* 记录录像时间信息,节点为两个double大小,第一个表示开始时间,第二个表示结束时间 */\
    /* FsStructList *__recordVideoListBuffer; */\
    /* 记录录像时间信息(在第一次收到数据时初始化),节点为两个double大小,第一个表示开始时间,第二个表示结束时间,用于缓存一次分段检测的结果,在数据收完整后再与__recordVideoList中的数据进行合并 */\
    if ((___recordInfo)->__recordVideoListBuffer) {\
        fs_StructList_delete__OI((___recordInfo)->__recordVideoListBuffer);\
        (___recordInfo)->__recordVideoListBuffer = NULL;\
    }\
    /* __recordVideoListBuffer还需要收取的数据量 */\
    (___recordInfo)->recordVideoListBufferRemainCount=0;\
}while(0)

    /* GB28181客户端项,有
     * 1,普通模式(sipServer为0,isForSip为0),用于接收对方的视频流,控制相机等;
     * 2,远程模式(sipServer不为0,isForSip为0),功能与普通模式相同,但内部需要把sip指令转发到sipServer指定的服务器执行;
     * 3,sip通信模式(isForSip不为0)(只能同sip中的一个client进行绑定,无法识别到底哪个client是有效的),此模式下对于使用本对象发出的sip信令,回执指令会直接回调出来,主要用于同远程模式的服务端配对(远程模式发送命令使用GB28181_P_control);
     */
    struct GB28181Client_item {
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         * 无锁的定义;
         */
        /* 删除sock[0]使用的指针 */
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写 */
        FsDelete *_delete1;
        /* 删除sock[1]使用的指针 */
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写 */
        FsDelete *_delete2;

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
            /* sip服务器地址,为0表示内部自建sip服务器(在回放独立运行时需要设置此值) */
            unsigned int _sipServer;
            /* 本地id,不能为空 */
#define GB28181Client_item_get_id(pGB28181Client_item) (((char*)((pGB28181Client_item)+1))+(pGB28181Client_item)->ro._id_)
            unsigned short _id_;
            /* SIP服务器ID,不能为空 */
#define GB28181Client_item_get_sip_id(pGB28181Client_item) (((char*)((pGB28181Client_item)+1))+(pGB28181Client_item)->ro._sip_id_)
            unsigned short _sip_id_;
            /* 检验和的长度,使用本地id+SIP服务器ID作为连接标示符 */
#define GB28181Client_item_get_checkSumLen(pGB28181Client_item) ((pGB28181Client_item)->ro._sip_id_+strlen(GB28181Client_item_get_sip_id(pGB28181Client_item))+1)
            /* SIP服务器域,可为空,不做校验 */
#define GB28181Client_item_get_sip_domain(pGB28181Client_item,nullValue) (!FsNumberIsMax((pGB28181Client_item)->ro._sip_domain_)?(((char*)((pGB28181Client_item)+1))+(pGB28181Client_item)->ro._sip_domain_):(nullValue))
            unsigned short _sip_domain_;
            /* SIP用户认证ID,可为空 */
#define GB28181Client_item_get_sip_username(pGB28181Client_item,nullValue) (!FsNumberIsMax((pGB28181Client_item)->ro._sip_username_)?(((char*)((pGB28181Client_item)+1))+(pGB28181Client_item)->ro._sip_username_):(nullValue))
            unsigned short _sip_username_;
            /* SIP用户认证密码,可为空 */
#define GB28181Client_item_get_sip_password(pGB28181Client_item,nullValue) (!FsNumberIsMax((pGB28181Client_item)->ro._sip_password_)?(((char*)((pGB28181Client_item)+1))+(pGB28181Client_item)->ro._sip_password_):(nullValue))
            unsigned short _sip_password_;
            /* 标识符,不能为空 */
#define GB28181Client_item_get_url(pGB28181Client_item) (((char*)((pGB28181Client_item)+1))+(pGB28181Client_item)->ro._url_)
            unsigned short _url_;
            /* 本地地址映射,拉流时使用的地址 */
            FsRouter * __pAddrMap;
            /* SIP服务器地址,为0表示只接受被注册 */
            unsigned int _sip_ipv4;
            /* SIP服务器端口 */
            unsigned short _sip_ipv4Port;
            /* 此客户端是否用于发送和接收sip信息,0-不是,1-是 */
            unsigned char _isForSip : 1;
            /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */
            unsigned char _sipOver : 2;
            /* 流的承载协议,&0x1-tcp被动模式,&0x2-udp,&0x4-kcp,&0x1-tcp主动模式 */
            unsigned char _streamOver : 4;

            /* 是否启用控制(isForSip为0时且为直播时有效),0-启用,1-不启用 */
            unsigned char _control : 1;
            /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */
            unsigned long long _controlMaskDefault;
            /* 视频帧传出时前面预留大小(isForSip为0时有效) */
            unsigned int _videoPrefix;
            /* 视频帧传出时后面预留大小(isForSip为0时有效) */
            unsigned int _videoSuffix;
            /* 音频帧传出时前面预留大小(isForSip为0时有效) */
            unsigned int _audioPrefix;
            //            /* 在isForSip不为0时,表示收到的sip配对信息 */
            //            void (*_cb)(struct GB28181Client_item * pGB28181Client_item,/* 连接变化,-1连接断开,0-连接未变化,1-连接成功 */signed char connectChange
            //             , /* 回调的数据,在connectChange为0时对会出现,总长度为headLen+4+dataLen */ char data_cin[], /* 回调数据中头部长度 */unsigned int headLen, /* 回调数据中数据区部分长度 */unsigned int dataLen, /* 外部传入的指针 */ void * p
            //             , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 收到数据后的回调函数(isForSip为0时此处表示收到视频数据,不为0时表示收到的sip配对信息),返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            int (*_cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
                    , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */double realTime, /* 绝对时间,可能不是每帧都变化(小于0.0无效)(如数据时从ps中解析到的时间) */ double absoluteTime, /* 相对时间(如rtp时间),在realTime无效时,使用absoluteTime+此参数的变化间接得到realTime */ double relativeTime
                    , /* 帧号,最大值只能到3个字节 */const unsigned int frameIndex
                    , /* 数据类型,1-h264,2-h265 */unsigned char type, /* 是否是关键帧(-1:无法判断;0;不是;1:是) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
                    , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
                    , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
                    , /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void * p);
            /* 外部传入的指针 */
            void *_externP;
        } ro;

        struct {
        }

        rw;

        struct {
            /* 所属的GB28181对象,不能根据其是否为空来判断是否加入或移除线程 */
            GB28181 *pGB28181;
            /* 本相机对应的录像信息,在不为空时,内部需要检查其录像信息 */
            struct GB28181Client_item_recordInfo *_recordInfo;
            /* 掩码(在直播时有效),按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位,8-gotoPTZ和getPTZ,9-图像参数设置与获取,10-直接向机芯发送命令 */
            unsigned long long mask;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除,(controlStatus&0x2)-表示是否开始实时流(未开始实时流时,对象不在工作线程中) */
            unsigned char _controlStatus : 2;
            unsigned char : 5;
            /* 此变量标识实时流的开始与结束(实现开始停止不使用busy变量),0-停止,1-开始,底层是否开始由(_controlStatus&0x2)判断 */
            unsigned char _streamStatus_set : 1;
            /* 与本对象关联的sip客户端,值必须通过sipStatus判定是否有效,在sipStatus为0时,此值不为空是储存的上一次关联的指针,以实现遍历关联 */
            void *pGB28181Sip_P_client;
            /* 当前的回放时间,为0.0表示直播 */
            double _playbackTime;
            /* 回放时的播放速度(playbackTime),初始化为1.0 */
            float _playSpeed;
            /* 用于计算丢包的参数,在丢包量累计到一定值时认为丢包过多 */
            int packetLossCheck;
            /* 关联sip的状态,此值只能在主线程中改变,0-无关联中,1-尝试关联中,2-已确认关联,3-拉流中,在streamStatus变为2时设置为此值 */
            unsigned char sipStatus;
            /* 拉流状态,0-未建立连接,1-已发送取流请求,2-连接成功,等待工作线程处理,对象指针从待取流链表中删除但可能未解除同取流端口的绑定(rtcp需要探测),通过GB28181_P_control结构传递,3-工作线程已接管处理 */
            unsigned char streamStatus;
            /* 回放时是否需要进行录像查询,0-不需要,1-仅发送,2-需要等待回执,3-等到回执后发送二次查询,4-需要等待2次回执,以此类推 */
            unsigned char _playBackQueryRecord : 3;
            /* 是否是暂停状态,0-不是,1-是,在invite时设置为0 */
            unsigned char isPause : 1;
            /* 发送invite后,有无收到回执,在发送invite时设置此值为0,0-表示未收到,1-表示已收到  */
            unsigned char inviteReturnRecv : 1;
            /* 是否在进行主动模式连接中 */
            unsigned char activeConnect : 1;
            /* 在使用udp连接时sock[1]是否已加入epoll */
            unsigned char udpSock1AddToEpoll : 1;
            /* socket的本地端口在发送invite时设置,在收到数据时校验,udp包含两个端口,第二个是port_l+1 */
            unsigned short port_l;
            /* 远程端口,在发送invite时置0,不为0时表示已加入到端口队列中,udp加入两个端口,tcp只加入一个,指invite时对方的端口,udp包含两个端口,第二个是port_ri+1 */
            unsigned short port_ri;
            /* 收发数据的socket,在streamStatus大于1时有效,-1表示无效,sock[1]可能无效,主线程肯定已创建sock[0],tcp只使用1个socket */
            int _sock[2];
            /* 关联的StreamPort,不为空表示本对象的某些端口还需要主线程探测,udp时为GB28181_P_StreamPort,操作需要使用udpPortList的锁,tcp时为GB28181_P_StreamPort_tcp */
            void *pStreamPort;
            /* 数据来源地址,业务上的变量,不一定是真实值,此值会回调到应用层标示数据的来源ip地址 */
            unsigned int ipv4Src;
            /* 流的检查编号,仅当值为0时才进行流检测
             * 在未确定关联sip时,如关联出错,则修改此值为待处理的关联数
             * 在拉流时,如因数据丢包过多,则修改为待拉流的总数
             */
            unsigned int streamCheckIndex;
            /* 上一次接收数据的时间,没有拉流时表示上一次尝试拉流的时间,如因端口添加时出错,设置为0 */
#define GB28181Client_item_streamTimeOut 8.0 // 没有收到流的超时时间 
            double lastRecvTime;
            /* 当前用于接收数据的Buffer,可为空 */
            FsObjectBase *pObjectBase_recv;
            /* 关闭视频流的信息,只记录到Call-ID,不包含最后的\r\n,前4字节表示长度,为空代表无信息,不为空时sipStatus肯定大于1 */
            char *byeInfo;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步 */
            int pipe_w;
#define GB28181Client_item_record_inviteTime // 定义是否记录发送invite的时间,仅用于调试,不参与业务
#ifdef GB28181Client_item_record_inviteTime  
            /* 接收rtp数据的返回状态,在发送invite时置0,其中-103表示丢包数据过多 */
            int recvResult;
            /* 最后一次发送invite的时间 */
            double lastSendInviteTime;
            /* 上上一次发送invite的时间 */
            double lastLastSendInviteTime;
#endif
            // 在isForSip不为0时后续变量为外部可自定义的空间,内部不使用(内部储存为 unsigned int data[5] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType})
#define GB28181Client_item_get_forSip_custom_memory(___pGB28181Client_item) (((char*)___pGB28181Client_item)+Memery_AlignmentI(FsPointer_to_integer(&(( struct GB28181Client_item *)0)->p.psTime),8))
            /* 分配的代理ID号,不能为0,1-表示未分配 */
#define GB28181Client_item_forSip_proxyID(___pGB28181Client_item) ((unsigned int*)GB28181Client_item_get_forSip_custom_memory(___pGB28181Client_item))[5]
            /* 收数据时从ps中解析到的时间(小于0.0无效) */
            double psTime;
            /* 把rtp的变化时间累加 */
            double rtpTime;
            /* 最近两次的rtp时间,因为回调时是回调前面一个包的数据,把当前包的值计入会导致结果偏大 */
            unsigned int rtpTimestamp[2];
            /* 同步号 */
            unsigned int ssrc;
            /* 帧号,仅使用后三字节 */
            unsigned int frameIndex_24bits;
            /* 剩下的还要接收的长度,指PS数据的长度 */
            unsigned int recvLenRemain;
            /* 上一次接收后未校验的数据长度 */
            unsigned int notCheckLen;
            //#define GB28181Client_item_enable_skipLen // 暂时启用此项无用
#ifdef GB28181Client_item_enable_skipLen
            /* 忽略的数据长度,在收到pes后可能因pes数据是不需要的数据,此处标记为忽略的长度,在数据收完后把数据忽略掉 */
            unsigned short skipLen;
#endif
            /* 上一个rtp包的序号 */
            unsigned char sequenceNumber;
            /* 是否已收到第一个包,收到后要进行丢包校验,0-未收到,1-已收到 */
            unsigned char firstPackage : 1;
            /* 是否丢失数据,(1:丢失,0:未丢失),开始收流时标记为1,让其收到BA后标记为0 */
            unsigned char lostData : 1;
            /* 当前的数据是否是关键帧,1-是,0-不是,关键帧时一般使用元数据内存 */
            unsigned char isKey : 1;
            /* 当前数据类型,1-h264,2-h265,3-aac */
            unsigned char dataType : 2;
            /* 承载协议,0-tcp被动模式,1-udp(当为udp时使用tcp接收也是可以的),2-kcp(当为kcp时使用udp或tcp传输也是可以的),3-tcp主动模式 */
#define GB28181Client_item_streamOver_next(___GB28181Client_item) do{\
    if(0==(___GB28181Client_item)->p.streamOver){\
        if((___GB28181Client_item)->ro._streamOver & 0x4) (___GB28181Client_item)->p.streamOver=2;\
        else if((___GB28181Client_item)->ro._streamOver & 0x2) (___GB28181Client_item)->p.streamOver=1;\
        else if((___GB28181Client_item)->ro._streamOver & 0x8) (___GB28181Client_item)->p.streamOver=3;\
    }else if(1==(___GB28181Client_item)->p.streamOver){\
        if((___GB28181Client_item)->ro._streamOver & 0x8) (___GB28181Client_item)->p.streamOver=3;\
        else if((___GB28181Client_item)->ro._streamOver & 0x1) (___GB28181Client_item)->p.streamOver=0;\
        else if((___GB28181Client_item)->ro._streamOver & 0x4) (___GB28181Client_item)->p.streamOver=2;\
    }else if(2==(___GB28181Client_item)->p.streamOver){\
        if((___GB28181Client_item)->ro._streamOver & 0x8) (___GB28181Client_item)->p.streamOver=3;\
        else if((___GB28181Client_item)->ro._streamOver & 0x1) (___GB28181Client_item)->p.streamOver=0;\
        else if(((___GB28181Client_item)->ro._streamOver & 0x6)==0x2) (___GB28181Client_item)->p.streamOver=1;\
    }else{\
        if((___GB28181Client_item)->ro._streamOver & 0x1) (___GB28181Client_item)->p.streamOver=0;\
        else if((___GB28181Client_item)->ro._streamOver & 0x4) (___GB28181Client_item)->p.streamOver=2;\
        else if((___GB28181Client_item)->ro._streamOver & 0x2) (___GB28181Client_item)->p.streamOver=1;\
    }\
}while(0)
            unsigned char streamOver : 2;

            /* 在streamStatus为3时使用的变量 */
            union {
                /* 使用udp或kcp时分配此数据 */
                FsKcp kcp;
            } streamStatus_3;

        } p;
    };
#define __GB28181Client_itemLog(___Log,___loglevel,___format, ...) do{\
    unsigned int ipv4_l0=0, ipv4_r0=0,ipv4_l1=0, ipv4_r1=0;\
    unsigned short ipv4Port_l0=0, ipv4Port_r0=0,ipv4Port_l1=0, ipv4Port_r1=0;\
    if(pGB28181Client_item->p.streamStatus>1){\
        fs_Socket_addr_get(&ipv4_l0, &ipv4Port_l0, &ipv4_r0, &ipv4Port_r0, pGB28181Client_item->p._sock[0]);\
        if(pGB28181Client_item->p._sock[1]>-1)fs_Socket_addr_get(&ipv4_l1, &ipv4Port_l1, &ipv4_r1, &ipv4Port_r1, pGB28181Client_item->p._sock[1]);\
    }\
    ___Log(___loglevel,5,___format",p=%p,sipServer:\"%hhu.%hhu.%hhu.%hhu\",isForSip=%hhu,sipOver=%hhx,streamOver=%hhx/%hhu,control=%hhu,controlMaskDefault=%#llx,status=%hhx/%hhx,sock=%d/%d,server:\"%hhu.%hhu.%hhu.%hhu:%hu to %hhu.%hhu.%hhu.%hhu:%hu\"/\"%hhu.%hhu.%hhu.%hhu:%hu to %hhu.%hhu.%hhu.%hhu:%hu\"\
,sipServer:\"%hhu.%hhu.%hhu.%hhu:%hu\"\
,id:\"%s\",sip_id:\"%s\",sip_domain:\"%s\",sip_username:\"%s\",sip_password:\"%s\",url:\"%s\",pAddrMap=%p,playbackTime=%lf,playSpeed=%f,recordInfo=%p,packetLossCheck=%u\
,playBackQueryRecord=%hhu,isPause=%hhu,inviteReturnRecv=%hhu,activeConnect=%hhu,udpSock1AddToEpoll=%hhu,port_l=%hu,port_ri=%hu\
,frameIndex_24bits=%u,recvLenRemain=%u,notCheckLen=%u\
,pStreamPort=%p,psTime=%lf,rtpTime=%lf,rtpTimestamp=%u/%u,lostData=%hhu.\n",##__VA_ARGS__\
        ,pGB28181Client_item,Fs_Network_Ipv4_print(pGB28181Client_item->ro._sipServer),pGB28181Client_item->ro._isForSip,pGB28181Client_item->ro._sipOver,pGB28181Client_item->ro._streamOver,pGB28181Client_item->p.streamOver\
        ,pGB28181Client_item->ro._control,pGB28181Client_item->ro._controlMaskDefault,pGB28181Client_item->p.sipStatus,pGB28181Client_item->p.streamStatus\
        ,pGB28181Client_item->p._sock[0],pGB28181Client_item->p._sock[1]\
        ,Fs_Network_Ipv4_print(ipv4_l0),ipv4Port_l0, Fs_Network_Ipv4_print(ipv4_r0),ipv4Port_r0,Fs_Network_Ipv4_print(ipv4_l1),ipv4Port_l1, Fs_Network_Ipv4_print(ipv4_r1),ipv4Port_r1\
        ,Fs_Network_Ipv4_print(pGB28181Client_item->ro._sip_ipv4),pGB28181Client_item->ro._sip_ipv4Port\
        ,GB28181Client_item_get_id(pGB28181Client_item),GB28181Client_item_get_sip_id(pGB28181Client_item),GB28181Client_item_get_sip_domain(pGB28181Client_item,"")\
        ,GB28181Client_item_get_sip_username(pGB28181Client_item,""), GB28181Client_item_get_sip_password(pGB28181Client_item,""),GB28181Client_item_get_url(pGB28181Client_item)\
        ,pGB28181Client_item->ro.__pAddrMap,pGB28181Client_item->p._playbackTime,pGB28181Client_item->p._playSpeed,pGB28181Client_item->p._recordInfo,pGB28181Client_item->p.packetLossCheck\
        ,pGB28181Client_item->p._playBackQueryRecord,pGB28181Client_item->p.isPause,pGB28181Client_item->p.inviteReturnRecv,pGB28181Client_item->p.activeConnect,pGB28181Client_item->p.udpSock1AddToEpoll,pGB28181Client_item->p.port_l\
        ,pGB28181Client_item->p.port_ri,pGB28181Client_item->p.frameIndex_24bits,pGB28181Client_item->p.recvLenRemain,pGB28181Client_item->p.notCheckLen\
        ,pGB28181Client_item->p.pStreamPort,pGB28181Client_item->p.psTime,pGB28181Client_item->p.rtpTime,pGB28181Client_item->p.rtpTimestamp[0],pGB28181Client_item->p.rtpTimestamp[1],pGB28181Client_item->p.lostData);\
}while(0)
#define GB28181Client_itemLog(___loglevel,___format, ...) __GB28181Client_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define GB28181Client_itemLog2(___loglevel,___format, ...) __GB28181Client_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* GB28181服务器条目,根据URL来划分 */
    struct GB28181Server_item {
#define GB28181Server_item_base0(___pGB28181Server_item) (((char*)((___pGB28181Server_item)+1))+(((___pGB28181Server_item)->r._modelType & 0x1) ? sizeof (unsigned short)*32 + sizeof (unsigned int)*32 : 0))
#define GB28181Server_item_base1(___pGB28181Server_item) (((char*)(___pGB28181Server_item))+ GB28181Server_item_ClusterLen)
#define GB28181Server_item_base(___pGB28181Server_item) (0==(___pGB28181Server_item)->r._ipv4?GB28181Server_item_base0(___pGB28181Server_item):GB28181Server_item_base1(___pGB28181Server_item))

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
            /* 标识符,不能为空 */
#define GB28181Server_item_get_url(___pGB28181Server_item) (GB28181Server_item_base(___pGB28181Server_item)+(___pGB28181Server_item)->r._url_)

            unsigned char _url_;
            /* 父组织,为空使用sip信息,集群时不赋值 */
#define GB28181Server_item_get_parent(___pGB28181Server_item,___nullValue) (0==(___pGB28181Server_item)->r._ipv4?(FsNumberIsMax((___pGB28181Server_item)->r._parent_)?(___nullValue):(GB28181Server_item_base0(___pGB28181Server_item)+(pGB28181Server_item)->r._parent_)):(___nullValue))
            unsigned char _parent_;
            /* 通道名字,不能为空,集群时不赋值 */
#define GB28181Server_item_get_name(___pGB28181Server_item) (0==(___pGB28181Server_item)->r._ipv4?(GB28181Server_item_base0(___pGB28181Server_item)+(___pGB28181Server_item)->r._name_):"")
            unsigned char _name_;
            /* 实时视频是否在线 */
            unsigned char _realOnline;
            /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */
            unsigned char _modelType;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
#define GB28181Server_item_wait(___pGB28181Server_item) do{while((___pGB28181Server_item)->r.busy)FsWaitSyncDo();}while(0)
            unsigned char busy : 1;
            /* 控制状态,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char controlStatus : 2;
            /* 扩展掩码,1-海康时间轴扩展 */
            unsigned char _externMask;
            /* 是否有新的实时客户端,1-有,0-没有 */
            unsigned char newRealClient;
            /* 是否有新的回放客户端,1-有,0-没有 */
            unsigned char newDemandClient;
            /* 主机ip,用于集群,0表示本机,低四位表示ip */
            unsigned int _ipv4;
            /* 控制掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */
            unsigned long long _controlMask;
            /* 经度,东经为正 */
            float _longitude;
            /* 纬度,向北为正,范围[-90,90] */
            float _latitude;
        } r;

        struct {
#define GB28181Server_item_ClusterLen (FsPointer_to_integer(&((struct GB28181Server_item*) 0)->ro.__realVideoList)+sizeof(void*)*3)
#define GB28181Server_item_Cluster_callTimes(___pGB28181Server_item) (*(unsigned int*)((&(___pGB28181Server_item)->ro.__realVideoList)+0))
#define GB28181Server_item_Cluster_next(___pGB28181Server_item) (*(struct GB28181Server_item**)((&(___pGB28181Server_item)->ro.__realVideoList)+1))
#define GB28181Server_item_Cluster_pGB28181Server(___pGB28181Server_item) (*(GB28181**)((&(___pGB28181Server_item)->ro.__realVideoList)+2))
#define GB28181Server_item_next(___pGB28181Server_item) (0==(___pGB28181Server_item)->r._ipv4?(___pGB28181Server_item)->p.next:GB28181Server_item_Cluster_next(___pGB28181Server_item))
#define GB28181Server_item_pGB28181Server(___pGB28181Server_item) (0==(___pGB28181Server_item)->r._ipv4?(___pGB28181Server_item)->p.pGB28181:GB28181Server_item_Cluster_pGB28181Server(___pGB28181Server_item))

            /* 实时视频数据链表,为空表示不支持实时视频 */
            FsObjectList *__realVideoList;
            /* 连接的实时客户端链表,只有当实时视频数据链表不为空时才有效,客户端与sip绑定,无法脱离线程后在多线程间切换 */
            FsCycleList __realClientList;
#define  GB28181_mutex_realClientList(___pGB28181) (___pGB28181)->ro.__mutex_realClientList;
            /* 实时客户端的互斥锁,只有当实时视频数据链表不为空时才有效 */
            pthread_mutex_t __mutex_realClientList;
            /* 本地地址映射,出拉流时使用的地址 */
            FsRouter * __pAddrMap;
            /* 有客户端时的回掉函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*_cb_client)(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p);
            /* 有客户端时的回掉函数关联的指针 */
            void *_cb_clientP;
            /* 查询录像时长的回调函数,成功返回FsObjectList指针(成员为两个double,第一个double为开始时间,第二个double为结束时间),失败返回NULL */
            FsStructList * (*_cb_client_recordinfo__IO) (/* 查找的开始时间 */double startTime, /* 查找的结束时间 */double endTime, void * p
                    , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 查询录像时长回掉函数关联的指针 */
            void *_cb_client_recordinfoP;
            /* 实时视频数据的索引,只有当实时视频数据链表不为空时才有效 */
            unsigned short *_realVideoIndex;
            /* 发送数据时rtp里面的timestamp值,只有当实时视频数据链表不为空时才有效 */
            unsigned int *_timestamp;
#define GB28181Server_item_get_ssrc(___pGB28181Server_item) ((unsigned int)FsPointer_to_integer((___pGB28181Server_item))&0xFFFFFFFU)
        } ro;

        struct {
            /* 点播客户端链表,实时视频数据链表不为空时,此链表储存历史点播的客户端,否则储存文件点播客户端,也可能不支持点播 */
            FsCycleList __demandClientList;
            /* 历史客户端的互斥锁,仅当有历史数据功能时有效 */
            pthread_mutex_t __mutex_demandClientList;
            /* 掩码,支持的所有组合 */
            unsigned int allMask;
        } rw;

        struct {
            /* 具有相同url的下一个GB28181Server通道 */
            struct GB28181Server_item *next;
            /* 所属的GB28181对象,不能根据其是否为空来判断是否加入或移除线程 */
            GB28181 *pGB28181;
            /* 控制函数指针 */
            struct Camera_ctrl_functionUser *ctrl_function;
            /* 控制对像指针 */
            void *ctrl;
            /* 内部控制的函数指针,可用于集群,通知收到的相机控制指令,连接其他本系统的平台进行相机控制 */
            struct Camera_ctrl_item *pCamera_ctrl_item;
            /* 缓存上一次控制的水平值,在收到停止时置0 */
            short hrotation;
            /* 缓存上一次控制的值,在收到停止时置0 */
            short vrotation;
            /* 统计实时流中的帧率,结果应更接近低频通道 */
            float _realRate_low;
            /* 用于统计realRate_low的辅助变量 */
            float realRate_low_last;
            /* 上一次发送实时数据的时间 */
            double realLastSendTime;
            /* 发送实时数据的时间间隔 */
            float realSendDataInterval;
            /* 上一次实时视频的数量 */
            unsigned int realVideoListCountLast;
            /* 实时数据的掩码 */
            unsigned int mask_realAll;
            /* 实时数据中不是所有帧都为关键帧的掩码 */
            unsigned int mask_realkey;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程 */
            int pipe_w;
            /* 是否有udp客户端,0-没有,1-有 */
            unsigned char hasUdpClient : 1;
        } p;
    };
#define __GB28181Server_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,url:\"%s\",name:\"%s\",ipv4:\"%hhu.%hhu.%hhu.%hhu\",modelType=%hhu.\n",##__VA_ARGS__\
        ,pGB28181Server_item,GB28181Server_item_get_url(pGB28181Server_item),GB28181Server_item_get_name(pGB28181Server_item)\
        ,Fs_Network_Ipv4_print(pGB28181Server_item->r._ipv4),pGB28181Server_item->r._modelType)
#define GB28181Server_itemLog(___loglevel,___format, ...) __GB28181Server_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define GB28181Server_itemLog2(___loglevel,___format, ...) __GB28181Server_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 点播的客户端链表结构 */
    struct GB28181Server_item_DemandClient {
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

        /* rw放前面,以便RTSP,gb28181的回放共用 */
        struct {
#ifndef __DemandClient_rw 
            unsigned int allMask;
            /* 外部标记是否出错,0-表示正常,1-表示已结束,-1表示出错 */
            signed char error;
            /* 数据发送类型(0--正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送,其他值-无效) */
            unsigned char sendType;
            /* 内部的发送缓存值 */
            unsigned char sendBufferCountMaxIn;
            /* 外部的发送缓存值 */
            unsigned char sendBufferCountMaxOut;
            /* 外部指针 */
            void *externP;
            /* 删除externP的函数指针,为空表示不删除 */
            void (*deleteExternP) (void *externP);
            /* 时长,为0表示未知,用clock格式一般用于按时点播类应用,不为0为文件点播用npt格式 */
            double duration;
            /* 跳转到的时间,为npt格式时表示距离文件开始的时间,为clock时表示1970年1月1号以来经过的秒数,在此值不为0时,调用者应改变视频流的的位置并把此值置0 */
            double jumpTime;
#endif
        } rw;

        struct {
            /* 数据帧链表 */
            FsObjectList *__dataList;
            /* 数据的索引 */
            unsigned short index[32];
            /* 发送数据时rtp里面的timestamp值 */
            unsigned int timestamp[32];
            // 播放录像时间段
            unsigned int starttime;
            unsigned int endtime;
        } ro;

        struct {
            /* 播放速度,为非负数,快进/慢进，取值1/8,1/4,1/2,1,2,4,8 */
            float _scale;
            /* 统计流中的帧率,结果应更接近低频通道 */
            float _rate_low;
            /* 用于统计rate_low的辅助变量 */
            float rate_low_last;
            /* 历史视频速率控制的参考时间,开机时间,偏差超过3S时重新计时 */
            double historyTime_uptime;
            /* 历史视频速率控制的参考时间,gmt时间,偏差超过3S时重新计时 */
            double historyTime_gmttime;
        } p;
    };

    /* 
     * 创建一个新的GB28181对象;
     * 返回GB28181指针.
     */

    GB28181 *gb28181_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port
            , /* 流端口,多个使用逗号隔开,如5061,5062,5063,5064或5061,5062-5063,5064或5061-5064 */const char streamPort[]
            , /* 每个线程最多处理的通道数 */ const unsigned int channelCountPerThread, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut, /* 配置管理对象,为空表示不支持与之相关的业务 */ void *const pConfigManager
            , /* 监听分类服务器对象,可为空 */ void *const pBindClassifyServer, /* 本地地址映射,拉流时使用的地址,用于初始化forSip的GB28181Client_item,可为空 */ FsRouter * const pAddrMap
            , /* 获取通道状态信息,为空表示不支持,校验和发生改变时返回FsEbml指针,否则返回NULL,返回NULL可能是出错 */
            FsEbml * (*const channelStatusGet__IO) (/* 调用时缓存的通道最后改变的编号,在发生改变时才返回数据,不为空 */unsigned long long *pChannelStatusIndex, void * channelStatusP, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 获取通道状态信息相关的指针 */ void *const channelStatusP
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 设置pGB28181的参数(需要线程同步的部分参数) */

    void gb28181_set_parameter_thread_pthreadSafety(GB28181 * const pGB28181, /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port
            , /* 流端口,多个使用逗号隔开,如5061,5062,5063,5064或5061,5062-5063,5064或5061-5064 */const char streamPort[]
            , /* 目录查询,启用后会查询并缓存目录查询结果,0-关闭,1-仅生成连接信息,3-启用 */const unsigned char catalog
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 设置pGB28181的参数(不需要线程同步的部分参数) */

    void gb28181_set_parameter_pthreadSafety(GB28181 * const pGB28181, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut);

    /* 删除pGB28181指向的实例对象 */

    void gb28181_delete__OI(GB28181 * const pGB28181);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void gb28181_startThread(GB28181 * const pGB28181
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void gb28181_stopThread_noWait(GB28181 * const pGB28181);

    /* 结束线程 */

    void gb28181_stopThread(GB28181 * const pGB28181);

    /* 通知pGB28181的通道状态变化了 */

    void gb28181_update_channelStauts_pthreadSafety(GB28181 * const pGB28181);

    /* 添加gb28181的组信息 */

    void gb28181_group_add_pthreadSafety(GB28181 * const pGB28181, /* 组id号,如65010200002160000001或65010200002150000001,其中215类型代表业务分组,216类型代表虚拟组织 */const char groupID[],
            /* 组织名,为空使用groupID */const char groupName[], /* 父组织,为空表示无父组织 */const char groupParent[]);

    /* 清空gb28181的组信息 */

    void gb28181_group_clean_pthreadSafety(GB28181 * const pGB28181);

    /* 打印组织信息 */

    void gb28181_group_out(GB28181 * const pGB28181, FILE * const fd);

    /* 
     * 创建一个新的GB28181Sip_item对象;
     * 返回GB28181Sip_item指针.
     */

    struct GB28181Sip_item * gb28181Sip_item_new__IO(/* 代理服务器地址,0表示不使用代理 */ const unsigned int proxyServer, /* 本地id,不能为空 */ const char id[], /* SIP服务器ID,不能为空 */ const char sip_id[]
            , /* SIP服务器域,可为空,不做校验 */const char sip_domain[], /* 本机使用的地址信息,一般在NAT环境中使用,为空时程序自动计算 */ const char addr_info[]
            , /* SIP用户认证ID,可为空 */ const char sip_username[], /* SIP用户认证密码,可为空 */ const char sip_password[]
            , /* SIP服务器地址,为0表示只接受被注册 */ const unsigned int sip_ipv4, /* SIP服务器端口 */ const unsigned short sip_ipv4Port
            /* 自定义配置,根据对方sip的特性开启特殊处理
             * (1 << 0):目录订阅回执中callid增加错误信息
             * (1 << 1):注册即推送目录订阅信息
             * (1 << 2):不进行目录查询
             */
            , const unsigned char customConfig, /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */ const unsigned char sipOver
            , /* 本地地址映射,主动发起命令时填写的本地地址,可为空 */ FsRouter * const pAddrMap);

    /* 删除pGB28181Sip_item指向的实例对象 */

    void gb28181Sip_item_delete__OI(struct GB28181Sip_item * const pGB28181Sip_item);

    /* 向pGB28181中添加一个GB28181 sip条目 */

    void gb28181Sip_add_item__OI_2(GB28181 * const pGB28181, struct GB28181Sip_item * const pGB28181Sip_item);

    /* 从pGB28181中移除一个GB28181 sip条目,未等待线程真正移除 */

    void gb28181Sip_remove_item_noWait__IO_2(GB28181 * const pGB28181, struct GB28181Sip_item * const pGB28181Sip_item);

    /* 从pGB28181中移除一个GB28181 sip条目 */

    void gb28181Sip_remove_item__IO_2(GB28181 * const pGB28181, struct GB28181Sip_item * const pGB28181Sip_item);

    /* 
     * 创建一个新的GB28181Client_item对象;
     * 返回GB28181Client_item指针.
     */


    typedef void (*GB28181Client_item_cb_forSip)(struct GB28181Client_item * pGB28181Client_item, /* 连接变化,-1连接断开,0-连接未变化,1-连接成功 */signed char connectChange
            , /* 回调的数据,在connectChange为0时对会出现,总长度为headLen+4+dataLen */ char data_cin[], /* 回调数据中头部长度 */unsigned int headLen, /* 回调数据中数据区部分长度 */unsigned int dataLen, /* 外部传入的指针 */ void * p
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    struct GB28181Client_item * gb28181Client_item_new__IO(/* sip服务器地址,为0表示内部自建sip服务器(在回放独立运行时需要设置此值) */const unsigned int sipServer
            , /* 此客户端是否用于发送和接收sip信息 */ const unsigned char isForSip
            , /* 本地id,不能为空 */ const char id[], /* SIP服务器ID,不能为空 */ const char sip_id[]
            , /* SIP服务器域,可为空,不做校验 */const char sip_domain[], /* SIP用户认证ID,可为空 */ const char sip_username[], /* SIP用户认证密码,可为空 */ const char sip_password[]
            , /* 标识符,不能为空 */ const char url[], /* 当前的回放时间,为0.0表示直播 */ const double playebackTime, /* 本地地址映射,拉流时使用的地址,可为空 */ FsRouter * const pAddrMap
            , /* SIP服务器地址,为0表示只接受被注册 */ const unsigned int sip_ipv4, /* SIP服务器端口 */ const unsigned short sip_ipv4Port, /* sip对外连接使用的协议,&0x1表示支持udp,&0x2表示支持tcp,仅作用于主动对外连接 */ const unsigned char sipOver
            , /* 视频帧传出时前面预留大小(isForSip为0时有效) */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小(isForSip为0时有效) */ const unsigned int videoSuffix
            , /* 音频帧传出时前面预留大小(isForSip为0时有效) */ const unsigned int audioPrefix
            , /* 流的承载协议(isForSip为0时有效),&0x1-tcp被动模式,&0x2-udp,&0x4-kcp,&0x1-tcp主动模式 */const unsigned char streamOver

            , /* 取流状态,0-停止取流,1-开始取流(isForSip为0时有效) */const unsigned char streamControl
            , /* 是否启用控制(isForSip为0时且为直播时有效),0-启用,1-不启用 */const unsigned char control, /* 默认的控制类型,0-表示自动探测(尽可可开启更多的控制类型),0x1表示探测结果最多只能包含任意转,更多定义参看Camera_ctrl_Mask开头的宏 */ const unsigned long long controlMaskDefault
            , /* 回放时是否需要进行录像查询,0-不需要,1-仅发送,2-需要等待回执,3-等到回执后发送二次查询,4-需要等待2次回执,以此类推 */ const unsigned char playBackQueryRecord
            , /* 在isForSip不为0时,表示收到的sip配对信息 */
            /* 收到数据后的回调函数(isForSip为0时此处表示收到视频数据,不为0时表示收到的sip配对信息),返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */ int (*const cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
            , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */double realTime, /* 绝对时间,可能不是每帧都变化(小于0.0无效)(如数据时从ps中解析到的时间) */ double absoluteTime, /* 相对时间(如rtp时间),在realTime无效时,使用absoluteTime+此参数的变化间接得到realTime */ double relativeTime
            , /* 帧号,最大值只能到3个字节 */const unsigned int frameIndex
            , /* 数据类型,1-h264,2-h265 */unsigned char type, /* 是否是关键帧(-1:无法判断;0;不是;1:是) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
            , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
            , /* 数据的来源ip */unsigned int ip, /* 外部传入的指针 */ void * p)
            , /* 外部传入的指针 */void *const externP, /* 本相机对应的录像信息,在不为空时,内部需要检查其录像信息 */ struct GB28181Client_item_recordInfo * const recordInfo);


    /* 删除pGB28181Client_item指向的实例对象 */

    void gb28181Client_item_delete__OI(struct GB28181Client_item * const pGB28181Client_item);

    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */

    void gb28181Client_item_streamControl_pthreadSafety(struct GB28181Client_item * const pGB28181Client_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);

    /* 向pGB28181中添加一个GB28181客户端条目 */

    void gb28181Client_add_item__OI_2(GB28181 * const pGB28181, struct GB28181Client_item * const pGB28181Client_item);

    /* 从pGB28181中移除一个GB28181客户端条目,未等待线程真正移除 */

    void gb28181Client_remove_item_noWait__IO_2(GB28181 * const pGB28181, struct GB28181Client_item * const pGB28181Client_item);

    /* 从pGB28181中移除一个GB28181客户端条目 */

    void gb28181Client_remove_item__IO_2(GB28181 * const pGB28181, struct GB28181Client_item * const pGB28181Client_item);

    /* 向pGB28181Client_item发送ptz命令,只是加到发送链表,命令是否生效只能在业务层来判断,成功返回1,失败返回-1 */

    int gb28181Client_item_control_ptz(struct GB28181Client_item * const pGB28181Client_item, /* 参考值精度为23040,负数向左,正数向右 */const short speedx
            , /* 参考值精度为23040,负数向上,正数向下 */const short speedy, /* 参考值精度为23040,负数缩小,正数放大 */const short speedz);

    /* 检查pGB28181Client_item能否被控制,可控返回1,不可控返回-1 */

    int gb28181Client_item_control_check(struct GB28181Client_item * const pGB28181Client_item);

    /* 在pGB28181Client_item为回放连接时,可调用此函数修改其播放速度 */

    void gb28181Client_item_play(struct GB28181Client_item * const pGB28181Client_item, /* 播放速度,0.0表示暂停 */ const float speed);

    /* 
     * 创建一个新的GB28181Server_item对象;
     * 返回GB28181Server_item指针.
     */

    struct GB28181Server_item * gb28181Server_item_new__IO(/* 标识符,不能为空 */ const char url[], /* 通道名字,为空表示使用与url相同的值,集群时不赋值 */const char name[]
            , /* 父组织,为空使用sip信息 */ const char parent[], /* 主机ip,为0表示本地主机,非0表示远程主机,之后的数据仅对此值为0时有效 */const unsigned int ipv4
            , /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */const char modelType
            , /* 控制掩码,按位排布,0-任意转,1-左右转,2-上下转,3-45度斜角转,4-变倍,5-聚焦,6-预置位,7-3d定位 */ const unsigned long long controlMask
            , /* 扩展掩码,1-海康时间轴扩展 */ const unsigned char externMask
            , /* 经度,东经为正 */ const float longitude, /* 纬度,向北为正,范围[-90,90] */ const float latitude
            , /* 本地地址映射,出流时使用的地址,本地主机有效,可为空 */ FsRouter * const pAddrMap
            , /* 有客户端时的回掉函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*const cb_client) (/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p)
            , /* 有客户端时的回掉函数关联的指针 */void *const cb_clientP
            , /* 查询录像时长的回调函数,成功返回FsObjectList指针(成员为两个double,第一个double为开始时间,第二个double为结束时间),失败返回NULL */
            FsStructList * (*const cb_client_recordinfo__IO) (/* 查找的开始时间 */double startTime, /* 查找的结束时间 */double endTime, void * p
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 查询录像时长回调函数关联的指针 */ void * cb_client_recordinfoP);

    /* 删除pGB28181Server_item指向的实例对象 */

    void gb28181Server_item_delete__OI(struct GB28181Server_item * const pGB28181Server_item);

    /* 向pGB28181中添加一个GB28181服务器条目 */

    void gb28181Server_add_item__OI_2(GB28181 * const pGB28181, struct GB28181Server_item * const pGB28181Server_item);

    /* 从pGB28181中移除一个GB28181服务器条目,未等待线程真正移除 */

    void gb28181Server_remove_item_noWait__IO_2(GB28181 * const pGB28181, struct GB28181Server_item * const pGB28181Server_item);

    /* 从pGB28181中移除一个GB28181服务器条目 */

    void gb28181Server_remove_item__IO_2(GB28181 * const pGB28181, struct GB28181Server_item * const pGB28181Server_item);

    /* 添加实时数据到pGB28181Server_item中 */

    void gb28181Server_item_add_frame(struct GB28181Server_item * const pGB28181Server_item, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);

    /* 添加实时数据到pGB28181Server_item中 */

    void gb28181Server_item_add_frame__OI_2(struct GB28181Server_item * const pGB28181Server_item, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);

    /* 添加点播数据到pDemandClient中 */

    void gb28181Server_item_DemandClient_add_frame__OI_2(struct GB28181Server_item_DemandClient * const pDemandClient, void *const pObject);


    unsigned int gb28181Server_item_ClusterLen_get();

    /* 设置控制 */

    void gb28181Server_item_set_ctrl(struct GB28181Server_item * const pGB28181Server_item, /* 控制函数指针 */ struct Camera_ctrl_functionUser * const ctrl_function
            , /* 控制对像指针 */ void *const ctrl, /* 内部控制的函数指针,可用于集群,通知收到的相机控制指令,连接其他本系统的平台进行相机控制 */ struct Camera_ctrl_item * const pCamera_ctrl_item);



#ifdef __cplusplus
}
#endif

#endif /* HLS_H */

