#include "vdomelinkage.h"

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
#include "../../Modules/track/FreeWalker.h"
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

struct GunData {
    unsigned int data[5]; //与枪机保持连接的数据
    /* 是否更新 */
    unsigned char status;
    int index; //目标编号
    int sendagain; //次数
    double left;
    double right;
    double top;
    double bottom;
    int dir;
    double speedx;
    double speedy;
    double thistime;
};
////////////////////////////////////////////////////////////////////////////////

/*********************************************************************************************************
函数名称：  AlgMapping   枪球坐标映射

输入参数：


        int gun_x1,				枪机画面上标定四个坐标点，依次是左上角，左下角，右上角，右下角
        int gun_y1,
        int gun_x2,
        int gun_y2,
        int gun_x3,
        int gun_y3,
        int gun_x4,
        int gun_y4,

        int ball_x1,			球机画面上标定四个坐标点，依次是左上角，左下角，右上角，右下角   
        int ball_y1,
        int ball_x2,
        int ball_y2,
        int ball_x3,
        int ball_y3,
        int ball_x4,
        int ball_y4,

        int in_x1,			    枪机上，用户框选的矩形框，依次是左上角，右下角
        int in_y1,
        int in_x2,
        int in_y2,
	
        int  w;					球机的图像尺寸
        int  h;	
	
	
	

输出参数：
    
        int* out_x1				枪机上框选的坐标位置，对应到球机的预置位上的坐标位置
        int* out_y1
        int* out_x2
        int* out_y2
        int* m_debug			输出中间调试数据


返回值： 							-1:输入参数不合理	0：正常检测
 *********************************************************************************************************/
int AlgMapping(int gun_x1, int gun_y1, int gun_x2, int gun_y2, int gun_x3, int gun_y3, int gun_x4, int gun_y4,
        int ball_x1, int ball_y1, int ball_x2, int ball_y2, int ball_x3, int ball_y3, int ball_x4, int ball_y4,
        int in_x1, int in_y1, int in_x2, int in_y2, int w, int h, int* out_x1, int* out_y1, int* out_x2, int* out_y2, int* m_debug) {

    if (in_x1 < 0 || in_y1 < 0 || in_x2 < 0 || in_y2 < 0 || in_x2 > w - 1 || in_y2 > h - 1 || w <= 0 || h <= 0) {
        return -1;
    }

    int qiangJiBiaoDing[8];
    int qiuJiBiaoDing[8];

    qiangJiBiaoDing[0] = gun_x1;
    qiangJiBiaoDing[1] = gun_y1;
    qiangJiBiaoDing[2] = gun_x2;
    qiangJiBiaoDing[3] = gun_y2;
    qiangJiBiaoDing[4] = gun_x3;
    qiangJiBiaoDing[5] = gun_y3;
    qiangJiBiaoDing[6] = gun_x4;
    qiangJiBiaoDing[7] = gun_y4;

    qiuJiBiaoDing[0] = ball_x1;
    qiuJiBiaoDing[1] = ball_y1;
    qiuJiBiaoDing[2] = ball_x2;
    qiuJiBiaoDing[3] = ball_y2;
    qiuJiBiaoDing[4] = ball_x3;
    qiuJiBiaoDing[5] = ball_y3;
    qiuJiBiaoDing[6] = ball_x4;
    qiuJiBiaoDing[7] = ball_y4;


    int i = 0, x, y, dx[4], dy[4];
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            x = in_x1;
            y = in_y1;
        } else if (i == 1) {
            x = in_x1;
            y = in_y2;
        } else if (i == 2) {
            x = in_x2;
            y = in_y1;
        } else {
            x = in_x2;
            y = in_y2;
        }

        if (qiangJiBiaoDing[3] <= qiangJiBiaoDing[1]) {
            m_debug[0] = 1;
            return -1;
        }
        int x1 = (qiangJiBiaoDing[2] - qiangJiBiaoDing[0])*(y - qiangJiBiaoDing[1]) / (qiangJiBiaoDing[3] - qiangJiBiaoDing[1]) + qiangJiBiaoDing[0];

        if (qiangJiBiaoDing[7] <= qiangJiBiaoDing[5]) {
            m_debug[0] = 2;
            return -1;
        }
        int x2 = (qiangJiBiaoDing[6] - qiangJiBiaoDing[4])*(y - qiangJiBiaoDing[5]) / (qiangJiBiaoDing[7] - qiangJiBiaoDing[5]) + qiangJiBiaoDing[4];

        if (x1 >= x2) {
            m_debug[0] = 3;
            return -1;
        }
        int y3 = (qiuJiBiaoDing[3] - qiuJiBiaoDing[1])*(y - qiangJiBiaoDing[1]) / (qiangJiBiaoDing[3] - qiangJiBiaoDing[1]) + qiuJiBiaoDing[1];

        int y4 = (qiuJiBiaoDing[7] - qiuJiBiaoDing[5])*(y - qiangJiBiaoDing[5]) / (qiangJiBiaoDing[7] - qiangJiBiaoDing[5]) + qiuJiBiaoDing[5];

        if (qiuJiBiaoDing[3] <= qiuJiBiaoDing[1]) {
            m_debug[0] = 4;
            return -1;
        }
        int x3 = (y3 - qiuJiBiaoDing[1])*(qiuJiBiaoDing[2] - qiuJiBiaoDing[0]) / (qiuJiBiaoDing[3] - qiuJiBiaoDing[1]) + qiuJiBiaoDing[0];

        if (qiuJiBiaoDing[7] <= qiuJiBiaoDing[5]) {
            m_debug[0] = 5;
            return -1;
        }
        int x4 = (y4 - qiuJiBiaoDing[5])*(qiuJiBiaoDing[6] - qiuJiBiaoDing[4]) / (qiuJiBiaoDing[7] - qiuJiBiaoDing[5]) + qiuJiBiaoDing[4];
        if (x3 >= x4) {
            m_debug[0] = 6;
            return -1;
        }

        dx[i] = (x - x1)*(x3 - x4) / (x1 - x2) + x3;
        dy[i] = (x - x1)*(y3 - y4) / (x1 - x2) + y3;


        m_debug[1] = dx[i];
        m_debug[2] = dy[i];
        m_debug[3] = w;
        m_debug[4] = h;


        if (dx[i] < 0) dx[i] = 0;
        else if (dx[i] + 2 > w) dx[i] = w - 2;
        if (dy[i] < 0) dy[i] = 0;
        else if (dy[i] + 2 > h) dy[i] = h - 2;

    }

    int xxi1, yyi1, xxi2, yyi2;
    for (xxi1 = dx[0], xxi2 = dx[0], yyi1 = dy[0], yyi2 = dy[0], i = 1; i < 4; i++) {
        if (xxi1 > dx[i])xxi1 = dx[i];
        if (xxi2 < dx[i])xxi2 = dx[i];
        if (yyi1 > dy[i])yyi1 = dy[i];
        if (yyi2 < dy[i])yyi2 = dy[i];
    }

    *out_x1 = xxi1;
    *out_y1 = yyi1;
    *out_x2 = xxi2;
    *out_y2 = yyi2;

    return 0;
}

int AlgMapping_1(double gun_x1, double gun_y1, double gun_x2, double gun_y2, double gun_x3, double gun_y3, double gun_x4, double gun_y4,
        double ball_x1, double ball_y1, double ball_x2, double ball_y2, double ball_x3, double ball_y3, double ball_x4, double ball_y4,
        double in_x1, double in_y1, double in_x2, double in_y2, int w, int h, double* out_x1, double* out_y1, double* out_x2, double* out_y2, double* m_debug) {

    if (in_x1 < 0 || in_y1 < 0 || in_x2 < 0 || in_y2 < 0 || in_x2 > w - 1 || in_y2 > h - 1 || w <= 0 || h <= 0) {
        return -1;
    }

    float qiangJiBiaoDing[8];
    float qiuJiBiaoDing[8];

    qiangJiBiaoDing[0] = gun_x1;
    qiangJiBiaoDing[1] = gun_y1;
    qiangJiBiaoDing[2] = gun_x2;
    qiangJiBiaoDing[3] = gun_y2;
    qiangJiBiaoDing[4] = gun_x3;
    qiangJiBiaoDing[5] = gun_y3;
    qiangJiBiaoDing[6] = gun_x4;
    qiangJiBiaoDing[7] = gun_y4;

    qiuJiBiaoDing[0] = ball_x1;
    qiuJiBiaoDing[1] = ball_y1;
    qiuJiBiaoDing[2] = ball_x2;
    qiuJiBiaoDing[3] = ball_y2;
    qiuJiBiaoDing[4] = ball_x3;
    qiuJiBiaoDing[5] = ball_y3;
    qiuJiBiaoDing[6] = ball_x4;
    qiuJiBiaoDing[7] = ball_y4;


    int i = 0;
    float x, y, dx[4], dy[4];
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            x = in_x1;
            y = in_y1;
        } else if (i == 1) {
            x = in_x1;
            y = in_y2;
        } else if (i == 2) {
            x = in_x2;
            y = in_y1;
        } else {
            x = in_x2;
            y = in_y2;
        }

        if (qiangJiBiaoDing[3] <= qiangJiBiaoDing[1]) {
            m_debug[0] = 1;
            return -1;
        }
        float x1 = (qiangJiBiaoDing[2] - qiangJiBiaoDing[0])*(y - qiangJiBiaoDing[1]) / (qiangJiBiaoDing[3] - qiangJiBiaoDing[1]) + qiangJiBiaoDing[0];

        if (qiangJiBiaoDing[7] <= qiangJiBiaoDing[5]) {
            m_debug[0] = 2;
            return -1;
        }
        float x2 = (qiangJiBiaoDing[6] - qiangJiBiaoDing[4])*(y - qiangJiBiaoDing[5]) / (qiangJiBiaoDing[7] - qiangJiBiaoDing[5]) + qiangJiBiaoDing[4];

        if (x1 >= x2) {
            m_debug[0] = 3;
            return -1;
        }
        float y3 = (qiuJiBiaoDing[3] - qiuJiBiaoDing[1])*(y - qiangJiBiaoDing[1]) / (qiangJiBiaoDing[3] - qiangJiBiaoDing[1]) + qiuJiBiaoDing[1];

        float y4 = (qiuJiBiaoDing[7] - qiuJiBiaoDing[5])*(y - qiangJiBiaoDing[5]) / (qiangJiBiaoDing[7] - qiangJiBiaoDing[5]) + qiuJiBiaoDing[5];

        if (qiuJiBiaoDing[3] <= qiuJiBiaoDing[1]) {
            m_debug[0] = 4;
            //return -1;
        }
        float x3 = (y3 - qiuJiBiaoDing[1])*(qiuJiBiaoDing[2] - qiuJiBiaoDing[0]) / (qiuJiBiaoDing[3] - qiuJiBiaoDing[1]) + qiuJiBiaoDing[0];

        if (qiuJiBiaoDing[7] <= qiuJiBiaoDing[5]) {
            m_debug[0] = 5;
            //return -1;
        }
        float x4 = (y4 - qiuJiBiaoDing[5])*(qiuJiBiaoDing[6] - qiuJiBiaoDing[4]) / (qiuJiBiaoDing[7] - qiuJiBiaoDing[5]) + qiuJiBiaoDing[4];
        if (x3 >= x4) {
            m_debug[0] = 6;
            //return -1;
        }

        dx[i] = (x - x1)*(x3 - x4) / (x1 - x2) + x3;
        dy[i] = (x - x1)*(y3 - y4) / (x1 - x2) + y3;


        m_debug[1] = dx[i];
        m_debug[2] = dy[i];
        m_debug[3] = w;
        m_debug[4] = h;


        if (dx[i] < 0) dx[i] = 0;
        else if (dx[i] > 1.0) dx[i] = 1.0;
        if (dy[i] < 0) dy[i] = 0;
        else if (dy[i] > 1.0) dy[i] = 1.0;

    }

    float xxi1, yyi1, xxi2, yyi2;
    for (xxi1 = dx[0], xxi2 = dx[0], yyi1 = dy[0], yyi2 = dy[0], i = 1; i < 4; i++) {
        if (xxi1 > dx[i])xxi1 = dx[i];
        if (xxi2 < dx[i])xxi2 = dx[i];
        if (yyi1 > dy[i])yyi1 = dy[i];
        if (yyi2 < dy[i])yyi2 = dy[i];
    }

    *out_x1 = xxi1;
    *out_y1 = yyi1;
    *out_x2 = xxi2;
    *out_y2 = yyi2;

    return 0;
}

/* 从配置获取通道数 */
static unsigned int domelinkage_private_get_channelCount(/* 可为空 */FsConfig *pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

/* 删除pvtrack_item指向的实例对象 */
static void domelinkage_private_item_delete__OI(struct Domelinkage_item * pDomelinkage_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {

    if (NULL == pDomelinkage_item)return;


    //将之前缓存帧去掉
    if ((pDomelinkage_item->ro.moduleMask & 0x8) != 0) {
        int i;
        for (i = 0; i < 5; i++) {
            if (pDomelinkage_item->rw._ppFrame[i] != NULL) {
                printf("%s %d, snap boat failed,delete the pframe._ppFrame[%d]:%p \n", __FUNCTION__, __LINE__, i, pDomelinkage_item->rw._ppFrame[i]);
                fs_Object_delete_pthreadSafety__OI(pDomelinkage_item->rw._ppFrame[i]);
                pDomelinkage_item->rw._ppFrame[i] = NULL;
            }
        }
    }

    fs_ObjectList_delete_custom__OI(pDomelinkage_item->ro.__framelistIn, fs_Object_delete_pthreadSafety__OI);
    fs_ObjectList_delete_custom__OI(pDomelinkage_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
    if ((pDomelinkage_item->ro.moduleMask & 0x8)) {

        /* 连接和调用外部命令所需要的同步锁 */
        pthread_mutex_destroy(&pDomelinkage_item->ro.__mutexCmdConnect);
        // 日志打印
        //pthread_mutex_destroy(&pDomelinkage_item->rw.m_log_mutex);

        /* 文字叠加 */
        if (pDomelinkage_item->ro.__pTypeFace) fs_TypeFace_delete__OI(pDomelinkage_item->ro.__pTypeFace);

        if (pDomelinkage_item->ro.m_config_devinfo.m_camera_model == 0 || pDomelinkage_item->ro.m_config_devinfo.m_camera_model == 1) {

            /* 注销录像的掩码 */
            configManager_mask_logoff_pthreadSafety(pDomelinkage_item->ro._pDomelinkage->ro._pConfigManager, "realStream", pDomelinkage_item->ro._uuid, pDomelinkage_item->ro._maskIndex);

            configManager_cmd_logoff(pDomelinkage_item->ro._pDomelinkage->ro._pConfigManager, "linkage_snap", pDomelinkage_item->ro._uuid, pDomelinkage_item->ro._ipv4, pDomelinkage_item, pShareBuffer);

            fs_ObjectList_delete_allSequence__OI(pDomelinkage_item->ro.__gunDataClientList);

            //关闭与相机的链接
            if (pDomelinkage_item->rw.pCamera_ctrl_item)camera_ctrl_tcp_delete__OI(pDomelinkage_item->rw.pCamera_ctrl_item);

            //销毁
            configManager_cmd_disconnect(pDomelinkage_item->ro._pDomelinkage->ro._pConfigManager, "video_part", pDomelinkage_item->ro._uuid, pDomelinkage_item);

            configManager_cmd_disconnect(pDomelinkage_item->ro._pDomelinkage->ro._pConfigManager, "cameractrl", pDomelinkage_item->ro._uuid, pDomelinkage_item);

            configManager_cmd_disconnect(pDomelinkage_item->ro._pDomelinkage->ro._pConfigManager, "record_inc", pDomelinkage_item->ro._uuid, pDomelinkage_item);

        }
    }
    fsFree(pDomelinkage_item);
}

/*  在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128  */
static int linkage_snap_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    struct Domelinkage_item * const pDomelinkage_item = (struct Domelinkage_item *) p;
    fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
    pthread_mutex_lock(&pDomelinkage_item->ro.__gunDataClientList->mutex);
    struct GunData *pGunData, **ppNode = (struct GunData**) pDomelinkage_item->ro.__gunDataClientList->pNode + pDomelinkage_item->ro.__gunDataClientList->startIndex;
    unsigned int i = pDomelinkage_item->ro.__gunDataClientList->nodeCount;
    for (; i > 0; i--) {
        pGunData = *ppNode++;
        if (pGunData->data[0] == requestID_3[0] && pGunData->data[1] == requestID_3[1] && pGunData->data[2] == requestID_3[2] &&
                pGunData->data[3] == head && pGunData->data[4] == (unsigned int) requestDataType) {
            break;
        }
    }

    //第一次接收到新目标
    if (0 == i) {
        pGunData = (struct GunData *) fsMalloc(sizeof (struct GunData));
        pGunData->data[0] = requestID_3[0];
        pGunData->data[1] = requestID_3[1];
        pGunData->data[2] = requestID_3[2];
        pGunData->data[3] = head;
        pGunData->data[4] = requestDataType;
        pGunData->sendagain = 0;

        printf("%s %d , First recive a new obj,v_linkage_times:%d \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_times);
        char message[512];
        sprintf(message, "%s %d , First recive a new obj,v_linkage_times:%d \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_times);
        MakeLog(message, 0);

        fs_ObjectList_insert_tail(pDomelinkage_item->ro.__gunDataClientList, pGunData);
    }


    /* 更新数据 */
    pGunData->status = 1;
    pGunData->index = *(long*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "number")->data.buffer;
    pGunData->left = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "left")->data.buffer;
    pGunData->right = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "right")->data.buffer;
    pGunData->top = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "top")->data.buffer;
    pGunData->bottom = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "bottom")->data.buffer;
    pGunData->dir = *(long*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "dir")->data.buffer;
    pGunData->speedx = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speedx")->data.buffer;
    pGunData->speedy = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "speedy")->data.buffer;
    pGunData->thistime = *(double*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "thistime")->data.buffer;


    struct FsEbml_node *action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "deviceno");
    sprintf(pDomelinkage_item->rw.m_domeparamcfg.deviceno, "%s", action_type->data.buffer);
    action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "devicename");
    sprintf(pDomelinkage_item->rw.m_domeparamcfg.devicename, "%s", action_type->data.buffer);
    action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "position_jwd");
    sprintf(pDomelinkage_item->rw.m_domeparamcfg.position_jwd, "%s", action_type->data.buffer);
    action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "area_jwd");
    sprintf(pDomelinkage_item->rw.m_domeparamcfg.area_jwd, "%s", action_type->data.buffer);
    action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "video_path");
    sprintf(pDomelinkage_item->rw.m_domeparamcfg.video_path, "%s", action_type->data.buffer);
    action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "real_url");
    sprintf(pDomelinkage_item->rw.m_domeparamcfg.real_url, "%s", action_type->data.buffer);
    pDomelinkage_item->rw.m_domeparamcfg.gun_camera_ip = *(long*) fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "gun_camera_ip")->data.buffer;
    action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "gun_uuid");
    sprintf(pDomelinkage_item->rw.v_linkage_curr_gunuuid, "%s", action_type->data.buffer);

    //0:左侧出，1：左侧入，2：右侧出，3：右侧入
    if (pGunData->dir == 0 || pGunData->dir == 3)
        pDomelinkage_item->rw.m_domeparamcfg.obj_dir = 1;
    else
        pDomelinkage_item->rw.m_domeparamcfg.obj_dir = 0;


    char loginfo[512];
    sprintf(loginfo, "{\n");
    MakeLog(loginfo, 0);
    sprintf(loginfo, "number:%d\n", pGunData->index);
    MakeLog(loginfo, 0);
    sprintf(loginfo, "dir:%d\n", pGunData->dir);
    MakeLog(loginfo, 0);
    sprintf(loginfo, "left:%f  right:%f  top:%f  bottom:%f \n", pGunData->left, pGunData->right, pGunData->top, pGunData->bottom);
    MakeLog(loginfo, 0);
    sprintf(loginfo, "speedx:%f  speedy:%f\n", pGunData->speedx, pGunData->speedy);
    MakeLog(loginfo, 0);
    sprintf(loginfo, "}\n");
    MakeLog(loginfo, 0);

    pDomelinkage_item->rw.v_linkage_curr_gunIndex = (unsigned int) - 1;
    int j;
    for (j = 0; j < MAX_GUN_COUNT; j++) {
        if (pDomelinkage_item->ro.m_calconfig[j].valid == 0)
            continue;
        //找到对应的uuid
        if (strcmp(pDomelinkage_item->rw.v_linkage_curr_gunuuid, pDomelinkage_item->ro.m_calconfig[j].gun_uuid) == 0) {
            pDomelinkage_item->rw.v_linkage_curr_gunIndex = j;
            printf("%s %d , find the gun uuid:%s index:%d \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_curr_gunuuid, pDomelinkage_item->rw.v_linkage_curr_gunIndex);
            char message[512];
            sprintf(message, "%s %d , find the gun uuid:%s index:%d \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_curr_gunuuid, pDomelinkage_item->rw.v_linkage_curr_gunIndex);
            MakeLog(message, 0);
            break;
        }
    }

    if (pDomelinkage_item->rw.v_linkage_curr_gunIndex == (unsigned int) - 1) {
        printf("%s %d ,cannot find the gun, uuid:%s \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_curr_gunuuid);
        char message[512];
        sprintf(message, "%s %d ,cannot find the gun, uuid:%s \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_curr_gunuuid);
        MakeLog(message, 0);
    }

    printf("%s %d : dome recv  a trigger info(obj_num:%d) (speedx:%f speedy:%f  thistime:%f) area_jwd:%s \n", __FUNCTION__, __LINE__,
            pGunData->index, pGunData->speedx, pGunData->speedy, pGunData->thistime, pDomelinkage_item->rw.m_domeparamcfg.area_jwd);
    char message[512];
    sprintf(message, "%s %d : dome recv  a trigger info(obj_num:%d) (speedx:%f speedy:%f  thistime:%f) area_jwd:%s \n", __FUNCTION__, __LINE__,
            pGunData->index, pGunData->speedx, pGunData->speedy, pGunData->thistime, pDomelinkage_item->rw.m_domeparamcfg.area_jwd);
    MakeLog(message, 0);

    pthread_mutex_unlock(&pDomelinkage_item->ro.__gunDataClientList->mutex);
    return 0 == i ? -128 : 1;
}


//外部调用

void domelinkage_alg_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "DomeLinkageConfig", "联动球机检测参数", "联动球机检测参数", 0, 0x7);

    //group 和 group1 是或者的关系，group内部是与的关系，也就是说，当条件 (moduleMask|&8) 和 (cameraModel == 0) 同时成立，或者(moduleMask|&8) 和 (cameraModel == 1) 同时成立   
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "0");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "1");
    }

    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "trackFrameInterval", "算法处理帧率", "算法处理帧率(2：原帧率1/2，3:原帧率1/3,其他值：原帧率)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2", "2", "2");

    void *pNode2 = fs_Config_node_integer_add(pConfig, parent, "zoomTimes", "算法处理图像缩小倍数", "算法处理图像缩小倍数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_default, 16, "缩小16倍", "缩小16倍");
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 4, "缩小4倍", "缩小4倍");
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 1, "原图尺寸", "原图尺寸");

    void *pNode1 = fs_Config_node_integer_add(pConfig, parent, "linkage_saveVideo", "是否保存通过录像", "是否保存通过录像(选1表示保存,选0表示不保存)", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, 0, "不保存", "不保存");
    fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 1, "保存", "保存");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_snap_number", "图片抓拍张数", "图片抓拍张数", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2", "2", "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_sample_time", "图片抓拍间隔时间", "图片抓拍间隔时间(单位为秒)", 0, 0x7, 0, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1.0", "1.0", "1.0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_focusRect_delay", "联动球机框选放大延迟时间(秒)", "联动球机框选放大延迟时间(秒)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5.0", "5.0", "5.0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_snap_delay", "联动球机拍照延迟时间(秒)", "联动球机拍照延迟时间(秒)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5.0", "5.0", "5.0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_left_speed_rate", "第一次左侧抓拍枪球速度比例", "第一次左侧抓拍枪球速度比例", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1.5", "1.5", "1.5");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_right_speed_rate", "第一次右侧抓拍枪球速度比例", "第一次右侧抓拍枪球速度比例", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1.5", "1.5", "1.5");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_left_speed_rate_1", "第二次左侧抓拍枪球速度比例", "第二次左侧抓拍枪球速度比例", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1.9", "1.9", "1.9");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_right_speed_rate_1", "第二次右侧抓拍枪球速度比例", "第二次右侧抓拍枪球速度比例", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2.0", "2.0", "2.0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_left_zoom_dif", "第一次左侧抓拍放大倍数偏移量", "第一次左侧抓拍放大倍数偏移量", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_right_zoom_dif", "第一次右侧抓拍放大倍数偏移量", "第一次右侧抓拍放大倍数偏移量", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_left_zoom_dif_1", "第二次左侧抓拍放大倍数偏移量", "第二次左侧抓拍放大倍数偏移量", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_right_zoom_dif_1", "第二次右侧抓拍放大倍数偏移量", "第二次右侧抓拍放大倍数偏移量", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_left_boathead", "第一次放大居中后，左侧船舱相对位置", "第一次放大居中后，左侧船舱相对位置(l-r-t-b)", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.037-0.4266-0.2157-0.8157", "0.037-0.4266-0.2157-0.8157", "0.037-0.4266-0.2157-0.8157");
    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_right_boathead", "第一次放大居中后，右侧船舱相对位置", "第一次放大居中后，右侧船舱相对位置(l-r-t-b)", 0, 0x7, 0, 32, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.4756-0.908-0.1731-0.681", "0.4756-0.908-0.1731-0.681", "0.4756-0.908-0.1731-0.681");

    pNode = fs_Config_node_integer_add(pConfig, parent, "linkage_gun_count", "关联枪机个数", "关联枪机个数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1", "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_debug_switch", "打开调试模式", "打开调试模式(0:关闭所有调试，>1开启不同的调试模式)", 0, 0x7, 1, 512, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
}

void domelinkage_cal_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "CalConfig1", "枪球联动标定参数-1", "枪球联动标定参数-1", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "0");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "1");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "DomeLinkageConfig linkage_gun_count", FsConfig_Condition_greater, "0");
    }
    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "gun_uuid", "关联枪机uuid", "关联枪机uuid", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "866", "866", "866");

    pNode = fs_Config_node_integer_add(pConfig, parent, "dome_preset_count", "球机预置位个数", "球机预置位个数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1", "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-1", "联动球机预置位-1(p-t-z)", "联动球机预置位-1(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-1", "联动枪机标定-1(像素坐标比例)", "联动枪机标定-1(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-1", "联动球机标定-1(像素坐标比例)", "联动球机标定-1(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-1", "联动枪机范围-1(像素坐标比例)", "联动枪机范围-1(l-r-t-b)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-2", "联动球机预置位-2(p-t-z)", "联动球机预置位-2(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-2", "联动枪机标定-2(像素坐标比例)", "联动枪机标定-2(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-2", "联动球机标定-2(像素坐标比例)", "联动球机标定-2(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-2", "联动枪机范围-2(像素坐标比例)", "联动枪机范围-1(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-3", "联动球机预置位-3(p-t-z)", "联动球机预置位-3(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-3", "联动枪机标定-3(像素坐标比例)", "联动枪机标定-3(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-3", "联动球机标定-3(像素坐标比例)", "联动球机标定-3(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-3", "联动枪机范围-3(像素坐标比例)", "联动枪机范围-3(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-4", "联动球机预置位-4(p-t-z)", "联动球机预置位-4(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-4", "联动枪机标定-4(像素坐标比例)", "联动枪机标定-4(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-4", "联动球机标定-4(像素坐标比例)", "联动球机标定-4(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-4", "联动枪机范围-4(像素坐标比例)", "联动枪机范围-4(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
}

void domelinkage_cal_1_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "CalConfig2", "枪球联动标定参数-2", "枪球联动标定参数-2", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "0");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "1");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "DomeLinkageConfig linkage_gun_count", FsConfig_Condition_greater, "1");
    }

    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "gun_uuid", "关联枪机uuid", "关联枪机uuid", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "866", "866", "866");

    pNode = fs_Config_node_integer_add(pConfig, parent, "dome_preset_count", "球机预置位个数", "球机预置位个数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1", "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-1", "联动球机预置位-1(p-t-z)", "联动球机预置位-1(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-1", "联动枪机标定-1(像素坐标比例)", "联动枪机标定-1(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-1", "联动球机标定-1(像素坐标比例)", "联动球机标定-1(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-1", "联动枪机范围-1(像素坐标比例)", "联动枪机范围-1(l-r-t-b)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-2", "联动球机预置位-2(p-t-z)", "联动球机预置位-2(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-2", "联动枪机标定-2(像素坐标比例)", "联动枪机标定-2(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-2", "联动球机标定-2(像素坐标比例)", "联动球机标定-2(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-2", "联动枪机范围-2(像素坐标比例)", "联动枪机范围-1(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-3", "联动球机预置位-3(p-t-z)", "联动球机预置位-3(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-3", "联动枪机标定-3(像素坐标比例)", "联动枪机标定-3(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-3", "联动球机标定-3(像素坐标比例)", "联动球机标定-3(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-3", "联动枪机范围-3(像素坐标比例)", "联动枪机范围-3(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-4", "联动球机预置位-4(p-t-z)", "联动球机预置位-4(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-4", "联动枪机标定-4(像素坐标比例)", "联动枪机标定-4(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-4", "联动球机标定-4(像素坐标比例)", "联动球机标定-4(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-4", "联动枪机范围-4(像素坐标比例)", "联动枪机范围-4(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
}

void domelinkage_cal_2_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "CalConfig3", "枪球联动标定参数-3", "枪球联动标定参数-3", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "0");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "8");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "cameraConfig cameraModel", FsConfig_Condition_equal, "1");
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "DomeLinkageConfig linkage_gun_count", FsConfig_Condition_greater, "2");
    }
    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "gun_uuid", "关联枪机uuid", "关联枪机uuid", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "866", "866", "866");

    pNode = fs_Config_node_integer_add(pConfig, parent, "dome_preset_count", "球机预置位个数", "球机预置位个数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1", "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-1", "联动球机预置位-1(p-t-z)", "联动球机预置位-1(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-1", "联动枪机标定-1(像素坐标比例)", "联动枪机标定-1(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-1", "联动球机标定-1(像素坐标比例)", "联动球机标定-1(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-1", "联动枪机范围-1(像素坐标比例)", "联动枪机范围-1(l-r-t-b)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "0");

    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-2", "联动球机预置位-2(p-t-z)", "联动球机预置位-2(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-2", "联动枪机标定-2(像素坐标比例)", "联动枪机标定-2(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-2", "联动球机标定-2(像素坐标比例)", "联动球机标定-2(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-2", "联动枪机范围-2(像素坐标比例)", "联动枪机范围-1(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "1");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-3", "联动球机预置位-3(p-t-z)", "联动球机预置位-3(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-3", "联动枪机标定-3(像素坐标比例)", "联动枪机标定-3(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-3", "联动球机标定-3(像素坐标比例)", "联动球机标定-3(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-3", "联动枪机范围-3(像素坐标比例)", "联动枪机范围-3(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "2");


    pNode = fs_Config_node_string_add(pConfig, parent, "linkage_presetPos-4", "联动球机预置位-4(p-t-z)", "联动球机预置位-4(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "355-16-16", "355-16-16", "355-16-16");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_cal-4", "联动枪机标定-4(像素坐标比例)", "联动枪机标定-4(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_dome_cal-4", "联动球机标定-4(像素坐标比例)", "联动球机标定-4(左上x-左上y-左下x-左下y-右上x-右上y-右下x-右下y)-1", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2", "0.1-0.1-0.1-0.2-0.2-0.1-0.2-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "linage_gun_area-4", "联动枪机范围-4(像素坐标比例)", "联动枪机范围-4(l-r-t-b)-2", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2", "0.1-0.2-0.1-0.2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "dome_preset_count", FsConfig_Condition_greater, "3");
}

void domelinkage_createConfig(FsConfig * const pConfig, void * parent) {

    domelinkage_alg_createConfig(pConfig, parent);

    domelinkage_cal_createConfig(pConfig, parent);
    domelinkage_cal_1_createConfig(pConfig, parent);
    domelinkage_cal_2_createConfig(pConfig, parent);
}

/* 检查pDomelinkage模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int domelinkage_check_channel_changed(struct Domelinkage * const pDomelinkage, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Domelinkage
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

void domelinkage_getConfig(struct Domelinkage_item *rst, FsConfig *pConfig, const void *const vsysChannel0, const void * const vsysChannel) {
    const void *DomeLinkageConfig0 = vsysChannel0;
    const void * const DomeLinkageConfig = fs_Config_node_get_first(pConfig, &DomeLinkageConfig0, vsysChannel, "DomeLinkageConfig");

    //处理抽帧
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "trackFrameInterval", NULL);
        rst->ro.m_linkageframeInterval = atoi(pstr);
    }

    rst->ro.m_dome_alg_config.m_linkage_saveVideo = fs_Config_node_integer_get_mask(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_saveVideo", NULL);

    //处理抽点
    rst->ro.zoomTimes = fs_Config_node_integer_get_mask(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "zoomTimes", NULL);

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_snap_delay", NULL);
        rst->ro.m_dome_alg_config.m_linkage_snap_delayTime = atof(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_left_speed_rate", NULL);
        rst->ro.m_dome_alg_config.m_linkage_left_speed_rate = atof(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_right_speed_rate", NULL);
        rst->ro.m_dome_alg_config.m_linkage_right_speed_rate = atof(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_left_speed_rate_1", NULL);
        rst->ro.m_dome_alg_config.m_linkage_left_speed_rate_1 = atof(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_right_speed_rate_1", NULL);
        rst->ro.m_dome_alg_config.m_linkage_right_speed_rate_1 = atof(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_left_zoom_dif", NULL);
        rst->ro.m_dome_alg_config.m_linkage_left_zoom_dif = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_right_zoom_dif", NULL);
        rst->ro.m_dome_alg_config.m_linkage_right_zoom_dif = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_left_zoom_dif_1", NULL);
        rst->ro.m_dome_alg_config.m_linkage_left_zoom_dif_1 = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_right_zoom_dif_1", NULL);
        rst->ro.m_dome_alg_config.m_linkage_right_zoom_dif_1 = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_debug_switch", NULL);
        rst->ro.m_debug_switch = atoi(pstr);
    }


    printf("%s %d, speed_rate:(%f-%f)-(%f-%f) zoom_dif: (%d-%d)-(%d-%d)\n", __FUNCTION__, __LINE__,
            rst->ro.m_dome_alg_config.m_linkage_left_speed_rate, rst->ro.m_dome_alg_config.m_linkage_right_speed_rate,
            rst->ro.m_dome_alg_config.m_linkage_left_speed_rate_1, rst->ro.m_dome_alg_config.m_linkage_right_speed_rate_1,
            rst->ro.m_dome_alg_config.m_linkage_left_zoom_dif, rst->ro.m_dome_alg_config.m_linkage_right_zoom_dif,
            rst->ro.m_dome_alg_config.m_linkage_left_zoom_dif_1, rst->ro.m_dome_alg_config.m_linkage_right_zoom_dif_1
            );

    char message[256];
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_left_boathead", NULL);
        sprintf(message, "%s", pstr);
    }
    sscanf(message, "%lf-%lf-%lf-%lf", &rst->ro.m_dome_alg_config.m_linkage_left_boathead_l, &rst->ro.m_dome_alg_config.m_linkage_left_boathead_r,
            &rst->ro.m_dome_alg_config.m_linkage_left_boathead_t, &rst->ro.m_dome_alg_config.m_linkage_left_boathead_b);

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_right_boathead", NULL);
        sprintf(message, "%s", pstr);
    }
    sscanf(message, "%lf-%lf-%lf-%lf", &rst->ro.m_dome_alg_config.m_linkage_right_boathead_l, &rst->ro.m_dome_alg_config.m_linkage_right_boathead_r,
            &rst->ro.m_dome_alg_config.m_linkage_right_boathead_t, &rst->ro.m_dome_alg_config.m_linkage_right_boathead_b);


    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_focusRect_delay", NULL);
        rst->ro.m_dome_alg_config.m_linkage_focusRect_delayTime = atof(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_snap_number", NULL);
        rst->ro.m_dome_alg_config.m_linkage_snapNumber = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_sample_time", NULL);
        rst->ro.m_dome_alg_config.m_linkage_sampleTime = atof(pstr);
    }

    //关联枪机个数
    int gun_count = fs_Config_node_integer_get_first(pConfig, DomeLinkageConfig0, DomeLinkageConfig, "linkage_gun_count", 1, NULL);

    printf(">>>>gun_count:%d<<<<\n", gun_count);

    int i;
    for (i = 0; i < gun_count; i++) {
        if (gun_count > MAX_GUN_COUNT)
            break;

        int errorno = 0;
        char nodename[64];

        sprintf(nodename, "CalConfig%d gun_uuid", i + 1);
        {
            const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
            sprintf(rst->ro.m_calconfig[i].gun_uuid, "%s", pstr);
        }

        int kk = 0;
        for (kk = 0; kk < i; kk++) {
            if (strcmp(rst->ro.m_calconfig[i].gun_uuid, rst->ro.m_calconfig[kk].gun_uuid) == 0) {
                printf(">>>>>the gun_uuid is the same:(%d %s)<<<<<<\n", i, rst->ro.m_calconfig[i].gun_uuid);
                continue;
            }
        }

        sprintf(nodename, "CalConfig%d dome_preset_count", i + 1);
        rst->ro.m_calconfig[i].calcount = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, nodename, NULL);

        int preset_count = rst->ro.m_calconfig[i].calcount > MAX_PRESET_COUNT ? MAX_PRESET_COUNT : rst->ro.m_calconfig[i].calcount;
        int j;
        for (j = 0; j < preset_count; j++) {

            sprintf(nodename, "CalConfig%d linkage_presetPos-%d", i + 1, j + 1);
            {
                const char *const pstr = fs_Config_node_string_get_first__IO(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
                printf("pstr:%p nodename:%s \n", pstr, nodename);
                char message[215];
                sprintf(message, "%s", pstr);
                if (strlen(message) != 0) {
                    sscanf(message, "%lf-%lf-%lf", &rst->ro.m_calconfig[i].m_calParam[j].p,
                            &rst->ro.m_calconfig[i].m_calParam[j].t,
                            &rst->ro.m_calconfig[i].m_calParam[j].z);
                } else {
                    errorno = 1;
                }

                printf("(i:%d j:%d) %lf-%lf-%lf \n", i, j, rst->ro.m_calconfig[i].m_calParam[j].p,
                        rst->ro.m_calconfig[i].m_calParam[j].t,
                        rst->ro.m_calconfig[i].m_calParam[j].z);
            }
            sprintf(nodename, "CalConfig%d linage_gun_cal-%d", i + 1, j + 1);
            {
                const char *const pstr = fs_Config_node_string_get_first__IO(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
                sprintf(message, "%s", pstr);
                if (strlen(message) != 0) {
                    sscanf(message, "%lf-%lf-%lf-%lf-%lf-%lf-%lf-%lf", &rst->ro.m_calconfig[i].m_calParam[j].gunSet[0],
                            &rst->ro.m_calconfig[i].m_calParam[j].gunSet[1], &rst->ro.m_calconfig[i].m_calParam[j].gunSet[2], &rst->ro.m_calconfig[i].m_calParam[j].gunSet[3],
                            &rst->ro.m_calconfig[i].m_calParam[j].gunSet[4], &rst->ro.m_calconfig[i].m_calParam[j].gunSet[5], &rst->ro.m_calconfig[i].m_calParam[j].gunSet[6],
                            &rst->ro.m_calconfig[i].m_calParam[j].gunSet[7]);
                } else {
                    errorno = 1;
                }

                printf("(i:%d j:%d) %lf-%lf-%lf-%lf-%lf-%lf-%lf-%lf \n", i, j, rst->ro.m_calconfig[i].m_calParam[j].gunSet[0],
                        rst->ro.m_calconfig[i].m_calParam[j].gunSet[1], rst->ro.m_calconfig[i].m_calParam[j].gunSet[2], rst->ro.m_calconfig[i].m_calParam[j].gunSet[3],
                        rst->ro.m_calconfig[i].m_calParam[j].gunSet[4], rst->ro.m_calconfig[i].m_calParam[j].gunSet[5], rst->ro.m_calconfig[i].m_calParam[j].gunSet[6],
                        rst->ro.m_calconfig[i].m_calParam[j].gunSet[7]);

                if (rst->ro.m_calconfig[i].m_calParam[j].gunSet[0] < 0 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[1] < 0 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[2] < 0 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[3] < 0 ||
                        rst->ro.m_calconfig[i].m_calParam[j].gunSet[4] < 0 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[5] < 0 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[6] < 0 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[7] < 0) {
                    errorno = 1;
                }
                if (rst->ro.m_calconfig[i].m_calParam[j].gunSet[0] > 1 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[1] > 1 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[2] > 1 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[3] > 1 ||
                        rst->ro.m_calconfig[i].m_calParam[j].gunSet[4] > 1 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[5] > 1 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[6] > 1 || rst->ro.m_calconfig[i].m_calParam[j].gunSet[7] > 1) {
                    errorno = 1;
                }
            }

            sprintf(nodename, "CalConfig%d linage_dome_cal-%d", i + 1, j + 1);
            {
                const char *const pstr = fs_Config_node_string_get_first__IO(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
                sprintf(message, "%s", pstr);
                if (strlen(message) != 0) {
                    sscanf(message, "%lf-%lf-%lf-%lf-%lf-%lf-%lf-%lf", &rst->ro.m_calconfig[i].m_calParam[j].domeSet[0],
                            &rst->ro.m_calconfig[i].m_calParam[j].domeSet[1], &rst->ro.m_calconfig[i].m_calParam[j].domeSet[2], &rst->ro.m_calconfig[i].m_calParam[j].domeSet[3],
                            &rst->ro.m_calconfig[i].m_calParam[j].domeSet[4], &rst->ro.m_calconfig[i].m_calParam[j].domeSet[5], &rst->ro.m_calconfig[i].m_calParam[j].domeSet[6], &rst->ro.m_calconfig[i].m_calParam[j].domeSet[7]);
                } else {
                    errorno = 1;
                }

                if (rst->ro.m_calconfig[i].m_calParam[j].domeSet[0] < 0 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[1] < 0 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[2] < 0 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[3] < 0 ||
                        rst->ro.m_calconfig[i].m_calParam[j].domeSet[4] < 0 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[5] < 0 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[6] < 0 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[7] < 0) {
                    errorno = 1;
                }
                if (rst->ro.m_calconfig[i].m_calParam[j].domeSet[0] > 1 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[1] > 1 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[2] > 1 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[3] > 1 ||
                        rst->ro.m_calconfig[i].m_calParam[j].domeSet[4] > 1 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[5] > 1 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[6] > 1 || rst->ro.m_calconfig[i].m_calParam[j].domeSet[7] > 1) {
                    errorno = 1;
                }

                printf("(i:%d j:%d) %lf-%lf-%lf-%lf-%lf-%lf-%lf-%lf \n", i, j, rst->ro.m_calconfig[i].m_calParam[j].domeSet[0],
                        rst->ro.m_calconfig[i].m_calParam[j].domeSet[1], rst->ro.m_calconfig[i].m_calParam[j].domeSet[2], rst->ro.m_calconfig[i].m_calParam[j].domeSet[3],
                        rst->ro.m_calconfig[i].m_calParam[j].domeSet[4], rst->ro.m_calconfig[i].m_calParam[j].domeSet[5], rst->ro.m_calconfig[i].m_calParam[j].domeSet[6], rst->ro.m_calconfig[i].m_calParam[j].domeSet[7]);
            }
            sprintf(nodename, "CalConfig%d linage_gun_area-%d", i + 1, j + 1);
            {
                const char *const pstr = fs_Config_node_string_get_first__IO(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
                sprintf(message, "%s", pstr);

                if (strlen(message) != 0) {
                    sscanf(message, "%lf-%lf-%lf-%lf", &rst->ro.m_calconfig[i].m_calParam[j].left,
                            &rst->ro.m_calconfig[i].m_calParam[j].right, &rst->ro.m_calconfig[i].m_calParam[j].top, &rst->ro.m_calconfig[i].m_calParam[j].bottom);
                } else {
                    errorno = 1;
                }

                if (rst->ro.m_calconfig[i].m_calParam[j].left < 0 || rst->ro.m_calconfig[i].m_calParam[j].right < 0 || rst->ro.m_calconfig[i].m_calParam[j].top < 0 || rst->ro.m_calconfig[i].m_calParam[j].bottom < 0) {
                    errorno = 1;
                }
                if (rst->ro.m_calconfig[i].m_calParam[j].left > 1 || rst->ro.m_calconfig[i].m_calParam[j].right > 1 || rst->ro.m_calconfig[i].m_calParam[j].top > 1 || rst->ro.m_calconfig[i].m_calParam[j].bottom > 1) {
                    errorno = 1;
                }

                printf("(i:%d j:%d) %lf-%lf-%lf-%lf \n", i, j, rst->ro.m_calconfig[i].m_calParam[j].left,
                        rst->ro.m_calconfig[i].m_calParam[j].right, rst->ro.m_calconfig[i].m_calParam[j].top, rst->ro.m_calconfig[i].m_calParam[j].bottom);
            }
        }

        if (errorno == 1) {
            rst->ro.m_calconfig[i].valid = 0;
            rst->ro.m_calconfig[i].m_calParam[j].valid = 0;
        } else {
            rst->ro.m_calconfig[i].valid = 1;
            rst->ro.m_calconfig[i].m_calParam[j].valid = 1;
        }
    }

    printf(">>>>gun_count:%d   OVER<<<<\n", gun_count);
}

static int domelinkage_check_pGunData(struct Domelinkage_item * pDomelinkage_item) {

    //检测目标坐标是否都为-1
    if (NULL != pDomelinkage_item->p.pGunData) {
        if (pDomelinkage_item->p.pGunData->status == 1) {
            //说明向枪机请球的最新目标出问题了(没找到目标或者其他问题)
            if (-1.0 == pDomelinkage_item->p.pGunData->bottom || -1.0 == pDomelinkage_item->p.pGunData->top) {
                printf("%s %d, dome get the Gundata,and the data is error,stop the inc !!!!\n", __FUNCTION__, __LINE__);
                char message[512];
                sprintf(message, "%s %d, dome get the Gundata,and the data is error,stop the inc !!!!\n", __FUNCTION__, __LINE__);
                MakeLog(message, 0);
                return -1;
            }
        }
    }

    return 0;
}

/*
 * 向枪机请求最新的目标坐标信息
 * 
 * 信息发送后，如果4秒内没有收到数据，直接将该联动数据从链表中删除
 */
static void domelinkage_request_updateData(struct Domelinkage_item * pDomelinkage_item, ConfigManager *pConfigManager, double thistime) {

    if (NULL == pDomelinkage_item->p.pGunData || pDomelinkage_item->p.pGunData->sendagain == 1) {
        //获取链表中的联动数据，然后向该连接(枪机)再次请求最新的坐标位置
        if (NULL == pDomelinkage_item->p.pGunData && pDomelinkage_item->ro.__gunDataClientList->nodeCount > 0) {

            pDomelinkage_item->p.pGunData = (struct GunData*) pDomelinkage_item->ro.__gunDataClientList->pNode[pDomelinkage_item->ro.__gunDataClientList->startIndex];

            printf("%s %d : dome send  a request for new info(obj_num:%d) \n", __FUNCTION__, __LINE__, pDomelinkage_item->p.pGunData->index);
            char message[512];
            sprintf(message, "%s %d : dome send  a request for new info(obj_num:%d) \n", __FUNCTION__, __LINE__, pDomelinkage_item->p.pGunData->index);
            MakeLog(message, 0);

            /* 发回执 */
            FsString* pSenddata = fs_String_new__IO(24);
            pSenddata->lenth = 12;
            *((unsigned int *) pSenddata->buffer) = pDomelinkage_item->p.pGunData->data[3], *((unsigned int *) (pSenddata->buffer + 4)) = 4;
            pSenddata->buffer[0] &= 0xF0;
            pSenddata->buffer[0] |= 0xB;
            *((unsigned int *) (pSenddata->buffer + 8)) = pDomelinkage_item->p.pGunData->index;
            memcpy(pSenddata->buffer + 12, pDomelinkage_item->p.pGunData->data, sizeof (unsigned int)*3);
            /* 对保留连接发送数据,发送前应先调用configManager_connect_check检查连接状态 */
            configManager_connect_send__OI_2__old(pSenddata);

            /*获取开始时间*/
            pDomelinkage_item->rw.v_linkage_incStartTime = thistime;

            pDomelinkage_item->p.pGunData->status = 0;
            pDomelinkage_item->p.gunDataTimeout = thistime + 4.0;

            /*获取一个新目标开始抓拍，先获取此次抓拍的图片张数，以及间隔时间*/
            pDomelinkage_item->rw.v_linkage_snap_times = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber;
            pDomelinkage_item->rw.v_linkage_snap_intervalTime = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime;
            pDomelinkage_item->rw.v_linkage_snap_lastTime = 0.0;
            pDomelinkage_item->rw.v_linkage_times = 0;

        } else if (pDomelinkage_item->p.pGunData != NULL && pDomelinkage_item->p.pGunData->sendagain == 1) { //再次获取坐标数据

            printf("%s %d : dome send again, a request for new info(obj_num:%d) \n", __FUNCTION__, __LINE__, pDomelinkage_item->p.pGunData->index);
            char message[512];
            sprintf(message, "%s %d : dome send again, a request for new info(obj_num:%d) \n", __FUNCTION__, __LINE__, pDomelinkage_item->p.pGunData->index);
            MakeLog(message, 0);

            pDomelinkage_item->p.pGunData->sendagain = 0;

            /* 发回执 */
            FsString* pSenddata = fs_String_new__IO(24);
            pSenddata->lenth = 12;
            *((unsigned int *) pSenddata->buffer) = pDomelinkage_item->p.pGunData->data[3], *((unsigned int *) (pSenddata->buffer + 4)) = 4;
            pSenddata->buffer[0] &= 0xF0;
            pSenddata->buffer[0] |= 0xB;
            *((unsigned int *) (pSenddata->buffer + 8)) = pDomelinkage_item->p.pGunData->index;
            memcpy(pSenddata->buffer + 12, pDomelinkage_item->p.pGunData->data, sizeof (unsigned int)*3);
            /* 对保留连接发送数据,发送前应先调用configManager_connect_check检查连接状态 */
            configManager_connect_send__OI_2__old(pSenddata);

            pDomelinkage_item->p.pGunData->status = 0;
            pDomelinkage_item->p.gunDataTimeout = thistime + 4.0;
        }

    } else if (pDomelinkage_item->p.pGunData->status == 0 && pDomelinkage_item->p.gunDataTimeout < thistime) {
        FsPrintf(1, "dome not recive the message over time.\n");
        fflush(stdout);
        char message[512];
        sprintf(message, "dome not recive the message over time.\n");
        MakeLog(message, 0);
        fs_ObjectList_remove_head_pthreadSafety(pDomelinkage_item->ro.__gunDataClientList);
        /* 释放原来保存的连接,必须是原来保存过的连接,否则会死机 */
        if (pDomelinkage_item->p.pGunData->data != NULL)
            configManager_connect_release(pConfigManager, pDomelinkage_item->p.pGunData->data, 0);

        if (pDomelinkage_item->p.pGunData->data != NULL) fsFree(pDomelinkage_item->p.pGunData);
        pDomelinkage_item->p.pGunData = NULL;
    }
}

void domelinkage_save_video(struct Domelinkage_item * pDomelinkage_item) {

    //如果不保存录像，直接返回
    if (pDomelinkage_item->ro.m_dome_alg_config.m_linkage_saveVideo == 0)
        return;

    pthread_mutex_lock(&pDomelinkage_item->ro.__mutexCmdConnect);
    if (pDomelinkage_item->p.record_item_add_partVideo) {

        char v_videoPath[512];
        time_t ts = (time_t) fs_time_GMT_get();
        struct tm ti_s;
        localtime_r(&ts, &ti_s);

        sprintf(v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pDomelinkage_item->ro._pDomelinkage->ro._dir,
                pDomelinkage_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);


        FsPrintf2(1, ">>>>save video path:%s time(%f %f %f)\n", v_videoPath,
                pDomelinkage_item->rw.v_linkage_incStartTime, pDomelinkage_item->rw.v_linkage_incEndTime, fs_time_uptime_get());


        pDomelinkage_item->p.record_item_add_partVideo(pDomelinkage_item->p.pRecord_item, v_videoPath, pDomelinkage_item->rw.v_linkage_incStartTime - 10, pDomelinkage_item->rw.v_linkage_incEndTime + 5, NULL);
        fflush(stdout);
    } else {
        FsPrintf(1, ".record_item_add_partVideo is NULL !!!.....\n");
        fflush(stdout);
    }
    pthread_mutex_unlock(&pDomelinkage_item->ro.__mutexCmdConnect);
}

/* 在期望连接的命令字和uuid改变时调用 */
static void record_inc_private_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p) {
    struct Domelinkage_item* pdome_item = (struct Domelinkage_item *) p;
    if (strcmp("record_inc", cmd) == 0 && strcmp(uuid, pdome_item->ro._uuid) == 0) {
        pthread_mutex_lock(&pdome_item->ro.__mutexCmdConnect);
        pdome_item->p.record_inc_item_add_partVideo__OI_2 = (Record_item_add_inc_pthreadSafety__OI_2) do_local;
        pdome_item->p.pRecord_inc_item = (struct Record_item*) do_local_p;
        pthread_mutex_unlock(&pdome_item->ro.__mutexCmdConnect);
    } else {
        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pdome_item->ro._uuid);
        fflush(stdout);
    }
}

/* 在期望连接的命令字和uuid改变时调用 */
static void incident_private_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p) {
    struct Domelinkage_item* pdome_item = (struct Domelinkage_item *) p;
    if (strcmp("video_part", cmd) == 0 && strcmp(uuid, pdome_item->ro._uuid) == 0) {
        pthread_mutex_lock(&pdome_item->ro.__mutexCmdConnect);
        pdome_item->p.record_item_add_partVideo = (Record_item_add_partVideo_pthreadSafety) do_local;
        pdome_item->p.pRecord_item = (struct Record_item*) do_local_p;
        pthread_mutex_unlock(&pdome_item->ro.__mutexCmdConnect);
    } else if (strcmp("cameractrl", cmd) == 0 && strcmp(uuid, pdome_item->ro._uuid) == 0) {
        pthread_mutex_lock(&pdome_item->ro.__mutexCmdConnect);
        pdome_item->p.pCapture_item = (struct Capture_item*) do_local;
        pthread_mutex_unlock(&pdome_item->ro.__mutexCmdConnect);
    } else {
        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pdome_item->ro._uuid);
        fflush(stdout);
    }
}

static void domelinkage_private_item_new(struct Domelinkage *pDomelinkage, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的采集项 */struct Domelinkage_item **ppDomelinkage_item, FsGroupSqrt * const pGroupSqrt
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pDomelinkage->ro._pConfigManager)->ro.__pConfig;
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
                        //                        if (ppNodeCapture_item[index] != NULL && ppNodeCapture_item[index]->ro._sum == fs_Config_get_sum((FsEbml*)pConfig, (struct FsEbml_node*) sumNode)) {
                        //                            index++;
                        //                            continue;
                        //                        }
                    } else {
                        sumNode = vsysChannel = list;
                        if (*ppDomelinkage_item != NULL && (*ppDomelinkage_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppDomelinkage_item) {
                        domelinkage_private_item_delete__OI(*ppDomelinkage_item, pShareBuffer);
                    }
                    ////////////////////////////////////////////////////////////
                    struct Domelinkage_item * rst;
                    unsigned long moduleMask;
                    if (Domelinkage_Mask != 0 && (Domelinkage_Mask & (moduleMask = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL))) == 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            rst = (struct Domelinkage_item*) fsMalloc(sizeof (struct Domelinkage_item) +len);

                            //结构体清零...
                            memset(rst, 0, sizeof (struct Domelinkage_item));
                            /* 掩码 */
                            rst->ro.moduleMask = moduleMask;
                            FsPrintf(1, "Domelinkage_private_item_new....malloc the  Domelinkage_item: %ld \n", sizeof (struct Domelinkage_item) +len);
                            fflush(stdout);
                            pd = (char*) (rst + 1);
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len);
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
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pDomelinkage->ro._pGroupSqrtOut->groupSqrt_member, pDomelinkage->ro._pGroupSqrtOut->groupSqrt_group
                                    , pDomelinkage->ro._pGroupSqrtOut->groupValue, channel - 1, &pDomelinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        ////////////////////////////////////////////////////////
                        if (channel > 0) {
                            {
                                const char* const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig ipv4", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ip, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ip, "192.168.8.252");
                            }
                            {
                                const char* const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rstpUsername", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraUid, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraUid, "admin");
                            }
                            {
                                const char* const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rstpPasswd", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraPwd, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraPwd, "12345");
                            }
                            rst->ro.m_config_devinfo.m_camera_port = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rtspPort", 0, NULL);

                            //相机云台控制IP
                            {
                                const char* const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig holderipv4", NULL);
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


                            rst->rw.pCamera_ctrl_item = NULL;
                            rst->ro.__gunDataClientList = NULL;

                            //如果相机类型为球机类型
                            if (rst->ro.m_config_devinfo.m_camera_model == 0 || rst->ro.m_config_devinfo.m_camera_model == 1) {

                                //联动次数
                                //rst->rw.v_linkage_times = 2;
                                domelinkage_getConfig(rst, pConfig, vsysChannel0, vsysChannel);


                                //拉流
                                rst->ro._maskIndex = configManager_mask_register_pthreadSafety(&rst->ro.__realMask, pDomelinkage->ro._pConfigManager, "realStream", rst->ro._uuid, Record_BaseLine + __LINE__);
                                configManager_mask_value_set_pthreadSafety(pDomelinkage->ro._pConfigManager, rst->ro.__realMask, rst->ro._maskIndex, 1);

                                //创建相机控制对象
                                rst->rw.pCamera_ctrl_item = camera_ctrl_tcp_item_new__IO(rst->ro.m_config_devinfo.m_camera_brand, rst->ro.m_config_devinfo.m_camera_model,
                                        rst->ro.m_config_devinfo.m_camera_ctrl_ip, rst->ro.m_config_devinfo.m_camera_ctrl_port, rst->ro.m_config_devinfo.m_szCameraUid,
                                        rst->ro.m_config_devinfo.m_szCameraPwd, 0);

                                //链接相机...
                                camera_ctrl_tcp_connect(rst->rw.pCamera_ctrl_item);

                                FsPrintf2(1, "create the camera_ctrl_item (cambrand:%d cammodel:%d camctrlip:%s camctrlport:%d user:%s pwd:%s _maskIndex:%d uuid:%s) \n", rst->ro.m_config_devinfo.m_camera_brand,
                                        rst->ro.m_config_devinfo.m_camera_model, rst->ro.m_config_devinfo.m_camera_ctrl_ip, rst->ro.m_config_devinfo.m_camera_ctrl_port,
                                        rst->ro.m_config_devinfo.m_szCameraUid, rst->ro.m_config_devinfo.m_szCameraPwd, rst->ro._maskIndex, rst->ro._uuid);
                                fflush(stdout);

                                // 注册与枪机通讯
                                configManager_cmd_register(pDomelinkage->ro._pConfigManager, "linkage_snap", rst->ro._uuid, 0, rst, 0, linkage_snap_private_cmd_cb, NULL, rst, pShareBuffer);

                                // 创建联动消息链表
                                rst->ro.__gunDataClientList = fs_ObjectList_new__IO(6);

                                /* 绑定命令字 保存视频录像*/
                                configManager_cmd_connect(pDomelinkage->ro._pConfigManager, "video_part", rst->ro._uuid, rst, incident_private_cmd_connect_cb, rst);
                                /* 绑定命令字 保存视频录像*/
                                configManager_cmd_connect(pDomelinkage->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, incident_private_cmd_connect_cb, rst);

                                /* 绑定命令字 */
                                configManager_cmd_connect(pDomelinkage->ro._pConfigManager, "record_inc", rst->ro._uuid, rst, record_inc_private_cmd_connect_cb, rst);
                            }

                            /* 所属的记录对象 */
                            rst->ro._pDomelinkage = pDomelinkage;
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 连接和调用外部命令所需要的同步锁 */
                            pthread_mutex_init(&rst->ro.__mutexCmdConnect, NULL);
                        }
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0) *ppDomelinkage_item = rst;
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


//交换

void order_swap(int * a, int * b) {
    int c = 0;
    if (!a || !b) return;
    if (*a < *b) return;
    c = *b;
    *b = *a;
    *a = c;
}

/*
 * 根据传过来的坐标位置，选择正确的球机预置位
 * 如果球机预置位只设置了一个，直接返回
 */
int domelinkage_chosePreset(struct Domelinkage_item * pDomelinkage_item, int w, int h) {

    //没有找到关联的枪机uuid
    if (pDomelinkage_item->rw.v_linkage_curr_gunIndex == (unsigned int) - 1)
        return -1;

    //球机标定参数个数为0
    if (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].calcount == 0)
        return -1;

    //球机标定参数个数为1，不需要选择，直接联动
    if (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].calcount == 1) {
        return 0;
    }


    int count = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].calcount;
    int maxarea = 0;
    int max_count = -1;
    int x0, y0, x1, y1, x2, y2, x3, y3;

    x0 = (int) (pDomelinkage_item->p.pGunData->left * w);
    y0 = (int) (pDomelinkage_item->p.pGunData->top * h);
    x1 = (int) (pDomelinkage_item->p.pGunData->right * w);
    y1 = (int) (pDomelinkage_item->p.pGunData->bottom * h);

    //确定目标区域(x0,y0,x1,y1) 与 球机多个标定点的区域 做比较，找到重叠面积最大的一个
    int i = 0;
    for (i = 0; i < count; i++) {

        x2 = (int) (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[i].left * w);
        y2 = (int) (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[i].top * h);
        x3 = (int) (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[i].right * w);
        y3 = (int) (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[i].bottom * h);

        int maxLeft = (x2 > x0) ? x2 : x0;
        int minRight = (x3 > x1) ? x1 : x3;
        int maxUp = (y2 > y0) ? y2 : y0;
        int minBottom = (y3 > y1) ? y1 : y3;

        if (maxLeft > minRight || maxUp > minBottom) {

        } else {

            int temp = (minRight - maxLeft) * (minBottom - maxUp);
            if (temp > maxarea) {
                maxarea = temp;
                max_count = i;
            }

            FsPrintf(2, "alg the %d area,(%d %d %d %d)-(%d %d %d %d)-(%d %d %d %d) \n", max_count, x0, y0, x1, y1, x2, y2, x3, y3, maxLeft, minRight, maxUp, minBottom);
            fflush(stdout);
        }
    }

    if (max_count == -1) {
        FsPrintf(2, ">>>>>>>>>>>>>>>>>>>>>the obj sit is not in the preset area<<<<<<<<<<<<<<<<<<<<\n");
        fflush(stdout);
    }

    return max_count;
}

/*
 * 检测两个ptz是否相同，返回0，表示相同，返回1，表示不同
 */
static int CheckPos(float set_p, float set_t, float set_z, float curr_p, float curr_t, float curr_z) {

    int isdif = 1;

    //如果当前的ptz与预置位的ptz全部相等
    if (set_p == curr_p && set_t == curr_t && set_z == curr_z) {
        isdif = 0;
    }

    //如果当前的z 与 预置位的 z相等，且pt与预置位的pt相差都小于等于1
    //    if (curr_z == set_z) {
    //        if (abs(curr_p - set_p) <= 1 && abs(curr_t - set_t) <= 1) {
    //            isdif = 0;
    //        }
    //    }

    return isdif;
}

/*
 * 回到球机检测预置位
 */
void domelinkage_goback_PrePTZ(struct Domelinkage_item * pDomelinkage_item) {

    //没有找到关联的枪机uuid
    if (pDomelinkage_item->rw.v_linkage_curr_gunIndex == (unsigned int) - 1)
        return;

    //球机标定参数个数为0
    if (pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].calcount == 0)
        return;


    int index = pDomelinkage_item->rw.v_linkage_curr_calIndex;
    if (index < MAX_PRESET_COUNT && index > -1) {

        float p = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].p;
        float t = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].t;
        float z = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].z;

        int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, p, t, z);
        if (ret != 0) {
            printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, p, t, z);
        }
        printf("%s %d Move the camera goback to the pre position(index=%d)\n", __FUNCTION__, __LINE__, index);
    } else {
        printf("%s %d error curr_calIndex, move failed (index=%d)\n", __FUNCTION__, __LINE__, index);
    }

}

/*
 * 框选放大操作
 */
void domelinkage_hiki_focusRect(struct Domelinkage_item * pDomelinkage_item, int w, int h, double map_left, double map_right, double map_top, double map_bottom) {

    //放大坐标
    double focus_left = 0, focus_right = 0, focus_top = 0, focus_bottom = 0; //用于框选放大的坐标

    focus_left = map_left;
    focus_right = map_right;
    focus_top = map_top;
    focus_bottom = map_bottom;

    //球机中坐标补差：
    /*坐标补差：1.根据船的运行速度，和拍照延迟时间*/
    double m_ratio = 1.0; //6.6; //同一目标在枪机和球机中的速度比例
    //1.延迟时间导致的 ： 船的速度(像素点距离/毫秒)×拍照延迟时间(秒*1000)
    double difdis = (double) (pDomelinkage_item->p.pGunData->speedx * pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime * 1000 * m_ratio);
    //向右运动
    if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2) {
        focus_right += difdis;
        focus_left += difdis;
    } else {
        focus_left -= difdis;
        focus_right -= difdis;
    }

    if (focus_left < 0 || focus_right > 1 || focus_left > 1 || focus_right < 0 || focus_top < 0 || focus_top > 1 || focus_bottom < 0 || focus_bottom > 1) {
        if (focus_left < 0) focus_left = 0.001;
        if (focus_right < 0) focus_right = 0.001;
        if (focus_left > 1) focus_left = 0.99;
        if (focus_right > 1) focus_right = 0.99;

        if (focus_top < 0) focus_top = 0.001;
        if (focus_bottom < 0) focus_bottom = 0.001;
        if (focus_top > 1) focus_top = 0.99;
        if (focus_bottom > 1) focus_bottom = 0.99;

        printf("the boat pos is error, %lf %lf %lf %lf \n", focus_left, focus_right, focus_top, focus_bottom);
    }

    pDomelinkage_item->rw.m_debug_adjust_left = focus_left;
    pDomelinkage_item->rw.m_debug_adjust_right = focus_right;
    pDomelinkage_item->rw.m_debug_adjust_top = focus_top;
    pDomelinkage_item->rw.m_debug_adjust_bottom = focus_bottom;


    printf("%s %d Fix the postion (%lf %lf %lf %lf)-((%lf %lf %lf %lf)) speed:%lf m_linkage_snap_delayTime:%lf difdis:%lf w=%d \n", __FUNCTION__, __LINE__,
            map_left, map_right, map_top, map_bottom, focus_left, focus_right, focus_top, focus_bottom,
            pDomelinkage_item->p.pGunData->speedx, pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime, difdis, w);


    //框选放大指令
    int ret = camera_crtl_tcp_FocusRect(pDomelinkage_item->rw.pCamera_ctrl_item, focus_left, focus_right, focus_top, focus_bottom);
    if (ret != 0) {
        printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, focus_left, focus_right, focus_top, focus_bottom);
    }

}

/*
 * 框选放大操作1
 * 
 * 在这里，我们做一些预言性质的操作
 * 
 *      1.根据目标框的大小，判定该目标的船舶类型
 *      a.大型船只(目标拉近之后，当船只刚好占满画面时，不能够看清楚船舷号)
 *              此目标需要多次拉近，才能完成业务所需的图片：
 *                                  第一次拉近，抓拍船只全景图
 *                                  第二次拉近，根据设定，拉近船身的某个特定位置，抓拍清晰船舷号
 * 
 *      b.小型船只(目标拉近之后，当船只刚好占满画面时，能够看清楚船舷号)
 *              此目标只需要拉近一次，即可实现业务所需的图片：船只全景图，清晰船舷号
 * 
 */
void domelinkage_walker_focusRect(struct Domelinkage_item * pDomelinkage_item, int w, int h, double map_left, double map_right, double map_top, double map_bottom) {

    //根据实际业务进行调整
    if (0) {
        int www = (map_right - map_left) * w;
        int hhh = (map_bottom - map_top) * h;

        int objtype = 0;

        //如果船长度大于设定值
        if (www > 100 && hhh > 50) {
            objtype = 1;
        }

        //小船
        if (objtype == 0) {

            //只需要联动一次
            pDomelinkage_item->rw.v_linkage_times = 0;

            //设置参数
            pDomelinkage_item->rw.v_linkage_snap_times = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber;
            pDomelinkage_item->rw.v_linkage_snap_intervalTime = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime;
            pDomelinkage_item->rw.v_linkage_snap_lastTime = 0.0;

            printf("%s %d,get a small boat,linkage one time.. \n", __FUNCTION__, __LINE__);
        }

        //大船
        if (objtype == 1) {

            if (pDomelinkage_item->rw.v_linkage_times == (unsigned int) - 1) { //第一次联动

                pDomelinkage_item->rw.v_linkage_times = 0;

                //设置参数
                pDomelinkage_item->rw.v_linkage_snap_times = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber;
                pDomelinkage_item->rw.v_linkage_snap_intervalTime = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime;
                pDomelinkage_item->rw.v_linkage_snap_lastTime = 0.0;

                //修改联动坐标

                printf("%s %d,get a large boat,linkage %d time.. \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_times + 1);

            } else if (pDomelinkage_item->rw.v_linkage_times == 0) { //第二次联动

                //设置参数
                pDomelinkage_item->rw.v_linkage_snap_times = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber;
                pDomelinkage_item->rw.v_linkage_snap_intervalTime = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime;
                pDomelinkage_item->rw.v_linkage_snap_lastTime = 0.0;


                //修改联动坐标

                printf("%s %d,get a large boat,linkage %d time.. \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_times + 1);
            }
        }
    }

    //放大坐标
    double focus_left = 0, focus_right = 0, focus_top = 0, focus_bottom = 0; //用于框选放大的坐标

    focus_left = map_left;
    focus_right = map_right;
    focus_top = map_top;
    focus_bottom = map_bottom;

    /* 球机中坐标补偿，1.根据船的运行速度，补偿运动偏差；2.根据拍照延迟时间(聚焦)，补偿时间延迟偏差  */
    int m_spnap_index = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - pDomelinkage_item->rw.v_linkage_snap_times;
    if (m_spnap_index == 0) {
        //球机中坐标补差：
        //坐标补差：1.根据船的运行速度，和拍照延迟时间
        double m_ratio = 0.0;
        //向右运动
        if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2)
            m_ratio = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_speed_rate; //6.6; //同一目标在枪机和球机中的速度比例
        else
            m_ratio = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_speed_rate; //6.6; //同一目标在枪机和球机中的速度比例

        //1.延迟时间导致的 ： 船的速度(像素点距离/毫秒)×拍照延迟时间(秒*1000)
        double difdisx = (double) (pDomelinkage_item->p.pGunData->speedx * pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime * 1000 * m_ratio);
        //向右运动
        if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2) {
            focus_right += difdisx;
            focus_left += difdisx;
        } else {
            focus_left -= difdisx;
            focus_right -= difdisx;
        }

        /*
        //speedy为正，向下运动，为负，向上运动
        double difdisy = (double) (pDomelinkage_item->p.pGunData->speedy * pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime * 1000 * m_ratio);
        focus_top += difdisy;
        focus_bottom += difdisy;
         */

        printf("%s %d,Adjust boat postion,[%d %d %d %d]-[%d %d %d %d],difdisx:%lf,speedx:%lf,delayTime:%lf,m_ratio:%lf \n", __FUNCTION__, __LINE__, map_left, map_right, map_top, map_bottom,
                focus_left, focus_right, focus_top, focus_bottom, difdisx, pDomelinkage_item->p.pGunData->speedx, pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime, m_ratio);
        char message[512];
        sprintf(message, "%s %d,Adjust boat postion,[%d %d %d %d]-[%d %d %d %d],difdisx:%lf,speedx:%lf,delayTime:%lf,m_ratio:%lf \n", __FUNCTION__, __LINE__, map_left, map_right, map_top, map_bottom,
                focus_left, focus_right, focus_top, focus_bottom, difdisx, pDomelinkage_item->p.pGunData->speedx, pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime, m_ratio);
        MakeLog(message, 0);

    } else if (m_spnap_index == 1) {

        double m_ratio = 0.0;
        //向右运动,左侧船舱
        if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2) {
            focus_left = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_boathead_l;
            focus_right = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_boathead_r;
            focus_top = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_boathead_t;
            focus_bottom = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_boathead_b;

            m_ratio = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_speed_rate_1;

        } else { //向左运动,右侧船舱
            focus_left = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_boathead_l;
            focus_right = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_boathead_r;
            focus_top = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_boathead_t;
            focus_bottom = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_boathead_b;

            m_ratio = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_speed_rate_1;
        }


        //1.延迟时间导致的 ： 船的速度(像素点距离/毫秒)×拍照延迟时间(秒*1000)
        double difdisx = (double) (pDomelinkage_item->p.pGunData->speedx * pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime * 1000 * m_ratio);
        //向右运动
        if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2) {
            focus_right += difdisx;
            focus_left += difdisx;
        } else {
            focus_left -= difdisx;
            focus_right -= difdisx;
        }

        printf("%s %d,Adjust boat postion,[%d %d %d %d]-[%d %d %d %d],difdisx:%lf,speedx:%lf,delayTime:%lf,m_ratio:%lf\n", __FUNCTION__, __LINE__, map_left, map_right, map_top, map_bottom,
                focus_left, focus_right, focus_top, focus_bottom, difdisx, pDomelinkage_item->p.pGunData->speedx, pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime, m_ratio);
        char message[512];
        sprintf(message, "%s %d,Adjust boat postion,[%d %d %d %d]-[%d %d %d %d],difdisx:%lf,speedx:%lf,delayTime:%lf,m_ratio:%lf\n", __FUNCTION__, __LINE__, map_left, map_right, map_top, map_bottom,
                focus_left, focus_right, focus_top, focus_bottom, difdisx, pDomelinkage_item->p.pGunData->speedx, pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime, m_ratio);
        MakeLog(message, 0);
    }


    //确定好目标位置后，操作框选放大：
    //    if (focus_left < 0) focus_left = 0.001;
    //    if (focus_right < 0) focus_right = 0.001;
    //    if (focus_left > 1) focus_left = 0.999;
    //    if (focus_right > 1) focus_right = 0.999;
    if (focus_top < 0) focus_top = 0.001;
    if (focus_bottom < 0) focus_bottom = 0.001;
    if (focus_top > 1) focus_top = 0.99;
    if (focus_bottom > 1) focus_bottom = 0.99;

    pDomelinkage_item->rw.m_debug_adjust_left = focus_left;
    pDomelinkage_item->rw.m_debug_adjust_right = focus_right;
    pDomelinkage_item->rw.m_debug_adjust_top = focus_top;
    pDomelinkage_item->rw.m_debug_adjust_bottom = focus_bottom;

    float curr_p = 0, curr_t = 0, curr_z = 0;
    int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
    if (ccc == -1) {
        printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
        char message[512];
        sprintf(message, "%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
        MakeLog(message, 0);
    }

    int ret = FreeWalkerInit_1(0, 1920, 1080, "/fs/project/data/config/model.dat", "/fs/project/data/config/p.dat", "/fs/project/data/config/t.dat", "/fs/project/data/config/z.dat", "");
    if (ret != 0) {
        printf("FreeWalkerInit_1 error \n");
        char message[512];
        sprintf(message, "FreeWalkerInit_1 error \n");
        MakeLog(message, 0);
    }

    int walker_left, walker_right, walker_top, walker_bottom;
    walker_left = focus_left * 1920;
    walker_right = focus_right * 1920;
    walker_top = focus_top * 1080;
    walker_bottom = focus_bottom * 1080;

    printf("walker postion:  %d %d %d %d \n", walker_left, walker_right, walker_top, walker_bottom);
    char message[512];
    sprintf(message, "walker postion:  %d %d %d %d \n", walker_left, walker_right, walker_top, walker_bottom);
    MakeLog(message, 0);
    int zoomdif = 0;
    if (m_spnap_index == 0) {
        //向右运动
        if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2) {
            zoomdif = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_zoom_dif;
        } else {
            zoomdif = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_zoom_dif;
        }
    } else if (m_spnap_index == 1) {
        //向右运动
        if (pDomelinkage_item->p.pGunData->dir == 1 || pDomelinkage_item->p.pGunData->dir == 2) {
            zoomdif = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_left_zoom_dif_1;
        } else {
            zoomdif = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_right_zoom_dif_1;
        }
    }

    int out_p, out_t, out_z;
    int outWidth = 0, outHeight = 0;
    FreeWalkerProc(0, (unsigned short) curr_p, (unsigned short) curr_t, (unsigned short) curr_z, walker_left, walker_top, walker_right, walker_bottom, zoomdif, &out_p, &out_t, &out_z, &outWidth, &outHeight);
    pDomelinkage_item->rw.m_debug_boat_height = (walker_bottom - walker_top);
    pDomelinkage_item->rw.m_debug_boat_fouce_height = outHeight;

    ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, (float) out_p, (float) out_t, (float) out_z);
    if (ret != 0) {
        printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) out_p, (float) out_t, (float) out_z);
        char message[512];
        sprintf(message, "%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) out_p, (float) out_t, (float) out_z);
        MakeLog(message, 0);
    } else {

        printf("%s %d goto the presetPTZ success (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) out_p, (float) out_t, (float) out_z);
        char message[512];
        sprintf(message, "%s %d goto the presetPTZ success (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) out_p, (float) out_t, (float) out_z);
        MakeLog(message, 0);
    }
}

/*******************************************************************************
 *                                  下一张抓拍准备工作
 * 
 * 1.计算下一张图片转向位置
 * 2.转向该位置
 * 3.居中放大操作
 * 4.延迟等待
 * 5.抓拍
 ********************************************************************************/
int domelinage_prepare_next_snap(struct Domelinkage_item * pDomelinkage_item, int w, int h) {

    //抓拍第二张图片的预备工作
    if (pDomelinkage_item->rw.v_linkage_snap_times = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - 1) {

    }

}

/*
 * 根据传过来的坐标位置，控制云台转动
 */
int domelinkage_crtl_holder(struct Domelinkage_item * pDomelinkage_item, int w, int h) {

    if (w == 0 || h == 0) {
        FsPrintf(1, "the input imagesize is error (w=%d,h=%d)\n", w, h);
        //return -1;
        w = 1920;
        h = 1080;
    }

    int ret = 0;

    ret = domelinkage_check_pGunData(pDomelinkage_item);
    if (ret == -1)
        return ret;

    int m_spnap_index = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - pDomelinkage_item->rw.v_linkage_snap_times;

    //第一次抓拍
    if (m_spnap_index == 0) {

        //如果相机没有回到预置位，先使相机回到预置位上
        if (pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[m_spnap_index] == 0) {

            //如果该该坐标不在范围内
            int index = domelinkage_chosePreset(pDomelinkage_item, w, h);
            if (index == -1) return -1;

            //记录当前标定序号
            pDomelinkage_item->rw.v_linkage_curr_calIndex = index;

            float p = 0.0, t = 0.0, z = 0.0;
            if (pDomelinkage_item->rw.v_linkage_curr_gunIndex > MAX_GUN_COUNT - 1 || pDomelinkage_item->rw.v_linkage_curr_gunIndex < 0 ||
                    pDomelinkage_item->rw.v_linkage_curr_calIndex > MAX_PRESET_COUNT - 1 || pDomelinkage_item->rw.v_linkage_curr_calIndex < 0) {
                printf("%s %d the calconfig is error !!! gunIndex:%d calIndex:%d\n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_curr_gunIndex, pDomelinkage_item->rw.v_linkage_curr_calIndex);
                char message[512];
                sprintf(message, "%s %d the calconfig is error !!! gunIndex:%d calIndex:%d\n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_curr_gunIndex, pDomelinkage_item->rw.v_linkage_curr_calIndex);
                MakeLog(message, 0);
                return -1;
            } else {
                p = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].p;
                t = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].t;
                z = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].z;
            }
            //for test
            //先获取一次ptz:
            float curr_p, curr_t, curr_z;
            int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);

            pDomelinkage_item->rw.v_linkage_checkPos_ready = 0;

            ccc = CheckPos(p, t, z, curr_p, curr_t, curr_z);
            //不相同
            if (ccc == 1) {
                ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, p, t, z);
                if (ret != 0) {
                    printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, p, t, z);
                    char message[512];
                    sprintf(message, "%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, p, t, z);
                    MakeLog(message, 0);
                } else {
                    printf("the ptz is diff,move to the obj pos,(%f %f %f)-(%f %f %f)\n", curr_p, curr_t, curr_z, p, t, z);
                    char message[512];
                    sprintf(message, "the ptz is diff,move to the obj pos,(%f %f %f)-(%f %f %f)\n", curr_p, curr_t, curr_z, p, t, z);
                    MakeLog(message, 0);
                }
            } else {
                pDomelinkage_item->rw.v_linkage_checkPos_ready = 100;
                printf("the ptz is same,do not move to the obj pos,(%f %f %f)\n", curr_p, curr_t, curr_z);
                char message[512];
                sprintf(message, "the ptz is same,do not move to the obj pos,(%f %f %f)\n", curr_p, curr_t, curr_z);
                MakeLog(message, 0);
            }

            pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[m_spnap_index] = 1;
            pDomelinkage_item->rw.v_linkage_gotoPTZ_startTime[m_spnap_index] = fs_time_uptime_get();
            pDomelinkage_item->rw.v_linkage_checkPTZ_startTime = fs_time_uptime_get();
        }

        //相机转向指令已经发送，这里根据获取ptz检测相机是否转动到位
        if (pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[m_spnap_index] == 1) {
            //半秒钟check一次相机ptz
            if (fs_time_uptime_get() - pDomelinkage_item->rw.v_linkage_checkPTZ_startTime > 0.3) {

                float p = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].p;
                float t = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].t;
                float z = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].z;

                float curr_p, curr_t, curr_z;
                int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
                if (ccc == 0) {

                    ccc = CheckPos(p, t, z, curr_p, curr_t, curr_z);
                }

                if (ccc == 0) {
                    pDomelinkage_item->rw.v_linkage_checkPos_ready++;
                    printf("the holder move to the right pos[index:%d](%f %f %f)-(%f %f %f)\n", pDomelinkage_item->rw.v_linkage_curr_calIndex, p, t, z, curr_p, curr_t, curr_z);
                    char message[512];
                    sprintf(message, "the holder move to the right pos[index:%d](%f %f %f)-(%f %f %f)\n", pDomelinkage_item->rw.v_linkage_curr_calIndex, p, t, z, curr_p, curr_t, curr_z);
                    MakeLog(message, 0);
                }
                printf("check curr ptz=(%f %f %f)-%d \n", curr_p, curr_t, curr_z, pDomelinkage_item->rw.v_linkage_checkPos_ready);
                char message[512];
                sprintf(message, "check curr ptz=(%f %f %f)-%d \n", curr_p, curr_t, curr_z, pDomelinkage_item->rw.v_linkage_checkPos_ready);
                MakeLog(message, 0);
                pDomelinkage_item->rw.v_linkage_checkPTZ_startTime = fs_time_uptime_get();
            }
        }

        //发起框选居中放大指令
        if (pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[m_spnap_index] == 1) {
            double diftime = fs_time_uptime_get() - pDomelinkage_item->rw.v_linkage_gotoPTZ_startTime[m_spnap_index];
            if (diftime > pDomelinkage_item->ro.m_dome_alg_config.m_linkage_focusRect_delayTime || pDomelinkage_item->rw.v_linkage_checkPos_ready >= 2) {

                //调用框选放大功能
                double temp_left = 0, temp_right = 0, temp_top = 0, temp_bottom = 0; //校准坐标
                double map_left = 0, map_right = 0, map_top = 0, map_bottom = 0; //用于框选放大的坐标

                temp_left = pDomelinkage_item->p.pGunData->left;
                temp_right = pDomelinkage_item->p.pGunData->right;
                temp_top = pDomelinkage_item->p.pGunData->top;
                temp_bottom = pDomelinkage_item->p.pGunData->bottom;

                int index = pDomelinkage_item->rw.v_linkage_curr_calIndex;

                //坐标映射(枪机映射到球机)
                double m_debug[10];
                ret = AlgMapping_1(pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[0], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[1],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[2], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[3],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[4], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[5],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[6], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[7],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[0], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[1],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[2], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[3],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[4], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[5],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[6], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[7],
                        temp_left, temp_top, temp_right, temp_bottom, w, h, &map_left, &map_top, &map_right, &map_bottom, m_debug);

                pDomelinkage_item->rw.m_debug_number = pDomelinkage_item->p.pGunData->index;
                pDomelinkage_item->rw.m_debug_left = map_left;
                pDomelinkage_item->rw.m_debug_right = map_right;
                pDomelinkage_item->rw.m_debug_top = map_top;
                pDomelinkage_item->rw.m_debug_bottom = map_bottom;

                printf("%s %d AlgMapping[index:%d] (%lf %lf %lf %lf)-((%lf %lf %lf %lf))\n", __FUNCTION__, __LINE__, index,
                        temp_left, temp_top, temp_right, temp_bottom, map_left, map_top, map_right, map_bottom);
                char message[512];
                sprintf(message, "%s %d AlgMapping[index:%d] (%lf %lf %lf %lf)-((%lf %lf %lf %lf))\n", __FUNCTION__, __LINE__, index,
                        temp_left, temp_top, temp_right, temp_bottom, map_left, map_top, map_right, map_bottom);
                MakeLog(message, 0);
                //框选放大操作
                domelinkage_walker_focusRect(pDomelinkage_item, w, h, map_left, map_right, map_top, map_bottom);

                pDomelinkage_item->rw.v_linkage_focusRect_flag[m_spnap_index] = 1;
                pDomelinkage_item->rw.v_linkage_focusRect_startTime[m_spnap_index] = fs_time_uptime_get();
                pDomelinkage_item->rw.m_debug_snapflag[m_spnap_index] = 1;

                //重置
                pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[m_spnap_index] = 0;
                pDomelinkage_item->rw.v_linkage_gotoPTZ_startTime[m_spnap_index] = 0.0;
                pDomelinkage_item->rw.v_linkage_checkPos_ready = 0;
                pDomelinkage_item->rw.v_linkage_checkPos_unready = 0;
            }
        }
    }//第二张
    else if (m_spnap_index == 1) {

        double map_left = 0, map_right = 0, map_top = 0, map_bottom = 0; //用于框选放大的坐标

        //框选放大操作
        domelinkage_walker_focusRect(pDomelinkage_item, w, h, map_left, map_right, map_top, map_bottom);

        pDomelinkage_item->rw.v_linkage_focusRect_flag[m_spnap_index] = 1;
        pDomelinkage_item->rw.v_linkage_focusRect_startTime[m_spnap_index] = fs_time_uptime_get();
        pDomelinkage_item->rw.m_debug_snapflag[m_spnap_index] = 1;

        //重置
        pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[m_spnap_index] = 0;
        pDomelinkage_item->rw.v_linkage_gotoPTZ_startTime[m_spnap_index] = 0.0;
        pDomelinkage_item->rw.v_linkage_checkPos_ready = 0;
        pDomelinkage_item->rw.v_linkage_checkPos_unready = 0;

    }

    return ret;
}

void domelinkage_saveResult(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    int len1 = strlen(pDomelinkage_item->rw.m_domeparamcfg.deviceno) + 1;
    len1 += strlen(pDomelinkage_item->rw.m_domeparamcfg.devicename) + 1;
    len1 += strlen(pDomelinkage_item->rw.m_domeparamcfg.position_jwd) + 1;
    len1 += strlen(pDomelinkage_item->rw.m_domeparamcfg.area_jwd) + 1;
    len1 += strlen(pDomelinkage_item->rw.m_domeparamcfg.video_path) + 1;
    len1 += strlen(pDomelinkage_item->rw.m_domeparamcfg.real_url) + 1;

    struct Record_item_Inc * pRecord_Inc_Item = (struct Record_item_Inc*) fsMalloc(sizeof (struct Record_item_Inc) +len1 + 4);
    memset(pRecord_Inc_Item, 0, sizeof ( struct Record_item_Inc));

    pRecord_Inc_Item->_type = 0x10;

    pRecord_Inc_Item->_starttime = pDomelinkage_item->rw.v_linkage_incStartTime;
    pRecord_Inc_Item->_endtime = pDomelinkage_item->rw.v_linkage_incEndTime;


    int i = 0;

    //清空数据列表
    for (i = 0; i < 5; i++) {
        pRecord_Inc_Item->_ppFrame[i] = NULL;
    }

    //添加图片数据帧
    for (i = 0; i < 5; i++) {
        if (i >= pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber) {
            pRecord_Inc_Item->_ppFrame[i] = NULL;
            //fs_Object_addRefer_pthreadSafety(pDomelinkage_item->rw._ppFrame[pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - 1], __LINE__);
            //pRecord_Inc_Item->_ppFrame[i] = pDomelinkage_item->rw._ppFrame[pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - 1];
        } else {
            pRecord_Inc_Item->_ppFrame[i] = pDomelinkage_item->rw._ppFrame[i];
            printf("add the pFrame,_ppFrame[%d]:%x \n", i, pDomelinkage_item->rw._ppFrame[i]);
        }
    }


    //清空...
    for (i = 0; i < 5; i++) {
        pDomelinkage_item->rw._ppFrame[i] = NULL;
    }

    char* pd = (char*) (pRecord_Inc_Item + 1);
    pRecord_Inc_Item->trigger.qqld.ipaddr = pDomelinkage_item->rw.m_domeparamcfg.gun_camera_ip;
    pRecord_Inc_Item->trigger.qqld.deviceno = pd, memcpy(pd, pDomelinkage_item->rw.m_domeparamcfg.deviceno, strlen(pDomelinkage_item->rw.m_domeparamcfg.deviceno) + 1), pd += strlen(pDomelinkage_item->rw.m_domeparamcfg.deviceno) + 1;
    pRecord_Inc_Item->trigger.qqld.devicename = pd, memcpy(pd, pDomelinkage_item->rw.m_domeparamcfg.devicename, strlen(pDomelinkage_item->rw.m_domeparamcfg.devicename) + 1), pd += strlen(pDomelinkage_item->rw.m_domeparamcfg.devicename) + 1;
    pRecord_Inc_Item->trigger.qqld.position_jwd = pd, memcpy(pd, pDomelinkage_item->rw.m_domeparamcfg.position_jwd, strlen(pDomelinkage_item->rw.m_domeparamcfg.position_jwd) + 1), pd += strlen(pDomelinkage_item->rw.m_domeparamcfg.position_jwd) + 1;
    pRecord_Inc_Item->trigger.qqld.incVideo = pd, memcpy(pd, pDomelinkage_item->rw.m_domeparamcfg.video_path, strlen(pDomelinkage_item->rw.m_domeparamcfg.video_path) + 1), pd += strlen(pDomelinkage_item->rw.m_domeparamcfg.video_path) + 1;
    pRecord_Inc_Item->trigger.qqld.realurl = pd, memcpy(pd, pDomelinkage_item->rw.m_domeparamcfg.real_url, strlen(pDomelinkage_item->rw.m_domeparamcfg.real_url) + 1), pd += strlen(pDomelinkage_item->rw.m_domeparamcfg.real_url) + 1;
    pRecord_Inc_Item->trigger.qqld.area_jwd = pd, memcpy(pd, pDomelinkage_item->rw.m_domeparamcfg.area_jwd, strlen(pDomelinkage_item->rw.m_domeparamcfg.area_jwd) + 1); //, pd += strlen(pDomelinkage_item->rw.m_domeparamcfg.area_jwd)+1;
    pRecord_Inc_Item->trigger.qqld.direction = pDomelinkage_item->rw.m_domeparamcfg.obj_dir;

    printf("%s %d , record a inc success(area_jwd:%s) direction:%d \n", __FUNCTION__, __LINE__, pRecord_Inc_Item->trigger.qqld.area_jwd, pRecord_Inc_Item->trigger.qqld.direction);
    char message[512];
    sprintf(message, "%s %d , record a inc success(area_jwd:%s) \n", __FUNCTION__, __LINE__, pRecord_Inc_Item->trigger.qqld.area_jwd);
    MakeLog(message, 0);

    pDomelinkage_item->p.record_inc_item_add_partVideo__OI_2(pDomelinkage_item->p.pRecord_item, pRecord_Inc_Item);
}

/*
 * 拍照结束回复枪机.这里不需要回复拍照图片
 * int flag: 0 表示拍照成功，返回图片 ; 非零:表示拍照失败，返回错误代码；
 */
void domelinkage_reply2gun(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame *pFrame, int ret) {

    ret = 1; //此处设置为1，表示球机不向枪机发送jpg数据，jpg数据保存在球机所在服务器

    //拍照之前的动作没有出错
    if (ret == 0) {

        double t1 = fs_time_uptime_get();
        FsObjectImageRGB * const pRGB = (FsObjectImageRGB*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RGB_0);
        unsigned char* pjpgbuffer = (unsigned char*) fsMalloc(1024 * 1024 * 1);
        int jpglen = image_jpeg_compress(pjpgbuffer, 1024 * 1024 * 1, pRGB->data, pFrame->width[0], pFrame->height[0], pFrame->width[0]*3, 90, 1, 1);
        if (jpglen <= 0) {
            jpglen = image_jpeg_compress(pjpgbuffer, 1024 * 1024 * 1, pRGB->data, pFrame->width[0], pFrame->height[0], pFrame->width[0]*3, 90, 1, 1);
            FsPrintf(1, "image_jpeg_compress failed(jpglen=%d) \n", jpglen);
        }
        fs_Object_delete_pthreadSafety__OI(pRGB);
        if (jpglen > 0) {
            double t2 = fs_time_uptime_get();
            if (t2 - t1 > 0.04)
                FsPrintf(1, "image_jpeg_compress cost long time(0.041)  %lf \n", t2 - t1);

            if (0) {
                FILE* f = fopen("/fs/project/data/hello.jpg", "wb");
                if (f != NULL) {
                    fwrite(pjpgbuffer, jpglen, 1, f);
                    fclose(f);
                }
            }

            printf("%s %d : dome ready snap a pic(obj_num:%d jpglen:%d)\n", __FUNCTION__, __LINE__, pDomelinkage_item->p.pGunData->index, jpglen);

            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            *(long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "snap", FsEbmlNodeType_Integer)->data.buffer = 0;
            *(long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "number", FsEbmlNodeType_Integer)->data.buffer = pDomelinkage_item->p.pGunData->index;
            *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "left", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->left;
            *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "right", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->right;
            *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "top", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->top;
            *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "bottom", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->bottom;

            struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "image_byte", FsEbmlNodeType_Binary);
            fs_Ebml_node_data_set__OI_3(pEbml_node, jpglen, (char *) pjpgbuffer);

            FsString* pSenddata = fs_Ebml_to_String__IO(rst, 8, 12, NULL);
            fs_Ebml_delete__OI(rst, NULL);

            int buflen = pSenddata->lenth - 20;
            pSenddata->lenth -= 12;

            *((unsigned int *) pSenddata->buffer) = pDomelinkage_item->p.pGunData->data[3], *((unsigned int *) (pSenddata->buffer + 4)) = buflen;
            pSenddata->buffer[0] &= 0xF0;
            pSenddata->buffer[0] |= 0x03;
            memcpy(pSenddata->buffer + buflen + 8, pDomelinkage_item->p.pGunData->data, sizeof (unsigned int)*3);
            configManager_connect_send__OI_2__old(pSenddata);
        } else {
            ret = 100;
            FsPrintf(1, "image_jpeg_compress failed(jpglen=%d) again ! \n", jpglen);
        }
    }

    if (ret != 0) {
        //发送错误信息给枪机
        FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);

        *(long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "snap", FsEbmlNodeType_Integer)->data.buffer = ret;
        *(long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "number", FsEbmlNodeType_Integer)->data.buffer = pDomelinkage_item->p.pGunData->index;
        *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "left", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->left;
        *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "right", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->right;
        *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "top", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->top;
        *(double*) fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "bottom", FsEbmlNodeType_Float)->data.buffer = pDomelinkage_item->p.pGunData->bottom;

        //struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "image_byte", FsEbmlNodeType_Binary);
        //fs_Ebml_node_data_set__OI_3(pEbml_node, jpglen, (char *) pjpgbuffer);

        FsString* pSenddata = fs_Ebml_to_String__IO(rst, 8, 12, NULL);
        fs_Ebml_delete__OI(rst, NULL);

        int buflen = pSenddata->lenth - 20;
        pSenddata->lenth -= 12;

        *((unsigned int *) pSenddata->buffer) = pDomelinkage_item->p.pGunData->data[3], *((unsigned int *) (pSenddata->buffer + 4)) = buflen;
        pSenddata->buffer[0] &= 0xF0;
        pSenddata->buffer[0] |= 0x03;
        memcpy(pSenddata->buffer + buflen + 8, pDomelinkage_item->p.pGunData->data, sizeof (unsigned int)*3);
        configManager_connect_send__OI_2__old(pSenddata);

        //printf("%s %d send the error data:%d \n", __FUNCTION__, __LINE__, ret);
    }
}

/*
 * 重置参数
 * 
 * flag：1： domelinage_finish_snap()结束拍照后调用
 */
void resetParam(struct Domelinkage_item * pDomelinkage_item, int flag) {

    if (flag == 1) {

        pDomelinkage_item->rw.v_linkage_checkPos_ready = 0;
        pDomelinkage_item->rw.v_linkage_checkPos_unready = 0;

        pDomelinkage_item->rw.v_linkage_curr_calIndex = 0;

        pDomelinkage_item->rw.v_linkage_checkPTZ_startTime = 0.0;
        int i = 0;
        for (i = 0; i < SNAP_PIC_COUNT; i++) {
            pDomelinkage_item->rw.v_linkage_gotoPTZ_flag[i] = 0;
            pDomelinkage_item->rw.v_linkage_gotoPTZ_startTime[i] = 0.0;


            pDomelinkage_item->rw.v_linkage_focusRect_flag[i] = 0;
            pDomelinkage_item->rw.v_linkage_focusRect_startTime[i] = 0.0;

            pDomelinkage_item->rw.v_linkage_snap_finish[i] = 0;
        }




        pDomelinkage_item->rw.v_linkage_snap_times = 0;

        pDomelinkage_item->rw.v_linkage_gobackPTZ_flag = 0;
        pDomelinkage_item->rw.v_linkage_snap_lastTime = 0.0;
        pDomelinkage_item->rw.v_linkage_snap_intervalTime = 0.0;
    }
}

/*
 * 拍照后，结束工作
 * 
 * 要等到相机转回预置位后，再结束
 */
void domelinage_finish_snap(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame *pFrame, ConfigManager * pConfigManager) {


    //框选放大操作完成，且拍照张数为0，表示拍照工作已经结束
    if (pDomelinkage_item->rw.v_linkage_snap_times == 0 && pDomelinkage_item->rw.v_linkage_snap_finish[pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - 1] == 1) {

        //球机回到预置位
        if (pDomelinkage_item->rw.v_linkage_gobackPTZ_flag == 0) {

            domelinkage_goback_PrePTZ(pDomelinkage_item);

            pDomelinkage_item->rw.v_linkage_checkPos_unready = 0;
            pDomelinkage_item->rw.v_linkage_checkPos_ready = 0;
            pDomelinkage_item->rw.v_linkage_gobackPTZ_flag = 1;
            pDomelinkage_item->rw.v_linkage_checkPTZ_startTime = fs_time_uptime_get();

            printf("%s %d snap finished, move camera goback prePTZ(time:%lf) \n", __FUNCTION__, __LINE__, fs_time_uptime_get());
            char message[512];
            sprintf(message, "%s %d snap finished, move camera goback prePTZ(time:%lf) \n", __FUNCTION__, __LINE__, fs_time_uptime_get());
            MakeLog(message, 0);
        }

        if (pDomelinkage_item->rw.v_linkage_gobackPTZ_flag == 1) {

            if (fs_time_uptime_get() - pDomelinkage_item->rw.v_linkage_checkPTZ_startTime > 0.3) {

                float p = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].p;
                float t = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].t;
                float z = pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[pDomelinkage_item->rw.v_linkage_curr_calIndex].z;

                float curr_p, curr_t, curr_z;
                int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
                if (ccc == 0) {
                    if (pDomelinkage_item->rw.v_linkage_curr_calIndex < 0) pDomelinkage_item->rw.v_linkage_curr_calIndex = 0;
                    ccc = CheckPos(p, t, z, curr_p, curr_t, curr_z);
                }

                if (ccc == 0) {
                    pDomelinkage_item->rw.v_linkage_checkPos_ready++;
                } else {
                    pDomelinkage_item->rw.v_linkage_checkPos_unready++;
                }

                printf("%s %d  check curr ptz=(%f %f %f)-(%f %f %f)-_ready:%d,unready:%d  v_linkage_curr_calIndex:%d \n", __FUNCTION__, __LINE__, curr_p, curr_t, curr_z,
                        p, t, z, pDomelinkage_item->rw.v_linkage_checkPos_ready, pDomelinkage_item->rw.v_linkage_checkPos_unready, pDomelinkage_item->rw.v_linkage_curr_calIndex);
                char message[512];
                sprintf(message, "%s %d  check curr ptz=(%f %f %f)-(%f %f %f)-_ready:%d,unready:%d  v_linkage_curr_calIndex:%d \n", __FUNCTION__, __LINE__, curr_p, curr_t, curr_z,
                        p, t, z, pDomelinkage_item->rw.v_linkage_checkPos_ready, pDomelinkage_item->rw.v_linkage_checkPos_unready, pDomelinkage_item->rw.v_linkage_curr_calIndex);
                MakeLog(message, 0);

                pDomelinkage_item->rw.v_linkage_checkPTZ_startTime = fs_time_uptime_get();
            }


            if (pDomelinkage_item->rw.v_linkage_checkPos_unready > 30) {
                pDomelinkage_item->rw.v_linkage_checkPos_unready = 0;
                domelinkage_goback_PrePTZ(pDomelinkage_item);
                printf("%s %d the camera did not goback prePTZ for long time, send the command again...\n", __FUNCTION__, __LINE__);
                char message[512];
                sprintf(message, "%s %d the camera did not goback prePTZ for long time, send the command again...\n", __FUNCTION__, __LINE__);
                MakeLog(message, 0);
            }

            //转动到位，处理相关结束工作
            //如果联动次数还未结束，继续向球机发起获取目标的请求，然后再来一次联动操作。
            if (pDomelinkage_item->rw.v_linkage_checkPos_ready > 2 && pDomelinkage_item->rw.v_linkage_times > 0) {

                pDomelinkage_item->rw.v_linkage_times--;
                pDomelinkage_item->p.pGunData->sendagain = 1;
                //重置参数
                resetParam(pDomelinkage_item, 1);
                printf("%s %d  this linkage is over, but need more linkage,v_linkage_times:%d..... \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_times);
                char message[512];
                sprintf(message, "%s %d  this linkage is over, but need more linkage,v_linkage_times:%d..... \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.v_linkage_times);
                MakeLog(message, 0);
            }

            //球机回到预置位，且联动次数结束，这代表整个事件真正结束
            if (pDomelinkage_item->rw.v_linkage_checkPos_ready > 2 && pDomelinkage_item->rw.v_linkage_times == 0) {

                //获取事件结束时间
                pDomelinkage_item->rw.v_linkage_incEndTime = fs_time_uptime_get();
                //将缓存的数据发送给记录模块做存储处理
                domelinkage_saveResult(pDomelinkage_item, pFrame);
                //向枪机回执信息
                domelinkage_reply2gun(pDomelinkage_item, pFrame, 0);
                //从缓存队列中移除该数据
                fs_ObjectList_remove_head_pthreadSafety(pDomelinkage_item->ro.__gunDataClientList);
                /* 释放原来保存的连接,必须是原来保存过的连接,否则会死机 */
                if (pDomelinkage_item->p.pGunData->data != NULL)
                    configManager_connect_release(pConfigManager, pDomelinkage_item->p.pGunData->data, 0);
                fsFree(pDomelinkage_item->p.pGunData);
                pDomelinkage_item->p.pGunData = NULL;
                //重置参数
                resetParam(pDomelinkage_item, 1);

                //测试：保存图片
                if (0) {

                    domelinkage_save_video(pDomelinkage_item);
                    //先获取一次ptz:
                    float curr_p, curr_t, curr_z;
                    int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);

                    int w = pFrame->width[0];
                    int h = pFrame->height[0];

                    int diftime = (int) ((fs_time_uptime_get() - pDomelinkage_item->p.pGunData->thistime)*1000);
                    char filepath[512];
                    sprintf(filepath, "/fs/project/data/temp/dome_%d_%d_%f_%f_%f.bmp", pDomelinkage_item->rw.m_debug_number, pFrame->index, curr_p, curr_t, curr_z);
                    ImageDib* pdib = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
                    memcpy(pdib->data, pFrame->data.raw[0]->data, w * h);
                    image_dib_svae_to_file(pdib, filepath);

                    image_dib_delete__OI(&pdib);
                }

                printf("%s %d  the incident finished !!   Reset all param... (time:%lf)..... \n", __FUNCTION__, __LINE__, fs_time_uptime_get());
                char message[512];
                sprintf(message, "%s %d  the incident finished !!   Reset all param... (time:%lf)..... \n", __FUNCTION__, __LINE__, fs_time_uptime_get());
                MakeLog(message, 0);
            }
        }
    }
}

/*
 * 负责快照工作
 */
void domelinage_snap_boat(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    //相机已经调用框选放大指令
    int m_spnap_index = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - pDomelinkage_item->rw.v_linkage_snap_times;

    //第一次抓拍
    if (m_spnap_index == 0) {
        if (pDomelinkage_item->rw.v_linkage_focusRect_flag[m_spnap_index] == 1) {
            //第一次抓拍:云台转动后，延迟2秒
            //后续几次抓拍：当前时间比上次抓拍时间间隔m_linkage_snap_intervalTime
            if (fs_time_uptime_get() - pDomelinkage_item->rw.v_linkage_focusRect_startTime[m_spnap_index] > pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snap_delayTime &&
                    pDomelinkage_item->p.pGunData != NULL) {

                //缓存数据帧
                fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
                pDomelinkage_item->rw._ppFrame[m_spnap_index] = pFrame;

                pDomelinkage_item->rw.v_linkage_snap_finish[m_spnap_index] = 1;
                //记录拍照时间，
                pDomelinkage_item->rw.v_linkage_snap_lastTime = fs_time_uptime_get();
                //抓拍张数减一
                pDomelinkage_item->rw.v_linkage_snap_times--;

                printf("%s %d, snap a picture,snap_times:%d, _ppFrame[%d]:%x \n", __FUNCTION__, __LINE__,
                        pDomelinkage_item->rw.v_linkage_snap_times, m_spnap_index, pDomelinkage_item->rw._ppFrame[m_spnap_index]);
                char message[512];
                sprintf(message, "%s %d, snap a picture,snap_times:%d v_linkage_snap_finish[%d]:%d \n", __FUNCTION__, __LINE__,
                        pDomelinkage_item->rw.v_linkage_snap_times, m_spnap_index, pDomelinkage_item->rw.v_linkage_snap_finish[m_spnap_index]);
                MakeLog(message, 0);

            }
        }
    } else if (m_spnap_index > 0) {

        //后续几张图片        
        if (pDomelinkage_item->rw.v_linkage_focusRect_flag[m_spnap_index] == 1) {

            if (fs_time_uptime_get() - pDomelinkage_item->rw.v_linkage_focusRect_startTime[m_spnap_index] > pDomelinkage_item->ro.m_dome_alg_config.m_linkage_sampleTime &&
                    pDomelinkage_item->p.pGunData != NULL) {

                //缓存数据帧
                fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
                pDomelinkage_item->rw._ppFrame[m_spnap_index] = pFrame;

                pDomelinkage_item->rw.v_linkage_snap_finish[m_spnap_index] = 1;
                //记录拍照时间，
                pDomelinkage_item->rw.v_linkage_snap_lastTime = fs_time_uptime_get();
                //抓拍张数减一
                pDomelinkage_item->rw.v_linkage_snap_times--;

                printf("%s %d, snap a picture,snap_times:%d _ppFrame[%d]:%x \n", __FUNCTION__, __LINE__,
                        pDomelinkage_item->rw.v_linkage_snap_times, m_spnap_index, pDomelinkage_item->rw._ppFrame[m_spnap_index]);
                char message[512];
                sprintf(message, "%s %d, snap a picture,snap_times:%d v_linkage_snap_finish[%d]:%d \n", __FUNCTION__, __LINE__,
                        pDomelinkage_item->rw.v_linkage_snap_times, m_spnap_index, pDomelinkage_item->rw.v_linkage_snap_finish[m_spnap_index]);
                MakeLog(message, 0);
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//调试工作

void test2(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    int pos[4];
    //读取坐标
    {
        FILE* f = fopen("/fs/project/data/temp/pos.txt", "r");
        if (NULL == f) {
            printf("Can not open file Data.txt!\n");
            return;
        }

        int i = 0;
        char ln[80];
        while (1) {
            if (NULL == fgets(ln, 80, f)) break;
            pos[i] = atoi(ln);
            i++;
        }
        fclose(f);
    }

    int w = 1920;
    int h = 1080;

    double map_left = pos[0] / (double) w;
    double map_top = pos[1] / (double) h;
    double map_right = pos[2] / (double) w;
    double map_bottom = pos[3] / (double) h;

    /////////////////////////////////////////////////
    //枪球映射     
    if (0) {
        double temp_left = pos[0] / (double) w;
        double temp_top = pos[1] / (double) h;
        double temp_right = pos[2] / (double) w;
        double temp_bottom = pos[3] / (double) h;

        printf("1 %lf %lf %lf %lf \n", temp_left, temp_top, temp_right, temp_bottom);
        int index = 0;
        //坐标映射(枪机映射到球机)
        double m_debug[10];
        int ret = AlgMapping_1(pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[0], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[1],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[2], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[3],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[4], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[5],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[6], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[7],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[0], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[1],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[2], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[3],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[4], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[5],
                pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[6], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[7],
                temp_left, temp_top, temp_right, temp_bottom, w, h, &map_left, &map_top, &map_right, &map_bottom, m_debug);


        printf("2 %lf %lf %lf %lf \n", map_left, map_top, map_right, map_bottom);
        //            printf("%s %d AlgMapping[index:%d] (%lf %lf %lf %lf)-((%lf %lf %lf %lf))\n", __FUNCTION__, __LINE__, index,
        //                temp_left, temp_top, temp_right, temp_bottom, map_left, map_top, map_right, map_bottom);
    }
    ////////////////////////////////////////////////


    map_left *= w;
    map_top *= h;
    map_right *= w;
    map_bottom *= h;

    printf("mapping: (%d %d %d %d)-(%d %d %d %d) \n", (int) map_left, (int) map_top, (int) map_right, (int) map_bottom, pos[0], pos[1], pos[2], pos[3]);

    float curr_p = 0, curr_t = 0, curr_z = 0;
    int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
    if (ccc == -1) {
        printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
    }

    int ret = FreeWalkerInit_1(0, 1920, 1080, "/fs/project/data/config/model.dat", "/fs/project/data/config/p.dat", "/fs/project/data/config/t.dat", "/fs/project/data/config/z.dat", "");
    if (ret != 0) {
        printf("FreeWalkerInit_1 error \n");
    }

    int out_p, out_t, out_z;
    int outWidth = 0, outHeight = 0;
    FreeWalkerProc(0, (unsigned short) curr_p, (unsigned short) curr_t, (unsigned short) curr_z, (int) map_left, (int) map_top, (int) map_right, (int) map_bottom, 0, &out_p, &out_t, &out_z, &outWidth, &outHeight);

    ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, (float) out_p, (float) out_t, (float) out_z);
    if (ret != 0) {
        printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) out_p, (float) out_t, (float) out_z);
    }
}

/*
unsigned short m_data[3310];
void test1(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {
    static int number = 0;
    static int delay = 0;
    if (number == 0) {

        FILE* f = fopen("/fs/project/data/temp/data.txt", "r");
        if (NULL == f) {
            printf("Can not open file Data.txt!\n");
            return;
        }

        int i = 0;
        char ln[80];
        while (1) {
            if (NULL == fgets(ln, 80, f)) break;
            m_data[i] = atoi(ln);
            i++;
        }
        fclose(f);
    }

    if (number >= 3306)
        return;


    if (delay % 20 == 0) {
        static float outP = 0, outT = 0, outZ = 0;
        outP = m_data[number];
        outT = 88;
        outZ = 16;

        int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, outP, outT, outZ);
        if (ret != 0) {
            printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, outP, outT, outZ);
        }

        number++;
    }

    delay++;
}
 */

void testRecordInc(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {


    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);
    fs_Object_addRefer_pthreadSafety(pFrame, __LINE__);


    char *qjVideo = (char *) fsMalloc(20);
    qjVideo[0] = 0;
    char *qjRealurl = (char *) fsMalloc(30);
    qjRealurl[0] = 0;
    struct Record_item_Inc * pRecord_Inc_Item = (struct Record_item_Inc*) fsMalloc(sizeof (struct Record_item_Inc)+strlen(qjVideo) + 1 + strlen(qjRealurl) + 1);
    memset(pRecord_Inc_Item, 0, sizeof (pRecord_Inc_Item));

    pRecord_Inc_Item->_type = 0x10;
    pRecord_Inc_Item->_starttime = fs_time_GMT_get() - 10;
    pRecord_Inc_Item->_endtime = fs_time_GMT_get();

    pRecord_Inc_Item->_ppFrame[0] = pFrame;
    pRecord_Inc_Item->_ppFrame[1] = pFrame;
    pRecord_Inc_Item->_ppFrame[2] = pFrame;
    pRecord_Inc_Item->_ppFrame[3] = pFrame;
    pRecord_Inc_Item->_ppFrame[4] = pFrame;


    char* pd = (char*) (pRecord_Inc_Item + 1);
    pRecord_Inc_Item->trigger.qqld.ipaddr = 0;
    pRecord_Inc_Item->trigger.qqld.deviceno = "devicenott";
    pRecord_Inc_Item->trigger.qqld.devicename = "devicenamett";

    pRecord_Inc_Item->trigger.qqld.position_jwd = "10.6,30.1";

    pRecord_Inc_Item->trigger.qqld.incVideo = pd, memcpy(pd, qjVideo, strlen(qjVideo)), pd += strlen(qjVideo);
    fsFree(qjVideo);

    pRecord_Inc_Item->trigger.qqld.realurl = pd, memcpy(pd, qjRealurl, strlen(qjRealurl));
    fsFree(qjRealurl);

    pRecord_Inc_Item->trigger.qqld.area_jwd = "(10.6,30.1)(12.6,32.1)(15.6,38.1)";

    pDomelinkage_item->p.record_inc_item_add_partVideo__OI_2(pDomelinkage_item->p.pRecord_item, pRecord_Inc_Item);
}

//统计有效值P

/*
 * return 0:还未结束，return:1 结束
 * 
 * return -1: 配置参数不对
 * return -2: 相机控制失败
 */
int testP(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    //    if (!access("/fs/project/data/config/p.dat", F_OK)) {
    //        return 1;
    //    }

    if (pDomelinkage_item->rw.m_configuration_file.get_p_init == 0) {

        pDomelinkage_item->rw.m_configuration_file.get_start = 0;
        pDomelinkage_item->rw.m_configuration_file.get_end = 13722;
        char buf[256];
        if (!access("/fs/project/data/makeconfig/param.dat", F_OK)) {
            if (ReadProfileStr("P", "startP", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_start);
            }
        }
        if (!access("/fs/project/data/makeconfig/param.dat", F_OK)) {
            if (ReadProfileStr("P", "endP", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_end);
            }
        }

        //参数不对
        if (pDomelinkage_item->rw.m_configuration_file.get_start < 0 || pDomelinkage_item->rw.m_configuration_file.get_end < 0 ||
                pDomelinkage_item->rw.m_configuration_file.get_start > pDomelinkage_item->rw.m_configuration_file.get_end) {
            printf("the input param is error: startP: %d endP:%d \n", pDomelinkage_item->rw.m_configuration_file.get_start, pDomelinkage_item->rw.m_configuration_file.get_end);
            return -1;
        }


        pDomelinkage_item->rw.m_configuration_file.get_index = pDomelinkage_item->rw.m_configuration_file.get_start;
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;
        pDomelinkage_item->rw.m_configuration_file.get_p_init = 1;
    }

    //完成
    if (pDomelinkage_item->rw.m_configuration_file.get_index > pDomelinkage_item->rw.m_configuration_file.get_end) {
        pDomelinkage_item->rw.m_configuration_file.get_p_done = 1;
        return 1;
    }

    static float outP = 0, outT = 0, outZ = 0;

    if (pDomelinkage_item->rw.m_configuration_file.get_delay == 0) {
        outP = pDomelinkage_item->rw.m_configuration_file.get_index;
        outT = 0;
        outZ = 16;

        int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, outP, outT, outZ);
        if (ret != 0) {
            printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, outP, outT, outZ);
            return -2;
        }
    }

    pDomelinkage_item->rw.m_configuration_file.get_delay++;

    float curr_p = 0, curr_t = 0, curr_z = 0;

    int nnnn = 80;
    if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
        nnnn = 80;
        //printf("%s %d, get P up data, startP:%d endP:%d \n", __FUNCTION__, __LINE__, startP, endP);
    } else {
        nnnn = 20;
    }

    if (pDomelinkage_item->rw.m_configuration_file.get_delay > nnnn) {
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;

        int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
        if (ccc == -1) {
            printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
            return -2;
        }

        printf(" curr_p:%f outP:%f \n", curr_p, outP);

        if (outP == curr_p) {

            //删除旧有的
            if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
                if (!access("/fs/project/data/config/p.dat", F_OK))
                    remove("/fs/project/data/config/p.dat");
            }

            FILE* f = fopen("/fs/project/data/config/p.dat", "a+");
            if (f) {
                char info[28];
                printf("write:%d \n", pDomelinkage_item->rw.m_configuration_file.get_index);
                sprintf(info, "%d\r\n", pDomelinkage_item->rw.m_configuration_file.get_index);
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
        }

        pDomelinkage_item->rw.m_configuration_file.get_index++;
    }

    return 0;
}

//统计有效值P

/*
 * 向上,递减
 */
int testT_u(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    //    if (!access("/fs/project/data/config/t.dat", F_OK)) {
    //        return 1;
    //    }

    if (pDomelinkage_item->rw.m_configuration_file.get_t_u_init == 0) {

        pDomelinkage_item->rw.m_configuration_file.get_start = 13722;
        pDomelinkage_item->rw.m_configuration_file.get_end = 13571;

        char buf[256];
        if (ReadProfileStr("T_U", "startT", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_start);
        }
        if (ReadProfileStr("T_U", "endT", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_end);
        }

        if (pDomelinkage_item->rw.m_configuration_file.get_start < 0 || pDomelinkage_item->rw.m_configuration_file.get_end < 0 || pDomelinkage_item->rw.m_configuration_file.get_start < pDomelinkage_item->rw.m_configuration_file.get_end) {
            return -1;
        }

        pDomelinkage_item->rw.m_configuration_file.get_index = pDomelinkage_item->rw.m_configuration_file.get_start;
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;
        pDomelinkage_item->rw.m_configuration_file.get_t_u_init = 1;
    }


    if (pDomelinkage_item->rw.m_configuration_file.get_index < pDomelinkage_item->rw.m_configuration_file.get_end) {
        pDomelinkage_item->rw.m_configuration_file.get_t_u_done = 1;
        printf("%s %d, get the T up data finish(get_index:%d get_end:%d)!! \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.m_configuration_file.get_index, pDomelinkage_item->rw.m_configuration_file.get_end);
        return 1;
    }

    static float outP = 0, outT = 0, outZ = 0;

    if (pDomelinkage_item->rw.m_configuration_file.get_delay == 0) {
        outP = 0;
        outT = pDomelinkage_item->rw.m_configuration_file.get_index;
        outZ = 16;

        int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, outP, outT, outZ);
        if (ret != 0) {
            printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, outP, outT, outZ);
            return -1;
        }
    }

    pDomelinkage_item->rw.m_configuration_file.get_delay++;

    float curr_p = 0, curr_t = 0, curr_z = 0;
    int nnnn = 80;
    if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
        //printf("%s %d, get T up data, startT:%d endT:%d \n", __FUNCTION__, __LINE__, startT, endT);
        nnnn = 80;
    } else {
        nnnn = 20;
    }

    if (pDomelinkage_item->rw.m_configuration_file.get_delay > nnnn) {
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;

        int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
        if (ccc == -1) {
            printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
            return -1;
        }

        printf(" curr_t:%f outT:%f \n", curr_t, outT);

        if (outT == curr_t) {

            if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
                if (!access("/fs/project/data/config/t.dat", F_OK))
                    remove("/fs/project/data/config/t.dat");
            }

            FILE* f = fopen("/fs/project/data/config/t.dat", "a+");
            if (f) {
                char info[28];
                printf("write:%d \n", pDomelinkage_item->rw.m_configuration_file.get_index);
                sprintf(info, "%d\r\n", pDomelinkage_item->rw.m_configuration_file.get_index);
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
        }

        pDomelinkage_item->rw.m_configuration_file.get_index--;
    }

    return 0;
}

/*
 * 向下,递增
 */
int testT_d(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    //    if (!access("/fs/project/data/config/t.dat", F_OK)) {
    //        return 1;
    //    }
    printf(">>>>>>>>>>>>>>>>>>>>>>get_t_d_init:%d<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", pDomelinkage_item->rw.m_configuration_file.get_t_d_init);

    if (pDomelinkage_item->rw.m_configuration_file.get_t_d_init == 0) {

        pDomelinkage_item->rw.m_configuration_file.get_start = 0;
        pDomelinkage_item->rw.m_configuration_file.get_end = 375;

        printf(">>>>>>>>>>>>>>>>>>>>>>1 get_t_d_init:%d<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", pDomelinkage_item->rw.m_configuration_file.get_t_d_init);
        char buf[256];
        if (ReadProfileStr("T_D", "startT", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_start);
        }

        printf(">>>>>>>>>>>>>>>>>>>>>2 >get_t_d_init:%d<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", pDomelinkage_item->rw.m_configuration_file.get_t_d_init);

        if (ReadProfileStr("T_D", "endT", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_end);
        }
        printf(">>>>>>>>>>>>>>>>>>>>>3 >get_t_d_init:%d<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", pDomelinkage_item->rw.m_configuration_file.get_t_d_init);

        if (pDomelinkage_item->rw.m_configuration_file.get_start < 0 || pDomelinkage_item->rw.m_configuration_file.get_end < 0 || pDomelinkage_item->rw.m_configuration_file.get_start > pDomelinkage_item->rw.m_configuration_file.get_end) {
            pDomelinkage_item->rw.m_configuration_file.get_t_d_done = 1;
            return -1;
        }
        printf(">>>>>>>>>>>>>>>>>>>>>4 >get_t_d_init:%d<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", pDomelinkage_item->rw.m_configuration_file.get_t_d_init);

        pDomelinkage_item->rw.m_configuration_file.get_index = pDomelinkage_item->rw.m_configuration_file.get_start;
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;
        pDomelinkage_item->rw.m_configuration_file.get_t_d_init = 1;
        printf(">>>>>>>>>>>>>>>>>>>>>5 >get_t_d_init:%d<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", pDomelinkage_item->rw.m_configuration_file.get_t_d_init);

    }

    if (pDomelinkage_item->rw.m_configuration_file.get_index > pDomelinkage_item->rw.m_configuration_file.get_end) {
        pDomelinkage_item->rw.m_configuration_file.get_t_d_done = 1;
        printf("%s %d, get the T down data finish(get_index:%d get_end:%d)!! \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.m_configuration_file.get_index, pDomelinkage_item->rw.m_configuration_file.get_end);
        return 1;
    }

    static float outP = 0, outT = 0, outZ = 0;

    if (pDomelinkage_item->rw.m_configuration_file.get_delay == 0) {
        outP = 0;
        outT = pDomelinkage_item->rw.m_configuration_file.get_index;
        outZ = 16;

        int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, outP, outT, outZ);
        if (ret != 0) {
            printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, outP, outT, outZ);
            return -1;
        }
    }

    pDomelinkage_item->rw.m_configuration_file.get_delay++;

    float curr_p = 0, curr_t = 0, curr_z = 0;

    int nnnn = 80;
    if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
        nnnn = 80;
        // printf("%s %d, get T down data, startT:%d endT:%d \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.m_configuration_file.get_start, endT);
    } else {
        nnnn = 20;
    }

    if (pDomelinkage_item->rw.m_configuration_file.get_delay > nnnn) {
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;

        int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
        if (ccc == -1) {
            printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
            return -1;
        }

        printf(" curr_t:%f outT:%f \n", curr_t, outT);

        if (outT == curr_t) {

            FILE* f = fopen("/fs/project/data/config/t.dat", "a+");
            if (f) {
                char info[28];
                printf("write:%d \n", pDomelinkage_item->rw.m_configuration_file.get_index);
                sprintf(info, "%d\r\n", pDomelinkage_item->rw.m_configuration_file.get_index);
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
        }

        pDomelinkage_item->rw.m_configuration_file.get_index++;
    }

    return 0;
}

int testZ(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    //    if (!access("/fs/project/data/config/z.dat", F_OK)) {
    //        return 1;
    //    }
    if (pDomelinkage_item->rw.m_configuration_file.get_z_init == 0) {

        pDomelinkage_item->rw.m_configuration_file.get_start = 0;
        pDomelinkage_item->rw.m_configuration_file.get_end = 1025;

        char buf[256];
        if (ReadProfileStr("Z", "startZ", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_start);
        }
        if (ReadProfileStr("Z", "endZ", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &pDomelinkage_item->rw.m_configuration_file.get_end);
        }

        if (pDomelinkage_item->rw.m_configuration_file.get_start < 0 || pDomelinkage_item->rw.m_configuration_file.get_end < 0 || pDomelinkage_item->rw.m_configuration_file.get_start > pDomelinkage_item->rw.m_configuration_file.get_end) {
            return -1;
        }

        pDomelinkage_item->rw.m_configuration_file.get_index = pDomelinkage_item->rw.m_configuration_file.get_start;
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;
        pDomelinkage_item->rw.m_configuration_file.get_z_init = 1;
    }


    if (pDomelinkage_item->rw.m_configuration_file.get_index > pDomelinkage_item->rw.m_configuration_file.get_end) {
        pDomelinkage_item->rw.m_configuration_file.get_z_done = 1;
        return 1;
    }

    static float outP = 0, outT = 0, outZ = 0;

    if (pDomelinkage_item->rw.m_configuration_file.get_delay == 0) {
        outP = 0;
        outT = 0;
        outZ = pDomelinkage_item->rw.m_configuration_file.get_index;

        int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, outP, outT, outZ);
        if (ret != 0) {
            printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, outP, outT, outZ);
            return -1;
        }
    }

    pDomelinkage_item->rw.m_configuration_file.get_delay++;

    float curr_p = 0, curr_t = 0, curr_z = 0;

    int nnnn = 80;
    if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
        //printf("%s %d, get Z data, startZ:%d endZ:%d \n", __FUNCTION__, __LINE__, pDomelinkage_item->rw.m_configuration_file.get_start, endZ);
        nnnn = 80;
    } else {
        nnnn = 80;
    }

    if (pDomelinkage_item->rw.m_configuration_file.get_delay > nnnn) {
        pDomelinkage_item->rw.m_configuration_file.get_delay = 0;

        int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
        if (ccc == -1) {
            printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
            return -1;
        }

        printf("11 curr_p:%f curr_p:%f \n", curr_p, outP);

        if (outZ == curr_z) {

            if (pDomelinkage_item->rw.m_configuration_file.get_index == pDomelinkage_item->rw.m_configuration_file.get_start) {
                if (!access("/fs/project/data/config/z.dat", F_OK))
                    remove("/fs/project/data/config/z.dat");
            }

            FILE* f = fopen("/fs/project/data/config/z.dat", "a+");
            if (f) {
                char info[28];
                printf("write:%d \n", pDomelinkage_item->rw.m_configuration_file.get_index);
                sprintf(info, "%d\r\n", pDomelinkage_item->rw.m_configuration_file.get_index);
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
        }

        pDomelinkage_item->rw.m_configuration_file.get_index++;
    }

    return 0;
}

/******************************************************************************
 *                      获取球机PTZ参数配置文件
 * 
 * 针对海康球机，ptz的值是不确定哪些是真实，哪些是虚假的，需要一一验证，并将真实数据保存到配置文件中
 * 
 * 创建 /fs/project/data/makeconfig/makePTZ.dat文件，开始进行验证，验证结束后，会自动删除该文件
 * 
 * 如果在../config/目录下，已经存在配置文件了，就不再获取，除非将其删除掉。
 * 
 ******************************************************************************/
void domelinkage_get_ptz_param(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    if (access("/fs/project/data/config", 0) == -1)//access函数是查看文件是不是存在
    {
        if (mkdir("/fs/project/data/config", 0777))//如果不存在就用mkdir函数来创建
        {
            // printf("creat file bag failed!!!");
        }
    }

    if (access("/fs/project/data/makeconfig", 0) == -1)//access函数是查看文件是不是存在
    {
        if (mkdir("/fs/project/data/makeconfig", 0777))//如果不存在就用mkdir函数来创建
        {
            // printf("creat file bag failed!!!");
        }
    }

    if (!access("/fs/project/data/makeconfig/makePTZ.dat", F_OK)) {

        int ret = 0;
        if (pDomelinkage_item->rw.m_configuration_file.get_p_done == 0)
            ret = testP(pDomelinkage_item, pFrame);
        if (ret == -1) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");

            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get P data,for the input para: startP,endP is error!");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        } else if (ret == -2) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get P data,for cannot control the camera !");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        }

        ret = 0;
        if (pDomelinkage_item->rw.m_configuration_file.get_p_done == 1 && pDomelinkage_item->rw.m_configuration_file.get_t_u_done == 0)
            ret = testT_u(pDomelinkage_item, pFrame);
        if (ret == -1) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get T data,for the input para: startT,endT is error!");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        } else if (ret == -2) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get T data,for cannot control the camera !");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        }



        ret = 0;
        if (pDomelinkage_item->rw.m_configuration_file.get_t_u_done == 1 && pDomelinkage_item->rw.m_configuration_file.get_t_d_done == 0)
            ret = testT_d(pDomelinkage_item, pFrame);
        if (ret == -1) {
            printf(">>>>>>>>>>1<<<<<<<<<<<<<<\n");
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            printf(">>>>>>>>>>2<<<<<<<<<<<<<<\n");
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            printf(">>>>>>>>>>3<<<<<<<<<<<<<<\n");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get T data,for the input para: startT,endT is error!");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            printf(">>>>>>>>>>4<<<<<<<<<<<<<<\n");
            //return;
        } else if (ret == -2) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get T data,for cannot control the camera !");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        }


        ret = 0;
        if (pDomelinkage_item->rw.m_configuration_file.get_t_d_done == 1 && pDomelinkage_item->rw.m_configuration_file.get_z_done == 0)
            ret = testZ(pDomelinkage_item, pFrame);
        if (ret == -1) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get Z data,for the input para: startZ,endZ is error!");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        } else if (ret == -2) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
            //删除生成的配置文件
            if (!access("/fs/project/data/config/p.dat", F_OK))
                remove("/fs/project/data/config/p.dat");
            if (!access("/fs/project/data/config/t.dat", F_OK))
                remove("/fs/project/data/config/t.dat");
            if (!access("/fs/project/data/config/z.dat", F_OK))
                remove("/fs/project/data/config/z.dat");
            //写入错误日志
            FILE* f = fopen("/fs/project/data/makeconfig/makePTZ_error.dat", "wb");
            if (f) {
                char info[28];
                sprintf(info, "cannot get Z data,for cannot control the camera !");
                fwrite(info, strlen(info), 1, f);
                fclose(f);
            }
            return;
        }

        if (pDomelinkage_item->rw.m_configuration_file.get_z_done == 1) {
            //清除标志位
            memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
            //删除触发标志
            remove("/fs/project/data/makeconfig/makePTZ.dat");
        }
    } else {
        memset(&pDomelinkage_item->rw.m_configuration_file, 0, sizeof (pDomelinkage_item->rw.m_configuration_file));
    }
}

/******************************************************************************
 *                获取一个p值和一个t值对应的像素点个数
 * 
 * 创建/fs/project/data/makeconfig/makeScale.dat文件，即可触发一条框选放大指令
 * 
 * 结束后，自动删除该文件
 * 
 * 初始ptz位置信息从/fs/project/data/makeconfig/param.dat文件中读取
 * 
 * 
 *******************************************************************************/
int dome_linkage_get_ptz_scale(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame, int w, int h) {

    if (!access("/fs/project/data/makeconfig/makeScale.dat", F_OK)) {
        int p, t, z;
        p = 0;
        t = 0;
        z = 16;
        char buf[256];
        if (ReadProfileStr("MakeScale", "p", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &p);
        }
        if (ReadProfileStr("MakeScale", "t", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &t);
        }
        if (ReadProfileStr("MakeScale", "z", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
            sscanf(buf, "%d", &z);
        }

        int p1, t1;
        int z1[100] = {0};
        static int z_count = 0;

        static int getpic = 0;
        if (getpic == 0) {

            getpic = 1;

            char ln[80];
            FILE *f = fopen("/fs/project/data/config/p.dat", "r");
            if (NULL == f) {
                printf("%s %d: cannot open the model file (pScale_path=/fs/project/data/config/p.dat)\n", __FUNCTION__, __LINE__);
                return -1;
            }

            int nnn = 0, mmm = 0;
            while (1) {
                if (NULL == fgets(ln, 80, f)) break;

                int nnn = atoi(ln);
                if (p == mmm) {
                    break;
                }
                mmm = nnn;
            }
            fclose(f);

            p1 = nnn;

            printf(">>>p1:%d<<<<<\n", p1);
            ///////////////////////////////////////////////////////////////////////////////
            f = fopen("/fs/project/data/config/t.dat", "r");
            if (NULL == f) {
                printf("%s %d: cannot open the model file (pScale_path=/fs/project/data/config/t.dat)\n", __FUNCTION__, __LINE__);
                return -1;
            }

            nnn = 0, mmm = 0;
            while (1) {
                if (NULL == fgets(ln, 80, f)) break;

                int nnn = atoi(ln);
                if (t == mmm) {
                    break;
                }
                mmm = nnn;
            }
            fclose(f);

            t1 = nnn;

            printf(">>>t1:%d<<<<<\n", t1);
            ////////////////////////////////////////////////////////////////////////////////////
            f = fopen("/fs/project/data/config/z.dat", "r");
            if (NULL == f) {
                printf("%s %d: cannot open the model file (pScale_path=/fs/project/data/config/z.dat)\n", __FUNCTION__, __LINE__);
                return -1;
            }

            nnn = -1, mmm = 0;
            while (1) {
                if (NULL == fgets(ln, 80, f)) break;

                if (z == mmm) {
                    z1[z_count] = atoi(ln);
                    z_count++;
                    nnn = 0;
                }

                if (nnn >= 0) {
                    nnn++;
                }

                if (nnn == 5) {
                    z1[z_count] = atoi(ln);
                    z_count++;
                    nnn = 0;
                }
            }
            fclose(f);
        }

        printf(">>>z_count:%d<<<<<\n", z_count);
        ////////////////////////////////////////////////////////////////////////////////////////////
        static int z_count1 = z_count;
        static int delay = 0;
        static int flag = 1;

        if (delay == 0 && flag == 1) {
            int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, (float) p, (float) t, (float) z1[z_count - z_count1]);
            if (ret != 0) {
                return -2;
                printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) p, (float) t, (float) z1[z_count - z_count1]);
            }
        }

        delay++;

        if (delay > 80 && flag == 1) {
            char filepath[512];
            sprintf(filepath, "/fs/project/data/makeconfig/%d/%d_%d_%d.bmp", z1[z_count - z_count1], p, t, z1[z_count - z_count1]);
            PrepareDir(filepath);

            ImageDib* pdib = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
            memcpy(pdib->data, pFrame->data.raw[0]->data, w * h);
            image_dib_svae_to_file(pdib, filepath);

            image_dib_delete__OI(&pdib);

            delay = 0;
            flag = 0;
        }

        if (delay == 0 && flag == 0) {
            int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, (float) p1, (float) t1, (float) z1[z_count - z_count1]);
            if (ret != 0) {
                return -2;
                printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) p1, (float) t1, (float) z1[z_count - z_count1]);
            }
        }


        if (delay > 80 && flag == 0) {

            char filepath[512];
            sprintf(filepath, "/fs/project/data/makeconfig/%d/%d_%d_%d.bmp", z1[z_count - z_count1], p1, t1, z1[z_count - z_count1]);
            PrepareDir(filepath);

            ImageDib* pdib = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
            memcpy(pdib->data, pFrame->data.raw[0]->data, w * h);
            image_dib_svae_to_file(pdib, filepath);

            image_dib_delete__OI(&pdib);

            delay = 0;
            flag = 1;
            z_count1--;
        }

        if (z_count1 < 1) {
            //删除文件       
            printf(">>>>>>>>>>>>>>>>>>>>>>>>>3 delete the data<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
            remove("/fs/project/data/makeconfig/makeScale.dat");
        }
    }

    return 0;
}

/******************************************************************************
 *                              验证框选放大功能
 * 
 * 创建/fs/project/data/makeconfig/setwalkman.dat文件，即可触发一条框选放大指令
 * 
 * 结束后，自动删除该文件
 * 
 * 框选放大指令的坐标信息从/fs/project/data/makeconfig/param.dat文件中读取
 * 
 * 
 *******************************************************************************/
void domelinkage_verify_focusRect(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {

    if (!access("/fs/project/data/makeconfig/verifyFocusRect.dat", F_OK)) {

        //读取坐标
        double pos[4];
        {

            pos[0] = 0.156250;
            pos[1] = 0.297917;
            pos[2] = 0.625926;
            pos[3] = 0.696296;

            char buf[256];
            if (ReadProfileStr("FocusRect", "left", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                sscanf(buf, "%lf", pos[0]);
            }
            if (ReadProfileStr("FocusRect", "top", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                sscanf(buf, "%lf", pos[1]);
            }
            if (ReadProfileStr("FocusRect", "right", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                sscanf(buf, "%lf", pos[2]);
            }
            if (ReadProfileStr("FocusRect", "bottom", buf, 256, "/fs/project/data/makeconfig/param.dat")) {
                sscanf(buf, "%lf", pos[3]);
            }

            int w = 1920;
            int h = 1080;

            double map_left = pos[0];
            double map_top = pos[1];
            double map_right = pos[2];
            double map_bottom = pos[3];

            /////////////////////////////////////////////////
            //枪球映射     
            if (0) {
                double temp_left = pos[0] / (double) w;
                double temp_top = pos[1] / (double) h;
                double temp_right = pos[2] / (double) w;
                double temp_bottom = pos[3] / (double) h;

                printf("1 %lf %lf %lf %lf \n", temp_left, temp_top, temp_right, temp_bottom);
                int index = 0;
                //坐标映射(枪机映射到球机)
                double m_debug[10];
                int ret = AlgMapping_1(pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[0], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[1],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[2], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[3],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[4], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[5],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[6], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].gunSet[7],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[0], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[1],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[2], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[3],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[4], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[5],
                        pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[6], pDomelinkage_item->ro.m_calconfig[pDomelinkage_item->rw.v_linkage_curr_gunIndex].m_calParam[index].domeSet[7],
                        temp_left, temp_top, temp_right, temp_bottom, w, h, &map_left, &map_top, &map_right, &map_bottom, m_debug);


                printf("2 %lf %lf %lf %lf \n", map_left, map_top, map_right, map_bottom);
                //            printf("%s %d AlgMapping[index:%d] (%lf %lf %lf %lf)-((%lf %lf %lf %lf))\n", __FUNCTION__, __LINE__, index,
                //                temp_left, temp_top, temp_right, temp_bottom, map_left, map_top, map_right, map_bottom);
            }
            ////////////////////////////////////////////////


            map_left *= w;
            map_top *= h;
            map_right *= w;
            map_bottom *= h;

            printf("mapping: (%d %d %d %d)-(%d %d %d %d) \n", (int) map_left, (int) map_top, (int) map_right, (int) map_bottom, pos[0], pos[1], pos[2], pos[3]);

            float curr_p = 0, curr_t = 0, curr_z = 0;
            int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
            if (ccc == -1) {
                printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
            }

            int ret = FreeWalkerInit_1(0, 1920, 1080, "/fs/project/data/config/model.dat", "/fs/project/data/config/p.dat", "/fs/project/data/config/t.dat", "/fs/project/data/config/z.dat", "");
            if (ret != 0) {
                printf("FreeWalkerInit_1 error \n");
            }

            int out_p, out_t, out_z;
            int outWidth = 0, outHeight = 0;
            FreeWalkerProc(0, (unsigned short) curr_p, (unsigned short) curr_t, (unsigned short) curr_z, (int) map_left, (int) map_top, (int) map_right, (int) map_bottom, 0, &out_p, &out_t, &out_z, &outWidth, &outHeight);

            ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, (float) out_p, (float) out_t, (float) out_z);
            if (ret != 0) {
                printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, (float) out_p, (float) out_t, (float) out_z);
            }
        }

        //删除文件
        remove("/fs/project/data/makeconfig/verifyFocusRect.dat");
        //        FILE* f = fopen("/fs/project/data/makeconfig/verifyFocusRect.dat", "wb");
        //        if (f != NULL) {
        //            fwrite("verify the focus rect !", strlen("verify the focus rect !"), 1, f);
        //            fclose(f);
        //        }
    }
}

/*******************************************************************************
 * //保存调试图片
 *******************************************************************************/
void domelinkage_save_debug_image(struct Domelinkage_item * pDomelinkage_item, FsObjectImageFrame * pFrame) {


    if (pDomelinkage_item->ro.m_debug_switch > 0) {
        //保存一张图
        int m_spnap_index = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - pDomelinkage_item->rw.v_linkage_snap_times;
        if (pDomelinkage_item->rw.m_debug_snapflag[m_spnap_index] == 1) {

            pDomelinkage_item->rw.m_debug_snapflag[m_spnap_index] = 0;

            int w = pFrame->width[0];
            int h = pFrame->height[0];
            ITG_RECT myrect;
            myrect.left = pDomelinkage_item->rw.m_debug_left * w;
            myrect.right = pDomelinkage_item->rw.m_debug_right * w;
            myrect.top = pDomelinkage_item->rw.m_debug_top * h;
            myrect.bottom = pDomelinkage_item->rw.m_debug_bottom * h;

            if (myrect.left < 0) myrect.left = 0;
            if (myrect.left > w - 1) myrect.left = w - 1;
            if (myrect.top < 0) myrect.top = 0;
            if (myrect.top > h - 1) myrect.top = h - 1;
            if (myrect.right < 0) myrect.right = 0;
            if (myrect.right > w - 10) myrect.right = w - 1;
            if (myrect.bottom < 0) myrect.bottom = 0;
            if (myrect.bottom > h - 1) myrect.bottom = h - 1;

            ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, NULL, NULL, w, &myrect, 255, 255, 255, 50, 1);

            myrect.left = pDomelinkage_item->rw.m_debug_adjust_left * w;
            myrect.right = pDomelinkage_item->rw.m_debug_adjust_right * w;
            myrect.top = pDomelinkage_item->rw.m_debug_adjust_top * h;
            myrect.bottom = pDomelinkage_item->rw.m_debug_adjust_bottom * h;

            if (myrect.left < 0) myrect.left = 0;
            if (myrect.left > w - 1) myrect.left = w - 1;
            if (myrect.top < 0) myrect.top = 0;
            if (myrect.top > h - 1) myrect.top = h - 1;
            if (myrect.right < 0) myrect.right = 0;
            if (myrect.right > w - 10) myrect.right = w - 1;
            if (myrect.bottom < 0) myrect.bottom = 0;
            if (myrect.bottom > h - 1) myrect.bottom = h - 1;

            ITG_DrawRect_y((char *) pFrame->data.raw[0]->data, NULL, NULL, w, &myrect, 255, 255, 255, 50, 1);

            int diftime = (int) ((fs_time_uptime_get() - pDomelinkage_item->p.pGunData->thistime)*1000);

            time_t ts = (time_t) fs_time_GMT_get();
            struct tm ti;
            localtime_r(&ts, &ti);
            char filepath[512];
            sprintf(filepath, "/fs/project/data/vsys/641/debug/%04d%02d%02d/%02d/dome_%d_%d_%d_%d.bmp",
                    1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, pDomelinkage_item->rw.m_debug_number, pFrame->index, pDomelinkage_item->rw.m_debug_boat_height,
                    pDomelinkage_item->rw.m_debug_boat_fouce_height);
            PrepareDir(filepath);

            ImageDib* pdib = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
            memcpy(pdib->data, pFrame->data.raw[0]->data, w * h);
            image_dib_svae_to_file(pdib, filepath);

            image_dib_delete__OI(&pdib);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 通道分层多线程写入法 */
static void *domelinkage_P_T(struct Domelinkage * const pDomelinkage) {
#undef FsFunctionName
#define FsFunctionName domelinkage_P_T
    //FsPrintf(1, "Thread Index=%hu.\n", pRecord->p.threadCount);

    FsPrintf(2, "domelinkage_private_thread begin**** ....\n");
    fflush(stdout);
    ////////////////////////////////////////////////////////////////////////////
    /* 创建线程监控 */
    void *pMonitor = pDomelinkage->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, pDomelinkage->ro._threadName, NULL, pDomelinkage, pDomelinkage->ro._pMonitor ? pDomelinkage->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pDomelinkage->ro._externRunStatus;
    //RtspServer *pRtspServer = pVtrack->ro._pRtspServer;
    ConfigManager * const pConfigManager = (ConfigManager*) pDomelinkage->ro._pConfigManager;
    FsObjectList * const itemList_ = pDomelinkage->ro.__itemList_;
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
#define __domelinkage_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
#define __domelinkage_P_T_clean2_count 0
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __domelinkage_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    //FsObjectBaseBuffer baseBuffer;
    //fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __domelinkage_P_T_clean1_3 //fs_ObjectBaseBuffer_release(&baseBuffer); 
    ////////////////////////////////////////////////////////////////////////////
    // 通道数
    unsigned int channelCount = 0;
    double thisUptime;
    /* 开方分组 */
    FsGroupSqrt groupSqrt;
    Fs_GroupSqrt_init(&groupSqrt);
#define __domelinkage_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __domelinkage_P_T_clean1_count 4
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    FsObjectImageFrame *pFrame;
    struct Domelinkage_item *pDomelinkage_item, **ppNode_itemList;
    unsigned int sleepTime = 0;
    ////////////////////////////////////////////////////////////////////////////
    int sw = 0; //算法处理的图像尺寸
    int sh = 0; //
    int currFrameIndex = 0; //当前图像帧号
    unsigned char* pSrcImage = NULL; //算法处理的图像数据指针
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 注册命令字
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    /* 主循环 */
    while (pDomelinkage->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        double tt0 = fs_time_uptime_get();
        usleep(1000);

        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        thisUptime = fs_time_uptime_get();
        ////////////////////////////////////////////////////////////////////////
        if (pDomelinkage->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */
            if (0 == pDomelinkage->rw._resetChannel) channelCount = domelinkage_private_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server Domelinkage
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
                pDomelinkage_item = (struct Domelinkage_item *) fs_ObjectList_remove_tail(itemList_);
                if (pDomelinkage_item) domelinkage_private_item_delete__OI(pDomelinkage_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);

            printf("*****************11 resetChannel:%d***************\n", pDomelinkage->rw._resetChannel);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pDomelinkage->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pDomelinkage->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pDomelinkage->rw._resetChannel > 0) {
                //FsPrintf(1, "%d\n", pDomelinkage->rw._resetChannel);

                pMonitorItem->rw.line = __LINE__;
                domelinkage_private_item_new(pDomelinkage, pDomelinkage->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Domelinkage_item **) (itemList_->pNode + pDomelinkage->rw._resetChannel - 1), &groupSqrt, &shareBuffer);

                pMonitorItem->rw.line = __LINE__;
            }
            pDomelinkage->rw._resetChannel = -1;
        }
        double tt1 = fs_time_uptime_get();
        ////////////////////////////////////////////////////////////////////////pRecord_itemNode = (struct Record_item **) itemList->pNode + itemList->startIndex;
        double lasttime = fs_time_uptime_get();
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct Domelinkage_item * * const ppNode_itemList = (struct Domelinkage_item **) itemList_->pNode + itemList_->startIndex;
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
                struct Domelinkage_item * const pDomelinkage_item = ppNode_itemList[ui];
                if (pDomelinkage_item == NULL)continue;
                if (0 == (pDomelinkage_item->ro.moduleMask & 0x8)) {
                    if (pDomelinkage_item->ro.__framelistIn->nodeCount > 0) {
                        pthread_mutex_lock(&pDomelinkage_item->ro.__framelistIn->mutex);
                        fs_ObjectList_insert_tail_all_clean_pthreadSafety(pDomelinkage_item->ro.__framelistOut, pDomelinkage_item->ro.__framelistIn);
                        pthread_mutex_unlock(&pDomelinkage_item->ro.__framelistIn->mutex);
                        Fs_GroupSqrtOut_value_set(pDomelinkage_item->rw._pGroupSqrtOut, &pDomelinkage_item->ro._pDomelinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                    }
                    continue;
                }
                /*检测是否有联动数据，如果有数据，则向枪机请求最新的目标坐标*/
                if (pDomelinkage_item->ro.m_config_devinfo.m_camera_model == 0 || pDomelinkage_item->ro.m_config_devinfo.m_camera_model == 1) {

                    //向枪机获取目标最新坐标信息
                    domelinkage_request_updateData(pDomelinkage_item, pConfigManager, thisUptime);

                    /*如果有需要联动的数据，先进行云台操作：1.转向预置位，2.调用框选放大指令*/
                    if (NULL != pDomelinkage_item->p.pGunData) {
                        int m_spnap_index = pDomelinkage_item->ro.m_dome_alg_config.m_linkage_snapNumber - pDomelinkage_item->rw.v_linkage_snap_times;
                        if (pDomelinkage_item->p.pGunData->status == 1 && pDomelinkage_item->rw.v_linkage_focusRect_flag[m_spnap_index] == 0) {

                            //云台转动指令      
                            int ret = domelinkage_crtl_holder(pDomelinkage_item, pDomelinkage_item->ro.imageWidth, pDomelinkage_item->ro.imageHeight);
                            if (ret != 0) {
                                //拍照失败，发送错误代码
                                domelinkage_reply2gun(pDomelinkage_item, NULL, ret);

                                domelinkage_goback_PrePTZ(pDomelinkage_item);

                                fs_ObjectList_remove_head_pthreadSafety(pDomelinkage_item->ro.__gunDataClientList);
                                /* 释放原来保存的连接,必须是原来保存过的连接,否则会死机 */
                                if (pDomelinkage_item->p.pGunData->data != NULL)
                                    configManager_connect_release(pConfigManager, pDomelinkage_item->p.pGunData->data, 0);

                                if (pDomelinkage_item->p.pGunData->data != NULL) fsFree(pDomelinkage_item->p.pGunData);
                                pDomelinkage_item->p.pGunData = NULL;

                                //将之前缓存帧去掉
                                int i;
                                for (i = 0; i < 5; i++) {
                                    if (pDomelinkage_item->rw._ppFrame[i] != NULL) {
                                        printf("%s %d, snap boat failed,delete the pframe._ppFrame[%d]:%p \n", __FUNCTION__, __LINE__, i, pDomelinkage_item->rw._ppFrame[i]);
                                        fs_Object_delete_pthreadSafety__OI(pDomelinkage_item->rw._ppFrame[i]);
                                        pDomelinkage_item->rw._ppFrame[i] = NULL;
                                    }
                                }

                                //重置参数
                                resetParam(pDomelinkage_item, 1);

                                printf("%s %d, control the camera holder error !, stop the inc...ret:%d \n", __FUNCTION__, __LINE__, ret);
                                char message[512];
                                sprintf(message, "%s %d, control the camera holder error !, stop the inc...ret:%d \n", __FUNCTION__, __LINE__, ret);
                                MakeLog(message, 0);
                            }
                        }
                    }
                }

                if (pDomelinkage_item->ro.__framelistIn->nodeCount > 1) {

                    do {
                        double tt2 = fs_time_uptime_get();
                        //获取图像数据帧
                        pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pDomelinkage_item->ro.__framelistIn);

                        //如果相机类型不是枪机，则直接出去
                        if (pDomelinkage_item->ro.m_config_devinfo.m_camera_model != 0 && pDomelinkage_item->ro.m_config_devinfo.m_camera_model != 1) {
                            fs_ObjectList_insert_tail_pthreadSafety(pDomelinkage_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pDomelinkage_item->rw._pGroupSqrtOut, &pDomelinkage_item->ro._pDomelinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                            continue;
                        }

                        //获取图像尺寸
                        pDomelinkage_item->ro.imageWidth = pFrame->width[0];
                        pDomelinkage_item->ro.imageHeight = pFrame->height[0];

                        //获取帧号
                        currFrameIndex = pFrame->index;
                        double t1 = fs_time_uptime_get();

                        //调试打印信息
                        if (currFrameIndex % 600 == 0) {
                            printf("---domelinkage 20171011/1- dome_uuid:%s-framedx:%d w:%d h:%d-\n", pDomelinkage_item->ro._uuid, pFrame->index, pDomelinkage_item->ro.imageWidth,
                                    pDomelinkage_item->ro.imageHeight);
                            fflush(stdout);
                            char message[512];
                            sprintf(message, "---domelinkage 20171011/1- rst->ro._uuid:%s-framedx:%d w:%d h:%d-\n", pDomelinkage_item->ro._uuid, pFrame->index, pDomelinkage_item->ro.imageWidth,
                                    pDomelinkage_item->ro.imageHeight);
                            MakeLog(message, 0);
                        }

                        if (0) {

                            static float outP = 0, outT = 0, outZ = 0;
                            float curr_p, curr_t, curr_z;
                            static int setflag = 0;
                            static double settime = fs_time_uptime_get();
                            static double gettime = fs_time_uptime_get();
                            outT = 0;
                            outZ = 16;

                            if (setflag == 0) {
                                int ret = camera_ctrl_tcp_goPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, outP, outT, outZ);
                                if (ret != 0) {
                                    printf("%s %d goto the presetPTZ failed (ret:%d  %lf %lf %lf)\n", __FUNCTION__, __LINE__, ret, outP, outT, outZ);
                                }
                                printf("set PTZ: outP:%f \n", outP);
                                settime = fs_time_uptime_get();
                                gettime = fs_time_uptime_get();
                                setflag = 1;

                            }

                            gettime = fs_time_uptime_get();

                            if (gettime > settime + 0.2) {
                                //获取ptz
                                int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
                                if (ccc == -1) {
                                    printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
                                }
                                printf("get PTZ[0.2] outP:%f curr_p:%f , t1:%lf,t2:%lf \n", outP, curr_p, settime, gettime);
                            }
                            if (gettime > settime + 0.4) {
                                //获取ptz
                                //获取ptz
                                int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
                                if (ccc == -1) {
                                    printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
                                }
                                printf("get PTZ[0.4] outP:%f curr_p:%f , t1:%lf,t2:%lf \n", outP, curr_p, settime, gettime);
                            }
                            if (gettime > settime + 0.6) {
                                //获取ptz
                                int ccc = camera_ctrl_tcp_getPTZ(pDomelinkage_item->rw.pCamera_ctrl_item, &curr_p, &curr_t, &curr_z);
                                if (ccc == -1) {
                                    printf("%s %d getPTZ  failed...\n", __FUNCTION__, __LINE__);
                                }
                                printf("get PTZ[0.6] outP:%f curr_p:%f , t1:%lf,t2:%lf \n", outP, curr_p, settime, gettime);

                                setflag = 0;
                                settime = fs_time_uptime_get();
                                gettime = fs_time_uptime_get();
                                outP++;
                            }
                        }

                        //拍照(根据配置，保存多张图片)
                        domelinage_snap_boat(pDomelinkage_item, pFrame);
                        //拍照结束后，回到预置位，当回到预置位后，才接收新的任务
                        domelinage_finish_snap(pDomelinkage_item, pFrame, pConfigManager);
                        //获取球机ptz的值，并生成配置文件
                        domelinkage_get_ptz_param(pDomelinkage_item, pFrame);
                        //验证球机框选放大功能
                        domelinkage_verify_focusRect(pDomelinkage_item, pFrame);
                        //保存调试图片信息
                        domelinkage_save_debug_image(pDomelinkage_item, pFrame);
                        //获取不同z值条件下，偏移一个t和p，画面像素点的偏移量
                        int ret = dome_linkage_get_ptz_scale(pDomelinkage_item, pFrame, pFrame->width[0], pFrame->height[0]);
                        if (ret == -1) {

                            printf(">>>>>>>>>>>>>>>>>>>>>>>>> 1 delete the data<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

                            FILE* f = fopen("/fs/project/data/makeconfig/makeScale_error.dat", "wb");
                            if (f) {
                                char info[28];
                                sprintf(info, "cannot find the p.dat ,t.dat or z.dat");
                                fwrite(info, strlen(info), 1, f);
                                fclose(f);
                            }
                            remove("/fs/project/data/makeconfig/makeScale.dat");
                        } else if (ret == -2) {

                            printf(">>>>>>>>>>>>>>>>>>>>>>>>>2 delete the data<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

                            FILE* f = fopen("/fs/project/data/makeconfig/makeScale_error.dat", "wb");
                            if (f) {
                                char info[28];
                                sprintf(info, "cannot control the camera!");
                                fwrite(info, strlen(info), 1, f);
                                fclose(f);
                            }
                            remove("/fs/project/data/makeconfig/makeScale.dat");
                        }

                        double t2 = fs_time_uptime_get();

                        //计算时间
                        double thistime = fs_time_uptime_get();
                        if (thistime - lasttime > 0.040) {
                            FsPrintf(1, "cost long time(0.041) :%lf %lf \n", thistime - lasttime, t2 - t1);
                            char message[512];
                            sprintf(message, "cost long time(0.041) :%lf %lf \n", thistime - lasttime, t2 - t1);
                            MakeLog(message, 0);
                        }
                        lasttime = fs_time_uptime_get();

                        //数据传递...
                        // if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                        //    image_frame_clear_data(pFrame, ImageFrame_H264_0);
                        fs_ObjectList_insert_tail_pthreadSafety(pDomelinkage_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pDomelinkage_item->rw._pGroupSqrtOut, &pDomelinkage_item->ro._pDomelinkage->ro._pGroupSqrtOut->groupSqrt_mutex);
                    } while (pDomelinkage_item->ro.__framelistIn->nodeCount > 1);
                }
            }
        }
    }

    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    if (pDomelinkage->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pDomelinkage->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    /* 资源是多线程共享的,由第一个线程控制 */
    /* 删除各通道 */
    fs_ObjectList_clear_custom2(pDomelinkage->ro.__itemList_, (void (*)(void*, void *))domelinkage_private_item_delete__OI, &shareBuffer);

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
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd,threadCount=%hu.\n", pDomelinkage->ro._threadName, pDomelinkage->rw.runStatus, pDomelinkage->p.threadCount);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pDomelinkage->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}

/* 
 * 创建一个新的Vlinkage对象;
 * 返回Vlinkage指针.
 */
struct Domelinkage * domelinkage_new__IO(/* 线程名 */const char threadName[]
        , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus
        , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* 配置管理对象 */ void *pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(dir);
    if (dir[len - 1] == '/')len++;
    else len += 2;
    len += strlen(threadName) + 1;
    struct Domelinkage* rst = (struct Domelinkage*) fsMalloc(sizeof (struct Domelinkage) +len);
    memset(rst, 0, sizeof (struct Domelinkage));
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
void domelinkage_delete__OI(struct Domelinkage * pDomelinkage) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pDomelinkage->ro.__itemList_);
    /* 多线程锁 */
    pthread_mutex_destroy(&pDomelinkage->p.__mutex);

    if (pDomelinkage) fsFree(pDomelinkage);

}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void domelinkage_startThread(struct Domelinkage* pDomelinkage,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (pDomelinkage->rw.runStatus == 0 && pDomelinkage->ro.stid == 0) {
        pDomelinkage->rw.runStatus = 1;
        pDomelinkage->rw.thisNice = thisNice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pDomelinkage->ro._threadName,
                (unsigned long) pDomelinkage->ro._pMonitor,
                pDomelinkage->ro._parentTid,
                (unsigned long) pDomelinkage->ro._externRunStatus);
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
        if (pthread_create(&pDomelinkage->ro.stid, &attr, (void* (*)(void*))domelinkage_P_T, pDomelinkage)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pDomelinkage->rw.runStatus, pDomelinkage->ro.stid,
                pDomelinkage->ro._threadName,
                (unsigned long) pDomelinkage->ro._pMonitor,
                pDomelinkage->ro._parentTid,
                (unsigned long) pDomelinkage->ro._externRunStatus);
        fflush(stdout);
    }
}

/* 结束线程 */

void domelinkage_stopThread(struct Domelinkage * pDomelinkage) {
    pDomelinkage->rw.runStatus = 0;
    if (pDomelinkage->ro.stid) {
#ifdef FsDebug

        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pDomelinkage->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pDomelinkage->ro.stid);
        pDomelinkage->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void domelinkage_item_frame_in_pthreadSafety__OI_4(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server Domelinkage
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
unsigned int domelinkage_item_frame_out_pthreadSafety__IO_4(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server Domelinkage
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
unsigned int domelinkage_item_get_frame_count_pthreadSafety(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server Domelinkage
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
void domelinkage_item_reset_groupSqrtOut(struct Domelinkage * const pDomelinkage, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Domelinkage
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