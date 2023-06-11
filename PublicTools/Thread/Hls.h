/* 
 * File:   Hls.h
 * Author: fslib
 *
 * Created on 2016年10月27日, 上午11:19
 */

#ifndef HLS_H
#include "../../PublicTools/publicDefine.h"
#define HLS_H
//#define Hls_Debug
//#define HlsServerDebug
//#define Hls_default_etag "0"
#define Hls_Access_Control_Allow_Credentials_enable
#define Hls_EXT_X_PROGRAM_DATE_TIME_enable
#define HlsServer_pts_diff 0
#define HlsServer_realFps 1.02
///* 一个连接最多收发数据的个数,不实现 */
//#define HlsConnect_doDataCount 5
#include <pthread.h>
#include <sys/epoll.h>

#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/CycleList.h"
#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/StringBuilder.h"
#include "../../PublicTools/Image/ImageFrame.h"
#ifdef __cplusplus
extern "C" {
#endif
#define HlsServer_DemandClient_get(___pClient) (((struct HlsServer_item_DemandClient**) (___pClient))[0])

    /* HLS服务器 */
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
            /* 管道,用于添加和删除文件或发送数据 */
            int __pipe[2];
            /* 线程名 */
#define Hls_get_threadName(___pFileStream) (((char*)(((Hls*)(___pFileStream))+1))+((Hls*)(___pFileStream))->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* 主页面数据,可为空 */
            void *__htmlData;
            /* 获取配置信息,为空表示不支持,校验和发生改变时返回FsObjectBase指针,否则返回NULL,返回NULL可能是出错 */
            FsObjectBase * (*_configGet__IO)(/* 调用时缓存的配置校验和,为空强制返回配置,不为空时会把最新的校验信息写入返回,在校验和发生改变时才返回配置数据 */unsigned int checkSum_4[],
                    void * const p, /* 类型 */ FsConfig_ExportType type, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 
             * 设置配置信息,为空表示不支持;
             * 成功返回1;
             * 失败返回-1;
             * 繁忙返回-120;
             */
            int (*_configSet__OI)(FsEbml * const pEbml, /* 设置类型,0-上传配置,1-合并配置 */const unsigned char type, void *const p
                    , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 配置相关的指针 */
            void *_configP;
            /* 获取通道状态信息,为空表示不支持,校验和发生改变时返回FsString指针,否则返回NULL,返回NULL可能是出错 */
            FsObjectBasePthreadSafety * (*_channelStatusGet__IO) (/* 调用时缓存的通道最后改变的编号,在发生改变时才返回数据,不为空 */unsigned long long *pChannelStatusIndex,
                    void * const p, /* 类型 1-ebml,2-xml,3-json */const unsigned char type, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 获取通道状态信息相关的指针 */
            void *_channelStatusP;
            /* hls本地服务器条目链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__itemListServer;
            /* 集群的通道链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__clusterItemListServer;
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
            /* 相机状态是否已准备好,1-表示已准备好,0-表示需要获取并更新 */
            unsigned char channelStatusReady : 1;
            /* hls线程一次等待多少个16毫秒 */
            unsigned char hlsSleep16;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* 监听有ipv4地址,为0表示监听所有地址 */
            unsigned int _ipv4;
            /* 实时视频缓存的超时时间 */
            float _realBufferTimeOut;
            /* 外部和内部线程异步通信的链表,结构为Hls_P_control */
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
            /* 当前服务端的工作线程数 */
            unsigned short workThreadServer;
            /* 线程的tid号 */
            pthread_t stid;
            /* 新加入的服务通道条目链表 */
            FsObjectList *newItemListServer;
            /* 本地的服务端通道链表,按指针排序 */
            FsObjectList *localItemList;
            //                     /* 本地的服务端通道数量 */
            //            unsigned long localItemListCount;
            /* 线程异步通信的链表,结构为Hls_P_control */
            FsStructList *controlListServer;
            /* 服务端端线程对象链表 */
            FsObjectList *threadListServer;
            /* 缓存未使用的客户端链表,指连接,使用newItemListServer的互斥锁 */
            FsCycleList cycleListClient_noUse;
            /* 缓存需要hls线程再处理的连接,指连接,使用newItemListServer的互斥锁 */
            FsCycleList cycleListClient_hlsDo;
        } p;
    } Hls;
#define __HlsLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  channelCountPerThread=%u\n  itemListServer=%p\n  clusterItemListServer=%p\n  ipv4:\"%hhu.%hhu.%hhu.%hhu:%hu\"\n  realBufferTimeOut=%f\n",##__VA_ARGS__\
        ,pHls,((Hls*)(pHls))->rw.runStatus,((Hls*)(pHls))->p.stid\
        ,((Hls*)(pHls))->ro._pMonitor,((Hls*)(pHls))->ro._externRunStatus,((Hls*)(pHls))->ro._parentTid, ((Hls*)(pHls))->ro.__pipe[0], ((Hls*)(pHls))->ro.__pipe[1],Hls_get_threadName(pHls)\
        ,((Hls*)(pHls))->ro._channelCountPerThread\
        ,((Hls*)(pHls))->ro.__itemListServer, ((Hls*)(pHls))->ro.__clusterItemListServer, Fs_Network_Ipv4_print(((Hls*)(pHls))->rw._ipv4),((Hls*)(pHls))->rw._ipv4Port, ((Hls*)(pHls))->rw._realBufferTimeOut)
#define HlsLog(___loglevel,___format, ...) __HlsLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define HlsLog2(___loglevel,___format, ...) __HlsLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* HLS服务器通道项 */
    struct HlsServer_item {

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
            /* 网络地址,为空意义不明确 */
            char *_url;
            /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */
            char _modelType;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            /* 是否有新的实时客户端,1-有,0-没有 */
            unsigned char newRealClient : 1;
            /* 主机ip,用于集群,0表示本机,低四位表示ip */
            unsigned int _ipv4;
        } r;

        struct {
#define HlsServer_item_ClusterLen (FsPointer_to_integer(&((struct HlsServer_item*) 0)->ro._cb_client)+sizeof(void*)*3)
#define HlsServer_item_Cluster_callTimes(___pHlsServer_item)  (*(unsigned int*)((&(___pHlsServer_item)->ro._cb_client)+0))
#define HlsServer_item_Cluster_next(___pHlsServer_item)   (*(struct HlsServer_item **)((&(___pHlsServer_item)->ro._cb_client)+1))
#define HlsServer_item_Cluster_pHlsServer(___pHlsServer_item)   (*(Hls**)((&(___pHlsServer_item)->ro._cb_client)+2))
#define HlsServer_item_next(___pHlsServer_item)   (0==(___pHlsServer_item)->r._ipv4?(___pHlsServer_item)->p.next:HlsServer_item_Cluster_next(___pHlsServer_item))
#define HlsServer_item_pHlsServer(___pHlsServer_item)   (0==(___pHlsServer_item)->r._ipv4?(___pHlsServer_item)->p.pHls:HlsServer_item_Cluster_pHlsServer(___pHlsServer_item))
            /* 有客户端时的回调函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (* _cb_client) (/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p);
            /* 获取快照,为空表示不支持,成功返回FsObjectImageFrame指针,否则返回NULL */
            FsObjectImageFrame * (*_snapGet__IO)(void * const p);
            /* 获取录像信息数据,为空表示不支持,成功返回FsEbml指针,否则返回NULL */
            FsEbml * (* _recordinfo__IO) (void * const p, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
            /* 有客户端时的回掉函数关联的指针,获取快照相关的指针 */
            void *_cb_clientP;
            /* 实时视频数据链表,为空表示不支持实时视频 */
            FsObjectList *__realVideoList;
            /* 连接的实时客户端链表,只有当实时视频数据链表不为空时才有效,使用__newClientList的同步锁 */
            FsObjectList *__realClientListOrder__newClientList;
            /* 实时视频数据的索引,只有当实时视频数据链表不为空时才有效 */
            unsigned char *_realVideoIndex;
            /* 发送数据时ts里面的timestamp值,只有当实时视频数据链表不为空时才有效 */
            unsigned long long *_timestamp;
        } ro;

        struct {
            /* 新的客户端连接或有改变的客户端,不排序 */
            FsObjectList *__newClientList;
            /* 点播客户端链表,实时视频数据链表不为空时,此链表储存历史点播的客户端,否则储存文件点播客户端,也可能不支持点播,使用__newClientList的同步锁 */
            FsObjectList *__demandClientListOrder__newClientList;
        } rw;

        struct {
            /* 上一次实时视频的数量 */
            unsigned int realVideoListCountLast;
            /* 实时数据的掩码 */
            unsigned int mask_realAll;
            /* 实时数据中不是所有帧都为关键帧的掩码 */
            unsigned int mask_realkey;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程 */
            int pipe_w;
            /* 统计实时流中的帧率,结果应更接近低频通道 */
            float _realRate_low;
            /* 用于统计realRate_low的辅助变量 */
            float realRate_low_last;
            /* 具有相同url的下一个HlsServer通道 */
            struct HlsServer_item *next;
            /* 所属的HlsServer对象 */
            Hls *pHls;
        } p;
    };
#define __HlsServer_itemLog(___Log,___loglevel,___format, ...) do{\
    if(0==((struct HlsServer_item*)(pHlsServer_item))->r._ipv4){\
        ___Log(___loglevel,5,___format",p=%p(%hhx/%hhx/%hhx/%hhx)\n  url:\"%s\"\n  _ipv4:\"%hhu.%hhu.%hhu.%hhu\"\n  cb_client=%p/%p/%p\n  realVideoList=%p%lu\n\
  realClientListOrder=%p/%lu\n  realVideoIndex=%p\n  timestamp=%p\n  newClientList=%p/%lu\n  demandClientListOrder=%p/%lu\n  realVideoListCountLast=%u\n  mask_realAll=%x/%x\n  pipe_w=%d\n\
  realRate_low=%f/%f,next=%p\n  pHls=%p\n",##__VA_ARGS__\
            ,pHlsServer_item,((struct HlsServer_item*)(pHlsServer_item))->r._modelType,((struct HlsServer_item*)(pHlsServer_item))->r.busy\
            ,((struct HlsServer_item*)(pHlsServer_item))->r._controlStatus,((struct HlsServer_item*)(pHlsServer_item))->r.newRealClient\
            ,FsStrprint(((struct HlsServer_item*)(pHlsServer_item))->r._url,""),Fs_Network_Ipv4_print(((struct HlsServer_item*)(pHlsServer_item))->r._ipv4)\
            ,((struct HlsServer_item*)(pHlsServer_item))->ro._cb_client,((struct HlsServer_item*)(pHlsServer_item))->ro._snapGet__IO,((struct HlsServer_item*)(pHlsServer_item))->ro._cb_clientP\
            ,((struct HlsServer_item*)(pHlsServer_item))->ro.__realVideoList\
            ,((struct HlsServer_item*)(pHlsServer_item))->ro.__realVideoList!=NULL?((struct HlsServer_item*)(pHlsServer_item))->ro.__realVideoList->nodeCount:0\
            ,((struct HlsServer_item*)(pHlsServer_item))->ro.__realClientListOrder__newClientList\
            ,((struct HlsServer_item*)(pHlsServer_item))->ro.__realClientListOrder__newClientList!=NULL?((struct HlsServer_item*)(pHlsServer_item))->ro.__realClientListOrder__newClientList->nodeCount:0\
            ,((struct HlsServer_item*)(pHlsServer_item))->ro._realVideoIndex,((struct HlsServer_item*)(pHlsServer_item))->ro._timestamp\
            ,((struct HlsServer_item*)(pHlsServer_item))->rw.__newClientList,((struct HlsServer_item*)(pHlsServer_item))->rw.__newClientList->nodeCount\
            ,((struct HlsServer_item*)(pHlsServer_item))->rw.__demandClientListOrder__newClientList\
            ,((struct HlsServer_item*)(pHlsServer_item))->rw.__demandClientListOrder__newClientList!=NULL?((struct HlsServer_item*)(pHlsServer_item))->rw.__demandClientListOrder__newClientList->nodeCount:0\
            ,((struct HlsServer_item*)(pHlsServer_item))->p.realVideoListCountLast,((struct HlsServer_item*)(pHlsServer_item))->p.mask_realAll,((struct HlsServer_item*)(pHlsServer_item))->p.mask_realkey\
            ,((struct HlsServer_item*)(pHlsServer_item))->p.pipe_w,((struct HlsServer_item*)(pHlsServer_item))->p._realRate_low,((struct HlsServer_item*)(pHlsServer_item))->p.realRate_low_last,((struct HlsServer_item*)(pHlsServer_item))->p.next,((struct HlsServer_item*)(pHlsServer_item))->p.pHls);\
    }else{\
        ___Log(___loglevel,5,___format",p=%p(%hhx/%hhx/%hhx/%hhx)\n  url:\"%s\"\n  _ipv4:\"%hhu.%hhu.%hhu.%hhu\"\n",##__VA_ARGS__\
            ,pHlsServer_item,((struct HlsServer_item*)(pHlsServer_item))->r._modelType,((struct HlsServer_item*)(pHlsServer_item))->r.busy\
            ,((struct HlsServer_item*)(pHlsServer_item))->r._controlStatus,((struct HlsServer_item*)(pHlsServer_item))->r.newRealClient\
            ,FsStrprint(((struct HlsServer_item*)(pHlsServer_item))->r._url,""),Fs_Network_Ipv4_print(((struct HlsServer_item*)(pHlsServer_item))->r._ipv4));\
    }\
}while(0)
#define HlsServer_itemLog(___loglevel,___format, ...) __HlsServer_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define HlsServer_itemLog2(___loglevel,___format, ...) __HlsServer_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 点播的客户端链表结构 */
    struct HlsServer_item_DemandClient {
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

        /* rw放前面,以便RTSP,gb28181,HLS的回放共用(此注视含义不明,待查证) */
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
            /* 数据链表 */
            FsObjectList *__dataList;
            /* 数据的索引 */
            unsigned char index[32];
            /* 发送数据时ts里面的timestamp值 */
            unsigned long long timestamp[32];
            /* url参数 */
            FsUrlParameter *__pUrlParameter;
        } ro;
    };
    /* 
     * 创建一个新的Hls对象;
     * 返回Hls指针.
     */
    Hls *hls_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 主html页面数据文件,为空表示无主页面 */const char htmlFile[], /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port
            , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut
            , /* 获取配置信息,为空表示不支持,校验和发生改变时返回FsObjectBase指针,否则返回NULL,返回NULL可能是出错 */
            FsObjectBase * (*configGet__IO)(/* 调用时缓存的配置校验和,为空强制返回配置,不为空时会把最新的校验信息写入返回,在校验和发生改变时才返回配置数据 */unsigned int checkSum_4[]
            , void * const p, /* 类型 */ FsConfig_ExportType type, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 
             * 设置配置信息,为空表示不支持;
             * 成功返回1;
             * 失败返回-1;
             * 繁忙返回-120;
             */
            int (*const configSet__OI) (FsEbml * const pEbml, /* 设置类型,0-上传配置,1-合并配置 */const unsigned char type, void *const p
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 配置相关的指针 */ void *const configP
            , /* 获取通道状态信息,为空表示不支持,校验和发生改变时返回FsString指针,否则返回NULL,返回NULL可能是出错 */
            FsObjectBasePthreadSafety * (*const channelStatusGet__IO) (/* 调用时缓存的通道最后改变的编号,在发生改变时才返回数据,不为空 */unsigned long long *pChannelStatusIndex
            , void * const p, /* 类型 1-ebml,2-xml,3-json */const unsigned char type, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer), /* 获取通道状态信息相关的指针 */ void *const channelStatusP);
    /* 设置pHls的参数(需要线程同步的部分参数) */
    void hls_set_parameter_thread_pthreadSafety(Hls * const pHls, /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port);
    /* 设置pHls的参数(不需要线程同步的部分参数) */
    void hls_set_parameter_pthreadSafety(Hls * const pHls, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut);
    /* 删除pHls指向的实例对象 */
    void hls_delete__OI(Hls * const pHls);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void hls_startThread(Hls * const pHls
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程,只发送信号,不回收线程资源 */

    void hls_stopThread_noWait(Hls * const pHls);

    /* 结束线程 */
    void hls_stopThread(Hls * const pHls);
    /* 通知pHls的通道状态变化了 */
    void hls_update_channelStauts_pthreadSafety(Hls * const pHls);
    /* 
     * 创建一个新的HlsServer_item对象;
     * 返回HlsServer_item指针.
     */
    struct HlsServer_item * hlsServer_item_new__IO(/* 网络地址,为空意义不明确 */const char url[], /* 主机ip,为0表示本地主机,非0表示远程主机,之后的数据仅对此值为0时有效 */const unsigned int ipv4
            , /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */const char modelType
            , /* 有客户端时的回调函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*const cb_client) (/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p)
            , /* 获取快照,为空表示不支持,成功返回FsObjectImageFrame指针,否则返回NULL */ FsObjectImageFrame * (*const snapGet__IO) (void * const p)
            , /* 获取录像信息数据,为空表示不支持,成功返回FsEbml指针,否则返回NULL */ FsEbml * (*const recordinfo__IO) (void * const p, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 有客户端时的回掉函数关联的指针,获取快照相关的指针 */ void *const cb_clientP);
    /* 删除pHlsServer_item指向的实例对象 */
    void hlsServer_item_delete__OI(struct HlsServer_item * const pHlsServer_item);
    /* 向pHls中添加一个hls服务端条目 */
    void hlsServer_add_item__OI_2(Hls * const pHls, struct HlsServer_item * const pHlsServer_item);
    /* 从pHls中移除一个hls服务端条目 */
    void hlsServer_remove_item__IO_2(Hls * const pHls, struct HlsServer_item * const pHlsServer_item);
    /* 添加数据到pHlsServer_item中 */
    void hlsServer_item_add_frame(struct HlsServer_item * const pHlsServer_item, /* 要添加的数据,只支持VideoH264Object */ void *const pObject);
    /* 添加数据到pHlsServer_item中 */
    void hlsServer_item_add_frame__OI_2(struct HlsServer_item * const pHlsServer_item, /* 要添加的数据,只支持VideoH264Object */ void *const pObject);
    /* 添加点播数据到pDemandClient中 */
    void hlsServer_item_DemandClient_add_frame(struct HlsServer_item_DemandClient * const pDemandClient, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);
    /* 添加点播数据到pDemandClient中 */
    void hlsServer_item_DemandClient_add_frame__OI_2(struct HlsServer_item_DemandClient * const pDemandClient, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);
    void hls_test();

#ifdef __cplusplus
}
#endif
#endif /* HLS_H */

