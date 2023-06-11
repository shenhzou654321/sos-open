/* 
 * File:   Log.h
 * Author: fslib
 *
 * Created on 2018年6月13日, 下午3:00
 */

#if !defined LOG_H && !defined JAVA_IN_C

#define LOG_H
#include "../../PublicTools/publicDefine.h"
#include <stdio.h>
#include "../../PublicTools/Fs/Ebml.h"
#include "../../PublicTools/Fs/Memery.h"
#ifdef __cplusplus
extern "C" {
#endif
#define __FsLog(___Printf,___loglevel,___index,___format, ...) do{ if((___loglevel)&loglevel)___Printf(((___index)<<28)|(___loglevel),___format,##__VA_ARGS__);}while(0)
#define __FsLogData(___PrintfData,___loglevel,___format, ...) do{ if((___loglevel)&loglevel)___PrintfData(___format,##__VA_ARGS__);}while(0)
#define FsLog(___loglevel,___index,___format, ...) __FsLog(FsPrintf,___loglevel,___index,___format,##__VA_ARGS__)
#define FsLogData(___loglevel,___format, ...) __FsLogData(FsPrintfData,___loglevel,___format,##__VA_ARGS__)
#define FsLog2(___loglevel,___index,___format, ...) __FsLog(FsPrintf2,___loglevel,___index,___format,##__VA_ARGS__)
#define FsLog2Data(___loglevel,___format, ...) __FsLogData(FsPrintf2Data,___loglevel,___format,##__VA_ARGS__)
#define FsLogTypeCheck(___type) ((___type)&loglevel)
#define FsLogExit() do{ if((FsLogType_fatalE)&loglevel) FsErrorDump();}while(0)
    /* 日志标记,格式为:FsLogTagB+标记名+空格+出现标记后还需要读取的行数+FsLogTagE */
#define FsLogTagB "\n###**#" // 日志标记中的开始标记
#define FsLogTagE "***##*\n" // 日志标记中的结束标记
#define FsLogTag(/* 标记后期望缓存的行数 */___nextLineCount) printf(FsLogTagB "%s[%d] " #___nextLineCount "" FsLogTagE , __FUNCTION__ , __LINE__)

    typedef enum {
        /* FATAL指出每个严重的错误事件将会导致应用程序的退出,不可容忍的错误 */
        FsLogType_fatal = 1U << 0,
#define FsLogType_fatal FsLogType_fatal
#define FsLogType_fatal_string_0 "0:fatal(严重的错误事件,程序可能崩溃)"
        /* 退出事件,一般是因为发生不可容忍的错误后强制退出 */
        FsLogType_fatalE = 1U << 1,
#define FsLogType_fatalE FsLogType_fatalE
#define FsLogType_fatalE_string_1 "1:fatalE(退出并打印堆栈标记)"
        /* ERROR指出虽然发生错误事件,但仍然不影响系统的继续运行,比如逻辑校验出错(删除不存在数据),可处理的磁盘读写错误 */
        FsLogType_error = 1U << 2,
#define FsLogType_error FsLogType_error
#define FsLogType_error_string_2 "2:error(错误事件,程序可继续运行)"
        /* 匹配错误的精简信息,如用户名,密码及协议中其它一些字段不匹配的问题 */
        FsLogType_matchErrorSimple = 1U << 3,
#define FsLogType_matchErrorSimple FsLogType_matchErrorSimple
#define FsLogType_matchErrorSimple_string_3 "3:matchErrorSimple(匹配错误的精简信息)"
        /* 匹配错误的详细信息,如用户名,密码及协议中其它一些字段不匹配的问题 */
        FsLogType_matchErrorInfo = 1U << 4,
#define FsLogType_matchErrorInfo FsLogType_matchErrorInfo
#define FsLogType_matchErrorInfo_string_4 "4:matchErrorInfo(匹配错误的详细信息)"
#define FsLogType_matchError (FsLogType_matchErrorSimple|FsLogType_matchErrorInfo)
        /* 匹配字段精简信息,如用户名,密码及协议的一些字段的打印 */
        FsLogType_matchSimple = 1U << 5,
#define FsLogType_matchSimple FsLogType_matchSimple
#define FsLogType_matchSimple_string_5 "5:matchSimple(匹配精简信息)"
        /* 匹配字段详细信息,如用户名,密码及协议的数据包打印 */
        FsLogType_matchInfo = 1U << 6,
#define FsLogType_matchInfo FsLogType_matchInfo
#define FsLogType_matchInfo_string_6 "6:matchInfo(匹配字段详细信息)"
#define FsLogType_match (FsLogType_matchSimple|FsLogType_matchInfo)  
        /* 数据包信息校验错误的精简信息,如数据包不能识别,丢包 */
        FsLogType_packageCheckErrorSimple = 1U << 7,
#define FsLogType_packageCheckErrorSimple FsLogType_packageCheckErrorSimple
#define FsLogType_packageCheckErrorSimple_string_7 "7:packageCheckErrorSimple(数据包信息校验错误的精简信息)"
        /* 数据包信息校验错误的详细信息,如数据包不能识别,丢包 */
        FsLogType_packageCheckErrorInfo = 1U << 8,
#define FsLogType_packageCheckErrorInfo FsLogType_packageCheckErrorInfo
#define FsLogType_packageCheckErrorInfo_string_8 "8:packageCheckErrorInfo(数据包信息校验错误的详细信息)"
#define FsLogType_packageCheckError (FsLogType_packageCheckErrorSimple|FsLogType_packageCheckErrorInfo)
        /* 数据包精简信息,收发数据包的提示 */
        FsLogType_packageSimple = 1U << 9,
#define FsLogType_packageSimple FsLogType_packageSimple
#define FsLogType_packageSimple_string_9 "9:packageSimple(数据包精简信息)"
        /* 数据包详细信息,数据包内容打印 */
        FsLogType_packageInfo = 1U << 10,
#define FsLogType_packageInfo FsLogType_packageInfo
#define FsLogType_packageInfo_string_10 "10:packageInfo(数据包详细信息)"
#define FsLogType_package (FsLogType_packageSimple|FsLogType_packageInfo)  
        /* 线程信息,如线程进入,退出信息 */
        FsLogType_threadDebug = 1U << 11,
#define FsLogType_threadDebug FsLogType_threadDebug
#define FsLogType_threadDebug_string_11 "11:threadDebug(线程信息)"
        /* 系统中断信息 */
        FsLogType_interruptDebug = 1U << 12,
#define FsLogType_interruptDebug FsLogType_interruptDebug
#define FsLogType_interruptDebug_string_12 "12:interruptDebug(系统中断)"
        /* 超时导致的错误 */
        FsLogType_timeoutError = 1U << 13,
#define FsLogType_timeoutError FsLogType_timeoutError
#define FsLogType_timeoutError_string_13 "13:timeoutError(超时导致的错误)"
        /* 不需要的数据简要提示,如出现不需要的字段,本机无法处理的资源 */
        FsLogType_noNeedSimple = 1U << 14,
#define FsLogType_noNeedSimple  FsLogType_noNeedSimple
#define FsLogType_noNeedSimple_string_14 "14:noNeedSimple(不需要的数据简要提示)"     
        /* 不需要的数据详细信息,如出现不需要的字段,本机无法处理的资源 */
        FsLogType_noNeedInfo = 1U << 15,
#define FsLogType_noNeedInfo  FsLogType_noNeedInfo
#define FsLogType_noNeedInfo_string_15 "15:noNeedInfo(不需要的数据详细信息)"   
#define FsLogType_noNeed (FsLogType_noNeedSimple|FsLogType_noNeedInfo)
        /* 平台错误,如平台不支持这一功能 */
        FsLogType_platformError = 1U << 16,
#define FsLogType_platformError  FsLogType_platformError
#define FsLogType_platformError_string_16 "16:platformError(平台错误)"  
        /* 忽略数据,数据被忽略 */
        FsLogType_skip = 1U << 17,
#define FsLogType_skip  FsLogType_skip
#define FsLogType_skip_string_17 "17:skip(忽略数据)"  
        /* 正常联调信息,打印一些过程信息,用于联调,正常流程打印 */
        FsLogType_info = 1U << 18,
#define FsLogType_info  FsLogType_info
#define FsLogType_info_string_18 "18:info(正常联调信息)"   
        /* 无规则信息,特指一些随意打印的信息,正式代码中不应包含此信息 */
        FsLogType_ruleless = 1U << 19,
#define FsLogType_ruleless  FsLogType_ruleless
#define FsLogType_ruleless_string_19 "19:ruleless(无规则信息)"   
    } FsLogType;

    /* 添加一个模块日志级别,以便外部可修改此日志级别 */

    void fs_log_addLevel(/* 指向模块自定义的日志级别 */unsigned int *const pLogLevel, /* 模块名,不能重复 */ const char moduleName[], /* 共享buffer, 可为空 */ FsShareBuffer * const pShareBuffer);

    /* 打印日志级别信息 */

    void fs_log_level_out_pthreadSafety(FILE * const fd);

    /* 把日志级别转ebml输出,成功返回FsEbml对象指针,失败返回NULL */

    FsEbml * fs_log_level_to_ebml_pthreadSafety__IO(/* 共享buffer, 可为空 */ FsShareBuffer * const pShareBuffer);

    /* 设置日志级别,成功返回1,失败返回-1 */

    int fs_log_level_set_pthreadSafety(FsEbml * const pEbml, /* 是否保存到文件,0-不保存,重启后失效,1-保存 */const char save
            , /* 共享buffer, 可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif


#endif /* LOG_H */

