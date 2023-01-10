#include "decode.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Log.h"
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
static void decode_P_cb_convert(struct Decode_item * const pDecode_item) {
    Fs_GroupSqrtOut_value_set(pDecode_item->rw._pGroupSqrtOut, &pDecode_item->ro._pDecode->ro._pGroupSqrtOut->groupSqrt_mutex);
}

/* 从配置获取通道数 */
static unsigned int decode_private_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {    
#ifndef __get_channelCount_vsys_vsysChannel 
    if (NULL == pConfig) {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
        *rst_pVsysChannel0 = NULL;
        return fs_ObjectList_new__IO(4);
#else
        return 0;
#endif    
    }
    /* 通道数 */
    const void *vsys0 = pConfig;
    const void *vsys;
    {
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0,ipList, 0, "vsys");
        if (NULL == list) {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
            *rst_pVsysChannel0 = NULL;
            return fs_ObjectList_new__IO(4);
#else
            return 0;
#endif   
        }
        vsys = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
    }
    {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
        *pCronfremethreshold = fs_Config_node_integer_get_first(pConfig, vsys0, vsys, "cronframethreshold", 1, NULL);
        *pCannelstatuscheckmode = fs_Config_node_integer_get_first(pConfig, vsys0, vsys, "channelstatuscheckmode", 0, NULL);

#endif
#ifdef __get_channelCount_vsys_vsysChannel_in_record
        if (pRecord->p.gbsdkConfig__videoInfoDataClientList) {
#define __record_P_update_sdk_gb28181
#ifndef __record_P_new_sdk_gb28181 
            if (Vsys_sdkPlayBack_is_process()) {
#ifdef __record_P_update_sdk_gb28181
                FsEbml * const gbsdkConfig = pRecord->p.gbsdkConfig__videoInfoDataClientList;
                struct FsEbml_node * const gb28181Config = fs_Ebml_node_get_first(gbsdkConfig, (struct FsEbml_node*) gbsdkConfig, "gb28181Config");
#else
                struct FsEbml_node * const gb28181Config = fs_Ebml_node_addChild(gbsdkConfig, (struct FsEbml_node*) gbsdkConfig, "gb28181Config", FsEbmlNodeType_Struct);
#endif
                {
                    const FsString * const ipv4_streamPort_PlayBack = fs_Config_node_string_get_first_String(pConfig, vsys0, vsys, "gb28181Config ipv4_streamPort_PlayBack", NULL);
#ifdef __record_P_update_sdk_gb28181
                    struct FsEbml_node * const pEbml_node_ipv4_streamPort_PlayBack = fs_Ebml_node_get_first(gbsdkConfig, gb28181Config, "ipv4_streamPort_PlayBack");
                    if (pEbml_node_ipv4_streamPort_PlayBack->data.lenth != ipv4_streamPort_PlayBack->lenth
                            || memcmp(pEbml_node_ipv4_streamPort_PlayBack->data.buffer, ipv4_streamPort_PlayBack->buffer, ipv4_streamPort_PlayBack->lenth) != 0) {
                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        fs_Ebml_node_data_set(pEbml_node_ipv4_streamPort_PlayBack, ipv4_streamPort_PlayBack->lenth, ipv4_streamPort_PlayBack->buffer);
                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                    }
#else             
                    fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                            fs_Ebml_node_addChild_with_a_property
#else
                            fs_Ebml_node_addChild
#endif
                            (gbsdkConfig, gb28181Config, "ipv4_streamPort_PlayBack", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                            , "comment", "回放使用的端口号"
#endif
                            ), ipv4_streamPort_PlayBack->lenth, ipv4_streamPort_PlayBack->buffer);
#endif
                }
                {
                    FsObjectList * const addrmapList = fs_Config_node_string_get_list__IO(pConfig, vsys0, vsys, "addrmap", NULL);
#ifdef __record_P_update_sdk_gb28181
                    FsObjectList * const addrmapList1 = fs_Ebml_node_get__IO(gbsdkConfig, gb28181Config, "addrmap");
                    if (addrmapList) {
                        if (addrmapList1 != NULL) {
                            const FsString **ppNode = (const FsString **) addrmapList->pNode + addrmapList->startIndex;
                            struct FsEbml_node **ppNode1 = (struct FsEbml_node **) addrmapList1->pNode + addrmapList1->startIndex;
                            unsigned int ui = addrmapList1->nodeCount;
                            if (ui > (unsigned int) addrmapList->nodeCount) {
                                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                do {
                                    fs_Ebml_node_delete(gbsdkConfig, ppNode1[--ui]);
                                } while (ui > (unsigned int) addrmapList->nodeCount);
                                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                            }
                            while (ui-- > 0) {
                                const FsString * const addrmap = *ppNode++;
                                struct FsEbml_node * const addrmap1 = *ppNode1++;
                                if (addrmap1->data.lenth != addrmap->lenth || memcmp(addrmap1->data.buffer, addrmap->buffer, addrmap->lenth) != 0) {
                                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    fs_Ebml_node_data_set(addrmap1, addrmap->lenth, addrmap->buffer);
                                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                                }
                            }
                            if (addrmapList->nodeCount > addrmapList1->nodeCount) {
                                ui = addrmapList->nodeCount - addrmapList1->nodeCount;
                                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                do {
                                    const FsString * const addrmap = *ppNode++;
                                    fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                                            fs_Ebml_node_addChild_with_a_property
#else
                                            fs_Ebml_node_addChild
#endif
                                            (gbsdkConfig, gb28181Config, "addrmap", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                                            , "comment", "本地IP映射,在一些应用场景中无法知道本机映射的ip时使用此数据作为映射,格式:'ip/掩码 映射地址 权重',映射地址可为域名,权重空时值为1"
#endif
                                            )
                                            , addrmap->lenth, addrmap->buffer);
                                } while (--ui > 0);
                                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                            }
                            fs_ObjectList_delete__OI(addrmapList);
                            fs_ObjectList_delete__OI(addrmapList1);
                        } else {
                            pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
#ifndef __record_P_new_sdk_gb28181_do_addrmapList 
                            const FsString **ppNode = (const FsString **) addrmapList->pNode + addrmapList->startIndex;
                            unsigned int ui = addrmapList->nodeCount;
                            while (ui-- > 0) {
                                const FsString * const addrmap = *ppNode++;
                                fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                                        fs_Ebml_node_addChild_with_a_property
#else
                                        fs_Ebml_node_addChild
#endif
                                        (gbsdkConfig, gb28181Config, "addrmap", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                                        , "comment", "本地IP映射,在一些应用场景中无法知道本机映射的ip时使用此数据作为映射,格式:'ip/掩码 映射地址 权重',映射地址可为域名,权重空时值为1"
#endif
                                        )
                                        , addrmap->lenth, addrmap->buffer);
                            }
                            fs_ObjectList_delete__OI(addrmapList);
#endif
                            pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                            Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                        }
                    } else if (addrmapList1) {
                        fs_ObjectList_delete__OI(addrmapList1);
                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        fs_Ebml_node_delete_child_byString(gbsdkConfig, gb28181Config, "addrmap");
                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                        Record_sdkConfigUpdate_set_0_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                    }
#else
                    if (addrmapList) {
#ifndef __record_P_new_sdk_gb28181_do_addrmapList
                        const FsString **ppNode = (const FsString **) addrmapList->pNode + addrmapList->startIndex;
                        unsigned int ui = addrmapList->nodeCount;
                        while (ui-- > 0) {
                            const FsString * const addrmap = *ppNode++;
                            fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                                    fs_Ebml_node_addChild_with_a_property
#else
                                    fs_Ebml_node_addChild
#endif
                                    (gbsdkConfig, gb28181Config, "addrmap", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                                    , "comment", "本地IP映射,在一些应用场景中无法知道本机映射的ip时使用此数据作为映射,格式:'ip/掩码 映射地址 权重',映射地址可为域名,权重空时值为1"
#endif
                                    )
                                    , addrmap->lenth, addrmap->buffer);
                        }
                        fs_ObjectList_delete__OI(addrmapList);
#endif
                    }
#endif
                }
            }
#ifdef __record_P_update_sdk_gb28181
#undef __record_P_update_sdk_gb28181
#endif
#endif
        }
        pRecord->rw._snapbuffertimeout = fs_Config_node_float_get_first(pConfig, vsys0, vsys, "snapbuffertimeout", 0.0, NULL);
#endif
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, vsys, 0,NULL, 0, "vsysChannel");
        if (NULL == list) {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
            *rst_pVsysChannel0 = NULL;
            return fs_ObjectList_new__IO(4);
#else
            return 0;
#endif   
        } else {
#ifdef __get_channelCount_vsys_vsysChannel_in_vsys
            *rst_pVsysChannel0 = vsys0;
            return list;
#else
            const unsigned int rst = list->nodeCount;
            fs_ObjectList_delete__OI(list);
            return rst;
#endif   
        }
    }
#ifndef __get_channelCount_vsys_vsysChannel_in_vsys
#undef __get_channelCount_vsys_vsysChannel_in_vsys
#endif
#ifdef __get_channelCount_vsys_vsysChannel_in_record
#undef __get_channelCount_vsys_vsysChannel_in_record
#endif
#endif
}

/* 删除pDecode_item指向的实例对象 */
static void decode_private_item_delete__OI(struct Decode_item * const pDecode_item) {
    /* 直正的解码对象 */
    frameConvert_item_delete__OI(pDecode_item->ro.__pFrameConvert_item);

    fsFree(pDecode_item);
}

static void decode_private_item_new(struct Decode * const pDecode, /* 通道号 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList,
        /* 原来的解码项 */struct Decode_item * * const ppDecode_item, /* 解码对象 */FrameConvert **ppFrameConvert) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pDecode->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, 0, ipList, 0 == channel, "vsys");
    if (clusterList) {
        void **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int uj = clusterList->nodeCount;
        do {
            const void *vsysChannel0 = vsys0;
            const void* const vsys = *ppNodeCluster++;
            FsObjectList * const list = 0 == channel ? fs_Config_node_template_orderFirst__IO(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel")
                    : (FsObjectList *) fs_Config_node_template_get_orderFirst(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel", channel - 1);
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
                        if (*ppDecode_item != NULL && (*ppDecode_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppDecode_item) {
                        decode_private_item_delete__OI(*ppDecode_item);
                    }
                    struct Decode_item* rst;
                    if ((Decode_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0 && Decode_Mask != 0) rst = NULL;
                    else {
                        char *pd;
                        //const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = 0; //uuid->lenth;
                        if (channel > 0) {
                            rst = (struct Decode_item*) fsMalloc(sizeof (struct Decode_item) +len);
                            memset(rst, 0, sizeof (struct Decode_item));
                            pd = ((char*) rst) + Memery_Alignment(sizeof (struct Decode_item));
                        } else {
                            //                        rst = (struct Record_item*) fsMalloc(Record_item_ClusterLen + len);
                            //                        memset(rst, 0, Record_item_ClusterLen);
                            //                        pd = ((char*) rst) + Record_item_ClusterLen;
                        }
                        if (channel > 0) {
                            /* 整个通道依赖的所有数据的校验和 */
                            rst->ro._sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode);
                            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pDecode->ro._pGroupSqrtOut->groupSqrt_member, pDecode->ro._pGroupSqrtOut->groupSqrt_group
                                    , pDecode->ro._pGroupSqrtOut->groupValue, channel - 1, &pDecode->ro._pGroupSqrtOut->groupSqrt_mutex);
                            /* 所属的解码对象 */
                            rst->ro._pDecode = pDecode;
                        }
                        if (channel > 0) {
                            if (NULL == *ppFrameConvert) {
                                *ppFrameConvert = frameConvert_new__IO(pDecode->ro._pMonitor, pDecode->ro._externRunStatus, systemThreadTid, "dec.FC", fs_system_cpu_thread_get());
                                frameConvert_startThread(*ppFrameConvert, 0);
                            }
                            struct FrameConvert_item * const pFrameConvert_item = frameConvert_item_new__IO(0, 1, 0, 0, (void (* const) (void *))decode_P_cb_convert, rst);
                            /* 直正的解码对象 */
                            rst->ro.__pFrameConvert_item = pFrameConvert_item;

                            frameConvert_add_item__OI_2(*ppFrameConvert, pFrameConvert_item);
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
                    if (channel > 0) *ppDecode_item = rst;
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

static void *decode_private_thread(struct Decode * const pDecode) {
    /* 创建线程监控 */
    void *pMonitor = pDecode->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, pDecode->ro._threadName, NULL, pDecode, pDecode->ro._pMonitor ? pDecode->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pDecode->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pDecode->ro._pConfigManager;
    FsObjectList * const itemList_ = pDecode->ro.__itemList_;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    // 解码对象
    FrameConvert *pFrameConvert = NULL;
    unsigned int sleepTime = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    struct Decode_item *pDecode_item;
    int i;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 根据
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pDecode->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        usleep(sleepTime);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pDecode->rw._resetChannel>-1) {
            sleepTime = 1000;
            /* 读取通道数 */
            if (0 == pDecode->rw._resetChannel)channelCount = decode_private_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            /* 删除通道 */
            while (itemList_->nodeCount > channelCount) {
                pDecode_item = (struct Decode_item *) fs_ObjectList_remove_tail(itemList_);
                if (pDecode_item)decode_private_item_delete__OI(pDecode_item);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pDecode->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pDecode->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pDecode->rw._resetChannel > 0) {
                pMonitorItem->rw.line = __LINE__;
                decode_private_item_new(pDecode, pDecode->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Decode_item **) (itemList_->pNode + itemList_->startIndex + pDecode->rw._resetChannel - 1),
                        &pFrameConvert);
                pMonitorItem->rw.line = __LINE__;
            }
            pDecode->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    if (pDecode->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pDecode->rw._resetChannel);
        fflush(stdout);
    }
    /* 删除各通道 */
    {
        struct Decode_item **pNode = (struct Decode_item **) itemList_->pNode + itemList_->startIndex;
        for (i = itemList_->nodeCount; i > 0; i--) {
            pDecode_item = *pNode++;
            if (pDecode_item)decode_private_item_delete__OI(pDecode_item);
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
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", pDecode->ro._threadName, pDecode->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pDecode->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void decode_createConfig(FsConfig * const pConfig, void * const parent) {
}

/* 检查pDecode模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int decode_check_channel_changed(struct Decode * const pDecode, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Decode
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
        if (sum == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum
#ifdef __check_channel_changed_checkTimeControl
                && fs_Config_get_sum_timeControl(pConfig, item0, item, __check_channel_changed_checkTimeControl) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._timeControlSum
#endif
                ) return 0;
    } else {
        FsLog(FsLogType_error, FsPrintfIndex, "Invalid itemPath:\"%s\".\n", itemPath);
        fflush(stdout);
        if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)) return 1;
        const void *parent0;
        const void *parent;
        {
            parent0 = pConfig;
            FsObjectList *list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0, ((ConfigManager*) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro._pConfigManager)->ro.__ipList_local, 0, "vsys");
            parent = list->pNode[list->startIndex];
            fs_ObjectList_delete__OI(list);
            list = fs_Config_node_template__IO(pConfig, &parent0, parent, 0, NULL, 0, "vsysChannel");
            parent = list->pNode[list->startIndex + index];
            fs_ObjectList_delete__OI(list);
        }
        if (fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node *) parent) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum
#ifdef __check_channel_changed_checkTimeControl
                && fs_Config_get_sum_timeControl(pConfig, item0, item, __check_channel_changed_checkTimeControl) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._timeControlSum
#endif
                )return 0;
    }
#ifdef FsDebug
    FsLog2(FsLogType_info, FsPrintfIndex, "Item(=%p) has changed,index=%u,itemPath:\"%s\"/%p,sum=%llx/%llx\n"
            , /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item), index, itemPath, item, sum, /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum);
#endif
    return 1;
#ifdef __check_channel_changed_itemListLock
#undef __check_channel_changed_itemListLock
#endif
#ifdef __check_channel_changed_checkTimeControl
#undef __check_channel_changed_checkTimeControl
#endif
#undef __check_channel_changed_Server
#endif
}

/* 
 * 创建一个新的Decode对象;
 * 返回Decode指针.
 */
struct Decode * decode_new__IO(/* 线程名 */const char threadName[]
        , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(threadName) + 1;
    struct Decode* rst = (struct Decode*) fsMalloc(sizeof (struct Decode) +len);
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
    rst->ro.__itemList_ = fs_ObjectList_new_unsafe__IO(8);
    /* 在有输出数据时向哪个开方分组写入信号 */
    rst->ro._pGroupSqrtOut = pGroupSqrtOut;
    /* 重置通道,负数表示重置完成,初始化为-1 */
    rst->rw._resetChannel = -1;

    return rst;
}

/* 删除pDecode指向的实例对象 */
void decode_delete__OI(struct Decode * const pDecode) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pDecode->ro.__itemList_);

    fsFree(pDecode);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void decode_startThread(struct Decode * const pDecode,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (pDecode->rw.runStatus == 0 && pDecode->ro.stid == 0) {
        pDecode->rw.runStatus = 1;
        pDecode->rw.thisNice = thisNice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pDecode->ro._threadName,
                (long) pDecode->ro._pMonitor,
                pDecode->ro._parentTid,
                (long) pDecode->ro._externRunStatus);
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
        if (pthread_create(&pDecode->ro.stid, &attr, (void* (*)(void*))decode_private_thread, pDecode)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
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
        thread_join_remove_check_pthreadSafety(pDecode->ro.stid);
        pDecode->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧,如果数据可作为快照则返回1,否则返回-1 */
void decode_item_frame_in__OI_4(struct Decode * const pDecode, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * const ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    register struct Decode_item * const pDecode_item = (struct Decode_item *) pDecode->ro.__itemList_->pNode[pDecode->ro.__itemList_->startIndex + index];
#ifdef FsDebug
    if (NULL == pDecode_item) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
#endif 
    struct FrameConvert_item * pFrameConvert_item = pDecode_item->ro.__pFrameConvert_item;
    /* 检查解码参数有无发生改变 */
    {
        const FsObjectStats * const pObjectStats = ppFrame[0]->stats;
        if (pObjectStats->decodeChanged && pObjectStats->decodeMask != pFrameConvert_item->ro._convertType) {
            //            FsPrintf(1, "decodeMask=%llx,convertType=%llx.\n\n\n", pObjectStats->decodeMask, pFrameConvert_item->ro._convertType);
            //            fflush(stdout);
            struct FrameConvert_item * const pFrameConvert_item1 = frameConvert_item_new__IO(pObjectStats->decodeMask, 2, 0, 0, (void (* const) (void *))decode_P_cb_convert, pDecode_item);

            frameConvert_add_item__OI_2(pFrameConvert_item->p.pFrameConvert, pFrameConvert_item1);
            /* 直正的解码对象 */
            pDecode_item->ro.__pFrameConvert_item = pFrameConvert_item1;
            frameConvert_item_delete__OI(pFrameConvert_item);
            pFrameConvert_item = pFrameConvert_item1;
        }
    }
    for (index = 0; index < frameCount;) {
        //        FsPrintf(1, "pFrame=%p\n", ppFrame[index]);
        //        fflush(stdout);
        frameConvert_item_frame_in_pthreadSafety__OI_2(pFrameConvert_item, ppFrame[index++]);
    }
}

/* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
unsigned int decode_item_frame_out__IO_4(struct Decode * const pDecode, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
    struct Decode_item * const pDecode_item = (struct Decode_item *) pDecode->ro.__itemList_->pNode[pDecode->ro.__itemList_->startIndex + index];
#ifdef FsDebug
    if (NULL == pDecode_item) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
#endif 
#if 1
    return frameConvert_item_frame_out_pthreadSafety__IO_4(pDecode_item->ro.__pFrameConvert_item, frameCount, ppFrame);
#else
    int rst = frameConvert_item_frame_out_pthreadSafety__IO_4(pDecode_item->ro.__pFrameConvert_item, frameCount, ppFrame);
    int i = 0;
    for (; i < rst; i++) {
        FsPrintf(1, "index=%u,%llx,convertType=%llx,decodeMask=%llx/%llx\n", ppFrame[i]->index, ppFrame[i]->dataValidType, pDecode_item->ro.__pFrameConvert_item->ro._convertType, ppFrame[i]->stats->decodeMask, ppFrame[i]->stats->decodeMask_set);
    }
    return rst;
#endif
}

/* 获取数据帧的数量 */
unsigned int decode_item_get_frame_count(struct Decode * const pDecode, /* 获取的通道编号,从0开始 */unsigned int index) {
    struct Decode_item * const pDecode_item = (struct Decode_item *) pDecode->ro.__itemList_->pNode[pDecode->ro.__itemList_->startIndex + index];
    if (NULL == pDecode_item)return 0;
    return pDecode_item->ro.__pFrameConvert_item->ro.__framelistOut->nodeCount;
}

/* 重置模块内通道对应的开方分组 */
void decode_item_reset_groupSqrtOut(struct Decode * const pDecode, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Decode
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
