/* 
 * File:   Dns.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#ifndef DNS_H
#define	DNS_H
#include "../publicDefine.h"
#include <stdio.h>
#include "ObjectList.h"
#include "Memery.h"
#include "Network.h"

#ifdef	__cplusplus
extern "C" {
#endif

    /* 
     * FsJson中的成员顺序不能改变,改变无法运行
     */
    typedef struct {
        /* 服务器ip端口号或文件名,不能为空 */
        char *server;
        /* epoll句柄 */
        long long epollfd;
        /* 域名有序链表 */
        FsObjectList *dmainList;
        /* 需要检查的域名无序链表 */
        FsObjectList *checkList;
        /* 默认的epollEvent参数值 */
        unsigned int epollEventDefault;
        /* 连接的句柄 */
        int sock;
        /* 当前使用的服务器ip地址 */
        unsigned int ipv4;
        /* 当前使用的服务器ip地址的端口号 */
        unsigned short ipv4Port;
        /* 下一次使用的服务器编号 */
        unsigned short serverIndex;
    } FsDns;
    #define __FsDnsLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,server:\"%s\"/\"%hhu.%hhu.%hhu.%hhu:%hu\",epollfd=%lld,dmainList=%p/%lu,checkList=%p/%lu,epollEventDefault=%x,sock=%d,serverIndex=%hu.\n",##__VA_ARGS__\
        ,pDns,pDns->server,Fs_Network_Ipv4_print(pDns->ipv4),pDns->ipv4Port,pDns->epollfd,pDns->dmainList,pDns->dmainList->nodeCount,pDns->checkList,pDns->checkList->nodeCount\
        ,pDns->epollEventDefault,pDns->sock,pDns->serverIndex)
#define FsDnsLog(___loglevel,___format, ...) __FsDnsLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsDnsLog2(___loglevel,___format, ...) __FsDnsLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    /*
     * 创建一个FsDns实例;
     * 返回创建的实例指针. 
     */
    FsDns* fs_dns_new__IO(/* 服务器ip端口号或文件名,不能为空 */const char server[], /* epoll句柄 */const long long epollfd, /* 默认的epollEvent参数值 */ const unsigned int epollEventDefault);

    /* 删除pDns指向的实例对象 */
    void fs_dns_delete__OI(FsDns * const pDns);
    /* 获取域名的ip,成功返回ip,失败返回0 */
    unsigned int fs_dns_get_ip(FsDns * const pDns, /* 域名,以$$开头会被忽略 */const char domain[], /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 断开与dns服务器建立连接 */
    void fs_dns_disconnect(FsDns * const pDns);
    /* 收到epoll可读时调用的函数 */
    void fs_dns_epoll_read_do(FsDns * const pDns, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 周期性调用此函数 */
    void fs_dns_do(FsDns * const pDns
    , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 打印所有缓存的dns信息 */
    void fs_dns_out(FsDns * const pDns, FILE *fd);
#ifdef FsDebug
    void fs_dns_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* DNS_H */

