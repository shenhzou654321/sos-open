/* 
 * File:   ImapPool.h
 * Author: 5aii
 *
 * Created on 2013年6月25日, 下午12:32
 */

#ifndef IMAPPOOL_H
#define	IMAPPOOL_H
#include "../C5aii/ObjectList.h"
#include "../C5aii/StringBuilder.h"
#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,2)

    /* IMAP邮件客户端对象 */
  struct ImapPool_item {

        /* 初始化时为可写全过程可读区*/
        struct {
            /* 用户名,如admin@5aii.com */
            char *userName;
            /* 密码 */
            char *password;
            /* 服务器IPv4地址 */
            char IPv4[16];
            /* 服务器IPv4地址端口 */
            unsigned short IPv4Port;
            /* 发送时的数据前缀号 */
            C5aiiObjectList *sendHeaderList;
            /* 要发送的队列,必须实现线程同步,列表长度由外部控制;
             * 各项的类型都为4字节的无符号整数+4字节要发送的数据长度+数据;
             * 发送后会删除发送项.
             */
            C5aiiObjectList *sendList;
            /* 接收的队列,必须实现线程同步,列表长度由内部控制;
             * 各项的类型都为4字节的无符号整数+4字节要接收的数据长度+数据;
             * 如果长度达到队列的最大值,会删除首项.
             */
            C5aiiObjectList *recvList;
            /* 连接的soket */
            int sock;
        } rIw; /* 初始化时为可写全过程可读区*/

        /* 全过程只读区,写可能会出错*/
        struct {
            /* 状态:
             * 0x01:连接状态;
             * 0x02:已获取欢迎信息状态;
             * 0x07:成功连接登陆状态.
             */
            unsigned char status;
            /* 最后一次收发数据的时间 */
            double lastSendOrRecvTime;
            /* 无用的数据是否收完:
             * 0:收完;
             * 1:未收完.
             */
            char hasNoUsefullData;
        } ro; /* 全过程只读区,写可能会出错*/

        /* 全过程私有区,读写可能存在异常*/
        struct {
            /* 数据发送的接收对象 */
            struct SocketPool_item *pSocketPool_item;
            /* 在接收数据时,未收完数据时,数据放在这里;
             * 格式:4bit序号+4bit的数据长度+数据(不包含结束符0)+结束确定串.
             */
            char *tempRecvData;
            /* 接收到的数据拆为以\t\n结尾的串 */
            C5aiiStringBuilder *recvDataStringBuilder;
            /* 已发送的长度 */
            unsigned int sendLen;
        } NrNw; /* 全过程私有区,读写可能存在异常*/
    };

    /* 
     * Imap客户端收发池.
     */
    typedef struct {

        /* 初始化时为可写全过程可读区*/
        struct {
            /* 线程名*/
            char *threadName;
            /* 处理的IMAP客户端链表 */
            C5aiiObjectList *imapList;
            /* 接收队列的最大容量 */
            unsigned int recvListMaxCount;
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中*/
            void *pMonitor;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t parentTid;
            /* listChange线程同步锁 */
            pthread_mutex_t listChange_mutex;
        } rIw; /* 初始化时为可写全过程可读区*/

        /* 全过程只读区,写可能会出错*/
        struct {
            /* 是否继续执行?
             * 0:表示重置,各种工作线程都会退出;
             * 1;保持运行,除非出现异常.
             */
            char keep_run;
            /* ThreadSocketIPv4线程的tid号*/
            pthread_t pTid;
            /* 是否有改变List的事件发生:
             * listChange==0x00:都没有;
             * (listChange&0x01)==0x1:添加条目;
             * (listChange&0x01)==0x2:删除条目;
             */
            unsigned char listChange;
        } ro; /* 全过程只读区,写可能会出错*/

        /* 全过程私有区,读写可能存在异常*/
        struct {
            /* 当要添加条目时设置此值 */
            struct ImapPool_item *addItem;
            /* 当要删除条目时设置此值 */
            struct ImapPool_item *deleteItem;
            /*  未建立连接的IMAP客户端链表 */
            C5aiiObjectList *noConnectImapList;
        } NrNw; /* 全过程私有区,读写可能存在异常*/
    } ImapPool;

#pragma pack(pop)
    /* 
     * 创建一个新的ImapPool对象;
     * 成功返回ImapPool指针;
     * 失败返回NULL;
     * recvListMaxCount:接收队列的最大容量;  
     * pMonitor:监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中;
     * parentTid:父线程的线程号,由systemThreadTid获得,为0表示无父线程.
     */
    ImapPool * imapPool_new__IO(const char* threadName, unsigned int recvListMaxCount, void *pMonitor,pthread_t parentTid);
    /*
     * 删除ppImapPool指向的实例指针指向的对象;
     * 并把ppImapPool指向的实例指针设为0.
     */
    void imapPool_delete__OI(ImapPool** ppImapPool);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数*/
    void imapPool_startThread(ImapPool *pImapPool);
    /* 结束线程*/
    void imapPool_stopThread(ImapPool *pImapPool);
    /* 
     * 向pImapPool中添加一个IMAP邮件客户端;
     * 成功返回ImapPool_item指针;
     * 失败返回NULL;
     * userName:用户名,如admin@5aii.com,不能为空;
     * password:密码,不能为空;
     * IPv4:服务器IPv4地址,必须为有效值;
     * IPv4Port:服务器IPv4地址端口.
     */
    struct ImapPool_item * imapPool_new_and_add_item(ImapPool* pImapPool, const char* userName, const char* password, const char *IPv4, const unsigned short IPv4Port);
    /* 
     * 从队列中移除任务,并删除.
     */
    void imapPool_remove_and_delete_item(ImapPool *pImapPool, struct ImapPool_item ** ppImapPool_item);
        /* 
     * 获取一条接收的数据,必须先检查pImapPool_item->rIw.recvList的长度大于0,才能调用此函数;
     * 格式都为4字节的无符号整数+4字节接收到的数据长度+数据.
     * 返回char指针;
     */
    char * imapPool_item_recv_get__IO(struct ImapPool_item* pImapPool_item);
    /* 
     * 添加一条发送的数据,请先检查pImapPool_item->rIw.sendList的长度用以控制发送队列的长度不要太长;
     * 格式都为4字节的无符号整数+4字节要发送的数据长度+数据.
     */
    void imapPool_item_send(struct ImapPool_item* pImapPool_item, const char* sendDataHead, const char* sendData);
    /*
     * 用pImapPool_item的初始化参数连接服务器,已注册,请不要再次注册;
     * waiteTime:建立连接的超时时间;
     * 成功返回1;
     * 连接失败返回-1;
     * 超时返回-2.
     */
    char imapPool_item_login(struct ImapPool_item* pImapPool_item, double waiteTime);
    /*
     * 在pImapPool_item注册后才可调用此函数注销,请先检查是否注册;
     * waiteTime:注销的超时时间;
     * 成功返回1;
     * 连接失败返回-1;
     * 超时返回-2.
     */
    char imapPool_item_logout(struct ImapPool_item* pImapPool_item, double waiteTime);

    //    
    //    
    //    /*      
//     * 创建一个新的EmailClient对象;
//     * 成功返回EmailClient指针;
//     * 失败返回NULL;
//     * userName:用户名,如admin@5aii.com,不能为空;
//     * password:密码,不能为空;
//     * IPv4:服务器IPv4地址,必须为有效值;
//     * IPv4Port:服务器IPv4地址端口;
//     * connectType:连接类型.
//     */
//    ImapPool * c5aii_EmailClient_new__IO(const char* userName, const char* password, const char *IPv4, const unsigned short IPv4Port, const C5aiiEmailClientConnectType connectType);
//    /*
//     * 删除ppEmailClient指向的实例指针指向的对象;
//     * 并把ppEmailClient指向的实例指针设为0.
//     */
//    void c5aii_EmailClient_delete__OI(ImapPool** ppEmailClient);
//    /*
//     * 用pEmailClient的初始化参数连接服务器,已注册,请不要再次注册;
//     * waiteTime:建立连接的超时时间,在waiteTime建立连接失败,返回-1;
//     * 成功返回1;
//     * 已经连接返回2;
//     * 连接失败返回-1;
//     * 收发注册数据失败返回-2;
//     * 注册失败返回-3;
//     * 超时返回-4.
//     */
//    char c5aii_EmailClient_login(ImapPool* pEmailClient, double waiteTime);
//    /*
//     * 注销pEmailClient.
//     */
//    void c5aii_EmailClient_logout(ImapPool* pEmailClient);
#ifdef	__cplusplus
}
#endif

#endif	/* IMAPPOOL_H */

