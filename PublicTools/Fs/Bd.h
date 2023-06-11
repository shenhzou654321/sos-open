/* 
 * File:   Bd.h
 * Author: fslib
 *
 * Created on 2014年3月18日, 下午3:00
 */

#if !defined BD_H && !defined JAVA_IN_C

#define BD_H
#include "../../PublicTools/publicDefine.h"
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct {
        /* 函数名 */
        const char *functionName;
        /* 对应的文件名,为空表示无法查找文件名 */
        const char *fileName;
        /* 行号,0表示无效 */
        unsigned int line;
    } FsBdInfo;


    /*
     * 获取程序指定地址的信息;
     * 成功返回FsBdInfo指针,用free释放内存;
     * 失败返回NULL. 
     */

    FsBdInfo * fs_Bd_get_info__IO(/* 程序名 */const char programName[], /* 地址,16进制 */const char address[]);


    /* 输出堆栈 */

    void fs_Bd_backtrace_out(FILE * const fd);


    /* 创建一个检查运行失败的对象,info1,info2,info3会拼接起来使用 */

    void * fs_Bd_runFailedCheck_new__IO(/* 等待时间(s) */const unsigned int waiteTime, /* 超时是否退出程序,0-不退出,1-退出 */const char timeoutWithExit, /* 是否打印提示信息 */const char printInfo
            ,/* 行号 */ const unsigned int line, /* 信息1,可为空 */const char info1[], /* 信息2,可为空 */const char info2[], /* 信息3,可为空 */const char info3[]);

    /* 删除一个检查运行失败的对象 */

    void fs_Bd_runFailedCheck_delete__OI(void *const p);


    void fs_Bd_test();

#ifdef __cplusplus
}
#endif

#endif /* BD_H */

