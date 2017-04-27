/* 
 * File:   Server.h
 * Author: fslib
 *
 * Created on 2013年11月20日, 下午4:37
 */

#ifndef SERVER_H
#define	SERVER_H
#include  "../publicDefine.h"
#include "String.h"
#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,2)
#pragma pack(pop)

    /* 
     * 通过fsserver程序上传数据,一般用于配置文件的修改;
     * 成功返回1
     * 连接失败返回-1;
     * 发送失败返回-2; 
     * 收数据失败返回-3;
     * 结果错误返回-4.
     */
    signed char fs_server_uploadfile__OI_4(/* 服务器的地址,为0表示用默认值 */ unsigned int ipv4, /* 服务器的端口号,,为0表示用默认值 */ unsigned short ipv4Port,
            /* 要保存的文件名 */const char filename[], /* 要发送的数据 */ FsString *pString);
    /* 
     * 通过fsserver程序上传数据,一般用于配置文件的修改;
     * 成功返回1
     * 连接失败返回-1;
     * 发送失败返回-2; 
     * 收数据失败返回-3;
     * 结果错误返回-4.
     */
    signed char fs_server_uploadfile(/* 服务器的地址,为0表示用默认值 */ unsigned int ipv4, /* 服务器的端口号,,为0表示用默认值 */ unsigned short ipv4Port,
            /* 要保存的文件名 */const char filename[], /* 要保存的数据 */ const char data[], /* 要保存的数据长度 */ const unsigned int dataLenth);
    /* 
     * 通过fsserver程序上传数据,保存到硬盘;
     * 成功返回1
     * 连接失败返回-1;
     * 发送失败返回-2; 
     * 收数据失败返回-3;
     * 结果错误返回-4.
     */
    signed char fs_server_save_to_disk(/* 服务器的地址,为0表示用默认值 */ unsigned int ipv4, /* 服务器的端口号,,为0表示用默认值 */ unsigned short ipv4Port,
            /* 要保存的硬盘名,如//dev/sda */const char diskname[], /* 要保存的数据 */ const char data[], /* 要保存的数据长度 */ const unsigned int dataLenth);
#ifdef	__cplusplus
}
#endif

#endif	/* SERVER_H */

