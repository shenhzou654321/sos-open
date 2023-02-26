#include "ptzObject.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Image/ImageExtern.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/camera_ctrl.h"
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
#if 1
static unsigned int moduleLoglevel = FsNumberMax(loglevel); // 本模块使用自定义的loglevel,最大值表示未加到日志级别管理模块中
#define loglevel moduleLoglevel
#endif
/* 开方分组用的互斥锁 */
static pthread_mutex_t groupSqrt_mutex = PTHREAD_MUTEX_INITIALIZER;

struct PtzObject_private_TargetPos_item {
    /* 报警目标点坐标 */
    unsigned int target_x;
    unsigned int target_y;
    /* 每帧全景图宽高 */
    unsigned int frameWidth;
    unsigned int frameHeight;
};

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

//static void inflrayObject_P_item_track_delete__OI(struct InflrayObject_P_item_track * const pInflrayObject_P_item_track) {
//    objectTrack_pos_item_delete__OI((struct ObjectTrack_pos_item *) pInflrayObject_P_item_track);
//}

/* 从ObjectTrack_pos_item对象中获取id的方法 */
//static int inflrayObject_P_item_track_get_id(struct InflrayObject_P_item_track * pInflrayObject_P_item_track) {
//    return pInflrayObject_P_item_track->id;
//}

/* 使用pObjectTrack_pos_data初始化pObjectTrack_pos_node中除ObjectTrack_pos_node结构中定义的数据,可为空 */
//static void inflrayObject_P_pos_node_init_extern(struct InflrayObject_P_pos_node* pInflrayObject_P_pos_node, struct DetectAlgorithmLib_ConventionalDetect_pos_data * pDetectAlgorithmLib_ConventionalDetect_pos_data) {
//    memcpy(&pInflrayObject_P_pos_node->prob, &pDetectAlgorithmLib_ConventionalDetect_pos_data->prob, FsPointer_to_integer((&((struct InflrayObject_P_pos_node*) 0)->isDrawing_tracks + 1)) - FsPointer_to_integer(&((struct InflrayObject_P_pos_node*) 0)->prob));
//}

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int ptzObject_P_item_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */struct PtzObject_item * const pPtzObject_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsStructList * const clientList = pPtzObject_item->ro.__clientList;
    pthread_mutex_lock(&clientList->mutex);
    configManager_conncet_refer_check(pPtzObject_item->ro._pPtzObject->ro._pConfigManager, clientList);
    pthread_mutex_unlock(&clientList->mutex);
    return 1;
}

/* 报警联动可见光球机 */
void ptz_item_linkPtz_pthreadSafety(struct PtzObject_item * const pPtzObject_item, /* 报警点坐标 */unsigned int target_x, unsigned int target_y, /* 每帧全景图宽高 */unsigned frameWidth,
        unsigned frameHeight) {
    FsPrintf(1, "ptz_item_linkPtz_pthreadSafety\n");
    struct PtzObject_private_TargetPos_item *pTargetPos_item = (struct PtzObject_private_TargetPos_item *) fsMalloc(sizeof ( struct PtzObject_private_TargetPos_item));
    pTargetPos_item->target_x = target_x;
    pTargetPos_item->target_y = target_y;
    pTargetPos_item->frameWidth = frameWidth;
    pTargetPos_item->frameHeight = frameHeight;

    fs_ObjectList_insert_tail_pthreadSafety(pPtzObject_item->ro.__linkPtzList, pTargetPos_item);
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int ptzObject_P_item_cb_debug(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct PtzObject_item * const pPtzObject_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int iTime = tv.tv_sec - pPtzObject_item->p.curLinkTime > pPtzObject_item->p.linkIntervalSeconds;
    FsPrintf(1, "next link time = %d\n", iTime);
    if (tv.tv_sec - pPtzObject_item->p.curLinkTime > pPtzObject_item->p.linkIntervalSeconds) {
        const int target_x = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "target_x", 0);
        const int target_y = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "target_y", 0);
        const int frameWidth = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "frame_width", 0);
        const int frameHeight = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "frame_height", 0);

        if ((target_x < 0) || (target_y < 0) || (frameWidth < 0) || (frameHeight < 0)) {
            FsLog(FsLogType_error, FsPrintfIndex, "Invalid value.\n");
            FsLogTag(10);
            fflush(stdout);
            //configManager_conncet_refer_send_buffer(FsStringLenData("Invalid value."), requestID_3, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Invalid value.\n") - 1) + sizeof (unsigned int)*3);
            fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Invalid value.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
            sendData->len = 8 + sizeof ("Invalid value.\n") - 1;
            *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
            memcpy(sendData->data + 8, "Invalid value.\n", sizeof ("Invalid value.\n") - 1);
            memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
            configManager_send_pthreadSafety__OI_2(pPtzObject_item->ro._pPtzObject->ro._pConfigManager, sendData);
            return -2;
        } else {
            ptz_item_linkPtz_pthreadSafety(pPtzObject_item, target_x, target_y, frameWidth, frameHeight);
            configManager_conncet_refer_send_buffer(FsStringLenData("ok"), requestID_3, headType, checkMethod, virtualConnection, (head & 0xFFFFFFF0U) | 0x2U, pObjectBaseBuffer);
            return 1;
        }
    } else {
        FsLog(FsLogType_error, FsPrintfIndex, "Operate too frequently.\n");
        FsLogTag(10);
        fflush(stdout);
        FsObjectBase * const sendData = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Operate too frequently.\n") - 1) + sizeof (unsigned int)*3);
        fs_ObjectBase_init(sendData, sizeof (FsObjectBase) + 8 + Memery_Alignment(sizeof ("Operate too frequently.\n") - 1) + sizeof (unsigned int)*3, sizeof (FsObjectBase));
        sendData->len = 8 + sizeof ("Operate too frequently.\n") - 1;
        *((unsigned int *) sendData->data) = (head & 0xFFFFFFF0U) | 0x6U, *((unsigned int *) (sendData->data + 4)) = sendData->len - 8;
        memcpy(sendData->data + 8, "Operate too frequently.\n", sizeof ("Operate too frequently.\n") - 1);
        memcpy(sendData->data + Memery_Alignment(sendData->len), requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pPtzObject_item->ro._pPtzObject->ro._pConfigManager, sendData);
        return -2;
    }
}

/* 在期望连接的命令字和uuid改变时调用 */
static void ptzObject_P_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p) {
    struct PtzObject_item* pPtzObject_item = (struct PtzObject_item *) p;
    if (strcmp("cameractrl", cmd) == 0 && strcmp(uuid, pPtzObject_item->ro._uuid) == 0) {
        pthread_mutex_lock(&pPtzObject_item->ro.__mutexCmdConnect);
        pPtzObject_item->p.pCapture_item = (struct Capture_item*) do_local;
        pthread_mutex_unlock(&pPtzObject_item->ro.__mutexCmdConnect);
    } else {
        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pPtzObject_item->ro._uuid);
        fflush(stdout);
    }
}

static inline int ptzObject_P_item_do_link(/* 联动项,保证有一个联动任务可用 */struct PtzObject_item * pPtzObject_item, FsObjectBaseBuffer * const pObjectBaseBuffer
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //    FsObjectList *linkPtzList = pPtzObject_item->ro.__linkPtzList;
    //    unsigned int count = linkPtzList->nodeCount;
    //    unsigned int ui = 0;
    //    for (; ui < count; ui++) {
    //        FsPrintf(1, "pPtzObject_item->ro.__linkPtzList count=%d\n", linkPtzList->nodeCount);
    //        fs_ObjectList_remove_head(linkPtzList);
    //    }
    //    return 1;

    if (pPtzObject_item->p.pCapture_item) {
        FsObjectList *linkPtzList = pPtzObject_item->ro.__linkPtzList;
        unsigned int count = linkPtzList->nodeCount;
        unsigned int ui = 0;
        double device_field_angle = (double) pPtzObject_item->p.device_field_angle / 10.0;
        struct PtzObject_private_TargetPos_item *pTargetPos_item;
        // 水平偏移角计算
        double horizontal_offset_degree = (double) pPtzObject_item->p.ptz_horizontal_offset / 10.0;
        // 判断俯仰方向 1:向上正方向 0 向下正方向
        unsigned char positive = 1;
        if (pPtzObject_item->p.ptz_up_range < pPtzObject_item->p.ptz_down_range)
            positive = 0;
        // 俯仰偏移角计算
        double vertical_offset_degerr = (double) pPtzObject_item->p.ptz_vertical_offset / 10.0;
        // 俯仰角范围
        unsigned short vertical_range = abs(pPtzObject_item->p.ptz_up_range) + abs(pPtzObject_item->p.ptz_down_range);

        FsPrintf(1, "pPtzObject_item.p device_field_angle=%lu ptz_zoom=%lu ptz_up_range=%d ptz_down_range=%d ptz_horizontal_offset=%d ptz_vertical_offset=%d\n",
                pPtzObject_item->p.device_field_angle, pPtzObject_item->p.ptz_zoom, pPtzObject_item->p.ptz_up_range, pPtzObject_item->p.ptz_down_range, pPtzObject_item->p.ptz_horizontal_offset,
                pPtzObject_item->p.ptz_vertical_offset);

        // 可见光转动
        const unsigned long long mask = pPtzObject_item->p.pCapture_item->ro._ctrl_function->mask_get_pthreadSafety(pPtzObject_item->p.pCapture_item->ro.__pCamera_item, pPtzObject_item->p.pCapture_item->ro.__pCamera_ctrl_item,
                NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, pObjectBaseBuffer, pShareBuffer);
        FsPrintf(1, "ptz_goto_pthreadSafety mask=%d\n", mask);

        for (; ui < count; ui++) {
            FsPrintf(1, "pPtzObject_item->ro.__linkPtzList count=%d\n", linkPtzList->nodeCount);

            pTargetPos_item = (struct PtzObject_private_TargetPos_item*) linkPtzList->pNode[linkPtzList->startIndex];

            FsPrintf(1, "pTargetPos_item pTargetPos_item->target_x=%lu pTargetPos_item->target_y=%lu pTargetPos_item->frameWidth=%lu pTargetPos_item->frameHeight=%lu\n", pTargetPos_item->target_x, pTargetPos_item->target_y, pTargetPos_item->frameWidth, pTargetPos_item->frameHeight, pPtzObject_item->p.ptz_zoom);

            // 计算目标方位俯仰角度
            double targetDegree_x = (double) pTargetPos_item->target_x / (double) pTargetPos_item->frameWidth * 360.0;
            double targetDegree_y = ((double) pTargetPos_item->target_y / (double) pTargetPos_item->frameHeight * device_field_angle - device_field_angle / 2.0) * -1.0;
            double pos_x = horizontal_offset_degree + targetDegree_x;
            unsigned short P;
            if (pos_x > 360) {
                pos_x -= 360.0;
            }
            if (pos_x < 180) {
                P = pos_x / 360.0 * 23040 + 11520;
            } else {
                P = pos_x / 360.0 * 23040 - 11520;
            }
            double pos_y = vertical_offset_degerr + targetDegree_y;
            unsigned short T;
            if (positive)
                T = 23040.0 / (double) vertical_range * ((double) abs(pPtzObject_item->p.ptz_up_range) - pos_y);
            else
                T = 23040.0 / (double) vertical_range * (pos_y + (double) abs(pPtzObject_item->p.ptz_down_range));

            if (mask & (0x1LLU << Camera_ctrl_Mask_PTZ_get_set)) {
                struct timeval tv;
                gettimeofday(&tv, NULL);
                if (tv.tv_sec - pPtzObject_item->p.curLinkTime > pPtzObject_item->p.linkIntervalSeconds) {
                    FsPrintf(1, "current time - last time:%ld\n", tv.tv_sec - pPtzObject_item->p.curLinkTime);
                    pPtzObject_item->p.curLinkTime = tv.tv_sec;
                    FsPrintf(1, "current time:%ld\n", tv.tv_sec);
                    // 两次联动间隔大于5s
                    if (pPtzObject_item->p.pCapture_item->ro._ctrl_function->ptz_goto_pthreadSafety(pPtzObject_item->p.pCapture_item->ro.__pCamera_item, pPtzObject_item->p.pCapture_item->ro.__pCamera_ctrl_item,
                            pPtzObject_item, 0, P, T, pPtzObject_item->p.ptz_zoom, pObjectBaseBuffer, pShareBuffer) != 1) {
                        FsPrintf(1, "ptz_goto_pthreadSafety failed\n");
                        if (pPtzObject_item->ro.__linkPtzList->nodeCount > 0)
                            free(fs_ObjectList_remove_head(pPtzObject_item->ro.__linkPtzList));
                        return -1;
                    } else {
                        FsPrintf(1, "ptz_goto_pthreadSafety P=%lu T=%lu Z=%lu\n", P, T, pPtzObject_item->p.ptz_zoom);
                        pPtzObject_item->p.pCapture_item->ro._ctrl_function->cancelKey_pthreadSafety(pPtzObject_item->p.pCapture_item->ro.__pCamera_item, pPtzObject_item->p.pCapture_item->ro.__pCamera_ctrl_item,
                                pPtzObject_item, pObjectBaseBuffer, pShareBuffer);
                    }
                }
            } else {
                FsPrintf(1, "ptz_goto_pthreadSafety mask failed\n");
                if (pPtzObject_item->ro.__linkPtzList->nodeCount > 0)
                    free(fs_ObjectList_remove_head(pPtzObject_item->ro.__linkPtzList));
                return -1;
            }
            if (pPtzObject_item->ro.__linkPtzList->nodeCount > 0)
                free(fs_ObjectList_remove_head(pPtzObject_item->ro.__linkPtzList));
        }
        return 1;
    }

    return -1;
}

static FsConfig *ptzObject_P_protocol_debug() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ptz_link", "ptz_link", "ptz_link");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    //    {
    //        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "priority", "控制优先级", "优先级,0-默认,不缓存调用者信息,1-比分析优先级低的定义,2-255:保留,256-29999:比分析优先级低的定义,\n"
    //                "30000-34999:视频分析专用的优先级,35000-64999:比分析优先级高的定义,65000-65534:保留;非0优先级中,\n"
    //                "    偶数表示可合并的优先级,即如有多个拥有相同偶数优先级的客户端访问同一相机,这些所有客户端的指令会按照新指令覆盖旧指令方式生效;\n"
    //                "    奇数表示独占优先级,即如有多个拥有相同奇数优先级的客户端访问同一相机,只有第一个客户端的指令会生效,其他客户端的指令会被拒绝", FsConfig_nodeShowType_default, 0, 0x7, 0, 65534, 1);
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 256, "256", "256");
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 257, "257", "257");
    //    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "target_x", "报警点坐标X", "报警点坐标X", FsConfig_nodeShowType_default, 0, 0x7, 0, 44800, 0);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "target_y", "报警点坐标Y", "报警点坐标Y", FsConfig_nodeShowType_default, 0, 0x7, 0, 1280, 0);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "frame_width", "全景图宽度", "全景图宽度", FsConfig_nodeShowType_default, 0, 0x7, 0, 44800, 0);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "frame_height", "全景图高度", "全景图高度", FsConfig_nodeShowType_default, 0, 0x7, 0, 1280, 0);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    //    {
    //        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
    //        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
    //        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
    //        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    //    }
    return pConfig;
}

/* 删除pInflrayObject指向的实例对象 */
static void ptzObject_P_item_delete__OI(struct PtzObject_item * const pPtzObject_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pPtzObject_item)return;
    void *const pConfigManager = pPtzObject_item->ro._pPtzObject->ro._pConfigManager;
    // 注销连接断开时的回调通知
    if (0 == pPtzObject_item->ro._ipv4) configManager_connect_error_logoff(pConfigManager, (int (*)(const unsigned int *, void *, char * * const))ptzObject_P_item_cb_connect_error, pPtzObject_item);
    /* 注销命令字 */
    if (0 == pPtzObject_item->ro._ipv4) configManager_cmd_logoff(pConfigManager, "ptz_link", pPtzObject_item->ro._uuid, pPtzObject_item->ro._ipv4, pPtzObject_item, pShareBuffer);
    if (0 == pPtzObject_item->ro._ipv4) configManager_cmd_disconnect(pConfigManager, "cameractrl", pPtzObject_item->ro._uuid, pPtzObject_item);
    if (0 == pPtzObject_item->ro._ipv4) {
        /* 传入的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pPtzObject_item->ro.__framelistIn);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pPtzObject_item->ro.__framelistOut);
        /* 关闭所有与之相关的客户端链表 */
        configManager_conncet_refer_close(pConfigManager, pPtzObject_item->ro.__clientList, 1);
        /* 与之相关的客户端链表 */
        fs_StructList_delete__OI(pPtzObject_item->ro.__clientList);
        /* 联动任务链表 */
        fs_ObjectList_delete_allSequence__OI(pPtzObject_item->ro.__linkPtzList);
    }
    fsFree(pPtzObject_item);
}

static void ptzObject_P_item_new(struct PtzObject * const pPtzObject, /* 通道号,从1开始,为0用于集群 */const unsigned int channel
        , /* 本地ip地址 */const FsArray * const ipList, /* 原来的检测项 */struct PtzObject_item * * const ppPtzObject_item, FsGroupSqrt * const pGroupSqrt
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pPtzObject->ro._pConfigManager)->ro.__pConfig;
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
                        //                        if (ppNodeCapture_item[index] != NULL && ppNodeCapture_item[index]->ro._sum == fs_Config_get_sum(pConfig, (struct FsEbml_node*) sumNode)) {
                        //                            index++;
                        //                            continue;
                        //                        }
                    } else {
                        sumNode = vsysChannel = list;
                        if (*ppPtzObject_item != NULL && (*ppPtzObject_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppPtzObject_item) {
                        ptzObject_P_item_delete__OI(*ppPtzObject_item, pShareBuffer);
                    }
                    struct PtzObject_item * rst;
                    if ((PtzObject_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0 && PtzObject_Mask != 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            rst = (struct PtzObject_item*) fsMalloc(Memery_Alignment(sizeof (struct PtzObject_item)) + len);
                            memset(rst, 0, sizeof (struct PtzObject_item));
                            pd = ((char*) rst) + Memery_Alignment(sizeof (struct PtzObject_item));
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 与之相关的客户端链表 */
                            rst->ro.__clientList = fs_StructList_new__IO(6, sizeof (int)*6);
                            /* 联动任务链表 */
                            rst->ro.__linkPtzList = fs_ObjectList_new__IO(4);
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
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pPtzObject->ro._pGroupSqrtOut->groupSqrt_member, pPtzObject->ro._pGroupSqrtOut->groupSqrt_group
                                    , pPtzObject->ro._pGroupSqrtOut->groupValue, channel - 1, &pPtzObject->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        if (channel > 0) {
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len);
                            /* 所属的目标检测对象 */
                            rst->ro._pPtzObject = pPtzObject;
                        }
                        if (channel > 0) {
                            /* 初始化当前联动时间 */
                            rst->p.curLinkTime = 0;
                            // 获取配置参数
                            const void *ptzObjectConfig0 = vsysChannel0;
                            const void *const ptzObjectConfig = fs_Config_node_get_first(pConfig, &ptzObjectConfig0, vsysChannel, "ptzObjectConfig");
                            if (ptzObjectConfig) {
                                rst->p.linkIntervalSeconds = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "interval_Time", 5, NULL);
                                rst->p.device_field_angle = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "device_field_angle", 64, NULL);
                                rst->p.ptz_zoom = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_zoom", 13040, NULL);
                                rst->p.ptz_horizontal_offset = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_horizontal_offset", 0, NULL);
                                rst->p.ptz_vertical_offset = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_vertical_offset", 0, NULL);
                                rst->p.ptz_up_range = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_up_range", 45, NULL);
                                rst->p.ptz_down_range = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_down_range", -45, NULL);
                                //FsPrintf(1, "item_new device_field_angle=%lu ptz_zoom=%lu ptz_up_range=%d ptz_down_range=%d ptz_horizontal_offset=%lu ptz_vertical_offset=%d\n", rst->p.device_field_angle, rst->p.ptz_zoom, rst->p.ptz_up_range, rst->p.ptz_down_range, rst->p.ptz_horizontal_offset, rst->p.ptz_vertical_offset);
                            }
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        // 注册连接断开时的回调通知
                        if (0 == ipv4) configManager_connect_error_register(pPtzObject->ro._pConfigManager
                                , (int (*)(const unsigned int *, void *, char * * const))ptzObject_P_item_cb_connect_error, rst);
                        /* 注册命令字 */
                        configManager_cmd_register(pPtzObject->ro._pConfigManager, "ptz_link", rst->ro._uuid, ipv4, rst, 0, 0 == ipv4 ? (int (* const) (const unsigned int *, unsigned char, unsigned char, unsigned int, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))ptzObject_P_item_cb_debug : NULL
                                , 0 == ipv4 ? (void*) ptz_item_linkPtz_pthreadSafety : NULL, rst, pShareBuffer);
                        /* 绑定命令字 保存视频录像*/
                        configManager_cmd_connect(pPtzObject->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, ptzObject_P_cmd_connect_cb, rst);
                        //if (0 == ipv4) configManager_cmd_register(pPtzObject->ro._pConfigManager, "ptzObject_debug", rst->ro._uuid, ipv4, rst, 0
                        //        , (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))ptzObject_P_item_cb_debug, NULL, rst, pShareBuffer);
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
                    if (channel > 0) *ppPtzObject_item = rst;
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

//static void ptzObject_P_item_new_conventionalDetect__IO(struct PtzObject_item * const pPtzObject_item, unsigned int index, unsigned short width, unsigned short height
//        , /* 参考标定的宽度 */unsigned int width0, /* 参考标定的高度 */ unsigned int height0
//        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
//    ConfigManager * const pConfigManager = (ConfigManager*) pPtzObject_item->ro._pPtzObject->ro._pConfigManager;
//    configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
//    FsConfig * const pConfig = pConfigManager->ro.__pConfig;
//    const void *ptzObjectConfig0 = pConfig;
//    const void *ptzObjectConfig;
//    {
//        ptzObjectConfig = fs_Config_node_template_get_orderFirst(pConfig, &ptzObjectConfig0, pConfig, pConfigManager->ro.__ipList_local, 0, "vsys", 0);
//        if (ptzObjectConfig) {
//            ptzObjectConfig = fs_Config_node_template_get(pConfig, &ptzObjectConfig0, ptzObjectConfig, NULL, 0, "vsysChannel", index);
//            if (ptzObjectConfig) {
//                ptzObjectConfig = fs_Config_node_get_first(pConfig, &ptzObjectConfig0, ptzObjectConfig, "ptzObjectConfig");
//            }
//        }
//    }
//    
//    if(ptzObjectConfig)
//    {
//        pPtzObject_item->p.device_field_angle = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "device_field_angle", 64, NULL);
//        pPtzObject_item->p.ptz_zoom = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_zoom", 13040, NULL);
//        pPtzObject_item->p.ptz_horizontal_offset = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_horizontal_offset", 0, NULL);
//        pPtzObject_item->p.ptz_vertical_offset = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_vertical_offset", 0, NULL);
//        pPtzObject_item->p.ptz_up_range = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_up_range", 45, NULL);
//        pPtzObject_item->p.ptz_down_range = fs_Config_node_integer_get_first(pConfig, ptzObjectConfig0, ptzObjectConfig, "ptz_down_range", -45, NULL);
//    }
//    
//    configManager_config_deleteRefer_pthreadSafety(pConfigManager);
//    /* 检测区域 */
//    // pVehicleDetect_item->p.x1 = x1, pVehicleDetect_item->p.y1 = y1, pVehicleDetect_item->p.x2 = x2, pVehicleDetect_item->p.y2 = y2;
//}

/* 从配置获取通道数 */
static unsigned int ptzObject_P_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

static void *ptzObject_P_T(struct PtzObject * const pPtzObject) {
#undef FsFunctionName
#define FsFunctionName ptzObject_P_T
    /* 创建线程监控 */
    void *pMonitor = pPtzObject->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, PtzObject_get_threadName(pPtzObject), NULL, pPtzObject, pPtzObject->ro._pMonitor != NULL ? pPtzObject->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pPtzObject->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pPtzObject->ro._pConfigManager;
    FsObjectList * const itemList_ = pPtzObject->ro.__itemList_;
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
#define __ptzObject_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __ptzObject_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __ptzObject_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);  
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    unsigned int sleepTime = 0;
    int frameBufferCount = 1;
    /* 目标编号 */
    unsigned int objIndex = 0;
    /* 开方分组 */
    FsGroupSqrt groupSqrt;
    Fs_GroupSqrt_init(&groupSqrt);
#define __ptzObject_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __ptzObject_P_T_clean1_count 4
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    configManager_protocol_publish(pConfigManager, "ptz_link", "ptz联动调试", 1, ptzObject_P_protocol_debug);
#define __ptzObject_P_T_clean2_1 {configManager_protocol_cancel(pConfigManager, "ptz_link");}
#define __ptzObject_P_T_clean2_count 1
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pPtzObject->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        usleep(1000);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pPtzObject->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (0 == pPtzObject->rw._resetChannel) channelCount = ptzObject_P_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server PtzObject
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
                struct PtzObject_item * const pPtzObject_item = (struct PtzObject_item *) fs_ObjectList_remove_tail(itemList_);
                if (pPtzObject_item)ptzObject_P_item_delete__OI(pPtzObject_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pPtzObject->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pPtzObject->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pPtzObject->rw._resetChannel > 0) {
                FsPrintf(1, "%d\n", pPtzObject->rw._resetChannel);
                pMonitorItem->rw.line = __LINE__;
                ptzObject_P_item_new(pPtzObject, pPtzObject->rw._resetChannel, pConfigManager->ro.__ipList_local
                        , (struct PtzObject_item **) (itemList_->pNode + itemList_->startIndex + pPtzObject->rw._resetChannel - 1), &groupSqrt
                        , & shareBuffer);
                pMonitorItem->rw.line = __LINE__;
            }
            pPtzObject->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct PtzObject_item * * const ppNode_itemList = (struct PtzObject_item **) itemList_->pNode + itemList_->startIndex;
            //FsPrintf(1, "channelCount=%u/%lu,%p,%hhu,groupSqrt_group=%u,groupSqrt_member=%u\n", channelCount,itemList_->nodeCount,groupSqrt.groupValue,groupSqrt.groupValue[0],groupSqrt.groupSqrt_group,groupSqrt.groupSqrt_member);
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
                struct PtzObject_item * const pPtzObject_item = ppNode_itemList[ui];
                if (NULL == pPtzObject_item)continue;
                //FsPrintf(1, "pPtzObject_item->ro.__linkPtzList->nodeCount=%lu\n", pPtzObject_item->ro.__linkPtzList->nodeCount);
                ////////////////////////////////////////////////////////////////
                //////////////////////// 状态机 开始 ////////////////////////////

                //                if (pPtzObject_item->ro.__linkPtzList->nodeCount > 0) {
                //                    //FsPrintf(1, "pPtzObject_item->ro.__linkPtzList->nodeCount=%lu\n", pPtzObject_item->ro.__linkPtzList->nodeCount);
                //                    // 执行联动
                //                    pthread_mutex_lock(&pPtzObject_item->ro.__linkPtzList->mutex);
                //                    ptzObject_P_item_do_link(pPtzObject_item, &baseBuffer, &shareBuffer);
                //                    pthread_mutex_unlock(&pPtzObject_item->ro.__linkPtzList->mutex);
                //                }

#define __ptzObject_P_T_state_check_end (1U<<0) // 检查缓存是否足够,不够退出循环
#define __ptzObject_P_T_state_check_module_init (1U<<1) // 检查模块初始化
#define __ptzObject_P_T_state_check_do (1U<<2) // 执行联动
#define __ptzObject_P_T_state_out_frame (1U<<3) // 输出数据帧
#define __ptzObject_P_T_state_end_break (1U<<4) // 退出循环
#define __ptzObject_P_T_state_reset (1U<<5) // 重置
#ifndef __ptzObject_P_T_state
                unsigned int state;
                //state = FsMacrosFunction(state_check_end);
                FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                for (;;) {
                    //void *pConventionalDetect;
                    FsObjectImageFrame * pFrame;
                    /* 检查缓存是否足够 */
                    if (state & FsMacrosFunction(state_check_end)) {
                        state ^= FsMacrosFunction(state_check_end);
                        FsMacrosFunctionTag(state_check_end) :;
                        //FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end\n");
                        ////////////////////////////////////////////////
                        if (((int) pPtzObject_item->ro.__framelistIn->nodeCount) >= frameBufferCount) {
                            /* 检查数据 */
                            //FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end\n");
                            pthread_mutex_lock(&pPtzObject_item->ro.__framelistIn->mutex);
                            pFrame = (FsObjectImageFrame*) pPtzObject_item->ro.__framelistIn->pNode[ pPtzObject_item->ro.__framelistIn->startIndex + frameBufferCount - 1];
                            pthread_mutex_unlock(&pPtzObject_item->ro.__framelistIn->mutex);
                            // FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end,classIndex=%u/%u,index=%u\n", pInflrayObject_item->ro.classIndex, pFrame->classIndex, pFrame->index);
                            if (pPtzObject_item->ro.classIndex != pFrame->classIndex) {
                                pPtzObject_item->ro.classIndex = pFrame->classIndex;
                                /* 重置 */
                                FsMacrosSetStates_OR_GotoFunctionTag(state, state_reset, FsMacrosFunction(state_check_end));
                            } else {
                                /* 检查模块有无初始化 */
                                // FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end,index=%u\n", pFrame->index);
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
                        //                        if (0 == pPtzObject_item.p.hasInit) {                            
                        pthread_mutex_lock(&pPtzObject_item->ro.__framelistIn->mutex);
                        int i = 0;
                        for (; i < frameBufferCount; i++) {
                            pFrame = (FsObjectImageFrame*) pPtzObject_item->ro.__framelistIn->pNode[ pPtzObject_item->ro.__framelistIn->startIndex + i];
                            if (pPtzObject_item->ro.classIndex != pFrame->classIndex)break;
                        }
                        pthread_mutex_unlock(&pPtzObject_item->ro.__framelistIn->mutex);
                        if (i != frameBufferCount) {
                            /* 重置 */
                            FsMacrosSetState_OR_GotoFunctionTag(state, state_reset);
                        } else {
                            pthread_mutex_lock(&pPtzObject_item->ro.__framelistIn->mutex);
                            pFrame = (FsObjectImageFrame*) pPtzObject_item->ro.__framelistIn->pNode[ pPtzObject_item->ro.__framelistIn->startIndex + frameBufferCount - 1];
                            pthread_mutex_unlock(&pPtzObject_item->ro.__framelistIn->mutex);
                            //FsPrintf(1, "TTTTTTTTTTTTTTTptzObject_P_item,index=%u\n", pFrame->index);
                        }
                        if (0 == state)break;
                    }
                    /* 执行联动 */
                    if (state & FsMacrosFunction(state_check_do)) {
                        state ^= FsMacrosFunction(state_check_do);
                        //FsMacrosFunctionTag(state_check_do) :;
                        ////////////////////////////////////////////////////////////
                        if (pPtzObject_item->ro.__linkPtzList->nodeCount > 0) {
                            FsPrintf(1, "pPtzObject_item->ro.__linkPtzList->nodeCount=%lu\n", pPtzObject_item->ro.__linkPtzList->nodeCount);
                            // 执行联动
                            pthread_mutex_lock(&pPtzObject_item->ro.__linkPtzList->mutex);
                            ptzObject_P_item_do_link(pPtzObject_item, &baseBuffer, &shareBuffer);
                            pthread_mutex_unlock(&pPtzObject_item->ro.__linkPtzList->mutex);
                        }
                        //                        /* 统计检测帧间隔 */
                        //                        int frameInterval = 1;
                        //                        //********************************************************//
                        //                        /* 检测模块 */
                        //                        // if (frameIterval < pCarDetect->p._frameInterval)frameIterval = pCarDetect->p._frameInterval;
                        //                        if (frameBufferCount < frameInterval) {
                        //                            frameBufferCount = frameInterval;
                        //                            FsMacrosSetStates_OR_GotoFunctionTag(state, state_reset, FsMacrosFunction(state_check_end));
                        //                        } else {
                        //                            if (0 == (pFrame->index % frameInterval) && ptzObject_P_item_do_detect(pPtzObject_item, pFrame, &objIndex, &baseBuffer, &shareBuffer) != 1) {
                        //                                FsLog(FsLogType_error, FsPrintfIndex, "%u/%lu:do_detect failed.\n", ui, itemList_->nodeCount);
                        //                            }
                        //                            //                            if (0 == (pFrame->index % frameInterval)) {
                        //                            //                                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
                        //                            //                                if (pYUV420P != NULL) {
                        //                            //                                  //  FsPrintf(1, "TTTTTTTTTTTTTTT,index=%u\n", pFrame->index);
                        //                            //                                    detectAlgorithmLib_ConventionalDetect_detect(pConventionalDetect, pYUV420P->data, pFrame->width[0]);
                        //                            //                                    fs_Object_delete_pthreadSafety__OI(pYUV420P);
                        //                            //                                    /* 发送调试数据到客户端 */
                        //                            //                                    // targetCheck_P_item_send_carDetect_debug(pTargetCheck_item, pFrame, &baseBuffer);
                        //                            //                                } else {
                        //                            //                                    FsLog(FsLogType_error, FsPrintfIndex, "%u/%lu:YUV420P (=%p) is NULL.\n", ui, itemList_->nodeCount, pYUV420P);
                        //                            //                                    fflush(stdout);
                        //                            //                                }
                        //                            //                            }
                        //                        }
                        //********************************************************//
                        if (0 == state)break;
                    }
                    /* 输出数据帧 */
                    if (state & FsMacrosFunction(state_out_frame)) {
                        state ^= FsMacrosFunction(state_out_frame);
                        //FsMacrosFunctionTag(state_out_frame) :;
                        ////////////////////////////////////////////////////////////
#ifndef __ptzObject_P_T_outFrame 
                        FsObjectImageFrame * const pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pPtzObject_item->ro.__framelistIn);
                        pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                        fs_ObjectList_insert_tail_pthreadSafety(pPtzObject_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pPtzObject_item->rw._pGroupSqrtOut, &pPtzObject_item->ro._pPtzObject->ro._pGroupSqrtOut->groupSqrt_mutex);
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
                        int rv = pPtzObject_item->ro.__framelistIn->nodeCount;
                        if (rv > frameBufferCount)rv = frameBufferCount;
                        while (rv-- > 0) {
#ifndef __ptzObject_P_T_outFrame 
                            FsObjectImageFrame * const pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pPtzObject_item->ro.__framelistIn);
                            pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                            fs_ObjectList_insert_tail_pthreadSafety(pPtzObject_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pPtzObject_item->rw._pGroupSqrtOut, &pPtzObject_item->ro._pPtzObject->ro._pGroupSqrtOut->groupSqrt_mutex);
#endif
                        }
                        if (FsMacrosFunction(state_check_end) == state) FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                        if (0 == state)break;
                    }
                }
#endif
                //////////////////////// 状态机 结束 ///////////////////////////
                ////////////////////////////////////////////////////////////////         
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    if (pPtzObject->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pPtzObject->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    /* 删除各通道 */
    {
        unsigned int ui = itemList_->nodeCount;
        struct PtzObject_item ** ppPtzObject_itemNode = (struct PtzObject_item **) itemList_->pNode + itemList_->startIndex;
        for (; ui > 0; ui--) {
            struct PtzObject_item * const pPtzObject_item = *ppPtzObject_itemNode++;
            if (pPtzObject_item)ptzObject_P_item_delete__OI(pPtzObject_item, &shareBuffer);
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
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", PtzObject_get_threadName(pPtzObject), pPtzObject->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pPtzObject->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void ptzObject_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "ptzObjectConfig", "Inflray球机联动配置", "Inflray球机联动配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "128");
    //    {
    //        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "linkageEnable", "目标报警联动", "目标报警联动", 0, 0x7, 0, 1, 1);
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "关闭", "关闭");
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "开启", "开启");
    //    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "interval_Time", "联动间隔时间(s)", "联动间隔时间(s)", FsConfig_nodeShowType_default, 0, 0x7, 0, 60, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 5, "5", "5");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0", "0");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "device_field_angle", "设备垂直视场角", "设备垂直视场角", FsConfig_nodeShowType_default, 0, 0x7, 64, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 64, "非制冷640", "非制冷640");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 96, "非制冷1280横放", "非制冷1280横放");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 128, "非制冷1280竖放", "非制冷1280竖放");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "ptz_zoom", "可见光变倍", "可见光变倍", FsConfig_nodeShowType_default, 0, 0x7, 0, 23040, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 7500, "7500", "7500");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 13040, "13040", "13040");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 23040, "23040", "23040");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "ptz_horizontal_offset", "方位偏移量", "方位偏移量,对应[0,360]度，一位小数", FsConfig_nodeShowType_default, 0, 0x7, 0, 3600, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "ptz_vertical_offset", "俯仰偏移量", "俯仰偏移量，对应[-90,90]度，一位小数", FsConfig_nodeShowType_default, 0, 0x7, -900, 900, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "ptz_up_range", "可见光向上俯仰角最大值", "可见光向上俯仰角最大值", FsConfig_nodeShowType_default, 0, 0x7, -90, 90, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 45, "45", "45");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "ptz_down_range", "可见光向下俯仰角最大值", "可见光向下俯仰角最大值", FsConfig_nodeShowType_default, 0, 0x7, -90, 90, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, -45, "-45", "-45");
    }
    //    {
    //        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "fakeAlarmEnabel", "模拟报警", "模拟报警", 0, 0x7, 0, 1, 1);
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "关闭", "关闭");
    //        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "开启", "开启");
    //    }
    //    {
    //        void* const pNode = fs_Config_node_string_add(pConfig, parent, "fakeAlarmPoint", "模拟报警点", "模拟报警点", 0, 0x7, 1, 1024, 1);
    //    }
}

/* 检查pinflrayObject模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int ptzObject_check_channel_changed(struct PtzObject * const pPtzObject, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server PtzObject
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
 * 创建一个新的inflrayObject对象;
 * 返回inflrayObject指针.
 */
struct PtzObject * ptzObject_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#ifdef loglevel
    if (loglevel == FsNumberMax(loglevel)) fs_log_addLevel(&loglevel, "PtzObject", pShareBuffer);
#endif
    unsigned int len = strlen(threadName) + 1;
    struct PtzObject * const rst = (struct PtzObject*) fsMalloc(sizeof (struct PtzObject) +len);
    memset(rst, 0, sizeof (struct PtzObject));
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

/* 删除pinflrayObject指向的实例对象 */
void ptzObject_delete__OI(struct PtzObject * const pPtzObject) {

    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pPtzObject->ro.__itemList_);

    fsFree(pPtzObject);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void ptzObject_startThread(struct PtzObject * const pPtzObject
        , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    pPtzObject->rw.runStatus = 1;
#ifdef FsDebug
    PtzObjectLog2(FsLogType_threadDebug, "Init Parameter");
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
    if (pthread_create(&pPtzObject->p.stid, &attr, (void* (*)(void*))ptzObject_P_T, pPtzObject)) {

        FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
        fflush(stdout);
        FsLogExit();
    }
    pthread_attr_destroy(&attr);
}

/* 结束线程 */
void ptzObject_stopThread(struct PtzObject * const pPtzObject) {
#define __stopThread_var pPtzObject
    //#define __stopThread_noWait
#define __stopThread_noPipe
#define __stopThread_noinRun
#ifndef __stopThread 
    (__stopThread_var)->rw.runStatus = 0;
    if ((__stopThread_var)->p.stid) {
#ifndef __stopThread_noPipe
        for (;;) {
#define  __control_pipe_write_var (__stopThread_var)->ro.__pipe[1]
#ifndef __control_pipe_write 
            if (write(__control_pipe_write_var, "w", 1) != 1) {

                FsLog(FsLogType_error, FsPrintfIndex, "Write pipe(=%d) error[%d/%d]:\"%s\".\n", __control_pipe_write_var, errno, WSAGetLastError(), strerror(errno));
                fflush(stdout);
            } else break;
#undef __control_pipe_write_var
#endif
        }
#endif
#ifndef __stopThread_noWait
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop Start.\n");
#endif
        pthread_join((__stopThread_var)->p.stid, 0);
        thread_join_remove_check_pthreadSafety((__stopThread_var)->p.stid);
        (__stopThread_var)->p.stid = 0;
#ifndef __stopThread_noinRun
#ifdef FsDebug
        if ((__stopThread_var)->p.inRun != 2) {

            FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid inRun(=%hhu).\n", (__stopThread_var)->p.inRun);
            fflush(stdout);
            FsLogExit();
        }
#endif
        (__stopThread_var)->p.inRun = 0;
#endif
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop successfully.\n");
#endif
#endif
    }
#undef __stopThread_var
#ifdef __stopThread_noWait
#undef __stopThread_noWait
#endif
#ifdef __stopThread_noPipe
#undef __stopThread_noPipe
#endif
#ifdef __stopThread_noinRun
#undef __stopThread_noinRun
#endif
#endif
}

/* 传入数据帧 */
void ptzObject_item_frame_in_pthreadSafety__OI_4(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server PtzObject
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
unsigned int ptzObject_item_frame_out_pthreadSafety__IO_4(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server PtzObject
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
        //FsPrintf(1, "ptzObject_item_frame_out_pthreadSafety__IO_4\n");
        pthread_mutex_lock(&/* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->mutex);
        index = /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->nodeCount;
        if (index > frameCount)index = frameCount;
        FsObjectImageFrame **ppNode = (FsObjectImageFrame**) /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->pNode + /* pRecognition_item */ FsMacrosValue3(p, __item_frame_out_pthreadSafety__IO_4_Server, _item)->ro.__framelistOut->startIndex;
        for (frameCount = index; frameCount > 0; frameCount--) {
            //FsPrintf(1, "index=%d,stats->decodeMask_set=%llx/%llx/%lx\n", (*ppNode)->index, (*ppNode)->stats->decodeMask_set, (*ppNode)->stats->decodeMask, ImageFrame_YUV420P_0);
            *ppFrame++ = *ppNode++;
        }
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
unsigned int ptzObject_item_get_frame_count_pthreadSafety(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server PtzObject
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
void ptzObject_item_reset_groupSqrtOut(struct PtzObject * const pPtzObject, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server PtzObject
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



