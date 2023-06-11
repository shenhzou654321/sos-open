/* 
 * File:   Shell.h
 * Author: RuiXue
 *
 * Created on 2013年3月14日, 下午7:22
 */

#ifndef SHELL_H
#define	SHELL_H
#include "Network.h"
#include "StringList.h"

#ifdef	__cplusplus
extern "C" {
#endif

    /* Telnet连接对象 */
    typedef struct {
        /* 连接套接字 */
        int sock;
        /* 命令行头长度,包括特殊字符 */
        unsigned short headLenth;
        /* 命令行头内容 */
        FsString *head;
    } FsTelnet;
    /* 执行commond命令;
     * 成功返回C5aiiStringList对象;
     * 失败返回NULL.
     */
    FsStringList* c5aii_shell__IO(const char commond[]);
    /* 执行commond命令;
     * 成功返回第一条数据;
     * 失败返回NULL.
     */
    char* c5aii_shell_first__IO(const char commond[]);
    /* 
     * 创建一个Telnet的连接对象;
     * 成功返回FsTelnet指针;
     * 失败返回NULL.
     */
    FsTelnet * fs_shell_telnet_new__IO(/* 服务器IPv4地址 */const unsigned int ipv4, /* 服务器IPv4端口号,建议为23 */ const unsigned short ipv4Port
            , /* 用户名,不能为空 */ const char username[], /* 密码,可为空 */ const char password[], /* 连接尝试次数,0和1都会尝试一次,失败等待1S再试 */int connectTryCount);
    /* 删除pTelnet指向的实例对象 */
    void fs_shell_telnet_delete__OI(FsTelnet *pTelnet);
    /* 
     * 执行telnet命令;
     * 成功返回1,如果终端有打印信息,则把打印信息写到*rst中,没有打印信息,*rst为空;
     * Select发送失败返回-1;
     * 发送数据失败返回-2;
     * Select接收失败返回-3;
     * 接收数据失败返回-4.
     */
    signed char c5aii_shell_telnet__IO_1(/* 保存终端打印信息,不需要保存终端数据请传入NULL */FsString **rst, /* 为c5aii_shell_telnet_new__IO的返回值 */FsTelnet *pTelnet,
            /* 要执行的命令,长度不能为0,必须以\r\n结尾 */const char cmd[]);
    /* 
     * telnet去运行一程序直到程序退出为止;
     * 成功返回1,如果终端有打印信息,则把打印信息写到*rst中,没有打印信息,*rst为空;
     * Select发送失败返回-1;
     * 发送数据失败返回-2;
     * Select接收失败返回-3;
     * 接收数据失败返回-4;
     * 认证失败返回-5.
     */
    signed char fs_shell_telnet_runPrograme(/* 服务器IPv4地址 */const unsigned int ipv4, /* 服务器IPv4端口号,建议为23 */ const unsigned short ipv4Port
            , /* 用户名,不能为空 */ const char username[], /* 密码,可为空 */ const char password[], /* 要执行的命令,长度不能为0,必须以\r\n结尾 */ const char cmd[]
            , /* 连接尝试次数,0和1都会尝试一次,失败等待1S再试 */const int connectTryCount);
    /* 
     * 获取shell的端口号;
     * 成功返回端口号;
     * 失败则继续获取直到成功为止.
     */
    unsigned short fs_shell_port_get();
    /* 
     * 运行shell命令;
     * 成功返回system的返回值;
     * 失败返回-1;
     * 其它都表示失败.
     */
    int fs_shell_system(/* 要执行的命令,为空表示让子进程退出 */const char cmd[]);
    /* 
     * 运行子程序;
     * 成功返回子程序的进程号;
     * 失败返回负数;
     */
    int fs_shell_runProgame(/* 要执行的命令,为空表示让子进程退出 */const char cmd[], /* 0-表示用1作父进程,1-表示以子进程运行,为此值时本进程退出时子进程会强制退出,其它值会死机 */const char child);
    /* 
     * 运行程序并设置程序的缓冲类型;
     * 成功后此函数无法返回;
     * 运行失败返回-1.
     */
    int fs_execvp_io_set(/* 程序路径 */const char path[], /* 程序参数 */char * const argv[]
            , /* 标准输入输出及错误是否允许重定向,(stdRedirect&0x1)==0x1表示不允许输入重定向,(stdRedirect&0x2)==0x2表示不允许输出重定向,(stdRedirect&0x4)==0x4表示不允许错误重定向 */ const unsigned char stdRedirect
            , /* 标准输入的模式,-1-不设置,0-全缓冲,1-行缓冲 */const signed char stdin_type, /* 标准输入的缓冲区大小,仅为全缓冲时有效 */const unsigned int stdin_bufferSize
            , /* 标准输出的模式,-1-不设置,0-全缓冲,1-行缓冲,2-无缓冲 */const signed char stdout_type, /* 标准输出的缓冲区大小,仅为全缓冲时有效 */const unsigned int stdout_bufferSize
            , /* 标准错误的模式,-1-不设置,0-全缓冲,1-行缓冲,2-无缓冲 */const signed char stderr_type, /* 标准错误的缓冲区大小,仅为全缓冲时有效 */const unsigned int stderr_bufferSize
            , /* 设置其他环境变量,为空表示无,不为空时,数组的最后一个元素应为空 */ char * const env[]);
#ifdef	__cplusplus
}
#endif

#endif	/* SHELL_H */

