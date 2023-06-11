/* 
 * File:   Network.h
 * Author: fslib
 *
 * Created on 2013年1月10日, 上午9:51
 */
#if !defined NETWORK_H && !defined JAVA_IN_C

#include "../../PublicTools/publicDefine.h"
#define NETWORK_H

#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/String.h"
#include "../../PublicTools/Fs/StringBuilder.h"
#define FS_INVALID_IPV4 0xFFFFFFFFU
#define Fs_Network_Ipv4(a,b,c,d) (((unsigned int)(a))|(((unsigned int)(b))<<8)|(((unsigned int)(c))<<16)|(((unsigned int)(d))<<24))
#define Fs_Network_Ipv4_print(ipv4) ((unsigned char*)&(ipv4))[0],((unsigned char*)&(ipv4))[1],((unsigned char*)&(ipv4))[2],((unsigned char*)&(ipv4))[3]
#ifdef __cplusplus
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
     * 失败返回FS_INVALID_IPV4.  
     */

    unsigned int fs_ipv4_network_get(/* 可为0 */unsigned char ipv4BufferLenth, /* 可为空 */const char ipv4Buffer[]);

    /* 把字符串转换为FsIpv4 */

    void fs_ipv4_get_from_string(/* 把结果写到此结构口中,失败返回的成员中ipv4为0xFFFFFFFFU */ FsIpv4 * const rst, unsigned char bufferLenth, const char buffer[]);

    /* 从地址addr中解析域名,ip,端口信息,如bb.com:80或192.168.30.1:80,有域名返回域名指针,内存空间在addr内,否则返回空 */

    const char * fs_network_addr_get(unsigned short *const rst_domainLen, unsigned int *const rst_ipv4, unsigned short *const rst_ipv4Port, const char addr[]);

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

    int fs_network_ipv4_set_(/* 网卡名,如eth0:1 */const char networkName[], /* ipv4地址 */const unsigned int ipv4, /* 掩码,0-32 */const unsigned char mask
            , /* mtu值,为0表示不设置 */const unsigned short mtu);

    /* 设置网卡的ip地址,成功返回1,失败返回-1 */

    int fs_network_ipv4_set(/* 网卡名,如eth0:1 */const char networkName[], /* ipv4地址,如192.168.1.1/24,无掩码时掩码默认为32 */const char ipv4[]
            , /* mtu值,为0表示不设置 */const unsigned short mtu);

    /* 添加路由,成功返回1,已存在返回0,失败返回-1 */

    int fs_network_v4_route_add_(/* 目的地址 */const unsigned int dst, /* 掩码,0-32 */const unsigned char mask
            , /* 网关 */const unsigned int gateway, /* 网络接口名,为空表示自动查找并使用最合适的 */const char netinterface[]);

    /* 添加路由,成功返回1,已存在返回0,失败返回-1 */

    int fs_network_v4_route_add(/* 路由信息,如0.0.0.0/0 192.168.1.1,不加掩码时,掩码默认为32 */const char route[], /* 网络接口名,为空表示自动查找并使用最合适的 */const char netinterface[]);

    /* 根据src地址查找对应的接口,并添加到host的路由,成功返回1,路由已存在返回0,查找接口失败返回-1,其他错误返回-2 */

    int fs_network_v4_route_host_add(/* 目的主机ip */const unsigned int host, /* 源ip */const unsigned int src, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 根据当前路由表查找去host的路由,并添加到host的路由表,成功返回1,路由已存在返回0,查找失败返回-1,其他错误返回-2 */

    int fs_network_v4_route_host_direct(/* 目的主机ip */const unsigned int host, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加网桥,成功返回1,失败返回-1 */

    signed char fs_network_bridge_add(/* 网桥名,最长15个字节 */const char brname[]);

    /* 删除网桥,成功返回1,失败返回-1 */

    signed char fs_network_bridge_del(/* 网桥名,最长15个字节 */const char brname[]);

    /* 向网桥添加接口,成功返回1,失败返回-1 */

    signed char fs_network_bridge_add_interface(/* 网桥名,最长15个字节 */const char bridge[], /* 接口名 */const char dev[]);

    /* 从网桥删除接口,成功返回1,失败返回-1 */

    signed char fs_network_bridge_del_interface(/* 网桥名,最长15个字节 */const char bridge[], /* 接口名 */const char dev[]);

    /* 获取所有的物理网口 */

    FsObjectList *fs_networkInterface_get__IO(/* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取所有的网桥 */

    FsObjectList *fs_networkBridge_get__IO();

    /* 清除所有NAT数据 */

    void fs_network_nat_clean();


    /* 添加NAT源端口自动转换指令,成功返回1,失败返回-1 */

    int fs_network_snat_MASQUERADE(/* ipv4Src地址,如192.168.1.1/24或!192.168.1.1/24,无掩码时掩码默认为32 */const char ipv4Src[]
            , /* ipv4Dst地址,如192.168.1.1/24或!192.168.1.1/24,无掩码时掩码默认为32 */const char ipv4Dst[]);



    /* 清除所有filter数据 */

    void fs_network_filter_clean();

    /* 添加防火墙规则,成功返回1,失败返回-1 */

    int fs_network_filter_add(/* 链,如INPUT,FORWARD,OUTPUT */const char chain[], /* ipv4Src地址,如192.168.1.1/24或!192.168.1.1/24,无掩码时掩码默认为32,可为空 */const char ipv4Src[]
            , /* ipv4Dst地址,如192.168.1.1/24或!192.168.1.1/24,无掩码时掩码默认为32,可为空 */const char ipv4Dst[], /* 协议,如tcp,udp,!tcp,可为空 */const char protocol[]
            , /* 源端口,tcp,udp时有效,可以是范围如55:56,55-56,或55,可为空 */const char srcPort[], /* 目的端口,tcp,udp时有效,可以是范围如55:56,55-56,或55,可为空 */ const char dstPort[]
            , /* 动作,如DROP,ACCEPT,RETURN */const char action[]);


    /* 获取系统的所有ipv4地址 */

    FsArray *fs_network_ipv4_get_all__IO(/* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取系统的所有ipv4地址和id标识(id也使用ipv4的方式表示,如ip映射和vpn连接时需要设置) */

    FsArray *fs_network_ipv4_and_id_get_all__IO(/* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef CONFIGMANAGER_enable

#endif
#ifdef __cplusplus
}
#endif

#endif /* NETWORK_H */

