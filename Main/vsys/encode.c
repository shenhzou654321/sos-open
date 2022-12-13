#include "encode.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Thread/monitor.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义外部变量与函数开始/////////////////////////////////
#ifdef FsPrintfIndex
#undef FsPrintfIndex
#endif 
#define FsPrintfIndex 1
///////////////////////////定义外部变量与函数结束/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义全局变量开始//////////////////////////////////////
///////////////////////////定义全局变量结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////
/* 本模块使用独立日志级别 */
#if 0
static unsigned int moduleLoglevel = FsNumberMax(loglevel); // 本模块使用自定义的loglevel,最大值表示未加到日志级别管理模块中
#define loglevel moduleLoglevel
#endif
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* 在内部进行了转换后,会调用此回调函数,可为空 */
static void encode_P_cb_convert(struct Encode_item * const pEncode_item) {
    Fs_GroupSqrtOut_value_set(pEncode_item->rw._pGroupSqrtOut, &pEncode_item->ro._pEncode->ro._pGroupSqrtOut->groupSqrt_mutex);
}

/* 从配置获取通道数 */
static unsigned int encode_private_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return 0;
    /* 通道数 */
    const void* parent0 = pConfig;
    FsObjectList *list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, ipList, 0, "vsys");
    if (NULL == list)return 0;
    const void *parent = list->pNode[list->startIndex];
    fs_ObjectList_delete__OI(list);
    list = fs_Config_node_template__IO(pConfig, &parent0, parent, NULL, 0, "vsysChannel");
    unsigned int rst = 0;
    if (list) {
        rst = list->nodeCount;
        fs_ObjectList_delete__OI(list);
    }
    return rst;
}

/* 删除pEncode_item指向的实例对象 */
static void encode_P_item_delete__OI(struct Encode_item * const pEncode_item) {
    /* 直正的解码对象 */
    frameConvert_item_delete__OI(pEncode_item->ro.__pFrameConvert_item);
    /* 文字叠加 */
    if (pEncode_item->ro.__pTypeFace)fs_TypeFace_delete__OI(pEncode_item->ro.__pTypeFace);

    fsFree(pEncode_item);
}

static void encode_P_item_new(struct Encode * const pEncode, /* 通道号 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的解码项 */struct Encode_item * * const ppEncode_item, /* 解码对象 */FrameConvert **ppFrameConvert, MonitorItem * const pMonitorItem) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pEncode->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, ipList, 0 == channel, "vsys");
    if (clusterList) {
        void **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int uj = clusterList->nodeCount;
        do {
            const void *vsysChannel0 = vsys0;
            const void* const vsys = *ppNodeCluster++;
            FsObjectList * const list = 0 == channel ? fs_Config_node_template_orderFirst__IO(pConfig, &vsysChannel0, vsys, NULL, 0, "vsysChannel")
                    : (FsObjectList *) fs_Config_node_template_get_orderFirst(pConfig, &vsysChannel0, vsys, NULL, 0, "vsysChannel", channel - 1);
            if (list) {
                void ** ppNode;
                unsigned int ui, ipv4;
                if (0 == channel) {
                    ppNode = list->pNode + list->startIndex;
                    ui = list->nodeCount;
                } else ui = 1;
                if (0 == channel) {
                    //                    if (NULL == pRecord->p.clusterItemList) pRecord->p.clusterItemList = fs_ObjectList_new__IO(4);
                    //                    while (pRecord->p.clusterItemList->nodeCount < index + j)fs_ObjectList_insert_tail(pRecord->p.clusterItemList, NULL);
                    //                    ppNodeRecord_item = (struct Record_item**) pRecord->p.clusterItemList->pNode + pRecord->p.clusterItemList->startIndex;
                    //                    uuid = fs_Config_node_string_get_first__IO(pConfig, parent, "hostname");
                    //                    pd = strchr(uuid, ':');
                    //                    ipv4 = fs_ipv4_network_get(pd ? pd - uuid : strlen(uuid), uuid);
                    //                    fsFree(uuid);
                } else ipv4 = 0;
                do {
                    void* vsysChannel, *sumNode;
                    if (0 == channel) {
                        //                        sumNode = vsysChannel = *ppNode++;
                        //                        if (ppNodeCapture_item[index] != NULL && ppNodeCapture_item[index]->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode)) {
                        //                            index++;
                        //                            continue;
                        //                        }
                    } else {
                        sumNode = vsysChannel = list;
                        if (*ppEncode_item != NULL && (*ppEncode_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppEncode_item) {
                        encode_P_item_delete__OI(*ppEncode_item);
                    }
                    struct Encode_item* rst;
                    if ((Encode_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0 && Encode_Mask != 0) rst = NULL;
                    else {
                        char *pd;
                        //const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = 0; //uuid->lenth;
                        if (channel > 0) {
                            unsigned short moduleMask = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)&0x2;
                            const void *watermarking0 = vsysChannel0;
                            const void *const watermarking = fs_Config_node_get_first(pConfig, &watermarking0, vsysChannel, "watermarking");
                            const FsString * const watermarkingData = moduleMask != 0 ? fs_Config_node_string_get_first_String(pConfig, watermarking0, watermarking, "watermarkingData", NULL) : NULL;
                            if (watermarkingData)len += watermarkingData->lenth;
                            else moduleMask = 0;

                            rst = (struct Encode_item*) fsMalloc(sizeof (struct Encode_item) +len);
                            memset(rst, 0, sizeof (struct Encode_item));
                            pd = ((char*) rst) + Memery_Alignment(sizeof (struct Encode_item));
                            /* 水印开始位置的X坐标 */
                            rst->ro._watermarking_startX = moduleMask != 0 ? fs_Config_node_integer_get_first(pConfig, watermarking0, watermarking, "watermarking_startX", 0, NULL) : 0;
                            /* 水印开始位置的Y坐标 */
                            rst->ro._watermarking_startY = moduleMask != 0 ? fs_Config_node_integer_get_first(pConfig, watermarking0, watermarking, "watermarking_startY", 0, NULL) : 0;
                            /* 水印颜色,RGB */
                            if (moduleMask) {
                                unsigned int watermarking_color = fs_Config_node_integer_get_first(pConfig, watermarking0, watermarking, "watermarking_color", 0, NULL);
                                rst->ro._watermarking_color = (((unsigned int) (0.257 * (watermarking_color >> 16) + 0.504 * ((watermarking_color >> 8)&0xFF) + 0.098 * (watermarking_color & 0xFF) + 16)) << 16)+
                                        (((unsigned int) (-0.148 * (watermarking_color >> 16) - 0.291 * ((watermarking_color >> 8)&0xFF) + 0.439 * (watermarking_color & 0xFF) + 128)) << 8) +
                                        0.439 * (watermarking_color >> 16) - 0.368 * ((watermarking_color >> 8)&0xFF) - 0.071 * (watermarking_color & 0xFF) + 128;
                            } else rst->ro._watermarking_color = 0;
                            /* 水印字符 */
                            if (moduleMask) rst->ro._watermarkingData = pd, memcpy(pd, watermarkingData->buffer, watermarkingData->lenth);
                            /* 文字叠加 */
                            if (moduleMask) {
                                const unsigned short watermarking_width = fs_Config_node_integer_get_first(pConfig, watermarking0, watermarking, "watermarking_width", 32, NULL);
                                const unsigned short watermarking_height = fs_Config_node_integer_get_first(pConfig, watermarking0, watermarking, "watermarking_height", 32, NULL);
                                rst->ro.__pTypeFace = fs_TypeFace_new__IO(NULL, watermarking_width, watermarking_height, 1, 0);
                            }
                        } else {
                            //                        rst = (struct Record_item*) fsMalloc(Record_item_ClusterLen + len);
                            //                        memset(rst, 0, Record_item_ClusterLen);
                            //                        pd = ((char*) rst) + Record_item_ClusterLen;
                        }
                        if (channel > 0) {
                            /* 整个通道依赖的所有数据的校验和 */
                            rst->ro._sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode);
                            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pEncode->ro._pGroupSqrtOut->groupSqrt_member, pEncode->ro._pGroupSqrtOut->groupSqrt_group
                                    , pEncode->ro._pGroupSqrtOut->groupValue, channel - 1, &pEncode->ro._pGroupSqrtOut->groupSqrt_mutex);
                            /* 所属的编码对象 */
                            rst->ro._pEncode = pEncode;
                        }
                        if (channel > 0) {
                            {
                                {
                                    FsObjectList * const videoCodingFormatList = fs_Config_node_integer_get_list__IO(pConfig, vsysChannel0, vsysChannel, "recordConfig videoCodingFormat", NULL);
                                    //                                    FsPrintf(1, "videoCodingFormatList=%p/%lu.\n", videoCodingFormatList, videoCodingFormatList ? videoCodingFormatList->nodeCount : 0);
                                    //                                    fflush(stdout);
                                    if (videoCodingFormatList) {
                                        const FsString **ppNode_videoCodingFormatList = (const FsString **) videoCodingFormatList->pNode + videoCodingFormatList->startIndex;
                                        unsigned int ui = videoCodingFormatList->nodeCount - 1;
                                        unsigned long long videoCodingFormatFirst = *(unsigned long long*) (*ppNode_videoCodingFormatList++)->buffer;
                                        unsigned long long videoCodingFormat = videoCodingFormatFirst;
                                        while (ui-- > 0) {
                                            const unsigned long long v = *(unsigned long long*) (*ppNode_videoCodingFormatList++)->buffer;
                                            if (0 == videoCodingFormatFirst)videoCodingFormatFirst = v;
                                            videoCodingFormat |= v;
                                        }
                                        fs_ObjectList_delete__OI(videoCodingFormatList);
                                        /* 期望的编码方式的第一个值,在videoCodingFormat标识的数据均无效时编码的数据类型,为0表示自动选择,&(0x1<<0)-大图H264,&(0x1<<1)-大图H265,&(0x1<<10<<0)-中图H264,&(0x1<<10<<1)-中图H265,&(0x1<<20<<0)-小图H264,&(0x1<<20<<1)-小图H265 */
                                        rst->ro._videoCodingFormatFirst = videoCodingFormatFirst;
                                        /* 期望的编码方式,&(0x1<<0)-大图H264,&(0x1<<1)-大图H265,&(0x1<<10<<0)-中图H264,&(0x1<<10<<1)-中图H265,&(0x1<<20<<0)-小图H264,&(0x1<<20<<1)-小图H265 */
                                        rst->ro._videoCodingFormat = videoCodingFormat != 0 ? videoCodingFormat : (0x1 << 0) | (0x1 << 1);
                                    } else {
                                        /* 期望的编码方式的第一个值,在videoCodingFormat标识的数据均无效时编码的数据类型,为0表示自动选择,&(0x1<<0)-大图H264,&(0x1<<1)-大图H265,&(0x1<<10<<0)-中图H264,&(0x1<<10<<1)-中图H265,&(0x1<<20<<0)-小图H264,&(0x1<<20<<1)-小图H265 */
                                        rst->ro._videoCodingFormatFirst = 0;
                                        /* 期望的编码方式,&(0x1<<0)-大图H264,&(0x1<<1)-大图H265,&(0x1<<10<<0)-中图H264,&(0x1<<10<<1)-中图H265,&(0x1<<20<<0)-小图H264,&(0x1<<20<<1)-小图H265 */
                                        rst->ro._videoCodingFormat = (0x1 << 0) | (0x1 << 1);
                                    }
                                }
                            }
                            {
                                if (NULL == *ppFrameConvert) {
                                    *ppFrameConvert = frameConvert_new__IO(pEncode->ro._pMonitor, pEncode->ro._externRunStatus, systemThreadTid, "enc.FC", fs_system_cpu_thread_get());
                                    frameConvert_startThread(*ppFrameConvert, 0);
                                }
                                const void *recordConfig0 = vsysChannel0;
                                const void *const recordConfig = fs_Config_node_get_first(pConfig, &recordConfig0, vsysChannel, "recordConfig");
                                const unsigned char keyInterval = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "keyInterval", 20, NULL);
                                const unsigned int encodeKbps = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "encodeKbps", 2048, NULL);
                                //#if defined __VDPAU
                                //                                struct FrameConvert_item * const pFrameConvert_item = frameConvert_item_new__IO(ImageFrame_H264_0, 1, keyInterval, encodeKbps,(void (* const) (void *))encode_P_cb_convert, rst);
                                //#else
                                //                                struct FrameConvert_item * const pFrameConvert_item = frameConvert_item_new__IO(ImageFrame_H264_0, 3, keyInterval, encodeKbps,(void (* const) (void *))encode_P_cb_convert, rst);
                                //#endif
                                struct FrameConvert_item * const pFrameConvert_item = frameConvert_item_new__IO(0, 1, keyInterval, encodeKbps, (void (* const) (void *))encode_P_cb_convert, rst);

                                /* 直正的解码对象 */
                                rst->ro.__pFrameConvert_item = pFrameConvert_item;
                                //double t1 = fs_time_uptime_get();
                                frameConvert_add_item__OI_2(*ppFrameConvert, pFrameConvert_item);
                            }
                            // FsPrintf(FsPrintfIndex, "use time %lf\n", fs_time_uptime_get() - t1).        fflush(stdout);
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        // 注册连接断开时的回调通知
                        //                        if (0 == ipv4) configManager_connect_error_register(pTargetCheck->ro._pConfigManager
                        //                                , (int (*)(const unsigned int *, void *, char * * const))targetCheck_P_item_cb_connect_error, rst);
                        /* 注册命令字 */
                        //                        if (0 == ipv4) configManager_cmd_register(pTargetCheck->ro._pConfigManager, "targetcheck_debug", rst->ro._uuid, ipv4, rst
                        //                                , (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))targetCheck_P_item_cb_debug
                        //                                , NULL, rst);
                        //                    if (0 == ipv4)configManager_cmd_register(pRecord->ro._pConfigManager, "record_inc", rst->ro._uuid, 0, rst, NULL, (void*) record_item_add_inc_pthreadSafety__OI_2, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "videolist_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "videotimeinfo_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "snapshort_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_snapshort_cb : NULL, NULL, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        //if (channel > 0) configManager_cmd_connect(pRecord->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))record_private_cmd_connect_cb, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 释放内存空间 */
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0) *ppEncode_item = rst;
                    //else ppNodeRecord_item[index++] = rst;
                } while (--ui > 0 && 0 == channel);
                if (0 == channel) fs_ObjectList_delete__OI(list);
            }
        } while (--uj > 0 && 0 == channel);
        fs_ObjectList_delete__OI(clusterList);
        //        if (0 == channel && pRecord->p.clusterItemList) {
        //            if (0 == index) {
        //                fs_ObjectList_delete_custom__OI(pRecord->p.clusterItemList, (void (*)(void*))record_P_item_delete__OI);
        //                pRecord->p.clusterItemList = NULL;
        //            } else
        //                while (pRecord->p.clusterItemList->nodeCount > index)record_P_item_delete__OI((struct Record_item*) fs_ObjectList_remove_tail(pRecord->p.clusterItemList));
        //        }
    }
}

static void *encode_P_T(struct Encode * const pEncode) {
    ////////////////////////////////////////////////////////////////////////////
    /* 创建线程监控 */
    void *pMonitor = pEncode->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, pEncode->ro._threadName, NULL, pEncode, pEncode->ro._pMonitor ? pEncode->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pEncode->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pEncode->ro._pConfigManager;
    FsObjectList * const itemList_ = pEncode->ro.__itemList_;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    // 解码对象
    FrameConvert *pFrameConvert = NULL;
    unsigned int sleepTime = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    struct Encode_item *pEncode_item;
    int i;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 根据
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pEncode->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        usleep(sleepTime);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pEncode->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (0 == pEncode->rw._resetChannel)channelCount = encode_private_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            /* 删除通道 */
            while (itemList_->nodeCount > channelCount) {
                pEncode_item = (struct Encode_item *) fs_ObjectList_remove_tail(itemList_);
                if (pEncode_item)encode_P_item_delete__OI(pEncode_item);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pEncode->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pEncode->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pEncode->rw._resetChannel > 0) {
                pMonitorItem->rw.line = __LINE__;
                //double t1 = fs_time_uptime_get();
                encode_P_item_new(pEncode, pEncode->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Encode_item **) (itemList_->pNode + itemList_->startIndex + pEncode->rw._resetChannel - 1),
                        &pFrameConvert, pMonitorItem);
                //FsPrintf(FsPrintfIndex, "use time %lf,sleepTime=%u\n", fs_time_uptime_get() - t1, sleepTime),                fflush(stdout);
            }
            pEncode->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    if (pEncode->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pEncode->rw._resetChannel);
        fflush(stdout);
    }
    /* 删除各通道 */
    {
        struct Encode_item **pNode = (struct Encode_item **) itemList_->pNode + itemList_->startIndex;
        for (i = itemList_->nodeCount; i > 0; i--) {
            pEncode_item = *pNode++;
            if (pEncode_item)encode_P_item_delete__OI(pEncode_item);
        }
        fs_ObjectList_clear(itemList_);
    }
    /* 删除解码对象 */
    if (pFrameConvert) {
        frameConvert_stopThread(pFrameConvert);
        frameConvert_delete__OI(pFrameConvert);
    }
    /* 释放空间 */
    /* 释放全局变量空间 */
    /* 释放中间变量空间 */
    ////////////////////////////////////////////////////////////////////////////
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", pEncode->ro._threadName, pEncode->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pEncode->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void encode_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "watermarking", "水印配置", "水印配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "2");

    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_startX", "水印开始位置的X坐标", "水印开始位置的X坐标", FsConfig_nodeShowType_default, 0, 0x7, 0, 1800, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_startY", "水印开始位置的Y坐标", "水印开始位置的Y坐标", FsConfig_nodeShowType_default, 0, 0x7, 0, 1000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_width", "水印字符宽度", "水印字符宽度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_height", "水印字符高度", "水印字符高度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "watermarking_color", "水印颜色,RGB", "水印颜色,RGB,16进制", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFF, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF0000, "红色", "红色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x00FF00, "绿色", "绿色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x0000FF, "蓝色", "蓝色");
    }
    {
        fs_Config_node_string_add(pConfig, parent, "watermarkingData", "水印数据", "水印数据", 0, 0x7, 1, 128, 1);
    }
}

/* 检查pEncode模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int encode_check_channel_changed(struct Encode * const pEncode, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Encode
#ifndef __check_channel_changed 
    FsConfig * const pConfig = ((ConfigManager*) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro._pConfigManager)->ro.__pConfig;
    struct /* Recognition_item */ FsMacrosValue2(__check_channel_changed_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__check_channel_changed_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro.
#ifdef __check_channel_changed_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro.
#ifdef __check_channel_changed_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
    if (strcmp("vsys vsysChannel", itemPath) == 0) {
        if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)) {
            if (/* Recognition */FsMacrosValue2(__check_channel_changed_Server, _Mask) != 0
                    && (/* Recognition */FsMacrosValue2(__check_channel_changed_Server, _Mask) & fs_Config_node_integer_get_mask(pConfig, item0, item, "moduleMask", NULL)) == 0)return 0;
            return 1;
        }
        if (sum == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum)return 0;
    } else {
        FsLog(FsLogType_error, FsPrintfIndex, "Invalid itemPath:\"%s\".\n", itemPath);
        fflush(stdout);
        if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)) return 1;
        const void *parent0;
        const void *parent;
        {
            parent0 = pConfig;
            FsObjectList *list = fs_Config_node_template__IO(pConfig, &parent0, pConfig
                    , ((ConfigManager*) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro._pConfigManager)->ro.__ipList_local, 0, "vsys");
            parent = list->pNode[list->startIndex];
            fs_ObjectList_delete__OI(list);
            list = fs_Config_node_template__IO(pConfig, &parent0, parent, NULL, 0, "vsysChannel");
            parent = list->pNode[list->startIndex + index];
            fs_ObjectList_delete__OI(list);
        }
        if (fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node *) parent) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum)return 0;
    }
#ifdef FsDebug
    FsLog2(FsLogType_info, FsPrintfIndex, "Item(=%p) has changed,index=%u,itemPath:\"%s\"/%p,sum=%llx/%llx\n"
            , /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item), index, itemPath, item, sum, /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum);
#endif
    return 1;
#ifdef __check_channel_changed_itemListLock
#undef __check_channel_changed_itemListLock
#endif
#undef __check_channel_changed_Server
#endif
}

/* 
 * 创建一个新的Encode对象;
 * 返回Encode指针.
 */
struct Encode *encode_new__IO(/* 线程名 */const char threadName[]
        , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(threadName) + 1;
    struct Encode* rst = (struct Encode*) fsMalloc(sizeof (struct Encode) +len);
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
    rst->ro.__itemList_ = fs_ObjectList_new_unsafe__IO(8);
    /* 在有输出数据时向哪个开方分组写入信号 */
    rst->ro._pGroupSqrtOut = pGroupSqrtOut;
    /* 重置通道,负数表示重置完成,初始化为-1 */
    rst->rw._resetChannel = -1;

    return rst;
}

/* 删除pEncode指向的实例对象 */
void encode_delete__OI(struct Encode * const pEncode) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pEncode->ro.__itemList_);

    fsFree(pEncode);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void encode_startThread(struct Encode * const pEncode,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (pEncode->rw.runStatus == 0 && pEncode->ro.stid == 0) {
        pEncode->rw.runStatus = 1;
        pEncode->rw.thisNice = thisNice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pEncode->ro._threadName,
                (long) pEncode->ro._pMonitor,
                pEncode->ro._parentTid,
                (long) pEncode->ro._externRunStatus);
#endif
        //#define __fs_thread_attr_stacksize 32*1024
#ifndef __fs_thread_attr
        pthread_attr_t attr;
        /* 初始化线程属性 */
        pthread_attr_init(&attr);
        /* 设置分离状态 */
        //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        /* 设置栈地址 */
        //pthread_attr_setstackaddr ( pthread_attr_t *attr, void *addr );
        /* 设置栈大小 */
#ifndef __fs_thread_attr_stacksize
        pthread_attr_setstacksize(&attr, FsStackBase + 32 * 1024);
#else
        pthread_attr_setstacksize(&attr, FsStackBase + __fs_thread_attr_stacksize);
#endif
        /* 设置线程优先级 */
        if (thisNice > 0 && thisNice < 100) {
            /* 修改为实时分时调度策略 */
            pthread_attr_setschedpolicy(&attr, SCHED_RR);
            struct sched_param param;
            param.sched_priority = thisNice;
            pthread_attr_setschedparam(&attr, &param);
        } else if (0 == thisNice) {
            /* 继承父进程优先级 */
#ifndef ANDROID
            pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
#endif
        } else if (-1 == thisNice) {
            /* 修改为分时调度策略 */
            pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        } else {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid thisNice(=%hhd).\n", thisNice);
            fflush(stdout);
            FsLogExit();
        }
#ifdef __fs_thread_attr_stacksize
#undef __fs_thread_attr_stacksize
#endif
#endif           
        if (pthread_create(&pEncode->ro.stid, &attr, (void* (*)(void*))encode_P_T, pEncode)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
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
        thread_join_remove_check_pthreadSafety(pEncode->ro.stid);
        pEncode->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧,如果数据可作为快照则返回1,否则返回-1 */
void encode_item_frame_in_pthreadSafety__OI_4(struct Encode * const pEncode, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * const ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName encode_item_frame_in_pthreadSafety__OI_4
    struct Encode_item * const pEncode_item = (struct Encode_item *) pEncode->ro.__itemList_->pNode[pEncode->ro.__itemList_->startIndex + index];
#ifdef FsDebug
    if (NULL == pEncode_item) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
#endif 
    struct FrameConvert_item * pFrameConvert_item = pEncode_item->ro.__pFrameConvert_item;
    unsigned long long decodeMask_set = 0;
    /* 检查解码参数有无发生改变 */
    {
        FsObjectImageFrame * const pFrame = ppFrame[0];
        {
            unsigned int videoCodingFormat = pEncode_item->ro._videoCodingFormat;
            for (;;) {
                //FsPrintf(1, "pFrame->dataValidType=%llx,%u/%u,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x\n", pFrame->dataValidType, pFrame->width[0], pFrame->height[0], videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst);
                /* 大图h264 */
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size 0
#ifndef __encode_item_frame_in_pthreadSafety__OI_4_check_encode 
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 1)
#elif defined __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 0)
#else
#error "error"
#endif
                if (pEncode_item->ro._videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) {
                    /* 期望的数据包含当前数据类型 */
                    unsigned char useThisType = 0;
                    if (pEncode_item->ro.__pTypeFace) {
                        if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type))useThisType = 1;
                    } else {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType)) useThisType = 1;
                        else {
                            if ((videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) == 0) {
                                /* 第一轮检查在pFrame未发现已有的期望数据 */
                                if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type)) useThisType = 1;
                            }
                        }
                    }
                    //FsPrintf(1, "pFrame->dataValidType=%llx ,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x,useThisType=%hhu\n", pFrame->dataValidType, videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst, useThisType);
                    if (useThisType) {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && NULL == pEncode_item->ro.__pTypeFace) {
                            goto FsMacrosFunctionTag(set_no_encode);
                            break;
                        } else {
                            decodeMask_set = ImageFrame_YUV420P_0;
                            if (pFrameConvert_item->ro._convertType != FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)) {
                                struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)

                                        , 3

                                        , pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);

                                frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                                /* 直正的编码对象 */
                                pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                                frameConvert_item_delete__OI(pFrameConvert_item);
                                pFrameConvert_item = pFrameConvert_item1;
                            }
                            break;
                        }
                    }
                    videoCodingFormat &= ~__encode_item_frame_in_pthreadSafety__OI_4_check_encode_type;
                }
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#endif  
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#endif
#endif
                /* 大图h265 */
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size 0
#ifndef __encode_item_frame_in_pthreadSafety__OI_4_check_encode 
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 1)
#elif defined __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 0)
#else
#error "error"
#endif
                if (pEncode_item->ro._videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) {
                    /* 期望的数据包含当前数据类型 */
                    unsigned char useThisType = 0;
                    if (pEncode_item->ro.__pTypeFace) {
                        if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type))useThisType = 1;
                    } else {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType)) useThisType = 1;
                        else {
                            if ((videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) == 0) {
                                /* 第一轮检查在pFrame未发现已有的期望数据 */
                                if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type)) useThisType = 1;
                            }
                        }
                    }
                    //FsPrintf(1, "pFrame->dataValidType=%llx ,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x,useThisType=%hhu\n", pFrame->dataValidType, videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst, useThisType);
                    if (useThisType) {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && NULL == pEncode_item->ro.__pTypeFace) {
                            goto FsMacrosFunctionTag(set_no_encode);
                            break;
                        } else {
                            decodeMask_set = ImageFrame_YUV420P_0;
                            if (pFrameConvert_item->ro._convertType != FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)) {
                                struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)

                                        , 3

                                        , pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);

                                frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                                /* 直正的编码对象 */
                                pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                                frameConvert_item_delete__OI(pFrameConvert_item);
                                pFrameConvert_item = pFrameConvert_item1;
                            }
                            break;
                        }
                    }
                    videoCodingFormat &= ~__encode_item_frame_in_pthreadSafety__OI_4_check_encode_type;
                }
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#endif  
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#endif
#endif
                /* 中图h264 */
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size 1
#ifndef __encode_item_frame_in_pthreadSafety__OI_4_check_encode 
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 1)
#elif defined __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 0)
#else
#error "error"
#endif
                if (pEncode_item->ro._videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) {
                    /* 期望的数据包含当前数据类型 */
                    unsigned char useThisType = 0;
                    if (pEncode_item->ro.__pTypeFace) {
                        if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type))useThisType = 1;
                    } else {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType)) useThisType = 1;
                        else {
                            if ((videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) == 0) {
                                /* 第一轮检查在pFrame未发现已有的期望数据 */
                                if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type)) useThisType = 1;
                            }
                        }
                    }
                    //FsPrintf(1, "pFrame->dataValidType=%llx ,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x,useThisType=%hhu\n", pFrame->dataValidType, videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst, useThisType);
                    if (useThisType) {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && NULL == pEncode_item->ro.__pTypeFace) {
                            goto FsMacrosFunctionTag(set_no_encode);
                            break;
                        } else {
                            decodeMask_set = ImageFrame_YUV420P_0;
                            if (pFrameConvert_item->ro._convertType != FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)) {
                                struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)

                                        , 3

                                        , pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);

                                frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                                /* 直正的编码对象 */
                                pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                                frameConvert_item_delete__OI(pFrameConvert_item);
                                pFrameConvert_item = pFrameConvert_item1;
                            }
                            break;
                        }
                    }
                    videoCodingFormat &= ~__encode_item_frame_in_pthreadSafety__OI_4_check_encode_type;
                }
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#endif  
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#endif
#endif
                /* 中图h265 */
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size 1
#ifndef __encode_item_frame_in_pthreadSafety__OI_4_check_encode 
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 1)
#elif defined __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 0)
#else
#error "error"
#endif
                if (pEncode_item->ro._videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) {
                    /* 期望的数据包含当前数据类型 */
                    unsigned char useThisType = 0;
                    if (pEncode_item->ro.__pTypeFace) {
                        if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type))useThisType = 1;
                    } else {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType)) useThisType = 1;
                        else {
                            if ((videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) == 0) {
                                /* 第一轮检查在pFrame未发现已有的期望数据 */
                                if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type)) useThisType = 1;
                            }
                        }
                    }
                    //FsPrintf(1, "pFrame->dataValidType=%llx ,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x,useThisType=%hhu\n", pFrame->dataValidType, videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst, useThisType);
                    if (useThisType) {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && NULL == pEncode_item->ro.__pTypeFace) {
                            goto FsMacrosFunctionTag(set_no_encode);
                            break;
                        } else {
                            decodeMask_set = ImageFrame_YUV420P_0;
                            if (pFrameConvert_item->ro._convertType != FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)) {
                                struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)

                                        , 3

                                        , pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);

                                frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                                /* 直正的编码对象 */
                                pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                                frameConvert_item_delete__OI(pFrameConvert_item);
                                pFrameConvert_item = pFrameConvert_item1;
                            }
                            break;
                        }
                    }
                    videoCodingFormat &= ~__encode_item_frame_in_pthreadSafety__OI_4_check_encode_type;
                }
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#endif  
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#endif
#endif
                /* 小图h264 */
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size 2
#ifndef __encode_item_frame_in_pthreadSafety__OI_4_check_encode 
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 1)
#elif defined __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 0)
#else
#error "error"
#endif
                if (pEncode_item->ro._videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) {
                    /* 期望的数据包含当前数据类型 */
                    unsigned char useThisType = 0;
                    if (pEncode_item->ro.__pTypeFace) {
                        if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type))useThisType = 1;
                    } else {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType)) useThisType = 1;
                        else {
                            if ((videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) == 0) {
                                /* 第一轮检查在pFrame未发现已有的期望数据 */
                                if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type)) useThisType = 1;
                            }
                        }
                    }
                    //FsPrintf(1, "pFrame->dataValidType=%llx ,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x,useThisType=%hhu\n", pFrame->dataValidType, videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst, useThisType);
                    if (useThisType) {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && NULL == pEncode_item->ro.__pTypeFace) {
                            goto FsMacrosFunctionTag(set_no_encode);
                            break;
                        } else {
                            decodeMask_set = ImageFrame_YUV420P_0;
                            if (pFrameConvert_item->ro._convertType != FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)) {
                                struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)

                                        , 3

                                        , pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);

                                frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                                /* 直正的编码对象 */
                                pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                                frameConvert_item_delete__OI(pFrameConvert_item);
                                pFrameConvert_item = pFrameConvert_item1;
                            }
                            break;
                        }
                    }
                    videoCodingFormat &= ~__encode_item_frame_in_pthreadSafety__OI_4_check_encode_type;
                }
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#endif  
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#endif
#endif
                /* 小图h265 */
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size 2
#ifndef __encode_item_frame_in_pthreadSafety__OI_4_check_encode 
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H265
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 1)
#elif defined __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag H264
#define __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type (0x1 <<(__encode_item_frame_in_pthreadSafety__OI_4_check_encode_size*10)<< 0)
#else
#error "error"
#endif
                if (pEncode_item->ro._videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) {
                    /* 期望的数据包含当前数据类型 */
                    unsigned char useThisType = 0;
                    if (pEncode_item->ro.__pTypeFace) {
                        if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type))useThisType = 1;
                    } else {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType)) useThisType = 1;
                        else {
                            if ((videoCodingFormat & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type) == 0) {
                                /* 第一轮检查在pFrame未发现已有的期望数据 */
                                if (0 == pEncode_item->ro._videoCodingFormatFirst || (pEncode_item->ro._videoCodingFormatFirst & __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type)) useThisType = 1;
                            }
                        }
                    }
                    //FsPrintf(1, "pFrame->dataValidType=%llx ,videoCodingFormat=%x,pEncode_item->ro._videoCodingFormat=%x,pEncode_item->ro._videoCodingFormatFirst=%x,useThisType=%hhu\n", pFrame->dataValidType, videoCodingFormat, pEncode_item->ro._videoCodingFormat, pEncode_item->ro._videoCodingFormatFirst, useThisType);
                    if (useThisType) {
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && NULL == pEncode_item->ro.__pTypeFace) {
                            goto FsMacrosFunctionTag(set_no_encode);
                            break;
                        } else {
                            decodeMask_set = ImageFrame_YUV420P_0;
                            if (pFrameConvert_item->ro._convertType != FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)) {
                                struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(FsMacrosValue4(ImageFrame_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)

                                        , 3

                                        , pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);

                                frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                                /* 直正的编码对象 */
                                pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                                frameConvert_item_delete__OI(pFrameConvert_item);
                                pFrameConvert_item = pFrameConvert_item1;
                            }
                            break;
                        }
                    }
                    videoCodingFormat &= ~__encode_item_frame_in_pthreadSafety__OI_4_check_encode_type;
                }
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_type
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h265
#endif  
#ifdef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#undef __encode_item_frame_in_pthreadSafety__OI_4_check_encode_h264
#endif
#endif
                if (videoCodingFormat != 0) {
                    Encode_itemLog(FsLogType_error, "videoCodingFormat(=%x) != 0", videoCodingFormat);
                    fflush(stdout);
                }
                continue;
                if (1) {
                    FsMacrosFunctionTag(set_no_encode) :;
                    if (pFrameConvert_item->ro._convertType != 0) {
                        struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(0, 1, pFrameConvert_item->ro._keyInterval, pFrameConvert_item->ro._kbps, (void (* const) (void *))encode_P_cb_convert, pEncode_item);
                        frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
                        /* 直正的编码对象 */
                        pEncode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
                        frameConvert_item_delete__OI(pFrameConvert_item);
                        pFrameConvert_item = pFrameConvert_item1;
                    }
                    break;
                }
            }
        }
    }
    if (pEncode_item->ro.__pTypeFace) {
        for (index = 0; index < frameCount;) {
            FsObjectImageFrame * const pFrame = ppFrame[index++];
            pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0 | decodeMask_set;
            pthread_mutex_lock(&((FsObject*) pFrame)->mutex);
            if (1 == image_frame_prapare(pFrame, ImageFrame_YUV420P_0)) {
                if (ImageFrame_valid_H264_0(pFrame->dataValidType))image_frame_clear_data(pFrame, ImageFrame_H264_0);
                if (ImageFrame_valid_H265_0(pFrame->dataValidType))image_frame_clear_data(pFrame, ImageFrame_H265_0);
                fs_TypeFace_write(pEncode_item->ro.__pTypeFace, (unsigned char*) pEncode_item->ro._watermarkingData, pFrame->data.yuv420p[0]->data, pFrame->width[0], pFrame->height[0],
                        pFrame->width[0], pEncode_item->ro._watermarking_startX, pEncode_item->ro._watermarking_startY, pEncode_item->ro._watermarking_color >> 16);
                fs_TypeFace_write_uv(pEncode_item->ro.__pTypeFace, (unsigned char*) pEncode_item->ro._watermarkingData, pFrame->data.yuv420p[0]->data + pFrame->sizeWidthHeight[0], pFrame->width[0], pFrame->height[0],
                        pFrame->width[0] >> 1, pEncode_item->ro._watermarking_startX, pEncode_item->ro._watermarking_startY, (pEncode_item->ro._watermarking_color >> 8)&0xFF);
                fs_TypeFace_write_uv(pEncode_item->ro.__pTypeFace, (unsigned char*) pEncode_item->ro._watermarkingData, pFrame->data.yuv420p[0]->data + pFrame->sizeWidthHeight[0]+(pFrame->sizeWidthHeight[0] >> 2), pFrame->width[0], pFrame->height[0],
                        pFrame->width[0] >> 1, pEncode_item->ro._watermarking_startX, pEncode_item->ro._watermarking_startY, pEncode_item->ro._watermarking_color & 0xFF);
            }
            pthread_mutex_unlock(&((FsObject*) pFrame)->mutex);
            frameConvert_item_frame_in_pthreadSafety__OI_2(pFrameConvert_item, pFrame);
        }
    } else {
        for (index = 0; index < frameCount;) {
            FsObjectImageFrame * const pFrame = ppFrame[index++];
            pFrame->stats->decodeMask_set |= decodeMask_set;
            //if (!ImageFrame_valid_H264_0(ppFrame[index]->dataValidType)) FsPrintf(1, "Index=%u\n", ppFrame[index]->index);
            frameConvert_item_frame_in_pthreadSafety__OI_2(pFrameConvert_item, pFrame);
        }
    }
}

/* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
unsigned int encode_item_frame_out_pthreadSafety__IO_4(struct Encode * const pEncode, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
    register struct Encode_item * const pEncode_item = (struct Encode_item *) pEncode->ro.__itemList_->pNode[pEncode->ro.__itemList_->startIndex + index];
#ifdef FsDebug
    if (NULL == pEncode_item) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
#endif 
    //    int tt = frameConvert_item_frame_out_pthreadSafety__IO_4(pEncode_item->ro.__pFrameConvert_item, frameCount, ppFrame);
    //    if (tt > 0) FsPrintf(1, "tt=%d\n", tt);
    //    return tt;
    return frameConvert_item_frame_out_pthreadSafety__IO_4(pEncode_item->ro.__pFrameConvert_item, frameCount, ppFrame);
}

/* 获取数据帧的数量 */
unsigned int encode_item_get_frame_count_pthreadSafety(struct Encode * const pEncode, /* 获取的通道编号,从0开始 */unsigned int index) {
    struct Encode_item * const pEncode_item = (struct Encode_item *) pEncode->ro.__itemList_->pNode[pEncode->ro.__itemList_->startIndex + index];
    if (NULL == pEncode_item)return 0;
    return pEncode_item->ro.__pFrameConvert_item->ro.__framelistOut->nodeCount;
}

/* 重置模块内通道对应的开方分组 */
void encode_item_reset_groupSqrtOut(struct Encode * const pEncode, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Encode
#ifndef __reset_groupSqrtOut 
    struct /* Recognition_item */ FsMacrosValue2(__reset_groupSqrtOut_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __reset_groupSqrtOut_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__reset_groupSqrtOut_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __reset_groupSqrtOut_Server)->ro.
#ifdef __reset_groupSqrtOut_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __reset_groupSqrtOut_Server)->ro.
#ifdef __reset_groupSqrtOut_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
    if (/* pRecognition_item */ FsMacrosValue3(p, __reset_groupSqrtOut_Server, _item))
        Fs_GroupSqrtOut_set(/* pRecognition_item */ FsMacrosValue3(p, __reset_groupSqrtOut_Server, _item)->rw._pGroupSqrtOut, pGroupSqrt->groupSqrt_member, pGroupSqrt->groupSqrt_group, groupValue, index);
#ifdef __reset_groupSqrtOut_itemListLock
#undef __reset_groupSqrtOut_itemListLock
#endif
#undef __reset_groupSqrtOut_Server
#endif
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
