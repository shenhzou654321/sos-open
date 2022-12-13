/* 
 * File:   BindClassifyServer.h
 * Author: fslib.org
 *
 * Created on 2017年8月25日, 上午11:19
 */

#if !defined BINDCLASSIFYSERVER_H && !defined JAVA_IN_C

#define BINDCLASSIFYSERVER_H
//#define SocketPool_HttpDebug
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/BindClassify.h"

#include "../../PublicTools/Fs/Ebml.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Object.h"
#include "../../PublicTools/Fs/ObjectList.h"
#include "../../PublicTools/Fs/String.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 
     * 带线程,带认证的BindClassify;
     */
    typedef struct {
        /* 继承FsBindClassify */
        FsBindClassify bindClassify;

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
#define BindClassifyServer_get_threadName(___pBindClassifyServer) (((char*)((___pBindClassifyServer)+1))+(___pBindClassifyServer)->ro._threadName_)
            unsigned char _threadName_;
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
            /* 外部和内部线程异步通信的链表,结构为BindClassifyServer_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
        } p;
    } BindClassifyServer;

#define __BindClassifyServerLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p,runStatus=%hhd/%hhu,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  \n",##__VA_ARGS__\
        ,pBindClassifyServer,((BindClassifyServer*)(pBindClassifyServer))->rw.runStatus,((BindClassifyServer*)(pBindClassifyServer))->p.inRun,((BindClassifyServer*)(pBindClassifyServer))->p.stid,((BindClassifyServer*)(pBindClassifyServer))->ro._pMonitor\
        ,((BindClassifyServer*)(pBindClassifyServer))->ro._externRunStatus,((BindClassifyServer*)(pBindClassifyServer))->ro._parentTid,((BindClassifyServer*)(pBindClassifyServer))->ro.__pipe[0], ((BindClassifyServer*)(pBindClassifyServer))->ro.__pipe[1],BindClassifyServer_get_threadName(pBindClassifyServer)\
        )
#define BindClassifyServerLog(___loglevel,___format, ...) __BindClassifyServerLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define BindClassifyServerLog2(___loglevel,___format, ...) __BindClassifyServerLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 
     * 创建一个新的BindClassifyServer发送与接收线程项;
     * 返回BindClassifyServer指针.
     */

    BindClassifyServer *bindClassifyServer_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]);

    /* 删除pBindClassifyServer指向的实例对象 */

    void bindClassifyServer_delete__OI(BindClassifyServer * const pBindClassifyServer);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void bindClassifyServer_startThread(BindClassifyServer * const pBindClassifyServer
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void bindClassifyServer_stopThread_noWait(BindClassifyServer * const pBindClassifyServer);

    /* 结束线程 */

    void bindClassifyServer_stopThread(BindClassifyServer * const pBindClassifyServer);

    /* 获取pBindClassifyServer的信息 */

    FsEbml * bindClassifyServer_get_info_pthreadSafety__IO(BindClassifyServer * const pBindClassifyServer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取已初始化的端口,在校验和发生变化时把结果存于pStructList_short中(存之前会清空pStructList_short中的已有数据),返回新的校验和 */

    unsigned int bindClassifyServer_get_hasInit_tcpIpv4_ports_pthreadSafety(/* 储存结果的FsStructList链表,结构大小不小于一个short,数据按端口排序,内部操作pStructList_short时会加锁 */FsStructList * const pStructList_short_
            , /* 修改pStructList_short时使用的互斥锁,为空表示无须加锁 */pthread_mutex_t * const pStructList_short_mutex, /* 当前pStructList_int数据的校验和 */const unsigned int sum
            , BindClassifyServer * const pBindClassifyServer, /* 匹配的ip地址,非0ip地址与0匹配,255.255.255.255与所有地址匹配 */const unsigned int ipv4, /* 查找业务掩码,只有匹配的才返回,0匹配所有 */const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 向pBindClassifyServer中添加一个pBindClassify_item */

    void bindClassifyServer_add_item__OI_2(BindClassifyServer * const pBindClassifyServer, struct FsBindClassify_item * const pBindClassify_item
            , /* pBindClassify_item是否作为pBindClassifyServer的默认项,0-不是,1-是,多次设置以最后一次设置为准 */const unsigned char isDefault);

    /* 从pBindClassifyServer中移除一个pBindClassify_item,未等待线程真正移除 */

    void bindClassifyServer_remove_item_noWait__IO_2(BindClassifyServer * const pBindClassifyServer, struct FsBindClassify_item * const pBindClassify_item);

    /* 从pBindClassifyServer中移除一个pBindClassify_item */

    void bindClassifyServer_remove_item__IO_2(BindClassifyServer * const pBindClassifyServer, struct FsBindClassify_item * const pBindClassify_item);

    /* 新建一个主动的ipv4 tcp连接,成功返回1,失败返回-1,localPort必须是pBindClassifyServer正在监听的端口,不是或者端口有主动连接任务都会返回-1 */

    void bindClassifyServer_tcpIpv4_connect_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* 查找业务掩码,只有匹配的才返回,0匹配所有 */const unsigned int mask
            , /* 连接的ip */const unsigned int ipv4, /* 连接的端口,0表示等待第一个被动连接 */const unsigned short ipv4Port, /* 连接使用的本地端口 */const unsigned short localIpv4Port
            , /* 连接成功时的回调函数,为空表示连接成功后不回调(和匹配时的回调函数相同),成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2);

    /* 取消一个主动的ipv4 tcp连接,内部成功建立连接或者内部关联的pBindClassify_item出错后会自动清理,此函数主要用于内部未成功建立连接时,外部移除时进行清理 */

    void bindClassifyServer_tcpIpv4_connect_cancel_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 连接成功时的回调函数,为空表示连接成功后不回调(和匹配时的回调函数相同),成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2);

    /* 添加一个自定义8字节头协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_haed8_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义8字节头协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_head8_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义8字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_haed8_1Byte_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义8字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_head8_1Byte_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义16字节头协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_haed16_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义16字节头协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_head16_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个自定义16字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_haed16_1Byte_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask);

    /* 删除一个自定义16字节头协议,并校验第一字节匹配,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_head16_1Byte_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* 要匹配的第一字节内容 */const unsigned char b
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个http控制协议,url+cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_http_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* http匹配的url前缀,可为空,如/aa */const char url[]
            , /* 协议方式,添加与移除时值应相同,&0x1==0x1表示支持GET,&0x2==0x2表示支持POST */unsigned char method
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除一个http控制协议,url+cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_http_pthreadSafety(BindClassifyServer * const pBindClassifyServer, /* http匹配的url前缀,可为空,如/aa */const char url[]
            , /* 协议方式,添加与移除时值应相同,&0x1==0x1表示支持GET,&0x2==0x2表示支持POST */unsigned char method
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个sip控制协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_sip_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除一个sip控制协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_sip_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



    /* 添加一个GB28181_rtp控制协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_add_protocol_GB28181_rtp_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 删除一个GB28181_rtp控制协议,cb+externP1+externP2必须为唯一值 */

    void bindClassifyServer_remove_protocol_GB28181_rtp_pthreadSafety(BindClassifyServer * const pBindClassifyServer
            , /* 匹配时的回调函数,成功返回1,客户端保留句柄但不对pBindClassify_item_Connect增加引用返回2,失败返回-1 */
            int (*const cb) (struct FsBindClassify_item_Connect *pBindClassify_item_Connect, /* 已收取的数据 */const char data[], /* 已收取的数据长度 */unsigned int dataLen
            , /* 当前开机时间(开机时间) */ double thisUptime, /* 外部传入参数1 */void *externP1, /* 外部传入参数2 */void *externP2), /* 外部传入参数1 */ void *const externP1, /* 外部传入参数2 */ void *const externP2
            , /* 业务掩码,如本级被匹配,读取连接的来源FsBindClassify_item的掩码为mask1,如(mask1&mask)==mask则回调,否则认为此端口不具备此业务功能 */ const unsigned int mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif

#endif /* BINDCLASSIFYSERVER_H */

