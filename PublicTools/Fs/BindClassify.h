/* 
 * File:   BindClassify.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#if !defined BINDCLASSIFY_H && !defined JAVA_IN_C

#define BINDCLASSIFY_H
#define FsBindClassify_enable_MSG_PEEK // 定义后内部收数据先尝试 MSG_PEEK 模式
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/CycleList.h"
#include "../../PublicTools/Fs/Ebml.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "Socket.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)

    struct FsBindClassify_item;

    /* 监听端口的管理 */
    typedef struct {
        /* 未初始化的FsBindClassify_item,按type+bindInfo+FsBindClassify_item指针排序,未加入epoll中 */
        FsObjectList *listNotInitOrder_;
        /* 已初始化的FsBindClassify_item,按type+bindInfo+FsBindClassify_item指针排序,已加到epoll中 */
        FsObjectList *listhasInitOrder_;
        /* 匹配的协议,节点为FsBindClassify_P_Match,退出时节点应为空 */
        FsObjectList *matchList_;
        /* tcp建立ipv4连接的链表,成员为struct FsBindClassify_P_tcpIpv4_connect指针,可为空 */
        FsObjectList *tcpIpv4_connectListOrder_;
        /* 默认的FsBindClassify_item,让FsBindClassify有一个默认项,为空表示没有默认项 */
        struct FsBindClassify_item *pBindClassify_itemDefault;
        /* 标记listhasInit的数据变化的校验和,变化后不会为0 */
#define FsBindClassify_listhasInit_sum_add(___pBindClassify) do{\
    unsigned int v=(___pBindClassify)->listhasInit_sum+1;\
    (___pBindClassify)->listhasInit_sum=v!=0?v:1;\
}while(0)   
        unsigned int listhasInit_sum;
    } FsBindClassify;

#define __FsBindClassifyLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,listNotInitOrder_=%p/%lu,listhasInitOrder_=%p/%lu,matchList_=%p/%lu,tcpIpv4_connectListOrder_=%p/%lu,pBindClassify_itemDefault=%p,listhasInit_sum=%x.\n",##__VA_ARGS__\
        ,pBindClassify,pBindClassify->listNotInitOrder_,pBindClassify->listNotInitOrder_->nodeCount,pBindClassify->listhasInitOrder_,pBindClassify->listhasInitOrder_->nodeCount,pBindClassify->matchList_,pBindClassify->matchList_->nodeCount\
        ,pBindClassify->tcpIpv4_connectListOrder_,pBindClassify->tcpIpv4_connectListOrder_?pBindClassify->tcpIpv4_connectListOrder_->nodeCount:0,pBindClassify->pBindClassify_itemDefault,pBindClassify->listhasInit_sum)
#define FsBindClassifyLog(___loglevel,___format, ...) __FsBindClassifyLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsBindClassifyLog2(___loglevel,___format, ...) __FsBindClassifyLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 监听分类--监听端口,在有连接时先判定其类型后通过回调把连接回调出去 */
    struct FsBindClassify_item {
        /* 在BindClassifyServer中使用的变量:是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
        unsigned char busy : 1;
        /* 在BindClassifyServer中使用的变量:控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,&0x1-表示是否添加或移除 */
        unsigned char controlStatus : 2;
        unsigned char : 5;
        /* 类型,0-ipv4的tcp端口 */
        unsigned char type : 6;
        /* 是否被禁用,在添加到FsBindClassify时设置,0-表示未被禁用,1-表示因同默认项相同或层级不够而被禁用 */
        unsigned char disabled : 1;

        /* 监听信息 */
        union {

            /* 监听tcp时的ipv4端口 */
            struct {
                /* 监听端口 */
                unsigned short port;
                /* 监听ip,0表示监听所有ip */
                unsigned int ipv4;
            } tcpIpv4;
        } bindInfo;
#define FsBindClassify_item_bind_is_equal(___pBindClassify_item1,___pBindClassify_item2) \
    ((___pBindClassify_item1)->type==(___pBindClassify_item2)->type&&memcmp(&(___pBindClassify_item1)->bindInfo,&(___pBindClassify_item2)->bindInfo,sizeof((___pBindClassify_item1)->bindInfo))==0)
        /* 监听的句柄,初始化为-1,表示无效 */
        int bindSock;
        /* 支持的业务掩码,在找到匹配项后需要校验是否包含匹配项的掩码,只有包含时才回调,否则拒绝连接,如FsBindClassify_P_Match_CallBack的掩码为mask1,如(mask1&mask)==mask1则回调,否则认为此端口不具备此业务功能 */
        unsigned int mask;
        /* 客户端链表 */
        FsCycleList clientList;
        /* 关联的建立tcp ipv4连接的任务,不为空时代表正在建立连接中 */
        void *pBindClassify_P_tcpIpv4_connect;
    };

#define __FsBindClassify_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p/%hhu/%hhx/%hhu/%hhu,tcpIpv4:\"%hhu.%hhu.%hhu.%hhu:%hu\",bindSock=%d,mask=%x,clientList.nodeCount=%lu,pFsBindClassify_P_tcpIpv4_connect=%p.\n",##__VA_ARGS__\
        ,pBindClassify_item,pBindClassify_item->busy,pBindClassify_item->controlStatus,pBindClassify_item->type,pBindClassify_item->disabled\
        ,Fs_Network_Ipv4_print(pBindClassify_item->bindInfo.tcpIpv4.ipv4),pBindClassify_item->bindInfo.tcpIpv4.port\
        ,pBindClassify_item->bindSock,pBindClassify_item->mask,Fs_CycleList_get_count(pBindClassify_item->clientList),pBindClassify_item->pBindClassify_P_tcpIpv4_connect)
#define FsBindClassify_itemLog(___loglevel,___format, ...) __FsBindClassify_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsBindClassify_itemLog2(___loglevel,___format, ...) __FsBindClassify_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 由FsBindClassify_item创建的连接 */
    struct FsBindClassify_item_Connect {
        /* 继承FsCycleListNode */
        struct FsCycleListNode cycleListNode;
        /* 所属的pBindClassify_item,外部不能读取此指针的值 */
        void *pBindClassify_item;
        /* 引用计数,在此值为0时删除 */
        unsigned int referCount;
        /* 错误,0-无错误,1-数据出错,101-强制断开 */
        unsigned char error;
        /* 类型,0-tcpIpv4连接 */
        unsigned char type : 7;
        /* 是否输出,0-未输出,1-输出,在确定协议后,句柄不再由本模块的epoll控制,本模块仅保留对其引用,以便做数据统计和提供一个强制断流的统一方法 */
        unsigned char out : 1;

        /* 地址信息 */
        union {

            /* type为0时的地址信息 */
            struct {
                /* 对端的端口,认为自己是服务器 */
                unsigned short remotePort;
                /* 本地端口,认为自己是服务器 */
                unsigned short localPort;
                /* 对端ip,认为自己是服务器 */
                unsigned int remoteIpv4;
                /* 本地ip,认为自己是服务器,此值使用本地服务器的监听ip,可能为0,并不对应socket的真实ip,真实ip可使用fs_Socket_addr_get_local获取 */
                unsigned int localIpv4;

            } tcpIpv4;
        } addrInfo;
        /* 连接的socket句柄,在回掉后如句柄被保留且不使用FsBindClassify_item_Connect,则需要把此值置-1 */
        int sock;
        /* 最后一次接收数据的时间,使用cpu的开机时间 */
        double lastRecvDataTime;
        /* 最后一次接收数据的时间,使用cpu的开机时间 */
        double lastSendDataTime;
        /* 在out为0时接收数据的空间,第一个int大小节表示数据长度,为空表示无暂存数据 */
#define FsBindClassify_item_Connect_recvData_Hi(___pBindClassify_item_Connect) (((char**)fs_pointer_to_void_pointer(&(___pBindClassify_item_Connect)->lastSendDataTime))[0])
    };

    /* 在外部保留FsBindClassify_item_Connect的句柄后,在有数据发送时应调用此宏更新其数据 */
#define FsBindClassify_item_Connect_sendInfo_update(___FsBindClassify_item_Connect,___sendDataTime) do{\
    /* 最后一次接收数据的时间,使用cpu的开机时间 */\
    ___FsBindClassify_item_Connect->lastSendDataTime=___sendDataTime;\
}while (0)
    /* 在外部保留FsBindClassify_item_Connect的句柄后,在数据接收后应调用此宏更新其数据 */
#define FsBindClassify_item_Connect_recvInfo_update(___FsBindClassify_item_Connect,___recvDataTime) do{\
    /* 最后一次接收数据的时间,使用cpu的开机时间 */\
    ___FsBindClassify_item_Connect->lastRecvDataTime=___recvDataTime;\
}while (0)
#define __FsBindClassify_item_ConnectLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p/%p/%u/%hhu/%hhu/%hhu,tcpIpv4:\"%hhu.%hhu.%hhu.%hhu:%hu\"/\"%hhu.%hhu.%hhu.%hhu:%hu\",sock=%d,lastRecvDataTime=%lf/%lf.\n",##__VA_ARGS__\
        ,pBindClassify_item_Connect,pBindClassify_item_Connect->pBindClassify_item,pBindClassify_item_Connect->referCount,pBindClassify_item_Connect->error,pBindClassify_item_Connect->type,pBindClassify_item_Connect->out\
        ,Fs_Network_Ipv4_print(pBindClassify_item_Connect->addrInfo.tcpIpv4.localIpv4),pBindClassify_item_Connect->addrInfo.tcpIpv4.localPort\
        ,Fs_Network_Ipv4_print(pBindClassify_item_Connect->addrInfo.tcpIpv4.remoteIpv4),pBindClassify_item_Connect->addrInfo.tcpIpv4.remotePort\
        ,pBindClassify_item_Connect->sock,pBindClassify_item_Connect->lastRecvDataTime,pBindClassify_item_Connect->lastSendDataTime)
#define FsBindClassify_item_ConnectLog(___loglevel,___format, ...) __FsBindClassify_item_ConnectLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsBindClassify_item_ConnectLog2(___loglevel,___format, ...) __FsBindClassify_item_ConnectLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个FsBindClassify;
     * 返回FsBindClassify指针.
     */

    FsBindClassify * fs_bindClassify_new__IO();

    /* 
     * 创建一个FsBindClassify;
     * 返回FsBindClassify指针.
     */

    FsBindClassify * fs_bindClassify_new_with_externData__IO(/* 附加数据的长度 */const unsigned int externDataLenth);

    /* 删除pBindClassify指向的实例对象 */

    void fs_bindClassify_delete__OI(FsBindClassify * const pBindClassify, /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd
            , /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent);
    
    /* 清理pBindClassify,在其依赖的epoll被关闭后,而pBindClassify不删除,需要调用此函数清理让其回到未初始化的状态 */

    void fs_bindClassify_clean(FsBindClassify * const pBindClassify, /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd
            , /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent);
  
    /* 获取pBindClassify的信息 */

    FsEbml * fs_bindClassify_get_info__IO(FsBindClassify * const pBindClassify, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
   
    /* 获取已初始化的tcp端口,在校验和发生变化时把结果存于pStructList_short中(存之前会清空pStructList_short中的已有数据),返回新的校验和 */

    unsigned int fs_bindClassify_get_hasInit_tcpIpv4_ports(/* 储存结果的FsStructList链表,结构大小不小于一个short,数据按端口排序,内部操作pStructList_short时会加锁 */FsStructList * const pStructList_short_
            , /* 修改pStructList_short时使用的互斥锁,为空表示无须加锁 */pthread_mutex_t * const pStructList_short_mutex, /* 当前pStructList_int数据的校验和 */const unsigned int sum
            , FsBindClassify * const pBindClassify, /* 匹配的ip地址,非0ip地址与0匹配,255.255.255.255与所有地址匹配 */const unsigned int ipv4, /* 查找业务掩码,只有匹配的才返回,0匹配所有 */const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    
    /* 新建一个主动的ipv4 tcp连接,成功返回1,失败返回-1,localPort必须是pBindClassifyServer正在监听的端口,不是或者端口有主动连接任务都会返回-1 */

    int fs_bindClassify_tcpIpv4_connect(FsBindClassify * const pBindClassify, /* 查找业务掩码,只有匹配的才返回,0匹配所有 */const unsigned int mask
            , /* 连接的ip */const unsigned int ipv4, /* 连接的端口,0表示等待第一个被动连接 */const unsigned short ipv4Port, /* 连接使用的本地端口 */const unsigned short localIpv4Port
            , /* 连接成功时的回调函数,为空表示连接成功后不回调(和匹配时的回调函数相同),成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2, /* 当前开机时间(开机时间) */ double thisUptime
            , /* epoll句柄 */const long long epollfd, /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent
            , /* 默认的epollEvent参数值 */ const unsigned int epollEventDefault
            , /* 把FsBindClassify_item加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_itemTag, /* 把FsBindClassify_item_Connect加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_item_ConnectTag
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
   
    /* 取消一个主动的ipv4 tcp连接,内部成功建立连接或者内部关联的pBindClassify_item出错后会自动清理,此函数主要用于内部未成功建立连接时,外部移除时进行清理 */

    void fs_bindClassify_tcpIpv4_connect_cancel(FsBindClassify * const pBindClassify
            , /* 连接成功时的回调函数,为空表示连接成功后不回调(和匹配时的回调函数相同),成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* epoll句柄 */const long long epollfd, /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent
            , /* 默认的epollEvent参数值 */ const unsigned int epollEventDefault
            , /* 把FsBindClassify_item加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_itemTag
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义8字节头协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_head8(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义8字节头协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_head8(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义8字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_head8_1Byte(FsBindClassify * const pBindClassify, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义8字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_head8_1Byte(FsBindClassify * const pBindClassify, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义16字节头协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_head16(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义16字节头协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_head16(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义16字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_head16_1Byte(FsBindClassify * const pBindClassify, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义16字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_head16_1Byte(FsBindClassify * const pBindClassify, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个http控制协议,url+cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_http(FsBindClassify * const pBindClassify, /* http匹配的url前缀,可为空,如/aa */const char url[]
            , /* 协议方式,添加与移除时值应相同,&0x1==0x1表示支持GET,&0x2==0x2表示支持POST */unsigned char method
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除一个http控制协议,url+cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_http(FsBindClassify * const pBindClassify, /* http匹配的url前缀,可为空,如/aa */const char url[]
            , /* 协议方式,添加与移除时值应相同,&0x1==0x1表示支持GET,&0x2==0x2表示支持POST */unsigned char method
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个sip控制协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_sip(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除一个sip控制协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_sip(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个GB28181_rtp协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_add_protocol_GB28181_rtp(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个GB28181_rtp协议,cb+externP1+externP2必须为唯一值 */

    void fs_bindClassify_remove_protocol_GB28181_rtp(FsBindClassify * const pBindClassify
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 向pBindClassify中添加一个pBindClassify_item条目 */

    void fs_bindClassify_add_item__OI_2(FsBindClassify * const pBindClassify, struct FsBindClassify_item * const pBindClassify_item
            , /* pBindClassify_item是否作为pBindClassifyServer的默认项,0-不是,1-是,多次设置以最后一次设置为准 */const unsigned char isDefault
            , /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd
            , /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * pEpollEvent);

    /* 从pBindClassify中移除一个pBindClassify_item条目 */

    void fs_bindClassify_remove_item__IO_2(/* 为空表示不用从pBindClassify中的链表移除,一般在fs_bindClassify_delete__OI中调用 */FsBindClassify * const pBindClassify
            , struct FsBindClassify_item * const pBindClassify_item, /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd
            , /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent);

    /* 
     * 创建一个监听tcp ipv4的FsBindClassify_item对象;
     * 返回FsBindClassify_item指针.
     */

    struct FsBindClassify_item *fs_bindClassify_item_tcpIpv4_new__IO(/* ipv4地址,高位在前 */const unsigned int ipv4, /* 端口 */const unsigned short ipv4Port
            , /* 支持的业务掩码,在找到匹配项后需要校验是否包含匹配项的掩码,只有包含时才回调,否则拒绝连接,如FsBindClassify_P_Match_CallBack的掩码为mask1,如(mask1&mask)==mask1则回调,否则认为此端口不具备此业务功能 */
            const unsigned int mask);

    /* 删除pBindClassify_item指向的实例对象,应先从FsBindClassify中移除后再移除 */

    void fs_bindClassify_item_delete__OI(struct FsBindClassify_item * const pBindClassify_item);

    /* 清理pBindClassify_item,让其回到未初始化的状态 */

    void fs_bindClassify_item_clean(/* pBindClassify_item所属的pBindClassify对象 */FsBindClassify * const pBindClassify, struct FsBindClassify_item * const pBindClassify_item
            , /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd
            , /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event *pEpollEvent);

    /* 在pBindClassify_item收到epoll事件时调用此函数处理 */

    void fs_bindClassify_item_do_event(/* pBindClassify_item所属的pBindClassify对象 */FsBindClassify * const pBindClassify
            , struct FsBindClassify_item * const pBindClassify_item, /* epoll事件 */ unsigned int epoll_events, /* 当前开机时间(开机时间) */ double thisUptime
            , /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd, /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent
            , /* 默认的epollEvent参数值 */ const unsigned int epollEventDefault
            , /* 把FsBindClassify_item加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_itemTag, /* 把FsBindClassify_item_Connect加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_item_ConnectTag);

    /* 删除pBindClassify_item_Connect指向的实例对象,在引用计数为0时,删除对象 */

    void fs_bindClassify_item_Connect_delete_pthreadSafety__IO(struct FsBindClassify_item_Connect * const pBindClassify_item_Connect, /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd
            , /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent);

    /* 在pBindClassify_item_Connect收到epoll事件时调用此函数处理 */

    void fs_bindClassify_item_Connect_do_event(/* pBindClassify_item所属的pBindClassify对象 */FsBindClassify * const pBindClassify
            , struct FsBindClassify_item_Connect * const pBindClassify_item_Connect, /* epoll事件 */ unsigned int epoll_events, /* 当前开机时间(开机时间) */ double thisUptime
            , /* epoll句柄,小于0表示不用从epollfd中删除 */const long long epollfd, /* pEpollEvent的个数 */const unsigned int epollEventCount, /* 不为空时表示需要查找对应的socket是否在pEpollEvent中,在要从pEpollEvent中清除 */struct epoll_event * const pEpollEvent
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 周期性调用此函数 */

    void fs_bindClassify_do(FsBindClassify * const pBindClassify, /* epoll句柄 */const long long epollfd, /* 默认的epollEvent参数值 */ const unsigned int epollEventDefault
            , /* 把FsBindClassify_item加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_itemTag, /* 把FsBindClassify_item_Connect加到epoll时的tag标记,只能取0,1,2,3 */const unsigned char bindClassify_item_ConnectTag
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 输出pBindClassifyk中的数据 */

    void fs_bindClassify_out(FsBindClassify * const pBindClassify, FILE * const fd
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);





#ifdef __cplusplus
}
#endif

#endif /* BINDCLASSIFY_H */

