/* 
 * File:   Socket.h
 * Author: 5aii
 *
 * Created on 2013年6月26日, 上午11:16
 */

#ifndef SOCKET_H
#define	SOCKET_H
#include "../publicDefine.h"
#ifndef MinGW
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif
#include "String.h"
#include "StringList.h"
#include "Network.h"


#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        /* 建立的连接sock */
        int sock;
        /* 对方的地址信息 */
        struct sockaddr_in severAddr;
    } C5aiiSocketUdpIpv4;

    /* 
     * 建立一个UDP连接;
     * c5aiiIpv4:自定义的IPv4结构,高位在前;
     * IPv4Port:IPv4地址端口;
     * 建立失败返回NULL;
     * 成功返回建立的连接C5aiiSocketUdpIpv4对象.
     */
    C5aiiSocketUdpIpv4* c5aii_Socket_udpIPv4_new__IO(FsIpv4 c5aiiIpv4, const unsigned short IPv4Port);
    /*
     * 删除ppSocketUdpIpv4指向的实例指针指向的对象;
     * 并把ppSocketUdpIpv4指向的实例指针设为0.
     */
    void c5aii_Socket_udpIPv4_delete__OI(C5aiiSocketUdpIpv4** ppSocketUdpIpv4);
    /*
     * 发送UDP数据包;
     * pSocketUdpIpv4:建立的UDP连接;
     * sendBuffer:发送的数据地址;
     * sendLenth:发送的数据长度;
     * 成功返回1;
     * 失败返回-1.
     */
    char c5aii_Socket_udpIPv4_send(C5aiiSocketUdpIpv4* pSocketUdpIpv4, const char* sendBuffer, const unsigned int sendLenth);
    /* 
     * 建立一个UDP连接;
     * 成功返回建立的连接sock;
     * 建立socket或绑定本地地址失败返回-1;
     * 设置SO_REUSEADDR出错返回-2;
     * 设置SO_LINGER出错返回-3;
     * 设置非阻塞失败返回-4;
     * 连接出错返回-5;
     * 获取本地端口号失败返回-6.
     */
    int fs_Socket_udpIPv4_connect__IO(/* ipv4地址,高位在前 */const unsigned int ipv4, /* 端口 */const unsigned short ipv4Port,
            /* 本地ip,为空或指向的值为0表示不指定本地ip,没有指定ip且不为空时会返回真实的ip */ unsigned int *ipv4_local,
            /* 本地端口,为空或指向的值为0表示不指定端口号,没有指定端口号且不为空时会返回真实的端口号 */ unsigned short *ipv4Port_local, /* 是否阻塞,0-阻塞,1-非阻塞 */ const char block);
    /* 
     * 建立一个UDP连接;
     * c5aiiIpv4:自定义的IPv4结构,高位在前;
     * IPv4Port:IPv4地址端口;
     * 建立socket失败返回-1;
     * 成功返回建立的连接sock.
     */
    int c5aii_Socket_udpIPv4_connect__IO_disablede(FsIpv4 c5aiiIpv4, const unsigned short IPv4Port);
    /* 
     * 接收数据;
     * 成功返回1;
     * select出错返回-1;
     * 接收出错返回-2;
     * 超时返回-3.
     */
    signed char fs_Socket_recv_by_sock(const int sock, /* 存放接收到的数据指针 */char buffer[], /* 要接收的数据长度 */unsigned int recvlen, /* 等待时间 */double waiteTime);

    /* 
     * 建立一个TCP连接;
     * 成功返回建立的连接sock;
     * 建立socket失败返回-1;
     * 设置SO_REUSEADDR出错返回-2;
     * 设置SO_LINGER出错返回-3;
     * 设置非阻塞失败返回-4;
     * 连接出错返回-5;
     * select出错返回-6;
     * getsockopt出错返回-7.
     */
    int fs_Socket_tcpIPv4_connect__IO(/* IPv4地址 */const unsigned int ipv4, /* IPv4地址端口 */ const unsigned short ipv4Port, /* 是否阻塞,0-阻塞,1-非阻塞 */ const char block,
            /* 建立连接的超时时间(单位:s) */ const double waiteTime);
    /* 
     * 建立一个TCP连接;
     * 成功返回建立的连接sock;
     * 建立socket失败返回-1;
     * getsockopt出错返回-2;
     * 等待连接超时返回-3;
     * 连接出错返回-4;
     * 设置非阻塞连接失败返回-5;
     * ioctl错误返回-6.
     */
    int fs_Socket_tcpIPv4_connect_byStringIP__IO(/* IPv4地址 */const char ipv4[], /* IPv4地址端口 */ const unsigned short ipv4Port,
            /* 是否阻塞,0-阻塞,1-非阻塞 */const char block, /* 建立连接的超时时间(单位:s) */ const double waiteTime);

    /* 
     * 绑定TCP监听端口;
     * 成功返回创建的socket;
     * 建立socket失败返回-1;
     * 设置SO_REUSEADDR出错返回-2;
     * 设置SO_LINGER出错返回-3;
     * 设置非阻塞失败返回-4;
     * bind出错返回-5;
     * listen出错返回-6.
     */
    int fs_Socket_tcpIpv4_bind__IO(/* 要监听的IPv4地址,为0表示所有IP */const unsigned int ipv4, /* 要监听的IPv4地址端口 */const unsigned short ipv4Port,
            /* 是否阻塞,0-阻塞,1-非阻塞 */const char block, /* 等待队列的连接数 */const unsigned short connectCount);
    /* 
     * 绑定TCP监听端口;
     * 成功返回创建的socket;
     * 建立socket失败返回-1;
     * 设置SO_REUSEADDR出错返回-2;
     * 设置SO_LINGER出错返回-3;
     * 设置非阻塞失败返回-4;
     * bind出错返回-5;
     * listen出错返回-6;
     * 无效IP返回-7.
     */
    int fs_Socket_tcpIpv4_bind_byStringIP__IO(/* 要监听的IPv4地址,为空表示监听所有 */const char ipv4[], /* 要监听的IPv4地址端口 */const unsigned short ipv4Port,
            /* 是否阻塞,0-阻塞,1-非阻塞 */const char block, /* 等待队列的连接数 */const unsigned short connectCount);

    /* 
     * 收取TCP连接数据;
     * 成功返回1;
     * 超时返回-1;
     * select失败返回-2;
     * 收出错返回-3.
     */
    signed char fs_Socket_tcpIPv4_recv_by_sock(/* TCP连接的sock */const int sock, /* 接收数据的空间 */char recvBuffer[],
            /* 接收数据大小 */const unsigned int recvBufferLen, /* 完成接收的最长时间 */ double waiteTime);
    /* 
     * 发送TCP连接数据;
     * 成功返回1;
     * 超时返回-1;
     * select失败返回-2;
     * 发出错返回-3.
     */
    signed char fs_Socket_tcpIPv4_send_by_sock(/* TCP连接的sock */const int sock, /* 发送数据的大小 */const unsigned int sendBufferLen, /* 发送数据的空间 */ char sendBuffer[], /* 完成发送的最长时间 */double waiteTime);
    /* 
     * 在指定时间内收取TCP连接数据;
     * 成功返回1;
     * select失败返回-1;
     * 超时返回-2;
     * 收出错返回-3;
     * 发出错返回-4.
     */
    signed char fs_Socket_tcpIPv4_send_and_recv_by_sock(/* TCP连接的sock */const int sock, /* 发送数据的空间 */const char sendBuffer[], /* 发送数据大小 */const unsigned int sendBufferLen,
            /* 接收数据的空间 */ char recvBuffer[], /* 接收数据大小 */ const unsigned int recvBufferLen, /* 完成接收的最长时间 */ double waiteTime);
    /* 
     * 在指定时间内收取TCP连接数据;
     * 建立socket失败返回-1;
     * getsockopt出错返回-2;
     * 等待连接超时返回-3;
     * 连接出错返回-4.
     * select失败返回-5;
     * 收发超时返回-6;
     * 收出错返回-7;
     * 发出错返回-8.
     */
    signed char fs_Socket_tcpIPv4_send_and_recv_by_ipv4(/* IPv4地址 */const char ipv4[], /* IPv4地址端口 */ const unsigned short ipv4Port,
            /* 发送数据的空间 */const char sendBuffer[], /* 发送数据大小 */const unsigned int sendBufferLen,
            /* 接收数据的空间 */ char recvBuffer[], /* 接收数据大小 */ const unsigned int recvBufferLen, /* 完成接收的最长时间 */double waiteTime);
    /* 
     * 在指定时间内收取TCP连接数据;
     * 成功返回FsString对象;
     * 失败返回NULL.
     */
    FsString * fs_Socket_tcpIPv4_recv_by_sock_until_string__IO(/* TCP连接的sock */const int sock, /* 结束接收数据的字条串 */ const char untilString[],
            /* 接收数据可能的大小 */const unsigned int possibleLenth, /* 完成接收的最长时间 */ double waiteTime);
    /* 
     * 在指定时间内收取TCP连接数据;
     * 成功返回收到的有效数据长度;
     * 超时返回-1;
     * select失败返回-2;
     * 收出错返回-3;
     * 缓存空间过小返回-4.
     */
    int fs_Socket_tcpIPv4_recv_by_sock_until_string(/* 接收数据的最大长度 */unsigned int bufferMaxLenth, /* 接收数据的指针 */char buffer[],
            /* TCP连接的sock */const int sock, /* 结束接收数据的字条串 */const char untilString[], /* 完成接收的最长时间 */ double waiteTime);
    /* 
     * 在指定时间内收取TCP连接数据;
     * 成功返回收到的有效数据长度;
     * 超时返回-1;
     * select失败返回-2;
     * 收出错返回-3;
     * 缓存空间过小返回-4.
     */
    int fs_Socket_tcpIPv4_recv_httpData_by_sock(/* 通过此指针返回http头的长度 */unsigned int *headLenth, /* 接收数据的最大长度 */unsigned int bufferMaxLenth, /* 接收数据的指针 */char buffer[],
            /* TCP连接的sock */const int sock, /* 完成接收的最长时间 */ double waiteTime);
    /* 
     * 在指定时间内收取TCP连接数据;
     * 成功返回FsString对象;
     * 失败返回NULL.
     */
    FsString * fs_Socket_tcpIPv4_send_and_recv_by_sock_until_string(/* TCP连接的sock */const int sock, /* 发送数据的空间 */ const char sendBuffer[], /* 发送数据大小 */const unsigned int sendBufferLen,
            /* 结束接收数据的字条串,不能为空 */ const char untilString[], /* 接收数据可能的大小 */const unsigned int possibleLenth, /* 完成接收的最长时间 */double waiteTime);
    /*
     * 依靠本机DNS设置解析域名domainName对应的IP地址;
     * 成功返回FsStringList对象;
     * 失败返回NULL.
     */
    FsStringList* fs_Socket_ipv4_get_from_localHost__IO(/* 域名 */const char domainName[]);
    /*
     * 解析域名domainName对应的IP地址;
     * 成功返回FsStringList对象;
     * 失败返回NULL.
     */
    FsStringList* fs_Socket_ipv4_get_from_dns__IO(/* 域名 */const char domainName[], /* dns服务器地址 */const char *ipv4,
            /* dns服务器端口 */const unsigned short ipv4Port, /* 建立连接的超时时间(单位:s) */double waiteTime);
    /* 
     * 设置sock的阻塞类型;
     * 成功返回1;
     * 获取参数失败返回-1;
     * 设置失败返回-2.
     */
    signed char fs_Socket_set_block(const int sock, /* 是否阻塞,0-阻塞,1-非阻塞 */const char block);

    /* 
     * 创建一个unix监听;
     * 成功返回创建的socket;
     * 建立socket失败返回-1;
     * 设置SO_REUSEADDR出错返回-2;
     * 设置SO_LINGER出错返回-3;
     * 设置非阻塞失败返回-4;
     * bind出错返回-5;
     * listen出错返回-6;
     * 检查绑定端口失败返回-7.
     */
    int fs_Socket_unix_bind__IO(/* 监听的端口,为0表示随机创建,并把监听的端口返回 */unsigned short *port, /* 是否阻塞,0-阻塞,1-非阻塞 */const char block, /* 等待队列的连接数 */const unsigned short connectCount);
    /*
     * 非阻塞方式接受一个客户端连接(外部已检查过此有新客户端到来才能用此函数);
     * 成功返回连接socket;
     * accept出错返回-2;
     * 设置SO_REUSEADDR出错返回-3;
     * 设置SO_LINGER出错返回-4;
     * 设置非阻塞失败返回-5;
     * 超时返回-6. 
     */
    int fs_Socket_unix_accept_noblock(/* 如果此值不为空,在连接成功时,返回客户端连接IP */unsigned int *ipv4, /* ipv4不为空时,此值也不能为空,返回客户端连接端口号 */unsigned short *ipv4Port,
            /* 监听的socket,只能是非阻塞型的 */const int listenSock, /* 是否阻塞,0-阻塞,1-非阻塞 */const char block);
    /*
     * 接受一个客户端连接;
     * 成功返回连接socket;
     * select出错返回-1;
     * accept出错返回-2;
     * 设置SO_REUSEADDR出错返回-3;
     * 设置SO_LINGER出错返回-4;
     * 设置非阻塞失败返回-5;
     * 超时返回-6. 
     */
    int fs_Socket_unix_accept(/* 如果此值不为空,在连接成功时,返回客户端连接IP */unsigned int *const ipv4, /* ipv4不为空时,此值也不能为空,返回客户端连接端口号 */unsigned short *const ipv4Port,
            /* 监听的socket,只能是非阻塞型的 */const int listenSock, /* 是否阻塞,0-阻塞,1-非阻塞 */const char block, /* 等待时间 */ const double waiteTime);
    /* 
     * 建立一个Unix连接;
     * 成功返回建立的连接sock;
     * 建立socket失败返回-1;
     * 设置SO_REUSEADDR出错返回-2;
     * 设置SO_LINGER出错返回-3;
     * 设置非阻塞失败返回-4;
     * 连接出错返回-5;
     * select出错返回-6;
     * getsockopt出错返回-7.
     */
    int fs_Socket_unix_connect__IO(/* 端口 */const unsigned short port, /* 是否阻塞,0-阻塞,1-非阻塞 */const char block, /* 等待时间 */ const double waiteTime);
    /* 
     * 获取连接的地址;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_Socket_addr_get(/* 本地ipv4指针,返回值 */unsigned int *ipv4_l, /* 本地ipv4端口号指针,返回值 */unsigned short *ipv4Port_l,
            /* 远程ipv4指针,返回值 */unsigned int *ipv4_r, /* 远程ipv4端口号指针,返回值 */unsigned short *ipv4Port_r, /* 已连接的socket */const int sock);
    /* 
     * 获取连接的本地地址;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_Socket_addr_get_local(/* ipv4指针,返回值 */unsigned int *ipv4, /* ipv4端口号指针,返回值 */unsigned short *ipv4Port, /* 已连接的socket */const int sock);
    /* 
     * 获取连接的远程地址;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_Socket_addr_get_remote(/* ipv4指针,返回值,可为空 */unsigned int *ipv4, /* ipv4端口号指针,返回值,可为空 */unsigned short *ipv4Port, /* 已连接的socket */const int sock);
    /* 
     * 用socket来模拟pipe;
     * 成功返回1;
     * 失败返回-1;
     */
    signed char fs_socket_pipe(int __fildes[2]);
#ifdef FsDebug
    void fs_Socket_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* SOCKET_H */

