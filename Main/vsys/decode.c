#include "decode.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Thread/monitor.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* 从配置获取通道数 */
static unsigned int decode_private_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return 0;
    /* 通道数 */
    FsObjectList *list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
    if (NULL == list)return 0;
    void *parent = list->pNode[list->startIndex];
    fs_ObjectList_delete__OI(list);
    list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
    unsigned int rst = 0;
    if (list) {
        rst = list->nodeCount;
        fs_ObjectList_delete__OI(list);
    }
    return rst;
}

/* 删除pDecode_item指向的实例对象 */
static void decode_private_item_delete__OI(struct Decode_item * const pDecode_item) {
    /* 直正的解码对象 */
    frameConvert_item_delete__IO(pDecode_item->ro.__pFrameConvert_item);

    free(pDecode_item);
}

static void decode_private_item_new(struct Decode * const pDecode, /* 通道号 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList,
        /* 原来的解码项 */struct Decode_item * * const ppDecode_item, /* 解码对象 */FrameConvert **ppFrameConvert) {
    FsConfig *pConfig = ((ConfigManager*) pDecode->ro._pConfigManager)->ro.__pConfig;
    void *parent;
    {
        FsObjectList *list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
        parent = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
        list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
        parent = list->pNode[list->startIndex + channel - 1];
        fs_ObjectList_delete__OI(list);
    }
    ////////////////////////////////////////////////////////////////////////////
    if (*ppDecode_item)decode_private_item_delete__OI(*ppDecode_item);
    unsigned int len = 0;
    struct Decode_item* rst = (struct Decode_item*) malloc(sizeof (struct Decode_item) +len);
    memset(rst, 0, sizeof (struct Decode_item));
    //char *pd = (char*) (rst + 1);
    if (1) {
        if (*ppFrameConvert == NULL) {
            *ppFrameConvert = frameConvert_new__IO("decode.FrameConvert", pDecode->ro._pMonitor, systemThreadTid, pDecode->ro._externRunStatus, fs_system_cpu_thread_get() << 1);
            frameConvert_startThread(*ppFrameConvert, 0);
        }
        struct FrameConvert_item *pFrameConvert_item = frameConvert_item_new__OI(ImageFrame_RAW_0, 2);
        /* 直正的解码对象 */
        rst->ro.__pFrameConvert_item = pFrameConvert_item;

        frameConvert_add_item__OI_2(*ppFrameConvert, pFrameConvert_item);
    }
    *ppDecode_item = rst;
    ////////////////////////////////////////////////////////////////////////////
    /* 释放内存空间 */
}

static void *decode_private_thread(struct Decode* pDecode) {
    /* 创建线程监控 */
    MonitorItem *pMonitorItem = monitorItem_new_and_add(pDecode->ro._pMonitor,
            pDecode->ro._threadName, NULL, pDecode, pDecode->ro._parentTid);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *externRunStatus = pDecode->ro._externRunStatus;
    ConfigManager *pConfigManager = (ConfigManager*) pDecode->ro._pConfigManager;
    FsObjectList *itemList = pDecode->ro.__itemList;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    // 解码对象
    FrameConvert *pFrameConvert = NULL;
    unsigned int sleepTime = 0;
    FsArray *ipList = NULL;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    struct Decode_item *pDecode_item;
    int i;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    pDecode->rw.resetChannel = -1;
    // 根据
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pDecode->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        usleep(sleepTime);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pDecode->rw.resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (ipList)free(ipList);
            ipList = fs_network_ipv4_get_all__IO();
            if (0 == pDecode->rw.resetChannel)channelCount = decode_private_get_channelCount(pConfigManager->ro.__pConfig, ipList);
            /* 删除通道 */
            while (itemList->nodeCount > channelCount) {
                pDecode_item = (struct Decode_item *) fs_ObjectList_remove_tail(itemList);
                if (pDecode_item)decode_private_item_delete__OI(pDecode_item);
            }
            /* 添加通道 */
            while (itemList->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList, NULL);
            /* 重置 */
            if (itemList->nodeCount < (unsigned int) pDecode->rw.resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pDecode->rw.resetChannel, itemList->nodeCount);
                fflush(stdout);
            } else if (pDecode->rw.resetChannel > 0) {
                pMonitorItem->rw.line = __LINE__;
                decode_private_item_new(pDecode, pDecode->rw.resetChannel, ipList, (struct Decode_item **) (itemList->pNode + itemList->startIndex + pDecode->rw.resetChannel - 1),
                        &pFrameConvert);
                pMonitorItem->rw.line = __LINE__;
            }
            pDecode->rw.resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 删除各通道 */
    {
        struct Decode_item **pNode = (struct Decode_item **) itemList->pNode + itemList->startIndex;
        for (i = itemList->nodeCount; i > 0; i--) {
            pDecode_item = *pNode++;
            if (pDecode_item)decode_private_item_delete__OI(pDecode_item);
        }
        fs_ObjectList_clear(itemList);
    }
    /* 删除解码对象 */
    if (pFrameConvert) {
        frameConvert_stopThread(pFrameConvert);
        frameConvert_delete__IO(pFrameConvert);
    }
    /* 释放空间 */
    /* 释放全局变量空间 */
    if (ipList)free(ipList);
    /* 释放中间变量空间 */
    ////////////////////////////////////////////////////////////////////////////
    FsPrintf2(1, "Thread:\"%s\" exit,runStatus=%hhd.\n", pDecode->ro._threadName, pDecode->rw.runStatus);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pDecode->ro._pMonitor, pMonitorItem);
    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void decode_createConfig(FsConfig * const pConfig, void * const parent) {
}

/* 
 * 创建一个新的Decode对象;
 * 返回Decode指针.
 */
struct Decode *decode_new__IO(/* 线程名 */const char threadName[],
        /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
        /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
        /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
        /* 配置管理对象 */ void *const pConfigManager) {
    unsigned int len = strlen(threadName) + 1;
    struct Decode* rst = (struct Decode*) malloc(sizeof (struct Decode) +len);
    memset(rst, 0, sizeof (struct Decode));
    char *ps = (char*) (rst + 1);
    /* 线程名 */
    rst->ro._threadName = ps;
    memcpy(ps, threadName, len), ps += len;
    /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
    rst->ro._pMonitor = pMonitor;
    /* 父线程,创建此线程的线程地址 */
    rst->ro._parentTid = parentTid;
    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    if (externRunStatus) rst->ro._externRunStatus = externRunStatus;
    else rst->ro._externRunStatus = &defaultRunStatus;
    /* 配置 */
    rst->ro._pConfigManager = pConfigManager;
    /* 通道链表 */
    rst->ro.__itemList = fs_ObjectList_new__IO(4);

    return rst;
}

/* 删除pDecode指向的实例对象 */
void decode_delete__OI(struct Decode * const pDecode) {
    /* 通道链表 */
    fs_ObjectList_delete__OI(pDecode->ro.__itemList);

    free(pDecode);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void decode_startThread(struct Decode * const pDecode,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice) {
    if (pDecode->rw.runStatus == 0 && pDecode->ro.stid == 0) {
        pDecode->rw.runStatus = 1;
        pDecode->rw.nice = nice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pDecode->ro._threadName,
                (long) pDecode->ro._pMonitor,
                pDecode->ro._parentTid,
                (long) pDecode->ro._externRunStatus);
#endif
        pthread_attr_t attr;
        /* 初始化线程属性 */
        pthread_attr_init(&attr);
        /* 设置分离状态 */
        //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        /* 设置栈地址 */
        //pthread_attr_setstackaddr ( pthread_attr_t *attr, void *addr );
        /* 设置栈大小 */
        pthread_attr_setstacksize(&attr, 32 * 1024);
        /* 设置线程优先级 */
        if (nice > 0 && nice < 100) {
            /* 修改为实时分时调度策略 */
            pthread_attr_setschedpolicy(&attr, SCHED_RR);
            struct sched_param param;
            param.sched_priority = nice;
            pthread_attr_setschedparam(&attr, &param);
        } else if (0 == nice) {
            /* 继承父进程优先级 */
            pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        } else if (-1 == nice) {
            /* 修改为分时调度策略 */
            pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        } else {
            FsPrintf(5, "Invalid nice(=%hhd).\n", nice);
            fflush(stdout);
        }
        if (pthread_create(&pDecode->ro.stid, &attr, (void* (*)(void*))decode_private_thread, pDecode)) {
            FsPrintf(1, "Create thread error:\"%s\".\n", strerror(errno));
            fflush(stdout);
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pDecode->rw.runStatus, pDecode->ro.stid,
                pDecode->ro._threadName,
                (long) pDecode->ro._pMonitor,
                pDecode->ro._parentTid,
                (long) pDecode->ro._externRunStatus);
        fflush(stdout);
    }
}

/* 结束线程 */
void decode_stopThread(struct Decode * const pDecode) {
    pDecode->rw.runStatus = 0;
    if (pDecode->ro.stid) {
#ifdef FsDebug
        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pDecode->ro.stid, 0);
        pDecode->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void decode_item_frame_in_pthreadSafety_OI(/* 解码项 */ struct Decode_item * const pDecode_item, FsObjectImageFrame *pFrame) {
    frameConvert_item_frame_in_pthreadSafety_OI(pDecode_item->ro.__pFrameConvert_item, pFrame);
}

/* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
FsObjectImageFrame *decode_item_frame_out_pthreadSafety_IO(/* 解码项,链表可为空 */ struct Decode_item * const pDecode_item) {
    return frameConvert_item_frame_out_pthreadSafety_IO(pDecode_item->ro.__pFrameConvert_item);
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
