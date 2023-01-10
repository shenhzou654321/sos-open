#include "targetCheck.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Image/ImageExtern.h"
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
/* 开方分组用的互斥锁 */
static pthread_mutex_t groupSqrt_mutex = PTHREAD_MUTEX_INITIALIZER;
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int targetCheck_P_item_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */ struct TargetCheck_item * const pTargetCheck_item, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsStructList * const clientList = pTargetCheck_item->ro.__clientList;
    pthread_mutex_lock(&clientList->mutex);
    configManager_conncet_refer_check(pTargetCheck_item->ro._pTargetCheck->ro._pConfigManager, clientList);
    pthread_mutex_unlock(&clientList->mutex);
    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int targetCheck_P_item_cb_debug(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型 */ char requestDataType, /* 调用函数的指针 */ struct TargetCheck_item * const pTargetCheck_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType
        , (unsigned int) fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "datatype", 0)};
    pthread_mutex_lock(&pTargetCheck_item->ro.__clientList->mutex);
    configManager_conncet_refer_check(pTargetCheck_item->ro._pTargetCheck->ro._pConfigManager, pTargetCheck_item->ro.__clientList);
    if (fs_StructList_insert_order(pTargetCheck_item->ro.__clientList, data) < 0) {
        pthread_mutex_unlock(&pTargetCheck_item->ro.__clientList->mutex);
        return 1;
    } else {
        pthread_mutex_unlock(&pTargetCheck_item->ro.__clientList->mutex);
        return -128;
    }
}

static FsConfig *targetCheck_P_protocol_debug() {
    FsConfig * const pConfig = fs_Config_new__IO();
    void *pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 17, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "targetcheck_debug", "targetcheck_debug", "targetcheck_debug");

    pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");

    pNode = fs_Config_node_integer_add(pConfig, pConfig, "datatype", "数据类型", "数据类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 6, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "水平边缘图(图片流)", "水平边缘图(图片流)");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "垂直边缘图(图片流)", "垂直边缘图(图片流)");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "帧差数据(图片流)", "帧差数据(图片流)");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "平滑后的边缘数据(图片流)", "平滑后的边缘数据(图片流)");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "平滑后的二值化数据(图片流)", "平滑后的二值化数据(图片流)");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 5, "目标提取图像(图片流)", "目标提取图像(图片流)");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 6, "目标框图(图片流)", "目标框图(图片流)");

    return pConfig;
}

/* 删除pTargetCheck_item指向的实例对象 */
static void targetCheck_P_item_delete__OI(struct TargetCheck_item * const pTargetCheck_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pTargetCheck_item)return;
    void *const pConfigManager = pTargetCheck_item->ro._pTargetCheck->ro._pConfigManager;
    // 注销连接断开时的回调通知
    if (0 == pTargetCheck_item->ro._ipv4) configManager_connect_error_logoff(pConfigManager, (int (*)(const unsigned int *, void *, char * * const))targetCheck_P_item_cb_connect_error, pTargetCheck_item);
    /* 注销命令字 */
    if (0 == pTargetCheck_item->ro._ipv4) configManager_cmd_logoff(pConfigManager, "targetcheck_debug", pTargetCheck_item->ro._uuid, pTargetCheck_item->ro._ipv4, pTargetCheck_item, pShareBuffer);
    if (0 == pTargetCheck_item->ro._ipv4) {
        /* 传入的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pTargetCheck_item->ro.__framelistIn);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pTargetCheck_item->ro.__framelistOut);
        /* 关闭所有与之相关的客户端链表 */
        configManager_conncet_refer_close(pConfigManager, pTargetCheck_item->ro.__clientList, 1);
        /* 与之相关的客户端链表 */
        fs_StructList_delete__OI(pTargetCheck_item->ro.__clientList);
        /* 车辆检测对象 */
        if (pTargetCheck_item->p.__pCarDetect)carDetect_delete__OI(pTargetCheck_item->p.__pCarDetect);
    }
    fsFree(pTargetCheck_item);
}

static void targetCheck_P_item_new(struct TargetCheck * const pTargetCheck, /* 通道号,从1开始,为0用于集群 */const unsigned int channel
        , /* 本地ip地址 */const FsArray * const ipList, /* 原来的检测项 */struct TargetCheck_item * * const ppTargetCheck_item, FsGroupSqrt * const pGroupSqrt
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pTargetCheck->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, 0,ipList, 0 == channel, "vsys");
    if (clusterList) {
        void **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int uj = clusterList->nodeCount;
        do {
            const void *vsysChannel0 = vsys0;
            const void* const vsys = *ppNodeCluster++;
            FsObjectList * const list = 0 == channel ? fs_Config_node_template_orderFirst__IO(pConfig, &vsysChannel0, vsys,0, NULL, 0, "vsysChannel")
                    : (FsObjectList *) fs_Config_node_template_get_orderFirst(pConfig, &vsysChannel0, vsys,0, NULL, 0, "vsysChannel", channel - 1);
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
                        if (*ppTargetCheck_item != NULL && (*ppTargetCheck_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppTargetCheck_item) {
                        targetCheck_P_item_delete__OI(*ppTargetCheck_item, pShareBuffer);
                    }
                    struct TargetCheck_item * rst;
                    if ((TargetCheck_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0 && TargetCheck_Mask != 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            rst = (struct TargetCheck_item*) fsMalloc(Memery_Alignment(sizeof (struct TargetCheck_item)) + len);
                            memset(rst, 0, sizeof (struct TargetCheck_item));
                            pd = ((char*) rst) + Memery_Alignment(sizeof (struct TargetCheck_item));
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 与之相关的客户端链表 */
                            rst->ro.__clientList = fs_StructList_new__IO(6, sizeof (int)*6);
                        } else {
                            //                        rst = (struct Record_item*) fsMalloc(Record_item_ClusterLen + len);
                            //                        memset(rst, 0, Record_item_ClusterLen);
                            //                        pd = ((char*) rst) + Record_item_ClusterLen;
                        }
                        if (channel > 0) {
                            /* 整个通道依赖的所有数据的校验和 */
                            rst->ro._sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode);
                            /* 当有数据输入时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set(rst->rw._pGroupSqrtIn, pGroupSqrt->groupSqrt_member, pGroupSqrt->groupSqrt_group
                                    , pGroupSqrt->groupValue, channel - 1);
                            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pTargetCheck->ro._pGroupSqrtOut->groupSqrt_member, pTargetCheck->ro._pGroupSqrtOut->groupSqrt_group
                                    , pTargetCheck->ro._pGroupSqrtOut->groupValue, channel - 1, &pTargetCheck->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        if (channel > 0) {
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len);
                            /* 所属的目标检测对象 */
                            rst->ro._pTargetCheck = pTargetCheck;
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        // 注册连接断开时的回调通知
                        if (0 == ipv4) configManager_connect_error_register(pTargetCheck->ro._pConfigManager
                                , (int (*)(const unsigned int *, void *, char * * const))targetCheck_P_item_cb_connect_error, rst);
                        /* 注册命令字 */
                        if (0 == ipv4) configManager_cmd_register(pTargetCheck->ro._pConfigManager, "targetcheck_debug", rst->ro._uuid, ipv4, rst, 0
                                , (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))targetCheck_P_item_cb_debug
                                , NULL, rst, pShareBuffer);
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
                    if (channel > 0) *ppTargetCheck_item = rst;
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

/* 发送调试数据 */
static void targetCheck_P_item_send_carDetect_debug(struct TargetCheck_item * const pTargetCheck_item, FsObjectImageFrame * const pFrame
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer) {
    FsStructList * const clientList = pTargetCheck_item->ro.__clientList;
    if (0 == clientList->nodeCount)return;
    FsObjectBase * ppObjectBase0[7];
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
                /* 水平边缘数据 */
            case 0:
            {
                if (ppObjectBaseCount < 4) {
                    FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, 4), fflush(stdout);
                    if (ppObjectBase == ppObjectBase0) {
                        ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * 4);
                        //memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * 0);
                    } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * 4);
                    ppObjectBaseCount = 4;
                }
                {
                    ppObjectBase[2] = (FsObjectBase *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_Hedge_2);
                    if (NULL == ppObjectBase[2]) {
                        FsPrintf(FsPrintfIndex, "Prapare HEDGE_2 failed.\n");
                        fflush(stdout);
                        head = 0;
                        break;
                    }
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                unsigned char fillLen;
                len = image_extern_gray_write_size_get(&fillLen, pFrame->sizeWidthHeight[2]*2, sizeof ("tt") - 1);
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
                image_extern_gray_write_head((unsigned char*) ppObjectBase[1]->data, (unsigned char*) ppObjectBase[3]->data, 1, sizeof ("tt") - 1, "tt"
                        , pFrame->width[2], pFrame->height[2], pFrame->width[2]*2, 0, 0, 0, 0, 1.0, 1.0, 16, pFrame->sizeWidthHeight[2]*2);

                len += fillLen + ppObjectBase[2]->len;
            }
                break;
                /* 垂直边缘数据 */
            case 1:
            {
                if (ppObjectBaseCount < 4) {
                    FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, 4), fflush(stdout);
                    if (ppObjectBase == ppObjectBase0) {
                        ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * 4);
                        //memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * 0);
                    } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * 4);
                    ppObjectBaseCount = 4;
                }
                {
                    ppObjectBase[2] = (FsObjectBase *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_Vedge_2);
                    if (NULL == ppObjectBase[2]) {
                        FsPrintf(FsPrintfIndex, "Prapare VEDGE_2 failed.\n");
                        fflush(stdout);
                        head = 0;
                        break;
                    }
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                unsigned char fillLen;
                len = image_extern_gray_write_size_get(&fillLen, pFrame->sizeWidthHeight[2]*2, sizeof ("tt") - 1);
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
                image_extern_gray_write_head((unsigned char*) ppObjectBase[1]->data, (unsigned char*) ppObjectBase[3]->data, 1, sizeof ("tt") - 1, "tt"
                        , pFrame->width[2], pFrame->height[2], pFrame->width[2]*2, 0, 0, 0, 0, 1.0, 1.0, 16, pFrame->sizeWidthHeight[2]*2);

                len += fillLen + ppObjectBase[2]->len;
            }
                break;
                /* 帧差数据 */
            case 2:
                /* 平滑后的边缘数据 */
            case 3:
            {
                if (ppObjectBaseCount < 4) {
                    FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, 4), fflush(stdout);
                    if (ppObjectBase == ppObjectBase0) {
                        ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * 4);
                        //memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * 0);
                    } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * 4);
                    ppObjectBaseCount = 4;
                }
                if (NULL == pTargetCheck_item->p.__pCarDetect) {
                    head = 0;
                    break;
                }
                ppObjectBase[2] = (FsObjectBase*) carDetect_debug__IO(pTargetCheck_item->p.__pCarDetect, debugType - 2, sizeof (FsObjectBasePthreadSafety), 0);
                if (NULL == ppObjectBase[2]) {
                    head = 0;
                    break;
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                unsigned char fillLen;
                len = image_extern_gray_write_size_get(&fillLen, ppObjectBase[2]->len, sizeof ("tt") - 1);
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
                image_extern_gray_write_head((unsigned char*) ppObjectBase[1]->data, (unsigned char*) ppObjectBase[3]->data, 1, sizeof ("tt") - 1, "tt"
                        , pFrame->width[2], pFrame->height[2], pFrame->width[2]*2, 0, 0, 0, 0, 1.0, 1.0, 16, ppObjectBase[2]->len);
                len += fillLen + ppObjectBase[2]->len;
            }
                break;
                /* 平滑后的二值化数据 */
            case 4:
                /* 目标提取图像 */
            case 5:
            {
                if (ppObjectBaseCount < 4) {
                    FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, 4), fflush(stdout);
                    if (ppObjectBase == ppObjectBase0) {
                        ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * 4);
                        //memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * 0);
                    } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * 4);
                    ppObjectBaseCount = 4;
                }
                if (NULL == pTargetCheck_item->p.__pCarDetect) {
                    head = 0;
                    break;
                }
                ppObjectBase[2] = (FsObjectBase*) carDetect_debug__IO(pTargetCheck_item->p.__pCarDetect, debugType - 2, sizeof (FsObjectBasePthreadSafety), 0);
                if (NULL == ppObjectBase[2]) {
                    head = 0;
                    break;
                }
                head = 0x0B, ppObjectBaseUseCount = 3;
                unsigned char fillLen;
                len = image_extern_gray_write_size_get(&fillLen, ppObjectBase[2]->len, sizeof ("tt") - 1);
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
                image_extern_gray_write_head((unsigned char*) ppObjectBase[1]->data, (unsigned char*) ppObjectBase[3]->data, 1, sizeof ("tt") - 1, "tt"
                        , pFrame->width[2], pFrame->height[2], pFrame->width[2], 0, 0, 0, 0, 1.0, 1.0, 8, ppObjectBase[2]->len);
                len += fillLen + ppObjectBase[2]->len;
            }
                break;
                /* 目标框图 */
            case 6:
            {
                if (ppObjectBaseCount < 7) {
                    FsPrintf(FsPrintfIndex, "Resize ppObjectBaseCount from %u to %u.\n", ppObjectBaseCount, 7), fflush(stdout);
                    if (ppObjectBase == ppObjectBase0) {
                        ppObjectBase = (FsObjectBase **) fsMalloc(sizeof (*ppObjectBase) * 7);
                        //memcpy(ppObjectBase, ppObjectBase0, sizeof (*ppObjectBase) * 0);
                    } else ppObjectBase = (FsObjectBase **) fsRealloc(ppObjectBase, sizeof (*ppObjectBase) * 7);
                    ppObjectBaseCount = 7;
                }
                {
                    ppObjectBase[2] = (FsObjectBase *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RGB_2);
                    if (NULL == ppObjectBase[2]) {
                        FsPrintf(FsPrintfIndex, "Prapare HEDGE_2 failed.\n");
                        fflush(stdout);
                        head = 0;
                        break;
                    }
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                {
                    unsigned char fillLen;
                    len = image_extern_rgb_write_size_get(&fillLen, pFrame->sizeWidthHeight[2]*3, sizeof ("tt") - 1);
                    ppObjectBase[1] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + len, sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[1]);
                    /* 把pObjectBase[1]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[0], sizeof (FsObjectBasePthreadSafety) + len);
                    ppObjectBase[1]->len = len;

                    ppObjectBase[3] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + fillLen, sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[3]);
                    /* 把pObjectBase[3]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[3], sizeof (FsObjectBasePthreadSafety) + fillLen);
                    ppObjectBase[3]->len = fillLen;
                    image_extern_rgb_write_head((unsigned char*) ppObjectBase[1]->data, (unsigned char*) ppObjectBase[3]->data, 1, sizeof ("tt") - 1, "tt"
                            , pFrame->width[2], pFrame->height[2], pFrame->width[2]*3, 0, 0, 0, 0, 1.0, 1.0, pFrame->sizeWidthHeight[2]*3);
                    len += fillLen + ppObjectBase[2]->len;
                }
                ////////////////////////////////////////////////////////////////
                {
                    if (NULL == pTargetCheck_item->p.__pCarDetect) break;
                    ppObjectBase[5] = (FsObjectBase*) carDetect_debug__IO(pTargetCheck_item->p.__pCarDetect, 4, sizeof (FsObjectBasePthreadSafety), 0);
                    if (NULL == ppObjectBase[5]) break;
                    ppObjectBaseUseCount += 3;
                    const unsigned int lineCount = ppObjectBase[5]->len / 16;
                    unsigned char fillLen;
                    unsigned int lineHeadLen = image_extern_line_write_size_get(&fillLen, lineCount, sizeof ("tt") - 1);
                    ppObjectBase[4] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + lineHeadLen, sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[4]);
                    /* 把pObjectBase[4]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[4], sizeof (FsObjectBasePthreadSafety) + lineHeadLen);
                    ppObjectBase[4]->len = lineHeadLen;

                    ppObjectBase[6] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + fillLen, sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[6]);
                    /* 把pObjectBase[6]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[6], sizeof (FsObjectBasePthreadSafety) + fillLen);
                    ppObjectBase[6]->len = fillLen;
                    image_extern_line_write_head((unsigned char*) ppObjectBase[4]->data, (unsigned char*) ppObjectBase[6]->data, 1, sizeof ("tt") - 1, "tt"
                            , pFrame->width[2], pFrame->height[2], 0x000000FF, 1.0, 1.0, lineCount);
                    len += lineHeadLen + ppObjectBase[5]->len + fillLen;
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

/* 从配置获取通道数 */
static unsigned int targetCheck_P_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

static CarDetect *targetCheck_P_item_new_CarDetect__IO(struct TargetCheck_item * const pTargetCheck_item, unsigned int index, unsigned short width, unsigned short height
        , /* 参考标定的宽度 */unsigned int width0, /* 参考标定的高度 */ unsigned int height0
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
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
    FsLocal_ShareBuffer_resize(width * height);
    unsigned char * mask = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
    memset(mask, 0, width * height);
    ConfigManager * const pConfigManager = (ConfigManager*) pTargetCheck_item->ro._pTargetCheck->ro._pConfigManager;
    configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
    FsConfig * const pConfig = pConfigManager->ro.__pConfig;
    const void *parent0 = pConfig;
    const void *parent;
    {
        FsObjectList * list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0,pConfigManager->ro.__ipList_local, 0, "vsys");
        if (list) {
            parent = list->pNode[list->startIndex];
            fs_ObjectList_delete__OI(list);
            list = fs_Config_node_template__IO(pConfig, &parent0, parent,0, NULL, 0, "vsysChannel");
            if (list) {
                if (index < list->nodeCount) {
                    parent = list->pNode[list->startIndex + index];
                    parent = fs_Config_node_get_first(pConfig, &parent0, parent, "targetCheckConfig");
                } else parent = NULL;
                fs_ObjectList_delete__OI(list);
            } else parent = NULL;
        } else parent = NULL;
    }
    unsigned short carThreshold = 20;
    if (parent) {
        carThreshold = fs_Config_node_integer_get_first(pConfig, parent0, parent, "carThreshold", 20, NULL);
        FsConfigResult_area * const pArea = fs_Config_node_string_area_lenth_get__IO(pConfig, parent0, parent, "targetCheckErea area", NULL, width0 * 1.0 / width, height0 * 1.0 / height, 0, 0);
        if (pArea) {
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + width * height);
            fs_Config_node_string_area_fill_ch1(pArea, mask, width, height, width, 255, 0, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            mask = (unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
            fsFree(pArea);
        }
    }
    configManager_config_deleteRefer_pthreadSafety(pConfigManager);
    if (0) {
        ImageDib *pDib = image_dib_new__IO(width, height, ImageColorType_Gray, width);
        memcpy(pDib->data, mask, pDib->dataLenth);
        image_dib_svae_to_file(pDib, "/fs/project/data/1.bmp");
        image_dib_delete__OI(&pDib);
    }
    CarDetect * const rst = pTargetCheck_item->p.__pCarDetect = carDetect_new__IO(width, height, carThreshold, height * 4, mask, width, 0);
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    return rst;
}

static void *targetCheck_P_T(struct TargetCheck * const pTargetCheck) {
#undef FsFunctionName
#define FsFunctionName targetCheck_P_T
    /* 创建线程监控 */
    void *pMonitor = pTargetCheck->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, TargetCheck_get_threadName(pTargetCheck), NULL, pTargetCheck, pTargetCheck->ro._pMonitor != NULL ? pTargetCheck->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pTargetCheck->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pTargetCheck->ro._pConfigManager;
    FsObjectList * const itemList_ = pTargetCheck->ro.__itemList_;
    ////////////////////////////////////////////////////////////////////////////
    /*
     * 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记;
     * 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记;
     * 未标记的数据可能不需要析构或需要在处理完1类数据前手动处理(可在2类数据前或2类数据后); 
     */
    ////////////////////////////////////////////////////////////////////////////
    /* 格式化变量 */
    /* 共享buffer,本函数不使用,只在子函数中使用 */
    FsShareBuffer shareBuffer = NULL;
#define __targetCheck_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __targetCheck_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __targetCheck_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);  
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    unsigned int sleepTime = 0;
    int frameBufferCount = 1;
    /* 开方分组 */
    FsGroupSqrt groupSqrt;
    Fs_GroupSqrt_init(&groupSqrt);
#define __targetCheck_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __targetCheck_P_T_clean1_count 4
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    FsObjectImageFrame *pFrame1, *pFrame2;
    struct TargetCheck_item *pTargetCheck_item, **ppTargetCheck_itemNode;
    /* 输出目标id */
    unsigned int id[32], x1[32], y1[32], x2[32], y2[32];
    unsigned int ui, frameIterval, state;
    int rv;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    configManager_protocol_publish(pConfigManager, "targetcheck_debug", "目标检测调试", 1, targetCheck_P_protocol_debug);
#define __targetCheck_P_T_clean2_1 {configManager_protocol_cancel(pConfigManager, "targetcheck_debug");}
#define __targetCheck_P_T_clean2_count 1
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pTargetCheck->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        usleep(1000);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pTargetCheck->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (0 == pTargetCheck->rw._resetChannel) channelCount = targetCheck_P_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server TargetCheck
#ifndef __groupSqrt_reset 
            if (itemList_->nodeCount != channelCount) {
                pthread_mutex_lock(&groupSqrt_mutex);
                Fs_GroupSqrt_resize(&groupSqrt, channelCount);
                memset(groupSqrt.groupValue, 1, groupSqrt.groupSqrt_group);
                // 把原来的通道指向新的分组
                {
                    struct /* Recognition_item */ FsMacrosValue2(__groupSqrt_reset_Server, _item) ** ppNode
                            = (struct /* Recognition_item */ FsMacrosValue2(__groupSqrt_reset_Server, _item) **) itemList_->pNode + itemList_->startIndex;
                    unsigned int ui = 0;
                    const unsigned int count = itemList_->nodeCount;
                    for (; ui < count; ui++) {
                        if (ppNode[ui])Fs_GroupSqrtOut_set(ppNode[ui]->rw._pGroupSqrtIn, groupSqrt.groupSqrt_member, groupSqrt.groupSqrt_group, groupSqrt.groupValue, ui);
                    }
                }
                pthread_mutex_unlock(&groupSqrt_mutex);
            }
#undef __groupSqrt_reset_Server
#endif
            /* 删除通道 */
            while (itemList_->nodeCount > channelCount) {
                pTargetCheck_item = (struct TargetCheck_item *) fs_ObjectList_remove_tail(itemList_);
                if (pTargetCheck_item)targetCheck_P_item_delete__OI(pTargetCheck_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pTargetCheck->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pTargetCheck->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pTargetCheck->rw._resetChannel > 0) {
                FsPrintf(1, "%d\n", pTargetCheck->rw._resetChannel);
                pMonitorItem->rw.line = __LINE__;
                targetCheck_P_item_new(pTargetCheck, pTargetCheck->rw._resetChannel, pConfigManager->ro.__ipList_local
                        , (struct TargetCheck_item **) (itemList_->pNode + itemList_->startIndex + pTargetCheck->rw._resetChannel - 1), &groupSqrt
                        , & shareBuffer);
                pMonitorItem->rw.line = __LINE__;
            }
            pTargetCheck->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct TargetCheck_item * * const ppNode_itemList = (struct TargetCheck_item **) itemList_->pNode + itemList_->startIndex;
            for (; ui < channelCount; ui++) {
#ifndef __groupSqrt_do_item 
                if (ui % groupSqrt.groupSqrt_member == 0) {
                    if (groupSqrt.groupValue[ui / groupSqrt.groupSqrt_member])groupSqrt.groupValue[ui / groupSqrt.groupSqrt_member] = 0;
                    else {
                        /* 跳过当前组 */
                        ui += groupSqrt.groupSqrt_member - 1;
                        continue;
                    }
                }
#endif
                struct TargetCheck_item * const pTargetCheck_item = ppNode_itemList[ui];
                if (NULL == pTargetCheck_item)continue;
                ////////////////////////////////////////////////////////////////////
                //////////////////////// 状态机 开始 ////////////////////////////////
#define __targetCheck_P_T_state_check_end (1U<<0) // 检查缓存是否足够,不够退出循环
#define __targetCheck_P_T_state_check_module_init (1U<<1) // 检查模块初始化
#define __targetCheck_P_T_state_check_do (1U<<2) // 执行检测
#define __targetCheck_P_T_state_out_frame (1U<<3) // 输出数据帧
#define __targetCheck_P_T_state_end_break (1U<<4) // 退出循环
#define __targetCheck_P_T_state_reset (1U<<5) // 重置
#ifndef __targetCheck_P_T_state
                //state = FsMacrosFunction(state_check_end);
                FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                for (;;) {
                    CarDetect *pCarDetect;
                    FsObjectImageFrame * pFrame;
                    /* 检查缓存是否足够 */
                    if (state & FsMacrosFunction(state_check_end)) {
                        state ^= FsMacrosFunction(state_check_end);
                        FsMacrosFunctionTag(state_check_end) :;
                        ////////////////////////////////////////////////
                        if (((int) pTargetCheck_item->ro.__framelistIn->nodeCount) >= frameBufferCount) {
                            /* 检查数据 */
                            pthread_mutex_lock(&pTargetCheck_item->ro.__framelistIn->mutex);
                            pFrame = (FsObjectImageFrame*) pTargetCheck_item->ro.__framelistIn->pNode[ pTargetCheck_item->ro.__framelistIn->startIndex + frameBufferCount - 1];
                            pthread_mutex_unlock(&pTargetCheck_item->ro.__framelistIn->mutex);
                            if (pTargetCheck_item->ro.classIndex != pFrame->classIndex) {
                                pTargetCheck_item->ro.classIndex = pFrame->classIndex;
                                /* 重置 */
                                FsMacrosSetStates_OR_GotoFunctionTag(state, state_reset, FsMacrosFunction(state_check_end));
                            } else {
                                /* 检查模块有无初始化 */
                                FsMacrosSetStates_OR_GotoFunctionTag(state, state_check_module_init, FsMacrosFunction(state_check_do) | FsMacrosFunction(state_out_frame)
                                        | FsMacrosFunction(state_check_end));
                            }
                        } else FsMacrosSetState_OR_GotoFunctionTag(state, state_end_break);
                        if (0 == state)break;
                    }
                    /* 检查模块初始化 */
                    if (state & FsMacrosFunction(state_check_module_init)) {
                        state ^= FsMacrosFunction(state_check_module_init);
                        FsMacrosFunctionTag(state_check_module_init) :;
                        ////////////////////////////////////////////////////////////                   
                        if (NULL == (pCarDetect = pTargetCheck_item->p.__pCarDetect)) {
                            pthread_mutex_lock(&pTargetCheck_item->ro.__framelistIn->mutex);
                            int i = 0;
                            for (; i < frameBufferCount; i++) {
                                pFrame = (FsObjectImageFrame*) pTargetCheck_item->ro.__framelistIn->pNode[ pTargetCheck_item->ro.__framelistIn->startIndex + i];
                                if (pTargetCheck_item->ro.classIndex != pFrame->classIndex)break;
                            }
                            pthread_mutex_unlock(&pTargetCheck_item->ro.__framelistIn->mutex);
                            if (i != frameBufferCount) {
                                /* 重置 */
                                FsMacrosSetState_OR_GotoFunctionTag(state, state_reset);
                            } else {
                                pthread_mutex_lock(&pTargetCheck_item->ro.__framelistIn->mutex);
                                pFrame = (FsObjectImageFrame*) pTargetCheck_item->ro.__framelistIn->pNode[ pTargetCheck_item->ro.__framelistIn->startIndex + frameBufferCount - 1];
                                pthread_mutex_unlock(&pTargetCheck_item->ro.__framelistIn->mutex);
                                pCarDetect = pTargetCheck_item->p.__pCarDetect = targetCheck_P_item_new_CarDetect__IO(pTargetCheck_item, ui
                                        , pFrame->width[2], pFrame->height[2], pFrame->width[0], pFrame->height[0], &shareBuffer);
                            }
                        }
                        if (0 == state)break;
                    }
                    /* 执行检测 */
                    if (state & FsMacrosFunction(state_check_do)) {
                        state ^= FsMacrosFunction(state_check_do);
                        //FsMacrosFunctionTag(state_check_do) :;
                        ////////////////////////////////////////////////////////////
                        /* 统计检测帧间隔 */
                        frameIterval = 0;
                        //********************************************************//
                        /* 车辆检测模块 */
                        if (frameIterval < pCarDetect->p._frameInterval)frameIterval = pCarDetect->p._frameInterval;
                        if (frameBufferCount < (int) pCarDetect->p._frameInterval * 3) {
                            frameBufferCount = pCarDetect->p._frameInterval * 3;
                            FsMacrosSetStates_OR_GotoFunctionTag(state, state_reset, FsMacrosFunction(state_check_end));
                        } else {
                            if (0 == (pFrame->index % frameIterval)) {
                                FsObjectImageEdge * const hEdge = (FsObjectImageEdge *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_Hedge_2);
                                //
                                pthread_mutex_lock(&pTargetCheck_item->ro.__framelistIn->mutex);
                                pFrame1 = (FsObjectImageFrame*) pTargetCheck_item->ro.__framelistIn->pNode[ pTargetCheck_item->ro.__framelistIn->startIndex + frameBufferCount - 1 - pCarDetect->p._frameInterval];
                                pthread_mutex_unlock(&pTargetCheck_item->ro.__framelistIn->mutex);
                                pthread_mutex_lock(&((FsObject*) pFrame1)->mutex);
                                FsObjectImageEdge * const hEdge1 = (FsObjectImageEdge *) image_frame_get_pthreadSafety__IO(pFrame1, ImageFrame_Hedge_2);
                                //
                                pthread_mutex_lock(&pTargetCheck_item->ro.__framelistIn->mutex);
                                pFrame2 = (FsObjectImageFrame*) pTargetCheck_item->ro.__framelistIn->pNode[ pTargetCheck_item->ro.__framelistIn->startIndex + frameBufferCount - 1 - pCarDetect->p._frameInterval * 2];
                                pthread_mutex_unlock(&pTargetCheck_item->ro.__framelistIn->mutex);
                                FsObjectImageEdge * const hEdge2 = (FsObjectImageEdge *) image_frame_get_pthreadSafety__IO(pFrame2, ImageFrame_Hedge_2);
                                //
                                if (hEdge != NULL && hEdge1 != NULL && hEdge2 != NULL) {
                                    carDetect_detect(pCarDetect, 32, id, x1, y1, x2, y2, hEdge->data, pFrame->index, hEdge1->data, pFrame1->index
                                            , hEdge2->data, pFrame2->index, pFrame->width[2]*2, pFrame->frameRate, &baseBuffer, &shareBuffer);
                                    fs_Object_delete_pthreadSafety__OI(hEdge);
                                    fs_Object_delete_pthreadSafety__OI(hEdge1);
                                    fs_Object_delete_pthreadSafety__OI(hEdge2);
                                    /* 发送调试数据到客户端 */
                                    targetCheck_P_item_send_carDetect_debug(pTargetCheck_item, pFrame, &baseBuffer);
                                } else {
                                    FsPrintf2(FsPrintfIndex, "%u/%lu:Edge(=%p,%p,%p) is NULL.\n", ui, itemList_->nodeCount, hEdge, hEdge1, hEdge2);
                                    if (hEdge) fs_Object_delete_pthreadSafety__OI(hEdge);
                                    if (hEdge1) fs_Object_delete_pthreadSafety__OI(hEdge1);
                                    if (hEdge2) fs_Object_delete_pthreadSafety__OI(hEdge2);
                                }
                            }
                        }
                        //********************************************************//
                        if (0 == state)break;
                    }
                    /* 输出数据帧 */
                    if (state & FsMacrosFunction(state_out_frame)) {
                        state ^= FsMacrosFunction(state_out_frame);
                        //FsMacrosFunctionTag(state_out_frame) :;
                        ////////////////////////////////////////////////////////////
#ifndef __targetCheck_P_T_outFrame 
                        pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pTargetCheck_item->ro.__framelistIn);
                        pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                        fs_ObjectList_insert_tail_pthreadSafety(pTargetCheck_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pTargetCheck_item->rw._pGroupSqrtOut, &pTargetCheck_item->ro._pTargetCheck->ro._pGroupSqrtOut->groupSqrt_mutex);
#endif
                        if (FsMacrosFunction(state_check_end) == state) FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                        if (0 == state)break;
                    }
                    ////////////////////////////////////////////////////////////////
                    /* 退出循环 */
                    if (state & FsMacrosFunction(state_end_break)) {
                        state ^= FsMacrosFunction(state_end_break);
                        FsMacrosFunctionTag(state_end_break) :;
                        break;
                        if (0 == state)break;
                    }
                    /* 重置 */
                    if (state & FsMacrosFunction(state_reset)) {
                        state ^= FsMacrosFunction(state_reset);
                        FsMacrosFunctionTag(state_reset) :;
                        /* 清空缓存 */
                        rv = pTargetCheck_item->ro.__framelistIn->nodeCount;
                        if (rv > frameBufferCount)rv = frameBufferCount;
                        while (rv-- > 0) {
#ifndef __targetCheck_P_T_outFrame 
                            pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pTargetCheck_item->ro.__framelistIn);
                            pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                            fs_ObjectList_insert_tail_pthreadSafety(pTargetCheck_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pTargetCheck_item->rw._pGroupSqrtOut, &pTargetCheck_item->ro._pTargetCheck->ro._pGroupSqrtOut->groupSqrt_mutex);
#endif
                        }
                        if (pTargetCheck_item->p.__pCarDetect) {
                            carDetect_delete__OI(pTargetCheck_item->p.__pCarDetect);
                            pTargetCheck_item->p.__pCarDetect = NULL;
                        }
                        if (FsMacrosFunction(state_check_end) == state) FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                        if (0 == state)break;
                    }
                }
#endif
                //////////////////////// 状态机 结束 ////////////////////////////////
                ////////////////////////////////////////////////////////////////////          
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    if (pTargetCheck->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pTargetCheck->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    /* 删除各通道 */
    {
        for (ui = itemList_->nodeCount, ppTargetCheck_itemNode = (struct TargetCheck_item **) itemList_->pNode + itemList_->startIndex; ui > 0; ui--) {
            pTargetCheck_item = *ppTargetCheck_itemNode++;
            if (pTargetCheck_item)targetCheck_P_item_delete__OI(pTargetCheck_item, &shareBuffer);
        }
        fs_ObjectList_clear(itemList_);
        //        /* 删除集群 */
        //        if (pCapture->p.clusterItemList) {
        //            fs_ObjectList_delete_custom__OI(pCapture->p.clusterItemList, (void (*)(void*))capture_P_item_delete__OI);
        //            pCapture->p.clusterItemList = NULL;
        //        }
    }
    /* 释放中间变量空间 */
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", TargetCheck_get_threadName(pTargetCheck), pTargetCheck->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pTargetCheck->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void targetCheck_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "targetCheckConfig", "运动目标检测配置", "运动目标检测配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "16");
    fs_Config_node_add_property_area(pConfig, parent, "targetCheckErea", "area", 0x000000FF);
    fs_Config_node_add_property_image(pConfig, parent, 1, "uuid", "recordConfig rtspServerURL");
    /* 车辆检测阈值  */
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "carThreshold", "车辆检测阈值", "车辆检测阈值", FsConfig_nodeShowType_default, 0, 0x7, 1, 32767, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 20, "20", "20");
    }
    // 创建检测区
    {

        void* const pNode = fs_Config_node_node_add(pConfig, parent, "targetCheckErea", "目标检测区", "目标检测区", 0, 0x7);
        fs_Config_node_string_add(pConfig, pNode, "area", "区域", "区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 1);
    }
}

/* 检查pTargetCheck模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int targetCheck_check_channel_changed(struct TargetCheck * const pTargetCheck, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server TargetCheck
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
 * 创建一个新的TargetCheck对象;
 * 返回TargetCheck指针.
 */
struct TargetCheck * targetCheck_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(threadName) + 1;
    struct TargetCheck * const rst = (struct TargetCheck*) fsMalloc(sizeof (struct TargetCheck) +len);
    memset(rst, 0, sizeof (struct TargetCheck));
    char *pd = (char*) (rst + 1);
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

/* 删除pTargetCheck指向的实例对象 */
void targetCheck_delete__OI(struct TargetCheck * const pTargetCheck) {

    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pTargetCheck->ro.__itemList_);

    fsFree(pTargetCheck);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void targetCheck_startThread(struct TargetCheck * const pTargetCheck,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (0 == pTargetCheck->rw.runStatus && 0 == pTargetCheck->ro.stid) {
        pTargetCheck->rw.runStatus = 1;
#ifdef FsDebug
        TargetCheckPrintf2("Init Parameter");
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
        if (pthread_create(&pTargetCheck->ro.stid, &attr, (void* (*)(void*))targetCheck_P_T, pTargetCheck)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {

        TargetCheckPrintf2("The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter", pTargetCheck->rw.runStatus, pTargetCheck->ro.stid);
        fflush(stdout);
    }
}

/* 结束线程 */
void targetCheck_stopThread(struct TargetCheck * const pTargetCheck) {
    pTargetCheck->rw.runStatus = 0;
    if (pTargetCheck->ro.stid) {
#ifdef FsDebug

        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pTargetCheck->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pTargetCheck->ro.stid);
        pTargetCheck->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void targetCheck_item_frame_in_pthreadSafety__OI_4(struct TargetCheck * const pTargetCheck, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server TargetCheck
#ifndef __item_frame_in_pthreadSafety__OI_4 
    struct /* Recognition_item */ FsMacrosValue2(__item_frame_in_pthreadSafety__OI_4_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __item_frame_in_pthreadSafety__OI_4_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__item_frame_in_pthreadSafety__OI_4_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __item_frame_in_pthreadSafety__OI_4_Server)->ro. __itemList_ ->pNode[/* pRecognition */FsMacrosValue2(p, __item_frame_in_pthreadSafety__OI_4_Server)->ro. __itemList_ ->startIndex + index];
#ifdef FsDebug
    if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __item_frame_in_pthreadSafety__OI_4_Server, _item)) {

        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
    //    if (pTargetCheck_item->ro.__framelistIn->nodeCount > 15) {
    //        FsPrintf2(1, "FrameListIn too long(=%lu).\n", pTargetCheck_item->ro.__framelistIn->nodeCount);
    //    }
#endif
    pthread_mutex_lock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_in_pthreadSafety__OI_4_Server, _item)->ro.__framelistIn->mutex);
    fs_ObjectList_insert_tail_array(/* pRecognition_item */ FsMacrosValue3(p, __item_frame_in_pthreadSafety__OI_4_Server, _item)->ro.__framelistIn, frameCount, (void**) ppFrame);
    pthread_mutex_unlock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_in_pthreadSafety__OI_4_Server, _item)->ro.__framelistIn->mutex);
    Fs_GroupSqrtOut_value_set(/* pRecognition_item */ FsMacrosValue3(p, __item_frame_in_pthreadSafety__OI_4_Server, _item)->rw._pGroupSqrtIn, &groupSqrt_mutex);
#undef __item_frame_in_pthreadSafety__OI_4_Server
#endif
}

/* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
unsigned int targetCheck_item_frame_out_pthreadSafety__IO_4(struct TargetCheck * const pTargetCheck, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server TargetCheck
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
unsigned int targetCheck_item_get_frame_count_pthreadSafety(struct TargetCheck * const pTargetCheck, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server TargetCheck
#ifndef __item_get_frame_count_pthreadSafety 
    struct /* Recognition_item */ FsMacrosValue2(__item_frame_out_pthreadSafety__IO_4_Server, _item) * const /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)
            = (struct /* Recognition_item */ FsMacrosValue2(__item_frame_out_pthreadSafety__IO_4_Server, _item) *) /* pRecognition */FsMacrosValue2(p, __item_frame_out_pthreadSafety__IO_4_Server)->ro.
#ifdef __item_get_frame_count_pthreadSafety_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->pNode[/* pRecognition */FsMacrosValue2(p, __item_frame_out_pthreadSafety__IO_4_Server)->ro.
#ifdef __item_get_frame_count_pthreadSafety_itemListLock
            __itemList
#else
            __itemList_
#endif
            ->startIndex + index];
    if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item))return 0;

    return /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistIn->nodeCount + /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount;
#ifdef __item_get_frame_count_pthreadSafety_itemListLock
#undef __item_get_frame_count_pthreadSafety_itemListLock
#endif
#endif
}

/* 重置模块内通道对应的开方分组 */
void targetCheck_item_reset_groupSqrtOut(struct TargetCheck * const pTargetCheck, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server TargetCheck
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
