#include "encode.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Thread/monitor.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* 从配置获取通道数 */
static unsigned int encode_private_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

/* 删除pEncode_item指向的实例对象 */
static void encode_private_item_delete__OI(struct Encode_item * const pEncode_item) {
    /* 直正的解码对象 */
    frameConvert_item_delete__IO(pEncode_item->ro.__pFrameConvert_item);
    /* 文字叠加 */
    if (pEncode_item->ro.__pTypeFace)fs_TypeFace_delete__OI(pEncode_item->ro.__pTypeFace);

    free(pEncode_item);
}

static void encode_private_item_new(struct Encode * const pEncode, /* 通道号 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList,
        /* 原来的解码项 */struct Encode_item * * const ppEncode_item, /* 解码对象 */FrameConvert **ppFrameConvert) {
    FsConfig *pConfig = ((ConfigManager*) pEncode->ro._pConfigManager)->ro.__pConfig;
    void *parent;
    {
        FsObjectList *list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
        parent = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
        list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
        parent = list->pNode[list->startIndex + channel - 1];
        fs_ObjectList_delete__OI(list);
    }
    unsigned short moduleMask = fs_Config_node_integer_get_first(pConfig, parent, "moduleMask", 0)&0x2;

    parent = fs_Config_node_get_first(pConfig, parent, "watermarking");
    const unsigned short watermarking_startX = fs_Config_node_integer_get_first(pConfig, parent, "watermarking_startX", 0);
    const unsigned short watermarking_startY = fs_Config_node_integer_get_first(pConfig, parent, "watermarking_startY", 0);
    const unsigned short watermarking_width = fs_Config_node_integer_get_first(pConfig, parent, "watermarking_width", 32);
    const unsigned short watermarking_height = fs_Config_node_integer_get_first(pConfig, parent, "watermarking_height", 32);
    char* pstr = fs_Config_node_string_get_first__IO(pConfig, parent, "watermarking_color");
    unsigned int watermarking_color = strtol(pstr, NULL, 16);
    FsPrintf(1, "%#x-%s\n", watermarking_color, pstr);
    free(pstr);
    pstr = fs_Config_node_string_get_first__IO(pConfig, parent, "watermarkingData");
    ////////////////////////////////////////////////////////////////////////////
    if (*ppEncode_item)encode_private_item_delete__OI(*ppEncode_item);
    unsigned int len = 0;
    if (pstr)len += strlen(pstr) + 1;
    else moduleMask = 0;
    struct Encode_item* rst = (struct Encode_item*) malloc(sizeof (struct Encode_item) +len);
    memset(rst, 0, sizeof (struct Encode_item));
    char *pd = (char*) (rst + 1);
    /* 水印开始位置的X坐标 */
    rst->ro._watermarking_startX = watermarking_startX;
    /* 水印开始位置的Y坐标 */
    rst->ro._watermarking_startY = watermarking_startY;
    /* 水印颜色,RGB */
    rst->ro._watermarking_color = (((unsigned int) (0.257 * (watermarking_color >> 16) + 0.504 * ((watermarking_color >> 8)&0xFF) + 0.098 * (watermarking_color & 0xFF) + 16)) << 16)+
            (((unsigned int) (-0.148 * (watermarking_color >> 16) - 0.291 * ((watermarking_color >> 8)&0xFF) + 0.439 * (watermarking_color & 0xFF) + 128)) << 8) +
            0.439 * (watermarking_color >> 16) - 0.368 * ((watermarking_color >> 8)&0xFF) - 0.071 * (watermarking_color & 0xFF) + 128;
    /* 水印字符 */
    rst->ro._watermarkingData = pd, memcpy(pd, pstr, len);
    if (1) {
        if (NULL == *ppFrameConvert) {
            *ppFrameConvert = frameConvert_new__IO("encode.FrameConvert", pEncode->ro._pMonitor, systemThreadTid, pEncode->ro._externRunStatus, fs_system_cpu_thread_get());
            frameConvert_startThread(*ppFrameConvert, 0);
        }
        struct FrameConvert_item *pFrameConvert_item = frameConvert_item_new__OI(ImageFrame_H264_0, 4);
        /* 直正的解码对象 */
        rst->ro.__pFrameConvert_item = pFrameConvert_item;
        frameConvert_add_item__OI_2(*ppFrameConvert, pFrameConvert_item);
    }
    /* 文字叠加 */
    if (moduleMask)rst->ro.__pTypeFace = fs_TypeFace_new__IO(NULL, watermarking_width, watermarking_height, 1, 0);
    *ppEncode_item = rst;
    ////////////////////////////////////////////////////////////////////////////
    /* 释放内存空间 */
    if (pstr)free(pstr);
}

static void *encode_private_thread(struct Encode* pEncode) {
    /* 创建线程监控 */
    MonitorItem *pMonitorItem = monitorItem_new_and_add(pEncode->ro._pMonitor,
            pEncode->ro._threadName, NULL, pEncode, pEncode->ro._parentTid);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *externRunStatus = pEncode->ro._externRunStatus;
    ConfigManager *pConfigManager = (ConfigManager*) pEncode->ro._pConfigManager;
    FsObjectList *itemList = pEncode->ro.__itemList;
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
    struct Encode_item *pEncode_item;
    int i;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    pEncode->rw.resetChannel = -1;
    // 根据
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pEncode->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        usleep(sleepTime);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pEncode->rw.resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (ipList)free(ipList);
            ipList = fs_network_ipv4_get_all__IO();
            if (0 == pEncode->rw.resetChannel)channelCount = encode_private_get_channelCount(pConfigManager->ro.__pConfig, ipList);
            /* 删除通道 */
            while (itemList->nodeCount > channelCount) {
                pEncode_item = (struct Encode_item *) fs_ObjectList_remove_tail(itemList);
                if (pEncode_item)encode_private_item_delete__OI(pEncode_item);
            }
            /* 添加通道 */
            while (itemList->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList, NULL);
            /* 重置 */
            if (itemList->nodeCount < (unsigned int) pEncode->rw.resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pEncode->rw.resetChannel, itemList->nodeCount);
                fflush(stdout);
            } else if (pEncode->rw.resetChannel > 0) {
                pMonitorItem->rw.line = __LINE__;
                encode_private_item_new(pEncode, pEncode->rw.resetChannel, ipList, (struct Encode_item **) (itemList->pNode + itemList->startIndex + pEncode->rw.resetChannel - 1),
                        &pFrameConvert);
                pMonitorItem->rw.line = __LINE__;
            }
            pEncode->rw.resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 删除各通道 */
    {
        struct Encode_item **pNode = (struct Encode_item **) itemList->pNode + itemList->startIndex;
        for (i = itemList->nodeCount; i > 0; i--) {
            pEncode_item = *pNode++;
            if (pEncode_item)encode_private_item_delete__OI(pEncode_item);
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
    FsPrintf2(1, "Thread:\"%s\" exit,runStatus=%hhd.\n", pEncode->ro._threadName, pEncode->rw.runStatus);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pEncode->ro._pMonitor, pMonitorItem);
    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void encode_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "watermarking", "水印配置", "水印配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_and, "2");

    void * pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_startX", "水印开始位置的X坐标", "水印开始位置的X坐标", 0, 0x7, 0, 1800, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_startY", "水印开始位置的Y坐标", "水印开始位置的Y坐标", 0, 0x7, 0, 1000, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_width", "水印字符宽度", "水印字符宽度", 0, 0x7, 1, 128, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
    pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_height", "水印字符高度", "水印字符高度", 0, 0x7, 1, 128, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
    pNode = fs_Config_node_string_add(pConfig, parent, "watermarking_color", "水印颜色,RGB", "水印颜色,RGB,16进制", 0, 0x7, 1, 8, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0xFF0000", "红色", "红色");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "0x00FF00", "绿色", "绿色");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "0x0000FF", "蓝色", "蓝色");
    pNode = fs_Config_node_string_add(pConfig, parent, "watermarkingData", "水印数据", "水印数据", 0, 0x7, 1, 128, 1);
}

/* 
 * 创建一个新的Encode对象;
 * 返回Encode指针.
 */
struct Encode *encode_new__IO(/* 线程名 */const char threadName[],
        /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
        /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
        /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
        /* 配置管理对象 */ void *const pConfigManager) {
    unsigned int len = strlen(threadName) + 1;
    struct Encode* rst = (struct Encode*) malloc(sizeof (struct Encode) +len);
    memset(rst, 0, sizeof (struct Encode));
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

/* 删除pEncode指向的实例对象 */
void encode_delete__OI(struct Encode * const pEncode) {
    /* 通道链表 */
    fs_ObjectList_delete__OI(pEncode->ro.__itemList);

    free(pEncode);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void encode_startThread(struct Encode * const pEncode,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice) {
    if (pEncode->rw.runStatus == 0 && pEncode->ro.stid == 0) {
        pEncode->rw.runStatus = 1;
        pEncode->rw.nice = nice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pEncode->ro._threadName,
                (long) pEncode->ro._pMonitor,
                pEncode->ro._parentTid,
                (long) pEncode->ro._externRunStatus);
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
        if (pthread_create(&pEncode->ro.stid, &attr, (void* (*)(void*))encode_private_thread, pEncode)) {
            FsPrintf(1, "Create thread error:\"%s\".\n", strerror(errno));
            fflush(stdout);
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pEncode->rw.runStatus, pEncode->ro.stid,
                pEncode->ro._threadName,
                (long) pEncode->ro._pMonitor,
                pEncode->ro._parentTid,
                (long) pEncode->ro._externRunStatus);
        fflush(stdout);
    }
}

/* 结束线程 */
void encode_stopThread(struct Encode * const pEncode) {
    pEncode->rw.runStatus = 0;
    if (pEncode->ro.stid) {
#ifdef FsDebug
        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pEncode->ro.stid, 0);
        pEncode->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void encode_item_frame_in_pthreadSafety_OI(/* 编码项,可为空 */ struct Encode_item * const pEncode_item, FsObjectImageFrame * const pFrame) {
    if (pEncode_item->ro.__pTypeFace) {
        if (ImageFrame_valid_H264_0(pFrame->dataValidType))image_frame_clear_data(pFrame, ImageFrame_H264_0);
        fs_TypeFace_write(pEncode_item->ro.__pTypeFace, (unsigned char*) pEncode_item->ro._watermarkingData, pFrame->data.raw[0]->data, pFrame->width[0], pFrame->height[0],
                pFrame->width[0], pEncode_item->ro._watermarking_startX, pEncode_item->ro._watermarking_startY, pEncode_item->ro._watermarking_color >> 16);
        fs_TypeFace_write_uv(pEncode_item->ro.__pTypeFace, (unsigned char*) pEncode_item->ro._watermarkingData, pFrame->data.raw[0]->data + pFrame->sizeWidthHeight[0], pFrame->width[0], pFrame->height[0],
                pFrame->width[0] >> 1, pEncode_item->ro._watermarking_startX, pEncode_item->ro._watermarking_startY, (pEncode_item->ro._watermarking_color >> 8)&0xFF);
        fs_TypeFace_write_uv(pEncode_item->ro.__pTypeFace, (unsigned char*) pEncode_item->ro._watermarkingData, pFrame->data.raw[0]->data + pFrame->sizeWidthHeight[0]+(pFrame->sizeWidthHeight[0] >> 2), pFrame->width[0], pFrame->height[0],
                pFrame->width[0] >> 1, pEncode_item->ro._watermarking_startX, pEncode_item->ro._watermarking_startY, pEncode_item->ro._watermarking_color & 0xFF);
    }
    frameConvert_item_frame_in_pthreadSafety_OI(pEncode_item->ro.__pFrameConvert_item, pFrame);
}

/* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
FsObjectImageFrame *encode_item_frame_out_pthreadSafety_IO(/* 解码项,可为空 */struct Encode_item * const pEncode_item) {
    return frameConvert_item_frame_out_pthreadSafety_IO(pEncode_item->ro.__pFrameConvert_item);
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
