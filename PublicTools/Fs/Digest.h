/* 
 * File:   Digest.h
 * Author: fslib
 *
 * Created on 2020年12月24日, 下午5:03
 */
#if !defined DIGEST_H && !defined JAVA_IN_C

#define DIGEST_H
#include "../../PublicTools/publicDefine.h"
#include "Memery.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

#pragma pack(pop)



    /* 
     * 计算摘要;
     * 成功返回1,结果存于rst_16中;
     * 失败返回-1.
     */

    int fs_digest_cal(char rst_32[], /* 用户名 */const char username_cin[], /* 用户名长度 */const unsigned int usernameLen, /* 密码 */const char password_cin[], /* 密码长度 */ const unsigned int passwordLen
            , /* uri地址 */const char uri_cin[], /* uri长度 */ const unsigned int uriLen, /* 方式,如GET,POST */const char method_cin[], /* 方法长度 */ const unsigned int methodLen
            , /* body数据内容,可为空,地址如不是4字节对齐内部需要进行拷贝 */const char body_cin[], /* body数据的长度,可为0 */const unsigned int bodyLen
            , /* 如0000001 */const char nc_cin[], /* 客户端的随机数 */const char cnonce_cin[]
            , /* Digest数据,如 realm="Login to CL0081PAZ00024",qop="auth",nonce="806656111",opaque="ecd96bfa32ed521cda6e9a8ed1701e6b3ef687d0" */const char digest[]
            , /* body数据的长度,可为0 */ unsigned int digestLen
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取http中401摘要数据,结果类似 realm="Login to CL0081PAZ00024",qop="auth",nonce="806656111",opaque="ecd96bfa32ed521cda6e9a8ed1701e6b3ef687d0";
     * 成功返回1;
     * 失败返回-1.
     */

    int fs_digest_401data_get_from_http(/* *rst指向401中的摘要数据 */const char * * const rst, /* *rst指向401中的摘要数据长度,不包含\r\n */unsigned int *const rst_Len
            , /* 401 http数据 */ const char data[], /* 401 http数据长度 */unsigned int dataLen);




#ifdef __cplusplus
}
#endif

#endif /* DIGEST_H */

