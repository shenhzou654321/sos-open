#include "vgunlinkage.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include <errno.h>
#include <malloc.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>  
#include <unistd.h>
#include <math.h>

#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/FileTool.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Image/ImageDib.h"
#include "../../PublicTools/Image/ImageJpeg.h"
#include "../../Modules/track/vibe_proc.h"
#include "../../Modules/track/v_tool.h"
#include "../../Modules/camera_ctrl/camera_ctrl_tcp.h"
#include "../../PublicTools/Fs/Socket.h"
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

/* 从配置获取通道数 */
static unsigned int gunlinkage_private_get_channelCount(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, ipList, 0, "vsys");
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
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, vsys, NULL, 0, "vsysChannel");
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

/* 删除pvtrack_item指向的实例对象 */
static void gunlinkage_private_item_delete__OI(struct Gunlinkage_item * pGunlinkage_item) {
    if (NULL == pGunlinkage_item)return;

    fs_ObjectList_delete_custom__OI(pGunlinkage_item->ro.__framelistIn, fs_Object_delete_pthreadSafety__OI);
    fs_ObjectList_delete_custom__OI(pGunlinkage_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
    if (pGunlinkage_item->ro.moduleMask & 0x8) {

        /* 连接和调用外部命令所需要的同步锁 */
        pthread_mutex_destroy(&pGunlinkage_item->ro.__mutexCmdConnect);
        // 日志打印
        pthread_mutex_destroy(&pGunlinkage_item->rw.m_log_mutex);

        /* 文字叠加 */
        if (pGunlinkage_item->ro.__pTypeFace)fs_TypeFace_delete__OI(pGunlinkage_item->ro.__pTypeFace);



        if (pGunlinkage_item->ro.m_config_devinfo.m_camera_model == 2 || pGunlinkage_item->ro.m_config_devinfo.m_camera_model == 3) {

            if (pGunlinkage_item->ro.__pSoketpool_item)socketPool_item_delete__OI(pGunlinkage_item->ro.__pSoketpool_item);
            /* 注销录像的掩码 */
            configManager_mask_logoff_pthreadSafety(pGunlinkage_item->ro._pGunlinkage->ro._pConfigManager, "realStream", pGunlinkage_item->ro._uuid, pGunlinkage_item->ro._maskIndex);

            //关闭与相机的链接
            if (pGunlinkage_item->rw.pCamera_ctrl_item)camera_ctrl_tcp_delete__OI(pGunlinkage_item->rw.pCamera_ctrl_item);

            //销毁
            configManager_cmd_disconnect(pGunlinkage_item->ro._pGunlinkage->ro._pConfigManager, "video_part", pGunlinkage_item->ro._uuid, pGunlinkage_item);
        }
    }
    fsFree(pGunlinkage_item);
}

/* 在期望连接的命令字和uuid改变时调用 */
static void gun_private_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p) {
    struct Gunlinkage_item* pgun_item = (struct Gunlinkage_item *) p;
    printf(">>>>>>>>>>>>>pgun_item:%p  %s<<<<<<<<<<<<<<<<\n", pgun_item, pgun_item->ro._uuid);

    if (strcmp("video_part", cmd) == 0 && strcmp(uuid, pgun_item->ro._uuid) == 0) {
        pthread_mutex_lock(&pgun_item->ro.__mutexCmdConnect);
        pgun_item->p.record_item_add_partVideo = (Record_item_add_partVideo_pthreadSafety) do_local;
        pgun_item->p.pRecord_item = (struct Record_item*) do_local_p;
        pthread_mutex_unlock(&pgun_item->ro.__mutexCmdConnect);
    } else {
        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pgun_item->ro._uuid);
        fflush(stdout);
    }
}

/*******************************************************************************
 * 生成模拟数据
 * 
 * 创建/fs/project/data/makeconfig/makeData.dat文件，产生一条数据记录
 * 
 * 产生后，自动删除该文件
 ********************************************************************1************/
void gunlinkage_make_debug_data(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame *pFrame, int currFrameIndex, int sw, int sh) {

    if (!access("/fs/project/data/makeconfig/makeTrigger.dat", F_OK)) {

        int i = 0;
        for (i = 0; i < TRIGGERCOUNT; i++) {
            pGunlinkage_item->rw.m_trig_out[i].used = 0;
        }


        for (i = 0; i < TRIGGERCOUNT; i++) {

            if (pGunlinkage_item->rw.m_trig_out[i].used == 1)
                continue;

            double pos[4];
            pos[0] = 0.156250;
            pos[1] = 0.297917;
            pos[2] = 0.625926;
            pos[3] = 0.696296;
            char buf[256] = {0};
            if (ReadProfileStr("Trigger", "left", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                printf("---%s---\n", buf);
                pos[0] = atof(buf);
                //sscanf(buf, "%lf", pos[0]);
            }
            if (ReadProfileStr("Trigger", "top", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                //sscanf(buf, "%lf", pos[1]);
                pos[1] = atof(buf);
            }
            if (ReadProfileStr("Trigger", "right", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                //sscanf(buf, "%lf", pos[2]);
                pos[2] = atof(buf);
            }
            if (ReadProfileStr("Trigger", "bottom", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                //sscanf(buf, "%lf", pos[3]);
                pos[3] = atof(buf);
            }

            pGunlinkage_item->rw.m_trig_out[i].used = 1;
            pGunlinkage_item->rw.m_trig_out[i].index = currFrameIndex;
            pGunlinkage_item->rw.m_trig_out[i].dealed = 0;

            pGunlinkage_item->rw.m_trig_out[i].count = 4;
            pGunlinkage_item->rw.m_trig_out[i].l[0] = pos[0] * 480;
            pGunlinkage_item->rw.m_trig_out[i].r[0] = pos[2] * 480;
            pGunlinkage_item->rw.m_trig_out[i].t[0] = pos[1] * 270;
            pGunlinkage_item->rw.m_trig_out[i].b[0] = pos[3] * 270;

            pGunlinkage_item->rw.m_trig_out[i].l[1] = pos[0] * 480;
            pGunlinkage_item->rw.m_trig_out[i].r[1] = pos[2] * 480;
            pGunlinkage_item->rw.m_trig_out[i].t[1] = pos[1] * 270;
            pGunlinkage_item->rw.m_trig_out[i].b[1] = pos[3] * 270;

            pGunlinkage_item->rw.m_trig_out[i].l[2] = pos[0] * 480;
            pGunlinkage_item->rw.m_trig_out[i].r[2] = pos[2] * 480;
            pGunlinkage_item->rw.m_trig_out[i].t[2] = pos[1] * 270;
            pGunlinkage_item->rw.m_trig_out[i].b[2] = pos[3] * 270;

            pGunlinkage_item->rw.m_trig_out[i].l[3] = pos[0] * 480;
            pGunlinkage_item->rw.m_trig_out[i].r[3] = pos[2] * 480;
            pGunlinkage_item->rw.m_trig_out[i].t[3] = pos[1] * 270;
            pGunlinkage_item->rw.m_trig_out[i].b[3] = pos[3] * 270;

            pGunlinkage_item->rw.m_trig_out[i].m_export_count = 100;

            printf("%s %d, %lf %lf %lf %lf \n", __FUNCTION__, __LINE__, pos[0], pos[1], pos[2], pos[3]);

            //保存录像...
            if (pGunlinkage_item->rw.m_linage_videoPath[0] == 0) {
                time_t ts = (time_t) fs_time_GMT_get();
                struct tm ti_s;
                localtime_r(&ts, &ti_s);

                sprintf(pGunlinkage_item->rw.m_linage_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pGunlinkage_item->ro._pGunlinkage->ro._dir,
                        pGunlinkage_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                        1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
            }
            double startT = fs_time_uptime_get() - 10;
            double endT = fs_time_uptime_get();
            FsPrintf2(1, ">>>>save video  path:%s time(%f %f %f)\n", pGunlinkage_item->rw.m_linage_videoPath, startT, endT, fs_time_uptime_get());
            //pGunlinkage_item->p.record_item_add_partVideo(pGunlinkage_item->p.pRecord_item, pGunlinkage_item->rw.m_linage_videoPath, startT, endT, NULL);
            //fflush(stdout);

            printf("%s %d trigger a obj(index:%d dealed:%d) \n", __FUNCTION__, __LINE__, i, pGunlinkage_item->rw.m_trig_out[i].dealed);
            break;
        }

        remove("/fs/project/data/makeconfig/makeTrigger.dat");
        //        FILE* f = fopen("/fs/project/data/makeconfig/makeData.dat", "wb");
        //        if (f != NULL) {
        //            fwrite("make a simulated data for test", strlen("make a simulated data for test"), 1, f);
        //            fclose(f);
        //        }
    }
}

void gunlinkage_save_debug_image(struct Gunlinkage_item *pGunlinkage_item, unsigned char *pSrcImage, int currFrameIndex, int sw, int sh) {

    //保存发送给球机的图片
    if (pGunlinkage_item->ro.m_debug_switch > 0) {

        //发送最新坐标给球机，保存一张图
        if (pGunlinkage_item->rw.m_debug_sendpos == 1) {
            pGunlinkage_item->rw.m_debug_sendpos = 0;


            ITG_RECT myrect;
            myrect.left = pGunlinkage_item->rw.m_debug_left*sw;
            myrect.right = pGunlinkage_item->rw.m_debug_right*sw;
            myrect.top = pGunlinkage_item->rw.m_debug_top*sh;
            myrect.bottom = pGunlinkage_item->rw.m_debug_bottom*sh;
            int boatw = myrect.right - myrect.left;
            int boath = myrect.bottom - myrect.top;
            if (boatw <= 0 || boath <= 0 || myrect.left <= 0 || myrect.right > sw - 1 || myrect.top <= 0 || myrect.bottom > sh - 1) {
                printf("%s,%d, the draw rect is error. left:%d, right:%d,top:%d,bottom:%d \n", __FUNCTION__, __LINE__, myrect.left, myrect.right, myrect.top, myrect.bottom);
            } else {
                ITG_DrawRect_y((char *) pSrcImage, NULL, NULL, sw, &myrect, 255, 255, 255, 50, 1);
            }
            //修正之后的套框
            myrect.left = pGunlinkage_item->rw.m_debug_c_left;
            myrect.right = pGunlinkage_item->rw.m_debug_c_right;
            myrect.top = pGunlinkage_item->rw.m_debug_c_top;
            myrect.bottom = pGunlinkage_item->rw.m_debug_c_bottom;
            boatw = myrect.right - myrect.left;
            boath = myrect.bottom - myrect.top;
            if (boatw <= 0 || boath <= 0 || myrect.left <= 0 || myrect.right > sw - 1 || myrect.top <= 0 || myrect.bottom > sh - 1) {
                printf("%s,%d, the draw rect is error. left:%d, right:%d,top:%d,bottom:%d \n", __FUNCTION__, __LINE__, myrect.left, myrect.right, myrect.top, myrect.bottom);
            } else {
                ITG_DrawRect_y((char *) pSrcImage, NULL, NULL, sw, &myrect, 255, 255, 255, 50, 1);
            }

            time_t ts = (time_t) fs_time_GMT_get();
            struct tm ti;
            localtime_r(&ts, &ti);
            char buffer1[512];
            sprintf(buffer1, "/fs/project/data/vsys/641/debug/%04d%02d%02d/%02d/gun_%d__w%d_h%d_l%d_b%d__%d.bmp",
                    1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, pGunlinkage_item->rw.curr_trig_num, boatw, boath, myrect.left, myrect.bottom, currFrameIndex);
            PrepareDir(buffer1);
            ImageDib* pdib = image_dib_new__IO(sw, sh, ImageColorType_Gray, 0);
            memcpy(pdib->data, pSrcImage, sw * sh);
            image_dib_svae_to_file(pdib, buffer1);

            image_dib_delete__OI(&pdib);

            ////////////
            //保存目标运动轨迹
            int i;
            for (i = 0; i < TRIGGERCOUNT; i++) {

                if (pGunlinkage_item->rw.m_trig_out[i].used == 0)
                    continue;

                if (pGunlinkage_item->rw.curr_trig_num == pGunlinkage_item->rw.m_trig_out[i].index) {

                    time_t ts = (time_t) fs_time_GMT_get();
                    struct tm ti;
                    localtime_r(&ts, &ti);
                    char path[512];
                    if (pGunlinkage_item->rw.m_trig_out[i].type == 1 || pGunlinkage_item->rw.m_trig_out[i].type == 2)
                        sprintf(path, "/fs/project/data/vsys/641/debug/%04d%02d%02d/%02d/route_right_%lld.txt",
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, pGunlinkage_item->rw.curr_trig_num);
                    else
                        sprintf(path, "/fs/project/data/vsys/641/debug/%04d%02d%02d/%02d/route_left_%lld.txt",
                            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, pGunlinkage_item->rw.curr_trig_num);
                    PrepareDir(path);

                    FILE* f = fopen(path, "wb");
                    if (f) {
                        //printf("save route cnt:%d  l:%d \n", pGunlinkage_item->rw.m_trig_out[i].count, pGunlinkage_item->rw.m_trig_out[i].r[0]);
                        char info[128];
                        sprintf(info, "aveflag:%d speed:%lf aveW:%d aveH:%d aveW1:%d type:%d \r\n", pGunlinkage_item->rw.m_debug_aveflag, pGunlinkage_item->rw.m_debug_c_speed,
                                pGunlinkage_item->rw.m_debug_aveW,
                                pGunlinkage_item->rw.m_debug_aveH, pGunlinkage_item->rw.m_debug_aveW1, pGunlinkage_item->rw.m_trig_out[i].type);
                        fwrite(info, strlen(info), 1, f);

                        if (pGunlinkage_item->rw.m_trig_out[i].count < POINTCOUNT) {
                            int j;
                            for (j = 0; j < pGunlinkage_item->rw.m_trig_out[i].count; j++) {
                                char temp[64];
                                int www = (pGunlinkage_item->rw.m_trig_out[i].r[j] - pGunlinkage_item->rw.m_trig_out[i].l[j]); //pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].l[j])*(pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].b[j] - pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].t[j]);
                                int hhh = (pGunlinkage_item->rw.m_trig_out[i].b[j] - pGunlinkage_item->rw.m_trig_out[i].t[j]);

                                sprintf(temp, "%03d_%03d  %03d_%03d_%03d_%03d  %d\r\n", www, hhh, pGunlinkage_item->rw.m_trig_out[i].l[j], pGunlinkage_item->rw.m_trig_out[i].r[j],
                                        pGunlinkage_item->rw.m_trig_out[i].t[j], pGunlinkage_item->rw.m_trig_out[i].b[j], pGunlinkage_item->rw.m_trig_out[i].difTime[j]);
                                fwrite(temp, strlen(temp), 1, f);
                            }
                        } else {

                            int j;
                            for (j = 0; j < POINTCOUNT; j++) {

                                int last_count = (pGunlinkage_item->rw.m_trig_out[i].count - j) % POINTCOUNT;
                                if (last_count == 0) last_count = POINTCOUNT - 1;
                                else last_count -= 1;

                                char temp[64];
                                int www = (pGunlinkage_item->rw.m_trig_out[i].r[last_count] - pGunlinkage_item->rw.m_trig_out[i].l[last_count]); //pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].l[j])*(pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].b[j] - pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].t[j]);
                                int hhh = (pGunlinkage_item->rw.m_trig_out[i].b[last_count] - pGunlinkage_item->rw.m_trig_out[i].t[last_count]);

                                sprintf(temp, "%03d_%03d  %03d_%03d_%03d_%03d  %d\r\n", www, hhh, pGunlinkage_item->rw.m_trig_out[i].l[last_count], pGunlinkage_item->rw.m_trig_out[i].r[last_count],
                                        pGunlinkage_item->rw.m_trig_out[i].t[last_count], pGunlinkage_item->rw.m_trig_out[i].b[last_count], pGunlinkage_item->rw.m_trig_out[i].difTime[last_count]);
                                fwrite(temp, strlen(temp), 1, f);
                            }
                        }
                        fclose(f);
                    }
                }
            }
            ///////////
        }
    }
}

/****************************************************************************
 *                   相机获取一张算法处理图片
 * 
 * 创建/fs/project/data/makeconfig/getpic_%s.dat文件，即可自动生成一张图片，图片保存在/fs/project/data/config/目录下
 * 
 * 生成后，会自动删除该文件
 ****************************************************************************/
void gunlinage_get_maskImage(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame *pFrame, int currFrameIndex, int sw, int sh) {

    //相机自动检测：取一张灰度小图
    if (currFrameIndex % 600 == 0) {

        if (access("/fs/project/data/makeconfig", 0) == -1)//access函数是查看文件是不是存在
        {
            if (mkdir("/fs/project/data/makeconfig", 0777))//如果不存在就用mkdir函数来创建
            {
                printf("creat file bag failed!!!");
            }
        }

        char path[512];
        sprintf(path, "/fs/project/data/makeconfig/getpic_%s.dat", pGunlinkage_item->ro._uuid);
        if (!access(path, F_OK)) {

            remove(path);

            int w, h;
            ImageDib* pdib;
            if (pGunlinkage_item->ro.zoomTimes == 16 || pGunlinkage_item->ro.zoomTimes == 0) {
                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_2);
                w = pFrame->width[2];
                h = pFrame->height[2];
                pdib = image_dib_new__IO(pFrame->width[2], pFrame->height[2], ImageColorType_Gray, 0);
                memcpy(pdib->data, pYUV420P->data, pFrame->width[2] * pFrame->height[2]);
                fs_Object_delete_pthreadSafety__OI(pYUV420P);
            } else if (pGunlinkage_item->ro.zoomTimes == 4) {
                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_1);
                w = pFrame->width[1];
                h = pFrame->height[1];
                pdib = image_dib_new__IO(pFrame->width[1], pFrame->height[1], ImageColorType_Gray, 0);
                memcpy(pdib->data, pYUV420P->data, pFrame->width[1] * pFrame->height[1]);
                fs_Object_delete_pthreadSafety__OI(pYUV420P);
            } else {
                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
                w = pFrame->width[0];
                h = pFrame->height[0];
                pdib = image_dib_new__IO(pFrame->width[0], pFrame->height[0], ImageColorType_Gray, 0);
                memcpy(pdib->data, pYUV420P->data, pFrame->width[0] * pFrame->height[0]);
                fs_Object_delete_pthreadSafety__OI(pYUV420P);
            }

            printf("%d %d \n", w, h);
            sprintf(path, "/fs/project/data/config/getpic_%s.bmp", pGunlinkage_item->ro._uuid);
            image_dib_svae_to_file(pdib, path);
            image_dib_delete__OI(&pdib);
        }


        sprintf(path, "/fs/project/data/makeconfig/get_maskdata.dat");
        if (!access(path, F_OK)) {
            remove(path);

            time_t ts = (time_t) fs_time_GMT_get();
            struct tm ti;
            localtime_r(&ts, &ti);
            char filepath[512];
            sprintf(filepath, "/fs/project/data/makeconfig/maskdata.txt");

            FILE* f = fopen(filepath, "wb");
            if (f) {
                int i, j;
                for (j = 0; j < sh; j++) {
                    char info[4];
                    for (i = 0; i < sw; i++) {
                        sprintf(info, "%d ", pGunlinkage_item->rw.m_vibeparam.model->dynamicThreahold[j * sw + i]);
                        fwrite(info, strlen(info), 1, f);
                    }
                    sprintf(info, "\r\n");
                    fwrite(info, strlen(info), 1, f);
                }
                fclose(f);
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void gunlinkage_getConfig(struct Gunlinkage_item *rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel);

void gunlinkage_load_maskImage_from_file(struct Gunlinkage_item *rst) {

    int maskindex = 0;
    for (maskindex = 0; maskindex < 4; maskindex++) {
        char samplepath[516];

        if (maskindex == 0) {
            sprintf(samplepath, "/fs/project/data/config/sample_linkage_%s_%d.bmp", rst->ro._uuid, maskindex + 1);
            if (!access(samplepath, F_OK)) {
                printf("%s %d : cannot find the file(%s) \n", __FUNCTION__, __LINE__, samplepath);
            } else {
                sprintf(samplepath, "/fs/project/data/config/sample_linkage_%s.bmp", rst->ro._uuid);
            }
        } else {
            sprintf(samplepath, "/fs/project/data/config/sample_linkage_%s_%d.bmp", rst->ro._uuid, maskindex + 1);
        }

        if (!access(samplepath, F_OK)) {

            ImageDib* pdib = image_dib_new_from_file__IO(samplepath, ImageColorType_Unknown, 0);
            //没有模板图片
            if (pdib == NULL) {
                printf("%s:%d  ImageDib load Fails \n", __FUNCTION__, __LINE__);
                continue;
            }

            int w, h;
            w = pdib->width;
            h = pdib->height;
            if (w > DIMAGEW || h > DIMAGEH) {
                printf("%s:%d  the file is error \n", __FUNCTION__, __LINE__);
                return;
            }

            memset(rst->rw.m_vibe_inparam.maskImage[maskindex], 0, w * h);

            int k = 0;
            int i, j;
            int r, g, b;
            for (j = 0; j < h; j++) {
                for (i = 0; i < w; i++) {
                    r = pdib->data[k];
                    k++;
                    g = pdib->data[k];
                    k++;
                    b = pdib->data[k];
                    k++;

                    //绿色区域:水域
                    if (g > r && g > b) {
                        rst->rw.m_vibe_inparam.maskImage[maskindex][j * w + i] = 255;
                        rst->rw.m_vibe_inparam.movethImage[j * w + i] = rst->rw.m_vibe_inparam.cplex_moveth[0];
                        //static int ccc = 0;
                        //if (ccc == 0) {
                        //    ccc++;
                        //printf("%s:%d  get the water area \n", __FUNCTION__, __LINE__);
                        //}
                    }

                    //红色区域：岸堤
                    if (r > b && r > g) {
                        rst->rw.m_vibe_inparam.maskImage[maskindex][j * w + i] = 255;
                        rst->rw.m_vibe_inparam.movethImage[j * w + i] = rst->rw.m_vibe_inparam.cplex_moveth[1];
                        static int ccc = 0;
                        if (ccc == 0) {
                            ccc++;
                            printf("%s:%d  get the land area \n", __FUNCTION__, __LINE__);
                        }
                    }

                }

                int asd = (w * 3) % 4;
                asd = (asd == 0) ? 0 : (4 - asd);
                k += asd;
            }
            image_dib_delete__OI(&pdib);

            ImageDib* pdib1 = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
            memcpy(pdib1->data, rst->rw.m_vibe_inparam.maskImage[maskindex], w * h);
            sprintf(samplepath, "/fs/project/data/config/result_linkage_%s_%d.bmp", rst->ro._uuid, maskindex + 1);
            image_dib_svae_to_file(pdib1, samplepath);
            image_dib_delete__OI(&pdib1);

            printf("%s:%d  read  the  sample file suc(%s) \n", __FUNCTION__, __LINE__, samplepath);
        } else {
            printf("%s:%d  cannot find the file(%s) \n", __FUNCTION__, __LINE__, samplepath);
        }
    }
}

static void gunlinkage_private_item_new(struct Gunlinkage *pGunlinkage, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的采集项 */struct Gunlinkage_item **ppGunlinkage_item, FsGroupSqrt * const pGroupSqrt) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pGunlinkage->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    FsObjectList * const clusterList = fs_Config_node_template_orderFirst__IO(pConfig, &vsys0, pConfig, ipList, 0 == channel, "vsys");
    if (clusterList) {
        const void **ppNodeCluster = (const void **) clusterList->pNode + clusterList->startIndex;
        unsigned int uj = clusterList->nodeCount;
        do {
            const void *vsysChannel0 = vsys0;
            const void* const vsys = *ppNodeCluster++;
            FsObjectList * const list = 0 == channel ? fs_Config_node_template_orderFirst__IO(pConfig, &vsysChannel0, vsys, NULL, 0, "vsysChannel")
                    : (FsObjectList *) fs_Config_node_template_get_orderFirst(pConfig, &vsysChannel0, vsys, NULL, 0, "vsysChannel", channel - 1);
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
                        if (*ppGunlinkage_item != NULL && (*ppGunlinkage_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppGunlinkage_item) {
                        gunlinkage_private_item_delete__OI(*ppGunlinkage_item);
                    }
                    ////////////////////////////////////////////////////////////
                    struct Gunlinkage_item * rst;
                    unsigned long moduleMask;
                    if (Gunlinkage_Mask != 0 && (Gunlinkage_Mask & (moduleMask = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL))) == 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            rst = (struct Gunlinkage_item*) fsMalloc(Memery_Alignment(sizeof (struct Gunlinkage_item)) + len);
                            //结构体清零...
                            memset(rst, 0, sizeof (struct Gunlinkage_item));
                            pd = (char*) (rst + 1);
                            /* 掩码 */
                            rst->ro.moduleMask = moduleMask;
                            FsPrintf(1, "linkage_private_item_new....malloc the  Vlinkage_item: %ld \n", sizeof (struct Gunlinkage_item) +len);
                            fflush(stdout);
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len), pd += len;
                            {
                                const char * const rtspURL = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "recordConfig rtspServerURL", NULL);
                                sprintf(rst->ro.m_linkage_rtspURL, "%s", rtspURL);
                            }
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
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pGunlinkage->ro._pGroupSqrtOut->groupSqrt_member, pGunlinkage->ro._pGroupSqrtOut->groupSqrt_group
                                    , pGunlinkage->ro._pGroupSqrtOut->groupValue, channel - 1, &pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        ////////////////////////////////////////////////////////
                        if (channel > 0) {//相机IP
                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig ipv4", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ip, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ip, "192.168.8.252");
                            }

                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rstpUsername", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraUid, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraUid, "admin");
                            }
                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rstpPasswd", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraPwd, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraPwd, "12345");
                            }
                            rst->ro.m_config_devinfo.m_camera_port = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rtspPort", 0, NULL);

                            //相机云台控制IP
                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig holderipv4", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ctrl_ip, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ctrl_ip, "192.168.8.252");
                            }
                            //相机云台控制端口   
                            rst->ro.m_config_devinfo.m_camera_ctrl_port = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig holderPort", 0, NULL);

                            //相机型号
                            rst->ro.m_config_devinfo.m_camera_model = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig cameraModel", 0, NULL);
                            //相机品牌
                            rst->ro.m_config_devinfo.m_camera_brand = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig cameraBrand", 0, NULL);

                            //
                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "recordConfig deviceno", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_param_config.deviceno, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_param_config.deviceno, "12345");
                            }

                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "recordConfig devicename", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_param_config.devicename, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_param_config.devicename, "12345");
                            }
                            {
                                const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "positon center", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_param_config.position_jwd, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_param_config.position_jwd, "0,0");
                            }
                            gunlinkage_getConfig(rst, pConfig, vsysChannel0, vsysChannel);

                            FsPrintf2(1, ">>deviceno:%s devicename:%s position_jwd:%s area_jwd:%s\n", rst->ro.m_param_config.deviceno, rst->ro.m_param_config.devicename,
                                    rst->ro.m_param_config.position_jwd, rst->ro.m_param_config.area_jwd);


                            FsPrintf2(1, "cameraip:%s cameraport:%d camera_model:%d  m_camera_brand:%d \n", rst->ro.m_config_devinfo.m_camera_ip,
                                    rst->ro.m_config_devinfo.m_camera_port, rst->ro.m_config_devinfo.m_camera_model, rst->ro.m_config_devinfo.m_camera_brand);
                            fflush(stdout);

                            FsPrintf2(1, "linkage_server_ip:%s linkage_server_uuid:%d linkage_open:%d modw:%d modh:%d moveth:%d obj_min_h:%d  loop_length:%d topth:%d lowth:%d loop_start_left:%d loop_start_right:%d area(%d-%d-%d-%d) zoomTimes:%d \n",
                                    rst->ro.m_config_devinfo.m_linkage_server_ip, rst->ro.m_config_devinfo.m_linkage_server_uuid, rst->rw.m_linkage_switch, rst->rw.m_vibe_inparam.modw, rst->rw.m_vibe_inparam.modh, rst->rw.m_vibe_inparam.moveth,
                                    rst->rw.m_vibe_inparam.obj_min_h, rst->rw.m_vibe_inparam.loop_length, rst->rw.m_vibe_inparam.topth, rst->rw.m_vibe_inparam.lowth, rst->rw.m_vibe_inparam.loop_start_left,
                                    rst->rw.m_vibe_inparam.loop_start_right, rst->rw.m_vibe_inparam.vibe_left, rst->rw.m_vibe_inparam.vibe_right, rst->rw.m_vibe_inparam.vibe_top, rst->rw.m_vibe_inparam.vibe_bottom, rst->ro.zoomTimes);
                            fflush(stdout);

                            FsPrintf2(1, "m_linkage_dome2gun: %d  m_linkage_pre_p:%lf m_linkage_pre_t:%lf m_linkage_pre_z:%lf m_camera_moved_overTime:%d \n", rst->rw.m_linkage_dome2gun, rst->rw.m_linkage_pre_p,
                                    rst->rw.m_linkage_pre_t, rst->rw.m_linkage_pre_z, rst->ro.m_config_devinfo.m_camera_moved_overTime);
                            fflush(stdout);

                            rst->rw.pCamera_ctrl_item = NULL;
                            rst->ro.__pSoketpool_item = NULL;

                            //如果相机类型为枪机类型
                            if (rst->ro.m_config_devinfo.m_camera_model == 2 || rst->ro.m_config_devinfo.m_camera_model == 3) {

                                unsigned short m_model = rst->ro.m_config_devinfo.m_camera_model;
                                if (rst->rw.m_linkage_dome2gun == 1) {
                                    if (rst->ro.m_config_devinfo.m_camera_model == 2)
                                        m_model = 0;
                                    else if (rst->ro.m_config_devinfo.m_camera_model == 3)
                                        m_model = 1;
                                }

                                //获取检测区域
                                gunlinkage_load_maskImage_from_file(rst);

                                printf(">>>>%d %d<<<<\n", rst->ro.m_config_devinfo.m_camera_brand, m_model);
                                //创建相机控制对象
                                rst->rw.pCamera_ctrl_item = camera_ctrl_tcp_item_new__IO(rst->ro.m_config_devinfo.m_camera_brand, m_model,
                                        rst->ro.m_config_devinfo.m_camera_ctrl_ip, rst->ro.m_config_devinfo.m_camera_ctrl_port, rst->ro.m_config_devinfo.m_szCameraUid,
                                        rst->ro.m_config_devinfo.m_szCameraPwd, 0);

                                FsPrintf2(1, "create the camera_ctrl_item (cambrand:%d cammodel:%d camctrlip:%s camctrlport:%d user:%s pwd:%s),m_model:%d \n", rst->ro.m_config_devinfo.m_camera_brand,
                                        rst->ro.m_config_devinfo.m_camera_model, rst->ro.m_config_devinfo.m_camera_ctrl_ip, rst->ro.m_config_devinfo.m_camera_ctrl_port,
                                        rst->ro.m_config_devinfo.m_szCameraUid, rst->ro.m_config_devinfo.m_szCameraPwd, m_model);
                                fflush(stdout);

                                //拉流
                                rst->ro._maskIndex = configManager_mask_register_pthreadSafety(&rst->ro.__realMask, pGunlinkage->ro._pConfigManager, "realStream", rst->ro._uuid, Record_BaseLine + __LINE__);
                                configManager_mask_value_set_pthreadSafety(pGunlinkage->ro._pConfigManager, rst->ro.__realMask, rst->ro._maskIndex, 1);

                                //创建socket连接池，用来和联动球机通讯
                                unsigned int ip = fs_ipv4_network_get(strlen(rst->ro.m_config_devinfo.m_linkage_server_ip), rst->ro.m_config_devinfo.m_linkage_server_ip);
                                rst->ro.__pSoketpool_item = socketPool_item_new_standard__IO(NULL, 0, ip, 16863, 0x3, sizeof (FsObjectBasePthreadSafety));

                                printf("linkage server ip:%s ip:%d \n", rst->ro.m_config_devinfo.m_linkage_server_ip, ip);

                                socketPool_add_item__OI_2(pGunlinkage->p.pSocketPool, rst->ro.__pSoketpool_item);

                                /* 绑定命令字 保存视频录像*/
                                configManager_cmd_connect(pGunlinkage->ro._pConfigManager, "video_part", rst->ro._uuid, rst, gun_private_cmd_connect_cb, rst);
                            }

                            FsPrintf2(1, "**************************vGunLinkageConfig*********************************\n");
                            fflush(stdout);

                            /* 所属的记录对象 */
                            rst->ro._pGunlinkage = pGunlinkage;
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 连接和调用外部命令所需要的同步锁 */
                            pthread_mutex_init(&rst->ro.__mutexCmdConnect, NULL);
                        }

                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        //if (channel > 0) configManager_cmd_connect(pRecord->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))record_private_cmd_connect_cb, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 释放内存空间 */
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0) *ppGunlinkage_item = rst;
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

void check(struct Gunlinkage_item *pGunlinkage_item) {
    //BOOL ret = camera_ctrl_tcp_GetDVRConfig(pGunlinkage_item->rw.pCamera_ctrl_item);
}

void test2(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame *pFrame) {

    double l = 100 / 1920.0;
    double r = 200 / 1920.0;
    double t = 100 / 1080.0;
    double b = 200 / 1080.0;
    printf(">>>>>>>>>>>test1 %p<<<<<<<<<<<<<\n", pGunlinkage_item->rw.pCamera_ctrl_item);
    //camera_crtl_tcp_FocusRect(pGunlinkage_item->rw.pCamera_ctrl_item, l, r, t, b);
    camera_ctrl_tcp_goPTZ(pGunlinkage_item->rw.pCamera_ctrl_item, 2.799988, 0.490387, 8.000000);
    //    char filepath[512];              
    //     sprintf(filepath, "/fs/project/data/temp/______.bmp");  
    //
    //     ImageDib* pdib = image_dib_new__IO(pFrame->width[0], pFrame->height[0], ImageColorType_Gray, 0);
    //     memcpy(pdib->data, pFrame->data.raw[0]->data, pFrame->width[0] * pFrame->height[0]);
    //     image_dib_svae_to_file(pdib, filepath);
    //     image_dib_delete__OI(&pdib);   
}

/*
 * 输入参数：
 * struct Gunlinkage_item *pGunlinkage_item：
 * int w, int h：       
 * int index：                                          目标序号
 * int model：                                          0：不需要计算平均速度，1：需要计算平均速度
 * 
 * 输出参数 
 * int *outl, int *outr, int *outt, int *outb：         修正后的目标坐标
 * float* speed:                                        目标运动的平均速度
 * 
 * 修正目标坐标位置
 * 1.目标的高度容易受倒影影响，以及船的桅杆影响。
 * 2.目标的长度容易受船尾水波纹影响。
 * 
 * 20170531：
 *  1.根据高度判断是否为大船
 *  2.如果是大渔船，则根据标定，求出标定下的大渔船的长度，如果跟踪算法输出的长度大于标定长度的20%,则使用标定的长度，根据运动方向修正坐标
 *  3.如果是小船，如果跟踪算法输出的平均长度小于10(经验值)，则判定为干扰
 *  4.如果是小船，如果跟踪算法输出的平均长度小于最后一次更新的长度，则修正坐标
 * 
 * 20170602：
 * 1.求船的平均速度
 */
void gunlinkage_correct_objPos(struct Gunlinkage_item *pGunlinkage_item, int w, int h, int index, int model, int *outl, int *outr, int *outt, int *outb, double* speedx, double* speedy) {

    //从跟踪轨迹中取出目标大小，并做一个平均
    int count = pGunlinkage_item->rw.m_trig_out[index].count;
    int i = 0, j = 0;

    int cnt = 0;
    int temp = 0;
    int temp1 = 0;
    int temp2 = 0;

    //
    if (count < POINTCOUNT) {

        for (i = 0; i < count - 1; i++) {

            //上一个点
            int last_count = (pGunlinkage_item->rw.m_trig_out[index].count - i) % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            //上上一个点
            int last_count_1 = last_count - 1;
            if (last_count_1 < 0) last_count_1 = POINTCOUNT - 1;

            int ww = pGunlinkage_item->rw.m_trig_out[index].r[last_count] - pGunlinkage_item->rw.m_trig_out[index].l[last_count];
            int ww1 = pGunlinkage_item->rw.m_trig_out[index].r[last_count_1] - pGunlinkage_item->rw.m_trig_out[index].l[last_count_1];
            int hh = pGunlinkage_item->rw.m_trig_out[index].b[last_count] - pGunlinkage_item->rw.m_trig_out[index].t[last_count];

            temp1 += hh;
            temp2 += ww;
            //前后两个宽度大于5，ww不纳入统计
            if (abs(ww - ww1) <= 5) {
                temp += ww;
                cnt++;
            }
        }
    } else {

        for (i = 0; i < POINTCOUNT - 1; i++) {
            //上一个点
            int last_count = (pGunlinkage_item->rw.m_trig_out[index].count - i) % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            //上上一个点
            int last_count_1 = last_count - 1;
            if (last_count_1 < 0) last_count_1 = POINTCOUNT - 1;

            int ww = pGunlinkage_item->rw.m_trig_out[index].r[last_count] - pGunlinkage_item->rw.m_trig_out[index].l[last_count];
            int ww1 = pGunlinkage_item->rw.m_trig_out[index].r[last_count_1] - pGunlinkage_item->rw.m_trig_out[index].l[last_count_1];
            int hh = pGunlinkage_item->rw.m_trig_out[index].b[last_count] - pGunlinkage_item->rw.m_trig_out[index].t[last_count];

            temp1 += hh;
            temp2 += ww;
            //前后两个宽度大于5，ww不纳入统计
            if (abs(ww - ww1) <= 5) {
                temp += ww;
                cnt++;
            }
        }
    }

    //均值
    int aveW = 0, aveH = 0, aveW1 = 0;
    if (cnt > 0)
        aveW = temp / cnt;
    if (cnt > 0) {
        aveH = temp1 / count;
        aveW1 = temp2 / count;
    }

    pGunlinkage_item->rw.m_debug_aveW = aveW;
    pGunlinkage_item->rw.m_debug_aveH = aveH;
    pGunlinkage_item->rw.m_debug_aveW1 = aveW1;


    //上一个点
    int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
    if (last_count == 0) last_count = POINTCOUNT - 1;
    else last_count -= 1;

    int boatw = pGunlinkage_item->rw.m_trig_out[index].r[last_count] - pGunlinkage_item->rw.m_trig_out[index].l[last_count];
    int boath = pGunlinkage_item->rw.m_trig_out[index].b[last_count] - pGunlinkage_item->rw.m_trig_out[index].t[last_count];
    int bottom = pGunlinkage_item->rw.m_trig_out[index].b[last_count];

    //相关标定参数
    int large_y0 = 140; //大船在这两个位置处的长度
    int large_y1 = 202;
    int large_len0 = 90;
    int large_len1 = 185;

    int small_maxlen = 100;

    //计算大船在bottom处的标定长度
    //大船: (y1-y0)/(length1-length0) = (bottom-y0)/(boatw-length0)
    int corr_boatw = 0;
    corr_boatw = ((large_len1 - large_len0) / (large_y1 - large_y0))*(bottom - large_y0) + large_len0;

    //干扰，水波
    //    if (aveW1 < 10 || aveH < 8) {
    //        *outl = -1;
    //        *outl = -1;
    //        *outl = -1;
    //        *outl = -1;
    //
    //        pGunlinkage_item->rw.m_debug_aveflag = 0;
    //        printf("%s %d find a error boat, aveW:%d , aveW1:%d ,aveH:%d ,count:%d ,cnt:%d ,boatw:%d ,boath:%d ,bottom:%d ,corr_boatw:%d ,model:%d \n", __FUNCTION__, __LINE__, aveW, aveW1,aveH,count,cnt,boatw,boath,bottom,corr_boatw,model);
    //    }

    //如果目标的长度大于标定长度的20%,则肯定船尾有水波纹

    //计算速度....
    if (model == 1) {
        int pointCnt = 20;
        pointCnt = pointCnt < count ? count : pointCnt;
        double len = 0.0;
        int time = 0;
        //向右运动,计算x方向速度
        if (pGunlinkage_item->rw.m_trig_out[index].type == 1 || pGunlinkage_item->rw.m_trig_out[index].type == 2) {

            //当前位置与初始位置相比：
            //上一个点的中心点坐标
            int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            len = (pGunlinkage_item->rw.m_trig_out[index].r[last_count] - pGunlinkage_item->rw.m_trig_out[index].init_r) / (double) w;
            time = pGunlinkage_item->rw.m_trig_out[index].difTime[last_count]; // - pGunlinkage_item->rw.m_trig_out[index].difTime[pGunlinkage_item->rw.m_trig_out[index].count - pointCnt + 1];
            if (time <= 0) *speedx = 0.0;
            else *speedx = (double) len / (double) time;

            printf("%s %d,speedx:%f -[time:%d difTime:%d startTime:%f]-[len:%lf r1[%d]:%d r2[0]:%d w:%d] \n", __FUNCTION__, __LINE__, *speedx, time,
                    pGunlinkage_item->rw.m_trig_out[index].difTime[last_count],
                    pGunlinkage_item->rw.m_trig_out[index].startTime, len,
                    last_count,
                    pGunlinkage_item->rw.m_trig_out[index].r[pGunlinkage_item->rw.m_trig_out[index].count - 1],
                    pGunlinkage_item->rw.m_trig_out[index].init_r, w);
            char message[512];
            sprintf(message, "%s %d,speedx:%f -[time:%d difTime:%d startTime:%f]-[len:%lf r1[%d]:%d r2[0]:%d w:%d] \n", __FUNCTION__, __LINE__, *speedx, time,
                    pGunlinkage_item->rw.m_trig_out[index].difTime[last_count],
                    pGunlinkage_item->rw.m_trig_out[index].startTime, len,
                    last_count,
                    pGunlinkage_item->rw.m_trig_out[index].r[pGunlinkage_item->rw.m_trig_out[index].count - 1],
                    pGunlinkage_item->rw.m_trig_out[index].init_r, w);
            MakeLog(message, 0);
        } else {

            //上一个点的中心点坐标
            int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            len = -(double) (pGunlinkage_item->rw.m_trig_out[index].l[last_count] - pGunlinkage_item->rw.m_trig_out[index].init_l) / (double) w;
            time = pGunlinkage_item->rw.m_trig_out[index].difTime[last_count]; //- pGunlinkage_item->rw.m_trig_out[index].difTime[pGunlinkage_item->rw.m_trig_out[index].count - pointCnt + 1];
            if (time <= 0) *speedx = 0.0;
            else *speedx = (double) len / (double) time;
            printf("%s %d,speedx:%f -(time:%d difTime:%d startTime:%f)-(len:%lf l1[%d]:%d l2[0]:%d w:%d) \n", __FUNCTION__, __LINE__, *speedx, time,
                    pGunlinkage_item->rw.m_trig_out[index].difTime[last_count],
                    pGunlinkage_item->rw.m_trig_out[index].startTime, len,
                    last_count,
                    pGunlinkage_item->rw.m_trig_out[index].l[pGunlinkage_item->rw.m_trig_out[index].count - 1],
                    pGunlinkage_item->rw.m_trig_out[index].init_r, w);
            char message[512];
            sprintf(message, "%s %d,speedx:%f -(time:%d difTime:%d startTime:%f)-(len:%lf l1[%d]:%d l2[0]:%d w:%d) \n", __FUNCTION__, __LINE__, *speedx, time,
                    pGunlinkage_item->rw.m_trig_out[index].difTime[last_count],
                    pGunlinkage_item->rw.m_trig_out[index].startTime, len,
                    last_count,
                    pGunlinkage_item->rw.m_trig_out[index].l[pGunlinkage_item->rw.m_trig_out[index].count - 1],
                    pGunlinkage_item->rw.m_trig_out[index].init_r, w);
            MakeLog(message, 0);
        }


        //当前位置与初始位置相比：
        //上一个点的中心点坐标
        int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;

        int len1 = (pGunlinkage_item->rw.m_trig_out[index].t[last_count] - pGunlinkage_item->rw.m_trig_out[index].init_t);
        int len2 = (pGunlinkage_item->rw.m_trig_out[index].b[last_count] - pGunlinkage_item->rw.m_trig_out[index].init_b);
        len = ((double) (len1 + len2) / 2) / (double) h;
        time = pGunlinkage_item->rw.m_trig_out[index].difTime[last_count]; // - pGunlinkage_item->rw.m_trig_out[index].difTime[pGunlinkage_item->rw.m_trig_out[index].count - pointCnt + 1];
        if (time <= 0) *speedy = 0.0;
        else *speedy = (double) len / (double) time;

        printf("len1:%d  t[%d]:%d  t[0]:%d \n", len1,
                last_count, pGunlinkage_item->rw.m_trig_out[index].t[last_count], pGunlinkage_item->rw.m_trig_out[index].init_t);

        printf("len2:%d  b[%d]:%d  b[0]:%d \n", len2,
                last_count, pGunlinkage_item->rw.m_trig_out[index].b[last_count], pGunlinkage_item->rw.m_trig_out[index].init_b);

        printf("%s %d,speedy:%f -(time:%d difTime:%d startTime:%d)-(len:%lf len1:%d len2:%d w:%d) \n", __FUNCTION__, __LINE__, *speedy, time,
                pGunlinkage_item->rw.m_trig_out[index].difTime[last_count],
                pGunlinkage_item->rw.m_trig_out[index].startTime, len, len1, len2, w);
        char message[512];
        sprintf(message, "%s %d,speedy:%f -(time:%d difTime:%d startTime:%d)-(len:%lf len1:%d len2:%d w:%d) \n", __FUNCTION__, __LINE__, *speedy, time,
                pGunlinkage_item->rw.m_trig_out[index].difTime[last_count],
                pGunlinkage_item->rw.m_trig_out[index].startTime, len, len1, len2, w);
        MakeLog(message, 0);
    }



    if (boatw > (corr_boatw + corr_boatw * 20 / 100)) {

        //上一个点的中心点坐标
        int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;


        //向右运动
        if (pGunlinkage_item->rw.m_trig_out[index].type == 1 || pGunlinkage_item->rw.m_trig_out[index].type == 2) {
            *outl = pGunlinkage_item->rw.m_trig_out[index].r[last_count] - corr_boatw;
            *outr = pGunlinkage_item->rw.m_trig_out[index].r[last_count];
            *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
            *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];
        } else {
            *outl = pGunlinkage_item->rw.m_trig_out[index].l[last_count];
            *outr = pGunlinkage_item->rw.m_trig_out[index].l[last_count] + corr_boatw;
            *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
            *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];
        }


        pGunlinkage_item->rw.m_trig_out[index].m_export_count = 100;
        //pGunlinkage_item->rw.m_debug_aveflag = 1;
        //        printf("%s %d find a obj follow a water ripple,[l:%d r:%d] aveW:%d , aveW1:%d ,aveH:%d ,count:%d ,cnt:%d ,boatw:%d ,boath:%d ,bottom:%d ,corr_boatw:%d ,model:%d \n",
        //                __FUNCTION__, __LINE__, *outl, *outr, aveW, aveW1, aveH, count, cnt, boatw, boath, bottom, corr_boatw, model);
        return;
    }

    if (pGunlinkage_item->rw.m_trig_out[index].m_export_count < EXPORT_CONDITION) {
        return;
    }


    int flag = 0;
    //判定是否为大船
    if (bottom < 160) {
        if ((aveH > boath ? aveH : boath) > 30) {
            flag = 1;
        }
    } else if (bottom < 190) {
        if ((aveH > boath ? aveH : boath) > 35) {
            flag = 1;
        }
    } else if (bottom < 210) {
        if ((aveH > boath ? aveH : boath) > 40) {
            flag = 1;
        }
    } else {
        if ((aveH > boath ? aveH : boath) > 50) {
            flag = 1;
        }
    }


    //小船类型，如果长度大于小船的最大长度
    if (flag == 0) {
        pGunlinkage_item->rw.m_debug_aveflag = 1;
        //如果是小船类型，如果长度大于小船的标定最大长度,则强制修正目标长度为标定最大长度
        if ((aveW > boatw ? boatw : aveW) > small_maxlen) {

            //上一个点的中心点坐标
            int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            //向右运动
            if (pGunlinkage_item->rw.m_trig_out[index].type == 1 || pGunlinkage_item->rw.m_trig_out[index].type == 2) {
                *outl = pGunlinkage_item->rw.m_trig_out[index].r[last_count] - small_maxlen;
                *outr = pGunlinkage_item->rw.m_trig_out[index].r[last_count];
                *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
                *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];
            } else {
                *outl = pGunlinkage_item->rw.m_trig_out[index].l[last_count];
                *outr = pGunlinkage_item->rw.m_trig_out[index].l[last_count] + small_maxlen;
                *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
                *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];
            }

            //            printf("%s %d, find a small boat,[l:%d r:%d],and it is too long,aveW:%d , aveW1:%d ,aveH:%d ,count:%d ,cnt:%d ,boatw:%d ,boath:%d ,bottom:%d ,corr_boatw:%d ,model:%d \n",
            //                    __FUNCTION__, __LINE__, *outl, *outr, aveW, aveW1, aveH, count, cnt, boatw, boath, bottom, corr_boatw, model);
            return;
        }
    }


    //大船或小船的长度都没有超过标定的长度：
    //上一个点的中心点坐标
    last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
    if (last_count == 0) last_count = POINTCOUNT - 1;
    else last_count -= 1;

    *outl = pGunlinkage_item->rw.m_trig_out[index].l[last_count];
    *outr = pGunlinkage_item->rw.m_trig_out[index].r[last_count];
    *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
    *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];

    //平均值小于最后一次值
    if (aveW < boatw) {
        //向右运动
        if (pGunlinkage_item->rw.m_trig_out[index].type == 1 || pGunlinkage_item->rw.m_trig_out[index].type == 2) {
            *outl = pGunlinkage_item->rw.m_trig_out[index].r[last_count] - aveW;
            *outr = pGunlinkage_item->rw.m_trig_out[index].r[last_count];
            *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
            *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];
        } else {

            *outl = pGunlinkage_item->rw.m_trig_out[index].l[last_count];
            *outr = pGunlinkage_item->rw.m_trig_out[index].l[last_count] + aveW;
            *outt = pGunlinkage_item->rw.m_trig_out[index].t[last_count];
            *outb = pGunlinkage_item->rw.m_trig_out[index].b[last_count];
        }
    }
}

int vibe_handle_obj_info(trigInfo* ptrigInfo, int index, int w, int h, double* speedx, double* speedy);
int vibe_handle_obj_info_1(trigInfo* ptrigInfo, int index, int *outl, int *outr, int *outt, int *outb, int* debug);

/*
 * 发送目标编号为obj_number的目标信息
 * 
 * int flag：  1:表示目标还在，发送正确的坐标信息； 0：表示目标已经消失，发送坐标信息为0
 * 
 */
void gunlinkage_send_obj(struct Gunlinkage_item *pGunlinkage_item, int w, int h, int index, int flag) {

    if (flag == 1) {

        int correct_left = 0, correct_right = 0, correct_top = 0, correct_bottom = 0;
        double correct_speed_x = 0.0, correct_speed_y = 0.0;

        int debug[5] = {0};
        vibe_handle_obj_info_1(pGunlinkage_item->rw.m_trig_out, index, &correct_left, &correct_right, &correct_top, &correct_bottom, debug);
        pGunlinkage_item->rw.m_debug_aveW = debug[0];
        pGunlinkage_item->rw.m_debug_aveW1 = debug[1];
        pGunlinkage_item->rw.m_debug_aveH = debug[2];
        vibe_handle_obj_info(pGunlinkage_item->rw.m_trig_out, index, w, h, &correct_speed_x, &correct_speed_y);

        //获取信息：
        double m_left = (double) correct_left / (double) w;
        double m_right = (double) correct_right / (double) w;
        double m_top = (double) correct_top / (double) h;
        double m_bottom = (double) correct_bottom / (double) h;
        double obj_num = pGunlinkage_item->rw.m_trig_out[index].index;
        int dir = pGunlinkage_item->rw.m_trig_out[index].type;
        double speedx = correct_speed_x;
        double speedy = correct_speed_y;

        //修正后的坐标
        pGunlinkage_item->rw.m_debug_c_left = correct_left;
        pGunlinkage_item->rw.m_debug_c_right = correct_right;
        pGunlinkage_item->rw.m_debug_c_top = correct_top;
        pGunlinkage_item->rw.m_debug_c_bottom = correct_bottom;
        pGunlinkage_item->rw.m_debug_c_speed = correct_speed_x;

        //修正前的坐标
        //上一个点的中心点坐标
        int last_count = pGunlinkage_item->rw.m_trig_out[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;

        pGunlinkage_item->rw.m_debug_left = (double) pGunlinkage_item->rw.m_trig_out[index].l[last_count] / (double) w;
        pGunlinkage_item->rw.m_debug_right = (double) pGunlinkage_item->rw.m_trig_out[index].r[last_count] / (double) w;
        pGunlinkage_item->rw.m_debug_top = (double) pGunlinkage_item->rw.m_trig_out[index].t[last_count] / (double) h;
        pGunlinkage_item->rw.m_debug_bottom = (double) pGunlinkage_item->rw.m_trig_out[index].b[last_count] / (double) h;

        FsEbml *pEbml = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "command_type", FsEbmlNodeType_String), "linkage_snap");

        char temp[512];
        struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "command_for_uuid", FsEbmlNodeType_String);
        sprintf(temp, "%d", pGunlinkage_item->ro.m_config_devinfo.m_linkage_server_uuid);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "deviceno", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.deviceno);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "devicename", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.devicename);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "position_jwd", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.position_jwd);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "area_jwd", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.area_jwd);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "video_path", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->rw.m_linage_videoPath + strlen("/fs/project/data"));
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "real_url", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_linkage_rtspURL);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "gun_uuid", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro._uuid);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        unsigned int m_ipv4 = 0;
        fs_Socket_addr_get_local(&m_ipv4, NULL, pGunlinkage_item->ro.__pSoketpool_item->p.__sock);

        * (long*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "gun_camera_ip", FsEbmlNodeType_Integer)->data.buffer = m_ipv4;

        *(long*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "number", FsEbmlNodeType_Integer)->data.buffer = obj_num;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "left", FsEbmlNodeType_Float)->data.buffer = m_left;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "right", FsEbmlNodeType_Float)->data.buffer = m_right;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "top", FsEbmlNodeType_Float)->data.buffer = m_top;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "bottom", FsEbmlNodeType_Float)->data.buffer = m_bottom;

        *(long*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "dir", FsEbmlNodeType_Integer)->data.buffer = dir;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "speedx", FsEbmlNodeType_Float)->data.buffer = speedx;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "speedy", FsEbmlNodeType_Float)->data.buffer = speedy;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "thistime", FsEbmlNodeType_Float)->data.buffer = fs_time_uptime_get();


        FsString* pSenddata = fs_Ebml_to_String__IO(pEbml, 8, 0, NULL);
        fs_Ebml_delete__OI(pEbml, NULL);
        ((unsigned int*) pSenddata->buffer)[0] = 0x1;
        ((unsigned int*) pSenddata->buffer)[1] = pSenddata->lenth - 8;

        socketPool_item_send_OI_2(pGunlinkage_item->ro.__pSoketpool_item, pSenddata);

    } else if (flag == 0) {

        FsEbml *pEbml = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        fs_Ebml_node_data_set_string(fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "command_type", FsEbmlNodeType_String), "linkage_snap");

        char temp[512];
        struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "command_for_uuid", FsEbmlNodeType_String);
        sprintf(temp, "%d", pGunlinkage_item->ro.m_config_devinfo.m_linkage_server_uuid);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "deviceno", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.deviceno);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "devicename", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.devicename);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "position_jwd", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.position_jwd);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "area_jwd", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro.m_param_config.area_jwd);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "video_path", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "test");

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "real_url", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "test");

        pEbml_node = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "gun_uuid", FsEbmlNodeType_String);
        sprintf(temp, "%s", pGunlinkage_item->ro._uuid);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        unsigned int m_ipv4 = 0;
        fs_Socket_addr_get_local(&m_ipv4, NULL, pGunlinkage_item->ro.__pSoketpool_item->p.__sock);

        * (long*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "gun_camera_ip", FsEbmlNodeType_Integer)->data.buffer = m_ipv4;

        *(long*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "number", FsEbmlNodeType_Integer)->data.buffer = 0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "left", FsEbmlNodeType_Float)->data.buffer = -1.0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "right", FsEbmlNodeType_Float)->data.buffer = -1.0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "top", FsEbmlNodeType_Float)->data.buffer = -1.0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "bottom", FsEbmlNodeType_Float)->data.buffer = -1.0;

        *(long*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "dir", FsEbmlNodeType_Integer)->data.buffer = 0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "speedx", FsEbmlNodeType_Float)->data.buffer = 0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "speedy", FsEbmlNodeType_Float)->data.buffer = 0;
        *(double*) fs_Ebml_node_addChild(pEbml, (struct FsEbml_node*) pEbml, "thistime", FsEbmlNodeType_Float)->data.buffer = 0;


        FsString* pSenddata = fs_Ebml_to_String__IO(pEbml, 8, 0, NULL);
        fs_Ebml_delete__OI(pEbml, NULL);
        ((unsigned int*) pSenddata->buffer)[0] = 0x1;
        ((unsigned int*) pSenddata->buffer)[1] = pSenddata->lenth - 8;

        socketPool_item_send_OI_2(pGunlinkage_item->ro.__pSoketpool_item, pSenddata);
    }

}

static int gunlinkage_recv_info(struct Gunlinkage_item *pGunlinkage_item, int w, int h
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {

    if (pGunlinkage_item->ro.__pSoketpool_item->ro.__recvList->nodeCount > 0) {
        FsString * pRecvdata = socketPool_item_recv_get__IO(pGunlinkage_item->ro.__pSoketpool_item);
        if (pRecvdata->lenth > 8) {
            if (pRecvdata->buffer[0] == 0xB && pRecvdata->lenth > 11) { //发送最终转动数据
                int obj_number = *(unsigned int *) (pRecvdata->buffer + 8);
                int i;
                for (i = 0; i < TRIGGERCOUNT; i++) {
                    if (pGunlinkage_item->rw.m_trig_out[i].used == 0)
                        continue;

                    if (pGunlinkage_item->rw.m_trig_out[i].index == obj_number) {

                        pGunlinkage_item->rw.m_debug_number = obj_number;
                        pGunlinkage_item->rw.m_debug_sendpos = 1;
                        //发送信息...
                        printf("%s %d : gun recv the request,and send the new info(obj_num:%d) w:%d h:%d\n", __FUNCTION__, __LINE__, obj_number, w, h);
                        char message[512];
                        sprintf(message, "%s %d : gun recv the request,and send the new info(obj_num:%d) w:%d h:%d\n", __FUNCTION__, __LINE__, obj_number, w, h);
                        MakeLog(message, 0);

                        gunlinkage_send_obj(pGunlinkage_item, w, h, i, 1);
                        break;
                    }
                }
                //当再次获取目标最新位置时，该目标已经消失
                if (i == TRIGGERCOUNT) {
                    gunlinkage_send_obj(pGunlinkage_item, w, h, i, 0);
                    printf("%s %d : gun recv the request,but cannot find the number(obj_num:%d) w:%d h:%d\n", __FUNCTION__, __LINE__, obj_number, w, h);
                    char message[512];
                    sprintf(message, "%s %d : gun recv the request,but cannot find the number(obj_num:%d) w:%d h:%d\n", __FUNCTION__, __LINE__, obj_number, w, h);
                    MakeLog(message, 0);
                }

            } else if (pRecvdata->buffer[0] == 0x3) { //接收球机结果

                FsEbml* pEbml = fs_Ebml_new_from_String__IO(pRecvdata->lenth - 8, (unsigned char *) pRecvdata->buffer + 8, NULL, pShareBuffer);
                if (fs_Ebml_analyzeAll(pEbml, (struct FsEbml_node*) pEbml) == NULL) {

                    //取数据    
                    int snap = *(long*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "snap")->data.buffer;
                    int obj_number = *(long*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "number")->data.buffer;
                    //snap为零，拍照成功，非零，拍照失败
                    if (snap == 0) {
                        //保存jpg数据
                        struct FsEbml_node * pNode = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "image_byte");
                        unsigned char* pdstbuffer = (unsigned char*) fsMalloc(1024 * 1024 * 1);
                        memcpy(pdstbuffer, pNode->data.buffer, pNode->data.lenth);

                        printf("%s %d : gun recv the pic success (obj_num:%d)\n", __FUNCTION__, __LINE__, obj_number);

                        if (0) {
                            char path[128];
                            sprintf(path, "/fs/project/data/temp/snap_%d.jpg", obj_number);
                            FILE* f = fopen(path, "wb");
                            if (f != NULL) {
                                fwrite(pdstbuffer, pNode->data.lenth, 1, f);
                                fclose(f);
                            }
                        }

                        if (pdstbuffer) fsFree(pdstbuffer);

                    } else {
                        //printf("%s %d : gun recv the pic failed (obj_num:%d errorno:%d)\n", __FUNCTION__, __LINE__, obj_number, snap);
                    }

                    //更新目标链表
                    int i;
                    for (i = 0; i < TRIGGERCOUNT; i++) {
                        if (pGunlinkage_item->rw.m_trig_out[i].used == 0)
                            continue;

                        if (pGunlinkage_item->rw.m_trig_out[i].index == obj_number) {
                            //将目标标志设置为2，已处理完毕
                            pGunlinkage_item->rw.m_trig_out[i].dealed = 2;
                            printf("%s %d : gun recv the pic(obj_num:%d),reset the param dealed:%d \n", __FUNCTION__, __LINE__, obj_number, pGunlinkage_item->rw.m_trig_out[i].dealed);
                            char message[512];
                            sprintf(message, "%s %d : gun recv the pic(obj_num:%d),reset the param dealed:%d \n", __FUNCTION__, __LINE__, obj_number, pGunlinkage_item->rw.m_trig_out[i].dealed);
                            MakeLog(message, 0);
                            break;
                        }
                    }
                } else {

                }
                fs_Ebml_delete__OI(pEbml, NULL);
            } else {
                //error
            }
        }

        fs_String_delete__OI(pRecvdata);
    }

    return 0;
}

int gunlinkage_checkTimeValid(struct Gunlinkage_item *pGunlinkage_item) {

    pGunlinkage_item->rw.m_linkage_timevalid_flag = 0;

    time_t ts = (time_t) fs_time_GMT_get();
    struct tm ti_s;
    localtime_r(&ts, &ti_s);

    int hour = ti_s.tm_hour;
    int min = ti_s.tm_min;

    double curr_time = hour + min / 100.0;

    if (pGunlinkage_item->ro.m_config_vibe.vibe_start_time < pGunlinkage_item->ro.m_config_vibe.vibe_end_time) {
        if (curr_time > pGunlinkage_item->ro.m_config_vibe.vibe_start_time && curr_time < pGunlinkage_item->ro.m_config_vibe.vibe_end_time) {
            pGunlinkage_item->rw.m_linkage_timevalid_flag = 1;
        }
    } else {
        if (curr_time < pGunlinkage_item->ro.m_config_vibe.vibe_end_time || curr_time > pGunlinkage_item->ro.m_config_vibe.vibe_start_time) {
            pGunlinkage_item->rw.m_linkage_timevalid_flag = 1;
        }
    }

    //如果不在
    if (0 == pGunlinkage_item->rw.m_linkage_timevalid_flag) {
        pGunlinkage_item->rw.m_vibeparam.m_bInitOK = 0;
    }

    return 0;
}

int gunlinkage_checkCameraMoved(struct Gunlinkage_item *pGunlinkage_item) {

    if (pGunlinkage_item->rw.m_linkage_dome2gun == 1) {
        if (abs(pGunlinkage_item->rw.m_linkave_camMoved_checkTime - fs_time_uptime_get()) > 1) {
            pGunlinkage_item->rw.m_linkave_camMoved_checkTime = fs_time_uptime_get();
            if (pGunlinkage_item->rw.m_linkage_camMoved_flag == 1) {
                if (abs(pGunlinkage_item->rw.m_linkage_camMoved_time - fs_time_uptime_get()) > pGunlinkage_item->ro.m_config_devinfo.m_camera_moved_overTime) {
                    int ret = camera_ctrl_tcp_goPTZ(pGunlinkage_item->rw.pCamera_ctrl_item, pGunlinkage_item->rw.m_linkage_pre_p,
                            pGunlinkage_item->rw.m_linkage_pre_t, pGunlinkage_item->rw.m_linkage_pre_z);

                    printf("%s %d the camera was not moved over %d seconds,handle to set the Pre PTZ(=%d).\n", __FUNCTION__, __LINE__, pGunlinkage_item->ro.m_config_devinfo.m_camera_moved_overTime, ret);

                    char message[512];
                    sprintf(message, "%s %d the camera was not moved over %d seconds,handle to set the Pre PTZ(=%d).\n", __FUNCTION__, __LINE__, pGunlinkage_item->ro.m_config_devinfo.m_camera_moved_overTime, ret);
                    MakeLog(message, 0);

                    if (ret != 0) {
                        printf("%s %d camera goto ptz failed (=%d).\n", __FUNCTION__, __LINE__, ret);
                        char message[512];
                        sprintf(message, "%s %d camera goto ptz failed (=%d).\n", __FUNCTION__, __LINE__, ret);
                        MakeLog(message, 0);
                        return -1;
                    }

                    pGunlinkage_item->rw.m_linkage_camMoved_time = 0.0;
                    pGunlinkage_item->rw.m_linkage_camMoved_flag = 0;
                }
            }

            //获取当前ptz
            float curr_p, curr_t, curr_z;
            int ret = camera_ctrl_tcp_getPTZ(pGunlinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
            if (ret != 0) {
                printf("%s %d camera get ptz failed (=%d).\n", __FUNCTION__, __LINE__, ret);
                char message[512];
                sprintf(message, "%s %d camera get ptz failed (=%d).\n", __FUNCTION__, __LINE__, ret);
                MakeLog(message, 0);
                return -1;
            }

            //如果相机已经移动，获取当前ptz与上次相机移动时ptz的差值
            //如果相机没有移动，获取当前ptz与相机检测预置位ptz的差值
            double dif_p, dif_t, dif_z;
            if (pGunlinkage_item->rw.m_linkage_camMoved_flag == 1) {
                dif_p = abs(pGunlinkage_item->rw.m_linkage_camMoved_p - curr_p);
                dif_t = abs(pGunlinkage_item->rw.m_linkage_camMoved_t - curr_t);
                dif_z = abs(pGunlinkage_item->rw.m_linkage_camMoved_z - curr_z);
            } else {
                dif_p = abs(curr_p - pGunlinkage_item->rw.m_linkage_pre_p);
                dif_t = abs(curr_t - pGunlinkage_item->rw.m_linkage_pre_t);
                dif_z = abs(curr_z - pGunlinkage_item->rw.m_linkage_pre_z);
            }

            if (dif_z > 2.0 || dif_p > 2.0 || dif_t > 2.0) {
                //第一次移动时刻
                if (0 == pGunlinkage_item->rw.m_linkage_camMoved_flag) {

                    printf("%s %d the camera first moved(%lf %lf %lf)-(%lf %lf %lf).\n", __FUNCTION__, __LINE__, curr_p, curr_t, curr_z, pGunlinkage_item->rw.m_linkage_pre_p,
                            pGunlinkage_item->rw.m_linkage_pre_t, pGunlinkage_item->rw.m_linkage_pre_z);
                    char message[512];
                    sprintf(message, "%s %d the camera first moved(%lf %lf %lf)-(%lf %lf %lf).\n", __FUNCTION__, __LINE__, curr_p, curr_t, curr_z, pGunlinkage_item->rw.m_linkage_pre_p,
                            pGunlinkage_item->rw.m_linkage_pre_t, pGunlinkage_item->rw.m_linkage_pre_z);
                    MakeLog(message, 0);
                }

                pGunlinkage_item->rw.m_linkage_camMoved_p = curr_p;
                pGunlinkage_item->rw.m_linkage_camMoved_t = curr_t;
                pGunlinkage_item->rw.m_linkage_camMoved_z = curr_z;
                pGunlinkage_item->rw.m_linkage_camMoved_flag = 1;
                pGunlinkage_item->rw.m_linkage_camMoved_time = fs_time_uptime_get();

                //清除相关数据
                vibe_Del(&pGunlinkage_item->rw.m_vibeparam);
                memset(pGunlinkage_item->rw.m_trig_out, 0, sizeof (pGunlinkage_item->rw.m_trig_out));
                pGunlinkage_item->rw.curr_trig_num = 0;
            }
        }
    }

    return 0;
}

/**********************************************************************
        目标信息处理： 对输出队列中m_triginfo，某个目标index进行处理

        根据目标的大小，修正目标的坐标位置

 * 20170531：
   1.根据高度判断是否为大船
   2.如果是大渔船，则根据标定，求出标定下的大渔船的长度，如果跟踪算法输出的长度大于标定长度的20%,则使用标定的长度，根据运动方向修正坐标
   3.如果是小船，如果跟踪算法输出的平均长度小于10(经验值)，则判定为干扰
   4.如果是小船，如果跟踪算法输出的平均长度小于最后一次更新的长度，则修正坐标
 ***********************************************************************/
int vibe_handle_obj_info_1(trigInfo* ptrigInfo, int index, int *outl, int *outr, int *outt, int *outb, int* debug) {


    //////////////////////////////////////////////////////////////////////////////
    //暂时不做处理
    {
        //上一个点
        int last_count = ptrigInfo[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;

        //大船或小船的长度都没有超过标定的长度：
        //        *outl = ptrigInfo[index].l[last_count];
        //        *outr = ptrigInfo[index].r[last_count];
        //        *outt = ptrigInfo[index].t[last_count];
        //        *outb = ptrigInfo[index].b[last_count];

        *outl = ptrigInfo[index].changed_l;
        *outr = ptrigInfo[index].changed_r;
        *outt = ptrigInfo[index].changed_t;
        *outb = ptrigInfo[index].changed_b;

        return 0;
    }
    ////////////////////////////////////////////////////////////////////////////// 


    int count = ptrigInfo[index].count;
    int i = 0, j = 0;

    int cnt = 0;
    int temp = 0;
    int temp1 = 0;
    int temp2 = 0;

    if (count < POINTCOUNT) {

        for (i = 0; i < count - 1; i++) {

            //上一个点
            int last_count = (ptrigInfo[index].count - i) % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            //上上一个点
            int last_count_1 = last_count - 1;
            if (last_count_1 < 0) last_count_1 = POINTCOUNT - 1;

            int ww = ptrigInfo[index].r[last_count] - ptrigInfo[index].l[last_count];
            int ww1 = ptrigInfo[index].r[last_count_1] - ptrigInfo[index].l[last_count_1];
            int hh = ptrigInfo[index].b[last_count] - ptrigInfo[index].t[last_count];

            temp1 += hh;
            temp2 += ww;
            //前后两个宽度大于5，ww不纳入统计
            if (abs(ww - ww1) <= 5) {
                temp += ww;
                cnt++;
            }
        }
    } else {

        for (i = 0; i < POINTCOUNT - 1; i++) {
            //上一个点
            int last_count = (ptrigInfo[index].count - i) % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            //上上一个点
            int last_count_1 = last_count - 1;
            if (last_count_1 < 0) last_count_1 = POINTCOUNT - 1;

            int ww = ptrigInfo[index].r[last_count] - ptrigInfo[index].l[last_count];
            int ww1 = ptrigInfo[index].r[last_count_1] - ptrigInfo[index].l[last_count_1];
            int hh = ptrigInfo[index].b[last_count] - ptrigInfo[index].t[last_count];

            temp1 += hh;
            temp2 += ww;
            //前后两个宽度大于5，ww不纳入统计
            if (abs(ww - ww1) <= 5) {
                temp += ww;
                cnt++;
            }
        }
    }

    //均值
    int aveW = 0, aveH = 0, aveW1 = 0;
    if (cnt > 0)
        aveW = temp / cnt;
    if (cnt > 0) {
        aveH = temp1 / count;
        aveW1 = temp2 / count;
    }

    debug[0] = aveW;
    debug[1] = aveW1;
    debug[2] = aveH;



    //上一个点
    int last_count = ptrigInfo[index].count % POINTCOUNT;
    if (last_count == 0) last_count = POINTCOUNT - 1;
    else last_count -= 1;

    //printf(">>>last_count:%d count:%d  index:%d <<<\n",last_count , pthis->m_triginfo[index].count,index);

    int boatw = ptrigInfo[index].r[last_count] - ptrigInfo[index].l[last_count];
    int boath = ptrigInfo[index].b[last_count] - ptrigInfo[index].t[last_count];
    int bottom = ptrigInfo[index].b[last_count];

    //相关标定参数
    int large_y0 = 140; //大船在这两个位置处的长度
    int large_y1 = 202;
    int large_len0 = 90;
    int large_len1 = 185;

    int small_maxlen = 100;

    //计算大船在bottom处的标定长度
    //大船: (y1-y0)/(length1-length0) = (bottom-y0)/(boatw-length0)
    int corr_boatw = 0;
    corr_boatw = ((large_len1 - large_len0) / (large_y1 - large_y0))*(bottom - large_y0) + large_len0;


    //如果目标的长度大于标定长度的20%,则肯定船尾有水波纹
    if (boatw > (corr_boatw + corr_boatw * 20 / 100)) {

        //上一个点的中心点坐标
        int last_count = ptrigInfo[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;


        //向右运动
        if (ptrigInfo[index].type == 1 || ptrigInfo[index].type == 2) {
            *outl = ptrigInfo[index].r[last_count] - corr_boatw;
            *outr = ptrigInfo[index].r[last_count];
            *outt = ptrigInfo[index].t[last_count];
            *outb = ptrigInfo[index].b[last_count];
        } else {
            *outl = ptrigInfo[index].l[last_count];
            *outr = ptrigInfo[index].l[last_count] + corr_boatw;
            *outt = ptrigInfo[index].t[last_count];
            *outb = ptrigInfo[index].b[last_count];
        }

        ptrigInfo[index].m_export_count = 100;

        return 0;
    }

    //printf(">>>2<<<\n");

    if (ptrigInfo[index].m_export_count < 5) {
        return 0;
    }

    //printf(">>>3<<<\n");

    int flag = 0;
    //判定是否为大船
    if (bottom < 160) {
        if ((aveH > boath ? aveH : boath) > 30) {
            flag = 1;
        }
    } else if (bottom < 190) {
        if ((aveH > boath ? aveH : boath) > 35) {
            flag = 1;
        }
    } else if (bottom < 210) {
        if ((aveH > boath ? aveH : boath) > 40) {
            flag = 1;
        }
    } else {
        if ((aveH > boath ? aveH : boath) > 50) {
            flag = 1;
        }
    }


    //小船类型，如果长度大于小船的最大长度
    if (flag == 0) {
        //pGunlinkage_item->rw.m_debug_aveflag = 1;
        //如果是小船类型，如果长度大于小船的标定最大长度,则强制修正目标长度为标定最大长度
        if ((aveW > boatw ? boatw : aveW) > small_maxlen) {

            //上一个点的中心点坐标
            int last_count = ptrigInfo[index].count % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            //向右运动
            if (ptrigInfo[index].type == 1 || ptrigInfo[index].type == 2) {
                *outl = ptrigInfo[index].r[last_count] - small_maxlen;
                *outr = ptrigInfo[index].r[last_count];
                *outt = ptrigInfo[index].t[last_count];
                *outb = ptrigInfo[index].b[last_count];
            } else {
                *outl = ptrigInfo[index].l[last_count];
                *outr = ptrigInfo[index].l[last_count] + small_maxlen;
                *outt = ptrigInfo[index].t[last_count];
                *outb = ptrigInfo[index].b[last_count];
            }

            return 0;
        }
    }

    //大船或小船的长度都没有超过标定的长度：
    *outl = ptrigInfo[index].l[last_count];
    *outr = ptrigInfo[index].r[last_count];
    *outt = ptrigInfo[index].t[last_count];
    *outb = ptrigInfo[index].b[last_count];

    //平均值小于最后一次值
    if (aveW < boatw) {
        //向右运动
        if (ptrigInfo[index].type == 1 || ptrigInfo[index].type == 2) {
            *outl = ptrigInfo[index].r[last_count] - aveW;
            *outr = ptrigInfo[index].r[last_count];
            *outt = ptrigInfo[index].t[last_count];
            *outb = ptrigInfo[index].b[last_count];
        } else {

            *outl = ptrigInfo[index].l[last_count];
            *outr = ptrigInfo[index].l[last_count] + aveW;
            *outt = ptrigInfo[index].t[last_count];
            *outb = ptrigInfo[index].b[last_count];
        }
    }

    return 0;
}

/**********************************************************************
        目标信息处理： 对输出队列中m_triginfo，某个目标index进行处理
 * 
 *       统计目标的运动速度   
 ****************************************************************************/
int vibe_handle_obj_info(trigInfo* ptrigInfo, int index, int w, int h, double* speedx, double* speedy) {

    int count = ptrigInfo[index].count;

    double len = 0.0;
    int time = 0;
    //向右运动,计算x方向速度
    if (ptrigInfo[index].type == 1 || ptrigInfo[index].type == 2) {

        //当前位置与初始位置相比：
        //上一个点的中心点坐标
        int last_count = ptrigInfo[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;

        len = (ptrigInfo[index].r[last_count] - ptrigInfo[index].init_r) / (double) w;
        time = ptrigInfo[index].difTime[last_count]; // - pGunlinkage_item->rw.m_trig_out[index].difTime[pGunlinkage_item->rw.m_trig_out[index].count - pointCnt + 1];
        if (time <= 0) *speedx = 0.0;
        else *speedx = (double) len / (double) time;

        printf("%s %d,speedx:%f -(time:%d difTime:%d startTime:%f)-(len:%lf r1[%d]:%d r2[0]:%d w:%d) \n", __FUNCTION__, __LINE__, *speedx, time,
                ptrigInfo[index].difTime[last_count],
                ptrigInfo[index].startTime, len,
                last_count,
                ptrigInfo[index].r[ptrigInfo[index].count - 1],
                ptrigInfo[index].init_r, w);
    } else {

        //上一个点的中心点坐标
        int last_count = ptrigInfo[index].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;

        len = -(double) (ptrigInfo[index].l[last_count] - ptrigInfo[index].init_l) / (double) w;
        time = ptrigInfo[index].difTime[last_count]; //- pGunlinkage_item->rw.m_trig_out[index].difTime[pGunlinkage_item->rw.m_trig_out[index].count - pointCnt + 1];
        if (time <= 0) *speedx = 0.0;
        else *speedx = (double) len / (double) time;
        printf("%s %d,speedx:%f -(time:%d difTime:%d startTime:%f)-(len:%lf l1[%d]:%d l2[0]:%d w:%d) \n", __FUNCTION__, __LINE__, *speedx, time,
                ptrigInfo[index].difTime[last_count],
                ptrigInfo[index].startTime, len,
                last_count,
                ptrigInfo[index].l[ptrigInfo[index].count - 1],
                ptrigInfo[index].init_r, w);
    }

    //当前位置与初始位置相比：
    //上一个点的中心点坐标
    int last_count = ptrigInfo[index].count % POINTCOUNT;
    if (last_count == 0) last_count = POINTCOUNT - 1;
    else last_count -= 1;

    int len1 = (ptrigInfo[index].t[last_count] - ptrigInfo[index].init_t);
    int len2 = (ptrigInfo[index].b[last_count] - ptrigInfo[index].init_b);
    len = ((double) (len1 + len2) / 2) / (double) h;
    time = ptrigInfo[index].difTime[last_count]; // - pGunlinkage_item->rw.m_trig_out[index].difTime[pGunlinkage_item->rw.m_trig_out[index].count - pointCnt + 1];
    if (time <= 0) *speedy = 0.0;
    else *speedy = (double) len / (double) time;

    printf("len1:%d  t[%d]:%d  t[0]:%d \n", len1,
            last_count, ptrigInfo[index].t[last_count], ptrigInfo[index].init_t);

    printf("len2:%d  b[%d]:%d  b[0]:%d \n", len2,
            last_count, ptrigInfo[index].b[last_count], ptrigInfo[index].init_b);

    printf("%s %d,speedy:%f -(time:%d difTime:%d startTime:%d)-(len:%lf len1:%d len2:%d w:%d) \n", __FUNCTION__, __LINE__, *speedy, time,
            ptrigInfo[index].difTime[last_count],
            ptrigInfo[index].startTime, len, len1, len2, w);

    return 0;
}

/*******************************************************************************
                    检查目标列表，确定是否有新目标发送
 * 
 * 检查目标列表：
 * 
 *      如果当前跟踪号大于0，表示有目标联动过：
 *          1. 检查当前目标队列，如果目标队列中，某个目标还在联动中，则不做处理； 如果队列中没有目标在联动中，则将当前跟踪号设置为0
 * 
 *          2. 当当前跟踪号为0，则检测目标队列中是否有缓存目标，如果有，将该目标信息发送给球机，并将该目标的处理状态设置为1
 * 
 *          3. 
 *  
 * 
 ********************************************************************************/
void gunlinkage_checkTrigList(struct Gunlinkage_item *pGunlinkage_item, int w, int h
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //
    int i, j;

    //接收数据, 通过数据判定是否向球机发送信息
    gunlinkage_recv_info(pGunlinkage_item, w, h, pShareBuffer);

    int m_dealed = 0; //是否处理结束   
    if (pGunlinkage_item->rw.curr_trig_num > 0) {

        for (i = 0; i < TRIGGERCOUNT; i++) {
            if (pGunlinkage_item->rw.m_trig_out[i].used == 0)
                continue;

            if (pGunlinkage_item->rw.curr_trig_num == pGunlinkage_item->rw.m_trig_out[i].index) {

                if (pGunlinkage_item->rw.m_trig_out[i].dealed == 1) { //开始处理
                    m_dealed = 1;
                } else if (pGunlinkage_item->rw.m_trig_out[i].dealed == 2) { //结束处理
                    m_dealed = 2;
                } else { //未处理
                    m_dealed = 0;
                }
                break;
            }
        }

        //如果该目标处理开始处理()
        if (m_dealed != 1) {
            pGunlinkage_item->rw.curr_trig_num = 0;
        }
    }



    //目标第一次发送时，产生事件存储路径
    if (pGunlinkage_item->rw.curr_trig_num == 0) {

        for (i = 0; i < TRIGGERCOUNT; i++) {

            if (pGunlinkage_item->rw.m_trig_out[i].used == 0 || pGunlinkage_item->rw.m_trig_out[i].dealed == 1 || pGunlinkage_item->rw.m_trig_out[i].dealed == 2)
                continue;

            //修正套框,并判断套框是否在检测区域内,
            //如果目标长度超过大船的标定长度，直接将：m_export_count 设置为100(防止目标进入画面时，尾部就有长波浪)
            int m_left = 0, m_right = 0, m_top = 0, m_bottom = 0;
            //double speedx, speedy;
            //gunlinkage_correct_objPos(pGunlinkage_item, w, h, i, 0, &m_left, &m_right, &m_top, &m_bottom, &speedx, &speedy);
            //int debug[5] = {0};
            //vibe_handle_obj_info_1(pGunlinkage_item->rw.m_trig_out, i, &m_left, &m_right, &m_top, &m_bottom, debug);

            int last_count = pGunlinkage_item->rw.m_trig_out[i].count % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;
            //大船或小船的长度都没有超过标定的长度：
            m_left = pGunlinkage_item->rw.m_trig_out[i].l[last_count];
            m_right = pGunlinkage_item->rw.m_trig_out[i].r[last_count];
            m_top = pGunlinkage_item->rw.m_trig_out[i].t[last_count];
            m_bottom = pGunlinkage_item->rw.m_trig_out[i].b[last_count];

            if (pGunlinkage_item->rw.m_trig_out[i].m_export_count > EXPORT_CONDITION) {

                int senfflag = 0;
                //左边：左边界大于设定值
                if (pGunlinkage_item->rw.m_trig_out[i].type == 0 || pGunlinkage_item->rw.m_trig_out[i].type == 1) {
                    if (m_left >= pGunlinkage_item->rw.m_vibe_inparam.vibe_output_left) {
                        printf("%s %d  a boat come in the area (left:%d vibe_output_left:%d)\n", __FUNCTION__, __LINE__, m_left, pGunlinkage_item->rw.m_vibe_inparam.vibe_output_left);
                        char message[512];
                        sprintf(message, "%s %d  a boat come in the area (left:%d vibe_output_left:%d)\n", __FUNCTION__, __LINE__, m_left, pGunlinkage_item->rw.m_vibe_inparam.vibe_output_left);
                        MakeLog(message, 0);
                        senfflag = 1;
                    }
                }
                if (pGunlinkage_item->rw.m_trig_out[i].type == 2 || pGunlinkage_item->rw.m_trig_out[i].type == 3) {
                    if (m_right <= pGunlinkage_item->rw.m_vibe_inparam.vibe_output_right) {
                        printf("%s %d  a boat come in the area (m_right:%d vibe_output_right:%d) \n", __FUNCTION__, __LINE__, m_right, pGunlinkage_item->rw.m_vibe_inparam.vibe_output_right);
                        char message[512];
                        sprintf(message, "%s %d  a boat come in the area (m_right:%d vibe_output_right:%d) \n", __FUNCTION__, __LINE__, m_right, pGunlinkage_item->rw.m_vibe_inparam.vibe_output_right);
                        MakeLog(message, 0);
                        senfflag = 1;
                    }
                }

                if (1 == senfflag) {

                    pGunlinkage_item->rw.m_linkage_objNumber++;
                    pGunlinkage_item->rw.m_trig_out[i].index = pGunlinkage_item->rw.m_linkage_objNumber;
                    //发送信息...
                    printf("%s %d : gun send a trigger info(obj_num:%d dealed:%d) w:%d h:%d \n", __FUNCTION__, __LINE__, pGunlinkage_item->rw.m_trig_out[i].index,
                            pGunlinkage_item->rw.m_trig_out[i].dealed, w, h);
                    char message[512];
                    sprintf(message, "%s %d : gun send a trigger info(obj_num:%d dealed:%d) w:%d h:%d \n", __FUNCTION__, __LINE__, pGunlinkage_item->rw.m_trig_out[i].index,
                            pGunlinkage_item->rw.m_trig_out[i].dealed, w, h);
                    MakeLog(message, 0);

                    //在此生成文件存储路径
                    if (pGunlinkage_item->rw.m_linage_videoPath[0] == 0) {

                        time_t ts = (time_t) fs_time_GMT_get();
                        struct tm ti_s;
                        localtime_r(&ts, &ti_s);

                        sprintf(pGunlinkage_item->rw.m_linage_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pGunlinkage_item->ro._pGunlinkage->ro._dir,
                                pGunlinkage_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
                    }

                    if (pGunlinkage_item->rw.m_linkage_switch == 1)
                        gunlinkage_send_obj(pGunlinkage_item, w, h, i, 1);

                    pGunlinkage_item->rw.m_trig_out[i].dealed = 1;
                    pGunlinkage_item->rw.curr_trig_num = pGunlinkage_item->rw.m_trig_out[i].index;

                    break;
                }
            }
        }
    }
}

/*检测目标队列：
 * 
 *  1. 检测目标队列的跟踪标志：trig_error； 如果为1，表示该目标跟踪结束；
 * 
 * 跟踪结束操作：
 * 
 *     1.如果该目标作为事件，则保存该目标视频录像。如果没有被联动过，则不保存该目标视频录像
 * 
 *     2.将该目标清零
 * 
 * 
 */
void gunlinkage_checkTrigger_1(struct Gunlinkage_item *pGunlinkage_item) {
    int j;
    for (j = 0; j < TRIGGERCOUNT; j++) {
        if (pGunlinkage_item->rw.m_trig_out[j].used == 0)
            continue;

        if (pGunlinkage_item->rw.m_trig_out[j].trig_error == 1) {

            printf("%s %d delete the trigInfo[number:%d] ,count %d m_export_count:%d startT:%f endT:%f \n", __FUNCTION__, __LINE__, pGunlinkage_item->rw.m_trig_out[j].index, pGunlinkage_item->rw.m_trig_out[j].count,
                    pGunlinkage_item->rw.m_trig_out[j].m_export_count, pGunlinkage_item->rw.m_trig_out[j].incVideoStartTime, pGunlinkage_item->rw.m_trig_out[j].incVideoEndTime);

            char message[512];
            sprintf(message, "%s %d delete the trigInfo[number:%d] ,count %d m_export_count:%d startT:%f endT:%f \n", __FUNCTION__, __LINE__, pGunlinkage_item->rw.m_trig_out[j].index, pGunlinkage_item->rw.m_trig_out[j].count,
                    pGunlinkage_item->rw.m_trig_out[j].m_export_count, pGunlinkage_item->rw.m_trig_out[j].incVideoStartTime, pGunlinkage_item->rw.m_trig_out[j].incVideoEndTime);
            MakeLog(message, 0);

            //输出目标结束，如果此目标的输出个数大于5。即，此目标很有可能被联动过
            if (pGunlinkage_item->rw.m_trig_out[j].m_export_count > EXPORT_CONDITION) {

                if (pGunlinkage_item->rw.m_linkage_saveVideo == 1) {
                    double startT = 0.0, endT = 0.0;
                    startT = pGunlinkage_item->rw.m_trig_out[j].incVideoStartTime;
                    endT = pGunlinkage_item->rw.m_trig_out[j].incVideoEndTime;

                    pthread_mutex_lock(&pGunlinkage_item->ro.__mutexCmdConnect);
                    if (pGunlinkage_item->p.record_item_add_partVideo) {

                        if (pGunlinkage_item->rw.m_linage_videoPath[0] == 0) {
                            time_t ts = (time_t) fs_time_GMT_get();
                            struct tm ti_s;
                            localtime_r(&ts, &ti_s);

                            sprintf(pGunlinkage_item->rw.m_linage_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pGunlinkage_item->ro._pGunlinkage->ro._dir,
                                    pGunlinkage_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
                        }

                        FsPrintf2(1, ">>>>save video objnumber:%d path:%s time(%f %f %f)\n", pGunlinkage_item->rw.m_trig_out[j].index, pGunlinkage_item->rw.m_linage_videoPath, startT, endT, fs_time_uptime_get());
                        pGunlinkage_item->p.record_item_add_partVideo(pGunlinkage_item->p.pRecord_item, pGunlinkage_item->rw.m_linage_videoPath, startT - 5, endT, NULL);
                        fflush(stdout);

                        char message[512];
                        sprintf(message, ">>>>save video objnumber:%d path:%s time(%f %f %f)\n", pGunlinkage_item->rw.m_trig_out[j].index, pGunlinkage_item->rw.m_linage_videoPath, startT, endT, fs_time_uptime_get());
                        //pGunlinkage_item->p.record_item_add_partVideo(pGunlinkage_item->p.pRecord_item, pGunlinkage_item->rw.m_linage_videoPath, startT - 5, endT, NULL);
                        MakeLog(message, 0);

                    } else {
                        FsPrintf(1, ".record_item_add_partVideo is NULL !!!.....\n");
                        fflush(stdout);
                        char message[512];
                        sprintf(message, ".record_item_add_partVideo is NULL !!!.....\n");
                        MakeLog(message, 0);
                    }
                    pthread_mutex_unlock(&pGunlinkage_item->ro.__mutexCmdConnect);
                }

                pGunlinkage_item->rw.m_linage_videoPath[0] = 0;
            }

            memset(&pGunlinkage_item->rw.m_trig_out[j], 0, sizeof (trigInfo));
        }
    }
}

void gunlinkage_draw_block_gray(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame *pFrame) {
    int blocksize = 32;
    int w = pGunlinkage_item->ro.imageWidth;
    int h = pGunlinkage_item->ro.imageHeight;

    int www = 128;

    unsigned char* p_y = (unsigned char *) pFrame->data.raw[0]->data;
    unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + w * h;
    unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + w * h * 5 / 4;

    int i, j;
    for (j = 0; j < h; j++) {
        for (i = 1; i < w; i++) {
            if (i % 128 == 0 || (i - 1) % 128 == 0 || (i - 2) % 128 == 0) {
                p_y[j * w + i] = 0;
            }
        }
    }

    for (i = 0; i < w; i++) {
        for (j = 1; j < h; j++) {
            if (j % 120 == 0 || (j - 1) % 120 == 0 || (j - 2) % 120 == 0) {
                p_y[j * w + i] = 0;
            }
        }
    }

    if (pGunlinkage_item->ro.__pTypeFace == NULL) {
        pGunlinkage_item->ro.__pTypeFace = fs_TypeFace_new__IO(NULL, 25, 25, 1, 0);
    }
    if (pGunlinkage_item->ro.__pTypeFace) {

        int watermarking_color = 0xFF0000;
        int m_overly_tracknum_color = (((unsigned int) (0.257 * (watermarking_color >> 16) + 0.504 * ((watermarking_color >> 8)&0xFF) + 0.098 * (watermarking_color & 0xFF) + 16)) << 16)+
                (((unsigned int) (-0.148 * (watermarking_color >> 16) - 0.291 * ((watermarking_color >> 8)&0xFF) + 0.439 * (watermarking_color & 0xFF) + 128)) << 8) +
                0.439 * (watermarking_color >> 16) - 0.368 * ((watermarking_color >> 8)&0xFF) - 0.071 * (watermarking_color & 0xFF) + 128;

        int www = 45;
        int hhh = 24;

        int i, j;
        for (j = 0; j < 9; j++) {
            for (i = 0; i < 15; i++) {
                int startx = i * 128 + (128 - 45) / 2;
                int starty = j * 120 + (120 - 24) / 2;
                char overlydata1[24];
                sprintf(overlydata1, "%03d", pGunlinkage_item->rw.m_vibeparam.blockgray[j * 15 + i]);

                fs_TypeFace_write(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_y, w, h,
                        w, startx, starty, m_overly_tracknum_color >> 16);
                fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_u, w, h,
                        w >> 1, startx, starty, (m_overly_tracknum_color >> 8)&0xFF);
                fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_v, w, h,
                        w >> 1, startx, starty, m_overly_tracknum_color & 0xFF);
            }
        }
    }
}

void gunlinkage_draw_trackint_info(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame * pFrame, int shinningpoint, int gray, int moveth) {

    if (pGunlinkage_item->ro.__pTypeFace == NULL) {
        pGunlinkage_item->ro.__pTypeFace = fs_TypeFace_new__IO(NULL, 25, 25, 1, 0);
    }
    if (pGunlinkage_item->ro.__pTypeFace) {

        char overlydata[8][256];
        if (1) {

            int width = pFrame->width[0];
            int height = pFrame->height[0];

            unsigned char* p_y = (unsigned char *) pFrame->data.raw[0]->data;
            unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width*height;
            unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;

            int watermarking_color = 0xFF0000;
            int m_overly_tracknum_color = (((unsigned int) (0.257 * (watermarking_color >> 16) + 0.504 * ((watermarking_color >> 8)&0xFF) + 0.098 * (watermarking_color & 0xFF) + 16)) << 16)+
                    (((unsigned int) (-0.148 * (watermarking_color >> 16) - 0.291 * ((watermarking_color >> 8)&0xFF) + 0.439 * (watermarking_color & 0xFF) + 128)) << 8) +
                    0.439 * (watermarking_color >> 16) - 0.368 * ((watermarking_color >> 8)&0xFF) - 0.071 * (watermarking_color & 0xFF) + 128;


            int objcountL = 0;
            int objcountR = 0;
            int objcountT = 0;
            int j;
            for (j = 0; j < BOATCOUNT; j++) {
                if (pGunlinkage_item->rw.m_vibeparam.m_Lobjinfo[j].used == 0)
                    continue;
                objcountL++;
            }

            for (j = 0; j < BOATCOUNT; j++) {
                if (pGunlinkage_item->rw.m_vibeparam.m_Robjinfo[j].used == 0)
                    continue;
                objcountR++;
            }

            for (j = 0; j < TRIGGERCOUNT; j++) {
                if (pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].used == 1) {
                    objcountT++;
                }
            }

            int startx, starty;
            startx = 10;
            starty = 650;

            char overlydata1[612];
            sprintf(overlydata1, "gray:%03d  shinningP:%d moveth:%d objcountL:%d objcountR:%d objcountT:%d", gray, shinningpoint, moveth, objcountL, objcountR, objcountT);

            fs_TypeFace_write(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_y, width, height,
                    width, startx, starty, m_overly_tracknum_color >> 16);
            fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_u, width, height,
                    width >> 1, startx, starty, (m_overly_tracknum_color >> 8)&0xFF);
            fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_v, width, height,
                    width >> 1, startx, starty, m_overly_tracknum_color & 0xFF);


            startx = 10;
            starty = 750;

            int i = 0;
            for (i = 0; i < TRIGGERCOUNT; i++) {

                if (pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].used == 0)
                    continue;

                startx = 10;
                starty += 50;

                int last_count = pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].count % POINTCOUNT;
                if (last_count == 0) last_count = POINTCOUNT - 1;
                else last_count -= 1;

                int last_count1 = (pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].count - 1) % POINTCOUNT;
                if (last_count1 == 0) last_count1 = POINTCOUNT - 1;
                else last_count1 -= 1;

                /*
                sprintf(overlydata1, "index:%d type:%d dealed:%03d unsafe:%d last(%03d %03d %03d %03d) curr(%03d %03d %03d %03d) changed(%03d %03d %03d %03d) pcnt:%04d  exp_cnt:%d prelen:%lf",
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].index, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].type,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].dealed, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].unsafe, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].l[last_count1],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].r[last_count1], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].t[last_count1],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].b[last_count1], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].l[last_count],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].r[last_count], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].t[last_count],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].b[last_count], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_l,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_r, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_t,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_b, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].count,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].m_export_count, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepushLen);
                 */
                /* 字太长
                sprintf(overlydata1, "index:%d type:%d dealed:%03d unsafe:%d leave:%d(%d)-%d(%d) prepush(%03d %03d %03d %03d) curr(%03d %03d %03d %03d) changed(%03d %03d %03d %03d) prespeed:%f prelen:%f",
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].index, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].type,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].dealed, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].unsafe,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_out_left, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_back_left,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_out_left, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_back_right,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_l,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_r, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_t,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_b, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].l[last_count],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].r[last_count], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].t[last_count],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].b[last_count], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_l,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_r, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_t,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_b, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].count,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].m_export_count, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_speed,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepushLen);
                 */

                sprintf(overlydata1, "index:%d type:%d dealed:%03d unsafe:%d leave:%d(%d)-%d(%d) prepush(%03d %03d %03d %03d) curr(%03d %03d %03d %03d) changed(%03d %03d %03d %03d)",
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].index, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].type,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].dealed, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].unsafe,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_out_left, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_back_left,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_out_left, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].flag_go_back_right,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_l,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_r, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_t,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_b, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].l[last_count],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].r[last_count], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].t[last_count],
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].b[last_count], pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_l,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_r, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_t,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].changed_b, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].count,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].m_export_count, pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepush_speed,
                        pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].prepushLen);

                fs_TypeFace_write(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_y, width, height,
                        width, startx, starty, m_overly_tracknum_color >> 16);
                fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_u, width, height,
                        width >> 1, startx, starty, (m_overly_tracknum_color >> 8)&0xFF);
                fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_v, width, height,
                        width >> 1, startx, starty, m_overly_tracknum_color & 0xFF);

                int k = 0;
                for (k = 0; k < 0; k++) {
                    if (pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].related_index[k] > 0) {
                        startx = 20;
                        starty += 50;

                        sprintf(overlydata1, "related_index:%d", pGunlinkage_item->rw.m_vibeparam.m_triginfo[i].related_index[k] - 1);

                        fs_TypeFace_write(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_y, width, height,
                                width, startx, starty, m_overly_tracknum_color >> 16);
                        fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_u, width, height,
                                width >> 1, startx, starty, (m_overly_tracknum_color >> 8)&0xFF);
                        fs_TypeFace_write_uv(pGunlinkage_item->ro.__pTypeFace, (unsigned char*) overlydata1, p_v, width, height,
                                width >> 1, startx, starty, m_overly_tracknum_color & 0xFF);
                    }
                }

            }
        }
    }
}

/*******************************************************************************
 * 左右两侧线圈触发目标框：绿色
 * 
 * 目标跟踪框：红色
 * 
 * 目标跟踪框修正：绿色
 * 
 * 目标相遇时，预推框：蓝色
 ********************************************************************************/
void gunlinkage_draw_tracking_rect(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame * pFrame) {

    int j;
    for (j = 0; j < BOATCOUNT; j++) {
        if (pGunlinkage_item->rw.m_vibeparam.m_Lobjinfo[j].used == 0)
            continue;

        ITG_RECT myrect;
        myrect.left = pGunlinkage_item->rw.m_vibeparam.m_Lobjinfo[j].left * 4;
        myrect.right = pGunlinkage_item->rw.m_vibeparam.m_Lobjinfo[j].right * 4;
        myrect.top = pGunlinkage_item->rw.m_vibeparam.m_Lobjinfo[j].top * 4;
        myrect.bottom = pGunlinkage_item->rw.m_vibeparam.m_Lobjinfo[j].bottom * 4;

        //green绿色
        unsigned char Y, Cb, Cr;
        Y = 0.504 * 255 + 16;
        Cb = -0.291 * 255 + 128;
        Cr = -0.368 * 255 + 128;

        int width = pGunlinkage_item->ro.imageWidth;
        int height = pGunlinkage_item->ro.imageHeight;

        if (myrect.left > myrect.right) myrect.right = myrect.left + 1;
        if (myrect.top > myrect.bottom)myrect.bottom = myrect.top + 1;

        if (myrect.left < 0) myrect.left = 0;
        if (myrect.left > width - 5) myrect.left = width - 5;
        if (myrect.top < 10) myrect.top = 10;
        if (myrect.top > height - 5) myrect.top = height - 5;
        if (myrect.right < 0) myrect.right = 0;
        if (myrect.right > width - 5) myrect.right = width - 5;
        if (myrect.bottom < 0) myrect.bottom = 0;
        if (myrect.bottom > height - 5) myrect.bottom = height - 5;


        int rate = 50;
        int LineWidth = 3;

        unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width * height;
        unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;



        ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, &myrect, Y, Cb, Cr, rate, LineWidth);
        ITG_DrawRect_YUV((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, height, &myrect, Y, Cb, Cr, rate, LineWidth);
    }


    for (j = 0; j < BOATCOUNT; j++) {
        if (pGunlinkage_item->rw.m_vibeparam.m_Robjinfo[j].used == 0)
            continue;

        ITG_RECT myrect;
        myrect.left = pGunlinkage_item->rw.m_vibeparam.m_Robjinfo[j].left * 4;
        myrect.right = pGunlinkage_item->rw.m_vibeparam.m_Robjinfo[j].right * 4;
        myrect.top = pGunlinkage_item->rw.m_vibeparam.m_Robjinfo[j].top * 4;
        myrect.bottom = pGunlinkage_item->rw.m_vibeparam.m_Robjinfo[j].bottom * 4;

        //green绿色
        unsigned char Y, Cb, Cr;
        Y = 0.504 * 255 + 16;
        Cb = -0.291 * 255 + 128;
        Cr = -0.368 * 255 + 128;

        int width = pGunlinkage_item->ro.imageWidth;
        int height = pGunlinkage_item->ro.imageHeight;

        if (myrect.left > myrect.right) myrect.right = myrect.left + 1;
        if (myrect.top > myrect.bottom)myrect.bottom = myrect.top + 1;

        if (myrect.left < 0) myrect.left = 0;
        if (myrect.left > width - 5) myrect.left = width - 5;
        if (myrect.top < 10) myrect.top = 10;
        if (myrect.top > height - 5) myrect.top = height - 5;
        if (myrect.right < 0) myrect.right = 0;
        if (myrect.right > width - 5) myrect.right = width - 5;
        if (myrect.bottom < 0) myrect.bottom = 0;
        if (myrect.bottom > height - 5) myrect.bottom = height - 5;

        int rate = 50;
        int LineWidth = 3;

        unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width * height;
        unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;
        ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, &myrect, Y, Cb, Cr, rate, LineWidth);
        ITG_DrawRect_YUV((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, height, &myrect, Y, Cb, Cr, rate, LineWidth);
    }



    for (j = 0; j < TRIGGERCOUNT; j++) {
        if (pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].used == 0)
            continue;

        ITG_RECT myrect;

        int last_count = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].count % POINTCOUNT;
        if (last_count == 0) last_count = POINTCOUNT - 1;
        else last_count -= 1;

        myrect.left = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].l[last_count]*4;
        myrect.right = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].r[last_count]*4;

        myrect.top = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].t[last_count]*4;
        myrect.bottom = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].b[last_count]*4;

        unsigned char Y, Cb, Cr;

        //red红色
        Y = 0.257 * 255 + 16;
        Cb = -0.148 * 255 + 128;
        Cr = 0.439 * 255 + 128;


        int width = pGunlinkage_item->ro.imageWidth;
        int height = pGunlinkage_item->ro.imageHeight;

        if (myrect.left > myrect.right) myrect.right = myrect.left + 1;
        if (myrect.top > myrect.bottom) myrect.bottom = myrect.top + 1;

        if (myrect.left < 0) myrect.left = 0;
        if (myrect.left > width - 5) myrect.left = width - 5;
        if (myrect.top < 10) myrect.top = 10;
        if (myrect.top > height - 5) myrect.top = height - 5;
        if (myrect.right < 0) myrect.right = 0;
        if (myrect.right > width - 5) myrect.right = width - 5;
        if (myrect.bottom < 0) myrect.bottom = 0;
        if (myrect.bottom > height - 5) myrect.bottom = height - 5;

        int rate = 50;
        int LineWidth = 3;

        unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width * height;
        unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;
        ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, &myrect, Y, Cb, Cr, rate, LineWidth);
        ITG_DrawRect_YUV((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, height, &myrect, Y, Cb, Cr, rate, LineWidth);

        //绘制修正坐标
        //green绿色
        Y = 0.504 * 255 + 16;
        Cb = -0.291 * 255 + 128;
        Cr = -0.368 * 255 + 128;

        myrect.left = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].changed_l * 4;
        myrect.right = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].changed_r * 4;

        myrect.top = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].changed_t * 4;
        myrect.bottom = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].changed_b * 4;

        if (myrect.left > myrect.right) myrect.right = myrect.left + 1;
        if (myrect.top > myrect.bottom)myrect.bottom = myrect.top + 1;

        if (myrect.left < 0) myrect.left = 0;
        if (myrect.left > width - 5) myrect.left = width - 5;
        if (myrect.top < 10) myrect.top = 10;
        if (myrect.top > height - 5) myrect.top = height - 5;
        if (myrect.right < 0) myrect.right = 0;
        if (myrect.right > width - 5) myrect.right = width - 5;
        if (myrect.bottom < 0) myrect.bottom = 0;
        if (myrect.bottom > height - 5) myrect.bottom = height - 5;

        ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, &myrect, Y, Cb, Cr, rate, LineWidth);
        ITG_DrawRect_YUV((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, height, &myrect, Y, Cb, Cr, rate, LineWidth);

        if (1 || pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].unsafe == 1) {
            //绘制预推坐标

            //blue蓝色
            Y = 0.098 * 255 + 16;
            Cb = 0.439 * 255 + 128;
            Cr = -0.071 * 255 + 128;

            myrect.left = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].prepush_l * 4;
            myrect.right = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].prepush_r * 4;

            myrect.top = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].prepush_t * 4;
            myrect.bottom = pGunlinkage_item->rw.m_vibeparam.m_triginfo[j].prepush_b * 4;

            if (myrect.left > myrect.right) myrect.right = myrect.left + 1;
            if (myrect.top > myrect.bottom)myrect.bottom = myrect.top + 1;

            if (myrect.left < 0) myrect.left = 0;
            if (myrect.left > width - 5) myrect.left = width - 5;
            if (myrect.top < 10) myrect.top = 10;
            if (myrect.top > height - 5) myrect.top = height - 5;
            if (myrect.right < 0) myrect.right = 0;
            if (myrect.right > width - 5) myrect.right = width - 5;
            if (myrect.bottom < 0) myrect.bottom = 0;
            if (myrect.bottom > height - 5) myrect.bottom = height - 5;

            ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, &myrect, Y, Cb, Cr, rate, LineWidth);
            ITG_DrawRect_YUV((char *) pFrame->data.raw[0]->data, (char *) p_u, (char *) p_v, width, height, &myrect, Y, Cb, Cr, rate, LineWidth);
        }
    }
}

void gunlinkage_provibe(struct Gunlinkage_item *pGunlinkage_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int frameindex) {

    if (pGunlinkage_item->rw.m_vibeparam.m_bInitOK == 0) {

        int model_index = 0;
        double currT = fs_time_uptime_get();
        vibe_Init_linkage(&pGunlinkage_item->rw.m_vibeparam, pSrcImage, currT, sw, sh, pGunlinkage_item->rw.m_vibe_inparam, model_index);

        memset(pGunlinkage_item->rw.m_trig_out, 0, sizeof (trigInfo) * TRIGGERCOUNT);
    }

    int moveth = 10;
    int headth = 15;
    int tailth = 15;
    int gray = 0;
    int debug[30] = {0};
    int s_point_count = 0;

    gunlinkage_checkTrigger_1(pGunlinkage_item);
    pGunlinkage_item->rw.m_vibeparam.dealtime = pFrame->capture_uptime;
    double currT = fs_time_uptime_get();
    int re = vibe_Proc_linkage(&pGunlinkage_item->rw.m_vibeparam, pSrcImage, currT, frameindex, sw, sh, moveth, headth, tailth, &gray, &s_point_count, pGunlinkage_item->rw.m_trig_out, debug);
    if (re == -1) {
        FsPrintf2(1, "(%s:%d) vibe is error... \n", __FUNCTION__, __LINE__);
        fflush(stdout);
        char message[512];
        sprintf(message, "(%s:%d) vibe is error... \n", __FUNCTION__, __LINE__);
        MakeLog(message, 0);

        vibe_Del(&pGunlinkage_item->rw.m_vibeparam);
    }

    if (1) {


        if (pGunlinkage_item->ro.m_debug_switch > 0) {
            //绘制目标跟踪框
            gunlinkage_draw_tracking_rect(pGunlinkage_item, pFrame);

            //叠加小图
            vPasteY_1((unsigned char *) pFrame->data.raw[0]->data, pGunlinkage_item->rw.m_vibeparam.segmentation_Image_4, sw * 4, sh * 4, sw, 0, sh, 0, 50);
            vPasteY_1((unsigned char *) pFrame->data.raw[0]->data, pGunlinkage_item->rw.m_vibeparam.segmentation_Image_2, sw * 4, sh * 4, sw, 0, sh, 5 + sw, 50);
            vPasteY_1((unsigned char *) pFrame->data.raw[0]->data, pGunlinkage_item->rw.m_vibeparam.segmentation_Image_3, sw * 4, sh * 4, sw, 0, sh, 5 + sw * 2, 50);
        }

        if (pGunlinkage_item->ro.m_debug_switch == 1) {
            //叠加目标输出坐标信息
            gunlinkage_draw_trackint_info(pGunlinkage_item, pFrame, s_point_count, gray, pGunlinkage_item->rw.m_vibeparam.moveth);
        }

        if (pGunlinkage_item->ro.m_debug_switch == 2) {
            //绘制画面平均灰度值
            gunlinkage_draw_block_gray(pGunlinkage_item, pFrame);
        }

    }
}

/* 通道分层多线程写入法 */
static void *gunlinkage_P_T(struct Gunlinkage * const pGunlinkage) {
#undef FsFunctionName
#define FsFunctionName gunlinkage_P_T
    //FsPrintf(1, "Thread Index=%hu.\n", pRecord->p.threadCount);

    FsPrintf(2, "vlinkage_private_thread begin**** ....\n");
    fflush(stdout);
    ////////////////////////////////////////////////////////////////////////////
    /* 创建线程监控 */
    void *pMonitor = pGunlinkage->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, pGunlinkage->ro._threadName, NULL, pGunlinkage, pGunlinkage->ro._pMonitor ? pGunlinkage->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pGunlinkage->ro._externRunStatus;
    //RtspServer *pRtspServer = pVtrack->ro._pRtspServer;
    ConfigManager * const pConfigManager = (ConfigManager*) pGunlinkage->ro._pConfigManager;
    FsObjectList * const itemList_ = pGunlinkage->ro.__itemList_;
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
#define __gunlinkage_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
#define __gunlinkage_P_T_clean2_count 0
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __gunlinkage_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    //FsObjectBaseBuffer baseBuffer;
    //fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __gunlinkage_P_T_clean1_3 //fs_ObjectBaseBuffer_release(&baseBuffer);  
    ////////////////////////////////////////////////////////////////////////////
    // 通道数
    unsigned int channelCount = 0;

    double keepTime = 30.0;
    unsigned int recvListMaxCount = 10;
    /* 开方分组 */
    FsGroupSqrt groupSqrt;
    Fs_GroupSqrt_init(&groupSqrt);
#define __gunlinkage_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __gunlinkage_P_T_clean1_count 4
    pGunlinkage->p.pSocketPool = socketPool_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ pGunlinkage->ro._pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ systemThreadTid
            , "SocketPool"
            , /* 接收队列的最大容量 */recvListMaxCount
            , /* 最长没有发送和接收数据的时间 */keepTime);
    socketPool_startThread(pGunlinkage->p.pSocketPool, 0);

    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    FsObjectImageFrame *pFrame;
    unsigned int sleepTime = 0;

    ////////////////////////////////////////////////////////////////////////////
    int sw = 0; //算法处理的图像尺寸
    int sh = 0; //
    int currFrameIndex = 0; //当前图像帧号
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pGunlinkage->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        double tt0 = fs_time_uptime_get();
        usleep(1000);

        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pGunlinkage->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */

            if (0 == pGunlinkage->rw._resetChannel) channelCount = gunlinkage_private_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server Gunlinkage
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
                struct Gunlinkage_item * const pGunlinkage_item = (struct Gunlinkage_item *) fs_ObjectList_remove_tail(itemList_);
                if (pGunlinkage_item)gunlinkage_private_item_delete__OI(pGunlinkage_item);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);

            printf("*****************resetChannel:%d***************\n", pGunlinkage->rw._resetChannel);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pGunlinkage->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pGunlinkage->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pGunlinkage->rw._resetChannel > 0) {
                FsPrintf(1, "%d\n", pGunlinkage->rw._resetChannel);

                pMonitorItem->rw.line = __LINE__;
                gunlinkage_private_item_new(pGunlinkage, pGunlinkage->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Gunlinkage_item **) (itemList_->pNode + pGunlinkage->rw._resetChannel - 1), &groupSqrt);

                pMonitorItem->rw.line = __LINE__;
            }
            pGunlinkage->rw._resetChannel = -1;
        }
        double tt1 = fs_time_uptime_get();
        ////////////////////////////////////////////////////////////////////////pRecord_itemNode = (struct Record_item **) itemList->pNode + itemList->startIndex;
        double lasttime = fs_time_uptime_get();
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct Gunlinkage_item * * const ppNode_itemList = (struct Gunlinkage_item **) itemList_->pNode + itemList_->startIndex;
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
                struct Gunlinkage_item * const pGunlinkage_item = ppNode_itemList[ui];
                if (pGunlinkage_item == NULL)continue;
                if (0 == (pGunlinkage_item->ro.moduleMask & 0x8)) {
                    if (pGunlinkage_item->ro.__framelistIn->nodeCount > 0) {
                        pthread_mutex_lock(&pGunlinkage_item->ro.__framelistIn->mutex);
                        fs_ObjectList_insert_tail_all_clean_pthreadSafety(pGunlinkage_item->ro.__framelistOut, pGunlinkage_item->ro.__framelistIn);
                        pthread_mutex_unlock(&pGunlinkage_item->ro.__framelistIn->mutex);
                        Fs_GroupSqrtOut_value_set(pGunlinkage_item->rw._pGroupSqrtOut, &pGunlinkage_item->ro._pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                    }
                    continue;
                }
                //相机类型为枪机，则进行联动数据检测
                if (pGunlinkage_item->ro.m_config_devinfo.m_camera_model == 2 || pGunlinkage_item->ro.m_config_devinfo.m_camera_model == 3) {

                    /*球机是否人为移动检测。相机人为移动后，清除相关参数*/
                    gunlinkage_checkCameraMoved(pGunlinkage_item);

                    /*枪机检测是否在有效时间内*/
                    gunlinkage_checkTimeValid(pGunlinkage_item);

                    if (pGunlinkage_item->ro.imageWidth > 0 && pGunlinkage_item->ro.imageHeight > 0) {
                        if (pGunlinkage_item->ro.zoomTimes > 0) {
                            int nnn = (int) sqrt(pGunlinkage_item->ro.zoomTimes * 1.0);
                            gunlinkage_checkTrigList(pGunlinkage_item, pGunlinkage_item->ro.imageWidth / nnn, pGunlinkage_item->ro.imageHeight / nnn, &shareBuffer);
                        } else {
                            FsPrintf(1, "zoomTimes set error :%d \n", pGunlinkage_item->ro.zoomTimes);
                        }
                    }
                }

                //进入图像数据链
                if (pGunlinkage_item->ro.__framelistIn->nodeCount > 1) {
                    do {
                        double tt2 = fs_time_uptime_get();
                        //获取图像数据帧
                        pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pGunlinkage_item->ro.__framelistIn);

                        //如果相机类型不是枪机，则直接出去
                        if (pGunlinkage_item->ro.m_config_devinfo.m_camera_model != 2 && pGunlinkage_item->ro.m_config_devinfo.m_camera_model != 3) {
                            fs_ObjectList_insert_tail_pthreadSafety(pGunlinkage_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pGunlinkage_item->rw._pGroupSqrtOut, &pGunlinkage_item->ro._pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                            continue;
                        }

                        //获取图像尺寸
                        pGunlinkage_item->ro.imageWidth = pFrame->width[0];
                        pGunlinkage_item->ro.imageHeight = pFrame->height[0];

                        //获取帧号
                        currFrameIndex = pFrame->index;

                        //调试打印信息
                        if (currFrameIndex % 600 == 0) {
                            printf("---vlinkage 20171030/1-%s-framedx:%d-imageSize(%d %d)-camMoved:%d-timevalid:%d-\n", pGunlinkage_item->ro._uuid, currFrameIndex,
                                    pGunlinkage_item->ro.imageWidth, pGunlinkage_item->ro.imageHeight, pGunlinkage_item->rw.m_linkage_camMoved_flag,
                                    pGunlinkage_item->rw.m_linkage_timevalid_flag);

                            char message[512];
                            sprintf(message, "---vlinkage 20171030/1-%s-framedx:%d-imageSize(%d %d)-camMoved:%d-timevalid:%d-\n", pGunlinkage_item->ro._uuid, currFrameIndex,
                                    pGunlinkage_item->ro.imageWidth, pGunlinkage_item->ro.imageHeight, pGunlinkage_item->rw.m_linkage_camMoved_flag,
                                    pGunlinkage_item->rw.m_linkage_timevalid_flag);
                            MakeLog(message, 0);
                            fflush(stdout);
                        }

                        //获取一张算法处理图片，用来绘制枪机检测区域
                        gunlinage_get_maskImage(pGunlinkage_item, pFrame, currFrameIndex, sw, sh);

                        //相机移动，或者不在检测有效时间内，不做算法处理
                        if (pGunlinkage_item->rw.m_linkage_camMoved_flag == 1 || pGunlinkage_item->rw.m_linkage_timevalid_flag == 0) {
                            fs_ObjectList_insert_tail_pthreadSafety(pGunlinkage_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pGunlinkage_item->rw._pGroupSqrtOut, &pGunlinkage_item->ro._pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                            continue;
                        }

                        //减帧处理(支持1/2原帧率处理和1/3原帧率处理，配置默认1/2) 
                        if (pGunlinkage_item->ro.m_linkageframeInterval == 2) {
                            if (currFrameIndex % 2 == 0) {
                                if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                                    image_frame_clear_data(pFrame, ImageFrame_H264_0); //需要重新编码
                                fs_ObjectList_insert_tail_pthreadSafety(pGunlinkage_item->ro.__framelistOut, pFrame);
                                Fs_GroupSqrtOut_value_set(pGunlinkage_item->rw._pGroupSqrtOut, &pGunlinkage_item->ro._pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                                continue;
                            }
                            currFrameIndex = currFrameIndex / 2;

                        } else if (pGunlinkage_item->ro.m_linkageframeInterval == 3) {
                            if (currFrameIndex % 3 == 0) {
                                if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                                    image_frame_clear_data(pFrame, ImageFrame_H264_0); //需要重新编码
                                fs_ObjectList_insert_tail_pthreadSafety(pGunlinkage_item->ro.__framelistOut, pFrame);
                                Fs_GroupSqrtOut_value_set(pGunlinkage_item->rw._pGroupSqrtOut, &pGunlinkage_item->ro._pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                                continue;
                            }
                        }
                        {
                            //准备图像数据(根据配置参数zoomTimes：16，4，1获取对应的小图数据)
                            FsObjectImageYUV420P * pYUV420P;
                            if (pGunlinkage_item->ro.zoomTimes == 16 || pGunlinkage_item->ro.zoomTimes == 0) {
                                pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RAW_2);
                                //获取处理小图的尺寸
                                sw = pFrame->width[2];
                                sh = pFrame->height[2];
                            } else if (pGunlinkage_item->ro.zoomTimes == 4) {
                                pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RAW_1);
                                //获取处理小图的尺寸
                                sw = pFrame->width[1];
                                sh = pFrame->height[1];
                            } else {
                                //图像指针指向大图数据
                                pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RAW_0);
                                //获取处理大图的尺寸
                                sw = pFrame->width[0];
                                sh = pFrame->height[0];
                            }

                            //生成模拟数据
                            gunlinkage_make_debug_data(pGunlinkage_item, pFrame, currFrameIndex, sw, sh);
                            //保存调试
                            gunlinkage_save_debug_image(pGunlinkage_item, pYUV420P->data, currFrameIndex, sw, sh);

                            //数据分析
                            double t1 = fs_time_uptime_get();
                            gunlinkage_provibe(pGunlinkage_item, pFrame, pYUV420P->data, sw, sh, currFrameIndex);
                            double t2 = fs_time_uptime_get();

                            fs_Object_delete_pthreadSafety__OI(pYUV420P);
                        }

                        //数据传递...
                        if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                            image_frame_clear_data(pFrame, ImageFrame_H264_0);
                        fs_ObjectList_insert_tail_pthreadSafety(pGunlinkage_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pGunlinkage_item->rw._pGroupSqrtOut, &pGunlinkage_item->ro._pGunlinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                    } while (pGunlinkage_item->ro.__framelistIn->nodeCount > 1);
                }
            }
        }
    }

    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    if (pGunlinkage->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pGunlinkage->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    /* 资源是多线程共享的,由第一个线程控制 */
    /* 删除各通道 */
    fs_ObjectList_clear_custom(pGunlinkage->ro.__itemList_, (void (*)(void*))gunlinkage_private_item_delete__OI);

    socketPool_stopThread(pGunlinkage->p.pSocketPool);
    socketPool_delete__OI(pGunlinkage->p.pSocketPool);

    /* 释放空间 */
    /* 释放全局变量空间 */
    /* 释放中间变量空间 */

    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd,threadCount=%hu.\n", pGunlinkage->ro._threadName, pGunlinkage->rw.runStatus, pGunlinkage->p.threadCount);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pGunlinkage->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}

void gunlinkage_vibe_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "GunLinkageConfig", "联动枪机检测参数", "联动枪机检测参数", 0, 0x7);

    //group 和 group1 是或者的关系，group内部是与的关系，也就是说，当条件 (moduleMask|&8) 和 (cameraModel == 2) 同时成立，或者(moduleMask|&8) 和 (cameraModel == 3) 同时成立
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "2");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "3");
    }
    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "trackFrameInterval", "算法处理帧率", "算法处理帧率(2：原帧率1/2，3:原帧率1/3,其他值：原帧率)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2", "2", "2");

    void *pNode2 = fs_Config_node_integer_add(pConfig, parent, "zoomTimes", "算法处理图像缩小倍数", "算法处理图像缩小倍数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_default, 16, "缩小16倍", "缩小16倍");
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 4, "缩小4倍", "缩小4倍");
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 1, "原图尺寸", "原图尺寸");

    void *pNode3 = fs_Config_node_integer_add(pConfig, parent, "linkage_saveVideo", "是否保存通过录像", "是否保存通过录像(选1表示保存,选0表示不保存)", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode3, FsConfig_nodeValue_default, 0, "不保存", "不保存");
    fs_Config_node_integer_add_value(pConfig, pNode3, FsConfig_nodeValue_optional, 1, "保存", "保存");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_server_ip", "联动相机所在服务器IP地址", "服务器IP地址", 0, 0x7, 1, 64, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "192.168.20.223", "192.168.20.223", "192.168.20.223");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_server_uuid", "联动相机的通道标识", "联动相机的通道标识(唯一性)", 0, 0x7, 1, 64, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "866", "866", "866");

    void *pNode1 = fs_Config_node_integer_add(pConfig, parent, "linkage_switch", "是否开启球机联动", "是否开启球机联动,选1表示开启联动,选0表示不开启联动", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, 1, "开启联动", "开启联动");
    fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0, "关闭联动", "关闭联动");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_time_valid", "枪机检测有效时间", "枪机检测有效时间(24小时制，开始时间-结束时间，小数点表示分钟)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "8.30-18.30", "8.30-18.30", "8.30-18.30");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_dome_to_gun", "是否球机做枪机使用", "是否球机做枪机使用(是：写入预置位；否：默认值)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.0-0.0-0.0", "0.0-0.0-0.0", "0.0-0.0-0.0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_moved_over_time", "球机人为移动后,超时回到检测位", "球机人为移动后,超过设置时间，自动回到预置位(单位：秒)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "300", "300", "300");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_m_objw", "目标过滤宽度", "目标过滤宽度", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_m_objh", "目标过滤高度", "目标过滤高度", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_m_moveth_1", "moveth阈值(水域)", "moveth阈值(水域)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "19", "19", "19");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_m_moveth_2", "moveth阈值(河岸)", "moveth阈值(河岸)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "25", "25", "25");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_obj_minh", "obj_minh阈值", "obj_minh阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "10", "10", "10");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_loop_length", "loop_length阈值", "loop_length阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "40", "40", "40");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_topth", "topth阈值", "topth阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "10", "10", "10");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_lowth", "lowth阈值", "lowth阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_loop_startL", "loop_startL阈值", "loop_startL阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_loop_startR", "loop_startR阈值", "loop_startR阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "465", "465", "465");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_loop_staytime", "目标占有线圈最短时间(单位：秒)", "目标占有线圈最短时间(单位：秒)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "10", "10", "10");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_expand_times", "运动空心填充次数", "运动空心填充次数", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "8", "8", "8");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_search_area", "检测范围", "检测范围(l-r-t-b)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2-478-12-258", "2-478-12-258", "2-478-12-258");
    //    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_output_area1", "目标联动左右区域限制", "目标联动左右区域限制(l-r，目标进入该区域才会联动)", 0, 0x7, 1, 512, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2-470", "2-470", "2-470");
    //    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_output_area2", "目标联动区域限制", "目标联动区域限制(l-r-t-b，目标进入该区域才会联动)", 0, 0x7, 1, 512, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2-478-12-265", "2-478-12-265", "2-478-12-265");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_output", "目标联动区域限制", "目标联动区域限制(l-r-t-b，目标进入该区域才会联动)", 0, 0x7, 1, 512, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2-470-10-260", "2-470-10-260", "2-470-10-260");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_area_jwd", "触发的区域", "用经纬度表示,如(10,10)(20,20) 每个点用括号括起来,点的个数不定", 0, 0x7, 1, 512, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "(10,10)(20,20)", "(10,10)(20,20)", "(10,10)(20,20)");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_debug_switch", "打开调试模式", "打开调试模式(0:关闭所有调试，>1开启不同的调试模式)", 0, 0x7, 1, 512, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
}

void gunlinkage_getConfig(struct Gunlinkage_item *rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {
    const void * GunLinkageConfig0 = vsysChannel0;
    const void *const GunLinkageConfig = fs_Config_node_get_first(pConfig, &GunLinkageConfig0, vsysChannel, "GunLinkageConfig");


    //处理抽帧
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "trackFrameInterval", NULL);
        rst->ro.m_linkageframeInterval = atoi(pstr);
    }
    rst->rw.m_linkage_saveVideo = fs_Config_node_integer_get_mask(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_saveVideo", NULL);

    //处理抽点
    rst->ro.zoomTimes = fs_Config_node_integer_get_mask(pConfig, GunLinkageConfig0, GunLinkageConfig, "zoomTimes", NULL);

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_server_ip", NULL);
        sprintf(rst->ro.m_config_devinfo.m_linkage_server_ip, "%s", pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_server_uuid", NULL);
        rst->ro.m_config_devinfo.m_linkage_server_uuid = atoi(pstr);
    }

    rst->rw.m_linkage_switch = fs_Config_node_integer_get_mask(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_switch", NULL);

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_m_objw", NULL);
        rst->rw.m_vibe_inparam.modw = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_m_objh", NULL);
        rst->rw.m_vibe_inparam.modh = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_m_moveth_1", NULL);
        rst->rw.m_vibe_inparam.cplex_moveth[0] = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_m_moveth_2", NULL);
        rst->rw.m_vibe_inparam.cplex_moveth[1] = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_m_moveth_2", NULL);
        rst->rw.m_vibe_inparam.moveth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_obj_minh", NULL);
        rst->rw.m_vibe_inparam.obj_min_h = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_loop_length", NULL);
        rst->rw.m_vibe_inparam.loop_length = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_topth", NULL);
        rst->rw.m_vibe_inparam.topth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_lowth", NULL);
        rst->rw.m_vibe_inparam.lowth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_loop_startL", NULL);
        rst->rw.m_vibe_inparam.loop_start_left = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_loop_staytime", NULL);
        rst->rw.m_vibe_inparam.loop_stay_time = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_expand_times", NULL);
        rst->rw.m_vibe_inparam.vibe_expand_times = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_loop_startR", NULL);
        rst->rw.m_vibe_inparam.loop_start_right = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_moved_over_time", NULL);
        rst->ro.m_config_devinfo.m_camera_moved_overTime = atoi(pstr);
    }
    char message[256];
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_search_area", NULL);
        sprintf(message, "%s", pstr);
        sscanf(message, "%d-%d-%d-%d", &rst->rw.m_vibe_inparam.vibe_left, &rst->rw.m_vibe_inparam.vibe_right,
                &rst->rw.m_vibe_inparam.vibe_top, &rst->rw.m_vibe_inparam.vibe_bottom);
    }
    {
        char message1[256];
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_output", NULL);
        sprintf(message1, "%s", pstr);
        sscanf(message1, "%d-%d-%d-%d", &rst->rw.m_vibe_inparam.vibe_output_left, &rst->rw.m_vibe_inparam.vibe_output_right,
                &rst->rw.m_vibe_inparam.vibe_output_top, &rst->rw.m_vibe_inparam.vibe_output_bottom);
        printf(">>>test:message:%s pstr:%s  vibe_output(%d %d)<<<<<\n", message1, pstr, rst->rw.m_vibe_inparam.vibe_output_left, rst->rw.m_vibe_inparam.vibe_output_right);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_area_jwd", NULL);
        sprintf(rst->ro.m_param_config.area_jwd, "%s", pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_debug_switch", NULL);
        rst->ro.m_debug_switch = atoi(pstr);
    }

    //printf(">>>test: area_jwd:%s message:%s  vibe_output(%d %d)<<<<<\n", rst->ro.m_param_config.area_jwd, message1, rst->rw.m_vibe_inparam.vibe_output_left, rst->rw.m_vibe_inparam.vibe_output_right);

    rst->rw.m_linkage_pre_p = 0.0;
    rst->rw.m_linkage_pre_t = 0.0;
    rst->rw.m_linkage_pre_z = 0.0;
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_dome_to_gun", NULL);
        sprintf(message, "%s", pstr);
        sscanf(message, "%lf-%lf-%lf", &rst->rw.m_linkage_pre_p, &rst->rw.m_linkage_pre_t, &rst->rw.m_linkage_pre_z);
        //如果都是0，则表示相机是把枪机当作枪机使用
        if (rst->rw.m_linkage_pre_p < 0.000001 && rst->rw.m_linkage_pre_t < 0.000001 && rst->rw.m_linkage_pre_z < 0.000001) {
            rst->rw.m_linkage_dome2gun = 0;
        } else {

            rst->rw.m_linkage_dome2gun = 1;
        }
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, GunLinkageConfig0, GunLinkageConfig, "linkage_time_valid", NULL);
        sprintf(message, "%s", pstr);
        sscanf(message, "%lf-%lf", &rst->ro.m_config_vibe.vibe_start_time, &rst->ro.m_config_vibe.vibe_end_time);
    }
}

//外部调用

void gunlinkage_createConfig(FsConfig * const pConfig, void * parent) {

    gunlinkage_vibe_createConfig(pConfig, parent);

}

/* 检查pGunlinkage模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int gunlinkage_check_channel_changed(struct Gunlinkage * const pGunlinkage, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Gunlinkage
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
        if (sum == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum)return 0;
    } else {
        FsLog(FsLogType_error, FsPrintfIndex, "Invalid itemPath:\"%s\".\n", itemPath);
        fflush(stdout);
        if (NULL == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)) return 1;
        const void *parent0;
        const void *parent;
        {
            parent0 = pConfig;
            FsObjectList *list = fs_Config_node_template__IO(pConfig, &parent0, pConfig
                    , ((ConfigManager*) /* pRecognition */FsMacrosValue2(p, __check_channel_changed_Server)->ro._pConfigManager)->ro.__ipList_local, 0, "vsys");
            parent = list->pNode[list->startIndex];
            fs_ObjectList_delete__OI(list);
            list = fs_Config_node_template__IO(pConfig, &parent0, parent, NULL, 0, "vsysChannel");
            parent = list->pNode[list->startIndex + index];
            fs_ObjectList_delete__OI(list);
        }
        if (fs_Config_get_sum((FsEbml*)pConfig, (struct FsEbml_node *) parent) == /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum)return 0;
    }
#ifdef FsDebug
    FsLog2(FsLogType_info, FsPrintfIndex, "Item(=%p) has changed,index=%u,itemPath:\"%s\"/%p,sum=%llx/%llx\n"
            , /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item), index, itemPath, item, sum, /* pRecognition_item */ FsMacrosValue3(p, __check_channel_changed_Server, _item)->ro._sum);
#endif
    return 1;
#ifdef __check_channel_changed_itemListLock
#undef __check_channel_changed_itemListLock
#endif
#undef __check_channel_changed_Server
#endif

}

/* 
 * 创建一个新的Vlinkage对象;
 * 返回Vlinkage指针.
 */
struct Gunlinkage * gunlinkage_new__IO(/* 线程名 */const char threadName[]
        , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus
        , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* 配置管理对象 */ void *pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(dir);
    if (dir[len - 1] == '/')len++;
    else len += 2;
    len += strlen(threadName) + 1;
    struct Gunlinkage* rst = (struct Gunlinkage*) fsMalloc(sizeof (struct Gunlinkage) +len);
    memset(rst, 0, sizeof (struct Gunlinkage));
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
    /* 配置管理对象 */
    rst->ro._pConfigManager = pConfigManager;
    /* 每个线程最多处理的通道数 */
    //rst->ro._channelCountPerThread = channelCountPerThread;
    /* 通道链表 */
    rst->ro.__itemList_ = fs_ObjectList_new_unsafe__IO(8);
    /* 在有输出数据时向哪个开方分组写入信号 */
    rst->ro._pGroupSqrtOut = pGroupSqrtOut;
    /* 重置通道,负数表示重置完成,初始化为-1 */
    rst->rw._resetChannel = -1;
    /* 多线程锁 */
    pthread_mutex_init(&rst->p.__mutex, NULL);

    return rst;
}

/* 删除pRecord指向的实例对象 */
void gunlinkage_delete__OI(struct Gunlinkage * pGunlinkage) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pGunlinkage->ro.__itemList_);
    /* 多线程锁 */
    pthread_mutex_destroy(&pGunlinkage->p.__mutex);

    if (pGunlinkage) fsFree(pGunlinkage);

}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void gunlinkage_startThread(struct Gunlinkage* pGunlinkage,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (pGunlinkage->rw.runStatus == 0 && pGunlinkage->ro.stid == 0) {
        pGunlinkage->rw.runStatus = 1;
        pGunlinkage->rw.thisNice = thisNice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pGunlinkage->ro._threadName,
                (unsigned long) pGunlinkage->ro._pMonitor,
                pGunlinkage->ro._parentTid,
                (unsigned long) pGunlinkage->ro._externRunStatus);
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
        if (pthread_create(&pGunlinkage->ro.stid, &attr, (void* (*)(void*))gunlinkage_P_T, pGunlinkage)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pGunlinkage->rw.runStatus, pGunlinkage->ro.stid,
                pGunlinkage->ro._threadName,
                (unsigned long) pGunlinkage->ro._pMonitor,
                pGunlinkage->ro._parentTid,
                (unsigned long) pGunlinkage->ro._externRunStatus);
        fflush(stdout);
    }
}

/* 结束线程 */

void gunlinkage_stopThread(struct Gunlinkage * pGunlinkage) {
    pGunlinkage->rw.runStatus = 0;
    if (pGunlinkage->ro.stid) {
#ifdef FsDebug

        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pGunlinkage->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pGunlinkage->ro.stid);
        pGunlinkage->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void gunlinkage_item_frame_in_pthreadSafety__OI_4(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server Gunlinkage
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
unsigned int gunlinkage_item_frame_out_pthreadSafety__IO_4(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server Gunlinkage
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
unsigned int gunlinkage_item_get_frame_count_pthreadSafety(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server Gunlinkage
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
void gunlinkage_item_reset_groupSqrtOut(struct Gunlinkage * const pGunlinkage, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Gunlinkage
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