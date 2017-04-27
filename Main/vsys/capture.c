#include "capture.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../Modules/camera_rtmp/camera_rtmp.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////

/* 本地推送的相机项 */
struct Capture_private_localPush_item {

    /* 
     * 结构约定:
     *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
     *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
     *     p中为私有变量,外部不可读也不可写;
     * 变量命名约定:
     *     凡需要(非0)初始化的变量,都要加上_的前缀;
     *     凡需要内存释放的变量,都要加上__的前缀. 
     */
    struct {
        /* 所属的采集对象 */
        struct Capture_item * _pCapture_item;
        /* 当前的数据来源指针 */
        const void *src_current;
        /* 备用的数据来源指针 */
        const void *src_spare;
        /* 备用的数据来源计数器,为0时才可替换当前数据源 */
        unsigned char spareCounter;
    } ro;
};

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* rtsp相机的回调函数,成功返回1,失败返回-1 */
static signed char capture_private_camera_rtsp_cb(FsObjectH264 * const pH264, /* 帧率 */const float frameRate, /* 采集项 */ struct Capture_item *pCapture_item, MonitorItem *pMonitorItem) {
    if (pCapture_item->ro.imageWidth != pH264->width || pCapture_item->ro.imageHeight != pH264->height) {
        pCapture_item->ro.imageWidth = pH264->width;
        pCapture_item->ro.imageHeight = pH264->height;
        pCapture_item->ro.classIndex++;
    }
    pMonitorItem->rw.line = 40000 + __LINE__;
    pH264->classIndex = pCapture_item->ro.classIndex;
    FsObjectImageFrame *pFrame = image_frame_new__IO(pCapture_item->ro.imageWidth, pCapture_item->ro.imageHeight,
            pCapture_item->ro.imageWidth >> 1, pCapture_item->ro.imageHeight >> 1,
            pCapture_item->ro.imageWidth >> 2, pCapture_item->ro.imageHeight >> 2,
            NULL, pH264->frameIndex, pH264->ipsrc, frameRate, pH264->capture_uptime, pH264->capture_gmtTime);
    fs_Object_addRefer_pthreadSafety(pH264, __LINE__);
    image_frame_set__OI_2(pFrame, (FsObject*) pH264, 0);
    pMonitorItem->rw.line = 40000 + __LINE__;
    fs_ObjectList_insert_tail_pthreadSafety(pCapture_item->ro.__framelistOut, pFrame);
    static int lostdata = 0;
    if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
    pMonitorItem->rw.line = 40000 + __LINE__;
    return 1;
}

/* localPush相机的回调函数,成功返回1,失败返回-1 */
static void capture_private_localPush_cb_pthreadSafety(/* 本地推送相机项 */ struct Capture_private_localPush_item * const plocalPush_item, FsObjectImageFrame * const pFrame, /* 上一个采集的索引 */ const void* const src) {
    if (plocalPush_item->ro.src_current == src) {
        fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
        fs_ObjectList_insert_tail_pthreadSafety(plocalPush_item->ro._pCapture_item->ro.__framelistOut, pFrame);
        if (plocalPush_item->ro.spareCounter < 32)plocalPush_item->ro.spareCounter <<= 1;
        else if (plocalPush_item->ro.spareCounter < 255)plocalPush_item->ro.spareCounter++;
    } else if (plocalPush_item->ro.src_spare == src || NULL == plocalPush_item->ro.src_current) {
        if (0 == plocalPush_item->ro.spareCounter--) {
            plocalPush_item->ro.src_current = src;
            fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
            fs_ObjectList_insert_tail_pthreadSafety(plocalPush_item->ro._pCapture_item->ro.__framelistOut, pFrame);
            plocalPush_item->ro.src_spare = NULL;
        } else if (plocalPush_item->ro.spareCounter > 16)plocalPush_item->ro.spareCounter--;
    } else {
        plocalPush_item->ro.src_spare = src;
        plocalPush_item->ro.spareCounter = 255;
    }
    static int lostdata = 0;
    if (plocalPush_item->ro._pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&plocalPush_item->ro._pCapture_item->ro.__framelistOut->mutex);
            if (plocalPush_item->ro._pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", plocalPush_item->ro._pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(plocalPush_item->ro._pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&plocalPush_item->ro._pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
}

/* localPush相机的回调函数,成功返回1,失败返回-1 */
static signed char capture_private_localPush_cb1(/* 采集项 */ struct Capture_item *pCapture_item, FsObjectImageFrame * const pFrame, /* 上一个采集的索引 */ const unsigned int index) {
    if (pCapture_item->ro.imageWidth != pFrame->classIndex || pCapture_item->ro.imageHeight != index) {
        pCapture_item->ro.imageWidth = pFrame->classIndex;
        pCapture_item->ro.imageHeight = index;
        pCapture_item->ro.classIndex++;
    }
    pFrame->classIndex = pCapture_item->ro.classIndex;
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_ObjectList_insert_tail_pthreadSafety(pCapture_item->ro.__framelistOut, pFrame);
    static int lostdata = 0;
    if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
        if (lostdata++ > 1024) {
            pthread_mutex_lock(&pCapture_item->ro.__framelistOut->mutex);
            if (pCapture_item->ro.__framelistOut->nodeCount > 32) {
                static int tttt = 0;
                if (tttt-- < 0) {
                    FsPrintf(1, "List count(=%lu) too long.\n", pCapture_item->ro.__framelistOut->nodeCount);
                } else tttt = 1000;
                fs_Object_delete_pthreadSafety__OI(fs_ObjectList_remove_head(pCapture_item->ro.__framelistOut));
            }
            pthread_mutex_unlock(&pCapture_item->ro.__framelistOut->mutex);
        }
    } else if (lostdata > 0)lostdata--;
    return 1;
}

/* 
 * 创建一个新的Captuer_private_localPush_item对象;
 * 返回Captuer_private_localPush_item指针.
 */
static struct Capture_private_localPush_item *capture_private_localPush_item_new__IO(struct Capture_item * const pCapture_item) {
    struct Capture_private_localPush_item* rst = (struct Capture_private_localPush_item*) malloc(sizeof (struct Capture_private_localPush_item));
    memset(rst, 0, sizeof (struct Capture_private_localPush_item));
    /* 所属的采集对象 */
    rst->ro._pCapture_item = pCapture_item;
    /* 注册本地推送数据指令 */
    configManager_cmd_register(pCapture_item->ro._pCapture->ro._pConfigManager, "localPush", pCapture_item->ro._uuid, 0, rst, NULL, (void*) capture_private_localPush_cb_pthreadSafety, rst);

    return rst;
}

/* 仿照其他相机从相机中移除一个连接项并删除,如果相机链表为空,把此相机也删除,并把相机指针置0 */
static void capture_private_localPush_remove_and_delete_item_and_emptyCamera(/* 不为空时,且pLocalPush_list所属的相机对象内没有相机项时,把pCamera_rtmp_item所属的相机对象从此链表中移除 */FsObjectList * const pLocalPush_list,
        /* 所属的相机对象内没有相机项时,所属的相机删除 */ struct Capture_private_localPush_item * const pLocalPush_item) {
    /* 注销本地推送数据指令 */
    configManager_cmd_logoff(pLocalPush_item->ro._pCapture_item->ro._pCapture->ro._pConfigManager, "localPush", pLocalPush_item->ro._pCapture_item->ro._uuid, 0, pLocalPush_item);

    free(pLocalPush_item);
}

/* 控制流开始停止,必须在添加到服务后才能调用此函数 */
static void capture_private_localPush_item_streamControl_pthreadSafety(struct Capture_private_localPush_item * const pLocalPush_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value) {
    /* 不需要任何内容 */
}

static inline unsigned int capture_private_item_ClusterLen_get() {
    struct Capture_item item;
    return ((long) (&item.ro._pCapture)-(long) (&item) + sizeof (void*));
}

/* 从配置获取通道数 */
static unsigned int capture_private_get_channelCount(/* 可为空 */FsConfig *pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

/* 删除pCapture_item指向的实例对象 */
static void capture_private_item_delete__OI(struct Capture_item* pCapture_item) {
    if (NULL == pCapture_item)return;
    /* 注销命令字 */
    if (0 == pCapture_item->ro._ipv4) configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "camctl", pCapture_item->ro._uuid, 0, pCapture_item);
    configManager_cmd_logoff(pCapture_item->ro._pCapture->ro._pConfigManager, "cameractrl", pCapture_item->ro._uuid, pCapture_item->ro._ipv4, pCapture_item);
    if (0 == pCapture_item->ro._ipv4) {
        /* 断开实时视频掩码通信 */
        configManager_mask_disconnect_pthreadSafety(pCapture_item->ro._pCapture->ro._pConfigManager, "realStream", pCapture_item->ro._uuid, pCapture_item);
        /* 删除相机 */
        pCapture_item->ro._camera_remove_and_delete_item_and_emptyCamera(pCapture_item->ro._pCameraList, pCapture_item->ro.__pCamera_item);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_custom__OI(pCapture_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
    }
    free(pCapture_item);
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
static signed char camctl_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
        /* 收到数据的前4字节 */unsigned int head, /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p) {
    struct Capture_item* pCapture_item = (struct Capture_item *) p;

    if (1) {
        fs_Ebml_analyzeAll(pEbml, (struct FsEbml_node*) pEbml);
        FsJson *pJson_1 = fs_Ebml_convert_to_Json__IO(pEbml);

        FsString* info = fs_Json_to_String__IO(pJson_1, 0, 0, "\r\n", NULL, 4);

        printf("\n%s\n", info->buffer);
        fs_Json_delete__OI(pJson_1);
        fs_String_delete__OI(info);
    }

    struct FsEbml_node *cmd = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_type");
    if (strcmp("camctl", cmd->data.buffer) == 0) {

        struct FsEbml_node *m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");

        printf("0 m_action: %d \n", pCapture_item->rw.m_action);

        if (strcmp("0002H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 1; //右
        if (strcmp("0004H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 2; //左
        if (strcmp("0008H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 3; //上
        if (strcmp("0010H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 4; //下
        if (strcmp("000CH", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 5; //左上
        if (strcmp("0014H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 6; //左下
        if (strcmp("000AH", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 7; //右上
        if (strcmp("0012H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 8; //右下
        if (strcmp("0020H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 9; //变倍大
        if (strcmp("0040H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 10; //变倍小
        if (strcmp("0080H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 11; //聚焦远
        if (strcmp("0100H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 12; //聚焦近
        if (strcmp("0200H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 13; //光圈大
        if (strcmp("0400H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 14; //光圈小
        if (strcmp("0000H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 15; //停止码
        if (strcmp("8100H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 16; //自动除雾开
        if (strcmp("8200H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 17; //自动除雾关
        if (strcmp("8300H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 18; //激光器开
        if (strcmp("8400H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 19; //激光器关
        if (strcmp("0500H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 20; //转向ptz
        if (strcmp("0600H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 21; //获取ptz
        if (strcmp("0900H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 22; //红光开
        if (strcmp("0800H", m_node->data.buffer) == 0) pCapture_item->rw.m_action = 23; //红光关

        printf("m_action: %d\n", pCapture_item->rw.m_action);

        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_speed");
        pCapture_item->rw.camera_speed = atoi(m_node->data.buffer);
        if (pCapture_item->rw.camera_speed > 7) pCapture_item->rw.camera_speed = 7;
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_time");
        pCapture_item->rw.camera_time = atoi(m_node->data.buffer);

        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "m_auto_focus");
        pCapture_item->rw.m_auto_focus = atoi(m_node->data.buffer);
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "m_target_id");
        pCapture_item->rw.m_target_id = atoi(m_node->data.buffer);

        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (strcmp("json", m_node->data.buffer) == 0) pCapture_item->rw.m_reply_type = 1;
        if (strcmp("xml", m_node->data.buffer) == 0) pCapture_item->rw.m_reply_type = 2;


        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "P");
        pCapture_item->rw.goto_p = atof(m_node->data.buffer);
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "T");
        pCapture_item->rw.goto_t = atof(m_node->data.buffer);
        m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "Z");
        pCapture_item->rw.goto_z = atof(m_node->data.buffer);

        printf("1 m_action: %d  _uuid:%s \n", pCapture_item->rw.m_action, pCapture_item->ro._uuid);

        //回执信息，默认都是成功
        if (1) {

            char temp[54];
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "code", FsEbmlNodeType_String);
            fs_Ebml_node_data_set_string(pEbml_node, "1000");
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "msg", FsEbmlNodeType_String);
            //sprintf(temp, "%s", "");
            //fs_Ebml_node_data_set_string(pEbml_node, temp);

            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "P", FsEbmlNodeType_String);
            sprintf(temp, "%f", pCapture_item->rw.camera_c_p);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "T", FsEbmlNodeType_String);
            sprintf(temp, "%f", pCapture_item->rw.camera_c_t);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "Z", FsEbmlNodeType_String);
            sprintf(temp, "%f", pCapture_item->rw.camera_c_z);
            fs_Ebml_node_data_set_string(pEbml_node, temp);

            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
            sprintf(temp, "%s", pCapture_item->ro._uuid);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
            m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");
            sprintf(temp, "%s", m_node->data.buffer);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "image_byte", FsEbmlNodeType_String);
            //sprintf(temp, "%s", "");
            //fs_Ebml_node_data_set_string(pEbml_node, temp);           

            FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
            fs_Ebml_delete__OI(rst);

            FsString * pString = fs_Json_to_String__IO(pJson, 8, sizeof (unsigned int)*3, "\r\n", NULL, 4);
            fs_Json_delete__OI(pJson);
            pString->lenth -= sizeof (unsigned int)*3;
            *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = pString->lenth - 8;
            pString->buffer[0] &= 0xF0;
            pString->buffer[0] |= 0xA;
            memcpy(pString->buffer + pString->lenth, requestID_3, sizeof (unsigned int)*3);
            configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        }

        if (write(pCapture_item->ro._pCapture->ro.__pipe[1], "1", 1) != 1) {
            FsPrintf(5, "write pipe(=%d) error:\"%s\".\n", pCapture_item->ro._pCapture->ro.__pipe[1], strerror(errno));
            fflush(stdout);
        }
    }

    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
static signed char capture_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
        /* 收到数据的前4字节 */unsigned int head, /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p) {
    struct Capture_item* pCapture_item = (struct Capture_item *) p;
    fs_Ebml_out_debug(pEbml, stdout), printf("\n");
    struct FsEbml_node *pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "ctrl_type");
    if (strcmp("turn", pEbml_node->data.buffer) == 0) {
        /* 转动 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speedx");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedx,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short x = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speedy");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short y = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid y,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        float time = (pEbml_node->type & 0x7) == FsEbmlNodeType_Float ? *((double*) pEbml_node->data.buffer) : atof(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->turn_pthreadSafety(pCapture_item->ro.__pCamera_item, x, y, time) != 1) {
            FsPrintf(1, "Ctrl camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("zoom", pEbml_node->data.buffer) == 0) {
        /* 变倍 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speed");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short speed = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid y,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        float time = (pEbml_node->type & 0x7) == FsEbmlNodeType_Float ? *((double*) pEbml_node->data.buffer) : atof(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->zoom_pthreadSafety(pCapture_item->ro.__pCamera_item, speed, time) != 1) {
            FsPrintf(1, "Ctr camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("focus", pEbml_node->data.buffer) == 0) {
        /* 聚焦 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speed");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short speed = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid y,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        float time = (pEbml_node->type & 0x7) == FsEbmlNodeType_Float ? *((double*) pEbml_node->data.buffer) : atof(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->focus_pthreadSafety(pCapture_item->ro.__pCamera_item, speed, time) != 1) {
            FsPrintf(1, "Ctr camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("preset_set", pEbml_node->data.buffer) == 0) {
        /* 设置预置位 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "presetIndex");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short presetIndex = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->preset_set_pthreadSafety(pCapture_item->ro.__pCamera_item, presetIndex) != 1) {
            FsPrintf(1, "Ctr camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("preset_goto", pEbml_node->data.buffer) == 0) {
        /* 调用预置位 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "presetIndex");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short presetIndex = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->preset_goto_pthreadSafety(pCapture_item->ro.__pCamera_item, presetIndex) != 1) {
            FsPrintf(1, "Ctr camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("preset_clean", pEbml_node->data.buffer) == 0) {
        /* 清除预置位 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "presetIndex");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short presetIndex = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->preset_clean_pthreadSafety(pCapture_item->ro.__pCamera_item, presetIndex) != 1) {
            FsPrintf(1, "Ctr camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("zoom3D", pEbml_node->data.buffer) == 0) {
        /* 3D定位 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "x1");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short x1 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "y1");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short y1 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "x2");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short x2 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "y2");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            FsPrintf(1, "Invalid speedy,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        short y2 = (pEbml_node->type & 0x7) == FsEbmlNodeType_Integer ? *((long long*) pEbml_node->data.buffer) : atoi(pEbml_node->data.buffer);
        if (pCapture_item->ro._ctl_function->zoom3D_pthreadSafety(pCapture_item->ro.__pCamera_item, x1, y1, x2, y2) != 1) {
            FsPrintf(1, "Ctr camera failed,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        }
        ////////////////////////////////////////////////////////////////////////
        /* 发回执数据 */
        FsString *pString = fs_String_new__IO(8 + 2 + sizeof (unsigned int)*3);
        pString->lenth -= sizeof (unsigned int)*3;
        *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = 2;
        pString->buffer[0] &= 0xF0;
        pString->buffer[0] |= 0x2;
        memcpy(pString->buffer + 8, "ok", 2), memcpy(pString->buffer + 8 + 2, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else if (strcmp("maskget", pEbml_node->data.buffer) == 0) {
        /* 控制掩码获取 */
        FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        *(unsigned long long *) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "mask", FsEbmlNodeType_Integer)->data.buffer = pCapture_item->ro._ctl_function->mask_get(pCapture_item->ro.__pCamera_item);
        struct FsEbml_node * return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        FsString * pString;
        if (return_type && 0 < return_type->data.lenth) {
            if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
            else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
        }
        if (1 == requestDataType) {
            pString = fs_Ebml_to_String__IO(rst, 8, sizeof (unsigned int)*3, NULL);
            pString->lenth -= sizeof (unsigned int)*3;
            *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = pString->lenth - 8;
            pString->buffer[0] &= 0xF0;
            pString->buffer[0] |= 0x3;
        } else if (2 == requestDataType) {
            FsXml *pXml = fs_Ebml_convert_to_Xml__IO(rst);
            pString = fs_Xml_to_String__IO(pXml, 8, sizeof (unsigned int)*3, "\r\n", NULL, 4);
            fs_Xml_delete__OI(pXml);
            pString->lenth -= sizeof (unsigned int)*3;
            *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = pString->lenth - 8;
            pString->buffer[0] &= 0xF0;
            pString->buffer[0] |= 0xA;
        } else if (3 == requestDataType) {
            FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
            pString = fs_Json_to_String__IO(pJson, 8, sizeof (unsigned int)*3, "\r\n", NULL, 4);
            fs_Json_delete__OI(pJson);
            pString->lenth -= sizeof (unsigned int)*3;
            *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = pString->lenth - 8;
            pString->buffer[0] &= 0xF0;
            pString->buffer[0] |= 0xA;
        } else {
            FsPrintf(5, "Invalid requestDataType(=%hhd) data:\n", requestDataType);
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            pString = fs_Ebml_to_String__IO(rst, 8, sizeof (unsigned int)*3, NULL);
            pString->lenth -= sizeof (unsigned int)*3;
            *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = pString->lenth - 8;
            pString->buffer[0] &= 0xF0;
            pString->buffer[0] |= 0x3;
        }
        fs_Ebml_delete__OI(rst);
        memcpy(pString->buffer + pString->lenth, requestID_3, sizeof (unsigned int)*3);
        configManager_send_pthreadSafety__OI_2(pCapture_item->ro._pCapture->ro._pConfigManager, pString);
        return 1;
    } else {
        FsPrintf(1, "Invalid data:\n");
        fs_Ebml_out_debug(pEbml, stdout), printf("\n");
        return -1;
    }
}

/* 控制相机取流的回调函数 */
static void capture_private_item_cb_streamControl(/* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 状态,0-全0,1-部分为1 */ const unsigned char status, /* 关联的对象 */ struct Capture_item * const pCapture_item) {
    pCapture_item->ro._streamControl(pCapture_item->ro.__pCamera_item, status);
}

static void capture_private_item_new(struct Capture *pCapture, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList,
        /* 原来的采集项 */struct Capture_item **ppCapture_item, /* rtsp相机指针 */FsObjectList *camera_rtsp_list, /* rtmp相机指针 */FsObjectList *camera_rtmp_list, /* gb28181相机指针 */FsObjectList *camera_gb28181_list) {
    FsConfig *pConfig = ((ConfigManager*) pCapture->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    FsObjectList * clusterList = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0 == channel, "vsys");
    if (clusterList) {
        void *parent, *pNode, **ppNode, **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int i = clusterList->nodeCount, j, k, index = 0, ipv4, len;
        FsObjectList *list;
        struct Capture_item **ppNodeCapture_item, *rst;
        char *uuid, *pd;
        unsigned int camera_ipv4;
        unsigned short rtspPort;
        unsigned long long streamMask;
        char *cameraType;
        do {
            parent = *ppNodeCluster++;
            list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
            if (list) {
                ppNode = list->pNode + list->startIndex;
                j = list->nodeCount;
                if (0 == channel) {
                    if (NULL == pCapture->p.clusterItemList) pCapture->p.clusterItemList = fs_ObjectList_new__IO(4);
                    while (pCapture->p.clusterItemList->nodeCount < index + j)fs_ObjectList_insert_tail(pCapture->p.clusterItemList, NULL);
                    ppNodeCapture_item = (struct Capture_item**) pCapture->p.clusterItemList->pNode + pCapture->p.clusterItemList->startIndex;
                    uuid = fs_Config_node_string_get_first__IO(pConfig, parent, "hostname");
                    pd = strchr(uuid, ':');
                    ipv4 = fs_ipv4_network_get(pd ? pd - uuid : strlen(uuid), uuid);
                    free(uuid);
                } else ipv4 = 0;
                do {
                    if (0 == channel)parent = *ppNode++;
                    else parent = ppNode[channel - 1];

                    uuid = fs_Config_node_string_get_first__IO(pConfig, parent, "uuid");
                    ////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        if (ppNodeCapture_item[index])capture_private_item_delete__OI(ppNodeCapture_item[index]);
                    } else if (*ppCapture_item)capture_private_item_delete__OI(*ppCapture_item);

                    len = strlen(uuid) + 1;
                    if (channel > 0) {
                        rst = (struct Capture_item*) malloc(sizeof (struct Capture_item) +len);
                        memset(rst, 0, sizeof (struct Capture_item));
                        pd = (char*) (rst + 1);
                    } else {
                        rst = (struct Capture_item*) malloc(capture_private_item_ClusterLen_get() + len);
                        memset(rst, 0, capture_private_item_ClusterLen_get());
                        pd = ((char*) rst) + capture_private_item_ClusterLen_get();
                    }
                    /* 本对象的uuid值,一般是从配置中读入的 */
                    rst->ro._uuid = pd, memcpy(pd, uuid, len), pd += len;
                    /* 所属主机的ip地址,用于集群,本机为0 */
                    rst->ro._ipv4 = ipv4;
                    /* 所属的记录对象 */
                    rst->ro._pCapture = pCapture;
                    if (channel > 0) {
                        /* 传出的数据帧链表 */
                        rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                        pNode = fs_Config_node_get_first(pConfig, parent, "cameraConfig");
                        cameraType = fs_Config_node_string_get_first__IO(pConfig, pNode, "cameraType");
                        if (strcmp("rtmp", cameraType) == 0 || strcmp("localPush", cameraType)) {
                            struct Camera_rtmp *pCamera_rtmp, **ppNode_Camera_rtmp;
                            struct Camera_rtmp_item *pCamera_rtmp_item;
                            {
                                k = camera_rtmp_list->nodeCount;
                                if (k > 0) {
                                    ppNode_Camera_rtmp = (struct Camera_rtmp **) camera_rtmp_list->pNode + camera_rtmp_list->startIndex + camera_rtmp_list->nodeCount - 1;
                                    for (; k > 0; k--) {
                                        pCamera_rtmp = *ppNode_Camera_rtmp--;
                                        if (pCamera_rtmp->ro.__cameralist->nodeCount < pCapture->ro._channelCountPerThread)break;
                                    }
                                }
                                if (0 == k) {
                                    pCamera_rtmp = camera_rtmp_new__IO("Camera_rtmp", pCapture->ro._pMonitor, systemThreadTid, pCapture->ro._externRunStatus, pCapture->ro._pRtmpClient);
                                    camera_rtmp_startThread(pCamera_rtmp, -1);
                                    fs_ObjectList_insert_tail(camera_rtmp_list, pCamera_rtmp);
                                }
                                pd = fs_Config_node_string_get_first__IO(pConfig, pNode, "ipv4");
                                if (pd) {
                                    camera_ipv4 = fs_ipv4_network_get(strlen(pd), pd);
                                    // if (FS_INVALID_IPV4 == camera_ipv4) FsPrintf(1, "Channel(=%u) get \"%s\" failed.\n", channel, pd), fflush(stdout);
                                } else camera_ipv4 = FS_INVALID_IPV4;

                                rtspPort = fs_Config_node_integer_get_first(pConfig, pNode, "rtspPort", 554);

                                char *rtspURL = fs_Config_node_string_get_first__IO(pConfig, pNode, "rtspURL");
                                char *rstpUsername = fs_Config_node_string_get_first__IO(pConfig, pNode, "rstpUsername");
                                char *rstpPasswd = fs_Config_node_string_get_first__IO(pConfig, pNode, "rstpPasswd");
                                streamMask = fs_Config_node_integer_get_mask(pConfig, pNode, "streamMask");
                                char *channelUUID = fs_Config_node_string_get_first__IO(pConfig, pNode, "channelUUID");

                                pCamera_rtmp_item = camera_rtmp_item_new__IO(streamMask, FS_INVALID_IPV4 == camera_ipv4 ? (pd ? pd : "Invlaid domain") : NULL,
                                        FS_INVALID_IPV4 == camera_ipv4 ? 0 : camera_ipv4, rtspPort, rstpUsername, rstpPasswd, rtspURL, channelUUID,
                                        (signed char (*)(FsObjectH264*, float, void*, MonitorItem*))capture_private_camera_rtsp_cb, rst);
                                if (pd)free(pd);
                                if (rtspURL)free(rtspURL);
                                if (rstpUsername)free(rstpUsername);
                                if (rstpPasswd)free(rstpPasswd);
                                if (channelUUID)free(channelUUID);
                            }
                            /* 对应的相机对象链表 */
                            rst->ro._pCameraList = camera_rtmp_list;
                            /* 相机项 */
                            rst->ro.__pCamera_item = pCamera_rtmp_item;
                            /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
                            rst->ro._camera_remove_and_delete_item_and_emptyCamera = (void (*)(void*const, void*const))camera_rtmp_remove_and_delete_item_and_emptyCamera;
                            camera_rtmp_add_item_pthreadSafety__OI_2(pCamera_rtmp, pCamera_rtmp_item);
                            /* 控制相机流的函数指针 */
                            rst->ro._streamControl = (void (*)(void*, unsigned char)) camera_rtmp_item_streamControl_pthreadSafety;
                            /* 连接实时视频掩码通信 */
                            configManager_mask_connect_pthreadSafety(pCapture->ro._pConfigManager, "realStream", uuid, rst, (void (*)(const char*, const char*, unsigned char, void*)) capture_private_item_cb_streamControl, rst);
                            /* 控制 */
                            rst->ro._ctl_function = NULL;




                            //获取云台ip和端口
                            pd = fs_Config_node_string_get_first__IO(pConfig, pNode, "holderipv4");
                            if (NULL == pd) pd = fs_Config_node_string_get_first__IO(pConfig, pNode, "ipv4");

                            sprintf(rst->p.camera_ip, "%s", pd);
                            free(pd);

                            rst->p.camera_port = fs_Config_node_integer_get_first(pConfig, pNode, "holderPort", 0);

                            rst->p.shot_type = fs_Config_node_integer_get_first(pConfig, pNode, "shotType", 0);



                            rst->rw.camera_c_p = 0.0;
                            rst->rw.camera_c_t = 0.0;
                            rst->rw.camera_c_z = 0.0;
                            rst->rw.goto_p = 0.0;
                            rst->rw.goto_t = 0.0;
                            rst->rw.goto_z = 0.0;
                            rst->rw.m_action = 0;
                            rst->rw.m_stopAction = 100;
                            rst->rw.camera_speed = 0;
                            rst->rw.camera_time = 0;
                            rst->rw.m_auto_focus = 0;

                            /* 注册命令字 */
                            configManager_cmd_register(pCapture->ro._pConfigManager, "camctl", rst->ro._uuid, 0, rst, camctl_private_cmd_cb, NULL, rst);
                        } else if (strcmp("localPush", cameraType) == 0) {
                            /* 对应的相机对象链表 */
                            //rst->ro._pCameraList = NULL;
                            /* 相机项 */
                            rst->ro.__pCamera_item = capture_private_localPush_item_new__IO(rst);
                            /* 从相机中删除相机项并删除相机链表中无相机的相机对象的函数指针,并把相机指针置0 */
                            rst->ro._camera_remove_and_delete_item_and_emptyCamera = (void (*)(void*const, void*const))capture_private_localPush_remove_and_delete_item_and_emptyCamera;
                            /* 控制相机流的函数指针 */
                            rst->ro._streamControl = (void (*)(void*, unsigned char)) capture_private_localPush_item_streamControl_pthreadSafety;
                            /* 连接实时视频掩码通信 */
                            configManager_mask_connect_pthreadSafety(pCapture->ro._pConfigManager, "realStream", uuid, rst, (void (*)(const char*, const char*, unsigned char, void*)) capture_private_item_cb_streamControl, rst);
                            /* 控制 */
                            rst->ro._ctl_function = NULL;




                            //获取云台ip和端口
                            pd = fs_Config_node_string_get_first__IO(pConfig, pNode, "holderipv4");
                            if (NULL == pd) pd = fs_Config_node_string_get_first__IO(pConfig, pNode, "ipv4");

                            sprintf(rst->p.camera_ip, "%s", pd);
                            free(pd);

                            rst->p.camera_port = fs_Config_node_integer_get_first(pConfig, pNode, "holderPort", 0);

                            rst->p.shot_type = fs_Config_node_integer_get_first(pConfig, pNode, "shotType", 0);



                            rst->rw.camera_c_p = 0.0;
                            rst->rw.camera_c_t = 0.0;
                            rst->rw.camera_c_z = 0.0;
                            rst->rw.goto_p = 0.0;
                            rst->rw.goto_t = 0.0;
                            rst->rw.goto_z = 0.0;
                            rst->rw.m_action = 0;
                            rst->rw.m_stopAction = 100;
                            rst->rw.camera_speed = 0;
                            rst->rw.camera_time = 0;
                            rst->rw.m_auto_focus = 0;

                            /* 注册命令字 */
                            configManager_cmd_register(pCapture->ro._pConfigManager, "camctl", rst->ro._uuid, 0, rst, camctl_private_cmd_cb, NULL, rst);
                        } else {
                            /* 是gb28181相机,因统一使用一个端口号,故gb28181不支持创建多对象 */
                        }
                        free(cameraType);

                        //configManager_cmd_register(pCapture->ro._pConfigManager, "cameractrl", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? capture_private_cmd_cb : NULL, NULL, rst);
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    /* 注册命令字 */
                    /* 要死机 */
                    configManager_cmd_register(pCapture->ro._pConfigManager, "cameractrl", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? capture_private_cmd_cb : NULL, 0 == ipv4 ? rst : NULL, rst);
                    ////////////////////////////////////////////////////////////////////////////
                    /* 绑定命令字 */
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0)*ppCapture_item = rst;
                    else ppNodeCapture_item[index++] = rst;
                    ////////////////////////////////////////////////////////////////////////////
                    /* 释放内存空间 */
                    free(uuid);

                } while (--j > 0 && 0 == channel);
                fs_ObjectList_delete__OI(list);
            }
        } while (--i > 0 && 0 == channel);
        fs_ObjectList_delete__OI(clusterList);
        if (0 == channel && pCapture->p.clusterItemList) {
            if (0 == index) {
                fs_ObjectList_delete_custom__OI(pCapture->p.clusterItemList, (void (*)(void*))capture_private_item_delete__OI);
                pCapture->p.clusterItemList = NULL;
            } else

                while (pCapture->p.clusterItemList->nodeCount > index)capture_private_item_delete__OI((struct Capture_item*) fs_ObjectList_remove_tail(pCapture->p.clusterItemList));
        }
    }
}

static void *capture_private_thread(struct Capture* pCapture) {
    /* 创建线程监控 */
    MonitorItem *pMonitorItem = monitorItem_new_and_add(pCapture->ro._pMonitor,
            pCapture->ro._threadName, NULL, pCapture, pCapture->ro._parentTid);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *externRunStatus = pCapture->ro._externRunStatus;
    ConfigManager *pConfigManager = (ConfigManager*) pCapture->ro._pConfigManager;
    FsObjectList *itemList = pCapture->ro.__itemList;
    int pipeFd = pCapture->ro.__pipe[0];
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    // 通道数
    unsigned int channelCount = 0;
    // 相机对象
    FsObjectList *camera_rtsp_list = fs_ObjectList_new__IO(2);
    FsObjectList *camera_rtmp_list = fs_ObjectList_new__IO(2);
    FsObjectList *camera_gb28181_list = fs_ObjectList_new__IO(2);
    FsArray *ipList = NULL;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    struct Capture_item *pCapture_item, **pCapture_itemNode;
    struct timeval tv;
    fd_set rd;
    int i;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    pCapture->rw.resetChannel = -1;
    // 根据
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pCapture->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        //usleep(sleepTime);
        //if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pCapture->rw.resetChannel>-1) {
            /* 读取通道数 */
            if (ipList)free(ipList);
            ipList = fs_network_ipv4_get_all__IO();
            if (0 == pCapture->rw.resetChannel) {
                channelCount = capture_private_get_channelCount(pConfigManager->ro.__pConfig, ipList);
                /* 删除通道 */
                while (itemList->nodeCount > channelCount) {
                    pCapture_item = (struct Capture_item *) fs_ObjectList_remove_tail(itemList);
                    if (pCapture_item)capture_private_item_delete__OI(pCapture_item);
                }
                /* 添加通道 */
                while (itemList->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList, NULL);
                /* 重置集群 */
                pCapture->rw.resetChannel = -1;
                capture_private_item_new(pCapture, 0, ipList, NULL, camera_rtsp_list, camera_rtmp_list, camera_gb28181_list);
            }
            /* 重置 */
            if (pCapture->rw.resetChannel > 0) {
                if (itemList->nodeCount < (unsigned int) pCapture->rw.resetChannel) {
                    /* 出错 */
                    FsPrintf(1, "Invalid channel(=%u/%lu).\n", pCapture->rw.resetChannel, itemList->nodeCount);
                    fflush(stdout);
                }
                pMonitorItem->rw.line = __LINE__;
                capture_private_item_new(pCapture, pCapture->rw.resetChannel, ipList, (struct Capture_item **) (itemList->pNode + itemList->startIndex + pCapture->rw.resetChannel - 1),
                        camera_rtsp_list, camera_rtmp_list, camera_gb28181_list);
                pMonitorItem->rw.line = __LINE__;
                pCapture->rw.resetChannel = -1;
            }
        }
        ////////////////////////////////////////////////////////////////////
        //在这里进行相机所有操作....
        int channelindex = 0;
        for (channelindex = channelCount, pCapture_itemNode = (struct Capture_item **) itemList->pNode + itemList->startIndex;
                channelindex > 0; channelindex--) {
            pCapture_item = *pCapture_itemNode++;
            if (pCapture_item == NULL)continue;
        }

        FD_ZERO(&rd);
        FD_SET(pipeFd, &rd);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        if (select(pipeFd + 1, &rd, NULL, NULL, &tv) > 0) {

            FsPrintf(1, "***********select*********\n");
            fflush(stdout);

            if (read(pipeFd, &tv, 1) != 1) {
                FsPrintf(6, "Read error:\"%s\".", strerror(errno));
                fflush(stdout);
            }
        }
        /////////////////////////////////////////////////////////////////////

    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 删除各通道 */
    {
        struct Capture_item **ppNode = (struct Capture_item **) itemList->pNode + itemList->startIndex;
        for (i = itemList->nodeCount; i > 0; i--) {
            pCapture_item = *ppNode++;
            if (pCapture_item)capture_private_item_delete__OI(pCapture_item);
        }
        fs_ObjectList_clear(itemList);
        /* 删除集群 */
        if (pCapture->p.clusterItemList) {
            fs_ObjectList_delete_custom__OI(pCapture->p.clusterItemList, (void (*)(void*))capture_private_item_delete__OI);
            pCapture->p.clusterItemList = NULL;
        }
    }
#ifdef FsDebug
    /* 检查各类相机是否释放完 */
    if (camera_rtsp_list->nodeCount > 0) {
        FsPrintf(1, "Error Camera_rtsp,count=%lu.\n", camera_rtsp_list->nodeCount);
        fflush(stdout);
    }
    if (camera_gb28181_list->nodeCount > 0) {
        FsPrintf(1, "Error Camera_gb28181,count=%lu.\n", camera_gb28181_list->nodeCount);
        fflush(stdout);
    }
#endif
    /* 释放空间 */
    /* 释放全局变量空间 */
    fs_ObjectList_delete__OI(camera_rtsp_list);
    fs_ObjectList_delete__OI(camera_rtmp_list);
    fs_ObjectList_delete__OI(camera_gb28181_list);
    if (ipList)free(ipList);
    /* 释放中间变量空间 */
    ////////////////////////////////////////////////////////////////////////////
    FsPrintf2(1, "Thread:\"%s\" exit,runStatus=%hhd.\n", pCapture->ro._threadName, pCapture->rw.runStatus);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pCapture->ro._pMonitor, pMonitorItem);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void capture_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "cameraConfig", "相机配置", "相机配置", 0, 0x7);

    void *pNode = fs_Config_node_string_add(pConfig, parent, "cameraType", "相机类型", "相机类型", 0, 0x7, 1, 16, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "rtmp", "rtmp相机", "rtmp相机");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "localPush", "本地推送相机", "本地推送相机,数据是其他通道根据uuid直接推送");

    pNode = fs_Config_node_integer_add(pConfig, parent, "streamMask", "数据流掩码", "数据流掩码,1-视频,2-音频,4-相机控制", 0, 0x7, 0, 4, 3);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "视频", "视频");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "音频", "音频");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "相机控制", "相机控制");

    pNode = fs_Config_node_string_add(pConfig, parent, "gb28181_id", "gb28181本机ID", "gb28181本机ID", 0, 0x7, 1, 20, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "34020000002000000001", "34020000002000000001", "34020000002000000001");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");

    pNode = fs_Config_node_string_add(pConfig, parent, "gb28181_sip_id", "gb28181平台或相机ID", "gb28181平台或相机ID", 0, 0x7, 1, 20, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "34020000001320000001", "34020000001320000001", "34020000001320000001");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");

    pNode = fs_Config_node_string_add(pConfig, parent, "gb28181_sip_domain", "gb28181平台或相机域", "gb28181平台或相机域,本机不校验此值", 0, 0x7, 1, 20, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "3402000000", "3402000000", "3402000000");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");

    pNode = fs_Config_node_string_add(pConfig, parent, "addr_info", "本机使用的地址信息", "本机使用的地址信息,一般在NAT环境中使用,为空时程序自动计算", 0, 0x7, 1, 22, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.10.254:5060", "192.168.10.254:5060", "192.168.10.254:5060");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "gb28181");

    pNode = fs_Config_node_string_add(pConfig, parent, "ipv4", "相机或平台的IP或域名", "相机或平台的IP或域名,本机做gb28181平台上级时允许为空", 0, 0x7, 1, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.50.252", "192.168.50.252", "192.168.50.252");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal_false, "localPush");

    pNode = fs_Config_node_integer_add(pConfig, parent, "rtspPort", "rts[m]p或平台端口", "rts[m]p或平台端口,本机做gb28181平台上级时允许为空", 0, 0x7, 1, 65534, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 554, "554", "554");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1935, "1935", "1935");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal_false, "localPush");

    pNode = fs_Config_node_string_add(pConfig, parent, "rtspURL", "rts[m]pURL或平台的通道编号", "rts[m]pURL,必须以/开头,不能为空", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "/", "/", "/");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "34020000001320000002", "34020000001320000002", "34020000001320000002");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal_false, "localPush");

    pNode = fs_Config_node_string_add(pConfig, parent, "rstpUsername", "rtsp或平台用户名", "rtsp或平台用户名,可为空", 0, 0x7, 0, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "34020000001320000001", "34020000001320000001", "34020000001320000001");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal_false, "localPush");

    pNode = fs_Config_node_string_add(pConfig, parent, "rstpPasswd", "rtsp或平台密码", "rtsp或平台密码,可为空", 0, 0x7, 0, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "12345678", "12345678", "12345678");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal_false, "localPush");

    pNode = fs_Config_node_string_add(pConfig, parent, "channelUUID", "相机通道标识", "相机通道标识,可为空,对同一ip有多相机的控制可能需要些参数", 0, 0x7, 0, 128, 1);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtmp");

    pNode = fs_Config_node_string_add(pConfig, parent, "holderipv4", "相机云台IP", "相机云台IP", 0, 0x7, 1, 16, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "192.168.50.252", "192.168.50.252", "192.168.50.252");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");

    pNode = fs_Config_node_integer_add(pConfig, parent, "holderPort", "相机云台端口", "相机云台端口", 0, 0x7, 1, 65534, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 4001, "4001", "4001");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");

    pNode = fs_Config_node_integer_add(pConfig, parent, "cameraModel", "相机型号(0：白光，1：红光)", "相机型号", 0, 0x7, 0, 65534, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");

    pNode = fs_Config_node_integer_add(pConfig, parent, "shotType", "光电仪镜头类型(0：反比例，1：正比例)", "光电仪镜头类型", 0, 0x7, 1, 65534, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "cameraType", FsConfig_Condition_equal, "rtsp");
}

/* 
 * 创建一个新的Capture对象;
 * 返回Capture指针.
 */
struct Capture *capture_new__IO(/* 线程名 */const char threadName[],
        /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
        /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
        /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
        /* 配置管理对象 */ void *const pConfigManager, /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread, /* rtmp客户端对象 */ Rtmp * const pRtmpClient) {
    unsigned int len = strlen(threadName) + 1;
    struct Capture* rst = (struct Capture*) malloc(sizeof (struct Capture) +len);
    memset(rst, 0, sizeof (struct Capture));
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

    /* 管道,用于添加和删除文件或发送数据 */
    if (pipe(rst->ro.__pipe)) {
        FsPrintf(5, "Pipe error:\"%s\".\n", strerror(errno));
        fflush(stdout);
    } else {
        /* 设置非阻塞模式 */
        len = 1;
        if (ioctl(rst->ro.__pipe[0], FIONBIO, &len) != 0) {
            FsPrintf(6, "Ioctl error:\"%s\",fd=%d.\n", strerror(errno), rst->ro.__pipe[0]);
            fflush(stdout);
        }
        len = 1;
        if (ioctl(rst->ro.__pipe[1], FIONBIO, &len) != 0) {
            FsPrintf(6, "Ioctl error:\"%s\",fd=%d.\n", strerror(errno), rst->ro.__pipe[1]);
            fflush(stdout);
        }
    }
    /* 每个线程最多处理的通道数 */
    rst->ro._channelCountPerThread = channelCountPerThread;
    /* rtmp客户端对象 */
    rst->ro._pRtmpClient = pRtmpClient;

    return rst;
}

/* 删除pCapture指向的实例对象 */
void capture_delete__OI(struct Capture * const pCapture) {

    /* 通道链表 */
    fs_ObjectList_delete__OI(pCapture->ro.__itemList);

    /* 管道, 唤醒主线程*/
    close(pCapture->ro.__pipe[0]);
    close(pCapture->ro.__pipe[1]);

    free(pCapture);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void capture_startThread(struct Capture* pCapture,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice) {
    if (pCapture->rw.runStatus == 0 && pCapture->ro.stid == 0) {
        pCapture->rw.runStatus = 1;
        pCapture->rw.nice = nice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n\
  itemList=%#lx\n  pipe=%d-%d\n  channelCountPerThread=%hu\n  pRtmpClient=%#lx\n",
                pCapture->ro._threadName,
                (unsigned long) pCapture->ro._pMonitor,
                pCapture->ro._parentTid,
                (unsigned long) pCapture->ro._externRunStatus,
                (long) pCapture->ro.__itemList,
                pCapture->ro.__pipe[0], pCapture->ro.__pipe[1],
                pCapture->ro._channelCountPerThread,
                (long) pCapture->ro._pRtmpClient);
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
        if (pthread_create(&pCapture->ro.stid, &attr, (void* (*)(void*))capture_private_thread, pCapture)) {
            FsPrintf(1, "Create thread error:\"%s\".\n", strerror(errno));
            fflush(stdout);
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n\
  itemList=%#lx\n  pipe=%d-%d\n  channelCountPerThread=%hu\n  pRtmpClient=%#lx\n", pCapture->rw.runStatus, pCapture->ro.stid,
                pCapture->ro._threadName,
                (unsigned long) pCapture->ro._pMonitor,
                pCapture->ro._parentTid,
                (unsigned long) pCapture->ro._externRunStatus,
                (long) pCapture->ro.__itemList,
                pCapture->ro.__pipe[0], pCapture->ro.__pipe[1],
                pCapture->ro._channelCountPerThread,
                (long) pCapture->ro._pRtmpClient);
        fflush(stdout);
    }
}

/* 结束线程 */
void capture_stopThread(struct Capture* pCapture) {
    pCapture->rw.runStatus = 0;
    if (pCapture->ro.stid) {
#ifdef FsDebug

        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pCapture->ro.stid, 0);
        pCapture->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
FsObjectImageFrame *capture_item_frame_out_pthreadSafety_IO(/* 采集项,可为空 */ struct Capture_item* pCapture_item) {
    if (pCapture_item == NULL) {
        FsPrintf(1, "%#lx\n", (long) pCapture_item);
        fflush(stdout);
    }
    if (pCapture_item->ro.__framelistOut->nodeCount > 0)return (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pCapture_item->ro.__framelistOut);
    else return NULL;
}

///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
