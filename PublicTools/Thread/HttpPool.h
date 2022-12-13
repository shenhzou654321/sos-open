/* 
 * File:   HttpPool.h
 * Author: RuiXue
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef HTTPPOOL_H
#define	HTTPPOOL_H
#include "../publicDefine.h"
#include "../Fs/ObjectList.h"
#include "../Fs/String.h"
#include "../Fs/Network.h"

#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,2)

    struct HttpPool_item {

        /* 
         * 结构约定:
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 连接的soket,不能根据此值判断连接是否有效 */
            int sock;
            /* 最后一次成功建立连接的时间(CPU的uptime时间)没有建立连接时为0或负数 */
            double createTime;
            /* 已发送的长度 */
            unsigned int sendLen;
            /* 已接收的长度 */
            unsigned int recvLen;
            /* 接收时应先接收的长度,用以调回调函数确定此次应接收的长度 */
            unsigned short _recvHeadLenth;
            /* 
             * 要发送的队列,列表长度由外部控制;
             * 每项必须封装为FsString传入;
             * 发送后会把此条数据放入sendListOk链表中.
             */
            FsObjectList *__sendList;
            /* 发送成功的数据 */
            FsObjectList *__sendListOk;
            /* 
             * 接收的队列,列表长度由内部控制;
             * 各项的类型都为4字节的无符号整数+4字节要接收的数据长度+数据;
             * 如果长度达到队列的最大值,会删除首项.
             */
            FsObjectList *__recvList;
            /*
             * 用以调确定此次应接收的长度;
             *     根据pSocketPool_item->p.recvHeadBuffer:确定此次应接收的长度;
             *     为pSocketPool_item->p.recvHeadBuffer分配空间;
             *     设定pSocketPool_item->ro.recvLen的值;
             *     成功返回1;
             *     失败返回-1;
             */
            char (*_pGetRecvLenth__IO_2)(struct HttpPool_item *pHttpPool_item);
        } ro;

        struct {
            /* IPV4地址 */
            FsNetworkIPv4 _ipv4;
            /* IPV4地址端口 */
            unsigned short _ipv4Port;
            /* 连接模式:
             * 0:默认模式,断开后自动连接;
             * 1:断开后不重连;
             * 2:保持断开模式;
             * 3:设定此值为0,如果是连接模式,则断开一次连接;    
             */
            unsigned char connMode : 2;
            /* 最后一次发数据的时间(CPU的uptime时间) */
            double lastSendTime;
            /* 最后一次收数据的时间(CPU的uptime时间) */
            double lastRecvTime;
            /* 最后一次收发数据的时间(CPU的uptime时间) */
            double lastSendOrRecvTime;
            /* 总发送的字节数 */
            unsigned long toallSendByte;
            /* 总发送的字节数 */
            unsigned long toallRecvByte;
        } rw;
        /* 附加数据,本对象不会使用 */
        char externData[sizeof (unsigned int)*2 + sizeof (void*)];

        struct {
            /* 正在发送的数据 */
            FsString *sendingData;
            /* 正在接收的数据 */
            FsString *recvdata;
            /* 收数据时的接收头,必须放在最后,在对象申请时根据实际分配空间 */
            char recvHeadBuffer[];
        } p;
    };

    /* socket连接池,外部创建连接后把HttpPool_item添加到本对象;
     * 添加的对象出错后会添加到errorSocketList.
     */
    typedef struct {

        /* 
         * 结构约定:
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 线程名 */
            char *_threadName;
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* 处理的Socket链表 */
            FsObjectList *__socketList;
            /* 添加或删除连接项(0:没有动作;1:添加;2:删除) */
            unsigned char changeItem:2;
            /* 线程是否在运行(0:没有运行,1:在运行) */
            unsigned char inRun : 1;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 线程同步锁,仅用于添加或删除连接项 */
            pthread_mutex_t __mutex;
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
            /* 最长没有发送和接收数据的时间 */
            double _keepTime;
            /* 接收队列的最大容量 */
            unsigned int _recvListMaxCount;
            /* 连接超时的时间 */
            double _connectTimeout;
            /* 总发送的字节数 */
            unsigned long toallSendByte;
            /* 总发送的字节数 */
            unsigned long toallRecvByte;
        } rw;

        struct {
            /* 将要添加或删除的连接项 */
            struct SocketPool_item *pSocketPool_item_change;
        } p;
    } HttpPool;

#pragma pack(pop)
    /* 
     * 创建一个新的HttpPool发送与接收线程项;
     * 返回HttpPool指针.
     */
    HttpPool *httpPool_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
            /* 最长没有发送和接收数据的时间 */ const double keepTime,
            /* 接收队列的最大容量 */const unsigned int recvListMaxCount,
            /* 连接超时的时间 */const double connectTimeout);
    /* 删除pSocketPool指向的实例对象 */
    void httpPool_delete__OI(HttpPool* pHttpPool);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void httpPool_startThread(HttpPool *pHttpPool, /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级 */const unsigned char nice);
    /* 结束线程 */
    void httpPool_stopThread(HttpPool *pHttpPool);
    /* 
     * 创建一个连接项;
     * 返回HttpPool_item指针.
     */
    struct HttpPool_item *httpPool_item_new__OI(/* IPV4地址,应是FsNetworkIPv4类型数据 */const unsigned int ipv4,
            /* IPV4地址端口 */ const unsigned short ipv4Port,
            /* 发送数据后的处理模式(0:直接删除;1:放到发送成功的链表,等待其它线程处理) */const unsigned char doSendMode);


    /* 
     * 创建一个新的标准连接项,如果连接断开不重连;
     * 有8个字节的头,其中后4个字节表示该数据包的长度,前4个字节,本对象不作解析,建议按如下定义:
     *     第一个字节的第八位:表示数据是否是EBML数据,1-是,0-不是;
     *     第一个字节的第七位:表示数据是否回执数据,1-是,0-不是;
     *     第一个字节的第六位:表示数据是否是错误数据,1-是(回执数据时,表示发送的命令或请求不支持或错误,不是回执数据时,报告服务端的一些错误),0-不是;
     *     第一个字节的第一到第五位:表示一些快速解析的数据类型,在实际通信中进行字义;
     *          其中全0:表示默认的设置和回执类型,第一个字节的第七位为0,第一个字节的第六位为0且数据长度为0时表示心跳(建议第一个字节的第八位为1);
     *              非0:表示各类流;
     *              全1:表示默认的流通道;
     *     第二个字节到第四个字节:表示发送的序号,低位在前,应实现循环使用;
     * 返回HttpPool_item指针.
     */
    struct HttpPool_item *httpPool_item_new_bySock__IO__OI(/* 已建立好的sock连接 */const int sock,
            /* IPV4地址,应是FsNetworkIPv4类型数据 */const unsigned int ipv4,
            /* IPV4地址端口 */ const unsigned short ipv4Port,
            /* 发送数据后的处理模式(0:直接删除;1:放到发送成功的链表,等待其它线程处理) */const unsigned char doSendMode);
    /*
     * 删除ppSocketPool_item指向的实例对象.
     */
    void socketPool_item_delete__OI(struct SocketPool_item* pSocketPool_item);
    /* 向pSocketPool中添加一个连接项,必须在线程已运行后执行,否则会阻塞 */
    void socketPool_add_item__OI_2(SocketPool* pSocketPool, struct SocketPool_item *pSocketPool_item);
    /* 从pSocketPool中移除一个连接项,必须在线程已运行后执行,否则会阻塞 */
    void socketPool_remove_item__IO_2(SocketPool* pSocketPool, struct SocketPool_item *pSocketPool_item);
    /* 从pSocketPool中移除一个连接项并删除,必须在线程已运行后执行,否则会阻塞 */
    void socketPool_remove_and_delete_item(SocketPool *pSocketPool, struct SocketPool_item * pSocketPool_item);
    /* 
     * 获取一条接收的数据,必须先检查pSocketPool_item->rIw.recvList的长度大于0,才能调用此函数;
     * 格式都为4字节的无符号整数+4字节接收到的数据长度+数据.
     * 返回char指针.
     */
    FsString * socketPool_item_recv_get__IO(struct SocketPool_item * pSocketPool_item);
    /* 
     * 获取所有接收的数据,建议先检查pSocketPool_item->rIw.recvList的长度大于1,才能调用此函数;
     * 格式都为4字节的无符号整数+4字节接收到的数据长度+数据.
     * 返回链表指针.
     */
    FsObjectList * socketPool_item_recv_get_all__IO(struct SocketPool_item * pSocketPool_item);
    /* 
     * 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长;
     * pSocketPool_item:要添加发送数据到的连接;
     * sendDataLenth:要发送的数据长度;
     * sendData:要发送的数据;
     */
    void socketPool_item_send_buffer(const struct SocketPool_item * pSocketPool_item, const unsigned int sendDataLenth, const char sendData[]);
    /* 
     * 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长;
     * pSocketPool_item:要添加发送数据到的连接;
     * sendData:要发送的数据.
     */
    void socketPool_item_send_OI_2(const struct SocketPool_item * pSocketPool_item, FsString *sendData);
    /* 
     * 添加一条发送的数据,请先检查pSocketPool_item->ro.__sendList的长度用以控制发送队列的长度不要太长;
     * pSocketPool_item:要添加发送数据到的连接;
     * sendDataLenth:要发送的数据长度;
     * sendData:要发送的数据;
     */
    void socketPool_item_send_OI_3(const struct SocketPool_item * pSocketPool_item, const unsigned int sendDataLenth, char* sendData);
#ifdef FsDebug
    /* 发送错误信息 */
    void __socketPool_item_send_errorData_standard(const struct SocketPool_item * pSocketPool_item, /* 出错相关信息的头,前四个字节 */const unsigned int head,
            /* 错误信息,格式参见fs_String_print__IO函数说明 */const char * __restrict __format, ...);
#define socketPool_item_send_errorData_standard(...) __socketPool_item_send_errorData_standard(__VA_ARGS__,~((int)0))
#else
    /* 发送错误信息 */
    void socketPool_item_send_errorData_standard(const struct SocketPool_item * pSocketPool_item, /* 出错相关信息的头,前四个字节 */const unsigned int head,
            /* 错误信息,格式参见fs_String_print__IO函数说明 */const char * __restrict __format, ...);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* HTTPPOOL_H */

