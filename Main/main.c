/* 
 * File:   main.c
 * Author: fslib
 *
 * Created on 2013年3月7日, 下午4:00
 */
#include "main.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include "../PublicTools/publicDefine.h"
#include "../PublicTools/Thread/monitor.h"
#include "../PublicTools/Thread/error.h"
#include "vsys/vsys.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/*
 * 创建一个RootStruct实例;
 * 返回创建的实例指针. 
 */
RootStruct* rootStruct_new__IO() {
    RootStruct* rst = (RootStruct*) malloc(sizeof (RootStruct));
    memset(rst, 0, sizeof (RootStruct));

    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    rst->ro._externRunStatus = &defaultRunStatus;

    return rst;
}

/* 删除pRootStruct指向的实例对象  */
void rootStruct_delete__OI(RootStruct* pRootStruct) {
    free(pRootStruct);
}

void bayer2RGB(/* 宽度 */unsigned int width, /* 高度 */unsigned int height,
        /* RGB图的储存宽度 */unsigned int dstLineBits, /* 储存结果的数据指针 */unsigned char dstBuffer[],
        /* 原图的储存宽度 */unsigned int srcLineBits, /* 原图的数据指针 */unsigned char bayer[]) {

}

void Bayer2RGB_P(unsigned char* pBayer, unsigned char* pRGB, int nWidth, int nHeight) {
    // GRGRGRGR
    // BGBGBGBG
    // GRGRGRGR
    // BGBGBGBG
    // 第一行和最后一行不处理

    unsigned char* ps1 = (unsigned char*) (pBayer);
    unsigned char* ps2 = (unsigned char*) (pBayer + nWidth);
    unsigned char* ps3 = (unsigned char*) (pBayer + nWidth * 2);
    unsigned char* ps4 = (unsigned char*) (pBayer + nWidth * 3);

    unsigned char* pr1 = (unsigned char*) (pRGB + nWidth);
    unsigned char* pr2 = (unsigned char*) (pRGB + 2 * nWidth);
    unsigned char* pg1 = (unsigned char*) (pRGB + nWidth + nWidth * nHeight);
    unsigned char* pg2 = (unsigned char*) (pRGB + 2 * nWidth + nWidth * nHeight);
    unsigned char* pb1 = (unsigned char*) (pRGB + nWidth + nWidth * nHeight * 2);
    unsigned char* pb2 = (unsigned char*) (pRGB + 2 * nWidth + nWidth * nHeight * 2);

    unsigned long long a1, a2, a3, a4;
    unsigned long long r1, r2, r3, r4;
    unsigned long long g1, g2, g3, g4, g22, g33;
    unsigned long long b1, b2, b3, b4;

    unsigned long long m1 = 0x00ff00ff00ff00ff;
    unsigned long long m2 = 0xff00ff00ff00ff00;
    int j, i;
    for (j = 1; j < nHeight - 2; j += 2) {
        for (i = 0; i < nWidth; i += 6) {
            a1 = *(unsigned long long *) (ps1 + i);
            a2 = *(unsigned long long *) (ps2 + i);
            a3 = *(unsigned long long *) (ps3 + i);
            a4 = *(unsigned long long*) (ps4 + i);

            g1 = a1 & m1;
            g2 = a2 & m2;
            g3 = a3 & m1;
            g4 = a4 & m2;

            //			g22 = g2 >> 8;
            //			g22 = (((g22 + (g22 >> 16)) >> 1) & m1) << 16;
            //			g22 = (g22 + (((g1+g3)>>1) & m1) >> 1) & m1;
            //			g33 = g3;
            //			g33 = ((g33 + (g33 >> 16)) >> 1) & m1;
            //			g33 = ((g33 + ((((g2>>8)+(g4>>8))>>1) & m1))>> 1) & m1;
            //			*(unsigned long*)(pg1+i+1) = (g2 | g22) >> 8;
            //			*(unsigned long*)(pg2+i+1) = (g3 | (g33<<8)) >> 8;

            *(unsigned long long*) (pg1 + i + 1) = (g2 | (((g1 + g3) >> 1) & m1)) >> 8;
            *(unsigned long long*) (pg2 + i + 1) = (g3 | (((g2 + g4) >> 1) & m2)) >> 8;

            b2 = a2 & m1;
            b4 = a4 & m1;
            b3 = ((b2 + b4) >> 1) & m1;
            b2 = b2 | (((b2 + (b2 >> 16)) << 7) & m2);
            b3 = b3 | (((b3 + (b3 >> 16)) << 7) & m2);
            *(unsigned long long*) (pb1 + i + 1) = b2 >> 8;
            *(unsigned long long*) (pb2 + i + 1) = b3 >> 8;

            r1 = (a1 >> 8) & m1;
            r3 = (a3 >> 8) & m1;
            r2 = ((r1 + r3) >> 1) & m1;
            r2 = r2 | (((r2 + (r2 >> 16)) << 7) & m2);
            r3 = r3 | (((r3 + (r3 >> 16)) << 7) & m2);
            *(unsigned long long*) (pr1 + i + 1) = r2 >> 8;
            *(unsigned long long*) (pr2 + i + 1) = r3 >> 8;
        }

        ps1 += nWidth * 2;
        ps2 += nWidth * 2;
        ps3 += nWidth * 2;
        ps4 += nWidth * 2;

        pr1 += nWidth * 2;
        pr2 += nWidth * 2;
        pg1 += nWidth * 2;
        pg2 += nWidth * 2;
        pb1 += nWidth * 2;
        pb2 += nWidth * 2;
    }
}
#include "../PublicTools/Fs/Time.h"
#define FIBER_STACK 8192
int a;

int do_something(void *aa) {
    printf("This is son, the pid is:%d, the a is: %d\n", getpid(), ++a);
    //free(stack); //这里我也不清楚，如果这里不释放，不知道子线程死亡后，该内存是否会释放，知情者可以告诉下,谢谢
    exit(0);
}
#include "../PublicTools/Fs/Json.h"
#include "../PublicTools/Thread/ConfigManager.h"
#include "../PublicTools/Fs/Memery.h"
#include "../PublicTools/Fs/Bd.h"
#include "../PublicTools/Media/VideoMKV.h"
#include "../PublicTools/Fs/TypeFace.h"
#include "../Modules/camera_ctrl/camera_ctrl_hk_http.h"
#include "../PublicTools/Thread/Rtmp.h"

int main(int argc, char** argv) {
    ////////////////////////////////////////////////////////////////////////////
    /* 等待资源初始化完成 */
    // 网络资源
    while (access("/tmp/sysinfo/networkOK", F_OK))usleep(10000);
    // 硬盘资源
    while (access("/tmp/sysinfo/diskOK", F_OK))usleep(10000);
    // 激活码
    while (access("/tmp/sysinfo/licOK", F_OK))usleep(10000);
    ////////////////////////////////////////////////////////////////////////////
    /* 函数中的公共变量初始化 */
    globalInit(5 * 1024 * 1024, NULL, NULL, NULL, 0);
    /* 创建根对象 */
    RootStruct *pRootStruct = rootStruct_new__IO();
    pRootStruct->rw.runStatus = 1;
    ////////////////////////////////////////////////////////////////////////////
    /* 注册异常捕捉 */
    if (1) {
        exceptionRegister(NULL, NULL, (signed char*) &pRootStruct->rw.runStatus);
        exception_skip(SIGCLD);
        exception_skip(SIGPIPE);
    }
    /* 创建线程监控对象 */
    Monitor *pMonitor = monitor_new__IO("main.monitor", 0,
            (signed char*) &pRootStruct->rw.runStatus, "/tmp/running/", 1, 5.0, 0.2);
    /* 添加线程监控 */
    MonitorItem *pMonitorItem = monitorItem_new_and_add(pMonitor, "main", NULL, pRootStruct, 0);
    pMonitorItem->rw.line = __LINE__;
    /* 注销循环死锁监控 */
    pMonitorItem->rw.cycleAlive = -1;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    signed char restart = 1;
    ////////////////////////////////////////////////////////////////////////////
    pMonitorItem->rw.line = __LINE__;
    while (restart) {
        pRootStruct->rw.runStatus = 1;
        pMonitorItem->rw.line = __LINE__;
        ConfigManager *pConfigManager = configManager_new__IO("ConfigManager", pMonitor, systemThreadTid, (signed char*) &pRootStruct->rw.runStatus, "/fs/project/etc/config-v0.cfg", 0, 16863, "/tmp/sysinfo/licInfo");
        configManager_startThread(pConfigManager, 0);
        pMonitorItem->rw.line = __LINE__;
        struct Vsys *pVsys = vsys_new__IO("vsys", pMonitor, systemThreadTid, (signed char*) &pRootStruct->rw.runStatus, pConfigManager);
        vsys_startThread(pVsys, 1);
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        monitor_startThread(pMonitor, 0);
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        /* 检查是否重启 */
        if (pRootStruct->rw.runStatus != 2) {
            restart = 0;
        }
        pRootStruct->rw.runStatus = 255;
        ////////////////////////////////////////////////////////////////////////
        pMonitorItem->rw.line = __LINE__;
        vsys_stopThread(pVsys);
        vsys_delete__OI(pVsys);
        pMonitorItem->rw.line = __LINE__;
        configManager_stopThread(pConfigManager);
        configManager_delete__OI(pConfigManager);
        pMonitorItem->rw.line = __LINE__;
        monitor_stopThread(pMonitor);
        pMonitorItem->rw.line = __LINE__;
        // 网络资源
        while (restart && access("/tmp/sysinfo/networkOK", F_OK))usleep(10000);
        // 硬盘资源
        while (restart && access("/tmp/sysinfo/diskOK", F_OK))usleep(10000);
        // 激活码
        while (restart && access("/tmp/sysinfo/licOK", F_OK))usleep(10000);
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 停止线程监控 */
    monitor_stopThread(pMonitor);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem);
    /* 删除线程监控对象 */
    monitor_delete__OI(pMonitor);
    ////////////////////////////////////////////////////////////////////////////
    /* 删除根对象 */
    rootStruct_delete__OI(pRootStruct);
    /* 释放异常捕捉内部变量的空间 */
    exceptionDelete();
    /* 函数中的公共变量释放 */
    globalRelease();
    return EXIT_SUCCESS;
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
