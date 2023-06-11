#include "inflrayObject.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/TypeFace.h"
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
#if 1
static unsigned int moduleLoglevel = FsNumberMax(loglevel); // 本模块使用自定义的loglevel,最大值表示未加到日志级别管理模块中
#define loglevel moduleLoglevel
#endif
/* 开方分组用的互斥锁 */
static pthread_mutex_t groupSqrt_mutex = PTHREAD_MUTEX_INITIALIZER;
/* 缓存轨迹的最大数量 */
#define InflrayObject_trail_count 64

#define PI acos(-1)

struct InflrayObject_P_pos_node {
    struct ObjectTrack_pos_node pos_node;
    /* 目标距离 */
    int distance;
    /* 目标经纬度 */
    double longitude, latitude;
    /* 类别的概率,(需要开启识别才有效,没有开启时为0.85) */
    float prob;
    /* 类型(需要开启识别才有效),-1未知,1-人 */
    signed char type;
    /* 是否检测到,算法在一定范围内未检测到也会输出,未检测到此值为0,检测到此值为1 */
    signed char is_detected;
    /* 是否要画轨迹,算法初期检测到目标可能还未确定目标,未确定时此值为0,确定此值为1 */
    signed char isDrawing_tracks;

    char belong_area_name[64];
};

struct InflrayObject_P_item_track {
    struct ObjectTrack_pos_item objectTrack_pos_item;
    /* 目标编号,使用时间*1000+随机码表示 */
    unsigned long long objIndex;
    /* 原始目标id用于匹配 */
    unsigned int id;
};

struct InflrayObject_P_area {
    /* 区域点集 */
    struct Fs_Points const *pPoint;
    /* 区域名称 */
    char area_name[64];
};

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

static void inflrayObject_P_item_track_delete__OI(struct InflrayObject_P_item_track * const pInflrayObject_P_item_track) {
    objectTrack_pos_item_delete__OI((struct ObjectTrack_pos_item *) pInflrayObject_P_item_track);
}

static void inflrayObject_P_area_delete__OI(struct InflrayObject_P_area * const pInflrayObject_P_area) {
    fsFree(pInflrayObject_P_area);
}

/* 从ObjectTrack_pos_item对象中获取id的方法 */
static int inflrayObject_P_item_track_get_id(struct InflrayObject_P_item_track * pInflrayObject_P_item_track) {
    return pInflrayObject_P_item_track->id;
}

/* 使用pObjectTrack_pos_data初始化pObjectTrack_pos_node中除ObjectTrack_pos_node结构中定义的数据,可为空 */
static void inflrayObject_P_pos_node_init_extern(struct InflrayObject_P_pos_node* pInflrayObject_P_pos_node, struct DetectAlgorithmLib_ConventionalDetect_pos_data * pDetectAlgorithmLib_ConventionalDetect_pos_data) {
    memcpy(&pInflrayObject_P_pos_node->prob, &pDetectAlgorithmLib_ConventionalDetect_pos_data->prob, FsPointer_to_integer((&((struct InflrayObject_P_pos_node*) 0)->isDrawing_tracks + 1)) - FsPointer_to_integer(&((struct InflrayObject_P_pos_node*) 0)->prob));
}

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int inflrayObject_P_item_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */struct InflrayObject_item * const pInflrayObject_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsStructList * const clientList_i8 = pInflrayObject_item->ro.__clientList_i8;
    pthread_mutex_lock(&clientList_i8->mutex);
    configManager_conncet_refer_check(pInflrayObject_item->ro._pInflrayObject->ro._pConfigManager, clientList_i8);
    pthread_mutex_unlock(&clientList_i8->mutex);
    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int inflrayObject_P_item_cb_debug(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct InflrayObject_item * const pInflrayObject_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    const unsigned int datatype = fs_Ebml_node_get_first_Integer(pEbml, (struct FsEbml_node*) pEbml, "datatype", 0);
    if (datatype <= 1) {
        if (0 == datatype) {
#ifndef __cmd_cb_cal_return_type 
            const char *const return_type = fs_Ebml_node_get_first_string(pEbml, (struct FsEbml_node*) pEbml, "return_type", NULL);
            if (return_type) {
                if (0 == strcmp("ebml", return_type)) requestDataType = 1;
                else if (0 == strcmp("xml", return_type)) requestDataType = 2;
                else if (0 == strcmp("json", return_type)) requestDataType = 3;
            }
#endif
        }
        unsigned int data[8] = {ConfigManager_refer_connect_node_make6_array(requestID_3, headType, checkMethod, virtualConnection, requestDataType, head), datatype, 0};
        pthread_mutex_lock(&pInflrayObject_item->ro.__clientList_i8->mutex);
        configManager_conncet_refer_check(pInflrayObject_item->ro._pInflrayObject->ro._pConfigManager, pInflrayObject_item->ro.__clientList_i8);
        if (fs_StructList_insert_order(pInflrayObject_item->ro.__clientList_i8, data) < 0) {
            pthread_mutex_unlock(&pInflrayObject_item->ro.__clientList_i8->mutex);
            return 1;
        } else {
            pthread_mutex_unlock(&pInflrayObject_item->ro.__clientList_i8->mutex);
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

static void inflray_private_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p) {
    struct InflrayObject_item* pInflrayObject_item = (struct InflrayObject_item *) p;
    if (strcmp("ptz_link", cmd) == 0 && strcmp(uuid, pInflrayObject_item->p.ptz_uuid) == 0) {
        FsPrintf(1, "inflray_private_cmd_connect_cb\n");
        pthread_mutex_lock(&pInflrayObject_item->ro.__mutexCmdConnect);
        pInflrayObject_item->p.ptz_item_linkPtz = (Ptz_item_linkPtz_pthreadSafety) do_local;
        pInflrayObject_item->p.pPtzObject_item = (struct PtzObject_item*) do_local_p;
        pthread_mutex_unlock(&pInflrayObject_item->ro.__mutexCmdConnect);
    }
}

static FsConfig *inflrayObject_P_protocol_debug() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 19, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "inflrayObject_debug", "inflrayObject_debug", "inflrayObject_debug");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "datatype", "数据类型", "数据类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 6, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "序列化数据", "序列化数据");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "目标框图(图片流)", "目标框图(图片流)");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "datatype", FsConfig_Condition_equal, "0");
    }
    return pConfig;
}

/* 删除pInflrayObject指向的实例对象 */
static void inflrayObject_P_item_delete__OI(struct InflrayObject_item * const pInflrayObject_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pInflrayObject_item)return;
    void *const pConfigManager = pInflrayObject_item->ro._pInflrayObject->ro._pConfigManager;
    // 注销连接断开时的回调通知
    if (0 == pInflrayObject_item->ro._ipv4) configManager_connect_error_logoff(pConfigManager, (int (*)(const unsigned int *, void *, char * * const))inflrayObject_P_item_cb_connect_error, pInflrayObject_item);
    /* 注销命令字 */
    if (0 == pInflrayObject_item->ro._ipv4) configManager_cmd_logoff(pConfigManager, "inflrayObject_debug", pInflrayObject_item->ro._uuid, pInflrayObject_item->ro._ipv4, pInflrayObject_item, pShareBuffer);
    //销毁
    FsPrintf(1, "delete rst->p.ptz_uuid=%s\n", pInflrayObject_item->p.ptz_uuid);
    if (0 == pInflrayObject_item->ro._ipv4 && pInflrayObject_item->p.ptz_uuid) configManager_cmd_disconnect(pConfigManager, "ptz_link", pInflrayObject_item->p.ptz_uuid, pInflrayObject_item);
    if (0 == pInflrayObject_item->ro._ipv4) {
        /* 传入的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pInflrayObject_item->ro.__framelistIn);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_destructor__OI(pInflrayObject_item->ro.__framelistOut);
        /* 关闭所有与之相关的客户端链表 */
        configManager_conncet_refer_close(pConfigManager, pInflrayObject_item->ro.__clientList_i8, 1);
        /* 与之相关的客户端链表 */
        fs_StructList_delete__OI(pInflrayObject_item->ro.__clientList_i8);
        /* 储存ConventionalDetect检测对象的链表 */
        if (pInflrayObject_item->p.__pConventionalDetectList_)fs_ObjectList_delete_unsafe_custom__OI(pInflrayObject_item->p.__pConventionalDetectList_, detectAlgorithmLib_ConventionalDetect_delete__OI);
        /* 跟踪中的所有目标 */
        fs_ObjectList_delete_unsafe_custom__OI(pInflrayObject_item->p.__objectList_, (void (*)(void*))inflrayObject_P_item_track_delete__OI);
        /* 区域线条链表(x1,y1)(x2,y2),每个数字占2字节 */
        fs_StructList_delete__OI(pInflrayObject_item->p.__areaLineList_);
        /* 屏蔽区域线条链表(x1,y1)(x2,y2),每个数字占2字节 */
        fs_StructList_delete__OI(pInflrayObject_item->p.__excludeAreaLineList_);
        /* 水印文字链表,成员为struct FsTypeFaceText */
        fs_ObjectList_delete_unsafe_custom__OI(pInflrayObject_item->p.__textList_, (void (*)(void*))fs_TypeFace_text_delete__OI);
        /* 区域及对应名称链表 */
        fs_ObjectList_delete_unsafe_custom__OI(pInflrayObject_item->p.__areaNameList, (void (*)(void*))inflrayObject_P_area_delete__OI);
    }
    fsFree(pInflrayObject_item);
}

static void inflrayObject_P_item_new(struct InflrayObject * const pInflrayObject, /* 通道号,从1开始,为0用于集群 */const unsigned int channel
        , /* 本地ip地址 */const FsArray * const ipList, /* 原来的检测项 */struct InflrayObject_item * * const ppInflrayObject_item, FsGroupSqrt * const pGroupSqrt
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pInflrayObject->ro._pConfigManager)->ro.__pConfig;
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
                        if (*ppInflrayObject_item != NULL && (*ppInflrayObject_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode)) {
                            /* 检查内部时间切换 */
                            struct InflrayObject_item * const rst = *ppInflrayObject_item;
                            const unsigned long long childTimeControlSum = fs_Config_get_sum_timeControl(pConfig, vsysChannel0, vsysChannel, "inflrayObjectConfig targetCheckErea");
                            if (rst->ro._timeControlSum != childTimeControlSum) {
                                rst->ro._timeControlSum = childTimeControlSum;
                                /* 储存ConventionalDetect检测对象的链表 */
                                if (rst->p.__pConventionalDetectList_) {
                                    fs_ObjectList_delete_unsafe_custom__OI(rst->p.__pConventionalDetectList_, detectAlgorithmLib_ConventionalDetect_delete__OI);
                                    rst->p.__pConventionalDetectList_ = NULL;
                                }
                                /* 跟踪中的所有目标 */
                                fs_ObjectList_clear_custom(rst->p.__objectList_, (void (*)(void*))inflrayObject_P_item_track_delete__OI);
                                /* 区域线条链表(x1,y1)(x2,y2),每个数字占2字节 */
                                fs_StructList_clear(rst->p.__areaLineList_);
                                /* 屏蔽区域线条链表(x1,y1)(x2,y2),每个数字占2字节 */
                                fs_StructList_clear(rst->p.__excludeAreaLineList_);
                                /* 水印文字链表,成员为struct FsTypeFaceText */
                                fs_ObjectList_clear_custom(rst->p.__textList_, (void (*)(void*))fs_TypeFace_text_delete__OI);
                                /* 区域及对应名称链表 */
                                fs_ObjectList_clear_custom(rst->p.__areaNameList, (void (*)(void*))inflrayObject_P_area_delete__OI);
                            }
                            break;
                        }
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppInflrayObject_item) {
                        inflrayObject_P_item_delete__OI(*ppInflrayObject_item, pShareBuffer);
                    }
                    struct InflrayObject_item * rst;
                    if ((InflrayObject_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0 && InflrayObject_Mask != 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        const FsString * ptz_uuid = NULL;
                        const FsString * device_name = NULL;
                        if (channel > 0) {
                            const void *inflrayObjectConfig0 = vsysChannel0;
                            const void *const inflrayObjectConfig = fs_Config_node_get_first(pConfig, &inflrayObjectConfig0, vsysChannel, "inflrayObjectConfig");
                            int ptz_link_enable = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "ptz_link_enable", 1, NULL);
                            if (ptz_link_enable) {
                                ptz_uuid = fs_Config_node_string_get_first_String(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "ptz_uuid", NULL);
                            }
                            if (ptz_uuid)len += ptz_uuid->lenth;
                            device_name = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "recordConfig devicename", NULL);
                            if (device_name)len += device_name->lenth;
                            rst = (struct InflrayObject_item*) fsMalloc(Memery_Alignment(sizeof (struct InflrayObject_item)) + len);
                            memset(rst, 0, sizeof (struct InflrayObject_item));
                            pd = ((char*) rst) + Memery_Alignment(sizeof (struct InflrayObject_item));
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 与之相关的客户端链表 */
                            rst->ro.__clientList_i8 = fs_StructList_new__IO(6, sizeof (int)*8);
                            /* 跟踪中的所有目标 */
                            rst->p.__objectList_ = fs_ObjectList_new_unsafe__IO(5);
                            /* 区域线条链表(x1,y1)(x2,y2),每个数字占2字节+color(4字节,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔) */
                            rst->p.__areaLineList_ = fs_StructList_new__IO(2, sizeof (unsigned short)*6);
                            /* 屏蔽区域线条链表(x1,y1)(x2,y2),每个数字占2字节 */
                            rst->p.__excludeAreaLineList_ = fs_StructList_new__IO(2, sizeof (unsigned short)*4);
                            /* 区域及对应名称链表 */
                            rst->p.__areaNameList = fs_ObjectList_new_unsafe__IO(6);
                            /* 联动目标编号 */
                            rst->p.objIndex = 0;
                            /* 水印文字链表,成员为struct FsTypeFaceText */
                            rst->p.__textList_ = fs_ObjectList_new_unsafe__IO(2);
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
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pInflrayObject->ro._pGroupSqrtOut->groupSqrt_member, pInflrayObject->ro._pGroupSqrtOut->groupSqrt_group
                                    , pInflrayObject->ro._pGroupSqrtOut->groupValue, channel - 1, &pInflrayObject->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        if (channel > 0) {
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len), pd += len;
                            /* 所属的目标检测对象 */
                            rst->ro._pInflrayObject = pInflrayObject;
                        }
                        if (channel > 0) {
                            // 获取配置参数
                            if (ptz_uuid) {
                                rst->p.ptz_uuid = pd, len = ptz_uuid->lenth, memcpy(pd, ptz_uuid->buffer, len), pd += len;
                                FsPrintf(1, "bind rst->ro._uuid=%s\n", rst->p.ptz_uuid);
                            }
                        }
                        if (channel > 0) {
                            rst->p.device_name = pd, len = device_name->lenth, memcpy(pd, device_name->buffer, len), pd += len;
                        }
                        if (channel > 0) {
                            // 获取距离标定参数
                            rst->p.target_distance_enable = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "distance distance_enable", 0, NULL);
                            FsPrintf(1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
                            FsPrintf(1, "target_distance_enable=%d\n", rst->p.target_distance_enable);
                            if (rst->p.target_distance_enable) {
                                rst->p.start_x = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "distance start_x", 0, NULL);
                                rst->p.start_y = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "distance start_y", 0, NULL);
                                rst->p.end_x = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "distance end_x", 0, NULL);
                                rst->p.end_y = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "distance end_y", 0, NULL);
                                rst->p.device_height = fs_Config_node_float_get_first(pConfig, vsysChannel0, vsysChannel, "distance device_height", 0.0, NULL);
                                rst->p.device_field_angle = fs_Config_node_float_get_first(pConfig, vsysChannel0, vsysChannel, "distance device_field_angle", 6.4, NULL);
                                const FsString * const position_jwd = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "positon center", NULL);
                                if (NULL == position_jwd || sscanf(position_jwd->buffer, "%f,%f", &rst->p.device_longitude, &rst->p.device_latitude) != 2) {
                                    rst->p.device_longitude = 0.0;
                                    rst->p.device_latitude = 0.0;
                                }
                                FsPrintf(1, "device_height=%f,device_latitude=%f", rst->p.device_longitude, rst->p.device_latitude);
                                //FsPrintf(1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
                                //FsPrintf(1, "start_pos=(%d, %d)--end_pos=(%d, %d)--device_height=%lf--device_field_angle=%lf\n", rst->p.start_x, rst->p.start_y, rst->p.end_x, rst->p.end_y, rst->p.device_height, rst->p.device_field_angle);

                            }
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        // 注册连接断开时的回调通知
                        if (0 == ipv4) configManager_connect_error_register(pInflrayObject->ro._pConfigManager
                                , (int (*)(const unsigned int *, void *, char * * const))inflrayObject_P_item_cb_connect_error, rst);
                        /* 注册命令字 */
                        if (0 == ipv4) configManager_cmd_register(pInflrayObject->ro._pConfigManager, "inflrayObject_debug", rst->ro._uuid, ipv4, rst, 0
                                , (int (* const) (const unsigned int *, unsigned char, unsigned char, unsigned int, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))inflrayObject_P_item_cb_debug, NULL, rst, pShareBuffer);
                        //                    if (0 == ipv4)configManager_cmd_register(pRecord->ro._pConfigManager, "record_inc", rst->ro._uuid, 0, rst, NULL, (void*) record_item_add_inc_pthreadSafety__OI_2, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "videolist_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "videotimeinfo_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                        //                    configManager_cmd_register(pRecord->ro._pConfigManager, "snapshort_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_snapshort_cb : NULL, NULL, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        FsPrintf(1, "bind rst->p.ptz_uuid=%s\n", rst->p.ptz_uuid);
                        if (channel > 0 && rst->p.ptz_uuid) configManager_cmd_connect(pInflrayObject->ro._pConfigManager, "ptz_link", rst->p.ptz_uuid, rst, inflray_private_cmd_connect_cb, rst);
                        //if (channel > 0) configManager_cmd_connect(pRecord->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))record_private_cmd_connect_cb, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 释放内存空间 */
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0) *ppInflrayObject_item = rst;
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

static FsObjectList* inflrayObject_P_item_new_conventionalDetect__IO(struct InflrayObject_item * const pInflrayObject_item, unsigned int index, unsigned short width, unsigned short height
        , /* 参考标定的宽度 */unsigned int width0, /* 参考标定的高度 */ unsigned int height0
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#ifdef FsDebug
    if (pInflrayObject_item->p.__pConventionalDetectList_ != NULL) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "pInflrayObject_item->p.__pConventionalDetectList_(=%p)!=NULL.\n", pInflrayObject_item->p.__pConventionalDetectList_);
        fflush(stdout);
        FsLogExit();
    }
#endif
    ConfigManager * const pConfigManager = (ConfigManager*) pInflrayObject_item->ro._pInflrayObject->ro._pConfigManager;
    configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
    FsConfig * const pConfig = pConfigManager->ro.__pConfig;
    const void *inflrayObjectConfig0 = pConfig;
    const void *inflrayObjectConfig;
    {
        inflrayObjectConfig = fs_Config_node_template_get_orderFirst(pConfig, &inflrayObjectConfig0, pConfig, 0, pConfigManager->ro.__ipList_local, 0, "vsys", 0);
        if (inflrayObjectConfig) {
            inflrayObjectConfig = fs_Config_node_template_get(pConfig, &inflrayObjectConfig0, inflrayObjectConfig, 0, NULL, 0, "vsysChannel", index);
            if (inflrayObjectConfig) {
                inflrayObjectConfig = fs_Config_node_get_first(pConfig, &inflrayObjectConfig0, inflrayObjectConfig, "inflrayObjectConfig");
            }
        }
    }
    int x1 = 0, y1 = 0, x2 = width - 1, y2 = height - 1;
    FsObjectList *rst = NULL;
    fs_StructList_clear(pInflrayObject_item->p.__areaLineList_);
    fs_StructList_clear(pInflrayObject_item->p.__excludeAreaLineList_);
    fs_ObjectList_clear_custom(pInflrayObject_item->p.__textList_, (void (*)(void*))fs_TypeFace_text_delete__OI);
    fs_ObjectList_clear_custom(pInflrayObject_item->p.__objectList_, (void (*)(void*))inflrayObject_P_item_track_delete__OI);
    fs_ObjectList_clear_custom(pInflrayObject_item->p.__areaNameList, (void (*)(void*))inflrayObject_P_area_delete__OI);
    if (inflrayObjectConfig) {
        const void * targetCheckErea0 = inflrayObjectConfig0;
        FsObjectList * const targetCheckEreaList = fs_Config_node_template__IO(pConfig, &targetCheckErea0, inflrayObjectConfig, 3, NULL, 0, "targetCheckErea");
        FsPrintf(1, "targetCheckEreaList->nodeCount=%lu,pInflrayObject_item=%p.\n", targetCheckEreaList ? targetCheckEreaList->nodeCount : 0, pInflrayObject_item);
        FsConfigResult_area * const pExcludeArea = fs_Config_node_string_area_lenth_get__IO(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "excludeArea", NULL, width0 * 1.0 / width, height0 * 1.0 / height, 0, 0);
        FsPrintf(1, "pExcludeArea=%p,pExcludeArea->count=%u.\n", pExcludeArea, pExcludeArea ? pExcludeArea->count : 0);
        /* 屏蔽区域颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */
        pInflrayObject_item->p.excludeAreaColor = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "excludeAreaColor", 0xFF000000U, NULL);
        FsConfigResult_string * const pExcludeAreaName = fs_Config_node_string_get__IO(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "excludeAreaName", NULL);
        if (pExcludeArea) {
            FsTypeFace *pTypeFace = NULL;
            unsigned int excludeAreaName_color = 0xFF0000;
            /* 计算区域的外接矩形 */
            if (1) {
                unsigned int uj;
                x1 = width - 1, y1 = height - 1, x2 = 0, y2 = 0;
                for (uj = 0; uj < pExcludeArea->count; uj++) {
                    int _x1 = width - 1, _y1 = height - 1, _x2 = 0, _y2 = 0;
                    // printf(" kkkkkkkkkkkkk1,uj =%u,pArea->count=%u,pArea=%p,%lu\n", uj, pArea->count,pArea,((char*) pArea->data)-((char*) pArea));
                    struct Fs_Points * const pPoint = (struct Fs_Points *) pExcludeArea->data[uj];
                    // printf("kkkkkkkkkkkkk2 uj =%u,pArea->count=%u,pPoint=%p/%lu\n", uj, pArea->count, pPoint, ((char*) pPoint)-((char*) pArea));
                    unsigned int ui = pPoint->count;
                    FsPrintf(1, "pExcludeArea=%p,pExcludeArea->count=%u,uj=%u,%p,pPoint->count=%u.\n", pExcludeArea, pExcludeArea->count, uj, pPoint, pPoint->count);
                    int (*thePoint)[2] = pPoint->point;
                    while (ui-- > 0) {
                        int x = thePoint[0][0], y = thePoint[0][1];
                        thePoint++;
                        if (ui > 0) {
                            unsigned short line[] = {(unsigned short) x, (unsigned short) y, (unsigned short) thePoint[0][0], (unsigned short) thePoint[0][1]};
                            //FsPrintf(1, "(%hu,%hu)(%hu,%hu)\n", line[0], line[1], line[2], line[3]);
                            fs_StructList_insert_tail(pInflrayObject_item->p.__excludeAreaLineList_, line);
                        } else {
                            unsigned short line[] = {(unsigned short) x, (unsigned short) y, (unsigned short) pPoint->point[0][0], (unsigned short) pPoint->point[0][1]};
                            //FsPrintf(1, "(%hu,%hu)(%hu,%hu)\n", line[0], line[1], line[2], line[3]);
                            fs_StructList_insert_tail(pInflrayObject_item->p.__excludeAreaLineList_, line);
                        }
                        if (_x1 > x)_x1 = x;
                        if (_x2 < x)_x2 = x;
                        if (_y1 > y)_y1 = y;
                        if (_y2 < y)_y2 = y;
                        if (x1 > x)x1 = x;
                        if (x2 < x)x2 = x;
                        if (y1 > y)y1 = y;
                        if (y2 < y)y2 = y;
                    }
                    if (pExcludeAreaName && uj < pExcludeAreaName->count) {
                        /* 有设置区域名字 */
                        if (NULL == pTypeFace) {
                            excludeAreaName_color = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "excludeAreaName_color", 0xFF0000, NULL);
                            if (excludeAreaName_color < 0xFF000000U) {
                                excludeAreaName_color = image_dib_rgb_to_ycc(excludeAreaName_color);
                                const unsigned short excludeAreaName_width = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "excludeAreaName_width", 32, NULL);
                                const unsigned short excludeAreaName_height = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "excludeAreaName_height", 32, NULL);
                                pTypeFace = fs_TypeFace_new__IO(NULL, excludeAreaName_width, excludeAreaName_height, 1, 0);
                            }
                        }
                        if (pTypeFace) {
                            const int chars = fs_String_buffer_get_chars_width(strlen(pExcludeAreaName->data[uj]), (const unsigned char*) pExcludeAreaName->data[uj], FsStringCode_Unkown);
                            int _x0 = (_x1 + _x2) / 2;
                            int _y0 = (_y1 + _y2) / 2;
                            _x0 -= pTypeFace->width * (chars + 1) / 2;
                            _y0 -= (pTypeFace->height + 1) / 2;
                            struct FsTypeFaceText * const pTypeFaceText = fs_TypeFace_text_new__IO(pTypeFace, Fs_num_max(_x0, _x1), Fs_num_max(_y0, _y1), excludeAreaName_color, pExcludeAreaName->data[uj]);
                            fs_ObjectList_insert_tail(pInflrayObject_item->p.__textList_, pTypeFaceText);
                        } else if (excludeAreaName_color < 0xFF000000U) {
                            FsLog(FsLogType_error, FsPrintfIndex, "New TypeFace failed.\n");
                            fflush(stdout);
                        }
                    }
                }
            }
            if (pTypeFace)fs_TypeFace_delete__OI(pTypeFace);
        }
        if (pExcludeAreaName)fsFree(pExcludeAreaName);
        if (targetCheckEreaList) {
            rst = fs_ObjectList_new_unsafe__IO(2);
            {
                void **ppNode = targetCheckEreaList->pNode + targetCheckEreaList->startIndex;
                unsigned int targetCheckEreaListCount = targetCheckEreaList->nodeCount;
                while (targetCheckEreaListCount-- > 0) {
                    const void * const targetCheckErea = *ppNode++;
                    if (fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "enable", 1, NULL) == 0)continue;
                    FsConfigResult_area * const pArea = fs_Config_node_string_area_lenth_get__IO(pConfig, targetCheckErea0, targetCheckErea, "area", NULL, width0 * 1.0 / width, height0 * 1.0 / height, 0, 0);
                    FsPrintf(1, "pArea=%p\n", pArea);
                    fflush(stdout);
                    if (pArea) {
                        FsConfigResult_integer * const pAreaColor = fs_Config_node_integer_get__IO(pConfig, targetCheckErea0, targetCheckErea, "areaColor", NULL);
                        FsConfigResult_string * const pAreaName = fs_Config_node_string_get__IO(pConfig, targetCheckErea0, targetCheckErea, "areaName", NULL);
                        FsConfigResult_integer * const pWatermarking_color = fs_Config_node_integer_get__IO(pConfig, targetCheckErea0, targetCheckErea, "watermarking_color", NULL);
                        {
                            FsConfigResult_integer * const pArea_enable = fs_Config_node_integer_get__IO(pConfig, targetCheckErea0, targetCheckErea, "area_enable", NULL);
                            if (pArea_enable) {
                                unsigned int uj, ui = 0;
                                for (uj = 0; uj < pArea->count; uj++) {
                                    if ((pArea_enable->count > uj && ((long long*) (((char*) pArea_enable) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[uj])
                                            || (pArea_enable->count <= uj && ((long long*) (((char*) pArea_enable) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[pArea_enable->count - 1])) {
                                        if (((long long*) (((char*) pArea_enable) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[uj]) {
                                            pArea->data[ui] = pArea->data[uj];
                                            if (pAreaColor) {
                                                if (pAreaColor->count > uj) {
                                                    ((long long*) (((char*) pAreaColor) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[ui] = ((long long*) (((char*) pAreaColor) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[uj];
                                                } else if (ui < pAreaColor->count) {
                                                    ((long long*) (((char*) pAreaColor) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[ui] = ((long long*) (((char*) pAreaColor) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[pAreaColor->count - 1];
                                                }
                                            }
                                            if (pAreaName) {
                                                if (pAreaName->count > uj) {
                                                    pAreaName->data[ui] = pAreaName->data[uj];
                                                } else if (ui < pAreaName->count) {
                                                    pAreaName->count = ui;
                                                }
                                            }
                                            if (pWatermarking_color) {
                                                if (pWatermarking_color->count > uj) {
                                                    ((long long*) (((char*) pWatermarking_color) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[ui] = ((long long*) (((char*) pWatermarking_color) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[uj];
                                                } else if (ui < pWatermarking_color->count) {
                                                    ((long long*) (((char*) pWatermarking_color) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[ui] = ((long long*) (((char*) pWatermarking_color) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))[pWatermarking_color->count - 1];
                                                }
                                            }
                                            ui++;
                                        }
                                    }
                                }
                                pArea->count = ui;
                                fsFree(pArea_enable);
                            }
                        }
                        FsPrintf(1, "pArea=%p/%u\n", pArea, pArea->count);
                        fflush(stdout);
                        if (pArea->count > 0) {
                            FsTypeFace *pTypeFace = NULL;
                            ////////////////////////////////////////////////////////////////////
                            /* 计算区域的外接矩形 */
                            if (1) {
                                unsigned int uj;
                                x1 = width - 1, y1 = height - 1, x2 = 0, y2 = 0;
                                for (uj = 0; uj < pArea->count; uj++) {
                                    int _x1 = width - 1, _y1 = height - 1, _x2 = 0, _y2 = 0;
                                    // printf(" kkkkkkkkkkkkk1,uj =%u,pArea->count=%u,pArea=%p,%lu\n", uj, pArea->count,pArea,((char*) pArea->data)-((char*) pArea));
                                    struct Fs_Points * const pPoint = (struct Fs_Points *) pArea->data[uj];
                                    // printf("kkkkkkkkkkkkk2 uj =%u,pArea->count=%u,pPoint=%p/%lu\n", uj, pArea->count, pPoint, ((char*) pPoint)-((char*) pArea));
                                    unsigned int ui = pPoint->count;
                                    int (*thePoint)[2] = pPoint->point;
                                    while (ui-- > 0) {
                                        int x = thePoint[0][0], y = thePoint[0][1];
                                        thePoint++;
                                        unsigned int areaColor = pAreaColor != NULL ? ((long long*) (((char*) pAreaColor) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8))) [pAreaColor->count > uj ? uj : pAreaColor->count - 1] : 0xFF000000;
                                        if (areaColor < 0xFF000000U) {
                                            areaColor = image_dib_rgb_to_ycc(areaColor);
                                            if (ui > 0) {
                                                unsigned short line[] = {(unsigned short) x, (unsigned short) y, (unsigned short) thePoint[0][0], (unsigned short) thePoint[0][1], ((unsigned short*) &areaColor)[0], ((unsigned short*) &areaColor)[1]};
                                                //FsPrintf(1, "(%hu,%hu)(%hu,%hu)\n", line[0], line[1], line[2], line[3]);
                                                fs_StructList_insert_tail(pInflrayObject_item->p.__areaLineList_, line);
                                            } else {
                                                unsigned short line[] = {(unsigned short) x, (unsigned short) y, (unsigned short) pPoint->point[0][0], (unsigned short) pPoint->point[0][1], ((unsigned short*) &areaColor)[0], ((unsigned short*) &areaColor)[1]};
                                                //FsPrintf(1, "(%hu,%hu)(%hu,%hu)\n", line[0], line[1], line[2], line[3]);
                                                fs_StructList_insert_tail(pInflrayObject_item->p.__areaLineList_, line);
                                            }
                                        }
                                        if (_x1 > x)_x1 = x;
                                        if (_x2 < x)_x2 = x;
                                        if (_y1 > y)_y1 = y;
                                        if (_y2 < y)_y2 = y;
                                        if (x1 > x)x1 = x;
                                        if (x2 < x)x2 = x;
                                        if (y1 > y)y1 = y;
                                        if (y2 < y)y2 = y;
                                    }
                                    if (pAreaName && uj < pAreaName->count) {
                                        /* 有设置区域名字 */
                                        if (NULL == pTypeFace) {
                                            const unsigned short watermarking_width = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "watermarking_width", 32, NULL);
                                            const unsigned short watermarking_height = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "watermarking_height", 32, NULL);
                                            pTypeFace = fs_TypeFace_new__IO(NULL, watermarking_width, watermarking_height, 1, 0);
                                        }
                                        if (pTypeFace) {
                                            unsigned int watermarking_color = pWatermarking_color != NULL ? ((long long*) (((char*) pWatermarking_color) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8)))
                                                    [pWatermarking_color->count > uj ? uj : pWatermarking_color->count - 1] : 0xFF0000;
                                            if (watermarking_color < 0xFF000000U) {
                                                watermarking_color = image_dib_rgb_to_ycc(watermarking_color);
                                                const int chars = fs_String_buffer_get_chars_width(strlen(pAreaName->data[uj]), (const unsigned char*) pAreaName->data[uj], FsStringCode_Unkown);
                                                int _x0 = (_x1 + _x2) / 2;
                                                int _y0 = (_y1 + _y2) / 2;
                                                _x0 -= pTypeFace->width * (chars + 1) / 2;
                                                _y0 -= (pTypeFace->height + 1) / 2;
                                                struct FsTypeFaceText * const pTypeFaceText = fs_TypeFace_text_new__IO(pTypeFace, Fs_num_max(_x0, _x1), Fs_num_max(_y0, _y1), watermarking_color, pAreaName->data[uj]);
                                                fs_ObjectList_insert_tail(pInflrayObject_item->p.__textList_, pTypeFaceText);
                                                {
                                                    // 插入区域名称表
                                                    unsigned int pCount = pPoint->count;
                                                    struct Fs_Points * const pFs_Points = (struct Fs_Points *) fsMalloc(sizeof ( struct Fs_Points) + sizeof (int) * 2 * pCount);
                                                    int (*resPoint)[2] = pPoint->point;
                                                    int (*dstPoint)[2] = pFs_Points->point;
                                                    pFs_Points->count = pCount;
                                                    while (pCount-- > 0) {
                                                        dstPoint[0][0] = resPoint[0][0], dstPoint[0][1] = resPoint[0][1];
                                                        resPoint++;
                                                        dstPoint++;
                                                    }
                                                    struct InflrayObject_P_area * const pInflrayObject_P_area = (struct InflrayObject_P_area *) fsMalloc(sizeof ( struct InflrayObject_P_area));
                                                    pInflrayObject_P_area->pPoint = pFs_Points;
                                                    int iLen = strlen(pAreaName->data[uj]) + 1;
                                                    memcpy(pInflrayObject_P_area->area_name, pAreaName->data[uj], iLen);
                                                    fs_ObjectList_insert_tail(pInflrayObject_item->p.__areaNameList, pInflrayObject_P_area);
                                                }
                                            }
                                        } else {
                                            FsLog(FsLogType_error, FsPrintfIndex, "New TypeFace failed.\n");
                                            fflush(stdout);
                                        }
                                    }
                                }
                                if (x2 > width - 1)x2 = width - 1;
                                if (y2 > height - 1)y2 = height - 1;
                                if (x2 <= x1 || y2 <= y1) {
                                    x1 = 0, y1 = 0, x2 = width - 1, y2 = height - 1;
                                }
                            }
                            ////////////////////////////////////////////////////////////////////   
                            {
                                const int eDetectAlgorithm = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "eDetectAlgorithm", 1, NULL);
                                const int iShiftRadius = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iShiftRadius", 10, NULL);
                                const int iBarRatio = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iBarRatio", 6, NULL);
                                const int iMinSizeX = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "minSizeX", 0, NULL);
                                const int iMinSizeY = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "minSizeY", 0, NULL);
                                const int iMaxSizeX = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "maxSizeX", 512, NULL);
                                const int iMaxSizeY = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "maxSizeY", 512, NULL);
                                const int classCheck = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "classCheck", 0, NULL);
                                const int trackEnable = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "trackEnable", 0, NULL);
                                const int iMaxAngle = trackEnable ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iMaxAngle", 45, NULL) : 45;
                                const double dLimitGauss = trackEnable ? fs_Config_node_float_get_first(pConfig, targetCheckErea0, targetCheckErea, "dLimitGauss", 0.4, NULL) : 0.4;
                                const int iLearnCount = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "learnCount", 5, NULL);
                                const int iSkipCount = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "skipCount", 3, NULL);
                                const int trackOut = fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "trackOut", 0, NULL);
                                const int iUseForSkyTHthresh = 0 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iUseForSkyTHthresh", 65, NULL) : 65;
                                const int iUseForSkyColorType = 0 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iUseForSkyColorType", 0, NULL) : 0;
                                const int iVibeRadius = 1 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iVibeRadius", 0, NULL) : 1;
                                const double fGmmD = 2 == eDetectAlgorithm ? fs_Config_node_float_get_first(pConfig, targetCheckErea0, targetCheckErea, "fGmmD", 1.2, NULL) : 1.2;
                                const int fGmmSdInit = 2 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "fGmmSdInit", 12, NULL) : 12;
                                const int iGrimsonGmmMatchThreshold = 3 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iGrimsonGmmMatchThreshold", 20, NULL) : 20;
                                const int iGrimsonGmmMedianBlurKernelSize = 3 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iGrimsonGmmMedianBlurKernelSize", 3, NULL) : 3;
                                const int iFireRadius = 4 == eDetectAlgorithm ? fs_Config_node_integer_get_first(pConfig, targetCheckErea0, targetCheckErea, "iFireRadius", 32, NULL) : 32;
                                void *const pConventionalDetect = detectAlgorithmLib_ConventionalDetect_new__IO(width, height, eDetectAlgorithm, classCheck, iShiftRadius, iMinSizeX, iMinSizeY, iMaxSizeX, iMaxSizeY, iBarRatio, trackEnable, iMaxAngle, dLimitGauss, iLearnCount, iSkipCount, trackOut
                                        , iUseForSkyTHthresh, iUseForSkyColorType, iVibeRadius, fGmmD, fGmmSdInit, iGrimsonGmmMatchThreshold, iGrimsonGmmMedianBlurKernelSize, iFireRadius, pArea, pExcludeArea, 0, 0);
                                if (NULL == pConventionalDetect) {
                                    FsLog2(FsLogType_info, FsPrintfIndex, "Run detectAlgorithmLib_ConventionalDetect_new__IO failed,uuid:\"%s\",ipv4:\"%hhu.%hhu.%hhu.%hhu\"", pInflrayObject_item->ro._uuid, Fs_Network_Ipv4_print(pInflrayObject_item->ro._ipv4));
                                } else {
                                    fs_ObjectList_insert_tail(rst, pConventionalDetect);
                                }
                            }
                            if (pTypeFace)fs_TypeFace_delete__OI(pTypeFace);
                        }
                        if (pAreaColor)fsFree(pAreaColor);
                        if (pAreaName)fsFree(pAreaName);
                        if (pWatermarking_color)fsFree(pWatermarking_color);
                        fsFree(pArea);
                    }
                }

            }
            fs_ObjectList_delete__OI(targetCheckEreaList);
            if (0 == rst->nodeCount) {
                fs_ObjectList_delete_unsafe__OI(rst);
                rst = NULL;
            } else {
                pInflrayObject_item->p.target_x_extern = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "target_x_extern", 60000, NULL);
                pInflrayObject_item->p.target_y_extern = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "target_y_extern", 60000, NULL);
                /* 屏蔽区域颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔,255个间隔时不叠加 */
                pInflrayObject_item->p.targetColor = fs_Config_node_integer_get_first(pConfig, inflrayObjectConfig0, inflrayObjectConfig, "targetColor", 0xFF000000U, NULL);
            }
        }
        if (pExcludeArea)fsFree(pExcludeArea);
    }
    configManager_config_deleteRefer_pthreadSafety(pConfigManager);
    /* 检测区域 */
    // pVehicleDetect_item->p.x1 = x1, pVehicleDetect_item->p.y1 = y1, pVehicleDetect_item->p.x2 = x2, pVehicleDetect_item->p.y2 = y2;

    return rst;

}

/* 从配置获取通道数 */
static unsigned int inflrayObject_P_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

/* 叠加目标框 */
static void inflrayObject_P_item_add_target_to_frame(struct InflrayObject_item * const pInflrayObject_item, FsObjectImageFrame * const pFrame) {
    if (pInflrayObject_item->p.targetColor >= 0xFF000000U)return;
    const unsigned int ycc = image_dib_rgb_to_ycc(pInflrayObject_item->p.targetColor);
    /* 先取图像 */
    FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
    if (NULL == pYUV420P) {
        FsLog(FsLogType_error, FsPrintfIndex, "Prapare YUV420P_0 failed.\n");
        FsLogTag(10);
        fflush(stdout);
        return;
    }
    struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
    const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
    unsigned int ui = 0;
    for (; ui < count; ui++) {
        //  FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
        struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
        int x1, y1, x2, y2;
        {
            {
                struct ObjectTrack_pos_node * const pObjectTrack_pos_node = pInflrayObject_P_item_track->objectTrack_pos_item.tail;
                x1 = pObjectTrack_pos_node->x1, y1 = pObjectTrack_pos_node->y1, x2 = pObjectTrack_pos_node->x2, y2 = pObjectTrack_pos_node->y2;
            }
            {
                unsigned int pX2[] = {(unsigned int) x2, (unsigned int) x2, (unsigned int) x1, (unsigned int) x1};
                unsigned int pY2[] = {(unsigned int) y1, (unsigned int) y2, (unsigned int) y2, (unsigned int) y1};
                image_dib_draw_lines_ch1(pYUV420P->data, pFrame->width[0], x1, y1, pX2, pY2, Fs_Array_len(pX2), ycc, 0);
            }
            {
                unsigned int pX2[] = {(unsigned int) x2 / 2, (unsigned int) x2 / 2, (unsigned int) x1 / 2, (unsigned int) x1 / 2};
                unsigned int pY2[] = {(unsigned int) y1 / 2, (unsigned int) y2 / 2, (unsigned int) y2 / 2, (unsigned int) y1 / 2};
                image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0], pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 8) | ((ycc & 0xFF000000U) / 2), 0);
                //FsPrintf(1, "tt (%d,%d)(%d,%d) %d,%d\n", x1, y1, x2, y2, pFrame->width[0], pFrame->height[0]);
                image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0]+ ((pFrame->width[0] + 1) / 2)* ((pFrame->height[0] + 1) / 2), pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 16) | ((ycc & 0xFF000000U) / 2), 0);
            }
        }
        if (pInflrayObject_P_item_track->objectTrack_pos_item.posCount > 1) {
            struct ObjectTrack_pos_node * pObjectTrack_pos_node = pInflrayObject_P_item_track->objectTrack_pos_item.head;
            x1 = (pObjectTrack_pos_node->x1 + pObjectTrack_pos_node->x2) / 2, y1 = (pObjectTrack_pos_node->y1 + pObjectTrack_pos_node->y2) / 2;
            pObjectTrack_pos_node = pObjectTrack_pos_node->next;
            do {
                x2 = (pObjectTrack_pos_node->x1 + pObjectTrack_pos_node->x2) / 2, y2 = (pObjectTrack_pos_node->y1 + pObjectTrack_pos_node->y2) / 2;
                {
                    unsigned int pX2[] = {(unsigned int) x2};
                    unsigned int pY2[] = {(unsigned int) y2};
                    image_dib_draw_lines_ch1(pYUV420P->data, pFrame->width[0], x1, y1, pX2, pY2, Fs_Array_len(pX2), ycc, 0);
                }
                {
                    unsigned int pX2[] = {(unsigned int) x2 / 2};
                    unsigned int pY2[] = {(unsigned int) y2 / 2};
                    image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0], pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 8) | ((ycc & 0xFF000000U) / 2), 0);
                    image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0]+ ((pFrame->width[0] + 1) / 2)* ((pFrame->height[0] + 1) / 2), pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 16) | ((ycc & 0xFF000000U) / 2), 0);
                }
                x1 = x2, y1 = y2;
            } while ((pObjectTrack_pos_node = pObjectTrack_pos_node->next) != NULL);
        }
    }
    fs_Object_delete_pthreadSafety__OI(pYUV420P);
}

/* 叠加区域框 */
static void inflrayObject_P_item_add_area_to_frame(struct InflrayObject_item * const pInflrayObject_item, FsObjectImageFrame * const pFrame) {
    if (0 == pInflrayObject_item->p.__areaLineList_->nodeCount)return;
    FsPrintf(1, "__areaLineList_=%p/%lu,%u,%u,count=%lu\n", pInflrayObject_item->p.__areaLineList_, pInflrayObject_item->p.__areaLineList_->nodeCount, pFrame->width[0], pFrame->height[0], pInflrayObject_item->p.__areaLineList_->nodeCount);
    /* 先取图像 */
    FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
    if (NULL == pYUV420P) {
        FsLog(FsLogType_error, FsPrintfIndex, "Prapare YUV420P_0 failed.\n");
        FsLogTag(10);
        fflush(stdout);
        return;
    }
    unsigned short (* const pNode) [6] = (unsigned short (*)[6]) (pInflrayObject_item->p.__areaLineList_->pNode + pInflrayObject_item->p.__areaLineList_->startIndex * pInflrayObject_item->p.__areaLineList_->structSize);
    const unsigned int count = pInflrayObject_item->p.__areaLineList_->nodeCount;
    unsigned int ui = 0;
    for (; ui < count; ui++) {
        //FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
        int x1 = pNode[ui][0], y1 = pNode[ui][1], x2 = pNode[ui][2], y2 = pNode[ui][3];
        const unsigned int ycc = ((unsigned int*) (pNode[ui] + 4))[0];
        //FsPrintf(1, "TTTTTTTTTTTTTTTTTTt(%d,%d)(%d,%d),%u/%u\n", x1, y1, x2, y2, ui, count);
        {
            unsigned int pX2[] = {(unsigned int) x2};
            unsigned int pY2[] = {(unsigned int) y2};
            image_dib_draw_lines_ch1(pYUV420P->data, pFrame->width[0], x1, y1, pX2, pY2, Fs_Array_len(pX2), ycc, 0);
        }
        {
            unsigned int pX2[] = {(unsigned int) x2 / 2};
            unsigned int pY2[] = {(unsigned int) y2 / 2};
            image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0], pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 8) | ((ycc & 0xFF000000U) / 2), 0);
            image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0]+ ((pFrame->width[0] + 1) / 2)* ((pFrame->height[0] + 1) / 2), pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 16) | ((ycc & 0xFF000000U) / 2), 0);
        }
    }
    fs_Object_delete_pthreadSafety__OI(pYUV420P);
}

/* 叠加屏蔽区域区域框 */
static void inflrayObject_P_item_add_excludeArea_to_frame(struct InflrayObject_item * const pInflrayObject_item, FsObjectImageFrame * const pFrame) {
    if (pInflrayObject_item->p.excludeAreaColor >= 0xFF000000U)return;
    FsPrintf(1, "excludeAreaColor=%x,%u,%u,count=%lu\n", pInflrayObject_item->p.excludeAreaColor, pFrame->width[0], pFrame->height[0], pInflrayObject_item->p.__excludeAreaLineList_->nodeCount);
    const unsigned int ycc = image_dib_rgb_to_ycc(pInflrayObject_item->p.excludeAreaColor);
    /* 先取图像 */
    FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
    if (NULL == pYUV420P) {
        FsLog(FsLogType_error, FsPrintfIndex, "Prapare YUV420P_0 failed.\n");
        FsLogTag(10);
        fflush(stdout);
        return;
    }
    unsigned short (* const pNode) [4] = (unsigned short (*)[4]) (pInflrayObject_item->p.__excludeAreaLineList_->pNode + pInflrayObject_item->p.__excludeAreaLineList_->startIndex * pInflrayObject_item->p.__excludeAreaLineList_->structSize);
    const unsigned int count = pInflrayObject_item->p.__excludeAreaLineList_->nodeCount;
    unsigned int ui = 0;
    for (; ui < count; ui++) {
        //FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
        int x1 = pNode[ui][0], y1 = pNode[ui][1], x2 = pNode[ui][2], y2 = pNode[ui][3];
        //FsPrintf(1, "TTTTTTTTTTTTTTTTTTt(%d,%d)(%d,%d),%u/%u\n", x1, y1, x2, y2, ui, count);
        {
            unsigned int pX2[] = {(unsigned int) x2};
            unsigned int pY2[] = {(unsigned int) y2};
            image_dib_draw_lines_ch1(pYUV420P->data, pFrame->width[0], x1, y1, pX2, pY2, Fs_Array_len(pX2), ycc, 0);
        }
        {
            unsigned int pX2[] = {(unsigned int) x2 / 2};
            unsigned int pY2[] = {(unsigned int) y2 / 2};
            image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0], pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 8) | ((ycc & 0xFF000000U) / 2), 0);
            image_dib_draw_lines_ch1(pYUV420P->data + pFrame->width[0] * pFrame->height[0]+ ((pFrame->width[0] + 1) / 2)* ((pFrame->height[0] + 1) / 2), pFrame->width[0] / 2, x1 / 2, y1 / 2, pX2, pY2, Fs_Array_len(pX2), (ycc >> 16) | ((ycc & 0xFF000000U) / 2), 0);
        }
    }
    fs_Object_delete_pthreadSafety__OI(pYUV420P);
}

/* 叠加文字 */
static void inflrayObject_P_item_add_text_to_frame(struct InflrayObject_item * const pInflrayObject_item, FsObjectImageFrame * const pFrame) {
    if (0 == pInflrayObject_item->p.__textList_->nodeCount)return;
    FsPrintf(1, "%u,%u,count=%lu\n", pFrame->width[0], pFrame->height[0], pInflrayObject_item->p.__textList_->nodeCount);
    /* 先取图像 */
    FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
    if (NULL == pYUV420P) {
        FsLog(FsLogType_error, FsPrintfIndex, "Prapare YUV420P_0 failed.\n");
        FsLogTag(10);
        fflush(stdout);
        return;
    }
    const struct FsTypeFaceText * * const ppNode = (const struct FsTypeFaceText **) pInflrayObject_item->p.__textList_->pNode + pInflrayObject_item->p.__textList_->startIndex;
    unsigned int count = pInflrayObject_item->p.__textList_->nodeCount;
    unsigned int ui = 0;
    for (; ui < count; ui++) {
        const struct FsTypeFaceText * const pTypeFaceText = ppNode[ui];
        fs_TypeFace_write(pTypeFaceText->__pTypeFace, (unsigned char*) pTypeFaceText->text, pFrame->data.yuv420p[0]->data, pFrame->width[0], pFrame->height[0],
                pFrame->width[0], pTypeFaceText->x, pTypeFaceText->y, pTypeFaceText->color & 0xFF);
        fs_TypeFace_write_uv(pTypeFaceText->__pTypeFace, (unsigned char*) pTypeFaceText->text, pFrame->data.yuv420p[0]->data + pFrame->sizeWidthHeight[0], pFrame->width[0], pFrame->height[0],
                pFrame->width[0] >> 1, pTypeFaceText->x, pTypeFaceText->y, (pTypeFaceText->color >> 8)&0xFF);
        fs_TypeFace_write_uv(pTypeFaceText->__pTypeFace, (unsigned char*) pTypeFaceText->text, pFrame->data.yuv420p[0]->data + pFrame->sizeWidthHeight[0]+(pFrame->sizeWidthHeight[0] >> 2), pFrame->width[0], pFrame->height[0],
                pFrame->width[0] >> 1, pTypeFaceText->x, pTypeFaceText->y, (pTypeFaceText->color >> 16)&0xFF);
    }
    fs_Object_delete_pthreadSafety__OI(pYUV420P);
}

static void inflrayObject_P_item_send_detect_debug(struct InflrayObject_item * const pInflrayObject_item, FsObjectImageFrame * const pFrame
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsStructList * const clientList_i8 = pInflrayObject_item->ro.__clientList_i8;
    if (0 == clientList_i8->nodeCount)return;
    FsObjectBase * ppObjectBase0[7];
    FsEbml * pEbml_send = NULL;
    FsObjectBase **ppObjectBase = ppObjectBase0;
    unsigned int ppObjectBaseCount = sizeof (ppObjectBase0) / sizeof (ppObjectBase0[0]);
    unsigned long pos = 0;
    /* 调试数据类型掩码,1,2,4,8,16... */
    unsigned long debugTypeMask = 0;
    while (1) {
        unsigned int debugType;
        pthread_mutex_lock(&clientList_i8->mutex);
        {
            unsigned int (*const ppNode) [8] = ((unsigned int (*)[8])clientList_i8->pNode) + clientList_i8->startIndex;
            for (; pos < clientList_i8->nodeCount; pos++) {
                debugType = ppNode[pos][6];
                if ((debugTypeMask & (1LU << debugType)) == 0) {
                    debugTypeMask |= 1LU << debugType;
                    break;
                }
            }
        }
        if (pos >= clientList_i8->nodeCount) {
            /* 无数据处理了 */
            pthread_mutex_unlock(&clientList_i8->mutex);
            return;
        }
        pthread_mutex_unlock(&clientList_i8->mutex);
        ////////////////////////////////////////////////////////////////////////
        unsigned int ppObjectBaseUseCount;
        unsigned int head, len;
        /* 根据debugType生成数据 */
        switch (debugType) {
                /* 序列化数据 */
            case 0:
            {
                const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
                if (0 == count) {
                    head = 0;
                    break;
                }
                /* 先取图像 */
                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
                if (NULL == pYUV420P) {
                    FsLog(FsLogType_error, FsPrintfIndex, "Prapare YUV420P_0 failed.\n");
                    FsLogTag(10);
                    fflush(stdout);
                    head = 0;
                    break;
                }
                head = 0x02, ppObjectBaseUseCount = 0;
                pEbml_send = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(pEbml_send, (struct FsEbml_node*) pEbml_send, "uuid", FsEbmlNodeType_String), pInflrayObject_item->ro._uuid);
                struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
                unsigned int ui = 0;
                for (; ui < count; ui++) {
                    //  FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
                    struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
                    struct FsEbml_node * const objectItem = fs_Ebml_node_addChild(pEbml_send, (struct FsEbml_node*) pEbml_send, "objectItem", FsEbmlNodeType_Struct);
                    *(long long*) fs_Ebml_node_addChild(pEbml_send, objectItem, "objIndex", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_item_track->objIndex;
                    struct InflrayObject_P_pos_node * pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_item_track->objectTrack_pos_item.head;
                    int x1 = 0xFFFFFFFU, y1 = 0xFFFFFFFU, x2 = 0, y2 = 0;
                    do {
                        // FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
                        struct FsEbml_node * const trackItem = fs_Ebml_node_addChild(pEbml_send, objectItem, "trackItem", FsEbmlNodeType_Struct);
                        *(double*) fs_Ebml_node_addChild(pEbml_send, trackItem, "capture_gmtTime", FsEbmlNodeType_Float)->data.buffer = pInflrayObject_P_pos_node->pos_node.capture_gmtTime;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "frameIndex", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->pos_node.frameIndex;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "x1", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->pos_node.x1;
                        if (x1 > pInflrayObject_P_pos_node->pos_node.x1)x1 = pInflrayObject_P_pos_node->pos_node.x1;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "y1", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->pos_node.y1;
                        if (y1 > pInflrayObject_P_pos_node->pos_node.y1)y1 = pInflrayObject_P_pos_node->pos_node.y1;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "x2", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->pos_node.x2;
                        if (x2 < pInflrayObject_P_pos_node->pos_node.x2)x2 = pInflrayObject_P_pos_node->pos_node.x1;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "y2", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->pos_node.y2;
                        if (y2 < pInflrayObject_P_pos_node->pos_node.y2)y2 = pInflrayObject_P_pos_node->pos_node.y2;
                        fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(pEbml_send, trackItem, "device_name", FsEbmlNodeType_String), pInflrayObject_item->p.device_name);
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml_send, trackItem, "area_name", FsEbmlNodeType_String), strlen(pInflrayObject_P_pos_node->belong_area_name) + 1, pInflrayObject_P_pos_node->belong_area_name);
                        *(double*) fs_Ebml_node_addChild(pEbml_send, trackItem, "target_azimuth", FsEbmlNodeType_Float)->data.buffer = 0;
                        *(double*) fs_Ebml_node_addChild(pEbml_send, trackItem, "target_pitch", FsEbmlNodeType_Float)->data.buffer = 0;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "distance", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->distance;
                        *(double*) fs_Ebml_node_addChild(pEbml_send, trackItem, "longitude", FsEbmlNodeType_Float)->data.buffer = pInflrayObject_P_pos_node->longitude;
                        *(double*) fs_Ebml_node_addChild(pEbml_send, trackItem, "latitude", FsEbmlNodeType_Float)->data.buffer = pInflrayObject_P_pos_node->latitude;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "alarm_type", FsEbmlNodeType_Integer)->data.buffer = 0;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "target_type", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->type;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "is_detected", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->is_detected;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, trackItem, "isDrawing_tracks", FsEbmlNodeType_Integer)->data.buffer = pInflrayObject_P_pos_node->isDrawing_tracks;
                        *(double*) fs_Ebml_node_addChild(pEbml_send, trackItem, "isDrawing_tracks", FsEbmlNodeType_Float)->data.buffer = pInflrayObject_P_pos_node->prob;
                    } while ((pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_pos_node->pos_node.next) != NULL);
                    // FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
                    {
                        struct FsEbml_node * const aisItem = fs_Ebml_node_addChild(pEbml_send, objectItem, "ais", FsEbmlNodeType_Struct);
                        FsString boatName;
                        boatName.buffer = (char*) "boat";
                        boatName.lenth = strlen(boatName.buffer) + 1;
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml_send, aisItem, "boat_name", FsEbmlNodeType_String), boatName.lenth, boatName.buffer);
                        *(double*) fs_Ebml_node_addChild(pEbml_send, aisItem, "boat_longitude", FsEbmlNodeType_Float)->data.buffer = 0;
                        *(double*) fs_Ebml_node_addChild(pEbml_send, aisItem, "boat_latitude", FsEbmlNodeType_Float)->data.buffer = 0;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, aisItem, "boat_type", FsEbmlNodeType_Integer)->data.buffer = 0;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, aisItem, "MMSI", FsEbmlNodeType_Integer)->data.buffer = 0;
                        *(long long*) fs_Ebml_node_addChild(pEbml_send, aisItem, "IMO", FsEbmlNodeType_Integer)->data.buffer = 0;
                        *(double*) fs_Ebml_node_addChild(pEbml_send, aisItem, "boat_speed", FsEbmlNodeType_Float)->data.buffer = 0;
                        FsString desName;
                        desName.buffer = (char*) "desName";
                        desName.lenth = strlen(desName.buffer) + 1;
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml_send, aisItem, "boat_destination", FsEbmlNodeType_String), desName.lenth, desName.buffer);
                    }
                    {
                        x1 -= pInflrayObject_item->p.target_x_extern;
                        if (x1 < 0)x1 = 0;
                        x2 += pInflrayObject_item->p.target_x_extern;
                        if (x2 >= (int) pFrame->width[0])x2 = pFrame->width[0] - 1;
                        y1 -= pInflrayObject_item->p.target_y_extern;
                        if (y1 < 0)y1 = 0;
                        y2 += pInflrayObject_item->p.target_y_extern;
                        if (y2 >= (int) pFrame->height[0])y2 = pFrame->height[0] - 1;
                        x1 = x1 / 4 * 4;
                        x2 = (x2 + 1 + 3) / 4 * 4 - 1;
                        y1 = y1 / 4 * 4;
                        y2 = (y2 + 1 + 3) / 4 * 4 - 1;
                    }
                    *(long long*) fs_Ebml_node_addChild(pEbml_send, objectItem, "x0", FsEbmlNodeType_Integer)->data.buffer = x1;
                    *(long long*) fs_Ebml_node_addChild(pEbml_send, objectItem, "y0", FsEbmlNodeType_Integer)->data.buffer = y1;
                    //FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
                    {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
                        /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 4);
                        /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
                        /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
                        /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
                        /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
                        FsLocal_ShareBuffer_resize((x2 - x1 + 1)* (y2 - y1 + 1) + 1024 * 1024);
                        //                        FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
                        //                        FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u,%p\n", ui, count, pYUV420P->data);
                        //                        FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u,%u\n", ui, count, pFrame->width[0]);
                        //                        FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%d,%d,%d,%d\n", x1, y1, x2, y2);
                        const int jpgSize = image_jpeg_compress((unsigned char*) FsLocal_ShareBuffer + FsLocal_ShareBufferPos, FsLocal_ShareBufferLen - FsLocal_ShareBufferPos
                                , pYUV420P->data + pFrame->width[0] * y1 + x1, x2 - x1 + 1, y2 - y1 + 1, pFrame->width[0], 85, -1, ImageColorType_Gray);
                        //FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u,jpgSize=%d\n", ui, count, jpgSize);
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml_send, objectItem, "jpgdata", FsEbmlNodeType_Binary), jpgSize, FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
                        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
                    }
                    // FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
                }
                fs_Object_delete_pthreadSafety__OI(pYUV420P);
            }
                break;
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
                    FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
                    if (NULL == pYUV420P) {
                        FsLog(FsLogType_error, FsPrintfIndex, "Prapare YUV420P_0 failed.\n");
                        FsLogTag(10);
                        fflush(stdout);
                        head = 0;
                        break;
                    }
                    ppObjectBase[2] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + pFrame->sizeWidthHeight[0], sizeof (FsObjectBasePthreadSafety));
                    fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[2]);
                    /* 把pObjectBase[2]后面的数据加入缓存 */
                    fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[2], sizeof (FsObjectBasePthreadSafety) + pFrame->sizeWidthHeight[0]);
                    ppObjectBase[2]->len = pFrame->sizeWidthHeight[0];
                    memcpy(ppObjectBase[2]->data, pYUV420P->data, pFrame->sizeWidthHeight[0]);
                    fs_Object_delete_pthreadSafety__OI(pYUV420P);
                }
                head = 0x0B, ppObjectBaseUseCount = 4;
                {
                    unsigned char fillLen;
                    len = image_extern_gray_write_size_get(&fillLen, pFrame->sizeWidthHeight[0], sizeof ("tt") - 1);
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
                            , pFrame->width[0], pFrame->height[0], pFrame->width[0], 0, 0, 0, 0, 1.0, 1.0, 8, pFrame->sizeWidthHeight[0]);
                    len += fillLen + ppObjectBase[2]->len;
                }
                ////////////////////////////////////////////////////////////////
                /* 画轨迹 */
                {
                    ppObjectBaseUseCount += 3;
                    /* 初始化ppObjectBase */
                    // 目标框
                    unsigned int *objectData;
                    // 轨迹头
                    unsigned int trailHeadLen;
                    // 轨迹
                    unsigned int *trailData;
                    // 文本信息
                    unsigned char *textData;
                    {
                        /* 存放框 */
                        {
                            const unsigned int lineCount = pInflrayObject_item->p.__objectList_->nodeCount * 4;
                            const unsigned int recLen = image_extern_line_write_size_get(NULL, lineCount, sizeof ("tt") - 1);
                            ppObjectBase[4] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBasePthreadSafety) + recLen, sizeof (FsObjectBasePthreadSafety));
                            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[4]);
                            /* 把pObjectBase[4]后面的数据加入缓存 */
                            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[4], sizeof (FsObjectBasePthreadSafety) + recLen);
                            ppObjectBase[4]->len = recLen;

                            objectData = (unsigned int*) image_extern_line_write_head((unsigned char*) ppObjectBase[4]->data, NULL, 1, sizeof ("tt") - 1, "tt"
                                    , pFrame->width[0], pFrame->height[0], 0x003CFF00, 1.0, 1.0, lineCount);
                        }
                        /* 存放轨迹 */
                        {
                            unsigned char fillLen;
                            const unsigned int lineCount = pInflrayObject_item->p.__objectList_->nodeCount*InflrayObject_trail_count;
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
                                    + (headSize + 32/* 文本内容大小 */ + 64) * pInflrayObject_item->p.__objectList_->nodeCount, sizeof (FsObjectBasePthreadSafety));
                            fs_ObjectBasePthreadSafety_init_from_objectBase(ppObjectBase[6]);
                            /* 把pObjectBase[6]后面的数据加入缓存 */
                            fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[6], sizeof (FsObjectBasePthreadSafety)
                                    + (headSize + 32/* 文本内容大小 */ + 64) * pInflrayObject_item->p.__objectList_->nodeCount);

                            textData = (unsigned char*) ppObjectBase[6]->data;
                        }
                    }
                    {
                        struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
                        const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
                        unsigned int ui = 0;
                        for (; ui < count; ui++) {
                            struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
                            int x1, y1, x2, y2;
                            {
                                struct ObjectTrack_pos_node * const pObjectTrack_pos_node = pInflrayObject_P_item_track->objectTrack_pos_item.tail;
                                x1 = pObjectTrack_pos_node->x1, y1 = pObjectTrack_pos_node->y1, x2 = pObjectTrack_pos_node->x2, y2 = pObjectTrack_pos_node->y2;
                            }
                            /* 目标框 */
                            {
                                // (x1,y1)(x2,y1)
                                objectData[0] = x1, objectData[1] = y1, objectData[2] = x2, objectData[3] = y1, objectData += 4;
                                // (x2,y1)(x2,y2)
                                objectData[0] = x2, objectData[1] = y1, objectData[2] = x2, objectData[3] = y2, objectData += 4;
                                // (x2,y2)(x1,y2)
                                objectData[0] = x2, objectData[1] = y2, objectData[2] = x1, objectData[3] = y2, objectData += 4;
                                // (x1,y2)(x1,y1)
                                objectData[0] = x1, objectData[1] = y2, objectData[2] = x1, objectData[3] = y1, objectData += 4;
                            }
                            /* 文本信息 */
                            {
                                char text[32];
                                struct InflrayObject_P_pos_node * const pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_item_track->objectTrack_pos_item.tail;
                                int textLen = snprintf(text, sizeof (text), "OID:%llu,t:%hhd,d=%hhd,tr=%hhd", pInflrayObject_P_item_track->objIndex
                                        , pInflrayObject_P_pos_node->type, pInflrayObject_P_pos_node->is_detected, pInflrayObject_P_pos_node->isDrawing_tracks);
                                unsigned char * const pd = image_extern_text_write_head(textData, NULL, 1, sizeof ("tt") - 1, "tt"
                                        , pFrame->width[0], pFrame->height[0], 0x0000FFFF, x1, y1, 32, 32, textLen);
                                memcpy(pd, text, textLen);
                                textData += image_extern_text_write_size_get(NULL, textLen, sizeof ("tt") - 1);
                            }
                            /* 轨迹 */
                            if (pInflrayObject_P_item_track->objectTrack_pos_item.posCount > 1) {
                                struct ObjectTrack_pos_node * pObjectTrack_pos_node = pInflrayObject_P_item_track->objectTrack_pos_item.head;
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
                        /* 目标框 */
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
        pthread_mutex_lock(&clientList_i8->mutex);
        {
            unsigned int (*const ppNode) [8] = ((unsigned int (*)[8])clientList_i8->pNode) + clientList_i8->startIndex;
            unsigned long uli;
            FsObjectBasePthreadSafety * pData_ebml = NULL;
            FsObjectBasePthreadSafety * pData_xml = NULL;
            FsObjectBasePthreadSafety * pData_json = NULL;
            for (uli = pos; uli < clientList_i8->nodeCount; uli++) {
                if (debugType == ppNode[uli][6]) {
                    if (0 == debugType) {
                        configManager_conncet_refer_sendData(&pData_ebml, &pData_xml, &pData_json, pEbml_send
                                , ConfigManager_refer_connect_node_get_requestID_3(ppNode[uli]), ConfigManager_refer_connect_node_get_headType(ppNode[uli])
                                , ConfigManager_refer_connect_node_get_checkMethod(ppNode[uli]), ConfigManager_refer_connect_node_get_virtualConnection(ppNode[uli])
                                , ConfigManager_refer_connect_node_get_head(ppNode[uli]) | 0x2, ConfigManager_refer_connect_node_get_requestDataType(ppNode[uli])
                                , pObjectBaseBuffer, pShareBuffer);
                    } else {
                        ppObjectBase[0] = fs_ObjectBaseBuffer_get__IO(pObjectBaseBuffer, 0, sizeof (FsObjectBase) + 8, sizeof (FsObjectBase));
                        /* 把pObjectBase[0]后面的数据加入缓存 */
                        fs_ObjectBaseBuffer_lastNode_add(pObjectBaseBuffer, ppObjectBase[0], sizeof (FsObjectBase) + 8);
                        ppObjectBase[0]->len = 8;
                        ////////////////////////////////////////////////////////////
                        ((unsigned int*) ppObjectBase[0]->data)[0] = (ppNode[uli][3]&0xFFFFFFF0U) | head, ((unsigned int*) ppObjectBase[0]->data)[1] = len;
                        unsigned int j;
                        for (j = 1; j < ppObjectBaseUseCount; j++) fs_ObjectBasePthreadSafety_addRefer(ppObjectBase[j]);
                        configManager_connect_refer_send__OI_3(ppNode[uli], ppObjectBaseUseCount, ppObjectBase);
                    }
                }
            }
            if (pData_ebml)pData_ebml->_delete(pData_ebml);
            if (pData_xml)pData_xml->_delete(pData_xml);
            if (pData_json)pData_json->_delete(pData_json);
        }
        pthread_mutex_unlock(&clientList_i8->mutex);
        {
            unsigned int j;
            for (j = 1; j < ppObjectBaseUseCount; j++) fs_ObjectBasePthreadSafety_delete__OI(ppObjectBase[j]);
        }
        if (pEbml_send)fs_Ebml_delete__OI(pEbml_send, pShareBuffer);
    }
    if (ppObjectBase != ppObjectBase0)fsFree(ppObjectBase);
}

/* 检测,成功返回1,失败返回-1 */
static inline int inflrayObject_P_item_do_detect(/* 检测项 */struct InflrayObject_item * const pInflrayObject_item, FsObjectImageFrame * const pFrame, unsigned int *const pObjIndex
        , FsObjectBaseBuffer * const pObjectBaseBuffer
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName inflrayObject_P_item_do_detect
    FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
    if (NULL == pYUV420P) {
        FsLog(FsLogType_error, FsPrintfIndex, "prapare YUV420P failed.\n");
        fflush(stdout);
        return -1;
    }

#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 4);
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var pos_data_in
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    /* 是多可输出的目标数量 */
    struct DetectAlgorithmLib_ConventionalDetect_pos_data *pos_data_in = (struct DetectAlgorithmLib_ConventionalDetect_pos_data *) (FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
    int checkCount = 0;
    {
        unsigned int conventionalDetectCount = pInflrayObject_item->p.__pConventionalDetectList_->nodeCount;
        FsPrintf(1, "index=%u,capture_gmtTime=%lf,size=%u/%u,conventionalDetectCount=%u\n", pFrame->index, pFrame->capture_gmtTime, pFrame->width[0], pFrame->height[0], conventionalDetectCount);
        void* * const ppNode_ConventionalDetect = pInflrayObject_item->p.__pConventionalDetectList_->pNode + pInflrayObject_item->p.__pConventionalDetectList_->startIndex;
        while (conventionalDetectCount-- > 0) {
            void *const pConventionalDetect = ppNode_ConventionalDetect[conventionalDetectCount];
            unsigned int targetCount = 64;
            FsLocal_ShareBuffer_resize(sizeof (struct DetectAlgorithmLib_ConventionalDetect_pos_data)* (checkCount + targetCount));
            targetCount = (FsLocal_ShareBufferLen - FsLocal_ShareBufferPos) / sizeof (struct DetectAlgorithmLib_ConventionalDetect_pos_data)-checkCount;
            double t1 = fs_time_uptime_get();
            //            {
            //                FsPrintf(1, "11111,pInflrayObject_item->p.__objectList_->nodeCount=%lu,FsLocal_ShareBufferLen=%lu\n", pInflrayObject_item->p.__objectList_->nodeCount,FsLocal_ShareBufferLen);
            //                const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
            //                struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
            //                unsigned int ui = 0;
            //                for (; ui < count; ui++) {
            //                    //  FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
            //                    struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
            //                    printf("[%u/%u]:%p,objIndex=%llu/%u,head=%p/%p/%p,frameIndex=%u,capture_gmtTime=%lf,(%d,%d)(%d,%d)\n", ui, count, pInflrayObject_P_item_track, pInflrayObject_P_item_track->objIndex, pInflrayObject_P_item_track->id
            //                            , pInflrayObject_P_item_track->objectTrack_pos_item.head, pInflrayObject_P_item_track->objectTrack_pos_item.head->next, pInflrayObject_P_item_track->objectTrack_pos_item.tail
            //                            , pInflrayObject_P_item_track->objectTrack_pos_item.head->frameIndex, pInflrayObject_P_item_track->objectTrack_pos_item.head->capture_gmtTime
            //                            , pInflrayObject_P_item_track->objectTrack_pos_item.head->x1, pInflrayObject_P_item_track->objectTrack_pos_item.head->y1, pInflrayObject_P_item_track->objectTrack_pos_item.head->x2, pInflrayObject_P_item_track->objectTrack_pos_item.head->y2);
            //                }
            //            }
            int thisCheckCount = detectAlgorithmLib_ConventionalDetect_detect(pConventionalDetect, targetCount, pos_data_in + checkCount, pYUV420P->data, pFrame->width[0]);
            //            {
            //                FsPrintf(1, "2222222222,pInflrayObject_item->p.__objectList_->nodeCount=%lu\n", pInflrayObject_item->p.__objectList_->nodeCount);
            //                const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
            //                struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
            //                unsigned int ui = 0;
            //                for (; ui < count; ui++) {
            //                    //  FsPrintf(1, "TTTTTTTTTTTTTTTTTTt,%u/%u\n", ui, count);
            //                    struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
            //                    printf("[%u/%u]:%p,objIndex=%llu/%u,head=%p/%p/%p,frameIndex=%u,capture_gmtTime=%lf,(%d,%d)(%d,%d)\n", ui, count, pInflrayObject_P_item_track, pInflrayObject_P_item_track->objIndex, pInflrayObject_P_item_track->id
            //                            , pInflrayObject_P_item_track->objectTrack_pos_item.head, pInflrayObject_P_item_track->objectTrack_pos_item.head->next, pInflrayObject_P_item_track->objectTrack_pos_item.tail
            //                            , pInflrayObject_P_item_track->objectTrack_pos_item.head->frameIndex, pInflrayObject_P_item_track->objectTrack_pos_item.head->capture_gmtTime
            //                            , pInflrayObject_P_item_track->objectTrack_pos_item.head->x1, pInflrayObject_P_item_track->objectTrack_pos_item.head->y1, pInflrayObject_P_item_track->objectTrack_pos_item.head->x2, pInflrayObject_P_item_track->objectTrack_pos_item.head->y2);
            //                }
            //            }
            //            fs_ObjectList_clear_custom(pInflrayObject_item->p.__objectList_, (void (* const) (void * const))inflrayObject_P_item_track_delete__OI);
            if (thisCheckCount >= (int) targetCount) {
                FsLocal_ShareBuffer_resize(sizeof (struct DetectAlgorithmLib_ConventionalDetect_pos_data)* (checkCount + targetCount + 2));
            }
            while (thisCheckCount-- > 0) {
                pos_data_in[checkCount].id = pos_data_in[checkCount].id * pInflrayObject_item->p.__pConventionalDetectList_->nodeCount + conventionalDetectCount;
                checkCount++;
            }
            FsPrintf(1, "index=%u,capture_gmtTime=%lf,useTime=%lf,checkCount=%d/%d,conventionalDetectIndex=%u\n", pFrame->index, pFrame->capture_gmtTime, fs_time_uptime_get() - t1, checkCount, thisCheckCount, conventionalDetectCount);
        }

        fs_Object_delete_pthreadSafety__OI(pYUV420P);
    }

    //        {
    //            unsigned int uj = 0;
    //            for (; uj < checkCount; uj++) {
    //                FsPrintf(1, "pos_data_in[%u].pos_data.tag=%d,(%d,%d)(%d,%d)\n", uj, pos_data_in[uj].pos_data.tag,pos_data_in[uj].pos_data.x1,pos_data_in[uj].pos_data.y1,pos_data_in[uj].pos_data.x2,pos_data_in[uj].pos_data.y2);
    //            }
    //        }
    /* 目标跟踪 */
    {

        FsObjectList * const objectList_ = pInflrayObject_item->p.__objectList_;
        const unsigned int frameIndex = pFrame->index;
        const unsigned int matchCount = objectTrack_pos_update_by_id(objectList_, (int (* const) (struct ObjectTrack_pos_data *))detectAlgorithmLib_ConventionalDetect_pos_data_get_id, (int (* const) (struct ObjectTrack_pos_item *)) inflrayObject_P_item_track_get_id
                , pFrame->capture_gmtTime, frameIndex, 0, sizeof (struct InflrayObject_P_pos_node), (void (* const) (struct ObjectTrack_pos_node *, struct ObjectTrack_pos_data *))inflrayObject_P_pos_node_init_extern, NULL
                , (struct ObjectTrack_pos_data*) pos_data_in, sizeof ( struct DetectAlgorithmLib_ConventionalDetect_pos_data), checkCount, InflrayObject_trail_count);
        FsPrintf(1, "matchCount=%u,objectList_->nodeCount=%lu\n", matchCount, objectList_->nodeCount);
        if (matchCount < (unsigned int) checkCount) {
            /* 有框没匹配,把没匹配的框创建新的跟踪目标 */
            unsigned long long objIndex = time(NULL)*1000;
            unsigned int uj = 0;
            for (; uj < (unsigned int) checkCount; uj++) {
                // FsPrintf(1, "pos_data_in[%u].pos_data.tag=%d\n", uj, pos_data_in[uj].pos_data.tag);
                if (pos_data_in[uj].pos_data.tag) {
                    // FsMacrosFunctionTag(new_obj_skip) :;
                    continue;
                }
                struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = (struct InflrayObject_P_item_track *) fsMalloc(sizeof ( struct InflrayObject_P_item_track));
                objectTrack_pos_item_init_by_pos_data((struct ObjectTrack_pos_item *) pInflrayObject_P_item_track, pFrame->capture_gmtTime, frameIndex, (void (* const) (struct ObjectTrack_pos_node *, struct ObjectTrack_pos_data *))inflrayObject_P_pos_node_init_extern
                        , (struct ObjectTrack_pos_data *) (pos_data_in + uj), sizeof (struct InflrayObject_P_pos_node));
                /* 目标编号,使用时间*1000+随机码表示 */
                pInflrayObject_P_item_track->objIndex = objIndex + (*pObjIndex) % 1000;
                if (FsNumberIsMax(*pObjIndex))*pObjIndex = 0;
                else (*pObjIndex)++;
                /* 原始目标id用于匹配 */
                pInflrayObject_P_item_track->id = pos_data_in[uj].id;

                fs_ObjectList_insert_tail(objectList_, pInflrayObject_P_item_track);
            }
        }
    }
    FsPrintf(1, "index=%u,capture_gmtTime=%lf,objCount=%lu\n", pFrame->index, pFrame->capture_gmtTime, pInflrayObject_item->p.__objectList_->nodeCount);

    {
        const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
        struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
        unsigned int ui = 0;
        for (; ui < count; ui++) {
            struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
            struct InflrayObject_P_pos_node * pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_item_track->objectTrack_pos_item.head;
            // 计算目标距离
            if (pInflrayObject_item->p.target_distance_enable) {
                // 目标框宽度
                int iPosRectW = pInflrayObject_P_pos_node->pos_node.x2 - pInflrayObject_P_pos_node->pos_node.x1;
                // 目标框底部位置
                double dPoint_x = pInflrayObject_P_pos_node->pos_node.x1 + (double) iPosRectW / 2.0;
                double dPoint_y = pInflrayObject_P_pos_node->pos_node.y2;
                // 计算
                double dV1_x = pInflrayObject_item->p.end_x - pInflrayObject_item->p.start_x;
                double dV1_y = pInflrayObject_item->p.end_y - pInflrayObject_item->p.start_y;
                double dV2_x = dPoint_x - pInflrayObject_item->p.start_x;
                double dV2_y = dPoint_y - pInflrayObject_item->p.start_y;

                double tmp = dV1_x * dV2_y - dV1_y * dV2_x;
                int vertical = fabs(tmp / sqrt(dV1_x * dV1_x + dV1_y * dV1_y));
                double angle = (double) vertical / pFrame->height[0] * pInflrayObject_item->p.device_field_angle;
                int distance = pInflrayObject_item->p.device_height / tan(angle) / 180.0 * PI;
                // 目标方向角
                double direction = dPoint_x / pFrame->width[0] * 360.0;

                // 计算目标经纬度
                pInflrayObject_P_pos_node->longitude = pInflrayObject_item->p.device_longitude + distance * sin(direction / 180.0 * PI) / cos(pInflrayObject_item->p.device_longitude / 180.0 * PI) / 63713930.0 / 2.0 / PI * 360.0;
                pInflrayObject_P_pos_node->latitude = pInflrayObject_item->p.device_latitude + distance * cos(direction / 180.0 * PI) / 63713930.0 / 2.0 / PI * 360.0;

                // 距离异常
                if (distance < 0) {
                    distance = 6000;
                    pInflrayObject_P_pos_node->longitude = 0;
                    pInflrayObject_P_pos_node->latitude = 0;
                } else if (distance > 6000) {
                    distance = 6000;
                    pInflrayObject_P_pos_node->longitude = 0;
                    pInflrayObject_P_pos_node->latitude = 0;
                }
                pInflrayObject_P_pos_node->distance = distance;
            } else {
                pInflrayObject_P_pos_node->distance = 0;
                pInflrayObject_P_pos_node->longitude = 0;
                pInflrayObject_P_pos_node->latitude = 0;
            }
            // 获取报警目标所属区域
            if (pInflrayObject_item->p.__areaNameList->nodeCount > 0) {
                const struct InflrayObject_P_area * * const ppNode = (const struct InflrayObject_P_area **) pInflrayObject_item->p.__areaNameList->pNode + pInflrayObject_item->p.__areaNameList->startIndex;
                const unsigned int count = pInflrayObject_item->p.__areaNameList->nodeCount;
                unsigned int ui = 0;
                // 目标中心坐标
                int target_x = (pInflrayObject_P_pos_node->pos_node.x2 - pInflrayObject_P_pos_node->pos_node.x1) / 2 + pInflrayObject_P_pos_node->pos_node.x1;
                int target_y = (pInflrayObject_P_pos_node->pos_node.y2 - pInflrayObject_P_pos_node->pos_node.y1) / 2 + pInflrayObject_P_pos_node->pos_node.y1;
                // 遍历区域
                for (; ui < count; ui++) {
                    const struct InflrayObject_P_area * const pInflrayObject_P_area = ppNode[ui];
                    //FsPrintf(1, "point count=%d\n", ui);
                    struct Fs_Points * const pPoint = (struct Fs_Points *) pInflrayObject_P_area->pPoint;
                    unsigned int iPtCount = pPoint->count;
                    int (*thePoint)[2] = pPoint->point;
                    unsigned int uj = 0, flag = 0;
                    for (; uj < iPtCount; uj++) {
                        // 多边形的边的两个点 p1, p2
                        int x1 = thePoint[0][0], y1 = thePoint[0][1];
                        thePoint++;
                        int x2, y2;
                        if (uj == iPtCount - 1) {
                            x2 = pPoint->point[0][0], y2 = pPoint->point[0][1];
                        } else {
                            x2 = thePoint[0][0], y2 = thePoint[0][1];
                        }

                        if ((target_y >= y1 && target_y < y2) || (target_y >= y2 && target_y < y1)) {
                            float tmp_x = (target_y - y1) * (x2 - x1) / (y2 - y1) + x1;
                            if (tmp_x > target_x) {
                                flag++;
                            }
                        }
                    }
                    if (flag % 2) {
                        const int iLen = strlen(pInflrayObject_P_area->area_name) + 1;
                        memcpy(pInflrayObject_P_pos_node->belong_area_name, pInflrayObject_P_area->area_name, iLen);
                        break;
                    }
                }
            }
        }
    }

    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    /* 叠加目标框 */
    inflrayObject_P_item_add_target_to_frame(pInflrayObject_item, pFrame);
    /* 叠加区域框 */
    inflrayObject_P_item_add_area_to_frame(pInflrayObject_item, pFrame);
    /* 叠加屏蔽区域框 */
    inflrayObject_P_item_add_excludeArea_to_frame(pInflrayObject_item, pFrame);
    /* 叠加文字 */
    inflrayObject_P_item_add_text_to_frame(pInflrayObject_item, pFrame);
    /* 发送调试数据到客户端 */
    inflrayObject_P_item_send_detect_debug(pInflrayObject_item, pFrame, pObjectBaseBuffer, pShareBuffer);

    return 1;
}

static void *inflrayObject_P_T(struct InflrayObject * const pInflrayObject) {
    ;
#undef FsFunctionName
#define FsFunctionName inflrayObject_P_T
    /* 创建线程监控 */
    void *pMonitor = pInflrayObject->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, InflrayObject_get_threadName(pInflrayObject), NULL, pInflrayObject, pInflrayObject->ro._pMonitor != NULL ? pInflrayObject->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pInflrayObject->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pInflrayObject->ro._pConfigManager;
    FsObjectList * const itemList_ = pInflrayObject->ro.__itemList_;
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
#define __inflrayObject_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __inflrayObject_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __inflrayObject_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);  
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
#define __inflrayObject_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __inflrayObject_P_T_clean1_count 4
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    configManager_protocol_publish(pConfigManager, "inflrayObject_debug", "InflrayObject检测调试", 1, inflrayObject_P_protocol_debug);
#define __inflrayObject_P_T_clean2_1 {configManager_protocol_cancel(pConfigManager, "inflrayObject_debug");}
#define __inflrayObject_P_T_clean2_count 1
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pInflrayObject->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        usleep(1000);
        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pInflrayObject->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (0 == pInflrayObject->rw._resetChannel) channelCount = inflrayObject_P_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server InflrayObject
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
                struct InflrayObject_item * const pInflrayObject_item = (struct InflrayObject_item *) fs_ObjectList_remove_tail(itemList_);
                if (pInflrayObject_item)inflrayObject_P_item_delete__OI(pInflrayObject_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pInflrayObject->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pInflrayObject->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pInflrayObject->rw._resetChannel > 0) {
                FsPrintf(1, "%d\n", pInflrayObject->rw._resetChannel);
                pMonitorItem->rw.line = __LINE__;
                inflrayObject_P_item_new(pInflrayObject, pInflrayObject->rw._resetChannel, pConfigManager->ro.__ipList_local
                        , (struct InflrayObject_item **) (itemList_->pNode + itemList_->startIndex + pInflrayObject->rw._resetChannel - 1), &groupSqrt
                        , & shareBuffer);
                pMonitorItem->rw.line = __LINE__;
            }
            pInflrayObject->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct InflrayObject_item * * const ppNode_itemList = (struct InflrayObject_item **) itemList_->pNode + itemList_->startIndex;
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
                struct InflrayObject_item * const pInflrayObject_item = ppNode_itemList[ui];
                if (NULL == pInflrayObject_item)continue;
                //FsPrintf(1, "pInflrayObject_item->ro.__framelistIn->nodeCount=%lu\n", pInflrayObject_item->ro.__framelistIn->nodeCount);
                ////////////////////////////////////////////////////////////////
                //////////////////////// 状态机 开始 ////////////////////////////
#define __inflrayObject_P_T_state_check_end (1U<<0) // 检查缓存是否足够,不够退出循环
#define __inflrayObject_P_T_state_check_module_init (1U<<1) // 检查模块初始化
#define __inflrayObject_P_T_state_check_do (1U<<2) // 执行检测
#define __inflrayObject_P_T_state_out_frame (1U<<3) // 输出数据帧
#define __inflrayObject_P_T_state_end_break (1U<<4) // 退出循环
#define __inflrayObject_P_T_state_reset (1U<<5) // 重置
#ifndef __inflrayObject_P_T_state
                unsigned int state;
                //state = FsMacrosFunction(state_check_end);
                FsMacrosSetState_OR_GotoFunctionTag(state, state_check_end);
                for (;;) {
                    FsObjectList *pConventionalDetectList_;
                    FsObjectImageFrame * pFrame;
                    /* 检查缓存是否足够 */
                    if (state & FsMacrosFunction(state_check_end)) {
                        state ^= FsMacrosFunction(state_check_end);
                        FsMacrosFunctionTag(state_check_end) :;
                        //FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end\n");
                        ////////////////////////////////////////////////
                        if (((int) pInflrayObject_item->ro.__framelistIn->nodeCount) >= frameBufferCount) {
                            /* 检查数据 */
                            //FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end\n");
                            pthread_mutex_lock(&pInflrayObject_item->ro.__framelistIn->mutex);
                            pFrame = (FsObjectImageFrame*) pInflrayObject_item->ro.__framelistIn->pNode[ pInflrayObject_item->ro.__framelistIn->startIndex + frameBufferCount - 1];
                            pthread_mutex_unlock(&pInflrayObject_item->ro.__framelistIn->mutex);
                            // FsPrintf(1, "TTTTTTTTTTTTTTT state_check_end,classIndex=%u/%u,index=%u\n", pInflrayObject_item->ro.classIndex, pFrame->classIndex, pFrame->index);
                            if (pInflrayObject_item->ro.classIndex != pFrame->classIndex) {
                                pInflrayObject_item->ro.classIndex = pFrame->classIndex;
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
                        if (NULL == (pConventionalDetectList_ = pInflrayObject_item->p.__pConventionalDetectList_)) {
                            pthread_mutex_lock(&pInflrayObject_item->ro.__framelistIn->mutex);
                            int i = 0;
                            for (; i < frameBufferCount; i++) {
                                pFrame = (FsObjectImageFrame*) pInflrayObject_item->ro.__framelistIn->pNode[ pInflrayObject_item->ro.__framelistIn->startIndex + i];
                                if (pInflrayObject_item->ro.classIndex != pFrame->classIndex)break;
                            }
                            pthread_mutex_unlock(&pInflrayObject_item->ro.__framelistIn->mutex);
                            if (i != frameBufferCount) {
                                /* 重置 */
                                FsMacrosSetState_OR_GotoFunctionTag(state, state_reset);
                            } else {
                                pthread_mutex_lock(&pInflrayObject_item->ro.__framelistIn->mutex);
                                pFrame = (FsObjectImageFrame*) pInflrayObject_item->ro.__framelistIn->pNode[ pInflrayObject_item->ro.__framelistIn->startIndex + frameBufferCount - 1];
                                pthread_mutex_unlock(&pInflrayObject_item->ro.__framelistIn->mutex);
                                FsPrintf(1, "TTTTTTTTTTTTTTTinflrayObject_P_item_new_conventionalDetect__IO,index=%u,classIndex=%u\n", pFrame->index, pFrame->classIndex);
                                pConventionalDetectList_ = pInflrayObject_item->p.__pConventionalDetectList_ = inflrayObject_P_item_new_conventionalDetect__IO(pInflrayObject_item, ui, pFrame->width[0], pFrame->height[0], pFrame->width[0], pFrame->height[0], &shareBuffer);
                                if (NULL == pConventionalDetectList_) {
                                    /* 重置 */
                                    FsMacrosSetState_OR_GotoFunctionTag(state, state_reset);
                                }
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
                        int frameInterval = 1;
                        //********************************************************//
                        /* 检测模块 */
                        // if (frameIterval < pCarDetect->p._frameInterval)frameIterval = pCarDetect->p._frameInterval;
                        if (frameBufferCount < frameInterval) {
                            frameBufferCount = frameInterval;
                            FsMacrosSetStates_OR_GotoFunctionTag(state, state_reset, FsMacrosFunction(state_check_end));
                        } else {
                            //FsPrintf(1, "TTTTTTTTTTTTTTT,index=%u\n", pFrame->index);
                            if (0 == (pFrame->index % frameInterval) && inflrayObject_P_item_do_detect(pInflrayObject_item, pFrame, &objIndex, &baseBuffer, &shareBuffer) != 1) {
                                FsLog(FsLogType_error, FsPrintfIndex, "%u/%lu:do_detect failed.\n", ui, itemList_->nodeCount);
                            } else {
                                // 插入联动任务
                                const unsigned int count = pInflrayObject_item->p.__objectList_->nodeCount;
                                //FsPrintf(1, "pInflrayObject_item->p.__objectList_->nodeCount=%d\n", count);
                                struct InflrayObject_P_item_track * * const ppNode = (struct InflrayObject_P_item_track * *) pInflrayObject_item->p.__objectList_->pNode + pInflrayObject_item->p.__objectList_->startIndex;
                                unsigned int ui = 0;
                                int res = 0;
                                for (; ui < count; ui++) {
                                    struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[ui];
                                    // FsPrintf(1, "pInflrayObject_item->p.objIndex=%llu, pInflrayObject_P_item_track->objIndex=%llu\n", pInflrayObject_item->p.objIndex, pInflrayObject_P_item_track->objIndex);
                                    if (pInflrayObject_item->p.objIndex == 0) {
                                        pInflrayObject_item->p.objIndex = pInflrayObject_P_item_track->objIndex;
                                        struct InflrayObject_P_pos_node * pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_item_track->objectTrack_pos_item.head;
                                        pthread_mutex_lock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                        if (pInflrayObject_item->p.ptz_item_linkPtz) {
                                            FsPrintf(1, "pInflrayObject_item->p.ptz_item_linkPtz_1 \n");
                                            pInflrayObject_item->p.ptz_item_linkPtz(pInflrayObject_item->p.pPtzObject_item, pInflrayObject_P_pos_node->pos_node.x1, pInflrayObject_P_pos_node->pos_node.y1, pFrame->width[0], pFrame->height[0]);
                                            res = 1;
                                            pthread_mutex_unlock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                            break;
                                        }
                                        pthread_mutex_unlock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                    }

                                    if (pInflrayObject_item->p.objIndex == pInflrayObject_P_item_track->objIndex) {
                                        struct InflrayObject_P_pos_node * pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_item_track->objectTrack_pos_item.head;
                                        pthread_mutex_lock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                        if (pInflrayObject_item->p.ptz_item_linkPtz) {
                                            FsPrintf(1, "pInflrayObject_item->p.ptz_item_linkPtz_2 \n");
                                            pInflrayObject_item->p.ptz_item_linkPtz(pInflrayObject_item->p.pPtzObject_item, pInflrayObject_P_pos_node->pos_node.x1, pInflrayObject_P_pos_node->pos_node.y1, pFrame->width[0], pFrame->height[0]);
                                            res = 1;
                                            pthread_mutex_unlock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                            break;
                                        }
                                        pthread_mutex_unlock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                    }
                                }

                                if ((count > 0) && (res == 0)) {
                                    //FsPrintf(1, "pInflrayObject_item->p.ptz_item_linkPtz_3 \n");
                                    struct InflrayObject_P_item_track * const pInflrayObject_P_item_track = ppNode[0];
                                    pInflrayObject_item->p.objIndex = pInflrayObject_P_item_track->objIndex;
                                    struct InflrayObject_P_pos_node * pInflrayObject_P_pos_node = (struct InflrayObject_P_pos_node*) pInflrayObject_P_item_track->objectTrack_pos_item.head;
                                    pthread_mutex_lock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                    if (pInflrayObject_item->p.ptz_item_linkPtz) {
                                        FsPrintf(1, "pInflrayObject_item->p.ptz_item_linkPtz_4 \n");
                                        pInflrayObject_item->p.ptz_item_linkPtz(pInflrayObject_item->p.pPtzObject_item, pInflrayObject_P_pos_node->pos_node.x1, pInflrayObject_P_pos_node->pos_node.y1, pFrame->width[0], pFrame->height[0]);
                                        res = 1;
                                    }
                                    pthread_mutex_unlock(&pInflrayObject_item->ro.__mutexCmdConnect);
                                }
                            }
                            //                            if (0 == (pFrame->index % frameInterval)) {
                            //                                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
                            //                                if (pYUV420P != NULL) {
                            //                                  //  FsPrintf(1, "TTTTTTTTTTTTTTT,index=%u\n", pFrame->index);
                            //                                    detectAlgorithmLib_ConventionalDetect_detect(pConventionalDetect, pYUV420P->data, pFrame->width[0]);
                            //                                    fs_Object_delete_pthreadSafety__OI(pYUV420P);
                            //                                    /* 发送调试数据到客户端 */
                            //                                    // targetCheck_P_item_send_carDetect_debug(pTargetCheck_item, pFrame, &baseBuffer);
                            //                                } else {
                            //                                    FsLog(FsLogType_error, FsPrintfIndex, "%u/%lu:YUV420P (=%p) is NULL.\n", ui, itemList_->nodeCount, pYUV420P);
                            //                                    fflush(stdout);
                            //                                }
                            //                            }
                        }
                        //********************************************************//
                        if (0 == state)break;
                    }
                    /* 输出数据帧 */
                    if (state & FsMacrosFunction(state_out_frame)) {
                        state ^= FsMacrosFunction(state_out_frame);
                        //FsMacrosFunctionTag(state_out_frame) :;
                        ////////////////////////////////////////////////////////////
#ifndef __inflrayObject_P_T_outFrame 
                        FsObjectImageFrame * const pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pInflrayObject_item->ro.__framelistIn);
                        pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                        fs_ObjectList_insert_tail_pthreadSafety(pInflrayObject_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pInflrayObject_item->rw._pGroupSqrtOut, &pInflrayObject_item->ro._pInflrayObject->ro._pGroupSqrtOut->groupSqrt_mutex);
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
                        int rv = pInflrayObject_item->ro.__framelistIn->nodeCount;
                        if (rv > frameBufferCount)rv = frameBufferCount;
                        while (rv-- > 0) {
#ifndef __inflrayObject_P_T_outFrame 
                            FsObjectImageFrame * const pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pInflrayObject_item->ro.__framelistIn);
                            pFrame->stats->decodeMask_set |= ImageFrame_YUV420P_0;
                            fs_ObjectList_insert_tail_pthreadSafety(pInflrayObject_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pInflrayObject_item->rw._pGroupSqrtOut, &pInflrayObject_item->ro._pInflrayObject->ro._pGroupSqrtOut->groupSqrt_mutex);
#endif
                        }
                        if (pInflrayObject_item->p.__pConventionalDetectList_) {
                            fs_ObjectList_delete_unsafe_custom__OI(pInflrayObject_item->p.__pConventionalDetectList_, detectAlgorithmLib_ConventionalDetect_delete__OI);
                            pInflrayObject_item->p.__pConventionalDetectList_ = NULL;
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
    FsPrintf(1, "TTTTTTTTTTTTT\n");
    if (pInflrayObject->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pInflrayObject->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    FsPrintf(1, "TTTTTTTTTTTTT\n");
    /* 删除各通道 */
    {
        unsigned int ui = itemList_->nodeCount;
        struct InflrayObject_item ** ppInflrayObject_itemNode = (struct InflrayObject_item **) itemList_->pNode + itemList_->startIndex;
        for (; ui > 0; ui--) {
            struct InflrayObject_item * const pInflrayObject_item = *ppInflrayObject_itemNode++;
            if (pInflrayObject_item)inflrayObject_P_item_delete__OI(pInflrayObject_item, &shareBuffer);
        }
        fs_ObjectList_clear(itemList_);
        //        /* 删除集群 */
        //        if (pCapture->p.clusterItemList) {
        //            fs_ObjectList_delete_custom__OI(pCapture->p.clusterItemList, (void (*)(void*))capture_P_item_delete__OI);
        //            pCapture->p.clusterItemList = NULL;
        //        }
    }
    FsPrintf(1, "TTTTTTTTTTTTT\n");
    /* 释放中间变量空间 */
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", InflrayObject_get_threadName(pInflrayObject), pInflrayObject->rw.runStatus);
#endif 
    FsPrintf(1, "TTTTTTTTTTTTT\n");
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pInflrayObject->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void inflrayObject_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "inflrayObjectConfig", "Inflray目标检测配置", "Inflray目标检测配置", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "64");
    fs_Config_node_add_property_area(pConfig, parent, NULL, "targetCheckErea", NULL, NULL, NULL, "area", 0x000000FF);
    fs_Config_node_add_property_area(pConfig, parent, "屏蔽区域", NULL, NULL, NULL, NULL, "excludeArea", 0x0000FF00);
    fs_Config_node_add_property_area(pConfig, parent, "测试区域", "tt1", "tt2", "tt3", "tt4", "area", 0x00FF0000);
    void *tt = fs_Config_node_add_property_area(pConfig, parent, "测试区域1", "tt1", NULL, NULL, NULL, "area", 0x0000FF00);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, tt), 1, "cc", FsConfig_Condition_orAnd, "64");
    fs_Config_node_add_property_image(pConfig, parent, 1, "uuid", "recordConfig rtspServerURL");
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "targetColor", "目标颜色", "目标颜色,RGB+虚线间隔,255虚线间隔表示禁用", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFFFFU, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF000000, "不叠加", "不叠加");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF, "实线红色", "实线红色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF00, "实线绿色", "实线绿色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF0000, "实线蓝色", "实线蓝色");
    }
    {
        void* const pNode = fs_Config_node_string_add(pConfig, parent, "ptz_uuid", "联动可见光通道标识", "联动可见光通道标识", 0, 0x7, 1, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "ptz_link_enable", "是否联动可见光", "是否联动可见光", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1-启用", "1-启用");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0-禁用", "0-禁用");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "target_x_extern", "目标x方向扩展大小", "目标x方向扩展大小", FsConfig_nodeShowType_default, 0, 0x7, 0, 65535, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 60000, "60000", "60000");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 30, "30", "30");
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "target_y_extern", "目标y方向扩展大小", "目标y方向扩展大小", FsConfig_nodeShowType_default, 0, 0x7, 0, 65535, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 60000, "60000", "60000");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 30, "30", "30");
    }
    {
        fs_Config_node_string_add(pConfig, parent, "excludeArea", "屏蔽区域", "屏蔽区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 50);
    }
    {
        void* const pNode = fs_Config_node_integer_add(pConfig, parent, "excludeAreaColor", "屏蔽区域颜色", "屏蔽区域颜色,RGB+虚线间隔,255虚线间隔表示禁用", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFFFFU, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF000000, "不叠加", "不叠加");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF, "实线红色", "实线红色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF00, "实线绿色", "实线绿色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF0000, "实线蓝色", "实线蓝色");
    }
    {
        void * const pNode = fs_Config_node_string_add(pConfig, parent, "excludeAreaName", "屏蔽区域名", "屏蔽区域名,与屏蔽区域对应,为空表示不叠加", 0, 0x7, 1, 64, 31);
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "excludeArea", FsConfig_Condition_equal_false, NULL);
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "excludeAreaName_width", "屏蔽区域名字符高度", "屏蔽区域名字符高度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "excludeArea", FsConfig_Condition_equal_false, NULL);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "excludeAreaName", FsConfig_Condition_equal_false, NULL);
        }
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "excludeAreaName_height", "屏蔽区域名字符高度", "屏蔽区域名字符高度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "excludeArea", FsConfig_Condition_equal_false, NULL);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "excludeAreaName", FsConfig_Condition_equal_false, NULL);
        }
    }
    {
        void * const pNode = fs_Config_node_integer_add(pConfig, parent, "excludeAreaName_color", "屏蔽区域名字符颜色,RGB", "屏蔽区域名字符颜色,RGB,16进制", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFFFFU, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF, "红色", "红色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF00, "绿色", "绿色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF0000, "蓝色", "蓝色");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF000000, "不叠加", "不叠加");
        {
            void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "excludeArea", FsConfig_Condition_equal_false, NULL);
            fs_Config_condition_add_static(pConfig, condition_group, 1, "excludeAreaName", FsConfig_Condition_equal_false, NULL);
        }
    }
    //    /* 车辆检测阈值  */
    //    void* pNode = fs_Config_node_integer_add(pConfig, parent, "carThreshold", "车辆检测阈值", "车辆检测阈值", FsConfig_nodeShowType_default, 0, 0x7, 1, 32767, 1);
    //    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 20, "20", "20");
    // 创建检测区
    {
        void* const targetCheckErea = fs_Config_node_template_add(pConfig, parent, "targetCheckErea", "算法模板", NULL, "targetCheckEreaName", "算法模板", NULL, NULL, "timerControl", 0, 0x7, 20);
        {
            //void *const pNode =
            fs_Config_node_string_add(pConfig, targetCheckErea, "targetCheckEreaName", "算法模板名字", "算法模板名字", 0, 0x7, 1, 32, 1);
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "enable", "是否启用", "是否启用", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1-启用", "1-启用");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0-禁用", "0-禁用");
        }
        {
            void *const pNode = fs_Config_node_string_add(pConfig, targetCheckErea, "timerControl", "有效时间", "有效时间,时区+时间区间", 0, 0x7, 24, 33, 7);
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "+08 01-01/00:00:00 12-31/23:59:59", "+08 01-01/00:00:00 12-31/23:59:59", "每年");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-01/00:00:00 00-31/23:59:59", "+08 00-01/00:00:00 00-31/23:59:59", "每月");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/00:00:00 00-00/23:59:59", "+08 00-00/00:00:00 00-00/23:59:59", "每天");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/24:00:00 00-00/24:59:59", "+08 00-00/24:00:00 00-00/24:59:59", "每时");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/24:60:00 00-00/24:60:59", "+08 00-00/24:60:00 00-00/24:60:59", "每分");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 1/00:00:00 7/23:59:59", "+08 1/00:00:00 7/23:59:59", "每周");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "eDetectAlgorithm", "预处理算法", "预处理算法", FsConfig_nodeShowType_default, 0, 0x7, 0, 4, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 3, "3-GSG：检测道路上的车和人", "3-GSG：检测道路上的车和人");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0-UFS：天空中的小目标", "0-UFS：天空中的小目标");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-VAL：检测道路上的车和人", "1-VAL：检测道路上的车和人");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "2-GMM：检测道路上的车和人", "2-GMM：检测道路上的车和人");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "4-Fire：火源检测", "4-Fire：火源检测");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iShiftRadius", "漂移半径", "漂移半径", FsConfig_nodeShowType_default, 0, 0x7, 1, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 10, "10", "10");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iBarRatio", "长宽比", "长宽比", FsConfig_nodeShowType_default, 0, 0x7, 1, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 6, "6", "6");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "minSizeX", "最小目标像素X", "最小目标像素X", FsConfig_nodeShowType_default, 0, 0x7, 0, 512, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "minSizeY", "最小目标像素Y", "最小目标像素Y", FsConfig_nodeShowType_default, 0, 0x7, 0, 512, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "maxSizeX", "最大目标像素X", "最大目标像素X", FsConfig_nodeShowType_default, 0, 0x7, 0, 512, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 512, "512", "512");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "maxSizeY", "最大目标像素Y", "最大目标像素Y", FsConfig_nodeShowType_default, 0, 0x7, 0, 512, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 512, "512", "512");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "classCheck", "目标类型识别", "目标类型识别", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "关闭", "关闭");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "开启", "开启");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "trackEnable", "目标跟踪", "目标跟踪", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "关闭", "关闭");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "开启", "开启");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iMaxAngle", "最大转弯角度", "最大转弯角度", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 45, "45", "45");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "trackEnable", FsConfig_Condition_equal, "1");
            }
        }
        {
            void* const pNode = fs_Config_node_float_add(pConfig, targetCheckErea, "dLimitGauss", "匹配阈值计算控制", "匹配阈值计算控制", 0, 0x7, 0.1, 1.0, 1);
            fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0.40, "0.40", "0.40");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "trackEnable", FsConfig_Condition_equal, "1");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "learnCount", "n次连续轨迹显示", "n次连续轨迹显示", FsConfig_nodeShowType_default, 0, 0x7, 0, 20, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 5, "5", "5");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "skipCount", "n次轨迹预测显示", "n次轨迹预测显示", FsConfig_nodeShowType_default, 0, 0x7, 0, 20, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 3, "3", "3");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "trackOut", "轨迹剔除", "轨迹剔除(剔除掉小于设置的“n次连续轨迹显示”数值的目标)", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "关闭", "关闭");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "开启", "开启");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iUseForSkyTHthresh", "灰度阈值", "灰度阈值", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 65, "65", "65");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "0");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iUseForSkyColorType", "机芯伪彩模式", "机芯伪彩模式", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-白热", "0-白热");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-黑热", "1-黑热");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "0");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iVibeRadius", "灰度阈值", "灰度阈值", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "1");
            }
        }
        {
            void* const pNode = fs_Config_node_float_add(pConfig, targetCheckErea, "fGmmD", "偏差阈值", "偏差阈值", 0, 0x7, 0.1, 100.0, 1);
            fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1.2, "1.2", "1.2");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "2");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "fGmmSdInit", "初始化标准差", "初始化标准差", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 12, "12", "12");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "2");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iGrimsonGmmMatchThreshold", "灰度阈值", "灰度阈值", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 20, "20", "20");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "3");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iGrimsonGmmMedianBlurKernelSize", "晃动抑制", "晃动抑制", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 3, "3", "3");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "3");
            }
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "iFireRadius", "灰度阈值", "灰度阈值", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "eDetectAlgorithm", FsConfig_Condition_equal, "4");
            }
        }
        {
            void* const pNode = fs_Config_node_string_add(pConfig, targetCheckErea, "area", "区域", "区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 50);
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "enable", FsConfig_Condition_equal, "1");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "area_enable", "区域是否有效", "区域是否有效,值的数量少于area时,使用最后一个值", FsConfig_nodeShowType_hex, 0, 0x7, 0, 1, 50);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1-有效", "1-有效");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0-无效", "0-无效");
        }
        {
            void* const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "areaColor", "区域颜色", "区域颜色,RGB+虚线间隔,255虚线间隔表示禁用,值的数量少于area时,使用最后一个值", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFFFFU, 50);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF000000, "不叠加", "不叠加");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF, "实线红色", "实线红色");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF00, "实线绿色", "实线绿色");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF0000, "实线蓝色", "实线蓝色");
        }
        {
            void * const pNode = fs_Config_node_string_add(pConfig, targetCheckErea, "areaName", "区域名", "区域名,与区域对应,为空表示不叠加", 0, 0x7, 1, 64, 31);
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "area", FsConfig_Condition_equal_false, NULL);
            }
        }
        {
            void * const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "watermarking_width", "水印字符宽度", "水印字符宽度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "area", FsConfig_Condition_equal_false, NULL);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "areaName", FsConfig_Condition_equal_false, NULL);
            }
        }
        {
            void * const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "watermarking_height", "水印字符高度", "水印字符高度", FsConfig_nodeShowType_default, 0, 0x7, 1, 128, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 32, "32", "32");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "area", FsConfig_Condition_equal_false, NULL);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "areaName", FsConfig_Condition_equal_false, NULL);
            }
        }
        {
            void * const pNode = fs_Config_node_integer_add(pConfig, targetCheckErea, "watermarking_color", "水印颜色,RGB", "水印颜色,RGB,16进制", FsConfig_nodeShowType_hex, 0, 0x7, 0, 0xFFFFFFFFU, 50);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0xFF, "红色", "红色");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF00, "绿色", "绿色");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF0000, "蓝色", "蓝色");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0xFF000000, "不叠加", "不叠加");
            {
                void *const condition_group = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "enable", FsConfig_Condition_equal, "1");
                fs_Config_condition_add_static(pConfig, condition_group, 1, "area", FsConfig_Condition_equal_false, NULL);
                fs_Config_condition_add_static(pConfig, condition_group, 1, "areaName", FsConfig_Condition_equal_false, NULL);
            }
        }
    }
    // 测试
    {
        void* const tt1 = fs_Config_node_node_add(pConfig, parent, "tt1", "tt1", "tt1", 0, 0x7);
        {
            void* const pNode = fs_Config_node_string_add(pConfig, tt1, "area", "区域", "区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 50);
        }
        void* const tt2 = fs_Config_node_template_add(pConfig, tt1, "tt2", "tt2", NULL, NULL, "tt2", NULL, NULL, NULL, 0, 0x7, 20);
        void* const tt3 = fs_Config_node_node_add(pConfig, tt2, "tt3", "tt3", "tt3", 0, 0x7);
        void* const tt4 = fs_Config_node_template_add(pConfig, tt3, "tt4", "tt4", NULL, NULL, "tt4", NULL, NULL, NULL, 0, 0x7, 20);
        {
            void* const pNode = fs_Config_node_string_add(pConfig, tt4, "area", "区域", "区域,以图像左上角为坐标原点,单位为像素点 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 50);
        }
    }
}

/* 检查pinflrayObject模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int inflrayObject_check_channel_changed(struct InflrayObject * const pInflrayObject, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server InflrayObject
#define __check_channel_changed_checkTimeControl "inflrayObjectConfig targetCheckErea"
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
struct InflrayObject * inflrayObject_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 配置管理对象 */ void *const pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#ifdef loglevel
    if (loglevel == FsNumberMax(loglevel)) fs_log_addLevel(&loglevel, "InflrayObject", pShareBuffer);
#endif
    unsigned int len = strlen(threadName) + 1;
    struct InflrayObject * const rst = (struct InflrayObject*) fsMalloc(sizeof (struct InflrayObject) +len);
    memset(rst, 0, sizeof (struct InflrayObject));
    char *pd = (char*) (rst + 1);
#ifndef __thread_base_init 
    rst->ro._pMonitor = pMonitor;
    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    if (externRunStatus) rst->ro._externRunStatus = externRunStatus;
    else rst->ro._externRunStatus = &defaultRunStatus;
    /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
    rst->ro._parentTid = parentTid;
    /* 线程名 */
    rst->ro._threadName_ = pd - (char*) (rst + 1), len = strlen(threadName) + 1, memcpy(pd, threadName, len);
#endif
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
void inflrayObject_delete__OI(struct InflrayObject * const pInflrayObject) {

    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pInflrayObject->ro.__itemList_);

    fsFree(pInflrayObject);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void inflrayObject_startThread(struct InflrayObject * const pInflrayObject
        , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    pInflrayObject->rw.runStatus = 1;
#ifdef FsDebug
    InflrayObjectLog2(FsLogType_threadDebug, "Init Parameter");
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
    if (pthread_create(&pInflrayObject->p.stid, &attr, (void* (*)(void*))inflrayObject_P_T, pInflrayObject)) {

        FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
        fflush(stdout);
        FsLogExit();
    }
    pthread_attr_destroy(&attr);
}

/* 结束线程 */
void inflrayObject_stopThread(struct InflrayObject * const pInflrayObject) {
#define __stopThread_var pInflrayObject
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
            if (write(__control_pipe_write_var, "w", 1) != 1)

            {
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
void inflrayObject_item_frame_in_pthreadSafety__OI_4(struct InflrayObject * const pInflrayObject, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server InflrayObject
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
unsigned int inflrayObject_item_frame_out_pthreadSafety__IO_4(struct InflrayObject * const pInflrayObject, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server InflrayObject
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
        for (frameCount = index; frameCount > 0; frameCount--) {
            // FsPrintf(1, "index=%d,stats->decodeMask_set=%llx/%llx/%lx\n", (*ppNode)->index, (*ppNode)->stats->decodeMask_set, (*ppNode)->stats->decodeMask, ImageFrame_YUV420P_0);
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
unsigned int inflrayObject_item_get_frame_count_pthreadSafety(struct InflrayObject * const pInflrayObject, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server InflrayObject
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
void inflrayObject_item_reset_groupSqrtOut(struct InflrayObject * const pInflrayObject, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server InflrayObject
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
