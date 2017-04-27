/* 
 * File:   Network.h
 * Author: fslib
 *
 * Created on 2013年1月10日, 上午9:51
 */

#ifndef NETWORK_H
#define	NETWORK_H
#include "../publicDefine.h"
#include "String.h"
#include "StringBuilder.h"

#define FS_INVALID_IPV4 0xFFFFFFFFU

#ifdef	__cplusplus
extern "C" {
#endif
    /* Http协议 */
#define FsUrlProtocol_Http 0x01
    /* Rtsp协议 */
#define FsUrlProtocol_Rtsp 0x02

    /* 网络中的IPv4表示方法 */
    typedef union {
        /* 网络中的IPv4表示,高字节在前 */
        unsigned int netNumber;

        /* IPv4分段表示 */
        struct {
            /* IPv4中第1位的数字 */
            unsigned char n1;
            /* IPv4中第2位的数字 */
            unsigned char n2;
            /* IPv4中第3位的数字 */
            unsigned char n3;
            /* IPv4中第4位的数字 */
            unsigned char n4;
        } n;

    } FsNetworkIPv4;

    /* IPv4结构 */
    typedef struct {
        /* IPv4地址,与网络传输中的相同,高字节在前 */
        FsNetworkIPv4 ipv4;
        /* 掩码0-32,如24表示255.255.255.0,操作函数一般都应初始化其为32 */
        unsigned char mask;
    } FsIpv4;

    /* URL参数,用free释放空间 */
    typedef struct {
        /* 请求的路径,如/index.html */
        char *uri;
        /* 参数数量 */
        unsigned int parameterCount;
        /* 参数(0:参数名;1:参数值) */
        char* (*parameter)[2];
    } FsUrlParameter;

    /* URL结构 */
    typedef struct {
        /* URL协议,用宏定义:
         * 0x01:Http协议;
         * 0x02:Rtsp协议;
         */
        unsigned int protocol;
        /* IPv4地址,与网络传输中的相同,高字节在前 */
        FsNetworkIPv4 ipv4;
        /* IPv4地址端口号 */
        unsigned short ipv4Port;
        /* 请求的路径,如/index.html */
        char *uri;
        /* 域名,可为空 */
        char *domain;
        /* 用户名,可为空 */
        char *userName;
        /* 密码,可为空 */
        char *password;

    } FsUrl;

    typedef struct {
        /* 网卡名 */
        char *name;
        /* 物理名,网桥为空 */
        char *physicalName;
        /* 物理地址 */
        char mac[18];
        /* mtu */
        unsigned short mtu;
    } FsNetworkInterface;
    /*
     * 根据url创建一个FsUrlParameter实例;
     * 返回创建的实例指针. 
     */
    FsUrlParameter* fs_UrlParameter_new_IO(/* 要获取的数据的地址,如/index.html?a=a */const char url[], /* url的长度 */const unsigned int urlLenth);
    /*
     * 根据url创建一个FsUrl实例;
     * 返回创建的实例指针. 
     */
    FsUrl* fs_Url_new__IO(/* 要获取的数据的地址,如baidu.com:80/index.html或http://baidu.com */char url[]);

    /* 删除pUrl指向的实例对象 */
    void fs_Url_delete__OI(FsUrl* pUrl);

    /*
     * 把ipv4Buffer转换为网络IP;
     * 成功返回网络IP;
     * 失败返回0xFFFFFFFF.  
     */
    unsigned int fs_ipv4_network_get(/* 可为0 */unsigned char ipv4BufferLenth, /* 可为空 */const char ipv4Buffer[]);
    /* 把字符串转换为FsIpv4 */
    void fs_ipv4_get_from_string(/* 把结果写到此结构口中,失败返回的成员中ipv4为0xFFFFFFFFU */ FsIpv4 * const rst, unsigned char bufferLenth, const char buffer[]);
    /*
     * 查找指定网卡的IP地址;
     * 成功返回指定网卡的IP地址;
     * 失败返回NULL;
     */
    FsString* fs_network_get_ip__IO(char networkName[]);
    /*
     * 查找指定网卡的IP地址;
     * 成功返回指定网卡的IP地址;
     * 失败返回-1;
     */
    long fs_network_get_ip(/* 网卡名 */const char networkName[]);
    /* 设置网卡的ip地址,成功返回1,失败返回-1 */
    signed char fs_network_ipv4_set(/* 网卡名,如eth0:1 */const char networkName[], /* ipv4地址,如192.168.1.1/24,无掩码时掩码默认为32 */const char ipv4[],
            /* mtu值,为0表示不设置 */const unsigned short mtu);
    /* 添加路由,成功返回1,失败返回-1 */
    signed char fs_network_v4_route_add(/* 路由信息,如0.0.0.0/0 192.168.1.1,不加掩码时,掩码默认为32 */const char route[], /* 网络接口名,为空表示自动查找并使用最合适的 */const char netinterface[]);
    /* 添加网桥,成功返回1,失败返回-1 */
    signed char fs_network_bridge_add(/* 网桥名,最长15个字节 */const char brname[]);
    /* 删除网桥,成功返回1,失败返回-1 */
    signed char fs_network_bridge_del(/* 网桥名,最长15个字节 */const char brname[]);
    /* 向网桥添加接口,成功返回1,失败返回-1 */
    signed char fs_network_bridge_add_interface(/* 网桥名,最长15个字节 */const char bridge[], /* 接口名 */const char dev[]);
    /* 从网桥删除接口,成功返回1,失败返回-1 */
    signed char fs_network_bridge_del_interface(/* 网桥名,最长15个字节 */const char bridge[], /* 接口名 */const char dev[]);
    /* 获取所有的物理网口 */
    FsObjectList *fs_networkInterface_get__IO();
    /* 获取所有的网桥 */
    FsObjectList *fs_networkBridge_get__IO();
    /* 获取系统的所有ipv4地址 */
    FsArray *fs_network_ipv4_get_all__IO();
#ifdef FsDebug
    void fs_network_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* NETWORK_H */

