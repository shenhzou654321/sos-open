#ifndef _ERROR_H_
#define _ERROR_H_
#include "../publicDefine.h"
#ifdef	__cplusplus
extern "C" {
#endif
#pragma pack(push,2)
    /* 
     * 注册异常捕捉(只能在单线程中调用,可多次调用);
     * 成功返回1;
     * 已注册返回2,processName和errorFileName不会被设置,但externRunStatus会被重置;
     * 失败返回-1.
     */
    signed char exceptionRegister(/* 程序名,NULL则使用默认文件名 */const char processName[],
            /* 出现异常时间文件保存的位置,NULL则保存在当前程序目录下的error.log中 */ const char errorFileName[],
            /* 多线程共有的运行状态,如果此值不为空,设置此值为2可实现系统重启,设置为复数能实现系统退出,设置为其它正数也可能导致系统退出 */signed char *externRunStatus);
    /* 
     * 清除异常捕捉内部变量的空间;
     * 不调用也不会有太大的问题;
     * 调用必须在程序结束时调用,否则程序会出错.
     */
    void exceptionDelete();
    /* 添加一个Monitor到异常列表 */
    void exception_addItem(void *pMonitor);
    /* 从异常列表删除一个Monitor */
    void exception_deleteItem(void *pMonitor);
    /* 忽略信号signal */
    void exception_skip(unsigned char signalNO);
#pragma pack(pop)
#ifdef	__cplusplus
}
#endif
#endif
