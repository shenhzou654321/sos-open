/* 
 * File:   Time.h
 * Author: fslib
 *
 * Created on 2013年11月1日, 下午4:57
 */

#ifndef TIME_H
#define	TIME_H
#include "../publicDefine.h"
#include <time.h>
#ifdef	__cplusplus
extern "C" {
#endif
    /* 获取当前系统时间(0时区) */
    double fs_time_GMT_get();
    /* 获取当前系统时区 */
    signed char fs_time_timezone_get();
    /* 获取CPU已运行周期数的低四位 */
    unsigned int fs_time_cpu_low_get();
    /* 获取CPU已运行周期数的高四位 */
    unsigned int fs_time_cpu_high_get();
    //    /* 获取系统运行时间使用clock_gettime函数 */
    //    double fs_time_uptime_clock_get();
    /* 获取系统运行时间 */
    double fs_time_uptime_get();
    /*
     * 获取系统当前时间(当前时区);
     */
    double fs_time_local_get();
    /* 
     * 从NTP服务器获取0时区的时间;
     * 成功返回0时区NTP时间;
     * 连接失败返回-1.0;
     * 发送失败返回-2.0;
     * 收数据超时返回-3.0;
     * 接收失败返回-4.0;
     * 收到的数据错误返回-5.0.
     */
    double fs_time_ntp_get(/* ipv4地址,高位在前 */const unsigned int ipv4, /* 端口 */const unsigned short ipv4Port);
    /* 
     * 从NTP服务器获取0时区的时间;
     * 成功返回0时区NTP时间;
     * 连接失败返回-1.0;
     * 发送失败返回-2.0;
     * 收数据超时返回-3.0;
     * 接收失败返回-4.0;
     * 收到的数据错误返回-5.0.
     */
    double fs_time_ntp_get_byString(/* ipv4地址 */const char ipv4[], /* 端口 */const unsigned short ipv4Port);
    /* 
     * 从NTP服务器获取0时区的时间;
     * 成功返回0时区NTP时间;
     * 连接失败返回-1.0;
     * 发送失败返回-2.0;
     * 收数据超时返回-3.0;
     * 接收失败返回-4.0;
     * 收到的数据错误返回-5.0.
     */
    double fs_time_ntp_get_byStringAll(/* ipv4地址,可能含端口号 */const char buffer[]);
    /* 
     * 从一个字符串中获取时间,时间格式必须是如"2014-05-01 12:00:00.000"或"2014-05-01/12:00:00.000"的格式,后面的可以省略,如"2014"表示"2014-01-01 00:00:00.000",此时间是指0时区的时间;
     * 成功返回从1970年1月1号经过的秒数,0时区的时间;
     * 失败返回-1.0.
     */
    double fs_time_get_string(const char string[]);
    /*
     * 设置系统GMT时间;
     * 成功返回1;
     * 失败返回-1. 
     */
    signed char fs_time_set_GMT(const double gmttime);
    /* 把时间转为本地时间,返回占用rst_24的空间大小 */
    unsigned char fs_time_to_string(/* 储存结果的空间,2000年这样的不小于24个字节 */char rst_24[], /* 0时区的时间 */const double time);
    /* 修正ti中的时间 */
    void fs_time_tm_corrected(struct tm * const ti);
#ifdef FsDebug
    void fs_time_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* TIME_H */

