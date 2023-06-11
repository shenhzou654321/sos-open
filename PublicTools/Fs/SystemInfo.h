/* 
 * File:   SystemInfo.h
 * Author: fslib.org
 *
 * Created on 2013年5月12日, 下午4:09
 */

#if !defined SYSTEMINFO_H && !defined JAVA_IN_C

#define SYSTEMINFO_H
#include "../../PublicTools/publicDefine.h"

#include <unistd.h>
#include "../../PublicTools/Fs/Memery.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 
     * 获取CPU的名字;
     * name:保存CPU名字的缓存空间,大小不小于49个字节;
     * 成功返回获得的name的字节数;
     * 失败返回-1.
     */

    int fs_cpu_name_get__(char name_49[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取CPU的Vendor;
     * 成功返回vendor的字节数;
     * 失败返回-1.
     */

    int fs_cpu_vendor_get__(/* 保存CPU Vendor的缓存空间,大小不小于13个字节 */char vendor_13[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 
     * 获取CPU的主频;
     * 成功返回CPU的主频;
     * 失败返回0.
     */

    unsigned long long fs_cpu_frequency_get();

    /*
     * 获取当前系统的时区;
     */

    int fs_timeZone_get();


    /* 获取CPU上电后的大概时间,只增不减,只能用于相对计时,单位S */

    unsigned int c5aii_time_cpuTime_get();

    /* 
     * 获取CPU已运行周期数;
     * 此函数只支持n为64bit的系统.
     */

    void fs_time_cpu_get(unsigned long *n);

    /* 
     * 线程延时,延时大于usleepTime时间,不精确;
     * usleepTime:延时的时间,单位微秒.
     */

    void fs_thread_usleep(const unsigned long usleepTime);

    /* 线程延时,延时到toUpTime,单位秒,有睡眠返回1,无睡眠返回-1 */

    signed char fs_thread_sleepTo(double toUpTime);

    /* 获取系统cpu支持的线程数,返回0表示错误 */

    unsigned short fs_system_cpu_thread_get();

    /* 获取内存信息,成功返回1,失败返回-1 */

    int fs_system_memory_get(/* 获取当前内存大小,单位字节,为空不获取此数据 */unsigned long long *const pMemorySize
            , /* 获取支持的最大内存大小,单位字节,为空不获取此数据 */unsigned long long *const pMemorySizeMax, /* 内存插槽数,为空不获取此数据 */unsigned int *const pCount
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取系统虚拟内存大小,返回0表示错误 */

    unsigned long long fs_system_vmemery_get(/* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 从memoryinfo文件获取内存信息,成功返回1,失败返回-1 */

    int fs_system_memoryinfo_get(/* 获取当前内存大小,单位字节,为空不获取此数据 */unsigned long long *const pMemorySize, /* 获取还剩余的内存大小,单位字节,为空不获取此数据 */unsigned long long *const pMemorySizeFree
            , /* 获取当前内存用于Buffer的大小,单位字节,为空不获取此数据 */unsigned long long *const pMemoryBufferSize, /* 获取当前内存用于Cached的大小,单位字节,为空不获取此数据 */unsigned long long *const pMemoryCachedSize
            , /* 获取当前虚拟内存大小,单位字节,为空不获取此数据 */unsigned long long *const pSwapMemorySize, /* 获取还剩余的虚拟内存内存大小,单位字节,为空不获取此数据 */unsigned long long *const pSwapMemorySizeFree
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取pid进程的内存使用量,成功返回1,失败返回-1 */

    int fs_system_process_memery_get(const pid_t pid, /* 获取当前内存大小,单位字节,为空不获取此数据 */unsigned long long *const pMemorySize, /* 虚拟地址空间大小,单位字节,为空不获取此数据 */unsigned long long *const pVmemorySizeFree
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 检查当前文件系统的dir相关函数是否支持文件类型,支持返回1,否则返回-1 */

    signed char fs_fileSystem_parameter_get_check();

    /* 获取鼠标信息,1表示成功,-1表示错误 */

    signed char fs_system_mouse_status_get(/* 位置x,为空表示不获取 */unsigned int *const x, /* 位置y,为空表示不获取 */unsigned int *const y,
            /* 鼠标左键的状态,1表示按下,0表示未按下,-1表示失败,为空表示不获取 */signed char *const status_Left, /* 鼠标右键的状态,1表示按下,0表示未按下,-1表示失败,为空表示不获取 */signed char *const status_Right);

    /* 获取鼠标信息,1表示成功,-1表示错误 */

    signed char fs_system_mouse_status_relative_get(/* 相对位置x,为空表示不获取 */ int *const x, /* 相对位置y,为空表示不获取 */ int *const y,
            /* 鼠标左键的状态,1表示按下,0表示未按下,-1表示失败,为空表示不获取 */signed char *const status_Left, /* 鼠标右键的状态,1表示按下,0表示未按下,-1表示失败,为空表示不获取 */signed char *const status_Right);




#ifdef __cplusplus
}
#endif

#endif /* SYSTEMINFO_H */

