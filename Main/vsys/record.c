#include "record.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <float.h>
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/FileTool.h"
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Media/AudioAAC.h"
#include "output-ebml.h"

#include "output-xml-comment.h"
#include "capture.h"
#include "recognition.h"
#include "vsys.h"
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
#define Record_P_baseDir "/fs/project/data"

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
struct Record_P_VideoDemand {
#define Record_P_VideoDemand_channelCountMax 4 // 文件的最大通道数
    /* 帧号,各通道的帧号 */
    unsigned int frameIndex[Record_P_VideoDemand_channelCountMax];
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
    /* 帧率的倍率,在多文件切换时,时间可能与文件名有偏差,需要使用此值校正,让发送时间也真实时间接近 */
    float frameRateMulti;
    /* 状态,0-未初始化,1-已读取过文件,解析文件信息 */
    unsigned char status : 1;
    /* 数据发送类型(0-正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送) */
    unsigned char sendType : 3;
    /* 已发送的帧数 */
    unsigned long sendCount;
    /* 视频必须发完的时间 */
    double endUptime;
    ////////////////////////////////////////////////////////////////////////////
    /* 父目录长度 */
    unsigned int dirLen;
    /* 当前文件名 */
    char *thisFileName;
    /* 文件名链表,第一个为当前文件名,没有时为空 */
    FsStringList *fileNameList;
    /* 当前文件的开始时间,没有时应设置为0 */
    double fileStartTime;
    ////////////////////////////////////////////////////////////////////////////
    /* 监控文件名,在使用第三方储存时需要监控第三方是否把数据准备好了,为空表示本地储存 */
    char *tagFile;

    /* 外部储存时需要的参数,在tagFile不为0时有效 */
    struct {
        /* tagFile不为空时检查tag的超时时间 */
#define Record_P_VideoDemand_tagTimeout 32.0        
        double tagTimeout;
        /* 操作pEbml_node时使用的互斥锁 */
        pthread_mutex_t *pEbml_node_mutex;
        /* 在tagFile不为空时,表示此节点的配置数据对应的节点 */
        struct FsEbml_node *pEbml_node;
        /* sdk配置是否更新的指针 */
        unsigned long long *sdkConfigUpdate;
        /* 在pEbml_node内的内容更新后应设置的sdk编号 */
        unsigned char sdkConfigUpdate_index;
        /* 目录的变化编号,在发生跳转后需要使用新目录 */
        unsigned int dirChangeIndex;
        /* 标记文件的具体内容,由跳转时间+当前使用文件的开始时间+播放类型组成,在等到后把值置0 */
#define Record_P_VideoDemand_tag_is_not_use(___pVideoDemand)  (0.0 == ((double*) (___pVideoDemand)->externStorage->tag)[0]&& 0.0==((double*) (___pVideoDemand)->externStorage->tag)[1] && 0 == ((unsigned char*) (((double*) (___pVideoDemand)->externStorage->tag) + 2))[0])
        __attribute__ ((aligned(sizeof (double)))) unsigned char tag[sizeof (double)*2 + sizeof (unsigned char)];
    } externStorage[0];
};
#define __Record_P_VideoDemandLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,FsPrintfIndex,___format",p=%p,frameRateMulti=%f,status=%hhu,sendType=%hhu,sendCount=%lu,thisFileName:\"%s\".\n",##__VA_ARGS__\
        ,pVideoDemand,pVideoDemand->frameRateMulti,pVideoDemand->status,pVideoDemand->sendType,pVideoDemand->sendCount,pVideoDemand->thisFileName)
#define Record_P_VideoDemandLog(___loglevel,___format, ...) __Record_P_VideoDemandLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define Record_P_VideoDemandLog2(___loglevel,___format, ...) __Record_P_VideoDemandLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

/* 从配置获取通道数 */
static unsigned int record_P_get_channelCount_and_get_snapbuffertimeout_and_update_gbsdk(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList, struct Record * const pRecord) {
#define __get_channelCount_vsys_vsysChannel_in_record
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

/* 删除pVideoDemand指向的实例对象 */
static void record_P_VideoDemand_delete__OI(struct Record_P_VideoDemand * const pVideoDemand) {
    Record_P_VideoDemandLog2(FsLogType_info, "Delete");
    /* 文件名链表,第一个为当前文件名,没有时为空 */
    if (pVideoDemand->fileNameList) fs_StringList_delete__OI(pVideoDemand->fileNameList);
    /* 视频对象 */
    if (pVideoDemand->pMKV)video_mkv_delete__OI(pVideoDemand->pMKV);
    /* 视频文件对象 */
    if (pVideoDemand->fd)fclose(pVideoDemand->fd);
    ////////////////////////////////////////////////////////////////////////////
    if (pVideoDemand->tagFile) {
        struct FsEbml_node * const playback = pVideoDemand->externStorage->pEbml_node->parent;
        pthread_mutex_lock(pVideoDemand->externStorage->pEbml_node_mutex);
        fs_Ebml_node_delete(NULL, playback->childNodeCount > 1 ? pVideoDemand->externStorage->pEbml_node : playback);
        pthread_mutex_unlock(pVideoDemand->externStorage->pEbml_node_mutex);
        Record_sdkConfigUpdate_set_1_1(pVideoDemand->externStorage->sdkConfigUpdate, pVideoDemand->externStorage->sdkConfigUpdate_index);
    }
    fsFree(pVideoDemand);
}

/* 读取检查文件是否在扩展,成功返回1,失败返回-1 */
static inline signed char record_private_VideoDemand_file_extern(struct Record_P_VideoDemand * const pVideoDemand) {
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

/* 使用第三方储存时,读一次标记,校验成功返回1,错误返回-1,错误返回-2时设置开始时间标记 */
static int record_P_VideoDemand_update_read_tag(struct Record_P_VideoDemand * const pVideoDemand, /* 不为空表示把读到的开始时间标记写入*pStartTime */double *const pStartTime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName record_P_VideoDemand_update_read_tag
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, sizeof (double));
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    FsLocal_ShareBuffer_resize(sizeof (pVideoDemand->externStorage->tag)*2 + 1);
    const int readLen = fs_file_readMax(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, sizeof (pVideoDemand->externStorage->tag)*2, pVideoDemand->tagFile);
    if (readLen == sizeof (pVideoDemand->externStorage->tag)*2) {
        /* 读取成功 */
        if (0) {
            (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[sizeof (pVideoDemand->externStorage->tag)*2] = 0;
            FsPrintf(1, "tag:\"%s\"\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
        }
        const unsigned char *const v = pVideoDemand->externStorage->tag;
        unsigned int ui = 0;
        for (; ui<sizeof (pVideoDemand->externStorage->tag); ui++) {
            if ((v[ui] >> 4) != ((unsigned char*) (FsLocal_ShareBuffer + FsLocal_ShareBufferPos))[2 * ui] - 'a'
                    || (v[ui] &0xF) != ((unsigned char*) (FsLocal_ShareBuffer + FsLocal_ShareBufferPos))[2 * ui + 1] - 'a') {
                goto FsMacrosFunctionTag(error);
            }
        }
        goto FsMacrosFunctionTag(end);
    } else if (pStartTime && sizeof (double) == readLen) {
        /* 读到文件的时间标记 */
        *pStartTime = *(double*) (FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
        goto FsMacrosFunctionTag(error__2);
    } else {
        /* 读取失败 */
        //FsPrintf(1, "read tagFile:\"%s\"\n", pVideoDemand->tagFile);
        goto FsMacrosFunctionTag(error);
    }
    ////////////////////////////////////////////////////////////////////////////
    FsMacrosFunctionTag(end) :;
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    return 1;
    FsMacrosFunctionTag(error) :;
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    return -1;
    FsMacrosFunctionTag(error__2) :;
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);

    return -2;
}

/* 使用第三方储存时,写标记,应检查到tag有变化后再调用此函数 */
static void record_P_VideoDemand_update_write_tag(struct Record_P_VideoDemand * const pVideoDemand, const unsigned char sendType, /* 当前使用的文件开始时间,在正序播放时,如最后一个文件就是当前文件,应该向后再准备一个文件,没有填0 */const double thisFileStartTime, const double playTime, const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#ifdef FsDebug
    if (((double*) pVideoDemand->externStorage->tag)[0] == playTime && ((double*) pVideoDemand->externStorage->tag)[1] == thisFileStartTime && ((unsigned char*) (((double*) pVideoDemand->externStorage->tag) + 2))[0] == sendType) {
        Record_P_VideoDemandLog(FsLogType_fatal, "tag not change,playTime=%lf/%lf,thisFileStartTime=%lf/%lf,sendType=%hhu/%hhu"
                , playTime, ((double*) pVideoDemand->externStorage->tag)[0], thisFileStartTime, ((double*) pVideoDemand->externStorage->tag)[1], sendType, ((unsigned char*) (((double*) pVideoDemand->externStorage->tag) + 2))[0]);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
#endif
    Record_P_VideoDemandLog2(FsLogType_info, "tag change,playTime=%lf/%lf,thisFileStartTime=%lf/%lf,sendType=%hhu/%hhu"
            , playTime, ((double*) pVideoDemand->externStorage->tag)[0], thisFileStartTime, ((double*) pVideoDemand->externStorage->tag)[1], sendType, ((unsigned char*) (((double*) pVideoDemand->externStorage->tag) + 2))[0]);
    if (/* 标记未使用 */Record_P_VideoDemand_tag_is_not_use(pVideoDemand) || /* 检查标记是否已完成 */1 == record_P_VideoDemand_update_read_tag(pVideoDemand, NULL, pShareBuffer)) {
        /* 更新等待时间 */
        if (pVideoDemand->externStorage->tagTimeout < thisUptime + Record_P_VideoDemand_tagTimeout) pVideoDemand->externStorage->tagTimeout = thisUptime + Record_P_VideoDemand_tagTimeout;
    }
    pthread_mutex_lock(pVideoDemand->externStorage->pEbml_node_mutex);
    if (((unsigned char*) (((double*) pVideoDemand->externStorage->tag) + 2))[0] != sendType) {
        ((unsigned char*) (((double*) pVideoDemand->externStorage->tag) + 2))[0] = sendType;
        struct FsEbml_node * const pEbml_node = fs_Ebml_node_get_first(NULL, pVideoDemand->externStorage->pEbml_node, "speed");
        if (sendType & 0x1)memcpy(pEbml_node->data.buffer, FsStringDataLen0("-64.0")), pEbml_node->data.lenth = sizeof ("-64.0");
        else memcpy(pEbml_node->data.buffer, FsStringDataLen0("64.0")), pEbml_node->data.lenth = sizeof ("64.0");
    }
    if (((double*) pVideoDemand->externStorage->tag)[0] != playTime) {
        ((double*) pVideoDemand->externStorage->tag)[0] = playTime;
        struct FsEbml_node * const pEbml_node = fs_Ebml_node_get_first(NULL, pVideoDemand->externStorage->pEbml_node, "time");
        pEbml_node->data.lenth = fs_time_to_stringGMT(pEbml_node->data.buffer, playTime);
        pEbml_node->data.buffer[pEbml_node->data.lenth++] = 0;
    }
    if (((double*) pVideoDemand->externStorage->tag)[1] != thisFileStartTime) {
        ((double*) pVideoDemand->externStorage->tag)[1] = thisFileStartTime;
        struct FsEbml_node * const pEbml_node = fs_Ebml_node_get_first(NULL, pVideoDemand->externStorage->pEbml_node, "ftime");
        pEbml_node->data.lenth = fs_time_to_stringGMT(pEbml_node->data.buffer, thisFileStartTime);
        pEbml_node->data.buffer[pEbml_node->data.lenth++] = 0;
    }
    {
        struct FsEbml_node * const pEbml_node = fs_Ebml_node_get_first(NULL, pVideoDemand->externStorage->pEbml_node, "tagdata");
        const unsigned char *const v = pVideoDemand->externStorage->tag;
        unsigned char *pd = (unsigned char *) pEbml_node->data.buffer;
#ifndef __Record_P_VideoDemand_set_tag 
        {
            unsigned int ui = 0;
            for (; ui<sizeof (((struct Record_P_VideoDemand*) 0)->externStorage->tag); ui++) {
                *pd++ = (v[ui] >> 4) + 'a';
                *pd++ = (v[ui] &0xF) + 'a';
            }
        }
#endif
    }
    pthread_mutex_unlock(pVideoDemand->externStorage->pEbml_node_mutex);
    Record_sdkConfigUpdate_set_1_1(pVideoDemand->externStorage->sdkConfigUpdate, pVideoDemand->externStorage->sdkConfigUpdate_index);
}

/* 使用第三方储存时,等待第三方的完成标记,完成返回1,继续等待返回-101(pStartTime不为空时可能设置开始时间标记),错误返回-1 */
static int record_P_VideoDemand_update_wait_tag(struct Record_P_VideoDemand * const pVideoDemand, /* 不为空表示把读到的开始时间标记写入*pStartTime */double *const pStartTime
        , const unsigned char sendType, /* 当前使用的文件开始时间,在正序播放时,如最后一个文件就是当前文件,应该向后再准备一个文件,没有填0 */const double thisFileStartTime, /* 准备播放的文件时间 */const double playTime, const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (((double*) pVideoDemand->externStorage->tag)[0] != playTime || ((double*) pVideoDemand->externStorage->tag)[1] != thisFileStartTime || ((unsigned char*) (((double*) pVideoDemand->externStorage->tag) + 2))[0] != sendType) {
        /* 需要更新 */
        record_P_VideoDemand_update_write_tag(pVideoDemand, sendType, thisFileStartTime, playTime, thisUptime, pShareBuffer);
        return -101;
    } else {
        /* 等待标记 */
        if (1 == record_P_VideoDemand_update_read_tag(pVideoDemand, pStartTime, pShareBuffer)) {
            /* 标记等到了 */
            ((double*) pVideoDemand->externStorage->tag)[0] = 0.0;
            ((double*) pVideoDemand->externStorage->tag)[1] = 0.0;
            (((unsigned char*) ((double*) pVideoDemand->externStorage->tag) + 2))[0] = 0;
            return 1;
        } else if (pVideoDemand->externStorage->tagTimeout < thisUptime) {
            /* 超时了 */
            FsLog(FsLogType_timeoutError, FsPrintfIndex, "Timeout,%lf/%lf,tagFile:\"%s\".\n", pVideoDemand->externStorage->tagTimeout, thisUptime, pVideoDemand->tagFile);
            fflush(stdout);
            return -1;
        } else {
            /* 继续等待 */
            return -101;
        }
    }
}

/* 读取下一个文件,成功返回1,需要等待返回-101,失败返回-1 */
static int record_P_VideoDemand_readNextFile(struct Record_P_VideoDemand * const pVideoDemand
        , /* 见pRecord_item->ro._recordVideoMode中的解释 */const unsigned char recordVideoMode
        , /* 是否强制读取下一个,0-不强制(如在nvr存时会先检查标记是否被设置,没被设置不会读取),1-强制 */const unsigned char forceRead
        , const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (0 == forceRead && Record_item_is_remoteVideoStorage(recordVideoMode)&&/* 是最后一个文件 */1 == pVideoDemand->fileNameList->list.nodeCount) {
        double remoteVideoStorage_startTime = -1.0;
        const int rst = record_P_VideoDemand_update_wait_tag(pVideoDemand, &remoteVideoStorage_startTime, pVideoDemand->sendType
                , pVideoDemand->fileStartTime, pVideoDemand->fileStartTime + ((pVideoDemand->sendType & 0x1) == 0 ? pVideoDemand->fileTime : 0.0), thisUptime, pShareBuffer);
        FsPrintf(1, "rv=%d,remoteVideoStorage_startTime=%lf/%lf/%lf,%s\n", rst, remoteVideoStorage_startTime, pVideoDemand->fileStartTime, pVideoDemand->fileTime, pVideoDemand->tagFile);
        if (rst != 1) {
            if (-101 == rst && /* 当前是正序发送 */0 == (pVideoDemand->sendType & 0x1) && remoteVideoStorage_startTime > 0.0) {
                /* 是正序,尝试此文件是否在扩展 */
                if (record_private_VideoDemand_file_extern(pVideoDemand) == 1)return 1;
                if (/* 读到的时间比当前时间大 */remoteVideoStorage_startTime >= pVideoDemand->fileStartTime + 1.0) {
                    /* 可尝试读取下一个文件 */
                } else return rst;
            } else return rst;
        } else if (/* 当前是正序发送 */0 == (pVideoDemand->sendType & 0x1)) {
            /* 虽然返回1,但原来文件也可能在扩展 */
            if (record_private_VideoDemand_file_extern(pVideoDemand) == 1)return 1;
        }
    }
    /* 正向须保留一个文件 */
    if (pVideoDemand->fileNameList->list.nodeCount > 1) {
        fsFree(fs_StringList_remove_head__IO(pVideoDemand->fileNameList));
        strcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, (char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
    } else {
        /* 只有一个文件 */
        if (/* 本地储存才需要扩展 */ Record_item_is_localVideoStorage(recordVideoMode)) {
            if ((pVideoDemand->sendType & 0x1) == 0) {
                /* 是正序,尝试此文件是否在扩展 */
                if (record_private_VideoDemand_file_extern(pVideoDemand) == 1)return 1;
            }
        }
        pVideoDemand->thisFileName[pVideoDemand->dirLen] = 0;
        FsStringList * const list = fs_dir_match_prefect__IO(pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex], (pVideoDemand->sendType & 0x1)^0x1);
        if (NULL == list) {
            /* 当前已有的文件都找不到了,出错 */
            FsPrintf(1, "Can not find the similar file:\"%s%s\".\n", pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
            fflush(stdout);
            return -1;
        }
        if (0 == strcmp((const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex], (const char*) list->list.pNode[ list->list.startIndex])) {
            /* 应该是相等的 */
            if (1 == list->list.nodeCount) {
                if (pVideoDemand->sendType & 0x1) {
                    /* 倒序已到极限了 */
                    FsPrintf(1, "Can not find the similar file:\"%s%s\".\n", pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
                    fflush(stdout);
                    fs_StringList_delete__OI(list);
                    return -1;
                }
                if (/* 远程不应该读取不到 */Record_item_is_remoteVideoStorage(recordVideoMode)) {
                    FsPrintf(1, "Can not find the similar file:\"%s%s\".\n", pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
                    fflush(stdout);
                    fs_StringList_delete__OI(list);
                    return -1;
                }
            }
            FsPrintf(1, "\nname:\"%s%s\"\n", pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
            fs_StringList_out(list, stdout, NULL, "\n");
            fs_StringList_delete__OI(pVideoDemand->fileNameList);
            pVideoDemand->fileNameList = list;
            if ((pVideoDemand->sendType & 0x1) == 0) {
                /* 是正序,尝试此文件是否在扩展 */
                if (0) {
                    memcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, list->list.pNode[list->list.startIndex], sizeof ("20151009/15/video20151009150400.mkv"));
                    if (record_private_VideoDemand_file_extern(pVideoDemand) == 1 || 1 == list->list.nodeCount)return 1;
                    else fsFree(fs_StringList_remove_head__IO(list));
                } else {
                    /* 之前已尝试过读取当前文件,不需要再尝试 */
                    if (1 == list->list.nodeCount) {
                        return 1;
                    } else fsFree(fs_StringList_remove_head__IO(list));
                }
            } else fsFree(fs_StringList_remove_head__IO(list));
            strcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, (char*) list->list.pNode[list->list.startIndex]);
        } else {
            /* 提示可能出现错误 */
            FsPrintf(1, "Has some error to find the similar file:\"%s%s\".\n", pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
            fflush(stdout);
            FsPrintf(1, "\nname:\"%s%s\"\n", pVideoDemand->thisFileName, (const char*) pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
            fs_StringList_out(list, stdout, NULL, "\n");
            fs_StringList_delete__OI(pVideoDemand->fileNameList);
            pVideoDemand->fileNameList = list;
            strcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, (char*) list->list.pNode[list->list.startIndex]);
        }
    }
    /* 根据上一个文件推算时间 */
    double fileStartTimeCalByPreviosFile;
    {
        if (0 == (pVideoDemand->sendType & 0x1)) {
            fileStartTimeCalByPreviosFile = pVideoDemand->fileStartTime + (pVideoDemand->pMKV->videoIndex > 0 ?
                    pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount / (pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].p.video.frameRate * pVideoDemand->frameRateMulti) :
                    pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount / (pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].p.audio.frameRate * pVideoDemand->frameRateMulti));
        } else {
            fileStartTimeCalByPreviosFile = pVideoDemand->fileStartTime - (pVideoDemand->pMKV->videoIndex > 0 ?
                    pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount / (pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].p.video.frameRate * pVideoDemand->frameRateMulti) :
                    pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount / (pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].p.audio.frameRate * pVideoDemand->frameRateMulti));
        }
    }
#ifdef FsDebug

    FsPrintf2(1, "Use file:\"%s\",fileStartTimeCalByPreviosFile=%lf/%lf/%lf.\n", pVideoDemand->thisFileName, fileStartTimeCalByPreviosFile, pVideoDemand->fileStartTime, pVideoDemand->fileTime);
#endif
    /* 当前文件的开始时间 */
#ifndef __record_P_VideoDemand_read_time_from_file 
    {
        struct tm ti;
        memset(&ti, 0, sizeof (struct tm));
        char *pTime = strstr(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15") - 1, "/video");
        if (NULL == pTime) {
            Record_P_VideoDemandLog(FsLogType_fatal, "Get time failed,str:\"%s\".\n", pVideoDemand->thisFileName);
            fflush(stdout);
            FsLogExit();
        }
        sscanf(pTime + sizeof ("/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
        //sscanf(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
        ti.tm_year -= 1900, ti.tm_mon -= 1;
        pVideoDemand->fileStartTime = mktime(&ti);
    }
#endif

    //FsPrintf2(1, "Use file---:\"%s\",%lf.\n", pVideoDemand->thisFileName, pVideoDemand->fileStartTime);
    ////////////////////////////////////////////////////////////////////////////
    video_mkv_delete__OI(pVideoDemand->pMKV);
    pVideoDemand->pMKV = NULL;
    fclose(pVideoDemand->fd);
    /* 视频文件对象 */
    pVideoDemand->fd = fopen(pVideoDemand->thisFileName, "rb");
    if (NULL == pVideoDemand->fd) {
        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", pVideoDemand->thisFileName, strerror(errno));
        fflush(stdout);
        return -1;
    }
    /* 视频对象 */
    VideoMKV *pMKV = video_mkv_new_from_file__IO(pVideoDemand->fd);
    if (NULL == pMKV) {
        FsPrintf(1, "Error new mkv from the file:\"%s\".\n", pVideoDemand->thisFileName);
        fflush(stdout);
        fclose(pVideoDemand->fd);
        pVideoDemand->fd = NULL;
        return -1;
    }
    pVideoDemand->pMKV = pMKV;

    /* 获取keydata */
    if (pMKV->videoIndex) {
        unsigned int l = pMKV->videoCount + pMKV->audioCount + pMKV->subtitleCount;
#ifndef __record_P_VideoDemand_keydata
        for (; l > 0; l--) {
            if (VideoMKV_trackVideo == pMKV->channel[l].type) {
                if (2 == pMKV->channel[l].p.video.encodeType) {
                    if (video_mkv_h264_get_keyData(pMKV->channel[l].p.video.codecPrivate, pMKV->channel[l].p.video.codecPrivate) != 1) {
                        FsPrintf(5, "Get keydata from the file:\"%s\".\n", pVideoDemand->thisFileName);
                        fflush(stdout);
                        return -1;
                    }
                } else if (3 == pMKV->channel[l].p.video.encodeType) {
                    if (video_mkv_h265_get_keyData(pMKV->channel[l].p.video.codecPrivate, pMKV->channel[l].p.video.codecPrivate) != 1) {
                        FsPrintf(5, "Get keydata from the file:\"%s\".\n", pVideoDemand->thisFileName);
                        fflush(stdout);
                        return -1;
                    }
                } else {
                    FsPrintf(5, "Unknown encodeType(=%hhu) from the file:\"%s\".\n", pMKV->channel[l].p.video.encodeType, pVideoDemand->thisFileName);
                    fflush(stdout);
                    return -1;
                }
            }
        }
#endif
    }
    if (pMKV->videoIndex) {
        /* 有视频 */
        /* 当前使用的帧率 */
        if (pVideoDemand->fpsSet <= 0.0) pVideoDemand->fps = pMKV->channel[pMKV->videoIndex].p.video.frameRate;
        /* 文件时长 */
        pVideoDemand->fileTime = pMKV->channel[pMKV->videoIndex].list->nodeCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
        /* 视频必须发完的时间 */
        pVideoDemand->endUptime = fs_time_uptime_get() + pMKV->channel[pMKV->videoIndex].list->nodeCount / pVideoDemand->fps;
    } else {
        /* 无视频 */
        /* 当前使用的帧率 */
        if (pVideoDemand->fpsSet <= 0.0) pVideoDemand->fps = pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
        /* 文件时长 */
        pVideoDemand->fileTime = pMKV->channel[pMKV->audioIndex].list->nodeCount / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
        /* 视频必须发完的时间 */
        pVideoDemand->endUptime = fs_time_uptime_get() + pMKV->channel[pMKV->audioIndex].list->nodeCount / pVideoDemand->fps;
    }
    /* 帧率的倍率,在多文件切换时,时间可能与文件名有偏差,需要使用此值校正,让发送时间也真实时间接近 */
    {
        float diff = fileStartTimeCalByPreviosFile - pVideoDemand->fileStartTime;
        if (0 == (pVideoDemand->sendType & 0x1)) {
#ifndef __record_P_VideoDemand_readNextFile_cal_frameRateMulti 
            if (diff
#ifdef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
                    <
#else
                    >
#endif
                    0) {
                /* 推算变化时间比真实时间多,应把本文件的帧率调高 */
#ifdef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
                diff = -diff;
#endif
                diff = pVideoDemand->fileTime + 1.0 - diff;
                if (diff < (pVideoDemand->fileTime + 1.0) / 2.0) {
                    /* 偏差过大 */
                    Record_P_VideoDemandLog(FsLogType_error, "diff(=%f) < (pVideoDemand->fileTime(=%lf) + 1.0) / 2.0", diff, pVideoDemand->fileTime);
                    fflush(stdout);
                    diff = (pVideoDemand->fileTime + 1.0) / 2.0;
                }
                pVideoDemand->frameRateMulti = (pVideoDemand->fileTime + 1.0) / diff;
            } else {
                /* 推算变化时间比真实时间少,应把本文件的帧率调低 */
#ifndef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
                diff = -diff;
#endif
                if (diff > 2.0) {
                    /* 偏差过大,可能是数据不连续产生了跳转 */
                    Record_P_VideoDemandLog(FsLogType_error, "diff(=%f) > 2.0", diff);
                    fflush(stdout);
                    pVideoDemand->frameRateMulti = 1.0;
                } else {
                    diff = pVideoDemand->fileTime + diff + 1.0;
                    pVideoDemand->frameRateMulti = (pVideoDemand->fileTime + 1.0) / diff;
                    if (pVideoDemand->frameRateMulti < 0.8) {
                        /* 调整控制在20%以内 */
                        Record_P_VideoDemandLog(FsLogType_error, "pVideoDemand->frameRateMulti(=%f) < 0.8", pVideoDemand->frameRateMulti);
                        fflush(stdout);
                        pVideoDemand->frameRateMulti = 0.8;
                    }
                }
            }
#ifdef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
#undef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
#endif
#endif
        } else {
            pVideoDemand->fileStartTime += pVideoDemand->fileTime;
#define __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
#ifndef __record_P_VideoDemand_readNextFile_cal_frameRateMulti 
            if (diff
#ifdef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
                    <
#else
                    >
#endif
                    0) {
                /* 推算变化时间比真实时间多,应把本文件的帧率调高 */
#ifdef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
                diff = -diff;
#endif
                diff = pVideoDemand->fileTime + 1.0 - diff;
                if (diff < (pVideoDemand->fileTime + 1.0) / 2.0) {
                    /* 偏差过大 */
                    Record_P_VideoDemandLog(FsLogType_error, "diff(=%f) < (pVideoDemand->fileTime(=%lf) + 1.0) / 2.0", diff, pVideoDemand->fileTime);
                    fflush(stdout);
                    diff = (pVideoDemand->fileTime + 1.0) / 2.0;
                }
                pVideoDemand->frameRateMulti = (pVideoDemand->fileTime + 1.0) / diff;
            } else {
                /* 推算变化时间比真实时间少,应把本文件的帧率调低 */
#ifndef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
                diff = -diff;
#endif
                if (diff > 2.0) {
                    /* 偏差过大,可能是数据不连续产生了跳转 */
                    Record_P_VideoDemandLog(FsLogType_error, "diff(=%f) > 2.0", diff);
                    fflush(stdout);
                    pVideoDemand->frameRateMulti = 1.0;
                } else {
                    diff = pVideoDemand->fileTime + diff + 1.0;
                    pVideoDemand->frameRateMulti = (pVideoDemand->fileTime + 1.0) / diff;
                    if (pVideoDemand->frameRateMulti < 0.8) {
                        /* 调整控制在20%以内 */
                        Record_P_VideoDemandLog(FsLogType_error, "pVideoDemand->frameRateMulti(=%f) < 0.8", pVideoDemand->frameRateMulti);
                        fflush(stdout);
                        pVideoDemand->frameRateMulti = 0.8;
                    }
                }
            }
#ifdef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
#undef __record_P_VideoDemand_readNextFile_cal_frameRateMulti_sendType_1
#endif
#endif
        }
    }
    /* 已发送的帧数 */
    pVideoDemand->sendCount = 0;

    return 1;
}

/* 更新目录,在cdn回放时需要(发生跳转时需要更显目录) */
static void record_P_VideoDemand_update_dir(struct Record_P_VideoDemand * const pVideoDemand) {
    pVideoDemand->externStorage->dirChangeIndex = pVideoDemand->externStorage->dirChangeIndex < FsNumberMax(pVideoDemand->externStorage->dirChangeIndex) ? pVideoDemand->externStorage->dirChangeIndex + 1 : 0;
    unsigned int pos = pVideoDemand->dirLen;
    while (pVideoDemand->thisFileName[pos - 1] != '_')pos--;
    pVideoDemand->dirLen = pos + sprintf(pVideoDemand->thisFileName + pos, "%u/", pVideoDemand->externStorage->dirChangeIndex);
    memcpy(pVideoDemand->tagFile + pos, pVideoDemand->thisFileName + pos, pVideoDemand->dirLen - pos);
    memcpy(pVideoDemand->tagFile + pVideoDemand->dirLen, FsStringDataLen0("__tag"));
    {
        FsString * const pString = fs_String_new__IO(pVideoDemand->dirLen + sizeof ("__tag") - 1);
        memcpy(pString->buffer, pVideoDemand->tagFile, pVideoDemand->dirLen + sizeof ("__tag") - 1);
        pString->lenth += 1;
        fs_Ebml_node_data_set__OI_2(fs_Ebml_node_get_first(NULL, pVideoDemand->externStorage->pEbml_node, "tag"), pString);
    }
    Record_P_VideoDemandLog2(FsLogType_info, "update dir,dirLen=%u,dirChangeIndex=%u,tagFile:\"%s\"", pVideoDemand->dirLen, pVideoDemand->externStorage->dirChangeIndex, pVideoDemand->tagFile);
    Record_sdkConfigUpdate_set_1_1(pVideoDemand->externStorage->sdkConfigUpdate, pVideoDemand->externStorage->sdkConfigUpdate_index);
}

/* 更新点播参数,可实现跳转,成功返回1,需要等待返回-101,失败返回-1 */
static int record_P_VideoDemand_update(struct Record_P_VideoDemand * const pVideoDemand, /* 见pRecord_item->ro._recordVideoMode中的解释 */const unsigned char recordVideoMode
        , /* 数据发送类型(0--正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送) */ const unsigned char sendType
        , /* 请求数据的时间,在原来有数据且sendType与原pVideoDemand中的sendType不相同时会忽略此值 */const double gmtTime
        , const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    /* 数据发送类型(0--正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送) */
    VideoMKV *pMKV = pVideoDemand->pMKV;
    if (pVideoDemand->fileNameList) {
        if (pVideoDemand->sendType == sendType) {
            /* 是跳转 */
            /* 帧率的倍率,在多文件切换时,时间可能与文件名有偏差,需要使用此值校正,让发送时间也真实时间接近 */
            pVideoDemand->frameRateMulti = 1.0;
            /* 已有数据 */
            /* 判断请求的时间是否在当前文件的时间内 */
            if (gmtTime >= pVideoDemand->fileStartTime && gmtTime < pVideoDemand->fileStartTime + pVideoDemand->fileTime) {
                /* 请求的时间在当前文件的时间内 */
                if (pMKV->videoIndex) {
                    /* 有视频 */
                    /* 已发送的帧数 */
                    pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].p.video.frameRate * (gmtTime - pVideoDemand->fileStartTime);
                    if (0 == (sendType & 0x1)) {
                        /* 向前找关键帧 */
                        for (; pVideoDemand->sendCount > 0 &&
                                0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex + pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount--);
                    } else {
                        if (0) {
                            /* 向后找关键帧 */
                            for (; pVideoDemand->sendCount < pMKV->channel[pMKV->videoIndex].list->nodeCount &&
                                    0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex + pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount++);
                            if (pVideoDemand->sendCount == pMKV->channel[pMKV->videoIndex].list->nodeCount) {
                                /* 向前找关键帧 */
                                for (pVideoDemand->sendCount--; pVideoDemand->sendCount > 0 &&
                                        0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex + pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount--);
                            }
                        }
                        pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount - 1;
                        if (pVideoDemand->sendCount > 0)pVideoDemand->sendCount--;
                        FsPrintf(1, "gmtTime=%lf/%lf\n", gmtTime, pVideoDemand->fileStartTime +
                                (pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount - 1) / pMKV->channel[pMKV->videoIndex].p.video.frameRate);
                        fflush(stdout);
                    }
                    /* 视频必须发完的时间 */
                    pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps - 1.0;
                } else {
                    /* 无视频 */
                    /* 已发送的帧数 */
                    pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].p.audio.frameRate * (gmtTime - pVideoDemand->fileStartTime);
                    if (sendType & 0x1) {
                        pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount - 1;
                        if (pVideoDemand->sendCount > 0)pVideoDemand->sendCount--;
                    }
                    /* 视频必须发完的时间 */
                    pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps - 1.0;
                }
                return 1;
            } else {
                if (Record_item_is_remoteVideoStorage(recordVideoMode)) {
                    /* 是远程储存,必须要重新请求数据 */
                    video_mkv_delete__OI(pMKV);
                    pVideoDemand->pMKV = NULL;
                    fclose(pVideoDemand->fd);
                    pVideoDemand->fd = NULL;
                    fs_StringList_delete__OI(pVideoDemand->fileNameList);
                    pVideoDemand->fileNameList = NULL;
                    pVideoDemand->fileStartTime = 0.0;
                    record_P_VideoDemand_update_dir(pVideoDemand);
                    ((double*) pVideoDemand->externStorage->tag)[0] = 0.0; // 避免之前的标记与跳转时间相同,而不会更新标记
                } else {
                    /* 在缓存链表内查找 */
                    video_mkv_delete__OI(pMKV);
                    pVideoDemand->pMKV = NULL;
                    fclose(pVideoDemand->fd);
                    pVideoDemand->fd = NULL;
                    time_t ts = (time_t) gmtTime;
                    struct tm ti;
                    localtime_r(&ts, &ti);
                    sprintf(pVideoDemand->thisFileName + pVideoDemand->dirLen, "%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv",
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    long j = pVideoDemand->fileNameList->list.nodeCount;
                    char **pNode = (char**) (pVideoDemand->fileNameList->list.pNode + pVideoDemand->fileNameList->list.startIndex);
                    long i = 0, k;
                    if (0 == (sendType & 0x1)) {
                        do {
                            k = (i + j) >> 1;
                            if (memcmp(pVideoDemand->thisFileName + pVideoDemand->dirLen, pNode[k], sizeof ("20151009/15/video20151009150400") - 1) < 0) j = k;
                            else i = k + 1;
                        } while (j > i);
                        if (j < (long) pVideoDemand->fileNameList->list.nodeCount && j-- > 0 &&
                                memcmp(pVideoDemand->thisFileName + pVideoDemand->dirLen, pNode[j], sizeof ("20151009/15/video20151009150400") - 1) >= 0) {
                            /* 找到了 */
                            while (j-- > 0)fsFree(fs_StringList_remove_head__IO(pVideoDemand->fileNameList));
                            memcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex], sizeof ("20151009/15/video20151009150400.mkv"));
#ifdef FsDebug
                            FsPrintf2(1, "Use file:\"%s\".\n", pVideoDemand->thisFileName);
#endif
                            struct tm ti;
                            memset(&ti, 0, sizeof (struct tm));
                            /* 当前文件的开始时间 */
                            sscanf(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                            ti.tm_year -= 1900;
                            ti.tm_mon -= 1;
                            pVideoDemand->fileStartTime = mktime(&ti);
                        } else {
                            fs_StringList_delete__OI(pVideoDemand->fileNameList);
                            pVideoDemand->fileNameList = NULL;
                            pVideoDemand->fileStartTime = 0.0;
                        }
                    } else {
                        do {
                            k = (i + j) >> 1;
                            if (memcmp(pVideoDemand->thisFileName + pVideoDemand->dirLen, pNode[k], sizeof ("20151009/15/video20151009150400")) < 0) i = k + 1;
                            else j = k;
                        } while (j > i);
                        if (j > 0 && j < (long) pVideoDemand->fileNameList->list.nodeCount) {
                            /* 找到了 */
                            while (j-- > 0)fsFree(fs_StringList_remove_head__IO(pVideoDemand->fileNameList));
                            memcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex], sizeof ("20151009/15/video20151009150400.mkv"));
#ifdef FsDebug
                            FsPrintf2(1, "Use file:\"%s\".\n", pVideoDemand->thisFileName);
#endif
                            /* 当前文件的开始时间 */
#ifndef __record_P_VideoDemand_read_time_from_file 
                            {
                                struct tm ti;
                                memset(&ti, 0, sizeof (struct tm));
                                char *pTime = strstr(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15") - 1, "/video");
                                if (NULL == pTime) {
                                    Record_P_VideoDemandLog(FsLogType_fatal, "Get time failed,str:\"%s\".\n", pVideoDemand->thisFileName);
                                    fflush(stdout);
                                    FsLogExit();
                                }
                                sscanf(pTime + sizeof ("/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                                //sscanf(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                                ti.tm_year -= 1900, ti.tm_mon -= 1;
                                pVideoDemand->fileStartTime = mktime(&ti);
                            }
#endif
                        } else {
                            fs_StringList_delete__OI(pVideoDemand->fileNameList);
                            pVideoDemand->fileNameList = NULL;
                            pVideoDemand->fileStartTime = 0.0;
                        }
                    }
                }
            }
        } else {
            /* 是改变数据的发送模式 */
            if ((pVideoDemand->sendType & 0x1) != (sendType & 0x1)) {
                /* 改变了方向 */
                while (pVideoDemand->fileNameList->list.nodeCount > 1)fsFree(fs_StringList_remove_tail__IO(pVideoDemand->fileNameList));
                if (pMKV->videoIndex) {
                    pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount;
                    if ((sendType & 0x1) == 0) {
                        /* 是正向发送,应向前找关键帧 */
                        for (; pVideoDemand->sendCount > 0 &&
                                0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex +
                                pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount--);
                    }
                    /* 视频必须发完的时间 */
                    pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps -
                            1.0 - pVideoDemand->fpsSet / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
                    FsPrintf(1, "pVideoDemand->fpsSet / pMKV->channel[pMKV->videoIndex].p.video.frameRate=%lf\n", pVideoDemand->fpsSet / pMKV->channel[pMKV->videoIndex].p.video.frameRate);
                } else {
                    pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount;
                    /* 视频必须发完的时间 */
                    pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps -
                            1.0 - pVideoDemand->fpsSet / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
                }
            } else if (1 || 0 == sendType) {
                /*  */
                FsPrintf(1, "%lf\n", pVideoDemand->fileStartTime + pVideoDemand->sendCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate);
            }
            pVideoDemand->sendType = sendType;
            return 1;
        }
    }
    double remoteVideoStorage_startTime = -1.0;
    int rst;
    if (Record_item_is_remoteVideoStorage(recordVideoMode)) {
        rst = record_P_VideoDemand_update_wait_tag(pVideoDemand, &remoteVideoStorage_startTime, sendType, pVideoDemand->fileStartTime, gmtTime, thisUptime, pShareBuffer);
        FsPrintf(1, "rv=%d,remoteVideoStorage_startTime=%lf,fileStartTime=%lf,sendType=%hhu,gmtTime=%lf,%s\n", rst, remoteVideoStorage_startTime, pVideoDemand->fileStartTime, sendType, gmtTime, pVideoDemand->tagFile);
        if (rst != 1) {
            if (/* 是新读取数据 */NULL == pVideoDemand->fileNameList&&-101 == rst && /* 当前是正序发送 */0 == (pVideoDemand->sendType & 0x1) && remoteVideoStorage_startTime > 0.0) {
                if (/* 读到的时间比当前时间大 */remoteVideoStorage_startTime >= pVideoDemand->fileStartTime + 1.0) {
                    /* 可尝试读取下一个文件 */

                } else {
                    return rst;
                }
            } else return rst;
        }
    } else rst = 1;
    /* 帧率的倍率,在多文件切换时,时间可能与文件名有偏差,需要使用此值校正,让发送时间也真实时间接近 */
    pVideoDemand->frameRateMulti = 1.0;
    pVideoDemand->sendType = sendType;
    if (NULL == pVideoDemand->fileNameList) {
        /* 读取文件名 */
        {
            time_t ts = (time_t) (1 == rst ? gmtTime : (Fs_num_min(remoteVideoStorage_startTime, gmtTime)));
            // int msec = ( int) ((gmtTime - ts)*1000);
            struct tm ti;
            localtime_r(&ts, &ti);
            sprintf(pVideoDemand->thisFileName + pVideoDemand->dirLen + 1, "%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv",
                    1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                    1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
            pVideoDemand->thisFileName[pVideoDemand->dirLen] = 0;
            pVideoDemand->fileNameList = fs_dir_match_prefect__IO(pVideoDemand->thisFileName, pVideoDemand->thisFileName + pVideoDemand->dirLen + 1, (sendType & 0x1)^0x1);
            if (NULL == pVideoDemand->fileNameList) {
                FsPrintf(1, "Can not find the similar file:\"%s\".\n", pVideoDemand->thisFileName);
                fflush(stdout);
                return -1;
            }
        }
        FsPrintf(1, "name:\"%s\"\n", pVideoDemand->thisFileName);
        fs_StringList_out(pVideoDemand->fileNameList, stdout, NULL, "\n");
        strcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
#ifdef FsDebug
        FsPrintf2(1, "Use file:\"%s\".\n", pVideoDemand->thisFileName);
#endif
        /* 当前文件的开始时间 */
#ifndef __record_P_VideoDemand_read_time_from_file 
        {
            struct tm ti;
            memset(&ti, 0, sizeof (struct tm));
            char *pTime = strstr(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15") - 1, "/video");
            if (NULL == pTime) {
                Record_P_VideoDemandLog(FsLogType_fatal, "Get time failed,str:\"%s\".\n", pVideoDemand->thisFileName);
                fflush(stdout);
                FsLogExit();
            }
            sscanf(pTime + sizeof ("/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
            //sscanf(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
            ti.tm_year -= 1900, ti.tm_mon -= 1;
            pVideoDemand->fileStartTime = mktime(&ti);
        }
#endif
        if (0x1 == (sendType & 0x1) && gmtTime < pVideoDemand->fileStartTime) {
            /* 读取下一个文件 */
            if (pVideoDemand->fileNameList->list.nodeCount > 1) {
                fsFree(fs_StringList_remove_head__IO(pVideoDemand->fileNameList));
                strcpy(pVideoDemand->thisFileName + pVideoDemand->dirLen, pVideoDemand->fileNameList->list.pNode[pVideoDemand->fileNameList->list.startIndex]);
#ifdef FsDebug
                FsPrintf2(1, "Use file:\"%s\".\n", pVideoDemand->thisFileName);
#endif
                /* 当前文件的开始时间 */
#ifndef __record_P_VideoDemand_read_time_from_file 
                {
                    struct tm ti;
                    memset(&ti, 0, sizeof (struct tm));
                    char *pTime = strstr(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15") - 1, "/video");
                    if (NULL == pTime) {
                        Record_P_VideoDemandLog(FsLogType_fatal, "Get time failed,str:\"%s\".\n", pVideoDemand->thisFileName);
                        fflush(stdout);
                        FsLogExit();
                    }
                    sscanf(pTime + sizeof ("/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                    //sscanf(pVideoDemand->thisFileName + pVideoDemand->dirLen + sizeof ("20151009/15/video") - 1, "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                    ti.tm_year -= 1900, ti.tm_mon -= 1;
                    pVideoDemand->fileStartTime = mktime(&ti);
                }
#endif
            } else {
                FsPrintf(1, "No next file.\n");
                fflush(stdout);
                return -1;
            }
        }
    }
    if (NULL == pVideoDemand->fd) {
        /* 视频文件对象 */
        pVideoDemand->fd = fopen(pVideoDemand->thisFileName, "rb");
        if (NULL == pVideoDemand->fd) {
            FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", pVideoDemand->thisFileName, strerror(errno));
            fflush(stdout);
            return -1;
        }
        /* 视频对象 */
        pVideoDemand->pMKV = pMKV = video_mkv_new_from_file__IO(pVideoDemand->fd);
        if (NULL == pMKV) {
            FsPrintf(1, "Error new mkv from the file:\"%s\".\n", pVideoDemand->thisFileName);
            fflush(stdout);
            fclose(pVideoDemand->fd);
            pVideoDemand->fd = NULL;
            return -1;
        }
        /* 获取keydata */
        if (pMKV->videoIndex) {
            unsigned int l = pMKV->videoCount + pMKV->audioCount + pMKV->subtitleCount;
#ifdef __VideoDemand_keydata_record
#undef __VideoDemand_keydata_record
#endif
#define __VideoDemand_keydata_record
#ifndef __VideoDemand_keydata
            for (; l > 0; l--) {
                if (VideoMKV_trackVideo == pMKV->channel[l].type) {
                    if (2 == pMKV->channel[l].p.video.encodeType) {
                        if (video_mkv_h264_get_keyData(pMKV->channel[l].p.video.codecPrivate, pMKV->channel[l].p.video.codecPrivate) != 1) {
#ifndef __VideoDemand_keydata_record
                            FsPrintf(5, "Get keydata from the file:\"%s\".\n", fileName);
                            fflush(stdout);
                            fileStream_P_VideoDemand_delete__OI(rst);
                            return NULL;
#else
                            FsPrintf(5, "Get keydata from the file:\"%s\".\n", pVideoDemand->thisFileName);
                            fflush(stdout);
                            return -1;
#endif
                        }
                    } else if (3 == pMKV->channel[l].p.video.encodeType) {
                        if (video_mkv_h265_get_keyData(pMKV->channel[l].p.video.codecPrivate, pMKV->channel[l].p.video.codecPrivate) != 1) {
#ifndef __VideoDemand_keydata_record
                            FsPrintf(5, "Get keydata from the file:\"%s\".\n", fileName);
                            fflush(stdout);
                            fileStream_P_VideoDemand_delete__OI(rst);
                            return NULL;
#else
                            FsPrintf(5, "Get keydata from the file:\"%s\".\n", pVideoDemand->thisFileName);
                            fflush(stdout);
                            return -1;
#endif
                        }
                    } else {
#ifndef __VideoDemand_keydata_record
                        FsPrintf(5, "Unknown encodeType(=%hhu) from the file:\"%s\".\n", pMKV->channel[l].p.video.encodeType, fileName);
                        fflush(stdout);
                        fileStream_P_VideoDemand_delete__OI(rst);
                        return NULL;
#else
                        FsPrintf(5, "Unknown encodeType(=%hhu) from the file:\"%s\".\n", pMKV->channel[l].p.video.encodeType, pVideoDemand->thisFileName);
                        fflush(stdout);
                        return -1;
#endif
                    }
                }
            }
#endif
        }
        if (pMKV->videoIndex) {
            /* 有视频 */
            /* 当前使用的帧率 */
            if (pVideoDemand->fpsSet <= 0.0) pVideoDemand->fps = pMKV->channel[pMKV->videoIndex].p.video.frameRate;
            /* 文件时长 */
            pVideoDemand->fileTime = pMKV->channel[pMKV->videoIndex].list->nodeCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
        } else {
            /* 无视频 */
            /* 当前使用的帧率 */
            if (pVideoDemand->fpsSet <= 0.0) pVideoDemand->fps = pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
            /* 文件时长 */
            pVideoDemand->fileTime = pMKV->channel[pMKV->audioIndex].list->nodeCount / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
        }
    }
    /* 已发送的帧数 */
    if (pMKV->videoIndex) {
        /* 有视频 */
        /* 已发送的帧数 */
        if (gmtTime < pVideoDemand->fileStartTime)pVideoDemand->sendCount = 0;
        else pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].p.video.frameRate * (gmtTime - pVideoDemand->fileStartTime);
        if (pMKV->channel[pMKV->videoIndex].list->nodeCount <= pVideoDemand->sendCount) {
            FsPrintf(1, "Invalid sendCount(=%lu/%lu),gmtTime=%lf/%lf,fileStartTime=%lf,file:\"%s\".\n", pVideoDemand->sendCount,
                    pMKV->channel[pMKV->videoIndex].list->nodeCount, gmtTime, pVideoDemand->fileStartTime, pVideoDemand->fileStartTime, pVideoDemand->thisFileName);
            fflush(stdout);
            /* 读取下一个记录 */
            if (0 == (sendType & 0x1)) {
                /* 认为现在的所有数据都已发完 */
                pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].list->nodeCount;
                /* 向前找关键帧 */
                if (pVideoDemand->sendCount > 0)pVideoDemand->sendCount--;
                for (; pVideoDemand->sendCount > 0 &&
                        0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex +
                        pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount--);
                if (rst != 1) {
                    if (((unsigned long long) pVideoDemand->fileStartTime) == (unsigned long long) remoteVideoStorage_startTime) {
                        /* 是对应远程储存的当前文件,不用读取下一个文件 */
                        return 1;
                    }
                }
                if (1 == record_P_VideoDemand_readNextFile(pVideoDemand, recordVideoMode, 1, thisUptime, pShareBuffer))return 1;
                else {
                    FsPrintf(1, "Read next file failed,thisFileName:\"%s\".\n", pVideoDemand->thisFileName);
                    fflush(stdout);
                    return -1;
                }
            } else {
                pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].list->nodeCount;
                if (pVideoDemand->sendCount > 0)pVideoDemand->sendCount--;
            }
        }
        if (0 == (sendType & 0x1)) {
            /* 向前找关键帧 */
            for (; pVideoDemand->sendCount > 0 &&
                    0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex +
                    pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount--);
        } else {
            if (0) {
                /* 向后找关键帧 */
                for (; pVideoDemand->sendCount < pMKV->channel[pMKV->videoIndex].list->nodeCount &&
                        0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex +
                        pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount++);
                if (pVideoDemand->sendCount == pMKV->channel[pMKV->videoIndex].list->nodeCount) {
                    /* 向前找关键帧 */
                    for (pVideoDemand->sendCount--; pVideoDemand->sendCount > 0 &&
                            0 == ((struct VideoMKV_node*) pMKV->channel[pMKV->videoIndex].list->pNode[pMKV->channel[pMKV->videoIndex].list->startIndex +
                            pVideoDemand->sendCount])->p.video.keyFrame; pVideoDemand->sendCount--);
                }
            }
            pVideoDemand->sendCount = pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount;
            if (pVideoDemand->sendCount > 0)pVideoDemand->sendCount--;
            FsPrintf(1, "gmtTime=%lf/%lf\n", gmtTime, pVideoDemand->fileStartTime +
                    (pMKV->channel[pMKV->videoIndex].list->nodeCount > 0 ? (pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount - 1) : 0.0) / pMKV->channel[pMKV->videoIndex].p.video.frameRate);
            fflush(stdout);
        }
        /* 视频必须发完的时间 */
        pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->videoIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps - 1.0;
    } else {
        /* 无视频 */
        /* 已发送的帧数 */
        if (gmtTime < pVideoDemand->fileStartTime)pVideoDemand->sendCount = 0;
        else pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].p.audio.frameRate * (gmtTime - pVideoDemand->fileStartTime);
        if (pMKV->channel[pMKV->audioIndex].list->nodeCount <= pVideoDemand->sendCount) {
            FsPrintf(1, "Invalid sendCount(=%lu/%lu),gmtTime=%lf/%lf,fileStartTime=%lf,file:\"%s\".\n", pVideoDemand->sendCount,
                    pMKV->channel[pMKV->audioIndex].list->nodeCount, gmtTime, pVideoDemand->fileStartTime, pVideoDemand->fileStartTime, pVideoDemand->thisFileName);
            fflush(stdout);
            /* 读取下一个记录 */
            if (0 == (sendType & 0x1)) {
                /* 认为现在的所有数据都已发完 */
                pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].list->nodeCount;
                if (rst != 1) {
                    if (((unsigned long long) pVideoDemand->fileStartTime) == (unsigned long long) remoteVideoStorage_startTime) {
                        /* 是对应远程储存的当前文件,不用读取下一个文件 */
                        return 1;
                    }
                }
                if (1 == record_P_VideoDemand_readNextFile(pVideoDemand, recordVideoMode, 1, thisUptime, pShareBuffer))return 1;
                else {
                    FsPrintf(1, "Read next file failed,thisFileName:\"%s\".\n", pVideoDemand->thisFileName);
                    fflush(stdout);
                    return -1;
                }
            } else {
                pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].list->nodeCount;
            }
        }
        if (sendType & 0x1) {
            pVideoDemand->sendCount = pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount;

            if (pVideoDemand->sendCount > 0)pVideoDemand->sendCount--;
        }
        /* 视频必须发完的时间 */
        pVideoDemand->endUptime = fs_time_uptime_get() + (pMKV->channel[pMKV->audioIndex].list->nodeCount - pVideoDemand->sendCount) / pVideoDemand->fps - 1.0;
    }
    return 1;
}

/* 创建或更新一个录像回放的对象,成功返回1,失败返回NULL */
static struct Record_P_VideoDemand * record_P_VideoDemand_new__IO(struct Record_item * const pRecord_item
        , /* 数据发送类型(0-正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送) */ const unsigned char sendType, /* 目录名,以'/'结尾 */const char dir_cin[]
        , /* 请求数据的时间 */const double playTime, /* 开始帧号,只有前三个字节有效 */const unsigned int frameIndexStart, /* 小于0.0表示未设置 */const double fps, const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check dir_cin
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    // 20151009/15/video20151009150400.mkv;
    const unsigned int dirLen = strlen(dir_cin);
    unsigned int remoteDirLen;
    unsigned int len;
    struct Record_P_VideoDemand * rst;
    if (Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode)) {
        len = sizeof ( ((struct Record_P_VideoDemand *) 0)->externStorage[0]);
        FsLocal_ShareBuffer_resize(dirLen + sizeof ("remote/v"Fs_date_year_string_max"090910/0cb/1234123_0x1122334455667788_0/"));
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm ti;
        localtime_r(&tv.tv_sec, &ti);

        /* /fs/project/data/vsys/34020000001310100001/video/remote/v2022031609/dpi/2651370_0x7f9c70008600_1/ */
        int pos = sprintf((FsLocal_ShareBuffer + FsLocal_ShareBufferPos), "%sremote/v%d%02d%02d%02d/", dir_cin, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour);
        (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[pos++] = ti.tm_min / 2 <= 9 ? ti.tm_min / 2 + '0' : ti.tm_min / 2 - 10 + 'a';
        remoteDirLen = pos + sizeof ("cb") - 1 + sprintf((FsLocal_ShareBuffer + FsLocal_ShareBufferPos) + pos + sizeof ("cb") - 1, "/%02d%02d%03ld_", ti.tm_min, ti.tm_sec, tv.tv_usec / 1000);
        len += remoteDirLen + sizeof ("0x1122334455667788_1234567890/") - 1 + sizeof (Fs_date_year_string_max"1009/15/video"Fs_date_year_string_max"1009150400.mkv") + 1 + remoteDirLen + sizeof ("0x1122334455667788_01234567890/") - 1 + sizeof ("__tag");
        rst = (struct Record_P_VideoDemand*) fsMalloc(sizeof (struct Record_P_VideoDemand) +len);
        remoteDirLen += sprintf((FsLocal_ShareBuffer + FsLocal_ShareBufferPos) + remoteDirLen, "%p_/", rst);
        {
            int sum = fs_String_buffer_24bitSum(remoteDirLen - 1 - (pos + sizeof ("cb") - 1 + 1), (FsLocal_ShareBuffer + FsLocal_ShareBufferPos) + pos + sizeof ("cb") - 1 + 1);
            sum = sum^(sum >> 12);
            sum >>= (sum & 0x1) ? 2 : 1;
            int v = sum & 0x1F;
            (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[pos++] = v < 10 ? v + '0' : v - 10 + 'a';
            v = (sum >> 5)&0x1F;
            (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[pos++] = v < 10 ? v + '0' : v - 10 + 'a';
        }
        if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0) {
            Record_itemLog(FsLogType_matchError, "remoteDir:\"%s\" has exit,dir:\"%s\",jumpTime=%lf,sendType=%hhu", (FsLocal_ShareBuffer + FsLocal_ShareBufferPos), dir_cin, playTime, sendType);
            fflush(stdout);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + remoteDirLen + 1);
            if (fs_rm_all(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, &FsLocal_ShareBuffer) != 1) {
                FsLocal_ShareBuffer_call_after();
                Record_itemLog(FsLogType_error, "Rm the remoteDir:\"%s\" failed,dir:\"%s\",jumpTime=%lf,sendType=%hhu", (FsLocal_ShareBuffer + FsLocal_ShareBufferPos), dir_cin, playTime, sendType);
                fflush(stdout);
            } else {
                FsLocal_ShareBuffer_call_after();
            }
        }
    } else {
        len = dirLen + sizeof (Fs_date_year_string_max"1009/15/video20151009150400.mkv") + 1;
        rst = (struct Record_P_VideoDemand*) fsMalloc(sizeof (struct Record_P_VideoDemand) +len);
    }
    memset(rst, 0, sizeof (struct Record_P_VideoDemand));
    char *pd;
    /* 帧号,各通道的帧号 */
    /* unsigned int frameIndex[Record_P_VideoDemand_channelCountMax]; */
    /* 父目录长度 */
    if (Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode)) {
        rst->dirLen = remoteDirLen;
    } else rst->dirLen = dirLen;
    /* 当前文件名 */
    if (Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode)) {
        pd = ((char*) (rst + 1)) + sizeof ( ((struct Record_P_VideoDemand *) 0)->externStorage[0]);
        rst->thisFileName = pd, memcpy(pd, (FsLocal_ShareBuffer + FsLocal_ShareBufferPos), remoteDirLen), pd[remoteDirLen] = 0, pd += remoteDirLen + sizeof ("1234567890") - 1 + sizeof (Fs_date_year_string_max"1009/15/video"Fs_date_year_string_max"1009150400.mkv") + 1;
    } else {
        pd = (char*) (rst + 1);
        rst->thisFileName = (char*) (rst + 1), memcpy(rst->thisFileName, dir_cin, dirLen);
    }
    /* 帧率的倍率,在多文件切换时,时间可能与文件名有偏差,需要使用此值校正,让发送时间也真实时间接近 */
    rst->frameRateMulti = 1.0;
    /* 用户指定的帧率,大于0有效 */
    rst->fpsSet = fps;
    /* 当前使用的帧率 */
    if (fps > 0.0)rst->fps = fps;
    if (Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode)) {
        /* 监控文件名,在使用第三方储存时需要监控第三方是否把数据准备好了,为空表示本地储存 */
        rst->tagFile = pd, memcpy(pd, (FsLocal_ShareBuffer + FsLocal_ShareBufferPos), remoteDirLen), pd += remoteDirLen, memcpy(pd, FsStringDataLen0("__tag"));
        /* 外部储存时需要的参数,在tagFile不为0时有效 */
        /* tagFile不为空时检查tag的超时时间 */
        rst->externStorage->tagTimeout = thisUptime + Record_P_VideoDemand_tagTimeout;
        /* 操作pEbml_node时使用的互斥锁 */
        rst->externStorage->pEbml_node_mutex = &pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex;
        /* 标记文件的具体内容,由跳转时间+当前使用文件的开始时间+播放类型组成,在等到后把值置0 */
        ((double*) rst->externStorage->tag)[0] = playTime, ((double*) rst->externStorage->tag)[1] = 0.0, ((unsigned char*) (((double*) rst->externStorage->tag) + 2))[0] = sendType;
        /* 在tagFile不为空时,表示此节点的配置数据对应的节点 */
        pthread_mutex_lock(rst->externStorage->pEbml_node_mutex);
        {
            struct FsEbml_node *playback = fs_Ebml_node_get_first(NULL, pRecord_item->ro.__storageConfigNode, "playback");
            if (NULL == playback) {
                playback =
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pRecord_item->ro.__storageConfigNode, "playback", FsEbmlNodeType_Struct
#ifdef Record_sdkConfig_comment
                        , "comment", "回放任务列表"
#endif
                        );
            }
            struct FsEbml_node * const pEbml_node =
#ifdef Record_sdkConfig_comment
                    fs_Ebml_node_addChild_with_a_property
#else
                    fs_Ebml_node_addChild
#endif
                    (NULL, playback, "task", FsEbmlNodeType_Struct
#ifdef Record_sdkConfig_comment
                    , "comment", "回放任务"
#endif
                    );
            {
                FsString * const pString = fs_String_new__IO(remoteDirLen);
                memcpy(pString->buffer, rst->tagFile, remoteDirLen);
                pString->lenth += 1;
                fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pEbml_node, "dir", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                        , "comment", "存放数据的目录,以/结尾"
#endif
                        ), pString);
            }
            {
                FsString * const pString = fs_String_new__IO(remoteDirLen + sizeof ("__tag") - 1);
                memcpy(pString->buffer, rst->tagFile, remoteDirLen + sizeof ("__tag") - 1);
                pString->lenth += 1;
                fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pEbml_node, "tag", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                        , "comment", "数据就绪后向哪个文件写入就绪信息"
#endif
                        ), pString);
            }
            {
                FsString * const pString = fs_String_new__IO(sizeof (((struct Record_P_VideoDemand*) 0)->externStorage->tag)*2);
                {
                    const unsigned char *const v = rst->externStorage->tag;
                    unsigned char *pd = (unsigned char *) pString->buffer;
#ifndef __Record_P_VideoDemand_set_tag 
                    {
                        unsigned int ui = 0;
                        for (; ui<sizeof (((struct Record_P_VideoDemand*) 0)->externStorage->tag); ui++) {
                            *pd++ = (v[ui] >> 4) + 'a';
                            *pd++ = (v[ui] &0xF) + 'a';
                        }
                    }
#endif
                }
                pString->lenth += 1;
                fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pEbml_node, "tagdata", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                        , "comment", "数据就绪后向tag写的数据内容"
#endif
                        ), pString);
            }
            {
                FsString * const pString = fs_String_new__IO(sizeof ("12345678901234567890.1234567890"));
                if (sendType & 0x1)memcpy(pString->buffer, FsStringDataLen0("-64.0")), pString->lenth = sizeof ("-64.0");
                else memcpy(pString->buffer, FsStringDataLen0("64.0")), pString->lenth = sizeof ("64.0");
                fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pEbml_node, "speed", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                        , "comment", "当前的速度,正数表示正向,负数表示逆向"
#endif
                        ), pString);
            }
            {
                FsString * const pString = fs_String_new__IO(sizeof (Fs_date_year_string_max"-10-01/00:00:00.123456"));
                pString->lenth = fs_time_to_stringGMT(pString->buffer, playTime);
                pString->buffer[pString->lenth++] = 0;
                fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pEbml_node, "time", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                        , "comment", "要播放的时间(0时区)"
#endif
                        ), pString);
            }
            {
                FsString * const pString = fs_String_new__IO(sizeof (Fs_date_year_string_max"-10-01/00:00:00.123456"));
                pString->lenth = fs_time_to_stringGMT(pString->buffer, 0.0);
                pString->buffer[pString->lenth++] = 0;
                fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                        fs_Ebml_node_addChild_with_a_property
#else
                        fs_Ebml_node_addChild
#endif
                        (NULL, pEbml_node, "ftime", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                        , "comment", "当前使用文件的时间(0时区)"
#endif
                        ), pString);
            }
            rst->externStorage->pEbml_node = pEbml_node;
        }
        pthread_mutex_unlock(rst->externStorage->pEbml_node_mutex);
        /* sdk配置是否更新的指针 */
        rst->externStorage->sdkConfigUpdate = pRecord_item->ro._pRecord->p._sdkConfigUpdate;
        /* 在pEbml_node内的内容更新后应设置的sdk编号 */
        switch (pRecord_item->ro._recordVideoMode) {
            case /* 3-大华sdk */3: rst->externStorage->sdkConfigUpdate_index = Record_sdkConfigUpdate_index_dh;
                break;
            case /* 4-GB28181 */4: rst->externStorage->sdkConfigUpdate_index = Record_sdkConfigUpdate_index_gb;
                break;
            default:
            {
                Record_itemLog(FsLogType_fatal, "Invalid recordVideoMode(=%hhd).\n", pRecord_item->ro._recordVideoMode);
                fflush(stdout);
                FsLogExit();
            }
                break;
        }
        Record_sdkConfigUpdate_set_1_1(rst->externStorage->sdkConfigUpdate, rst->externStorage->sdkConfigUpdate_index);
    }
    //    else if (record_P_VideoDemand_update(rst, sendType, playTime) == 1) rst->status = 1;
    //    else {
    //        Record_itemLog(FsLogType_matchError, "Do record_P_VideoDemand_update failed,dir:\"%s\",jumpTime=%lf,sendType=%hhu", dir, playTime, sendType);
    //        fflush(stdout);
    //        record_private_VideoDemand_delete__OI(rst);
    //        return NULL;
    //    }
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);

    return rst;
}

/* node1<node2返回负数;node1=node2返回0;node1>node2返回正数; */
static int record_private_item_add_partVideo_compare(struct Record_private_PartVideo_item *node1, struct Record_private_PartVideo_item * node2) {
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
static void record_P_item_delete__OI(struct Record_item * pRecord_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pRecord_item)return;
    /* 注销命令字 */
    //    FsPrintf(1, "%s-%hhu.%hhu.%hhu.%hhu.\n", pRecord_item->ro._uuid, ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n1, ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n2,
    //            ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n3, ((FsNetworkIPv4*) & pRecord_item->ro._ipv4)->n.n4);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "video_part", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item, pShareBuffer);
    if (0 == pRecord_item->ro._ipv4) configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "record_watch", pRecord_item->ro._uuid, 0, pRecord_item, pShareBuffer);
    if (0 == pRecord_item->ro._ipv4) configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "record_inc", pRecord_item->ro._uuid, 0, pRecord_item, pShareBuffer);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "videolist_get", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item, pShareBuffer);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "videotimeinfo_get", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item, pShareBuffer);
    configManager_cmd_logoff(pRecord_item->ro._pRecord->ro._pConfigManager, "snapshort_get", pRecord_item->ro._uuid, pRecord_item->ro._ipv4, pRecord_item, pShareBuffer);
    if (0 == pRecord_item->ro._ipv4)configManager_cmd_disconnect(pRecord_item->ro._pRecord->ro._pConfigManager, "cameractrl", pRecord_item->ro._uuid, pRecord_item);
    ////////////////////////////////////////////////////////////////////////////
    /* rtsp视频转发+历史点播+集群 */
    if (pRecord_item->ro.__pRtspServer_item) {
        rtspServer_item_delete__OI(pRecord_item->ro.__pRtspServer_item);
        if (0 == pRecord_item->ro._ipv4) {
            /* 注销rtsp实时视频掩码通信 */
            configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_rtsp);
        }
    }
    /* hls视频转发+集群 */
    if (pRecord_item->ro.__pHlsServer_item) {
        hlsServer_item_delete__OI(pRecord_item->ro.__pHlsServer_item);
        if (0 == pRecord_item->ro._ipv4) {
            /* 注销hls实时视频掩码通信 */
            configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_hls);
        }

    }
    /* GB28181视频转发+集群 */
    if (pRecord_item->ro.__pGB28181Server_item) {
        gb28181Server_item_delete__OI(pRecord_item->ro.__pGB28181Server_item);
        if (0 == pRecord_item->ro._ipv4) {
            /* 注销rtsp实时视频掩码通信 */
            configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_gb28181);
        }
    }
    if (0 == pRecord_item->ro._ipv4) {
        /* 注销录像的掩码 */
        configManager_mask_logoff_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, "realStream", pRecord_item->ro._uuid, pRecord_item->ro._maskIndex_record);
        /* 外部储存的节点,为空表示不使用外部储存,比如_recordVideoMode为3时,此值指向pRecord->p.dhsdkConfig__videoInfoDataClientList中的节点,删除时需要同步删除 */
        if (pRecord_item->ro.__storageConfigNode) {
            switch (pRecord_item->ro._recordVideoMode) {
                case /* 3-大华sdk */3:
                {
                    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    if (pRecord_item->ro._pRecord->p.dhsdkConfig__videoInfoDataClientList->childNodeCount > 1) {
                        fs_Ebml_node_delete(pRecord_item->ro._pRecord->p.dhsdkConfig__videoInfoDataClientList, pRecord_item->ro.__storageConfigNode);
                    } else {
                        fs_Ebml_delete__OI(pRecord_item->ro._pRecord->p.dhsdkConfig__videoInfoDataClientList, NULL);
                        pRecord_item->ro._pRecord->p.dhsdkConfig__videoInfoDataClientList = NULL;
                    }
                    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    Record_sdkConfigUpdate_set_01_1(pRecord_item->ro._pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_dh);
                }
                    break;
                case /* 4-GB28181 */4:
                {
                    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    if (pRecord_item->ro._pRecord->p.gbsdkConfig__videoInfoDataClientList->childNodeCount > 2) {
                        fs_Ebml_node_delete(pRecord_item->ro._pRecord->p.gbsdkConfig__videoInfoDataClientList, pRecord_item->ro.__storageConfigNode);
                    } else {
                        fs_Ebml_delete__OI(pRecord_item->ro._pRecord->p.gbsdkConfig__videoInfoDataClientList, NULL);
                        pRecord_item->ro._pRecord->p.gbsdkConfig__videoInfoDataClientList = NULL;
                    }
                    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    Record_sdkConfigUpdate_set_01_1(pRecord_item->ro._pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                }
                    break;
                default:
                {
                    Record_itemLog(FsLogType_fatal, "Invalid recordVideoMode(=%hhu)", pRecord_item->ro._recordVideoMode);
                    fflush(stdout);
                    FsLogExit();
                }
                    break;
            }
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
                fsFree(pVideoBuffer_item);
            } while (--l > 0);
            fclose(fd);
        }
        fs_ObjectList_delete__OI(pRecord_item->ro.__videoCacheList);
        /* 视频合成任务链表,任务项按结束时间排序 */
        fs_ObjectList_delete_allSequence__OI(pRecord_item->ro.__videoPartList);
        /* 事件任务链表,储存struct Record_item_Inc指针 */
        fs_ObjectList_delete_custom__OI(pRecord_item->ro.__incList, (void (* const) (void * const))record_item_inc_delete__OI);
        /* 卡口任务链表,储存struct Record_item_Watch指针 */
        fs_ObjectList_delete_custom__OI(pRecord_item->ro.__watchList, (void (* const) (void * const))record_item_watch_delete__OI);

        /* 缓存的快照数据 */
        if (pRecord_item->p.__snap)fs_Object_delete_pthreadSafety__OI(pRecord_item->p.__snap);
    }
    fsFree(pRecord_item);
    //FsPrintf(1, "TTTTTTTTTTTTTTTTTTTTTTT\n");
}

/* 检查数据是否可发送的函数指针,成功返回1,出错返回-1,0表示还需要等待 */
static signed char record_P_videoPart_condition(void *p, /* 发送的数据 */const FsObjectBase * const sendData) {
    struct stat statbuf;
    if (lstat(((char*) p) + sizeof (unsigned long), &statbuf) == 0) {

        if (*((unsigned long*) p) == (unsigned long) statbuf.st_size)return 1;
        *((unsigned long*) p) = statbuf.st_size;
    }
    return 0;
}

static inline void record_P_videoInfo_create(struct Record * const pRecord, /* 本地ip地址 */const FsArray * const ipList) {
    FsConfig * const pConfig = ((ConfigManager*) pRecord->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    FsEbml * const rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    if (0 == pRecord->p.videoInfoIndex)pRecord->p.videoInfoIndex++;
    *(unsigned long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "sum", FsEbmlNodeType_Integer)->data.buffer = pRecord->p.videoInfoIndex;
    struct FsEbml_node * parent = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "cluster", FsEbmlNodeType_Struct);
    {
        const void *vsys0 = pConfig;
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0, ipList, 1, "vsys");
        if (list) {
            void **ppNode = list->pNode + list->startIndex;
            unsigned int ui = list->nodeCount;
            for (; ui > 0; ui--) {
                const FsString * const hostname = fs_Config_node_string_get_first_String(pConfig, vsys0, *ppNode++, "hostname", NULL);
                fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, parent, "host", FsEbmlNodeType_String), hostname->lenth, hostname->buffer);
            }
            fs_ObjectList_delete__OI(list);
        }
    }
    {
        const void *vsys0 = pConfig;
        FsObjectList * list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0, ipList, 0, "vsys");
        if (list) {
            const void *const vsys = (struct FsEbml_node*) list->pNode[list->startIndex];
            {
                const FsString * const hostname = fs_Config_node_string_get_first_String(pConfig, vsys0, vsys, "hostname", NULL);
                fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, parent, "host", FsEbmlNodeType_String), hostname->lenth, hostname->buffer);
            }
            fs_ObjectList_delete__OI(list);
            const void *vsysChannel0 = vsys0;
            list = fs_Config_node_template__IO(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel");
            if (list) {
                parent = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "videoInfo", FsEbmlNodeType_Struct);
                *(unsigned long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node *) parent, "timezone", FsEbmlNodeType_Integer)->data.buffer = fs_time_timezone_get();
                const unsigned int count = list->nodeCount;
                unsigned int ui = 0;
                for (; ui < count; ui++) {
                    struct FsEbml_node * const pEbml_node = fs_Ebml_node_addChild(rst, parent, "channel", FsEbmlNodeType_Struct);
                    if (pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + ui]) {
                        struct Record_item * const pRecord_item = ((struct Record_item*) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + ui]);
                        fs_Ebml_node_addChild_from_node_child(rst, pEbml_node, pRecord_item->ro._videoInfo_pos);
                        pRecord_item->ro._videoInfo_pos = pEbml_node;
                    } else {
#define Record_P_videoInfo_HeadCount 2
                        fs_Ebml_node_addChild(rst, pEbml_node, "uuid", FsEbmlNodeType_String);
                        *(unsigned long long*) fs_Ebml_node_addChild(rst, pEbml_node, "realVideoSaveTime", FsEbmlNodeType_Integer)->data.buffer = 0;
                    }
                }
                fs_ObjectList_delete__OI(list);
            }
        }
    }
    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);

    if (pRecord->p.videoInfoData__videoInfoDataClientList)fs_Ebml_delete__OI(pRecord->p.videoInfoData__videoInfoDataClientList, NULL);
    pRecord->p.videoInfoData__videoInfoDataClientList = rst;
    //    FsPrintf(1, "ttttttttttt\n");
    //    fs_Ebml_out_debug(rst, stdout);
    //    FsPrintf(1, "ttttttttttt\n");
    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
}

/* 从硬盘文件更新某相机的录像信息有更新时会标记videoInfoChange为1 */
static void record_P_videoInfo_update_from_file(struct Record_item * const pRecord_item
        , /* 调用此函数时是否已判定录像状态肯定发生改变,0-未判定,1-已判定 */const unsigned char videoInfoChange
        , /* 下一个录像的开始时间,已有的录像的时间必须比此值小 */const double newBeginTime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var info_in,rst_1_in,rst_2_in,buffer_in
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    const int len0 = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/99999999/08/video20151204080000.mkv");
    int pos = 0, len = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/__info.txt");
    char * info_in = FsLocal_ShareBuffer + FsLocal_ShareBufferPos + sizeof ("99999999/08/video20151204080000.mkv");
    char * rst_1_in = info_in + sizeof ("99999999/08/video20151204080000.mkv")*2;
    char * rst_2_in = rst_1_in + len0;
    // Fs_Memery_resizeBuffer(pBuffer, pBufferLen, len + sizeof ("<begin>2018-03-23/13:39:48.272</begin>\n<begin>"));
    char * buffer_in = info_in + len;
    FsLocal_ShareBuffer_resize(sizeof ("99999999/08/video20151204080000.mkv")*3 + len0 * 2);
    FsStructList * const pStructList = fs_StructList_new__IO(6, sizeof (double)*3);
    FILE *fd;
    /* 第一个double表示开始时间,第2个double表示结束时间,
     * 第3个double的第一个字节
     *     第8位:0-表示从__info.txt读取的数据,1-表示从__inf0.txt读取的数;
     *     第7位:表示结束时间是否有效(0-无效,1-有效);
     *     第6位:表示是最后一条数据.
     */
    double data[3] = {-1.0};
    time_t ts;
    struct tm ti;
    int timeZone = fs_time_timezone_get()*3600;
    double theTime;
#define Record_P_videoinfo_precision 0.0000005

    //#define __Record_P_videoInfo_update_0_1
#ifndef __Record_P_videoInfo_update_0
#ifndef __Record_P_videoInfo_update_0_1
    sprintf(info_in, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
#else 
    sprintf(info_in, "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
#endif
    if ((fd = fopen(info_in, "rb")) != NULL) {
#define __read_video_info_in_Record
#define __read_video_info_precision Record_P_videoinfo_precision     
#ifndef __read_video_info 
        while ((len = fread(buffer_in + pos, 1, sizeof ("<begin>2018-03-23/13:39:48.272</begin>\n<begin>") - pos - 1, fd)) > 0) {
            pos += len;
            buffer_in[pos] = 0;
            //FsPrintf(1, "info_in:\"%s\",checkUpdate=%hhu,pStructList->nodeCount=%lu,len=%d,%s.\n", info_in, checkUpdate, pStructList->nodeCount, len, buffer_in);
            if ('<' == buffer_in[0]) {
                if (memcmp("begin>", buffer_in + 1, sizeof ("begin>") - 1) == 0) {
                    /* 是开始位置 */
                    const char *ps;
                    if ((ps = strstr(buffer_in + 1 + sizeof ("begin>") - 1, "</begin>")) != NULL) {
                        /* 找到了 */
                        if (data[0] > 0.0) {
                            /* 上一条数据没有找到结束位置 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "No end tag from file:\"%s\",this data:\"%s\"/%lf,pStructList->nodeCount=%lu.\n", info_in, buffer_in, data[0], pStructList->nodeCount);
                            fflush(stdout);
#ifdef __read_video_info_in_Record
                            fs_StructList_insert_order_double(pStructList, data);
#else
                            fs_StructList_clear(pStructList);
                            data[0] = -1.0;
                            break;
#endif
                        }
                        data[0] = fs_time_get_string2(buffer_in + 1 + sizeof ("begin>") - 1, ps - (buffer_in + 1 + sizeof ("begin>") - 1)) - timeZone + __read_video_info_precision;
#ifdef __read_video_info_in_Record
#ifndef __Record_P_videoInfo_update_0_1
                        ((char*) (data + 2))[0] = 0;
#else
                        ((char*) (data + 2))[0] = 1;
#endif
#else
                        /* 校验数据的合法性 */
                        if (data[1] > 0.0 && data[0] <= data[1]) {
                            /* 无效的开始时间 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "Invalid startTime(=%lf/%lf),file:\"%s\",data:\"%s\",pStructList->nodeCount=%lu.\n", data[0], data[1], info_in, buffer_in, pStructList->nodeCount);
                            fflush(stdout);
                            fs_StructList_clear(pStructList);
                            data[0] = -1.0;
                            break;
                        }
#endif
                        len = ps - buffer_in + sizeof ("</begin>") - 1;
                    } else {
                        /* 不存在 */
                        FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                        fflush(stdout);
                        len = 1 + sizeof ("begin>") - 1;
                    }
                } else if (memcmp("end>", buffer_in + 1, sizeof ("end>") - 1) == 0) {
                    /* 是结束位置 */
                    const char *ps;
                    if ((ps = strstr(buffer_in + 1 + sizeof ("end>") - 1, "</end>")) != NULL) {
                        /* 找到了 */
                        if (data[0] < 0.0) {
                            /* 上一条数据不是开始标记,忽略本条数据 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "No start tag from file:\"%s\",this data:\"%s\"/%lf,pStructList->nodeCount=%lu.\n", info_in, buffer_in, data[0], pStructList->nodeCount);
                            fflush(stdout);
                        } else {
                            data[1] = fs_time_get_string2(buffer_in + 1 + sizeof ("end>") - 1, ps - (buffer_in + 1 + sizeof ("end>") - 1)) - timeZone + __read_video_info_precision;
                            if (data[1] > data[0]) {
                                /* 数据有效 */
#ifdef __read_video_info_in_Record
#ifndef __Record_P_videoInfo_update_0_1
                                ((char*) (data + 2))[0] = 2;
#else
                                ((char*) (data + 2))[0] = 3;
#endif
                                if (fs_StructList_insert_order_double(pStructList, data) < 0) {
                                    FsLog(FsLogType_matchError, FsPrintfIndex, "Insert failed,end=%lf/%lf,file:\"%s\",data:\"%s\",pStructList->nodeCount=%lu.\n",
                                            data[1], data[0], info_in, buffer_in, pStructList->nodeCount);
                                    fflush(stdout);
                                }
#else
                                fs_StructList_insert_tail(pStructList, data);
#endif

                            } else {
                                FsLog(FsLogType_matchError, FsPrintfIndex, "Invalid end(=%lf/%lf) from file:\"%s\",this data:\"%s\",pStructList->nodeCount=%lu.\n", data[1], data[0], info_in, buffer_in, pStructList->nodeCount);
                                FsLogTag(10);
                                fflush(stdout);
#ifdef __read_video_info_in_Record
                                /* 差值不到1S的忽略,浪费时间 */
                                if (data[0] - data[1] > 1.0) fs_StructList_insert_order_double(pStructList, data);
#else
                                fs_StructList_clear(pStructList);
                                data[0] = -1.0;
                                break;
#endif                              
                            }
                            data[0] = -1.0;
                        }
                        len = ps - buffer_in + sizeof ("</end>") - 1;
                    } else {
                        /* 不存在 */
                        FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                        fflush(stdout);
                        len = 1 + sizeof ("end>") - 1;
                    }
                } else {
                    /* 无法识别的数据 */
                    FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                    fflush(stdout);
                    len = 1;
                }
            }
            /* 向后查找数据 */
            for (; buffer_in[len] != '<' && len < pos; len++);
            if ((pos -= len) > 0) memmove(buffer_in, buffer_in + len, pos);
        }
#ifdef __read_video_info_in_Record
#undef __read_video_info_in_Record
#endif
#endif
        fclose(fd);
        if (data[0] > 0.0) {
            /* 上一条数据没有找到结束位置 */
            FsPrintf(1, "No end tag from file:\"%s\",start=%lf, pStructList->nodeCount=%lu,uuid:\"%s\".\n",
                    info_in, data[0], pStructList->nodeCount, pRecord_item->ro._uuid);
            fflush(stdout);
            /* 标记为最后一条数据 */
            ((char*) (data + 2))[0] |= 4;
            fs_StructList_insert_order_double(pStructList, data);
            data[0] = -1.0;
        }
    } else if (access(info_in, F_OK) == 0) {
        FsPrintf(1, "Open file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
        fflush(stdout);
    }
#ifdef __Record_P_videoInfo_update_0_1
#undef __Record_P_videoInfo_update_0_1
#endif
#endif

#define __Record_P_videoInfo_update_0_1
#ifndef __Record_P_videoInfo_update_0
#ifndef __Record_P_videoInfo_update_0_1
    sprintf(info_in, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
#else 
    sprintf(info_in, "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
#endif
    if ((fd = fopen(info_in, "rb")) != NULL) {
#define __read_video_info_in_Record
#define __read_video_info_precision Record_P_videoinfo_precision  
#ifndef __read_video_info 
        while ((len = fread(buffer_in + pos, 1, sizeof ("<begin>2018-03-23/13:39:48.272</begin>\n<begin>") - pos - 1, fd)) > 0) {
            pos += len;
            buffer_in[pos] = 0;
            //FsPrintf(1, "info_in:\"%s\",checkUpdate=%hhu,pStructList->nodeCount=%lu,len=%d,%s.\n", info_in, checkUpdate, pStructList->nodeCount, len, buffer_in);
            if ('<' == buffer_in[0]) {
                if (memcmp("begin>", buffer_in + 1, sizeof ("begin>") - 1) == 0) {
                    /* 是开始位置 */
                    const char *ps;
                    if ((ps = strstr(buffer_in + 1 + sizeof ("begin>") - 1, "</begin>")) != NULL) {
                        /* 找到了 */
                        if (data[0] > 0.0) {
                            /* 上一条数据没有找到结束位置 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "No end tag from file:\"%s\",this data:\"%s\"/%lf,pStructList->nodeCount=%lu.\n", info_in, buffer_in, data[0], pStructList->nodeCount);
                            fflush(stdout);
#ifdef __read_video_info_in_Record
                            fs_StructList_insert_order_double(pStructList, data);
#else
                            fs_StructList_clear(pStructList);
                            data[0] = -1.0;
                            break;
#endif
                        }
                        data[0] = fs_time_get_string2(buffer_in + 1 + sizeof ("begin>") - 1, ps - (buffer_in + 1 + sizeof ("begin>") - 1)) - timeZone + __read_video_info_precision;
#ifdef __read_video_info_in_Record
#ifndef __Record_P_videoInfo_update_0_1
                        ((char*) (data + 2))[0] = 0;
#else
                        ((char*) (data + 2))[0] = 1;
#endif
#else
                        /* 校验数据的合法性 */
                        if (data[1] > 0.0 && data[0] <= data[1]) {
                            /* 无效的开始时间 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "Invalid startTime(=%lf/%lf),file:\"%s\",data:\"%s\",pStructList->nodeCount=%lu.\n", data[0], data[1], info_in, buffer_in, pStructList->nodeCount);
                            fflush(stdout);
                            fs_StructList_clear(pStructList);
                            data[0] = -1.0;
                            break;
                        }
#endif
                        len = ps - buffer_in + sizeof ("</begin>") - 1;
                    } else {
                        /* 不存在 */
                        FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                        fflush(stdout);
                        len = 1 + sizeof ("begin>") - 1;
                    }
                } else if (memcmp("end>", buffer_in + 1, sizeof ("end>") - 1) == 0) {
                    /* 是结束位置 */
                    const char *ps;
                    if ((ps = strstr(buffer_in + 1 + sizeof ("end>") - 1, "</end>")) != NULL) {
                        /* 找到了 */
                        if (data[0] < 0.0) {
                            /* 上一条数据不是开始标记,忽略本条数据 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "No start tag from file:\"%s\",this data:\"%s\"/%lf,pStructList->nodeCount=%lu.\n", info_in, buffer_in, data[0], pStructList->nodeCount);
                            fflush(stdout);
                        } else {
                            data[1] = fs_time_get_string2(buffer_in + 1 + sizeof ("end>") - 1, ps - (buffer_in + 1 + sizeof ("end>") - 1)) - timeZone + __read_video_info_precision;
                            if (data[1] > data[0]) {
                                /* 数据有效 */
#ifdef __read_video_info_in_Record
#ifndef __Record_P_videoInfo_update_0_1
                                ((char*) (data + 2))[0] = 2;
#else
                                ((char*) (data + 2))[0] = 3;
#endif
                                if (fs_StructList_insert_order_double(pStructList, data) < 0) {
                                    FsLog(FsLogType_matchError, FsPrintfIndex, "Insert failed,end=%lf/%lf,file:\"%s\",data:\"%s\",pStructList->nodeCount=%lu.\n",
                                            data[1], data[0], info_in, buffer_in, pStructList->nodeCount);
                                    fflush(stdout);
                                }
#else
                                fs_StructList_insert_tail(pStructList, data);
#endif

                            } else {
                                FsLog(FsLogType_matchError, FsPrintfIndex, "Invalid end(=%lf/%lf) from file:\"%s\",this data:\"%s\",pStructList->nodeCount=%lu.\n", data[1], data[0], info_in, buffer_in, pStructList->nodeCount);
                                FsLogTag(10);
                                fflush(stdout);
#ifdef __read_video_info_in_Record
                                /* 差值不到1S的忽略,浪费时间 */
                                if (data[0] - data[1] > 1.0) fs_StructList_insert_order_double(pStructList, data);
#else
                                fs_StructList_clear(pStructList);
                                data[0] = -1.0;
                                break;
#endif                              
                            }
                            data[0] = -1.0;
                        }
                        len = ps - buffer_in + sizeof ("</end>") - 1;
                    } else {
                        /* 不存在 */
                        FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                        fflush(stdout);
                        len = 1 + sizeof ("end>") - 1;
                    }
                } else {
                    /* 无法识别的数据 */
                    FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                    fflush(stdout);
                    len = 1;
                }
            }
            /* 向后查找数据 */
            for (; buffer_in[len] != '<' && len < pos; len++);
            if ((pos -= len) > 0) memmove(buffer_in, buffer_in + len, pos);
        }
#ifdef __read_video_info_in_Record
#undef __read_video_info_in_Record
#endif
#endif
        fclose(fd);
        if (data[0] > 0.0) {
            /* 上一条数据没有找到结束位置 */
            FsPrintf(1, "No end tag from file:\"%s\",start=%lf, pStructList->nodeCount=%lu,uuid:\"%s\".\n",
                    info_in, data[0], pStructList->nodeCount, pRecord_item->ro._uuid);
            fflush(stdout);
            /* 标记为最后一条数据 */
            ((char*) (data + 2))[0] |= 4;
            fs_StructList_insert_order_double(pStructList, data);
            data[0] = -1.0;
        }
    } else if (access(info_in, F_OK) == 0) {
        FsPrintf(1, "Open file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
        fflush(stdout);
    }
#ifdef __Record_P_videoInfo_update_0_1
#undef __Record_P_videoInfo_update_0_1
#endif
#endif
    //FsPrintf(1, "tttttttttttt,%d,pStructList->nodeCount=%lu\n", access("/fs/project/data/vsys/163/video/__info.txt", F_OK), pStructList->nodeCount);
    /* 过滤pStructList中的无效数据 */
#ifndef __Record_P_videoInfo_update_check_invalidData 
    /* 读取最旧的视频时间,过滤pStructList中的老数据 */
    sprintf(info_in, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
    fs_dir_match_prefect(rst_1_in, rst_2_in, info_in, "00000000/08/video20151204080000.mkv", info_in, 1);
    if (0 == rst_2_in[0]) {
        FsLog2(FsLogType_info, FsPrintfIndex, "Can not find data for the uuid:\"%s\",recordVideoMode=%hhu.\n", pRecord_item->ro._uuid, pRecord_item->ro._recordVideoMode);
        /* 删除硬盘记录文件 */
        len = sprintf(info_in, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        // FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
        if (access(info_in, F_OK) == 0 && unlink(info_in)) {
            FsLog(FsLogType_error, FsPrintfIndex, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        }
        info_in[len - 5] = '0';
        //FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
        if (access(info_in, F_OK) == 0 && unlink(info_in)) {
            FsLog(FsLogType_error, FsPrintfIndex, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        }
        pRecord_item->p._videoInfo_file = 0;
        pRecord_item->p.videoTimeEnd[0] = 0.0;
    } else {
        memset(&ti, 0, sizeof (struct tm));
        /* 当前文件的开始时间 */
        sscanf(rst_2_in + len0 - sizeof ("20151204080000.mkv"), "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
        ti.tm_year -= 1900;
        ti.tm_mon -= 1;
        theTime = mktime(&ti);
        /* 遍历删除pStructList中无效数据 */
        while (pStructList->nodeCount > 1 && *(double*) (pStructList->pNode + pStructList->structSize * (pStructList->startIndex + 1)) < theTime) {
            //            FsPrintf(1, "Invalid time(=%lf/%lf),uuid:\"%s\".\n",
            //                    *(double*) (pStructList->pNode + pStructList->structSize * (pStructList->startIndex + 1)), theTime, pRecord_item->ro._uuid);
            //            fflush(stdout);
            fs_StructList_remove_head(pStructList);
        }
    }
    /* 读取最新的视频时间,过滤pStructList中的新数据(可能是因校时导致的时间不对) */
    sprintf(info_in, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
    fs_dir_match_prefect(rst_1_in, rst_2_in, info_in, "99999999/08/video20151204080000.mkv", info_in, 0);
    if (0 == rst_1_in[0]) {
        FsLog2(FsLogType_info, FsPrintfIndex, "Can not find data for the uuid:\"%s\",recordVideoMode=%hhu.\n", pRecord_item->ro._uuid, pRecord_item->ro._recordVideoMode);
        /* 删除硬盘记录文件 */
        len = sprintf(info_in, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        // FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
        if (access(info_in, F_OK) == 0 && unlink(info_in)) {
            FsLog(FsLogType_error, FsPrintfIndex, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        }
        info_in[len - 5] = '0';
        //FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
        if (access(info_in, F_OK) == 0 && unlink(info_in)) {
            FsLog(FsLogType_error, FsPrintfIndex, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        }
        pRecord_item->p._videoInfo_file = 0;
        pRecord_item->p.videoTimeEnd[0] = 0.0;
    } else {
        memset(&ti, 0, sizeof (struct tm));
        /* 当前文件的开始时间 */
        sscanf(rst_1_in + len0 - sizeof ("20151204080000.mkv"), "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
        ti.tm_year -= 1900;
        ti.tm_mon -= 1;
        theTime = mktime(&ti);
        //        FsPrintf(1, "Open the file:\"%s\" for the uuid:\"%s\",theTime=%lf.\n", rst_1, pRecord_item->ro._uuid, theTime);
        //        fflush(stdout);
        fd = fopen(rst_1_in, "rb");
        if (NULL == fd) {
            FsPrintf(1, "Open the file:\"%s\" error:\"%s\" for the uuid:\"%s\".\n", rst_1_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        } else {
            VideoMKV *pMKV = video_mkv_new_from_file__IO(fd);
            if (NULL == pMKV) {
                FsPrintf(1, "Read mkv from the file:\"%s\" error:\"%s\" for the uuid:\"%s\".\n", rst_1_in, strerror(errno), pRecord_item->ro._uuid);
                fflush(stdout);
            } else {
                if (pMKV->videoIndex) theTime += pMKV->channel[pMKV->videoIndex].list->nodeCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
                else if (pMKV->audioIndex) theTime += pMKV->channel[pMKV->audioIndex].list->nodeCount / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
                video_mkv_delete__OI(pMKV);
            }
            fclose(fd);
        }
        /* 遍历删除pStructList中无效数据 */
        while (pStructList->nodeCount > 0 && *(double*) (pStructList->pNode + pStructList->structSize * (pStructList->startIndex + pStructList->nodeCount - 1)) > theTime) {
            FsPrintf(1, "Invalid time(=%lf/%lf),uuid:\"%s\".\n",
                    *(double*) (pStructList->pNode + pStructList->structSize * (pStructList->startIndex + pStructList->nodeCount - 1)), theTime, pRecord_item->ro._uuid);
            fflush(stdout);
            fs_StructList_remove_tail(pStructList);
        }
    }
#endif
    //FsPrintf(1, "tttttttttttt,%d,pStructList->nodeCount=%lu\n", access("/fs/project/data/vsys/163/video/__info.txt", F_OK), pStructList->nodeCount);
    /* 遍历pStructList,补全没有结束时间的结束数据,并判断两文件中是否有可删除的文件 */
#ifndef __Record_P_videoInfo_update_check_end
    struct FsEbml_node *pEbml_node;
    double (*pData)[3] = ((double(*)[3])pStructList->pNode) + pStructList->startIndex;
    pRecord_item->p.videoTimeEnd[0] = pRecord_item->p.videoTimeEnd[1] = 0.0;
    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount)
        fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, pRecord_item->ro._videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
    //FsPrintf(1, "pStructList->nodeCount=%lu,uuid:\"%s\"\n", pStructList->nodeCount, pRecord_item->ro._uuid);
    for (pos = 0, len = pStructList->nodeCount; len > 0; len--, pData++) {
        //        if (0) {
        //            char _tttt[32];
        //            _tttt[fs_time_to_string(_tttt, pData[0][0])] = 0;
        //            FsPrintf(1, "%d/%lu,%lf-%s,%hhx\n", len, pStructList->nodeCount, pData[0][0], _tttt, ((char*) (pData[0] + 2))[0]);
        //        }
        if ((((unsigned char*) (pData[0] + 2))[0]&0x2) == 0 || (len > 1 && pData[1][0] < pData[0][1])) {
            /* 没有结束时间,或结束时间比下一个开始时间还要大,重新计算结束时间 */
            sprintf(info_in, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
            if (len > 1) {
                ts = ((time_t) pData[1][0]) - 1;
                // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                localtime_r(&ts, &ti);
                sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                        1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                fs_dir_match_prefect(rst_1_in, rst_2_in, info_in, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, info_in, 0);
            } else fs_dir_match_prefect(rst_1_in, rst_2_in, info_in, "99999999/08/video20151204080000.mkv", info_in, 0);
            if (0 == rst_1_in[0]) {
                FsPrintf(1, "Can not find data for the uuid:\"%s\",len=%d/%lu.\n", pRecord_item->ro._uuid, len, pStructList->nodeCount);
                fflush(stdout);
                /* 删除本条记录 */
                len = pStructList->nodeCount - len;
                fs_StructList_remove_at(pStructList, len);
                pData = ((double(*)[3])pStructList->pNode) + (pStructList->startIndex + len - 1);
                len = pStructList->nodeCount - len + 1;
            } else {
                memset(&ti, 0, sizeof (struct tm));
                /* 当前文件的开始时间 */
                sscanf(rst_1_in + len0 - sizeof ("20151204080000.mkv"), "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                ti.tm_year -= 1900;
                ti.tm_mon -= 1;
                pData[0][1] = mktime(&ti);
                fd = fopen(rst_1_in, "rb");
                if (NULL == fd) {
                    FsPrintf(1, "Open the file:\"%s\" error:\"%s\" for the uuid:\"%s\".\n", rst_1_in, strerror(errno), pRecord_item->ro._uuid);
                    fflush(stdout);
                } else {
                    VideoMKV *pMKV = video_mkv_new_from_file__IO(fd);
                    if (NULL == pMKV) {
                        FsPrintf(1, "Read mkv from the file:\"%s\" error:\"%s\" for the uuid:\"%s\".\n", rst_1_in, strerror(errno), pRecord_item->ro._uuid);
                        fflush(stdout);
                    } else {
                        if (pMKV->videoIndex) pData[0][1] += pMKV->channel[pMKV->videoIndex].list->nodeCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
                        else if (pMKV->audioIndex) pData[0][1] += pMKV->channel[pMKV->audioIndex].list->nodeCount / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
                        video_mkv_delete__OI(pMKV);
                    }
                    fclose(fd);
                }
                if (pData[0][1] < pData[0][0]) {
                    /* 无效数据 */
                    FsPrintf(1, "Invalid time(=%lf/%lf) for the uuid:\"%s\".\n", pData[0][0], pData[0][1], pRecord_item->ro._uuid);
                    fflush(stdout);
                    /* 删除本条记录 */
                    len = pStructList->nodeCount - len;
                    fs_StructList_remove_at(pStructList, len);
                    pData = ((double(*)[3])pStructList->pNode) + (pStructList->startIndex + len - 1);
                    len = pStructList->nodeCount - len + 1;
                } else if (len > 1 && (pData[1][0] < pData[0][1] || pData[1][0] - 1 <= pData[0][0] || (pData[0][1] = pData[1][0] - 1) == pData[0][0])) {
                    FsPrintf(1, "Invalid time(=%lf/%lf/%lf) for the uuid:\"%s\".\n", pData[0][0], pData[0][1], pData[1][0], pRecord_item->ro._uuid);
                    fflush(stdout);
                    if (pData[1][0] < pData[0][1] && (pData[0][1] = mktime(&ti) - 1) - 1 > pData[0][0]) {
                        /* 把结束时间定为查找文件时间-1 */
#ifndef __record_P_videoInfo_update_from_file_check_write_end
                        pos |= 1 << (((unsigned char*) (pData[0] + 2))[0]&0x1);
                        //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);
                        pRecord_item->p.videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1] = pData[0][1];
                        //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                        pEbml_node = fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct);
                        *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "begin", FsEbmlNodeType_Time)->data.buffer = pData[0][0];
                        *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "end", FsEbmlNodeType_Time)->data.buffer =
                                ((unsigned long long) (pData[0][1] * 1000)) / 1000.0 + Record_P_videoinfo_precision;
                        /* 判断当前结束标记是否可插入到某文件尾部 */
                        if (((unsigned char*) (pData[0] + 2))[0]& 0x4) {
                            /* 结束时间可回写入磁盘文件 */
                            ts = (time_t) pData[0][1];
                            localtime_r(&ts, &ti);
                            ts = sprintf(buffer_in, "<end>%04d-%02d-%02d/%02d:%02d:%02d.%03d</end>\n", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec,
                                    (int) ((pData[0][1] - ts) * 1000));
                            sprintf(info_in, 0 == (((unsigned char*) (pData[0] + 2))[0]&0x1) ? "%s%s/video/__info.txt" : "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                            fd = fopen(info_in, "ab+");
                            if (fd) {
                                if (fwrite(buffer_in, ts, 1, fd) != 1) {
                                    FsPrintf(1, "Write(len=%ld) file:\"%s\" error:\"%s\",uuid:\"%s\".\n", ts, info_in, strerror(errno), pRecord_item->ro._uuid);
                                    fflush(stdout);
                                }
                                fclose(fd);
                            } else {
                                FsPrintf(1, "Open file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
                                fflush(stdout);
                            }
                        }
#endif
                    } else {
                        /* 删除本条记录 */
                        len = pStructList->nodeCount - len;
                        fs_StructList_remove_at(pStructList, len);
                        pData = ((double(*)[3])pStructList->pNode) + (pStructList->startIndex + len - 1);
                        len = pStructList->nodeCount - len + 1;
                    }
                } else if (pData[0][1] < newBeginTime) {
#ifndef __record_P_videoInfo_update_from_file_check_write_end
                    pos |= 1 << (((unsigned char*) (pData[0] + 2))[0]&0x1);
                    //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                    pRecord_item->p.videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1] = pData[0][1];
                    //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                    pEbml_node = fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct);
                    *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "begin", FsEbmlNodeType_Time)->data.buffer = pData[0][0];
                    *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "end", FsEbmlNodeType_Time)->data.buffer =
                            ((unsigned long long) (pData[0][1] * 1000)) / 1000.0 + Record_P_videoinfo_precision;
                    /* 判断当前结束标记是否可插入到某文件尾部 */
                    if (((unsigned char*) (pData[0] + 2))[0]& 0x4) {
                        /* 结束时间可回写入磁盘文件 */
                        ts = (time_t) pData[0][1];
                        localtime_r(&ts, &ti);
                        ts = sprintf(buffer_in, "<end>%04d-%02d-%02d/%02d:%02d:%02d.%03d</end>\n", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec,
                                (int) ((pData[0][1] - ts) * 1000));
                        sprintf(info_in, 0 == (((unsigned char*) (pData[0] + 2))[0]&0x1) ? "%s%s/video/__info.txt" : "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                        fd = fopen(info_in, "ab+");
                        if (fd) {
                            if (fwrite(buffer_in, ts, 1, fd) != 1) {
                                FsPrintf(1, "Write(len=%ld) file:\"%s\" error:\"%s\",uuid:\"%s\".\n", ts, info_in, strerror(errno), pRecord_item->ro._uuid);
                                fflush(stdout);
                            }
                            fclose(fd);
                        } else {
                            FsPrintf(1, "Open file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
                            fflush(stdout);
                        }
                    }
#endif
                } else if (pData[0][0] < newBeginTime) {
                    /* 开始时间不大,结束时时过大,结束时间取整 */
                    FsPrintf(1, "Invalid time(=%lf/%lf/%lf) for the uuid:\"%s\".\n", pData[0][0], pData[0][1], newBeginTime, pRecord_item->ro._uuid);
                    fflush(stdout);
                    pData[0][1] = (time_t) newBeginTime;
                    if (pData[0][0] < pData[0][1]) {
#ifndef __record_P_videoInfo_update_from_file_check_write_end
                        pos |= 1 << (((unsigned char*) (pData[0] + 2))[0]&0x1);
                        //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                        pRecord_item->p.videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1] = pData[0][1];
                        //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                        pEbml_node = fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct);
                        *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "begin", FsEbmlNodeType_Time)->data.buffer = pData[0][0];
                        *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "end", FsEbmlNodeType_Time)->data.buffer =
                                ((unsigned long long) (pData[0][1] * 1000)) / 1000.0 + Record_P_videoinfo_precision;
                        /* 判断当前结束标记是否可插入到某文件尾部 */
                        if (((unsigned char*) (pData[0] + 2))[0]& 0x4) {
                            /* 结束时间可回写入磁盘文件 */
                            ts = (time_t) pData[0][1];
                            localtime_r(&ts, &ti);
                            ts = sprintf(buffer_in, "<end>%04d-%02d-%02d/%02d:%02d:%02d.%03d</end>\n", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec,
                                    (int) ((pData[0][1] - ts) * 1000));
                            sprintf(info_in, 0 == (((unsigned char*) (pData[0] + 2))[0]&0x1) ? "%s%s/video/__info.txt" : "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                            fd = fopen(info_in, "ab+");
                            if (fd) {
                                if (fwrite(buffer_in, ts, 1, fd) != 1) {
                                    FsPrintf(1, "Write(len=%ld) file:\"%s\" error:\"%s\",uuid:\"%s\".\n", ts, info_in, strerror(errno), pRecord_item->ro._uuid);
                                    fflush(stdout);
                                }
                                fclose(fd);
                            } else {
                                FsPrintf(1, "Open file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
                                fflush(stdout);
                            }
                        }
#endif
                        /* 删除本条之后的所有记录 */
                        fs_StructList_remove_tail_n(pStructList, pStructList->nodeCount - len);
                    } else {
                        /* 删除本条及之后的所有记录 */
                        fs_StructList_remove_tail_n(pStructList, pStructList->nodeCount - len + 1);
                    }
                    break;
                } else {
                    FsPrintf(1, "Invalid time(=%lf/%lf/%lf) for the uuid:\"%s\".\n", pData[0][0], pData[0][1], newBeginTime, pRecord_item->ro._uuid);
                    fflush(stdout);
                    /* 删除本条及之后的所有记录 */
                    fs_StructList_remove_tail_n(pStructList, pStructList->nodeCount - len + 1);
                    break;
                }
            }
        } else if (pData[0][1] < newBeginTime) {
            pos |= 1 << (((unsigned char*) (pData[0] + 2))[0]&0x1);
            //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

            pRecord_item->p.videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1] = pData[0][1];
            //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

            pEbml_node = fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct);
            *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "begin", FsEbmlNodeType_Time)->data.buffer = pData[0][0];
            *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "end", FsEbmlNodeType_Time)->data.buffer = pData[0][1];
        } else if (pData[0][0] < newBeginTime) {
            /* 开始时间不大,结束时时过大,结束时间取整 */
            FsPrintf(1, "Invalid time(=%lf/%lf/%lf) for the uuid:\"%s\".\n", pData[0][0], pData[0][1], newBeginTime, pRecord_item->ro._uuid);
            fflush(stdout);
            pData[0][1] = (time_t) newBeginTime;
            if (pData[0][0] < pData[0][1]) {
                pos |= 1 << (((unsigned char*) (pData[0] + 2))[0]&0x1);
                //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                pRecord_item->p.videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1] = pData[0][1];
                //FsPrintf(1, "uuid:\"%s\",index=%hhu,%lf,%lf,%lf.\n", pRecord_item->ro._uuid, pData[0][1], pData[0][1], ((unsigned char*) (pData[0] + 2))[0]&0x1, pRecord_item->ro._videoTimeEnd[((unsigned char*) (pData[0] + 2))[0]&0x1]);

                pEbml_node = fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct);
                *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "begin", FsEbmlNodeType_Time)->data.buffer = pData[0][0];
                *(double *) fs_Ebml_node_addChild(NULL, pEbml_node, "end", FsEbmlNodeType_Time)->data.buffer = pData[0][1];
                /* 删除本条之后的所有记录 */
                fs_StructList_remove_tail_n(pStructList, pStructList->nodeCount - len);
            } else {
                /* 删除本条及之后的所有记录 */
                fs_StructList_remove_tail_n(pStructList, pStructList->nodeCount - len + 1);
            }
            break;
        } else {
            FsPrintf(1, "Invalid time(=%lf/%lf/%lf) for the uuid:\"%s\".\n", pData[0][0], pData[0][1], newBeginTime, pRecord_item->ro._uuid);
            fflush(stdout);
            /* 删除本条及之后的所有记录 */
            fs_StructList_remove_tail_n(pStructList, pStructList->nodeCount - len + 1);
            break;
        }
    }
    //FsPrintf(1, "pStructList->nodeCount=%lu,uuid:\"%s\"\n", pStructList->nodeCount, pRecord_item->ro._uuid);
    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    //FsPrintf(1, "tttttttttttt,%d,pStructList->nodeCount=%lu\n", access("/fs/project/data/vsys/163/video/__info.txt", F_OK), pStructList->nodeCount);
    if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
        pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
        pRecord_item->p._videoInfo_file = ((char*) (((double*) (pStructList->pNode + (pStructList->startIndex + pStructList->nodeCount - 1) * pStructList->structSize)) + 2))[0] & 0x1;
    } else if (1 == videoInfoChange)pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
    fs_StructList_delete__OI(pStructList);
    /* 判断两文件是否有可删除的文件 */
    if ((pos & 0x2) == 0) {
        /* 删除硬盘记录文件 */
        sprintf(info_in, "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        //FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
        if (access(info_in, F_OK) == 0 && unlink(info_in)) {
            FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        }
        pRecord_item->p._videoInfo_file = 1;
    }
    //FsPrintf(1, "tttttttttttt,%d\n", access("/fs/project/data/vsys/163/video/__info.txt", F_OK));
    if ((pos & 0x1) == 0) {
        /* 删除硬盘记录文件 */
        sprintf(info_in, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        //FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
        if (access(info_in, F_OK) == 0 && unlink(info_in)) {

            FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", info_in, strerror(errno), pRecord_item->ro._uuid);
            fflush(stdout);
        }
        pRecord_item->p._videoInfo_file = 0;
    }
    //FsPrintf(1, "tttttttttttt,%d\n", access("/fs/project/data/vsys/163/video/__info.txt", F_OK));
#endif      
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
}

/* 更新某相机的录像信息有更新时会标记videoInfoChange为1 */
static void record_P_videoInfo_update(struct Record_item * const pRecord_item
        , /* 0-表示确认并完善原来的数据是否正常结束(videoStartTime大于0时会删除最后一条记录,否则更新),1-表示记录开始(videoStartTime大于等于0时更新内存,否则更新硬盘),2-表示检查并更新记录的开始时间 */const char type, /* 时间,只对type为1有效,gmt时间 */double theTime
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
    if (0) {
        int len = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/00000000/08/video20151204080000.mkv");
        FsLocal_ShareBuffer_resize(sizeof ("00000000/08/video20151204080000.mkv")*2 + len * 2);
        len = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0) {
            sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos + len + 1, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
            fs_file_copy(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, FsLocal_ShareBuffer + FsLocal_ShareBufferPos + len + 1);
        }
        len = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0) {
            sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos + len + 1, "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
            fs_file_copy(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, FsLocal_ShareBuffer + FsLocal_ShareBufferPos + len + 1);
        }
    }
    if (2 == type) {
        /* 检查并更新记录的开始时间 */
        /* 获取最老的文件时间 */
        int len = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/00000000/08/video20151204080000.mkv");
        FsLocal_ShareBuffer_resize(sizeof ("00000000/08/video20151204080000.mkv")*2 + len * 2);
        sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        char *rst_1 = FsLocal_ShareBuffer + FsLocal_ShareBufferPos + sizeof ("00000000/08/video20151204080000.mkv")*2;
        char *rst_2 = rst_1 + len;
        //pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
        fs_dir_match_prefect(rst_1, rst_2, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "00000000/08/video20151204080000.mkv", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 1);
        if (0 == rst_2[0]) {
            //            if (pRecord_item->ro._recordVideoMode != 0) {
            //                FsPrintf(1, "Can not find data for the uuid:\"%s\",recordVideoMode=%hhu.\n", pRecord_item->ro._uuid, pRecord_item->ro._recordVideoMode);
            //                fflush(stdout);
            //            }
            if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
                /* 删除硬盘记录文件 */
                len = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && unlink(FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                    FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                    fflush(stdout);
                }
                (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[len - 5] = '0';
                FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && unlink(FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                    FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                    fflush(stdout);
                }
                pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, pRecord_item->ro._videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
                pRecord_item->p._videoInfo_file = 0;
                pRecord_item->p.videoTimeEnd[0] = 0;
                pRecord_item->rw.videoRecordCheckInterval0 = 0;
            } else {
                if (pRecord_item->rw.videoRecordCheckInterval0 < FsNumberMax(pRecord_item->rw.videoRecordCheckInterval0))pRecord_item->rw.videoRecordCheckInterval0++;
                pRecord_item->rw._videoRecordCheckInterval = pRecord_item->rw.videoRecordCheckInterval0;
            }
        } else {
            struct tm ti;
            memset(&ti, 0, sizeof (struct tm));
            /* 当前文件的开始时间 */
            sscanf(rst_2 + len - sizeof ("20151204080000.mkv"), "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
            ti.tm_year -= 1900;
            ti.tm_mon -= 1;
            theTime = mktime(&ti);
            if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
                //FsPrintf(1, "rst_2:\"%s\",rst_2+len(=%d):\"%s\",uuid:\"%s\",%lf,%lf.\n", rst_2, len, rst_2 + len, pRecord_item->ro._uuid, pRecord_item->ro._videoTimeEnd[(pRecord_item->p._videoInfo_file + 1)&0x1], theTime);
                if (*(double*) pRecord_item->ro._videoInfo_pos->child[Record_P_videoInfo_HeadCount]->child[0]->data.buffer < theTime) {
                    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    do {
                        if (pRecord_item->ro._videoInfo_pos->child[Record_P_videoInfo_HeadCount]->childNodeCount < 2 ||
                                *(double*) pRecord_item->ro._videoInfo_pos->child[Record_P_videoInfo_HeadCount]->child[1]->data.buffer > theTime) {
                            *(double*) pRecord_item->ro._videoInfo_pos->child[Record_P_videoInfo_HeadCount]->child[0]->data.buffer = theTime;
                            break;
                        } else {
                            fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, 1);
                        }
                    } while (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount);
                    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
                    pRecord_item->rw.videoRecordCheckInterval0 = 0;
                    /* 判断两个文件是否可以做切换 */
                    if (pRecord_item->p.videoTimeEnd[(pRecord_item->p._videoInfo_file + 1)&0x1] < theTime) {
                        /* 可以切换,把备用文件删除,待写结束标记后再切换 */
                        sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0 == pRecord_item->p._videoInfo_file ? "%s%s/video/__inf0.txt" : "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                        FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\",%lf,%lf.\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid
                                , pRecord_item->p.videoTimeEnd[(pRecord_item->p._videoInfo_file + 1)&0x1], theTime);
                        if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && unlink(FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                            FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                            fflush(stdout);
                        }
                        /* 修改值,以免程序再次进入 */
                        pRecord_item->p.videoTimeEnd[(pRecord_item->p._videoInfo_file + 1)&0x1] = theTime + 365 * 24 * 3600;
                    }
                } else {
                    if (pRecord_item->rw.videoRecordCheckInterval0 < FsNumberMax(pRecord_item->rw.videoRecordCheckInterval0))pRecord_item->rw.videoRecordCheckInterval0++;
                    pRecord_item->rw._videoRecordCheckInterval = pRecord_item->rw.videoRecordCheckInterval0;
                }
                /* 更新两个文件的修改时间,以免被磁盘管理软件删除及切换时老文件被磁盘管理软件删除 */
                len = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                //FsPrintf(1, "Update the file:\"%s\",uuid:\"%s\".\n", theBuffer, pRecord_item->ro._uuid);
                if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && fs_file_time_update(FsLocal_ShareBuffer + FsLocal_ShareBufferPos) != 1) {
                    FsPrintf(1, "Update the file:\"%s\" time failed,uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                    fflush(stdout);
                }
                (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[len - 5] = '0';
                //FsPrintf(1, "Update the file:\"%s\",uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && fs_file_time_update(FsLocal_ShareBuffer + FsLocal_ShareBufferPos) != 1) {
                    FsPrintf(1, "Update the file:\"%s\" time failed,uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                    fflush(stdout);
                }

            } else {
                /* 出错 */
                //                FsPrintf(1, "videoInfo is NULL,uuid:\"%s\".\n", pRecord_item->ro._uuid);
                //                fflush(stdout);
            }
        }
        //pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
    } else if (1 == type) {
        /* 记录开始 */
        //pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
        if (pRecord_item->p.videoStartTime < 0.0) {
            /* 写硬盘 */
            int msec = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/__info.txt");
            FsLocal_ShareBuffer_resize(msec + sizeof ("<begin>2018-03-23/13:39:48.272</begin>\n"));
            char *const buffer = FsLocal_ShareBuffer + FsLocal_ShareBufferPos + msec;
            time_t ts = (time_t) theTime;
            msec = (theTime - ts) * 1000;
            struct tm ti;
            localtime_r(&ts, &ti);
            msec = sprintf(buffer, "<begin>%04d-%02d-%02d/%02d:%02d:%02d.%03d</begin>\n", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec, msec);
            sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0 == pRecord_item->p._videoInfo_file ? "%s%s/video/__info.txt" : "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);

            fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
            FILE * const fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "ab+");
            if (fd) {
                if (fwrite(buffer, msec, 1, fd) != 1) {
                    FsPrintf(1, "Write(len=%d) file:\"%s\" error:\"%s\",type=%hhu,uuid:\"%s\".\n", msec, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), type, pRecord_item->ro._uuid);
                    fflush(stdout);
                }
                fclose(fd);
            } else {
                FsPrintf(1, "Open file:\"%s\" error:\"%s\",type=%hhu,uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), type, pRecord_item->ro._uuid);
                fflush(stdout);
            }
        } else {
            /* 写内存 */
            if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
                /* 倒序确定时间是逐渐增大的,如不是删除非逐渐增大的时间 */
                /* 结束时间 */
                if (theTime < *(double*) pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1]->child[1]->data.buffer) {
                    /* 时间重叠 */
                    FsPrintf(1, "endtime(%lf/%lf) error,type=%hhu,uuid:\"%s\".\n", theTime,
                            *(double*) pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1]->child[1]->data.buffer,
                            type, pRecord_item->ro._uuid);
                    FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
                    fs_Ebml_out_debug(pRecord_item->ro._pRecord->p.videoInfoData__videoInfoDataClientList, stdout, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    fflush(stdout);
                    /* 重新从文件中加载 */
                    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, pRecord_item->ro._videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
                    record_P_videoInfo_update_from_file(pRecord_item, 1, theTime, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                }
            }
            pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
            *(double*) fs_Ebml_node_addChild(NULL, fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct),
                    "begin", FsEbmlNodeType_Time)->data.buffer = ((unsigned long long) (theTime * 1000)) / 1000.0 + Record_P_videoinfo_precision;
            pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
            //pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
            pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
        }
    } else if (0 == type) {
        /* 确认并完善原来的数据是否正常结束 */
        if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
            if (pRecord_item->p.videoStartTime < 0.0) {
                /* 写过硬盘了 */
                /* 数据已扫描过了,只需确定结束时间即可 */
                /* 需要查找结束位置 */
                int len = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/99999999/08/video20151204080000.mkv");
                FsLocal_ShareBuffer_resize(sizeof ("99999999/08/video20151204080000.mkv")*2 + len * 2);
                sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                char *rst_1 = FsLocal_ShareBuffer + FsLocal_ShareBufferPos + sizeof ("99999999/08/video20151204080000.mkv")*2;
                char *rst_2 = rst_1 + len;
                //pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                fs_dir_match_prefect(rst_1, rst_2, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "99999999/08/video20151204080000.mkv", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0);
                if (0 == rst_1[0]) {
                    FsPrintf(1, "Can not find data for the uuid:\"%s\".\n", pRecord_item->ro._uuid);
                    fflush(stdout);
                    /* 删除硬盘记录文件 */
                    len = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                    FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                    if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && unlink(FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                        FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                        fflush(stdout);
                    }
                    (FsLocal_ShareBuffer + FsLocal_ShareBufferPos)[len - 5] = '0';
                    FsPrintf(1, "Unlink file:\"%s\" for uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, pRecord_item->ro._uuid);
                    if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK) == 0 && unlink(FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                        FsPrintf(1, "Unlink file:\"%s\" error:\"%s\",uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                        fflush(stdout);
                    }
                    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, pRecord_item->ro._videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                    pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
                    pRecord_item->p._videoInfo_file = 0;
                    pRecord_item->p.videoTimeEnd[0] = 0;
                } else {
                    FILE *fd;
                    struct tm ti;
                    memset(&ti, 0, sizeof (struct tm));
                    /* 当前文件的开始时间 */
                    sscanf(rst_1 + len - sizeof ("20151204080000.mkv"), "%4d%2d%2d%2d%2d%2d*", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
                    ti.tm_year -= 1900;
                    ti.tm_mon -= 1;
                    theTime = mktime(&ti);
                    double thisGmtTime = fs_time_GMT_get();
                    if (theTime < thisGmtTime) {
                        fd = fopen(rst_1, "rb");
                        if (NULL == fd) {
                            FsPrintf(1, "Open the file:\"%s\" error:\"%s\" for the uuid:\"%s\".\n", rst_1, strerror(errno), pRecord_item->ro._uuid);
                            fflush(stdout);
                        } else {
                            VideoMKV *pMKV = video_mkv_new_from_file__IO(fd);
                            if (NULL == pMKV) {
                                FsPrintf(1, "Read mkv from the file:\"%s\" error:\"%s\" for the uuid:\"%s\".\n", rst_1, strerror(errno), pRecord_item->ro._uuid);
                                fflush(stdout);
                            } else {
                                if (pMKV->videoIndex) theTime += pMKV->channel[pMKV->videoIndex].list->nodeCount / pMKV->channel[pMKV->videoIndex].p.video.frameRate;
                                else if (pMKV->audioIndex) theTime += pMKV->channel[pMKV->audioIndex].list->nodeCount / pMKV->channel[pMKV->audioIndex].p.audio.frameRate;
                                video_mkv_delete__OI(pMKV);
                                if (theTime > thisGmtTime) {
                                    /* 可能是校过时,也可能是帧率统计错误 */
                                    FsPrintf(1, "Time has error,theTime=%lf,thisGmtTime=%lf,rst_1:\"%s\",uuid:\"%s\".\n", theTime, thisGmtTime, rst_1, pRecord_item->ro._uuid);
                                    fflush(stdout);
                                    /* 结束时间取整 */
                                    theTime = (time_t) thisGmtTime;
                                }
                            }
                            fclose(fd);
                        }
                    } else {
                        /* 可能是校过时间 */
                        FsPrintf(1, "Time has error,theTime=%lf,thisGmtTime=%lf,rst_1:\"%s\",uuid:\"%s\".\n", theTime, thisGmtTime, rst_1, pRecord_item->ro._uuid);
                        fflush(stdout);
                        /* 结束时间取整 */
                        theTime = (time_t) theTime;
                    }
                    time_t ts = (time_t) theTime;
                    localtime_r(&ts, &ti);
                    len = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "<end>%04d-%02d-%02d/%02d:%02d:%02d.%03d</end>\n", 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec,
                            (int) ((theTime - ts) * 1000));
                    rst_1 = FsLocal_ShareBuffer + FsLocal_ShareBufferPos + len + 1;
                    sprintf(rst_1, 0 == pRecord_item->p._videoInfo_file ? "%s%s/video/__info.txt" : "%s%s/video/__inf0.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                    fd = fopen(rst_1, "ab+");
                    if (fd) {
                        if (fwrite(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, len, 1, fd) != 1) {
                            FsPrintf(1, "Write(len=%d) file:\"%s\" error:\"%s\",type=%hhu,uuid:\"%s\".\n", len, rst_1, strerror(errno), type, pRecord_item->ro._uuid);
                            fflush(stdout);
                        }
                        fclose(fd);
                    } else {
                        FsPrintf(1, "Open file:\"%s\" error:\"%s\",type=%hhu,uuid:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), type, pRecord_item->ro._uuid);
                        fflush(stdout);
                    }
                    if (pRecord_item->p.videoTimeEnd[pRecord_item->p._videoInfo_file] < theTime)pRecord_item->p.videoTimeEnd[pRecord_item->p._videoInfo_file] = theTime;
                    if (1 == pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1]->childNodeCount) {
                        if (*(double*) pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1]->child[0]->data.buffer < theTime) {
                            pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                            *(double*) fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1],
                                    "end", FsEbmlNodeType_Time)->data.buffer = ((unsigned long long) (theTime * 1000)) / 1000.0 + Record_P_videoinfo_precision;
                            pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                            /* 判断是否可切换文件 */
                            sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0 == pRecord_item->p._videoInfo_file ? "%s%s/video/__inf0.txt" : "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                            if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK)) {
                                /* 可以切换 */
                                pRecord_item->p._videoInfo_file = (pRecord_item->p._videoInfo_file + 1)&0x1;
                                pRecord_item->p.videoTimeEnd[pRecord_item->p._videoInfo_file] = 0;
                                FsPrintf(1, "switch to %hhu for uuid:\"%s\".\n", pRecord_item->p._videoInfo_file, pRecord_item->ro._uuid);
                            }
                        } else {
                            FsPrintf(1, "Invalid time(=%lf/%lf),type=%hhu,uuid:\"%s\".\n",
                                    *(double*) pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1]->child[0]->data.buffer,
                                    theTime, type, pRecord_item->ro._uuid);
                            fflush(stdout);
                            if (1) {
                                /* 删除当前的这个时间 */
                                pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                                fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, pRecord_item->ro._videoInfo_pos->childNodeCount - 1, 1);
                                pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                                /* 判断是否可切换文件 */
                                sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0 == pRecord_item->p._videoInfo_file ? "%s%s/video/__inf0.txt" : "%s%s/video/__info.txt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
                                if (access(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, F_OK)) {
                                    /* 可以切换 */
                                    pRecord_item->p._videoInfo_file = (pRecord_item->p._videoInfo_file + 1)&0x1;
                                    pRecord_item->p.videoTimeEnd[pRecord_item->p._videoInfo_file] = 0;
                                    FsPrintf(1, "switch to %hhu for uuid:\"%s\".\n", pRecord_item->p._videoInfo_file, pRecord_item->ro._uuid);
                                }
                            } else {
                                /* 从文件中重建 */
                                pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                                fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, pRecord_item->ro._videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                                pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                                FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
                                record_P_videoInfo_update_from_file(pRecord_item, 1, theTime, &FsLocal_ShareBuffer);
                                FsLocal_ShareBuffer_call_after();
                            }
                        }
                    } else {
                        FsPrintf(1, "childNodeCount(=%u) is invalid,type=%hhu,uuid:\"%s\".\n",
                                pRecord_item->ro._videoInfo_pos->child[pRecord_item->ro._videoInfo_pos->childNodeCount - 1]->childNodeCount, type, pRecord_item->ro._uuid);
                        fflush(stdout);
                    }
                    pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
                }
                //pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
            } else {
                /* 数据只写到了内存,未写入硬盘,删除最后一个item */
                pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                fs_Ebml_node_delete_child_index(NULL, pRecord_item->ro._videoInfo_pos, pRecord_item->ro._videoInfo_pos->childNodeCount - 1);
                pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
                pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
            }
        } else {
            FsPrintf(1, "Item count(=%u/%u) is invalid,type=%hhu,uuid:\"%s\".\n",
                    pRecord_item->ro._videoInfo_pos->childNodeCount, Record_P_videoInfo_HeadCount, type, pRecord_item->ro._uuid);
            fflush(stdout);
            /* 从硬盘文件读取 */
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            //pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
            record_P_videoInfo_update_from_file(pRecord_item, 0, fs_time_GMT_get(), &FsLocal_ShareBuffer);
            //pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
            FsLocal_ShareBuffer_call_after();
        }
    } else {

        FsPrintf(1, "Invalid type(=%hhu),uuid:\"%s\".\n", type, pRecord_item->ro._uuid);
        fflush(stdout);
    }
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
}

/* 从硬盘文件更新某相机的录像信息,读取返回1,未读取返回0,出错返回-1 */
static int record_P_videoInfo_update_remote(struct Record_item * const pRecord_item, /* 是否检查更新标记,0-不检查,1-检查 */const unsigned char checkUpdate
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
    /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var info_in,buffer_in
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
    /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
    /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
    char * info_in = FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
    const int len0 = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/remote/__info.txt.upt");
    char * buffer_in = info_in + len0;
    FsLocal_ShareBuffer_resize(len0 + sizeof ("<begin>2018-03-23/13:39:48.272</begin>\n<begin>"));
    sprintf(info_in, "%s%s/video/remote/__info.txt.upt", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
    //FsPrintf(1, "info_in:\"%s\",checkUpdate=%hhu.\n", info_in, checkUpdate);
    if (checkUpdate && access(info_in, F_OK) != 0) {
        /* 不需要更新 */
        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
        /* 检查时间限制 */
        return 0;
    }
    info_in[len0 - sizeof (".upt")] = 0;
    FsStructList * const pStructList = fs_StructList_new__IO(6, sizeof (double)*2);
    {
        FILE * const fd = fopen(info_in, "rb");
        if (fd != NULL) {
            double data[2] = {-1.0, -1.0};
            int timeZone = fs_time_timezone_get()*3600;
            int pos = 0;
            int len;
            //#define __read_video_info_in_Record
#define __read_video_info_precision Record_P_videoinfo_precision
#ifndef __read_video_info 
            while ((len = fread(buffer_in + pos, 1, sizeof ("<begin>2018-03-23/13:39:48.272</begin>\n<begin>") - pos - 1, fd)) > 0) {
                pos += len;
                buffer_in[pos] = 0;
                //FsPrintf(1, "info_in:\"%s\",checkUpdate=%hhu,pStructList->nodeCount=%lu,len=%d,%s.\n", info_in, checkUpdate, pStructList->nodeCount, len, buffer_in);
                if ('<' == buffer_in[0]) {
                    if (memcmp("begin>", buffer_in + 1, sizeof ("begin>") - 1) == 0) {
                        /* 是开始位置 */
                        const char *ps;
                        if ((ps = strstr(buffer_in + 1 + sizeof ("begin>") - 1, "</begin>")) != NULL) {
                            /* 找到了 */
                            if (data[0] > 0.0) {
                                /* 上一条数据没有找到结束位置 */
                                FsLog(FsLogType_matchError, FsPrintfIndex, "No end tag from file:\"%s\",this data:\"%s\"/%lf,pStructList->nodeCount=%lu.\n", info_in, buffer_in, data[0], pStructList->nodeCount);
                                fflush(stdout);
#ifdef __read_video_info_in_Record
                                fs_StructList_insert_order_double(pStructList, data);
#else
                                fs_StructList_clear(pStructList);
                                data[0] = -1.0;
                                break;
#endif
                            }
                            data[0] = fs_time_get_string2(buffer_in + 1 + sizeof ("begin>") - 1, ps - (buffer_in + 1 + sizeof ("begin>") - 1)) - timeZone + __read_video_info_precision;
#ifdef __read_video_info_in_Record
#ifndef __Record_P_videoInfo_update_0_1
                            ((char*) (data + 2))[0] = 0;
#else
                            ((char*) (data + 2))[0] = 1;
#endif
#else
                            /* 校验数据的合法性 */
                            if (data[1] > 0.0 && data[0] <= data[1]) {
                                /* 无效的开始时间 */
                                FsLog(FsLogType_matchError, FsPrintfIndex, "Invalid startTime(=%lf/%lf),file:\"%s\",data:\"%s\",pStructList->nodeCount=%lu.\n", data[0], data[1], info_in, buffer_in, pStructList->nodeCount);
                                fflush(stdout);
                                fs_StructList_clear(pStructList);
                                data[0] = -1.0;
                                break;
                            }
#endif
                            len = ps - buffer_in + sizeof ("</begin>") - 1;
                        } else {
                            /* 不存在 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                            fflush(stdout);
                            len = 1 + sizeof ("begin>") - 1;
                        }
                    } else if (memcmp("end>", buffer_in + 1, sizeof ("end>") - 1) == 0) {
                        /* 是结束位置 */
                        const char *ps;
                        if ((ps = strstr(buffer_in + 1 + sizeof ("end>") - 1, "</end>")) != NULL) {
                            /* 找到了 */
                            if (data[0] < 0.0) {
                                /* 上一条数据不是开始标记,忽略本条数据 */
                                FsLog(FsLogType_matchError, FsPrintfIndex, "No start tag from file:\"%s\",this data:\"%s\"/%lf,pStructList->nodeCount=%lu.\n", info_in, buffer_in, data[0], pStructList->nodeCount);
                                fflush(stdout);
                            } else {
                                data[1] = fs_time_get_string2(buffer_in + 1 + sizeof ("end>") - 1, ps - (buffer_in + 1 + sizeof ("end>") - 1)) - timeZone + __read_video_info_precision;
                                if (data[1] > data[0]) {
                                    /* 数据有效 */
#ifdef __read_video_info_in_Record
#ifndef __Record_P_videoInfo_update_0_1
                                    ((char*) (data + 2))[0] = 2;
#else
                                    ((char*) (data + 2))[0] = 3;
#endif
                                    if (fs_StructList_insert_order_double(pStructList, data) < 0) {
                                        FsLog(FsLogType_matchError, FsPrintfIndex, "Insert failed,end=%lf/%lf,file:\"%s\",data:\"%s\",pStructList->nodeCount=%lu.\n",
                                                data[1], data[0], info_in, buffer_in, pStructList->nodeCount);
                                        fflush(stdout);
                                    }
#else
                                    fs_StructList_insert_tail(pStructList, data);
#endif

                                } else {
                                    FsLog(FsLogType_matchError, FsPrintfIndex, "Invalid end(=%lf/%lf) from file:\"%s\",this data:\"%s\",pStructList->nodeCount=%lu.\n", data[1], data[0], info_in, buffer_in, pStructList->nodeCount);
                                    FsLogTag(10);
                                    fflush(stdout);
#ifdef __read_video_info_in_Record
                                    /* 差值不到1S的忽略,浪费时间 */
                                    if (data[0] - data[1] > 1.0) fs_StructList_insert_order_double(pStructList, data);
#else
                                    fs_StructList_clear(pStructList);
                                    data[0] = -1.0;
                                    break;
#endif                              
                                }
                                data[0] = -1.0;
                            }
                            len = ps - buffer_in + sizeof ("</end>") - 1;
                        } else {
                            /* 不存在 */
                            FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                            fflush(stdout);
                            len = 1 + sizeof ("end>") - 1;
                        }
                    } else {
                        /* 无法识别的数据 */
                        FsLog(FsLogType_matchError, FsPrintfIndex, "Unkown data(len=%d):\"%s\" from file:\"%s\".\n", pos, buffer_in, info_in);
                        fflush(stdout);
                        len = 1;
                    }
                }
                /* 向后查找数据 */
                for (; buffer_in[len] != '<' && len < pos; len++);
                if ((pos -= len) > 0) memmove(buffer_in, buffer_in + len, pos);
            }
#ifdef __read_video_info_in_Record
#undef __read_video_info_in_Record
#endif
#endif
            if (data[0] > 0.0) {
                /* 上一条数据没有找到结束位置 */
                data[1] = -1.0;
                fs_StructList_insert_tail(pStructList, data);
            }
            fclose(fd);
        } else if (access(info_in, F_OK) == 0) {
            Record_itemLog(FsLogType_error, "Open the file:\"%s\" error:\"%s\"", info_in, strerror(errno));
            fflush(stdout);
        }
    }
    //FsPrintf(1, "info_in:\"%s\",checkUpdate=%hhu,pStructList->nodeCount=%lu.\n", info_in, checkUpdate, pStructList->nodeCount);
    unsigned char locked = 0, change = 0;
    if (pRecord_item->ro._videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
        change = 1;
        if (0 == locked) locked = 1, pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
        fs_Ebml_node_delete_child_n(NULL, pRecord_item->ro._videoInfo_pos, Record_P_videoInfo_HeadCount, pRecord_item->ro._videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
    }
    if (pStructList->nodeCount > 0) {
        change = 1;
        if (0 == locked) locked = 1, pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
        unsigned int ui = pStructList->nodeCount;
        double (* pNode) [2] = (((double (*)[2])pStructList->pNode) + pStructList->startIndex);
        for (; ui > 0; ui--) {
            double *const pData = *pNode++;
            struct FsEbml_node * const pEbml_node = fs_Ebml_node_addChild(NULL, pRecord_item->ro._videoInfo_pos, "item", FsEbmlNodeType_Struct);
            *(double*) fs_Ebml_node_addChild(NULL, pEbml_node, "begin", FsEbmlNodeType_Time)->data.buffer = pData[0];
            if (pData[1] > 0.0) *(double*) fs_Ebml_node_addChild(NULL, pEbml_node, "end", FsEbmlNodeType_Time)->data.buffer = pData[1];
        }
    }
    if (locked) pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    if (change)pRecord_item->ro._pRecord->rw.videoInfoChange = 1;
    fs_StructList_delete__OI(pStructList);
    /* 清除.upt标记 */
    info_in[len0 - sizeof (".upt")] = '.';
    if (unlink(info_in) != 0 && access(info_in, F_OK) == 0) {
        Record_itemLog(FsLogType_error, "Unlink the file:\"%s\" error:\"%s\"", info_in, strerror(errno));
        fflush(stdout);
    }
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);

    return change > 0 ? 1 : 0;
}

/* 获取录像信息,成功返回FsEbml指针,失败返回NULL */
static FsEbml * record_P_item_recordInfo_get_pthreadSafety__IO(struct Record_item * const pRecord_item, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    /* 不记录视频 */
    if (0 == pRecord_item->ro._recordVideoMode)return NULL;
    if (0 == pRecord_item->p.videoInfoHasLoad) {
        Record_itemLog(FsLogType_info, "Video not init");
        fflush(stdout);
        return NULL;
    }
    pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
    if (Record_item_is_localVideoStorage(pRecord_item->ro._recordVideoMode)) record_P_videoInfo_update(pRecord_item, 2, 0.0, pShareBuffer);
    else record_P_videoInfo_update_remote(pRecord_item, 1, pShareBuffer);
    pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
    /* 把数据取出来 */
    FsEbml * const rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    fs_Ebml_node_addChild_from_node(rst, (struct FsEbml_node*) rst, pRecord_item->ro._videoInfo_pos->parent->child[0]);
    fs_Ebml_node_addChild_from_node_child(rst, (struct FsEbml_node*) rst, pRecord_item->ro._videoInfo_pos);
    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    if (pRecord_item->ro._realVideoSaveTime) {
        /* 限制储存时长 */
        double starttime = fs_time_GMT_get() - pRecord_item->ro._realVideoSaveTime;
        if (rst->childNodeCount > Record_P_videoInfo_HeadCount + 1) {
            if (*(double*) rst->child[Record_P_videoInfo_HeadCount + 1]->child[0]->data.buffer < starttime) {
                do {
                    if (rst->child[Record_P_videoInfo_HeadCount + 1]->childNodeCount < 2 ||
                            *(double*) rst->child[Record_P_videoInfo_HeadCount + 1]->child[1]->data.buffer > starttime) {
                        *(double*) rst->child[Record_P_videoInfo_HeadCount + 1]->child[0]->data.buffer =
                                ((unsigned long long) (starttime * 1000)) / 1000.0 + Record_P_videoinfo_precision;
                        break;
                    } else {
                        fs_Ebml_node_delete_child_n(NULL, (struct FsEbml_node*) rst, Record_P_videoInfo_HeadCount + 1, 1);
                    }
                } while (rst->childNodeCount > Record_P_videoInfo_HeadCount + 1);
            }
        } else {

            /* 出错 */
            FsPrintf(FsPrintfIndex, "videoInfo is NULL,uuid:\"%s\".\n", pRecord_item->ro._uuid);
            fflush(stdout);
        }
    }
    return rst;
}
static FsConfig * record_P_protocol_videolist_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 17, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "videolist_get", "videolist_get", "videolist_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号,为空表示获取本机的所有相机数据,为空时在录像信息发生变化时会主动推送", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "sort", "结果的排序方式", "结果的排序方式,缺失或无效时不排序,ASC-升序,DESC-降序", 0, 0x7, 0, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "ASC", "ASC-升序", "ASC-升序");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "DESC", "DESC-降序", "DESC-降序");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "time_start", "开始时间", "开始时间,0时区的时间", 0, 0x7, 0, 22, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2022-05-16/5:13:30.0", "2022-05-16/5:13:30.0", "2022-05-16/5:13:30.0");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "time_end", "结束时间", "结束时间,0时区的时间", 0, 0x7, 0, 22, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2022-05-17/5:13:30.0", "2022-05-17/5:13:30.0", "2022-05-17/5:13:30.0");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}

static FsConfig * record_P_protocol_videotimeinfo_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 17, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "videotimeinfo_get", "videotimeinfo_get", "videotimeinfo_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号,为空表示获取本机的所有相机数据,为空时在录像信息发生变化时会主动推送", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "1", "1", "1");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}


static FsConfig * record_P_protocol_video_part() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "video_part", "video_part", "video_part");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号,为空表示获取本机的所有相机数据,为空时在录像信息发生变化时会主动推送", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "time_start", "开始时间", "开始时间,0时区的时间", 0, 0x7, 0, 24, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2016-7-2/5:13:30.0", "2016-7-2/5:13:30.0", "2016-7-2/5:13:30.0");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "time_end", "结束时间", "结束时间,0时区的时间", 0, 0x7, 0, 24, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2016-7-2/5:13:30.0", "2016-7-2/5:13:30.0", "2016-7-2/5:13:30.0");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定,支持ebml(压缩数据),xml,json", 0, 0x7, 3, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "raw", "文本数据", "文本数据");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "ebml", "ebml", "ebml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
int record_P_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#undef FsFunctionName
#define FsFunctionName record_P_cmd_cb
    struct FsEbml_node *pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_type");
    if (strcmp("video_part", pEbml_node->data.buffer) == 0) {
        /* 录像合成 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time_start");
        double startTime, endTime;
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth || (startTime = fs_time_get_string(pEbml_node->data.buffer)) < 0.1) {
            FsPrintf(FsPrintfIndex, "Invalid startTime,data:\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            startTime = fs_time_GMT_get();
        }
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time_end");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth || (endTime = fs_time_get_string(pEbml_node->data.buffer)) < 0.1) {
            FsPrintf(FsPrintfIndex, "Invalid endTime,data:\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            endTime = startTime + 5.0;
        }
        if (startTime + 1.0 > endTime) {
            FsPrintf(FsPrintfIndex, "Invalid time(=%lf/%lf),data:\n", startTime, endTime);
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            return -1;
        } else {
            double thisTime = fs_time_uptime_get();
            FsPrintf2(FsPrintfIndex, "Time(=%lf/%lf,gmttime=%lf),data:\n", startTime, endTime, fs_time_GMT_get());
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            struct Record_item *pRecord_item = (struct Record_item*) p;
            time_t ts = (time_t) startTime;
            struct tm ti_s;
            localtime_r(&ts, &ti_s);
            ts = (time_t) endTime;
            struct tm ti_e;
            localtime_r(&ts, &ti_e);
#undef Fs_ShareBuffer_var    
#undef Fs_ShareBuffer_var_check
#undef Fs_ShareBuffer_var_array
#undef Fs_ShareBuffer_var_array_check
            /* 初始化共享buffer的使用环境 */ FsLocal_ShareBuffer_init(pShareBuffer, 1);
            /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_in结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var a_in_acin,b_in */
#define Fs_ShareBuffer_var buffer_in
            /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部变量,以_cin结尾,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_check a_cin_acin,b_cin */
#define Fs_ShareBuffer_var_check
            /* 定义与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_ain结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array a_cin_ain,2 */
#define Fs_ShareBuffer_var_array
            /* 定义可能与共享buffer--FsLocal_ShareBuffer关联的局部指针数组变量,以_acin结尾,需要指明数组成员的个数,多个变量使用逗号隔开,如#define Fs_ShareBuffer_var_array_check a_cin_acin,2 */
#define Fs_ShareBuffer_var_array_check
            char *buffer_in = FsLocal_ShareBuffer + FsLocal_ShareBufferPos;
            FsLocal_ShareBuffer_resize(strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid)
                    + sizeof ("/incident/20151007/23/inc20151014185944-20151014185954.mkv"));
            int len = sprintf(buffer_in, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir,
                    pRecord_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec,
                    1900 + ti_e.tm_year, ti_e.tm_mon + 1, ti_e.tm_mday, ti_e.tm_hour, ti_e.tm_min, ti_e.tm_sec);
            endTime -= startTime;
            startTime = thisTime + (startTime - fs_time_GMT_get());
            endTime += startTime;
            record_item_add_partVideo_pthreadSafety(pRecord_item, buffer_in, startTime, endTime, NULL);
            ////////////////////////////////////////////////////////////////////////
            /* 发回执数据 */
            buffer_in += sizeof ("/fs/project/data") - 1;
            len -= sizeof ("/fs/project/data") - 1;
            {
                struct FsEbml_node * const return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
                if (return_type && 0 < return_type->data.lenth) {
                    if (0 == strcmp("raw", return_type->data.buffer)) requestDataType = 0;
                    else if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
                    else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
                    else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
                } else requestDataType = 0;
            }
            FsObjectBase * pObjectBase;
            if (0 == requestDataType) {
                pObjectBase = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8
                        + Memery_Alignment(len) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data")));
                fs_ObjectBase_init(pObjectBase, sizeof (FsObjectBase) + 8 + Memery_Alignment(len) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data")), sizeof (FsObjectBase));
                pObjectBase->len = 8 + len;
                ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0x2U, ((unsigned int *) pObjectBase->data)[1] = len;
                memcpy(pObjectBase->data + 8, buffer_in, len);
            } else {
                FsEbml * const pEbml_send = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml_send, (struct FsEbml_node*) pEbml_send, "videoname", FsEbmlNodeType_String), len + 1, buffer_in);
                if (1 == requestDataType) {
                    pObjectBase = fs_Ebml_to_ObjectBase__IO(pEbml_send, sizeof (FsObjectBase) + 8
                            , Memery_Alignment(1) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data")), NULL);
                    FsLocal_ShareBuffer_call_before(buffer_in + len + 1 - FsLocal_ShareBuffer);
                    fs_Ebml_delete__OI(pEbml_send, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    pObjectBase->len -= Memery_Alignment(1) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data"));
                    pObjectBase->data -= 8;
                    *((unsigned int *) pObjectBase->data) = (head & 0xFFFFFFF0U) | 0x3, *((unsigned int *) (pObjectBase->data + 4)) = pObjectBase->len;
                    pObjectBase->len += 8;
                } else if (2 == requestDataType) {
                    FsXml * const pXml = fs_Ebml_convert_to_Xml__IO(pEbml_send);
                    FsLocal_ShareBuffer_call_before(buffer_in + len + 1 - FsLocal_ShareBuffer);
                    fs_Ebml_delete__OI(pEbml_send, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    pObjectBase = fs_Xml_to_ObjectBase__IO(pXml, sizeof (FsObjectBase) + 8
                            , Memery_Alignment(1) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data")), FsStringLenData("\r\n"), 0, NULL, 4);
                    FsLocal_ShareBuffer_call_before(buffer_in + len + 1 - FsLocal_ShareBuffer);
                    fs_Xml_delete__OI(pXml, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    pObjectBase->len -= Memery_Alignment(1) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data"));
                    pObjectBase->data -= 8;
                    *((unsigned int *) pObjectBase->data) = (head & 0xFFFFFFF0U) | 0xA, *((unsigned int *) (pObjectBase->data + 4)) = pObjectBase->len;
                    pObjectBase->len += 8;
                } else if (3 == requestDataType) {
                    FsJson * const pJson = fs_Ebml_convert_to_Json__IO(pEbml_send);
                    FsLocal_ShareBuffer_call_before(buffer_in + len + 1 - FsLocal_ShareBuffer);
                    fs_Ebml_delete__OI(pEbml_send, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8
                            , Memery_Alignment(1) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data")), FsStringLenData("\r\n"), 0, NULL, 4);
                    FsLocal_ShareBuffer_call_before(buffer_in + len + 1 - FsLocal_ShareBuffer);
                    fs_Json_delete__OI(pJson, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    pObjectBase->len -= Memery_Alignment(1) + Memery_Alignment(sizeof (unsigned int)*3) + (sizeof (unsigned long)+len + sizeof ("/fs/project/data"));
                    pObjectBase->data -= 8;
                    *((unsigned int *) pObjectBase->data) = (head & 0xFFFFFFF0U) | 0xA, *((unsigned int *) (pObjectBase->data + 4)) = pObjectBase->len;
                    pObjectBase->len += 8;
                } else {
                    /* 程序不会到这 */
                    FsPrintf(FsPrintfIndex, "Invalid requestDataType(=%hhd) data:\n", requestDataType);
                    fflush(stdout);
                    FsLocal_ShareBuffer_call_before(buffer_in + len + 1 - FsLocal_ShareBuffer);
                    fs_Ebml_delete__OI(pEbml_send, &FsLocal_ShareBuffer);
                    FsLocal_ShareBuffer_call_after();
                    pObjectBase = NULL;
                }
            }
            /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);

            memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);
            buffer_in = pObjectBase->data + Memery_Alignment(pObjectBase->len) + Memery_Alignment(sizeof (unsigned int)*3);
            len += sizeof ("/fs/project/data");
            *((unsigned long*) buffer_in) = ~0L, memcpy(buffer_in + sizeof (unsigned long), FsLocal_ShareBuffer + FsLocal_ShareBufferPos, len);
            configManager_send_pthreadSafety__OI_3(pRecord_item->ro._pRecord->ro._pConfigManager, endTime > thisTime ? endTime + 1.0 + (endTime - startTime) / 8 : thisTime + 1.0 + (endTime - startTime) / 8,
                    pObjectBase, record_P_videoPart_condition, buffer_in, NULL);

            return 1;
        }
    } else if (strcmp("videolist_get", pEbml_node->data.buffer) == 0) {
        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
        /* 获取实时视频的文件名 */
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time_start");
        double startTime, endTime;
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth || (startTime = fs_time_get_string(pEbml_node->data.buffer)) < 0.1) {
            FsPrintf(1, "Invalid startTime,data:\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            return -1;
        }
        struct Record_item *pRecord_item = (struct Record_item*) p;
        if (pRecord_item->ro._realVideoSaveTime > 0 && startTime + pRecord_item->ro._realVideoSaveTime < fs_time_GMT_get()) {
            FsPrintf2(1, "Reset old startTime=%lf,realVideoSaveTime=%u\n", startTime, pRecord_item->ro._realVideoSaveTime);
            startTime = fs_time_GMT_get() - pRecord_item->ro._realVideoSaveTime;
            FsPrintf2(1, "Reset new startTime=%lf,realVideoSaveTime=%u\n", startTime, pRecord_item->ro._realVideoSaveTime);
        }
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "time_end");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth || (endTime = fs_time_get_string(pEbml_node->data.buffer)) < 0.1) {
            FsPrintf(1, "Invalid endTime,data:\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            return -1;
        }
        if (startTime + 1.0 > endTime) {
            FsPrintf(1, "Invalid time(=%lf/%lf),realVideoSaveTime=%u,data:\n", startTime, endTime, pRecord_item->ro._realVideoSaveTime);
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            return -1;
        }
        unsigned int dirLen = strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/") - 1;
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
        char *const buffer1 = FsLocal_ShareBuffer + FsLocal_ShareBufferPos + dirLen + sizeof ("99999999/08/video20151204080000.mkv");
        char *const buffer2 = buffer1 + sizeof ("99999999/08/video20151204080000.mkv");
        FsLocal_ShareBuffer_resize(dirLen + sizeof ("99999999/08/video20151204080000.mkv")*3);
        time_t ts = (time_t) startTime;
        struct tm ti;
        localtime_r(&ts, &ti);
        sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
        sprintf(buffer1, "%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv",
                1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
        ts = (time_t) endTime;
        localtime_r(&ts, &ti);
        sprintf(buffer2, "%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv",
                1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
        FsStringList *list;
        {
            struct FsEbml_node *sort = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "sort");
            if (sort && 0 < sort->data.lenth && (0 == strcmp("ASC", sort->data.buffer) || 0 == strcmp("1", sort->data.buffer)))
                list = fs_dir_between_ASC__IO(dirLen, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, buffer1, buffer2, FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
            else if (sort && 0 < sort->data.lenth && (0 == strcmp("DESC", sort->data.buffer) || 0 == strcmp("2", sort->data.buffer)))
                list = fs_dir_between_DESC__IO(dirLen, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, buffer1, buffer2, FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
            else list = fs_dir_between__IO(dirLen, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, buffer1, buffer2, FsLocal_ShareBuffer + FsLocal_ShareBufferPos);
        }
        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
        if (list) {
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            char **ppNode = (char**) list->list.pNode + list->list.startIndex;
            unsigned int l = list->list.nodeCount;
            for (; l > 0; l--)
                fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "videoname", FsEbmlNodeType_String), (*ppNode++) + strlen("/fs/project/data"));
            fs_StringList_delete__OI(list);
            struct FsEbml_node * return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
            if (return_type && 0 < return_type->data.lenth) {
                if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
                else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
                else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
            }
            configManager_conncet_refer_sendData(NULL, NULL, NULL, rst, requestID_3, head | 0x2, requestDataType, pObjectBaseBuffer);
            fs_Ebml_delete__OI(rst, pShareBuffer);
            return 1;
        } else {
            FsPrintf(FsPrintfIndex, "No file find.\n,data:\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            configManager_send_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, FsStringLenData("No file find.\n"), requestID_3, (head & 0xFFFFFFF0U) | 0x6U);
            return 1;
        }
    } else if (strcmp("videotimeinfo_get", pEbml_node->data.buffer) == 0) {
        /* 获取录像记录的时间信息 */
        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_for_uuid");
        if (NULL == pEbml_node || 0 == pEbml_node->data.lenth) {
            /* 获取本机所有相机的记录信息 */
            struct Record *pRecord = (struct Record*) p;
            pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
            if (pRecord->p.videoInfoData__videoInfoDataClientList) {
                /* 统计下是否有通道断开了连接 */
                configManager_conncet_refer_check(pRecord->ro._pConfigManager, pRecord->ro.__videoInfoDataClientList);
                {
#ifndef __cmd_cb_cal_return_type 
                    const char *const return_type = fs_Ebml_node_get_first_string(pEbml, (struct FsEbml_node*) pEbml, "return_type", NULL);
                    if (return_type) {
                        if (0 == strcmp("ebml", return_type)) requestDataType = 1;
                        else if (0 == strcmp("xml", return_type)) requestDataType = 2;
                        else if (0 == strcmp("json", return_type)) requestDataType = 3;
                    }
#endif
                }
                configManager_conncet_refer_sendData(&pRecord->p.videoInfoData_ebml__videoInfoDataClientList, &pRecord->p.videoInfoData_xml__videoInfoDataClientList, &pRecord->p.videoInfoData_json__videoInfoDataClientList
                        , pRecord->p.videoInfoData__videoInfoDataClientList, requestID_3, head | 0x2, requestDataType, pObjectBaseBuffer);
                unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
                if (fs_StructList_insert_order(pRecord->ro.__videoInfoDataClientList, data) < 0) {
                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    return 1;
                } else {
                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    return -128;
                }
            } else {
                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                FsPrintf(1, "Vsys is busy.\n");
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                fflush(stdout);
                return -1;
            }
        } else {
            FsEbml * const pEbml_send = record_P_item_recordInfo_get_pthreadSafety__IO((struct Record_item*) p, pShareBuffer);
            if (NULL == pEbml_send)return -1;
            {
                struct FsEbml_node * pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
                if (pEbml_node && 0 < pEbml_node->data.lenth) {
                    if (0 == strcmp("ebml", pEbml_node->data.buffer)) requestDataType = 1;
                    else if (0 == strcmp("xml", pEbml_node->data.buffer)) requestDataType = 2;
                    else if (0 == strcmp("json", pEbml_node->data.buffer)) requestDataType = 3;
                }
            }
            configManager_conncet_refer_sendData(NULL, NULL, NULL, pEbml_send, requestID_3, head | 0x2, requestDataType, pObjectBaseBuffer);
            fs_Ebml_delete__OI(pEbml_send, pShareBuffer);
            return 1;
        }
    } else {
        FsPrintf(FsPrintfIndex, "Invalid cmd:\"%s\".\n,data:\n", pEbml_node->data.buffer);
        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");

        return -1;
    }
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
int record_P_cmd_snapshort_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Record_item * const pRecord_item
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    /* 通道快照获取 */
    fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
    FsObjectImageFrame * const pFrame = record_item_snap_get_pthreadSafety__IO(pRecord_item);
    if (pFrame) {
        struct FsEbml_node * pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (pEbml_node != NULL) {
            if (4 == pEbml_node->data.lenth && memcmp("raw", pEbml_node->data.buffer, 3) == 0) requestDataType = 0;
            else if (5 == pEbml_node->data.lenth && memcmp("ebml", pEbml_node->data.buffer, 4) == 0) requestDataType = 1;
            else if (4 == pEbml_node->data.lenth && memcmp("xml", pEbml_node->data.buffer, 3) == 0) requestDataType = 2;
            else if (5 == pEbml_node->data.lenth && memcmp("json", pEbml_node->data.buffer, 4) == 0) requestDataType = 3;
        }
        pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "snapshort_type");
        if (pEbml_node && 4 == pEbml_node->data.lenth && memcmp("jpg", pEbml_node->data.buffer, 3) == 0) {
            /* 获取jpeg数据 */
            FsObjectBase * ppObjectBase[2];
            ppObjectBase[1] = (FsObjectBase *) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_Jpeg_0);
            if (ppObjectBase[1]) {
                if (0 == requestDataType) {
                    fs_Object_delete_pthreadSafety__OI(pFrame);
                    /* 裸的二进制数据 */
                    ppObjectBase[0] = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8);
                    fs_ObjectBase_init(ppObjectBase[0], sizeof (FsObjectBase) + 8, sizeof (FsObjectBase));
                    ppObjectBase[0]->len = 8;
                    ((unsigned int*) ppObjectBase[0]->data)[0] = (head & 0xFFFFFFF0U) | 0x0BU, ((unsigned int*) ppObjectBase[0]->data)[1] = ppObjectBase[1]->len;
                    configManager_connect_refer_send__OI_3(requestID_3, 2, ppObjectBase);
                } else {
                    FsEbml * const rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                    *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "capture_gmtTime", FsEbmlNodeType_Time)->data.buffer = pFrame->capture_gmtTime;
                    *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "frameIndex", FsEbmlNodeType_Integer)->data.buffer = pFrame->index;
                    *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "width", FsEbmlNodeType_Integer)->data.buffer = pFrame->width[0];
                    *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "height", FsEbmlNodeType_Integer)->data.buffer = pFrame->height[0];
                    fs_Object_delete_pthreadSafety__OI(pFrame);
                    fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "snapshort_type", FsEbmlNodeType_String), "jpg");
                    fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "data", FsEbmlNodeType_Binary), ppObjectBase[1]->len, ppObjectBase[1]->data);
                    fs_Object_delete_pthreadSafety__OI(ppObjectBase[1]);
#ifndef __record_P_cmd_snapshort_cb_do_to_pObjectBase_send 
                    FsObjectBase * pObjectBase;
                    if (1 == requestDataType) {
                        pObjectBase = fs_Ebml_to_ObjectBase__IO(rst, sizeof (FsObjectBase) + 8, 0, NULL);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0x3U, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    } else if (2 == requestDataType) {
                        FsXml * const pXml = fs_Ebml_convert_to_Xml__IO(rst);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase = fs_Xml_to_ObjectBase__IO(pXml, sizeof (FsObjectBase) + 8, 0, FsStringLenData("\r\n"), 0, NULL, 4);
                        fs_Xml_delete__OI(pXml, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    } else if (3 == requestDataType) {
                        FsJson * const pJson = fs_Ebml_convert_to_Json__IO(rst);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, 0, FsStringLenData("\r\n"), 0, NULL, 4);
                        fs_Json_delete__OI(pJson, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    } else {
                        FsPrintf(FsPrintfIndex, "Invalid requestDataType(=%hhd) data:\n", requestDataType);
                        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                        pObjectBase = fs_Ebml_to_ObjectBase__IO(rst, sizeof (FsObjectBase) + 8, 0, NULL);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0x3U, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    }
                    configManager_connect_refer_send__OI_2(requestID_3, pObjectBase);
#endif
                }
            } else {
                /* 准备jpg数据出错 */
                FsPrintf(FsPrintfIndex, "Get jpg snapshort failed,classIndex=%hu,index=%u,dataValidType=%#llx,ipsrc=%#x,frameRate=%f,capture_uptime=%lf,capture_gmtTime=%lf,data:\n.\n",
                        pFrame->classIndex, pFrame->index, pFrame->dataValidType, pFrame->ipsrc, pFrame->frameRate, pFrame->capture_uptime, pFrame->capture_gmtTime);
                fs_Object_delete_pthreadSafety__OI(pFrame);
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                configManager_conncet_refer_send_buffer(FsStringLenData("Prapare jpg failed.\n"), requestID_3, (head & 0xFFFFFFF0U) | 0x6U, pObjectBaseBuffer);
            }
        }
        else //if (pEbml_node && 4 == pEbml_node->data.lenth && memcmp("bmp", pEbml_node->data.buffer, 3) == 0) 
        {
            /* 获取bmp数据 */
            ImageDib *pDib;
            pthread_mutex_lock(&((FsObject*) pFrame)->mutex);
            if (ImageFrame_valid_RGB_0(pFrame->dataValidType)) {
                pDib = image_dib_new__IO(pFrame->width[0], pFrame->height[0], ImageColorType_RGB, pFrame->width[0]*3);
                memcpy(pDib->data, pFrame->data.rgb[0]->data, pDib->dataLenth);
            } else if (ImageFrame_valid_YCC_0(pFrame->dataValidType) || 1 == image_frame_prapare(pFrame, ImageFrame_YCC_0)) {
                pDib = image_dib_new__IO(pFrame->width[0], pFrame->height[0], ImageColorType_YCbCr, pFrame->width[0]);
                memcpy(pDib->data, pFrame->data.ycc[0]->data, pDib->dataLenth);
            } else {
                pDib = NULL;
                /* 准备bmp数据出错 */
                FsPrintf(FsPrintfIndex, "Get bmp snapshort failed,classIndex=%hu,index=%u,dataValidType=%#llx,ipsrc=%#x,frameRate=%f,capture_uptime=%lf,capture_gmtTime=%lf,data:\n.\n",
                        pFrame->classIndex, pFrame->index, pFrame->dataValidType, pFrame->ipsrc, pFrame->frameRate, pFrame->capture_uptime, pFrame->capture_gmtTime);
                fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                configManager_conncet_refer_send_buffer(FsStringLenData("Prapare bmp failed.\n"), requestID_3, (head & 0xFFFFFFF0U) | 0x6U, pObjectBaseBuffer);
            }
            pthread_mutex_unlock(&((FsObject*) pFrame)->mutex);
            if (pDib) {
                const unsigned int bmpLen = image_dib_get_bmpDataLenth(pDib);
                if (0 == requestDataType) {
                    /* 裸的二进制数据 */
                    fs_Object_delete_pthreadSafety__OI(pFrame);
                    FsObjectBase * const pObjectBase = (FsObjectBase*) fsMalloc(sizeof (FsObjectBase) + 8 + Memery_Alignment(bmpLen));
                    fs_ObjectBase_init(pObjectBase, sizeof (FsObjectBase) + 8 + Memery_Alignment(bmpLen), sizeof (FsObjectBase));
                    image_dib_to_bmpData(pDib, (unsigned char*) pObjectBase->data + 8), pObjectBase->len = 8 + bmpLen;
                    image_dib_delete__OI(&pDib);
                    ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xBU, ((unsigned int*) pObjectBase->data)[1] = bmpLen;
                    configManager_connect_refer_send__OI_2(requestID_3, pObjectBase);
                } else {
                    FsEbml * const rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                    *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "capture_gmtTime", FsEbmlNodeType_Time)->data.buffer = pFrame->capture_gmtTime;
                    *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "frameIndex", FsEbmlNodeType_Integer)->data.buffer = pFrame->index;
                    *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "width", FsEbmlNodeType_Integer)->data.buffer = pFrame->width[0];
                    *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "height", FsEbmlNodeType_Integer)->data.buffer = pFrame->height[0];
                    fs_Object_delete_pthreadSafety__OI(pFrame);
                    fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "snapshort_type", FsEbmlNodeType_String), "bmp");
                    {
                        char *bmpdData = (char*) fsMalloc(bmpLen);
                        image_dib_to_bmpData(pDib, (unsigned char*) bmpdData);
                        image_dib_delete__OI(&pDib);
                        fs_Ebml_node_data_set__OI_3(fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "data", FsEbmlNodeType_Binary), bmpLen, bmpdData);
                    }
#ifndef __record_P_cmd_snapshort_cb_do_to_pObjectBase_send 
                    FsObjectBase * pObjectBase;
                    if (1 == requestDataType) {
                        pObjectBase = fs_Ebml_to_ObjectBase__IO(rst, sizeof (FsObjectBase) + 8, 0, NULL);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0x3U, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    } else if (2 == requestDataType) {
                        FsXml * const pXml = fs_Ebml_convert_to_Xml__IO(rst);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase = fs_Xml_to_ObjectBase__IO(pXml, sizeof (FsObjectBase) + 8, 0, FsStringLenData("\r\n"), 0, NULL, 4);
                        fs_Xml_delete__OI(pXml, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    } else if (3 == requestDataType) {
                        FsJson * const pJson = fs_Ebml_convert_to_Json__IO(rst);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, 0, FsStringLenData("\r\n"), 0, NULL, 4);
                        fs_Json_delete__OI(pJson, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    } else {
                        FsPrintf(FsPrintfIndex, "Invalid requestDataType(=%hhd) data:\n", requestDataType);
                        fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
                        pObjectBase = fs_Ebml_to_ObjectBase__IO(rst, sizeof (FsObjectBase) + 8, 0, NULL);
                        fs_Ebml_delete__OI(rst, pShareBuffer);
                        pObjectBase->data -= 8;
                        ((unsigned int*) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0x3U, ((unsigned int*) pObjectBase->data)[1] = pObjectBase->len;
                        pObjectBase->len += 8;
                    }
                    configManager_connect_refer_send__OI_2(requestID_3, pObjectBase);
#endif
                }
            } else {
                fs_Object_delete_pthreadSafety__OI(pFrame);
            }
        }

    } else {

        /* 快照不存在 */
        configManager_conncet_refer_send_buffer(FsStringLenData("Get snapshort frame failed.\n"), requestID_3, (head & 0xFFFFFFF0U) | 0x6U, pObjectBaseBuffer);
    }
    return 1;
}
static FsConfig * record_P_protocol_snapshort_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "snapshort_get", "snapshort_get", "snapshort_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "snapshort_type", "快照类型", "快照类型", 0, 0x7, 0, 3, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "jpg", "jpg", "jpg");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "bmp", "bmp", "bmp");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 4, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "raw", "二进制裸数据", "二进制裸数据");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "ebml", "压缩数据", "压缩数据");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int record_P_cmd_cb_recordInfo(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Record * const pRecord
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //fs_Ebml_out_debug(pEbml, stdout), printf("\n");
    {
#ifndef __cmd_cb_cal_return_type 
        struct FsEbml_node * const return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (return_type && return_type->data.lenth > 0 && FsEbmlNodeType_is_String(return_type->type)) {
            if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
            else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
        }
#endif
    }
    struct ConfigManager_connectNode_useOnce * const pConnectNode = configManager_connectNode_useOnce_new__IO(requestID_3, head, requestDataType
            , 0, fs_Ebml_node_get_mask(pEbml, (struct FsEbml_node*) pEbml, "type"));
    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
    pConnectNode->next = (struct ConfigManager_connectNode_useOnce *) pRecord->p.pConnectNode__videoInfoDataClientList;
    pRecord->p.pConnectNode__videoInfoDataClientList = pConnectNode;
    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);

    return 1;
}

static FsConfig * record_P_protocol_recordInfo_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "recordInfo_get", "recordInfo_get", "recordInfo_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "record", "record", "record");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "type", "数据类型", "数据类型", FsConfig_nodeShowType_default, 0, 0x7, 1, 7, 3);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0x1, "基本数据", "基本数据");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x2, "外部储存配置", "外部储存配置");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}


/* 在期望连接的命令字和uuid改变时调用 */
static void record_P_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[]
        , /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p
        , /* 关联的对象 */ struct Record_item * const pRecord_item) {
    // FsPrintf(1, "cmd:\"%s\",uuid:\"%s\",%p ,%p,localPushCameraFun=%p/%p\n", cmd, uuid, do_local, do_local_p,   pRecord_item->p.localPushCameraFun,pRecord_item->p.pLocalPushCamera);
    if (strcmp("cameractrl", cmd) == 0 && strcmp(uuid, pRecord_item->ro._uuid) == 0) {
        if (pRecord_item->ro.__pGB28181Server_item) {
            //FsPrintf(1, "tttttttttt-%#lx,%#lx\n", (long) do_local, (long) do_local_p);
            if (do_local) {
                struct Capture_item * const pCapture_item = (struct Capture_item*) do_local;
                gb28181Server_item_set_ctrl(pRecord_item->ro.__pGB28181Server_item, pCapture_item->ro._ctrl_function, pCapture_item->ro.__pCamera_item, pCapture_item->ro.__pCamera_ctrl_item);
            } else gb28181Server_item_set_ctrl(pRecord_item->ro.__pGB28181Server_item, NULL, NULL, NULL);
        }
    } else {

        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pRecord_item->ro._uuid);
        fflush(stdout);
    }
}

/* 用于rtsp服务器在有实时客户端时的回调函数 */
static void record_private_cb_realRtsp(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, struct Record_item * const pRecord_item) {
    if (index < 0) {
        configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro._realMask, pRecord_item->ro._maskIndex_rtsp, index + 2);
        //FsPrintf(1, "-------------RtspSatus:[%d],uuid:\"%s\",localPush:\"%s\"-----\n", index + 2, pRecord_item->ro._uuid, pRecord_item->ro._localPush);
        //fflush(stdout);
        //configManager_mask_out_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, stdout);
    } else {
        FsPrintf(1, "-------------index=%d,uuid:\"%s\",localPush:\"%s\"-----\n", index, pRecord_item->ro._uuid
                , ""
                );
        fflush(stdout);
    }
}
/* 用于hls服务器在有实时客户端时的回调函数 */
static void record_P_cb_realHls(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, struct Record_item * const pRecord_item) {
    if (index < 0) {
        configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro._realMask, pRecord_item->ro._maskIndex_hls, index + 2);
        FsPrintf(1, "-------------HlsSatus:[%d],uuid:\"%s\",localPush:\"%s\"-----\n", index + 2, pRecord_item->ro._uuid
                , ""
                );
        fflush(stdout);
        //configManager_mask_out_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, stdout);
    } else {

        FsPrintf(1, "-------------index=%d,uuid:\"%s\",localPush:\"%s\"-----\n", index, pRecord_item->ro._uuid
                , ""
                );
        fflush(stdout);
    }
}

/* 用于gb28181服务器在有实时客户端时的回调函数 */
static void record_private_cb_realGB28181(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, struct Record_item * const pRecord_item) {
    if (index < 0) {
        configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro._realMask, pRecord_item->ro._maskIndex_gb28181, index + 2);
        FsPrintf(1, "-------------RtspSatus:[%d],uuid:\"%s\",localPush:\"%s\"-----\n", index + 2, pRecord_item->ro._uuid
                , ""
                );
        fflush(stdout);
        //configManager_mask_out_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, stdout);
    } else {

        FsPrintf(1, "-------------index=%d,uuid:\"%s\",localPush:\"%s\"-----\n", index, pRecord_item->ro._uuid
                , ""
                );
        fflush(stdout);
    }
}
/* 查询录像时长的回调函数,成功返回FsObjectList指针(成员为两个double,第一个double为开始时间,第二个double为结束时间),失败返回NULL */
static FsStructList * record_P_item_recordInfo_for_GB28181__IO(/* 查找的开始时间 */double startTime, /* 查找的结束时间 */double endTime, struct Record_item * const pRecord_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

static void record_P_new_sdk_gb28181(struct Record * const pRecord, FsConfig * const pConfig, const void *const vsys0, const void *const vsys, FsEbml * const gbsdkConfig) {
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

static void record_P_item_new_make_remoteStorageNode(FsEbml * const sdkConfig, struct FsEbml_node * const storageConfigNode, const FsString * const uuid
        , const FsString * const storageAddr) {
    {
        FsString * const pString = fs_String_new__IO(sizeof (Record_P_baseDir"/vsys/") - 1 + uuid->lenth - 1 + sizeof ("/video/remote/") - 1);
        memcpy(pString->buffer, FsStringDataLen(Record_P_baseDir"/vsys/"));
        memcpy(pString->buffer + sizeof (Record_P_baseDir"/vsys/") - 1, uuid->buffer, uuid->lenth - 1);
        memcpy(pString->buffer + sizeof (Record_P_baseDir"/vsys/") - 1 + uuid->lenth - 1, FsStringDataLen0("/video/remote/"));
        pString->lenth++;
        fs_Ebml_node_data_set__OI_2(
#ifdef Record_sdkConfig_comment
                fs_Ebml_node_addChild_with_a_property
#else
                fs_Ebml_node_addChild
#endif
                (sdkConfig, storageConfigNode, "base", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                , "comment", "录像储存的根路径,以/结尾"
#endif
                ), pString);
    }
    {

        fs_Ebml_node_data_set(
#ifdef Record_sdkConfig_comment
                fs_Ebml_node_addChild_with_a_property
#else
                fs_Ebml_node_addChild
#endif
                (sdkConfig, storageConfigNode, "url", FsEbmlNodeType_String
#ifdef Record_sdkConfig_comment
                , "comment", "连接地址"
#endif
                ), storageAddr->lenth, storageAddr->buffer);
    }
    //    {
    //#ifdef Record_dhsdkConfig_comment
    //        fs_Ebml_node_addChild_with_a_property
    //#else
    //        fs_Ebml_node_addChild
    //#endif
    //                (dhsdkConfig, storageConfigNode, "playback", FsEbmlNodeType_Struct
    //#ifdef Record_dhsdkConfig_comment
    //                , "comment", "回放任务列表"
    //#endif
    //                );
    //    }
}

static void record_P_item_new(struct Record * const pRecord, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的采集项 */struct Record_item * * const ppRecord_item, Rtsp * const pRtspServer
        , Hls * const pHls, GB28181 * const pGB28181
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pRecord->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, 0, ipList, 0 == channel, "vsys");
    if (clusterList) {
        void **ppNodeCluster = clusterList->pNode + clusterList->startIndex;
        unsigned int uj = clusterList->nodeCount, index = 0;
        ////////////////////////////////////////////////////////////////////////
        //////////////////////// 以下变量为原有item对应的成员变量值 //////////////
        /* 缓存原来的记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-在有直播请求时记录,3-大华sdk */
        unsigned char recordVideoModeLast = 0;
        /* 上一次录像的时间,小于0.00001表示未录像 */
        double videoLastTime = 0.0;
        /* 在录像开始时记录录像开始时间,在录像持续时间超过一定时间(如1min)后,再把录像开始时间写入记录,以避免快照或视频质量差导致录像频繁中断的问题,在记录开始时间后取原值的相反数 */
        double videoStartTime = 0.0;
        /* 在录像开始时记录录像开始时间(逻辑时间) */
        double videoStartTimeGmt = 0.0;
        /* 缓存原来的最新保存视频信息的文件,0-表示使用__info.txt,1-表示使用__inf0.txt */
        unsigned char videoInfo_file = 0;
        /* 缓存录像信息是否已加载,0-表示未加载,1-表示已加载 */
        unsigned char videoInfoHasLoad = 0;
        /* 缓存两个录像信息文件的时间 */
        double videoTimeEnd[2] = {0.0, 0.0};
        ////////////////////////////////////////////////////////////////////////
        do {
            const void *vsysChannel0 = vsys0;
            void *const vsys = *ppNodeCluster++;
            FsObjectList * const list = 0 == channel ? fs_Config_node_template_orderFirst__IO(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel")
                    : (FsObjectList *) fs_Config_node_template_get_orderFirst(pConfig, &vsysChannel0, vsys, 0, NULL, 0, "vsysChannel", channel - 1);
            if (list) {
                void ** ppNode;
                unsigned int ui, ipv4;
                if (0 == channel) {
                    ppNode = list->pNode + list->startIndex;
                    ui = list->nodeCount;
                } else ui = 1;
                struct Record_item **ppNodeRecord_item;
                if (0 == channel) {
                    if (NULL == pRecord->p.clusterItemList) pRecord->p.clusterItemList = fs_ObjectList_new__IO(4);
                    while (pRecord->p.clusterItemList->nodeCount < index + ui)fs_ObjectList_insert_tail(pRecord->p.clusterItemList, NULL);
                    ppNodeRecord_item = (struct Record_item**) pRecord->p.clusterItemList->pNode + pRecord->p.clusterItemList->startIndex;
                    {
                        const char *const hostname = fs_Config_node_string_get_first(pConfig, vsys0, vsys, "hostname", NULL);
                        const char *const pd = strchr(hostname, ':');
                        ipv4 = fs_ipv4_network_get(pd != NULL ? (unsigned int) (pd - hostname) : strlen(hostname), hostname);
                    }
                } else ipv4 = 0;
                do {
                    //FsPrintf(1, "channel=%u,parent=%#lx,list->nodeCount=%lu\n", channel, (long) parent, list->nodeCount);
                    void *vsysChannel, *sumNode;
                    if (0 == channel) {
                        sumNode = vsysChannel = *ppNode++;
                        if (ppNodeRecord_item[index] != NULL && ppNodeRecord_item[index]->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode)) {
                            index++;
                            continue;
                        }
                    } else {
                        sumNode = vsysChannel = list;
                        if (*ppRecord_item != NULL && (*ppRecord_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index], pShareBuffer);
                    } else if (*ppRecord_item) {
                        /* 缓存原来的记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-在有直播请求时记录,3-大华sdk */
                        recordVideoModeLast = (*ppRecord_item)->ro._recordVideoMode;
                        /* 上一次录像的时间,小于0.00001表示未录像 */
                        videoLastTime = (*ppRecord_item)->p.videoLastTime;
                        /* 在录像开始时记录录像开始时间,在录像持续时间超过一定时间(如1min)后,再把录像开始时间写入记录,以避免快照或视频质量差导致录像频繁中断的问题,在记录开始时间后取原值的相反数 */
                        videoStartTime = (*ppRecord_item)->p.videoStartTime;
                        /* 在录像开始时记录录像开始时间(逻辑时间) */
                        videoStartTimeGmt = (*ppRecord_item)->p.videoStartTimeGmt;
                        /* 缓存原来的最新保存视频信息的文件,0-表示使用__info.txt,1-表示使用__inf0.txt */
                        videoInfo_file = (*ppRecord_item)->p._videoInfo_file;
                        /* 缓存录像信息是否已加载,0-表示未加载,1-表示已加载 */
                        videoInfoHasLoad = (*ppRecord_item)->p.videoInfoHasLoad;
                        /* 缓存两个录像信息文件的时间 */
                        videoTimeEnd[0] = (*ppRecord_item)->p.videoTimeEnd[0], videoTimeEnd[1] = (*ppRecord_item)->p.videoTimeEnd[1];

                        record_P_item_delete__OI(*ppRecord_item, pShareBuffer);
                    }
                    struct Record_item * rst;
                    if (Record_Mask != 0 && (Record_Mask & fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL)) == 0)rst = NULL;
                    else {
                        //FsPrintf(1, "channel=%u,parent=%p,list->nodeCount=%lu\n", channel, parent, list->nodeCount);
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        //FsPrintf(1, "channel=%u,parent=%#lx\n", channel, (long) parent);
                        const unsigned long long streamMask = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "cameraConfig streamMask", NULL);
                        const void *recordConfig0 = vsysChannel0;
                        const void *const recordConfig = fs_Config_node_get_first(pConfig, &recordConfig0, vsysChannel, "recordConfig");
                        const char *const rtspServerURL = fs_Config_node_string_get_first(pConfig, recordConfig0, recordConfig, "rtspServerURL", NULL);
                        unsigned char recordVideoMode = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "recordVideoMode", 0, NULL);
                        const FsString * storageAddr;
                        if (3 == recordVideoMode) {
                            storageAddr = fs_Config_node_string_get_first_String(pConfig, recordConfig0, recordConfig, "storageAddr", NULL);
                            if (storageAddr) {
                                const char *const key[] = {"d"/* "dhsdk" */, "g" /* "gb28181" */};
                                const unsigned char v[] = {3, 4};
                                unsigned int _ui = 0, _uj = Fs_Array_len(key) - 1;
                                for (;;) {
                                    const unsigned int _uk = (_ui + _uj) >> 1;
                                    const int _rv = FsCompareCaseInsensitive(storageAddr->buffer, key[_uk], 1);
                                    if (_rv > 0) {
                                        _ui = _uk + 1;
                                        if (_ui > _uj) {
                                            FsLog(FsLogType_error, FsPrintfIndex, "Unkown storageAddr:\"%s\".\n", storageAddr->buffer);
                                            fflush(stdout);
                                            recordVideoMode = 0;
                                            break;
                                        }
                                    } else if (_rv < 0) {
                                        if (_uk > _ui)_uj = _uk - 1;
                                        else {
                                            FsLog(FsLogType_error, FsPrintfIndex, "Unkown storageAddr:\"%s\".\n", storageAddr->buffer);
                                            fflush(stdout);
                                            recordVideoMode = 0;
                                            break;
                                        }
                                    } else {
                                        recordVideoMode = v[_uk];
                                        break;
                                    }
                                }
                            } else recordVideoMode = 0;
                        } else if (-1 == recordVideoMode)recordVideoMode = 15;
                        const FsString *deviceno, *devicename, *positionname, *position_jwd;
                        char *pd;
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            deviceno = fs_Config_node_string_get_first_String(pConfig, recordConfig0, recordConfig, "deviceno", NULL);
                            len += deviceno->lenth;
                            devicename = fs_Config_node_string_get_first_String(pConfig, recordConfig0, recordConfig, "devicename", NULL);
                            len += devicename->lenth;
                            positionname = fs_Config_node_string_get_first_String(pConfig, recordConfig0, recordConfig, "positionname", NULL);
                            len += positionname->lenth;
                            position_jwd = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "positon center", NULL);
                            if (position_jwd) len += position_jwd->lenth;
                            else len += sizeof ("0,0");

                            rst = (struct Record_item*) fsMalloc(sizeof (struct Record_item) +len);
                            memset(rst, 0, sizeof (struct Record_item));
                            pd = (char*) (rst + 1);
                        } else {
                            position_jwd = NULL; //fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "positon center", NULL);
                            rst = (struct Record_item*) fsMalloc(Record_item_ClusterLen + len);
                            memset(rst, 0, Record_item_ClusterLen);
                            pd = ((char*) rst) + Record_item_ClusterLen;
                        }
                        /* 本对象的uuid值,一般是从配置中读入的 */
                        rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len), pd += len;
                        /* 所属主机的ip地址,用于集群,本机为0 */
                        rst->ro._ipv4 = ipv4;
                        /* 所属的记录对象 */
                        rst->ro._pRecord = pRecord;
                        /* 视频转发+历史点播+集群 */
                        if (rtspServerURL) {
                            if (channel > 0) {
                                /* 注册rtsp实时视频掩码通信 */
                                rst->ro._maskIndex_rtsp = configManager_mask_register_pthreadSafety(&rst->ro._realMask, pRecord->ro._pConfigManager, "realStream", uuid->buffer, Record_BaseLine + __LINE__);
                                rst->ro.__pRtspServer_item = rtspServer_item_new__IO(rtspServerURL, ipv4, Record_item_is_localVideoStorage(recordVideoMode)
                                        || (Record_item_is_remoteVideoStorage(recordVideoMode)&&(((ConfigManager*) pRecord->ro._pConfigManager)->ro._mask & (1LLU << 20))) ? 3 : 1, 0, NULL, NULL, NULL
                                        , (void (* const) (const int, void * const))record_private_cb_realRtsp, rst, 0x7, NULL, 0);
                                /* 读取推送信息 */
                                {
                                    FsConfigResult_string * const pArray = fs_Config_node_string_get__IO(pConfig, recordConfig0, recordConfig, "rtspPushURL", NULL);
                                    if (pArray) {
                                        /* 读取推送的协议 */
                                        const unsigned int rtspPushProtocol = fs_Config_node_integer_get_mask(pConfig, recordConfig0, recordConfig, "rtspPushProtocol", NULL);
                                        unsigned int _i = 0;
                                        for (; _i < pArray->count; _i++) {
                                            struct RtspServer_item_push * const pRtspServer_item_push = rtspServer_item_push_new_byURL__IO(pArray->data[_i], rtspPushProtocol);
                                            if (pRtspServer_item_push) rtspServer_item_add_push__OI_3(NULL, rst->ro.__pRtspServer_item, pRtspServer_item_push);
                                            else {
                                                FsLog(FsLogType_error, FsPrintfIndex, "Invalid url:\"%s\",count=%u/%u,channel=%u.\n", pArray->data[_i], pArray->count, _i, channel);
                                                fflush(stdout);
                                            }
                                        }
                                        fsFree(pArray);
                                    }
                                }

                            } else rst->ro.__pRtspServer_item = rtspServer_item_new__IO(rtspServerURL, ipv4, Record_item_is_localVideoStorage(recordVideoMode)
                                    || (Record_item_is_remoteVideoStorage(recordVideoMode)&&(((ConfigManager*) pRecord->ro._pConfigManager)->ro._mask & (1LLU << 20))) ? 3 : 1, 0, NULL, NULL, NULL
                                    , NULL, NULL, 0x0, NULL, 0);
                            rtspServer_add_item__OI_2(pRtspServer, rst->ro.__pRtspServer_item);
                            /* hls视频转发+集群 */
                            if (channel > 0) {
                                /* 注册hls实时视频掩码通信 */
                                rst->ro._maskIndex_hls = configManager_mask_register_pthreadSafety(&rst->ro._realMask, pRecord->ro._pConfigManager, "realStream", uuid->buffer, Record_BaseLine + __LINE__);
                                rst->ro.__pHlsServer_item = hlsServer_item_new__IO(rtspServerURL, ipv4, Record_item_is_localVideoStorage(recordVideoMode)
                                        || (Record_item_is_remoteVideoStorage(recordVideoMode)&&(((ConfigManager*) pRecord->ro._pConfigManager)->ro._mask & (1LLU << 21))) ? 3 : 1
                                        , (void (*const) (const int, void*))record_P_cb_realHls, (FsObjectImageFrame * (* const) (void * const))record_item_snap_get_pthreadSafety__IO
                                        , (FsEbml * (* const) (void * const, char * * const))record_P_item_recordInfo_get_pthreadSafety__IO, rst);
                            } else rst->ro.__pHlsServer_item = hlsServer_item_new__IO(rtspServerURL, ipv4, Record_item_is_localVideoStorage(recordVideoMode)
                                    || (Record_item_is_remoteVideoStorage(recordVideoMode)&&(((ConfigManager*) pRecord->ro._pConfigManager)->ro._mask & (1LLU << 21))) ? 3 : 1, NULL, NULL, NULL, NULL);
                            hlsServer_add_item__OI_2(pHls, rst->ro.__pHlsServer_item);
                        }
                        /* gb28181视频转发+集群 */
                        {
                            const char * const gb28181ID = fs_Config_node_string_get_first(pConfig, recordConfig0, recordConfig, "gb28181ID", NULL);
                            if (gb28181ID) {
                                unsigned long long gb28181ControlMask;
                                if (streamMask & 0x4) gb28181ControlMask = fs_Config_node_integer_get_mask(pConfig, recordConfig0, recordConfig, "gb28181ControlMask", NULL);
                                else gb28181ControlMask = 0;
                                if (channel > 0) {
                                    /* 注册gb28181实时视频掩码通信 */
                                    rst->ro._maskIndex_gb28181 = configManager_mask_register_pthreadSafety(&rst->ro._realMask, pRecord->ro._pConfigManager, "realStream", uuid->buffer, Record_BaseLine + __LINE__);
                                    const char * const gb28181Name = fs_Config_node_string_get_first(pConfig, recordConfig0, recordConfig, "gb28181Name", NULL);
                                    const char * const groupParent = fs_Config_node_string_get_first(pConfig, recordConfig0, recordConfig, "groupParent", NULL);
                                    float longitude, latitude;
                                    if (NULL == position_jwd || sscanf(position_jwd->buffer, "%f,%f", &longitude, &latitude) != 2) {
                                        longitude = 0.0;
                                        latitude = 0.0;
                                    }
                                    rst->ro.__pGB28181Server_item = gb28181Server_item_new__IO(gb28181ID, gb28181Name, groupParent, ipv4, Record_item_is_localVideoStorage(recordVideoMode)
                                            || (Record_item_is_remoteVideoStorage(recordVideoMode)&&(((ConfigManager*) pRecord->ro._pConfigManager)->ro._mask & (1LLU << 20))) ? 3 : 1, gb28181ControlMask
                                            , fs_Config_node_integer_get_mask(pConfig, recordConfig0, recordConfig, "gb28181Extern", NULL), longitude, latitude
                                            , NULL
                                            , (void(* const) (const int, void * const))record_private_cb_realGB28181, rst
                                            , (FsStructList * (*) (double, double, void *, char * * const))record_P_item_recordInfo_for_GB28181__IO, rst);
                                } else rst->ro.__pGB28181Server_item = gb28181Server_item_new__IO(gb28181ID, NULL, NULL, ipv4, Record_item_is_localVideoStorage(recordVideoMode)
                                        || (Record_item_is_remoteVideoStorage(recordVideoMode)&&(((ConfigManager*) pRecord->ro._pConfigManager)->ro._mask & (1LLU << 20))) ? 3 : 1, gb28181ControlMask, 0, 0.0, 0.0, NULL, NULL, NULL, NULL, NULL);
                                gb28181Server_add_item__OI_2(pGB28181, rst->ro.__pGB28181Server_item);
                            }
                        }
                        /* 整个通道依赖的所有数据的校验和 */
                        rst->ro._sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode);
                        if (channel > 0) {
                            /* 当有数据可输出时写的字节,使用Fs_GroupSqrtOut_value_set宏写此字节 */
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pRecord->ro._pGroupSqrtOut->groupSqrt_member, pRecord->ro._pGroupSqrtOut->groupSqrt_group
                                    , pRecord->ro._pGroupSqrtOut->groupValue, channel - 1, &pRecord->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        ////////////////////////////////////////////////////////
                        if (channel > 0) {
                            /* 连接和调用外部命令所需要的同步锁 */
                            pthread_mutex_init(&rst->ro.__mutexCmdConnect, NULL);
                            /* 视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-在有直播请求时记录,3-大华sdk,4-GB28181 */
                            rst->ro._recordVideoMode = recordVideoMode;
                            /* 数据记录类型,0-ebml数据,1-xml数据,2-xml含注释数据,3-json数据 */
                            rst->ro._recordType = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "recordType", 2, NULL);
                            if (0) {
                                /* 测试掩码通信的能力 */
                                int count = 300000, i;
                                double t1 = fs_time_uptime_get();
                                char out = 0;
                                unsigned int *pIndex = (unsigned int*) fsMalloc(sizeof (unsigned int)*count);
                                if (out) {
                                    FsPrintf(1, "1111111111111-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                for (i = 0; i < count; i++) {
                                    pIndex[i] = configManager_mask_register_pthreadSafety(&rst->ro._realMask, pRecord->ro._pConfigManager, "realStream", uuid->buffer, Record_BaseLine + __LINE__);
                                    // FsPrintf(1, "[%d]-regiter index=%u\n", i, pIndex[i]);
                                }
                                if (out) {
                                    FsPrintf(1, "22222222222222-add-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                for (i = count / 5 * 1; i < count / 5 * 4; i++) {
                                    configManager_mask_value_set_pthreadSafety(pRecord->ro._pConfigManager, rst->ro._realMask, pIndex[i], 1);
                                    //FsPrintf(1, "[%d]-set value 1 index=%u\n", i, pIndex[i]);
                                }
                                if (out) {
                                    FsPrintf(1, "33333333333333-set 1-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                for (i = count / 5 * 2; i < count / 7 * 5; i++) {
                                    configManager_mask_value_set_pthreadSafety(pRecord->ro._pConfigManager, rst->ro._realMask, pIndex[i], 0);
                                    // FsPrintf(1, "[%d]-set value 0 index=%u\n", i, pIndex[i]);
                                }
                                if (out) {
                                    FsPrintf(1, "44444444444444-set 0-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                for (i = count / 3; i < count / 3 * 2; i++) {
                                    configManager_mask_logoff_pthreadSafety(pRecord->ro._pConfigManager, "realStream", uuid->buffer, pIndex[i]);
                                    //FsPrintf(1, "[%d]-logoff index=%u\n", i, pIndex[i]);
                                }
                                if (out) {
                                    FsPrintf(1, "5555555555-logoff 1-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                for (i = 0; i < count / 3; i++) {
                                    configManager_mask_logoff_pthreadSafety(pRecord->ro._pConfigManager, "realStream", uuid->buffer, pIndex[i]);
                                    //FsPrintf(1, "[%d]-logoff index=%u\n", i, pIndex[i]);
                                }
                                if (out) {
                                    FsPrintf(1, "6666666666-logoff 2-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                for (i = count / 3 * 2; i < count; i++) {
                                    configManager_mask_logoff_pthreadSafety(pRecord->ro._pConfigManager, "realStream", uuid->buffer, pIndex[i]);
                                    // FsPrintf(1, "[%d]-logoff index=%u\n", i, pIndex[i]);
                                }
                                if (out) {
                                    FsPrintf(1, "77777777777-logoff 2-mask info.\n");
                                    configManager_mask_out_pthreadSafety(pRecord->ro._pConfigManager, stdout);
                                    fflush(stdout);
                                }
                                fsFree(pIndex);
                                FsPrintf(1, "Test mask(count=%d) use time:%lf.\n", count, fs_time_uptime_get() - t1);
                                fflush(stdout);
                            }

                            /* 录像的掩码 */
                            rst->ro._maskIndex_record = configManager_mask_register_pthreadSafety(&rst->ro._realMask, pRecord->ro._pConfigManager, "realStream", uuid->buffer, Record_BaseLine + __LINE__);
                            if (Record_item_is_pullStream(rst->ro._recordVideoMode)) {
                                configManager_mask_value_set_pthreadSafety(pRecord->ro._pConfigManager, rst->ro._realMask, rst->ro._maskIndex_record, 1);
                            }
                            /* 实时视频保存的最长时间,单位秒,0表示不限制 */
                            if (rst->ro._recordVideoMode) rst->ro._realVideoSaveTime = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "realVideoSaveTime", 0, NULL);
                            else rst->ro._realVideoSaveTime = 1;
                            /* 外部储存的节点,为空表示不使用外部储存,比如_recordVideoMode为3时,此值指向pRecord->p.dhsdkConfig__videoInfoDataClientList中的节点,删除时需要同步删除 */
                            if (Record_item_is_remoteVideoStorage(recordVideoMode)) {
                                switch (recordVideoMode) {
                                    case /* 3-大华sdk */3:
                                    {

                                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        FsEbml * dhsdkConfig = pRecord->p.dhsdkConfig__videoInfoDataClientList;
                                        if (NULL == dhsdkConfig) {
                                            dhsdkConfig = pRecord->p.dhsdkConfig__videoInfoDataClientList = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                                        }
                                        struct FsEbml_node * const storageConfigNode = fs_Ebml_node_addChild(dhsdkConfig, (struct FsEbml_node*) pRecord->p.dhsdkConfig__videoInfoDataClientList, "item", FsEbmlNodeType_Struct);
                                        /* 外部储存的节点(Record_item_is_remoteVideoStorage(recordVideoMode)为真时有效),为空表示不使用外部储存,比如_recordVideoMode为3时,此值指向pRecord->p.dhsdkConfig__videoInfoDataClientList中的节点,删除时需要同步删除 */
                                        rst->ro.__storageConfigNode = storageConfigNode;
                                        record_P_item_new_make_remoteStorageNode(dhsdkConfig, storageConfigNode, uuid, storageAddr);
                                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        Record_sdkConfigUpdate_set_01_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_dh);
                                    }
                                        break;
                                    case /* 4-GB28181 */4:
                                    {
                                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        FsEbml * gb28181Config = pRecord->p.gbsdkConfig__videoInfoDataClientList;
                                        if (NULL == gb28181Config) {
                                            gb28181Config = pRecord->p.gbsdkConfig__videoInfoDataClientList = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                                            record_P_new_sdk_gb28181(pRecord, pConfig, vsys0, vsys, gb28181Config);
                                        }
                                        struct FsEbml_node * const storageConfigNode = fs_Ebml_node_addChild(gb28181Config, (struct FsEbml_node*) pRecord->p.gbsdkConfig__videoInfoDataClientList, "item", FsEbmlNodeType_Struct);
                                        /* 外部储存的节点(Record_item_is_remoteVideoStorage(recordVideoMode)为真时有效),为空表示不使用外部储存,比如_recordVideoMode为3时,此值指向pRecord->p.dhsdkConfig__videoInfoDataClientList中的节点,删除时需要同步删除 */
                                        rst->ro.__storageConfigNode = storageConfigNode;
                                        record_P_item_new_make_remoteStorageNode(gb28181Config, storageConfigNode, uuid, storageAddr);
                                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        Record_sdkConfigUpdate_set_01_1(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                                    }
                                        break;
                                    default:
                                    {
                                        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid recordVideoMode(=%hhu),channel=%u.\n", recordVideoMode, channel);
                                        fflush(stdout);
                                        FsLogExit();
                                    }
                                        break;
                                }
                            }
                            /* 设备编号 */
                            rst->ro._deviceno = pd, len = deviceno->lenth, memcpy(pd, deviceno->buffer, len), pd += len;
                            /* 设备名 */
                            rst->ro._devicename = pd, len = devicename->lenth, memcpy(pd, devicename->buffer, len), pd += len;
                            /* 安装位置 */
                            rst->ro._positionname = pd, len = positionname->lenth, memcpy(pd, positionname->buffer, len), pd += len;
                            /* 经纬度位置,经度与纬度之间用逗号隔开如 10.1,20.3 */
                            if (position_jwd) rst->ro._position_jwd = pd, len = position_jwd->lenth, memcpy(pd, position_jwd->buffer, len)/* , pd += len */;
                            else rst->ro._position_jwd = pd, len = strlen("0,0") + 1, memcpy(pd, "0,0", len)/*, fsFree(position_jwd) , pd += len */;
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 视频缓存链表 */
                            rst->ro.__videoCacheList = fs_ObjectList_new__IO(10);
                            /* 视频合成任务链表,任务项按结束时间排序 */
                            rst->ro.__videoPartList = fs_ObjectList_new__IO(4);
                            /* 卡口任务链表,储存struct Record_item_Watch指针 */
                            rst->ro.__watchList = fs_ObjectList_new__IO(4);
                            /* 事件任务链表,储存struct Record_item_Inc指针 */
                            rst->ro.__incList = fs_ObjectList_new__IO(4);
                            /* 录像信息在内存中保存的位置指针 */
                            rst->ro._videoInfo_pos = fs_Ebml_node_get_first(pRecord->p.videoInfoData__videoInfoDataClientList, (struct FsEbml_node*) pRecord->p.videoInfoData__videoInfoDataClientList, "videoInfo")->child[/* 从1开始 */channel];
                            /* 录像文件的周期,单位秒 */
                            rst->ro._recordVideoFileInterval = 60;
                            /* 录像信息更新,为0表示更检查并把值置为videoRecordCheckInterval0,否则减1 */
                            rst->rw._videoRecordCheckInterval = FsNumberMax(rst->rw._videoRecordCheckInterval);
                            {
                                /* 更新uuid及录像时长 */
                                struct FsEbml_node * const videoInfo_pos = rst->ro._videoInfo_pos;
                                unsigned char change = 0;
#define __Record_item_check_uuid_and_realVideoSaveTime_pRecord_item rst
#ifndef __Record_item_check_uuid_and_realVideoSaveTime 
                                if (videoInfo_pos->child[0]->data.lenth < 1 || strcmp(__Record_item_check_uuid_and_realVideoSaveTime_pRecord_item->ro._uuid, videoInfo_pos->child[0]->data.buffer) != 0) {
                                    /* 通道发生改变 */
                                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    fs_Ebml_node_data_set_string(videoInfo_pos->child[0], __Record_item_check_uuid_and_realVideoSaveTime_pRecord_item->ro._uuid);
                                    *(unsigned long long*) videoInfo_pos->child[1]->data.buffer = __Record_item_check_uuid_and_realVideoSaveTime_pRecord_item->ro._realVideoSaveTime;
                                    if (videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount)
                                        fs_Ebml_node_delete_child_n(NULL, videoInfo_pos, Record_P_videoInfo_HeadCount, videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    change = 1;
                                } else if (*(unsigned long long*) videoInfo_pos->child[1]->data.buffer != __Record_item_check_uuid_and_realVideoSaveTime_pRecord_item->ro._realVideoSaveTime) {
                                    /* 录像最长记录时间发生改变 */
                                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    *(unsigned long long*) videoInfo_pos->child[1]->data.buffer = __Record_item_check_uuid_and_realVideoSaveTime_pRecord_item->ro._realVideoSaveTime;
                                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                    change = 1;
                                }
#endif
                                if (Record_item_is_localVideoStorage(recordVideoMode)) {
                                    /* 本地录像 */
                                    if (videoInfoHasLoad && Record_item_is_localVideoStorage(recordVideoModeLast)) {
                                        /* 前后两次都是本地录像且之前的录像信息是从硬盘加载过的 */
                                        /* 如uuid及录像时长没有变化,则认为录像信息不用重新加载 */
                                        if (0 == change) {
                                            /* 上一次录像的时间,小于0.00001表示未录像 */
                                            rst->p.videoLastTime = videoLastTime;
                                            /* 在录像开始时记录录像开始时间,在录像持续时间超过一定时间(如1min)后,再把录像开始时间写入记录,以避免快照或视频质量差导致录像频繁中断的问题,在记录开始时间后取原值的相反数 */
                                            rst->p.videoStartTime = videoStartTime;
                                            /* 在录像开始时记录录像开始时间(逻辑时间) */
                                            rst->p.videoStartTimeGmt = videoStartTimeGmt;
                                            /* 没有变化,标记其已完成加载 */
                                            rst->p.videoInfoHasLoad = videoInfoHasLoad;
                                            /* 最新保存视频信息的文件(Record_item_is_localVideoStorage(pRecord_item)为真时有效),0-表示使用__info.txt,1-表示使用__inf0.txt */
                                            rst->p._videoInfo_file = videoInfo_file;
                                            /* 两个录像信息文件的结束时间(Record_item_is_localVideoStorage(recordVideoMode)为真且videoInfoHasLoad为1时有效) */
                                            rst->p.videoTimeEnd[0] = videoTimeEnd[0], rst->p.videoTimeEnd[1] = videoTimeEnd[1];
                                        } else pRecord->rw.videoInfoChange = 1;
                                    } else if (videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
                                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        fs_Ebml_node_delete_child_n(NULL, videoInfo_pos, Record_P_videoInfo_HeadCount, videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        pRecord->rw.videoInfoChange = 1;
                                    }
                                } else if (Record_item_is_remoteVideoStorage(recordVideoMode)) {
                                    /* 第三方储存 */
                                    if (videoInfoHasLoad && recordVideoMode == recordVideoModeLast) {
                                        /* 前后两次都是相同的第三方储存且之前的录像信息是从硬盘加载过的 */
                                        /* 如uuid及录像时长没有变化,则认为录像信息不用重新加载 */
                                        if (0 == change) {
                                            /* 没有变化,标记其已完成加载 */
                                            rst->p.videoInfoHasLoad = 1;
                                        } else pRecord->rw.videoInfoChange = 1;
                                    } else if (videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
                                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        fs_Ebml_node_delete_child_n(NULL, videoInfo_pos, Record_P_videoInfo_HeadCount, videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        pRecord->rw.videoInfoChange = 1;
                                    }
                                } else {
                                    /* 不记录 */
                                    if (videoInfo_pos->childNodeCount > Record_P_videoInfo_HeadCount) {
                                        pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        fs_Ebml_node_delete_child_n(NULL, videoInfo_pos, Record_P_videoInfo_HeadCount, videoInfo_pos->childNodeCount - Record_P_videoInfo_HeadCount);
                                        pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                        change = 1;
                                    }
                                    if (change)pRecord->rw.videoInfoChange = 1;
                                }
                            }
                        }
                        ////////////////////////////////////////////////////////////////////////////
                        /* 注册命令字 */
                        configManager_cmd_register(pRecord->ro._pConfigManager, "video_part", rst->ro._uuid, ipv4, rst, 0, 0 == ipv4 ? record_P_cmd_cb : NULL, 0 == ipv4 ? (void*) record_item_add_partVideo_pthreadSafety : NULL, rst, pShareBuffer);
                        if (0 == ipv4)configManager_cmd_register(pRecord->ro._pConfigManager, "record_watch", rst->ro._uuid, 0, rst, 0, NULL, (void*) record_item_add_watch_pthreadSafety__OI_2, rst, pShareBuffer);
                        if (0 == ipv4)configManager_cmd_register(pRecord->ro._pConfigManager, "record_inc", rst->ro._uuid, 0, rst, 0, NULL, (void*) record_item_add_inc_pthreadSafety__OI_2, rst, pShareBuffer);
                        configManager_cmd_register(pRecord->ro._pConfigManager, "videolist_get", rst->ro._uuid, ipv4, rst, 0, 0 == ipv4 ? record_P_cmd_cb : NULL, NULL, rst, pShareBuffer);
                        configManager_cmd_register(pRecord->ro._pConfigManager, "videotimeinfo_get", rst->ro._uuid, ipv4, rst, 0, 0 == ipv4 ? record_P_cmd_cb : NULL, NULL, rst, pShareBuffer);
                        configManager_cmd_register(pRecord->ro._pConfigManager, "snapshort_get", rst->ro._uuid, ipv4, rst, 0, 0 == ipv4 ?
                                (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))record_P_cmd_snapshort_cb : NULL, NULL, rst, pShareBuffer);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        if (0 == ipv4) configManager_cmd_connect(pRecord->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))record_P_cmd_connect_cb, rst);
                        ////////////////////////////////////////////////////////////////////////////
                    }
                    if (channel > 0)*ppRecord_item = rst;
                    else ppNodeRecord_item[index++] = rst;
                    ////////////////////////////////////////////////////////////////////////////
                    /* 释放内存空间 */
                } while (--ui > 0 && 0 == channel);
                if (0 == channel) fs_ObjectList_delete__OI(list);
            }
        } while (--uj > 0 && 0 == channel);
        fs_ObjectList_delete__OI(clusterList);
        if (0 == channel && pRecord->p.clusterItemList) {
            if (0 == index) {
                fs_ObjectList_delete_custom2__OI(pRecord->p.clusterItemList, (void (*)(void*, void *))record_P_item_delete__OI, (void*) pShareBuffer);
                pRecord->p.clusterItemList = NULL;
            } else {
                while (pRecord->p.clusterItemList->nodeCount > index)record_P_item_delete__OI((struct Record_item*) fs_ObjectList_remove_tail(pRecord->p.clusterItemList), pShareBuffer);
            }
        }
    }
}

/* 缓存视频信息 */
static void record_private_item_addVideoBuffer(/* 记录项,保证有一帧数据可用 */struct Record_item *pRecord_item, /* 帧号 */const unsigned long frameIndex, /* 采集时间,用系统开机时间 */const double captureTime_uptime,
        /* 数据帧类型,0表示普通数据类型,1表示关键帧,对jpeg来说都是关键帧 */const char keyFrame,
        /* 数据类型,1表示jpeg,2表示H264,3表示H265 */const char type, /* 数据帧所在的文件句柄 */FILE *fd, /* 数据帧在文件内的偏移量 */const unsigned long offset,
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
        pItem = (struct Record_private_VideoBuffer_item*) fsMalloc(sizeof (struct Record_private_VideoBuffer_item));
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
static void record_P_item_do_videoRecord(/* 记录项,保证有一帧数据可用 */struct Record_item * const pRecord_item, FsObjectImageFrame const *const pFrame
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
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
    pMonitorItem->rw.line = __LINE__;
    unsigned int frameLenth;
    long long offset;
    time_t ts;
    struct tm ti;
    FsLocal_ShareBuffer_resize(strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/00000000/08/video20151204080000.mkv"));
    if (ImageFrame_valid_H265_0(pFrame->dataValidType)) {
#define __record_P_item_do_videoRecord_do_size 0
#define __record_P_item_do_videoRecord_do_h265
#ifndef __record_P_item_do_videoRecord_do 
#ifdef __record_P_item_do_videoRecord_do_h265
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH265 * const *pData = pFrame->data.h265[__record_P_item_do_videoRecord_do_size];
#elif defined __record_P_item_do_videoRecord_do_h264
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH264 * const *pData = pFrame->data.h264[__record_P_item_do_videoRecord_do_size];
#else
#error "error"
#endif
        for (; NULL != *pData; pData++) {
            /* 完成一次记录片段,1分钟 */
            if (3 == pData[0]->keyFrame) {
                if (((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->p.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                /* 创建新文件片段,为mkv文件写入头信息 */
                if (NULL == pRecord_item->p._recordVideoFile_fd) {
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p.recordVideoFile_startTime = ((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pData[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir
                            , pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p._recordVideoFile_fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "w+");
                    pMonitorItem->rw.line = __LINE__;
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno));
                        fflush(stdout);
                    } else {
#ifdef __record_P_item_do_videoRecord_do_h265
                        char *const h265CodecPrivate = video_mkv_codecPrivate_h265_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 3,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h265CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h265CodecPrivate);
#elif defined __record_P_item_do_videoRecord_do_h264
                        char *const h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h264CodecPrivate);
#else
#error "error"
#endif
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
            /* 继续写当前文件，写入帧信息 */
            if (pRecord_item->p._recordVideoFile_fd) {
                frameLenth = pData[0]->datalenth;
                pMonitorItem->rw.line = __LINE__;
                offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pData[0]->keyFrame & 0x1, &frameLenth, pData[0]->data);
                pMonitorItem->rw.line = __LINE__;
                if (offset < 1) {
                    Record_itemLog(FsLogType_fatal, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pData[0]->keyFrame, pData[0]->datalenth);
                    fflush(stdout);
                    FsLogExit();
                } else {
                    pMonitorItem->rw.line = __LINE__;
                    record_private_item_addVideoBuffer(pRecord_item, pData[0]->frameIndex, pData[0]->capture_uptime, pData[0]->keyFrame & 0x1
#if defined __record_P_item_do_videoRecord_do_h265
                            , 3
#elif defined __record_P_item_do_videoRecord_do_h264
                            , 2
#else
#error "error"
#endif
                            , pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pData[0]->width, pData[0]->height, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
#undef __record_P_item_do_videoRecord_do_size
#ifdef __record_P_item_do_videoRecord_do_h265
#undef __record_P_item_do_videoRecord_do_h265
#endif
#ifdef __record_P_item_do_videoRecord_do_h264
#undef __record_P_item_do_videoRecord_do_h264
#endif
#endif
    } else if (ImageFrame_valid_H264_0(pFrame->dataValidType)) {
#define __record_P_item_do_videoRecord_do_size 0
#define __record_P_item_do_videoRecord_do_h264
#ifndef __record_P_item_do_videoRecord_do 
#ifdef __record_P_item_do_videoRecord_do_h265
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH265 * const *pData = pFrame->data.h265[__record_P_item_do_videoRecord_do_size];
#elif defined __record_P_item_do_videoRecord_do_h264
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH264 * const *pData = pFrame->data.h264[__record_P_item_do_videoRecord_do_size];
#else
#error "error"
#endif
        for (; NULL != *pData; pData++) {
            /* 完成一次记录片段,1分钟 */
            if (3 == pData[0]->keyFrame) {
                if (((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->p.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                /* 创建新文件片段,为mkv文件写入头信息 */
                if (NULL == pRecord_item->p._recordVideoFile_fd) {
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p.recordVideoFile_startTime = ((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pData[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir
                            , pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p._recordVideoFile_fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "w+");
                    pMonitorItem->rw.line = __LINE__;
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno));
                        fflush(stdout);
                    } else {
#ifdef __record_P_item_do_videoRecord_do_h265
                        char *const h265CodecPrivate = video_mkv_codecPrivate_h265_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 3,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h265CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h265CodecPrivate);
#elif defined __record_P_item_do_videoRecord_do_h264
                        char *const h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h264CodecPrivate);
#else
#error "error"
#endif
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
            /* 继续写当前文件，写入帧信息 */
            if (pRecord_item->p._recordVideoFile_fd) {
                frameLenth = pData[0]->datalenth;
                pMonitorItem->rw.line = __LINE__;
                offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pData[0]->keyFrame & 0x1, &frameLenth, pData[0]->data);
                pMonitorItem->rw.line = __LINE__;
                if (offset < 1) {
                    Record_itemLog(FsLogType_fatal, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pData[0]->keyFrame, pData[0]->datalenth);
                    fflush(stdout);
                    FsLogExit();
                } else {
                    pMonitorItem->rw.line = __LINE__;
                    record_private_item_addVideoBuffer(pRecord_item, pData[0]->frameIndex, pData[0]->capture_uptime, pData[0]->keyFrame & 0x1
#if defined __record_P_item_do_videoRecord_do_h265
                            , 3
#elif defined __record_P_item_do_videoRecord_do_h264
                            , 2
#else
#error "error"
#endif
                            , pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pData[0]->width, pData[0]->height, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
#undef __record_P_item_do_videoRecord_do_size
#ifdef __record_P_item_do_videoRecord_do_h265
#undef __record_P_item_do_videoRecord_do_h265
#endif
#ifdef __record_P_item_do_videoRecord_do_h264
#undef __record_P_item_do_videoRecord_do_h264
#endif
#endif
    } else if (ImageFrame_valid_H265_1(pFrame->dataValidType)) {
#define __record_P_item_do_videoRecord_do_size 1
#define __record_P_item_do_videoRecord_do_h265
#ifndef __record_P_item_do_videoRecord_do 
#ifdef __record_P_item_do_videoRecord_do_h265
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH265 * const *pData = pFrame->data.h265[__record_P_item_do_videoRecord_do_size];
#elif defined __record_P_item_do_videoRecord_do_h264
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH264 * const *pData = pFrame->data.h264[__record_P_item_do_videoRecord_do_size];
#else
#error "error"
#endif
        for (; NULL != *pData; pData++) {
            /* 完成一次记录片段,1分钟 */
            if (3 == pData[0]->keyFrame) {
                if (((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->p.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                /* 创建新文件片段,为mkv文件写入头信息 */
                if (NULL == pRecord_item->p._recordVideoFile_fd) {
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p.recordVideoFile_startTime = ((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pData[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir
                            , pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p._recordVideoFile_fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "w+");
                    pMonitorItem->rw.line = __LINE__;
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno));
                        fflush(stdout);
                    } else {
#ifdef __record_P_item_do_videoRecord_do_h265
                        char *const h265CodecPrivate = video_mkv_codecPrivate_h265_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 3,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h265CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h265CodecPrivate);
#elif defined __record_P_item_do_videoRecord_do_h264
                        char *const h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h264CodecPrivate);
#else
#error "error"
#endif
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
            /* 继续写当前文件，写入帧信息 */
            if (pRecord_item->p._recordVideoFile_fd) {
                frameLenth = pData[0]->datalenth;
                pMonitorItem->rw.line = __LINE__;
                offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pData[0]->keyFrame & 0x1, &frameLenth, pData[0]->data);
                pMonitorItem->rw.line = __LINE__;
                if (offset < 1) {
                    Record_itemLog(FsLogType_fatal, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pData[0]->keyFrame, pData[0]->datalenth);
                    fflush(stdout);
                    FsLogExit();
                } else {
                    pMonitorItem->rw.line = __LINE__;
                    record_private_item_addVideoBuffer(pRecord_item, pData[0]->frameIndex, pData[0]->capture_uptime, pData[0]->keyFrame & 0x1
#if defined __record_P_item_do_videoRecord_do_h265
                            , 3
#elif defined __record_P_item_do_videoRecord_do_h264
                            , 2
#else
#error "error"
#endif
                            , pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pData[0]->width, pData[0]->height, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
#undef __record_P_item_do_videoRecord_do_size
#ifdef __record_P_item_do_videoRecord_do_h265
#undef __record_P_item_do_videoRecord_do_h265
#endif
#ifdef __record_P_item_do_videoRecord_do_h264
#undef __record_P_item_do_videoRecord_do_h264
#endif
#endif
    } else if (ImageFrame_valid_H264_1(pFrame->dataValidType)) {
#define __record_P_item_do_videoRecord_do_size 1
#define __record_P_item_do_videoRecord_do_h264
#ifndef __record_P_item_do_videoRecord_do 
#ifdef __record_P_item_do_videoRecord_do_h265
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH265 * const *pData = pFrame->data.h265[__record_P_item_do_videoRecord_do_size];
#elif defined __record_P_item_do_videoRecord_do_h264
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH264 * const *pData = pFrame->data.h264[__record_P_item_do_videoRecord_do_size];
#else
#error "error"
#endif
        for (; NULL != *pData; pData++) {
            /* 完成一次记录片段,1分钟 */
            if (3 == pData[0]->keyFrame) {
                if (((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->p.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                /* 创建新文件片段,为mkv文件写入头信息 */
                if (NULL == pRecord_item->p._recordVideoFile_fd) {
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p.recordVideoFile_startTime = ((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pData[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir
                            , pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p._recordVideoFile_fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "w+");
                    pMonitorItem->rw.line = __LINE__;
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno));
                        fflush(stdout);
                    } else {
#ifdef __record_P_item_do_videoRecord_do_h265
                        char *const h265CodecPrivate = video_mkv_codecPrivate_h265_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 3,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h265CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h265CodecPrivate);
#elif defined __record_P_item_do_videoRecord_do_h264
                        char *const h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h264CodecPrivate);
#else
#error "error"
#endif
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
            /* 继续写当前文件，写入帧信息 */
            if (pRecord_item->p._recordVideoFile_fd) {
                frameLenth = pData[0]->datalenth;
                pMonitorItem->rw.line = __LINE__;
                offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pData[0]->keyFrame & 0x1, &frameLenth, pData[0]->data);
                pMonitorItem->rw.line = __LINE__;
                if (offset < 1) {
                    Record_itemLog(FsLogType_fatal, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pData[0]->keyFrame, pData[0]->datalenth);
                    fflush(stdout);
                    FsLogExit();
                } else {
                    pMonitorItem->rw.line = __LINE__;
                    record_private_item_addVideoBuffer(pRecord_item, pData[0]->frameIndex, pData[0]->capture_uptime, pData[0]->keyFrame & 0x1
#if defined __record_P_item_do_videoRecord_do_h265
                            , 3
#elif defined __record_P_item_do_videoRecord_do_h264
                            , 2
#else
#error "error"
#endif
                            , pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pData[0]->width, pData[0]->height, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
#undef __record_P_item_do_videoRecord_do_size
#ifdef __record_P_item_do_videoRecord_do_h265
#undef __record_P_item_do_videoRecord_do_h265
#endif
#ifdef __record_P_item_do_videoRecord_do_h264
#undef __record_P_item_do_videoRecord_do_h264
#endif
#endif
    } else if (ImageFrame_valid_H265_2(pFrame->dataValidType)) {
#define __record_P_item_do_videoRecord_do_size 2
#define __record_P_item_do_videoRecord_do_h265
#ifndef __record_P_item_do_videoRecord_do 
#ifdef __record_P_item_do_videoRecord_do_h265
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH265 * const *pData = pFrame->data.h265[__record_P_item_do_videoRecord_do_size];
#elif defined __record_P_item_do_videoRecord_do_h264
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH264 * const *pData = pFrame->data.h264[__record_P_item_do_videoRecord_do_size];
#else
#error "error"
#endif
        for (; NULL != *pData; pData++) {
            /* 完成一次记录片段,1分钟 */
            if (3 == pData[0]->keyFrame) {
                if (((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->p.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                /* 创建新文件片段,为mkv文件写入头信息 */
                if (NULL == pRecord_item->p._recordVideoFile_fd) {
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p.recordVideoFile_startTime = ((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pData[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir
                            , pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p._recordVideoFile_fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "w+");
                    pMonitorItem->rw.line = __LINE__;
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno));
                        fflush(stdout);
                    } else {
#ifdef __record_P_item_do_videoRecord_do_h265
                        char *const h265CodecPrivate = video_mkv_codecPrivate_h265_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 3,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h265CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h265CodecPrivate);
#elif defined __record_P_item_do_videoRecord_do_h264
                        char *const h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h264CodecPrivate);
#else
#error "error"
#endif
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
            /* 继续写当前文件，写入帧信息 */
            if (pRecord_item->p._recordVideoFile_fd) {
                frameLenth = pData[0]->datalenth;
                pMonitorItem->rw.line = __LINE__;
                offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pData[0]->keyFrame & 0x1, &frameLenth, pData[0]->data);
                pMonitorItem->rw.line = __LINE__;
                if (offset < 1) {
                    Record_itemLog(FsLogType_fatal, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pData[0]->keyFrame, pData[0]->datalenth);
                    fflush(stdout);
                    FsLogExit();
                } else {
                    pMonitorItem->rw.line = __LINE__;
                    record_private_item_addVideoBuffer(pRecord_item, pData[0]->frameIndex, pData[0]->capture_uptime, pData[0]->keyFrame & 0x1
#if defined __record_P_item_do_videoRecord_do_h265
                            , 3
#elif defined __record_P_item_do_videoRecord_do_h264
                            , 2
#else
#error "error"
#endif
                            , pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pData[0]->width, pData[0]->height, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
#undef __record_P_item_do_videoRecord_do_size
#ifdef __record_P_item_do_videoRecord_do_h265
#undef __record_P_item_do_videoRecord_do_h265
#endif
#ifdef __record_P_item_do_videoRecord_do_h264
#undef __record_P_item_do_videoRecord_do_h264
#endif
#endif
    } else if (ImageFrame_valid_H264_2(pFrame->dataValidType)) {
#define __record_P_item_do_videoRecord_do_size 2
#define __record_P_item_do_videoRecord_do_h264
#ifndef __record_P_item_do_videoRecord_do 
#ifdef __record_P_item_do_videoRecord_do_h265
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H265_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH265 * const *pData = pFrame->data.h265[__record_P_item_do_videoRecord_do_size];
#elif defined __record_P_item_do_videoRecord_do_h264
        if (pRecord_item->p.recordMask != FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size)) {
            record_private_item_clear_video(pRecord_item);
            pRecord_item->p.recordMask = FsMacrosValue2(ImageFrame_H264_, __record_P_item_do_videoRecord_do_size);
        }
        FsObjectH264 * const *pData = pFrame->data.h264[__record_P_item_do_videoRecord_do_size];
#else
#error "error"
#endif
        for (; NULL != *pData; pData++) {
            /* 完成一次记录片段,1分钟 */
            if (3 == pData[0]->keyFrame) {
                if (((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval != pRecord_item->p.recordVideoFile_startTime)
                    record_private_item_clear_video(pRecord_item);
                /* 创建新文件片段,为mkv文件写入头信息 */
                if (NULL == pRecord_item->p._recordVideoFile_fd) {
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p.recordVideoFile_startTime = ((unsigned long) pData[0]->capture_gmtTime) / pRecord_item->ro._recordVideoFileInterval;
                    ts = (time_t) pData[0]->capture_gmtTime;
                    // int msec = ( int) ((pFrame->capture_gmtTime - ts)*1000);
                    localtime_r(&ts, &ti);
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/%04d%02d%02d/%02d/video%04d%02d%02d%02d%02d%02d.mkv", pRecord_item->ro._pRecord->ro._dir
                            , pRecord_item->ro._uuid, 1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour,
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
                    fs_dir_make(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, 0777);
                    pMonitorItem->rw.line = __LINE__;
                    pRecord_item->p._recordVideoFile_fd = fopen(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "w+");
                    pMonitorItem->rw.line = __LINE__;
                    if (NULL == pRecord_item->p._recordVideoFile_fd) {
                        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno));
                        fflush(stdout);
                    } else {
#ifdef __record_P_item_do_videoRecord_do_h265
                        char *const h265CodecPrivate = video_mkv_codecPrivate_h265_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 3,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h265CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h265CodecPrivate);
#elif defined __record_P_item_do_videoRecord_do_h264
                        char *const h264CodecPrivate = video_mkv_codecPrivate_h264_get__IO(pData[0]->datalenth, (unsigned char*) pData[0]->data);
                        struct VideoMKV_track track[1];
                        memset(track, 0, sizeof ( struct VideoMKV_track));
                        track[0].type = VideoMKV_trackVideo, track[0].index = 1, track[0].p.video.width = pData[0]->width, track[0].p.video.height = pData[0]->height, track[0].p.video.encodeType = 2,
                                track[0].p.video.frameRate = pFrame->frameRate, track[0].p.video.codecPrivate = h264CodecPrivate;
                        pMonitorItem->rw.line = __LINE__;
                        pRecord_item->p._pMKV = video_mkv_new_for_file_save__IO(1, track, 1, 0, 0, pRecord_item->p._recordVideoFile_fd);
                        pMonitorItem->rw.line = __LINE__;
                        fsFree(h264CodecPrivate);
#else
#error "error"
#endif
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
            /* 继续写当前文件，写入帧信息 */
            if (pRecord_item->p._recordVideoFile_fd) {
                frameLenth = pData[0]->datalenth;
                pMonitorItem->rw.line = __LINE__;
                offset = video_mkv_file_save_frame(pRecord_item->p._pMKV, 1, pData[0]->keyFrame & 0x1, &frameLenth, pData[0]->data);
                pMonitorItem->rw.line = __LINE__;
                if (offset < 1) {
                    Record_itemLog(FsLogType_fatal, "Save mkv frame failed,rv=%lld,keyFrame=%hhu,datalenth=%u.\n", offset, pData[0]->keyFrame, pData[0]->datalenth);
                    fflush(stdout);
                    FsLogExit();
                } else {
                    pMonitorItem->rw.line = __LINE__;
                    record_private_item_addVideoBuffer(pRecord_item, pData[0]->frameIndex, pData[0]->capture_uptime, pData[0]->keyFrame & 0x1
#if defined __record_P_item_do_videoRecord_do_h265
                            , 3
#elif defined __record_P_item_do_videoRecord_do_h264
                            , 2
#else
#error "error"
#endif
                            , pRecord_item->p._recordVideoFile_fd, offset, frameLenth, pData[0]->width, pData[0]->height, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
#undef __record_P_item_do_videoRecord_do_size
#ifdef __record_P_item_do_videoRecord_do_h265
#undef __record_P_item_do_videoRecord_do_h265
#endif
#ifdef __record_P_item_do_videoRecord_do_h264
#undef __record_P_item_do_videoRecord_do_h264
#endif
#endif
    }
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
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
        fsFree(fs_ObjectList_remove_head(videoPartList));
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
    if (11 == b) {
        /* 还是找不到,写的程序有BUG了 */
        FsPrintf(1, "Can't find key frame for the video:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        fsFree(fs_ObjectList_remove_head(videoPartList));
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
        fsFree(pPartVideo_item);
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
        fsFree(pPartVideo_item);
        return -1;
    }
    if (0) {
        /* AVI保存,要求原视频也要是AVI */
        VideoAVI *pAVI = video_avi_new_for_file_save__IO(pNodeVideoBuffer[b]->width, pNodeVideoBuffer[b]->height, pNodeVideoBuffer[b]->type, fd);
        if (pAVI) {
            fflush(pNodeVideoBuffer[e - 1]->fd);
            unsigned long offset = ftell(pNodeVideoBuffer[e - 1]->fd);
            unsigned int bufferLen = 1024 * 128;
            char *buffer = (char*) fsMalloc(bufferLen);
            for (; b < e; b++) {
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
                    fsFree(buffer);
                    buffer = (char*) fsMalloc(bufferLen);
                }
                if (fread(buffer, i, 1, pNodeVideoBuffer[b]->fd) != 1) {
                    FsPrintf(1, "Run fread(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                            i, (long) pNodeVideoBuffer[b]->fd, b, e, strerror(errno),
                            pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                            pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                    break;
                }
                if (video_avi_file_save_frame(pAVI, (unsigned int*) &i, buffer) < 1) {
                    FsPrintf(1, "Write data(%lu,%#lx,%lu,%lu) failed for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                            i, (long) pNodeVideoBuffer[b]->fd, b, e,
                            pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                            pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
                    break;
                }
            }
            fsFree(buffer);
            /* 前面的视频文件都不会再写了,只需重置最后一个文件的偏移量 */
            if (fseek(pNodeVideoBuffer[e - 1]->fd, offset, SEEK_SET) != 0) {
                FsPrintf(1, "Run fseek(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n", offset, (long) pNodeVideoBuffer[e - 1]->fd, b, e, strerror(errno),
                        pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                        pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
            }
            if (video_avi_file_save_finish(pAVI, theFrameRate) != 1) {
                FsPrintf(1, "Write tail(%lu,%#lx,%lu,%lu) failed for the video file:\"%s\",%lf-%lf,%lf-%lf.\n",
                        i, (long) pNodeVideoBuffer[b]->fd, b, e,
                        pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                        pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
            }
            video_avi_delete__OI(&pAVI);
        } else {
            FsPrintf(1, "Failed to create AVI for the video file:\"%s\",%lf-%lf,%lf-%lf.\n", pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        }
    }

    if (1) {
        /* mkv保存 */
        fflush(pNodeVideoBuffer[e - 1]->fd);
        unsigned long offset = ftell(pNodeVideoBuffer[e - 1]->fd);
        /* 先读取关键帧 */
        i = pNodeVideoBuffer[b]->dataLenth;
        unsigned int bufferLen = i;
        char *buffer = (char*) fsMalloc(bufferLen);
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
            if (2 == pNodeVideoBuffer[b]->type) {
                /* h264编码 */
                /* 这个不会出错,除非有人恶意更改了数据 */
                codecPrivate = video_mkv_codecPrivate_h264_get__IO(i, (unsigned char*) buffer);
            } else if (3 == pNodeVideoBuffer[b]->type) {
                /* h265编码 */
                /* 这个不会出错,除非有人恶意更改了数据 */
                codecPrivate = video_mkv_codecPrivate_h265_get__IO(i, (unsigned char*) buffer);
            }
            else {
                /* mjpeg */
                codecPrivate = video_mkv_codecPrivate_jpeg_get__IO();
            }

            track[0].name = (char*) "video1", track[0].type = VideoMKV_trackVideo, track[0].index = 1,
                    track[0].p.video.width = pNodeVideoBuffer[b]->width, track[0].p.video.height = pNodeVideoBuffer[b]->height, track[0].p.video.encodeType = pNodeVideoBuffer[b]->type,
                    track[0].p.video.frameRate = theFrameRate, track[0].p.video.codecPrivate = codecPrivate;
            track[1].name = (char*) "subtitle1", track[1].type = VideoMKV_trackSubtitle, track[1].index = 2,
                    track[1].p.subtitle.width = pNodeVideoBuffer[b]->width, track[1].p.subtitle.height = pNodeVideoBuffer[b]->height, track[1].p.subtitle.fontSize = 30;
            VideoMKV *pMKV = video_mkv_new_for_file_save__IO(2, track, 1, 0, 1, fd);
            fsFree(codecPrivate);
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
                            fsFree(buffer);
                            buffer = (char*) fsMalloc(bufferLen);
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
        fsFree(buffer);
        /* 前面的视频文件都不会再写了,只需重置最后一个文件的偏移量 */
        if (fseek(pNodeVideoBuffer[e - 1]->fd, offset, SEEK_SET) != 0) {
            FsPrintf(1, "Run fseek(=%lu,%#lx,%lu,%lu) error:\"%s\" for the video file:\"%s\",%lf-%lf,%lf-%lf.\n", offset, (long) pNodeVideoBuffer[e - 1]->fd, b, e, strerror(errno),
                    pPartVideo_item->fileName, pPartVideo_item->startTime, pPartVideo_item->endTime,
                    pNodeVideoBuffer[0]->captureTime_uptime, pNodeVideoBuffer[l1 - 1]->captureTime_uptime), fflush(stdout);
        }
    }
    fclose(fd);
    fsFree(pPartVideo_item);

    return 1;
}

/* 正序正常发送 */
static void record_P_item_do_videoDemand_send_0(struct Record_P_VideoDemand * const pVideoDemand,
        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) {
    //#define __videoDemand_send_FileStream
    //#define __videoDemand_send_2_3
#ifndef __videoDemand_send_0_2 
    struct VideoMKV_node * pMKV_node, **ppMKV_node_main;
    struct VideoMKV_channel *pMKV_channel_main, *pMKV_channel;
    double thistime = fs_time_uptime_get();
#ifdef __videoDemand_send_FileStream
    const float frameRateMulti = 1.0;
#else
    const float frameRateMulti = pVideoDemand->frameRateMulti;
#endif
    unsigned int sendCountIndex, channelIndex;
    if (pVideoDemand->pMKV->videoIndex > 0)pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->videoIndex;
    else pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->audioIndex;
    if (1 == pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) {
        /* 单通道,这只是对多通道的优化,可以全使用多通道来处理 */
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            /* 发数据 */
            unsigned int l;
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->videoIndex - 1;
            sendCountIndex = pVideoDemand->sendCount;
            if (3 == pMKV_channel->p.video.encodeType) {
                /* h265 */
                FsObjectH265 *pObjectH265;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, sendCountIndex++) {
                    pMKV_node = *ppMKV_node_main++;
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate,
                                pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                }
            } else if (2 == pMKV_channel->p.video.encodeType) {
                /* h264 */
                FsObjectH264 *pObjectH264;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, sendCountIndex++) {
                    pMKV_node = *ppMKV_node_main++;
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                }
            } else {
                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                fflush(stdout);
            }
            pVideoDemand->sendCount = sendCountIndex;
        }
        else {
            /* 肯定有音频 */
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->audioIndex - 1;
            sendCountIndex = pVideoDemand->sendCount;
            /* 发数据 */
            FsObjectAAC *pObjectAAC;
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                    sendCount--, sendCountIndex++) {
                pMKV_node = *ppMKV_node_main++;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate,
                        pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti),
                        pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
            }
            pVideoDemand->sendCount = sendCountIndex;
        }

    } else {
        /* 多通道 */
        FsObjectH264 *pObjectH264;
        FsObjectH265 *pObjectH265;
        FsObjectAAC *pObjectAAC;

        struct VideoMKV_node **ppMKV_node;
        unsigned int l, i;
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + pVideoDemand->sendCount; sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++) {
                pMKV_node = *ppMKV_node_main++;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->videoIndex - 1;
                sendCountIndex = pVideoDemand->sendCount;
                if (3 == pMKV_channel->p.video.encodeType) {
                    /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                } else if (2 == pMKV_channel->p.video.encodeType) {
                    /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                } else {
                    FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                    fflush(stdout);
                }
                /* 处理其他通道的数据 */
                for (channelIndex = pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount, pMKV_channel = pVideoDemand->pMKV->channel + channelIndex;
                        channelIndex-- > 0; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (sendCountIndex + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        sendCountIndex = sendCountIndex / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (sendCountIndex > pMKV_channel->list->nodeCount)sendCountIndex = pMKV_channel->list->nodeCount;
                        for (ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; sendCountIndex < i; sendCountIndex++) {
                            pMKV_node = *ppMKV_node++;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else

                        if (VideoMKV_trackVideo == pMKV_channel->type) {
                        i = (sendCountIndex + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        sendCountIndex = sendCountIndex / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (sendCountIndex > pMKV_channel->list->nodeCount)sendCountIndex = pMKV_channel->list->nodeCount;
                        for (ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; sendCountIndex < i; sendCountIndex++) {
                            pMKV_node = *ppMKV_node++;
                            if (3 == pMKV_channel->p.video.encodeType) {
                                /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                                } else {
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
                                }
#endif
                            } else if (2 == pMKV_channel->p.video.encodeType) {
                                /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                                    video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                                } else {
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h264_object_reset_datalenth_small(pObjectH264, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
                                }
#endif
                            } else {
                                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                                fflush(stdout);
                            }
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }
        else {
            /* 肯定有音频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + pVideoDemand->sendCount;
                    sendCount > 0; sendCount--, pVideoDemand->sendCount++) {
                pMKV_node = *ppMKV_node_main++;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->audioIndex - 1;
                sendCountIndex = pVideoDemand->sendCount;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate,
                        pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                        , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                        ,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                /* 处理其他通道的数据 */
                for (channelIndex = pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount, pMKV_channel = pVideoDemand->pMKV->channel + channelIndex;
                        channelIndex-- > 0; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (sendCountIndex + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        sendCountIndex = sendCountIndex / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (sendCountIndex > pMKV_channel->list->nodeCount)sendCountIndex = pMKV_channel->list->nodeCount;
                        for (ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; sendCountIndex < i; sendCountIndex++) {
                            pMKV_node = *ppMKV_node++;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }

    }
#ifdef __videoDemand_send_FileStream
#undef __videoDemand_send_FileStream
#endif
#ifdef __videoDemand_send_2_3
#undef __videoDemand_send_2_3
#endif
#endif
}

/* 倒序正常发送 */
static void record_P_item_do_videoDemand_send_1(struct Record_P_VideoDemand * const pVideoDemand,
        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) {
    //#define __videoDemand_send_FileStream
    //#define __videoDemand_send_2_3
#ifndef __videoDemand_send_1_3 
    struct VideoMKV_node * pMKV_node, **ppMKV_node_main;
    struct VideoMKV_channel *pMKV_channel_main, *pMKV_channel;
    double thistime = fs_time_uptime_get();
#ifdef __videoDemand_send_FileStream
    const float frameRateMulti = 1.0;
#else
    const float frameRateMulti = pVideoDemand->frameRateMulti;
#endif
    if (pVideoDemand->pMKV->videoIndex > 0)pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->videoIndex;
    else pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->audioIndex;
    unsigned int sendCountIndex_main = pMKV_channel_main->list->nodeCount - pVideoDemand->sendCount - 1, sendCountIndex, channelIndex;
    if (1 == pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) {
        /* 单通道,这只是对多通道的优化,可以全使用多通道来处理 */
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            /* 发数据 */
            unsigned int l;
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->videoIndex - 1;
            sendCountIndex = sendCountIndex_main;
            if (3 == pMKV_channel->p.video.encodeType) {
                /* h265 */
                FsObjectH265 *pObjectH265;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, pVideoDemand->sendCount++, sendCountIndex--) {
                    pMKV_node = *ppMKV_node_main--;
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                }
            } else if (2 == pMKV_channel->p.video.encodeType) {
                /* h264 */
                FsObjectH264 *pObjectH264;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, pVideoDemand->sendCount++, sendCountIndex--) {
                    pMKV_node = *ppMKV_node_main--;
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                }
            } else {
                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                fflush(stdout);
            }
        }
        else {
            /* 肯定有音频 */
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->audioIndex - 1;
            sendCountIndex = sendCountIndex_main;
            /* 发数据 */
            FsObjectAAC *pObjectAAC;
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + sendCountIndex); sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++, sendCountIndex--) {
                pMKV_node = *ppMKV_node_main--;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                        , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                        , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                        ,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
            }
        }

    } else {
        /* 多通道 */
        FsObjectH264 *pObjectH264;
        FsObjectH265 *pObjectH265;
        FsObjectAAC *pObjectAAC;

        struct VideoMKV_node **ppMKV_node;
        unsigned int l, i;
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + sendCountIndex_main); sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++, sendCountIndex_main--) {
                pMKV_node = *ppMKV_node_main--;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->videoIndex - 1;
                sendCountIndex = sendCountIndex_main;
                if (3 == pMKV_channel->p.video.encodeType) {
                    /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate,
                                pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                } else if (2 == pMKV_channel->p.video.encodeType) {
                    /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate,
                                pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                } else {
                    FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                    fflush(stdout);
                }
                /* 处理其他通道的数据 */
                for (channelIndex = pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount, pMKV_channel = pVideoDemand->pMKV->channel + channelIndex;
                        channelIndex-- > 0; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (pVideoDemand->sendCount + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        l = pVideoDemand->sendCount / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (l > pMKV_channel->list->nodeCount)l = pMKV_channel->list->nodeCount;
                        sendCountIndex = pMKV_channel->list->nodeCount - l - 1;
                        for (i -= l, ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; i > 0; i--, sendCountIndex++) {
                            pMKV_node = *ppMKV_node--;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else

                        if (VideoMKV_trackVideo == pMKV_channel->type) {
                        i = (pVideoDemand->sendCount + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        l = pVideoDemand->sendCount / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (l > pMKV_channel->list->nodeCount)l = pMKV_channel->list->nodeCount;
                        sendCountIndex = pMKV_channel->list->nodeCount - l - 1;
                        for (i -= l, ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; i > 0; i--, sendCountIndex++) {
                            pMKV_node = *ppMKV_node--;
                            if (3 == pMKV_channel->p.video.encodeType) {
                                /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                                } else {
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
                                }
#endif
                            } else if (2 == pMKV_channel->p.video.encodeType) {
                                /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                                    video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                                } else {
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h264_object_reset_datalenth_small(pObjectH264, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
                                }
#endif
                            } else {
                                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                                fflush(stdout);
                            }
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }
        else {
            /* 肯定有音频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + sendCountIndex_main); sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++, sendCountIndex_main--) {
                pMKV_node = *ppMKV_node_main--;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->audioIndex - 1;
                sendCountIndex = sendCountIndex_main;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                        , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                        , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                        ,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                /* 处理其他通道的数据 */
                for (pMKV_channel = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount;
                        pMKV_channel > pVideoDemand->pMKV->channel; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (pVideoDemand->sendCount + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        l = pVideoDemand->sendCount / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (l > pMKV_channel->list->nodeCount)l = pMKV_channel->list->nodeCount;
                        sendCountIndex = pMKV_channel->list->nodeCount - l - 1;
                        for (i -= l, ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; i > 0; i--, sendCountIndex++) {
                            pMKV_node = *ppMKV_node--;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }

    }
#ifdef __videoDemand_send_FileStream
#undef __videoDemand_send_FileStream
#endif
#ifdef __videoDemand_send_2_3
#undef __videoDemand_send_2_3
#endif
#endif
}

/* 正序关键帧发送 */
static void record_P_item_do_videoDemand_send_2(struct Record_P_VideoDemand * const pVideoDemand,
        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) {
    //#define __videoDemand_send_FileStream
#define __videoDemand_send_2_3
#ifndef __videoDemand_send_0_2 
    struct VideoMKV_node * pMKV_node, **ppMKV_node_main;
    struct VideoMKV_channel *pMKV_channel_main, *pMKV_channel;
    double thistime = fs_time_uptime_get();
#ifdef __videoDemand_send_FileStream
    const float frameRateMulti = 1.0;
#else
    const float frameRateMulti = pVideoDemand->frameRateMulti;
#endif
    unsigned int sendCountIndex, channelIndex;
    if (pVideoDemand->pMKV->videoIndex > 0)pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->videoIndex;
    else pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->audioIndex;
    if (1 == pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) {
        /* 单通道,这只是对多通道的优化,可以全使用多通道来处理 */
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            /* 发数据 */
            unsigned int l;
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->videoIndex - 1;
            sendCountIndex = pVideoDemand->sendCount;
            if (3 == pMKV_channel->p.video.encodeType) {
                /* h265 */
                FsObjectH265 *pObjectH265;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, sendCountIndex++) {
                    pMKV_node = *ppMKV_node_main++;
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate,
                                pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                }
            } else if (2 == pMKV_channel->p.video.encodeType) {
                /* h264 */
                FsObjectH264 *pObjectH264;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, sendCountIndex++) {
                    pMKV_node = *ppMKV_node_main++;
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                }
            } else {
                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                fflush(stdout);
            }
            pVideoDemand->sendCount = sendCountIndex;
        }
        else {
            /* 肯定有音频 */
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->audioIndex - 1;
            sendCountIndex = pVideoDemand->sendCount;
            /* 发数据 */
            FsObjectAAC *pObjectAAC;
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                    sendCount--, sendCountIndex++) {
                pMKV_node = *ppMKV_node_main++;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate,
                        pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti),
                        pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
            }
            pVideoDemand->sendCount = sendCountIndex;
        }

    } else {
        /* 多通道 */
        FsObjectH264 *pObjectH264;
        FsObjectH265 *pObjectH265;
        FsObjectAAC *pObjectAAC;

        struct VideoMKV_node **ppMKV_node;
        unsigned int l, i;
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + pVideoDemand->sendCount; sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++) {
                pMKV_node = *ppMKV_node_main++;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->videoIndex - 1;
                sendCountIndex = pVideoDemand->sendCount;
                if (3 == pMKV_channel->p.video.encodeType) {
                    /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                } else if (2 == pMKV_channel->p.video.encodeType) {
                    /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                } else {
                    FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                    fflush(stdout);
                }
                /* 处理其他通道的数据 */
                for (channelIndex = pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount, pMKV_channel = pVideoDemand->pMKV->channel + channelIndex;
                        channelIndex-- > 0; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (sendCountIndex + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        sendCountIndex = sendCountIndex / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (sendCountIndex > pMKV_channel->list->nodeCount)sendCountIndex = pMKV_channel->list->nodeCount;
                        for (ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; sendCountIndex < i; sendCountIndex++) {
                            pMKV_node = *ppMKV_node++;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else

                        if (VideoMKV_trackVideo == pMKV_channel->type) {
                        i = (sendCountIndex + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        sendCountIndex = sendCountIndex / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (sendCountIndex > pMKV_channel->list->nodeCount)sendCountIndex = pMKV_channel->list->nodeCount;
                        for (ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; sendCountIndex < i; sendCountIndex++) {
                            pMKV_node = *ppMKV_node++;
                            if (3 == pMKV_channel->p.video.encodeType) {
                                /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                                } else {
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
                                }
#endif
                            } else if (2 == pMKV_channel->p.video.encodeType) {
                                /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                                    video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                                } else {
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h264_object_reset_datalenth_small(pObjectH264, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
                                }
#endif
                            } else {
                                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                                fflush(stdout);
                            }
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }
        else {
            /* 肯定有音频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + pVideoDemand->sendCount;
                    sendCount > 0; sendCount--, pVideoDemand->sendCount++) {
                pMKV_node = *ppMKV_node_main++;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->audioIndex - 1;
                sendCountIndex = pVideoDemand->sendCount;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate,
                        pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                        , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                        ,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                /* 处理其他通道的数据 */
                for (channelIndex = pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount, pMKV_channel = pVideoDemand->pMKV->channel + channelIndex;
                        channelIndex-- > 0; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (sendCountIndex + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        sendCountIndex = sendCountIndex / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (sendCountIndex > pMKV_channel->list->nodeCount)sendCountIndex = pMKV_channel->list->nodeCount;
                        for (ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; sendCountIndex < i; sendCountIndex++) {
                            pMKV_node = *ppMKV_node++;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }

    }
#ifdef __videoDemand_send_FileStream
#undef __videoDemand_send_FileStream
#endif
#ifdef __videoDemand_send_2_3
#undef __videoDemand_send_2_3
#endif
#endif
}

/* 倒序关键帧发送 */
static void record_P_item_do_videoDemand_send_3(struct Record_P_VideoDemand * const pVideoDemand,
        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) {
    //#define __videoDemand_send_FileStream
#define __videoDemand_send_2_3
#ifndef __videoDemand_send_1_3 
    struct VideoMKV_node * pMKV_node, **ppMKV_node_main;
    struct VideoMKV_channel *pMKV_channel_main, *pMKV_channel;
    double thistime = fs_time_uptime_get();
#ifdef __videoDemand_send_FileStream
    const float frameRateMulti = 1.0;
#else
    const float frameRateMulti = pVideoDemand->frameRateMulti;
#endif
    if (pVideoDemand->pMKV->videoIndex > 0)pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->videoIndex;
    else pMKV_channel_main = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->audioIndex;
    unsigned int sendCountIndex_main = pMKV_channel_main->list->nodeCount - pVideoDemand->sendCount - 1, sendCountIndex, channelIndex;
    if (1 == pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) {
        /* 单通道,这只是对多通道的优化,可以全使用多通道来处理 */
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            /* 发数据 */
            unsigned int l;
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->videoIndex - 1;
            sendCountIndex = sendCountIndex_main;
            if (3 == pMKV_channel->p.video.encodeType) {
                /* h265 */
                FsObjectH265 *pObjectH265;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, pVideoDemand->sendCount++, sendCountIndex--) {
                    pMKV_node = *ppMKV_node_main--;
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                }
            } else if (2 == pMKV_channel->p.video.encodeType) {
                /* h264 */
                FsObjectH264 *pObjectH264;
                for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex); sendCount > 0;
                        sendCount--, pVideoDemand->sendCount++, sendCountIndex--) {
                    pMKV_node = *ppMKV_node_main--;
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                }
            } else {
                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                fflush(stdout);
            }
        }
        else {
            /* 肯定有音频 */
            pMKV_channel = pMKV_channel_main;
            channelIndex = pVideoDemand->pMKV->audioIndex - 1;
            sendCountIndex = sendCountIndex_main;
            /* 发数据 */
            FsObjectAAC *pObjectAAC;
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + sendCountIndex); sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++, sendCountIndex--) {
                pMKV_node = *ppMKV_node_main--;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                        , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                        , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                        ,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
            }
        }

    } else {
        /* 多通道 */
        FsObjectH264 *pObjectH264;
        FsObjectH265 *pObjectH265;
        FsObjectAAC *pObjectAAC;

        struct VideoMKV_node **ppMKV_node;
        unsigned int l, i;
        if (pVideoDemand->pMKV->videoIndex > 0) {
            /* 有视频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + sendCountIndex_main); sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++, sendCountIndex_main--) {
                pMKV_node = *ppMKV_node_main--;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->videoIndex - 1;
                sendCountIndex = sendCountIndex_main;
                if (3 == pMKV_channel->p.video.encodeType) {
                    /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                    } else {
                        pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate,
                                pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h265_object_reset_datalenth_small(pObjectH265, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                        video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH265->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH265);
                    }
#endif
                } else if (2 == pMKV_channel->p.video.encodeType) {
                    /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                    if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                        video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                    } else {
                        pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                        l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate,
                                pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                        video_h264_object_reset_datalenth_small(pObjectH264, l);
                        short cutLeft, cutRight;
                        const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                        video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                ,
#ifndef __videoDemand_send_FileStream
                                pVideoDemand->fileStartTime +
#endif
                                sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                        pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                        pObjectH264->object.channelID = channelIndex;
                        add_frame__OI_2(pDemandClient, pObjectH264);
                    }
#endif
                } else {
                    FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                    fflush(stdout);
                }
                /* 处理其他通道的数据 */
                for (channelIndex = pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount, pMKV_channel = pVideoDemand->pMKV->channel + channelIndex;
                        channelIndex-- > 0; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (pVideoDemand->sendCount + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        l = pVideoDemand->sendCount / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (l > pMKV_channel->list->nodeCount)l = pMKV_channel->list->nodeCount;
                        sendCountIndex = pMKV_channel->list->nodeCount - l - 1;
                        for (i -= l, ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; i > 0; i--, sendCountIndex++) {
                            pMKV_node = *ppMKV_node--;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else

                        if (VideoMKV_trackVideo == pMKV_channel->type) {
                        i = (pVideoDemand->sendCount + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        l = pVideoDemand->sendCount / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.video.frameRate;
                        if (l > pMKV_channel->list->nodeCount)l = pMKV_channel->list->nodeCount;
                        sendCountIndex = pMKV_channel->list->nodeCount - l - 1;
                        for (i -= l, ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; i > 0; i--, sendCountIndex++) {
                            pMKV_node = *ppMKV_node--;
                            if (3 == pMKV_channel->p.video.encodeType) {
                                /* h265 */
#ifndef __videoDemand_send_0_1_2_3_h265
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth);
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 0, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
#endif
                                } else {
                                    pObjectH265 = video_h265_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h265Stream_from_file((unsigned char*) pObjectH265->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h265_object_reset_datalenth_small(pObjectH265, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h265_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH265->data, pObjectH265->datalenth);
                                    video_h265_object_set_data(pObjectH265, 3, stitchIndex, cutLeft, cutRight, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH265->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH265);
                                }
#endif
                            } else if (2 == pMKV_channel->p.video.encodeType) {
                                /* h264 */
#ifndef __videoDemand_send_0_1_2_3_h264
                                if (0 == pMKV_node->p.video.keyFrame) {
#ifndef __videoDemand_send_2_3
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth);
                                    video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, NULL, pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 0, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
#endif
                                } else {
                                    pObjectH264 = video_h264_object_new__IO(pMKV_node->lenth + *((unsigned int*) pMKV_channel->p.video.codecPrivate));
                                    l = video_mkv_read_h264Stream_from_file((unsigned char*) pObjectH264->data, pMKV_channel->p.video.codecPrivate
                                            , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                                    video_h264_object_reset_datalenth_small(pObjectH264, l);
                                    short cutLeft, cutRight;
                                    const unsigned char stitchIndex = video_h264_get_stitchIndex(&cutLeft, &cutRight, (unsigned char*) pObjectH264->data, pObjectH264->datalenth);
                                    video_h264_object_set_data(pObjectH264, 0, pVideoDemand->frameIndex[channelIndex], 0, (pMKV_channel->p.video.frameRate * frameRateMulti)
                                            , pMKV_channel->p.video.width, pMKV_channel->p.video.height, 3, stitchIndex, cutLeft, cutRight, thistime
                                            ,
#ifndef __videoDemand_send_FileStream
                                            pVideoDemand->fileStartTime +
#endif
                                            sendCountIndex / (pMKV_channel->p.video.frameRate * frameRateMulti), NULL);
                                    pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                                    pObjectH264->object.channelID = channelIndex;
                                    add_frame__OI_2(pDemandClient, pObjectH264);
                                }
#endif
                            } else {
                                FsPrintf(1, "Unknown encodeType(=%hhu).\n", pMKV_channel->p.video.encodeType);
                                fflush(stdout);
                            }
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }
        else {
            /* 肯定有音频 */
            for (ppMKV_node_main = (struct VideoMKV_node**) (pMKV_channel_main->list->pNode + pMKV_channel_main->list->startIndex + sendCountIndex_main); sendCount > 0;
                    sendCount--, pVideoDemand->sendCount++, sendCountIndex_main--) {
                pMKV_node = *ppMKV_node_main--;
                pMKV_channel = pMKV_channel_main;
                channelIndex = pVideoDemand->pMKV->audioIndex - 1;
                sendCountIndex = sendCountIndex_main;
#ifndef __videoDemand_send_0_1_2_3_aac         
                pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                        , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                        , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                        ,
#ifndef __videoDemand_send_FileStream
                        pVideoDemand->fileStartTime +
#endif
                        sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                pObjectAAC->object.channelID = channelIndex;
                add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                /* 处理其他通道的数据 */
                for (pMKV_channel = pVideoDemand->pMKV->channel + pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount;
                        pMKV_channel > pVideoDemand->pMKV->channel; pMKV_channel--) {
                    if (pMKV_channel == pMKV_channel_main)continue;
                    if (VideoMKV_trackAudio == pMKV_channel->type) {
                        i = (pVideoDemand->sendCount + 1) / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (i > pMKV_channel->list->nodeCount)i = pMKV_channel->list->nodeCount;
                        l = pVideoDemand->sendCount / pMKV_channel_main->p.video.frameRate * pMKV_channel->p.audio.frameRate;
                        if (l > pMKV_channel->list->nodeCount)l = pMKV_channel->list->nodeCount;
                        sendCountIndex = pMKV_channel->list->nodeCount - l - 1;
                        for (i -= l, ppMKV_node = (struct VideoMKV_node**) pMKV_channel->list->pNode + pMKV_channel->list->startIndex + sendCountIndex; i > 0; i--, sendCountIndex++) {
                            pMKV_node = *ppMKV_node--;
#ifndef __videoDemand_send_0_1_2_3_aac         
                            pObjectAAC = audio_aac_object_new__IO(pMKV_node->lenth + 7);
                            video_mkv_read_aacStream_from_file((unsigned char*) pObjectAAC->data, pMKV_channel->p.audio.codecPrivate
                                    , pVideoDemand->fd, pMKV_node->offset, pMKV_node->lenth);
                            audio_aac_object_set_data(pObjectAAC, 0, pVideoDemand->frameIndex[channelIndex], (pMKV_channel->p.audio.frameRate * frameRateMulti)
                                    , pMKV_channel->p.audio.profile, pMKV_channel->p.audio.channels, pMKV_channel->p.audio.sampleRate, thistime
                                    ,
#ifndef __videoDemand_send_FileStream
                                    pVideoDemand->fileStartTime +
#endif
                                    sendCountIndex / (pMKV_channel->p.audio.frameRate * frameRateMulti), NULL);
                            pVideoDemand->frameIndex[channelIndex] = (pVideoDemand->frameIndex[channelIndex] + 1)&0xFFFFFFU;
                            pObjectAAC->object.channelID = channelIndex;
                            add_frame__OI_2(pDemandClient, pObjectAAC);
#endif
                        }
                    } else {
                        /* 是字幕 */
                    }
                }
            }
        }

    }
#ifdef __videoDemand_send_FileStream
#undef __videoDemand_send_FileStream
#endif
#ifdef __videoDemand_send_2_3
#undef __videoDemand_send_2_3
#endif
#endif
}

/* 记录卡口 */
static inline void record_P_item_do_watch(/* 记录项 */struct Record_item * const pRecord_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
    FsObjectList * const watchList = pRecord_item->ro.__watchList;
    if (watchList->nodeCount > 0) {
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
        static char* (*const recordWatch[]) (struct Record_item * const pRecord_item, struct Record_item_Watch * const pWatch
                , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) = {
            /* 0-ebml数据 */
            output_xml_comment_watch,
            /* 1-xml数据 */
            output_xml_comment_watch,
            /* 2-xml含注释数据 */
            output_xml_comment_watch,
            /* 3-json数据 */
            output_xml_comment_watch
        };
        unsigned long i = 0;
        FsLocal_ShareBuffer_resize(sizeof ("/tmp/upload/") + 32);
        do {
            struct Record_item_Watch * const pWatch = (struct Record_item_Watch*) fs_ObjectList_remove_head_pthreadSafety(watchList);
            /* 判断事件的所有数据是否已准备好 */
            if (pWatch->_pPlate != NULL && 0 == pWatch->_pPlate->_tag) {
                i++;
                fs_ObjectList_insert_tail_pthreadSafety(watchList, pWatch);
                FsPrintf(FsPrintfIndex, "i=%lu,watchList->nodeCount=%lu,%p,tag=%hhu,referCount=%d\n", i, watchList->nodeCount, pWatch->_pPlate, pWatch->_pPlate->_tag, fs_ObjectBase_referCount_get(pWatch->_pPlate));
            } else {
                /* 准备好了 */
                /* 判断是否需要输出 */
                if (pWatch->_recordIndex < 2 || NULL == pWatch->_pPlate || (pWatch->_pPlate->_tag & 0x3) == 0x3) {
                    FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + sizeof ("/tmp/upload/") + 32);
                    const char *const fileName = recordWatch[pRecord_item->ro._recordType](pRecord_item, pWatch, &FsLocal_ShareBuffer, pMonitorItem);
                    FsLocal_ShareBuffer_call_after();
                    if (fileName != NULL) {
                        sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "/tmp/upload/%lx%hhu", (long) pRecord_item, pRecord_item->p.recordIndex);
                        if (symlink(fileName, FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                            FsPrintf(FsPrintfIndex, "Symlink from:\"%s\" to:\"%s\" error:\"%s\",uuid:\"%s\".\n", fileName, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                            fflush(stdout);
                        }
                    } else {
                        FsPrintf(FsPrintfIndex, "Record inc failed,uuid:\"%s\".\n", pRecord_item->ro._uuid);
                        fflush(stdout);
                    }
                    if (pRecord_item->p.recordIndex < 99) pRecord_item->p.recordIndex++;
                    else pRecord_item->p.recordIndex = 0;
                } else {
                    FsPrintf(FsPrintfIndex, "Skip watch,uuid:\"%s\",objIndex=%llu,recordIndex=%u,pPlate=%p.\n", pRecord_item->ro._uuid, pWatch->_objIndex, pWatch->_recordIndex, pWatch->_pPlate);
                    fflush(stdout);
                }
                record_item_watch_delete__OI(pWatch);
            }
        } while (watchList->nodeCount > i);
        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    }
}

/* 记录事件 */
static inline void record_P_item_do_inc(/* 记录项 */struct Record_item * const pRecord_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
    FsObjectList * const incList = pRecord_item->ro.__incList;
    if (incList->nodeCount > 0) {
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
        static char* (*const recordInc[]) (struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc
                , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) = {
            /* 0-ebml数据 */
            output_xml_comment_inc,
            /* 1-xml数据 */
            output_xml_comment_inc,
            /* 2-xml含注释数据 */
            output_xml_comment_inc,
            /* 3-json数据 */
            output_xml_comment_inc
        };

        unsigned long i = 0;
        FsLocal_ShareBuffer_resize(sizeof ("/tmp/upload/") + 32);
        do {
            struct Record_item_Inc * const pInc = (struct Record_item_Inc*) fs_ObjectList_remove_head_pthreadSafety(incList);
            /* 判断事件的所有数据是否已准备好 */
            if (0) {
                i++;
                fs_ObjectList_insert_tail_pthreadSafety(incList, pInc);
            } else {
                /* 准备好了 */
                FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + sizeof ("/tmp/upload/") + 32);
                const char *const fileName = recordInc[pRecord_item->ro._recordType](pRecord_item, pInc, &FsLocal_ShareBuffer, pMonitorItem);
                FsLocal_ShareBuffer_call_after();
                if (fileName != NULL) {
                    sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "/tmp/upload/%lx%hhu", (long) pRecord_item, pRecord_item->p.recordIndex);
                    if (symlink(fileName, FsLocal_ShareBuffer + FsLocal_ShareBufferPos)) {
                        FsPrintf(FsPrintfIndex, "Symlink from:\"%s\" to:\"%s\" error:\"%s\",uuid:\"%s\".\n", fileName, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, strerror(errno), pRecord_item->ro._uuid);
                        fflush(stdout);
                    }
                } else {
                    FsPrintf(FsPrintfIndex, "Record inc failed,uuid:\"%s\".\n", pRecord_item->ro._uuid);
                    fflush(stdout);
                }
                if (pRecord_item->p.recordIndex < 99) pRecord_item->p.recordIndex++;
                else pRecord_item->p.recordIndex = 0;
                record_item_inc_delete__OI(pInc);
            }
        } while (incList->nodeCount > i);
        /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
    }
}

/* 视频点播 */
static inline void record_P_item_do_videoDemand_for_rtsp(/* 记录项 */struct Record_item * const pRecord_item, const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
    pMonitorItem->rw.line = __LINE__;
    struct RtspServer_item * const pRtspServer_item = pRecord_item->ro.__pRtspServer_item;
    if (NULL == pRtspServer_item || 0 == (pRtspServer_item->r._modelType & 0x2) || 0 == Fs_CycleList_get_count(pRtspServer_item->rw.__demandClientList))return;
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
    pthread_mutex_lock(&pRecord_item->ro.__pRtspServer_item->rw.__mutex_demandClientList);
    unsigned int l = Fs_CycleList_get_count(pRecord_item->ro.__pRtspServer_item->rw.__demandClientList);
    void *pClient = Fs_CycleList_get_head(pRecord_item->ro.__pRtspServer_item->rw.__demandClientList);
    for (; l > 0; l--, pClient = Fs_CycleList_node_get_next(pClient)) {
        struct RtspServer_item_DemandClient * const pDemandClient = RtspServer_DemandClient_get(pClient);
        //        FsPrintf(1, "a videoDemand,uri:\"%s\",parameterCount=%u.\n", pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
        //        fflush(stdout);

#define __record_P_item_do_videoDemand_add_frame__OI_2 rtspServer_item_DemandClient_add_frame__OI_2
#ifndef __record_P_item_do_videoDemand 
        pMonitorItem->rw.line = __LINE__;
        struct Record_P_VideoDemand * pVideoDemand;
        // 创建回放对象
        if (NULL == pDemandClient->rw.deleteExternP) {
            pMonitorItem->rw.line = __LINE__;
            if (pDemandClient->rw.error != 0)continue;
#ifndef __record_P_item_do_videoDemand_gb28181
            Record_itemLog(FsLogType_matchError, "pDemandClient(=%p),uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                    , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
            if (FsLogTypeCheck(FsLogType_matchError)) {
                unsigned int ui = 0;
                for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                    printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                }
            }
#endif
            /* 初始化 */
            // 获取时间
#ifndef __record_P_item_do_videoDemand_gb28181
            double fps = -1.0;
            pMonitorItem->rw.line = __LINE__;
            // 获取参数fps参数
            {
                unsigned int ui = 0;
                for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                    if (0 == strcmp("fps", pDemandClient->ro.__pUrlParameter->parameter[ui][0])) {
                        if (pDemandClient->ro.__pUrlParameter->parameter[ui][1]) fps = atof(pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                        Record_itemLog2(FsLogType_info, "fps:\"%s\"/%lf.\n", pDemandClient->ro.__pUrlParameter->parameter[ui][1], fps);
                        break;
                    }
                }
            }
#else
            double fps = 25.0 * 16.0;
#endif
            pMonitorItem->rw.line = __LINE__;
            // 没有设置startTime,不允许点播
            if (pDemandClient->rw.jumpTime < 0.00001) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog(FsLogType_matchError, "Get start time from the pDemandClient(=%p) error,uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                        , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
                if (FsLogTypeCheck(FsLogType_matchError)) {
                    unsigned int ui = 0;
                    for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                        printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                    }
                }
#else
                Record_itemLog(FsLogType_matchError, "Get start time from the pDemandClient(=%p) error,uri:\"%s\",name=\"%s\",starttime:%u, endtime:%u\n", pDemandClient
                        , GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item), pDemandClient->ro.starttime, pDemandClient->ro.endtime);

#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                continue;
            }
            double jumpTime = pDemandClient->rw.jumpTime;
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif

                }
            }
            pMonitorItem->rw.line = __LINE__;
            FsLocal_ShareBuffer_resize(strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/"));
            const unsigned int dirLen = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + dirLen + 1);
            pDemandClient->rw.externP = pVideoDemand = record_P_VideoDemand_new__IO(pRecord_item, pDemandClient->rw.sendType, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, jumpTime, 0, fps, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            pMonitorItem->rw.line = __LINE__;
            // 获取回放对象（pVideoDemand）失败,continue
            if (NULL == pVideoDemand) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog(FsLogType_error, "New from the pDemandClient(=%p) error,uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                        , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
                if (FsLogTypeCheck(FsLogType_error)) {
                    unsigned int ui = 0;
                    for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                        printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                    }
                }
#else
                Record_itemLog(FsLogType_error, "New from the pDemandClient(=%p) error,uri:\"%s\",name=\"%s\",starttime:%u,endtime:%u\n", pDemandClient
                        , GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item), pDemandClient->ro.starttime, pDemandClient->ro.endtime);
#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                continue;
            }
#ifdef __record_P_item_do_videoDemand_gb28181 
            //设置fps
            FsPrintf(1, "gb28181 scale: %lf\r\n", pDemandClient->p._scale);
            FsPrintf(1, "gb28181 fps: %lf\r\n", pVideoDemand->fps);
            //pVideoDemand->fps *= pDemandClient->ro._scale;
            FsPrintf(1, "gb28181 fps: %lf\r\n", pVideoDemand->fps);
#endif
            pMonitorItem->rw.line = __LINE__;
            pDemandClient->rw.deleteExternP = (void (*)(void*))record_P_VideoDemand_delete__OI;
            pMonitorItem->rw.line = __LINE__;
        } else pVideoDemand = (struct Record_P_VideoDemand*) pDemandClient->rw.externP;
        pMonitorItem->rw.line = __LINE__;
        // 解析文件信息
        if (0 == pVideoDemand->status) {
            double jumpTime = pDemandClient->rw.jumpTime;
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif

                }
            }
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pDemandClient->rw.sendType, jumpTime, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for init failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for init failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
            /* 成功了 */
            pDemandClient->rw.jumpTime = 0.0;
            pVideoDemand->status = 1;
            /* 设置掩码 */
            {
                unsigned short ui = pVideoDemand->pMKV->videoCount/* + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount*/;
                for (; ui > 0; pDemandClient->rw.allMask |= (1U << --ui));
            }
            /* 初始化缓存 */
            pDemandClient->rw.sendBufferCountMaxIn = 64;
            pDemandClient->rw.sendBufferCountMaxOut = 64;
        }
        // 改变发送模式
        if (pVideoDemand->sendType != pDemandClient->rw.sendType) {
            /* 改变发送模式 */
            Record_itemLog2(FsLogType_info, "Change sendType:%hhu/%hhu", pVideoDemand->sendType, pDemandClient->rw.sendType);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pDemandClient->rw.sendType, pDemandClient->rw.jumpTime, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for sendType failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for sendType failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
        }
        pMonitorItem->rw.line = __LINE__;
        // 跳到指定时间段, 成功后continue,处理下一个
        if (pDemandClient->rw.jumpTime > 0.0) {
            /* 跳转 */
            double jumpTime = pDemandClient->rw.jumpTime;
            Record_itemLog2(FsLogType_info, "jumpTime:%lf", pDemandClient->rw.jumpTime);
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                }
                FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
                const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pVideoDemand->sendType, jumpTime, thisUptime, &FsLocal_ShareBuffer);
                FsLocal_ShareBuffer_call_after();
                if (rv != 1) {
                    if (-101 == rv) {
                        /* 还需要等待 */
                    } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                        Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for jump failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                                , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                        Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for jump failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                                , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                        fflush(stdout);
                        pDemandClient->rw.error = -1;
                        /* 先把不用的内存删了 */
                        record_P_VideoDemand_delete__OI(pVideoDemand);
                        pDemandClient->rw.deleteExternP = NULL;
                    }
                    continue;
                }
            }
            pDemandClient->rw.jumpTime = 0.0;
        }
        pMonitorItem->rw.line = __LINE__;
        if ((pVideoDemand->pMKV->videoIndex > 0 && pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount == pVideoDemand->sendCount) ||
                (pVideoDemand->pMKV->videoIndex < 1 && pVideoDemand->pMKV->audioIndex > 0 && pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount == pVideoDemand->sendCount)) {
            /* 没有数据发了 */
            pMonitorItem->rw.line = __LINE__;
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_readNextFile(pVideoDemand, pRecord_item->ro._recordVideoMode, 0, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update failed,thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pVideoDemand->thisFileName, pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update failed,thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pVideoDemand->thisFileName, pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
            if (pRecord_item->ro._realVideoSaveTime > 0 && pVideoDemand->fileStartTime + pRecord_item->ro._realVideoSaveTime < fs_time_GMT_get()) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog2(FsLogType_info, "fileStartTime(=%lf) out of realVideoSaveTime(=%u),thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n"
                        , pVideoDemand->fileStartTime, pRecord_item->ro._realVideoSaveTime, pVideoDemand->thisFileName, pDemandClient
                        , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                Record_itemLog2(FsLogType_info, "fileStartTime(=%lf) out of realVideoSaveTime(=%u),thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n"
                        , pVideoDemand->fileStartTime, pRecord_item->ro._realVideoSaveTime, pVideoDemand->thisFileName, pDemandClient
                        , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                /* 先把不用的内存删了 */
                record_P_VideoDemand_delete__OI(pVideoDemand);
                pDemandClient->rw.deleteExternP = NULL;
                continue;
            }
            pMonitorItem->rw.line = __LINE__;
        } else if (/* 是远程调用 */Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode) && /* 没有使用 */Record_P_VideoDemand_tag_is_not_use(pVideoDemand)) {
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            record_P_VideoDemand_update_write_tag(pVideoDemand, pVideoDemand->sendType, pVideoDemand->fileStartTime, pVideoDemand->fileStartTime + ((pVideoDemand->sendType & 0x1) == 0 ? pVideoDemand->fileTime : 0.0), thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
        }
        pMonitorItem->rw.line = __LINE__;
        /* 记算本次要发送的帧数 */
        long sendCount = (pVideoDemand->endUptime - fs_time_uptime_get()) * pVideoDemand->fps;
        if (sendCount < 0)sendCount = 0;
        sendCount = (pVideoDemand->pMKV->videoIndex > 0 ? pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount : pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount)
                - pVideoDemand->sendCount - sendCount;
        if (sendCount > 0) {
            //FsPrintf(1, "00000000000000,%ld,%lu,%hhu/%hhu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn);
            if (pDemandClient->ro.__dataList->nodeCount <= (((unsigned long) pDemandClient->rw.sendBufferCountMaxOut) >> 1)) {
                if (pDemandClient->rw.sendBufferCountMaxOut < 250 && pDemandClient->rw.sendBufferCountMaxOut < sendCount + pDemandClient->ro.__dataList->nodeCount)pDemandClient->rw.sendBufferCountMaxOut += 4;
            } else if (pDemandClient->ro.__dataList->nodeCount < pDemandClient->rw.sendBufferCountMaxOut)pDemandClient->rw.sendBufferCountMaxOut--;
            const unsigned long count = pDemandClient->rw.sendBufferCountMaxOut + pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount;
            sendCount = pDemandClient->ro.__dataList->nodeCount < count ? (sendCount + pDemandClient->ro.__dataList->nodeCount < count ? sendCount : (long) (count - pDemandClient->ro.__dataList->nodeCount)) : 0;
            //            i = (pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) << 4;
            //            sendCount = pDemandClient->ro.__dataList->nodeCount < i ? (sendCount + pDemandClient->ro.__dataList->nodeCount < i ? sendCount : i - pDemandClient->ro.__dataList->nodeCount) : 0;
            //FsPrintf(1, "sendCount=%d\n", sendCount);
            //fflush(stdout);
            //FsPrintf(1, "11111111111111111,%ld,%lu,%hhu/%hhu,%lu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn, i);
            if (sendCount > 0) {
                /* 发数据 */
                //FsPrintf(1, "222222222222222,%ld,%lu,%hhu/%hhu,%lu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn, i);

                void (*const dataSend[]) (struct Record_P_VideoDemand * const pVideoDemand,
                        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) = {
                    /* 正序正常发送 */
                    record_P_item_do_videoDemand_send_0,
                    /* 倒序正常发送 */
                    record_P_item_do_videoDemand_send_1,
                    /* 正序关键帧发送 */
                    record_P_item_do_videoDemand_send_2,
                    /* 倒序关键帧发送 */
                    record_P_item_do_videoDemand_send_3,
                };
                dataSend[pVideoDemand->sendType & 0x3](pVideoDemand, (void (* const) (void * const, void * const))__record_P_item_do_videoDemand_add_frame__OI_2, pDemandClient, sendCount);
            }
        }
#undef __record_P_item_do_videoDemand_add_frame__OI_2
#ifdef __record_P_item_do_videoDemand_gb28181
#undef __record_P_item_do_videoDemand_gb28181
#endif
#endif
    }
    pthread_mutex_unlock(&pRecord_item->ro.__pRtspServer_item->rw.__mutex_demandClientList);
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
}

/* 视频点播 */
static inline void record_P_item_do_videoDemand_for_hls(/* 记录项 */struct Record_item * const pRecord_item, const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
    pMonitorItem->rw.line = __LINE__;
    struct HlsServer_item * const pHlsServer_item = pRecord_item->ro.__pHlsServer_item;
    if (NULL == pHlsServer_item || 0 == (pHlsServer_item->r._modelType & 0x2) || 0 == pHlsServer_item->rw.__demandClientListOrder__newClientList->nodeCount)return;
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
    pthread_mutex_lock(&pHlsServer_item->rw.__newClientList->mutex);
    unsigned int l = pHlsServer_item->rw.__demandClientListOrder__newClientList->nodeCount;
    void **ppClient_node = pHlsServer_item->rw.__demandClientListOrder__newClientList->pNode + pHlsServer_item->rw.__demandClientListOrder__newClientList->startIndex;
    for (; l > 0; l--, ppClient_node++) {
        struct HlsServer_item_DemandClient * const pDemandClient = HlsServer_DemandClient_get(*ppClient_node);
        //        FsPrintf(1, "a videoDemand,uri:\"%s\",parameterCount=%u.\n", pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
        //        fflush(stdout);
#define __record_P_item_do_videoDemand_add_frame__OI_2 hlsServer_item_DemandClient_add_frame__OI_2
#ifndef __record_P_item_do_videoDemand 
        pMonitorItem->rw.line = __LINE__;
        struct Record_P_VideoDemand * pVideoDemand;
        // 创建回放对象
        if (NULL == pDemandClient->rw.deleteExternP) {
            pMonitorItem->rw.line = __LINE__;
            if (pDemandClient->rw.error != 0)continue;
#ifndef __record_P_item_do_videoDemand_gb28181
            Record_itemLog(FsLogType_matchError, "pDemandClient(=%p),uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                    , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
            if (FsLogTypeCheck(FsLogType_matchError)) {
                unsigned int ui = 0;
                for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                    printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                }
            }
#endif
            /* 初始化 */
            // 获取时间
#ifndef __record_P_item_do_videoDemand_gb28181
            double fps = -1.0;
            pMonitorItem->rw.line = __LINE__;
            // 获取参数fps参数
            {
                unsigned int ui = 0;
                for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                    if (0 == strcmp("fps", pDemandClient->ro.__pUrlParameter->parameter[ui][0])) {
                        if (pDemandClient->ro.__pUrlParameter->parameter[ui][1]) fps = atof(pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                        Record_itemLog2(FsLogType_info, "fps:\"%s\"/%lf.\n", pDemandClient->ro.__pUrlParameter->parameter[ui][1], fps);
                        break;
                    }
                }
            }
#else
            double fps = 25.0 * 16.0;
#endif
            pMonitorItem->rw.line = __LINE__;
            // 没有设置startTime,不允许点播
            if (pDemandClient->rw.jumpTime < 0.00001) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog(FsLogType_matchError, "Get start time from the pDemandClient(=%p) error,uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                        , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
                if (FsLogTypeCheck(FsLogType_matchError)) {
                    unsigned int ui = 0;
                    for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                        printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                    }
                }
#else
                Record_itemLog(FsLogType_matchError, "Get start time from the pDemandClient(=%p) error,uri:\"%s\",name=\"%s\",starttime:%u, endtime:%u\n", pDemandClient
                        , GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item), pDemandClient->ro.starttime, pDemandClient->ro.endtime);

#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                continue;
            }
            double jumpTime = pDemandClient->rw.jumpTime;
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif

                }
            }
            pMonitorItem->rw.line = __LINE__;
            FsLocal_ShareBuffer_resize(strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/"));
            const unsigned int dirLen = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + dirLen + 1);
            pDemandClient->rw.externP = pVideoDemand = record_P_VideoDemand_new__IO(pRecord_item, pDemandClient->rw.sendType, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, jumpTime, 0, fps, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            pMonitorItem->rw.line = __LINE__;
            // 获取回放对象（pVideoDemand）失败,continue
            if (NULL == pVideoDemand) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog(FsLogType_error, "New from the pDemandClient(=%p) error,uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                        , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
                if (FsLogTypeCheck(FsLogType_error)) {
                    unsigned int ui = 0;
                    for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                        printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                    }
                }
#else
                Record_itemLog(FsLogType_error, "New from the pDemandClient(=%p) error,uri:\"%s\",name=\"%s\",starttime:%u,endtime:%u\n", pDemandClient
                        , GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item), pDemandClient->ro.starttime, pDemandClient->ro.endtime);
#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                continue;
            }
#ifdef __record_P_item_do_videoDemand_gb28181 
            //设置fps
            FsPrintf(1, "gb28181 scale: %lf\r\n", pDemandClient->p._scale);
            FsPrintf(1, "gb28181 fps: %lf\r\n", pVideoDemand->fps);
            //pVideoDemand->fps *= pDemandClient->ro._scale;
            FsPrintf(1, "gb28181 fps: %lf\r\n", pVideoDemand->fps);
#endif
            pMonitorItem->rw.line = __LINE__;
            pDemandClient->rw.deleteExternP = (void (*)(void*))record_P_VideoDemand_delete__OI;
            pMonitorItem->rw.line = __LINE__;
        } else pVideoDemand = (struct Record_P_VideoDemand*) pDemandClient->rw.externP;
        pMonitorItem->rw.line = __LINE__;
        // 解析文件信息
        if (0 == pVideoDemand->status) {
            double jumpTime = pDemandClient->rw.jumpTime;
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif

                }
            }
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pDemandClient->rw.sendType, jumpTime, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for init failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for init failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
            /* 成功了 */
            pDemandClient->rw.jumpTime = 0.0;
            pVideoDemand->status = 1;
            /* 设置掩码 */
            {
                unsigned short ui = pVideoDemand->pMKV->videoCount/* + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount*/;
                for (; ui > 0; pDemandClient->rw.allMask |= (1U << --ui));
            }
            /* 初始化缓存 */
            pDemandClient->rw.sendBufferCountMaxIn = 64;
            pDemandClient->rw.sendBufferCountMaxOut = 64;
        }
        // 改变发送模式
        if (pVideoDemand->sendType != pDemandClient->rw.sendType) {
            /* 改变发送模式 */
            Record_itemLog2(FsLogType_info, "Change sendType:%hhu/%hhu", pVideoDemand->sendType, pDemandClient->rw.sendType);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pDemandClient->rw.sendType, pDemandClient->rw.jumpTime, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for sendType failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for sendType failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
        }
        pMonitorItem->rw.line = __LINE__;
        // 跳到指定时间段, 成功后continue,处理下一个
        if (pDemandClient->rw.jumpTime > 0.0) {
            /* 跳转 */
            double jumpTime = pDemandClient->rw.jumpTime;
            Record_itemLog2(FsLogType_info, "jumpTime:%lf", pDemandClient->rw.jumpTime);
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                }
                FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
                const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pVideoDemand->sendType, jumpTime, thisUptime, &FsLocal_ShareBuffer);
                FsLocal_ShareBuffer_call_after();
                if (rv != 1) {
                    if (-101 == rv) {
                        /* 还需要等待 */
                    } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                        Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for jump failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                                , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                        Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for jump failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                                , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                        fflush(stdout);
                        pDemandClient->rw.error = -1;
                        /* 先把不用的内存删了 */
                        record_P_VideoDemand_delete__OI(pVideoDemand);
                        pDemandClient->rw.deleteExternP = NULL;
                    }
                    continue;
                }
            }
            pDemandClient->rw.jumpTime = 0.0;
        }
        pMonitorItem->rw.line = __LINE__;
        if ((pVideoDemand->pMKV->videoIndex > 0 && pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount == pVideoDemand->sendCount) ||
                (pVideoDemand->pMKV->videoIndex < 1 && pVideoDemand->pMKV->audioIndex > 0 && pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount == pVideoDemand->sendCount)) {
            /* 没有数据发了 */
            pMonitorItem->rw.line = __LINE__;
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_readNextFile(pVideoDemand, pRecord_item->ro._recordVideoMode, 0, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update failed,thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pVideoDemand->thisFileName, pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update failed,thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pVideoDemand->thisFileName, pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
            if (pRecord_item->ro._realVideoSaveTime > 0 && pVideoDemand->fileStartTime + pRecord_item->ro._realVideoSaveTime < fs_time_GMT_get()) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog2(FsLogType_info, "fileStartTime(=%lf) out of realVideoSaveTime(=%u),thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n"
                        , pVideoDemand->fileStartTime, pRecord_item->ro._realVideoSaveTime, pVideoDemand->thisFileName, pDemandClient
                        , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                Record_itemLog2(FsLogType_info, "fileStartTime(=%lf) out of realVideoSaveTime(=%u),thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n"
                        , pVideoDemand->fileStartTime, pRecord_item->ro._realVideoSaveTime, pVideoDemand->thisFileName, pDemandClient
                        , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                /* 先把不用的内存删了 */
                record_P_VideoDemand_delete__OI(pVideoDemand);
                pDemandClient->rw.deleteExternP = NULL;
                continue;
            }
            pMonitorItem->rw.line = __LINE__;
        } else if (/* 是远程调用 */Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode) && /* 没有使用 */Record_P_VideoDemand_tag_is_not_use(pVideoDemand)) {
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            record_P_VideoDemand_update_write_tag(pVideoDemand, pVideoDemand->sendType, pVideoDemand->fileStartTime, pVideoDemand->fileStartTime + ((pVideoDemand->sendType & 0x1) == 0 ? pVideoDemand->fileTime : 0.0), thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
        }
        pMonitorItem->rw.line = __LINE__;
        /* 记算本次要发送的帧数 */
        long sendCount = (pVideoDemand->endUptime - fs_time_uptime_get()) * pVideoDemand->fps;
        if (sendCount < 0)sendCount = 0;
        sendCount = (pVideoDemand->pMKV->videoIndex > 0 ? pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount : pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount)
                - pVideoDemand->sendCount - sendCount;
        if (sendCount > 0) {
            //FsPrintf(1, "00000000000000,%ld,%lu,%hhu/%hhu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn);
            if (pDemandClient->ro.__dataList->nodeCount <= (((unsigned long) pDemandClient->rw.sendBufferCountMaxOut) >> 1)) {
                if (pDemandClient->rw.sendBufferCountMaxOut < 250 && pDemandClient->rw.sendBufferCountMaxOut < sendCount + pDemandClient->ro.__dataList->nodeCount)pDemandClient->rw.sendBufferCountMaxOut += 4;
            } else if (pDemandClient->ro.__dataList->nodeCount < pDemandClient->rw.sendBufferCountMaxOut)pDemandClient->rw.sendBufferCountMaxOut--;
            const unsigned long count = pDemandClient->rw.sendBufferCountMaxOut + pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount;
            sendCount = pDemandClient->ro.__dataList->nodeCount < count ? (sendCount + pDemandClient->ro.__dataList->nodeCount < count ? sendCount : (long) (count - pDemandClient->ro.__dataList->nodeCount)) : 0;
            //            i = (pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) << 4;
            //            sendCount = pDemandClient->ro.__dataList->nodeCount < i ? (sendCount + pDemandClient->ro.__dataList->nodeCount < i ? sendCount : i - pDemandClient->ro.__dataList->nodeCount) : 0;
            //FsPrintf(1, "sendCount=%d\n", sendCount);
            //fflush(stdout);
            //FsPrintf(1, "11111111111111111,%ld,%lu,%hhu/%hhu,%lu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn, i);
            if (sendCount > 0) {
                /* 发数据 */
                //FsPrintf(1, "222222222222222,%ld,%lu,%hhu/%hhu,%lu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn, i);

                void (*const dataSend[]) (struct Record_P_VideoDemand * const pVideoDemand,
                        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) = {
                    /* 正序正常发送 */
                    record_P_item_do_videoDemand_send_0,
                    /* 倒序正常发送 */
                    record_P_item_do_videoDemand_send_1,
                    /* 正序关键帧发送 */
                    record_P_item_do_videoDemand_send_2,
                    /* 倒序关键帧发送 */
                    record_P_item_do_videoDemand_send_3,
                };
                dataSend[pVideoDemand->sendType & 0x3](pVideoDemand, (void (* const) (void * const, void * const))__record_P_item_do_videoDemand_add_frame__OI_2, pDemandClient, sendCount);
            }
        }
#undef __record_P_item_do_videoDemand_add_frame__OI_2
#ifdef __record_P_item_do_videoDemand_gb28181
#undef __record_P_item_do_videoDemand_gb28181
#endif
#endif
    }
    pthread_mutex_unlock(&pHlsServer_item->rw.__newClientList->mutex);
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
}

/* 查询录像时长的回调函数,成功返回FsObjectList指针(成员为两个double,第一个double为开始时间,第二个double为结束时间),失败返回NULL */
static FsStructList * record_P_item_recordInfo_for_GB28181__IO(/* 查找的开始时间 */double startTime, /* 查找的结束时间 */double endTime, struct Record_item * const pRecord_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    /* 不记录视频 */
    if (0 == pRecord_item->ro._recordVideoMode) {
        FsPrintf2(1, "uuid: %s, no record video,startTime=%lf,endTime=%lf\r\n", pRecord_item->ro._uuid, startTime, endTime);
        return NULL;
    }
    // 强制更新录像时间
    pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
    record_P_videoInfo_update(pRecord_item, 2, 0.0, pShareBuffer);
    pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
    /* 限制储存时长 */
    double thisTimeGMT = fs_time_GMT_get();
    if (pRecord_item->ro._realVideoSaveTime && startTime < thisTimeGMT - pRecord_item->ro._realVideoSaveTime) {
        startTime = thisTimeGMT - pRecord_item->ro._realVideoSaveTime;
    }
    FsStructList * const rst = fs_StructList_new__IO(4, sizeof (double)*2);
    double data[2];
    unsigned int i = Record_P_videoInfo_HeadCount, j = pRecord_item->ro._videoInfo_pos->childNodeCount, k;
    struct FsEbml_node** child = pRecord_item->ro._videoInfo_pos->child;
    pthread_mutex_lock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    /* 查找开始位置 */
    while (i < j) {
        k = (i + j) >> 1;
        if (child[k]->childNodeCount > 1 && startTime>*(double*) child[k]->child[1]->data.buffer)i = k + 1;
        else j = k;
    }
    //FsPrintf(1, "i=%u/%lu\n", i, pRecord_item->p.videoInfo_pos->childNodeCount);
    for (j = pRecord_item->ro._videoInfo_pos->childNodeCount; i < j; i++) {
        data[0] = *(double*) child[i]->child[0]->data.buffer;
        if (data[0] < startTime)data[0] = startTime;
        if (child[i]->childNodeCount > 1)data[1] = *(double*) child[i]->child[1]->data.buffer;
        else data[1] = thisTimeGMT;
        if (data[1] > endTime)data[1] = endTime;
        if (data[1] > startTime) {
            if (data[0] < endTime) {
                fs_StructList_insert_tail(rst, data);
            } else break;
        }
    }
    pthread_mutex_unlock(&pRecord_item->ro._pRecord->ro.__videoInfoDataClientList->mutex);
    if (0 == rst->nodeCount) {
        fs_StructList_delete__OI(rst);
        FsPrintf2(1, "No data for uuid:\"%s\",startTime=%lf,endTime=%lf\r\n", pRecord_item->ro._uuid, startTime, endTime);

        return NULL;
    }
    return rst;
}

/* GB28181视频点播 */
static inline void record_P_item_do_videoDemand_for_GB28181(/* 记录项 */struct Record_item * const pRecord_item, const double thisUptime
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer, MonitorItem * const pMonitorItem) {
    pMonitorItem->rw.line = __LINE__;
    struct GB28181Server_item * const pGB28181Server_item = pRecord_item->ro.__pGB28181Server_item;
    if (NULL == pGB28181Server_item || 0 == (pGB28181Server_item->r._modelType & 0x2) || 0 == Fs_CycleList_get_count(pGB28181Server_item->rw.__demandClientList))return;
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
    pthread_mutex_lock(&pRecord_item->ro.__pGB28181Server_item->rw.__mutex_demandClientList);
    unsigned int l = Fs_CycleList_get_count(pRecord_item->ro.__pGB28181Server_item->rw.__demandClientList);
    void *pClient = Fs_CycleList_get_head(pRecord_item->ro.__pGB28181Server_item->rw.__demandClientList);
    //    FsPrintf(1, "jinhuan, lock gb28181 demand client list count: %d\r\n", l);
    for (; l > 0; l--, pClient = Fs_CycleList_node_get_next(pClient)) {
        struct GB28181Server_item_DemandClient * const pDemandClient = GB28181Server_DemandClient_get(pClient);
        pMonitorItem->rw.line = __LINE__;
#define __record_P_item_do_videoDemand_gb28181
#define __record_P_item_do_videoDemand_add_frame__OI_2 gb28181Server_item_DemandClient_add_frame__OI_2
#ifndef __record_P_item_do_videoDemand 
        pMonitorItem->rw.line = __LINE__;
        struct Record_P_VideoDemand * pVideoDemand;
        // 创建回放对象
        if (NULL == pDemandClient->rw.deleteExternP) {
            pMonitorItem->rw.line = __LINE__;
            if (pDemandClient->rw.error != 0)continue;
#ifndef __record_P_item_do_videoDemand_gb28181
            Record_itemLog(FsLogType_matchError, "pDemandClient(=%p),uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                    , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
            if (FsLogTypeCheck(FsLogType_matchError)) {
                unsigned int ui = 0;
                for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                    printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                }
            }
#endif
            /* 初始化 */
            // 获取时间
#ifndef __record_P_item_do_videoDemand_gb28181
            double fps = -1.0;
            pMonitorItem->rw.line = __LINE__;
            // 获取参数fps参数
            {
                unsigned int ui = 0;
                for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                    if (0 == strcmp("fps", pDemandClient->ro.__pUrlParameter->parameter[ui][0])) {
                        if (pDemandClient->ro.__pUrlParameter->parameter[ui][1]) fps = atof(pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                        Record_itemLog2(FsLogType_info, "fps:\"%s\"/%lf.\n", pDemandClient->ro.__pUrlParameter->parameter[ui][1], fps);
                        break;
                    }
                }
            }
#else
            double fps = 25.0 * 16.0;
#endif
            pMonitorItem->rw.line = __LINE__;
            // 没有设置startTime,不允许点播
            if (pDemandClient->rw.jumpTime < 0.00001) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog(FsLogType_matchError, "Get start time from the pDemandClient(=%p) error,uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                        , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
                if (FsLogTypeCheck(FsLogType_matchError)) {
                    unsigned int ui = 0;
                    for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                        printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                    }
                }
#else
                Record_itemLog(FsLogType_matchError, "Get start time from the pDemandClient(=%p) error,uri:\"%s\",name=\"%s\",starttime:%u, endtime:%u\n", pDemandClient
                        , GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item), pDemandClient->ro.starttime, pDemandClient->ro.endtime);

#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                continue;
            }
            double jumpTime = pDemandClient->rw.jumpTime;
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif

                }
            }
            pMonitorItem->rw.line = __LINE__;
            FsLocal_ShareBuffer_resize(strlen(pRecord_item->ro._pRecord->ro._dir) + strlen(pRecord_item->ro._uuid) + sizeof ("/video/"));
            const unsigned int dirLen = sprintf(FsLocal_ShareBuffer + FsLocal_ShareBufferPos, "%s%s/video/", pRecord_item->ro._pRecord->ro._dir, pRecord_item->ro._uuid);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos + dirLen + 1);
            pDemandClient->rw.externP = pVideoDemand = record_P_VideoDemand_new__IO(pRecord_item, pDemandClient->rw.sendType, FsLocal_ShareBuffer + FsLocal_ShareBufferPos, jumpTime, 0, fps, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            pMonitorItem->rw.line = __LINE__;
            // 获取回放对象（pVideoDemand）失败,continue
            if (NULL == pVideoDemand) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog(FsLogType_error, "New from the pDemandClient(=%p) error,uri:\"%s\",parameterCount=%u,data:\n", pDemandClient
                        , pDemandClient->ro.__pUrlParameter->uri, pDemandClient->ro.__pUrlParameter->parameterCount);
                if (FsLogTypeCheck(FsLogType_error)) {
                    unsigned int ui = 0;
                    for (; ui < pDemandClient->ro.__pUrlParameter->parameterCount; ui++) {
                        printf("parameter[%u]:name:\"%s\",value:\"%s\".\n", ui, pDemandClient->ro.__pUrlParameter->parameter[ui][0], pDemandClient->ro.__pUrlParameter->parameter[ui][1]);
                    }
                }
#else
                Record_itemLog(FsLogType_error, "New from the pDemandClient(=%p) error,uri:\"%s\",name=\"%s\",starttime:%u,endtime:%u\n", pDemandClient
                        , GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item), pDemandClient->ro.starttime, pDemandClient->ro.endtime);
#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                continue;
            }
#ifdef __record_P_item_do_videoDemand_gb28181 
            //设置fps
            FsPrintf(1, "gb28181 scale: %lf\r\n", pDemandClient->p._scale);
            FsPrintf(1, "gb28181 fps: %lf\r\n", pVideoDemand->fps);
            //pVideoDemand->fps *= pDemandClient->ro._scale;
            FsPrintf(1, "gb28181 fps: %lf\r\n", pVideoDemand->fps);
#endif
            pMonitorItem->rw.line = __LINE__;
            pDemandClient->rw.deleteExternP = (void (*)(void*))record_P_VideoDemand_delete__OI;
            pMonitorItem->rw.line = __LINE__;
        } else pVideoDemand = (struct Record_P_VideoDemand*) pDemandClient->rw.externP;
        pMonitorItem->rw.line = __LINE__;
        // 解析文件信息
        if (0 == pVideoDemand->status) {
            double jumpTime = pDemandClient->rw.jumpTime;
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif

                }
            }
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pDemandClient->rw.sendType, jumpTime, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for init failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for init failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
            /* 成功了 */
            pDemandClient->rw.jumpTime = 0.0;
            pVideoDemand->status = 1;
            /* 设置掩码 */
            {
                unsigned short ui = pVideoDemand->pMKV->videoCount/* + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount*/;
                for (; ui > 0; pDemandClient->rw.allMask |= (1U << --ui));
            }
            /* 初始化缓存 */
            pDemandClient->rw.sendBufferCountMaxIn = 64;
            pDemandClient->rw.sendBufferCountMaxOut = 64;
        }
        // 改变发送模式
        if (pVideoDemand->sendType != pDemandClient->rw.sendType) {
            /* 改变发送模式 */
            Record_itemLog2(FsLogType_info, "Change sendType:%hhu/%hhu", pVideoDemand->sendType, pDemandClient->rw.sendType);
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pDemandClient->rw.sendType, pDemandClient->rw.jumpTime, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for sendType failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for sendType failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
        }
        pMonitorItem->rw.line = __LINE__;
        // 跳到指定时间段, 成功后continue,处理下一个
        if (pDemandClient->rw.jumpTime > 0.0) {
            /* 跳转 */
            double jumpTime = pDemandClient->rw.jumpTime;
            Record_itemLog2(FsLogType_info, "jumpTime:%lf", pDemandClient->rw.jumpTime);
            {
                double gmtTime;
                if (pRecord_item->ro._realVideoSaveTime > 0 && jumpTime + pRecord_item->ro._realVideoSaveTime < (gmtTime = fs_time_GMT_get())) {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset old jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    jumpTime = gmtTime - pRecord_item->ro._realVideoSaveTime;
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog2(FsLogType_info, "Reset new startTime=%lf,realVideoSaveTime=%u,uri:\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog2(FsLogType_info, "Reset new jumpTime=%lf,realVideoSaveTime=%u,uri:\"%s\",name=\"%s\".\n", jumpTime, pRecord_item->ro._realVideoSaveTime, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                }
                FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
                const int rv = record_P_VideoDemand_update(pVideoDemand, pRecord_item->ro._recordVideoMode, pVideoDemand->sendType, jumpTime, thisUptime, &FsLocal_ShareBuffer);
                FsLocal_ShareBuffer_call_after();
                if (rv != 1) {
                    if (-101 == rv) {
                        /* 还需要等待 */
                    } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                        Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for jump failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pDemandClient
                                , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                        Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update for jump failed,pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pDemandClient
                                , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                        fflush(stdout);
                        pDemandClient->rw.error = -1;
                        /* 先把不用的内存删了 */
                        record_P_VideoDemand_delete__OI(pVideoDemand);
                        pDemandClient->rw.deleteExternP = NULL;
                    }
                    continue;
                }
            }
            pDemandClient->rw.jumpTime = 0.0;
        }
        pMonitorItem->rw.line = __LINE__;
        if ((pVideoDemand->pMKV->videoIndex > 0 && pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount == pVideoDemand->sendCount) ||
                (pVideoDemand->pMKV->videoIndex < 1 && pVideoDemand->pMKV->audioIndex > 0 && pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount == pVideoDemand->sendCount)) {
            /* 没有数据发了 */
            pMonitorItem->rw.line = __LINE__;
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            const int rv = record_P_VideoDemand_readNextFile(pVideoDemand, pRecord_item->ro._recordVideoMode, 0, thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
            if (rv != 1) {
                if (-101 == rv) {
                    /* 还需要等待 */
                } else {
#ifndef __record_P_item_do_videoDemand_gb28181
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update failed,thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n", pVideoDemand->thisFileName, pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                    Record_itemLog(FsLogType_error, "Do record_P_VideoDemand_update failed,thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n", pVideoDemand->thisFileName, pDemandClient
                            , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                    fflush(stdout);
                    pDemandClient->rw.error = -1;
                    /* 先把不用的内存删了 */
                    record_P_VideoDemand_delete__OI(pVideoDemand);
                    pDemandClient->rw.deleteExternP = NULL;
                }
                continue;
            }
            if (pRecord_item->ro._realVideoSaveTime > 0 && pVideoDemand->fileStartTime + pRecord_item->ro._realVideoSaveTime < fs_time_GMT_get()) {
#ifndef __record_P_item_do_videoDemand_gb28181
                Record_itemLog2(FsLogType_info, "fileStartTime(=%lf) out of realVideoSaveTime(=%u),thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\".\n"
                        , pVideoDemand->fileStartTime, pRecord_item->ro._realVideoSaveTime, pVideoDemand->thisFileName, pDemandClient
                        , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, pDemandClient->ro.__pUrlParameter->uri);
#else
                Record_itemLog2(FsLogType_info, "fileStartTime(=%lf) out of realVideoSaveTime(=%u),thisfile:\"%s\",pDemandClient=%p,jumpTime=%lf,sendType=%hhu,uri:\"%s\",name=\"%s\".\n"
                        , pVideoDemand->fileStartTime, pRecord_item->ro._realVideoSaveTime, pVideoDemand->thisFileName, pDemandClient
                        , pDemandClient->rw.jumpTime, pDemandClient->rw.sendType, GB28181Server_item_get_url(pGB28181Server_item), GB28181Server_item_get_name(pGB28181Server_item));
#endif
                fflush(stdout);
                pDemandClient->rw.error = -1;
                /* 先把不用的内存删了 */
                record_P_VideoDemand_delete__OI(pVideoDemand);
                pDemandClient->rw.deleteExternP = NULL;
                continue;
            }
            pMonitorItem->rw.line = __LINE__;
        } else if (/* 是远程调用 */Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode) && /* 没有使用 */Record_P_VideoDemand_tag_is_not_use(pVideoDemand)) {
            FsLocal_ShareBuffer_call_before(FsLocal_ShareBufferPos);
            record_P_VideoDemand_update_write_tag(pVideoDemand, pVideoDemand->sendType, pVideoDemand->fileStartTime, pVideoDemand->fileStartTime + ((pVideoDemand->sendType & 0x1) == 0 ? pVideoDemand->fileTime : 0.0), thisUptime, &FsLocal_ShareBuffer);
            FsLocal_ShareBuffer_call_after();
        }
        pMonitorItem->rw.line = __LINE__;
        /* 记算本次要发送的帧数 */
        long sendCount = (pVideoDemand->endUptime - fs_time_uptime_get()) * pVideoDemand->fps;
        if (sendCount < 0)sendCount = 0;
        sendCount = (pVideoDemand->pMKV->videoIndex > 0 ? pVideoDemand->pMKV->channel[pVideoDemand->pMKV->videoIndex].list->nodeCount : pVideoDemand->pMKV->channel[pVideoDemand->pMKV->audioIndex].list->nodeCount)
                - pVideoDemand->sendCount - sendCount;
        if (sendCount > 0) {
            //FsPrintf(1, "00000000000000,%ld,%lu,%hhu/%hhu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn);
            if (pDemandClient->ro.__dataList->nodeCount <= (((unsigned long) pDemandClient->rw.sendBufferCountMaxOut) >> 1)) {
                if (pDemandClient->rw.sendBufferCountMaxOut < 250 && pDemandClient->rw.sendBufferCountMaxOut < sendCount + pDemandClient->ro.__dataList->nodeCount)pDemandClient->rw.sendBufferCountMaxOut += 4;
            } else if (pDemandClient->ro.__dataList->nodeCount < pDemandClient->rw.sendBufferCountMaxOut)pDemandClient->rw.sendBufferCountMaxOut--;
            const unsigned long count = pDemandClient->rw.sendBufferCountMaxOut + pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount;
            sendCount = pDemandClient->ro.__dataList->nodeCount < count ? (sendCount + pDemandClient->ro.__dataList->nodeCount < count ? sendCount : (long) (count - pDemandClient->ro.__dataList->nodeCount)) : 0;
            //            i = (pVideoDemand->pMKV->videoCount + pVideoDemand->pMKV->audioCount + pVideoDemand->pMKV->subtitleCount) << 4;
            //            sendCount = pDemandClient->ro.__dataList->nodeCount < i ? (sendCount + pDemandClient->ro.__dataList->nodeCount < i ? sendCount : i - pDemandClient->ro.__dataList->nodeCount) : 0;
            //FsPrintf(1, "sendCount=%d\n", sendCount);
            //fflush(stdout);
            //FsPrintf(1, "11111111111111111,%ld,%lu,%hhu/%hhu,%lu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn, i);
            if (sendCount > 0) {
                /* 发数据 */
                //FsPrintf(1, "222222222222222,%ld,%lu,%hhu/%hhu,%lu\n", sendCount, pDemandClient->ro.__dataList->nodeCount, pDemandClient->rw.sendBufferCountMaxOut, pDemandClient->rw.sendBufferCountMaxIn, i);

                void (*const dataSend[]) (struct Record_P_VideoDemand * const pVideoDemand,
                        void (*const add_frame__OI_2) (void * const pDemandClient, /* 要添加的数据,参考具体的函数的注释 */ void *const pObject), void * const pDemandClient, long sendCount) = {
                    /* 正序正常发送 */
                    record_P_item_do_videoDemand_send_0,
                    /* 倒序正常发送 */
                    record_P_item_do_videoDemand_send_1,
                    /* 正序关键帧发送 */
                    record_P_item_do_videoDemand_send_2,
                    /* 倒序关键帧发送 */
                    record_P_item_do_videoDemand_send_3,
                };
                dataSend[pVideoDemand->sendType & 0x3](pVideoDemand, (void (* const) (void * const, void * const))__record_P_item_do_videoDemand_add_frame__OI_2, pDemandClient, sendCount);
            }
        }
#undef __record_P_item_do_videoDemand_add_frame__OI_2
#ifdef __record_P_item_do_videoDemand_gb28181
#undef __record_P_item_do_videoDemand_gb28181
#endif
#endif
    }
    //    FsPrintf(1, "jinhuan, unlock gb28181 demand client list count: %d\r\n", Fs_CycleList_get_count(pRecord_item->ro.__pGB28181Server_item->rw.__demandClientList));
    pthread_mutex_unlock(&pRecord_item->ro.__pGB28181Server_item->rw.__mutex_demandClientList);
    /* 释放共享buffer */ FsLocal_ShareBuffer_release(pShareBuffer, 0);
}
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */
static int record_P_save_sdkConfig_0_filter(struct FsEbml_node * pEbml_node) {
    if (strcmp(pEbml_node->nodeName, "playback") != 0)return 1;

    else return -1;
}

/* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */
static int record_P_save_sdkConfig_1_filter(struct FsEbml_node * pEbml_node) {
    if (strcmp(pEbml_node->nodeName, "playback") == 0)return 3;
    else if (strcmp(pEbml_node->nodeName, "item") == 0)return 1;

    else return -1;
}

/* 保存外部储存信息,成功返回1,失败返回-1  */
static int record_P_save_sdkConfig(FsEbml * const sdkConfig, /* &0x1表示更新基础信息,0x2表示更新回放信息 */ unsigned char update
        , /* 储存更新标记的文件名,内容为update+'0'的值,filename_0的文件名为filename_tag+"_0.cfg",filename_1的文件名为filename_tag+"_1.cfg" */const char filename_tag[]
        , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer) {
    {
        const unsigned int filename_tagLen = strlen(filename_tag);
        char *const buffer = Fs_ShareBuffer_simple_fsMalloc(pShareBuffer, filename_tagLen + sizeof ("0.cfg"), 1);
        memcpy(buffer, filename_tag, filename_tagLen), memcpy(buffer + filename_tagLen, FsStringDataLen0("0.cfg"));
        if (sdkConfig) {
            if ((update & 0x1) != 0 && fs_Ebml_save_to_file_custom_filter(sdkConfig, buffer, 1, record_P_save_sdkConfig_0_filter) != 1) {
                FsLog(FsLogType_error, FsPrintfIndex, "Save ebml to file:\"%s\" failed.\n", buffer);
                fflush(stdout);
                return -1;
            }
            if ((update & 0x2) != 0) {
                buffer[filename_tagLen] = '1';
                if (fs_Ebml_save_to_file_custom_filter(sdkConfig, buffer, 1, record_P_save_sdkConfig_1_filter) != 1) {
                    FsLog(FsLogType_error, FsPrintfIndex, "Save ebml to file:\"%s\" failed.\n", buffer);
                    fflush(stdout);
                    return -1;
                }
            }
        } else {
            if ((update & 0x1) != 0 && unlink(buffer) != 0 && access(buffer, F_OK) == 0) {
                FsLog(FsLogType_error, FsPrintfIndex, "Unlink the file:\"%s\" error:\"%s\".\n", buffer, strerror(errno));
                fflush(stdout);
                return -1;
            }
            if ((update & 0x2) != 0) {
                buffer[filename_tagLen ] = '1';
                if (unlink(buffer) != 0 && access(buffer, F_OK) == 0) {
                    FsLog(FsLogType_error, FsPrintfIndex, "Unlink the file:\"%s\" error:\"%s\".\n", buffer, strerror(errno));
                    fflush(stdout);
                    return -1;
                }
            }
        }
    }
    update += '0';
    if (fs_String_buffer_save_to_file(1, (char*) &update, filename_tag) != 1) {
        FsLog(FsLogType_error, FsPrintfIndex, "Write file:\"%s\" failed,sdkConfig=%p.\n", filename_tag, sdkConfig);
        if (FsLogTypeCheck(FsLogType_error) && sdkConfig) fs_Ebml_out_debug(sdkConfig, stdout, pShareBuffer);
        fflush(stdout);
        return -1;
    } else {
        FsLog2(FsLogType_info, FsPrintfIndex, "Save file:\"%s\" ok,sdkConfig=%p.\n", filename_tag, sdkConfig);
        if (FsLogTypeCheck(FsLogType_info) && sdkConfig) fs_Ebml_out_debug(sdkConfig, stdout, pShareBuffer);

        return 1;
    }
}

/* 通道分层多线程写入法,不处理实时转发 */
static void *record_P_T(struct Record * const pRecord) {
#undef FsFunctionName
#define FsFunctionName record_P_T
    ////////////////////////////////////////////////////////////////////////////
    /* 创建线程监控 */
    void *pMonitor = pRecord->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_extern_and_add(pMonitor, Record_get_threadName(pRecord), NULL, pRecord->p.threadCount, pRecord, pRecord->ro._pMonitor ? pRecord->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pRecord->ro._externRunStatus;
    Rtsp * const pRtspServer = pRecord->ro._pRtspServer;
    Hls * const pHlsServer = pRecord->ro._pHlsServer;
    GB28181 * const pGB28181Server = pRecord->ro._pGB28181Server;
    ConfigManager * const pConfigManager = (ConfigManager*) pRecord->ro._pConfigManager;
    FsObjectList * const itemList = pRecord->ro.__itemList;
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
#define __record_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    /* 不可跨线程的缓存buffer */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __record_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可跨线程的缓存buffer,一般用于收数据或传递数据到其他线程 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __record_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    double sleepToTime = 0.0;
    unsigned short sleepToTimeInterval = 1;
    unsigned short resetSerialCount = 0; // 连续reset次数;
    /* 所有通道的平钧帧率 */
    float frameRateAvg = 25.0;
    /* 实时录像缓存 */
    int recordVideoBufferCount = 2;
    /* 所在写入层数 */
    unsigned short layer = 0;
    /* 录像信息开始时间检查的层数,当videoInfoCheckLayer大于layer且writeDiskFrameCount>0时才会检查其中一个通道的录像信息的开始时间 */
    unsigned short videoInfoBeginCheckLayer = 0;
    /* 录像信息开始时间检查的通道 */
    unsigned int videoInfoBeginCheckChannel = 0;
    /* 开始位置 */
    const unsigned int startIndex = pRecord->ro._channelCountPerThread * pRecord->p.threadCount++;
    /* 处理的数量 */
    unsigned int doCount;
    /* 子线程的线程号 */
    pthread_t childTid = 0;
    /* 储存当前要处理的通道的数据指针 */
    FsObjectList * const list_ = fs_ObjectList_new_unsafe__IO(6);
#define __record_P_T_clean1_4 {fs_ObjectList_delete_unsafe__OI(list_);}
#define __record_P_T_clean1_count 4
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    int i;
    FsObjectImageFrame * pFrame, **ppFrame_node;
    struct Record_item * pRecord_item, **pRecord_itemNode;
    unsigned int frameCount;
    unsigned short layerMax = 0;
    int writeDiskFrameCount = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    if (0 == startIndex) {
        /* 注册获取线程信息的的命令字,cmd+uuid+ipv4必须是唯一值 */
        configManager_cmd_register_and_protocol_publish(pConfigManager, "recordInfo_get", "record", 0, pRecord, 0
                , (int (* const) (const unsigned int *, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))record_P_cmd_cb_recordInfo, NULL, pRecord
                ////////////////////////////////////////////////////////////////
                , "recordInfo_get", "record信息获取", ConfigManager_Module5_protocol_classIndex0, record_P_protocol_recordInfo_get, &shareBuffer);
#define __record_P_T_clean2_1 {if (0 == startIndex){configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "recordInfo_get", "record", 0, pRecord,"recordInfo_get",&shareBuffer);\
    if(pRecord->p.pConnectNode__videoInfoDataClientList)configManager_connectNode_useOnce_list_delete((struct ConfigManager_connectNode_useOnce *)pRecord->p.pConnectNode__videoInfoDataClientList);}}

        configManager_protocol_publish(pConfigManager, "video_part", "视频合成", 1, record_P_protocol_video_part);
#define __record_P_T_clean2_2 {if (0 == startIndex){configManager_protocol_cancel(pConfigManager, "video_part");}}
        configManager_protocol_publish(pConfigManager, "videolist_get", "视频文件列表获取", 1, record_P_protocol_videolist_get);
#define __record_P_T_clean2_3 {if (0 == startIndex){configManager_protocol_cancel(pConfigManager, "videolist_get");}}
        configManager_cmd_register_and_protocol_publish(pConfigManager, "videotimeinfo_get", NULL, 0, pRecord, 0
                , record_P_cmd_cb, NULL, pRecord
                ////////////////////////////////////////////////////////////////
                , "videotimeinfo_get", "视频时间信息获取", ConfigManager_Module5_protocol_classIndex0, record_P_protocol_videotimeinfo_get, &shareBuffer);
#define __record_P_T_clean2_4 {if (0 == startIndex){configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "videotimeinfo_get", NULL, 0, pRecord,"videotimeinfo_get",&shareBuffer);\
    configManager_conncet_refer_close(pRecord->ro._pConfigManager, pRecord->ro.__videoInfoDataClientList, 1);\
    if (pRecord->p.videoInfoData__videoInfoDataClientList)fs_Ebml_delete__OI(pRecord->p.videoInfoData__videoInfoDataClientList,&shareBuffer), pRecord->p.videoInfoData__videoInfoDataClientList = NULL;\
    if (pRecord->p.videoInfoData_ebml__videoInfoDataClientList)fs_ObjectBasePthreadSafety_delete__OI(pRecord->p.videoInfoData_ebml__videoInfoDataClientList), pRecord->p.videoInfoData_ebml__videoInfoDataClientList = NULL;\
    if (pRecord->p.videoInfoData_xml__videoInfoDataClientList)fs_ObjectBasePthreadSafety_delete__OI(pRecord->p.videoInfoData_xml__videoInfoDataClientList), pRecord->p.videoInfoData_xml__videoInfoDataClientList = NULL;\
    if (pRecord->p.videoInfoData_json__videoInfoDataClientList)fs_ObjectBasePthreadSafety_delete__OI(pRecord->p.videoInfoData_json__videoInfoDataClientList), pRecord->p.videoInfoData_json__videoInfoDataClientList = NULL; }}
        configManager_protocol_publish(pConfigManager, "snapshort_get", "通道快照获取", 1, record_P_protocol_snapshort_get);
#define __record_P_T_clean2_5 {if (0 == startIndex){configManager_protocol_cancel(pConfigManager, "snapshort_get");}}

#define __record_P_T_clean2_count 5
    }
    // 处理线程
    if (itemList->nodeCount > startIndex + pRecord->ro._channelCountPerThread) {
        doCount = pRecord->ro._channelCountPerThread;
        /* 自己处理不完,让子线程来做 */
        if (1) {
            const signed char thisNice = pRecord->rw.thisNice;
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
            if (pthread_create(&childTid, &attr, (void* (*)(void*))record_P_T, pRecord)) {
                FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
                fflush(stdout);
                FsLogExit();
                childTid = 0;
            }
            pthread_attr_destroy(&attr);
        }
    } else doCount = itemList->nodeCount - startIndex;
    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pRecord->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        const double thisTime = fs_time_uptime_get();
        if (sleepToTime > thisTime) {
            if (0 == layerMax) {
                fs_thread_sleepTo(sleepToTime);
                if (writeDiskFrameCount == recordVideoBufferCount && recordVideoBufferCount > 2) {
                    recordVideoBufferCount--;
                }
            } else if (writeDiskFrameCount <= 0 && (sleepToTime - thisTime) * frameRateAvg > (1.0 - 1.0 / layerMax) / (sleepToTimeInterval + 1.0)) {
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
            FsPrintf2(FsPrintfIndex, "layer=%03hu/%03hu,bufferCount=%05u/%05d,frameRateAvg=%lf.\n", layer, layerMax, recordVideoBufferCount, writeDiskFrameCount, frameRateAvg);
        }
        pMonitorItem->rw.line = __LINE__;
        if (sleepToTimeInterval > 0)sleepToTimeInterval--;
        if (frameRateAvg < 5.0)frameRateAvg = 5.0;
        else frameRateAvg *= 0.9999;
        sleepToTime = thisTime + 1.0 / frameRateAvg / (sleepToTimeInterval + 1.0);
        //        FsPrintf2(1, "layer=%03hu/%03hu,bufferCount=%05u/%05d,frameRateAvg=%lf/%lf/%lf.\n", layer, layerMax, recordVideoBufferCount, writeDiskFrameCount, frameRateAvg, sleepToTime, thisTime);
        //        fflush(stdout);
        ////////////////////////////////////////////////////////////////////////
        // rw.resetChannel需要重置通道,当p.syncCount与threadCount相等时,认为所有子线程都已准备好,此时可增、删通道,否则在此处等待其它线程准备好
        i = pRecord->rw._resetChannel;
        if (0 == startIndex) {
            if (0 == i) {
                /* 读取通道数 */
                unsigned int channelCount = record_P_get_channelCount_and_get_snapbuffertimeout_and_update_gbsdk(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local, pRecord);
                if (itemList->nodeCount != channelCount) {
                    /* 让所有线程同步一下 */
#ifndef __record_P_T_do_syncCount
                    pRecord->p.syncCount = 1;
                    unsigned short threadCount = (itemList->nodeCount + pRecord->ro._channelCountPerThread - 1) / pRecord->ro._channelCountPerThread;
                    if (0 == threadCount)threadCount = 1;
                    while (pRecord->p.syncCount != threadCount && pRecord->rw.runStatus == *externRunStatus)FsWaitSyncDo();
                    if (pRecord->p.syncCount != threadCount) while (pRecord->p.threadCount != 1)FsWaitSyncDo();
#endif
                    /* 删除通道 */
                    while (itemList->nodeCount > channelCount) {
                        pRecord_item = (struct Record_item *) fs_ObjectList_remove_tail(itemList);
                        pMonitorItem->rw.line = __LINE__;
                        if (pRecord_item)record_P_item_delete__OI(pRecord_item, &shareBuffer);
                        pMonitorItem->rw.line = __LINE__;
                    }
                    /* 添加通道 */
                    while (itemList->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList, NULL);
                    //FsPrintf(1, "%#lx,%lu,%lu.\n", itemList->pNode, itemList->startIndex, itemList->nodeCount);
                    /* 重建相机录像信息 */
                    record_P_videoInfo_create(pRecord, pConfigManager->ro.__ipList_local);
                    pRecord->rw.videoInfoChange = 1;
                }
                pMonitorItem->rw.line = __LINE__;
                /* 重置集群 */
                record_P_item_new(pRecord, i, pConfigManager->ro.__ipList_local
                        , NULL, pRtspServer
                        , pHlsServer, pGB28181Server, &shareBuffer);
                //FsPrintf(1, "channel=%d/%d\n", i, pRecord->rw._resetChannel);
                pMonitorItem->rw.line = __LINE__;
                pRecord->rw._resetChannel = -1;
            } else if (pRecord->p.pConnectNode__videoInfoDataClientList) {
                /* 让所有线程同步一下 */
                //FsPrintf(FsPrintfIndex, "ttttt\n");
#ifndef __record_P_T_do_syncCount
                pRecord->p.syncCount = 1;
                unsigned short threadCount = (itemList->nodeCount + pRecord->ro._channelCountPerThread - 1) / pRecord->ro._channelCountPerThread;
                if (0 == threadCount)threadCount = 1;
                while (pRecord->p.syncCount != threadCount && pRecord->rw.runStatus == *externRunStatus)FsWaitSyncDo();
                if (pRecord->p.syncCount != threadCount) while (pRecord->p.threadCount != 1)FsWaitSyncDo();
#endif
                //FsPrintf(FsPrintfIndex, "ttttt\n");
                do {
                    /* 线程调试信息 */
#if 1
                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    struct ConfigManager_connectNode_useOnce * const pConnectNode = (struct ConfigManager_connectNode_useOnce *) pRecord->p.pConnectNode__videoInfoDataClientList;
                    pRecord->p.pConnectNode__videoInfoDataClientList = pConnectNode->next;
                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    //FsPrintf(FsPrintfIndex, "ttttt\n");
                    {
                        FsEbml * const pEbml = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                        *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "frameRateAvg", FsEbmlNodeType_Float
                                , "comment", "平均帧率")->data.buffer = frameRateAvg;
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "sleepToTimeInterval", FsEbmlNodeType_Integer
                                , "comment", "程序运行倍率")->data.buffer = sleepToTimeInterval;
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "resetSerialCount", FsEbmlNodeType_Integer
                                , "comment", "重置连续次数")->data.buffer = resetSerialCount;
                        *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "snapbuffertimeout", FsEbmlNodeType_Float
                                , "comment", "快照缓存有效期,单位秒,0不表示不限制")->data.buffer = pRecord->rw._snapbuffertimeout;
                        unsigned int count = itemList->nodeCount;
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelCount", FsEbmlNodeType_Integer
                                , "comment", "通道数")->data.buffer = count;
                        struct Record_item ** pRecord_itemNode = (struct Record_item **) itemList->pNode + itemList->startIndex;
                        struct FsEbml_node * const parent = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node *) pEbml, "channelInfo", FsEbmlNodeType_Struct);
                        for (; count > 0; count--) {
                            struct Record_item * const pRecord_item = *pRecord_itemNode++;
                            struct FsEbml_node * const item = fs_Ebml_node_addChild(pEbml, parent, "item", FsEbmlNodeType_Struct);
                            if (NULL == pRecord_item)continue;
                            fs_Ebml_node_data_set_string(fs_Ebml_node_addChild_with_a_property(pEbml, item, "uuid", FsEbmlNodeType_String
                                    , "comment", "uuid"), pRecord_item->ro._uuid);
                            //                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoCodingFormatFirst", FsEbmlNodeType_Integer
                            //                                    , "comment", "期望的编码方式的第一个值,在videoCodingFormat标识的数据均无效时编码的数据类型,&0x1-h264,&0x2-h265")->data.buffer = pRecord_item->ro._videoCodingFormatFirst;
                            //                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoCodingFormat", FsEbmlNodeType_Integer
                            //                                    , "comment", "期望的编码方式,&0x1-h264,&0x2-h265")->data.buffer = pRecord_item->ro._videoCodingFormat;
                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "recordVideoMode", FsEbmlNodeType_Integer
                                    , "comment", "视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-在有直播请求时记录,3-大华sdk,4-GB28181,15-取流但不记录")->data.buffer = pRecord_item->ro._recordVideoMode;
                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoInfoHasLoad", FsEbmlNodeType_Integer
                                    , "comment", "录像信息是否已加载,0-表示未加载,1-表示已加载")->data.buffer = pRecord_item->p.videoInfoHasLoad;
                            *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoTimeEnd1", FsEbmlNodeType_Float
                                    , "comment", "__info.txt录像信息文件的结束时间(Record_item_is_localVideoStorage(recordVideoMode)为真且videoInfoHasLoad为1时有效)")->data.buffer = pRecord_item->p.videoTimeEnd[0];
                            *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoTimeEnd2", FsEbmlNodeType_Float
                                    , "comment", "__inf0.txt录像信息文件的结束时间(Record_item_is_localVideoStorage(recordVideoMode)为真且videoInfoHasLoad为1时有效)")->data.buffer = pRecord_item->p.videoTimeEnd[1];
                            *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoLastTime", FsEbmlNodeType_Float
                                    , "comment", "上一次录像的时间,小于0.00001表示未录像")->data.buffer = pRecord_item->p.videoLastTime;
                            *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoStartTime", FsEbmlNodeType_Float
                                    , "comment", "在录像开始时记录录像开始时间,在录像持续时间超过一定时间(如1min)后,再把录像开始时间写入记录,以避免快照或视频质量差导致录像频繁中断的问题,在记录开始时间后取原值的相反数")->data.buffer = pRecord_item->p.videoStartTime;
                            *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "videoStartTimeGmt", FsEbmlNodeType_Float
                                    , "comment", "在录像开始时记录录像开始时间(逻辑时间)")->data.buffer = pRecord_item->p.videoStartTimeGmt;
                            unsigned long long dataValidType = 0;
                            if (pRecord_item->p.__snap) {
                                pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                                if (pRecord_item->p.__snap) {
                                    dataValidType = pRecord_item->p.__snap->dataValidType;
                                }
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                            }
                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, item, "dataValidType", FsEbmlNodeType_Integer
                                    , "comment", "通道中的有效数据,(0x1LU<<3)-表示大图h264,(0x1LU<<6)-表示大图h265")->data.buffer = dataValidType;
                        }
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "layer", FsEbmlNodeType_Integer
                                , "comment", "上一次的层数")->data.buffer = layer;
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "layerMax", FsEbmlNodeType_Integer
                                , "comment", "层数最大值")->data.buffer = layerMax;
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "recordVideoBufferCount", FsEbmlNodeType_Integer
                                , "comment", "一次最多写入的量")->data.buffer = recordVideoBufferCount;
                        *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "writeDiskFrameCount", FsEbmlNodeType_Integer
                                , "comment", "上一次写入后的量,负数表示超量写入")->data.buffer = writeDiskFrameCount;
                        if (pConnectNode->mask & (0x1 << 1)) {
                            /* 外部储存数据 */
                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "dhsdkConfigUpdate", FsEbmlNodeType_Integer
                                    , "comment", "dhsdkConfig的配置是否有更新")->data.buffer = Record_sdkConfigUpdate_get(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_dh);
                            if (pRecord->p.dhsdkConfig__videoInfoDataClientList) {
                                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                if (pRecord->p.dhsdkConfig__videoInfoDataClientList) {
                                    struct FsEbml_node * const dhsdk = fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node *) pEbml, "dhsdk", FsEbmlNodeType_Struct
                                            , "comment", "大华SDK");
                                    fs_Ebml_node_addChild_from_node_child(pEbml, dhsdk, (struct FsEbml_node *) pRecord->p.dhsdkConfig__videoInfoDataClientList);
                                }
                                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                            }
                            *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "gb28181ConfigUpdate", FsEbmlNodeType_Integer
                                    , "comment", "dhsdkConfig的配置是否有更新")->data.buffer = Record_sdkConfigUpdate_get(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                            if (pRecord->p.gbsdkConfig__videoInfoDataClientList) {
                                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                                if (pRecord->p.gbsdkConfig__videoInfoDataClientList) {
                                    struct FsEbml_node * const dhsdk = fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node *) pEbml, "gb28181sdk", FsEbmlNodeType_Struct
                                            , "comment", "gb28181回放");
                                    fs_Ebml_node_addChild_from_node_child(pEbml, dhsdk, (struct FsEbml_node *) pRecord->p.gbsdkConfig__videoInfoDataClientList);
                                }
                                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                            }
                        }
                        configManager_send_pthreadSafety__OI_2_default(pConfigManager, pEbml, pConnectNode->requestID, pConnectNode->head, pConnectNode->return_type, &shareBuffer);
                    }
                    fsFree(pConnectNode);
#endif
                } while (pRecord->p.pConnectNode__videoInfoDataClientList);
                pRecord->p.syncCount = 0;
            }
        }
        if (i > (int) startIndex && i < (int) (startIndex + doCount + 1)) {
#ifndef __sleepToTimeInterval_add 
            sleepToTime -= 0.5 / frameRateAvg / (sleepToTimeInterval + 1);
            resetSerialCount++;
            if (resetSerialCount > 3) {
                if (sleepToTimeInterval < (unsigned short) (FsNumberMax(sleepToTimeInterval) - resetSerialCount)) {
                    sleepToTimeInterval += resetSerialCount;
                } else sleepToTimeInterval = FsNumberMax(sleepToTimeInterval);
            } else if (sleepToTimeInterval < (unsigned short) (FsNumberMax(sleepToTimeInterval) - 3)) {
                sleepToTimeInterval += 3;
            } else sleepToTimeInterval = FsNumberMax(sleepToTimeInterval);
#endif
            //FsPrintf(1, "theread:%ld,channel=%d/%d,startIndex=%d,doCount=%d\n", systemThreadTid, i, pRecord->rw._resetChannel, startIndex, doCount);
            record_P_item_new(pRecord, i, pConfigManager->ro.__ipList_local
                    , (struct Record_item **) (itemList->pNode + itemList->startIndex + i - 1)
                    , pRtspServer
                    , pHlsServer, pGB28181Server, &shareBuffer);
            //FsPrintf(1, "theread:%ld,channel=%d/%d,startIndex=%d,doCount=%d\n", systemThreadTid, i, pRecord->rw._resetChannel, startIndex, doCount);
            pRecord->rw._resetChannel = -1;
        } else if (pRecord->p.syncCount != 0) {
#ifndef __sleepToTimeInterval_add 
            sleepToTime -= 0.5 / frameRateAvg / (sleepToTimeInterval + 1);
            resetSerialCount++;
            if (resetSerialCount > 3) {
                if (sleepToTimeInterval < (unsigned short) (FsNumberMax(sleepToTimeInterval) - resetSerialCount)) {
                    sleepToTimeInterval += resetSerialCount;
                } else sleepToTimeInterval = FsNumberMax(sleepToTimeInterval);
            } else if (sleepToTimeInterval < (unsigned short) (FsNumberMax(sleepToTimeInterval) - 3)) {
                sleepToTimeInterval += 3;
            } else sleepToTimeInterval = FsNumberMax(sleepToTimeInterval);
#endif
            /* 其他线程要监控是否要与第一个线程同步 */
            if (0 == startIndex) {
                /* 第一个线程 */
                pRecord->p.syncCount = 0;
            } else {
                pthread_mutex_lock(&itemList->mutex);
                pRecord->p.syncCount++;
                pthread_mutex_unlock(&itemList->mutex);
                while (pRecord->p.syncCount != 0 && pRecord->rw.runStatus == *externRunStatus)FsWaitSyncDo();
            }
            /* 已重置完成了 */
            if (itemList->nodeCount > startIndex && pRecord->rw.runStatus == *externRunStatus) {
                /* 本线程还可以活着 */
                if (itemList->nodeCount > startIndex + pRecord->ro._channelCountPerThread) {
                    doCount = pRecord->ro._channelCountPerThread;
                    /* 自己处理不完,让子线程来做 */
                    if (0 == childTid) {
                        const signed char thisNice = pRecord->rw.thisNice;
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
                        if (pthread_create(&childTid, &attr, (void* (*)(void*))record_P_T, pRecord)) {
                            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
                            fflush(stdout);
                            FsLogExit();
                            childTid = 0;
                        }
                        pthread_attr_destroy(&attr);
                    }
                } else {
                    doCount = itemList->nodeCount - startIndex;
                    /* 自己已能完全处理所有数据,不需要子线程帮忙 */
                    if (childTid) {
                        pthread_join(childTid, 0);
                        thread_join_remove_check_pthreadSafety(childTid);
                        childTid = 0;
                    }
                }
            } else if (0 == startIndex) {
                doCount = 0;
                /* 自己已能完全处理所有数据,不需要子线程帮忙 */
                if (childTid) {
                    pthread_join(childTid, 0);
                    thread_join_remove_check_pthreadSafety(childTid);
                    childTid = 0;
                }
            } else {
                /* 已经是多余的了 */
                break;
            }
        }
        ////////////////////////////////////////////////////////////////////////
        /* 处理各通道数据 */ //根据各通道的rw.layer来确定先入磁盘的优先级
        {
            double snapbuffertimeoutEnd = pRecord->rw._snapbuffertimeout;
            snapbuffertimeoutEnd = snapbuffertimeoutEnd > 0.0 ? thisTime - snapbuffertimeoutEnd : 0.0;
            for (i = doCount, writeDiskFrameCount = recordVideoBufferCount, layer = layerMax, layerMax = 0, pRecord_itemNode = (struct Record_item **) itemList->pNode + itemList->startIndex + startIndex; i > 0; i--) {
                struct Record_item * const pRecord_item = *pRecord_itemNode++;
                if (NULL == pRecord_item)continue;
                // 记录卡口
                record_P_item_do_watch(pRecord_item, &shareBuffer, pMonitorItem);
                /* 记录事件 */ // 为视频分析使用
                record_P_item_do_inc(pRecord_item, &shareBuffer, pMonitorItem);
                /* 记录录像 */ // 无需记录的直接写__framelistOut
                if (Record_item_is_localVideoStorage(pRecord_item->ro._recordVideoMode) && /* 录像信息未加载的不录像 */pRecord_item->p.videoInfoHasLoad) {
                    /* 自动选择合适的码流来记录 */
                    frameCount = pRecord_item->ro.__framelistIn->nodeCount;
                    if (frameCount > 0) {
                        // 根据rw.layer来判断写入优先级，rw.layer大到一定程度优先写入
                        if (pRecord_item->rw.layer > layer && writeDiskFrameCount > 0) {
                            /* 做录像信息记录 */
                            // 开始记录
                            if (pRecord_item->p.videoLastTime <= 0.0) {
                                FsLog2(FsLogType_info, FsPrintfIndex, "record begin[1],%s,%hu/%hu\n", pRecord_item->ro._uuid, pRecord_item->rw._videoRecordCheckInterval, pRecord_item->rw.videoRecordCheckInterval0);
                                pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                                pRecord_item->p.videoStartTimeGmt = ((FsObjectImageFrame*) pRecord_item->ro.__framelistIn->pNode[pRecord_item->ro.__framelistIn->startIndex])->capture_gmtTime;
                                pRecord_item->p.videoStartTime = thisTime + 0.00001;
                                /* 写开始时间,更新内存 */
                                record_P_videoInfo_update(pRecord_item, 1, pRecord_item->p.videoStartTimeGmt, &shareBuffer);
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                            } else if (pRecord_item->p.videoStartTime > 0.0 && pRecord_item->p.videoStartTime + Record_Continue_time < thisTime) {
                                /* 写开始时间,更新内存 */
                                pRecord_item->p.videoStartTime = -pRecord_item->p.videoStartTime;
                                /* 写开始时间,更新硬盘 */
                                pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                                record_P_videoInfo_update(pRecord_item, 1, pRecord_item->p.videoStartTimeGmt, &shareBuffer);
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                            }
                            pRecord_item->p.videoLastTime = thisTime + 5.0;
                            ////////////////////////////////////////////////////////
                            pRecord_item->rw.layer = 1;
                            writeDiskFrameCount -= frameCount;
                            if (frameCount > 1) {
                                /* 有多帧数据 */
                                pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                                fs_ObjectList_insert_tail_all(list_, pRecord_item->ro.__framelistIn);
                                fs_ObjectList_clear(pRecord_item->ro.__framelistIn);
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                                for (frameCount = list_->nodeCount, ppFrame_node = (FsObjectImageFrame**) list_->pNode + list_->startIndex; frameCount > 0; frameCount--) {
                                    pFrame = *ppFrame_node++;
                                    frameRateAvg = frameRateAvg + (pFrame->frameRate - frameRateAvg) / itemList->nodeCount;
                                    /* 录像 */
                                    pMonitorItem->rw.line = __LINE__;
                                    if (0) {
                                        if (access("/fs/project/data/__inctest.txt", F_OK)) {
                                            FILE *fd = fopen("/fs/project/data/__inctest.txt", "wb");
                                            if (fd)fclose(fd);
                                            struct Record_item_Inc * pInc = (struct Record_item_Inc *) fsMalloc(sizeof (struct Record_item_Inc));
                                            memset(pInc, 0, sizeof (struct Record_item_Inc));
                                            /* 事件类型,0x10-枪球联动过船信息 */
                                            pInc->_type = 0x10;
                                            /* 事件开始时间,0时区的时间 */
                                            pInc->_starttime = fs_time_uptime_get();
                                            /* 事件开始时间,0时区的时间,小于0表示无结束时间 */
                                            pInc->_endtime = fs_time_uptime_get() + 10;
                                            /* 图片信息,没有图片的必须置空 */
                                            fs_Object_addRefer_custom_pthreadSafety(pFrame, 5, 0);
                                            pInc->_ppFrame[0] = pInc->_ppFrame[1] = pInc->_ppFrame[2] = pInc->_ppFrame[3] = pInc->_ppFrame[4] = pFrame;
                                            record_item_add_inc_pthreadSafety__OI_2(pRecord_item, pInc);
                                        }
                                    }
                                    //将pFrame写到MKV文件中
                                    record_P_item_do_videoRecord(pRecord_item, pFrame, &shareBuffer, pMonitorItem);
                                    pMonitorItem->rw.line = __LINE__;
                                }
                                /* 传出数据 */
                                fs_ObjectList_insert_tail_all_clean_pthreadSafety(pRecord_item->ro.__framelistOut, list_);
                                Fs_GroupSqrtOut_value_set(pRecord_item->rw._pGroupSqrtOut, &pRecord_item->ro._pRecord->ro._pGroupSqrtOut->groupSqrt_mutex);
                            } else {
                                /* 只有一帧数据 */
                                pMonitorItem->rw.line = __LINE__;
                                pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pRecord_item->ro.__framelistIn);
                                frameRateAvg = frameRateAvg + (pFrame->frameRate - frameRateAvg) / itemList->nodeCount;
                                /* 录像 */
                                pMonitorItem->rw.line = __LINE__;
                                record_P_item_do_videoRecord(pRecord_item, pFrame, &shareBuffer, pMonitorItem);
                                pMonitorItem->rw.line = __LINE__;
                                /* 传出数据 */
                                fs_ObjectList_insert_tail_pthreadSafety(pRecord_item->ro.__framelistOut, pFrame);
                                Fs_GroupSqrtOut_value_set(pRecord_item->rw._pGroupSqrtOut, &pRecord_item->ro._pRecord->ro._pGroupSqrtOut->groupSqrt_mutex);
                                pMonitorItem->rw.line = __LINE__;
                            }
                            /* 录像合成 */
                            if (pRecord_item->ro.__videoPartList->nodeCount > 0 && pRecord_item->ro.__videoCacheList->nodeCount > 0) {
                                if (1 == record_private_item_do_partVideo(pRecord_item)) {
                                    /* 让系统不要睡眠 */
                                    sleepToTime = thisTime;
                                }
                            }
                        } else /*优先级较低的暂不写入*/ {
                            if (layerMax < pRecord_item->rw.layer)layerMax = pRecord_item->rw.layer;
                            pRecord_item->rw.layer++;
                        }
                    } else if /* 完善原来的数据是否正常 */(pRecord_item->p.videoLastTime < thisTime) {
                        if (pRecord_item->p.videoLastTime > 0.0) {
                            FsLog(FsLogType_info, FsPrintfIndex, "record end[0],%s,%hu/%hu\n", pRecord_item->ro._uuid, pRecord_item->rw._videoRecordCheckInterval, pRecord_item->rw.videoRecordCheckInterval0);
                            pRecord_item->p.videoLastTime = 0.0;
                            pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                            record_P_videoInfo_update(pRecord_item, 0, 0.0, &shareBuffer);
                            pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                        } else if (pRecord_item->p.__snap) {
                            pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                            if (pRecord_item->p.__snap->capture_uptime < snapbuffertimeoutEnd) {
                                fs_Object_delete_pthreadSafety__OI(pRecord_item->p.__snap);
                                pRecord_item->p.__snap = NULL;
                            }
                            pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                        }
                    }
                    /* 历史点播 */
                    pMonitorItem->rw.line = __LINE__;
                    record_P_item_do_videoDemand_for_rtsp(pRecord_item, thisTime, &shareBuffer, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                    record_P_item_do_videoDemand_for_hls(pRecord_item, thisTime, &shareBuffer, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                    record_P_item_do_videoDemand_for_GB28181(pRecord_item, thisTime, &shareBuffer, pMonitorItem);
                    pMonitorItem->rw.line = __LINE__;
                } else {
                    if (Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode) && /* 录像信息未加载的不处理回放 */pRecord_item->p.videoInfoHasLoad) {
                        pMonitorItem->rw.line = __LINE__;
                        record_P_item_do_videoDemand_for_rtsp(pRecord_item, thisTime, &shareBuffer, pMonitorItem);
                        pMonitorItem->rw.line = __LINE__;
                        record_P_item_do_videoDemand_for_hls(pRecord_item, thisTime, &shareBuffer, pMonitorItem);
                        pMonitorItem->rw.line = __LINE__;
                        record_P_item_do_videoDemand_for_GB28181(pRecord_item, thisTime, &shareBuffer, pMonitorItem);
                        pMonitorItem->rw.line = __LINE__;
                    }
                    /* 传出数据 */
                    if (pRecord_item->ro.__framelistIn->nodeCount > 0) {
                        pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                        fs_ObjectList_insert_tail_all_clean_pthreadSafety(pRecord_item->ro.__framelistOut, pRecord_item->ro.__framelistIn);
                        pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                        Fs_GroupSqrtOut_value_set(pRecord_item->rw._pGroupSqrtOut, &pRecord_item->ro._pRecord->ro._pGroupSqrtOut->groupSqrt_mutex);
                    } else if (pRecord_item->p.__snap) {
                        pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                        if (pRecord_item->p.__snap->capture_uptime < snapbuffertimeoutEnd) {
                            fs_Object_delete_pthreadSafety__OI(pRecord_item->p.__snap);
                            pRecord_item->p.__snap = NULL;
                        }
                        pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                    }
                    pMonitorItem->rw.line = __LINE__;
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////       
        if ((writeDiskFrameCount + 1) > (int) (recordVideoBufferCount >> 1) && videoInfoBeginCheckLayer++>layer) {
            videoInfoBeginCheckLayer = 0;
            if (videoInfoBeginCheckChannel < doCount) {
                do {
                    struct Record_item * const pRecord_item = ((struct Record_item **) itemList->pNode + itemList->startIndex + startIndex)[videoInfoBeginCheckChannel++];
                    if (NULL == pRecord_item) {
                        /* 通道未加载完成,不着急统计 */
                        videoInfoBeginCheckChannel--;
                        break;
                    }
                    if (Record_item_is_localVideoStorage(pRecord_item->ro._recordVideoMode)) {
                        /* 是本地录像 */
                        if (pRecord_item->p.videoInfoHasLoad) {
                            /* 已加载本地录像信息 */
                            if (0 == pRecord_item->rw._videoRecordCheckInterval) {
                                /* 更新通道的录像开始时间 */
                                //FsPrintf(1, "record start check[2],%s,%hu/%hu\n", pRecord_item->ro._uuid, pRecord_item->rw._videoRecordCheckInterval, pRecord_item->rw.videoRecordCheckInterval0);
                                pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                                record_P_videoInfo_update(pRecord_item, 2, 0.0, &shareBuffer);
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                                break;
                            } else {
                                pRecord_item->rw._videoRecordCheckInterval--;
                            }
                        } else {
                            /* 未加载 */
#ifdef FsDebug
                            if (pRecord_item->ro._videoInfo_pos->childNodeCount != Record_P_videoInfo_HeadCount) {
                                Record_itemLog(FsLogType_fatal, "Invalid videoInfo_pos->childNodeCount(=%u)", pRecord_item->ro._videoInfo_pos->childNodeCount);
                                fflush(stdout);
                                FsLogExit();
                            }
#endif
                            /* 从文件中加载 */
                            record_P_videoInfo_update_from_file(pRecord_item, 1, fs_time_GMT_get(), &shareBuffer);
                            pRecord_item->p.videoInfoHasLoad = 1;
                            //Record_itemLog2(FsLogType_error, "local");
                            break;
                        }
                    } else if (Record_item_is_remoteVideoStorage(pRecord_item->ro._recordVideoMode)) {
                        /* 是远程第三方储存 */
                        if (pRecord_item->p.videoInfoHasLoad) {
                            /* 已加载过远程录像信息 */
                            pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                            if (record_P_videoInfo_update_remote(pRecord_item, 1, &shareBuffer) > 0) {
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                                break;
                            }
                            pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                        } else {
                            /* 未加载 */
                            pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
                            if (record_P_videoInfo_update_remote(pRecord_item, 0, &shareBuffer) > 0) {
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                                pRecord_item->p.videoInfoHasLoad = 1;
                                break;
                            } else {
                                pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
                                pRecord_item->p.videoInfoHasLoad = 1;
                            }
                            // Record_itemLog2(FsLogType_error, "remote");
                        }
                    }
                } while (videoInfoBeginCheckChannel < doCount);
            } else videoInfoBeginCheckChannel = 0;
        }
        ////////////////////////////////////////////////////////////////////////
        if (0 == startIndex) {
            /* 判断外部储存配置是否有更新 */
            if (Record_sdkConfigUpdate_check_not0(pRecord->p._sdkConfigUpdate)) {
                unsigned char update;
                if ((update = Record_sdkConfigUpdate_get(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_dh)) != 0 && access("/tmp/config/dhsdk_", F_OK)) {
                    if (access("/tmp/config", F_OK))mkdir("/tmp/config", 0777);
                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    if (record_P_save_sdkConfig(pRecord->p.dhsdkConfig__videoInfoDataClientList, update, "/tmp/config/dhsdk_", &shareBuffer) == 1) {
                        Record_sdkConfigUpdate_set_01_0(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_dh);
                    } else {
                        FsLog(FsLogType_error, FsPrintfIndex, "Write dhsdkConfig failed.\n");
                        fflush(stdout);
                    }
                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                }
                if ((update = Record_sdkConfigUpdate_get(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb)) != 0 && access("/tmp/config/gbsdk_", F_OK)) {
                    if (access("/tmp/config", F_OK))mkdir("/tmp/config", 0777);
                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    if (record_P_save_sdkConfig(pRecord->p.gbsdkConfig__videoInfoDataClientList, update, "/tmp/config/gbsdk_", &shareBuffer) == 1) {
                        Record_sdkConfigUpdate_set_01_0(pRecord->p._sdkConfigUpdate, Record_sdkConfigUpdate_index_gb);
                    } else {
                        FsLog(FsLogType_error, FsPrintfIndex, "Write gbsdkConfig failed.\n");
                        fflush(stdout);
                    }
                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                }
            }
            if (writeDiskFrameCount > 0) {
                /* 判断录像信息是否发生改变,改变发送数据到所有连接中,只在第一个线程中判断 */
                if (pRecord->rw.videoInfoChange) {
                    pRecord->rw.videoInfoChange = 0;
                    pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                    if (pRecord->p.videoInfoData_ebml__videoInfoDataClientList)fs_ObjectBasePthreadSafety_delete__OI(pRecord->p.videoInfoData_ebml__videoInfoDataClientList), pRecord->p.videoInfoData_ebml__videoInfoDataClientList = NULL;
                    if (pRecord->p.videoInfoData_xml__videoInfoDataClientList)fs_ObjectBasePthreadSafety_delete__OI(pRecord->p.videoInfoData_xml__videoInfoDataClientList), pRecord->p.videoInfoData_xml__videoInfoDataClientList = NULL;
                    if (pRecord->p.videoInfoData_json__videoInfoDataClientList)fs_ObjectBasePthreadSafety_delete__OI(pRecord->p.videoInfoData_json__videoInfoDataClientList), pRecord->p.videoInfoData_json__videoInfoDataClientList = NULL;
                    configManager_conncet_refer_send(pConfigManager, pRecord->ro.__videoInfoDataClientList
                            , &pRecord->p.videoInfoData_ebml__videoInfoDataClientList, &pRecord->p.videoInfoData_xml__videoInfoDataClientList, &pRecord->p.videoInfoData_json__videoInfoDataClientList, pRecord->p.videoInfoData__videoInfoDataClientList
                            , 0x2, &baseBuffer);
                    pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                }
                //            else {
                //                pthread_mutex_lock(&pRecord->ro.__videoInfoDataClientList->mutex);
                //                configManager_conncet_refer_check(pRecord->ro._pConfigManager, pRecord->ro.__videoInfoDataClientList);
                //                pthread_mutex_unlock(&pRecord->ro.__videoInfoDataClientList->mutex);
                //            }    
            }
        }
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    //FsPrintf(1, "tttttttttttt\n");
    /* 处理子线程 */
    if (childTid) {
        pthread_join(childTid, 0);
        thread_join_remove_check_pthreadSafety(childTid);
    }
    /* 线程数-1,必须放在处理子线程之后,这样可以避免使用锁 */
    pRecord->p.threadCount--;

    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    pMonitorItem->rw.line = __LINE__;
    if (0 == startIndex) {
        //____________________________________________________________________// 
#ifdef FsDebug
        if (pRecord->rw._resetChannel != -1) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid resetChannel(=%d)", pRecord->rw._resetChannel);
            fflush(stdout);
            FsLogExit();
        }
#endif
        /* 资源是多线程共享的,由第一个线程控制 */
        /* 删除各通道 */
        {
            struct Record_item **ppNode = (struct Record_item **) itemList->pNode + itemList->startIndex;
            for (i = itemList->nodeCount; i > 0; i--) {
                pRecord_item = *ppNode++;
                if (pRecord_item)record_P_item_delete__OI(pRecord_item, &shareBuffer);
            }
            fs_ObjectList_clear(itemList);
        }
        /* 删除集群 */
        if (pRecord->p.clusterItemList) {
            fs_ObjectList_delete_custom__OI(pRecord->p.clusterItemList, (void (*)(void*))record_P_item_delete__OI);
            pRecord->p.clusterItemList = NULL;
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;

    ////////////////////////////////////////////////////////////////////////////
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s.%hu\" exit,runStatus=%hhd.\n", Record_get_threadName(pRecord), pRecord->p.threadCount, pRecord->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pRecord->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 创建配置 */
void record_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, void * parent) {
    parent = fs_Config_node_node_add(pConfig, parent, "recordConfig", "数据记录配置", "数据记录配置", 0, 0x7);
    fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, parent, "deviceno", "设备编号", "设备编号", 0, 0x7, 1, 128, 1),
            FsConfig_nodeValue_default, "设备编号", "设备编号", "设备编号");
    fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, parent, "devicename", "设备名", "设备名", 0, 0x7, 1, 128, 1),
            FsConfig_nodeValue_default, "设备名", "设备名", "设备名");
    fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, parent, "positionname", "安装位置", "安装位置", 0, 0x7, 1, 128, 1),
            FsConfig_nodeValue_default, "安装位置", "安装位置", "安装位置");
    if (mask & (0x1LLU << 1)) {
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "recordVideoMode", "视频记录模式", "视频记录模式", FsConfig_nodeShowType_default, 0, 0x7, -1, 4, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "不记录", "不记录,删除已储存的数据");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "自动选择合适的码流", "自动选择合适的码流");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "在有直播请求时记录", "在有直播请求时记录");
            if (mask & ((0x1LLU << 20) | (0x1LLU << 21))) {
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "CDN模式", "CDN模式");
            }
            //fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -1, "取流但不记录", "取流但不记录");  // 功能以实现,暂不在界面上显示
        }
        if (mask & ((0x1LLU << 20) | (0x1LLU << 21))) {
            void *const pNode = fs_Config_node_string_add(pConfig, parent, "storageAddr", "储存地址", "储存地址", 0, 0x7, 16, 256, 1);
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "dhsdk://admin:123456@192.168.1.161:37777/1"
                    , "dhsdk://admin:123456@192.168.1.161:37777/1", "dhsdk://admin:123456@192.168.1.161:37777/1");
            //                        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "dhsdk://admin:SChh88@192.168.2.110:37779/1"
            //                    , "dhsdk://admin:SChh88@192.168.2.110:37779/1", "dhsdk://admin:SChh88@192.168.2.110:37779/1");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "3");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional
                    , "gb28181://34020000001320000001:12345678@192.168.1.161:5080/34020000001320000002|34020000002000000001|34020000001320000001|3402000000"
                    , "gb28181://34020000001320000001:12345678@192.168.1.161:5080/34020000001320000002|34020000002000000001|34020000001320000001|3402000000"
                    , "gb28181://34020000001320000001:12345678@192.168.1.161:5080/34020000001320000002|34020000002000000001|34020000001320000001|3402000000");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional
                    , "gb28181://0.0.0.0:0/34020000001320000002|34020000002000000001|34020000001320000001|3402000000"
                    , "gb28181://0.0.0.0:0/34020000001320000002|34020000002000000001|34020000001320000001|3402000000"
                    , "gb28181://0.0.0.0:0/34020000001320000002|34020000002000000001|34020000001320000001|3402000000");
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional
                    , "gb28181://0.0.0.0:0/34020000001320000002|34020000002000000001|34020000001320000001|3402000000?sipOver=1&streamOver=1&playBackQueryRecord=1"
                    , "gb28181://0.0.0.0:0/34020000001320000002|34020000002000000001|34020000001320000001|3402000000?sipOver=1&streamOver=1&playBackQueryRecord=1"
                    , "gb28181://0.0.0.0:0/34020000001320000002|34020000002000000001|34020000001320000001|3402000000?sipOver=1&streamOver=1&playBackQueryRecord=1");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "recordType", "数据记录格式", "数据记录格式", FsConfig_nodeShowType_default, 0, 0x7, 0, 3, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "0-压缩二进制", "0-压缩二进制");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-xml", "1-xml");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 2, "2-xml含注释", "2-xml含注释");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "3-json", "3-json");
        }
        {
            void *const pNode = fs_Config_node_template_add(pConfig, parent, "dataServer", "数据上传服务器", "name", NULL, "数据上传服务器", NULL, NULL, NULL, 0, 0x7, 1);
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal_false, "0");
            {
                void *const pNode1 = fs_Config_node_string_add(pConfig, pNode, "name", "服务器名", "服务器名,上传后会用此名标记已上传的数据", 0, 0x7, 1, 16, 1);
                fs_Config_node_string_add_value(pConfig, pNode1, FsConfig_nodeValue_default, "center", "center", "center");
            }
            {
                void *const pNode1 = fs_Config_node_integer_add(pConfig, pNode, "uploadType", "上传协议", "上传协议", FsConfig_nodeShowType_default, 0, 0x7, 0, 3, 1);
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0, "0-压缩二进制", "0-压缩二进制");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 1, "1-xml", "1-xml");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, 2, "2-xml含注释", "2-xml含注释");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 3, "3-json", "3-json");
            }
            {
                void *const pNode1 = fs_Config_node_string_add(pConfig, pNode, "addr", "服务器地址", "服务器地址,可包含端口,用户名,密码", 0, 0x7, 1, 32, 1);
                fs_Config_node_string_add_value(pConfig, pNode1, FsConfig_nodeValue_default, "192.168.12.23:16800", "192.168.12.23:16800", "192.168.12.23:16800");
            }
            {
                void *const pNode1 = fs_Config_node_integer_add(pConfig, pNode, "historyTime", "重传的时间(s)", "定义多久之内的历史数据可重传", FsConfig_nodeShowType_default, 0, 0x7, 0, 3600 * 24 * 365, 1);
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, 3600 * 24, "一天", "一天");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0, "只上传实时数据", "只上传实时数据");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 60, "一分钟", "一分钟");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 3600, "一小时", "一小时");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 3600 * 24 * 7, "一周", "一周");
                fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 3600 * 24 * 30, "30天", "30天");
            }
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "videoCodingFormat", "视频编码格式", "视频编码格式,多值时只要有一种格式存在即满足需求,没有时编码为设定的第一种格式", FsConfig_nodeShowType_default, 0, 0x7, 0, 0x3FFFFFFFU, 6);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "自动", "自动");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 0, "大图H264", "大图H264");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 1, "大图H265", "大图H265");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 10 << 0, "中图H264", "中图H264");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 10 << 1, "中图H265", "中图H265");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 20 << 0, "小图H264", "小图H264");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 20 << 1, "小图H265", "小图H265");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "keyInterval", "关键帧间隔", "关键帧间隔", FsConfig_nodeShowType_default, 0, 0x7, 1, 200, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 20, "20", "20");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 10, "10", "10");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 50, "50", "50");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "encodeKbps", "码率(kbps)", "码率(kbps)", FsConfig_nodeShowType_default, 0, 0x7, 64, 8000, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 2048, "2048", "2048");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4096, "4096", "4096");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "realVideoProportion", "实时视频比例", "实时视频比例,是相对比例", FsConfig_nodeShowType_default, 0, 0x7, 1, 1000, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 30, "30", "30");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal_false, "0");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "realVideoSaveTime", "实时视频保存的最长时间(S)", "实时视频保存的最长时间,单位秒,0表示不限制", FsConfig_nodeShowType_default, 0, 0x7, 0, 20000000000, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "不限制", "不限制");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 604800, "一周", "一周");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2592000, "30天", "30天");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 5184000, "60天", "60天");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 7776000, "90天", "90天");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "1");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "2");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "watchProportion", "卡口数据比例", "卡口数据比例,是相对比例", FsConfig_nodeShowType_default, 0, 0x7, 1, 1000, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 40, "40", "40");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "1");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "2");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "incidentProportion", "事件数据比例", "事件数据比例,是相对比例", FsConfig_nodeShowType_default, 0, 0x7, 1, 1000, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 60, "60", "60");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "1");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "recordVideoMode", FsConfig_Condition_equal, "2");
        }
    } else {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "recordVideoMode", "视频记录模式", "视频记录模式", FsConfig_nodeShowType_default, 0, 0x0, 0, 0, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "不记录", "不记录,删除已储存的数据");
    }
    if (mask & (0x1LLU << 0)) {
        {
            void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspServerURL", "rtsp转发地址", "rtsp转发地址,为空表示不转发", 0, 0x7, 1, 64, 1);
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "/ch1", "/ch1", "/ch1");
        }
        {
            void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspPushURL", "rtsp推送地址", "rtsp推送地址,为空表示不推送", 0, 0x7, 0, 128, 1);
            fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "rtsp://admin:admin@192.168.12.254/tt", "rtsp://admin:admin@192.168.12.254/tt", "rtsp://admin:admin@192.168.12.254/tt");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, parent, "rtspPushProtocol", "rtsp推送协议", "rtsp推送协议", FsConfig_nodeShowType_default, 0, 0x7, 1, 7, 3);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "tcp", "tcp");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "udp", "udp");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 4, "kcp", "kcp");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "rtspPushURL", FsConfig_Condition_equal_false, NULL);
        }

    } else {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspServerURL", "rtsp转发地址", "rtsp转发地址,为空表示不转发", 0, 0x7, 1, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "", "不转发", "不转发");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "gb28181ID", "GB28181通道编号", "GB28181通道编号,为空表示不注册到GB28181平台", 0, 0x7, 1, 20, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "34020000001320000165", "34020000001320000165", "34020000001320000165");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "gb28181Extern", "GB28181扩展", "GB28181扩展,1-海康时间轴扩展", FsConfig_nodeShowType_default, 0, 0x3, 0, 1, 10);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "海康时间轴扩展", "海康时间轴扩展");
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "gb28181ID", FsConfig_Condition_equal_false, NULL);
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "gb28181Name", "GB28181通道名", "GB28181通道名,为空使用GB28181通道编号", 0, 0x7, 1, 256, 1);
        fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "gb28181ID", FsConfig_Condition_equal_false, NULL);
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "groupParent", "父组织", "父组织,为空GB28181使用SIP为本机分配的ID", 0, 0x7, 1, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "13010100002000000900", "13010100002000000900", "13010100002000000900");
        //fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "gb28181ID", FsConfig_Condition_equal_false, NULL);
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "gb28181ControlMask", "GB28181控制掩码", "GB28181控制掩码,1-开启转动,0x10-开启变倍", FsConfig_nodeShowType_default, 0, 0x7, 0, (0x1 << 0) | (0x1 << 4), 2);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 0, "开启转动", "开启转动");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0x1 << 4, "开启变倍", "开启变倍");
        void *pGroupConfition = fs_Config_condition_group_add(pConfig, pNode);
        fs_Config_condition_add_static(pConfig, pGroupConfition, 2, "cameraConfig streamMask", FsConfig_Condition_orAnd, "4");
        fs_Config_condition_add_static(pConfig, pGroupConfition, 1, "gb28181ID", FsConfig_Condition_equal_false, NULL);
    }
    {

        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "jpg_compressRate", "jpg图像压缩因子", "jpg图像压缩因子(50到95)", FsConfig_nodeShowType_default, 0, 0x7, 50, 90, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 85, "85", "85");
    }
}

/* 检查pRecord模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int record_check_channel_changed(struct Record * const pRecord, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Record
#define __check_channel_changed_itemListLock 
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
 * 创建一个新的Record对象;
 * 返回Record指针.
 */
struct Record * record_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* rtsp服务器对象 */ Rtsp * const pRtspServer
        , /* hls服务器对象 */ Hls * const pHlsServer, /* GB28181服务器对象 */ GB28181 * const pGB28181Server, /* 配置管理对象 */ void *const pConfigManager
        , /* 每个线程最多处理的通道数 */ const unsigned short channelCountPerThread
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
#ifdef loglevel
    if (loglevel == FsNumberMax(loglevel)) fs_log_addLevel(&loglevel, "record", NULL);
#endif
    unsigned int len = strlen(dir);
    if (dir[len - 1] == '/')len++;
    else len += 2;
    len += strlen(threadName) + 1;
    struct Record * const rst = (struct Record*) fsMalloc(sizeof (struct Record) +len);
    memset(rst, 0, sizeof (struct Record));
    char *pd = (char*) (rst + 1);
    /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
    rst->ro._pMonitor = pMonitor;
    /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
    if (externRunStatus) rst->ro._externRunStatus = externRunStatus;
    else rst->ro._externRunStatus = &defaultRunStatus;
    /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
    rst->ro._parentTid = parentTid;
    /* 线程名 */
    rst->ro._threadName_ = pd - (char*) (rst + 1), len = strlen(threadName) + 1, memcpy(pd, threadName, len), pd += len;
    /* 根目录,以'/'结尾 */
    rst->ro._dir = pd, len = strlen(dir), memcpy(pd, dir, len), pd += len;
    if (*(pd - 1) != '/')*pd++ = '/';
    *pd++ = 0;
    /* rtsp服务器对象 */
    rst->ro._pRtspServer = pRtspServer;
    /* hls服务器对象 */
    rst->ro._pHlsServer = pHlsServer;
    /* GB28181服务器对象 */
    rst->ro._pGB28181Server = pGB28181Server;
    /* 配置管理对象 */
    rst->ro._pConfigManager = pConfigManager;
    /* 每个线程最多处理的通道数 */
    rst->ro._channelCountPerThread = channelCountPerThread;
    /* 通道链表 */
    rst->ro.__itemList = fs_ObjectList_new__IO(8);
    /* 在有输出数据时向哪个开方分组写入信号 */
    rst->ro._pGroupSqrtOut = pGroupSqrtOut;
    /* 获取录像信息数据的客户端链表 */
    rst->ro.__videoInfoDataClientList = fs_StructList_new__IO(6, sizeof (int)*6);
    /* 重置通道,负数表示重置完成,初始化为-1 */
    rst->rw._resetChannel = -1;
    ////////////////////////////////////////////////////////////////////////////
    /* sdk配置是否更新,如Record_sdkConfigUpdate_index_dh为0,则sdkConfigUpdate[0]&0x1表示配置文件有更新,sdkConfigUpdate[0]&0x2表示点播客户端信息有更新 */
    memset(rst->p._sdkConfigUpdate, 0xFF, sizeof ( rst->p._sdkConfigUpdate));

    return rst;
}

/* 删除pRecord指向的实例对象 */
void record_delete__OI(struct Record * const pRecord) {
    /* 通道链表 */
    fs_ObjectList_delete__OI(pRecord->ro.__itemList);
    /* 获取通道状态的客户端链表 */
    fs_StructList_delete__OI(pRecord->ro.__videoInfoDataClientList);

    fsFree(pRecord);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void record_startThread(struct Record* pRecord,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (pRecord->rw.runStatus == 0 && pRecord->ro.stid == 0) {
        pRecord->rw.runStatus = 1;
        pRecord->rw.thisNice = thisNice;
#ifdef FsDebug
        RecordLog2(FsLogType_threadDebug, "Init Parameter");
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
        if (pthread_create(&pRecord->ro.stid, &attr, (void* (*)(void*))record_P_T, pRecord)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {

        RecordLog(FsLogType_error, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter", pRecord->rw.runStatus, pRecord->ro.stid);
        fflush(stdout);
    }
}

/* 结束线程 */
void record_stopThread(struct Record * pRecord) {
    pRecord->rw.runStatus = 0;
    if (pRecord->ro.stid) {
#ifdef FsDebug

        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop Start.\n");
#endif
        pthread_join(pRecord->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pRecord->ro.stid);
        pRecord->ro.stid = 0;
#ifdef FsDebug
        FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧,如果数据可作为快照则返回1,否则返回-1 */
int record_item_frame_in_pthreadSafety__OI_4(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    struct Record_item * const pRecord_item = (struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + index];
#ifdef FsDebug
    if (NULL == pRecord_item) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "item is NULL,index=%u.\n", index);
        FsLogTag(100);
        fflush(stdout);
        FsLogExit();
    }
    //    if (pRecord_item->ro.__framelistIn->nodeCount > 15) {
    //        FsPrintf2(1, "FrameListIn too long(=%lu).\n", pRecord_item->ro.__framelistIn->nodeCount);
    //    }
#endif
    /* 传递h264,h265数据帧到服务中 */
#define __record_item_frame_in_server_h264_h265(/* 服务名 */pServer_item,/* 添加的方法 */add_fun, /* 数据元素个数为3的指针数组 */data_3) do{\
    if (data_3[0]) {\
        add_fun(pServer_item, data_3[0]);\
        if (data_3[1]) {\
            add_fun(pServer_item, data_3[1]);\
            if (data_3[2]) add_fun(pServer_item, data_3[2]);\
        }\
    }\
}while (0)
    register FsObjectImageFrame *pFrame;
    for (index = 0; index < frameCount; index++) {
        pFrame = ppFrame[index];
        /* 发送实时rtsp数据 */
        if (pRecord_item->ro.__pRtspServer_item) {
            // FsPrintf(1, "tttt,pFrame=%p\n", pFrame);
            if (ImageFrame_valid_H265_0(pFrame->dataValidType)) {
                // FsPrintf(1, "tttt h265,pFrame=%p,%llx,%hhu,keyFrame=%hhx\n", pFrame, pFrame->dataValidType, pFrame->data.h265[0][0]->object.channelID, pFrame->data.h265[0][0]->keyFrame);
                //  fs_String_buffer_out_hex(pFrame->data.h265[0][0]->datalenth > 64 ? 64 : pFrame->data.h265[0][0]->datalenth, (unsigned char*)pFrame->data.h265[0][0]->data, stdout),printf("\n");
                __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtspServer_item, rtspServer_item_add_frame, pFrame->data.h265[0]);
            } else if (ImageFrame_valid_H264_0(pFrame->dataValidType)) {
                //FsPrintf(1, "tttt h264,pRtspServer_item=%p,pFrame=%p,%llx,%hhu,keyFrame=%hhu\n", pRecord_item->ro.__pRtspServer_item, pFrame, pFrame->dataValidType, pFrame->data.h264[0][0]->object.channelID, pFrame->data.h264[0][0]->keyFrame);
                __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtspServer_item, rtspServer_item_add_frame, pFrame->data.h264[0]);
            } else if (ImageFrame_valid_H265_1(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtspServer_item, rtspServer_item_add_frame, pFrame->data.h265[1]);
            else if (ImageFrame_valid_H264_1(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtspServer_item, rtspServer_item_add_frame, pFrame->data.h264[1]);
            else if (ImageFrame_valid_H265_2(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtspServer_item, rtspServer_item_add_frame, pFrame->data.h265[2]);
            else if (ImageFrame_valid_H264_2(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtspServer_item, rtspServer_item_add_frame, pFrame->data.h264[2]);
        }
        /* 发送实时rtmp数据 */
        if (pRecord_item->ro.__pRtmpServer_item) {
            if (1) {
                if (ImageFrame_valid_H264_0(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtmpServer_item, rtmpServer_item_add_frame, pFrame->data.h264[0]);
                else if (ImageFrame_valid_H264_1(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtmpServer_item, rtmpServer_item_add_frame, pFrame->data.h264[1]);
                else if (ImageFrame_valid_H264_2(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pRtmpServer_item, rtmpServer_item_add_frame, pFrame->data.h264[2]);
            } else {
                static FsObjectH264 *pH264 = NULL;
                if (NULL == pH264) {

                }
                rtmpServer_item_add_frame(pRecord_item->ro.__pRtmpServer_item, pH264);
            }
        }

        /* 发送实时hls数据 */
        if (pRecord_item->ro.__pHlsServer_item) {
            if (ImageFrame_valid_H264_0(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pHlsServer_item, hlsServer_item_add_frame, pFrame->data.h264[0]);
            else if (ImageFrame_valid_H264_1(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pHlsServer_item, hlsServer_item_add_frame, pFrame->data.h264[1]);
            else if (ImageFrame_valid_H264_2(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pHlsServer_item, hlsServer_item_add_frame, pFrame->data.h264[2]);
        }
        /* gb28181转发数据 */
        if (pRecord_item->ro.__pGB28181Server_item) {
            if (ImageFrame_valid_H265_0(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pGB28181Server_item, gb28181Server_item_add_frame, pFrame->data.h265[0]);
            else if (ImageFrame_valid_H264_0(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pGB28181Server_item, gb28181Server_item_add_frame, pFrame->data.h264[0]);
            else if (ImageFrame_valid_H265_1(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pGB28181Server_item, gb28181Server_item_add_frame, pFrame->data.h265[1]);
            else if (ImageFrame_valid_H264_1(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pGB28181Server_item, gb28181Server_item_add_frame, pFrame->data.h264[1]);
            else if (ImageFrame_valid_H265_2(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pGB28181Server_item, gb28181Server_item_add_frame, pFrame->data.h265[2]);
            else if (ImageFrame_valid_H264_2(pFrame->dataValidType)) __record_item_frame_in_server_h264_h265(pRecord_item->ro.__pGB28181Server_item, gb28181Server_item_add_frame, pFrame->data.h264[2]);
        }
    }
    signed char rst = -1;
    pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
    for (index = 0; index < frameCount; index++) {
        pFrame = ppFrame[index];
        fs_ObjectList_insert_tail(pRecord_item->ro.__framelistIn, pFrame);
        if (ImageFrame_can_snapshort(pFrame)) {
            if (pRecord_item->p.__snap)fs_Object_delete_pthreadSafety__OI(pRecord_item->p.__snap);
            fs_Object_addRefer_pthreadSafety(pFrame, Record_BaseLine + __LINE__);
            pRecord_item->p.__snap = pFrame;
            rst = 1;
        }
    }
    pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);

    return rst;
}

/* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
unsigned int record_item_frame_out_pthreadSafety__IO_4(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index, /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server Record
#define __item_frame_out_pthreadSafety__IO_4_itemListLock
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
unsigned int record_item_get_frame_count_pthreadSafety(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server Record
#define __item_get_frame_count_pthreadSafety_itemListLock
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
void record_item_reset_groupSqrtOut(struct Record * const pRecord, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_itemListLock
#define __reset_groupSqrtOut_Server Record
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

/* 添加视频合成任务 */
void record_item_add_partVideo_pthreadSafety(/* 记录项 */ struct Record_item * const pRecord_item, /* 必需为全路径 */const char fileName[],
        /* 开始时间,推荐使用CPU时钟 */const double startiTime, /* 结束时间,推荐使用CPU时钟 */const double endTime, /* 字幕,可为空 */const char subtitle[]) {
    unsigned int fileNameLen = strlen(fileName) + 1;
    unsigned int subtitleLen = subtitle == NULL ? 0 : strlen(subtitle) + 1;
    struct Record_private_PartVideo_item *pPartVideo_item = (struct Record_private_PartVideo_item *) fsMalloc(sizeof ( struct Record_private_PartVideo_item)+fileNameLen + subtitleLen);
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
    fs_ObjectList_insert_custom_pthreadSafety(pRecord_item->ro.__videoPartList, pPartVideo_item, (int (*)(const void* const, const void* const))record_private_item_add_partVideo_compare);
}

/* 删除pWatch的空间 */
void record_item_watch_delete__OI(struct Record_item_Watch * const pWatch) {
    unsigned char i = 0;
    /* 图片信息,没有图片的必须置空 */
    for (; i < sizeof (pWatch->_ppFrame) / sizeof (void*); i++) {
        if (pWatch->_ppFrame[i]) fs_Object_delete_pthreadSafety__OI(pWatch->_ppFrame[i]);
    }

    /* 车牌识别对象 */
    if (pWatch->_pPlate)((FsObjectBase*) pWatch->_pPlate)->_delete(pWatch->_pPlate);

    fsFree(pWatch);
}

/* 添加卡口记录任务 */
void record_item_add_watch_pthreadSafety__OI_2(/* 记录项 */ struct Record_item * const pRecord_item, struct Record_item_Watch * const pWatch) {

    fs_ObjectList_insert_tail_pthreadSafety(pRecord_item->ro.__watchList, pWatch);
}

/* 删除pInc的空间 */
void record_item_inc_delete__OI(struct Record_item_Inc * const pInc) {
    unsigned char i = 0;
    /* 图片信息,没有图片的必须置空 */
    for (; i < sizeof (pInc->_ppFrame) / sizeof (void*); i++) {

        if (pInc->_ppFrame[i]) fs_Object_delete_pthreadSafety__OI(pInc->_ppFrame[i]);
    }
    fsFree(pInc);
}

/* 添加事件记录任务 */
void record_item_add_inc_pthreadSafety__OI_2(/* 记录项 */ struct Record_item * const pRecord_item, struct Record_item_Inc * const pInc) {

    fs_ObjectList_insert_tail_pthreadSafety(pRecord_item->ro.__incList, pInc);
}

/* 用以检测相机是否在线调用的控制取流的接口,只能在vsys中调用 */
void record_item_realStream_for_checkStatus(/* 记录项 */ struct Record_item * const pRecord_item, /* 状态,0-停止取流,1-表示取流 */const unsigned char status) {
    //FsPrintf(1, "uuid:\"%s\",status=%hhu.\n", pRecord_item->ro._uuid, status);
    //fflush(stdout);
    /* 视频记录模式,0-表示不记录,1-表示自动选择合适的码流来记录,2-在有直播请求时记录,3-CDN模式 */
    //    FsPrintf(1, "-------------LocalPushSatus:[%hhu],uuid:\"%s\",localPush:\"%s\"-----\n", status, pRecord_item->ro._uuid, pRecord_item->ro._localPush);
    //    fflush(stdout);

    if (!Record_item_is_pullStream(pRecord_item->ro._recordVideoMode))
        configManager_mask_value_set_pthreadSafety(pRecord_item->ro._pRecord->ro._pConfigManager, pRecord_item->ro._realMask, pRecord_item->ro._maskIndex_record, status);
    //    FsPrintf(1, "-------------LocalPushSatus:[%hhu],uuid:\"%s\",localPush:\"%s\"-----\n", status, pRecord_item->ro._uuid, pRecord_item->ro._localPush);
    //    fflush(stdout);
}

/* 获取快照,成功返回FsObjectImageFrame指针,失败返回NULL */
FsObjectImageFrame * record_item_snap_get_pthreadSafety__IO(/* 采集项,不可为空 */ struct Record_item * const pRecord_item) {
    FsObjectImageFrame *rst = NULL;
    if (pRecord_item->p.__snap) {
        pthread_mutex_lock(&pRecord_item->ro.__framelistIn->mutex);
        if (pRecord_item->p.__snap) {
            rst = pRecord_item->p.__snap;
            fs_Object_addRefer_pthreadSafety(rst, Record_BaseLine + __LINE__);
        }
        pthread_mutex_unlock(&pRecord_item->ro.__framelistIn->mutex);
    }
    return rst;
}
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
