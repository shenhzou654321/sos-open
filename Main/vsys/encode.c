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
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0, ipList, 0, "vsys");
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
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, vsys, 0, NULL, 0, "vsysChannel");
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

/* 删除pEncode_item指向的实例对象 */
static void encode_P_item_delete__OI(struct Encode_item * const pEncode_item) {
    /* 直正的解码对象 */
    frameConvert_item_delete__OI(pEncode_item->ro.__pFrameConvert_item);
    /* 文字叠加 */
    {
        unsigned int ui = pEncode_item->p._watermarkingCount;
        while (ui-- > 0) {
            fs_TypeFace_delete__OI(pEncode_item->p.watermarking[ui].__pTypeFace);
        }
    }
    fsFree(pEncode_item);
}

static void encode_P_item_new(struct Encode * const pEncode, /* 通道号 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的解码项 */struct Encode_item * * const ppEncode_item, /* 解码对象 */FrameConvert **ppFrameConvert, MonitorItem * const pMonitorItem) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pEncode->ro._pConfigManager)->ro.__pConfig;
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
                            const void *watermarking_item0 = vsysChannel0;
                            FsObjectList * const watermarking_itemList = moduleMask != 0 ? fs_Config_node_template__IO(pConfig, &watermarking_item0, vsysChannel, 3, NULL, 0, "watermarking watermarking_item") : NULL;
                            unsigned int watermarking_itemListCount = 0;
                            if (watermarking_itemList) {
                                void **ppNode = watermarking_itemList->pNode + watermarking_itemList->startIndex;
                                unsigned int count = watermarking_itemList->nodeCount;
                                while (count-- > 0) {
                                    const void * const watermarking_item = *ppNode++;
                                    const FsString * const watermarkingData = fs_Config_node_string_get_first_String(pConfig, watermarking_item0, watermarking_item, "watermarkingData", NULL);
                                    if (watermarkingData) {
                                        len += watermarkingData->lenth;
                                        watermarking_itemListCount++;
                                    }
                                }
                                if (0 == len)moduleMask = 0;
                            }
                            rst = (struct Encode_item*) fsMalloc(sizeof (struct Encode_item) + sizeof (rst->p.watermarking[0]) * watermarking_itemListCount + len);
                            memset(rst, 0, sizeof (struct Encode_item));
                            pd = ((char*) rst) + sizeof (struct Encode_item) + sizeof (rst->p.watermarking[0]) * watermarking_itemListCount;
                            /* 水印叠加的任务数 */
                            rst->p._watermarkingCount = watermarking_itemListCount;
                            if (watermarking_itemList) {
                                void **ppNode = watermarking_itemList->pNode + watermarking_itemList->startIndex;
                                unsigned int count = watermarking_itemList->nodeCount;
                                watermarking_itemListCount = 0;
                                while (count-- > 0) {
                                    const void * const watermarking_item = *ppNode++;
                                    const FsString * const watermarkingData = fs_Config_node_string_get_first_String(pConfig, watermarking_item0, watermarking_item, "watermarkingData", NULL);
                                    if (watermarkingData) {
                                        rst->p.watermarking[watermarking_itemListCount]._watermarkingData = pd, memcpy(pd, watermarkingData->buffer, watermarkingData->lenth), pd += watermarkingData->lenth;
                                        /* 水印开始位置的X坐标 */
                                        rst->p.watermarking[watermarking_itemListCount]._watermarking_startX = fs_Config_node_integer_get_first(pConfig, watermarking_item0, watermarking_item, "watermarking_startX", 0, NULL);
                                        /* 水印开始位置的Y坐标 */
                                        rst->p.watermarking[watermarking_itemListCount]._watermarking_startY = fs_Config_node_integer_get_first(pConfig, watermarking_item0, watermarking_item, "watermarking_startY", 0, NULL);
                                        /* 水印颜色,ycbcr */
                                        {
                                            unsigned int watermarking_color = fs_Config_node_integer_get_first(pConfig, watermarking_item0, watermarking_item, "watermarking_color", 0, NULL);
                                            rst->p.watermarking[watermarking_itemListCount]._watermarking_color = (((unsigned int) (0.257 * (watermarking_color >> 16) + 0.504 * ((watermarking_color >> 8)&0xFF) + 0.098 * (watermarking_color & 0xFF) + 16)) << 16)+
                                                    (((unsigned int) (-0.148 * (watermarking_color >> 16) - 0.291 * ((watermarking_color >> 8)&0xFF) + 0.439 * (watermarking_color & 0xFF) + 128)) << 8) +
                                                    0.439 * (watermarking_color >> 16) - 0.368 * ((watermarking_color >> 8)&0xFF) - 0.071 * (watermarking_color & 0xFF) + 128;
                                        }
                                        /* 文字叠加 */
                                        {
                                            const unsigned short watermarking_width = fs_Config_node_integer_get_first(pConfig, watermarking_item0, watermarking_item, "watermarking_width", 32, NULL);
                                            const unsigned short watermarking_height = fs_Config_node_integer_get_first(pConfig, watermarking_item0, watermarking_item, "watermarking_height", 32, NULL);
                                            rst->p.watermarking[watermarking_itemListCount].__pTypeFace = fs_TypeFace_new__IO(NULL, watermarking_width, watermarking_height, 1, 0);
                                        }
                                        watermarking_itemListCount++;
                                    }
                                }
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

    void *const watermarking_item = fs_Config_node_template_add(pConfig, parent, "watermarking_item", "水印", NULL, "watermarkingData", "水印", NULL, NULL, NULL, 0, 0x7, 32);
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, watermarking_item, "watermarking_startX", "水印开始位置的X坐标", "水印开始位置的X坐标", FsConfig_nodeShowType_default, 0, 0x7, 0, 1800, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, watermarking_item, "watermarking_startY", "水印开始位置的Y坐标", "水印开始位置的Y坐标", FsConfig_nodeShowType_default, 0, 0x7, 0, 1000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, watermarking_item, "watermarking_width", "水印字符宽度", "水印字符宽度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, watermarking_item, "watermarking_height", "水印字符高度", "水印字符高度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, watermarking_item, "watermarking_color", "水印颜色,RGB", "水印颜色,RGB,16进制", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFF, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF0000, "红色", "红色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x00FF00, "绿色", "绿色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x0000FF, "蓝色", "蓝色");
    }
    {
        void * const pNode = fs_Config_node_string_add(pConfig, watermarking_item, "watermarkingData", "水印数据", "水印数据", 0, 0x7, 1, 128, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time1", "时间1,格式:2022-12-30/10:24:30.123", "时间1,格式:2022-12-30/10:24:30.123");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time2", "时间2,格式:2022-12-30 10:24:30", "时间2,格式:2022-12-30 10:24:30");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time3", "时间3,格式:2022-12-30 10:24", "时间3,格式:2022-12-30 10:24");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time4", "时间4,格式:2022-12-30 10:24:30AM/PM", "时间4,格式:2022-12-30 10:24:30AM/PM");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time5", "时间5,格式:2022-12-30 10:24AM/PM", "时间5,格式:2022-12-30 10:24AM/PM");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time6", "时间6,格式:12-30-22 10:24:30", "时间6,格式:12-30-22 10:24:30");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time7", "时间7,格式:12-30-22 10:24", "时间7,格式:12-30-22 10:24");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time8", "时间8,格式:12-30-22 10:24:30AM/PM", "时间8,格式:12-30-22 10:24:30AM/PM");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time9", "时间9,格式:12-30-22 10:24AM/PM", "时间9,格式:12-30-22 10:24AM/PM");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time10", "时间10,格式:30-12-22 10:24:30", "时间2,格式:30-12-22 10:24:30");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time11", "时间11,格式:30-12-22 10:24", "时间3,格式:30-12-22 10:24");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time12", "时间12,格式:30-12-22 10:24:30AM/PM", "时间4,格式:30-12-22 10:24:30AM/PM");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "$$time13", "时间13,格式:30-12-22 10:24AM/PM", "时间5,格式:30-12-22 10:24AM/PM");
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
 * 创建一个新的Encode对象;
 * 返回Encode指针.
 */
struct Encode * encode_new__IO(/* 线程名 */const char threadName[]
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
                    if (pEncode_item->p._watermarkingCount > 0) {
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
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && 0 == pEncode_item->p._watermarkingCount) {
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
                    if (pEncode_item->p._watermarkingCount > 0) {
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
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && 0 == pEncode_item->p._watermarkingCount) {
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
                    if (pEncode_item->p._watermarkingCount > 0) {
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
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && 0 == pEncode_item->p._watermarkingCount) {
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
                    if (pEncode_item->p._watermarkingCount > 0) {
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
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && 0 == pEncode_item->p._watermarkingCount) {
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
                    if (pEncode_item->p._watermarkingCount > 0) {
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
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && 0 == pEncode_item->p._watermarkingCount) {
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
                    if (pEncode_item->p._watermarkingCount > 0) {
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
                        if (FsMacrosValue4(ImageFrame_valid_, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_tag, _, __encode_item_frame_in_pthreadSafety__OI_4_check_encode_size)(pFrame->dataValidType) && 0 == pEncode_item->p._watermarkingCount) {
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
    if (pEncode_item->p._watermarkingCount > 0) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
        /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
        /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
        /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
        /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
        /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check

        for (index = 0; index < frameCount;) {
            FsObjectImageFrame * const pFrame = ppFrame[index++];
            pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0 | decodeMask_set;
            pthread_mutex_lock(&((FsObject*) pFrame)->mutex);
            if (1 == image_frame_prapare(pFrame, ImageFrame_YUV420P_0)) {
                if (ImageFrame_valid_H264_0(pFrame->dataValidType))image_frame_clear_data(pFrame, ImageFrame_H264_0);
                if (ImageFrame_valid_H265_0(pFrame->dataValidType))image_frame_clear_data(pFrame, ImageFrame_H265_0);
                unsigned int watermarkingIndex = 0;
                for (; watermarkingIndex < pEncode_item->p._watermarkingCount; watermarkingIndex++) {
                    unsigned char * watermarkingData = (unsigned char*) pEncode_item->p.watermarking[watermarkingIndex]._watermarkingData;
                    ////////////////////////////////////////////////////
#define __encode_item_frame_in_pthreadSafety__OI_4_error 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time1 1
#define __encode_item_frame_in_pthreadSafety__OI_4_time1_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time1_gt 2
#define __encode_item_frame_in_pthreadSafety__OI_4_time10 2
#define __encode_item_frame_in_pthreadSafety__OI_4_time10_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time10_gt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time11 3
#define __encode_item_frame_in_pthreadSafety__OI_4_time11_lt 1
#define __encode_item_frame_in_pthreadSafety__OI_4_time11_gt 5
#define __encode_item_frame_in_pthreadSafety__OI_4_time12 4
#define __encode_item_frame_in_pthreadSafety__OI_4_time12_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time12_gt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time13 5
#define __encode_item_frame_in_pthreadSafety__OI_4_time13_lt 4
#define __encode_item_frame_in_pthreadSafety__OI_4_time13_gt 6
#define __encode_item_frame_in_pthreadSafety__OI_4_time2 6
#define __encode_item_frame_in_pthreadSafety__OI_4_time2_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time2_gt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time3 7
#define __encode_item_frame_in_pthreadSafety__OI_4_time3_lt 3
#define __encode_item_frame_in_pthreadSafety__OI_4_time3_gt 10
#define __encode_item_frame_in_pthreadSafety__OI_4_time4 8
#define __encode_item_frame_in_pthreadSafety__OI_4_time4_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time4_gt 9
#define __encode_item_frame_in_pthreadSafety__OI_4_time5 9
#define __encode_item_frame_in_pthreadSafety__OI_4_time5_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time5_gt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time6 10
#define __encode_item_frame_in_pthreadSafety__OI_4_time6_lt 8
#define __encode_item_frame_in_pthreadSafety__OI_4_time6_gt 12
#define __encode_item_frame_in_pthreadSafety__OI_4_time7 11
#define __encode_item_frame_in_pthreadSafety__OI_4_time7_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time7_gt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time8 12
#define __encode_item_frame_in_pthreadSafety__OI_4_time8_lt 11
#define __encode_item_frame_in_pthreadSafety__OI_4_time8_gt 13
#define __encode_item_frame_in_pthreadSafety__OI_4_time9 13
#define __encode_item_frame_in_pthreadSafety__OI_4_time9_lt 0
#define __encode_item_frame_in_pthreadSafety__OI_4_time9_gt 0
                    goto FsMacrosFunctionTag(7);
                    for (;;) {
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(error) >= 0
                        FsMacrosFunctionTag(FsMacrosFunction(error)) :;
                        {
                            break;
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time1) > 0 // 时间1,格式:2022-12-30/10:24:30.123
                        FsMacrosFunctionTag(FsMacrosFunction(time1)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time1");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof (Fs_date_year_string_max"-12-30/10:24:30.123"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%d-%02d-%02d/%02d:%02d:%02d.%03d", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday
                                        , ti.tm_hour, ti.tm_min, ti.tm_sec, (int) ((pFrame->capture_gmtTime - ts)*1000));
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time1_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time1_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time2) > 0 // 时间2,格式:2022-12-30 10:24:30
                        FsMacrosFunctionTag(FsMacrosFunction(time2)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time2");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof (Fs_date_year_string_max"-12-30 10:24:30"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%d-%02d-%02d %02d:%02d:%02d", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday
                                        , ti.tm_hour, ti.tm_min, ti.tm_sec);
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time2_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time2_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time3) > 0 // 时间3,格式:2022-12-30 10:24
                        FsMacrosFunctionTag(FsMacrosFunction(time3)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time3");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof (Fs_date_year_string_max"-12-30 10:24"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%d-%02d-%02d %02d:%02d", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday
                                        , ti.tm_hour, ti.tm_min);
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time3_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time3_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time4) > 0 // 时间4,格式:2022-12-30 10:24:30AM/PM
                        FsMacrosFunctionTag(FsMacrosFunction(time4)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time4");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof (Fs_date_year_string_max"-12-30 10:24:30AM/PM"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%d-%02d-%02d %02d:%02d:%02d%s", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday
                                        , ti.tm_hour > 12 ? ti.tm_hour - 12 : ti.tm_hour, ti.tm_min, ti.tm_sec, ti.tm_hour > 12 ? "PM" : "AM");
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time4_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time4_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time5) > 0 // 时间5,格式:2022-12-30 10:24AM/PM
                        FsMacrosFunctionTag(FsMacrosFunction(time5)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time5");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof (Fs_date_year_string_max"-12-30 10:24AM/PM"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%d-%02d-%02d %02d:%02d%s", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday
                                        , ti.tm_hour > 12 ? ti.tm_hour - 12 : ti.tm_hour, ti.tm_min, ti.tm_hour > 12 ? "PM" : "AM");
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time5_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time5_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time6) > 0 // 时间6,格式:12-30-22 10:24:30
                        FsMacrosFunctionTag(FsMacrosFunction(time6)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time6");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("12-30-22 10:24:30"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d:%02d", ti.tm_mon + 1, ti.tm_mday, ti.tm_year % 100
                                        , ti.tm_hour, ti.tm_min, ti.tm_sec);
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time6_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time6_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time7) > 0 // 时间7,格式:12-30-22 10:24
                        FsMacrosFunctionTag(FsMacrosFunction(time7)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time7");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("12-30-22 10:24"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d", ti.tm_mon + 1, ti.tm_mday, ti.tm_year % 100
                                        , ti.tm_hour, ti.tm_min);
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time7_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time7_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time8) > 0 // 时间8,格式:12-30-22 10:24:30AM/PM
                        FsMacrosFunctionTag(FsMacrosFunction(time8)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time8");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("12-30-22 10:24:30AM/PM"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d:%02d%s", ti.tm_mon + 1, ti.tm_mday, ti.tm_year % 100
                                        , ti.tm_hour > 12 ? ti.tm_hour - 12 : ti.tm_hour, ti.tm_min, ti.tm_sec, ti.tm_hour > 12 ? "PM" : "AM");
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time8_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time8_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time9) > 0 // 时间9,格式:12-30-22 10:24AM/PM
                        FsMacrosFunctionTag(FsMacrosFunction(time9)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time9");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("12-30-22 10:24AM/PM"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d%s", ti.tm_mon + 1, ti.tm_mday, ti.tm_year % 100
                                        , ti.tm_hour > 12 ? ti.tm_hour - 12 : ti.tm_hour, ti.tm_min, ti.tm_hour > 12 ? "PM" : "AM");
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time9_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time9_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time10) > 0 // 时间10,格式:30-12-22 10:24:30
                        FsMacrosFunctionTag(FsMacrosFunction(time10)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time10");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("30-12-22 10:24:30"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d:%02d", ti.tm_mday, ti.tm_mon + 1, ti.tm_year % 100
                                        , ti.tm_hour, ti.tm_min, ti.tm_sec);
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time10_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time10_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time11) > 0 // 时间11,格式:30-12-22 10:24
                        FsMacrosFunctionTag(FsMacrosFunction(time11)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time11");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("30-12-22 10:24"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d", ti.tm_mday, ti.tm_mon + 1, ti.tm_year % 100
                                        , ti.tm_hour, ti.tm_min);
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time11_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time11_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time12) > 0 // 时间12,格式:30-12-22 10:24:30AM/PM
                        FsMacrosFunctionTag(FsMacrosFunction(time12)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time12");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("30-12-22 10:24:30AM/PM"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d:%02d%s", ti.tm_mday, ti.tm_mon + 1, ti.tm_year % 100
                                        , ti.tm_hour > 12 ? ti.tm_hour - 12 : ti.tm_hour, ti.tm_min, ti.tm_sec, ti.tm_hour > 12 ? "PM" : "AM");
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time12_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time12_gt));
                        }
#endif
                        ////////////////////////////////////////////////////////
#if FsMacrosFunction(time13) > 0 // 时间13,格式:30-12-22 10:24AM/PM
                        FsMacrosFunctionTag(FsMacrosFunction(time13)) :;
                        {
                            int _rv = strcmp((char*) watermarkingData, "$$time13");
                            if (0 == _rv) {
                                FsLocal_ShareBuffer_resize(sizeof ("30-12-22 10:24AM/PM"));
                                watermarkingData = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
                                time_t ts = (time_t) pFrame->capture_gmtTime;
                                struct tm ti;
                                localtime_r(&ts, &ti);

                                sprintf((char*) watermarkingData, "%02d-%02d-%02d %02d:%02d%s", ti.tm_mday, ti.tm_mon + 1, ti.tm_year % 100
                                        , ti.tm_hour > 12 ? ti.tm_hour - 12 : ti.tm_hour, ti.tm_min, ti.tm_hour > 12 ? "PM" : "AM");
                                break;
                            } else if (_rv < 0)goto FsMacrosFunctionTag(FsMacrosFunction(time13_lt));
                            else goto FsMacrosFunctionTag(FsMacrosFunction(time13_gt));
                        }
#endif
                    }
                    ////////////////////////////////////////////////////////////     
                    fs_TypeFace_write(pEncode_item->p.watermarking[watermarkingIndex].__pTypeFace, watermarkingData, pFrame->data.yuv420p[0]->data, pFrame->width[0], pFrame->height[0],
                            pFrame->width[0], pEncode_item->p.watermarking[watermarkingIndex]._watermarking_startX, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_startY, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_color >> 16);
                    fs_TypeFace_write_uv(pEncode_item->p.watermarking[watermarkingIndex].__pTypeFace, watermarkingData, pFrame->data.yuv420p[0]->data + pFrame->sizeWidthHeight[0], pFrame->width[0], pFrame->height[0],
                            pFrame->width[0] >> 1, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_startX, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_startY, (pEncode_item->p.watermarking[watermarkingIndex]._watermarking_color >> 8)&0xFF);
                    fs_TypeFace_write_uv(pEncode_item->p.watermarking[watermarkingIndex].__pTypeFace, watermarkingData, pFrame->data.yuv420p[0]->data + pFrame->sizeWidthHeight[0]+(pFrame->sizeWidthHeight[0] >> 2), pFrame->width[0], pFrame->height[0],
                            pFrame->width[0] >> 1, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_startX, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_startY, pEncode_item->p.watermarking[watermarkingIndex]._watermarking_color & 0xFF);
                }
            }
            pthread_mutex_unlock(&((FsObject*) pFrame)->mutex);
            frameConvert_item_frame_in_pthreadSafety__OI_2(pFrameConvert_item, pFrame);
        }
        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
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
