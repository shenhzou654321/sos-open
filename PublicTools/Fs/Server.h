/* 
 * File:   Server.h
 * Author: fslib
 *
 * Created on 2013年11月20日, 下午4:37
 */

#if !defined SERVER_H && !defined JAVA_IN_C

#define SERVER_H
#include  "../publicDefine.h"
#include "String.h"
#include "Memery.h"
#include "Ebml.h"
#ifdef __cplusplus
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

    int fs_server_uploadfile__OI_4(/* 服务器的地址,为0表示用默认值 */ unsigned int ipv4, /* 服务器的端口号,,为0表示用默认值 */ unsigned short ipv4Port
            , /* 要保存的文件名 */const char filename[], /* 要发送的数据 */ FsString * pString);

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

    /* 分析内存块数据,成功返回分析得到到的配置,失败返回NULL */

    FsEbml * fs_server_configRead_from_data__IO(/* 在不为空时,把原始数据类型写入*rst_type中,1-xml,2-json */unsigned char *const rst_type, const int dataLen, const char data[]
            , /* 是否要执行数据修复操作,0-不执行,1-执行 */const char repair, /* 获取的ebml是否包含属性信息,0-不包含,1-包含 */const char getProperty
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 读取配置文件,成功返回读取到的配置,失败返回NULL */

    FsEbml * fs_server_configRead__IO(/* 在不为空时,把原始数据类型写入*rst_type中,1-xml,2-json */unsigned char *const rst_type, /* 配置文件名 */const char configFile[]
            , /* 是否要执行数据修复操作,0-不执行,1-执行 */const char repair, /* 获取的ebml是否包含属性信息,0-不包含,1-包含 */const char getProperty
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 更新root的密码,成功返回1,失败返回-1 */

    int fs_server_password_root_update(const char passwd[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 校验root的密码,成功返回1,失败返回-1,未设置返回0 */

    int fs_server_password_root_check(const char passwd[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 检查pEbml中是否有账户配置,没有导入原账户配置 */

    void fs_server_config_add_accountData(FsEbml * const pEbml);
  

    /* 从历史运行信息中获取线程倍频 */

    int fs_server_histroy_runInfo_get_threadMulti(/* 失败时返回的默认值 */int defaultValue
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 更新历史运行信息中的线程倍频,成功返回1,失败返回-1 */

    int fs_server_histroy_runInfo_update_threadMulti(/* 线程倍频 */const unsigned int threadMulti
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);






#ifdef __cplusplus
}
#endif

#endif /* SERVER_H */

