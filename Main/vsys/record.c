#include "record.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "capture.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////

/* 缓存录像时每条数据的结构,要求创建此对象时必须分配连续内存 */
struct Record_private_VideoBuffer_item {
    /* 帧号 */
    unsigned long frameIndex;
    /* 采集时间,用系统开机时间 */
    double captureTime_uptime;
    /* 数据帧类型,0表示普通数据类型,1表示关键帧,对jpeg来说都是关键帧 */
    unsigned char keyFrame : 1;
    /* 数据类型,1表示jpeg,2表示H264 */
    unsigned char type : 2;
    /* 文件句柄 */
    FILE *fd;
    /* 偏移量,绝对位置 */
    unsigned long offset;
    /* 数据长度 */
    unsigned long dataLenth;
    /* 图像宽度 */
    unsigned int width;
    /* 图像高度 */
    unsigned int height;
};

/* 录像截取对象的结构,要求创建此对象时必须分配连续的空间 */
struct Record_private_PartVideo_item {
    /* 开始时间,推荐使用CPU时钟 */
    double startTime;
    /* 结束时间,推荐使用CPU时钟 */
    double endTime;
    /* 文件名,绝对路径 */
    char *fileName;
    /* 字幕 */
    char *subtitle;
};

/* 录像回放 */
struct Record_private_VideoDemand {
    /* 帧号,各通道的帧号 */
    unsigned int *frameIndex;
    /* 父目录长度 */
    unsigned int dirLen;
    /* 当前文件名 */
    char *thisFileName;
    /* 文件名链表,第一个为当前文件名 */
    FsStringList *fileNameList;
    /* 当前文件的开始时间 */
    double fileStartTime;
    /* 视频文件对象 */
    FILE *fd;
    /* 视频对象 */
    VideoMKV *pMKV;
    /* 用户指定的帧率,大于0有效 */
    double fpsSet;
    /* 当前使用的帧率 */
    double fps;
    /* 文件时长 */
    double fileTime;
    /* 数据发送类型(0--正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送) */
    unsigned char sendType : 3;
    /* 已发送的帧数 */
    unsigned long sendCount;
    /* 视频必须发完的时间 */
    double endUptime;
};

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

static inline unsigned int record_private_item_ClusterLen_get() {
    struct Record_item item;
    return ((long) (&item.ro.__pHlsServer_item)-(long) (&item) + sizeof (void*));
}

/* 从配置获取通道数 */
static unsigned int record_private_get_channelCount(/* 可为空 */FsConfig *pConfig, /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return 0;
    /* 通道数 */
    FsObjectList * list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
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

/* 删除pVideoDemand指向的实例对象 */
static void record_private_VideoDemand_delete__OI(struct Record_private_VideoDemand* pVideoDemand) {
    /* 文件名链表,第一个为当前文件名 */
    if (pVideoDemand->fileNameList) fs_StringList_delete__OI(pVideoDemand->fileNameList);
    /* 视频对象 */
    if (pVideoDemand->pMKV)video_mkv_delete__OI(pVideoDemand->pMKV);
    /* 视频文件对象 */
    if (pVideoDemand->fd)fclose(pVideoDemand->fd);

    free(pVideoDemand);
}

/* 读取检查文件是否在扩展,成功返回1,失败返回-1 */
static inline signed char record_private_VideoDemand_file_extern(struct Record_private_VideoDemand * const pVideoDemand) {
    //    fseek(pVideoDemand->fd, 0, SEEK_END);
    //    FsPrintf(1, "%lld\n", ftell(pVideoDemand->fd));
    //    return -1;

    VideoMKV *pMKV = pVideoDemand->pMKV;
    /* 尝试重新打开 */
    //FsPrintf(1, "%s.\n", pVideoDemand->thisFileName);
#if 0
    FILE *fd = fopen(pVideoDemand->thisFileName, "rb");
    if (fd) {
        fclose(pVideoDemand->fd);
        pVideoDemand->fd = fd;
        signed char rv = video_mkv_file_try_read(pMKV, fd);
        //fflush(pVideoDemand->fd);

#else
    if (1) {
        signed char rv = video_mkv_file_try_read(pMKV, NULL);
#endif
        if (1 == rv) {
            /* 扩展成功 */
            if (pMKV->videoIndex) {
                /* 有视频 */
                /* 当前使用的帧率 */
                if (pVideoDemand->fpsSet <= 0.0) pVideoDemand->fps = pMKV->channel[pMKV->videoIndex].p.video.frameRate;
                /* 文件时长 */
                pVideoDemand->fileTime = pMKV->channel[pMKV->videoIndex].list->nodeCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
                /* 视频必须发完的时间 */
                pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps;
            } else {
                /* 无视频 */
                /* 当前使用的帧率 */
                if (pVideoDemand->fpsSet <= 0.0) pVideoDemand->fps = pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
                /* 文件时长 */
                pVideoDemand->fileTime = pMKV->channel[pMKV->audioIndex].list->nodeCount / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
                /* 视频必须发完的时间 */
                pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps;
            }
            return 1;
        } else {
            if (rv < 0) {
                /* 失败,是什么原因暂不清楚 */
                FsPrintf(1, "Extrn the file:\"%s\" failed", pVideoDemand->thisFileName);
                fflush(stdout);
            }
            return -1;
        }
    } else {
        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", pVideoDemand->thisFileName, strerror(errno));
        fflush(stdout);
        return -1;
    }
}

/* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */
static signed char record_private_item_add_partVideo_compare(struct Record_private_PartVideo_item *node1, struct Record_private_PartVideo_item * node2) {
    if (node1->endTime > node2->endTime)return 1;
    else if (node1->endTime < node2->endTime)return -1;

    else return 0;
}

/* 删除与录像相关的变量 */
static void record_private_item_clear_video(struct Record_item * pRecord_item) {
    if (pRecord_item->p._recordVideoFile_fd) {

        video_mkv_file_save_finish(pRecord_item->p._pMKV);
        video_mkv_delete__OI(pRecord_item->p._pMKV);
        pRecord_item->p._recordVideoFile_fd = NULL;
    }
}

/* 删除pRecord_item指向的实例对象 */
static void record_private_item_delete__OI(struct Record_item * pRecord_item) {
    if (NULL == pRecord_item)return;
    /* 注销命令字 */
    FsPrintf(1, "%s-%hhu.%hhu.%hhu.%hhu.\n", pRecord_item->ro._uuid, ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n1, ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n2,
            ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n3, ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n4);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "video_part", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "videolist_get", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "videotimeinfo_get", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item);
    if (0 == pRecord_item->ro._ipv4)configManager_cmd_disconnect(pRecord_item->ro._pRecord->ro._pConfigManager, "cameractrl", pRecord_item->ro._uuid, pRecord_item);
    ////////////////////////////////////////////////////////////////////////////
    /* 视频转发+集群 */
    if (pRecord_item->ro.__pRtmpServer_item) {
        rtmpServer_item_delete__OI(pRecord_item->ro.__pRtmpServer_item);
        if (0 == pRecord_item->ro._ipv4) {
            /* 注销rtmp实时视频掩码通信 */
            configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_rtmp);
        }
    }
    if (0 == pRecord_item->ro._ipv4) {
        /* 注销录像的掩码 */
        configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_record);
        /* 内部数据推送,为空表示不推送 */
        if (pRecord_item->ro._localPush) {
            /* 解绑定命令字 */
            configManager_cmd_disconnect(pRecord_item->ro._pRecord->ro._pConfigManager, "localPush", pRecord_item->ro._localPush, pRecord_item);
            /* 断开实时视频掩码通信 */
            configManager_mask_disconnect_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._localPush, pRecord_item);
            /* 注销本地推送实时视频掩码通信 */
            configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_localPush);
        }
        /* 连接和调用外部命令所需要的同步锁 */
        pthread_mutex_destroy(&pRecord_item->ro.__mutexCmdConnect);
        /* 保存录像相关 */
        record_private_item_clear_video(pRecord_item);
        /* 传入的数据帧链表 */
        fs_ObjectList_delete_custom__OI(pRecord_item->ro.__framelistIn, fs_Object_delete_pthreadSafety__OI);
        /* 传出的数据帧链表 */
        fs_ObjectList_delete_custom__OI(pRecord_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
        /* 视频缓存链表 */
        if (pRecord_item->ro.__videoCacheList->nodeCount > 0) {
            struct Record_private_VideoBuffer_item *pVideoBuffer_item, **pNode = (struct Record_private_VideoBuffer_item **) (pRecord_item->ro.__videoCacheList->pNode +
                    pRecord_item->ro.__videoCacheList->startIndex);
            unsigned long l = pRecord_item->ro.__videoCacheList->nodeCount;
            FILE *fd = pNode[0]->fd;
            do {
                pVideoBuffer_item = *pNode++;
                if (pVideoBuffer_item->fd != fd) {
                    fclose(fd);
                    fd = pVideoBuffer_item->fd;
                }
                free(pVideoBuffer_item);
            } while (--l > 0);
            fclose(fd);
        }
        fs_ObjectList_delete__OI(pRecord_item->ro.__videoCacheList);
        /* 视频合成任务链表,任务项按结束时间排序 */
        fs_ObjectList_delete_allSequence_OI(pRecord_item->ro.__videoPartList);
        /* 录像信息 */
        if (pRecord_item->ro.__videoInfo)fclose(pRecord_item->ro.__videoInfo);
    }
    free(pRecord_item);
    FsPrintf(1, "TTTTTTTTTTTTTTTTTTTTTTT\n");
}

/* 检查数据是否可发送的函数指针,成功返回1,出错返回-1,0表示还需要等待 */
static signed char record_private_videoPart_condition(void *p, /* 发送的数据 */const FsString * const sendData) {
    struct stat statbuf;
    if (lstat(((char*) p) + sizeof (unsigned long), &statbuf) == 0) {
        if (*((unsigned long*) p) == (unsigned long) statbuf.st_size)return 1;
        *((unsigned long*) p) = statbuf.st_size;
    }
    return 0;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
signed char record_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
        /* 收到数据的前4字节 */unsigned int head, /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p) {
    struct FsEbml_node *pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_type");
    if (strcmp("video_part", pEbml_node->data.buffer) == 0) {
        /* 录像合成 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time_start");
        double startTime, endTime;
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth || (startTime = fs_time_get_string(pEbml_node->data.buffer)) < 0.1) {
            FsPrintf(1, "Invalid startTime,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            startTime = fs_time_GMT_get();
        }
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time_end");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth || (endTime = fs_time_get_string(pEbml_node->data.buffer)) < 0.1) {
            FsPrintf(1, "Invalid endTime,data:\n");
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            endTime = startTime + 5.0;
        }
        if (startTime + 1.0 > endTime) {
            FsPrintf(1, "Invalid time(=%lf/%lf),data:\n", startTime, endTime);
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            return -1;
        } else {
            double thisTime = fs_time_uptime_get();
            FsPrintf2(1, "Time(=%lf/%lf,gmttime=%lf),data:\n", startTime, endTime, fs_time_GMT_get());
            fs_Ebml_out_debug(pEbml, stdout), printf("\n");
            struct Record_item *pRecord_item = (struct Record_item*) p;
            time_t ts = (time_t) startTime;
            struct tm ti_s;
            localtime_r(&ts, &ti_s);
            ts = (time_t) endTime;
            struct tm ti_e;
            localtime_r(&ts, &ti_e);
            char *buffer = pRecord_item->ro._tmpBufferForConfig1;
            sprintf(buffer, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir,
                    pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
                    1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec);
            endTime -= startTime;
            startTime = thisTime + (startTime - fs_time_GMT_get());
            endTime += startTime;
            record_item_add_partVideo(pRecord_item, buffer, startTime, endTime, NULL);
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
            buffer += strlen("/fs/project/data");
            int len = strlen(buffer);
            FsString *pString = fs_String_new__IO(8 + len + sizeof (unsigned int)*3);
            pString->lenth -= sizeof (unsigned int)*3;
            *((unsigned int *) pString->buffer) = head, *((unsigned int *) (pString->buffer + 4)) = len;
            pString->buffer[0] &= 0xF0;
            pString->buffer[0] |= 0x2;
            memcpy(pString->buffer + 8, buffer, len), memcpy(pString->buffer + 8 + len, requestID_3, sizeof (unsigned int)*3);

            buffer = (char*) malloc(sizeof (unsigned long) +strlen(pRecord_item->ro._tmpBufferForConfig1) + 1);
            *((unsigned long*) buffer) = ~0L, strcpy(buffer + sizeof (unsigned long), pRecord_item->ro._tmpBufferForConfig1);
            configManager_send_pthreadSafety__OI_3(pRecord_item->ro._pRecord->ro._pConfigManager, endTime > thisTime ? endTime + 1.0 + (endTime - startTime) / 8 : thisTime + 1.0 + (endTime - startTime) / 8,
                    pString, record_private_videoPart_condition, buffer, free);
            return 1;
        }
    } else if (strcmp("videolist_get", pEbml_node->data.buffer) == 0) {
        return -1;
    } else if (strcmp("videotimeinfo_get", pEbml_node->data.buffer) == 0) {
        /* 获取录像记录的时间信息 */
        return -1;
    } else {
        FsPrintf(1, "Invalid cmd:\"%s\".\n,data:\n", pEbml_node->data.buffer);
        fs_Ebml_out_debug(pEbml, stdout), printf("\n");

        return -1;
    }
}

/* 在期望连接的命令字和uuid改变时调用 */
static void record_private_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p,
        /* 关联的对象 */ struct Record_item * const pRecord_item) {
    if (strcmp("cameractrl", cmd) == 0 && strcmp(uuid, pRecord_item->ro._uuid) == 0) {
    } else if (pRecord_item->ro._localPush != NULL && strcmp("localPush", cmd) == 0 && strcmp(uuid, pRecord_item->ro._localPush) == 0) {
        pthread_mutex_lock(&pRecord_item->ro.__mutexCmdConnect);
        pRecord_item->p.localPushCameraFun = (capture_loaclPush_frame_pthreadSafety) do_local;
        pRecord_item->p.pLocalPushCamera = do_local_p;
        pthread_mutex_unlock(&pRecord_item->ro.__mutexCmdConnect);
    } else {
        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pRecord_item->ro._uuid);
        fflush(stdout);
    }
}

/* 用于rtsp服务器在有实时客户端时的回调函数 */
static void record_private_cb_realRtsp(/* 状态,0-表示无客户端,1-表示有客户端 */const unsigned char status, struct Record_item * const pRecord_item) {
    configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro.__realMask, pRecord_item->ro._maskIndex_rtsp, status);
    FsPrintf(1, "-------------RtspSatus:[%hhu]-----\n", status);
    fflush(stdout);
}

/* 用于rtsp服务器在有实时客户端时的回调函数 */
static void record_private_cb_realRtmp(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, struct Record_item * const pRecord_item) {
    if (index < 0) {
        configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro.__realMask, pRecord_item->ro._maskIndex_rtmp, index + 2);
        FsPrintf(1, "-------------RtmpSatus:[%d]-----\n", index + 2);
        fflush(stdout);
        //configManager_mask_out_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, stdout);
    } else {
        FsPrintf(1, "-------------RtmpSatus:[%d]-----\n", index);
        fflush(stdout);
    }
}

/* 用于hls服务器在有实时客户端时的回调函数 */
static void record_private_cb_realHls(/* 状态,0-表示无客户端,1-表示有客户端 */const unsigned char status, struct Record_item * const pRecord_item) {
    configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro.__realMask, pRecord_item->ro._maskIndex_hls, status);
    FsPrintf(1, "-------------HlsSatus:[%hhu]-----\n", status);
    fflush(stdout);
}

/* 用于推送目的端在有实时客户端时的回调函数 */
static void record_private_item_cb_realLocalPush(/* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 状态,0-全0,1-部分为1 */ const unsigned char status, /* 关联的对象 */ struct Record_item * const pRecord_item) {
    configManager_mask_value_set(pRecord_item->ro.__realMask, pRecord_item->ro._maskIndex_localPush, status);
    FsPrintf(1, "-------------LocalPushSatus:[%hhu]-----\n", status);
    fflush(stdout);
}

static void record_private_item_new(struct Record * const pRecord, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList,
        /* 原来的采集项 */struct Record_item * * const ppRecord_item, void * const pRtspServer, Rtmp * const pRtmp, void * const pHls, void * const pGB28181Server) {
    FsConfig *pConfig = ((ConfigManager*) pRecord->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    FsObjectList * clusterList = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0 == channel, "vsys");
    if (clusterList) {
        void *parent, **ppNode, **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int i = clusterList->nodeCount, j, index = 0, ipv4, len;
        FsObjectList *list;
        struct Record_item **ppNodeRecord_item;
        char *uuid, *localPush, *rtspServerURL, *pd, *gb28181ID, *gb28181Name;
        struct Record_item * rst;
        unsigned long long streamMask;
        do {
            parent = *ppNodeCluster++;
            list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
            if (list) {
                ppNode = list->pNode + list->startIndex;
                j = list->nodeCount;
                if (0 == channel) {
                    if (NULL == pRecord->p.clusterItemList) pRecord->p.clusterItemList = fs_ObjectList_new__IO(4);
                    while (pRecord->p.clusterItemList->nodeCount < index + j)fs_ObjectList_insert_tail(pRecord->p.clusterItemList, NULL);
                    ppNodeRecord_item = (struct Record_item**) pRecord->p.clusterItemList->pNode + pRecord->p.clusterItemList->startIndex;
                    uuid = fs_Config_node_string_get_first__IO(pConfig, parent, "hostname");
                    pd = strchr(uuid, ':');
                    ipv4 = fs_ipv4_network_get(pd ? pd - uuid : strlen(uuid), uuid);
                    free(uuid);
                } else ipv4 = 0;
                do {
                    if (0 == channel)parent = *ppNode++;
                    else parent = ppNode[channel - 1];
                    uuid = fs_Config_node_string_get_first__IO(pConfig, parent, "uuid");
                    streamMask = fs_Config_node_integer_get_mask(pConfig, parent, "cameraConfig streamMask");
                    parent = fs_Config_node_get_first(pConfig, parent, "recordConfig");
                    rtspServerURL = fs_Config_node_string_get_first__IO(pConfig, parent, "rtspServerURL");
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        if (ppNodeRecord_item[index])record_private_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppRecord_item)record_private_item_delete__OI(*ppRecord_item);
                    len = strlen(uuid) + 1;
                    if (channel > 0) {
                        localPush = fs_Config_node_string_get_first__IO(pConfig, parent, "localPush");
                        if (localPush)len += strlen(localPush) + 1;
                        /* 本线程函数要用到的临时空间 */
                        // 实时录像文件名 %s%s/video/20151007/23/video20151007230000.mkv,最大42+;
                        /* 配置线程函数要用到的临时空间 */
                        // 实时录像文件名 %s%s/video/20151007/23/video20151007230000.mkv,最大42+;
                        // 视频合成文件名 %s%s/incident/20151007/23/inc20151014185944-20151014185954.mkv,最大58+;   
                        rst = (struct Record_item*) malloc(sizeof (struct Record_item) +len + (strlen(pRecord->ro._dir) + len + 42)+(strlen(pRecord->ro._dir) + len + 58)*2);
                        memset(rst, 0, sizeof (struct Record_item));
                        pd = (char*) (rst + 1);
                    } else {
                        rst = (struct Record_item*) malloc(record_private_item_ClusterLen_get() + len);
                        memset(rst, 0, record_private_item_ClusterLen_get());
                        pd = ((char*) rst) + record_private_item_ClusterLen_get();
                    }
                    /* 本对象的uuid值,一般是从配置中读入的 */
                    rst->ro._uuid = pd, len = strlen(uuid) + 1, memcpy(pd, uuid, len), pd += len;
                    /* 所属主机的ip地址,用于集群,本机为0 */
                    rst->ro._ipv4 = ipv4;
                    /* 所属的记录对象 */
                    rst->ro._pRecord = pRecord;
                    /* 视频转发+历史点播+集群 */
                    if (rtspServerURL) {
                        /* rtmp实时视频掩码 */
                        if (channel > 0) {
                            /* 注册rtsp实时视频掩码通信 */
                            rst->ro._maskIndex_rtmp = configManager_mask_register_pthreadSafety(&rst->ro.__realMask, pRecord->ro._pConfigManager, "realStream", uuid, Record_BaseLine + __LINE__);
                            rst->ro.__pRtmpServer_item = rtmpServer_item_new__IO(rtspServerURL + 1, ipv4, 1, (void (* const) (const int, void * const))record_private_cb_realRtmp, rst);
                        } else rst->ro.__pRtmpServer_item = rtmpServer_item_new__IO(rtspServerURL + 1, ipv4, 1, NULL, NULL);
                        rtmpServer_add_item__OI_2(pRtmp, rst->ro.__pRtmpServer_item);
                    }
                    if (channel > 0) {
                        /* 连接和调用外部命令所需要的同步锁 */
                        pthread_mutex_init(&rst->ro.__mutexCmdConnect, NULL);
                        /* gb28181视频转发 */
                        /* 视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-不记录,但保留已储存的数据 */
                        rst->ro._recordVideoMode = fs_Config_node_integer_get_first(pConfig, parent, "recordVideoMode", 0);
                        /* 录像的掩码 */
                        rst->ro._maskIndex_record = configManager_mask_register_pthreadSafety(&rst->ro.__realMask, pRecord->ro._pConfigManager, "realStream", uuid, Record_BaseLine + __LINE__);
                        if (rst->ro._recordVideoMode) {
                            configManager_mask_value_set_pthreadSafety(pRecord->ro._pConfigManager, rst->ro.__realMask, rst->ro._maskIndex_record, 1);
                        }
                        /* 实时视频保存的最长时间,单位秒,0表示不限制 */
                        if (rst->ro._recordVideoMode) rst->ro._realVideoSaveTime = fs_Config_node_integer_get_first(pConfig, parent, "realVideoSaveTime", 0);
                        else rst->ro._realVideoSaveTime = 1;
                        /* 内部数据推送,为空表示不推送 */
                        if (localPush) {
                            rst->ro._localPush = pd, len = strlen(localPush) + 1, memcpy(pd, localPush, len), pd += len;
                            /* 注册本地推送实时视频掩码通信 */
                            rst->ro._maskIndex_localPush = configManager_mask_register_pthreadSafety(&rst->ro.__realMask, pRecord->ro._pConfigManager, "realStream", uuid, Record_BaseLine + __LINE__);
                            /* 连接实时视频掩码通信 */
                            configManager_mask_connect_pthreadSafety(pRecord->ro._pConfigManager, "realStream", localPush, rst, (void (*)(const char*, const char*, unsigned char, void*)) record_private_item_cb_realLocalPush, rst);
                            /* 绑定命令字 */
                            configManager_cmd_connect(pRecord->ro._pConfigManager, "localPush", localPush, rst, (void (*)(const char*, const char*, void*, void*, void*))record_private_cmd_connect_cb, rst);
                            free(localPush);
                        }
                        /* 传入的数据帧链表 */
                        rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                        /* 传出的数据帧链表 */
                        rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                        /* 视频缓存链表 */
                        rst->ro.__videoCacheList = fs_ObjectList_new__IO(10);
                        /* 视频合成任务链表,任务项按结束时间排序 */
                        rst->ro.__videoPartList = fs_ObjectList_new__IO(4);
                        /* 录像文件的周期,单位秒 */
                        rst->ro._recordVideoFileInterval = 60;
                        /* 本线程函数要用到的临时空间 */
                        rst->ro._tmpBufferForThis = pd, pd += (strlen(pRecord->ro._dir) + len + 42);
                        /* 配置线程函数要用到的临时空间1 */
                        rst->ro._tmpBufferForConfig1 = pd, pd += (strlen(pRecord->ro._dir) + len + 58);
                        /* 配置线程函数要用到的临时空间2 */
                        rst->ro._tmpBufferForConfig2 = pd;
                        /* 记录的信息,用于记录录像的有无 */
                        if (rst->ro._recordVideoMode) {
                            sprintf(rst->ro._tmpBufferForConfig1, "%s%s/video/__info.txt", pRecord->ro._dir, uuid);
                            rst->ro.__videoInfo = fopen(rst->ro._tmpBufferForConfig1, "ab+");
                        }
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    /* 注册命令字 */
                    configManager_cmd_register(pRecord->ro._pConfigManager, "video_part", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, 0 == ipv4 ? (void*) record_item_add_partVideo : NULL, rst);
                    configManager_cmd_register(pRecord->ro._pConfigManager, "videolist_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                    configManager_cmd_register(pRecord->ro._pConfigManager, "videotimeinfo_get", rst->ro._uuid, ipv4, rst, 0 == ipv4 ? record_private_cmd_cb : NULL, NULL, rst);
                    ////////////////////////////////////////////////////////////////////////////
                    /* 绑定命令字 */
                    if (channel > 0) configManager_cmd_connect(pRecord->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))record_private_cmd_connect_cb, rst);
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0)*ppRecord_item = rst;
                    else ppNodeRecord_item[index++] = rst;
                    ////////////////////////////////////////////////////////////////////////////
                    /* 释放内存空间 */
                    free(uuid);
                    if (rtspServerURL)free(rtspServerURL);
                } while (--j > 0 && 0 == channel);
                fs_ObjectList_delete__OI(list);
            }
        } while (--i > 0 && 0 == channel);
        fs_ObjectList_delete__OI(clusterList);
        if (0 == channel && pRecord->p.clusterItemList) {
            if (0 == index) {
                fs_ObjectList_delete_custom__OI(pRecord->p.clusterItemList, (void (*)(void*))record_private_item_delete__OI);
                pRecord->p.clusterItemList = NULL;
            } else
                while (pRecord->p.clusterItemList->nodeCount > index)record_private_item_delete__OI((struct Record_item*) fs_ObjectList_remove_tail(pRecord->p.clusterItemList));
        }
    }
}

/* 缓存视频信息 */
static void record_private_item_addVideoBuffer(/* 记录项,保证有一帧数据可用 */struct Record_item *pRecord_item, /* 帧号 */const unsigned long frameIndex, /* 采集时间,用系统开机时间 */const double captureTime_uptime,
        /* 数据帧类型,0表示普通数据类型,1表示关键帧,对jpeg来说都是关键帧 */const char keyFrame,
        /* 数据类型,1表示jpeg,2表示H264 */const char type, /* 数据帧所在的文件句柄 */FILE *fd, /* 数据帧在文件内的偏移量 */const unsigned long offset,
        /* 数据帧长度 */const unsigned long bufferLen, /* 图像的宽度 */const unsigned short width, /* 图像的高度 */const unsigned short height, MonitorItem * pMonitorItem) {
    pMonitorItem->rw.line = __LINE__;
    struct Record_private_VideoBuffer_item *pItem;
    FsObjectList *videoCacheList = pRecord_item->ro.__videoCacheList;
    if (videoCacheList->nodeCount > 15000) {
        pMonitorItem->rw.line = __LINE__;
        pItem = (struct Record_private_VideoBuffer_item*) fs_ObjectList_remove_head(videoCacheList);
        if (pItem->fd != ((struct Record_private_VideoBuffer_item*) (videoCacheList->pNode[videoCacheList->startIndex]))->fd) {
            fclose(pItem->fd);
        }
        pMonitorItem->rw.line = __LINE__;
    } else {

        pMonitorItem->rw.line = __LINE__;
        pItem = (struct Record_private_VideoBuffer_item*) malloc(sizeof (struct Record_private_VideoBuffer_item));
        pMonitorItem->rw.line = __LINE__;
    }
    pMonitorItem->rw.line = __LINE__;
    /* 帧号 */
    pItem->frameIndex = frameIndex;
    /* 采集时间,用系统开机时间 */
    pItem->captureTime_uptime = captureTime_uptime;
    /* 数据帧类型,0表示普通数据类型,1表示关键帧,对jpeg来说都是关键帧 */
    pItem->keyFrame = keyFrame;
    /* 数据类型,1表示jpeg,2表示H264 */
    pItem->type = type;
    /* 文件句柄 */
    pItem->fd = fd;
    /* 偏移量,绝对位置 */
    pItem->offset = offset;
    /* 数据长度 */
    pItem->dataLenth = bufferLen;
    /* 图像宽度 */
    pItem->width = width;
    /* 图像高度 */
    pItem->height = height;

    fs_ObjectList_insert_tail(videoCacheList, pItem);
    pMonitorItem->rw.line = __LINE__;
}

/* 录像 */
static void record_private_item_do_videoRecord(/* 记录项,保证有一帧数据可用 */struct Record_item *pRecord_item, FsObjectImageFrame const *pFrame, MonitorItem * pMonitorItem) {
    pMonitorItem->rw.line = __LINE__;
    unsigned int frameLenth;
    long long offset;
    time_t ts;
    struct tm ti;
    char *buffer = pRecord_item->ro._tmpBufferForThis;
    FsObjectH264 * const *pH264 = pFrame->data.h264[0];
    for (; NULL != *pH264; pH264++) {
        if (pH264[0]->keyFrame == 3 && ((unsigned long) pH264[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->ro.recordVideoFile_startTime)
            record_private_item_clear_video(pRecord_item);
        if (NULL == pRecord_item->p._recordVideoFile_fd && 3 == pH264[0]->keyFrame) {
            pMonitorItem->rw.line = __LINE__;
            pRecord_item->ro.recordVideoFile_startTime = ((unsigned long) pH264[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
            ts = (time_t) pH264[0]->capture_gmtTime;
            // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
            localtime_r(&ts, &ti);
            sprintf(buffer, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir,
                    pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                    1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
            fs_dir_make(buffer, 0777);
            pMonitorItem->rw.line = __LINE__;
            pRecord_item->p._recordVideoFile_fd = fopen(buffer, "w+");
            pMonitorItem->rw.line = __LINE__;
            if (NULL == pRecord_item->p._recordVideoFile_fd) {
                FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", buffer, strerror(errno));
                fflush(stdout);
            } else {
                char *h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO((unsigned char*) pH264[0]->data);
                struct VideoMKV_track track[1];
                memset(track, 0, sizeof ( struct VideoMKV_track));
                track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pH264[0]->width, track[0].p.video.height = pH264[0]->height, track[0].p.video.encodeType = 2,
                        track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                pMonitorItem->rw.line = __LINE__;
                pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                pMonitorItem->rw.line = __LINE__;
                free(h264CodecPrivate);
            }
            pMonitorItem->rw.line = __LINE__;
        }
        if (pRecord_item->p._recordVideoFile_fd) {
            frameLenth = pH264[0]->datalenth;
            pMonitorItem->rw.line = __LINE__;
            offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pH264[0]->keyFrame & 0x1, &frameLenth, pH264[0]->data);
            pMonitorItem->rw.line = __LINE__;
            if (offset < 1) {
                FsPrintf(1, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pH264[0]->keyFrame, pH264[0]->datalenth);
                fflush(stdout);
            } else {

                pMonitorItem->rw.line = __LINE__;
                record_private_item_addVideoBuffer(pRecord_item, pH264[0]->frameIndex, pH264[0]->capture_uptime, pH264[0]->keyFrame & 0x1, 2,
                        pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pH264[0]->width, pH264[0]->height, pMonitorItem);
                pMonitorItem->rw.line = __LINE__;
            }
        }
    }
}

/* 录像 */
static void record_private_item_do_videoRecord1(/* 记录项,保证有一帧数据可用 */struct Record_item *pRecord_item, MonitorItem * pMonitorItem) {
    pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
    unsigned int count = pRecord_item->ro.__framelistIn->nodeCount;
    {
        FsObjectImageFrame ** ppFrame = (FsObjectImageFrame**) pRecord_item->ro.__framelistIn->pNode + pRecord_item->ro.__framelistIn->startIndex + count - 1;
        for (; 0 < count && pRecord_item->p.recordVideoFrameIndex != (*ppFrame)->index; count--, ppFrame--);
    }
    pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
    if (count < pRecord_item->ro.__framelistIn->nodeCount) {
        FsObjectImageFrame *pFrame;
        FsObjectH264 * H264list[4];
        H264list[3] = NULL;
        FsObjectH264 **pH264;
        unsigned int frameLenth;
        long long offset;
        time_t ts;
        struct tm ti;
        char *buffer = pRecord_item->ro._tmpBufferForThis;
        do {
            pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
            pFrame = (FsObjectImageFrame*) pRecord_item->ro.__framelistIn->pNode[pRecord_item->ro.__framelistIn->startIndex + count];
            pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
            H264list[0] = pFrame->data.h264[0][0], H264list[1] = pFrame->data.h264[0][1], H264list[2] = pFrame->data.h264[0][2];
            pH264 = H264list;
            for (; NULL != *pH264; pH264++) {
                if (pH264[0]->keyFrame == 3 && ((unsigned long) pH264[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->ro.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                if (NULL == pRecord_item->p._recordVideoFile_fd && 3 == pH264[0]->keyFrame) {
                    pRecord_item->ro.recordVideoFile_startTime = ((unsigned long) pH264[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pH264[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(buffer, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir,
                            pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(buffer, 0777);
                    pRecord_item->p._recordVideoFile_fd = fopen(buffer, "w+");
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", buffer, strerror(errno));
                        fflush(stdout);
                    } else {
                        char *h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO((unsigned char*) pH264[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pH264[0]->width, track[0].p.video.height = pH264[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        free(h264CodecPrivate);
                    }
                }
                if (pRecord_item->p._recordVideoFile_fd) {
                    frameLenth = pH264[0]->datalenth;
                    offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pH264[0]->keyFrame & 0x1, &frameLenth, pH264[0]->data);
                    if (offset < 1) {
                        FsPrintf(1, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pH264[0]->keyFrame, pH264[0]->datalenth);
                        fflush(stdout);
                    } else {
                        pMonitorItem->rw.line = __LINE__;
                        record_private_item_addVideoBuffer(pRecord_item, pH264[0]->frameIndex, pH264[0]->capture_uptime, pH264[0]->keyFrame & 0x1, 2,
                                pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pH264[0]->width, pH264[0]->height, pMonitorItem);
                        pMonitorItem->rw.line = __LINE__;
                    }
                }
            }
        } while (++count < pRecord_item->ro.__framelistIn->nodeCount);
        pRecord_item->p.recordVideoFrameIndex = pFrame->index;
    }
}

/* 录像,只处理一个录像合成任务,有合成任务处理完成返回1,出错返回-1,还需要等待返回0 */
static signed char record_private_item_do_partVideo(/* 记录项,保证有一个录像合成任务可用 */struct Record_item * pRecord_item) {
    FsObjectList *videoCacheList = pRecord_item->ro.__videoCacheList;
    FsObjectList *videoPartList = pRecord_item->ro.__videoPartList;
    pthread_mutex_lock(&videoPartList->mutex);
    struct Record_private_VideoBuffer_item **pNodeVideoBuffer = (struct Record_private_VideoBuffer_item**) pRecord_item->ro.__videoCacheList->pNode + pRecord_item->ro.__videoCacheList->startIndex;
    struct Record_private_PartVideo_item *pPartVideo_item;
    unsigned long l1 = videoCacheList->nodeCount;
    /* 把无效的数据全删除 */
    while (videoPartList->nodeCount > 0 && (pPartVideo_item = (struct Record_private_PartVideo_item*) videoPartList->pNode[videoPartList->startIndex])->startTime < pNodeVideoBuffer[0]->captureTime_uptime) {
        /* 没有缓存 */
        FsPrintf(1, "Can't part the video:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        free(fs_ObjectList_remove_head(videoPartList));
    }
    unsigned long l2 = videoPartList->nodeCount;
    if (l2 == 0 || pPartVideo_item->endTime > pNodeVideoBuffer[l1 - 1]->captureTime_uptime) {
        pthread_mutex_unlock(&videoPartList->mutex);
        return 0;
    }
    /* 缓存可能足够了 */
    /* 定位开始帧 */
    /* 先大致定位 */
    unsigned long b = 0, i = 0, e = l1;
    while (b < e) {
        i = (b + e) >> 1;
        if (pNodeVideoBuffer[i]->captureTime_uptime < pPartVideo_item->startTime) b = i + 1;
        else e = i;
    }
    /* 精确定位,查找关键帧 */
    while (b > 0 && pNodeVideoBuffer[b]->keyFrame != 1) b--;
    if (pNodeVideoBuffer[b]->keyFrame != 1) {
        /* 没法,没找到,只能勉强用后面的了 */
        b++;
        while (b < l1 && pNodeVideoBuffer[b]->keyFrame != 1) b++;
    }
    if (b == l1) {
        /* 还是找不到,写的程序有BUG了 */
        FsPrintf(1, "Can't find key frame for the video:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        free(fs_ObjectList_remove_head(videoPartList));
        pthread_mutex_unlock(&videoPartList->mutex);
        return -1;
    }
    /* 找到了 */
    /* 找结束位置 */
    /* 先重定位下结束时间 */
    double theFrameRate = pNodeVideoBuffer[b]->captureTime_uptime + pPartVideo_item->endTime - pPartVideo_item->startTime;
    if (theFrameRate > pNodeVideoBuffer[l1 - 1]->captureTime_uptime) {
        /* 还是再等等吧,到这的概率极低 */
        pthread_mutex_unlock(&videoPartList->mutex);
        return 0;
    } else {
        /* 移出合成项 */
        fs_ObjectList_remove_head(videoPartList);
        pthread_mutex_unlock(&videoPartList->mutex);
    }
    e = l1 - 1;
    while (e > b && pNodeVideoBuffer[e]->captureTime_uptime > theFrameRate) e--;
    if (pNodeVideoBuffer[e]->captureTime_uptime > theFrameRate ||
            pNodeVideoBuffer[e]->captureTime_uptime <= pNodeVideoBuffer[b]->captureTime_uptime) {
        /* 出错了 */
        FsPrintf(1, "Failed to find endTime for the video:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        free(pPartVideo_item);
        return -1;
    }
    if (e < l1 - 1)e++;
    e++;
    theFrameRate = (e - b) / (pNodeVideoBuffer[e - 1]->captureTime_uptime - pNodeVideoBuffer[b]->captureTime_uptime);
    fs_dir_make(pPartVideo_item->fileName, 0777);
    FILE * fd = fopen(pPartVideo_item->fileName, "wb");
    if (fd == NULL) {
        FsPrintf(1, "Failed to open the video file:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        free(pPartVideo_item);
        return -1;
    }
    if (1) {
        /* mkv保存 */
        fflush(pNodeVideoBuffer[e - 1]->fd);
        unsigned long offset = ftell(pNodeVideoBuffer[e - 1]->fd);
        /* 先读取关键帧 */
        i = pNodeVideoBuffer[b]->dataLenth;
        unsigned int bufferLen = i;
        char *buffer = (char*) malloc(bufferLen);
        if (fseek(pNodeVideoBuffer[b]->fd, pNodeVideoBuffer[b]->offset, SEEK_SET) != 0) {
            FsPrintf(1, "Run fseek(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                    pNodeVideoBuffer[b]->offset, (long) pNodeVideoBuffer[b]->fd, b, e, strerror(errno),
                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        } else if (fread(buffer, i, 1, pNodeVideoBuffer[b]->fd) != 1) {
            FsPrintf(1, "Run fread(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                    i, (long) pNodeVideoBuffer[b]->fd, b, e, strerror(errno),
                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        } else {
            /* 读到数据了 */
            char *codecPrivate = NULL;
            struct VideoMKV_track track[2];
            if (pNodeVideoBuffer[b]->type == 2) {
                /* h264编码 */
                /* 这个不会出错,除非有人恶意更改了数据 */
                codecPrivate = video_mkv_codecPrivate_h264_get__IO((unsigned char*) buffer);
            } else {
                /* mjpeg */
                codecPrivate = video_mkv_codecPrivate_jpeg_get__IO();
            }
            track[0].name = (char*) "video1", track[0].type = VideoMKV_trackVideo, track[0].index = 1,
                    track[0].p.video.width = pNodeVideoBuffer[b]->width, track[0].p.video.height = pNodeVideoBuffer[b]->height, track[0].p.video.encodeType = pNodeVideoBuffer[b]->type,
                    track[0].p.video.frameRate = theFrameRate, track[0].p.video.codecPrivate = codecPrivate;
            track[1].name = (char*) "subtitle1", track[1].type = VideoMKV_trackSubtitle, track[1].index = 2,
                    track[1].p.subtitle.width = pNodeVideoBuffer[b]->width, track[1].p.subtitle.height = pNodeVideoBuffer[b]->height, track[1].p.subtitle.fontSize = 30;
            VideoMKV *pMKV = video_mkv_new_for_file_save__IO(2, track, 1, 0, 1, fd);
            free(codecPrivate);
            if (pMKV == NULL) {
                FsPrintf(1, "Failed to create MKV for the video file:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                        pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
            } else {
                double timeDiff = fs_time_GMT_get() - fs_time_uptime_get();
                double ft = pNodeVideoBuffer[b]->captureTime_uptime + timeDiff;
                time_t ts = (time_t) ft;
                ft -= ts;
                ft *= 1000;
                int msec = (int) ft;
                struct tm ti;
                localtime_r(&ts, &ti);
                char timeBuffer[128];
                sprintf(timeBuffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                        1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec, msec);
                if (video_mkv_file_save_subtitle(pMKV, 2, 0, 0, 1.0 / theFrameRate, strlen(timeBuffer), timeBuffer) != 1 || (pPartVideo_item->subtitle &&
                        video_mkv_file_save_subtitle(pMKV, 2, 0, 35, pNodeVideoBuffer[e]->captureTime_uptime - pNodeVideoBuffer[b]->captureTime_uptime,
                        strlen(pPartVideo_item->subtitle), pPartVideo_item->subtitle) != 1)) {
                    FsPrintf(1, "Write subtitle(%s) error for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                            pPartVideo_item->subtitle,
                            pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                            pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                } else if (video_mkv_file_save_frame(pMKV, 1, 1, (unsigned int*) &i, buffer) < 1) {
                    FsPrintf(1, "Write data(=%lu,%#lx,%lu,%lu) error for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                            i, (long) pNodeVideoBuffer[b]->fd, b, e,
                            pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                            pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                } else {
                    for (b++; b < e; b++) {
                        ft = pNodeVideoBuffer[b]->captureTime_uptime + timeDiff;
                        ts = (time_t) ft;
                        ft -= ts;
                        ft *= 1000;
                        msec = (int) ft;
                        localtime_r(&ts, &ti);
                        sprintf(timeBuffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                                1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec, msec);
                        if (video_mkv_file_save_subtitle(pMKV, 2, 0, 0, 1.0 / theFrameRate, strlen(timeBuffer), timeBuffer) != 1) {
                            FsPrintf(1, "Write subtitle(%s) error for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                                    timeBuffer,
                                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                            break;
                        }
                        if (fseek(pNodeVideoBuffer[b]->fd, pNodeVideoBuffer[b]->offset, SEEK_SET) != 0) {
                            FsPrintf(1, "Run fseek(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                                    pNodeVideoBuffer[b]->offset, (long) pNodeVideoBuffer[b]->fd, b, e, strerror(errno),
                                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                            break;
                        }
                        i = pNodeVideoBuffer[b]->dataLenth;
                        if (bufferLen < i) {
                            bufferLen = i;
                            free(buffer);
                            buffer = (char*) malloc(bufferLen);
                        }
                        if (fread(buffer, i, 1, pNodeVideoBuffer[b]->fd) != 1) {
                            FsPrintf(1, "Run fread(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                                    i, (long) pNodeVideoBuffer[b]->fd, b, e, strerror(errno),
                                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                            break;
                        }
                        if (video_mkv_file_save_frame(pMKV, 1, pNodeVideoBuffer[b]->keyFrame, (unsigned int*) &i, buffer) < 1) {
                            FsPrintf(1, "Write data(%lu,%#lx,%lu,%lu) failed for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                                    i, (long) pNodeVideoBuffer[b]->fd, b, e,
                                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                            break;
                        }
                    }
                }
                if (video_mkv_file_save_finish(pMKV) != 1) {
                    FsPrintf(1, "Write tail(%lu,%#lx,%lu,%lu) failed for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                            i, (long) pNodeVideoBuffer[b]->fd, b, e,
                            pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                            pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                }
                video_mkv_delete__OI(pMKV);
            }
        }
        free(buffer);
        /* 前面的视频文件都不会再写了,只需重置最后一个文件的偏移量 */
        if (fseek(pNodeVideoBuffer[e - 1]->fd, offset, SEEK_SET) != 0) {
            FsPrintf(1, "Run fseek(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n", offset, (long) pNodeVideoBuffer[e - 1]->fd, b, e, strerror(errno),
                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        }
    }
    fclose(fd);
    free(pPartVideo_item);

    return 1;
}

/* 通道分层多线程写入法,不处理实时转发 */
static void *record_private_thread(struct Record * const pRecord) {
    //FsPrintf(1, "Thread Index=%hu.\n", pRecord->p.threadCount);
    /* 创建线程监控 */
    MonitorItem *pMonitorItem = monitorItem_new_and_add(pRecord->ro._pMonitor,
            pRecord->ro._threadName, NULL, pRecord, pRecord->ro._parentTid);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *externRunStatus = pRecord->ro._externRunStatus;
    void *pRtspServer = pRecord->ro._pRtspServer;
    Rtmp *pRtmp = pRecord->ro._pRtmp;
    void *pHls = pRecord->ro._pHls;
    void *pGB28181Server = pRecord->ro._pGB28181Server;
    ConfigManager *pConfigManager = (ConfigManager*) pRecord->ro._pConfigManager;
    FsObjectList *itemList = pRecord->ro.__itemList;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    double sleepToTime = 0.0, thisTime = -10.0, thisTime0;
    /* 所有通道的平钧帧率 */
    float frameRateAvg = (pRecord->p.threadCount > 0) ? 65535.0 : 5.0;
    /* 实时录像缓存 */
    unsigned int recordVideoBufferCount = 0;
    /* 所在写入层数 */
    unsigned short layer = 0;
    /* 开始位置 */
    const unsigned int startIndex = pRecord->ro._channelCountPerThread * pRecord->p.threadCount++;
    /* 处理的数量 */
    unsigned int doCount;
    /* 子线程的线程号 */
    pthread_t childTid = 0;
    /* 储存当前要处理的通道的数据指针 */
    FsObjectList *list = fs_ObjectList_new__IO(6);
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    int i;
    FsObjectImageFrame *pFrame, **ppFrame_node;
    struct Record_item *pRecord_item, **pRecord_itemNode;
    unsigned int frameCount;
    unsigned short layerMax = 0;
    int writeDiskFrameCount = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    if (0 == startIndex) pRecord->rw.resetChannel = -1;
    // 注册命令字
    // 处理线程
    if (itemList->nodeCount > startIndex + pRecord->ro._channelCountPerThread) {
        doCount = pRecord->ro._channelCountPerThread;
        /* 自己处理不完,让子线程来做 */
        if (1) {
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
            if (pRecord->rw.nice) {
                /* 修改为实时调度策略 */
                pthread_attr_setschedpolicy(&attr, SCHED_RR);
                struct sched_param param;
                param.sched_priority = pRecord->rw.nice;
                pthread_attr_setschedparam(&attr, &param);
            } else {
                /* 继承父进程优先级 */
                pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
                pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
            }
            if (pthread_create(&childTid, &attr, (void* (*)(void*))record_private_thread, pRecord)) {
                FsPrintf(1, "Create thread error:\"%s\".\n", strerror(errno));
                fflush(stdout);
                childTid = 0;
            }
            pthread_attr_destroy(&attr);
        }
    } else doCount = itemList->nodeCount - startIndex;
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pRecord->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        if (frameRateAvg < 5.0)frameRateAvg = 5.0;
        else frameRateAvg *= 0.9999;
        sleepToTime = thisTime + 1.0 / frameRateAvg;
        thisTime0 = thisTime = fs_time_uptime_get();
        //        FsPrintf2(1, "layer=%03hu/%03hu,bufferCount=%05u/%05d,frameRateAvg=%lf/%lf/%lf.\n", layer, layerMax, recordVideoBufferCount, writeDiskFrameCount, frameRateAvg, sleepToTime, thisTime);
        //        fflush(stdout);
        if (sleepToTime > thisTime) {
            if (0 == layerMax) fs_thread_sleepTo(sleepToTime);
            else if (writeDiskFrameCount < 0 && (sleepToTime - thisTime) * frameRateAvg > (1.0 - 1.0 / layerMax)) {
                //#ifdef FsDebug
                //                if (-writeDiskFrameCount > itemList->nodeCount) {
                //                    FsPrintf2(1, "layer=%03hu/%03hu,bufferCount=%05u/%05d,frameRateAvg=%lf.\n", layer, layerMax, recordVideoBufferCount, writeDiskFrameCount, frameRateAvg);
                //                    fflush(stdout);
                //                }
                //#endif
                recordVideoBufferCount++;
            }
        } else if (recordVideoBufferCount > 2) {
            recordVideoBufferCount--;
            //            FsPrintf2(1, "bufferCount=%u/%d,layer=%hu/%hu.\n", recordVideoBufferCount, writeDiskFrameCount, layer, layerMax);
            //            fflush(stdout);
        } else recordVideoBufferCount = 2;
        if (layer != layerMax && layerMax > 8 && layerMax > pRecord->ro._channelCountPerThread / 2) {
            FsPrintf2(1, "layer=%03hu/%03hu,bufferCount=%05u/%05d,frameRateAvg=%lf.\n", layer, layerMax, recordVideoBufferCount, writeDiskFrameCount, frameRateAvg);
        }
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        if (pRecord->rw.resetChannel>-1) {
            if (frameRateAvg < 65535)frameRateAvg *= 4.0;
            if (0 == startIndex && 0 == pRecord->rw.resetChannel) {
                /* 读取通道数 */
                if (pRecord->p.ipList)free(pRecord->p.ipList);
                pRecord->p.ipList = fs_network_ipv4_get_all__IO();
                unsigned int channelCount = record_private_get_channelCount(pConfigManager->ro.__pConfig, pRecord->p.ipList);
                if (itemList->nodeCount != channelCount) {
                    /* 让所有线程同步一下 */
                    pRecord->p.syncCount = 1;
                    unsigned short threadCount = (itemList->nodeCount + pRecord->ro._channelCountPerThread - 1) / pRecord->ro._channelCountPerThread;
                    if (0 == threadCount)threadCount = 1;
                    while (pRecord->p.syncCount != threadCount && pRecord->rw.runStatus == *externRunStatus)usleep(1);
                    if (pRecord->p.syncCount != threadCount) while (pRecord->p.threadCount != 1)usleep(1);
                    /* 删除通道 */
                    while (itemList->nodeCount > channelCount) {
                        pRecord_item = (struct Record_item *) fs_ObjectList_remove_tail(itemList);
                        pMonitorItem->rw.line = __LINE__;
                        if (pRecord_item)record_private_item_delete__OI(pRecord_item);
                        pMonitorItem->rw.line = __LINE__;
                    }
                    /* 添加通道 */
                    while (itemList->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList, NULL);
                    //FsPrintf(1, "%#lx,%lu,%lu.\n", itemList->pNode, itemList->startIndex, itemList->nodeCount);
                }
                pMonitorItem->rw.line = __LINE__;
                /* 重置集群 */
                i = pRecord->rw.resetChannel;
                if (0 == pRecord->p.syncCount) pRecord->rw.resetChannel = -1;
                pMonitorItem->rw.line = __LINE__;
                record_private_item_new(pRecord, i, pRecord->p.ipList, NULL, pRtspServer, pRtmp, pHls, pGB28181Server);
                pMonitorItem->rw.line = __LINE__;
            }
            if (pRecord->rw.resetChannel > (int) startIndex && pRecord->rw.resetChannel < (int) (startIndex + doCount + 1)) {
                record_private_item_new(pRecord, pRecord->rw.resetChannel, pRecord->p.ipList, (struct Record_item **) (itemList->pNode + itemList->startIndex + pRecord->rw.resetChannel - 1),
                        pRtspServer, pRtmp, pHls, pGB28181Server);
                pRecord->rw.resetChannel = -1;
            } else if (pRecord->p.syncCount != 0) {
                /* 其他线程要监控是否要与第一个线程同步 */
                if (0 == startIndex) {
                    pRecord->p.syncCount = 0;
                    pRecord->rw.resetChannel = -1;
                } else {
                    pthread_mutex_lock(&pRecord->p.__mutex);
                    pRecord->p.syncCount++;
                    pthread_mutex_unlock(&pRecord->p.__mutex);
                    while (pRecord->p.syncCount != 0 && pRecord->rw.runStatus == *externRunStatus) usleep(1);
                }
                /* 已重置完成了 */
                if (itemList->nodeCount > startIndex && pRecord->rw.runStatus == *externRunStatus) {
                    /* 本线程还可以活着 */
                    if (itemList->nodeCount > startIndex + pRecord->ro._channelCountPerThread) {
                        doCount = pRecord->ro._channelCountPerThread;
                        /* 自己处理不完,让子线程来做 */
                        if (0 == childTid) {
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
                            if (pRecord->rw.nice) {
                                /* 修改为实时调度策略 */
                                pthread_attr_setschedpolicy(&attr, SCHED_RR);
                                struct sched_param param;
                                param.sched_priority = pRecord->rw.nice;
                                pthread_attr_setschedparam(&attr, &param);
                            } else {
                                /* 继承父进程优先级 */
                                pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
                                pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
                            }
                            if (pthread_create(&childTid, &attr, (void* (*)(void*))record_private_thread, pRecord)) {
                                FsPrintf(1, "Create thread error:\"%s\".\n", strerror(errno));
                                fflush(stdout);
                                childTid = 0;
                            }
                            pthread_attr_destroy(&attr);
                        }
                    } else {
                        doCount = itemList->nodeCount - startIndex;
                        /* 自己已能完全处理所有数据,不需要子线程帮忙 */
                        if (childTid) {
                            pthread_join(childTid, 0);
                            childTid = 0;
                        }
                    }
                } else if (0 == startIndex) {
                    doCount = 0;
                    /* 自己已能完全处理所有数据,不需要子线程帮忙 */
                    if (childTid) {
                        pthread_join(childTid, 0);
                        childTid = 0;
                    }
                } else {
                    /* 已经是多余的了 */
                    break;
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */
        for (i = doCount, writeDiskFrameCount = recordVideoBufferCount, layer = layerMax, layerMax = 0, pRecord_itemNode = (struct Record_item **) itemList->pNode + itemList->startIndex + startIndex;
                i > 0; i--) {
            pRecord_item = *pRecord_itemNode++;
            if (NULL == pRecord_item)continue;
            if (1 == pRecord_item->ro._recordVideoMode) {
                /* 自动选择合适的码流来记录 */
                frameCount = pRecord_item->ro.__framelistIn->nodeCount;
                if (frameCount > 0) {
                    if (pRecord_item->rw.layer > layer && writeDiskFrameCount > 0) {
                        /* 做录像信息记录 */
                        if (pRecord_item->rw.videoLastTime < 0.00001) {
                            FsPrintf(1, "ttt1,%s\n", pRecord_item->ro._uuid);
                            pRecord_item->rw.videoRecordCheck = 51200;
                        } else if (1 == pRecord_item->rw.videoRecordCheck--) {
                            FsPrintf(1, "ttt2,%s\n", pRecord_item->ro._uuid);;
                            pRecord_item->rw.videoRecordCheck = 51200;
                        }
                        pRecord_item->rw.videoLastTime = thisTime0 + 5.0;
                        ////////////////////////////////////////////////////////
                        pRecord_item->rw.layer = 1;
                        writeDiskFrameCount -= frameCount;
                        if (frameCount > 1) {
                            /* 有多帧数据 */
                            pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                            fs_ObjectList_insert_tail_all(list, pRecord_item->ro.__framelistIn);
                            fs_ObjectList_clear(pRecord_item->ro.__framelistIn);
                            pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                            for (frameCount = list->nodeCount, ppFrame_node = (FsObjectImageFrame**) list->pNode + list->startIndex; frameCount > 0; frameCount--) {
                                pFrame = *ppFrame_node++;
                                frameRateAvg = frameRateAvg + (pFrame->frameRate - frameRateAvg) / itemList->nodeCount;
                                /* 录像 */
                                pMonitorItem->rw.line = __LINE__;
                                record_private_item_do_videoRecord(pRecord_item, pFrame, pMonitorItem);
                                pMonitorItem->rw.line = __LINE__;
                            }
                            /* 传出数据 */
                            pthread_mutex_lock(&pRecord_item->ro.__framelistOut->mutex);
                            fs_ObjectList_insert_tail_all(pRecord_item->ro.__framelistOut, list);
                            fs_ObjectList_clear(list);
                            pthread_mutex_unlock(&pRecord_item->ro.__framelistOut->mutex);
                        } else {
                            /* 只有一帧数据 */
                            pMonitorItem->rw.line = __LINE__;
                            pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pRecord_item->ro.__framelistIn);
                            frameRateAvg = frameRateAvg + (pFrame->frameRate - frameRateAvg) / itemList->nodeCount;
                            /* 录像 */
                            pMonitorItem->rw.line = __LINE__;
                            record_private_item_do_videoRecord(pRecord_item, pFrame, pMonitorItem);
                            pMonitorItem->rw.line = __LINE__;
                            /* 传出数据 */
                            fs_ObjectList_insert_tail_pthreadSafety(pRecord_item->ro.__framelistOut, pFrame);
                            pMonitorItem->rw.line = __LINE__;
                        }
                        /* 录像合成 */
                        if (pRecord_item->ro.__videoPartList->nodeCount > 0 && pRecord_item->ro.__videoCacheList->nodeCount > 0) {
                            if (1 == record_private_item_do_partVideo(pRecord_item)) {
                                /* 让系统不要睡眠 */
                                thisTime = 0.0;
                            }
                        }
                    } else {
                        if (layerMax < pRecord_item->rw.layer)layerMax = pRecord_item->rw.layer;
                        pRecord_item->rw.layer++;
                    }
                } else if (pRecord_item->rw.videoLastTime < thisTime0 && pRecord_item->rw.videoLastTime > 0.00001) {
                    FsPrintf(1, "ttt0,%s\n", pRecord_item->ro._uuid);
                    pRecord_item->rw.videoLastTime = 0.0;
                }
                /* 历史点播 */
                pMonitorItem->rw.line = __LINE__;
                pMonitorItem->rw.line = __LINE__;
            } else {
                /* 传出数据 */
                while (pRecord_item->ro.__framelistIn->nodeCount > 0) {
                    fs_ObjectList_insert_tail_pthreadSafety(pRecord_item->ro.__framelistOut, fs_ObjectList_remove_head_pthreadSafety(pRecord_item->ro.__framelistIn));
                }
                pMonitorItem->rw.line = __LINE__;
            }
        }
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 处理子线程 */
    if (childTid) {
        pthread_join(childTid, NULL);
    }
    fs_ObjectList_delete__OI(list);
    /* 线程数-1,必须放在处理子线程之后,这样可以避免使用锁 */
    pRecord->p.threadCount--;
    if (0 == startIndex) {
        /* 资源是多线程共享的,由第一个线程控制 */
        /* 删除各通道 */
        struct Record_item **ppNode = (struct Record_item **) itemList->pNode + itemList->startIndex;
        for (i = itemList->nodeCount; i > 0; i--) {
            pRecord_item = *ppNode++;
            if (pRecord_item)record_private_item_delete__OI(pRecord_item);
        }
        fs_ObjectList_clear(itemList);
        /* 删除集群 */
        if (pRecord->p.clusterItemList) {
            fs_ObjectList_delete_custom__OI(pRecord->p.clusterItemList, (void (*)(void*))record_private_item_delete__OI);
            pRecord->p.clusterItemList = NULL;
        }
        if (pRecord->p.ipList) {
            free(pRecord->p.ipList);
            pRecord->p.ipList = NULL;
        }
        /* 释放空间 */
        /* 释放全局变量空间 */
        /* 释放中间变量空间 */
    }
    ////////////////////////////////////////////////////////////////////////////
    FsPrintf2(1, "Thread:\"%s\" exit,runStatus=%hhd,threadCount=%hu.\n", pRecord->ro._threadName, pRecord->rw.runStatus, pRecord->p.threadCount);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pRecord->ro._pMonitor, pMonitorItem);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void record_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "recordConfig", "数据记录配置", "数据记录配置", 0, 0x7);
    void *pNode;
    if (mask & (0x1LLU << 1)) {
        pNode = fs_Config_node_integer_add(pConfig, parent, "recordVideoMode", "视频记录模式", "视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录", 0, 0x7, 0, 2, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "自动选择合适的码流", "自动选择合适的码流");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "不记录", "不记录,删除已储存的数据");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "不记录,但保留已储存的数据", "不记录,但保留已储存的数据");
        pNode = fs_Config_node_integer_add(pConfig, parent, "realVideoProportion", "实时视频比例", "实时视频比例,是相对比例", 0, 0x7, 1, 1000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 30, "30", "30");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal_false, "0");
        pNode = fs_Config_node_integer_add(pConfig, parent, "realVideoSaveTime", "实时视频保存的最长时间(S)", "实时视频保存的最长时间,单位秒,0表示不限制", 0, 0x7, 0, 20000000000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "不限制", "不限制");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 604800, "一周", "一周");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2592000, "30天", "30天");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 5184000, "60天", "60天");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 7776000, "90天", "90天");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal_false, "0");
        pNode = fs_Config_node_integer_add(pConfig, parent, "incidentProportion", "事件数据比例", "事件数据比例,是相对比例", 0, 0x7, 1, 1000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 60, "60", "60");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal_false, "0");
    } else {
        pNode = fs_Config_node_integer_add(pConfig, parent, "recordVideoMode", "视频记录模式", "视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录", 0, 0x0, 0, 0, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "不记录", "不记录,删除已储存的数据");
    }
    pNode = fs_Config_node_string_add(pConfig, parent, "localPush", "本地推送", "本地推送,填写目的通道的uuid,为空表示不推送,支持多通道向同一目的通道同时推送", 0, 0x7, 1, 16, 1);
    if (mask & (0x1LLU << 0)) {
        pNode = fs_Config_node_string_add(pConfig, parent, "rtspServerURL", "rtsp转发地址", "rtsp转发地址,为空表示不转发", 0, 0x7, 1, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "/ch1", "/ch1", "/ch1");
    } else {
        pNode = fs_Config_node_string_add(pConfig, parent, "rtspServerURL", "rtsp转发地址", "rtsp转发地址,为空表示不转发", 0, 0x0, 1, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "", "不转发", "不转发");
    }
    pNode = fs_Config_node_string_add(pConfig, parent, "gb28181ID", "GB28181通道编号", "GB28181通道编号,为空表示不注册到GB28181平台", 0, 0x0, 1, 20, 1);

    pNode = fs_Config_node_string_add(pConfig, parent, "gb28181Name", "GB28181通道名", "GB28181通道名,为空使用GB28181通道编号", 0, 0x0, 1, 64, 1);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "gb28181ID", FsConfig_Condition_equal_false, NULL);

    pNode = fs_Config_node_integer_add(pConfig, parent, "gb28181ControlMask", "GB28181控制掩码", "GB28181控制掩码,1-开启转动,0x10-开启变倍", 0, 0x7, 0, 0x1 << 4, 2);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "开启转动", "开启转动");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 4, "开启变倍", "开启变倍");
    {
        void *pGroupConfition = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, pGroupConfition, 2, "cameraConfig streamMask", FsConfig_Condition_orAnd, "4");
        fs_Config_condition_add_static(pConfig, pGroupConfition, 1, "gb28181ID", FsConfig_Condition_equal_false, NULL);
    }
    pNode = fs_Config_node_integer_add(pConfig, parent, "jpg_compressRate", "jpg图像压缩因子", "jpg图像压缩因子(50到95)", 0, 0x7, 50, 90, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 85, "85", "85");
}

/* 
 * 创建一个新的Record对象;
 * 返回Record指针.
 */
struct Record *record_new__IO(/* 线程名 */const char threadName[],
        /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor,
        /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
        /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus,
        /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* rtsp服务器对象 */ void * const pRtspServer, /* rtmp服务器对象 */ Rtmp * const pRtmp, /* hls服务器对象 */ void * const pHls,
        /* GB28181服务器对象 */ void * const pGB28181Server, /* 配置管理对象 */ void *const pConfigManager,
        /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread) {
    unsigned int len = strlen(dir);
    if (dir[len - 1] == '/')len++;
    else len += 2;
    len += strlen(threadName) + 1;
    struct Record* rst = (struct Record*) malloc(sizeof (struct Record) +len);
    memset(rst, 0, sizeof (struct Record));
    char *ps = (char*) (rst + 1);
    /* 线程名 */
    rst->ro._threadName = ps, len = strlen(threadName) + 1, memcpy(ps, threadName, len), ps += len;
    /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
    rst->ro._pMonitor = pMonitor;
    /* 父线程,创建此线程的线程地址 */
    rst->ro._parentTid = parentTid;
    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    if (externRunStatus) rst->ro._externRunStatus = externRunStatus;
    else rst->ro._externRunStatus = &defaultRunStatus;
    /* 根目录,以'/'结尾 */
    rst->ro._dir = ps, len = strlen(dir), memcpy(ps, dir, len), ps += len;
    if (*(ps - 1) != '/')*ps++ = '/';
    *ps++ = 0;
    /* rtsp服务器对象 */
    rst->ro._pRtspServer = pRtspServer;
    /* rtmp服务器对象 */
    rst->ro._pRtmp = pRtmp;
    /* hls服务器对象 */
    rst->ro._pHls = pHls;
    /* GB28181服务器对象 */
    rst->ro._pGB28181Server = pGB28181Server;
    /* 配置管理对象 */
    rst->ro._pConfigManager = pConfigManager;
    /* 每个线程最多处理的通道数 */
    rst->ro._channelCountPerThread = channelCountPerThread;
    /* 通道链表 */
    rst->ro.__itemList = fs_ObjectList_new__IO(4);
    /* 多线程锁 */
    pthread_mutex_init(&rst->p.__mutex, NULL);

    return rst;
}

/* 删除pRecord指向的实例对象 */
void record_delete__OI(struct Record * const pRecord) {

    /* 通道链表 */
    fs_ObjectList_delete__OI(pRecord->ro.__itemList);
    /* 多线程锁 */
    pthread_mutex_destroy(&pRecord->p.__mutex);

    free(pRecord);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void record_startThread(struct Record* pRecord,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice) {
    if (pRecord->rw.runStatus == 0 && pRecord->ro.stid == 0) {
        pRecord->rw.runStatus = 1;
        pRecord->rw.nice = nice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n\
    dir:\"%s\"\n  pRtspServer=%#lx\n  pGB28181Server=%#lx\n  pConfigManager=%#lx\n  channelCountPerThread=%hu\n  itemList=%#lx\n",
                pRecord->ro._threadName,
                (unsigned long) pRecord->ro._pMonitor,
                pRecord->ro._parentTid,
                (unsigned long) pRecord->ro._externRunStatus,
                pRecord->ro._dir,
                (unsigned long) pRecord->ro._pRtspServer,
                (unsigned long) pRecord->ro._pGB28181Server,
                (unsigned long) pRecord->ro._pConfigManager,
                pRecord->ro._channelCountPerThread,
                (unsigned long) pRecord->ro.__itemList);
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
        if (pthread_create(&pRecord->ro.stid, &attr, (void* (*)(void*))record_private_thread, pRecord)) {
            FsPrintf(1, "Create thread error:\"%s\".\n", strerror(errno));
            fflush(stdout);
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf(1, "The thread is allready running(runStatus=%hhu,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n\
    dir:\"%s\"\n  pRtspServer=%#lx\n  pGB28181Server=%#lx\n  pConfigManager=%#lx\n  channelCountPerThread=%hu\n  itemList=%#lx\n",
                pRecord->rw.runStatus, pRecord->ro.stid,
                pRecord->ro._threadName,
                (unsigned long) pRecord->ro._pMonitor,
                pRecord->ro._parentTid,
                (unsigned long) pRecord->ro._externRunStatus,
                pRecord->ro._dir,
                (unsigned long) pRecord->ro._pRtspServer,
                (unsigned long) pRecord->ro._pGB28181Server,
                (unsigned long) pRecord->ro._pConfigManager,
                pRecord->ro._channelCountPerThread,
                (unsigned long) pRecord->ro.__itemList);
        fflush(stdout);
    }
}

/* 结束线程 */
void record_stopThread(struct Record * pRecord) {
    pRecord->rw.runStatus = 0;
    if (pRecord->ro.stid) {
#ifdef FsDebug

        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pRecord->ro.stid, 0);
        pRecord->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧,如果数据可作为快照则返回1,否则返回-1 */
signed char record_item_frame_in_pthreadSafety_OI(/* 采集项,可为空 */ struct Record_item* pRecord_item, FsObjectImageFrame *pFrame) {
#ifdef FsDebug
    //    if (pRecord_item->ro.__framelistIn->nodeCount > 15) {
    //        FsPrintf2(1, "FrameListIn too long(=%lu).\n", pRecord_item->ro.__framelistIn->nodeCount);
    //    }
#endif
    if (pRecord_item == NULL || pFrame == NULL) {
        FsPrintf(1, "%#lx,%#lx\n", (long) pRecord_item, (long) pFrame);
        fflush(stdout);
    }

    /* 发送实时rtmp数据 */
    if (pRecord_item->ro.__pRtmpServer_item) rtmpServer_item_add_frame(pRecord_item->ro.__pRtmpServer_item, pFrame->data.h264[0][0]);
    /* 本地推送 */
    if (pRecord_item->p.localPushCameraFun) {
        pthread_mutex_lock(&pRecord_item->ro.__mutexCmdConnect);
        if (pRecord_item->p.localPushCameraFun) pRecord_item->p.localPushCameraFun(pRecord_item->p.pLocalPushCamera, pFrame, pRecord_item);
        pthread_mutex_unlock(&pRecord_item->ro.__mutexCmdConnect);
    }
    fs_ObjectList_insert_tail_pthreadSafety(pRecord_item->ro.__framelistIn, pFrame);

    return 1;
}

/* 传出数据帧,有数据返回FsObjectImageFrame指针,无数据返回NULL */
FsObjectImageFrame * record_item_frame_out_pthreadSafety_IO(/* 采集项,可为空 */ struct Record_item * pRecord_item) {
    if (pRecord_item && pRecord_item->ro.__framelistOut->nodeCount > 0)return (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pRecord_item->ro.__framelistOut);

    else return NULL;
}

/* 添加视频合成任务 */
void record_item_add_partVideo(/* 记录项 */ struct Record_item * const pRecord_item, /* 必需为全路径 */const char fileName[],
        /* 开始时间,推荐使用CPU时钟 */const double startiTime, /* 结束时间,推荐使用CPU时钟 */const double endTime, /* 字幕,可为空 */const char subtitle[]) {
    unsigned int fileNameLen = strlen(fileName) + 1;
    unsigned int subtitleLen = subtitle == NULL ? 0 : strlen(subtitle) + 1;
    struct Record_private_PartVideo_item *pPartVideo_item = (struct Record_private_PartVideo_item *) malloc(sizeof ( struct Record_private_PartVideo_item)+fileNameLen + subtitleLen);
    char *ps = (char*) (pPartVideo_item + 1);
    /* 开始时间,推荐使用CPU时钟 */
    pPartVideo_item->startTime = startiTime;
    /* 结束时间,推荐使用CPU时钟 */
    pPartVideo_item->endTime = endTime;
    /* 文件名,绝对路径 */
    pPartVideo_item->fileName = ps;
    memcpy(ps, fileName, fileNameLen), ps += fileNameLen;
    /* 字幕 */
    if (subtitle) {
        pPartVideo_item->subtitle = ps;
        memcpy(ps, subtitle, subtitleLen);
    } else {
        pPartVideo_item->subtitle = NULL;
    }
    fs_ObjectList_insert_custom_pthreadSafety(pRecord_item->ro.__videoPartList, pPartVideo_item, (signed char (*)(const void* const, const void* const))record_private_item_add_partVideo_compare);
}

/* 用以检测相机是否在线调用的控制取流的接口,只能在vsys中调用 */
void record_item_realStream_for_checkStatus(/* 记录项 */ struct Record_item * const pRecord_item, /* 状态,0-停止取流,1-表示取流 */const unsigned char status) {
    //FsPrintf(1, "uuid:\"%s\",status=%hhu.\n", pRecord_item->ro._uuid, status);
    //fflush(stdout);
    if (0 == pRecord_item->ro._recordVideoMode)
        configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro.__realMask, pRecord_item->ro._maskIndex_record, status);
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
