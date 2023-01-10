#include "recognition.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <malloc.h> 
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Image/ImageExtern.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../Modules/plateRec/plateRec.h"
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

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int recognition_P_item_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */ struct Recognition_item * const pRecognition_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsStructList * const clientList = pRecognition_item->ro.__clientList;
    pthread_mutex_lock(&clientList->mutex);
    configManager_conncet_refer_check(pRecognition_item->ro._pRecognition->ro._pConfigManager, clientList);
    pthread_mutex_unlock(&clientList->mutex);
    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int recognition_P_item_cb_debug(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型 */ char requestDataType, /* 调用函数的指针 */struct Recognition_item * const pRecognition_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    const unsigned int datatype = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "datatype", 0);
    if (datatype > 0) {
        unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, datatype};
        pthread_mutex_lock(&pRecognition_item->ro.__clientList->mutex);
        configManager_conncet_refer_check(pRecognition_item->ro._pRecognition->ro._pConfigManager, pRecognition_item->ro.__clientList);
        if (fs_StructList_insert_order(pRecognition_item->ro.__clientList, data) < 0) {
            pthread_mutex_unlock(&pRecognition_item->ro.__clientList->mutex);
            return 1;
        } else {
            pthread_mutex_unlock(&pRecognition_item->ro.__clientList->mutex);
            return -128;
        }
    } else if (0 == datatype) {
        /* 获取车牌识别掩码图 */
        pthread_mutex_t * const pMutex = &pRecognition_item->p.plateRecognition.__list->mutex;
        pthread_mutex_lock(pMutex);
        FsObjectBasePthreadSafety * const mask = pRecognition_item->p.plateRecognition.__mask;
        if (mask) {
            fs_ObjectBasePthreadSafety_addRefer(mask);
            pthread_mutex_unlock(pMutex);
            FsObjectBase * ppObjectBase[3];
            ////////////////////////////////////////////////////////////////////
            unsigned char fillLen;
            unsigned int len = image_extern_gray_write_size_get(&fillLen, mask->len, sizeof ("tt") - 1);
            ppObjectBase[0] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + 8 + len, sizeof (FsObjectBasePthreadSafety));
            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[0]);
            /* 把pObjectBase[1]后面的数据加入缓存 */
            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[0], sizeof (FsObjectBasePthreadSafety) + 8 + len);
            ppObjectBase[0]->len = 8 + len;
            ////////////////////////////////////////////////////////////////////     
            ppObjectBase[1] = (FsObjectBase*) mask;
            len += mask->len;
            ////////////////////////////////////////////////////////////////////
            ppObjectBase[2] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + fillLen, sizeof (FsObjectBasePthreadSafety));
            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[2]);
            /* 把pObjectBase[1]后面的数据加入缓存 */
            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[2], sizeof (FsObjectBasePthreadSafety) + fillLen);
            ppObjectBase[2]->len = fillLen;
            image_extern_gray_write_head((unsigned char*) ppObjectBase[0]->data + 8, (unsigned char*) ppObjectBase[2]->data, 1, sizeof ("tt") - 1, "tt"
                    , pRecognition_item->p.plateRecognition.maskWidth, pRecognition_item->p.plateRecognition.maskHeight, pRecognition_item->p.plateRecognition.maskWidth, 0, 0, 0, 0, 1.0, 1.0, 8, ppObjectBase[1]->len);
            len += fillLen;
            ////////////////////////////////////////////////////////////////////
            ((unsigned int*) ppObjectBase[0]->data)[0] = (head & 0xFFFFFFF0U) | 0x0BU, ((unsigned int*) ppObjectBase[0]->data)[1] = len;
            configManager_connect_refer_send__OI_3(requestID_3, sizeof (ppObjectBase) / sizeof (ppObjectBase[0]), ppObjectBase);
            return 1;
        } else {
            pthread_mutex_unlock(pMutex);
            return -1;
        }
    } else {
        FsPrintf(FsPrintfIndex, "Invalid datatype(=%u).\n", datatype);
        fflush(stdout);
        return -1;
    }
}

static FsConfig *recognition_P_protocol_debug() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "recognition_debug", "recognition_debug", "recognition_debug");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "datatype", "数据类型", "数据类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 6, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "掩码图", "掩码图");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "车牌识别(图片流)", "车牌识别(图片流)");
    }

    return pConfig;
}

/* 清除pRecognition_item中车牌识别的参数 */
static void recognition_P_item_clear_parameter_plate(struct Recognition_item * const pRecognition_item) {
    pthread_mutex_t * const pMutex = &pRecognition_item->p.plateRecognition.__list->mutex;
    pthread_mutex_lock(pMutex);
    /* 掩码图,0-不检测,非0代表不同区域,大小使用检测图的1/16,为空时表示需要重新获取车牌识别的参数,使用__list的锁 */
    fs_ObjectBasePthreadSafety_delete__OI(pRecognition_item->p.plateRecognition.__mask), pRecognition_item->p.plateRecognition.__mask = NULL;
    pthread_mutex_unlock(pMutex);
}

/* 删除pRecognition_item指向的实例对象 */
static void recognition_P_item_delete__OI(struct Recognition_item * const pRecognition_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    void *const pConfigManager = pRecognition_item->ro._pRecognition->ro._pConfigManager;
    // 注销连接断开时的回调通知
    if (0 == pRecognition_item->ro._ipv4) configManager_connect_error_logoff(pConfigManager, (int (*)(const unsigned int *, void *, char * * const))recognition_P_item_cb_connect_error, pRecognition_item);
    /* 注销命令字 */
    if (0 == pRecognition_item->ro._ipv4) configManager_cmd_logoff(pConfigManager, "recognition_debug", pRecognition_item->ro._uuid, pRecognition_item->ro._ipv4, pRecognition_item, pShareBuffer);
    if (0 == pRecognition_item->ro._ipv4 && pRecognition_item->p.plateRecognition.__list)
        configManager_cmd_logoff(pConfigManager, "plate_recognition", pRecognition_item->ro._uuid, pRecognition_item->ro._ipv4, pRecognition_item, pShareBuffer);
    if (0 == pRecognition_item->ro._ipv4) {
        /* 传入的数据帧链表 */
        //fs_ObjectList_delete_destructor__OI(pRecognition_item->ro.__framelistIn);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pRecognition_item->ro.__framelistOut);
        /* 关闭所有与之相关的客户端链表 */
        configManager_conncet_refer_close(pConfigManager, pRecognition_item->ro.__clientList, 1);
        /* 与之相关的客户端链表 */
        fs_StructList_delete__OI(pRecognition_item->ro.__clientList);
        /* 车牌识别 */
        if (pRecognition_item->p.plateRecognition.__list) {
            /* 掩码图,0-不检测,非0代表不同区域,大小使用检测图的1/16,为空时表示需要重新获取车牌识别的参数 */
            if (pRecognition_item->p.plateRecognition.__mask) {
                recognition_P_item_clear_parameter_plate(pRecognition_item);
            }
            /* 车牌识别的任务链表,为空表示车牌识别未初始化,节点类型为struct Recognition_item_Plate */
            fs_ObjectList_delete_destructor__OI(pRecognition_item->p.plateRecognition.__list);
        };
    }
    fsFree(pRecognition_item);
}

static void recognition_P_item_new_plateRec(struct Recognition_item * const pRecognition_item, unsigned int index, unsigned short width, unsigned short height
        , /* 参考标定的宽度 */unsigned int width0, /* 参考标定的高度 */ unsigned int height0
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    width /= 4;
    height /= 4;
    FsObjectBasePthreadSafety * const mask = (FsObjectBasePthreadSafety*) fsMalloc(sizeof ( FsObjectBasePthreadSafety) + width * height);
    fs_ObjectBasePthreadSafety_init(mask, sizeof ( FsObjectBasePthreadSafety) + width * height, sizeof ( FsObjectBasePthreadSafety));
    mask->len = width * height;
    ConfigManager * const pConfigManager = (ConfigManager*) pRecognition_item->ro._pRecognition->ro._pConfigManager;
    configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
    FsConfig * const pConfig = pConfigManager->ro.__pConfig;
    const void *parent0;
    const void *parent;
    {
        parent0 = pConfig;
        FsObjectList * list = fs_Config_node_template__IO(pConfig, &parent0, pConfig,0, pConfigManager->ro.__ipList_local, 0, "vsys");
        if (list) {
            parent = list->pNode[list->startIndex];
            fs_ObjectList_delete__OI(list);
            list = fs_Config_node_template__IO(pConfig, &parent0, parent,0, NULL, 0, "vsysChannel");
            if (list) {
                if (index < list->nodeCount) {
                    parent = list->pNode[list->startIndex + index];
                    parent = fs_Config_node_get_first(pConfig, &parent0, parent, "recognitionPlateConfig");
                } else parent = NULL;
                fs_ObjectList_delete__OI(list);
            } else parent = NULL;
        } else parent = NULL;
    }
    if (parent) {
        FsConfigResult_area * const pArea = fs_Config_node_string_area_lenth_get__IO(pConfig, parent0, parent, "recognitionPlateErea area", NULL, width0 * 1.0 / width, height0 * 1.0 / height, 0, 0);
        if (pArea) {
            memset(mask->data, 0, width * height);
            fs_Config_node_string_area_fill_ch1(pArea, (unsigned char*) mask->data, width, height, width, 255, 0, pShareBuffer);
            fsFree(pArea);
        } else memset(mask->data, 255, width * height);
    } else {
        memset(mask->data, 255, width * height);
    }
    configManager_config_deleteRefer_pthreadSafety(pConfigManager);
    if (0) {
        ImageDib *pDib = image_dib_new__IO(width, height, ImageColorType_Gray, width);
        memcpy(pDib->data, mask, pDib->dataLenth);
        image_dib_svae_to_file(pDib, "/fs/project/data/1.bmp");
        image_dib_delete__OI(&pDib);
    }
    {
        pthread_mutex_t * const pMutex = &pRecognition_item->p.plateRecognition.__list->mutex;
        pthread_mutex_lock(pMutex);
        pRecognition_item->p.plateRecognition.__mask = mask;
        pRecognition_item->p.plateRecognition.maskWidth = width;
        pRecognition_item->p.plateRecognition.maskHeight = height;
        pthread_mutex_unlock(pMutex);
    }
}

static void recognition_P_item_new(struct Recognition * const pRecognition, /* 通道号,从1开始,为0用于集群 */const unsigned int channel
        , /* 本地ip地址 */const FsArray * const ipList, /* 原来的识别项 */struct Recognition_item * * const ppRecognition_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pRecognition->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, 0, ipList, 0 == channel, "vsys");
    if (clusterList) {
        const void **ppNodeCluster = (const void **) clusterList->pNode + clusterList->startIndex;
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
                        if (*ppRecognition_item != NULL && (*ppRecognition_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppRecognition_item) {
                        recognition_P_item_delete__OI(*ppRecognition_item, pShareBuffer);
                    }
                    ////////////////////////////////////////////////////////////
                    struct Recognition_item * rst;
                    if (Recognition_Mask != 0 && (Recognition_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            rst = (struct Recognition_item*) fsMalloc(Memery_Alignment(sizeof (struct Recognition_item)) + len);
                            memset(rst, 0, sizeof (struct Recognition_item));
                            pd = ((char*) rst) + Memery_Alignment(sizeof (struct Recognition_item));
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len);
                            /* 传入的数据帧链表 */
                            //rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 与之相关的客户端链表 */
                            rst->ro.__clientList = fs_StructList_new__IO(6, sizeof (int)*6);
                            /* 车牌识别 */
                            {
                                /* 车牌识别的任务链表,为空表示车牌识别未初始化,节点类型为struct Recognition_item_Plate */
                                rst->p.plateRecognition.__list = fs_ObjectList_new__IO(4);
                            };
                        } else {
                            //                        rst = (struct Record_item*) fsMalloc(Record_item_ClusterLen + len);
                            //                        memset(rst, 0, Record_item_ClusterLen);
                            //                        pd = ((char*) rst) + Record_item_ClusterLen;
                        }
                        if (channel > 0) {
                            /* 所属的车辆检测对象 */
                            rst->ro._pRecognition = pRecognition;
                            /* 整个通道依赖的所有数据的校验和 */
                            rst->ro._sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode);
                            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pRecognition->ro._pGroupSqrtOut->groupSqrt_member, pRecognition->ro._pGroupSqrtOut->groupSqrt_group
                                    , pRecognition->ro._pGroupSqrtOut->groupValue, channel - 1, &pRecognition->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        ////////////////////////////////////////////////////////
                        if (channel > 0) {
                            /* 帧率 */
                            rst->p._frameRate = 25.0;
                            ////////////////////////////////////////////////////
                            // 注册连接断开时的回调通知
                            configManager_connect_error_register(pRecognition->ro._pConfigManager
                                    , (int (*)(const unsigned int *, void *, char * * const))recognition_P_item_cb_connect_error, rst);
                        }
                        /* 注册命令字 */
                        if (0 == ipv4)configManager_cmd_register(pRecognition->ro._pConfigManager, "recognition_debug", rst->ro._uuid, ipv4, rst, 0
                                , (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))recognition_P_item_cb_debug, NULL, rst, pShareBuffer);
                        if (0 == ipv4 && rst->p.plateRecognition.__list != NULL)
                            configManager_cmd_register(pRecognition->ro._pConfigManager, "plate_recognition", rst->ro._uuid, 0, rst, 0, NULL, (void*) recognition_item_add_plateRecognition_pthreadSafety__IO, rst, pShareBuffer);
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
                    if (channel > 0) *ppRecognition_item = rst;
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

/* 从配置获取通道数 */
static unsigned int recognition_P_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

static inline void recognition_P_updateframeRate(struct Recognition_item * const pRecognition_item, /* 帧率 */ float frameRate) {
    pRecognition_item->p._frameRate += (frameRate - pRecognition_item->p._frameRate) / (pRecognition_item->p._frameRate + 1.0);
}

/* 发送车牌调试数据 */
static void recognition_P_item_send_plate_debug(struct Recognition_item * const pRecognition_item, struct Recognition_item_Plate * const pPlate
        , /* 车牌位置信息 */char cResult[], int retLen, /* 车牌结果信息,每个位置的车牌使用\0分隔 */char plateRst[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer) {
    FsStructList * const clientList = pRecognition_item->ro.__clientList;
    if (0 == clientList->nodeCount)return;
    FsObjectBase * ppObjectBase0[9];
    FsObjectBase **ppObjectBase = ppObjectBase0;
    unsigned int ppObjectBaseCount = sizeof (ppObjectBase0) / sizeof (ppObjectBase0[0]);
    unsigned long pos = 0;
    /* 调试数据类型掩码,1,2,4,8,16... */
    unsigned long debugTypeMask = 0;
    while (1) {
        unsigned int debugType;
        pthread_mutex_lock(&clientList->mutex);
        {
            unsigned int (*const ppNode) [6] = ((unsigned int (*)[6])clientList->pNode) + clientList->startIndex;
            for (; pos < clientList->nodeCount; pos++) {
                debugType = ppNode[pos][5];
                if ((debugTypeMask & (1LU << debugType)) == 0) {
                    debugTypeMask |= 1LU << debugType;
                    break;
                }
            }
        }
        if (pos >= clientList->nodeCount) {
            /* 无数据处理了 */
            pthread_mutex_unlock(&clientList->mutex);
            return;
        }
        pthread_mutex_unlock(&clientList->mutex);
        ////////////////////////////////////////////////////////////////////////  
        unsigned int ppObjectBaseUseCount;
        unsigned int head, len;
        /* 根据debugType生成数据 */
        switch (debugType) {
                /* 车牌识别结果 */
            case 1:
            {
                if (ppObjectBaseCount < 9) {
                    FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, 9), fflush(stdout);
                    if (ppObjectBase == ppObjectBase0) {
                        ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * 9);
                        //memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * 0);
                    } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * 9);
                    ppObjectBaseCount = 9;
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                FsObjectImageFrame * const pFrame = pPlate->_pFrame;
                {
                    {

                        ppObjectBase[2] = (FsObjectBase*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RGB_0);
                        if (NULL == ppObjectBase[2]) {
                            FsPrintf(FsPrintfIndex, "Prapare RGB failed.\n");
                            fflush(stdout);
                            head = 0;
                            break;
                        }
                        fs_Object_addRefer_pthreadSafety(ppObjectBase[2], __LINE__);
                    }

                    unsigned char fillLen;
                    len = image_extern_rgb_write_size_get(&fillLen, pFrame->sizeWidthHeight[0]*3, sizeof ("tt") - 1);
                    ppObjectBase[1] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + len, sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[1]);
                    /* 把pObjectBase[1]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[1], sizeof (FsObjectBasePthreadSafety) + len);
                    ppObjectBase[1]->len = len;

                    ppObjectBase[3] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + fillLen, sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[3]);
                    /* 把pObjectBase[3]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[3], sizeof (FsObjectBasePthreadSafety) + fillLen);
                    ppObjectBase[3]->len = fillLen;
                    image_extern_rgb_write_head((unsigned char*) ppObjectBase[1]->data, (unsigned char*) ppObjectBase[3]->data, 1, sizeof ("tt") - 1, "tt"
                            , pFrame->width[0], pFrame->height[0], pFrame->width[0]*3, 0, 0, 0, 0, 1.0, 1.0, pFrame->sizeWidthHeight[0]*3);
                    len += fillLen + ppObjectBase[2]->len;
                }
                ////////////////////////////////////////////////////////////////
                /* 画车辆位置 */
                {
                    ppObjectBaseUseCount++;
                    {
                        const unsigned int size = image_extern_line_write_size_get(NULL, 4, sizeof ("tt") - 1);
                        ppObjectBase[4] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + size, sizeof (FsObjectBasePthreadSafety));
                        fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[4]);
                        /* 把pObjectBase[4]后面的数据加入缓存 */
                        fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[4], sizeof (FsObjectBasePthreadSafety) + size);
                        ppObjectBase[4]->len = size;
                        len += size;
                    }
                    {
                        unsigned int * pd = (unsigned int*) image_extern_line_write_head((unsigned char*) ppObjectBase[4]->data, NULL, 1, sizeof ("tt") - 1, "tt"
                                , pFrame->width[0], pFrame->height[0], 0x0000FF00, 1.0, 1.0, 4);
                        const int x1 = pPlate->_x1, y1 = pPlate->_y1, x2 = pPlate->_x2, y2 = pPlate->_y2;
                        // (x1,y1)(x2,y1)
                        pd[0] = x1, pd[1] = y1, pd[2] = x2, pd[3] = y1, pd += 4;
                        // (x2,y1)(x2,y2)
                        pd[0] = x2, pd[1] = y1, pd[2] = x2, pd[3] = y2, pd += 4;
                        // (x2,y2)(x1,y2)
                        pd[0] = x2, pd[1] = y2, pd[2] = x1, pd[3] = y2, pd += 4;
                        // (x1,y2)(x1,y1)
                        pd[0] = x1, pd[1] = y2, pd[2] = x1, pd[3] = y1, pd += 4;
                    }
                }
                ////////////////////////////////////////////////////////////////
                /* 画车牌位置 */
                {
                    ppObjectBaseUseCount += 3;
                    {
                        /* 最多显示100个对象 */
                        int showCount = 100;
                        ppObjectBase[6] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + 16 * 4 * showCount, sizeof (FsObjectBasePthreadSafety));
                        fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[6]);
                        int i;
                        unsigned int *pd = (unsigned int*) ppObjectBase[6]->data;
#if 1
                        for (i = 0; i < retLen && showCount > 0; i++, showCount--) {
                            int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
                            // x1
                            for (; i < retLen; i++) {
                                if (cResult[i] >= '0' && cResult[i] <= '9') {
                                    x1 = x1 * 10 + (cResult[i] - '0');
                                } else break;
                            }
                            if (i >= retLen)break;
                            // y1
                            for (i++; i < retLen; i++) {
                                if (cResult[i] >= '0' && cResult[i] <= '9') {
                                    y1 = y1 * 10 + (cResult[i] - '0');
                                } else break;
                            }
                            if (i >= retLen)break;
                            // x2
                            for (i++; i < retLen; i++) {
                                if (cResult[i] >= '0' && cResult[i] <= '9') {
                                    x2 = x2 * 10 + (cResult[i] - '0');
                                } else break;
                            }
                            if (i >= retLen)break;
                            // y2
                            for (i++; i < retLen; i++) {
                                if (cResult[i] >= '0' && cResult[i] <= '9') {
                                    y2 = y2 * 10 + (cResult[i] - '0');
                                } else break;
                            }
                            if (i >= retLen)break;
                            ////////////////////////////////////////////////////////
                            // FsPrintf(1, "(%d,%d)(%d,%d)\n", x1, y1, x2, y2);
                            x1 += pPlate->_x1, y1 += pPlate->_y1, x2 += pPlate->_x1, y2 += pPlate->_y1;
                            // (x1,y1)(x2,y1)
                            pd[0] = x1, pd[1] = y1, pd[2] = x2, pd[3] = y1, pd += 4;
                            // (x2,y1)(x2,y2)
                            pd[0] = x2, pd[1] = y1, pd[2] = x2, pd[3] = y2, pd += 4;
                            // (x2,y2)(x1,y2)
                            pd[0] = x2, pd[1] = y2, pd[2] = x1, pd[3] = y2, pd += 4;
                            // (x1,y2)(x1,y1)
                            pd[0] = x1, pd[1] = y2, pd[2] = x1, pd[3] = y1, pd += 4;
                            ////////////////////////////////////////////////////
                            // 解析识别结果
                            {
                                int plateLen;
                                for (plateLen = 0; plateRst[plateLen] != 0; plateLen++);
                                if (plateLen != 0) {
                                    if (ppObjectBaseUseCount == ppObjectBaseCount) {
                                        FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, ppObjectBaseCount + 1), fflush(stdout);
                                        if (ppObjectBase == ppObjectBase0) {
                                            ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * (ppObjectBaseCount + 1));
                                            memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * ppObjectBaseCount);
                                        } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * (ppObjectBaseCount + 1));
                                        ppObjectBaseCount++;
                                    }
                                    {
                                        const unsigned int size = image_extern_text_write_size_get(NULL, plateLen, sizeof ("tt") - 1);
                                        ppObjectBase[ppObjectBaseUseCount] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + size, sizeof (FsObjectBasePthreadSafety));
                                        fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[ppObjectBaseUseCount]);
                                        /* 把pObjectBase[ppObjectBaseUseCount]后面的数据加入缓存 */
                                        fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[ppObjectBaseUseCount], sizeof (FsObjectBasePthreadSafety) + size);
                                        ppObjectBase[ppObjectBaseUseCount]->len = size;
                                        len += size;
                                    }
                                    {
                                        unsigned char * const pd = image_extern_text_write_head((unsigned char*) ppObjectBase[ppObjectBaseUseCount]->data, NULL, 1, sizeof ("tt") - 1, "tt"
                                                , pFrame->width[0], pFrame->height[0], 0x000000FF, x1, y2, 32, 32, plateLen);
                                        memcpy(pd, plateRst, plateLen);
                                    }
                                    ppObjectBaseUseCount++;
                                }
                                plateRst += plateLen + 1;
                            }
                        }
#endif
                        ppObjectBase[6]->len = ((char*) pd) - ppObjectBase[6]->data;
                        if (0 == ppObjectBase[6]->len) {
                            pd[0] = 0, pd[1] = 0, pd[2] = 0, pd[3] = 0;
                            ppObjectBase[6]->len = sizeof (*pd)*4;
                        }
                        /* 把pObjectBase[6]后面的数据加入缓存 */
                        fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[6], sizeof (FsObjectBasePthreadSafety) + ppObjectBase[6]->len);
                    }
                    //                pObjectBase[5] = (FsObjectBase*) carDetect_debug__IO(pTargetCheck_item->p.__pCarDetect, 4, sizeof (FsObjectBasePthreadSafety), 0);
                    //                if (NULL == pObjectBase[5]) break;                    
                    {
                        unsigned char fillLen;
                        const unsigned int lineCount = ppObjectBase[6]->len / 16;
                        unsigned int lineHeadLen = image_extern_line_write_size_get(&fillLen, lineCount, sizeof ("tt") - 1);
                        ppObjectBase[5] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + lineHeadLen, sizeof (FsObjectBasePthreadSafety));
                        fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[5]);
                        /* 把pObjectBase[5]后面的数据加入缓存 */
                        fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[5], sizeof (FsObjectBasePthreadSafety) + lineHeadLen);
                        ppObjectBase[5]->len = lineHeadLen;

                        ppObjectBase[7] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + fillLen, sizeof (FsObjectBasePthreadSafety));
                        fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[7]);
                        /* 把pObjectBase[7]后面的数据加入缓存 */
                        fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[7], sizeof (FsObjectBasePthreadSafety) + fillLen);
                        ppObjectBase[7]->len = fillLen;
                        image_extern_line_write_head((unsigned char*) ppObjectBase[5]->data, (unsigned char*) ppObjectBase[7]->data, 1, sizeof ("tt") - 1, "tt"
                                , pFrame->width[0], pFrame->height[0], 0x000000FF, 1.0, 1.0, lineCount);
                        len += lineHeadLen + ppObjectBase[6]->len + fillLen;
                    }
                }
            }
                break;
            default:
                head = 0;
                break;
        }
        if (0 == head)continue;
        ////////////////////////////////////////////////////////////////////////
        /* 遍历连接,发送数据 */
        pthread_mutex_lock(&clientList->mutex);
        {
            unsigned int (*const ppNode) [6] = ((unsigned int (*)[6])clientList->pNode) + clientList->startIndex;
            unsigned long i;
            for (i = pos; i < clientList->nodeCount; i++) {
                if (debugType == ppNode[i][5]) {
                    ppObjectBase[0] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBase) + 8, sizeof (FsObjectBase));
                    /* 把pObjectBase[0]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[0], sizeof (FsObjectBase) + 8);
                    ppObjectBase[0]->len = 8;
                    ////////////////////////////////////////////////////////////
                    ((unsigned int*) ppObjectBase[0]->data)[0] = (ppNode[i][3]&0xFFFFFFF0U) | head, ((unsigned int*) ppObjectBase[0]->data)[1] = len;
                    unsigned int j;
                    for (j = 1; j < ppObjectBaseUseCount; j++) fs_ObjectBasePthreadSafety_addRefer(ppObjectBase[j]);
                    configManager_connect_refer_send__OI_3(ppNode[i], ppObjectBaseUseCount, ppObjectBase);
                }
            }
        }
        pthread_mutex_unlock(&clientList->mutex);
        {
            unsigned int j;
            for (j = 1; j < ppObjectBaseUseCount; j++) fs_ObjectBasePthreadSafety_delete__OI(ppObjectBase[j]);
        }
    }
    if (ppObjectBase != ppObjectBase0)fsFree(ppObjectBase);
}

/* 车牌识别,成功返回1,失败返回-1 */
static inline int recognition_P_item_do_plate(struct Recognition_item * const pRecognition_item, struct Recognition_item_Plate * const pPlate
        , FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName recognition_P_item_do_plate
    FsPrintf(FsPrintfIndex, "pPlate=%p,tag=%hhu.\n", pPlate, pPlate->_tag);
    FsObjectImageFrame * const pFrame = pPlate->_pFrame;
    //    /* 检查车是否在识别范围内 */
    //    {
    //        if (0 == ((unsigned char*) pRecognition_item->p.plateRecognition.__mask->data)[
    //                ((((pPlate->_y1 + pPlate->_y2) / 2) * pRecognition_item->p.plateRecognition.maskHeight) / pFrame->height[0]) * pRecognition_item->p.plateRecognition.maskWidth
    //                + (((pPlate->_x1 + pPlate->_x2) / 2) * pRecognition_item->p.plateRecognition.maskWidth) / pFrame->width[0]]) {
    //            pPlate->plateNo[0] = 0;
    //            pPlate->_tag = 1;
    //            return 1;
    //        }
    //    }
    FsObjectImageRGB * const pRGB = (FsObjectImageRGB*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RGB_0);
    if (NULL == pRGB) {
        FsPrintf2(FsPrintfIndex, "prapare RGB failed.\n");
        pPlate->_tag = 1;
        return -1;
    }
    FsPrintf(1, "ttttttt\n");
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
    FsPrintf(1, "ttttttt\n");
    int platePositionLen = 1024;
    FsLocal_ShareBuffer_resize(platePositionLen + 1);
    char *cResult = FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
    {
        const int x1 = pPlate->_x1, y1 = pPlate->_y1, x2 = pPlate->_x2, y2 = pPlate->_y2;
        licensePlateDetect(y2 - y1 + 1, x2 - x1 + 1, pFrame->width[0] * 3, pRGB->data + pFrame->width[0] * 3 * y1 + x1 * 3
                , pFrame->width[0] * 3 * (y2 - y1 + 1), &cResult, &platePositionLen);
        if (platePositionLen > 1024) {
            FsPrintf(FsPrintfIndex, "Invalid retLen(=%d).\n", platePositionLen);
            fflush(stdout);
        }
        cResult[platePositionLen] = 0;
        //FsPrintf(FsPrintfIndex, "x1=%d,y1=%d,x2=%d,y2=%d,cResult[%d]:%s\n", x1, y1, x2, y2, platePositionLen, cResult), fflush(stdout);
    }

    /* 识别车牌 */
    // 识别的车牌坐标 
    int xd1, yd1, xd2, yd2;
    // 车牌号(相对FsLocal_ShareBuffer的偏移量)
    unsigned int plateNo = 0;
    // 车牌长度,为0表示无效
    unsigned int plateNoLen = 0;
    {
        int i;
        // 车牌储存的位置相对FsLocal_ShareBuffer的偏移量
        int plateRstLen = 0;
        const int x0 = pPlate->_x1, y0 = pPlate->_y1;
        for (i = 0; i < platePositionLen; i++) {
            int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            // x1
            for (; i < platePositionLen; i++) {
                if (cResult[i] >= '0' && cResult[i] <= '9') {
                    x1 = x1 * 10 + (cResult[i] - '0');
                } else break;
            }
            if (i >= platePositionLen)break;
            // y1
            for (i++; i < platePositionLen; i++) {
                if (cResult[i] >= '0' && cResult[i] <= '9') {
                    y1 = y1 * 10 + (cResult[i] - '0');
                } else break;
            }
            if (i >= platePositionLen)break;
            // x2
            for (i++; i < platePositionLen; i++) {
                if (cResult[i] >= '0' && cResult[i] <= '9') {
                    x2 = x2 * 10 + (cResult[i] - '0');
                } else break;
            }
            if (i >= platePositionLen)break;
            // y2
            for (i++; i < platePositionLen; i++) {
                if (cResult[i] >= '0' && cResult[i] <= '9') {
                    y2 = y2 * 10 + (cResult[i] - '0');
                } else break;
            }
            if (i >= platePositionLen)break;
            ////////////////////////////////////////////////////////////////////
            // FsPrintf(1, "(%d,%d)(%d,%d)\n", x1, y1, x2, y2);   
            x1 += x0, y1 += y0, x2 += x0, y2 += y0;
            int retLen1 = 1024;
            FsLocal_ShareBuffer_resize(platePositionLen + 1 + plateRstLen + retLen1 + 1);
            cResult = FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
            char *cResult1 = cResult + platePositionLen + 1 + plateRstLen;
            memset(cResult1, 0, retLen1);
            licensePlateRecog(y2 - y1 + 1, x2 - x1 + 1, pFrame->width[0] * 3, pRGB->data + pFrame->width[0] * 3 * y1 + x1 * 3
                    , pFrame->width[0] * 3 * (y2 - y1 + 1), &cResult1, &retLen1);
            if (retLen1 > 1024) {
                FsPrintf(FsPrintfIndex, "Invalid retLen(=%d).\n", retLen1);
                fflush(stdout);
            }
            cResult1[retLen1] = 0;
            /* 校验车牌的有效性 */
            {
                int j = 0;
                for (; j < retLen1 && cResult1[j] != ','; j++);
                if (',' == cResult1[j]) {
                    plateNo = cResult1 - FsLocal_ShareBuffer;
                    plateNoLen = j;
                    xd1 = x1, yd1 = y1, xd2 = x2, yd2 = y2;
                }
            }
            FsPrintf(FsPrintfIndex, "x1=%d,y1=%d,x2=%d,y2=%d,cResult1[%d]:%s\n", x1, y1, x2, y2, retLen1, cResult1), fflush(stdout);
            plateRstLen += retLen1 + 1;
        }
    }
    fs_Object_delete_pthreadSafety__OI(pRGB);

    if (plateNoLen != 0) {
        /* 车牌已识别 */
        if (plateNoLen >= sizeof (pPlate->plateNo)) {
            FsPrintf(FsPrintfIndex, "plateNoLen(=%d/%lu) too long.\n", plateNoLen, sizeof (pPlate->plateNo));
            fflush(stdout);
            plateNoLen = sizeof (pPlate->plateNo) - 1;
        }
        memcpy(pPlate->plateNo, FsLocal_ShareBuffer + plateNo, plateNoLen), pPlate->plateNo[plateNoLen] = 0;
        /* 车牌位置 */
        pPlate->platepos_x1 = xd1, pPlate->platepos_y1 = yd1, pPlate->platepos_x2 = xd2, pPlate->platepos_y2 = yd2;
        pPlate->_tag = 0x3;
    } else pPlate->_tag = 1;
    /* 发送调试数据到客户端 */
    recognition_P_item_send_plate_debug(pRecognition_item, pPlate, cResult, platePositionLen, cResult + platePositionLen + 1, pObjectBaseBuffer);

    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    return 1;
}

static void *recognition_P_T(struct Recognition * const pRecognition) {
#undef FsFunctionName
#define FsFunctionName recognition_P_T
    /* 创建线程监控 */
    void *pMonitor = pRecognition->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, Recognition_get_threadName(pRecognition), NULL, pRecognition, pRecognition->ro._pMonitor != NULL ? pRecognition->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pRecognition->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pRecognition->ro._pConfigManager;
    FsObjectList * const itemList_ = pRecognition->ro.__itemList_;
    ////////////////////////////////////////////////////////////////////////////
    /* 格式化变量 */
    FsShareBuffer shareBuffer = NULL; // 共享buffer,本函数不使用,只在子函数中使用
#define __recognition_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    //    FsObjectSingleBuffer singleBuffer; // 发送数据的缓存
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __recognition_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可跨线程的缓存buffer,一般用于收数据或传递数据到其他线程 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __recognition_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);
#define __recognition_P_T_clean1_count 3
    ////////////////////////////////////////////////////////////////////////////    
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    float frameRateAvg = 25.0;
    double sleepToTime = 0.0;
    // 上一次被处理的通道编号
    unsigned int lastDoIndex = 0;
    // 还能处理的数量
    unsigned short doCount = 0;
    // 最多一次处理的数量
    unsigned short doCount0 = 0;
    // 车牌识别初始化是否被调用
    char plateHasInit = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    //    CarDetect *pCarDetect;
    //    FsObjectImageFrame *pFrame, *pFrame1, *pFrame2;
    //    /* 输出目标id */
    //    unsigned int id[32], x1[32], y1[32], x2[32], y2[32];
    //    unsigned int frameIterval, state;
    //    int rv;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    configManager_protocol_publish(pConfigManager, "recognition_debug", "识别调试", 1, recognition_P_protocol_debug);
#define __recognition_P_T_clean2_1 {configManager_protocol_cancel(pConfigManager, "recognition_debug");}
#define __recognition_P_T_clean2_count 1
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pRecognition->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        pMonitorItem->rw.line = __LINE__;
        if (fs_thread_sleepTo(sleepToTime) == 1) {
            if (0 == doCount)doCount0++;
        } else if (doCount != 0 && doCount0 > 1)doCount0--;
        doCount = doCount0;
        const double thisTime = fs_time_uptime_get();
        if (frameRateAvg < 5.0)frameRateAvg = 5.0;
        else frameRateAvg *= 0.9999;
        sleepToTime = thisTime + 1.0 / frameRateAvg;
        ////////////////////////////////////////////////////////////////////////
        if (pRecognition->rw._resetChannel>-1) {
            if (frameRateAvg < 65535.0)frameRateAvg *= 4.0;
            /* 读取通道数 */
            if (0 == pRecognition->rw._resetChannel) channelCount = recognition_P_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 删除通道 */
            while (itemList_->nodeCount > channelCount) {
                struct Recognition_item * const pRecognition_item = (struct Recognition_item *) fs_ObjectList_remove_tail(itemList_);
                if (pRecognition_item)recognition_P_item_delete__OI(pRecognition_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pRecognition->rw._resetChannel) {
                /* 出错 */
                FsPrintf(FsPrintfIndex, "Invalid channel(=%u/%lu).\n", pRecognition->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pRecognition->rw._resetChannel > 0) {
                FsPrintf(FsPrintfIndex, "%d\n", pRecognition->rw._resetChannel);
                pMonitorItem->rw.line = __LINE__;
                recognition_P_item_new(pRecognition, pRecognition->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Recognition_item **) (itemList_->pNode + itemList_->startIndex + pRecognition->rw._resetChannel - 1),
                        &shareBuffer);
                pMonitorItem->rw.line = __LINE__;
            }
            pRecognition->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */
        {
            struct Recognition_item ** ppRecognition_itemNode = (struct Recognition_item **) itemList_->pNode + itemList_->startIndex;
            const unsigned int count = itemList_->nodeCount;
            if (lastDoIndex >= count)lastDoIndex = 0;
            for (ppRecognition_itemNode += lastDoIndex; lastDoIndex < count; lastDoIndex++) {
                struct Recognition_item * const pRecognition_item = *ppRecognition_itemNode++;
                if (NULL == pRecognition_item)continue;
                while (doCount > 0) {
                    char needDo = 0;
                    ////////////////////////////////////////////////////////////
                    // 车牌识别
                    pMonitorItem->rw.line = __LINE__;
                    if (pRecognition_item->p.plateRecognition.__list->nodeCount > 0) {
                        needDo = 1;
                        pMonitorItem->rw.line = __LINE__;
                        struct Recognition_item_Plate * const pPlate = (struct Recognition_item_Plate *) fs_ObjectList_remove_head_pthreadSafety(pRecognition_item->p.plateRecognition.__list);
                        pMonitorItem->rw.line = __LINE__;
                        if (0 == pPlate->_tag) {
                            /* 未识别 */
                            if (pRecognition_item->ro.classIndex != pPlate->_pFrame->classIndex) {
                                pRecognition_item->ro.classIndex = pPlate->_pFrame->classIndex;
                                /* 重置 */
                                if (pRecognition_item->p.plateRecognition.__mask) {
                                    recognition_P_item_clear_parameter_plate(pRecognition_item);
                                }
                            }
                            if (NULL == pRecognition_item->p.plateRecognition.__mask) {
                                if (0 == plateHasInit) {
                                    FsPrintf2(FsPrintfIndex, "Run InitLPD start.\n");
                                    if (InitLPD1() == 1)plateHasInit |= 1;
                                    else {
                                        FsPrintf(FsPrintfIndex, "InitLPD1 failed.\n");
                                        fflush(stdout);
                                    }
                                    FsPrintf2(FsPrintfIndex, "Run InitLPD end.\n");
                                    if (plateHasInit) {
                                        FsPrintf2(FsPrintfIndex, "Run InitLPR start.\n");
                                        if (InitLPR1() == 1)plateHasInit |= 2;
                                        else {
                                            FsPrintf(FsPrintfIndex, "InitLPR1 failed.\n");
                                            fflush(stdout);
                                        }
                                        FsPrintf2(FsPrintfIndex, "Run InitLPR end.\n");
                                    }
                                    if (3 == plateHasInit)plateHasInit = 1;
                                    else {
                                        if (plateHasInit & 0x1)UnInitLPD();
                                        if (plateHasInit & 0x2)UnInitLPR();
                                        plateHasInit = 0;
                                    }
                                }
                                if (plateHasInit) recognition_P_item_new_plateRec(pRecognition_item, lastDoIndex
                                        , pPlate->_pFrame->width[0], pPlate->_pFrame->height[0], pPlate->_pFrame->width[0], pPlate->_pFrame->height[0], &shareBuffer);
                            }
                            if (doCount > 0)doCount--;
                            pMonitorItem->rw.line = __LINE__;
                            if (plateHasInit != 0) {
                                if (recognition_P_item_do_plate(pRecognition_item, pPlate, &baseBuffer, &shareBuffer) != 1) {
                                    FsPrintf(FsPrintfIndex, "%u/%lu:do_plate failed.\n", lastDoIndex, itemList_->nodeCount);
                                    fflush(stdout);
                                }
                            } else {
                                FsPrintf(FsPrintfIndex, "plateHasInit=%hhd.\n", plateHasInit);
                                fflush(stdout);
                                pPlate->_tag |= 0x1;
                            }
                            pMonitorItem->rw.line = __LINE__;
                        }
                        FsPrintf(FsPrintfIndex, "pPlate=%p,tag=%hhu,referCount=%d,plateHasInit=%hhx,mask=%p.\n", pPlate, pPlate->_tag, fs_ObjectBase_referCount_get(pPlate), plateHasInit, pRecognition_item->p.plateRecognition.__mask);
                        //pPlate->_tag |= 0x1;
                        ((FsObjectBase *) pPlate)->_delete(pPlate);
                    }
                    pMonitorItem->rw.line = __LINE__;
                    ////////////////////////////////////////////////////////////
                    if (0 == needDo)break;
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    if (pRecognition->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pRecognition->rw._resetChannel);
        fflush(stdout);
    }
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    pMonitorItem->rw.line = __LINE__;
    //________________________________________________________________________// 
    if (plateHasInit) {
        FsPrintf2(FsPrintfIndex, "Run UnInitLPD start.\n");
        UnInitLPD();
        FsPrintf2(FsPrintfIndex, "Run UnInitLPD end.\n");
        FsPrintf2(FsPrintfIndex, "Run UnInitLPR start.\n");
        UnInitLPR();
        FsPrintf2(FsPrintfIndex, "Run UnInitLPR end.\n");
    }
    /* 删除各通道 */
    {
        struct Recognition_item ** ppRecognition_itemNode = (struct Recognition_item **) itemList_->pNode + itemList_->startIndex;
        unsigned int ui = itemList_->nodeCount;
        for (; ui > 0; ui--) {
            struct Recognition_item * const pRecognition_item = *ppRecognition_itemNode++;
            if (pRecognition_item)recognition_P_item_delete__OI(pRecognition_item, &shareBuffer);
        }
        fs_ObjectList_clear(itemList_);
        //        /* 删除集群 */
        //        if (pCapture->p.clusterItemList) {
        //            fs_ObjectList_delete_custom__OI(pCapture->p.clusterItemList, (void (*)(void*))capture_P_item_delete__OI);
        //            pCapture->p.clusterItemList = NULL;
        //        }
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 释放全局变量空间 */
    /////////////////////
    /* 释放引用变量空间 */
    ////////////////////////////////////////////////////////////////////////////
    /* 释放中间变量空间 */
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsPrintf2(FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", Recognition_get_threadName(pRecognition), pRecognition->rw.runStatus);
#endif
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pRecognition->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void recognition_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "recognitionPlateConfig", "车牌识别配置", "车牌识别配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "32");
    fs_Config_node_add_property_area(pConfig, parent, "recognitionPlateErea", "area", 0x000000FF);
    fs_Config_node_add_property_image(pConfig, parent, 1, "uuid", "recordConfig rtspServerURL");
    //    /* 车辆检测阈值  */
    //    void* pNode = fs_Config_node_integer_add(pConfig, parent, "carThreshold", "车辆检测阈值", "车辆检测阈值", FsConfig_nodeShowType_default, 0, 0x7, 1, 32767, 1);
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 20, "20", "20");
    // 创建检测区
    {
        void* const pNode = fs_Config_node_node_add(pConfig, parent, "recognitionPlateErea", "车牌识别区", "车牌识别区", 0, 0x7);
        fs_Config_node_string_add(pConfig, pNode, "area", "区域", "区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 1);
    }
}

/* 检查pRecognition模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int recognition_check_channel_changed(struct Recognition * const pRecognition, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Recognition
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
 * 创建一个新的Recognition对象;
 * 返回Recognition指针.
 */
struct Recognition *recognition_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(threadName) + 1;
    struct Recognition * const rst = (struct Recognition*) fsMalloc(Memery_Alignment(sizeof (struct Recognition)) + len);
    memset(rst, 0, sizeof (struct Recognition));
    char *pd = ((char*) (rst)) + Memery_Alignment(sizeof (struct Recognition));
    /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
    rst->ro._pMonitor = pMonitor;
    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    if (externRunStatus) rst->ro._externRunStatus = externRunStatus;
    else rst->ro._externRunStatus = &defaultRunStatus;
    /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
    rst->ro._parentTid = parentTid;
    /* 线程名 */
    rst->ro._threadName_ = pd - (char*) (rst + 1), len = strlen(threadName) + 1, memcpy(pd, threadName, len);
    /* 配置管理对象 */
    rst->ro._pConfigManager = pConfigManager;
    /* 通道链表 */
    rst->ro.__itemList_ = fs_ObjectList_new_unsafe__IO(8);
    /* 在有输出数据时向哪个开方分组写入信号 */
    rst->ro._pGroupSqrtOut = pGroupSqrtOut;
    /* 重置通道,负数表示重置完成,初始化为-1 */
    rst->rw._resetChannel = -1;

    return rst;
}

/* 删除pRecognition指向的实例对象 */
void recognition_delete__OI(struct Recognition * const pRecognition) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pRecognition->ro.__itemList_);

    fsFree(pRecognition);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void recognition_startThread(struct Recognition * const pRecognition,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (0 == pRecognition->rw.runStatus && 0 == pRecognition->ro.stid) {
        pRecognition->rw.runStatus = 1;
#ifdef FsDebug
        RecognitionPrintf2("Init Parameter");
#endif
#define __fs_thread_attr_stacksize 8*1024*1024
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
        if (pthread_create(&pRecognition->ro.stid, &attr, (void* (*)(void*))recognition_P_T, pRecognition)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {
        RecognitionPrintf("The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter", pRecognition->rw.runStatus, pRecognition->ro.stid);
        fflush(stdout);
    }
}

/* 结束线程 */
void recognition_stopThread(struct Recognition * const pRecognition) {
    pRecognition->rw.runStatus = 0;
    if (pRecognition->ro.stid) {
#ifdef FsDebug
        FsPrintf2(FsPrintfIndex, "Stop Start.\n");
#endif
        pthread_join(pRecognition->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pRecognition->ro.stid);
        pRecognition->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(FsPrintfIndex, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void recognition_item_frame_in_pthreadSafety__OI_4(struct Recognition * const pRecognition, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    register struct Recognition_item * const pRecognition_item = (struct Recognition_item *) pRecognition->ro.__itemList_->pNode[pRecognition->ro.__itemList_->startIndex + index];
#ifdef FsDebug
    if (NULL == pRecognition_item) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
    //    if (pTargetCheck_item->ro.__framelistIn->nodeCount > 15) {
    //        FsPrintf2(1, "FrameListIn too long(=%lu).\n", pTargetCheck_item->ro.__framelistIn->nodeCount);
    //    }
#endif
    pthread_mutex_lock(&pRecognition_item->ro.__framelistOut->mutex);
    fs_ObjectList_insert_tail_array(pRecognition_item->ro.__framelistOut, frameCount, (void**) ppFrame);
    pthread_mutex_unlock(&pRecognition_item->ro.__framelistOut->mutex);
    Fs_GroupSqrtOut_value_set(pRecognition_item->rw._pGroupSqrtOut, &pRecognition_item->ro._pRecognition->ro._pGroupSqrtOut->groupSqrt_mutex);
}

/* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
unsigned int recognition_item_frame_out_pthreadSafety__IO_4(struct Recognition * const pRecognition, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server Recognition
#ifndef __item_frame_out_pthreadSafety__IO_4 
    struct /* Recognition_item */ FsMacrosValue2(__item_frame_out_pthreadSafety__IO_4_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__item_frame_out_pthreadSafety__IO_4_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __item_frame_out_pthreadSafety__IO_4_Server)->ro.
#ifdef __item_frame_out_pthreadSafety__IO_4_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __item_frame_out_pthreadSafety__IO_4_Server)->ro.
#ifdef __item_frame_out_pthreadSafety__IO_4_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
#ifdef FsDebug
    if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
    //    if (/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount > frameCount) {
    //        FsLog(FsLogType_matchError, FsPrintfIndex, "Can out frame(=%lu/%u) too big,index=%u.\n", /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount, frameCount, index);
    //        fflush(stdout);
    //    }
#endif
    if (/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount > 0) {
        pthread_mutex_lock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->mutex);
        index = /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount;
        if (index > frameCount)index = frameCount;
        FsObjectImageFrame **ppNode = (FsObjectImageFrame**) /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->pNode + /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->startIndex;
        for (frameCount = index; frameCount > 0; frameCount--) *ppFrame++ = *ppNode++;
        fs_ObjectList_remove_head_n(/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut, index);
        pthread_mutex_unlock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->mutex);
        return index;
    }
    return 0;
#undef __item_frame_out_pthreadSafety__IO_4_Server
#ifdef __item_frame_out_pthreadSafety__IO_4_itemListLock
#undef __item_frame_out_pthreadSafety__IO_4_itemListLock
#endif
#endif
}

/* 获取数据帧的数量 */
unsigned int recognition_item_get_frame_count_pthreadSafety(struct Recognition * const pRecognition, /* 获取的通道编号,从0开始 */unsigned int index) {
    struct Recognition_item * const pRecognition_item = (struct Recognition_item *) pRecognition->ro.__itemList_->pNode[pRecognition->ro.__itemList_->startIndex + index];
    if (NULL == pRecognition_item)return 0;
    return pRecognition_item->ro.__framelistOut->nodeCount;
}

/* 重置模块内通道对应的开方分组 */
void recognition_item_reset_groupSqrtOut(struct Recognition * const pRecognition, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Recognition
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

static void recognition_P_item_plateRecognition_delete__OI(struct Recognition_item_Plate * const pPlate) {
    pthread_mutex_lock(&((FsObjectBasePthreadSafety*) pPlate)->_mutex);
    /* 先判断引用计数(不包含拥有者标记)是否为1 */
    if (1 == fs_ObjectBase_referCount_get(pPlate)) {
        pthread_mutex_unlock(&((FsObjectBasePthreadSafety*) pPlate)->_mutex);
        /* 先删除本对象特有的变量 */
        if (pPlate->_pFrame)fs_Object_delete_pthreadSafety__OI(pPlate->_pFrame);
        fs_ObjectBasePthreadSafety_delete__OI(pPlate);
    } else {
        ((FsObjectBase*) pPlate)->referCount--;
        pthread_mutex_unlock(&((FsObjectBasePthreadSafety*) pPlate)->_mutex);
    }
}

/* 创建一个车牌识别的任务结构 */
static struct Recognition_item_Plate * recognition_item_plateRecognition_new__IO(/* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer
        , /* 图片信息 */ FsObjectImageFrame * const pFrame, /* 识别区域 */ const short x1, const short y1, const short x2, const short y2) {
    struct Recognition_item_Plate * const rst = (struct Recognition_item_Plate *) fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (struct Recognition_item_Plate), sizeof (FsObjectBasePthreadSafety));
    fs_ObjectBasePthreadSafety_init_from_objectBase(rst);
    ((FsObjectBasePthreadSafety*) rst)->_delete = (void (*)(void *const))recognition_P_item_plateRecognition_delete__OI;
    /* 把rst后面的数据加入缓存 */
    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, (FsObjectBase*) rst, sizeof (struct Recognition_item_Plate));
    /* 图片信息,没有图片的必须置空 */
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    rst->_pFrame = pFrame;
    /* 识别区域 */
    rst->_x1 = x1, rst->_y1 = y1, rst->_x2 = x2, rst->_y2 = y2;
    /* 车牌位置 */
    rst->platepos_x1 = x1, rst->platepos_y1 = y1, rst->platepos_x2 = x2, rst->platepos_y2 = y2;
    /* 标记,0-等待识别,1-已识别 */
    rst->_tag = 0;
    /* 车牌识别结果 */
    rst->plateNo[0] = 0;

    return rst;
}

/* 添加车牌识别任务 */
struct Recognition_item_Plate * recognition_item_add_plateRecognition_pthreadSafety__IO(/* 识别项 */ struct Recognition_item * const pRecognition_item, /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer
        , /* 图片信息 */ FsObjectImageFrame * const pFrame, /* 识别区域 */ const short x1, const short y1, const short x2, const short y2
        , /* 上一张图片信息 */ FsObjectImageFrame * const pFrame_last, /* 上一个位置的识别区域 */ const short x1_last, const short y1_last, const short x2_last, const short y2_last) {
    /* 检查车是否在识别范围内 */
    pthread_mutex_t * const pMutex = &pRecognition_item->p.plateRecognition.__list->mutex;
    pthread_mutex_lock(pMutex);
    FsObjectBasePthreadSafety * const mask = pRecognition_item->p.plateRecognition.__mask;
    if (mask) {
        short yd1 = y2_last * pRecognition_item->p.plateRecognition.maskHeight / pFrame->height[0], yd2 = y2 * pRecognition_item->p.plateRecognition.maskHeight / pFrame->height[0];
        const unsigned char *const ps = (unsigned char*) mask->data + (x1 / 4 + x2 / 4 + x1_last / 4 + x2_last / 4) * pRecognition_item->p.plateRecognition.maskWidth / pFrame->width[0];
        if (yd2 > yd1) {
            /* 由远及近 */
            do {
                if (ps[yd2 * pRecognition_item->p.plateRecognition.maskWidth] != 0) {
                    struct Recognition_item_Plate * const rst = recognition_item_plateRecognition_new__IO(pObjectBaseBuffer, pFrame_last, x1_last, y1_last, x2_last, y2_last);
                    fs_ObjectBasePthreadSafety_addRefer(rst);
                    if (pRecognition_item->p.plateRecognition.__list->nodeCount > 10)FsPrintf(FsPrintfIndex, "plateRecognition.__list->nodeCount=%lu.\n", pRecognition_item->p.plateRecognition.__list->nodeCount);
                    fs_ObjectList_insert_tail(pRecognition_item->p.plateRecognition.__list, rst);
                    pthread_mutex_unlock(pMutex);
                    return rst;
                }
            } while (--yd2 >= yd1);
        } else {
            /* 由近及远 */
            do {
                if (ps[yd2 * pRecognition_item->p.plateRecognition.maskWidth] != 0) {
                    struct Recognition_item_Plate * const rst = recognition_item_plateRecognition_new__IO(pObjectBaseBuffer, pFrame, x1, y1, x2, y2);
                    fs_ObjectBasePthreadSafety_addRefer(rst);
                    if (pRecognition_item->p.plateRecognition.__list->nodeCount > 10)FsPrintf(FsPrintfIndex, "plateRecognition.__list->nodeCount=%lu.\n", pRecognition_item->p.plateRecognition.__list->nodeCount);
                    fs_ObjectList_insert_tail(pRecognition_item->p.plateRecognition.__list, rst);
                    pthread_mutex_unlock(pMutex);
                    return rst;
                }
            } while (++yd2 <= yd1);
        }
        /* 不在识别范围 */
        pthread_mutex_unlock(pMutex);
        return NULL;
    } else if (0 == pRecognition_item->p.plateRecognition.__list->nodeCount) {
        /* 识别未初始化,创建一个识别让其初始化 */
        struct Recognition_item_Plate * const rst = recognition_item_plateRecognition_new__IO(pObjectBaseBuffer, pFrame, x1, y1, x2, y2);
        fs_ObjectList_insert_tail(pRecognition_item->p.plateRecognition.__list, rst);
        pthread_mutex_unlock(pMutex);
        return NULL;
    } else {
        pthread_mutex_unlock(pMutex);
        return NULL;
    }
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
