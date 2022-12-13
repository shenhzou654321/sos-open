/* 
 * File:   authorize.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#if !defined AUTHORIZE_H && !defined JAVA_IN_C

#define AUTHORIZE_H
#include "../PublicTools/publicDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop) 

    /* 端口协议的掩码定义 */
    typedef enum {
        /* 自定义8字节头 */
        FsPortProtocolMask_head8 = 1U << 0,
#define FsPortProtocolMask_head8 FsPortProtocolMask_head8
#define FsPortProtocolMask_head8_string_0 "自定义8字节头"
        /* 自定义的16字节头 */
        FsPortProtocolMask_head16 = 1U << 1,
#define FsPortProtocolMask_head16 FsPortProtocolMask_head16
#define FsPortProtocolMask_head16_string_1 "自定义的16字节头"  
        /* cgi接口 */
        FsPortProtocolMask_cgi = 1U << 2,
#define FsPortProtocolMask_cgi FsPortProtocolMask_cgi
#define FsPortProtocolMask_cgi_string_2 "CGI接口"
        /* SIP,GB28181中的sip */
        FsPortProtocolMask_sip = 1U << 3,
#define FsPortProtocolMask_sip FsPortProtocolMask_sip
#define FsPortProtocolMask_sip_string_3 "SIP"
        /* RTP,GB28181中的rtp */
        FsPortProtocolMask_GB28181_rtp = 1U << 4,
#define FsPortProtocolMask_GB28181_rtp FsPortProtocolMask_GB28181_rtp
#define FsPortProtocolMask_GB28181_rtp_string_4 "GB28181 RTP"
        /* kvpn,kvpn协议 */
        FsPortProtocolMask_kvpn = 1U << 5,
#define FsPortProtocolMask_kvpn FsPortProtocolMask_kvpn
#define FsPortProtocolMask_kvpn_string_5 "kvpn"
        /* ddns,动态dns */
        FsPortProtocolMask_ddns = 1U << 6,
#define FsPortProtocolMask_ddns FsPortProtocolMask_ddns
#define FsPortProtocolMask_ddns_string_6 "ddns"
    } FsPortProtocolMask;



    /* 
     * 编码16字节头,rst_16和head_16可能是同一地址;
     * 2字节异或校验.
     */

    void fs_package_head16_encode1(unsigned char rst_16[]);

#define fs_package_head16_encode(rst_16) fs_package_head16_encode1(rst_16)
    /* 解码16字节头,成功返回1,失败返回-1 */

    extern int (* fsPackageHeadDecode[32]) (unsigned char rst_16[]);



#ifdef __cplusplus
}
#endif

#endif /* AUTHORIZE_H */

