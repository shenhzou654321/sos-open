#include "vehicleDetect.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <bits/pthreadtypes.h>
#include <malloc.h>
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Image/ImageExtern.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../Modules/vehicleDetectLib/VehicleDetectLib.h"
#include "../../PublicTools/Analyze/ObjectTrack.h"

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
/* 缓存轨迹的最大数量 */
#define VehicleDetect_trail_count 64

struct VehicleDetect_P_item_track {
    struct ObjectTrack_pos_item objectTrack_pos_item;
    /* 目标编号,使用时间*1000+随机码表示 */
    unsigned long long objIndex;
    /* 已被识别的次数 */
    unsigned int recCount;
    /* 标记是否已认到车牌,0-未认到,1-已认到 */
    unsigned char hasPlate : 1;
    /* 关联的被识别对象,在认不到车牌时会尝试多次识别 */
    struct Recognition_item_Plate * pPlate;
    /* 上一张图片,为空表示没有上一张图片 */
    FsObjectImageFrame *pFrame_last;
};
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

static void vehicleDetect_P_item_track_clean(struct VehicleDetect_P_item_track * const pVehicleDetect_P_item_track) {
    {
        /* 关联的被识别对象,在认不到车牌时会尝试多次识别 */
        struct Recognition_item_Plate * const pPlate = pVehicleDetect_P_item_track->pPlate;
        if (pPlate)((FsObjectBase*) pPlate)->_delete(pPlate);
    }
    {
        FsObjectImageFrame * const pFrame_last = pVehicleDetect_P_item_track->pFrame_last;
        if (pFrame_last)fs_Object_delete_pthreadSafety__OI(pFrame_last);
    }

}

static void vehicleDetect_P_item_track_delete__OI(struct VehicleDetect_P_item_track * const pVehicleDetect_P_item_track) {
    vehicleDetect_P_item_track_clean(pVehicleDetect_P_item_track);
    objectTrack_pos_item_delete__OI((struct ObjectTrack_pos_item *) pVehicleDetect_P_item_track);
}

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int vehicleDetect_P_item_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */ struct VehicleDetect_item * const pVehicleDetect_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsStructList * const clientList = pVehicleDetect_item->ro.__clientList;
    pthread_mutex_lock(&clientList->mutex);
    configManager_conncet_refer_check(pVehicleDetect_item->ro._pVehicleDetect->ro._pConfigManager, clientList);
    pthread_mutex_unlock(&clientList->mutex);
    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int vehicleDetect_P_item_cb_debug( /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
            , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
            , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
            , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct VehicleDetect_item * const pVehicleDetect_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    const unsigned int datatype = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "datatype", 0);
    if (1 == datatype) {
        unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, datatype};
        pthread_mutex_lock(&pVehicleDetect_item->ro.__clientList->mutex);
        configManager_conncet_refer_check(pVehicleDetect_item->ro._pVehicleDetect->ro._pConfigManager, pVehicleDetect_item->ro.__clientList);
        if (fs_StructList_insert_order(pVehicleDetect_item->ro.__clientList, data) < 0) {
            pthread_mutex_unlock(&pVehicleDetect_item->ro.__clientList->mutex);
            return 1;
        } else {
            pthread_mutex_unlock(&pVehicleDetect_item->ro.__clientList->mutex);
            return -128;
        }
        //    } else if (0 == datatype) {
        //        /* 获取掩码图 */
        //      
    } else {
        FsLog(FsLogType_error, FsPrintfIndex, "Invalid datatype(=%u).\n", datatype);
        FsLogTag(10);
        fflush(stdout);
        return -1;
    }
}

/* 在期望连接的命令字和uuid改变时调用 */
static void vehicleDetect_P_item_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[]
        , /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p
        , /* 关联的对象 */ struct VehicleDetect_item * const pVehicleDetect_item) {
    if (strcmp("plate_recognition", cmd) == 0
#ifdef FsDebug
            && strcmp(uuid, pVehicleDetect_item->ro._uuid) == 0
#endif
            ) {
        pthread_mutex_lock(&pVehicleDetect_item->p.__mutexCmdConnect);
        pVehicleDetect_item->p.add_plateRecognition_pthreadSafety__IO = (Recognition_item_add_plateRecognition_pthreadSafety__IO) do_local;
        pVehicleDetect_item->p.pRecognition_item = (struct Recognition_item *) do_local_p;
        pthread_mutex_unlock(&pVehicleDetect_item->p.__mutexCmdConnect);
    } else if (strcmp("record_watch", cmd) == 0
#ifdef FsDebug
            && strcmp(uuid, pVehicleDetect_item->ro._uuid) == 0
#endif
            ) {
        pthread_mutex_lock(&pVehicleDetect_item->p.__mutexCmdConnect);
        pVehicleDetect_item->p.add_watch_pthreadSafety__OI_2 = (Record_item_add_watch_pthreadSafety__OI_2) do_local;
        pVehicleDetect_item->p.pRecord_item = (struct Record_item *) do_local_p;
        pthread_mutex_unlock(&pVehicleDetect_item->p.__mutexCmdConnect);
    } else {
        FsPrintf(FsPrintfIndex, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pVehicleDetect_item->ro._uuid);
        fflush(stdout);
    }
}

static FsConfig *vehicleDetect_P_protocol_debug() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vehicleDetect_debug", "vehicleDetect_debug", "vehicleDetect_debug");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "datatype", "数据类型", "数据类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 6, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "目标框图(图片流)", "目标框图(图片流)");
        // fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "抓拍掩码图", "抓拍掩码图");
    }

    return pConfig;
}

/* 删除pVehicleDetect_item指向的实例对象 */
static void vehicleDetect_P_item_delete__OI(struct VehicleDetect_item * const pVehicleDetect_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    void *const pConfigManager = pVehicleDetect_item->ro._pVehicleDetect->ro._pConfigManager;
    // 注销连接断开时的回调通知
    if (0 == pVehicleDetect_item->ro._ipv4) configManager_connect_error_logoff(pConfigManager, (int (*)(const unsigned int *, void *, char * * const))vehicleDetect_P_item_cb_connect_error, pVehicleDetect_item);
    /* 注销命令字 */
    if (0 == pVehicleDetect_item->ro._ipv4) configManager_cmd_logoff(pConfigManager, "vehicleDetect_debug", pVehicleDetect_item->ro._uuid, pVehicleDetect_item->ro._ipv4, pVehicleDetect_item, pShareBuffer);
    /* 解除命令字绑定 */
    if (0 == pVehicleDetect_item->ro._ipv4)configManager_cmd_disconnect(pConfigManager, "plate_recognition", pVehicleDetect_item->ro._uuid, pVehicleDetect_item);
    if (0 == pVehicleDetect_item->ro._ipv4)configManager_cmd_disconnect(pConfigManager, "record_watch", pVehicleDetect_item->ro._uuid, pVehicleDetect_item);

    if (0 == pVehicleDetect_item->ro._ipv4) {
        /* 传入的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pVehicleDetect_item->ro.__framelistIn);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pVehicleDetect_item->ro.__framelistOut);
        /* 关闭所有与之相关的客户端链表 */
        configManager_conncet_refer_close(pConfigManager, pVehicleDetect_item->ro.__clientList, 1);
        /* 与之相关的客户端链表 */
        fs_StructList_delete__OI(pVehicleDetect_item->ro.__clientList);
        /* 连接和调用外部命令所需要的同步锁 */
        pthread_mutex_destroy(&pVehicleDetect_item->p.__mutexCmdConnect);
        /* 跟踪中的所有目标 */
        fs_ObjectList_delete_unsafe_custom__OI(pVehicleDetect_item->p.__objectList_, (void (*)(void*))vehicleDetect_P_item_track_delete__OI);
    }
    fsFree(pVehicleDetect_item);
}

static void vehicleDetect_P_item_new_vehicleDetect(struct VehicleDetect_item * const pVehicleDetect_item, unsigned int index, unsigned short width, unsigned short height
        , /* 参考标定的宽度 */unsigned int width0, /* 参考标定的高度 */ unsigned int height0
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    ConfigManager * const pConfigManager = (ConfigManager*) pVehicleDetect_item->ro._pVehicleDetect->ro._pConfigManager;
    configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
    FsConfig * const pConfig = pConfigManager->ro.__pConfig;
    const void *vehicleDetectConfig0 = pConfig;
    const void *vehicleDetectConfig;
    {
        vehicleDetectConfig = fs_Config_node_template_get_orderFirst(pConfig, &vehicleDetectConfig0, pConfig, 0, pConfigManager->ro.__ipList_local, 0, "vsys", 0);
        if (vehicleDetectConfig) {
            vehicleDetectConfig = fs_Config_node_template_get(pConfig, &vehicleDetectConfig0, vehicleDetectConfig, 0, NULL, 0, "vsysChannel", index);
            if (vehicleDetectConfig) {
                vehicleDetectConfig = fs_Config_node_get_first(pConfig, &vehicleDetectConfig0, vehicleDetectConfig, "vehicleDetectConfig");
            }
        }
    }
    int x1 = 0, y1 = 0, x2 = width - 1, y2 = height - 1;
    if (vehicleDetectConfig) {
        FsConfigResult_area * const pArea = fs_Config_node_string_area_lenth_get__IO(pConfig, vehicleDetectConfig0, vehicleDetectConfig, "vehicleDetectErea area", NULL, width0 * 1.0 / width, height0 * 1.0 / height, 0, 0);
        if (pArea) {
            unsigned int uj;
            x1 = width - 1, y1 = height - 1, x2 = 0, y2 = 0;
            for (uj = 0; uj < pArea->count; uj++) {
                struct Fs_Points * const pPoint = (struct Fs_Points *) pArea->data[uj];
                unsigned int ui = pPoint->count;
                int (*thePoint)[2] = pPoint->point;
                while (ui-- > 0) {
                    int x = thePoint[0][0], y = thePoint[0][1];
                    thePoint++;
                    if (x1 > x)x1 = x;
                    if (x2 < x)x2 = x;
                    if (y1 > y)y1 = y;
                    if (y2 < y)y2 = y;
                }
            }
            fsFree(pArea);
            if (x2 > width - 1)x2 = width - 1;
            if (y2 > height - 1)y2 = height - 1;
            if (x2 <= x1 || y2 <= y1) {
                x1 = 0, y1 = 0, x2 = width - 1, y2 = height - 1;
            }
        }
    }
    configManager_config_deleteRefer_pthreadSafety(pConfigManager);
    /* 检测区域 */
    pVehicleDetect_item->p.x1 = x1, pVehicleDetect_item->p.y1 = y1, pVehicleDetect_item->p.x2 = x2, pVehicleDetect_item->p.y2 = y2;

}

static void vehicleDetect_P_item_new(struct VehicleDetect * const pVehicleDetect, /* 通道号,从1开始,为0用于集群 */const unsigned int channel
        , /* 本地ip地址 */const FsArray * const ipList, /* 原来的检测项 */struct VehicleDetect_item * * const ppVehicleDetect_item, FsGroupSqrt * const pGroupSqrt
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig *pConfig = ((ConfigManager*) pVehicleDetect->ro._pConfigManager)->ro.__pConfig;
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
                        if (*ppVehicleDetect_item != NULL && (*ppVehicleDetect_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppVehicleDetect_item) {
                        vehicleDetect_P_item_delete__OI(*ppVehicleDetect_item, pShareBuffer);
                    }
                    ////////////////////////////////////////////////////////////
                    struct VehicleDetect_item * rst;
                    if ((VehicleDetect_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0 && VehicleDetect_Mask != 0) rst = NULL;
                    else {
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        {
                            char *pd;
                            unsigned int len = uuid->lenth;
                            if (channel > 0) {
                                rst = (struct VehicleDetect_item*) fsMalloc(Memery_Alignment(sizeof (struct VehicleDetect_item)) + len);
                                memset(rst, 0, sizeof (struct VehicleDetect_item));
                                pd = ((char*) rst) + Memery_Alignment(sizeof (struct VehicleDetect_item));
                                /* 传入的数据帧链表 */
                                rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                                /* 传出的数据帧链表 */
                                rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                                /* 与之相关的客户端链表 */
                                rst->ro.__clientList = fs_StructList_new__IO(6, sizeof (int)*6);
                                /* 连接和调用外部命令所需要的同步锁 */
                                pthread_mutex_init(&rst->p.__mutexCmdConnect, NULL);
                                /* 跟踪中的所有目标 */
                                rst->p.__objectList_ = fs_ObjectList_new_unsafe__IO(5);
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
                                Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pVehicleDetect->ro._pGroupSqrtOut->groupSqrt_member, pVehicleDetect->ro._pGroupSqrtOut->groupSqrt_group
                                        , pVehicleDetect->ro._pGroupSqrtOut->groupValue, channel - 1, &pVehicleDetect->ro._pGroupSqrtOut->groupSqrt_mutex);
                            }
                            ////////////////////////////////////////////////////
                            if (channel > 0) {
                                /* 所属主机的ip地址,用于集群,本机为0 */
                                rst->ro._ipv4 = ipv4;
                                /* 本对象的uuid值,一般是从配置中读入的 */
                                rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len);
                                /* 所属的车辆检测对象 */
                                rst->ro._pVehicleDetect = pVehicleDetect;
                                /* 帧间隔 */
                                rst->p._frameInterval = 5;
                                /* 帧率 */
                                rst->p._frameRate = 25.0;
                            }
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        // 注册连接断开时的回调通知
                        if (0 == ipv4) configManager_connect_error_register(pVehicleDetect->ro._pConfigManager
                                , (int (*)(const unsigned int *, void *, char * * const))vehicleDetect_P_item_cb_connect_error, rst);
                        /* 注册命令字 */
                        if (0 == ipv4) configManager_cmd_register(pVehicleDetect->ro._pConfigManager, "vehicleDetect_debug", rst->ro._uuid, ipv4, rst, 0
                                , (int (* const) (const unsigned int *,unsigned char, unsigned char, unsigned int, unsigned int,  FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))vehicleDetect_P_item_cb_debug, NULL, rst, pShareBuffer);
                        //                    if (0 == ipv4)configManager_cmd_register(pRecord->ro._pConfigManager, "record_inc", rst->ro._uuid, 0, rst, NULL, (void*) record_item_add_inc_pthreadSafety__OI_2, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "videolist_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "videotimeinfo_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "snapshort_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_snapshort_cb : NULL, NULL, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        if (0 == ipv4)configManager_cmd_connect(pVehicleDetect->ro._pConfigManager, "plate_recognition", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))vehicleDetect_P_item_cmd_connect_cb, rst);
                        if (0 == ipv4)configManager_cmd_connect(pVehicleDetect->ro._pConfigManager, "record_watch", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))vehicleDetect_P_item_cmd_connect_cb, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 释放内存空间 */
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0) *ppVehicleDetect_item = rst;
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
static void vehicleDetect_P_item_send_vehicleDetect_debug(struct VehicleDetect_item * const pVehicleDetect_item, FsObjectImageFrame * const pFrame
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer) {
    FsStructList * const clientList = pVehicleDetect_item->ro.__clientList;
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
                /* 目标框图 */
            case 1:
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
                    ppObjectBase[2] = (FsObjectBase*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RGB_0);
                    if (NULL == ppObjectBase[2]) {
                        FsLog(FsLogType_error, FsPrintfIndex, "Prapare RGB_0 failed.\n");
                        FsLogTag(10);
                        fflush(stdout);
                        head = 0;
                        break;
                    }
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                {
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
                /* 画轨迹 */
                {
                    ppObjectBaseUseCount += 3;
                    /* 初始化ppObjectBase */
                    // 抓车框
                    unsigned int *recData;
                    // 轨迹头
                    unsigned int trailHeadLen;
                    // 轨迹
                    unsigned int *trailData;
                    // 文本信息
                    unsigned char *textData;
                    {
                        /* 存放抓车框 */
                        {
                            const unsigned int lineCount = pVehicleDetect_item->p.__objectList_->nodeCount * 4;
                            const unsigned int recLen = image_extern_line_write_size_get(NULL, lineCount, sizeof ("tt") - 1);
                            ppObjectBase[4] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + recLen, sizeof (FsObjectBasePthreadSafety));
                            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[4]);
                            /* 把pObjectBase[4]后面的数据加入缓存 */
                            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[4], sizeof (FsObjectBasePthreadSafety) + recLen);
                            ppObjectBase[4]->len = recLen;

                            recData = (unsigned int*) image_extern_line_write_head((unsigned char*) ppObjectBase[4]->data, NULL, 1, sizeof ("tt") - 1, "tt"
                                    , pFrame->width[0], pFrame->height[0], 0x003CFF00, 1.0, 1.0, lineCount);
                        }
                        /* 存放轨迹 */
                        {
                            unsigned char fillLen;
                            const unsigned int lineCount = pVehicleDetect_item->p.__objectList_->nodeCount*VehicleDetect_trail_count;
                            trailHeadLen = image_extern_line_write_size_get(&fillLen, lineCount, sizeof ("tt") - 1);
                            ppObjectBase[5] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + trailHeadLen + lineCount * 16 + fillLen, sizeof (FsObjectBasePthreadSafety));
                            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[5]);
                            /* 把pObjectBase[5]后面的数据加入缓存 */
                            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[5], sizeof (FsObjectBasePthreadSafety) + trailHeadLen + lineCount * 16 + fillLen);

                            trailData = (unsigned int*) (ppObjectBase[5]->data + trailHeadLen);
                        }
                        /* 文本信息 */
                        {
                            unsigned char fillLen;
                            const unsigned int headSize = image_extern_text_write_size_get(&fillLen, 1, sizeof ("tt") - 1);
                            ppObjectBase[6] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety)
                                    + (headSize + 32/* 文本内容大小 */ + 64) * pVehicleDetect_item->p.__objectList_->nodeCount, sizeof (FsObjectBasePthreadSafety));
                            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[6]);
                            /* 把pObjectBase[6]后面的数据加入缓存 */
                            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[6], sizeof (FsObjectBasePthreadSafety)
                                    + (headSize + 32/* 文本内容大小 */ + 64) * pVehicleDetect_item->p.__objectList_->nodeCount);

                            textData = (unsigned char*) ppObjectBase[6]->data;
                        }
                    }
                    {
                        struct ObjectTrack_pos_item * * const ppNode = (struct ObjectTrack_pos_item * *) pVehicleDetect_item->p.__objectList_->pNode + pVehicleDetect_item->p.__objectList_->startIndex;
                        const unsigned int count = pVehicleDetect_item->p.__objectList_->nodeCount;
                        unsigned int ui = 0;
                        for (; ui < count; ui++) {
                            struct ObjectTrack_pos_item * const pObjectTrack_pos_item = ppNode[ui];
                            int x1, y1, x2, y2;
                            {
                                struct ObjectTrack_pos_node * const pObjectTrack_pos_node = pObjectTrack_pos_item->tail;
                                x1 = pObjectTrack_pos_node->x1, y1 = pObjectTrack_pos_node->y1, x2 = pObjectTrack_pos_node->x2, y2 = pObjectTrack_pos_node->y2;
                            }
                            /* 抓车框 */
                            {
                                // (x1,y1)(x2,y1)
                                recData[0] = x1, recData[1] = y1, recData[2] = x2, recData[3] = y1, recData += 4;
                                // (x2,y1)(x2,y2)
                                recData[0] = x2, recData[1] = y1, recData[2] = x2, recData[3] = y2, recData += 4;
                                // (x2,y2)(x1,y2)
                                recData[0] = x2, recData[1] = y2, recData[2] = x1, recData[3] = y2, recData += 4;
                                // (x1,y2)(x1,y1)
                                recData[0] = x1, recData[1] = y2, recData[2] = x1, recData[3] = y1, recData += 4;
                            }
                            /* 文本信息 */
                            {
                                char text[32];
                                int textLen = snprintf(text, sizeof (text), "diff:%.3f,tag:%hhu", pObjectTrack_pos_item->diff, 0);
                                unsigned char * const pd = image_extern_text_write_head(textData, NULL, 1, sizeof ("tt") - 1, "tt"
                                        , pFrame->width[0], pFrame->height[0], 0x0000FFFF, x1, y1, 32, 32, textLen);
                                memcpy(pd, text, textLen);
                                textData += image_extern_text_write_size_get(NULL, textLen, sizeof ("tt") - 1);
                            }
                            /* 轨迹 */
                            if (pObjectTrack_pos_item->posCount > 1) {
                                struct ObjectTrack_pos_node * pObjectTrack_pos_node = pObjectTrack_pos_item->head;
                                x1 = (pObjectTrack_pos_node->x1 + pObjectTrack_pos_node->x2) / 2, y1 = (pObjectTrack_pos_node->y1 + pObjectTrack_pos_node->y2) / 2;
                                pObjectTrack_pos_node = pObjectTrack_pos_node->next;
                                do {
                                    x2 = (pObjectTrack_pos_node->x1 + pObjectTrack_pos_node->x2) / 2, y2 = (pObjectTrack_pos_node->y1 + pObjectTrack_pos_node->y2) / 2;
                                    trailData[0] = x1, trailData[1] = y1, trailData[2] = x2, trailData[3] = y2, trailData += 4;
                                    x1 = x2, y1 = y2;
                                } while ((pObjectTrack_pos_node = pObjectTrack_pos_node->next) != NULL);
                            }
                        }
                    }
                    {
                        /* 抓车框 */
                        len += ppObjectBase[4]->len;
                        /* 轨迹 */
                        {
                            unsigned int lineCount = (((char*) trailData)-(ppObjectBase[5]->data + trailHeadLen)) / 16;
                            image_extern_line_write_head((unsigned char*) ppObjectBase[5]->data, NULL, 1, sizeof ("tt") - 1, "tt"
                                    , pFrame->width[0], pFrame->height[0], 0x00FF00F0, 1.0, 1.0, lineCount);
                            ppObjectBase[5]->len = lineCount = image_extern_line_write_size_get(NULL, lineCount, sizeof ("tt") - 1);
                            len += lineCount;
                        }
                        /* 文本信息 */
                        {
                            unsigned int textLen = textData - (unsigned char*) ppObjectBase[6]->data;
                            if (textLen > 0) {
                                ppObjectBase[6]->len = textLen;
                                len += textLen;
                            } else {
                                /* 跳过数据 */
                                ppObjectBaseUseCount--;
                                fs_ObjectBasePthreadSafety_delete__OI(ppObjectBase[6]);
                            }
                        }
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

/* 从配置获取通道数 */
static unsigned int vehicleDetect_P_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

static inline void vehicleDetect_P_updateframeRate_frameInterval(struct VehicleDetect_item * const pVehicleDetect_item, /* 帧率 */ float frameRate) {
#define __vehicleDetect_frameInterval 4.0
    pVehicleDetect_item->p._frameRate += (frameRate - pVehicleDetect_item->p._frameRate) / (pVehicleDetect_item->p._frameRate + 1.0);
    unsigned int rv = (pVehicleDetect_item->p._frameRate + pVehicleDetect_item->p.frameIntervalCompensate) / __vehicleDetect_frameInterval;
    if (rv != pVehicleDetect_item->p._frameInterval) {
        rv = pVehicleDetect_item->p._frameRate / __vehicleDetect_frameInterval;
        if (rv < 1)rv = 1;
        pVehicleDetect_item->p._frameInterval = rv;
        pVehicleDetect_item->p.frameIntervalCompensate = (rv + 0.5) * __vehicleDetect_frameInterval - pVehicleDetect_item->p._frameRate;
    }
}

/* 检测,成功返回1,失败返回-1 */
static inline int vehicleDetect_P_item_do_detect(/* 检测项 */struct VehicleDetect_item * const pVehicleDetect_item, FsObjectImageFrame * const pFrame, unsigned int *const pObjIndex
        , FsObjectBaseBuffer * const pObjectBaseBuffer
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName vehicleDetect_P_item_do_detect
    FsObjectImageRGB * const pRGB = (FsObjectImageRGB*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RGB_0);
    if (NULL == pRGB) {
        FsLog(FsLogType_error, FsPrintfIndex, "prapare RGB failed.\n");
        fflush(stdout);
        return -1;
    }
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
    int retLen = 1024;
    FsLocal_ShareBuffer_resize(retLen + 1);
    char *cResult = FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
    {
        const int x1 = pVehicleDetect_item->p.x1, y1 = pVehicleDetect_item->p.y1, x2 = pVehicleDetect_item->p.x2, y2 = pVehicleDetect_item->p.y2;
        vehicleDetect(y2 - y1 + 1, x2 - x1 + 1, pFrame->width[0] * 3, pRGB->data + pFrame->width[0] * 3 * y1 + x1 * 3
                , pFrame->width[0] * 3 * (y2 - y1 + 1), &cResult, &retLen);
        if (retLen > 1024) {
            FsPrintf(FsPrintfIndex, "Invalid retLen(=%d).\n", retLen);
            fflush(stdout);
        }
        cResult[retLen] = 0;
        //FsPrintf(FsPrintfIndex, "x1=%d,y1=%d,x2=%d,y2=%d,cResult[%d]:%s\n", x1, y1, x2, y2, retLen, cResult), fflush(stdout);
    }
    fs_Object_delete_pthreadSafety__OI(pRGB);
    /* 解析坐标,存于ObjectTrack_pos_data结构体中 */
    unsigned int recCount = 0;
    const unsigned int objectTrack_pos_dataPos = Memery_Alignment4(FsLocal_ShareBufferPos + retLen + 1);
    {
        int i;
        const int x0 = pVehicleDetect_item->p.x1, y0 = pVehicleDetect_item->p.y1;
        for (i = 0; i < retLen; i++) {
            int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            // x1
            for (; i < retLen; i++) {
                if ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] >= '0' && (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] <= '9') {
                    x1 = x1 * 10 + ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] - '0');
                } else break;
            }
            if (i >= retLen)break;
            // y1
            for (i++; i < retLen; i++) {
                if ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] >= '0' && (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] <= '9') {
                    y1 = y1 * 10 + ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] - '0');
                } else break;
            }
            if (i >= retLen)break;
            // x2
            for (i++; i < retLen; i++) {
                if ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] >= '0' && (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] <= '9') {
                    x2 = x2 * 10 + ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] - '0');
                } else break;
            }
            if (i >= retLen)break;
            // y2
            for (i++; i < retLen; i++) {
                if ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] >= '0' && (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] <= '9') {
                    y2 = y2 * 10 + ((FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[i] - '0');
                } else break;
            }
            if (i >= retLen)break;

            FsLocal_ShareBuffer_resize(objectTrack_pos_dataPos - FsLocal_ShareBufferPos + (recCount + 1) * sizeof (struct ObjectTrack_pos_data));
            objectTrack_pos_data_set(((struct ObjectTrack_pos_data*) (FsLocal_ShareBuffer + objectTrack_pos_dataPos)) + recCount++, x1 + x0, y1 + y0, x2 + x0, y2 + y0);
        }
    }
    /* 目标跟踪 */
    {
        struct ObjectTrack_pos_data * const pObjectTrack_pos_data = (struct ObjectTrack_pos_data*) (FsLocal_ShareBuffer + objectTrack_pos_dataPos);
        FsObjectList * const objectList_ = pVehicleDetect_item->p.__objectList_;
        const unsigned int frameIndex = pFrame->index;
        const unsigned int matchCount = objectTrack_pos_update(objectList_, pFrame->capture_gmtTime, frameIndex, 10, sizeof (struct ObjectTrack_pos_node), NULL, (void (*)(struct ObjectTrack_pos_item*)) vehicleDetect_P_item_track_clean
                , pObjectTrack_pos_data, sizeof ( struct ObjectTrack_pos_data), recCount, VehicleDetect_trail_count, 0.5);
        //FsPrintf(1, "matchCount=%u,objectList_->nodeCount=%lu\n", matchCount, pVehicleDetect_item->p.__objectList_->nodeCount);
        struct ObjectTrack_pos_item * * const ppNode_objectList = (struct ObjectTrack_pos_item * *) objectList_->pNode + objectList_->startIndex;
        const unsigned int count_objectList = objectList_->nodeCount;
        if (matchCount < recCount) {
            /* 有框没匹配,把没匹配的框创建新的跟踪目标 */
            unsigned long long objIndex = time(NULL)*1000;
            unsigned int uj = 0;
            for (; uj < recCount; uj++) {
                if (pObjectTrack_pos_data[uj].tag) {
                    FsMacrosFunctionTag(new_obj_skip) :;
                    continue;
                }
                const int x1 = pObjectTrack_pos_data[uj].x1, y1 = pObjectTrack_pos_data[uj].y1, x2 = pObjectTrack_pos_data[uj].x2, y2 = pObjectTrack_pos_data[uj].y2;
                /* 与轨迹匹配 */
                {
                    const int x0 = (x1 + x2) / 2, y0 = (y1 + y2) / 2;
                    unsigned int ui = 0;
                    for (; ui < count_objectList; ui++) {
                        struct ObjectTrack_pos_node * const pObjectTrack_pos_node = ppNode_objectList[ui]->tail;
                        if (x1 >= pObjectTrack_pos_node->x1 && x2 <= pObjectTrack_pos_node->x2 && y1 >= pObjectTrack_pos_node->y1 && y2 <= pObjectTrack_pos_node->y2) {
                            int _x0 = (pObjectTrack_pos_node->x1 + pObjectTrack_pos_node->x2) / 2 - x0, _y0 = (pObjectTrack_pos_node->y1 + pObjectTrack_pos_node->y2) / 2 - y0;
                            if (_x0 < 0)_x0 = -_x0;
                            if (_y0 < 0)_y0 = -_y0;
                            if (x2 - x1 > y2 - y1) {
                                //                                FsPrintf(FsPrintfIndex, "(x1 - pObjectTrack_pos_node->x1) + (pObjectTrack_pos_node->x2 - x2) + _x0 + _y0=%d"
                                //                                        ",((x2 - x1)+ (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1))/4*3 =%d.\n"
                                //                                        , (x1 - pObjectTrack_pos_node->x1) + (pObjectTrack_pos_node->x2 - x2) + _x0 + _y0
                                //                                        , ((x2 - x1)+ (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1))/4*3);
                                //                                fflush(stdout);
                                if ((x1 - pObjectTrack_pos_node->x1) + (pObjectTrack_pos_node->x2 - x2) + _x0 + _y0 < ((x2 - x1)+ (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1)) / 4 * 3) {
                                    FsPrintf2(FsPrintfIndex, "Skip in obj x1=%d/%d,y1=%d/%d,x2=%d/%d,y2=%d/%d.\n"
                                            , x1, pObjectTrack_pos_node->x1, y1, pObjectTrack_pos_node->y1, x2, pObjectTrack_pos_node->x2, y2, pObjectTrack_pos_node->y2);
                                    fflush(stdout);
                                    goto FsMacrosFunctionTag(new_obj_skip);
                                }
                            } else {
                                //                                FsPrintf(FsPrintfIndex, "(y1 - pObjectTrack_pos_node->y1) + (pObjectTrack_pos_node->y2 - y2) + _x0 + _y0 =%d"
                                //                                        ",((y2 - y1)+ (pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1))/4*3 =%d.\n"
                                //                                        , (y1 - pObjectTrack_pos_node->y1) + (pObjectTrack_pos_node->y2 - y2) + _x0 + _y0
                                //                                        , ((y2 - y1)+ (pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1))/4*3);
                                //                                fflush(stdout);
                                if ((y1 - pObjectTrack_pos_node->y1) + (pObjectTrack_pos_node->y2 - y2) + _x0 + _y0 < ((y2 - y1)+ (pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1)) / 4 * 3) {
                                    FsPrintf2(FsPrintfIndex, "Skip out obj x1=%d/%d,y1=%d/%d,x2=%d/%d,y2=%d/%d.\n"
                                            , x1, pObjectTrack_pos_node->x1, y1, pObjectTrack_pos_node->y1, x2, pObjectTrack_pos_node->x2, y2, pObjectTrack_pos_node->y2);
                                    fflush(stdout);
                                    goto FsMacrosFunctionTag(new_obj_skip);
                                }
                            }
                        } else if (x1 <= pObjectTrack_pos_node->x1 && x2 >= pObjectTrack_pos_node->x2 && y1 <= pObjectTrack_pos_node->y1 && y2 >= pObjectTrack_pos_node->y2) {
                            int _x0 = (pObjectTrack_pos_node->x1 + pObjectTrack_pos_node->x2) / 2 - x0, _y0 = (pObjectTrack_pos_node->y1 + pObjectTrack_pos_node->y2) / 2 - y0;
                            if (_x0 < 0)_x0 = -_x0;
                            if (_y0 < 0)_y0 = -_y0;
                            if (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1 > pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1) {
                                //                                FsPrintf(FsPrintfIndex, "(x1 - pObjectTrack_pos_node->x1) + (pObjectTrack_pos_node->x2 - x2) + _x0 + _y0=%d"
                                //                                        ",((x2 - x1)+ (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1))/4*3 =%d.\n"
                                //                                        , (x1 - pObjectTrack_pos_node->x1) + (pObjectTrack_pos_node->x2 - x2) + _x0 + _y0
                                //                                        , ((x2 - x1)+ (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1))/4*3);
                                //                                fflush(stdout);
                                if (-(x1 - pObjectTrack_pos_node->x1) - (pObjectTrack_pos_node->x2 - x2) + _x0 + _y0 < ((x2 - x1)+ (pObjectTrack_pos_node->x2 - pObjectTrack_pos_node->x1)) / 4 * 3) {
                                    FsPrintf2(FsPrintfIndex, "Skip in obj x1=%d/%d,y1=%d/%d,x2=%d/%d,y2=%d/%d.\n"
                                            , x1, pObjectTrack_pos_node->x1, y1, pObjectTrack_pos_node->y1, x2, pObjectTrack_pos_node->x2, y2, pObjectTrack_pos_node->y2);
                                    fflush(stdout);
                                    goto FsMacrosFunctionTag(new_obj_skip);
                                }
                            } else {
                                //                                FsPrintf(FsPrintfIndex, "(y1 - pObjectTrack_pos_node->y1) + (pObjectTrack_pos_node->y2 - y2) + _x0 + _y0 =%d"
                                //                                        ",((y2 - y1)+ (pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1))/4*3 =%d.\n"
                                //                                        , (y1 - pObjectTrack_pos_node->y1) + (pObjectTrack_pos_node->y2 - y2) + _x0 + _y0
                                //                                        , ((y2 - y1)+ (pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1))/4*3);
                                //                                fflush(stdout);
                                if (-(y1 - pObjectTrack_pos_node->y1) - (pObjectTrack_pos_node->y2 - y2) + _x0 + _y0 < ((y2 - y1)+ (pObjectTrack_pos_node->y2 - pObjectTrack_pos_node->y1)) / 4 * 3) {
                                    FsPrintf2(FsPrintfIndex, "Skip  obj x1=%d/%d,y1=%d/%d,x2=%d/%d,y2=%d/%d.\n"
                                            , x1, pObjectTrack_pos_node->x1, y1, pObjectTrack_pos_node->y1, x2, pObjectTrack_pos_node->x2, y2, pObjectTrack_pos_node->y2);
                                    fflush(stdout);
                                    goto FsMacrosFunctionTag(new_obj_skip);
                                }
                            }
                        }
                    }
                }
                struct VehicleDetect_P_item_track * const pVehicleDetect_p_item_track = (struct VehicleDetect_P_item_track *) fsMalloc(sizeof ( struct VehicleDetect_P_item_track));
                objectTrack_pos_item_init_by_pos_data((struct ObjectTrack_pos_item *) pVehicleDetect_p_item_track, pFrame->capture_gmtTime, frameIndex, NULL
                        , pObjectTrack_pos_data + uj, sizeof (struct ObjectTrack_pos_node));
                /* 目标编号,使用时间*1000+随机码表示 */
                pVehicleDetect_p_item_track->objIndex = objIndex + (*pObjIndex) % 1000;
                if (FsNumberIsMax(*pObjIndex))*pObjIndex = 0;
                else (*pObjIndex)++;
                /* 已被识别的次数,为0表示从来没有被识别过 */
                pVehicleDetect_p_item_track->recCount = 0;
                /* 标记是否已认到车牌,0-未认到,1-已认到 */
                pVehicleDetect_p_item_track->hasPlate = 0;
                /* 关联的被识别对象,在认不到车牌时会尝试多次识别 */
                pVehicleDetect_p_item_track->pPlate = NULL;
                /* 上一张图片,为空表示没有上一张图片 */
                pVehicleDetect_p_item_track->pFrame_last = NULL;

                fs_ObjectList_insert_tail(objectList_, pVehicleDetect_p_item_track);
            }
        }
    }
    /* 识别车牌 */
    {
        struct VehicleDetect_P_item_track * * const ppNode = (struct VehicleDetect_P_item_track * *) pVehicleDetect_item->p.__objectList_->pNode + pVehicleDetect_item->p.__objectList_->startIndex;
        const unsigned int count = pVehicleDetect_item->p.__objectList_->nodeCount;
        unsigned int i = 0;
        for (; i < count; i++) {
            struct VehicleDetect_P_item_track * const pVehicleDetect_p_item_track = ppNode[i];
            struct Recognition_item_Plate * pPlate = pVehicleDetect_p_item_track->pPlate;
            if (pPlate) {
                if (pPlate->_tag) {
                    /* 已识别 */
                    if (pPlate->_tag & 0x2) {
                        /* 已认到车牌 */
                        pVehicleDetect_p_item_track->hasPlate = 1;
                    }
                    pVehicleDetect_p_item_track->pPlate = NULL;
                    ((FsObjectBase *) pPlate)->_delete(pPlate);
                    pPlate = NULL;
                }
            }
            if (NULL == pPlate && 0 == pVehicleDetect_p_item_track->hasPlate && pVehicleDetect_p_item_track->pFrame_last != NULL) {
                /* 没有认到车牌 */
                pthread_mutex_lock(&pVehicleDetect_item->p.__mutexCmdConnect);
                if (pVehicleDetect_item->p.add_plateRecognition_pthreadSafety__IO) {
                    struct ObjectTrack_pos_node * const pObjectTrack_pos_node = ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->tail;
                    pPlate = pVehicleDetect_item->p.add_plateRecognition_pthreadSafety__IO(pVehicleDetect_item->p.pRecognition_item, pObjectBaseBuffer
                            , pFrame, pObjectTrack_pos_node->x1, pObjectTrack_pos_node->y1, pObjectTrack_pos_node->x2, pObjectTrack_pos_node->y2,
                            pVehicleDetect_p_item_track->pFrame_last, ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->x1_tailBefore
                            , ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->y1_tailBefore, ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->x2_tailBefore
                            , ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->y2_tailBefore);
                    if (pPlate) {
                        pVehicleDetect_p_item_track->pPlate = pPlate;
                        /* 已被识别的次数 */
                        pVehicleDetect_p_item_track->recCount++;
                        /* 输出卡口记录 */
                        if (pVehicleDetect_item->p.add_watch_pthreadSafety__OI_2) {
                            struct Record_item_Watch * const pWatch = (struct Record_item_Watch*) fsMalloc(sizeof (struct Record_item_Watch));
                            memset(pWatch, 0, sizeof (struct Record_item_Watch));
                            /* 目标编号,使用时间*1000+随机码表示 */
                            pWatch->_objIndex = pVehicleDetect_p_item_track->objIndex;
                            /* 被记录的次数,从1开始 */
                            pWatch->_recordIndex = pVehicleDetect_p_item_track->recCount;
                            if (pObjectTrack_pos_node->y2 > ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->y2_tailBefore) {
                                // 由远及近
                                /* 目标位置 */
                                pWatch->x1 = ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->x1_tailBefore, pWatch->y1 = ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->y1_tailBefore;
                                pWatch->x2 = ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->x2_tailBefore, pWatch->y2 = ((struct ObjectTrack_pos_item*) pVehicleDetect_p_item_track)->y2_tailBefore;
                                /* 图片信息,没有图片的必须置空 */
                                fs_Object_addRefer_pthreadSafety(pVehicleDetect_p_item_track->pFrame_last, __LINE__);
                                pWatch->_ppFrame[0] = pVehicleDetect_p_item_track->pFrame_last;
                            } else {
                                // 由近及远
                                /* 目标位置 */
                                pWatch->x1 = pObjectTrack_pos_node->x1, pWatch->y1 = pObjectTrack_pos_node->y1, pWatch->x2 = pObjectTrack_pos_node->x2, pWatch->y2 = pObjectTrack_pos_node->y2;
                                /* 图片信息,没有图片的必须置空 */
                                fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
                                pWatch->_ppFrame[0] = pFrame;
                            }
                            /* 车牌识别对象,可为空 */
                            fs_ObjectBasePthreadSafety_addRefer(pPlate);
                            pWatch->_pPlate = pPlate;

                            pVehicleDetect_item->p.add_watch_pthreadSafety__OI_2(pVehicleDetect_item->p.pRecord_item, pWatch);
                        }
                    }
                }
                pthread_mutex_unlock(&pVehicleDetect_item->p.__mutexCmdConnect);
            }
            /* 更新最后一张图信息 */
            if (pVehicleDetect_p_item_track->pFrame_last)fs_Object_delete_pthreadSafety__OI(pVehicleDetect_p_item_track->pFrame_last);
            fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
            pVehicleDetect_p_item_track->pFrame_last = pFrame;
        }
    }
    /* 发送调试数据到客户端 */
    vehicleDetect_P_item_send_vehicleDetect_debug(pVehicleDetect_item, pFrame, pObjectBaseBuffer);

    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    return 1;
}

static void *vehicleDetect_P_T(struct VehicleDetect * const pVehicleDetect) {
#undef FsFunctionName
#define FsFunctionName vehicleDetect_P_T
    /* 创建线程监控 */
    void *pMonitor = pVehicleDetect->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, VehicleDetect_get_threadName(pVehicleDetect), NULL, pVehicleDetect, pVehicleDetect->ro._pMonitor != NULL ? pVehicleDetect->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pVehicleDetect->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pVehicleDetect->ro._pConfigManager;
    FsObjectList * const itemList_ = pVehicleDetect->ro.__itemList_;
    ////////////////////////////////////////////////////////////////////////////        
    /* 格式化变量 */
    /* 共享buffer,本函数不使用,只在子函数中使用 */
    FsShareBuffer shareBuffer = NULL;
#define __vehicleDetect_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __vehicleDetect_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __vehicleDetect_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer); 
    ////////////////////////////////////////////////////////////////////////////    
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    float frameRateAvg = 25.0;
    double sleepToTime = 0.0;
    // InitVD初始化是否被调用
    char InitVDHasInit = 0;
    /* 目标编号 */
    unsigned int objIndex = 0;
    /* 开方分组 */
    FsGroupSqrt groupSqrt;
    Fs_GroupSqrt_init(&groupSqrt);
#define __vehicleDetect_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __vehicleDetect_P_T_clean1_count 4
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
    configManager_protocol_publish(pConfigManager, "vehicleDetect_debug", "车辆检测调试", 1, vehicleDetect_P_protocol_debug);
#define __vehicleDetect_P_T_clean2_1 {configManager_protocol_cancel(pConfigManager, "vehicleDetect_debug");}
#define __vehicleDetect_P_T_clean2_count 1
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pVehicleDetect->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        pMonitorItem->rw.line = __LINE__;
        fs_thread_sleepTo(sleepToTime);
        const double thisTime = fs_time_uptime_get();
        if (frameRateAvg < 5.0)frameRateAvg = 5.0;
        else frameRateAvg *= 0.9999;
        sleepToTime = thisTime + 1.0 / frameRateAvg;
        ////////////////////////////////////////////////////////////////////////
        if (pVehicleDetect->rw._resetChannel>-1) {
            if (frameRateAvg < 65535.0)frameRateAvg *= 4.0;
            /* 读取通道数 */
            if (0 == pVehicleDetect->rw._resetChannel) channelCount = vehicleDetect_P_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server VehicleDetect
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
                struct VehicleDetect_item * const pVehicleDetect_item = (struct VehicleDetect_item *) fs_ObjectList_remove_tail(itemList_);
                if (pVehicleDetect_item)vehicleDetect_P_item_delete__OI(pVehicleDetect_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pVehicleDetect->rw._resetChannel) {
                /* 出错 */
                FsPrintf(FsPrintfIndex, "Invalid channel(=%u/%lu).\n", pVehicleDetect->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pVehicleDetect->rw._resetChannel > 0) {
                FsPrintf(FsPrintfIndex, "%d\n", pVehicleDetect->rw._resetChannel);
                pMonitorItem->rw.line = __LINE__;
                vehicleDetect_P_item_new(pVehicleDetect, pVehicleDetect->rw._resetChannel, pConfigManager->ro.__ipList_local
                        , (struct VehicleDetect_item **) (itemList_->pNode + itemList_->startIndex + pVehicleDetect->rw._resetChannel - 1), &groupSqrt
                        , & shareBuffer);
                pMonitorItem->rw.line = __LINE__;
            }
            pVehicleDetect->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct VehicleDetect_item * * const ppNode_itemList = (struct VehicleDetect_item **) itemList_->pNode + itemList_->startIndex;
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
                struct VehicleDetect_item * const pVehicleDetect_item = ppNode_itemList[ui];
                if (NULL == pVehicleDetect_item)continue;
                if (0) {
                    if (pVehicleDetect_item->ro.__framelistIn->nodeCount > 0) {
                        pthread_mutex_lock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                        fs_ObjectList_insert_tail_all_clean_pthreadSafety(pVehicleDetect_item->ro.__framelistOut, pVehicleDetect_item->ro.__framelistIn);
                        pthread_mutex_unlock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                        Fs_GroupSqrtOut_value_set(pVehicleDetect_item->rw._pGroupSqrtOut, &pVehicleDetect_item->ro._pVehicleDetect->ro._pGroupSqrtOut->groupSqrt_mutex);
                    }
                }
                ////////////////////////////////////////////////////////////////
                //////////////////////// 状态机 开始 ////////////////////////////
                {
#define __vehicleDetect_P_T_state_check_end (1U<<0) // 检查缓存是否足够,不够退出循环
#define __vehicleDetect_P_T_state_check_module_init (1U<<1) // 检查模块初始化
#define __vehicleDetect_P_T_state_check_do (1U<<2) // 执行检测
#define __vehicleDetect_P_T_state_out_frame (1U<<3) // 输出数据帧
#define __vehicleDetect_P_T_state_end_break (1U<<4) // 退出循环
#define __vehicleDetect_P_T_state_reset (1U<<5) // 重置
#ifndef __vehicleDetect_P_T_state
                    unsigned int frameInterval = pVehicleDetect_item->p._frameInterval;
                    unsigned int state;
                    //state = FsMacrosFunction(state_check_end);
                    FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                    for (;;) {
                        FsObjectImageFrame * pFrame;
                        /* 检查缓存是否足够 */
                        if (state & FsMacrosFunction(state_check_end)) {
                            state ^= FsMacrosFunction(state_check_end);
                            FsMacrosFunctionTag(state_check_end) :;
                            ////////////////////////////////////////////////////
#define __vehicleDetect_P_T_state_frameBufferCount (frameInterval*1)
                            if (pVehicleDetect_item->ro.__framelistIn->nodeCount >= FsMacrosFunction(state_frameBufferCount)) {
                                /* 检查数据 */
                                pthread_mutex_lock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                                pFrame = (FsObjectImageFrame*) pVehicleDetect_item->ro.__framelistIn->pNode[ pVehicleDetect_item->ro.__framelistIn->startIndex + frameInterval - 1];
                                pthread_mutex_unlock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                                if (pVehicleDetect_item->ro.classIndex != pFrame->classIndex) {
                                    pVehicleDetect_item->ro.classIndex = pFrame->classIndex;
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
                            ////////////////////////////////////////////////////
                            if (0 == pVehicleDetect_item->p.hasInit) {
                                if (0 == InitVDHasInit) {
                                    FsPrintf2(FsPrintfIndex, "Run InitVD start.\n");
                                    if (InitVD1() == 1)InitVDHasInit = 1;
                                    else {
                                        FsPrintf(FsPrintfIndex, "InitVD1 failed.\n");
                                        fflush(stdout);
                                        /* 重置 */
                                        FsMacrosSetState_OR_GotoFunctionTag(state, state_reset);
                                    }
                                    FsPrintf2(FsPrintfIndex, "Run InitVD end.\n");
                                }
                                pthread_mutex_lock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                                unsigned int ui;
                                for (ui = 0; ui < FsMacrosFunction(state_frameBufferCount); ui++) {
                                    pFrame = (FsObjectImageFrame*) pVehicleDetect_item->ro.__framelistIn->pNode[ pVehicleDetect_item->ro.__framelistIn->startIndex + ui];
                                    if (pVehicleDetect_item->ro.classIndex != pFrame->classIndex)break;
                                }
                                pthread_mutex_unlock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                                if (0 == InitVDHasInit || ui != FsMacrosFunction(state_frameBufferCount)) {
                                    /* 重置 */
                                    FsMacrosSetState_OR_GotoFunctionTag(state, state_reset);
                                } else {
                                    pthread_mutex_lock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                                    pFrame = (FsObjectImageFrame*) pVehicleDetect_item->ro.__framelistIn->pNode[ pVehicleDetect_item->ro.__framelistIn->startIndex + FsMacrosFunction(state_frameBufferCount) - 1];
                                    pthread_mutex_unlock(&pVehicleDetect_item->ro.__framelistIn->mutex);
                                    vehicleDetect_P_item_new_vehicleDetect(pVehicleDetect_item, ui
                                            , pFrame->width[0], pFrame->height[0], pFrame->width[0], pFrame->height[0], &shareBuffer);
                                    pVehicleDetect_item->p.hasInit = 1;
                                }
                            }
                            if (0 == state)break;
                        }
                        /* 执行检测 */
                        if (state & FsMacrosFunction(state_check_do)) {
                            state ^= FsMacrosFunction(state_check_do);
                            //FsMacrosFunctionTag(state_check_do) :;
                            ////////////////////////////////////////////////////////////
                            //********************************************************//
                            /* 车辆检测模块 */
                            if (0 == (pFrame->index % frameInterval) && vehicleDetect_P_item_do_detect(pVehicleDetect_item, pFrame, &objIndex, &baseBuffer, &shareBuffer) != 1) {
                                FsLog(FsLogType_error, FsPrintfIndex, "%u/%lu:do_detect failed.\n", ui, itemList_->nodeCount);
                            }
                            //********************************************************//
                            if (0 == state)break;
                        }
                        /* 输出数据帧 */
                        if (state & FsMacrosFunction(state_out_frame)) {
                            state ^= FsMacrosFunction(state_out_frame);
                            //FsMacrosFunctionTag(state_out_frame) :;
                            ////////////////////////////////////////////////////////////
#ifndef __vehicleDetect_P_T_outFrame 
                            pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pVehicleDetect_item->ro.__framelistIn);
                            pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                            vehicleDetect_P_updateframeRate_frameInterval(pVehicleDetect_item, pFrame->frameRate);
                            fs_ObjectList_insert_tail_pthreadSafety(pVehicleDetect_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pVehicleDetect_item->rw._pGroupSqrtOut, &pVehicleDetect_item->ro._pVehicleDetect->ro._pGroupSqrtOut->groupSqrt_mutex);
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
                            unsigned int rv = pVehicleDetect_item->ro.__framelistIn->nodeCount;
                            if (rv > FsMacrosFunction(state_frameBufferCount))rv = FsMacrosFunction(state_frameBufferCount);
                            while (rv-- > 0) {
#ifndef __vehicleDetect_P_T_outFrame 
                                pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pVehicleDetect_item->ro.__framelistIn);
                                pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                                vehicleDetect_P_updateframeRate_frameInterval(pVehicleDetect_item, pFrame->frameRate);
                                fs_ObjectList_insert_tail_pthreadSafety(pVehicleDetect_item->ro.__framelistOut, pFrame);
                                Fs_GroupSqrtOut_value_set(pVehicleDetect_item->rw._pGroupSqrtOut, &pVehicleDetect_item->ro._pVehicleDetect->ro._pGroupSqrtOut->groupSqrt_mutex);
#endif
                            }
                            if (1 == pVehicleDetect_item->p.hasInit) {
                                pVehicleDetect_item->p.hasInit = 0;
                                if (InitVDHasInit != 1) {
                                    FsPrintf(FsPrintfIndex, "Invalid InitVDHasInit(=%hhu),%u/%lu.\n", InitVDHasInit, ui, itemList_->nodeCount);
                                    fflush(stdout);
                                }
                            }

                            if (FsMacrosFunction(state_check_end) == state) FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                            if (0 == state)break;
                        }
                    }
#endif
                }
                //////////////////////// 状态机 结束 ////////////////////////////////
                ////////////////////////////////////////////////////////////////////          
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    if (pVehicleDetect->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pVehicleDetect->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    if (1 == InitVDHasInit) {
        FsPrintf2(FsPrintfIndex, "Run UnInitVD start.\n");
        UnInitVD();
        FsPrintf2(FsPrintfIndex, "Run UnInitVD end.\n");
    }
    /* 删除各通道 */
    {
        struct VehicleDetect_item ** ppVehicleDetect_itemNode = (struct VehicleDetect_item **) itemList_->pNode + itemList_->startIndex;
        unsigned int ui = itemList_->nodeCount;
        for (; ui > 0; ui--) {
            struct VehicleDetect_item * const pVehicleDetect_item = *ppVehicleDetect_itemNode++;
            if (pVehicleDetect_item)vehicleDetect_P_item_delete__OI(pVehicleDetect_item, &shareBuffer);
        }
        fs_ObjectList_clear(itemList_);
        //        /* 删除集群 */
        //        if (pCapture->p.clusterItemList) {
        //            fs_ObjectList_delete_custom__OI(pCapture->p.clusterItemList, (void (*)(void*))capture_P_item_delete__OI);
        //            pCapture->p.clusterItemList = NULL;
        //        }
    }
    /* 释放全局变量空间 */
    /* 释放中间变量空间 */
    /* 释放引用变量空间 */

    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", VehicleDetect_get_threadName(pVehicleDetect), pVehicleDetect->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pVehicleDetect->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void vehicleDetect_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "vehicleDetectConfig", "车辆检测配置", "车辆检测配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "32");
    fs_Config_node_add_property_area(pConfig, parent, "检测区", "vehicleDetectErea", "area", NULL, NULL, NULL, 0x000000FF);
    fs_Config_node_add_property_image(pConfig, parent, 1, "uuid", "recordConfig rtspServerURL");
    //    /* 车辆检测阈值  */
    //    void* pNode = fs_Config_node_integer_add(pConfig, parent, "carThreshold", "车辆检测阈值", "车辆检测阈值",  FsConfig_nodeShowType_default,0, 0x7, 1, 32767, 1);
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 20, "20", "20");
    // 创建检测区
    {
        void* const pNode = fs_Config_node_node_add(pConfig, parent, "vehicleDetectErea", "目标检测区", "目标检测区", 0, 0x7);
        fs_Config_node_string_add(pConfig, pNode, "area", "区域", "区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 1);
    }
}

/* 检查pVehicleDetect模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int vehicleDetect_check_channel_changed(struct VehicleDetect * const pVehicleDetect, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server VehicleDetect
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
 * 创建一个新的VehicleDetect对象;
 * 返回VehicleDetect指针.
 */
struct VehicleDetect * vehicleDetect_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(threadName) + 1;
    struct VehicleDetect * const rst = (struct VehicleDetect*) fsMalloc(Memery_Alignment(sizeof (struct VehicleDetect)) + len);
    memset(rst, 0, sizeof (struct VehicleDetect));
    char *pd = ((char*) (rst)) + Memery_Alignment(sizeof (struct VehicleDetect));
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

/* 删除pVehicleDetect指向的实例对象 */
void vehicleDetect_delete__OI(struct VehicleDetect * const pVehicleDetect) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pVehicleDetect->ro.__itemList_);

    fsFree(pVehicleDetect);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void vehicleDetect_startThread(struct VehicleDetect * const pVehicleDetect,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (0 == pVehicleDetect->rw.runStatus && 0 == pVehicleDetect->ro.stid) {
        pVehicleDetect->rw.runStatus = 1;
#ifdef FsDebug
        VehicleDetectPrintf2("Init Parameter");
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
        if (pthread_create(&pVehicleDetect->ro.stid, &attr, (void* (*)(void*))vehicleDetect_P_T, pVehicleDetect)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {
        VehicleDetectPrintf2("The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter", pVehicleDetect->rw.runStatus, pVehicleDetect->ro.stid);
    }
}

/* 结束线程 */
void vehicleDetect_stopThread(struct VehicleDetect * const pVehicleDetect) {
    pVehicleDetect->rw.runStatus = 0;
    if (pVehicleDetect->ro.stid) {
#ifdef FsDebug
        FsPrintf2(FsPrintfIndex, "Stop Start.\n");
#endif
        pthread_join(pVehicleDetect->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pVehicleDetect->ro.stid);
        pVehicleDetect->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(FsPrintfIndex, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void vehicleDetect_item_frame_in_pthreadSafety__OI_4(struct VehicleDetect * const pVehicleDetect, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server VehicleDetect
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
unsigned int vehicleDetect_item_frame_out_pthreadSafety__IO_4(struct VehicleDetect * const pVehicleDetect, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server VehicleDetect
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
unsigned int vehicleDetect_item_get_frame_count_pthreadSafety(struct VehicleDetect * const pVehicleDetect, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server VehicleDetect
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
void vehicleDetect_item_reset_groupSqrtOut(struct VehicleDetect * const pVehicleDetect, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server VehicleDetect
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
