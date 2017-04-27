/* 
 * File:   Bd.h
 * Author: fslib
 *
 * Created on 2014年3月18日, 下午3:00
 */

#ifndef BD_H
#define	BD_H
#include "../publicDefine.h"
#ifdef	__cplusplus
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
#ifdef FsDebug
    void fs_Bd_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* BD_H */

