#ifndef _ERROR_H_
#define _ERROR_H_
#include "../publicDefine.h"
#define ExceptionErrorFileName2_Default "/fs/project/data/syslog/core/error.log"
#ifdef __cplusplus
extern "C" {
#endif
    /* 
     * 注册异常捕捉(只能在单线程中调用,可多次调用);
     * 成功返回1;
     * 已注册返回2,processName和errorFileName不会被设置,但externRunStatus和externRunStatusExit会被重置;
     * 失败返回-1.
     */
    int exceptionRegister_pthreadSafety(/* 程序名,NULL则使用默认文件名 */const char processName[]
            , /* 出现异常时间文件保存的位置1,NULL则保存在当前程序目录下的error.log中 */ const char errorFileName1[]
            , /* 出现异常时间文件保存的位置2,为空不保存 */ const char errorFileName2[]
            , /* 多线程共有的运行状态,如果此值不为空,设置此值为2可实现系统重启,设置为负数能实现系统退出,设置为其它正数也可能导致系统退出 */signed char *const externRunStatus
            , /* 退出的运行标记,定义与externRunStatus相同,在有线程倍频时需要设置此值,externRunStatus不为空且此值不为空时会先设置externRunStatusExit再设置externRunStatus */signed char *const externRunStatusExit);
    /* 
     * 清除异常捕捉内部变量的空间;
     * 不调用也不会有太大的问题;
     * 调用必须在程序结束时调用,否则程序会出错.
     */
    void exceptionDelete();
    /* 忽略信号signal */
    void exception_skip(unsigned char signalNO);
    /* 为pMonitorItem触发了一堆栈打印,成功返回1,繁忙返回-128,错误返回-1 */
    int exception_callStackGet_pthreadSafety(void *const pMonitorItem);

#ifdef __cplusplus
}
#endif
#endif
