#include "vtrack.h"
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

#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/FileTool.h"
#include "../../PublicTools/Fs/Socket.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../Modules/track/v_search.h"
#include "../../Modules/track/v_Track.h"
#include "../../Modules/track/v_findLine.h"
#include "../../PublicTools/Image/ImageDib.h"
#include "../../Modules/camera_ctrl/ZGXAPI.h"
#include "../../Modules/camera_ctrl/HCNetSDK.h"
#include "../../PublicTools/Image/ImageJpeg.h"
//#include "../../Modules/track/wave.h"
//#include "../../Modules/track/wave.h"
#include "../../Modules/trackcar/v_trackcar.h"
#include "../../Modules/track/vibe_proc.h"
#include "../../Modules/derelict/CamDetect.h"
#include "../../Modules/derelict/MetroDLL.h"
#include "../../Modules/track/v_tool.h"
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
char m_debugPath[32] = "/fs/project/data/temp"; //保存中间调试数据的目录
char m_debugPath_1[32] = "/fs/project/data/temp/search"; //保存中间调试数据的目录

unsigned int m_camera_connect_count = 0;
int m_camcrl_requesID_3[3] = {0}; //保存控制器的相关信息(回执信息使用)
unsigned int m_camcrl_head = 0;
unsigned int m_camcrl_requesID_data[5] = {0}; //保存控制器的相关信息(回执信息使用)


// fot test
//hiki:
#define MAX_NOTCONNECT_TIME     2000
#define MAX_RECONNECT_TIME      1000
#define MAX_FUN_FREQ_TIME       200
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

void MakeLogRecord(char* p, Vtrack_item *pVtrack_item) {

    if (p == NULL || pVtrack_item->ro.m_config_debug.savelog_flag != 1) return;

    pthread_mutex_lock(&pVtrack_item->rw.m_log_mutex);

    char filepath[512];
    char dir[512];
    time_t ts = (time_t) fs_time_uptime_get();
    struct tm ti;
    localtime_r(&ts, &ti);

    sprintf(filepath, "%04d%02d%02d%02d%02d.txt",
            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min);
    sprintf(dir, "%s/%s/%04d%02d%02d/%02d/%s", pVtrack_item->ro.m_config_debug.savelog_path, pVtrack_item->ro._uuid,
            1900 + ti.tm_year, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, filepath);
    PrepareDir(dir);
    FILE* f = fopen(dir, "a+b");
    if (f != NULL) {
        fprintf(f, "%s\r\n", p);
        fclose(f);
    }

    pthread_mutex_unlock(&pVtrack_item->rw.m_log_mutex);
}

/* 从配置获取通道数 */
static unsigned int vtrack_private_get_channelCount(/* 可为空 */FsConfig *pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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
static void vtrack_private_item_delete__OI(struct Vtrack_item * pVtrack_item
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {

    if (NULL == pVtrack_item)return;
    if (pVtrack_item->ro.moduleMask & Vtrack_Mask) {
        //    printf("***************************************************\n");
        //    printf("%s %d,release the connect,id:%d %d %d \n",__FUNCTION__,__LINE__,pVtrack_item->ro.m_camcrl_requesID_data[0],pVtrack_item->ro.m_camcrl_requesID_data[1],pVtrack_item->ro.m_camcrl_requesID_data[2]);      
        //    printf("***************************************************\n");
        //如果有连接，断开连接...
        if (pVtrack_item->ro.m_camctrl_flag == 1 && pVtrack_item->ro.m_camcrl_requesID_data[0] != 0) {
            printf("%s %d,release the connect,id:%d %d %d \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_camcrl_requesID_data[0], pVtrack_item->ro.m_camcrl_requesID_data[1], pVtrack_item->ro.m_camcrl_requesID_data[2]);
            configManager_connect_release(pVtrack_item->ro._pVtrack->ro._pConfigManager, pVtrack_item->ro.m_camcrl_requesID_data, 0);
        }

        pVtrack_item->ro.m_camctrl_flag = 0;

        configManager_cmd_logoff(pVtrack_item->ro._pVtrack->ro._pConfigManager, "snap", pVtrack_item->ro._uuid, pVtrack_item->ro._ipv4, pVtrack_item, pShareBuffer);
        configManager_cmd_logoff(pVtrack_item->ro._pVtrack->ro._pConfigManager, "track", pVtrack_item->ro._uuid, pVtrack_item->ro._ipv4, pVtrack_item, pShareBuffer);
        configManager_cmd_disconnect(pVtrack_item->ro._pVtrack->ro._pConfigManager, "video_part", pVtrack_item->ro._uuid, pVtrack_item);
    }

    fs_ObjectList_delete_custom__OI(pVtrack_item->ro.__framelistIn, fs_Object_delete_pthreadSafety__OI);
    fs_ObjectList_delete_custom__OI(pVtrack_item->ro.__framelistOut, fs_Object_delete_pthreadSafety__OI);
    if (pVtrack_item->ro.moduleMask & Vtrack_Mask) {
        /* 连接和调用外部命令所需要的同步锁 */
        pthread_mutex_destroy(&pVtrack_item->ro.__mutexCmdConnect);
        // 日志打印
        pthread_mutex_destroy(&pVtrack_item->rw.m_log_mutex);

        /* 文字叠加 */
        if (pVtrack_item->ro.__pTypeFace)fs_TypeFace_delete__OI(pVtrack_item->ro.__pTypeFace);

        if (pVtrack_item->rw.pCamera_ctrl_item)camera_ctrl_tcp_delete__OI(pVtrack_item->rw.pCamera_ctrl_item);
    }
    fsFree(pVtrack_item);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////客户端////////////////////////////////////////////////////
//连接

static int zchx_Connect(char* ip, unsigned int port, struct timeval* Timeout) {
    return fs_Socket_tcpIPv4_connect_byStringIP__IO(ip, port, 1, Timeout->tv_sec + Timeout->tv_usec / 1000000.0);
}
//断开

static int zchx_Disconnect(int socket) {
    close(socket);
    return 1;
}
//发送数据

static bool zchx_sendmsg(int socket, char* nCmd, UINT nCmdLen, struct timeval* time_out) {
    return fs_Socket_tcpIPv4_send_by_sock(socket, nCmdLen, nCmd, time_out->tv_sec + time_out->tv_usec / 1000000.0) == 1 ? true : false;
}

void switchStr(char* hex) {
    char p[512];
    memset(p, 0, 512);
    switch (strlen(hex)) {
        case 1:
            memset(p, '0', 7);
            strcat(p, hex);
            break;
        case 2:
            memset(p, '0', 6);
            strcat(p, hex);
            break;
        case 3:
            memset(p, '0', 5);
            strcat(p, hex);
            break;
        case 4:
            memset(p, '0', 4);
            strcat(p, hex);
            break;
        case 5:
            memset(p, '0', 3);
            strcat(p, hex);
            break;
        case 6:
            memset(p, '0', 2);
            strcat(p, hex);
            break;
        case 7:
            memset(p, '0', 1);
            strcat(p, hex);
            break;
    }

    memcpy(hex, p, 8);
    //return hex;

}

char * mid(char *dst, char *src, int n, int m) /*n为长度，m为位置*/ {
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if (n > len) n = len - m; /*从第m个到最后*/
    if (m < 0) m = 0; /*从第一个开始*/
    if (m > len) return NULL;
    p += m;
    while (n--) *(q++) = *(p++);
    *(q++) = '\0'; /*有必要吗？很有必要*/
    return dst;
}

int convert(char *str) {
    int i, sum = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9')
            sum += sum * 16 + str[i] - '0';
        else if (str[i] <= 'f' && str[i] >= 'a')
            sum += sum * 16 + str[i] - 'a' + 1;
        else if (str[i] <= 'F' && str[i] >= 'A')
            sum += sum * 16 + str[i] - 'A' + 1;
    }
    return sum;
}

/* 
 * 将字符转换为数值 
 * */
int c2i(char ch) {
    // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2  
    if (isdigit(ch))
        return ch - 48;

    // 如果是字母，但不是A~F,a~f则返回  
    if (ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z')
        return -1;

    // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10  
    // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10  
    if (isalpha(ch))
        return isupper(ch) ? ch - 55 : ch - 87;

    return -1;
}

/* 
 * 功能：将十六进制字符串转换为整型(int)数值 
 * */
int hex2dec(char *hex) {
    int len;
    int num = 0;
    int temp;
    int bits;
    int i;

    // 此例中 hex = "1de" 长度为3, hex是main函数传递的  
    len = strlen(hex);

    for (i = 0, temp = 0; i < len; i++, temp = 0) {
        // 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1  
        // 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13  
        // 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14  
        temp = c2i(*(hex + i));
        // 总共3位，一个16进制位用 4 bit保存  
        // 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位  
        // 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位  
        // 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位  
        bits = (len - i - 1) * 4;
        temp = temp << bits;

        // 此处也可以用 num += temp;进行累加  
        num = num | temp;
    }

    // 返回结果  
    return num;
}

//向控制器发送数据

int sendStop2Camcontrol_1(struct Vtrack_item *pTrack_item) {

    if (pTrack_item->ro.m_camcrl_requesID_3[0] == 0 && m_camcrl_requesID_3[0] == 0) {
        printf(">>>>>pTrack_item->ro.m_camcrl_requesID_3 is null<<<<<<<\n", __FUNCTION__, __LINE__);
        return 0;
    } else if (pTrack_item->ro.m_camcrl_requesID_3[0] == 0 && m_camcrl_requesID_3[0] != 0) {
        pTrack_item->ro.m_camcrl_head = m_camcrl_head;
        memcpy(pTrack_item->ro.m_camcrl_requesID_3, m_camcrl_requesID_3, 3 * sizeof (int));
        pTrack_item->ro.m_camcrl_requesID_data[0] = m_camcrl_requesID_data[0];
        pTrack_item->ro.m_camcrl_requesID_data[1] = m_camcrl_requesID_data[1];
        pTrack_item->ro.m_camcrl_requesID_data[2] = m_camcrl_requesID_data[2];
        pTrack_item->ro.m_camcrl_requesID_data[3] = m_camcrl_requesID_data[3];
        pTrack_item->ro.m_camcrl_requesID_data[4] = m_camcrl_requesID_data[4];
        printf(">>>>>pTrack_item->ro.m_camcrl_requesID_3 is null，but m_camcrl_requesID_3 is not null<<<<<<<\n", __FUNCTION__, __LINE__);
    }


    //向控制器发送目标锁定指令
    char temp[512];
    FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "track");
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
    sprintf(temp, "%s", pTrack_item->ro._uuid);
    fs_Ebml_node_data_set_string(pEbml_node, temp);
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "stop_radarguide_track");
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "1");

    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
    sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
    fs_Ebml_node_data_set_string(pEbml_node, temp);

    if (pTrack_item->rw.v_videoPath[0] == 0 || pTrack_item->rw.v_videoPath_1[0] == 0) {
        //返回跟踪录像路径名
        time_t ts = (time_t) fs_time_GMT_get();
        struct tm ti_s;
        localtime_r(&ts, &ti_s);
        sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

        sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
    }

    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
    sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
    fs_Ebml_node_data_set_string(pEbml_node, temp);


    FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
    fs_Ebml_delete__OI(rst, NULL);

    //FsXml *pXml = fs_Ebml_convert_to_Xml__IO(rst);
    FsObjectBase * pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
    fs_Json_delete__OI(pJson, NULL);
    pObjectBase->data -= 8;
    pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
    ((unsigned int *) pObjectBase->data)[0] = (pTrack_item->ro.m_camcrl_head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
    memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), pTrack_item->ro.m_camcrl_requesID_3, sizeof (unsigned int)*3);
    configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);

    printf(">>>>>>>>>>>>>>>>>>>>>send m_camcrl_head:%d  m_camcrl_requesID_3:%d %d %d<<<<<<<<<<<<<<<<<<<<\n", pTrack_item->ro.m_camcrl_head, pTrack_item->ro.m_camcrl_requesID_3[0], pTrack_item->ro.m_camcrl_requesID_3[1], pTrack_item->ro.m_camcrl_requesID_3[2]);

    return 0;
}

/******************************************************************************
 *                           向控制器发结束跟踪指令
 * 
 *  int type: 2 
 * 
 **********************************************************************************/
int sendStop2Camcontrol(struct Vtrack_item *pTrack_item, int type) {

    if (pTrack_item->ro.m_camcrl_requesID_3[0] == 0 && m_camcrl_requesID_3[0] == 0) {
        printf(">>>>>pTrack_item->ro.m_camcrl_requesID_3 is null<<<<<<<\n", __FUNCTION__, __LINE__);
        return 0;
    } else if (pTrack_item->ro.m_camcrl_requesID_3[0] == 0 && m_camcrl_requesID_3[0] != 0) {
        pTrack_item->ro.m_camcrl_head = m_camcrl_head;
        memcpy(pTrack_item->ro.m_camcrl_requesID_3, m_camcrl_requesID_3, 3 * sizeof (int));
        pTrack_item->ro.m_camcrl_requesID_data[0] = m_camcrl_requesID_data[0];
        pTrack_item->ro.m_camcrl_requesID_data[1] = m_camcrl_requesID_data[1];
        pTrack_item->ro.m_camcrl_requesID_data[2] = m_camcrl_requesID_data[2];
        pTrack_item->ro.m_camcrl_requesID_data[3] = m_camcrl_requesID_data[3];
        pTrack_item->ro.m_camcrl_requesID_data[4] = m_camcrl_requesID_data[4];
        printf(">>>>>pTrack_item->ro.m_camcrl_requesID_3 is null，but m_camcrl_requesID_3 is not null<<<<<<<\n", __FUNCTION__, __LINE__);
    }


    //向控制器发送目标锁定指令
    char temp[512];
    FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "track");
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
    sprintf(temp, "%s", pTrack_item->ro._uuid);
    fs_Ebml_node_data_set_string(pEbml_node, temp);
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "stop_video_track");

    if (type == 2) {
        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "1");
    } else {
        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "0");
    }

    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
    sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
    fs_Ebml_node_data_set_string(pEbml_node, temp);

    if (pTrack_item->rw.v_videoPath[0] == 0 || pTrack_item->rw.v_videoPath_1[0] == 0) {
        //返回跟踪录像路径名
        time_t ts = (time_t) fs_time_GMT_get();
        struct tm ti_s;
        localtime_r(&ts, &ti_s);
        sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

        sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
    }

    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
    sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
    fs_Ebml_node_data_set_string(pEbml_node, temp);


    FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
    fs_Ebml_delete__OI(rst, NULL);

    //FsXml *pXml = fs_Ebml_convert_to_Xml__IO(rst);
    FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
    fs_Json_delete__OI(pJson, NULL);
    pObjectBase->data -= 8;
    pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
    ((unsigned int *) pObjectBase->data)[0] = (pTrack_item->ro.m_camcrl_head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
    memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), pTrack_item->ro.m_camcrl_requesID_3, sizeof (unsigned int)*3);
    configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);


    printf(">>>>>>>>>>>>>>>>>>>>>send m_camcrl_head:%d  m_camcrl_requesID_3:%d %d %d<<<<<<<<<<<<<<<<<<<<\n", pTrack_item->ro.m_camcrl_head, pTrack_item->ro.m_camcrl_requesID_3[0], pTrack_item->ro.m_camcrl_requesID_3[1], pTrack_item->ro.m_camcrl_requesID_3[2]);

    return 0;


    if (0) {
        struct timeval tv;
        int socket = 0;
        tv.tv_sec = 1;
        tv.tv_usec = 200 * 1000;
        socket = zchx_Connect("192.168.8.214", 8888, &tv);
        if (socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }

        unsigned int sendindex = 0;

        FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
        fs_Ebml_delete__OI(rst, NULL);

        FsString * pString = fs_Json_to_String__IO(pJson, 8, 0, FsStringLenData("\r\n"), 0, NULL, 4);
        fs_Json_delete__OI(pJson, NULL);

        //8个字节头
        //前4个字节，"aaaaaaaa"
        pString->buffer[0] = -86;
        pString->buffer[1] = -86;
        pString->buffer[2] = -86;
        pString->buffer[3] = -86;

        //后4个字节，数据长度
        //后4个字节，数据长度
        int buflen = pString->lenth - 8;
        char ch[215] = {0};
        sprintf(ch, "%X", buflen);
        switchStr(ch); // 将其拼接为8位
        int num = 0;
        for (int i = 0; i < 4; i++) {

            char temp[2] = {0};
            mid(temp, ch, 2, num);
            num++;
            num++;
            int nn = hex2dec(temp);

            pString->buffer[4 + i] = (char) nn;
        }


        if (zchx_sendmsg(socket, pString->buffer, pString->lenth, &tv)) {
            //printf("%s:%d  zchx_sendmsg stop track success !! \n",__FUNCTION__,__LINE__);

        } else {
            printf("%s:%d  zchx_sendmsg fails !! \n", __FUNCTION__, __LINE__);
        }

        fs_String_delete__OI(pString);
        zchx_Disconnect(socket);
    }

}

//向控制器发送数据

int sendlock2Camcontrol(struct Vtrack_item *pTrack_item) {

    if (pTrack_item->ro.m_camcrl_requesID_3[0] == 0 && m_camcrl_requesID_3[0] == 0) {
        printf(">>>>>pTrack_item->ro.m_camcrl_requesID_3 is null<<<<<<<\n", __FUNCTION__, __LINE__);
        return 0;
    } else if (pTrack_item->ro.m_camcrl_requesID_3[0] == 0 && m_camcrl_requesID_3[0] != 0) {
        pTrack_item->ro.m_camcrl_head = m_camcrl_head;
        memcpy(pTrack_item->ro.m_camcrl_requesID_3, m_camcrl_requesID_3, 3 * sizeof (int));
        pTrack_item->ro.m_camcrl_requesID_data[0] = m_camcrl_requesID_data[0];
        pTrack_item->ro.m_camcrl_requesID_data[1] = m_camcrl_requesID_data[1];
        pTrack_item->ro.m_camcrl_requesID_data[2] = m_camcrl_requesID_data[2];
        pTrack_item->ro.m_camcrl_requesID_data[3] = m_camcrl_requesID_data[3];
        pTrack_item->ro.m_camcrl_requesID_data[4] = m_camcrl_requesID_data[4];
        printf(">>>>>pTrack_item->ro.m_camcrl_requesID_3 is null，but m_camcrl_requesID_3 is not null<<<<<<<\n", __FUNCTION__, __LINE__);
    }


    //返回相机状态：
    char temp[54];
    FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "track");
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
    sprintf(temp, "%s", pTrack_item->ro._uuid);
    fs_Ebml_node_data_set_string(pEbml_node, temp);
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
    fs_Ebml_node_data_set_string(pEbml_node, "lock_video_track");
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
    sprintf(temp, "%d", pTrack_item->rw.v_findObj);
    fs_Ebml_node_data_set_string(pEbml_node, temp);
    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
    sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
    fs_Ebml_node_data_set_string(pEbml_node, temp);

    if (pTrack_item->rw.v_videoPath[0] == 0 || pTrack_item->rw.v_videoPath_1[0] == 0) {
        //返回跟踪录像路径名
        time_t ts = (time_t) fs_time_GMT_get();
        struct tm ti_s;
        localtime_r(&ts, &ti_s);
        sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

        sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
    }

    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
    sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
    fs_Ebml_node_data_set_string(pEbml_node, temp);

    FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
    fs_Ebml_delete__OI(rst, NULL);

    //FsXml *pXml = fs_Ebml_convert_to_Xml__IO(rst);
    FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
    fs_Json_delete__OI(pJson, NULL);
    pObjectBase->data -= 8;
    pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
    ((unsigned int *) pObjectBase->data)[0] = (pTrack_item->ro.m_camcrl_head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
    memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), pTrack_item->ro.m_camcrl_requesID_3, sizeof (unsigned int)*3);
    configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);

    printf(">>>>>>>>>>>>>>>>>>>>>send m_camcrl_head:%d  m_camcrl_requesID_3:%d %d %d<<<<<<<<<<<<<<<<<<<<\n", pTrack_item->ro.m_camcrl_head, pTrack_item->ro.m_camcrl_requesID_3[0], pTrack_item->ro.m_camcrl_requesID_3[1], pTrack_item->ro.m_camcrl_requesID_3[2]);

    return 0;



    if (0) {
        struct timeval tv;
        int socket = 0;
        tv.tv_sec = 1;
        tv.tv_usec = 200 * 1000;
        socket = zchx_Connect("192.168.8.214", 8888, &tv);
        if (socket <= 0) {
            printf("(%s:%d) cam connect error!\n", __FUNCTION__, __LINE__);
            return -1;
        }


        //向控制器发送目标锁定指令
        char temp[512];
        FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "track");
        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
        sprintf(temp, "%s", pTrack_item->ro._uuid);
        fs_Ebml_node_data_set_string(pEbml_node, temp);
        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "lock_video_track");
        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
        fs_Ebml_node_data_set_string(pEbml_node, "2");

        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
        sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        if (pTrack_item->rw.v_videoPath[0] == 0 || pTrack_item->rw.v_videoPath_1[0] == 0) {
            //返回跟踪录像路径名
            time_t ts = (time_t) fs_time_GMT_get();
            struct tm ti_s;
            localtime_r(&ts, &ti_s);
            sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                    pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

            sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                    pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
        }

        pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
        sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
        fs_Ebml_node_data_set_string(pEbml_node, temp);

        unsigned int sendindex = 0;

        FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
        fs_Ebml_delete__OI(rst, NULL);

        FsString * pString = fs_Json_to_String__IO(pJson, 8, 0, FsStringLenData("\r\n"), 0, NULL, 4);
        fs_Json_delete__OI(pJson, NULL);

        //8个字节头
        //前4个字节，"aaaaaaaa"
        pString->buffer[0] = -86;
        pString->buffer[1] = -86;
        pString->buffer[2] = -86;
        pString->buffer[3] = -86;

        //后4个字节，数据长度
        int buflen = pString->lenth - 8;
        char ch[215] = {0};
        sprintf(ch, "%X", buflen);
        switchStr(ch); // 将其拼接为8位
        int num = 0;
        for (int i = 0; i < 4; i++) {

            char temp[2] = {0};
            mid(temp, ch, 2, num);
            num++;
            num++;
            int nn = hex2dec(temp);

            pString->buffer[4 + i] = (char) nn;
        }


        if (zchx_sendmsg(socket, pString->buffer, pString->lenth, &tv)) {
            //printf("%s:%d  zchx_sendmsg  lock success(buflen:%d %c %c %c %c) !! \n",__FUNCTION__,__LINE__,buflen,pString->buffer[4],pString->buffer[5],pString->buffer[6],pString->buffer[7]);

        } else {
            printf("%s:%d  zchx_sendmsg fails !! \n", __FUNCTION__, __LINE__);
        }

        fs_String_delete__OI(pString);
        zchx_Disconnect(socket);
    }
}


////////////////////////////////////////////////////////////////////////////////////////
//联动控制-连接云台(二次连接，第一次连接失败后，再次连接，然后返回)
//
//返回值：< 0 : 连接失败；  >= 0：连接成功

static int linkage_ctrl_connect(struct Vtrack_item *pVtrack_item) {

    return 0;

    /*
        int conn = 0;
        if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1 || pVtrack_item->ro.m_config_devinfo.m_camera_brand == 2) {

            conn = camera_ctrl_connect(pVtrack_item);
            if (conn < 0) {
                FsPrintf(1, "ZGXLIGHT connect fails:(ip:%s port:%d),try again....\n",
                        pVtrack_item->ro.m_config_holder.m_holder_ip, pVtrack_item->ro.m_config_holder.m_holder_port);
                fflush(stdout);
                char log[256];
                sprintf(log, "ZGXLIGHT connect fails:(ip:%s port:%d),try again....\n",
                        pVtrack_item->ro.m_config_holder.m_holder_ip, pVtrack_item->ro.m_config_holder.m_holder_port);
                MakeLogRecord(log, pVtrack_item);
                usleep(1000 * 20);
                conn = camera_ctrl_connect(pVtrack_item);
                if (conn < 0) {
                    FsPrintf(1, "ZGXLIGHT connect fails:(ip:%s port:%d),give up....\n",
                            pVtrack_item->ro.m_config_holder.m_holder_ip, pVtrack_item->ro.m_config_holder.m_holder_port);
                    fflush(stdout);
                    char log[256];
                    sprintf(log, "ZGXLIGHT connect fails:(ip:%s port:%d),give up....\n",
                            pVtrack_item->ro.m_config_holder.m_holder_ip, pVtrack_item->ro.m_config_holder.m_holder_port);
                    MakeLogRecord(log, pVtrack_item);
                }
            }
        } else if(pVtrack_item->ro.m_config_devinfo.m_camera_brand == 0){
            conn = hiki_camera_ctrl_connect(pVtrack_item);
            if (conn < 0) {
                FsPrintf(1, "HIKVISION connect fails,try again: m_nUserID:%d ip:%s port:%d Uid:%s pwd:%s.m_nControlChannel:%d.\n", pVtrack_item->rw.m_hikiCamera.m_nUserID,
                        pVtrack_item->rw.m_hikiCamera.m_szCameraIP, pVtrack_item->rw.m_hikiCamera.m_nCameraPort, pVtrack_item->rw.m_hikiCamera.m_szCameraUid,
                        pVtrack_item->rw.m_hikiCamera.m_szCameraPwd, pVtrack_item->rw.m_hikiCamera.m_nControlChannel);
                fflush(stdout);
                char log[256];
                sprintf(log, "HIKVISION connect fails,,try again: m_nUserID:%d ip:%s port:%d Uid:%s pwd:%s.m_nControlChannel:%d.\n", pVtrack_item->rw.m_hikiCamera.m_nUserID,
                        pVtrack_item->rw.m_hikiCamera.m_szCameraIP, pVtrack_item->rw.m_hikiCamera.m_nCameraPort, pVtrack_item->rw.m_hikiCamera.m_szCameraUid,
                        pVtrack_item->rw.m_hikiCamera.m_szCameraPwd, pVtrack_item->rw.m_hikiCamera.m_nControlChannel);
                MakeLogRecord(log, pVtrack_item);

                usleep(1000 * 20);
                conn = hiki_camera_ctrl_connect(pVtrack_item);
                if (conn < 0) {
                    FsPrintf(1, "HIKVISION connect fails,give up: m_nUserID:%d ip:%s port:%d Uid:%s pwd:%s.m_nControlChannel:%d.\n", pVtrack_item->rw.m_hikiCamera.m_nUserID,
                            pVtrack_item->rw.m_hikiCamera.m_szCameraIP, pVtrack_item->rw.m_hikiCamera.m_nCameraPort, pVtrack_item->rw.m_hikiCamera.m_szCameraUid,
                            pVtrack_item->rw.m_hikiCamera.m_szCameraPwd, pVtrack_item->rw.m_hikiCamera.m_nControlChannel);
                    fflush(stdout);
                    char log[256];
                    sprintf(log, "HIKVISION connect fails,give up: m_nUserID:%d ip:%s port:%d Uid:%s pwd:%s.m_nControlChannel:%d.\n", pVtrack_item->rw.m_hikiCamera.m_nUserID,
                            pVtrack_item->rw.m_hikiCamera.m_szCameraIP, pVtrack_item->rw.m_hikiCamera.m_nCameraPort, pVtrack_item->rw.m_hikiCamera.m_szCameraUid,
                            pVtrack_item->rw.m_hikiCamera.m_szCameraPwd, pVtrack_item->rw.m_hikiCamera.m_nControlChannel);
                    MakeLogRecord(log, pVtrack_item);
                }
            }
        }

        return conn;
     */
}

//联动控制-断开云台

static int linkage_ctrl_disconnect(struct Vtrack_item *pVtrack_item) {
    /*
        if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
            camera_ctrl_disconnect(pVtrack_item);
        } else {
            //测试，海康球机在程序启动后，不主动断开
            //hiki_camera_ctrl_disconnect(pVtrack_item);
        }
     */
    return 0;
}

//联动控制-向左右转动

static int linkage_ctrl_moveLeftRight(struct Vtrack_item *pVtrack_item) {

    if (pVtrack_item->rw.v_camctrl_dir == 2) {
        pVtrack_item->rw.pCamera_ctrl_item->rw.status_set = 4; //cam_p = CAM_PARAM_DIRECT_RIGHT;
    } else if (pVtrack_item->rw.v_camctrl_dir == 1) {
        pVtrack_item->rw.pCamera_ctrl_item->rw.status_set = 3; //cam_p = CAM_PARAM_DIRECT_LEFT;
    }
    pVtrack_item->rw.v_camctrl_movingX = 1;

    //调用相机控制
    int ret = camera_ctrl_tcp_setParam(pVtrack_item->rw.pCamera_ctrl_item, pVtrack_item->ro.m_config_holder.m_holder_speed);
    if (ret != 0) {
        printf("%s %d : setParam Failed \n", __FUNCTION__, __LINE__);
    }

    //    if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
    //        camera_ctrl_moveLeftRight(pVtrack_item);
    //    } else {
    //        hiki_camera_ctrl_moveLeftRight(pVtrack_item);
    //    }
}

//联动控制-停止左右转动

static int linkage_ctrl_stopMoveLeftRight(struct Vtrack_item *pVtrack_item) {


    pVtrack_item->rw.pCamera_ctrl_item->rw.status_set = 2; //cam_p = CAM_PARAM_DIRECT_LEFT;
    pVtrack_item->rw.v_camctrl_movingX = 0;

    //调用相机控制
    int ret = camera_ctrl_tcp_setParam(pVtrack_item->rw.pCamera_ctrl_item, pVtrack_item->ro.m_config_holder.m_holder_speed);
    if (ret != 0) {
        printf("%s %d : setParam Failed \n", __FUNCTION__, __LINE__);
    }

    /*
    if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
        int re = camera_ctrl_stopMoveLeftRight(pVtrack_item);
        if (re == -1) {
            camera_ctrl_stopMoveLeftRight(pVtrack_item);
        }
    } else {
        int re = hiki_camera_ctrl_stopMoveLeftRight(pVtrack_item);
        if (re == -1) {
            re = hiki_camera_ctrl_stopMoveLeftRight(pVtrack_item);
            if (re == -1) {
                pVtrack_item->rw.v_hiki_unConnect = 1;
            }
        }
    }
     */
}

//联动控制-向上下转动

static int linkage_ctrl_moveUpDown(struct Vtrack_item *pVtrack_item) {

    if (pVtrack_item->rw.v_camctrl_dirY == 2)
        pVtrack_item->rw.pCamera_ctrl_item->rw.status_set = 6; //cam_p = CAM_PARAM_DIRECT_DOWN;
    else if (pVtrack_item->rw.v_camctrl_dirY == 1)
        pVtrack_item->rw.pCamera_ctrl_item->rw.status_set = 5; //cam_p = CAM_PARAM_DIRECT_UP;

    pVtrack_item->rw.v_camctrl_movingY = 1;

    //调用相机控制
    int ret = camera_ctrl_tcp_setParam(pVtrack_item->rw.pCamera_ctrl_item, pVtrack_item->ro.m_config_holder.m_holder_speed);
    if (ret != 0) {
        printf("%s %d : setParam Failed \n", __FUNCTION__, __LINE__);
    }

    /*
    if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
        camera_ctrl_moveUpDown(pVtrack_item);
    } else {
        hiki_camera_ctrl_moveUpDown(pVtrack_item);
    }
     */
}

//联动控制-向上下转动

static int linkage_ctrl_stopMoveUpDown(struct Vtrack_item *pVtrack_item) {

    pVtrack_item->rw.pCamera_ctrl_item->rw.status_set = 2; //cam_p = CAM_PARAM_DIRECT_LEFT;
    pVtrack_item->rw.v_camctrl_movingY = 0;

    //调用相机控制
    int ret = camera_ctrl_tcp_setParam(pVtrack_item->rw.pCamera_ctrl_item, pVtrack_item->ro.m_config_holder.m_holder_speed);
    if (ret != 0) {
        printf("%s %d : setParam Failed \n", __FUNCTION__, __LINE__);
    }

    /*
    if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
        int re = camera_ctrl_stopMoveUpDown(pVtrack_item);
        if (re == -1) {
            camera_ctrl_stopMoveUpDown(pVtrack_item);
        }
    } else {
        hiki_camera_ctrl_stopMoveUpDown(pVtrack_item);
    }
     */
}

/************************************************************************
 *********************联动跟踪-转向设定的ptz位置***********************
 * type == 1: 云台转向雷达目标ptz位置
 * type == 0; 云台转向预置位ptz位置
 * type == 2; 云台转向当前ptz位置
 *************************************************************************/
static int linkage_ctrl_goPTZ(struct Vtrack_item *pVtrack_item, int type) {

    int success = 0;
    float p = 0.0;
    float t = 0.0;
    float z = 0.0;

    if (type == 1) {
        p = pVtrack_item->rw.v_obj_p;
        t = pVtrack_item->rw.v_obj_t;
        z = pVtrack_item->rw.v_obj_z;
    } else if (type == 0) {
        p = pVtrack_item->ro.m_config_holder.m_p;
        t = pVtrack_item->ro.m_config_holder.m_t;
        z = pVtrack_item->ro.m_config_holder.m_z;
    } else if (type == 2) {
        p = pVtrack_item->rw.v_current_p;
        t = pVtrack_item->rw.v_current_t;
        z = pVtrack_item->rw.v_current_z;
    } else {
        FsPrintf(1, "%s:%d, unknown type:%d \n", __FUNCTION__, __LINE__, type);
        fflush(stdout);
        return -1;
    }

    int ret = camera_ctrl_tcp_goPTZ(pVtrack_item->rw.pCamera_ctrl_item, p, t, z);
    if (ret != 0) {
        printf("%s %d : setParam Failed \n", __FUNCTION__, __LINE__);
    }


    /*
    if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
        printf(">>>>>>>>guide2 (p:%lf t:%lf z:%lf)<<<<<<<<<<<<\n", p, t, z);
        success = zgx_ctrl_goPTZ(pVtrack_item, p, t, z);
    } else {
        //红光球机
        if (pVtrack_item->ro.m_config_devinfo.m_camera_model == 1) {
            hiki_camera_ctrl_Preset_Opt_Goto(pVtrack_item);

        } else {
            success = hiki_ctrl_goPTZ(pVtrack_item, p, t, z);
        }
    }
     */

    //    FsPrintf(1, "linkage_ctrl_goPTZ,m_holder_type:%d m_camera_model:%d type:%d \n", pVtrack_item->ro.m_config_holder.m_holder_type,
    //            pVtrack_item->ro.m_config_devinfo.m_camera_model,type);
    //    fflush(stdout);

    return success;
}

//获取相机的当前ptz值

static int linkage_ctrl_getPTZ(struct Vtrack_item *pVtrack_item) {

    pVtrack_item->rw.v_current_p = 0.0;
    pVtrack_item->rw.v_current_t = 0.0;
    pVtrack_item->rw.v_current_z = 0.0;

    int ret = camera_ctrl_tcp_getPTZ(pVtrack_item->rw.pCamera_ctrl_item, &pVtrack_item->rw.v_current_p, &pVtrack_item->rw.v_current_t, &pVtrack_item->rw.v_current_z);
    if (ret != 0) {
        printf("%s %d : setParam Failed \n", __FUNCTION__, __LINE__);
    }

    return ret;

    /*
    int success = 0;
    if (pVtrack_item->ro.m_config_holder.m_holder_type == 1 || pVtrack_item->ro.m_config_holder.m_holder_type == 3) {
        //success = cmaera_ctrl_goPTZ_2(pVtrack_item);
        success = _camera_get_ptz(pVtrack_item, &pVtrack_item->rw.v_current_p, &pVtrack_item->rw.v_current_t, &pVtrack_item->rw.v_current_z);

    } else if (pVtrack_item->ro.m_config_holder.m_holder_type == 2) {
        //success = hiki_cmaera_ctrl_goPTZ_2(pVtrack_item);
        success = hiki_GetPtzParam(pVtrack_item, &pVtrack_item->rw.v_current_p, &pVtrack_item->rw.v_current_t, &pVtrack_item->rw.v_current_z);
    }

    return success;
     */
}
////////////////////////////////////////////////////////////////////////////////////////////

void DrawTrackRect(char* pData, int width, int height, Vtrack_item *pVtrack_item, int l, int r, int t, int b) {
    // Y’ = 0.257*R' + 0.504*G' + 0.098*B' +   16
    //  Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
    //  Cr' = 0.439*R' - 0.368*G' - 0.071*B' +  128
    //如果需要叠加跟踪框
    if (pVtrack_item->ro.m_config_overly.m_overly_line != 0) {
        unsigned char Y, Cb, Cr;
        int color = pVtrack_item->ro.m_config_overly.m_overly_linecolor;

        //red：255 0 0
        if (color == 1) {
            Y = 0.257 * 255 + 16;
            Cb = -0.148 * 255 + 128;
            Cr = 0.439 * 255 + 128;
        }
        //green:0 255 0
        if (color == 2) {
            Y = 0.504 * 255 + 16;
            Cb = -0.291 * 255 + 128;
            Cr = -0.368 * 255 + 128;
        }
        //blue: 0 0 255
        if (color == 3) {
            Y = 0.098 * 255 + 16;
            Cb = 0.439 * 255 + 128;
            Cr = -0.071 * 255 + 128;
        }

        tag_ITG_RECT m_rect;
        m_rect.left = l;
        m_rect.right = r;
        m_rect.top = t;
        m_rect.bottom = b;

        if (m_rect.left > m_rect.right) {
            m_rect.right = m_rect.left + 20;
        }
        if (m_rect.bottom < m_rect.top) {
            m_rect.bottom = m_rect.top + 20;
        }

        if (m_rect.left < 10) m_rect.left = 10;
        if (m_rect.left > width - 10) m_rect.left = width - 10;
        if (m_rect.top < 10) m_rect.top = 10;
        if (m_rect.top > height - 10) m_rect.top = height - 10;
        if (m_rect.right < 10) m_rect.right = 10;
        if (m_rect.right > width - 10) m_rect.right = width - 10;
        if (m_rect.bottom < 10) m_rect.bottom = 10;
        if (m_rect.bottom > height - 10) m_rect.bottom = height - 10;

        int rate = pVtrack_item->ro.m_config_overly.m_overly_linerate;
        int LineWidth = pVtrack_item->ro.m_config_overly.m_overly_linewidth;

        unsigned char* p_u = (unsigned char *) pData + width*height;
        unsigned char* p_v = (unsigned char *) pData + width * height * 5 / 4;

        ITG_DrawRect_y((char *) pData, (char *) p_u, (char *) p_v, width, &m_rect, Y, Cb, Cr, rate, LineWidth);
        ITG_DrawRect_YUV((char *) pData, (char *) p_u, (char *) p_v, width, height, &m_rect, Y, Cb, Cr, rate, LineWidth);
    }


    //如果需要叠加目标跟踪编号
    if (pVtrack_item->ro.m_config_overly.m_overly_tracknum != 0) {

        if (pVtrack_item->ro.__pTypeFace) {

            unsigned char* p_y = (unsigned char *) pData;
            unsigned char* p_u = (unsigned char *) pData + width*height;
            unsigned char* p_v = (unsigned char *) pData + width * height * 5 / 4;

            int startx, starty;
            char overlydata[256];
            sprintf(overlydata, "%lld", pVtrack_item->rw.v_trackNum);

            int count = 0;
            long long num = pVtrack_item->rw.v_trackNum;
            if (num > 0) {

                while (num / 10 > 0) {
                    count++;
                    num = num / 10;
                }
                count++; //首位也要加进去
            }

            //顶部
            if (pVtrack_item->ro.m_config_overly.m_overly_tracknum_pos == 1) {
                startx = (l + r) / 2 - count * pVtrack_item->ro.m_config_overly.m_overly_tracknum_width / 2;
                starty = t - pVtrack_item->ro.m_config_overly.m_overly_tracknum_height / 2;
            }//中心
            else if (pVtrack_item->ro.m_config_overly.m_overly_tracknum_pos == 2) {
                startx = (l + r) / 2 - count * pVtrack_item->ro.m_config_overly.m_overly_tracknum_width / 2;
                starty = (t + b) / 2 - pVtrack_item->ro.m_config_overly.m_overly_tracknum_height / 2;
            }//底部
            else {
                startx = (l + r) / 2 - count * pVtrack_item->ro.m_config_overly.m_overly_tracknum_width / 2;
                starty = b - pVtrack_item->ro.m_config_overly.m_overly_tracknum_height / 2;
            }

            if (startx < 0) {
                startx = (l + r) / 2;
            }
            if (starty < 0) {
                starty = t;
            }

            if (startx < 0 || startx > width - 1 || starty < 0 || starty > height - 1) {
                printf("%s:%d  error data: startx:%d starty:%d width:%d height:%d l:%d,r:%d,t:%d,b:%d count:%d tracknum_width:%d tracknum_heigh:%d \n", __FUNCTION__, __LINE__,
                        startx, starty, width, height, l, r, t, b, count, pVtrack_item->ro.m_config_overly.m_overly_tracknum_width, pVtrack_item->ro.m_config_overly.m_overly_tracknum_height);
                return;
            }

            fs_TypeFace_write(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_y, width, height,
                    width, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 16);
            fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_u, width, height,
                    width >> 1, startx, starty, (pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 8)&0xFF);
            fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_v, width, height,
                    width >> 1, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color & 0xFF);

        } else {

        }
    }
}


//重置参数

/*
 *每次跟踪结束后，掉用此函数，将所有的中间值都重置为零(所有v_开头的变量)
 * 
 * 跟踪结束位置：
 *            1.在规定时间内找到不到合适运动块，结束跟踪
 *            2.目标开始跟踪后，满足跟踪结束条件判断
 */
static void resetParam(Vtrack_item *pVtrack_item) {

    pVtrack_item->rw.v_findObj = 0;
    pVtrack_item->rw.v_trackingObj = 0;
    pVtrack_item->rw.v_findmove = 0;

    pVtrack_item->rw.v_reduce_rate = 0.0;

    pVtrack_item->rw.v_hiki_unConnect = 0;

    pVtrack_item->rw.v_handleToStop = 0;

    pVtrack_item->rw.v_movedir = 0;
    pVtrack_item->rw.v_max_x = 0;
    pVtrack_item->rw.v_max_y = 0;

    pVtrack_item->rw.v_autofocus = 0;
    pVtrack_item->rw.v_assistfind = 0;
    pVtrack_item->rw.v_holder_lastConnectdx = 0;
    pVtrack_item->rw.v_holder_lastctrlDx = 0;
    pVtrack_item->rw.v_holder_lastmoveXDx = 0;
    pVtrack_item->rw.v_holder_lastmoveYDx = 0;
    pVtrack_item->rw.v_holder_incmove = 0;
    pVtrack_item->rw.v_holder_notmatchCnt = 0;


    pVtrack_item->rw.v_holder_p[0] = 0.0;
    pVtrack_item->rw.v_holder_p[1] = 0.0;
    pVtrack_item->rw.v_holder_t[0] = 0.0;
    pVtrack_item->rw.v_holder_t[1] = 0.0;
    pVtrack_item->rw.v_holder_z[0] = 0.0;
    pVtrack_item->rw.v_holder_z[1] = 0.0;
    pVtrack_item->rw.v_holder_getptzcnt = 0;

    pVtrack_item->rw.v_effective = 0;
    pVtrack_item->rw.v_boatheight = 0;
    pVtrack_item->rw.v_boat_left = 0;
    pVtrack_item->rw.v_boat_right = 0;
    pVtrack_item->rw.v_boat_bottom_y = 0;
    pVtrack_item->rw.v_boat_enterall = 0;
    pVtrack_item->rw.v_changeBoatpos_flag = 0;
    pVtrack_item->rw.v_boat_rect_l = 0;
    pVtrack_item->rw.v_boat_rect_r = 0;
    pVtrack_item->rw.v_boat_rect_t = 0;
    pVtrack_item->rw.v_boat_rect_b = 0;
    //pVtrack_item->rw.v_obj_number = 0;

    pVtrack_item->rw.v_camctrl_dir = 0;
    pVtrack_item->rw.v_camctrl_dirY = 0;
    pVtrack_item->rw.v_camctrl_movingX = 0;
    pVtrack_item->rw.v_camctrl_movingY = 0;

    pVtrack_item->rw.v_rightMoveSteps = 0;
    pVtrack_item->rw.v_rightMoveDistance = 0;
    pVtrack_item->rw.v_leftMoveSteps = 0;
    pVtrack_item->rw.v_leftMoveDistance = 0;
    pVtrack_item->rw.v_changeBoatpos_flag = 0;

    pVtrack_item->rw.v_adjustIndex = 0;
    pVtrack_item->rw.v_adjustpos = -1;
    pVtrack_item->rw.v_locktrackObj = -1;
    pVtrack_item->rw.v_waitforcamera = pVtrack_item->ro.m_config_track.m_trackdelay; //跟踪延迟时间(等待相机停止转动)

    pVtrack_item->rw.v_obj_number++; //每次跟踪结束，跟踪序号加一
    pVtrack_item->rw.v_trackobj_times = pVtrack_item->ro.m_config_track.m_trackAgainTimes; //跟踪次数

    pVtrack_item->rw.v_gotoptz = 0;

    pVtrack_item->rw.v_videoPath[0] = 0;
    pVtrack_item->rw.v_videoPath_1[0] = 0;

    pVtrack_item->rw.v_trackNum = 0;

    pVtrack_item->rw.m_water_grass_count = 0;

    memset(&pVtrack_item->rw.m_vsearch, 0, sizeof (vSeach));
    memset(&pVtrack_item->rw.g_vproc, 0, sizeof (vProc));

    int j;
    for (j = 0; j < MAX_OBJ; j++) {
        pVtrack_item->rw.pm[j].bUsed = 0;
        memset(&pVtrack_item->rw.pm[j], 0, sizeof (TrackParam)); //清零
    }

    FsPrintf(1, ".reset all the param(%s) v_trackNum:%lld..\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum);
    fflush(stdout);

    char message[256];
    sprintf(message, "(%s:%d)..reset all the param \n", __FUNCTION__, __LINE__);
    MakeLogRecord(message, pVtrack_item);
}



//重置参数

/*
 *目标跟丢之后。但是，还没有到达跟踪次数，调用此函数
 * 
 */
static void resetParam_1(Vtrack_item *pVtrack_item) {
    pVtrack_item->rw.v_findObj = 1;
    pVtrack_item->rw.v_trackingObj = 0;
    pVtrack_item->rw.v_findmove = 0;

    pVtrack_item->rw.v_reduce_rate = 0.0;

    pVtrack_item->rw.v_hiki_unConnect = 0;

    pVtrack_item->rw.v_handleToStop = 0;
    pVtrack_item->rw.v_autofocus = 0;
    pVtrack_item->rw.v_assistfind = 0;
    pVtrack_item->rw.v_movedir = 0;
    pVtrack_item->rw.v_max_x = 0;
    pVtrack_item->rw.v_max_y = 0;
    pVtrack_item->rw.v_holder_lastConnectdx = 0;
    pVtrack_item->rw.v_holder_lastctrlDx = 0;
    pVtrack_item->rw.v_holder_lastmoveXDx = 0;
    pVtrack_item->rw.v_holder_lastmoveYDx = 0;
    pVtrack_item->rw.v_holder_incmove = 0;
    pVtrack_item->rw.v_holder_notmatchCnt = 0;

    pVtrack_item->rw.v_holder_p[0] = 0.0;
    pVtrack_item->rw.v_holder_p[1] = 0.0;
    pVtrack_item->rw.v_holder_t[0] = 0.0;
    pVtrack_item->rw.v_holder_t[1] = 0.0;
    pVtrack_item->rw.v_holder_z[0] = 0.0;
    pVtrack_item->rw.v_holder_z[1] = 0.0;
    pVtrack_item->rw.v_holder_getptzcnt = 0;

    pVtrack_item->rw.v_effective = 0;
    pVtrack_item->rw.v_boatheight = 0;
    pVtrack_item->rw.v_boat_left = 0;
    pVtrack_item->rw.v_boat_right = 0;
    pVtrack_item->rw.v_boat_bottom_y = 0;
    pVtrack_item->rw.v_boat_enterall = 0;
    pVtrack_item->rw.v_changeBoatpos_flag = 0;
    pVtrack_item->rw.v_boat_rect_l = 0;
    pVtrack_item->rw.v_boat_rect_r = 0;
    pVtrack_item->rw.v_boat_rect_t = 0;
    pVtrack_item->rw.v_boat_rect_b = 0;
    //pVtrack_item->rw.v_obj_number = 0;

    pVtrack_item->rw.v_camctrl_dir = 0;
    pVtrack_item->rw.v_camctrl_dirY = 0;
    pVtrack_item->rw.v_camctrl_movingX = 0;
    pVtrack_item->rw.v_camctrl_movingY = 0;

    pVtrack_item->rw.v_rightMoveSteps = 0;
    pVtrack_item->rw.v_rightMoveDistance = 0;
    pVtrack_item->rw.v_leftMoveSteps = 0;
    pVtrack_item->rw.v_leftMoveDistance = 0;
    pVtrack_item->rw.v_changeBoatpos_flag = 0;

    pVtrack_item->rw.v_adjustIndex = 0;
    pVtrack_item->rw.v_adjustpos = -1;

    pVtrack_item->rw.v_locktrackObj = -1;
    //pVtrack_item->rw.v_obj_number++;                    //每次跟踪结束，跟踪序号加一
    //pVtrack_item->rw.v_trackobj_times = pVtrack_item->ro.m_config_track.m_trackAgainTimes;  //跟踪次数

    memset(&pVtrack_item->rw.m_vsearch, 0, sizeof (vSeach));
    memset(&pVtrack_item->rw.g_vproc, 0, sizeof (vProc));

    int j;
    for (j = 0; j < MAX_OBJ; j++) {
        pVtrack_item->rw.pm[j].bUsed = 0;
        memset(&pVtrack_item->rw.pm[j], 0, sizeof (TrackParam)); //清零
    }

    FsPrintf(1, ".reset all the param(%s) v_trackNum:%lld..\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum);
    fflush(stdout);

    char message[256];
    sprintf(message, "(%s:%d)..reset all the param \n", __FUNCTION__, __LINE__);
    MakeLogRecord(message, pVtrack_item);
}

/*
 * 当云台在设定时间内不转动：
 *      1.开始对图像做三帧差处理
 *      2.在跟踪框附近寻找运动块
 *      3.如果在设定帧数内都找不到运动块，就结束跟踪
 * 
 * 如果在处理(1-3步骤)的过程中，云台发生转动，则立即结束，相关参数清零
 */
void reSearchMotionblock(Vtrack_item *pVtrack_item, unsigned char *image, int sw, int sh, int frameindex) {

    int j;
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

            //如果云台在设定时间内都没有转动
            if ((pVtrack_item->rw.v_holder_lastctrlDx == 0 && pVtrack_item->rw.pm[j].track_count > 25 * pVtrack_item->ro.m_config_track.m_trackOverFlag3) ||
                    ((pVtrack_item->rw.v_holder_lastctrlDx > 0) && (frameindex - pVtrack_item->rw.v_holder_lastctrlDx) > 25 * pVtrack_item->ro.m_config_track.m_trackOverFlag3)) {

                //如果离上一次二次搜索运动块成功时的帧号相差大于阈值，则再次进行二次搜索
                if (frameindex - pVtrack_item->rw.m_vsearch.m_reFindMoveFramedx > 25 * pVtrack_item->ro.m_config_track.m_trackOverFlag3) {
                    //当运动检测没有初始化时，先进行初始化
                    if (pVtrack_item->rw.m_vsearch.m_bInitOK_1 == 0) {

                        int l = pVtrack_item->rw.pm[j].ol - pVtrack_item->ro.m_config_track.m_trackWidth;
                        int r = pVtrack_item->rw.pm[j].orr + pVtrack_item->ro.m_config_track.m_trackWidth;
                        int t = pVtrack_item->rw.pm[j].ot - pVtrack_item->ro.m_config_track.m_trackHeight;
                        int b = pVtrack_item->rw.pm[j].ob + pVtrack_item->ro.m_config_track.m_trackHeight;

                        if (l < 10) l = 10;
                        if (r > sw - 10) r = sw - 10;
                        if (t < 10) t = 10;
                        if (b > sh - 10) b = sh - 10;

                        reInitSearchObj(&pVtrack_item->rw.m_vsearch, sw, sh, &pVtrack_item->ro.m_config_search, l, t, r, b);
                    }

                    int ret = reSearchObjProc(&pVtrack_item->rw.m_vsearch, (unsigned char *) image, frameindex);

                    //在设定帧数内没有搜索到运动块
                    if (ret == -1) {

                        FsPrintf(1, "(%s:%d).framedx:%d  cannot find the moveobj..\n", __FUNCTION__, __LINE__, frameindex);
                        fflush(stdout);

                        pVtrack_item->rw.v_findmove = -1;
                    }//在设定帧数内搜索到运动块，
                    else if (ret == 1) {

                        FsPrintf(1, "(%s:%d).framedx:%d  find the moveobj..\n", __FUNCTION__, __LINE__, frameindex);
                        fflush(stdout);
                        pVtrack_item->rw.m_vsearch.m_bInitOK_1 = 0;
                        pVtrack_item->rw.m_vsearch.m_reFindMoveFramedx = frameindex;
                    } else if (ret == 0) {
                        //...
                    }

                } else {
                    //...
                }
            } else {
                pVtrack_item->rw.m_vsearch.m_bInitOK_1 = 0;
            }

        }
    }
}

//车辆检测+跟踪
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

void DectionCar(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame) {/*

    int sw = pFrame->width[2];
    int sh = pFrame->height[2];
                    
    //车道数
    int RoadNum = 4;
        
    //是否初始化
    if(pVtrack_item->rw.theCapInit == 0){
        pVtrack_item->rw.theCapInit = 1;
        
        //车道边沿线
        int Road_TopLeftX[4];
        int Road_TopLeftY[4];
        int Road_TopRightX[4];
        int Road_TopRightY[4];
        int Road_BottomLeftX[4];
        int Road_BottomLeftY[4];
        int Road_BottomRightX[4];
        int Road_BottomRightY[4];

        double Road_k1[4];
        double Road_k2[4];
        double Road_b1[4];
        double Road_b2[4];
        
        //车道底
        int theBottom[8];
        
        Road_TopLeftX[0] = 319;//384;//
	Road_TopLeftY[0] = 35;//370;//
	Road_TopRightX[0] = 336;//524;//
	Road_TopRightY[0] = 37;//524;//
	Road_BottomLeftX[0] = 10;//429;//
	Road_BottomLeftY[0] = 179;//510;//
	Road_BottomRightX[0] = 78;//608;//
	Road_BottomRightY[0] = 197;//476;//

	Road_TopLeftX[1] = 322;//223;//
	Road_TopLeftY[1] = 46;//360;//
	Road_TopRightX[1] = 349;//366;//
	Road_TopRightY[1] = 47;//366;//
	Road_BottomLeftX[1] = 81;//194;//
	Road_BottomLeftY[1] = 197;//510;//
	Road_BottomRightX[1] = 155;//411;//
	Road_BottomRightY[1] = 221;//510;//

	Road_TopLeftX[2] = 351;//384;//
	Road_TopLeftY[2] = 48;//370;//
	Road_TopRightX[2] = 379;//524;//
	Road_TopRightY[2] = 50;//524;//
	Road_BottomLeftX[2] = 156;//429;//
	Road_BottomLeftY[2] = 234;//510;//
	Road_BottomRightX[2] = 282;//608;//
	Road_BottomRightY[2] = 246;//476;//

	Road_TopLeftX[3] = 385;//32;//
	Road_TopLeftY[3] = 40;//350;//
	Road_TopRightX[3] = 408;//180;//
	Road_TopRightY[3] = 41;//180;//
	Road_BottomLeftX[3] = 292;//5;//
	Road_BottomLeftY[3] = 249;//386;//
	Road_BottomRightX[3] = 407;//132;//
	Road_BottomRightY[3] = 254;//500;//	
        
      
        
        int HEIGHT = sh;
        int WIDTH = sw;
        
        int k;
	for(int i=0;i<RoadNum;i++)
	{
            Road_k1[i] = 1.0*(Road_BottomLeftY[i]-Road_TopLeftY[i])/(Road_BottomLeftX[i]-Road_TopLeftX[i]);
            Road_b1[i] = 1.0*Road_BottomLeftY[i]-Road_k1[i]*Road_BottomLeftX[i];
            Road_k2[i] = 1.0*(Road_BottomRightY[i]-Road_TopRightY[i])/(Road_BottomRightX[i]-Road_TopRightX[i]);
            Road_b2[i] = 1.0*Road_BottomRightY[i]-Road_k2[i]*Road_BottomRightX[i];
	}
        
        int * theXleft[8];
	int * theXright[8];
        int theLane[8];
                
	for(k=0;k<RoadNum;k++)
	{
            theBottom[k] = 0;

            theXleft[k] = new int [HEIGHT];
            memset(theXleft[k],0,sizeof(int)*HEIGHT);

            theXright[k] = new int [HEIGHT];
            memset(theXright[k],0,sizeof(int)*HEIGHT);
	}
        
       //车道底部
        theBottom[0] = 80;
	theBottom[1] = 80;
	theBottom[2] = HEIGHT-30;
	theBottom[3] = HEIGHT-30;
        
        //车道类型
	theLane[0] = 4;	
	theLane[1] = 4;
	theLane[2] = 0;
	theLane[3] = 0;
        
        
	for(k=0;k<RoadNum;k++)
	{
            for(int j=0;j<HEIGHT;j++)
            {
                int x1 = 0;
                int x2 = 0;
                x1 = max(0,(j-Road_b1[k])/Road_k1[k]);
                x2 = min(WIDTH-1,(j-Road_b2[k])/Road_k2[k]);

                if(x2>x1)
                {
                    theXleft[k][j] = x1;
                    theXright[k][j] = x2;
                }
            }
	}
        
        int theFVehicleLen = 20;
        int theFVehicleWidth = 20;
        int theFMoveTH = 12;
        
        int theFLoop_length= theFVehicleLen/3;
        int theVehicleLen= 50;
        int theLoop_length =  theVehicleLen /3;
                
        int theheadLoop_length,headcap_position,thebackLoop_length,backcap_position;
	int thefheadLoop_length,fheadcap_position;

	thefheadLoop_length = theFLoop_length;
	fheadcap_position =  thefheadLoop_length/2;

	theheadLoop_length = 30;//theVehicleLen;
	headcap_position = theheadLoop_length/2;
	thebackLoop_length =  theLoop_length;
	backcap_position = thebackLoop_length/2;
        
        int theTopTH	= 30;
        int theLowTH	= 15;
        int theMoveTH   = 16;
        int theDay      = 1;
        int theVehicleWidth  = 35;

        //初始化
	pVtrack_item->rw.theCapture1.IniPara(RoadNum,theLane,WIDTH,HEIGHT,
				   theVehicleLen,theVehicleWidth,theFVehicleLen,theFVehicleWidth,
				   theheadLoop_length,headcap_position,thefheadLoop_length,fheadcap_position,
				   thebackLoop_length,backcap_position,theTopTH,theLowTH,theMoveTH,theFMoveTH,theBottom,theXleft,theXright,10,12);
        
        vTrackcar_create();
    }
    
    int theResult[8];
    int theResult_1[8];
    double theSpeed[8];
    int theSpeedPosition[8];
    int theX1[8];
    int theX2[8];
    int theY1[8];
    int theY2[8];
    int ToFlashBack[8];
    int theP1[8];
    int theP2[8];
        
    for(int k=0;k<8;k++)
    {
        theResult[k] = -1;
        theResult_1[k] = -1;
        theSpeed[k] = 0;

        theX1[k] = 0;
        theX2[k] = 0;
        theY1[k] = 0;
        theY2[k] = 0;
        theP1[k] = 0;
        theP2[k] = 0;
        ToFlashBack[k] = 0;
        theSpeedPosition[k] = 0;
    }
        
    pVtrack_item->rw.theCapture1.DetectionVehicle(pFrame->data.raw[2]->data,theResult,theX1,theX2,theY1,theY2,
            ToFlashBack,theSpeed,theSpeedPosition,theP1,theP2,1);

    int i;
    for (i = 0; i < RoadNum; i++) {
        if (theResult[i] == 0 || theResult[i] == 1) {
            printf("******************************\n");
            printf("#############get a car (i:%d)################\n",i);
            printf("******************************\n");            
        }
    }
    
    //
    for (i = 0; i < MAX_OBJ; i++) {
        if (pVtrack_item->rw.trackcarparam[i].bUsed == 1 && pVtrack_item->rw.trackcarparam[i].track_count < 100000) {
            pVtrack_item->rw.trackcarparam[i].nProcType = 1;
            pVtrack_item->rw.trackcarparam[i].frameIdx = pFrame->index;
            pVtrack_item->rw.trackcarparam[i].ol = 0;
            pVtrack_item->rw.trackcarparam[i].ot = 0;
            pVtrack_item->rw.trackcarparam[i].oor = 0;
            pVtrack_item->rw.trackcarparam[i].ob = 0;
            pVtrack_item->rw.trackcarparam[i].x2 = 0;
            pVtrack_item->rw.trackcarparam[i].y2 = 0;
            pVtrack_item->rw.trackcarparam[i].x3 = 0;
            pVtrack_item->rw.trackcarparam[i].y3 = 0;
        }
    }

    for (i = 0; i < RoadNum; i++) {
        if (theResult[i] == 0 || theResult[i] == 1) {
            int j = 0;
            for (j = 0; j < MAX_OBJ; j++) {
                if (pVtrack_item->rw.trackcarparam[j].bUsed == 0) {
                    pVtrack_item->rw.trackcarparam[j].bUsed = 1;
                    pVtrack_item->rw.trackcarparam[j].nProcType = 0;

                    pVtrack_item->rw.trackcarparam[j].disp = 0;
                    pVtrack_item->rw.trackcarparam[j].frameIdx = pFrame->index;


                    pVtrack_item->rw.trackcarparam[j].il = theX1[i];
                    pVtrack_item->rw.trackcarparam[j].ir = theX2[i];
                    pVtrack_item->rw.trackcarparam[j].ib = theY2[i];
                    pVtrack_item->rw.trackcarparam[j].it = theY1[i];


                    if (pVtrack_item->rw.trackcarparam[j].il < 10) pVtrack_item->rw.trackcarparam[j].il = 10;
                    if (pVtrack_item->rw.trackcarparam[j].ir > sw - 10) pVtrack_item->rw.trackcarparam[j].ir = sw - 10;
                    if (pVtrack_item->rw.trackcarparam[j].it < 10) pVtrack_item->rw.trackcarparam[j].it = 10;
                    if (pVtrack_item->rw.trackcarparam[j].ib > sh - 10) pVtrack_item->rw.trackcarparam[j].ib = sh - 10;

                    pVtrack_item->rw.trackcarparam[j].ol = 0;
                    pVtrack_item->rw.trackcarparam[j].ot = 0;
                    pVtrack_item->rw.trackcarparam[j].oor = 0;
                    pVtrack_item->rw.trackcarparam[j].ob = 0;
                    pVtrack_item->rw.trackcarparam[j].x2 = 0;
                    pVtrack_item->rw.trackcarparam[j].y2 = 0;
                    pVtrack_item->rw.trackcarparam[j].x3 = 0;
                    pVtrack_item->rw.trackcarparam[j].y3 = 0;
                    pVtrack_item->rw.trackcarparam[j].track_count = 0;
                    pVtrack_item->rw.trackcarparam[j].track_error = 0;
                    pVtrack_item->rw.trackcarparam[j].search_height = 16;

                    break;

                }
            }
        }
    }
    
    vTrackcar_proc(pFrame->data.raw[2]->data, sw, sh, pVtrack_item->rw.trackcarparam, NULL);
    
    
    //继续跟踪...
    for (i = 0; i < MAX_OBJ; i++) {
        if (pVtrack_item->rw.trackcarparam[i].bUsed == 1 && pVtrack_item->rw.trackcarparam[i].track_count > 1) {
            int x1 = pVtrack_item->rw.trackcarparam[i].ol;
            int y1 = pVtrack_item->rw.trackcarparam[i].ot;
            int x2 = pVtrack_item->rw.trackcarparam[i].oor;
            int y2 = pVtrack_item->rw.trackcarparam[i].ob;

            if (y2 > 270 - 3) {
                pVtrack_item->rw.trackcarparam[i].bUsed = 0;
                printf("************track over :%d*************\n",y2);
            }
        }
    }
   
    for (i = 0; i < MAX_OBJ; i++) {
        if (pVtrack_item->rw.trackcarparam[i].bUsed == 1 && (pVtrack_item->rw.trackcarparam[i].track_error || pVtrack_item->rw.trackcarparam[i].track_count > 100000)) {
            pVtrack_item->rw.trackcarparam[i].bUsed = 0;
            printf("************track over track_error:%d track_count:%d*************\n",pVtrack_item->rw.trackcarparam[i].track_error,pVtrack_item->rw.trackcarparam[i].track_count);
        }
    }
    
    
    {
        for (i = 0; i < MAX_OBJ; i++) {
            if (pVtrack_item->rw.trackcarparam[i].bUsed == 1 && pVtrack_item->rw.trackcarparam[i].track_count < 1000) {
               
                int l,t,r,b;
                
                l = pVtrack_item->rw.trackcarparam[i].ol*4;
                r = pVtrack_item->rw.trackcarparam[i].oor*4;
                t = pVtrack_item->rw.trackcarparam[i].ot*4;
                b = pVtrack_item->rw.trackcarparam[i].ob*4;
                
                if(l < 0 || r > sw*4-1 || t < 0 || b > sh*4)
                {
                    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                    printf("error: l:%d t:%d t:%d b:%d \n",l,r,t,b);
                     printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                }
                if(l < 0) l = 0;
                if(r > sw*4-1) r = sw*4 -1;
                if(t < 0) t = 0;
                if(b > sh*4) b = sh*4-1;
                
                //DrawTrackRect((char *) pFrame->data.raw[0]->data, pFrame->width[0], pFrame->height[0], pVtrack_item, l, r, t, b);
            }
        }
    }
*/
}

static void vGetDataRectImage(unsigned char * ps, unsigned char * pd, int w, int h, int x1, int y1, int rw, int rh) {
    printf("%s: w:%d h:%d x1:%d y1:%d rw:%d rh:%d \n", __FUNCTION__, w, h, x1, y1, rw, rh);

    int i = 0, j = 0;
    unsigned char *pss = ps + y1 * w + x1;
    unsigned char *pdd = pd;

    for (i = 0; i < rh; i++) {
        for (j = 0; j < rw; j++) {
            pdd[j] = pss[j];
        }

        pss += w;
        pdd += rw;
    }
}

/*******************************************************************************
 *                      检测套框区域是否为浮游物 
 * 
 * Vtrack_item *pVtrack_item：  
 * FsObjectImageFrame *pFrame:       数据帧
 * int x0, int y0, int x1, int y1:   套框坐标
 * int debug:                        是否开启调试
 * 
 * 返回值： 0：不是漂浮物
 *          1：套框区域可能为水草
 *          2：套框区域可能为强光反射
 * 
 *******************************************************************************/
int alg_check_floater(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, int x0, int y0, int x1, int y1, int debug) {

    unsigned int m_flag = 0;

    int w = pFrame->width[0];
    int h = pFrame->height[0];

    if (x0 <= 0 || y0 <= 0 || x1 <= 0 || y1 <= 0 || x1 > w - 1 || y1 > h - 1 || x0 > x1 || y0 > y1) {
        return m_flag;
    }

    unsigned long lineBits_src = w;
    unsigned long lineBits_dst = w * 3;

    ImageDib* pdib = image_dib_new__IO(w, h, ImageColorType_RGB, 0);

    unsigned char* data_r = (unsigned char*) fsMalloc(w * h * sizeof (unsigned char));
    unsigned char* data_g = (unsigned char*) fsMalloc(w * h * sizeof (unsigned char));
    unsigned char* data_b = (unsigned char*) fsMalloc(w * h * sizeof (unsigned char));
    image_dib_copy(/* 目标图数据 */pdib->data, NULL, NULL, /* 目标图数据的储存宽度 */lineBits_dst,
            /* 目标图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,ImageColorType_YUV420p=5,ImageColorType_GBR = 6 */ ImageColorType_RGB,
            /* 原图数据 */pFrame->data.raw[0]->data, pFrame->data.raw[0]->data + pFrame->sizeWidthHeight[0]
            , pFrame->data.raw[0]->data + pFrame->sizeWidthHeight[0] / 4 * 5, /* 原图数据的储存宽度 */lineBits_src,
            /* 原图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,ImageColorType_YUV420p=5,ImageColorType_GBR = 6 */ ImageColorType_YUV420p,
            /* 图像宽度 */w, /* 图像高度 */h
            );

    int top = y0;
    int bottom = y1;
    int left = x0;
    int right = x1;

    int i = 0;
    int j = 0;
    int k = 0;
    //for (j = h - 1; j >= 0; j--) {
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            data_r[j * w + i] = pdib->data[k];
            k++;
            data_g[j * w + i] = pdib->data[k];
            k++;
            data_b[j * w + i] = pdib->data[k];
            k++;

            if (j == top && i == left) {
                printf("1 r:%d g:%d b:%d \n", data_r[j * w + i], data_g[j * w + i], data_b[j * w + i]);
            }
        }
        int asd = (w * 3) % 4;
        asd = (asd == 0) ? 0 : (4 - asd);
        k += asd;
    }

    //水草判断
    int count = 0;
    for (j = top; j < bottom; j++) {
        for (i = left; i < right; i++) {
            int b = data_b[j * w + i];
            int g = data_g[j * w + i];
            int r = data_r[j * w + i];

            if (j == top && i == left) {
                printf("r:%d g:%d b:%d \n", r, g, b);
            }

            if (g > (b + 8) && g > (r + 8)) {
                count++;
            }
        }
    }

    //强光反射判断
    int count1 = 0;
    for (j = top; j < bottom; j++) {
        for (i = left; i < right; i++) {
            int b = data_b[j * w + i];
            int g = data_g[j * w + i];
            int r = data_r[j * w + i];

            if (pVtrack_item->ro.m_config_videoDect.m_re_light_strength > 0 && g > pVtrack_item->ro.m_config_videoDect.m_re_light_strength && r > pVtrack_item->ro.m_config_videoDect.m_re_light_strength && b > pVtrack_item->ro.m_config_videoDect.m_re_light_strength) {
                count1++;
            }
        }
    }

    //水草
    pVtrack_item->rw.m_water_grass_count = count;
    if (pVtrack_item->ro.m_config_videoDect.m_watergrass_factor > 0 && count > pVtrack_item->ro.m_config_videoDect.m_watergrass_factor) {
        m_flag = 1;
    }
    //强光反射
    double rate = 0.0;
    int area = (x1 - x0)*(y1 - y0);

    if (area > 0 && pVtrack_item->rw.v_avegray > 60) {
        rate = (double) count1 / (double) area;
        if (rate * 100 > pVtrack_item->ro.m_config_videoDect.m_re_light_rate * 1.0) {
            m_flag = 2;
        }
    }

    /*
    if (pVtrack_item->ro.m_config_videoDect.m_re_light_strength > 0) {
        rate = count1 * 100 / (x1 - x0)*(y1 - y0);
        if (rate > pVtrack_item->ro.m_config_videoDect.m_re_light_rate) {
            m_flag = 2;
        }
    }*/

    printf("%s %d,uuid:%s ,count:%d pos(%d %d %d %d) w:%d h:%d,count1:%d rate:%d \n", __FUNCTION__, __LINE__,
            pVtrack_item->ro._uuid, count, left, top, right, bottom, w, h, count1, rate);

    if (debug > 0) {
        char savepath[256];
        sprintf(savepath, "/fs/project/data/temp/count_%d_pos_%d_%d_%d_%d_g_%d_c_%d_%d_rate_%lf_%d_%d_m_flag_%d.bmp", count, left, top, right, bottom, pVtrack_item->rw.v_avegray, count1,
                area, rate, pVtrack_item->ro.m_config_videoDect.m_re_light_strength, pVtrack_item->ro.m_config_videoDect.m_re_light_rate, m_flag);
        image_dib_svae_to_file(pdib, savepath);
    }

    image_dib_delete__OI(&pdib);
    fsFree(data_r);
    fsFree(data_g);
    fsFree(data_b);

    return m_flag;
}

/*
 * int isfalse:通过图像检测，判定是否是虚警，如果是(isfalse == 1),且离上一次检测到该目标的时间差不超过30分钟，则该目标虚警次数加一
 *                                           如果不是(isfalse == 0)，该目标虚警次数清零
 */
void AddtoFalseAlarm(Vtrack_item *pVtrack_item, int isfalse) {
    int i;
    for (i = 0; i < 20; i++) {
        //如果本相机已经跟踪过此目标编号,且离上次跟踪时间相差30分钟
        if (pVtrack_item->rw.v_trackNum == pVtrack_item->rw.m_objInfo[i].tracknum) {

            if (abs(fs_time_uptime_get() - pVtrack_item->rw.m_objInfo[i].time) < 30 * 60 && (isfalse == 1)) {
                pVtrack_item->rw.m_objInfo[i].m_false_alarm_count++;
                printf("%s:%d  add the falsealarm count(tracknum:%lld falarm_count:%d) \n", __FUNCTION__, __LINE__, pVtrack_item->rw.v_trackNum,
                        pVtrack_item->rw.m_objInfo[i].m_false_alarm_count);
            }

            if (isfalse == 0) {

                pVtrack_item->rw.m_objInfo[i].m_false_alarm_count = 0;
                printf("%s:%d  the obj is not falsealarm,clear the count (tracknum:%lld falarm_count:%d) \n", __FUNCTION__, __LINE__, pVtrack_item->rw.v_trackNum,
                        pVtrack_item->rw.m_objInfo[i].m_false_alarm_count);
            }

            break;
        }
    }
}

/*
 * 将目标编号信息添加到缓存队列当中
 */
int handleToFalseAlarm(Vtrack_item *pVtrack_item) {
    pVtrack_item->rw.m_isfalsealarm = 0;
    int found = 0;
    int i;
    for (i = 0; i < 20; i++) {
        //如果本相机已经跟踪过此目标编号,且离上次跟踪时间相差30分钟
        if (pVtrack_item->rw.v_trackNum == pVtrack_item->rw.m_objInfo[i].tracknum) {

            found = 1;
            printf("%s:%d  check the objnum(tracknum:%lld falarm_count:%d) \n", __FUNCTION__, __LINE__, pVtrack_item->rw.v_trackNum,
                    pVtrack_item->rw.m_objInfo[i].m_false_alarm_count);
            if (abs(fs_time_uptime_get() - pVtrack_item->rw.m_objInfo[i].time) < 30 * 60) {

                //如果该编号记录为虚景的次数为2，则认定该目标一直为虚景
                if (pVtrack_item->rw.m_objInfo[i].m_false_alarm_count > 2) {
                    pVtrack_item->rw.m_isfalsealarm = 1;
                    pVtrack_item->rw.m_objInfo[i].m_false_alarm_count++;
                }
            } else {//同一个目标编号超过30分钟后再次被检测到，则判定该目标为一个新目标，将虚警计数归零

                pVtrack_item->rw.m_objInfo[i].m_false_alarm_count = 0;
                printf("%s:%d  time is over,clear the false_alarm_count(tracknum:%lld falarm_count:%d) \n", __FUNCTION__, __LINE__, pVtrack_item->rw.v_trackNum,
                        pVtrack_item->rw.m_objInfo[i].m_false_alarm_count);
            }

            //更新目标编号信息
            pVtrack_item->rw.m_objInfo[i].time = fs_time_uptime_get();
            pVtrack_item->rw.m_objInfo[i].p = pVtrack_item->rw.v_obj_p;
            pVtrack_item->rw.m_objInfo[i].t = pVtrack_item->rw.v_obj_t;
            pVtrack_item->rw.m_objInfo[i].z = pVtrack_item->rw.v_obj_z;

            break;
        }
    }

    if (found == 0) {
        //在队列中找到没有使用的； 如果都使用了，选择最早的一个；
        double tt1 = fs_time_uptime_get();
        int index1 = -1;
        int index2 = -1;
        for (i = 0; i < 20; i++) {
            if (pVtrack_item->rw.m_objInfo[i].tracknum > 0) {
                if (pVtrack_item->rw.m_objInfo[i].time < tt1) {
                    tt1 = pVtrack_item->rw.m_objInfo[i].time;
                    index1 = i;
                }
            } else {
                index2 = i;
                break;
            }
        }

        int index = 0;
        if (index2 > -1) {
            index = index2;
        } else if (index1 > -1) {
            index = index1;
        }

        printf("%s:%d insert the tracknum: (old tracknum:%lld new tracknum:%lld) \n", __FUNCTION__, __LINE__,
                pVtrack_item->rw.m_objInfo[index].tracknum, pVtrack_item->rw.v_trackNum);

        //覆盖目标编号信息
        memset(&pVtrack_item->rw.m_objInfo[index], 0, sizeof (ObjInfo));

        pVtrack_item->rw.m_objInfo[index].time = fs_time_uptime_get();
        pVtrack_item->rw.m_objInfo[index].p = pVtrack_item->rw.v_obj_p;
        pVtrack_item->rw.m_objInfo[index].t = pVtrack_item->rw.v_obj_t;
        pVtrack_item->rw.m_objInfo[index].z = pVtrack_item->rw.v_obj_z;

        pVtrack_item->rw.m_objInfo[index].tracknum = pVtrack_item->rw.v_trackNum;
        pVtrack_item->rw.m_objInfo[index].m_false_alarm_count = 0;
    }
}

//运动块检测

/*************************************************
 *          搜索运动块
 * 
 * 返回值：
 * pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y                                                 ： 最佳运动块的左上角坐标             
 * pVtrack_item->rw.v_boatheight                                                                      ： 运动块的高度
 * pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right, pVtrack_item->rw.v_boat_bottom_y      ： 船体的左边界，右边界，底部       
 * pVtrack_item->rw.v_boat_enterall                                                                   ： 船是否全部在图像视野中 
 * pVtrack_item->rw.v_effective                                                                       ： 船体检测是否有效，无效：只有v_max_x，v_max_y两个参数可用
 *  
 * 
 * v_effective无效条件:
 * 
 *          1.检测到两侧都有水平直线
 *          2.检测到的水平直线不符合要求
 *          3. 
 * 
 * 正常返回0，如果在最大帧数内找不到运动块，返回-1
 **************************************************/
void searchMotionblock(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *image, int sw, int sh, int frameindex) {

    //检测图像边缘图(3个边缘图)
    //Check_Edge(&pVtrack_item->rw.m_vsearch, (unsigned char *) pFrame->data.raw[2]->data);

    //当本路发现目标，且没有正在跟踪中，且没有发现运动块，则进行运动检测，定位船的位置
    //if (pVtrack_item->rw.v_findObj == 1 && pVtrack_item->rw.v_trackingObj == 0 && pVtrack_item->rw.v_findmove == 0) 
    if (pVtrack_item->rw.v_findmove == 0) {
        //当运动检测没有初始化时，先进行初始化
        if (pVtrack_item->rw.m_vsearch.m_bInitOK == 0) {

            //对该目标进行一次判断
            handleToFalseAlarm(pVtrack_item);

            vSearchCreate(&pVtrack_item->rw.m_vsearch);
            InitSearchObj(&pVtrack_item->rw.m_vsearch, image, sw, sh, &pVtrack_item->ro.m_config_search);
            AssistSearchObjInit(&pVtrack_item->rw.m_vsearch, image, sw, sh);
        }

        pVtrack_item->rw.v_litter = 0;
        int debug_x = 0;
        int debug_y = 0;
        //进行运动检测，设置标记m_findmove      
        pVtrack_item->rw.v_findmove = SearchObjProc(&pVtrack_item->rw.m_vsearch, image, &pVtrack_item->rw.v_max_x, &pVtrack_item->rw.v_max_y,
                &pVtrack_item->rw.v_boatheight, &pVtrack_item->rw.v_boat_left, &pVtrack_item->rw.v_boat_right, &pVtrack_item->rw.v_boat_bottom_y, &pVtrack_item->rw.v_boat_enterall,
                &pVtrack_item->rw.v_effective, &pVtrack_item->rw.v_litter, &debug_x, &debug_y);

        //打印SearchObjProc输出参数结果
        if (pVtrack_item->rw.v_findmove == 1) {
            FsPrintf2(1, "(%s:%d)-(%s tracknum:%lld) max_x:%d max_y:%d boatheight:%d boatl:%d boatr:%d boatb:%d enterall:%d effect:%d litter:%d \n", __FUNCTION__, __LINE__,
                    pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                    pVtrack_item->rw.v_boatheight, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                    pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boat_enterall,
                    pVtrack_item->rw.v_effective, pVtrack_item->rw.v_litter);
        }

        int m_debug[25] = {0};
        if (pVtrack_item->ro.m_config_search.isAssistCheck == 1) //是否调用辅助函数
        {
            pVtrack_item->rw.v_assistfind = AssistSearchObjProc(&pVtrack_item->rw.m_vsearch, image, sw, sh, pVtrack_item->rw.v_trackNum,
                    &pVtrack_item->rw.v_max_x, &pVtrack_item->rw.v_max_y, &pVtrack_item->rw.v_boatheight, &pVtrack_item->rw.v_boat_left,
                    &pVtrack_item->rw.v_boat_right, &pVtrack_item->rw.v_boat_bottom_y, &pVtrack_item->rw.v_boat_enterall, &pVtrack_item->rw.v_effective,
                    pVtrack_item->ro.m_config_search.isAssistfindbottom, pVtrack_item->ro.m_config_search.assistparam1, pVtrack_item->ro.m_config_search.assistparam2, m_debug);
        }

        //辅助函数检测出运动块区域是干扰，直接退出跟踪
        if (pVtrack_item->rw.v_assistfind == -1)
            pVtrack_item->rw.v_findmove = -1;

        //通过算法处理检测为干扰，则干扰次数加一
        if (pVtrack_item->rw.v_findmove == -1) {
            AddtoFalseAlarm(pVtrack_item, 1);
        }


        //保存通过雷达编号过滤的问题：
        if (0 && pVtrack_item->ro.m_config_debug.m_opendebug == 1 && pVtrack_item->rw.m_isfalsealarm == 1) {
            char filepath[512];
            sprintf(filepath, "/fs/project/data/temp/%s_%04d_%04d_litter.bmp", pVtrack_item->ro.m_config_devinfo.m_camera_ip, frameindex, pVtrack_item->rw.v_trackNum);
            ImageDib* pdib = image_dib_new__IO(sw, sh, ImageColorType_Gray, 0);
            memcpy(pdib->data, image, sw * sh);
            image_dib_svae_to_file(pdib, filepath);

            image_dib_delete__OI(&pdib);
        }

        //发现运动块，锁定目标(当目标跟踪5次以上时，在发送锁定成功指令)
        //        if(pVtrack_item->rw.v_findmove == 1){
        //            FsPrintf(1, "(%s:%d)-(%s tracknum:%d) lock the obj success(before...) \n",__FUNCTION__,__LINE__,pVtrack_item->rw.m_camera_ip,pVtrack_item->rw.v_trackNum);
        //            //pVtrack_item->rw.v_locktrackObj = 1;
        //         }

        //叠加
        if (pVtrack_item->ro.m_config_debug.m_opendebug == 1) {
            vPasteY((unsigned char *) pFrame->data.raw[0]->data, (unsigned char *) pVtrack_item->rw.m_vsearch.projImage,
                    pFrame->width[0], pFrame->height[0], sw, 0, sh, 10, 20);
        }

        //没有发现运动块，未锁定目标
        if (pVtrack_item->rw.v_findmove == -1) {
            FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj fail(m_isfalsealarm:%d v_assistfind:%d)... \n", __FUNCTION__, __LINE__,
                    pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.m_isfalsealarm, pVtrack_item->rw.v_assistfind);
            pVtrack_item->rw.v_locktrackObj = 0;
        }


        if (pVtrack_item->rw.v_findmove == 1 || pVtrack_item->rw.v_assistfind == -1) {
            //删除辅助函数对象
            DeleteSearchObj(&pVtrack_item->rw.m_vsearch);
        }



        //for test
        //辅助函数检测，相关调试数据保存
        if (0 && pVtrack_item->ro.m_config_debug.m_opendebug == 1 && (pVtrack_item->rw.v_findmove == 1 || pVtrack_item->rw.v_assistfind == -1)) {
            char filepath[512];
            sprintf(filepath, "/fs/project/data/temp/%s_%04d_%04d_litter(%d_%d_%d)_(%d_%d_%d).bmp", pVtrack_item->ro.m_config_devinfo.m_camera_ip, frameindex, pVtrack_item->rw.v_trackNum,
                    m_debug[20], m_debug[21], m_debug[22], m_debug[5], m_debug[4], m_debug[6]);
            ImageDib* pdib = image_dib_new__IO(sw, sh, ImageColorType_Gray, 0);
            memcpy(pdib->data, pVtrack_item->rw.m_vsearch.projImage, sw * sh);
            image_dib_svae_to_file(pdib, filepath);

            sprintf(filepath, "/fs/project/data/temp/%s_%04d_%04d_%d_%d_%d_%d_%d_%d_site_%d.bmp", pVtrack_item->ro.m_config_devinfo.m_camera_ip, frameindex, pVtrack_item->rw.v_trackNum,
                    m_debug[4], m_debug[5], m_debug[6], m_debug[7], m_debug[8], pVtrack_item->rw.v_assistfind, pVtrack_item->rw.v_obj_alarmsite);
            memcpy(pdib->data, pVtrack_item->rw.m_vsearch.segmentation_Image, sw * sh);
            image_dib_svae_to_file(pdib, filepath);

            if (pVtrack_item->rw.v_assistfind != -1) {
                tag_ITG_RECT m_rect;

                if (pVtrack_item->rw.v_effective > 0) {
                    //船体在左侧
                    if (pVtrack_item->rw.v_effective == 1) {

                        //宽度改为设定值的2倍
                        if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {
                            m_rect.left = pVtrack_item->rw.v_max_x - pVtrack_item->ro.m_config_track.m_trackWidth;
                            m_rect.right = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth;
                        } else {
                            m_rect.left = pVtrack_item->rw.v_max_x - pVtrack_item->ro.m_config_track.m_trackWidth;
                            m_rect.right = pVtrack_item->rw.v_max_x + pVtrack_item->rw.m_vsearch.m_modWidth;
                        }

                        m_rect.top = pVtrack_item->rw.v_max_y;

                        //当运动块高度比设置的要小，且大于设定值的1/3，则说明该高度比较合理
                        if ((pVtrack_item->ro.m_config_track.m_trackHeight > pVtrack_item->rw.v_boatheight) && pVtrack_item->rw.v_boatheight > pVtrack_item->ro.m_config_track.m_trackHeight / 3) {
                            m_rect.bottom = pVtrack_item->rw.v_max_y + pVtrack_item->rw.v_boatheight;
                        } else {
                            m_rect.bottom = pVtrack_item->rw.v_max_y + pVtrack_item->ro.m_config_track.m_trackHeight;
                        }
                    }
                    //船体在右侧
                    if (pVtrack_item->rw.v_effective == 2) {

                        //宽度改为设定值的2倍
                        m_rect.left = pVtrack_item->rw.v_max_x;
                        m_rect.right = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth * 2;

                        m_rect.top = pVtrack_item->rw.v_max_y;
                        //当运动块高度比设置的要小，且大于设定值的1/3，则说明该高度比较合理
                        if ((pVtrack_item->ro.m_config_track.m_trackHeight > pVtrack_item->rw.v_boatheight) && pVtrack_item->rw.v_boatheight > pVtrack_item->ro.m_config_track.m_trackHeight / 3) {
                            m_rect.bottom = pVtrack_item->rw.v_max_y + pVtrack_item->rw.v_boatheight;
                        } else {
                            m_rect.bottom = pVtrack_item->rw.v_max_y + pVtrack_item->ro.m_config_track.m_trackHeight;
                        }
                    }
                } else { //没有定位到船体

                    m_rect.left = pVtrack_item->rw.v_max_x - pVtrack_item->ro.m_config_track.m_trackWidth / 2;
                    m_rect.right = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth / 2;
                    m_rect.top = pVtrack_item->rw.v_max_y;
                    m_rect.bottom = pVtrack_item->rw.v_max_y + pVtrack_item->ro.m_config_track.m_trackHeight;
                }

                //重新定位船体上下界
                if (pVtrack_item->rw.v_assistfind == 1) {
                    m_rect.bottom = pVtrack_item->rw.v_boat_bottom_y;
                    m_rect.top = pVtrack_item->rw.v_boat_bottom_y - pVtrack_item->rw.v_boatheight;
                }

                if (m_rect.left < 0) m_rect.left = 5;
                if (m_rect.right > sw - 5) m_rect.left = sw - 5;
                if (m_rect.top < 0) m_rect.top = 5;
                if (m_rect.bottom > sh - 5) m_rect.bottom = sh - 5;

                ITG_DrawRect((char *) image, sw, &m_rect, 255);
            }

            sprintf(filepath, "/fs/project/data/temp/%s_%04d_%04d_gray(%d_%d_%d_%d_%d)__ptz(%d_%d_%d).bmp", pVtrack_item->ro.m_config_devinfo.m_camera_ip, frameindex, pVtrack_item->rw.v_trackNum,
                    m_debug[15], m_debug[16], m_debug[17], m_debug[18], m_debug[19], (int) (pVtrack_item->rw.v_obj_p), (int) (pVtrack_item->rw.v_obj_t), (int) (pVtrack_item->rw.v_obj_z));
            memcpy(pdib->data, image, sw * sh);
            image_dib_svae_to_file(pdib, filepath);

            image_dib_delete__OI(&pdib);
        }
    }
}


//红外相机自动视频运动块检测1

/*************************************************
 **************************************************/
void RedsearchMotionblock_auto(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int frameindex) {

    //如果已经找到最佳目标，不再继续寻找
    //if (pVtrack_item->rw.v_findmove == 1)
    //return;
    if (pVtrack_item->rw.m_vibeparam.m_bInitOK == 0 && pVtrack_item->ro.m_config_videoDect.m_valid == 1) {

        int index = 0;
        for (index = 0; index < pVtrack_item->ro.m_config_videoDect.m_ptz_count; index++) {
            if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused == 1) {
                break;
            }
        }

        if (index == pVtrack_item->ro.m_config_videoDect.m_ptz_count) {
            index = 0;
        }

        printf("===m_camera_ip:%s  index:%d  m_valid:%d=====\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, index, pVtrack_item->ro.m_config_videoDect.m_valid);

        pVtrack_item->rw.m_vibe_inparam.headth = 16;
        pVtrack_item->rw.m_vibe_inparam.moveth = 16;
        pVtrack_item->rw.m_vibe_inparam.stopframecount = 250;
        pVtrack_item->rw.m_vibe_inparam.tailth = 16;

        //
        sprintf(pVtrack_item->rw.m_vibe_inparam.cam_uuid, "%s", pVtrack_item->ro._uuid);

        pVtrack_item->rw.m_vibe_inparam.objminWidth = pVtrack_item->ro.m_config_videoDect.m_objminWidth;
        pVtrack_item->rw.m_vibe_inparam.objminHeight = pVtrack_item->ro.m_config_videoDect.m_objminHeight;

        pVtrack_item->rw.m_vibe_inparam.objmaxWidth = pVtrack_item->ro.m_config_videoDect.m_objmaxWidth;
        pVtrack_item->rw.m_vibe_inparam.objmaxHeight = pVtrack_item->ro.m_config_videoDect.m_objmaxHeight;


        pVtrack_item->rw.m_vibe_inparam.modw = pVtrack_item->ro.m_config_videoDect.m_pos[index].modw;
        pVtrack_item->rw.m_vibe_inparam.modh = pVtrack_item->ro.m_config_videoDect.m_pos[index].modh;
        pVtrack_item->rw.m_vibe_inparam.vibe_left = pVtrack_item->ro.m_config_videoDect.m_pos[index].left; //pVtrack_item->rw.m_incashore.m_ashore_search_left[0];//560/4; 
        pVtrack_item->rw.m_vibe_inparam.vibe_right = pVtrack_item->ro.m_config_videoDect.m_pos[index].right; //pVtrack_item->rw.m_incashore.m_ashore_search_right[0];//sw-10;
        pVtrack_item->rw.m_vibe_inparam.vibe_top = pVtrack_item->ro.m_config_videoDect.m_pos[index].top; //pVtrack_item->rw.m_incashore.m_ashore_search_top[0];//50/4;
        pVtrack_item->rw.m_vibe_inparam.vibe_bottom = pVtrack_item->ro.m_config_videoDect.m_pos[index].bottom; //pVtrack_item->rw.m_incashore.m_ashore_search_bottom[0];//sh-50/4;

        pVtrack_item->rw.m_vibe_inparam.vibe_obj_keepTime = pVtrack_item->ro.m_config_videoDect.vibe_obj_keepTime;

        pVtrack_item->rw.m_vibe_inparam.vibe_obj_moveMinDis = pVtrack_item->ro.m_config_videoDect.vibe_obj_moveMinDis;

        printf("vibe_infrared_Init l:%d r:%d b:%d t:%d  modw:%d modh:%d \n", pVtrack_item->ro.m_config_videoDect.m_pos[0].left, pVtrack_item->ro.m_config_videoDect.m_pos[0].right,
                pVtrack_item->ro.m_config_videoDect.m_pos[0].top, pVtrack_item->ro.m_config_videoDect.m_pos[0].bottom, pVtrack_item->rw.m_vibe_inparam.modw, pVtrack_item->rw.m_vibe_inparam.modh);

        vibe_infrared_Init(&pVtrack_item->rw.m_vibeparam, pSrcImage, sw, sh, pVtrack_item->rw.m_vibe_inparam, index);

        printf("(%s:%d)(ip:%s) index:%d  vibe init m_bInitOK:%d ,detection area(%d %d %d %d) mod(%d %d) minobj(%d %d) maxobj(%d %d) checked:%d.. \n", __FUNCTION__, __LINE__,
                pVtrack_item->ro.m_config_devinfo.m_camera_ip, index, pVtrack_item->rw.m_vibeparam.m_bInitOK,
                pVtrack_item->rw.m_vibe_inparam.vibe_left, pVtrack_item->rw.m_vibe_inparam.vibe_right,
                pVtrack_item->rw.m_vibe_inparam.vibe_top, pVtrack_item->rw.m_vibe_inparam.vibe_bottom, pVtrack_item->rw.m_vibe_inparam.modw,
                pVtrack_item->rw.m_vibe_inparam.modh, pVtrack_item->rw.m_vibe_inparam.objminWidth, pVtrack_item->rw.m_vibe_inparam.objminHeight,
                pVtrack_item->rw.m_vibe_inparam.objmaxWidth, pVtrack_item->rw.m_vibe_inparam.objmaxHeight, pVtrack_item->rw.m_vibeparam.checked);
    }

    int moveth = 10;
    int headth = 15;
    int tailth = 15;
    int gray = 0;

    int debug[6] = {0};
    //暂时使用!!
    //memcpy(pVtrack_item->rw.RedImage, pSrcImage, sw * sh);
    pVtrack_item->rw.v_findmove = vibe_infrared_Proc(&pVtrack_item->rw.m_vibeparam, pSrcImage, frameindex, sw, sh,
            &gray, &pVtrack_item->rw.m_boatinfo, debug);

    //pVtrack_item->rw.v_findmove = 0;

    //如果背景刷新出现问题，则删除vibe，重新初始化并检测
    if (pVtrack_item->rw.v_findmove == -1) {
        FsPrintf2(1, "(%s:%d) vibe is error... \n", __FUNCTION__, __LINE__);
        fflush(stdout);
        vibe_Del(&pVtrack_item->rw.m_vibeparam);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    //将相关中间变量数据叠加到图像视频中，方便调试使用
    if (pVtrack_item->ro.m_config_debug.m_opendebug == 2) {

        vPasteY_1((unsigned char *) pFrame->data.raw[0]->data, pVtrack_item->rw.m_vibeparam.checkdir_head_image, sw * 2, sh * 2, sw, 0, sh, 0, 10);

        int i;
        for (i = 0; i < pVtrack_item->rw.m_vibeparam.m_boatHeadCount; i++) {
            tag_ITG_RECT m_rect;
            m_rect.left = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].l[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1] * 2;
            m_rect.right = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].r[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1] * 2;
            m_rect.top = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].t[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1] * 2;
            m_rect.bottom = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].b[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1] * 2;

            if (m_rect.left <= 0) m_rect.left = 0;
            if (m_rect.right <= 0) m_rect.right = 0;
            if (m_rect.top <= 0) m_rect.top = 0;
            if (m_rect.bottom <= 0) m_rect.bottom = 0;
            if (m_rect.left > sw * 2 - 1) m_rect.left = sw * 2 - 1;
            if (m_rect.right > sw * 2 - 1) m_rect.right = sw * 2 - 1;
            if (m_rect.top > sh * 2 - 1) m_rect.top = sh * 2 - 1;
            if (m_rect.bottom > sh * 2 - 1) m_rect.bottom = sh * 2 - 1;

            ITG_DrawRect((char *) pFrame->data.raw[0]->data, sw * 2, &m_rect, 255);
        }

        if (0 && pVtrack_item->ro.__pTypeFace) {

            int width = sw * 2;
            int height = sh * 2;

            unsigned char* p_y = (unsigned char *) pFrame->data.raw[0]->data;
            //unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width*height;
            //unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;

            int startx, starty;
            startx = 10;
            starty = 100;

            char overlydata[256];
            int www = abs(pVtrack_item->rw.m_boatinfo.r - pVtrack_item->rw.m_boatinfo.l);
            int hhh = abs(pVtrack_item->rw.m_boatinfo.b - pVtrack_item->rw.m_boatinfo.t);

            printf("--w:%d  h:%d---\n", width, height);

            sprintf(overlydata, "boatnum:%03d bestObj(%d %d %d %d) bestSize(%d %d)", pVtrack_item->rw.m_vibeparam.m_boatHeadCount, pVtrack_item->rw.m_boatinfo.l,
                    pVtrack_item->rw.m_boatinfo.r, pVtrack_item->rw.m_boatinfo.t, pVtrack_item->rw.m_boatinfo.b, www, hhh);
            fs_TypeFace_write(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_y, width, height,
                    width, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 16);
            //fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_u, width, height,
            //        width >> 1, startx, starty, (pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 8)&0xFF);
            //fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_v, width, height,
            //        width >> 1, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color & 0xFF);

        } else {

        }
    }
    ///////////////////////////////////////////////////////////////////////////////////

}
//自动视频运动块检测
/*************************************************
 **************************************************/
void SaveTrackVideo(Vtrack_item *pVtrack_item);

void searchMotionblock_auto(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int frameindex) {

    //if(pVtrack_item->rw.v_findmove == 1)
    //return;

    if (pVtrack_item->rw.m_vibeparam.m_bInitOK == 0 && pVtrack_item->ro.m_config_videoDect.m_valid == 1) {

        //如果是枪机，一个也找不到，默认选择第一个m_pos[0]的参数
        int index = 0;
        for (index = 0; index < pVtrack_item->ro.m_config_videoDect.m_ptz_count; index++) {
            if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused == 1) {
                break;
            }
        }

        if (index == pVtrack_item->ro.m_config_videoDect.m_ptz_count) {
            index = 0;
        }

        printf("===m_camera_ip:%s  index:%d  m_valid:%d=====\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, index, pVtrack_item->ro.m_config_videoDect.m_valid);

        pVtrack_item->rw.m_vibe_inparam.headth = 16;
        pVtrack_item->rw.m_vibe_inparam.moveth = pVtrack_item->ro.m_config_videoDect.m_moveth;
        pVtrack_item->rw.m_vibe_inparam.stopframecount = 250;
        pVtrack_item->rw.m_vibe_inparam.tailth = 16;

        pVtrack_item->rw.m_vibe_inparam.objminWidth = pVtrack_item->ro.m_config_videoDect.m_objminWidth;
        pVtrack_item->rw.m_vibe_inparam.objminHeight = pVtrack_item->ro.m_config_videoDect.m_objminHeight;

        pVtrack_item->rw.m_vibe_inparam.objmaxWidth = pVtrack_item->ro.m_config_videoDect.m_objmaxWidth;
        pVtrack_item->rw.m_vibe_inparam.objmaxHeight = pVtrack_item->ro.m_config_videoDect.m_objmaxHeight;

        pVtrack_item->rw.m_vibe_inparam.cloudminWidth = pVtrack_item->ro.m_config_videoDect.m_cloud_minWidth;
        pVtrack_item->rw.m_vibe_inparam.cloudminHeight = pVtrack_item->ro.m_config_videoDect.m_cloud_minHeight;

        pVtrack_item->rw.m_vibe_inparam.modw = pVtrack_item->ro.m_config_videoDect.m_pos[index].modw;
        pVtrack_item->rw.m_vibe_inparam.modh = pVtrack_item->ro.m_config_videoDect.m_pos[index].modh;
        pVtrack_item->rw.m_vibe_inparam.vibe_left = pVtrack_item->ro.m_config_videoDect.m_pos[index].left; //pVtrack_item->rw.m_incashore.m_ashore_search_left[0];//560/4; 
        pVtrack_item->rw.m_vibe_inparam.vibe_right = pVtrack_item->ro.m_config_videoDect.m_pos[index].right; //pVtrack_item->rw.m_incashore.m_ashore_search_right[0];//sw-10;
        pVtrack_item->rw.m_vibe_inparam.vibe_top = pVtrack_item->ro.m_config_videoDect.m_pos[index].top; //pVtrack_item->rw.m_incashore.m_ashore_search_top[0];//50/4;
        pVtrack_item->rw.m_vibe_inparam.vibe_bottom = pVtrack_item->ro.m_config_videoDect.m_pos[index].bottom; //pVtrack_item->rw.m_incashore.m_ashore_search_bottom[0];//sh-50/4;

        pVtrack_item->rw.m_vibe_inparam.vibe_obj_keepTime = pVtrack_item->ro.m_config_videoDect.vibe_obj_keepTime;
        pVtrack_item->rw.m_vibe_inparam.vibe_obj_moveMinDis = pVtrack_item->ro.m_config_videoDect.vibe_obj_moveMinDis;

        printf("l:%d r:%d b:%d t:%d cloud:%d-%d \n", pVtrack_item->ro.m_config_videoDect.m_pos[0].left, pVtrack_item->ro.m_config_videoDect.m_pos[0].right,
                pVtrack_item->ro.m_config_videoDect.m_pos[0].top, pVtrack_item->ro.m_config_videoDect.m_pos[0].bottom, pVtrack_item->rw.m_vibe_inparam.cloudminWidth
                , pVtrack_item->rw.m_vibe_inparam.cloudminWidth);

        vibe_Light_Init(&pVtrack_item->rw.m_vibeparam, pSrcImage, sw, sh, pVtrack_item->rw.m_vibe_inparam, index);

        printf("(%s:%d)(ip:%s) index:%d  vibe init m_bInitOK:%d ,detection area(%d %d %d %d) mod(%d %d) minobj(%d %d) maxobj(%d %d) vibe_obj_keepTime:%d.. \n", __FUNCTION__, __LINE__,
                pVtrack_item->ro.m_config_devinfo.m_camera_ip, index, pVtrack_item->rw.m_vibeparam.m_bInitOK,
                pVtrack_item->rw.m_vibe_inparam.vibe_left, pVtrack_item->rw.m_vibe_inparam.vibe_right,
                pVtrack_item->rw.m_vibe_inparam.vibe_top, pVtrack_item->rw.m_vibe_inparam.vibe_bottom, pVtrack_item->rw.m_vibe_inparam.modw,
                pVtrack_item->rw.m_vibe_inparam.modh, pVtrack_item->rw.m_vibe_inparam.objminWidth, pVtrack_item->rw.m_vibe_inparam.objminHeight,
                pVtrack_item->rw.m_vibe_inparam.objmaxWidth, pVtrack_item->rw.m_vibe_inparam.objmaxHeight, pVtrack_item->rw.m_vibe_inparam.vibe_obj_keepTime);
    }

    int moveth = 10;
    int headth = 15;
    int tailth = 15;
    int gray = 0;
    int debug[6] = {0};

    pVtrack_item->rw.v_findmove = vibe_Light_Proc(&pVtrack_item->rw.m_vibeparam, pSrcImage, frameindex, sw, sh, moveth, headth, tailth,
            &pVtrack_item->rw.v_avegray, &pVtrack_item->rw.m_boatinfo, debug);

    //如果背景刷新出现问题，则删除vibe，重新初始化并检测
    if (pVtrack_item->rw.v_findmove == -1) {
        FsPrintf2(1, "(%s:%d) vibe is error xxx... \n", __FUNCTION__, __LINE__);
        fflush(stdout);
        vibe_Del(&pVtrack_item->rw.m_vibeparam);
    }

    if (pVtrack_item->ro.m_config_debug.m_opendebug == 2) {
        //将中间处理图片叠加到原始图像上
        vPasteY_1((unsigned char *) pFrame->data.raw[0]->data, pVtrack_item->rw.m_vibeparam.segmentation_Image, sw * 4, sh * 4, sw, 0, sh, 0, 50);
        vPasteY_1((unsigned char *) pFrame->data.raw[0]->data, pVtrack_item->rw.m_vibeparam.segmentation_Image_temp, sw * 4, sh * 4, sw, 0, sh, 20 + sw, 50);
        //叠加目标检测框

        int i;
        for (i = 0; i < pVtrack_item->rw.m_vibeparam.m_boatHeadCount; i++) {

            int last_count = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count % POINTCOUNT;
            if (last_count == 0) last_count = POINTCOUNT - 1;
            else last_count -= 1;

            tag_ITG_RECT m_rect;
            m_rect.left = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].l[last_count]*4;
            m_rect.right = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].r[last_count]*4;
            m_rect.top = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].t[last_count]*4;
            m_rect.bottom = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].b[last_count]*4;
            if (m_rect.left < 0) m_rect.left = 0;
            if (m_rect.right > (sw * 4 - 1))m_rect.right = sw * 4 - 1;
            if (m_rect.top < 0) m_rect.top = 0;
            if (m_rect.bottom > (sh * 4 - 1))m_rect.bottom = sh * 4 - 1;
            ITG_DrawRect((char *) pFrame->data.raw[0]->data, sw * 4, &m_rect, 255);
        }
    }

    /////////////////////////////////////////////////////////
    //单路测试使用
    //static int aaaaaa = 0;  objtest这个变量暂时使用，之后删除
    //如果发现目标
    if (0) {
        if (pVtrack_item->ro.m_config_debug.m_opendebug == 2 && pVtrack_item->rw.m_vibeparam.m_boatHeadCount > 0 && pVtrack_item->rw.m_vibeparam.objtest == 0) {
            FsPrintf2(1, "(%s:%d) vibe check find a obj(framedx:%d m_boatHeadCount:%d)... \n", __FUNCTION__, __LINE__, frameindex, pVtrack_item->rw.m_vibeparam.m_boatHeadCount);
            fflush(stdout);

            char filepath[512];
            sprintf(filepath, "/fs/project/data/temp/______%d_%d_%d.bmp", sw, sh, frameindex);

            ImageDib* pdib = image_dib_new__IO(pFrame->width[0], pFrame->height[0], ImageColorType_Gray, 0);
            memcpy(pdib->data, pFrame->data.raw[0]->data, pFrame->width[0] * pFrame->height[0]);
            image_dib_svae_to_file(pdib, filepath);
            image_dib_delete__OI(&pdib);

            pVtrack_item->rw.m_vibeparam.objtest = 1;
        }

        if (pVtrack_item->rw.m_vibeparam.objtest == 1) {
            if (pVtrack_item->rw.m_vibeparam.m_boatHeadCount == 0) {
                pVtrack_item->rw.m_vibeparam.objtest = 0;
            }
        }
    }
    //单路测试使用
    ///////////////////////////////////////////
    //将相关中间变量数据叠加到图像视频中，方便调试使用
    if (pVtrack_item->ro.m_config_debug.m_opendebug == 2) {

        if (pVtrack_item->ro.__pTypeFace) {

            int width = pFrame->width[0];
            int height = pFrame->height[0];

            unsigned char* p_y = (unsigned char *) pFrame->data.raw[0]->data;
            unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width*height;
            unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;

            int startx, starty;
            startx = 10;
            starty = 800;

            char overlydata[256];
            int www = abs(pVtrack_item->rw.m_boatinfo.r - pVtrack_item->rw.m_boatinfo.l);
            int hhh = abs(pVtrack_item->rw.m_boatinfo.b - pVtrack_item->rw.m_boatinfo.t);

            int dis = 0;
            int w0 = 0, h0 = 0;
            int i = 0;
            for (i = 0; i < pVtrack_item->rw.m_vibeparam.m_boatHeadCount; i++) {
                if (pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].chosen == 1) {

                    w0 = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_r - pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_l;
                    h0 = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_b - pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_t;

                    //上一个点
                    int last_count = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count % POINTCOUNT;
                    if (last_count == 0) last_count = POINTCOUNT - 1;
                    else last_count -= 1;

                    int startx = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].l[last_count];
                    int endx = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].r[last_count];
                    int starty = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].t[last_count];
                    int endy = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].b[last_count];
                    int midx = (startx + endx) / 2;
                    int midy = (starty + endy) / 2;
                    int startx1 = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_l;
                    int endx1 = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_r;
                    int starty1 = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_t;
                    int endy1 = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].init_b;
                    int midx1 = (startx1 + endx1) / 2;
                    int midy1 = (starty1 + endy1) / 2;

                    dis = (midx - midx1)*(midx - midx1) + (midy - midy1)*(midy - midy1);

                    break;
                }
            }

            sprintf(overlydata, "gray:%d, boatnum:%03d initobj(%d %d) dis:%d bestObj(%d %d %d %d) bestSize(%d %d) watergrass:%d", pVtrack_item->rw.v_avegray, pVtrack_item->rw.m_vibeparam.m_boatHeadCount,
                    w0, h0, dis, pVtrack_item->rw.m_boatinfo.l, pVtrack_item->rw.m_boatinfo.r, pVtrack_item->rw.m_boatinfo.t,
                    pVtrack_item->rw.m_boatinfo.b, www, hhh, pVtrack_item->rw.m_water_grass_count);

            fs_TypeFace_write(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_y, width, height,
                    width, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 16);
            fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_u, width, height,
                    width >> 1, startx, starty, (pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 8)&0xFF);
            fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_v, width, height,
                    width >> 1, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color & 0xFF);

        } else {

        }
    }

    //将相关中间变量数据叠加到图像视频中，方便调试使用
    if (0 && pVtrack_item->ro.m_config_debug.m_opendebug == 2) {
        if (debug[2] == 1) {
            printf("*%s :gray change fast(framedx:%d lastg:%d cg:%d debug3:%d debug4:%d)*\n",
                    pVtrack_item->ro.m_config_devinfo.m_camera_ip, frameindex, debug[1], debug[0], debug[3], debug[4]);
        }

        if (pVtrack_item->ro.__pTypeFace) {

            int width = pFrame->width[0];
            int height = pFrame->height[0];

            unsigned char* p_y = (unsigned char *) pFrame->data.raw[0]->data;
            unsigned char* p_u = (unsigned char *) pFrame->data.raw[0]->data + width*height;
            unsigned char* p_v = (unsigned char *) pFrame->data.raw[0]->data + width * height * 5 / 4;

            int startx, starty;
            startx = 10;
            starty = 800;

            char overlydata[256];
            sprintf(overlydata, "boatnum:%03d m_aveGray：%03d debug(%d %d %d %d %d)", pVtrack_item->rw.m_vibeparam.m_boatHeadCount,
                    gray, debug[0], debug[1], debug[2], debug[3], debug[4]);
            fs_TypeFace_write(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_y, width, height,
                    width, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 16);
            fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_u, width, height,
                    width >> 1, startx, starty, (pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 8)&0xFF);
            fs_TypeFace_write_uv(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_v, width, height,
                    width >> 1, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color & 0xFF);

            int i = 0;
            for (i = 0; i < pVtrack_item->rw.m_vibeparam.m_boatHeadCount; i++) {
                tag_ITG_RECT m_rect;
                m_rect.left = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].l[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1]*4;
                m_rect.right = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].r[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1]*4;
                m_rect.top = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].t[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1]*4;
                m_rect.bottom = pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].b[pVtrack_item->rw.m_vibeparam.m_boatheadPos[i].count - 1]*4;

                startx = 10;
                starty = 800 + i * 10;
                sprintf(overlydata, "boatpos:%d %d %d %d", m_rect.left, m_rect.right, m_rect.top, m_rect.bottom);
                fs_TypeFace_write(pVtrack_item->ro.__pTypeFace, (unsigned char*) overlydata, p_y, width, height,
                        width, startx, starty, pVtrack_item->ro.m_config_overly.m_overly_tracknum_color >> 16);
            }
        } else {

        }
    }

}

void setVideoDecParam(Vtrack_item *pVtrack_item);
void reSetVideoDectParam(Vtrack_item *pVtrack_item);

void addTrackObj_auto(Vtrack_item *pVtrack_item, unsigned char *image, int sw, int sh, int frameindex) {

    int j;
    //如果跟踪初始化后，设置nProcType = 1
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count < 10240) {
            pVtrack_item->rw.pm[j].nProcType = 1;
            pVtrack_item->rw.pm[j].ol = 0;
            pVtrack_item->rw.pm[j].ot = 0;
            pVtrack_item->rw.pm[j].orr = 0;
            pVtrack_item->rw.pm[j].ob = 0;
            pVtrack_item->rw.pm[j].x2 = 0;
            pVtrack_item->rw.pm[j].y2 = 0;
            pVtrack_item->rw.pm[j].x3 = 0;
            pVtrack_item->rw.pm[j].y3 = 0;

            pVtrack_item->rw.pm[j].frameIdx = frameindex;
        }
    }

    //添加新的跟踪目标，然后v_trackingObj置为1
    if (pVtrack_item->rw.v_findmove == 1 && pVtrack_item->rw.v_trackingObj == 0) {

        for (j = 0; j < MAX_OBJ; j++) {
            if (pVtrack_item->rw.pm[j].bUsed == 0) {

                pVtrack_item->rw.pm[j].bUsed = 1;
                pVtrack_item->rw.pm[j].nProcType = 0;
                pVtrack_item->rw.pm[j].disp = 0;
                pVtrack_item->rw.pm[j].frameIdx = frameindex;

                pVtrack_item->rw.pm[j].il = pVtrack_item->rw.m_boatinfo.l;
                pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.m_boatinfo.r;
                pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.m_boatinfo.b;
                pVtrack_item->rw.pm[j].it = pVtrack_item->rw.m_boatinfo.t;

                pVtrack_item->rw.pm[j].search_height = 40; //pVtrack_item->ro.m_config_track.m_trackSearchHeight;
                pVtrack_item->rw.pm[j].search_width = 40; //pVtrack_item->ro.m_config_track.m_trackSearchWidth;

                if (pVtrack_item->rw.pm[j].il < 10) pVtrack_item->rw.pm[j].il = 10;
                if (pVtrack_item->rw.pm[j].ir > sw - 10) pVtrack_item->rw.pm[j].ir = sw - 10;
                if (pVtrack_item->rw.pm[j].it < 10) pVtrack_item->rw.pm[j].it = 10;
                if (pVtrack_item->rw.pm[j].ib > sh - 10) pVtrack_item->rw.pm[j].ib = sh - 10;


                pVtrack_item->rw.v_rightMoveSteps = 0;
                pVtrack_item->rw.v_rightMoveDistance = 0;
                pVtrack_item->rw.v_leftMoveSteps = 0;
                pVtrack_item->rw.v_leftMoveDistance = 0;

                pVtrack_item->rw.pm[j].ol = 0;
                pVtrack_item->rw.pm[j].ot = 0;
                pVtrack_item->rw.pm[j].orr = 0;
                pVtrack_item->rw.pm[j].ob = 0;
                pVtrack_item->rw.pm[j].x2 = 0;
                pVtrack_item->rw.pm[j].y2 = 0;
                pVtrack_item->rw.pm[j].x3 = 0;
                pVtrack_item->rw.pm[j].y3 = 0;
                pVtrack_item->rw.pm[j].track_count = 0;
                pVtrack_item->rw.pm[j].track_error = 0;
                //pVtrack_item->rw.pm[j].search_height = 16;
                pVtrack_item->rw.pm[j].frameIdx = frameindex;


                FsPrintf(1, "(%s:%d) Add new Obj framedx:%d (l:%d r:%d t:%d b:%d)-(max_x:%d max_y:%d trackW:%d trackH:%d)-v_effective:%d-boat(l:%d r:%d b:%d h:%d)-searcharea(w:%d h:%d) \n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.pm[j].il, pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                        pVtrack_item->ro.m_config_track.m_trackWidth, pVtrack_item->ro.m_config_track.m_trackHeight, pVtrack_item->rw.v_effective, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                        pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boatheight, pVtrack_item->rw.pm[j].search_width, pVtrack_item->rw.pm[j].search_height);
                fflush(stdout);

                char message[256];
                sprintf(message, "(%s:%d) Add new Obj framedx:%d (l:%d r:%d t:%d b:%d)-(max_x:%d max_y:%d trackW:%d trackH:%d)-v_effective:%d-boat(l:%d r:%d b:%d h:%d)-searcharea(w:%d h:%d) \n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.pm[j].il, pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                        pVtrack_item->ro.m_config_track.m_trackWidth, pVtrack_item->ro.m_config_track.m_trackHeight, pVtrack_item->rw.v_effective, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                        pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boatheight, pVtrack_item->rw.pm[j].search_width, pVtrack_item->rw.pm[j].search_height);
                MakeLogRecord(message, pVtrack_item);

                break;
            }
        }

        {
            //设置跟踪编号
            int a = 0;
            int b = 99;
            pVtrack_item->rw.v_trackNum = pVtrack_item->ro.m_config_videoDect.m_devnumber * 100 + (rand() % (b - a + 1)) + a;

            pVtrack_item->rw.v_incStartTime = fs_time_uptime_get() - 8; //开始跟踪时间

            //锁定本预置位，轮循不受时间限制
            setVideoDecParam(pVtrack_item);
            //标志置为1
            pVtrack_item->rw.v_trackingObj = 1;

            FsPrintf(1, ">>>>>Add new Obj framedx:%d  v_trackNum:%lld v_incStartTime:%f<<<<<< \n", frameindex, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_incStartTime);
            fflush(stdout);
        }
    }
}

void RedaddTrackObj_auto(Vtrack_item *pVtrack_item, unsigned char *image, int sw, int sh, int frameindex) {

    int j;
    //如果跟踪初始化后，设置nProcType = 1
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count < 10240) {
            pVtrack_item->rw.pm[j].nProcType = 1;
            pVtrack_item->rw.pm[j].ol = 0;
            pVtrack_item->rw.pm[j].ot = 0;
            pVtrack_item->rw.pm[j].orr = 0;
            pVtrack_item->rw.pm[j].ob = 0;
            pVtrack_item->rw.pm[j].x2 = 0;
            pVtrack_item->rw.pm[j].y2 = 0;
            pVtrack_item->rw.pm[j].x3 = 0;
            pVtrack_item->rw.pm[j].y3 = 0;

            pVtrack_item->rw.pm[j].frameIdx = frameindex;
        }
    }

    //添加新的跟踪目标，然后v_trackingObj置为1
    if (pVtrack_item->rw.v_findmove == 1 && pVtrack_item->rw.v_trackingObj == 0) {

        for (j = 0; j < MAX_OBJ; j++) {
            if (pVtrack_item->rw.pm[j].bUsed == 0) {

                pVtrack_item->rw.pm[j].bUsed = 1;
                pVtrack_item->rw.pm[j].nProcType = 0;
                pVtrack_item->rw.pm[j].disp = 0;
                pVtrack_item->rw.pm[j].frameIdx = frameindex;

                pVtrack_item->rw.pm[j].il = pVtrack_item->rw.m_boatinfo.l;
                pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.m_boatinfo.r;
                pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.m_boatinfo.b;
                pVtrack_item->rw.pm[j].it = pVtrack_item->rw.m_boatinfo.t;

                pVtrack_item->rw.pm[j].search_height = pVtrack_item->ro.m_config_track.m_trackSearchHeight;
                pVtrack_item->rw.pm[j].search_width = pVtrack_item->ro.m_config_track.m_trackSearchWidth;

                if (pVtrack_item->rw.pm[j].il < 10) pVtrack_item->rw.pm[j].il = 10;
                if (pVtrack_item->rw.pm[j].ir > sw - 10) pVtrack_item->rw.pm[j].ir = sw - 10;
                if (pVtrack_item->rw.pm[j].it < 10) pVtrack_item->rw.pm[j].it = 10;
                if (pVtrack_item->rw.pm[j].ib > sh - 10) pVtrack_item->rw.pm[j].ib = sh - 10;


                pVtrack_item->rw.v_rightMoveSteps = 0;
                pVtrack_item->rw.v_rightMoveDistance = 0;
                pVtrack_item->rw.v_leftMoveSteps = 0;
                pVtrack_item->rw.v_leftMoveDistance = 0;

                pVtrack_item->rw.pm[j].ol = 0;
                pVtrack_item->rw.pm[j].ot = 0;
                pVtrack_item->rw.pm[j].orr = 0;
                pVtrack_item->rw.pm[j].ob = 0;
                pVtrack_item->rw.pm[j].x2 = 0;
                pVtrack_item->rw.pm[j].y2 = 0;
                pVtrack_item->rw.pm[j].x3 = 0;
                pVtrack_item->rw.pm[j].y3 = 0;
                pVtrack_item->rw.pm[j].track_count = 0;
                pVtrack_item->rw.pm[j].track_error = 0;
                //pVtrack_item->rw.pm[j].search_height = 16;
                pVtrack_item->rw.pm[j].frameIdx = frameindex;


                FsPrintf(1, "(%s:%d) Add new Obj framedx:%d (l:%d r:%d t:%d b:%d)-(max_x:%d max_y:%d trackW:%d trackH:%d)-v_effective:%d-boat(l:%d r:%d b:%d h:%d)-searcharea(w:%d h:%d) \n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.pm[j].il, pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                        pVtrack_item->ro.m_config_track.m_trackWidth, pVtrack_item->ro.m_config_track.m_trackHeight, pVtrack_item->rw.v_effective, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                        pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boatheight, pVtrack_item->rw.pm[j].search_width, pVtrack_item->rw.pm[j].search_height);
                fflush(stdout);

                char message[256];
                sprintf(message, "(%s:%d) Add new Obj framedx:%d (l:%d r:%d t:%d b:%d)-(max_x:%d max_y:%d trackW:%d trackH:%d)-v_effective:%d-boat(l:%d r:%d b:%d h:%d)-searcharea(w:%d h:%d) \n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.pm[j].il, pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                        pVtrack_item->ro.m_config_track.m_trackWidth, pVtrack_item->ro.m_config_track.m_trackHeight, pVtrack_item->rw.v_effective, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                        pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boatheight, pVtrack_item->rw.pm[j].search_width, pVtrack_item->rw.pm[j].search_height);
                MakeLogRecord(message, pVtrack_item);

                break;
            }
        }

        {
            //设置跟踪编号
            int a = 0;
            int b = 99;
            pVtrack_item->rw.v_trackNum = pVtrack_item->ro.m_config_videoDect.m_devnumber * 100 + (rand() % (b - a + 1)) + a;

            pVtrack_item->rw.v_incStartTime = fs_time_uptime_get() - 8; //开始跟踪时间

            //锁定本预置位，轮循不受时间限制
            setVideoDecParam(pVtrack_item);
            //标志置为1
            pVtrack_item->rw.v_trackingObj = 1;

            FsPrintf(1, ">>>>>11 Add new Obj framedx:%d  v_trackNum:%lld v_incStartTime:%f<<<<<< \n", frameindex, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_incStartTime);
            fflush(stdout);
        }
    }
}

//添加跟踪目标

/*************************************************
 *             添加跟踪目标
 * 
 * 设置跟踪初始位置：
 * 
 * 1.船体检测有效时
 *      根据船体的位置，适当调整跟踪初始位置
 * 
 * 2.船体检测无效时：
 *       通过(v_max_x,v_max_y)来设置跟踪框位置，跟踪框的大小m_trackWidth，m_trackHeight   
 * 
 **************************************************/
void addTrackObj(Vtrack_item *pVtrack_item, unsigned char *image, int sw, int sh, int frameindex) {

    int j;
    //如果跟踪初始化后，设置nProcType = 1
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count < 10240) {
            pVtrack_item->rw.pm[j].nProcType = 1;
            pVtrack_item->rw.pm[j].ol = 0;
            pVtrack_item->rw.pm[j].ot = 0;
            pVtrack_item->rw.pm[j].orr = 0;
            pVtrack_item->rw.pm[j].ob = 0;
            pVtrack_item->rw.pm[j].x2 = 0;
            pVtrack_item->rw.pm[j].y2 = 0;
            pVtrack_item->rw.pm[j].x3 = 0;
            pVtrack_item->rw.pm[j].y3 = 0;

            pVtrack_item->rw.pm[j].frameIdx = frameindex;
        }
    }

    //添加新的跟踪目标，然后v_trackingObj置为1
    if (pVtrack_item->rw.v_findmove == 1 && pVtrack_item->rw.v_trackingObj == 0) {

        for (j = 0; j < MAX_OBJ; j++) {
            if (pVtrack_item->rw.pm[j].bUsed == 0) {

                pVtrack_item->rw.pm[j].bUsed = 1;
                pVtrack_item->rw.pm[j].nProcType = 0;
                pVtrack_item->rw.pm[j].disp = 0;
                pVtrack_item->rw.pm[j].frameIdx = frameindex;

                //有效定位到船体
                if (pVtrack_item->rw.v_effective > 0) {
                    //船体在左侧
                    if (pVtrack_item->rw.v_effective == 1) {

                        //宽度改为设定值的2倍
                        if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {
                            pVtrack_item->rw.pm[j].il = pVtrack_item->rw.v_max_x - pVtrack_item->ro.m_config_track.m_trackWidth;
                            pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth;
                        } else {
                            pVtrack_item->rw.pm[j].il = pVtrack_item->rw.v_max_x - pVtrack_item->ro.m_config_track.m_trackWidth;
                            pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.v_max_x + pVtrack_item->rw.m_vsearch.m_modWidth;
                        }

                        pVtrack_item->rw.pm[j].it = pVtrack_item->rw.v_max_y;

                        //当运动块高度比设置的要小，且大于设定值的1/3，则说明该高度比较合理
                        if ((pVtrack_item->ro.m_config_track.m_trackHeight > pVtrack_item->rw.v_boatheight) && pVtrack_item->rw.v_boatheight > pVtrack_item->ro.m_config_track.m_trackHeight / 3) {
                            pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.v_max_y + pVtrack_item->rw.v_boatheight;
                        } else {
                            pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.v_max_y + pVtrack_item->ro.m_config_track.m_trackHeight;
                        }
                    }
                    //船体在右侧
                    if (pVtrack_item->rw.v_effective == 2) {

                        //宽度改为设定值的2倍
                        pVtrack_item->rw.pm[j].il = pVtrack_item->rw.v_max_x;
                        pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth * 2;

                        pVtrack_item->rw.pm[j].it = pVtrack_item->rw.v_max_y;
                        //当运动块高度比设置的要小，且大于设定值的1/3，则说明该高度比较合理
                        if ((pVtrack_item->ro.m_config_track.m_trackHeight > pVtrack_item->rw.v_boatheight) && pVtrack_item->rw.v_boatheight > pVtrack_item->ro.m_config_track.m_trackHeight / 3) {
                            pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.v_max_y + pVtrack_item->rw.v_boatheight;
                        } else {
                            pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.v_max_y + pVtrack_item->ro.m_config_track.m_trackHeight;
                        }
                    }
                } else { //没有定位到船体

                    if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {
                        pVtrack_item->rw.pm[j].il = pVtrack_item->rw.v_max_x;
                        pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth;
                    } else {
                        pVtrack_item->rw.pm[j].il = pVtrack_item->rw.v_max_x - pVtrack_item->ro.m_config_track.m_trackWidth / 2;
                        pVtrack_item->rw.pm[j].ir = pVtrack_item->rw.v_max_x + pVtrack_item->ro.m_config_track.m_trackWidth / 2;
                    }

                    pVtrack_item->rw.pm[j].it = pVtrack_item->rw.v_max_y;
                    pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.v_max_y + pVtrack_item->ro.m_config_track.m_trackHeight;
                }

                //重新定位船体上下界
                if (pVtrack_item->rw.v_assistfind == 1) {
                    pVtrack_item->rw.pm[j].ib = pVtrack_item->rw.v_boat_bottom_y;
                    pVtrack_item->rw.pm[j].it = pVtrack_item->rw.pm[j].ib - pVtrack_item->rw.v_boatheight;
                }

                pVtrack_item->rw.pm[j].search_height = pVtrack_item->ro.m_config_track.m_trackSearchHeight;
                pVtrack_item->rw.pm[j].search_width = pVtrack_item->ro.m_config_track.m_trackSearchWidth;

                if (pVtrack_item->rw.pm[j].il < 10) pVtrack_item->rw.pm[j].il = 10;
                if (pVtrack_item->rw.pm[j].ir > sw - 10) pVtrack_item->rw.pm[j].ir = sw - 10;
                if (pVtrack_item->rw.pm[j].it < 10) pVtrack_item->rw.pm[j].it = 10;
                if (pVtrack_item->rw.pm[j].ib > sh - 10) pVtrack_item->rw.pm[j].ib = sh - 10;


                pVtrack_item->rw.v_rightMoveSteps = 0;
                pVtrack_item->rw.v_rightMoveDistance = 0;
                pVtrack_item->rw.v_leftMoveSteps = 0;
                pVtrack_item->rw.v_leftMoveDistance = 0;

                pVtrack_item->rw.pm[j].ol = 0;
                pVtrack_item->rw.pm[j].ot = 0;
                pVtrack_item->rw.pm[j].orr = 0;
                pVtrack_item->rw.pm[j].ob = 0;
                pVtrack_item->rw.pm[j].x2 = 0;
                pVtrack_item->rw.pm[j].y2 = 0;
                pVtrack_item->rw.pm[j].x3 = 0;
                pVtrack_item->rw.pm[j].y3 = 0;
                pVtrack_item->rw.pm[j].track_count = 0;
                pVtrack_item->rw.pm[j].track_error = 0;
                //pVtrack_item->rw.pm[j].search_height = 16;
                pVtrack_item->rw.pm[j].frameIdx = frameindex;


                FsPrintf(1, "(%s:%d) Add new Obj framedx:%d (l:%d r:%d t:%d b:%d)-(max_x:%d max_y:%d trackW:%d trackH:%d)-v_effective:%d-boat(l:%d r:%d b:%d h:%d)-searcharea(w:%d h:%d) \n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.pm[j].il, pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                        pVtrack_item->ro.m_config_track.m_trackWidth, pVtrack_item->ro.m_config_track.m_trackHeight, pVtrack_item->rw.v_effective, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                        pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boatheight, pVtrack_item->rw.pm[j].search_width, pVtrack_item->rw.pm[j].search_height);
                fflush(stdout);

                char message[256];
                sprintf(message, "(%s:%d) Add new Obj framedx:%d (l:%d r:%d t:%d b:%d)-(max_x:%d max_y:%d trackW:%d trackH:%d)-v_effective:%d-boat(l:%d r:%d b:%d h:%d)-searcharea(w:%d h:%d) \n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.pm[j].il, pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib, pVtrack_item->rw.v_max_x, pVtrack_item->rw.v_max_y,
                        pVtrack_item->ro.m_config_track.m_trackWidth, pVtrack_item->ro.m_config_track.m_trackHeight, pVtrack_item->rw.v_effective, pVtrack_item->rw.v_boat_left, pVtrack_item->rw.v_boat_right,
                        pVtrack_item->rw.v_boat_bottom_y, pVtrack_item->rw.v_boatheight, pVtrack_item->rw.pm[j].search_width, pVtrack_item->rw.pm[j].search_height);
                MakeLogRecord(message, pVtrack_item);

                //for test
                if (pVtrack_item->ro.m_config_debug.savepic_flag == 1) {

                    ImageDib* pdib = image_dib_new__IO(sw, sh, ImageColorType_Gray, 0);

                    tag_ITG_RECT m_rect;
                    m_rect.left = pVtrack_item->rw.pm[j].il;
                    m_rect.right = pVtrack_item->rw.pm[j].ir;
                    m_rect.top = pVtrack_item->rw.pm[j].it;
                    m_rect.bottom = pVtrack_item->rw.pm[j].ib;

                    ITG_DrawRect((char *) image, sw, &m_rect, 0);

                    memcpy(pdib->data, image, sw * sh);
                    char path[64];
                    sprintf(path, "%s/trackinit-%d.bmp", pVtrack_item->ro.m_config_debug.savepic_path, frameindex);
                    image_dib_svae_to_file(pdib, path);
                    image_dib_delete__OI(&pdib);
                }
                //for test
                break;
            }
        }

        //标志置为1
        pVtrack_item->rw.v_trackingObj = 1;
    }
}


//方向检测

/*************************************************
 *          运动方向检测
 * 
 *  在开始跟踪时，先不要转动相机，先根据运动轨迹判断轮船行驶方向
 *  当判定行驶方向后，再联动相机(在这里，我们先假设轮船只能往一个方向行驶，不考虑折返情况) 
 * 
 * 检测条件： 方向标记pVtrack_item->rw.v_movedir==0，且正在跟踪标记pVtrack_item->rw.v_trackingObj==1
 * 
 * 检测结果输出: pVtrack_item->rw.v_movedir = 1: 向左    pVtrack_item->rw.v_movedir = 2 ： 向右
 * 
 * 如果检测到运动方向，则连接云台，准备转动！
 * 这里连接云台两次，第一次连接失败后，sleep 20毫秒，然后再连接一次。
 * 
 * 20160615修改逻辑内容；
 * 
 *    1.当运动第一次被检测到的位置太靠近两侧时，先转动一次球机，等跟踪框接近图像中间时，才开始进行方向判断（见逻辑结构图）
 * 
 **************************************************/
void detectionDir(Vtrack_item *pVtrack_item, int frameindex, int sw, int sh) {
    int j;
    if (pVtrack_item->rw.v_movedir == 0 && pVtrack_item->rw.v_trackingObj == 1) {

        for (j = 0; j < MAX_OBJ; j++) {
            if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

                //比较前后两个中心点的位置关系，做一个统计
                int lastposx = pVtrack_item->rw.pm[j].x2;
                int currentposx = (pVtrack_item->rw.pm[j].ol + pVtrack_item->rw.pm[j].orr) / 2;

                //光电仪，先屏蔽此部分...
                if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {
                    pVtrack_item->rw.v_adjustpos = 0;
                }


                //第一次先判断跟踪位置是否太靠近两侧
                if (pVtrack_item->rw.v_adjustpos == -1) {

                    if (pVtrack_item->rw.pm[j].ol < 50) {

                        FsPrintf(1, "***obj is too near to left ol:%d ..\n", pVtrack_item->rw.pm[j].ol);
                        fflush(stdout);
                        pVtrack_item->rw.v_adjustpos = 1; //太靠左
                    } else if (pVtrack_item->rw.pm[j].orr > sw - 50) {

                        FsPrintf(1, "***obj is too near to right ol:%d ..\n", pVtrack_item->rw.pm[j].orr);
                        fflush(stdout);
                        pVtrack_item->rw.v_adjustpos = 2; //太靠右
                    } else {
                        pVtrack_item->rw.v_adjustpos = 0;
                    }
                }

                //如果太靠近了，直接转动球机
                if (pVtrack_item->rw.v_adjustpos == 1 || pVtrack_item->rw.v_adjustpos == 2) {

                    int m_adjusting = 0;
                    //太靠左，云台左移..
                    if (pVtrack_item->rw.v_adjustpos == 1) {

                        if (pVtrack_item->rw.pm[j].ol > 120) {

#ifdef PRINT_DetectionDir                                 
                            FsPrintf(1, "*** the pos is ok ,stop to move  ol:%d ..\n", pVtrack_item->rw.pm[j].ol);
                            fflush(stdout);
#endif                                 
                            m_adjusting = 0;
                            pVtrack_item->rw.v_adjustpos = 0; //可以停止了
                            pVtrack_item->rw.v_adjustIndex = frameindex;

                        } else {

                            m_adjusting = 1; //左转动
                        }
                    }

                    if (pVtrack_item->rw.v_adjustpos == 2) {

                        if (pVtrack_item->rw.pm[j].orr < 360) {

#ifdef PRINT_DetectionDir                                
                            FsPrintf(1, "*** the pos is ok ,stop to move  or:%d ..\n", pVtrack_item->rw.pm[j].orr);
                            fflush(stdout);
#endif  

                            m_adjusting = 0;
                            pVtrack_item->rw.v_adjustpos = 0; //可以停止了
                            pVtrack_item->rw.v_adjustIndex = frameindex;

                        } else {

                            m_adjusting = 2; //右转动
                        }
                    }

                    pVtrack_item->rw.v_camctrl_dir = m_adjusting;

#ifdef PRINT_DetectionDir
                    FsPrintf(1, "*** v_camctrl_dir:%d  index:(%d %d %d)..\n", m_adjusting, frameindex, pVtrack_item->rw.v_holder_lastctrlDx, pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1);
                    fflush(stdout);
#endif  

                    if (pVtrack_item->rw.v_camctrl_dir != 0) {
                        if (frameindex - pVtrack_item->rw.v_holder_lastctrlDx > pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1) {

                            linkage_ctrl_moveLeftRight(pVtrack_item);

                            pVtrack_item->rw.v_holder_lastmoveXDx = frameindex; //记录上一次云台向左转动的帧号(用来控制前后两次转动命令的间隔帧)
                            pVtrack_item->rw.v_holder_lastctrlDx = frameindex; //记录上一次云台转动的帧号(用来控制向云台发送心跳信号的时间)
                            pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DetectionDir
                            FsPrintf(1, "*** turn (famedx:%d  ==movingX:%d  v_camctrl_dir:%d) ..\n", frameindex,
                                    pVtrack_item->rw.v_camctrl_movingX, pVtrack_item->rw.v_camctrl_dir);
                            fflush(stdout);
                            char message[256];
                            sprintf(message, "(%s:%d)  turn framedx:%d. movingX:%d \n", __FUNCTION__, __LINE__,
                                    frameindex, pVtrack_item->rw.v_camctrl_movingX);
                            MakeLogRecord(message, pVtrack_item);
#endif                        
                        }
                        //转动球机

                        //强制停止左右转动：间隔帧时间就是球机转动的时间
                        if (pVtrack_item->rw.v_camctrl_movingX != 0) {
                            if (frameindex - pVtrack_item->rw.v_holder_lastmoveXDx >= (pVtrack_item->ro.m_config_holder.m_holder_forceStopX/*+pVtrack_item->rw.v_holder_incmove*/)) {

                                linkage_ctrl_stopMoveLeftRight(pVtrack_item);
                                pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DetectionDir                        
                                FsPrintf(1, "**..stop turn (famedx:%d  movingX:%d) ..\n", frameindex, pVtrack_item->rw.v_camctrl_movingX);
                                fflush(stdout);
                                char message[256];
                                sprintf(message, "(%s:%d)  stop turn  framedx:%d. movingX:%d \n", __FUNCTION__, __LINE__,
                                        frameindex, pVtrack_item->rw.v_camctrl_movingX);
                                MakeLogRecord(message, pVtrack_item);
#endif                        
                            }
                        }
                    } else {
                        pVtrack_item->rw.v_camctrl_dir = 0;
                        pVtrack_item->rw.v_camctrl_movingX = 0;
                        pVtrack_item->rw.v_holder_lastConnectdx = 0;
                        pVtrack_item->rw.v_holder_lastmoveXDx = 0;
                        pVtrack_item->rw.v_holder_lastctrlDx = 0;

                        linkage_ctrl_stopMoveLeftRight(pVtrack_item);
                    }
                }

                //如果还在调整过程中，或者离调整结束时间不到1秒。就直接返回
                if (pVtrack_item->rw.v_adjustpos == 1 || pVtrack_item->rw.v_adjustpos == 2 ||
                        (pVtrack_item->rw.v_adjustpos == 0 && (frameindex - pVtrack_item->rw.v_adjustIndex) < 20)) {

                    return;
                }



                //右移
                if ((currentposx - lastposx) >= 1) {
                    pVtrack_item->rw.v_rightMoveDistance += (currentposx - lastposx);
                    pVtrack_item->rw.v_rightMoveSteps++;
                    if (pVtrack_item->rw.v_leftMoveSteps > 0) pVtrack_item->rw.v_leftMoveSteps--;
                }
                //左移
                if ((lastposx - currentposx) >= 1) {
                    pVtrack_item->rw.v_leftMoveDistance += (lastposx - currentposx);
                    pVtrack_item->rw.v_leftMoveSteps++;
                    if (pVtrack_item->rw.v_rightMoveSteps > 0) pVtrack_item->rw.v_rightMoveSteps--;
                }

#ifdef PRINT_DEBUG              
                FsPrintf(1, "(%s:%d)..detecting the direction:framedx:%d  currentposx:%d lastposx:%d  right_times:%d right_num:%d left_times:%d left_num:%d  m_cond_dir:%d \n", __FUNCTION__, __LINE__,
                        frameindex, currentposx, lastposx, pVtrack_item->rw.v_rightMoveSteps, pVtrack_item->rw.v_rightMoveDistance, pVtrack_item->rw.v_leftMoveSteps,
                        pVtrack_item->rw.v_leftMoveDistance, pVtrack_item->ro.m_config_holder.m_cond_dir);
                fflush(stdout);

                char message[256];
                sprintf(message, "(%s:%d)..detecting the direction:framedx:%d  currentposx:%d lastposx:%d  right_times:%d right_num:%d left_times:%d left_num:%d  m_cond_dir:%d \n", __FUNCTION__, __LINE__,
                        frameindex, currentposx, lastposx, pVtrack_item->rw.v_rightMoveSteps, pVtrack_item->rw.v_rightMoveDistance, pVtrack_item->rw.v_leftMoveSteps,
                        pVtrack_item->rw.v_leftMoveDistance, pVtrack_item->ro.m_config_holder.m_cond_dir);
                MakeLogRecord(message, pVtrack_item);
#endif

                //如果目标连续往右移动五次以上，且移动总距离大于阈值，则判定船向右运动
                if (pVtrack_item->rw.v_rightMoveSteps > 5 && pVtrack_item->rw.v_rightMoveDistance > pVtrack_item->ro.m_config_holder.m_cond_dir) {

                    pVtrack_item->rw.v_movedir = 2;

                    FsPrintf(1, "(%s:%d).trackNum:%lld detect the direction success(move to right): right_times:%d right_num:%d v_movedir:%d \n", __FUNCTION__, __LINE__,
                            pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_rightMoveSteps, pVtrack_item->rw.v_rightMoveDistance, pVtrack_item->rw.v_movedir);
                    fflush(stdout);
                    char message[256];
                    sprintf(message, "(%s:%d).detect the direction success(move to right): right_times:%d right_num:%d v_movedir:%d \n", __FUNCTION__, __LINE__,
                            pVtrack_item->rw.v_rightMoveSteps, pVtrack_item->rw.v_rightMoveDistance, pVtrack_item->rw.v_movedir);
                    MakeLogRecord(message, pVtrack_item);

                    pVtrack_item->rw.v_rightMoveSteps = 0;
                    pVtrack_item->rw.v_rightMoveDistance = 0;
                    pVtrack_item->rw.v_leftMoveSteps = 0;
                    pVtrack_item->rw.v_leftMoveDistance = 0;
                }

                if (pVtrack_item->rw.v_leftMoveSteps > 5 && pVtrack_item->rw.v_leftMoveDistance > pVtrack_item->ro.m_config_holder.m_cond_dir) {

                    pVtrack_item->rw.v_movedir = 1;

                    FsPrintf(1, "(%s:%d).trackNum:%lld detect the direction success(move to left): right_times:%d right_num:%d v_movedir:%d \n", __FUNCTION__, __LINE__,
                            pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_leftMoveSteps, pVtrack_item->rw.v_leftMoveDistance, pVtrack_item->rw.v_movedir);
                    fflush(stdout);
                    char message[256];
                    sprintf(message, "(%s:%d).detect the direction success(move to left): right_times:%d right_num:%d v_movedir:%d \n", __FUNCTION__, __LINE__,
                            pVtrack_item->rw.v_leftMoveSteps, pVtrack_item->rw.v_leftMoveDistance, pVtrack_item->rw.v_movedir);
                    MakeLogRecord(message, pVtrack_item);

                    pVtrack_item->rw.v_rightMoveSteps = 0;
                    pVtrack_item->rw.v_rightMoveDistance = 0;
                    pVtrack_item->rw.v_leftMoveSteps = 0;
                    pVtrack_item->rw.v_leftMoveDistance = 0;
                }
            }
        }
    }
}


//保存跟踪录像

/*************************************************
 *          保存跟踪录像
 * 
 *  将开始时间和结束时间以及保存路径传入函数record_item_add_partVideo()中
 * 
 * 这里需要判断record_item_add_partVideo函数指针是否为空，如果为空，表示保存线程还未准备好!
 * 
 **************************************************/
void SaveTrackVideo(Vtrack_item *pVtrack_item) {
    pthread_mutex_lock(&pVtrack_item->ro.__mutexCmdConnect);
    if (pVtrack_item->p.record_item_add_partVideo) {

        if (pVtrack_item->rw.v_videoPath[0] == 0) {

            FsPrintf2(1, "!!!!! video_path is error error !!!!! \n");

            time_t ts = (time_t) fs_time_GMT_get();
            struct tm ti_s;
            localtime_r(&ts, &ti_s);
            sprintf(pVtrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                    pVtrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

            sprintf(pVtrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pVtrack_item->ro._pVtrack->ro._dir,
                    pVtrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
        }


        FsPrintf2(1, "#(%s %d)save video path:%s time(%f %f %f)\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_videoPath,
                pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime, fs_time_uptime_get());

        if (pVtrack_item->rw.v_incEndTime <= pVtrack_item->rw.v_incStartTime) {
            pVtrack_item->rw.v_incEndTime = pVtrack_item->rw.v_incStartTime + 0.5;

            FsPrintf2(1, "(%s %d)the inc video time is exceptional:%s time(%f %f %f)\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_videoPath,
                    pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime, fs_time_uptime_get());
        } else if (pVtrack_item->rw.v_incEndTime - pVtrack_item->rw.v_incStartTime > 60 * 10) {
            pVtrack_item->rw.v_incEndTime = pVtrack_item->rw.v_incStartTime + 60 * 10;

            FsPrintf2(1, "(%s %d)the inc video time is exceptional:%s time(%f %f %f)\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.v_videoPath,
                    pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime, fs_time_uptime_get());
        }

        pVtrack_item->p.record_item_add_partVideo(pVtrack_item->p.pRecord_item, pVtrack_item->rw.v_videoPath, pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime, NULL);
        fflush(stdout);
    } else {
        FsPrintf(1, ".record_item_add_partVideo is NULL !!!.....\n");
        fflush(stdout);
        char message[256];
        sprintf(message, ".record_item_add_partVideo is NULL !!!.....\n");
        MakeLogRecord(message, pVtrack_item);
    }
    pthread_mutex_unlock(&pVtrack_item->ro.__mutexCmdConnect);
}

void debugRecord(Vtrack_item *pVtrack_item);
void sendMsg2Camctl(Vtrack_item *pVtrack_item, int tpye);

void checkTrack_auto(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, int frameindex, int sw, int sh) {
    int j;

    int nEraseReason = 0;

    //如果在设定帧数内检测不到合适运动块，跟踪结束
    if (pVtrack_item->rw.v_findmove == -1) {
        //nEraseReason = 100;
    }

    //如果海康球机控制失败，跟踪结束(因为无法进行联动了)
    if (pVtrack_item->rw.v_hiki_unConnect == 1) {
        nEraseReason = 101;
    }

    //在跟踪过程中，判定结束...
    int m_track_count = 0;
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1) {
            //当前坐标位置与初始位置之间的相对位置
            int posy3 = pVtrack_item->rw.pm[j].y2;
            int posy2 = (pVtrack_item->rw.pm[j].it + pVtrack_item->rw.pm[j].ib) / 2;
            int posy1 = (pVtrack_item->rw.pm[j].ot + pVtrack_item->rw.pm[j].ob) / 2;

            int posx3 = pVtrack_item->rw.pm[j].x2;
            int posx2 = (pVtrack_item->rw.pm[j].ol + pVtrack_item->rw.pm[j].orr) / 2;
            int posx1 = (pVtrack_item->rw.pm[j].il + pVtrack_item->rw.pm[j].ir) / 2;

            int dy = (posy1 - posy3) > 0 ? (posy1 - posy3) : (posy3 - posy1);
            int dx = posx2 - posx3;

            //当跟踪次数等于25时，才向控制器发送锁定指令    
            if (pVtrack_item->rw.pm[j].track_count == 13 * pVtrack_item->ro.m_config_videoDect.m_keepTime) {
                if (pVtrack_item->rw.m_autotrack_status == 0) {

                    int m_floater_valid = 0;
                    //漂浮物检测标志有效，且水草检测因子大于零
                    if (pVtrack_item->ro.m_config_videoDect.m_floater_valid == 1) {

                        int x0 = (pVtrack_item->rw.pm[j].ol * 4 - 5) > 0 ? (pVtrack_item->rw.pm[j].ol * 4 - 5) : 0;
                        int y0 = (pVtrack_item->rw.pm[j].ot * 4 - 5) > 0 ? (pVtrack_item->rw.pm[j].ot * 4 - 5) : 0;
                        int x1 = (pVtrack_item->rw.pm[j].orr * 4 + 5) < (sw * 4 - 1) ? (pVtrack_item->rw.pm[j].orr * 4 + 5) : (sw * 4 - 1);
                        int y1 = (pVtrack_item->rw.pm[j].ob * 4 + 5) < (sh * 4 - 1) ? (pVtrack_item->rw.pm[j].ob * 4 + 5) : (sh * 4 - 1);
                        m_floater_valid = alg_check_floater(pVtrack_item, pFrame, x0, y0, x1, y1, pVtrack_item->ro.m_config_debug.m_opendebug);
                    }

                    //不是水草
                    if (m_floater_valid == 0) {
                        //标志置为1
                        pVtrack_item->rw.v_trackingObj = 1;
                        //向控制器发送指令
                        sendMsg2Camctl(pVtrack_item, 1);
                        FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj success(m_keepTime:%d)... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->ro.m_config_videoDect.m_keepTime);
                    } else {
                        nEraseReason = 200;
                        FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj fails(m_autotrack_status:%d)... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.m_autotrack_status);
                    }
                }
            }


            ////////////////////////////
            //锁定成功之后，继续进行目标判定
            //1.继续进行,暂时先不继续
            if (0 && pVtrack_item->rw.v_trackingObj == 1) {
                if (pVtrack_item->rw.pm[j].track_count % 10 == 0) {
                    //漂浮物检测标志有效，且水草检测因子大于零
                    if (pVtrack_item->ro.m_config_videoDect.m_floater_valid == 1 && pVtrack_item->ro.m_config_videoDect.m_watergrass_factor > 0) {
                        //检测一下，是否为水草   
                        int x0 = (pVtrack_item->rw.pm[j].ol * 4 - 5) > 0 ? (pVtrack_item->rw.pm[j].ol * 4 - 5) : 0;
                        int y0 = (pVtrack_item->rw.pm[j].ot * 4 - 5) > 0 ? (pVtrack_item->rw.pm[j].ot * 4 - 5) : 0;
                        int x1 = (pVtrack_item->rw.pm[j].orr * 4 + 5) < (sw * 4 - 1) ? (pVtrack_item->rw.pm[j].orr * 4 + 5) : (sw * 4 - 1);
                        int y1 = (pVtrack_item->rw.pm[j].ob * 4 + 5) < (sh * 4 - 1) ? (pVtrack_item->rw.pm[j].ob * 4 + 5) : (sh * 4 - 1);
                        pVtrack_item->rw.m_water_grass_count = alg_check_floater(pVtrack_item, pFrame, x0, y0, x1, y1, 0);

                        if (pVtrack_item->rw.m_water_grass_count > pVtrack_item->ro.m_config_videoDect.m_watergrass_factor) {
                            nEraseReason = 200;
                        }
                    }
                }
            }
            ////////////////////////////
            m_track_count = pVtrack_item->rw.pm[j].track_count;

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].track_error > 0) {
                nEraseReason = 1;
            }
            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].track_count > 100000) {
                nEraseReason = 2;
            }
            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ol < 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 3;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].orr > sw - 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 4;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ot < 5 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 5;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ob > sh - 5 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 6;
            }

            if (nEraseReason == 0 && posx2 < 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 7;
            }
            if (nEraseReason == 0 && posx2 > sw - 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 8;
                FsPrintf(2, ".nEraseReason:%d  or:%d.....\n", nEraseReason, pVtrack_item->rw.pm[j].orr);
                fflush(stdout);
            }

            //目标在y轴方向上的运动位移超过阈值
            if (nEraseReason == 0 && dy > pVtrack_item->ro.m_config_track.m_trackOverFlag2 && pVtrack_item->rw.pm[j].track_count > 1) {
                // nEraseReason = 9;
                // FsPrintf(2, ".nEraseReason:%d  dy:%d.....\n", nEraseReason, dy);
                //  fflush(stdout);
            }

            //检测方向为左，但目标向右运动超过阈值
            if (nEraseReason == 0 && pVtrack_item->rw.v_movedir == 1 && (dx > pVtrack_item->ro.m_config_track.m_trackOverFlag1) && pVtrack_item->rw.pm[j].track_count > 1) {
                // nEraseReason = 10;
                // FsPrintf(2, ".nEraseReason:%d  dx:%d.....\n", nEraseReason, dx);
                // fflush(stdout);
            }
            //检测方向为右，但目标向左运动超过阈值
            if (nEraseReason == 0 && pVtrack_item->rw.v_movedir == 2 && (dx < -pVtrack_item->ro.m_config_track.m_trackOverFlag1) && pVtrack_item->rw.pm[j].track_count > 1) {
                //nEraseReason = 11;
                //FsPrintf(2, ".nEraseReason:%d  dx:%d.....\n", nEraseReason, dx);
                // fflush(stdout);
            }

            //到达最长跟踪时限了..
            if (nEraseReason == 0 && (float) (fs_time_uptime_get() - pVtrack_item->rw.v_incStartTime) > pVtrack_item->ro.m_config_track.m_trackOverTime * 60) {
                nEraseReason = 12;
                FsPrintf(2, ".track too long  dx:%d  [%f  %f  %f].....\n", nEraseReason, dx, fs_time_uptime_get(), pVtrack_item->rw.v_incStartTime, pVtrack_item->ro.m_config_track.m_trackOverTime);
                fflush(stdout);
            }

            //vibe运动块删除掉了....
            if (pVtrack_item->rw.m_boatinfo.isvalid == 0) {
                nEraseReason = 11;
                FsPrintf(2, ".....the vibe_boat is delete.....\n");
                fflush(stdout);
            }
        }
    }

    //跟踪结束
    if (nEraseReason > 0) {

        //海康球机，如果是和球机的通讯有问题，直接结束跟踪，并且不再操作球机
        if (nEraseReason == 101) {

            FsPrintf(1, "(%s:%d)....track over，for the hiki connect is wrong, nEraseReason:%d  v_trackobj_times:%d.....\n", __FUNCTION__, __LINE__, nEraseReason, pVtrack_item->rw.v_trackobj_times);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            //再次尝试发送停止指令
            linkage_ctrl_stopMoveLeftRight(pVtrack_item);
            //pVtrack_item->rw.v_incEndTime = fs_time_uptime_get(); 
            debugRecord(pVtrack_item);

            //向控制器发送指令
            sendMsg2Camctl(pVtrack_item, 2);

            resetParam(pVtrack_item);
            reSetVideoDectParam(pVtrack_item);
        }/* (跟踪过程检测为虚警) */
        else if (nEraseReason == 200) {
            FsPrintf(1, "(%s:%d).....track over nEraseReason:%d  (tracknum:%lld track_count:%d movedir:%d falsealarm:%d).....\n", __FUNCTION__, __LINE__, nEraseReason, pVtrack_item->rw.v_trackNum,
                    m_track_count, pVtrack_item->rw.v_movedir, pVtrack_item->rw.m_isfalsealarm);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            pVtrack_item->rw.v_incEndTime = fs_time_uptime_get() + 8;
            FsPrintf(1, "before savetrackvideo(Line:%d ip:%s)(v_incStartTime:%f v_incEndTime:%f).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime);
            fflush(stdout);
            //SaveTrackVideo(pVtrack_item);

            //清零所有中间参数
            //向控制器发送指令
            //debugRecord(pVtrack_item);
            //sendMsg2Camctl(pVtrack_item, 3);
            resetParam(pVtrack_item);
            reSetVideoDectParam(pVtrack_item);

        }/* 真正结束...(跟踪次数到了,或者已经达到最长跟踪时间) */
        else if (pVtrack_item->rw.v_trackobj_times <= 0 || nEraseReason == 12) {

            FsPrintf(1, "(%s:%d).....track over nEraseReason:%d  (tracknum:%lld track_count:%d movedir:%d falsealarm:%d).....\n", __FUNCTION__, __LINE__, nEraseReason, pVtrack_item->rw.v_trackNum,
                    m_track_count, pVtrack_item->rw.v_movedir, pVtrack_item->rw.m_isfalsealarm);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            if (pVtrack_item->rw.v_trackingObj == 1) {

                pVtrack_item->rw.v_incEndTime = fs_time_uptime_get() + 8;
                FsPrintf(1, "before savetrackvideo(Line:%d ip:%s)(v_incStartTime:%f v_incEndTime:%f).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime);
                fflush(stdout);
                SaveTrackVideo(pVtrack_item);

                //清零所有中间参数
                //向控制器发送指令
                debugRecord(pVtrack_item);
                sendMsg2Camctl(pVtrack_item, 2);
                resetParam(pVtrack_item);
                reSetVideoDectParam(pVtrack_item);

                pVtrack_item->rw.v_lastIncTime = fs_time_uptime_get();
            } else {

                printf("%s %d, %s: the obj was not locked, so do not saveTrackVideo... \n", __FUNCTION__, __LINE__, pVtrack_item->ro._uuid);
                resetParam(pVtrack_item);
                reSetVideoDectParam(pVtrack_item);
            }

        } else if (pVtrack_item->rw.v_trackobj_times > 0) { //跟踪次数没到，且没有到时间

            pVtrack_item->rw.v_trackobj_times--; //使用次数减一

            resetParam_1(pVtrack_item);

            FsPrintf(1, "......track again  (nEraseReason:%d  v_trackobj_times:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track again  (nEraseReason:%d  v_trackobj_times:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);
        }
    }
}

void RedcheckTrack_auto(Vtrack_item *pVtrack_item, int frameindex, int sw, int sh) {
    int j;

    int nEraseReason = 0;

    //如果在设定帧数内检测不到合适运动块，跟踪结束
    if (pVtrack_item->rw.v_findmove == -1) {
        nEraseReason = 100;
    }

    //如果海康球机控制失败，跟踪结束(因为无法进行联动了)
    if (pVtrack_item->rw.v_hiki_unConnect == 1) {
        nEraseReason = 101;
    }

    //在跟踪过程中，判定结束...
    int m_track_count = 0;
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1) {
            //当前坐标位置与初始位置之间的相对位置
            int posy3 = pVtrack_item->rw.pm[j].y2;
            int posy2 = (pVtrack_item->rw.pm[j].it + pVtrack_item->rw.pm[j].ib) / 2;
            int posy1 = (pVtrack_item->rw.pm[j].ot + pVtrack_item->rw.pm[j].ob) / 2;

            int posx3 = pVtrack_item->rw.pm[j].x2;
            int posx2 = (pVtrack_item->rw.pm[j].ol + pVtrack_item->rw.pm[j].orr) / 2;
            int posx1 = (pVtrack_item->rw.pm[j].il + pVtrack_item->rw.pm[j].ir) / 2;


            int dy = (posy1 - posy3) > 0 ? (posy1 - posy3) : (posy3 - posy1);
            int dx = posx2 - posx3;

            //pVtrack_item->ro.m_config_videoDect.vibe_obj_moveMinDis
            int dis = (posx2 - posx1)*(posx2 - posx1) + (posy2 - posy1)*(posy2 - posy1);

            //当跟踪次数等于25时，才向控制器发送锁定指令  
            if (pVtrack_item->rw.pm[j].track_count == 13 * pVtrack_item->ro.m_config_videoDect.m_keepTime) {
                //如果超过跟踪最短距离
                if (dis > pVtrack_item->ro.m_config_videoDect.vibe_obj_locked_moveMinDis * pVtrack_item->ro.m_config_videoDect.vibe_obj_locked_moveMinDis) {

                    //pVtrack_item->rw.v_lastIncTime = fs_time_uptime_get();
                    //还在时限内...
                    if (pVtrack_item->ro.m_config_videoDect.m_time_interval > 0 && fs_time_uptime_get() - pVtrack_item->rw.v_lastIncTime < pVtrack_item->ro.m_config_videoDect.m_time_interval) {

                        FsPrintf(1, "(%s:%d)  the inc is too close to last inc....lock the obj fails(v_lastIncTime:%lf,m_time_interval:%d)... \n", __FUNCTION__, __LINE__,
                                pVtrack_item->rw.v_lastIncTime, pVtrack_item->ro.m_config_videoDect.m_time_interval);

                        nEraseReason = 200;
                    } else if (pVtrack_item->rw.m_autotrack_status == 0) {
                        //标志置为1
                        pVtrack_item->rw.v_trackingObj = 1;
                        //向控制器发送指令
                        sendMsg2Camctl(pVtrack_item, 1);
                        FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj success(m_keepTime:%d,dis:%d vibe_obj_locked_moveMinDis:%d)... \n", __FUNCTION__, __LINE__,
                                pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->ro.m_config_videoDect.m_keepTime, dis, pVtrack_item->ro.m_config_videoDect.vibe_obj_locked_moveMinDis);
                    } else {
                        FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj fails(m_autotrack_status:%d)... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.m_autotrack_status);
                    }
                } else {

                    FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj fails，for the obj move too little(dis:%d vibe_obj_locked_moveMinDis:%d)... \n",
                            __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, dis, pVtrack_item->ro.m_config_videoDect.vibe_obj_locked_moveMinDis);
                    nEraseReason = 200;
                }
            }
            //            //当跟踪次数等于25时，才向控制器发送锁定指令    
            //            if (pVtrack_item->rw.pm[j].track_count == 15 * 7) {
            //
            //                FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj success... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum);
            //                //标志置为1
            //                pVtrack_item->rw.v_trackingObj = 1;
            //                //向控制器发送指令
            //                //sendMsg2Camctl(pVtrack_item,1);
            //            }

            m_track_count = pVtrack_item->rw.pm[j].track_count;

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].track_error > 0) {
                nEraseReason = 1;
            }
            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].track_count > 100000) {
                nEraseReason = 2;
            }
            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ol < 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 3;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].orr > sw - 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 4;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ot < 5 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 5;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ob > sh - 5 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 6;
            }

            if (nEraseReason == 0 && posx2 < 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 7;
            }
            if (nEraseReason == 0 && posx2 > sw - 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 8;
                FsPrintf(2, ".nEraseReason:%d  or:%d.....\n", nEraseReason, pVtrack_item->rw.pm[j].orr);
                fflush(stdout);
            }

            //目标在y轴方向上的运动位移超过阈值
            if (nEraseReason == 0 && dy > pVtrack_item->ro.m_config_track.m_trackOverFlag2 && pVtrack_item->rw.pm[j].track_count > 1) {
                // nEraseReason = 9;
                // FsPrintf(2, ".nEraseReason:%d  dy:%d.....\n", nEraseReason, dy);
                //  fflush(stdout);
            }

            //检测方向为左，但目标向右运动超过阈值
            if (nEraseReason == 0 && pVtrack_item->rw.v_movedir == 1 && (dx > pVtrack_item->ro.m_config_track.m_trackOverFlag1) && pVtrack_item->rw.pm[j].track_count > 1) {
                // nEraseReason = 10;
                // FsPrintf(2, ".nEraseReason:%d  dx:%d.....\n", nEraseReason, dx);
                // fflush(stdout);
            }
            //检测方向为右，但目标向左运动超过阈值
            if (nEraseReason == 0 && pVtrack_item->rw.v_movedir == 2 && (dx < -pVtrack_item->ro.m_config_track.m_trackOverFlag1) && pVtrack_item->rw.pm[j].track_count > 1) {
                //nEraseReason = 11;
                //FsPrintf(2, ".nEraseReason:%d  dx:%d.....\n", nEraseReason, dx);
                // fflush(stdout);
            }

            //到达最长跟踪时限了..
            if (nEraseReason == 0 && (float) (fs_time_uptime_get() - pVtrack_item->rw.v_incStartTime) > pVtrack_item->ro.m_config_track.m_trackOverTime * 60) {
                nEraseReason = 12;
                FsPrintf(2, ".track too long  dx:%d  [%f  %f  %f].....\n", nEraseReason, dx, fs_time_uptime_get(), pVtrack_item->rw.v_incStartTime, pVtrack_item->ro.m_config_track.m_trackOverTime);
                fflush(stdout);
            }

            //vibe运动块删除掉了....
            if (pVtrack_item->rw.m_boatinfo.isvalid == 0) {
                nEraseReason = 11;
                FsPrintf(2, ".....the vibe_boat is delete.....\n");
                fflush(stdout);
            }
        }
    }

    //跟踪结束
    if (nEraseReason > 0) {

        //海康球机，如果是和球机的通讯有问题，直接结束跟踪，并且不再操作球机
        if (nEraseReason == 101) {

            FsPrintf(1, "(%s:%d)....track over，for the hiki connect is wrong, nEraseReason:%d  v_trackobj_times:%d.....\n", __FUNCTION__, __LINE__, nEraseReason, pVtrack_item->rw.v_trackobj_times);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            //再次尝试发送停止指令
            //linkage_ctrl_stopMoveLeftRight(pVtrack_item);
            //pVtrack_item->rw.v_incEndTime = fs_time_uptime_get(); 

            //向控制器发送指令
            sendMsg2Camctl(pVtrack_item, 2);
            resetParam(pVtrack_item);
            reSetVideoDectParam(pVtrack_item);
        }/* (跟踪过程检测为虚警) */
        else if (nEraseReason == 200) {

            FsPrintf(1, "(%s:%d).....track over nEraseReason:%d  the inc is too close to last inc.....\n", __FUNCTION__, __LINE__, nEraseReason);
            fflush(stdout);

            pVtrack_item->rw.v_incEndTime = fs_time_uptime_get() + 8;
            FsPrintf(1, "before savetrackvideo(Line:%d ip:%s)(v_incStartTime:%f v_incEndTime:%f).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime);
            fflush(stdout);
            //SaveTrackVideo(pVtrack_item);

            //清零所有中间参数
            //向控制器发送指令
            //debugRecord(pVtrack_item);
            //sendMsg2Camctl(pVtrack_item, 3);
            resetParam(pVtrack_item);
            reSetVideoDectParam(pVtrack_item);
        }//真正结束...(跟踪次数到了，或者已经达到最长跟踪时间)
        else if (pVtrack_item->rw.v_trackobj_times <= 0 || nEraseReason == 12) {

            FsPrintf(1, "(%s:%d).....track over nEraseReason:%d  (tracknum:%lld track_count:%d movedir:%d falsealarm:%d).....\n", __FUNCTION__, __LINE__, nEraseReason, pVtrack_item->rw.v_trackNum,
                    m_track_count, pVtrack_item->rw.v_movedir, pVtrack_item->rw.m_isfalsealarm);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            if (pVtrack_item->rw.v_trackingObj == 1) {

                pVtrack_item->rw.v_incEndTime = fs_time_uptime_get() + 8;
                FsPrintf(1, "before savetrackvideo(Line:%d ip:%s)(v_incStartTime:%f v_incEndTime:%f).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_incStartTime, pVtrack_item->rw.v_incEndTime);
                fflush(stdout);
                SaveTrackVideo(pVtrack_item);

                //清零所有中间参数
                //向控制器发送指令
                sendMsg2Camctl(pVtrack_item, 2);
                resetParam(pVtrack_item);
                reSetVideoDectParam(pVtrack_item);

                pVtrack_item->rw.v_lastIncTime = fs_time_uptime_get();
            } else {
                printf("%s %d, %s: the obj was not locked, so do not saveTrackVideo... \n", __FUNCTION__, __LINE__, pVtrack_item->ro._uuid);
                resetParam(pVtrack_item);
                reSetVideoDectParam(pVtrack_item);
            }

        } else if (pVtrack_item->rw.v_trackobj_times > 0) { //跟踪次数没到，且没有到时间

            pVtrack_item->rw.v_trackobj_times--; //使用次数减一

            resetParam_1(pVtrack_item);

            FsPrintf(1, "......track again  (nEraseReason:%d  v_trackobj_times:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track again  (nEraseReason:%d  v_trackobj_times:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);
        }
    }
}
//结束跟踪判断

/*************************************************
 *                 跟踪结束判断
 * 
 *  跟踪结束条件：
 *  
 **************************************************/
int checkTrack(Vtrack_item *pVtrack_item, int frameindex, int sw, int sh) {
    int j;

    int nEraseReason = 0;

    //如果在设定帧数内检测不到合适运动块，跟踪结束
    if (pVtrack_item->rw.v_findmove == -1) {
        nEraseReason = 100;
    }

    //如果海康球机控制失败，跟踪结束(因为无法进行联动了)
    if (pVtrack_item->rw.v_hiki_unConnect == 1) {
        nEraseReason = 101;
    }

    //在跟踪过程中，判定结束...
    int m_track_count = 0;
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1) {
            //当前坐标位置与初始位置之间的相对位置
            int posy3 = pVtrack_item->rw.pm[j].y2;
            int posy2 = (pVtrack_item->rw.pm[j].it + pVtrack_item->rw.pm[j].ib) / 2;
            int posy1 = (pVtrack_item->rw.pm[j].ot + pVtrack_item->rw.pm[j].ob) / 2;

            int posx3 = pVtrack_item->rw.pm[j].x2;
            int posx2 = (pVtrack_item->rw.pm[j].ol + pVtrack_item->rw.pm[j].orr) / 2;
            int posx1 = (pVtrack_item->rw.pm[j].il + pVtrack_item->rw.pm[j].ir) / 2;

            int dy = (posy1 - posy3) > 0 ? (posy1 - posy3) : (posy3 - posy1);
            int dx = posx2 - posx3;

            //目标跟踪5次的时候，再发送锁定成功的指令
            //如果本目标已经连续2次被判定为虚警。
            //那么，从第三次开始，通过图像检测，当目标检测出运动方向以后，再发送锁定指令

            //如果本目标并未连续2此被判定为虚警，则目标跟踪25次时，发送锁定指令
            if (pVtrack_item->rw.m_isfalsealarm == 1) {

                if (pVtrack_item->rw.pm[j].track_count == 50 && (pVtrack_item->rw.v_movedir == 1 || pVtrack_item->rw.v_movedir == 2)) {
                    FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj success(track_cout:%d v_movedir:%d)... \n", __FUNCTION__, __LINE__,
                            pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum, pVtrack_item->rw.pm[j].track_count, pVtrack_item->rw.v_movedir);

                    pVtrack_item->rw.v_locktrackObj = 1;
                    AddtoFalseAlarm(pVtrack_item, 0);
                }
            } else {

                if (pVtrack_item->rw.pm[j].track_count == 25) {
                    FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) lock the obj success... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum);
                    pVtrack_item->rw.v_locktrackObj = 1;
                    AddtoFalseAlarm(pVtrack_item, 0);
                }
            }

            m_track_count = pVtrack_item->rw.pm[j].track_count;

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].track_error > 0) {
                nEraseReason = 1;
            }
            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].track_count > 100000) {
                nEraseReason = 2;
            }
            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ol < 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 3;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].orr > sw - 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 4;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ot < 5 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 5;
            }

            if (nEraseReason == 0 && pVtrack_item->rw.pm[j].ob > sh - 5 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 6;
            }

            if (nEraseReason == 0 && posx2 < 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 7;
            }
            if (nEraseReason == 0 && posx2 > sw - 2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 8;
                FsPrintf(2, ".nEraseReason:%d  or:%d.....\n", nEraseReason, pVtrack_item->rw.pm[j].orr);
                fflush(stdout);
            }

            //目标在y轴方向上的运动位移超过阈值
            if (nEraseReason == 0 && dy > pVtrack_item->ro.m_config_track.m_trackOverFlag2 && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 9;
                FsPrintf(2, ".nEraseReason:%d  dy:%d.....\n", nEraseReason, dy);
                fflush(stdout);
            }

            //检测方向为左，但目标向右运动超过阈值
            if (nEraseReason == 0 && pVtrack_item->rw.v_movedir == 1 && (dx > pVtrack_item->ro.m_config_track.m_trackOverFlag1) && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 10;
                FsPrintf(2, ".nEraseReason:%d  dx:%d.....\n", nEraseReason, dx);
                fflush(stdout);
            }
            //检测方向为右，但目标向左运动超过阈值
            if (nEraseReason == 0 && pVtrack_item->rw.v_movedir == 2 && (dx < -pVtrack_item->ro.m_config_track.m_trackOverFlag1) && pVtrack_item->rw.pm[j].track_count > 1) {
                nEraseReason = 11;
                FsPrintf(2, ".nEraseReason:%d  dx:%d.....\n", nEraseReason, dx);
                fflush(stdout);
            }

            //到达最长跟踪时限了..
            if (nEraseReason == 0 && (float) (fs_time_uptime_get() - pVtrack_item->rw.v_incStartTime) > pVtrack_item->ro.m_config_track.m_trackOverTime * 60) {
                nEraseReason = 12;
                FsPrintf(2, ".nEraseReason:%d  dx:%d  [%f  %f  %f].....\n", nEraseReason, dx, fs_time_uptime_get(), pVtrack_item->rw.v_incStartTime, pVtrack_item->ro.m_config_track.m_trackOverTime);
                fflush(stdout);
            }

            //检测船底部的长度(效果不好，暂不使用)
            int boatlen = 0;
            boatlen = TrackTheboatBottom(&pVtrack_item->rw.m_vsearch, frameindex, pVtrack_item->rw.pm[j].ol,
                    pVtrack_item->rw.pm[j].ot, pVtrack_item->rw.pm[j].orr, pVtrack_item->rw.pm[j].ob, &pVtrack_item->rw.v_reduce_rate);
            if (pVtrack_item->ro.m_config_devinfo.m_camera_brand != 1 && nEraseReason == 0 && boatlen == 0) {
                //nEraseReason = 13;
            }

            //检测在跟踪框内边缘像素点(大于110)的个数总和，如果总数小于初始个数总和的1/10
            //仅限海康球机场景使用
            int re = CheckTheEdgeCount(&pVtrack_item->rw.m_vsearch, pVtrack_item->rw.pm[j].track_count, frameindex,
                    pVtrack_item->rw.pm[j].ol, pVtrack_item->rw.pm[j].ot, pVtrack_item->rw.pm[j].orr, pVtrack_item->rw.pm[j].ob);
            if (pVtrack_item->ro.m_config_devinfo.m_camera_brand != 1 && nEraseReason == 0 && re < 0) {
                nEraseReason = 14;
            }

            //如果云台在设定时间内没有转动，结束跟踪
            if ((pVtrack_item->rw.v_holder_lastctrlDx == 0 && pVtrack_item->rw.pm[j].track_count > 25 * pVtrack_item->ro.m_config_track.m_trackOverFlag4) ||
                    ((pVtrack_item->rw.v_holder_lastctrlDx > 0) && (frameindex - pVtrack_item->rw.v_holder_lastctrlDx) > 25 * pVtrack_item->ro.m_config_track.m_trackOverFlag4)) {

                if (nEraseReason == 0)
                    nEraseReason = 15;
            }
        }
    }

    //跟踪结束
    if (nEraseReason > 0) {

        //海康球机，如果是和球机的通讯有问题，直接结束跟踪，并且不再操作球机
        if (nEraseReason == 101) {

            FsPrintf(1, "......track over，for the hiki connect is wrong, nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            //再次尝试发送停止指令
            linkage_ctrl_stopMoveLeftRight(pVtrack_item);
            pVtrack_item->rw.v_incEndTime = fs_time_uptime_get();

            FsPrintf(1, "1 before savetrackvideo(Line:%d ip:%s).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            SaveTrackVideo(pVtrack_item);

            //清零所有中间参数
            resetParam(pVtrack_item);

            return 1;

        }//真正结束...(跟踪次数到了，或者已经达到最长跟踪时间)
        else if (pVtrack_item->rw.v_trackobj_times <= 0 || nEraseReason == 12) {

            FsPrintf(1, "......track over nEraseReason:%d  (tracknum:%lld track_count:%d movedir:%d falsealarm:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackNum,
                    m_track_count, pVtrack_item->rw.v_movedir, pVtrack_item->rw.m_isfalsealarm);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track over nEraseReason:%d  v_trackobj_times:%d.....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);

            //先判定是否需要回到预置位，再断开相机控制端口连接
            if (pVtrack_item->ro.m_config_track.m_trackOverPTZ == 0) {

                int ret = linkage_ctrl_connect(pVtrack_item);
                if (ret < 0) {

                    FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                    fflush(stdout);
                }
                FsPrintf(1, "(%s:%d)-(%s)  stop the camera,and not to turn to ptz... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                fflush(stdout);

                linkage_ctrl_stopMoveLeftRight(pVtrack_item);

            } else {

                if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {
                    int ret = linkage_ctrl_connect(pVtrack_item);
                    if (ret < 0) {

                        FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                        fflush(stdout);
                    }
                }

                linkage_ctrl_goPTZ(pVtrack_item, 0);
            }

            //断开连接
            linkage_ctrl_disconnect(pVtrack_item);


            pVtrack_item->rw.v_incEndTime = fs_time_uptime_get();
            FsPrintf(1, "before savetrackvideo(Line:%d ip:%s).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            SaveTrackVideo(pVtrack_item);

            //清零所有中间参数
            resetParam(pVtrack_item);

            return 1;

        } else if (pVtrack_item->rw.v_trackobj_times > 0) { //跟踪次数没到，且没有到时间

            pVtrack_item->rw.v_trackobj_times--; //使用次数减一

            resetParam_1(pVtrack_item);

            FsPrintf(1, "......track again  (nEraseReason:%d  v_trackobj_times:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            fflush(stdout);

            char message[256];
            sprintf(message, "......track again  (nEraseReason:%d  v_trackobj_times:%d).....\n", nEraseReason, pVtrack_item->rw.v_trackobj_times);
            MakeLogRecord(message, pVtrack_item);
        }
    }

    return 0;
}




//改变初始位置

/************************************************
 * 1.调整相对位置： 
 * 调整轮船在图像中的相对位置。在跟踪过程中，就一直保持当前跟踪坐标与相对位置坐标一致
 *       我们将相对位置保存在pVtrack_item->rw.pm[j].il，pVtrack_item->rw.pm[j].ir，pVtrack_item->rw.pm[j].it，pVtrack_item->rw.pm[j].ib 中
 *   因此，我们需要在联动之前改变il,ir,it,ib这几个值       
 * 
 * 调整方法：  
 *      1.如果船体检测无效，则将（il,ir,it,ib）修改至图像中间位置
 *      2.如果船体检测有效，且船体在左侧，将（il,ir,it,ib）修改至图像的右侧
 *      3.如果船体检测有效，且船体在右侧，将（il,ir,it,ib）修改至图像的左侧  
 *
 ----------------------------------------------------
 2016.05.25: 去掉当判断出运动方向才调整相对位置的条件(Line：2464)
 ----------------------------------------------------
 **************************************************/
void changeLocation(Vtrack_item *pVtrack_item, int frameindex) {
    //如果已经改变了船的相对初始坐标，直接返回
    if (pVtrack_item->rw.v_changeBoatpos_flag == 1)
        return;

    int j;
    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

            //if (pVtrack_item->rw.v_movedir > 0) {
            if (1) {
                //检测船轮廓有效 ...
                if (pVtrack_item->rw.v_effective > 0) {

                    //船体在初始坐标左边时
                    if (pVtrack_item->rw.v_effective == 1) {
                        int temp = pVtrack_item->rw.pm[j].ir - pVtrack_item->rw.pm[j].il;
                        pVtrack_item->rw.pm[j].il = 360 - temp;
                        pVtrack_item->rw.pm[j].ir = 360;
                    }
                    //船体在初始坐标右边时
                    if (pVtrack_item->rw.v_effective == 2) {
                        int temp = pVtrack_item->rw.pm[j].ir - pVtrack_item->rw.pm[j].il;
                        pVtrack_item->rw.pm[j].il = 120;
                        pVtrack_item->rw.pm[j].ir = 120 + temp;
                    }

                } else { //检测船轮廓无效时..                    
                    int temp = pVtrack_item->rw.pm[j].ir - pVtrack_item->rw.pm[j].il;
                    pVtrack_item->rw.pm[j].il = 240 - temp / 2;
                    pVtrack_item->rw.pm[j].ir = 240 + temp / 2;
                }

                pVtrack_item->rw.v_changeBoatpos_flag = 1; //修改处置坐标标记

#ifdef PRINT_DEBUG
                FsPrintf(1, "(%s:%d) framedx:%d (v_movedir :%d ,v_effective:%d ) change the initpos (il:%d ir:%d it:%d ib:%d) ..\n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.v_movedir, pVtrack_item->rw.v_effective, pVtrack_item->rw.pm[j].il,
                        pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib);
                fflush(stdout);
                char message[256];
                sprintf(message, "(%s:%d) framedx:%d (v_movedir :%d ,v_effective:%d ) change the initpos (il:%d ir:%d it:%d ib:%d) ..\n", __FUNCTION__, __LINE__,
                        frameindex, pVtrack_item->rw.v_movedir, pVtrack_item->rw.v_effective, pVtrack_item->rw.pm[j].il,
                        pVtrack_item->rw.pm[j].ir, pVtrack_item->rw.pm[j].it, pVtrack_item->rw.pm[j].ib);
                MakeLogRecord(message, pVtrack_item);
#endif                

            }
        }
    }
}

//联动跟踪

/*************************************************
 *          联动跟踪
 * 
 * 
 * 
//已经判断出运动方向,在这里控制云台联动
// 
//   联动的原则是，保持跟踪目标在原图上的相对位置保持不变(相对位置一般是初始跟踪坐标)(比如，初始坐标为(a,b),那么我们需要转动云台使得每次跟踪的当前坐标保持不变)
//   1.但是，当相对位置坐标比较靠近图像两侧时，很有可能在之后移动云台的过程中，该坐标会跑出图像范围（云台某次转动太大）
//     因此，当初始跟踪坐标不理想时，我们需要先调整
//          我们将图的宽分为4等份，当初始坐标位于最左侧等份或最右等份中则需要调整(max_x < 120 || (max_x+trackwidth) > 360)
//  设计：
//      检测船轮廓无效时:
//         1.当初始坐标在左侧(max_x < 120 ),且船向右行驶： 保持云台不转动，当跟踪当前坐标大于120时，将其设置为相对位置坐标
//         2.当初始坐标在右侧（(max_x+trackwidth) > 360）,且船向左行驶，当跟踪当前坐标小于360时，将其设置为相对位置坐标
//         3.当初始坐标在左侧(max_x < 120 )，且船向左行驶，则正常转动云台，且不断的增大初始坐标的值，直到该值大于120
//         4.当初始坐标在右侧（(max_x+trackwidth) > 360）,且船向右行驶，则正常转动云台，且不断的减小初始坐标的值，直到该值小于360
//                          
//
//      检测船轮廓有效时:
//         1.当船向右行驶：
//                    船体在初始坐标左边时，如果初始坐标小于350时，保持云台不转动，直到当跟踪当前坐标大于350时，将其设置为相对位置坐标
//                                          如果初始坐标大于350时，则正常转动云台，且不断的减小初始坐标的值，初始值小于350时，将其设置为相对位置坐标
//
//                    船体在初始坐标右边时，如果初始坐标小于120时，保持云台不转动，直到当前跟踪坐标大于120时，将其设置为相对位置坐标
//                                          如果初始坐标大于120时，则正常转动云台，且不断的减小初始坐标值，初始坐标小于120时，将其设置为相对位置坐标

//       2.当船向左行驶：
//                    船体在初始坐标左边时，如果初始坐标小于350时，则正常转动云台，且不断的增大初始坐标的值，初始值大于350时，将其设置为相对位置坐标
//                                          如果初始坐标大于360时，保持云台不转动，直到当跟踪当前坐标小于350时，将其设置为相对位置坐标
//
//                    船体在初始坐标右边时，如果初始坐标小于120时，则正常转动云台，且不断的增大初始坐标值，初始坐标大于120时，将其设置为相对位置坐标
//                                          如果初始坐标大于120时，保持云台不转动，直到当跟踪当前坐标小于120时，将其设置为相对位置坐标                                                     
//
//

 **************************************************/
void linkageCamera(Vtrack_item *pVtrack_item, int frameindex, int sw, int sh) {

    int j;

    //当判断出运动方向之后，再联动云台
    if (pVtrack_item->rw.v_movedir > 0) {
        for (j = 0; j < MAX_OBJ; j++) {
            if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

                //当前坐标位置与初始位置之间的相对位置          
                int posy1 = (pVtrack_item->rw.pm[j].ot + pVtrack_item->rw.pm[j].ob) / 2;
                int posx1 = (pVtrack_item->rw.pm[j].il + pVtrack_item->rw.pm[j].ir) / 2; //初始位置
                int posx2 = (pVtrack_item->rw.pm[j].ol + pVtrack_item->rw.pm[j].orr) / 2; //当前位置
                int dx = posx2 - posx1;

                //左右判断,左右移动相机：
                pVtrack_item->rw.v_camctrl_dir = pVtrack_item->rw.v_movedir;

                //                printf("+++++v_movedir:%d dx:%d  m_cond_start:%d  (frameindex:%d lastctrlDx:%d forceStopX_1:%d )+++++++\n",pVtrack_item->rw.v_movedir,dx,
                //                        pVtrack_item->ro.m_config_holder.m_cond_start,frameindex ,pVtrack_item->rw.v_holder_lastctrlDx , pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1);

                //条件：向左行驶,偏移量大于阈值，离上次转动命令帧差大于阈值
                if (pVtrack_item->rw.v_movedir == 1 && dx < -pVtrack_item->ro.m_config_holder.m_cond_start) {
                    if (frameindex - pVtrack_item->rw.v_holder_lastctrlDx > pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1) {

                        linkage_ctrl_moveLeftRight(pVtrack_item);

                        pVtrack_item->rw.v_holder_lastmoveXDx = frameindex; //记录上一次云台向左转动的帧号(用来控制前后两次转动命令的间隔帧)
                        pVtrack_item->rw.v_holder_lastctrlDx = frameindex; //记录上一次云台转动的帧号(用来控制向云台发送心跳信号的时间)
                        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DEBUG
                        FsPrintf(1, "turn left(famedx:%d dx:%d ==movingX:%d) ..\n", frameindex, dx, pVtrack_item->rw.v_camctrl_movingX);
                        fflush(stdout);
                        char message[256];
                        sprintf(message, "(%s:%d)  turn left  framedx:%d. dx:%d movingX:%d \n", __FUNCTION__, __LINE__,
                                frameindex, dx, pVtrack_item->rw.v_camctrl_movingX);
                        MakeLogRecord(message, pVtrack_item);
#endif                        
                    }
                }

                //条件：向右行驶,偏移量大于阈值，离上次转动命令帧差大于阈值               
                if (pVtrack_item->rw.v_movedir == 2 && dx > pVtrack_item->ro.m_config_holder.m_cond_start) {
                    if (frameindex - pVtrack_item->rw.v_holder_lastctrlDx > pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1) {

                        linkage_ctrl_moveLeftRight(pVtrack_item);

                        pVtrack_item->rw.v_holder_lastmoveXDx = frameindex; //记录上一次云台向左转动的帧号(用来控制前后两次转动命令的间隔帧)
                        pVtrack_item->rw.v_holder_lastctrlDx = frameindex; //记录上一次云台转动的帧号(用来控制向云台发送心跳信号的时间)
                        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DEBUG                        
                        FsPrintf(1, "..turn right(famedx:%d dx:%d ==movingX:%d) ..\n", frameindex, dx, pVtrack_item->rw.v_camctrl_movingX);
                        fflush(stdout);
                        char message[256];
                        sprintf(message, "(%s:%d)  turn right  framedx:%d. dx:%d movingX:%d \n", __FUNCTION__, __LINE__,
                                frameindex, dx, pVtrack_item->rw.v_camctrl_movingX);
                        MakeLogRecord(message, pVtrack_item);
#endif                        
                    }
                }


                //强制停止左右转动：间隔帧时间就是球机转动的时间
                if (pVtrack_item->rw.v_camctrl_movingX != 0) {
                    if (frameindex - pVtrack_item->rw.v_holder_lastmoveXDx >= (pVtrack_item->ro.m_config_holder.m_holder_forceStopX/*+pVtrack_item->rw.v_holder_incmove*/)) {

                        linkage_ctrl_stopMoveLeftRight(pVtrack_item);
                        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DEBUG                        
                        FsPrintf(1, "..stop turn (famedx:%d dx:%d movingX:%d) ..\n", frameindex, dx, pVtrack_item->rw.v_camctrl_movingX);
                        fflush(stdout);
                        char message[256];
                        sprintf(message, "(%s:%d)  stop turn  framedx:%d. dx:%d movingX:%d \n", __FUNCTION__, __LINE__,
                                frameindex, dx, pVtrack_item->rw.v_camctrl_movingX);
                        MakeLogRecord(message, pVtrack_item);
#endif                        
                    }
                }
                //左右判断,左右移动相机
                ///////////////////////////////////////////////////////////////////////////////////////

                ///////////////////////////////////////////////////////////////////////////////////////
                //上下移动相机：在判断相机没有左右移动的情况下移动
                //向上移动
                if (posy1 < sh / 2 - 40) {

                    if (frameindex - pVtrack_item->rw.v_holder_lastctrlDx > pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1) {

                        pVtrack_item->rw.v_camctrl_dirY = 1;

                        linkage_ctrl_moveUpDown(pVtrack_item);

                        pVtrack_item->rw.v_camctrl_movingY = 1;
                        pVtrack_item->rw.v_holder_lastmoveYDx = frameindex;
                        pVtrack_item->rw.v_holder_lastctrlDx = frameindex;
                        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DEBUG                            
                        FsPrintf(1, "(%s:%d)  turn up (famedx:%d posy1:%d v_camctrl_dirY:%d movingY:%d) ..\n", __FUNCTION__, __LINE__,
                                frameindex, posy1, pVtrack_item->rw.v_camctrl_dirY, pVtrack_item->rw.v_camctrl_movingY);
                        fflush(stdout);
#endif                            
                    }
                }

                //向下移动
                if (posy1 > sh / 2 + 40) {

                    if (frameindex - pVtrack_item->rw.v_holder_lastctrlDx > pVtrack_item->ro.m_config_holder.m_holder_forceStopX_1) {
                        pVtrack_item->rw.v_camctrl_dirY = 2;

                        linkage_ctrl_moveUpDown(pVtrack_item);

                        pVtrack_item->rw.v_camctrl_movingY = 1;
                        pVtrack_item->rw.v_holder_lastmoveYDx = frameindex;
                        pVtrack_item->rw.v_holder_lastctrlDx = frameindex;
                        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

#ifdef PRINT_DEBUG
                        FsPrintf(1, "(%s:%d)  turn down (famedx:%d posy1:%d v_camctrl_dirY:%d movingY:%d) ..\n", __FUNCTION__, __LINE__,
                                frameindex, posy1, pVtrack_item->rw.v_camctrl_dirY, pVtrack_item->rw.v_camctrl_movingY);
                        fflush(stdout);
#endif                            
                    }
                }

                if (pVtrack_item->rw.v_camctrl_movingY != 0) {
                    if (frameindex - pVtrack_item->rw.v_holder_lastmoveYDx >= pVtrack_item->ro.m_config_holder.m_holder_forceStopY) {

                        linkage_ctrl_stopMoveUpDown(pVtrack_item);

                        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;
                        pVtrack_item->rw.v_camctrl_movingY = 0;
#ifdef PRINT_DEBUG                            
                        FsPrintf(1, "(%s:%d)  stop updown (famedx:%d dx:%d movingY:%d) ..\n", __FUNCTION__, __LINE__,
                                frameindex, posy1, pVtrack_item->rw.v_camctrl_movingY);
                        fflush(stdout);
#endif                            
                    }
                }

                //上下移动相机：在判断相机没有左右移动的情况下移动
                ///////////////////////////////////////////////////////////////////////////////////////              
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //强制性暂停一次：为了不失去连接（光电仪才需要）
    if ((pVtrack_item->rw.v_movedir > 0) && (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) &&
            ((frameindex - pVtrack_item->rw.v_holder_lastConnectdx) > pVtrack_item->ro.m_config_holder.m_holder_keeptouchFrames)) {

#ifdef PRINT_DEBUG               
        FsPrintf(1, "..Send a heart...:(%d %d %d  m_nUserID:%d) ..\n", frameindex,
                pVtrack_item->rw.v_holder_lastctrlDx, pVtrack_item->ro.m_config_holder.m_holder_keeptouchFrames,
                pVtrack_item->rw.m_hikiCamera.m_nUserID);
        fflush(stdout);
#endif                    

        pVtrack_item->rw.v_holder_lastConnectdx = frameindex;

        linkage_ctrl_stopMoveLeftRight(pVtrack_item);
    }
    //强制性暂停一次：为了不失去连接
    ///////////////////////////////////////////////////////////////////////////////////////    
}

void drawTrackLoop_1008(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame) {
    int j;
    int sw = pFrame->width[2];
    int sh = pFrame->height[2];

    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

            //绘制轮船跟踪框...
            int l = 0;
            int r = 0;
            int t = 0;
            int b = 0;
            l = pVtrack_item->rw.pm[j].ol * 4;
            r = pVtrack_item->rw.pm[j].orr * 4;
            t = (pVtrack_item->rw.pm[j].ot)*4;
            b = (pVtrack_item->rw.pm[j].ob)*4;

            if (l < 0) l = 0;
            if (r > (int) pFrame->width[0]) r = pFrame->width[0] - 1;
            if (t < 0) t = 0;
            if (b > (int) pFrame->height[0]) b = pFrame->height[0] - 1;

            //DrawTrackRect((char *) pFrame->data.raw[0]->data, pFrame->width[0], pFrame->height[0], pVtrack_item, l, r, t, b);       
        }
    }
}

void drawTrackLoop_auto(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame) {
    int j;

    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

            int l = 0;
            int r = 0;
            int t = 0;
            int b = 0;

            if (pVtrack_item->ro.zoomTimes == 16) {
                l = pVtrack_item->rw.m_boatinfo.l * 4;
                r = pVtrack_item->rw.m_boatinfo.r * 4;
                t = pVtrack_item->rw.m_boatinfo.t * 4;
                b = pVtrack_item->rw.m_boatinfo.b * 4;
            } else if (pVtrack_item->ro.zoomTimes == 4) {
                l = pVtrack_item->rw.m_boatinfo.l * 2;
                r = pVtrack_item->rw.m_boatinfo.r * 2;
                t = pVtrack_item->rw.m_boatinfo.t * 2;
                b = pVtrack_item->rw.m_boatinfo.b * 2;
            } else {
                l = pVtrack_item->rw.m_boatinfo.l * 4;
                r = pVtrack_item->rw.m_boatinfo.r * 4;
                t = pVtrack_item->rw.m_boatinfo.t * 4;
                b = pVtrack_item->rw.m_boatinfo.b * 4;
            }

            DrawTrackRect((char *) pFrame->data.raw[0]->data, pFrame->width[0], pFrame->height[0], pVtrack_item, l, r, t, b);
        }
    }
}

//绘制跟踪框

/*************************************************
绘制检测框（尽量把整个船都框住）:
          
1.当检测船轮廓无效时：
          就将跟踪框扩一点

2.当检测船轮廓有效时：
      把轮廓作为 检测框
      当轮船全部在图像内时：       直接把轮廓作为 检测框    
      当轮船只有一部分在图像内时： 还需要继续检测轮廓，直到轮船全部进入图像内 
 **************************************************/
void drawTrackLoop(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame) {
    int j;

    for (j = 0; j < MAX_OBJ; j++) {
        if (pVtrack_item->rw.pm[j].bUsed == 1 && pVtrack_item->rw.pm[j].track_count > 2) {

            //光电仪
            if (1)/* (pVtrack_item->rw.v_movedir > 0)*/ {

                //绘制轮船跟踪框...
                int l = 0;
                int r = 0;
                int t = 0;
                int b = 0;
                //船轮廓检测无效,且为光电仪的场景
                /*if (pVtrack_item->rw.v_effective == 0 && pVtrack_item->ro.m_config_holder.m_holder_type == 1) */
                if (pVtrack_item->rw.v_effective == 0 && 0) {

                    if (pVtrack_item->ro.m_config_devinfo.m_camera_model == 0)//白光
                    {
                        l = (pVtrack_item->rw.pm[j].ol - pVtrack_item->ro.m_config_track.m_trackWidth / 4)*4;
                        r = (pVtrack_item->rw.pm[j].orr + pVtrack_item->ro.m_config_track.m_trackWidth / 4)*4;
                        t = (pVtrack_item->rw.pm[j].ot)*4;
                        b = (pVtrack_item->rw.pm[j].ob)*4;
                    } else { //红光
                        l = pVtrack_item->rw.pm[j].ol - pVtrack_item->ro.m_config_track.m_trackWidth / 2;
                        r = pVtrack_item->rw.pm[j].orr + pVtrack_item->ro.m_config_track.m_trackWidth / 2;
                        t = pVtrack_item->rw.pm[j].ot;
                        b = pVtrack_item->rw.pm[j].ob;
                    }
                }

                /*if (pVtrack_item->rw.v_effective >= 0 && pVtrack_item->ro.m_config_holder.m_holder_type != 1) */
                if (pVtrack_item->rw.v_effective >= 0) {

                    if (pVtrack_item->ro.zoomTimes == 16)//白光
                    {
                        l = (pVtrack_item->rw.pm[j].ol)*4;
                        r = (pVtrack_item->rw.pm[j].orr)*4;
                        t = (pVtrack_item->rw.pm[j].ot)*4;
                        b = (pVtrack_item->rw.pm[j].ob)*4;
                    } else if (pVtrack_item->ro.zoomTimes == 4) { //红光
                        l = pVtrack_item->rw.pm[j].ol * 2;
                        r = pVtrack_item->rw.pm[j].orr * 2;
                        t = pVtrack_item->rw.pm[j].ot * 2;
                        b = pVtrack_item->rw.pm[j].ob * 2;
                    } else {
                        l = pVtrack_item->rw.pm[j].ol;
                        r = pVtrack_item->rw.pm[j].orr;
                        t = pVtrack_item->rw.pm[j].ot;
                        b = pVtrack_item->rw.pm[j].ob;
                    }
                    //向左，r保持不变
                    if (pVtrack_item->rw.v_reduce_rate > 0.0 && 0) {

                        if (pVtrack_item->rw.v_movedir == 1) {
                            int len = r - l;
                            int hight = b - t;
                            l = l + len * pVtrack_item->rw.v_reduce_rate;
                            t = t + hight * pVtrack_item->rw.v_reduce_rate / 2;
                            b = b - hight * pVtrack_item->rw.v_reduce_rate / 2;
                        }

                        if (pVtrack_item->rw.v_movedir == 2) {
                            int len = r - l;
                            int hight = b - t;
                            r = r - len * pVtrack_item->rw.v_reduce_rate;
                            t = t + hight * pVtrack_item->rw.v_reduce_rate / 2;
                            b = b - hight * pVtrack_item->rw.v_reduce_rate / 2;
                        }
                    }
                }

                DrawTrackRect((char *) pFrame->data.raw[0]->data, pFrame->width[0], pFrame->height[0], pVtrack_item, l, r, t, b);
            }
        }
    }
}

void load_maskImage_from_file(Vtrack_item *pVtrack_item) {
    //不需要视频检测
    if (pVtrack_item->ro.m_config_videoDect.m_valid == 0) {
        return;
    }

    int maskindex = 0;
    for (maskindex = 0; maskindex < MASKCOUNT; maskindex++) {
        char samplepath[516];

        if (maskindex == 0) {
            sprintf(samplepath, "/fs/project/data/config/sample_%s_%d.bmp", pVtrack_item->ro._uuid, maskindex + 1);
            if (!access(samplepath, F_OK)) {
                printf("%s %d : cannot find the file(%s) \n", __FUNCTION__, __LINE__, samplepath);
            } else {
                sprintf(samplepath, "/fs/project/data/config/sample_%s.bmp", pVtrack_item->ro._uuid);
            }
        } else {
            sprintf(samplepath, "/fs/project/data/config/sample_%s_%d.bmp", pVtrack_item->ro._uuid, maskindex + 1);
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

            memset(pVtrack_item->rw.m_vibe_inparam.maskImage[maskindex], 0, w * h);

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

                    //绿色区域
                    if (0 && g > r && g > b) {
                        pVtrack_item->rw.m_vibe_inparam.maskImage[maskindex][j * w + i] = 255;
                    }

                    //红色区域
                    if (r > b && r > g) {
                        pVtrack_item->rw.m_vibe_inparam.maskImage[maskindex][j * w + i] = 255;
                    }

                }

                int asd = (w * 3) % 4;
                asd = (asd == 0) ? 0 : (4 - asd);
                k += asd;
            }
            image_dib_delete__OI(&pdib);

            ImageDib* pdib1 = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
            memcpy(pdib1->data, pVtrack_item->rw.m_vibe_inparam.maskImage[maskindex], w * h);
            sprintf(samplepath, "/fs/project/data/config/result_%s_%d.bmp", pVtrack_item->ro._uuid, maskindex + 1);
            image_dib_svae_to_file(pdib1, samplepath);
            image_dib_delete__OI(&pdib1);

            printf("%s:%d  read  the  sample file suc(%s) \n", __FUNCTION__, __LINE__, samplepath);
        } else {
            printf("%s:%d  cannot find the file(%s) \n", __FUNCTION__, __LINE__, samplepath);
        }
    }
}

void load_maskImage_from_file_bak(Vtrack_item *pVtrack_item) {
    //不需要视频检测
    if (pVtrack_item->ro.m_config_videoDect.m_valid == 0) {
        return;
    }

    char samplepath[516];
    sprintf(samplepath, "/fs/project/data/config/sample_%s.bmp", pVtrack_item->ro._uuid);
    ImageDib* pdib = image_dib_new_from_file__IO(samplepath, ImageColorType_Unknown, 0);

    //没有模板图片
    if (pdib == NULL) {
        printf("%s:%d  cannot find the file \n", __FUNCTION__, __LINE__);
        return;
    }

    int w, h;
    w = pdib->width;
    h = pdib->height;

    if (w > DIMAGEW || h > DIMAGEH) {
        printf("%s:%d  the file is error \n", __FUNCTION__, __LINE__);
        return;
    }

    memset(pVtrack_item->rw.m_vibe_inparam.maskImage[0], 0, w * h);
    memset(pVtrack_item->rw.m_vibe_inparam.maskImage[1], 0, w * h);
    memset(pVtrack_item->rw.m_vibe_inparam.maskImage[2], 0, w * h);

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

            //绿色区域
            if (0 && g > r && g > b) {
                pVtrack_item->rw.m_vibe_inparam.maskImage[0][j * w + i] = 255;
            }

            //红色区域
            if (r > b && r > g) {
                pVtrack_item->rw.m_vibe_inparam.maskImage[0][j * w + i] = 255;
            }

        }

        int asd = (w * 3) % 4;
        asd = (asd == 0) ? 0 : (4 - asd);
        k += asd;
    }
    image_dib_delete__OI(&pdib);

    ImageDib* pdib1 = image_dib_new__IO(w, h, ImageColorType_Gray, 0);
    memcpy(pdib1->data, pVtrack_item->rw.m_vibe_inparam.maskImage, w * h);
    sprintf(samplepath, "/fs/project/data/config/result_%s.bmp", pVtrack_item->ro._uuid);
    image_dib_svae_to_file(pdib1, samplepath);
    image_dib_delete__OI(&pdib1);

    printf("%s:%d  read  the  sample file suc \n", __FUNCTION__, __LINE__);
}

/* 检查数据是否可发送的函数指针,成功返回1,出错返回-1,0表示还需要等待 */
static signed char track_over_status_condition(void *p, /* 发送的数据 */const FsObjectBase * const sendData) {

    Vtrack_item *pVtrack_item = (Vtrack_item *) p;

    if (pVtrack_item->rw.v_findObj == 1) {
        return 0;
    } else if (pVtrack_item->rw.v_findObj == 0) {

        FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) send the track over info... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_trackNum);
        fflush(stdout);
        //FsPrintf(1, ">>>>>>>>>>>>>>>.track_over_status_condition<<<<<<<<<<<<<<<<\n");
        //fflush(stdout);
        return 1;
    }
}

/* 检查数据是否可发送的函数指针,成功返回1,出错返回-1,0表示还需要等待 */
static signed char track_lock_status_condition(void *p, /* 发送的数据 */const FsObjectBase * const sendData) {
    Vtrack_item *pVtrack_item = (Vtrack_item *) p;

    //锁定标记为1，说明成功锁定目标，返回1，发送信息
    if (pVtrack_item->rw.v_locktrackObj == 1) {

        char temp = sendData->data[sendData->len];
        sendData->data[sendData->len] = 0;
        FsPrintf(1, "(%s:%d)-(%s tracknum:%lld) send the lock info: %s... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip,
                pVtrack_item->rw.v_trackNum, sendData->data + 8);
        fflush(stdout);
        sendData->data[sendData->len] = temp;

        //置为初始值
        pVtrack_item->rw.v_locktrackObj = -1;

        return 1;
    }

    //如果没有发现目标，就直接返回-1，这里避免，前一次锁定失败还未超时返回，又来第二次跟踪请求。
    if (pVtrack_item->rw.v_findObj == 0) {
        return -1;
    }

    return 0;
}

/*
 *  在这里，接收雷达指令：（哪一路相机探测到目标）
 * 
 */

static int snap_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {

    struct Vtrack_item* pTrack_item = (struct Vtrack_item *) p;

    if (0) {
        fs_Ebml_analyzeAll(pEbml, (struct FsEbml_node*) pEbml);
        FsJson *pJson_1 = fs_Ebml_convert_to_Json__IO(pEbml);

        FsString* info = fs_Json_to_String__IO(pJson_1, 0, 0, FsStringLenData("\r\n"), 0, NULL, 4);

        char log[256];
        sprintf(log, "snap_private_cmd_cb: %s\n", info->buffer);
        MakeLogRecord(log, pTrack_item);

        printf("\n%s\n", info->buffer);
        fs_Json_delete__OI(pJson_1, pShareBuffer);
        fs_String_delete__OI(info);
    }

    struct FsEbml_node *cmd = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_type");
    if (strcmp("snap", cmd->data.buffer) == 0) {

        cmd = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");
        if (strcmp("0700H", cmd->data.buffer) == 0) {

            printf("%s:%d *****snap a picture**** \n", __FUNCTION__, __LINE__);

            pthread_mutex_lock(&pTrack_item->ro.__framelistIn->mutex);
            FsObjectImageFrame *pframe = NULL;
            if (pTrack_item->ro.__framelistIn->nodeCount > 0) {
                pframe = (FsObjectImageFrame *) pTrack_item->ro.__framelistIn->pNode[pTrack_item->ro.__framelistIn->startIndex];
                fs_Object_addRefer_pthreadSafety(pframe, __LINE__);
            }
            pthread_mutex_unlock(&pTrack_item->ro.__framelistIn->mutex);

            if (pframe != NULL) {
                //处理
                FsObjectImageRGB * const pRGB = (FsObjectImageRGB *) image_frame_get_pthreadSafety__IO(pframe, ImageFrame_RGB_0);

                unsigned char* pdstbuffer = (unsigned char*) fsMalloc(1024 * 1024 * 1);

                if (pTrack_item->ro.m_compressRate < 30) pTrack_item->ro.m_compressRate = 30;
                if (pTrack_item->ro.m_compressRate > 95) pTrack_item->ro.m_compressRate = 95;

                int jpglen = image_jpeg_compress(pdstbuffer, 1024 * 1024 * 1, pRGB->data, pframe->width[0], pframe->height[0], pframe->width[0]*3, pTrack_item->ro.m_compressRate, 1, 1);
                fs_Object_delete_pthreadSafety__OI(pRGB);
                fs_Object_delete_pthreadSafety__OI(pframe);

                printf("%s:%d ****prapare the image (len:%d compressRate:%d w:%d  h:%d)**** \n", __FUNCTION__, __LINE__, jpglen, pTrack_item->ro.m_compressRate, pframe->width[0], pframe->height[0]);
                if (0) {
                    FILE* f = fopen("/fs/project/data/hello.jpg", "wb");
                    if (f != NULL) {
                        fwrite(pdstbuffer, jpglen, 1, f);
                        fclose(f);
                    }
                }


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
                    //sprintf(temp, "%f", 12.0);
                    fs_Ebml_node_data_set_string(pEbml_node, "12.0");
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "T", FsEbmlNodeType_String);
                    //sprintf(temp, "%f", 12.0);
                    fs_Ebml_node_data_set_string(pEbml_node, "12.0");
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "Z", FsEbmlNodeType_String);
                    //sprintf(temp, "%f", 12.0);
                    fs_Ebml_node_data_set_string(pEbml_node, "12.0");
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
                    sprintf(temp, "%s", pTrack_item->ro._uuid);
                    fs_Ebml_node_data_set_string(pEbml_node, temp);
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
                    struct FsEbml_node *m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");
                    sprintf(temp, "%s", m_node->data.buffer);
                    fs_Ebml_node_data_set_string(pEbml_node, temp);

                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "image_byte", FsEbmlNodeType_Binary);
                    fs_Ebml_node_data_set__OI_3(pEbml_node, jpglen, (char *) pdstbuffer);
                    FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
                    fs_Ebml_delete__OI(rst, pShareBuffer);

                    FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
                    fs_Json_delete__OI(pJson, pShareBuffer);
                    pObjectBase->data -= 8;
                    pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
                    ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
                    memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);
                    configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);

                    printf("%s:%d ****snap the picture success !**** \n", __FUNCTION__, __LINE__);

                }
            } else {

                //回执信息，默认都是成功
                if (1) {

                    char temp[54];
                    FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                    struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "code", FsEbmlNodeType_String);
                    fs_Ebml_node_data_set_string(pEbml_node, "1001");
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "msg", FsEbmlNodeType_String);
                    //sprintf(temp, "%s", "");
                    //fs_Ebml_node_data_set_string(pEbml_node, temp);

                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "P", FsEbmlNodeType_String);
                    //sprintf(temp, "%f", 12);
                    fs_Ebml_node_data_set_string(pEbml_node, "12.0");
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "T", FsEbmlNodeType_String);
                    //sprintf(temp, "%f", 12);
                    fs_Ebml_node_data_set_string(pEbml_node, "12.0");
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "Z", FsEbmlNodeType_String);
                    //sprintf(temp, "%f", 12);
                    fs_Ebml_node_data_set_string(pEbml_node, "12.0");

                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
                    sprintf(temp, "%s", pTrack_item->ro._uuid);
                    fs_Ebml_node_data_set_string(pEbml_node, temp);
                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
                    struct FsEbml_node *m_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");
                    sprintf(temp, "%s", m_node->data.buffer);
                    fs_Ebml_node_data_set_string(pEbml_node, temp);

                    pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "image_byte", FsEbmlNodeType_String);
                    //sprintf(temp, "%s", "");
                    //fs_Ebml_node_data_set_string(pEbml_node, temp);              

                    FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
                    fs_Ebml_delete__OI(rst, pShareBuffer);

                    FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
                    fs_Json_delete__OI(pJson, pShareBuffer);
                    pObjectBase->data -= 8;
                    pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
                    ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
                    memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);
                    configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);

                    printf("%s:%d ****snap the picture fails !**** \n", __FUNCTION__, __LINE__);
                }
            }
        }
    }

    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1 */
static int track_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {

    struct Vtrack_item* pTrack_item = (struct Vtrack_item *) p;

    if (0) {
        fs_Ebml_analyzeAll(pEbml, (struct FsEbml_node*) pEbml);
        FsJson *pJson_1 = fs_Ebml_convert_to_Json__IO(pEbml);

        FsString* info = fs_Json_to_String__IO(pJson_1, 0, 0, FsStringLenData("\r\n"), 0, NULL, 4);

        char log[256];
        sprintf(log, "track_cmd_cb: %s\n", info->buffer);
        MakeLogRecord(log, pTrack_item);

        printf("\n%s\n", info->buffer);
        fs_Json_delete__OI(pJson_1, pShareBuffer);
        fs_String_delete__OI(info);
    }

    struct FsEbml_node *cmd = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "command_type");
    if (strcmp("track", cmd->data.buffer) == 0) {
        struct FsEbml_node *action_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "action_type");

        //初始化连接，用来保存客户端的连接信息(ip，端口等)
        if (strcmp("init_status", action_type->data.buffer) == 0) {

            char path[256];
            sprintf(path, "touch /fs/project/data/temp/init_status_%s.dat", pTrack_item->ro._uuid);
            system(path);



            m_camcrl_head = head;
            memcpy(m_camcrl_requesID_3, requestID_3, 3 * sizeof (int));
            m_camcrl_requesID_data[0] = requestID_3[0];
            m_camcrl_requesID_data[1] = requestID_3[1];
            m_camcrl_requesID_data[2] = requestID_3[2];
            m_camcrl_requesID_data[3] = head;
            m_camcrl_requesID_data[4] = requestDataType;



            pTrack_item->ro.m_camcrl_head = head;
            memcpy(pTrack_item->ro.m_camcrl_requesID_3, requestID_3, 3 * sizeof (int));
            pTrack_item->ro.m_camcrl_requesID_data[0] = requestID_3[0];
            pTrack_item->ro.m_camcrl_requesID_data[1] = requestID_3[1];
            pTrack_item->ro.m_camcrl_requesID_data[2] = requestID_3[2];
            pTrack_item->ro.m_camcrl_requesID_data[3] = head;
            pTrack_item->ro.m_camcrl_requesID_data[4] = requestDataType;
            pTrack_item->ro.m_camctrl_flag = 1;

            //立即返回
            {
                //返回相机状态：
                char temp[54];
                FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
                fs_Ebml_node_data_set_string(pEbml_node, "track");
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
                sprintf(temp, "%s", pTrack_item->ro._uuid);
                fs_Ebml_node_data_set_string(pEbml_node, temp);
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
                fs_Ebml_node_data_set_string(pEbml_node, "init_success");
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
                sprintf(temp, "%d", pTrack_item->rw.v_findObj);
                fs_Ebml_node_data_set_string(pEbml_node, temp);

                FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
                fs_Ebml_delete__OI(rst, pShareBuffer);

                //FsXml *pXml = fs_Ebml_convert_to_Xml__IO(rst);
                FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
                fs_Json_delete__OI(pJson, pShareBuffer);
                pObjectBase->data -= 8;
                pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
                ((unsigned int *) pObjectBase->data)[0] = (pTrack_item->ro.m_camcrl_head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
                memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), pTrack_item->ro.m_camcrl_requesID_3, sizeof (unsigned int)*3);
                configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);

            }

            printf("%s %d: >>>>init status<<<< \n", __FUNCTION__, __LINE__);
            return -128;
        }

        if (strcmp("get_status", action_type->data.buffer) == 0) {

            //返回相机状态：
            char temp[54];
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
            fs_Ebml_node_data_set_string(pEbml_node, "track");
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
            sprintf(temp, "%s", pTrack_item->ro._uuid);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
            fs_Ebml_node_data_set_string(pEbml_node, "get_status");
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
            sprintf(temp, "%d", pTrack_item->rw.v_findObj);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
            sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
            fs_Ebml_node_data_set_string(pEbml_node, temp);

            FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
            fs_Ebml_delete__OI(rst, pShareBuffer);

            //FsXml *pXml = fs_Ebml_convert_to_Xml__IO(rst);
            FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
            fs_Json_delete__OI(pJson, pShareBuffer);
            pObjectBase->data -= 8;
            pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
            ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
            memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);
            configManager_send_pthreadSafety__OI_2(pTrack_item->ro._pVtrack->ro._pConfigManager, pObjectBase);

        }
        if (strcmp("start_track", action_type->data.buffer) == 0) {

            struct FsEbml_node *ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "obj_num");
            pTrack_item->rw.v_trackNum = atoll(ObjNumber->data.buffer);
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "P");
            pTrack_item->rw.v_obj_p = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "T");
            pTrack_item->rw.v_obj_t = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "Z");
            pTrack_item->rw.v_obj_z = (float) (atof(ObjNumber->data.buffer));

            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "lat");
            pTrack_item->rw.v_obj_lat = atof(ObjNumber->data.buffer); //雷达目标第一次经纬度
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "lon");
            pTrack_item->rw.v_obj_lon = atof(ObjNumber->data.buffer);
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "alarmtype");
            pTrack_item->rw.v_obj_alarmsite = atoi(ObjNumber->data.buffer); //目标告警地点类型
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "trackmodel");
            pTrack_item->rw.v_trackmodel = atoi(ObjNumber->data.buffer); //跟踪模式

            pTrack_item->rw.v_findObj = 1;
            pTrack_item->rw.v_gotoptz = 1; //转向ptz      
            pTrack_item->rw.v_incStartTime = fs_time_uptime_get(); //开始跟踪时间

            if (1) {
                FsPrintf(1, "set the camera track status (camera_ip:%s  findobj:%d) v_trackNum:%lld  p:%f t:%f z:%f v_incStartTime:%f lat:%f lon:%f alarmsite:%d trackmodel:%d \n",
                        pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_findObj, pTrack_item->rw.v_trackNum, pTrack_item->rw.v_obj_p,
                        pTrack_item->rw.v_obj_t, pTrack_item->rw.v_obj_z, pTrack_item->rw.v_incStartTime, pTrack_item->rw.v_obj_lat,
                        pTrack_item->rw.v_obj_lon, pTrack_item->rw.v_obj_alarmsite, pTrack_item->rw.v_trackmodel);
                fflush(stdout);
                char log[256];
                sprintf(log, "set the camera track status (camera_ip:%s  findobj:%d) v_trackNum:%lld  p:%d t:%d z:%d  v_incStartTime:%f \n",
                        pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_findObj, pTrack_item->rw.v_trackNum, (int) pTrack_item->rw.v_obj_p,
                        (int) pTrack_item->rw.v_obj_t, (int) pTrack_item->rw.v_obj_z, pTrack_item->rw.v_incStartTime);
                MakeLogRecord(log, pTrack_item);
            }

            if (1) {
                //向控制器发送跟踪结束指令
                char temp[512];
                FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
                fs_Ebml_node_data_set_string(pEbml_node, "track");
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
                sprintf(temp, "%s", pTrack_item->ro._uuid);
                fs_Ebml_node_data_set_string(pEbml_node, temp);
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
                fs_Ebml_node_data_set_string(pEbml_node, "stop_track");
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
                fs_Ebml_node_data_set_string(pEbml_node, "1");
                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
                sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
                fs_Ebml_node_data_set_string(pEbml_node, temp);


                if (pTrack_item->rw.v_videoPath[0] == 0 || pTrack_item->rw.v_videoPath_1[0] == 0) {
                    //返回跟踪录像路径名
                    time_t ts = (time_t) fs_time_GMT_get();
                    struct tm ti_s;
                    localtime_r(&ts, &ti_s);
                    sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                            pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                            1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

                    sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                            pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                            1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
                }

                pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
                sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
                fs_Ebml_node_data_set_string(pEbml_node, temp);


                FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
                fs_Ebml_delete__OI(rst, pShareBuffer);

                FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
                fs_Json_delete__OI(pJson, pShareBuffer);
                pObjectBase->data -= 8;
                pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
                ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
                memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);

                configManager_send_pthreadSafety__OI_3(pTrack_item->ro._pVtrack->ro._pConfigManager, fs_time_uptime_get()+(pTrack_item->ro.m_config_track.m_trackOverTime + 1.0)*60,
                        pObjectBase, track_over_status_condition, pTrack_item, NULL);
            }
        }

        if (strcmp("lock_track", action_type->data.buffer) == 0) {

            struct FsEbml_node *ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "obj_num");
            pTrack_item->rw.v_trackNum = atoll(ObjNumber->data.buffer);

            //向控制器发送跟踪结束指令
            char temp[512];
            FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
            struct FsEbml_node* pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_type", FsEbmlNodeType_String);
            fs_Ebml_node_data_set_string(pEbml_node, "track");
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "command_for_uuid", FsEbmlNodeType_String);
            sprintf(temp, "%s", pTrack_item->ro._uuid);
            fs_Ebml_node_data_set_string(pEbml_node, temp);
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "action_type", FsEbmlNodeType_String);
            fs_Ebml_node_data_set_string(pEbml_node, "lock_track");
            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "camera_status", FsEbmlNodeType_String);
            //sprintf(temp, "%d", pTrack_item->rw.v_locktrackObj);
            //fs_Ebml_node_data_set_string(pEbml_node, temp);
            fs_Ebml_node_data_set_string(pEbml_node, "1");

            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "obj_num", FsEbmlNodeType_String);
            sprintf(temp, "%lld", pTrack_item->rw.v_trackNum);
            fs_Ebml_node_data_set_string(pEbml_node, temp);

            if (pTrack_item->rw.v_videoPath[0] == 0 || pTrack_item->rw.v_videoPath_1[0] == 0) {
                //返回跟踪录像路径名
                time_t ts = (time_t) fs_time_GMT_get();
                struct tm ti_s;
                localtime_r(&ts, &ti_s);
                sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                        pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                        1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

                sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                        pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                        1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);
            }

            pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
            sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
            fs_Ebml_node_data_set_string(pEbml_node, temp);

            FsJson *pJson = fs_Ebml_convert_to_Json__IO(rst);
            fs_Ebml_delete__OI(rst, pShareBuffer);

            FsObjectBase * const pObjectBase = fs_Json_to_ObjectBase__IO(pJson, sizeof (FsObjectBase) + 8, Memery_Alignment(sizeof (unsigned int)*3), FsStringLenData("\r\n"), 0, NULL, 4);
            fs_Json_delete__OI(pJson, pShareBuffer);
            pObjectBase->data -= 8;
            pObjectBase->len = pObjectBase->len - Memery_Alignment(sizeof (unsigned int)*3) + 8;
            pObjectBase->data[pObjectBase->len] = 0;
            FsPrintf(1, "get lock (camera_ip:%s v_trackNum:%lld)   info:  %s \n", pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_trackNum, pObjectBase->data + 8);
            fflush(stdout);
            ((unsigned int *) pObjectBase->data)[0] = (head & 0xFFFFFFF0U) | 0xAU, ((unsigned int *) pObjectBase->data)[1] = pObjectBase->len - 8;
            memcpy(pObjectBase->data + Memery_Alignment(pObjectBase->len), requestID_3, sizeof (unsigned int)*3);

            configManager_send_pthreadSafety__OI_3(pTrack_item->ro._pVtrack->ro._pConfigManager, fs_time_uptime_get()+(pTrack_item->ro.m_config_track.m_trackOverTime + 1.0)*60,
                    pObjectBase, track_lock_status_condition, pTrack_item, NULL);

        }

        //手动停止跟踪：停止相机跟踪，停止相机转动
        if (strcmp("stop_track", action_type->data.buffer) == 0) {

            pTrack_item->rw.v_handleToStop = 1;

            FsPrintf(1, "(%s)Hand to stop the tracking... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            char log[256];
            sprintf(log, "(%s)Hand to stop the tracking... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip);
            MakeLogRecord(log, pTrack_item);
        }
        //手动停止跟踪1：停止相机跟踪，转动相机到ptz预置位
        if (strcmp("stop_track_toptz", action_type->data.buffer) == 0) {

            pTrack_item->rw.v_handleToStop = 2;

            FsPrintf(1, "(%s)Hand to stop the tracking,and go to preset ptz... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            char log[256];
            sprintf(log, "(%s)Hand to stop the tracking,and go to preset ptz... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip);
            MakeLogRecord(log, pTrack_item);
        }

        //雷达导引相机，转动相机到指定ptz位置
        if (strcmp("radar_guide", action_type->data.buffer) == 0) {

            pTrack_item->rw.v_handleToStop = 3;

            //雷达导引跟踪...
            struct FsEbml_node *ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "obj_num");
            int num = atoll(ObjNumber->data.buffer);
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "P");
            pTrack_item->rw.v_obj_p = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "T");
            pTrack_item->rw.v_obj_t = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "Z");
            pTrack_item->rw.v_obj_z = (float) (atof(ObjNumber->data.buffer));

            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "lat");
            pTrack_item->rw.v_obj_lat = atof(ObjNumber->data.buffer); //雷达目标第一次经纬度
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "lon");
            pTrack_item->rw.v_obj_lon = atof(ObjNumber->data.buffer);
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "alarmtype");
            pTrack_item->rw.v_obj_alarmsite = atoi(ObjNumber->data.buffer); //目标告警地点类型
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "trackmodel");
            pTrack_item->rw.v_trackmodel = atoi(ObjNumber->data.buffer); //跟踪模式

            //pTrack_item->rw.v_findObj = 1;
            //pTrack_item->rw.v_gotoptz = 1; //转向ptz      

            if (pTrack_item->rw.v_incStartTime == 0.0)
                pTrack_item->rw.v_incStartTime = fs_time_uptime_get(); //开始跟踪时间

            FsPrintf(1, "(%s) Test === radar  guide the camera to(p:%f t:%f z:%f) StartTime:%f  %f ... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_obj_p,
                    pTrack_item->rw.v_obj_t, pTrack_item->rw.v_obj_z, pTrack_item->rw.v_incStartTime, fs_time_uptime_get());
            fflush(stdout);

        }
        //手动导引相机：停止相机跟踪，转动相机到指定ptz位置
        if (strcmp("hand_guide", action_type->data.buffer) == 0 || strcmp("guide", action_type->data.buffer) == 0) {

            pTrack_item->rw.v_handleToStop = 4;

            struct FsEbml_node *ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "obj_num");
            int num = atoll(ObjNumber->data.buffer);

            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "P");
            pTrack_item->rw.v_obj_p = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "T");
            pTrack_item->rw.v_obj_t = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "Z");
            pTrack_item->rw.v_obj_z = (float) (atof(ObjNumber->data.buffer));

            FsPrintf(1, "(%s)Hand to stop the tracking,and guide the camera to(p:%f t:%f z:%f)... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_obj_p,
                    pTrack_item->rw.v_obj_t, pTrack_item->rw.v_obj_z);
            fflush(stdout);
            char log[256];
            sprintf(log, "(%s)Hand to stop the tracking,and guide the camera to(p:%d t:%d z:%d)... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip, (int) pTrack_item->rw.v_obj_p,
                    (int) pTrack_item->rw.v_obj_t, (int) pTrack_item->rw.v_obj_z);
            MakeLogRecord(log, pTrack_item);

        }

        //停止雷达导引跟踪
        if (strcmp("stop_radarguide_track", action_type->data.buffer) == 0) {

            struct FsEbml_node *ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "obj_num");
            pTrack_item->rw.v_trackNum = atoll(ObjNumber->data.buffer);
            //ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "trackmodel");
            //pTrack_item->rw.v_trackmodel = atoi(ObjNumber->data.buffer); //跟踪模式
            //ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "alarmtype");
            //pTrack_item->rw.v_obj_alarmsite = atoi(ObjNumber->data.buffer); //目标告警地点类型

            //保存录像
            pTrack_item->rw.v_incEndTime = fs_time_uptime_get(); //结束跟踪时间
            SaveTrackVideo(pTrack_item);

            //回执信息
            sendStop2Camcontrol_1(pTrack_item);

            //重置数据结构体
            resetParam(pTrack_item);
            pTrack_item->rw.v_incStartTime = 0.0;
            pTrack_item->rw.v_incEndTime = 0.0;

            FsPrintf(1, "(%s) stop_radarguide_track.. \n", pTrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            char log[256];
            sprintf(log, "(%s)  stop_radarguide_track... \n", pTrack_item->ro.m_config_devinfo.m_camera_ip);
            MakeLogRecord(log, pTrack_item);
        }

        if (strcmp("test_track", action_type->data.buffer) == 0) {

            struct FsEbml_node *ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "obj_num");
            pTrack_item->rw.v_trackNum = atoll(ObjNumber->data.buffer);
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "P");
            pTrack_item->rw.v_obj_p = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "T");
            pTrack_item->rw.v_obj_t = (float) (atof(ObjNumber->data.buffer));
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "Z");
            pTrack_item->rw.v_obj_z = (float) (atof(ObjNumber->data.buffer));

            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "lat");
            pTrack_item->rw.v_obj_lat = atof(ObjNumber->data.buffer); //雷达目标第一次经纬度
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "lon");
            pTrack_item->rw.v_obj_lon = atof(ObjNumber->data.buffer);
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "alarmtype");
            pTrack_item->rw.v_obj_alarmsite = atoi(ObjNumber->data.buffer); //目标告警地点类型
            ObjNumber = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "trackmodel");
            pTrack_item->rw.v_trackmodel = atoi(ObjNumber->data.buffer); //跟踪模式

            pTrack_item->rw.v_findObj = 1;
            pTrack_item->rw.v_gotoptz = 100; //转向ptz      
            pTrack_item->rw.v_incStartTime = fs_time_uptime_get(); //开始跟踪时间

            //返回跟踪录像路径名
            time_t ts = (time_t) fs_time_GMT_get();
            struct tm ti_s;
            localtime_r(&ts, &ti_s);
            sprintf(pTrack_item->rw.v_videoPath_1, "/vsys/%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv",
                    pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

            sprintf(pTrack_item->rw.v_videoPath, "%s%s/incident/%04d%02d%02d/%02d/inc%04d%02d%02d%02d%02d%02d.mkv", pTrack_item->ro._pVtrack->ro._dir,
                    pTrack_item->ro._uuid, 1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour,
                    1900 + ti_s.tm_year, ti_s.tm_mon + 1, ti_s.tm_mday, ti_s.tm_hour, ti_s.tm_min, ti_s.tm_sec);

            FsPrintf(1, "====test track v_videoPath:%s  \n", pTrack_item->rw.v_videoPath);
            fflush(stdout);
            //pEbml_node = fs_Ebml_node_addChild(rst, (struct FsEbml_node*) rst, "video_path", FsEbmlNodeType_String);
            //sprintf(temp, "%s", pTrack_item->rw.v_videoPath_1);
            //fs_Ebml_node_data_set_string(pEbml_node, temp);

            if (1) {
                FsPrintf(1, "set the camera track status (camera_ip:%s  findobj:%d) v_trackNum:%lld  p:%f t:%f z:%f v_incStartTime:%f lat:%f lon:%f alarmsite:%d trackmodel:%d \n",
                        pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_findObj, pTrack_item->rw.v_trackNum, pTrack_item->rw.v_obj_p,
                        pTrack_item->rw.v_obj_t, pTrack_item->rw.v_obj_z, pTrack_item->rw.v_incStartTime, pTrack_item->rw.v_obj_lat,
                        pTrack_item->rw.v_obj_lon, pTrack_item->rw.v_obj_alarmsite, pTrack_item->rw.v_trackmodel);
                fflush(stdout);
                char log[256];
                sprintf(log, "set the camera track status (camera_ip:%s  findobj:%d) v_trackNum:%lld  p:%d t:%d z:%d  v_incStartTime:%f \n",
                        pTrack_item->ro.m_config_devinfo.m_camera_ip, pTrack_item->rw.v_findObj, pTrack_item->rw.v_trackNum, (int) pTrack_item->rw.v_obj_p,
                        (int) pTrack_item->rw.v_obj_t, (int) pTrack_item->rw.v_obj_z, pTrack_item->rw.v_incStartTime);
                MakeLogRecord(log, pTrack_item);
            }
        }
    }
    return 1;
}

/* 在期望连接的命令字和uuid改变时调用 */
static void track_private_cmd_connect_cb(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
        /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p) {
    struct Vtrack_item* pTrack_item = (struct Vtrack_item *) p;
    if (strcmp("video_part", cmd) == 0 && strcmp(uuid, pTrack_item->ro._uuid) == 0) {
        pthread_mutex_lock(&pTrack_item->ro.__mutexCmdConnect);
        pTrack_item->p.record_item_add_partVideo = (Record_item_add_partVideo_pthreadSafety) do_local;
        pTrack_item->p.pRecord_item = (struct Record_item*) do_local_p;
        pthread_mutex_unlock(&pTrack_item->ro.__mutexCmdConnect);
    } else {
        FsPrintf(1, "Invalid data,cmd:\"%s\",uuid:\"%s/%s\"\n", cmd, uuid, pTrack_item->ro._uuid);
        fflush(stdout);
    }
}

void videodect_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel);
void track_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel);
void holder_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel);
void overly_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel);
void search_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel);
void debug_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannelt);

static void track_private_item_new(struct Vtrack *pVtrack, /* 通道号,从1开始,为0用于集群 */const unsigned int channel, /* 本地ip地址 */const FsArray * const ipList
        , /* 原来的采集项 */struct Vtrack_item **ppVtrack_item, FsGroupSqrt * const pGroupSqrt
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //FsPrintf(1, "channel=%u\n", channel);
    FsConfig * const pConfig = ((ConfigManager*) pVtrack->ro._pConfigManager)->ro.__pConfig;
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
                    //                    uuid = fs_Config_node_string_get_first(pConfig, parent, "hostname");
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
                        if (*ppVtrack_item != NULL && (*ppVtrack_item)->ro._sum == fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) sumNode))break;
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (0 == channel) {
                        // if (ppNodeRecord_item[index])record_P_item_delete__OI(ppNodeRecord_item[index]);
                    } else if (*ppVtrack_item) {
                        vtrack_private_item_delete__OI(*ppVtrack_item, pShareBuffer);
                    }
                    ////////////////////////////////////////////////////////////
                    struct Vtrack_item * rst;
                    unsigned long moduleMask;
                    if (Vtrack_Mask != 0 && (Vtrack_Mask & (moduleMask = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "moduleMask", NULL))) == 0) rst = NULL;
                    else {
                        char *pd;
                        const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                        unsigned int len = uuid->lenth;
                        if (channel > 0) {
                            rst = (struct Vtrack_item*) fsMalloc(Memery_Alignment(sizeof (struct Vtrack_item)) + len);
                            //结构体清零...
                            memset(rst, 0, sizeof (struct Vtrack_item));
                            /* 掩码 */
                            rst->ro.moduleMask = moduleMask;
                            FsPrintf(1, "track_private_item_new....malloc the  Vtrack_item: %ld \n", sizeof (struct Vtrack_item) +len);
                            fflush(stdout);

                            pd = (char*) (rst + 1);
                            /* 所属主机的ip地址,用于集群,本机为0 */
                            rst->ro._ipv4 = ipv4;
                            /* 本对象的uuid值,一般是从配置中读入的 */
                            rst->ro._uuid = pd, len = uuid->lenth, memcpy(pd, uuid->buffer, len), pd += len;
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
                            Fs_GroupSqrtOut_set_pthreadSafety(rst->rw._pGroupSqrtOut, pVtrack->ro._pGroupSqrtOut->groupSqrt_member, pVtrack->ro._pGroupSqrtOut->groupSqrt_group
                                    , pVtrack->ro._pGroupSqrtOut->groupValue, channel - 1, &pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                        }
                        ////////////////////////////////////////////////////////
                        if (channel > 0) {
                            {
                                const char * const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig ipv4", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ip, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_camera_ip, "192.168.8.252");
                            }
                            rst->ro.m_config_devinfo.m_camera_port = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rtspPort", 0, NULL);

                            {
                                const char * const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rstpUsername", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraUid, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraUid, "admin");
                            }

                            {
                                const char * const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig rstpPasswd", NULL);
                                if (pstr)
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraPwd, "%s", pstr);
                                else
                                    sprintf(rst->ro.m_config_devinfo.m_szCameraPwd, "12345");
                            }
                            //相机云台控制IP
                            {
                                const char * const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "cameraConfig holderipv4", NULL);
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

                            FsPrintf2(1, "**************************Track CameraInfo*********************************\n");
                            FsPrintf2(1, "cameraip:%s camera_port:%d camera_user:%s camera_pwd:%s camera_model:%d camera_brand:%d  m_camera_ctrl_ip:%s m_camera_ctrl_port:%d \n", rst->ro.m_config_devinfo.m_camera_ip, rst->ro.m_config_devinfo.m_camera_port,
                                    rst->ro.m_config_devinfo.m_szCameraUid, rst->ro.m_config_devinfo.m_szCameraPwd, rst->ro.m_config_devinfo.m_camera_model, rst->ro.m_config_devinfo.m_camera_brand,
                                    rst->ro.m_config_devinfo.m_camera_ctrl_ip, rst->ro.m_config_devinfo.m_camera_ctrl_port);
                            fflush(stdout);
                            FsPrintf2(1, "**************************Track CameraInfo*********************************\n");

                            //获取配置参数
                            debug_getConfig(rst, pConfig, vsysChannel0, vsysChannel);
                            search_getConfig(rst, pConfig, vsysChannel0, vsysChannel);
                            track_getConfig(rst, pConfig, vsysChannel0, vsysChannel);
                            overly_getConfig(rst, pConfig, vsysChannel0, vsysChannel);
                            holder_getConfig(rst, pConfig, vsysChannel0, vsysChannel);
                            videodect_getConfig(rst, pConfig, vsysChannel0, vsysChannel);

                            //读取模板图片
                            load_maskImage_from_file(rst);

                            //初始化相关参数
                            rst->rw.m_hikiCamera.m_nUserID = -1;
                            rst->rw.m_hikiCamera.m_nControlChannel = -1;
                            rst->rw.max_pfs = 5.0;
                            rst->rw.v_trackobj_times = rst->ro.m_config_track.m_trackAgainTimes;
                            rst->rw.v_adjustpos = -1;
                            rst->rw.v_waitforcamera = rst->ro.m_config_track.m_trackdelay;
                            rst->rw.v_locktrackObj = -1;
                            //rst->rw.m_incashore.m_ashore_delay = -1;

                            pthread_mutex_init(&rst->rw.m_log_mutex, NULL);

                            //初始化跟踪算法
                            vEpl_create(&rst->rw.g_vproc);

                            //创建相机控制对象
                            rst->rw.pCamera_ctrl_item = camera_ctrl_tcp_item_new__IO(rst->ro.m_config_devinfo.m_camera_brand, rst->ro.m_config_devinfo.m_camera_model,
                                    rst->ro.m_config_devinfo.m_camera_ctrl_ip, rst->ro.m_config_devinfo.m_camera_ctrl_port, rst->ro.m_config_devinfo.m_szCameraUid,
                                    rst->ro.m_config_devinfo.m_szCameraPwd, 0);

                            /* 所属的记录对象 */
                            rst->ro._pVtrack = pVtrack;
                            /* 传入的数据帧链表 */
                            rst->ro.__framelistIn = fs_ObjectList_new__IO(6);
                            /* 传出的数据帧链表 */
                            rst->ro.__framelistOut = fs_ObjectList_new__IO(6);
                            /* 连接和调用外部命令所需要的同步锁 */
                            pthread_mutex_init(&rst->ro.__mutexCmdConnect, NULL);
                            ////////////////////////////////////////////////////////////////////////////

                            /* 注册命令字 */
                            configManager_cmd_register(pVtrack->ro._pConfigManager, "track", rst->ro._uuid, 0, rst, 0, track_private_cmd_cb, NULL, rst, pShareBuffer);

                            /* 注册命令字 */
                            configManager_cmd_register(pVtrack->ro._pConfigManager, "snap", rst->ro._uuid, 0, rst, 0, snap_private_cmd_cb, NULL, rst, pShareBuffer);
                            ////////////////////////////////////////////////////////////////////////////
                            /* 绑定命令字 */
                            configManager_cmd_connect(pVtrack->ro._pConfigManager, "video_part", rst->ro._uuid, rst, track_private_cmd_connect_cb, rst);
                            ////////////////////////////////////////////////////////////////////////////
                        }
                        /* 注册命令字 */
                        ////////////////////////////////////////////////////////////////////////////
                        /* 绑定命令字 */
                        //if (channel > 0) configManager_cmd_connect(pRecord->ro._pConfigManager, "cameractrl", rst->ro._uuid, rst, (void (*)(const char*, const char*, void*, void*, void*))record_private_cmd_connect_cb, rst);
                        ////////////////////////////////////////////////////////////////////////////
                        /* 释放内存空间 */
                    }
                    ////////////////////////////////////////////////////////////////////////////
                    if (channel > 0) *ppVtrack_item = rst;
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

//半秒获取一次：

static int CheckPos(float set_p, float set_t, float set_z, float curr_p, float curr_t, float curr_z) {
    int isready = 0;

    //保存上一次获取到的ptz值
    static float last_p = 0.0;
    static float last_t = 0.0;
    static float last_z = 0.0;

    //如果当前的ptz与预置位的ptz全部相等
    if (set_p == curr_p && set_t == curr_t && set_z == curr_z) {
        last_p = 0.0;
        last_t = 0.0;
        last_z = 0.0;
        isready = 1;
    }

    //如果当前的z 与 预置位的 z相等，且pt与预置位的pt相差都小于等于1
    if (curr_z == set_z) {
        if (abs(curr_p - set_p) <= 1 && abs(curr_t - set_t) <= 1) {
            last_p = 0.0;
            last_t = 0.0;
            last_z = 0.0;
            isready = 1;
        }
    }

    //如果两次获取的ptz都相等,且获取的ptz三个量与预置位的ptz三个量相差均在一定范围内
    //    if(curr_p == last_p && curr_t == last_t && curr_z == last_z)
    //    {
    //        if(abs(curr_p - set_p) <= 2 && abs(curr_t - set_t) <= 2 && abs(curr_z - set_z) <= 2 )
    //        {
    //            last_p = 0.0;
    //            last_t = 0.0;
    //            last_z = 0.0;            
    //            return  1;
    //        }
    //     }



    last_p = curr_p;
    last_t = curr_t;
    last_z = curr_z;


    return isready;

}


int checkCamPTZ(Vtrack_item *pVtrack_item);

/*
 主动向控制器发送指令
 * tpye = 1：发送目标锁定指令 
 *        2：发送目标跟踪结束指令,并且通知大数据保存此告警记录
 *        3: 发送目标跟踪结束指令,并且通知大数据不保存此告警记录(此目标在锁定之后，又被判定为虚警)
 */
void sendMsg2Camctl(Vtrack_item *pVtrack_item, int tpye) {
    //如果禁止上传或者开启的是调试模式，都直接返回
    if (pVtrack_item->ro.m_config_videoDect.m_upload == 0)// || pVtrack_item->ro.m_config_debug.m_opendebug == 2) 
    {
        printf("%s:%d the data is forbided upload...\n", __FUNCTION__, __LINE__);
        return;
    }

    int re = checkCamPTZ(pVtrack_item);
    if (re > 0) {
        printf("%s:%d camera is moved,give up the result...\n", __FUNCTION__, __LINE__);
        return;
    }

    //发送锁定指令
    if (tpye == 1) {
        pVtrack_item->rw.v_locktrackObj = -1;
        sendlock2Camcontrol(pVtrack_item);
    }

    //发送结束指令
    if (tpye == 2 || tpye == 3) {
        sendStop2Camcontrol(pVtrack_item, tpye);
    }
}

void debugRecord(Vtrack_item *pVtrack_item) {
    //记录轨迹
    if (0) {
        int cnt = pVtrack_item->rw.m_vibeparam.m_boatHeadCount;
        int index = 0;
        for (index = 0; index < cnt; index++) {
            if (pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].chosen == 1) {
                break;
            }
        }

        printf(">>>>>boatHeadCount:%d index:%d<<<<\n", cnt, index);

        char path[56];
        sprintf(path, "/fs/project/data/temp/route_%lld.txt", pVtrack_item->rw.v_trackNum);
        FILE* f = fopen(path, "wb");
        if (index < cnt) {
            if (f) {
                int j;
                for (j = 0; j < pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].count; j++) {
                    char temp[54];
                    int size = (pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].r[j] - pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].l[j])*(pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].b[j] - pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].t[j]);
                    sprintf(temp, "%d,%d,%d,%d,  %d\r\n", pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].l[j], pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].r[j],
                            pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].t[j], pVtrack_item->rw.m_vibeparam.m_boatheadPos[index].b[j], size);
                    fwrite(temp, strlen(temp), 1, f);
                }
            }
        }

        int j;
        for (j = 0; j < MAX_OBJ; j++) {
            if (pVtrack_item->rw.pm[j].bUsed == 1) {
                int cnt = pVtrack_item->rw.g_vproc.m_track[j].track_count;
                int i;
                for (i = 0; i < cnt; i++) {
                    char temp[54];
                    sprintf(temp, "%d,%d,%d,%d\r\n", pVtrack_item->rw.g_vproc.m_track[j].area_rect[i].l, pVtrack_item->rw.g_vproc.m_track[j].area_rect[i].r,
                            pVtrack_item->rw.g_vproc.m_track[j].area_rect[i].t, pVtrack_item->rw.g_vproc.m_track[j].area_rect[i].b);
                    fwrite(temp, strlen(temp), 1, f);
                }
            }
        }

        if (f)
            fclose(f);
    }
}

/*
 * 清除视频检测相关中间变量(跟踪结束时，或者出现其他错误时)
 */
void reSetVideoDectParam(Vtrack_item *pVtrack_item) {
    int index = 0;
    for (index = 0; index < pVtrack_item->ro.m_config_videoDect.m_ptz_count; index++) {
        //如果相机处于使用状态，
        if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused == 1) {
            pVtrack_item->ro.m_config_videoDect.m_pos[index].isready = 0;
        }
        //跟踪结束后，回到预置位，且该预置位由锁定状态改为使用状态
        if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused == 2) {
            pVtrack_item->ro.m_config_videoDect.m_pos[index].isused = 3;
        }
    }

    vibe_Del(&pVtrack_item->rw.m_vibeparam);
    printf("(%s:%d) reset param(ip:%s)... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
}

/*
 * 当在某个预置位处，开始检测到目标，且开始跟踪后： 锁定该预置位，不进行轮循
 */
void setVideoDecParam(Vtrack_item *pVtrack_item) {

    int i;
    for (i = 0; i < pVtrack_item->ro.m_config_videoDect.m_ptz_count; i++) {
        //如果在使用，将该预置位锁定
        if (pVtrack_item->ro.m_config_videoDect.m_pos[i].isused == 1) {
            pVtrack_item->ro.m_config_videoDect.m_pos[i].isused = 2;
        }
    }
}

/*****************************************
相机轮循：查看相机预置位是否需要轮循切换 

 返回值： 0：无需切换  1:需要切换预置位
 ********************************************/
int cameraPolling(Vtrack_item *pVtrack_item) {
    //如果不需要视频检测，返回
    if (pVtrack_item->ro.m_config_videoDect.m_valid == 0)
        return 0;

    //如果是枪机，返回
    if (pVtrack_item->ro.m_config_videoDect.m_camtpye == 0)
        return 0;

    //如果不需要轮循，且为球机，则默认第一个预置位有效。
    if (pVtrack_item->ro.m_config_videoDect.m_polltype == 0 && pVtrack_item->ro.m_config_videoDect.m_pos[0].isused == 0) {
        printf("(%s:%d) first use the cam, need change the camera prepos \n", __FUNCTION__, __LINE__);
        pVtrack_item->ro.m_config_videoDect.m_pos[0].isused = 1;
        pVtrack_item->ro.m_config_videoDect.m_pos[0].startTime = fs_time_uptime_get();
        pVtrack_item->ro.m_config_videoDect.m_pos[0].isready = 0;
        return 0;
    } else if (pVtrack_item->ro.m_config_videoDect.m_polltype == 0 && pVtrack_item->ro.m_config_videoDect.m_pos[0].isused == 1) {
        return 0;
    }



    //如果有雷达目标
    if (pVtrack_item->rw.v_findObj == 1) {
        int i;
        for (i = 0; i < pVtrack_item->ro.m_config_videoDect.m_ptz_count; i++) {
            if (pVtrack_item->ro.m_config_videoDect.m_pos[i].isused != 0) {
                pVtrack_item->ro.m_config_videoDect.m_pos[i].isused = 1;
                pVtrack_item->ro.m_config_videoDect.m_pos[i].isready = 0;
                break;
            }
        }
        return 0;
    }

    int i;
    int used = 0;
    int changed = 0;
    for (i = 0; i < pVtrack_item->ro.m_config_videoDect.m_ptz_count; i++) {

        //如果某个预置位点正在使用，且使用时间超过轮循时间，则让相机转到下一个预置位点进行检测
        if (pVtrack_item->ro.m_config_videoDect.m_pos[i].isused == 1) {

            int nnn = 1;
            //如果是球机，但不需要轮循
            if (pVtrack_item->ro.m_config_videoDect.m_camtpye != 0 && pVtrack_item->ro.m_config_videoDect.m_polltype == 0) {
                nnn = 0;
            }

            //如果是球机，但是检测点个数小于2，不需要轮循
            if (pVtrack_item->ro.m_config_videoDect.m_camtpye != 0 && pVtrack_item->ro.m_config_videoDect.m_ptz_count == 1) {
                nnn = 0;
            }

            //            printf("(%s:%d) m_camtpye:%d m_polltype:%d m_ptz_count:%d nn :%d  \n",__FUNCTION__,__LINE__,pVtrack_item->ro.m_config_videoDect.m_camtpye,
            //                    pVtrack_item->ro.m_config_videoDect.m_polltype,pVtrack_item->ro.m_config_videoDect.m_ptz_count,nnn);
            //            
            if ((nnn == 1) && (abs(fs_time_uptime_get() - pVtrack_item->ro.m_config_videoDect.m_pos[i].startTime) > pVtrack_item->ro.m_config_videoDect.m_polltime)) {

                pVtrack_item->ro.m_config_videoDect.m_pos[i].isused = 0;
                pVtrack_item->ro.m_config_videoDect.m_pos[i].isready = 0;

                if (i == pVtrack_item->ro.m_config_videoDect.m_ptz_count - 1) {
                    pVtrack_item->ro.m_config_videoDect.m_pos[0].isready = 0;
                    pVtrack_item->ro.m_config_videoDect.m_pos[0].isused = 1;
                    pVtrack_item->ro.m_config_videoDect.m_pos[0].startTime = fs_time_uptime_get();
                } else {
                    pVtrack_item->ro.m_config_videoDect.m_pos[i + 1].isready = 0;
                    pVtrack_item->ro.m_config_videoDect.m_pos[i + 1].isused = 1;
                    pVtrack_item->ro.m_config_videoDect.m_pos[i + 1].startTime = fs_time_uptime_get();
                }

                changed = 1;
                printf("(%s:%d) the time is over, need change the camera prepos(startTime:%f m_polltime:%d) \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_videoDect.m_pos[i].startTime,
                        pVtrack_item->ro.m_config_videoDect.m_polltime);

            }

            used = 1;
            break;
        }

        //如果是锁定状态，则更新其开始时间
        if (pVtrack_item->ro.m_config_videoDect.m_pos[i].isused == 2) {
            pVtrack_item->ro.m_config_videoDect.m_pos[i].startTime = fs_time_uptime_get();
            used = 1;
            break;
        }

        //如果是重置状态，则需要返回到本预置位，且将状态改为在使用(一般来说，相机在某个预置位处视频跟踪结束后，会触发这里)
        if (pVtrack_item->ro.m_config_videoDect.m_pos[i].isused == 3) {

            printf("(%s:%d) the camera is need to goback the prepos \n", __FUNCTION__, __LINE__);

            pVtrack_item->ro.m_config_videoDect.m_pos[i].startTime = fs_time_uptime_get();
            pVtrack_item->ro.m_config_videoDect.m_pos[i].isused = 1;
            pVtrack_item->ro.m_config_videoDect.m_pos[i].isready = 0;

            changed = 1;
            used = 1;
            break;
        }
    }

    //如果刚开始，一个都未使用
    if (used == 0) {

        printf("(%s:%d) first use the cam, need change the camera prepos \n", __FUNCTION__, __LINE__);
        changed = 1;
        pVtrack_item->ro.m_config_videoDect.m_pos[0].isused = 1;
        pVtrack_item->ro.m_config_videoDect.m_pos[0].startTime = fs_time_uptime_get();
        pVtrack_item->ro.m_config_videoDect.m_pos[0].isready = 0;
    }

    return changed;
}

/*****************************************
 相机转向指定的预置位

 int needed： 是否需要转向
  
 返回值： 0：已经到位，可以执行视频检测； 1：相机还在转动,或者不需要转动，需要等待
 ********************************************/
int cameraGoPrePostion(Vtrack_item *pVtrack_item, int needed) {
    //如果有雷达目标,返回1
    if (pVtrack_item->rw.v_findObj == 1)
        return 1;

    //如果不需要视频检测，返回1
    if (pVtrack_item->ro.m_config_videoDect.m_valid == 0)
        return 1;

    //如果是枪机，返回0
    if (pVtrack_item->ro.m_config_videoDect.m_camtpye == 0)
        return 0;

    //如果不支持预置位检测，返回0
    if (pVtrack_item->ro.m_config_videoDect.m_camera_movevalid == 0)
        return 0;


    int index = 0;
    for (index = 0; index < pVtrack_item->ro.m_config_videoDect.m_ptz_count; index++) {

        if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused == 1) {
            break;
        }

        //锁定状态,直接返回0
        if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused == 2) {
            return 0;
        }
    }

    if (index > pVtrack_item->ro.m_config_videoDect.m_ptz_count - 1) {
        printf("%s %d: cannot find one ptz is usefull (index:%d m_ptz_count:%d)...\n", __FUNCTION__, __LINE__, index, pVtrack_item->ro.m_config_videoDect.m_ptz_count);
        return 1;
    }


    //如果isready = 0； 发送转向指令； isready大于0小于50；等待延迟； isready大于50;准备到位，可以开始检测
    if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isready > 15 * 15) {
        return 0;
    } else if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isready == 0) {

        pVtrack_item->ro.m_config_videoDect.m_pos[index].isready++;

        pVtrack_item->rw.v_obj_p = (float) (pVtrack_item->ro.m_config_videoDect.m_pos[index].search_p);
        pVtrack_item->rw.v_obj_t = (float) (pVtrack_item->ro.m_config_videoDect.m_pos[index].search_t);
        pVtrack_item->rw.v_obj_z = (float) (pVtrack_item->ro.m_config_videoDect.m_pos[index].search_z);


        //如果连接不上相机，清除中间变量
        int ret = linkage_ctrl_connect(pVtrack_item);
        if (ret < 0) {

            reSetVideoDectParam(pVtrack_item);
            FsPrintf(1, "(%s:%d) - (%s) cannot connect the holder,reset params,and stop the track... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);

        } else {

            ret = linkage_ctrl_goPTZ(pVtrack_item, 1);
            FsPrintf(1, "(%s:%d) -(%s) set the holder to go to ptz(ret:%d) v_waitforcamera:%d (pFramedx:%d).. \n", __FUNCTION__, __LINE__,
                    pVtrack_item->ro.m_config_devinfo.m_camera_ip, ret, pVtrack_item->rw.v_waitforcamera, index);
            fflush(stdout);

            //重新初始化vibe
            vibe_Del(&pVtrack_item->rw.m_vibeparam);
            pVtrack_item->rw.m_vibeparam.m_bInitOK = 0;
        }

        return 1;


    } else {

        pVtrack_item->ro.m_config_videoDect.m_pos[index].isready++;
        return 1;
    }
}

/***************************************
跟踪前的预处理：

 一. 如果跟踪标记为0,但靠岸检测标记为1
        需要视频自动检测(这里做一个延迟，延迟时间为距离上一次转向预置位的时间超过50秒)

 二.  如果跟踪标记为1,
     1.连接云台，转动球机到目标ptz位置
     2.跟踪前延迟帧数倒计时

 * 返回值： 0：外部往后依次执行算法处理;  1：外部调用continue，跳过之后的算法处理
 ****************************************/
int Preparetrack(Vtrack_item *pVtrack_item, int frameindex) {
    //相机轮循检测
    int nnn = cameraPolling(pVtrack_item);

    //相机是否转动到位，0：表示相机转动到位了，且可以检测 1:表示没有到位或者没有启动视频检测
    int ret = cameraGoPrePostion(pVtrack_item, nnn);


    //如果没有收到雷达目标，且视频检测已经准备到位
    if (pVtrack_item->rw.v_findObj == 0 && ret == 0) {
        return 0;
    }

    //如果没有发现目标
    if (pVtrack_item->rw.v_findObj == 0) {
        return 1;
    }

    //如果发现目标，v_gotoptz == 100 表示不转向ptz，直接在当前位置寻找运动目标
    if (pVtrack_item->rw.v_findObj == 1 && pVtrack_item->rw.v_gotoptz == 100) {

        pVtrack_item->rw.v_gotoptz = 0;
        int ret = linkage_ctrl_connect(pVtrack_item);
        //如果连接不上相机，直接结束跟踪...
        if (ret < 0) {
            //将所有缓存数据清零
            pVtrack_item->rw.v_findObj = 0;
            resetParam(pVtrack_item);

            FsPrintf(1, "(%s:%d) - (%s) cannot connect the holder,reset params,and stop the track... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);

            return 1;

        }

        pVtrack_item->rw.v_waitforcamera = -1; //这种情况，不延迟
        FsPrintf(1, "(%s:%d) - (%s) test for track........ \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
        fflush(stdout);

        return 0;
    }

    //如果发现目标，如果需要转向目标，先发送转向ptz指令，只发送一次
    if (pVtrack_item->rw.v_findObj == 1 && pVtrack_item->rw.v_gotoptz == 1) {

        pVtrack_item->rw.v_gotoptz = 0;
        int ret = linkage_ctrl_connect(pVtrack_item);
        //如果连接不上相机，直接结束跟踪...
        if (ret < 0) {
            //将所有缓存数据清零
            pVtrack_item->rw.v_findObj = 0;
            resetParam(pVtrack_item);

            FsPrintf(1, "(%s:%d) - (%s) cannot connect the holder,reset params,and stop the track... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);

            return 1;

        } else {

            ret = linkage_ctrl_goPTZ(pVtrack_item, 1);

        }
    }


    //--------------------------------------------------------------------------
    //以下的步骤是做一个延迟，当相机转向ptz后，需要延迟几秒钟的时间，相机才能转到位
    //延迟的时间通过v_waitforcamera这个数值递减来完成(自减一次代表延迟一帧)，当v_waitforcamera递减到小于0时，结束延迟
    //这里，海康球机可以通过比较相机的实时ptz值和设置的ptz值是否相等，如果相等，则表示已经转到到位，这时可以提前结束延迟
    //--------------------------------------------------------------------------


    //如果跟踪延迟帧数 倒数到0，就直接返回0
    if (pVtrack_item->rw.v_waitforcamera < 0) {

        pVtrack_item->rw.v_autofocus = 0;
        return 0;
    }

    //返回标记，0：表示延迟结束，可以开始后续处理，1：表示需要继续延迟
    int m_wait = 0;

    //当目标标记为1时，还需要延迟几秒(等待相机转动到位,控制器不再延迟)
    if (pVtrack_item->rw.v_findObj == 1 && pVtrack_item->rw.v_waitforcamera > 0) {

        //每个半秒获取一次相机ptz，如果连续几次ptz保持不变，则表明球机已经转动到位！
        if (pVtrack_item->rw.v_waitcount == 0) {
            int ret = linkage_ctrl_connect(pVtrack_item);
            if (ret < 0) {

                FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                fflush(stdout);
            }
        }

        //海康球机：
        if (pVtrack_item->rw.v_waitcount % 6 == 0 && (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 0)) {
            int re = linkage_ctrl_getPTZ(pVtrack_item);

            //            FsPrintf(1, "(%s) start to strack(re:%d) (p:%f t:%f z:%f)- (p:%f t:%f z:%f) (pFramedx:%d) (v_waitforcamera:%d). \n",pVtrack_item->rw.m_camera_ip,re,pVtrack_item->rw.v_obj_p,pVtrack_item->rw.v_obj_t,
            //                pVtrack_item->rw.v_obj_z,pVtrack_item->rw.v_current_p,pVtrack_item->rw.v_current_t,pVtrack_item->rw.v_current_z,
            //                index,pVtrack_item->rw.v_waitforcamera);
            //            fflush(stdout);  

            int isready = CheckPos(pVtrack_item->rw.v_obj_p, pVtrack_item->rw.v_obj_t, pVtrack_item->rw.v_obj_z, pVtrack_item->rw.v_current_p, pVtrack_item->rw.v_current_t, pVtrack_item->rw.v_current_z);
            if (isready == 1) {
                pVtrack_item->rw.v_iscamready++;
            }
        }

        pVtrack_item->rw.v_waitcount++;
        pVtrack_item->rw.v_waitforcamera--;
        m_wait = 1;

        //如果连续两次判定已经转动到位
        if (pVtrack_item->rw.v_iscamready == 5) {

            FsPrintf(1, "(%s:%d)  the camera is ready!.. \n", __FUNCTION__, __LINE__);
            fflush(stdout);

            pVtrack_item->rw.v_waitcount = 0;
            pVtrack_item->rw.v_iscamready = 0;
            pVtrack_item->rw.v_waitforcamera = -1;
            m_wait = 0;
        }

    } else {

        int ret = linkage_ctrl_connect(pVtrack_item);
        if (ret < 0) {

            FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
        }
        int re = linkage_ctrl_getPTZ(pVtrack_item);
        pVtrack_item->rw.v_holder_lastConnectdx = 0;

        pVtrack_item->rw.v_waitforcamera = -1;
        pVtrack_item->rw.v_waitcount = 0;

        m_wait = 0;
    }

    //在等待过程中，给光电仪发送停止指令，以维持连接
    //    if(m_wait == 1 && pVtrack_item->ro.m_config_holder.m_holder_type == 1){
    //            if ((pFrame->index - pVtrack_item->rw.v_holder_lastConnectdx) > pVtrack_item->ro.m_config_holder.m_holder_keeptouchFrames) {
    //                
    //            pVtrack_item->rw.v_holder_lastConnectdx = pFrame->index;
    //            linkage_ctrl_stopMoveLeftRight(pVtrack_item);
    //        }
    //    }

    return m_wait;
}

/******************************************
 手动停止跟踪处理：
 1.手动停止，期望相机回到阈值位(客户端关闭跟踪)
 2.手动停止(联动跟踪)，期望相机不回到预置位
 3.热点功能，手动停止跟踪，并将相机转向设置的位置
 ********************************************/
void handleToStopTrack(Vtrack_item *pVtrack_item) {
    if (pVtrack_item->rw.v_handleToStop == 1 || pVtrack_item->rw.v_handleToStop == 2 || pVtrack_item->rw.v_handleToStop == 3) {

        //不需要回到预置位，发送停止命令
        if (pVtrack_item->rw.v_handleToStop == 1) {

            int ret = linkage_ctrl_connect(pVtrack_item);
            if (ret < 0) {
                FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                fflush(stdout);
            }

            FsPrintf(1, "(%s:%d)-(%s)  stop the camera,and not to turn to ptz... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            linkage_ctrl_stopMoveLeftRight(pVtrack_item);
        }

        //需要回到预置位
        if (pVtrack_item->rw.v_handleToStop == 2) {

            //先判定是否需要回到预置位，再断开相机控制端口连接
            if (pVtrack_item->ro.m_config_track.m_trackOverPTZ == 0) {

                int ret = linkage_ctrl_connect(pVtrack_item);
                if (ret < 0) {

                    FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                    fflush(stdout);
                }
                FsPrintf(1, "(%s:%d)-(%s)  stop the camera,and not to turn to ptz... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                fflush(stdout);

                linkage_ctrl_stopMoveLeftRight(pVtrack_item);

            } else {

                if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {
                    int ret = linkage_ctrl_connect(pVtrack_item);
                    if (ret < 0) {

                        FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                        fflush(stdout);
                    }
                }

                linkage_ctrl_goPTZ(pVtrack_item, 0);
            }
        }

        //转动相机到指定ptz位置
        if (pVtrack_item->rw.v_handleToStop == 3) {

            printf(">>>>>>>>guide (m_camera_brand:%d)<<<<<<<<<<<<\n", pVtrack_item->ro.m_config_devinfo.m_camera_brand);
            if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1 || pVtrack_item->ro.m_config_devinfo.m_camera_brand == 2) {
                int ret = linkage_ctrl_connect(pVtrack_item);
                if (ret < 0) {
                    FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                    fflush(stdout);
                }
            }
            linkage_ctrl_goPTZ(pVtrack_item, 1);
        }


        if (pVtrack_item->rw.v_findObj == 1) {

            FsPrintf(1, "(%s)Hand to stop the tracking,and save the video... \n", pVtrack_item->ro.m_config_devinfo.m_camera_ip);
            fflush(stdout);
            //保存跟踪录像
            pVtrack_item->rw.v_incEndTime = fs_time_uptime_get();
            FsPrintf(1, "before savetrackvideo(Line:%d ip:%s path:%s).....\n", __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->rw.v_videoPath);
            fflush(stdout);
            SaveTrackVideo(pVtrack_item);
        }

        //清零所有中间参数
        resetParam(pVtrack_item);
    }

    //雷达导引相机转动
    if (pVtrack_item->rw.v_handleToStop == 4) {
        //printf(">>>>>>>>guide (m_camera_brand:%d)<<<<<<<<<<<<\n", pVtrack_item->ro.m_config_devinfo.m_camera_brand);
        if (pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1 || pVtrack_item->ro.m_config_devinfo.m_camera_brand == 2) {
            int ret = linkage_ctrl_connect(pVtrack_item);
            if (ret < 0) {
                FsPrintf(1, "(%s:%d)-(%s) cannot connect the camera... \n", __FUNCTION__, __LINE__, pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                fflush(stdout);
            }
        }
        linkage_ctrl_goPTZ(pVtrack_item, 1);

        pVtrack_item->rw.v_handleToStop = 0;
    }
}

//红光球机视频自动检测跟踪

void auto_redballcam_track(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int currFrameIndex) {

    //如果发现目标(雷达导引目标),或者不支持視頻自動檢測
    if (pVtrack_item->rw.v_findObj == 1 || pVtrack_item->ro.m_config_videoDect.m_valid == 0) {
        return;
    }

    //先进行运动目标检测算法测试
    double tttt1 = fs_time_uptime_get();
    RedsearchMotionblock_auto(pVtrack_item, pFrame, pSrcImage, sw, sh, currFrameIndex);

    //添加新的跟踪目标
    RedaddTrackObj_auto(pVtrack_item, pSrcImage, sw, sh, currFrameIndex);
    double tttt2 = fs_time_uptime_get();
    //跟踪算法处理...
    vEpl_proc(pSrcImage, sw, sh, &pVtrack_item->rw.g_vproc, pVtrack_item->rw.pm,
            currFrameIndex, 100, pVtrack_item->rw.v_holder_lastctrlDx,
            pVtrack_item->rw.v_holder_lastmoveXDx, pVtrack_item->rw.v_holder_lastmoveYDx,
            pVtrack_item->rw.v_camctrl_dir, pVtrack_item->rw.v_camctrl_dirY, pVtrack_item->ro.m_config_track.m_trackOverFlag3, NULL);

    //方向检测：检测船的运动方向
    //detectionDir(pVtrack_item, currFrameIndex, sw, sh);
    //结束跟踪判断
    RedcheckTrack_auto(pVtrack_item, currFrameIndex, sw, sh);
    //修改船的初始跟踪位置(船在画面中的相对位置)
    //changeLocation(pVtrack_item, currFrameIndex);
    //跟踪联动相机
    //linkageCamera(pVtrack_item, currFrameIndex, sw, sh);
    //绘制跟踪框
    drawTrackLoop(pVtrack_item, pFrame);
    //drawTrackLoop_auto(pVtrack_item, pFrame);
}


//白光球机视频自动检测跟踪

void auto_ballcam_track(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int currFrameIndex) {

    //如果发现目标(雷达导引目标),或者不支持視頻自動檢測
    if (pVtrack_item->rw.v_findObj == 1 || pVtrack_item->ro.m_config_videoDect.m_valid == 0) {
        return;
    }

    //先进行运动目标检测算法测试
    double tttt1 = fs_time_uptime_get();
    searchMotionblock_auto(pVtrack_item, pFrame, pSrcImage, sw, sh, currFrameIndex);

    //添加新的跟踪目标
    addTrackObj_auto(pVtrack_item, pSrcImage, sw, sh, currFrameIndex);
    double tttt2 = fs_time_uptime_get();
    //跟踪算法处理...
    vEpl_proc(pSrcImage, sw, sh, &pVtrack_item->rw.g_vproc, pVtrack_item->rw.pm,
            currFrameIndex, 100, pVtrack_item->rw.v_holder_lastctrlDx,
            pVtrack_item->rw.v_holder_lastmoveXDx, pVtrack_item->rw.v_holder_lastmoveYDx,
            pVtrack_item->rw.v_camctrl_dir, pVtrack_item->rw.v_camctrl_dirY, pVtrack_item->ro.m_config_track.m_trackOverFlag3, NULL);

    //方向检测：检测船的运动方向
    //detectionDir(pVtrack_item, currFrameIndex,sw,sh);
    //结束跟踪判断
    checkTrack_auto(pVtrack_item, pFrame, currFrameIndex, sw, sh);
    //修改船的初始跟踪位置(船在画面中的相对位置)
    //changeLocation(pVtrack_item, currFrameIndex);  
    //跟踪联动相机
    //linkageCamera(pVtrack_item, currFrameIndex,sw,sh);
    //绘制跟踪框
    drawTrackLoop(pVtrack_item, pFrame);
    //drawTrackLoop_auto(pVtrack_item, pFrame);
}

/*
 * 返回值0：还没有到屏蔽视频自动检测时间段
 * 返回值1：进入屏蔽视频自动检测时间段
 */
int timevalidDetect(Vtrack_item *pVtrack_item) {

    //pVtrack_item->ro.m_config_videoDect.m_close_starttime = 20;
    //pVtrack_item->ro.m_config_videoDect.m_close_endtime   =  6;

    int valid = 0;
    if (pVtrack_item->ro.m_config_videoDect.m_close_starttime == pVtrack_item->ro.m_config_videoDect.m_close_endtime) {
        return 0;
    }

    time_t ts = (time_t) fs_time_GMT_get();
    struct tm ti_s;
    localtime_r(&ts, &ti_s);

    int hour = ti_s.tm_hour;

    if (pVtrack_item->ro.m_config_videoDect.m_close_starttime < pVtrack_item->ro.m_config_videoDect.m_close_endtime) {
        if (hour > pVtrack_item->ro.m_config_videoDect.m_close_starttime && hour < pVtrack_item->ro.m_config_videoDect.m_close_endtime) {
            valid = 1;
        }
    } else {
        if (hour > pVtrack_item->ro.m_config_videoDect.m_close_starttime || hour < pVtrack_item->ro.m_config_videoDect.m_close_endtime) {
            valid = 1;
        }
    }

    return valid;
}

//白天夜间自动检测

void daynightDetect(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int currFrameIndex) {
    //pVtrack_item->ro.m_config_videoDect.m_avegray = 40;
    //printf("%d %d %d %d\n",currFrameIndex,pVtrack_item->ro.m_config_videoDect.m_avegray,pVtrack_item->rw.m_checkdayparam.daynight,pVtrack_item->rw.m_checkdayparam.nightcount);

    //红外相机不进行白昼检测
    if (pVtrack_item->ro.m_config_devinfo.m_camera_model == 1 || pVtrack_item->ro.m_config_devinfo.m_camera_model == 3 || pVtrack_item->ro.m_config_devinfo.m_camera_model == 5)
        return;

    //隔帧统计一次，并该阈值大于零
    if (currFrameIndex % 2 == 0 && pVtrack_item->ro.m_config_videoDect.m_avegray > 0) {

        int m_avegray = getAveGray(pSrcImage, sw, sh, 1, sw - 1, 30, sh - 30);
        pVtrack_item->rw.m_autotrack_aveGray = m_avegray;
        //当前状态是白天
        if (pVtrack_item->rw.m_checkdayparam.daynight == 0) {
            if (m_avegray < pVtrack_item->ro.m_config_videoDect.m_avegray) {
                pVtrack_item->rw.m_checkdayparam.nightcount++;
            } else {
                pVtrack_item->rw.m_checkdayparam.nightcount = 0;
            }
            //连续2分钟都是小于阈值平均灰度
            if (pVtrack_item->rw.m_checkdayparam.nightcount > 15 * 60 * 2) {
                pVtrack_item->rw.m_checkdayparam.daynight = 1;
                pVtrack_item->rw.m_checkdayparam.nightcount = 0;
                printf("****the gray is too low,change to night,(gray:%d th:%d)*****\n", m_avegray, pVtrack_item->ro.m_config_videoDect.m_avegray);
            }
        }

        //当前状态是夜间
        if (pVtrack_item->rw.m_checkdayparam.daynight == 1) {
            if (m_avegray > pVtrack_item->ro.m_config_videoDect.m_avegray + 15) {
                pVtrack_item->rw.m_checkdayparam.daycount++;
            } else {
                pVtrack_item->rw.m_checkdayparam.daycount = 0;
            }
            //连续2分钟都是小于阈值平均灰度
            if (pVtrack_item->rw.m_checkdayparam.daycount > 15 * 60 * 2) {
                pVtrack_item->rw.m_checkdayparam.daynight = 0;
                pVtrack_item->rw.m_checkdayparam.daycount = 0;
                printf("****the gray is too hight,change to day,(gray:%d th:%d)*****\n", m_avegray, pVtrack_item->ro.m_config_videoDect.m_avegray);
            }
        }
    }
}


//获取相机当前ptz，并且与相机的检测预置位比较，看相机是否回到预置位上

/*
 * 返回值：10:获取ptz失败  
 *          1：正在使用的预置点不存在：直接返回1，当做相机不在预置位处理
 *          2: 正在使用的预置点存在，但云台转向位置不是设置的预置位：判定为相机不在预置位点
 *          3: 正在使用的预置点存在，云台转向位置是设置的预置位，但不是正在使用的那一个：相机被认为移动到另一个预置点
 *      
 *          0: 云台在当前预置位上
 */
int checkCamPTZ(Vtrack_item *pVtrack_item) {
    int unready = 0;

    //如果是球机，且为海康云台
    if (pVtrack_item->ro.m_config_videoDect.m_camtpye != 0 && pVtrack_item->ro.m_config_devinfo.m_camera_brand == 0) {
        //获取一次ptz    
        int re = linkage_ctrl_getPTZ(pVtrack_item);
        if (re == 0) {
            //一次比较每个轮巡点的ptz，
            int currpos_id = -1;
            int usingpos_id = -1;
            int index = 0;
            int dif_p = 0;
            int dif_t = 0;
            int dif_z = 0;
            for (index = 0; index < pVtrack_item->ro.m_config_videoDect.m_ptz_count; index++) {
                dif_p = abs(pVtrack_item->ro.m_config_videoDect.m_pos[index].search_p - (int) pVtrack_item->rw.v_current_p);
                dif_t = abs(pVtrack_item->ro.m_config_videoDect.m_pos[index].search_t - (int) pVtrack_item->rw.v_current_t);
                dif_z = abs(pVtrack_item->ro.m_config_videoDect.m_pos[index].search_z - (int) pVtrack_item->rw.v_current_z);
                //如果当前ptz与index位置的ptz相近，就认为相机在该位置
                if (dif_z < 2 && dif_p < 2 && dif_t < 2) {
                    currpos_id = index;
                }
                //正在使用的预置位
                if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused > 0) {
                    usingpos_id = index;
                }
            }

            //正在使用的预置点不存在：直接返回1，当做相机不在预置位处理
            if (usingpos_id == -1) {
                printf("%s:%d  no positon is using \n", __FUNCTION__, __LINE__);
                unready = 1;
                //return 1;
            }

            //正在使用的预置点存在，但云台转向位置不是设置的预置位：判定为相机不在预置位点
            if (usingpos_id > -1 && currpos_id == -1) {
                pVtrack_item->rw.m_cammoved_flag++;
                unready = 2;
            }

            //相机移动开始时间
            if (pVtrack_item->rw.m_cammoved_flag == 1) {
                pVtrack_item->rw.v_cammoved_last_p = pVtrack_item->rw.v_current_p;
                pVtrack_item->rw.v_cammoved_last_t = pVtrack_item->rw.v_current_t;
                pVtrack_item->rw.v_cammoved_last_z = pVtrack_item->rw.v_current_z;
                pVtrack_item->rw.m_cammoved_start_time = fs_time_uptime_get();
                resetParam(pVtrack_item);
                printf("%s:%d  check the camera moved,reset some param \n", __FUNCTION__, __LINE__);
            }

            //持续偏离预置位，计算前后两次ptz的差值，如果有移动，更新相机开始移动时间
            if (pVtrack_item->rw.m_cammoved_flag > 1) {

                dif_p = (int) abs(pVtrack_item->rw.v_cammoved_last_p - pVtrack_item->rw.v_current_p);
                dif_t = (int) abs(pVtrack_item->rw.v_cammoved_last_t - pVtrack_item->rw.v_current_t);
                dif_z = (int) abs(pVtrack_item->rw.v_cammoved_last_z - pVtrack_item->rw.v_current_z);
                //前后有移动
                if (dif_z > 2 || dif_p > 2 || dif_t > 2) {
                    pVtrack_item->rw.m_cammoved_start_time = fs_time_uptime_get();
                }

                pVtrack_item->rw.v_cammoved_last_p = pVtrack_item->rw.v_current_p;
                pVtrack_item->rw.v_cammoved_last_t = pVtrack_item->rw.v_current_t;
                pVtrack_item->rw.v_cammoved_last_z = pVtrack_item->rw.v_current_z;
            }

            //相机持续偏离预置位时间超时
            if ((pVtrack_item->rw.m_cammoved_flag > 1) && (abs(fs_time_uptime_get() - pVtrack_item->rw.m_cammoved_start_time) > pVtrack_item->ro.m_config_videoDect.m_camera_movetime)) {
                reSetVideoDectParam(pVtrack_item);
                pVtrack_item->rw.m_cammoved_flag = 0;
                printf("%s:%d  the camera moved too long, auto goto the prepos \n", __FUNCTION__, __LINE__);
            }

            //正在使用的预置点存在，云台转向位置是设置的预置位，但不是正在使用的那一个：相机被认为移动到另一个预置点    
            if (currpos_id > -1 && usingpos_id != currpos_id) {

                //将当前使用的相机状态重置
                if (usingpos_id > -1) {
                    pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].startTime = 0;
                    pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].isused = 0;
                    pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].isready = 0;
                }
                //设置云台转向的预置位为可使用状态
                pVtrack_item->ro.m_config_videoDect.m_pos[currpos_id].startTime = fs_time_uptime_get();
                pVtrack_item->ro.m_config_videoDect.m_pos[currpos_id].isused = 1;
                pVtrack_item->ro.m_config_videoDect.m_pos[currpos_id].isready = 0;

                pVtrack_item->rw.m_cammoved_flag = 0;
                printf("%s:%d  the camera turn to the next pos,change the camera status \n", __FUNCTION__, __LINE__);
                unready = 3;
                //return 1;                    
            }

            //正在使用的预置点存在，云台转向位置是设置的预置位，正好是正在使用的那一个：相机在当前预置点
            if (usingpos_id > -1 && currpos_id > -1 && usingpos_id == currpos_id) {
                pVtrack_item->rw.m_cammoved_flag = 0;
            }

        } else {
            printf("%s:%d GetPTZ failed...t:%f \n", __FUNCTION__, __LINE__);
            unready = 10;
        }
    } else if (pVtrack_item->ro.m_config_videoDect.m_camtpye != 0 && pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1) {

        float currP, currT, currZ;
        //int re = _zgx_camera_getptz(pVtrack_item, &currP, &currT, &currZ);
        int re = camera_ctrl_tcp_getPTZ(pVtrack_item->rw.pCamera_ctrl_item, &currP, &currT, &currZ);
        if (re == 0) {
            //一次比较每个轮巡点的ptz，
            int currpos_id = -1;
            int usingpos_id = -1;
            int index = 0;
            int dif_p = 0;
            int dif_t = 0;
            int dif_z = 0;
            for (index = 0; index < pVtrack_item->ro.m_config_videoDect.m_ptz_count; index++) {
                dif_p = abs(pVtrack_item->ro.m_config_videoDect.m_pos[index].search_p - (int) currP);
                dif_t = abs(pVtrack_item->ro.m_config_videoDect.m_pos[index].search_t - (int) currT);
                dif_z = abs(pVtrack_item->ro.m_config_videoDect.m_pos[index].search_z - (int) currZ);
                //如果当前ptz与index位置的ptz相近，就认为相机在该位置
                if (dif_z < 20 && dif_p < 2 && dif_t < 2) {

                    //                    printf("%s:%d curr p-t-z(%f %f %f)  set p-t-z(%d %d %d) dif_z:%d dif_p:%d dif_t:%d \n", __FUNCTION__, __LINE__, currP, currT, currZ,
                    //                            pVtrack_item->ro.m_config_videoDect.m_pos[index].search_p, pVtrack_item->ro.m_config_videoDect.m_pos[index].search_t,
                    //                            pVtrack_item->ro.m_config_videoDect.m_pos[index].search_z, dif_z, dif_p, dif_t);

                    currpos_id = index;
                }
                //正在使用的预置位
                if (pVtrack_item->ro.m_config_videoDect.m_pos[index].isused > 0) {
                    usingpos_id = index;
                }
            }


            //正在使用的预置点不存在：直接返回1，当做相机不在预置位处理
            if (usingpos_id == -1) {
                printf("%s:%d  no positon is using \n", __FUNCTION__, __LINE__);
                unready = 1;
                //return 1;
            }
            //正在使用的预置点存在，但云台转向位置不是设置的预置位：判定为相机不在预置位点
            if (usingpos_id > -1 && currpos_id == -1) {
                pVtrack_item->rw.m_cammoved_flag++;
                unready = 2;
                //return 1;     
            }

            //相机移动开始时间
            if (pVtrack_item->rw.m_cammoved_flag == 1) {
                pVtrack_item->rw.v_cammoved_last_p = pVtrack_item->rw.v_current_p;
                pVtrack_item->rw.v_cammoved_last_t = pVtrack_item->rw.v_current_t;
                pVtrack_item->rw.v_cammoved_last_z = pVtrack_item->rw.v_current_z;
                pVtrack_item->rw.m_cammoved_start_time = fs_time_uptime_get();
                resetParam(pVtrack_item);
                printf("%s:%d  check the camera moved,reset some param \n", __FUNCTION__, __LINE__);
            }

            //持续偏离预置位，计算前后两次ptz的差值，如果有移动，更新相机开始移动时间
            if (pVtrack_item->rw.m_cammoved_flag > 1) {

                dif_p = (int) abs(pVtrack_item->rw.v_cammoved_last_p - pVtrack_item->rw.v_current_p);
                dif_t = (int) abs(pVtrack_item->rw.v_cammoved_last_t - pVtrack_item->rw.v_current_t);
                dif_z = (int) abs(pVtrack_item->rw.v_cammoved_last_z - pVtrack_item->rw.v_current_z);
                //前后有移动
                if (dif_z > 15 || dif_p > 2 || dif_t > 2) {
                    pVtrack_item->rw.m_cammoved_start_time = fs_time_uptime_get();
                    printf("%s:%d the camera is moving \n", __FUNCTION__, __LINE__);
                }

                pVtrack_item->rw.v_cammoved_last_p = pVtrack_item->rw.v_current_p;
                pVtrack_item->rw.v_cammoved_last_t = pVtrack_item->rw.v_current_t;
                pVtrack_item->rw.v_cammoved_last_z = pVtrack_item->rw.v_current_z;
            }

            //相机持续偏离预置位时间超时
            if ((pVtrack_item->rw.m_cammoved_flag > 1) && (abs(fs_time_uptime_get() - pVtrack_item->rw.m_cammoved_start_time) > pVtrack_item->ro.m_config_videoDect.m_camera_movetime)) {
                reSetVideoDectParam(pVtrack_item);
                pVtrack_item->rw.m_cammoved_flag = 0;
                printf("%s:%d  the camera moved too long, auto goto the prepos \n", __FUNCTION__, __LINE__);
            }

            //正在使用的预置点存在，云台转向位置是设置的预置位，但不是正在使用的那一个：相机被认为移动到另一个预置点
            if (currpos_id > -1 && usingpos_id != currpos_id) {
                //将当前使用的相机状态重置
                if (usingpos_id > -1) {
                    pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].startTime = 0;
                    pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].isused = 0;
                    pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].isready = 0;
                }

                //设置云台转向的预置位为可使用状态
                pVtrack_item->ro.m_config_videoDect.m_pos[currpos_id].startTime = fs_time_uptime_get();
                pVtrack_item->ro.m_config_videoDect.m_pos[currpos_id].isused = 1;
                pVtrack_item->ro.m_config_videoDect.m_pos[currpos_id].isready = 0;

                pVtrack_item->rw.m_cammoved_flag = 0;
                printf("%s:%d  the camera turn to the next pos,change the camera status \n", __FUNCTION__, __LINE__);
                unready = 3;
            }

            //正在使用的预置点存在，云台转向位置是设置的预置位，正好是正在使用的那一个：相机在当前预置点
            if (usingpos_id > -1 && currpos_id > -1 && usingpos_id == currpos_id) {
                pVtrack_item->rw.m_cammoved_flag = 0;
            }

            //            if (strcmp(pVtrack_item->ro._uuid, "2076") == 0) {
            //
            //                printf("%s:%d curr p-t-z(%f %f %f)  set p-t-z(%d %d %d)  unready:%d usingpos_id:%d,currpos_id:%d\n", __FUNCTION__, __LINE__, currP, currT, currZ,
            //                        pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].search_p, pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].search_t,
            //                        pVtrack_item->ro.m_config_videoDect.m_pos[usingpos_id].search_z, unready, usingpos_id, currpos_id);
            //            }

        } else {
            printf("%s:%d GetPTZ failed.. \n", __FUNCTION__, __LINE__);
            unready = 10;
        }
    }

    return unready;
}

/*
  1.检测白天和夜间
 * 全图平均灰度持续2分钟小于阈值，   判定为夜间
 * 全图平均灰度持续2分钟大于阈值+15，判定为白天
 * 
 * 返回值0：进行视频自动检测
 * 返回值1：不进行视频自动检测
 */
int auto_prapare_track(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int currFrameIndex) {

    pVtrack_item->rw.m_autotrack_status = 0;

    //如果发现目标(雷达导引目标),或者不支持視頻自動檢測
    if (pVtrack_item->rw.v_findObj == 1 || pVtrack_item->ro.m_config_videoDect.m_valid == 0) {
        return 1;
    }

    //白昼自动检测
    daynightDetect(pVtrack_item, pFrame, pSrcImage, sw, sh, currFrameIndex);
    //时间判断
    int ret = timevalidDetect(pVtrack_item);

    if (ret == 1) pVtrack_item->rw.m_autotrack_status = 1;
    if (pVtrack_item->rw.m_checkdayparam.daynight == 1) pVtrack_item->rw.m_autotrack_status = 2;

    //当前时间是视频自动检测屏蔽时间段，或者白昼自动检测为夜间
    if (ret == 1 || pVtrack_item->rw.m_checkdayparam.daynight == 1) {
        return 1;
    }

    //如果不支持预置位检测，直接返回
    if (pVtrack_item->ro.m_config_videoDect.m_camera_movevalid == 0)
        return 0;


    //球机隔25帧获取一次，光电仪隔50帧获取一次(光电仪频繁获取有问题)，其他设备(枪机)，不做处理
    int dif = 0;
    if (pVtrack_item->ro.m_config_videoDect.m_camtpye != 0 && pVtrack_item->ro.m_config_devinfo.m_camera_brand == 0)
        dif = 15 * 60;
    else if (pVtrack_item->ro.m_config_videoDect.m_camtpye != 0 && pVtrack_item->ro.m_config_devinfo.m_camera_brand == 1)
        dif = 15 * 60 * 2;
    else
        dif = 15 * 60 * 3;


    if (dif > 0 && currFrameIndex % dif == 0) {
        int last_ready_status = pVtrack_item->rw.m_videotrack_ready;
        pVtrack_item->rw.m_videotrack_ready = checkCamPTZ(pVtrack_item);

        //球机从没有在预置位切换到到预置位处，此时重新开始进行视频自动检测
        if (last_ready_status > 0 && pVtrack_item->rw.m_videotrack_ready == 0) {
            printf("%s:%d the camera moved to the right pos...\n", __FUNCTION__, __LINE__);
            reSetVideoDectParam(pVtrack_item);
            return 1;
        }

        //socket链接不上
        if (pVtrack_item->rw.m_videotrack_ready == 10) {

        }
    }

    pVtrack_item->rw.m_autotrack_status = pVtrack_item->rw.m_videotrack_ready;
    return (pVtrack_item->rw.m_videotrack_ready > 0) ? 1 : 0;
}

//白光枪机视频自动检测跟踪

void auto_shotcam_track(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int currFrameIndex) {

    //如果发现目标(雷达导引目标),或者不支持視頻自動檢測
    if (pVtrack_item->rw.v_findObj == 1 || pVtrack_item->ro.m_config_videoDect.m_valid == 0) {
        return;
    }

    //先进行运动目标检测算法测试
    double tttt1 = fs_time_uptime_get();
    searchMotionblock_auto(pVtrack_item, pFrame, pSrcImage, sw, sh, currFrameIndex);

    //添加新的跟踪目标
    addTrackObj_auto(pVtrack_item, pSrcImage, sw, sh, currFrameIndex);
    double tttt2 = fs_time_uptime_get();
    //跟踪算法处理...
    vEpl_proc(pSrcImage, sw, sh, &pVtrack_item->rw.g_vproc, pVtrack_item->rw.pm,
            currFrameIndex, 100, pVtrack_item->rw.v_holder_lastctrlDx,
            pVtrack_item->rw.v_holder_lastmoveXDx, pVtrack_item->rw.v_holder_lastmoveYDx,
            pVtrack_item->rw.v_camctrl_dir, pVtrack_item->rw.v_camctrl_dirY, pVtrack_item->ro.m_config_track.m_trackOverFlag3, NULL);

    //方向检测：检测船的运动方向
    //detectionDir(pVtrack_item, currFrameIndex,sw,sh);
    //结束跟踪判断
    checkTrack_auto(pVtrack_item, pFrame, currFrameIndex, sw, sh);
    //修改船的初始跟踪位置(船在画面中的相对位置)
    //changeLocation(pVtrack_item, currFrameIndex);  
    //跟踪联动相机
    //linkageCamera(pVtrack_item, currFrameIndex,sw,sh);
    //绘制跟踪框
    drawTrackLoop(pVtrack_item, pFrame);
    //drawTrackLoop_auto(pVtrack_item,pFrame);    
}

//雷达导引跟踪

int radar_track(Vtrack_item *pVtrack_item, FsObjectImageFrame *pFrame, unsigned char *pSrcImage, int sw, int sh, int currFrameIndex) {

    //如果没有发现目标(雷达导引目标)
    if (pVtrack_item->rw.v_findObj == 0) {
        return 0;
    }

    int trackover = 0;

    //搜索运动块,如果找到运动块之后，连接上云台控制
    double tttt1 = fs_time_uptime_get();

    //if(pVtrack_item->rw.m_camera_model == 0){ //白光相机，处理图像为小图
    searchMotionblock(pVtrack_item, pFrame, pSrcImage, sw, sh, currFrameIndex);
    //}else{                                    //红光相机，处理图像为大图
    //    searchRedMotionblock(pVtrack_item,pFrame, pSrcImage,sw,sh,currFrameIndex);
    //}


    //二次搜索运动块(云台在设定时间内没有转动之后，开始搜索)
    reSearchMotionblock(pVtrack_item, pSrcImage, sw, sh, currFrameIndex);

    //添加新的跟踪目标
    addTrackObj(pVtrack_item, pSrcImage, sw, sh, currFrameIndex);

    //获取ptz的值，判定球机是否在转动(无法高频率获取ptz，云台不支持)
    //4：表示球机在远近拉伸； 3：表示球机在垂直转动(上)； 2：表示球机在水平转动(左)  0：表示球机停止转动  -1：表示ptz获取失败
    //                       -3：表示球机在垂直转动(下)；-2：表示球机在水平转动(右)   
    //int holder_status = 3;//camera_get_ptz(pVtrack_item, currFrameIndex); //暂不用

    double tttt2 = fs_time_uptime_get();
    //跟踪算法处理...
    vEpl_proc(pSrcImage, sw, sh, &pVtrack_item->rw.g_vproc, pVtrack_item->rw.pm,
            currFrameIndex, pVtrack_item->ro.m_config_devinfo.m_camera_brand, pVtrack_item->rw.v_holder_lastctrlDx,
            pVtrack_item->rw.v_holder_lastmoveXDx, pVtrack_item->rw.v_holder_lastmoveYDx,
            pVtrack_item->rw.v_camctrl_dir, pVtrack_item->rw.v_camctrl_dirY, pVtrack_item->ro.m_config_track.m_trackOverFlag3, NULL);

    //方向检测：检测船的运动方向
    detectionDir(pVtrack_item, currFrameIndex, sw, sh);

    //结束跟踪判断
    trackover = checkTrack(pVtrack_item, currFrameIndex, sw, sh);

    //修改船的初始跟踪位置(船在画面中的相对位置)
    changeLocation(pVtrack_item, currFrameIndex);

    //跟踪联动相机
    linkageCamera(pVtrack_item, currFrameIndex, sw, sh);

    //绘制跟踪框
    drawTrackLoop(pVtrack_item, pFrame);

    //1:本次跟踪结束
    return trackover;
}

/* 通道分层多线程写入法 */
static void *vtrack_P_T(struct Vtrack * const pVtrack) {
#undef FsFunctionName
#define FsFunctionName vtrack_P_T
    //FsPrintf(1, "Thread Index=%hu.\n", pRecord->p.threadCount);

    FsPrintf(2, "vtrack_private_thread begin ....\n");
    fflush(stdout);
    ////////////////////////////////////////////////////////////////////////////
    /* 创建线程监控 */
    void *pMonitor = pVtrack->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, pVtrack->ro._threadName, NULL, pVtrack, pVtrack->ro._pMonitor ? pVtrack->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pVtrack->ro._externRunStatus;
    //RtspServer *pRtspServer = pVtrack->ro._pRtspServer;
    ConfigManager * const pConfigManager = (ConfigManager*) pVtrack->ro._pConfigManager;
    FsObjectList * const itemList_ = pVtrack->ro.__itemList_;
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
#define __vtrack_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
#define __vtrack_P_T_clean2_count 0
    //    /* 只能在单线程中使用的缓存数据,一般用于发数据 */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __vtrack_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可在多线程中使用的缓存buffer,申请的数据可传给其他线程,一般用于收数据 */
    //FsObjectBaseBuffer baseBuffer;
    //fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __vtrack_P_T_clean1_3 //fs_ObjectBaseBuffer_release(&baseBuffer);  
    ////////////////////////////////////////////////////////////////////////////
    // 通道数
    unsigned int channelCount = 0;
    /* 开方分组 */
    FsGroupSqrt groupSqrt;
    Fs_GroupSqrt_init(&groupSqrt);
#define __vtrack_P_T_clean1_4 {Fs_GroupSqrt_release(&groupSqrt);}   
#define __vtrack_P_T_clean1_count 4
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    FsObjectImageFrame *pFrame;
    struct Vtrack_item *pVtrack_item, **ppNode_itemList;
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
    while (pVtrack->rw.runStatus == *externRunStatus) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        usleep(1000);

        if (sleepTime < 1000000)sleepTime = (sleepTime + 1) << 1;
        ////////////////////////////////////////////////////////////////////////
        if (pVtrack->rw._resetChannel>-1) {
            sleepTime = 0;
            /* 读取通道数 */

            if (0 == pVtrack->rw._resetChannel) channelCount = vtrack_private_get_channelCount(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            //FsPrintf(1, "%u\n", channelCount);
            /* 重置开方分组 */
#define __groupSqrt_reset_Server Vtrack
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
                pVtrack_item = (struct Vtrack_item *) fs_ObjectList_remove_tail(itemList_);
                if (pVtrack_item)vtrack_private_item_delete__OI(pVtrack_item, &shareBuffer);
            }
            /* 添加通道 */
            while (itemList_->nodeCount < channelCount) fs_ObjectList_insert_tail(itemList_, NULL);
            /* 重置 */
            if (itemList_->nodeCount < (unsigned int) pVtrack->rw._resetChannel) {
                /* 出错 */
                FsPrintf(1, "Invalid channel(=%u/%lu).\n", pVtrack->rw._resetChannel, itemList_->nodeCount);
                fflush(stdout);
            } else if (pVtrack->rw._resetChannel > 0) {
                FsPrintf(1, "%d\n", pVtrack->rw._resetChannel);

                pMonitorItem->rw.line = __LINE__;
                track_private_item_new(pVtrack, pVtrack->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Vtrack_item **) (itemList_->pNode + pVtrack->rw._resetChannel - 1), &groupSqrt, &shareBuffer);
                //capture_item_new(pCapture, pCapture->rw._resetChannel, pConfigManager->ro.__ipList_local, (struct Capture_item **) (itemList->pNode + itemList->startIndex + pCapture->rw._resetChannel - 1),
                //       &pCamera_rtsp);
                pMonitorItem->rw.line = __LINE__;
            }
            pVtrack->rw._resetChannel = -1;
        }
        ////////////////////////////////////////////////////////////////////////pRecord_itemNode = (struct Record_item **) itemList->pNode + itemList->startIndex;
        /* 处理各通道数据 */
        {
            unsigned int ui = 0;
            struct Vtrack_item * * const ppNode_itemList = (struct Vtrack_item **) itemList_->pNode + itemList_->startIndex;
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
                struct Vtrack_item * const pVtrack_item = ppNode_itemList[ui];
                if (pVtrack_item == NULL)continue;
                if (0 == pVtrack_item->ro.moduleMask) {
                    if (pVtrack_item->ro.__framelistIn->nodeCount > 0) {
                        pthread_mutex_lock(&pVtrack_item->ro.__framelistIn->mutex);
                        fs_ObjectList_insert_tail_all_clean_pthreadSafety(pVtrack_item->ro.__framelistOut, pVtrack_item->ro.__framelistIn);
                        pthread_mutex_unlock(&pVtrack_item->ro.__framelistIn->mutex);
                        Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                    }
                    continue;
                }
                //            char buffer[512];
                //            sprintf(buffer, "%stt.mkv", pVtrack_item->ro._pVtrack->ro._dir);
                //            if (access("/fs/project/data/vsys/t.log", F_OK)) {
                //                pthread_mutex_lock(&pVtrack_item->ro.__mutexCmdConnect);
                //                if (pVtrack_item->p.record_item_add_partVideo) {
                //                    fs_String_buffer_save_to_file(2, "1\n", "/fs/project/data/vsys/t.log");
                //                    pVtrack_item->p.record_item_add_partVideo(pVtrack_item->p.pRecord_item, buffer, fs_time_uptime_get() - 10.0, fs_time_uptime_get() + 10.0, NULL);
                //                }
                //                pthread_mutex_unlock(&pVtrack_item->ro.__mutexCmdConnect);
                //            }

                if (pVtrack_item->ro.__framelistIn->nodeCount > 1) {

                    double ttt1 = fs_time_uptime_get();

                    //获取图像数据帧
                    pFrame = (FsObjectImageFrame*) fs_ObjectList_remove_head_pthreadSafety(pVtrack_item->ro.__framelistIn);
                    //获取帧号
                    currFrameIndex = pFrame->index;

                    //调试打印信息
                    if (currFrameIndex % 600 == 0) {
                        FsPrintf2(1, "(1102-2)[ip:%s uuid:%s framedx:%d] m_findObj:%d [auto_valid:%d autotrack:%d avegray:%d] m_camcrlrequesID:%d \n",
                                pVtrack_item->ro.m_config_devinfo.m_camera_ip, pVtrack_item->ro._uuid, currFrameIndex, pVtrack_item->rw.v_findObj,
                                pVtrack_item->ro.m_config_videoDect.m_valid, pVtrack_item->rw.m_autotrack_status, pVtrack_item->rw.m_autotrack_aveGray,
                                pVtrack_item->ro.m_camcrl_requesID_3[0]);
                        fflush(stdout);
                    }

                    //相机自动检测：取一张灰度小图
                    if (pVtrack_item->ro.m_config_videoDect.m_valid == 1 && currFrameIndex % 600 == 0) {

                        if (access("/fs/project/data/config", 0) == -1)//access函数是查看文件是不是存在
                        {
                            if (mkdir("/fs/project/data/config", 0777))//如果不存在就用mkdir函数来创建
                            {
                                printf("creat file bag failed!!!");
                            }
                        }

                        char path[512];
                        sprintf(path, "/fs/project/data/config/getpic_%s.dat", pVtrack_item->ro._uuid);
                        if (access(path, F_OK)) {
                            int w, h;
                            ImageDib* pdib;
                            sprintf(path, "/fs/project/data/config/getpic_%s.dat", pVtrack_item->ro._uuid);
                            FILE* f = fopen(path, "wb");
                            if (f != NULL) {
                                fwrite("get a bmp picture", strlen("get a bmp picture"), 1, f);
                                fclose(f);
                            }

                            if (pVtrack_item->ro.zoomTimes == 16 || pVtrack_item->ro.zoomTimes == 0) {
                                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_2);
                                w = pFrame->width[2];
                                h = pFrame->height[2];
                                pdib = image_dib_new__IO(pFrame->width[2], pFrame->height[2], ImageColorType_Gray, 0);
                                memcpy(pdib->data, pYUV420P->data, pFrame->width[2] * pFrame->height[2]);
                                fs_Object_delete_pthreadSafety__OI(pYUV420P);
                            } else if (pVtrack_item->ro.zoomTimes == 4) {
                                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_1);
                                w = pFrame->width[1];
                                h = pFrame->height[1];
                                pdib = image_dib_new__IO(pFrame->width[1], pFrame->height[1], ImageColorType_Gray, 0);
                                memcpy(pdib->data, pYUV420P->data, pFrame->width[1] * pFrame->height[1]);
                                fs_Object_delete_pthreadSafety__OI(pYUV420P);
                            } else {
                                FsObjectImageYUV420P * const pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_YUV420P_0);
                                pdib = image_dib_new__IO(pFrame->width[0], pFrame->height[0], ImageColorType_Gray, 0);
                                memcpy(pdib->data, pYUV420P->data, pFrame->width[0] * pFrame->height[0]);
                                fs_Object_delete_pthreadSafety__OI(pYUV420P);
                            }

                            printf("%d %d \n", w, h);
                            sprintf(path, "/fs/project/data/config/getpic_%s.bmp", pVtrack_item->ro._uuid);
                            image_dib_svae_to_file(pdib, path);
                            image_dib_delete__OI(&pdib);
                        }
                    }

                    //for test
                    if (0) {
                        //test_2(pVtrack_item, pFrame);

                        if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                            image_frame_clear_data(pFrame, ImageFrame_H264_0);
                        fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);

                        continue;
                    }

                    char filepath[256];
                    sprintf(filepath, "/fs/project/data/makeconfig/makeTrigger_%s.dat", pVtrack_item->ro._uuid);

                    if (!access(filepath, F_OK)) {

                        if (1) {
                            int x0 = 1257;
                            int y0 = 907;
                            int x1 = 1515;
                            int y1 = 1047;
                            alg_check_floater(pVtrack_item, pFrame, x0, y0, x1, y1, 1);
                        }
                        if (0) {
                            int w = pFrame->width[0];
                            int h = pFrame->height[0];
                            unsigned long lineBits_src = w;
                            unsigned long lineBits_dst = w * 3;

                            ImageDib* pdib = image_dib_new__IO(w, h, ImageColorType_RGB, 0);

                            // unsigned char* data_dst = (unsigned char*) fsMalloc(w * h * 3 * sizeof (unsigned char));
                            image_dib_copy(/* 目标图数据 */pdib->data, NULL, NULL, /* 目标图数据的储存宽度 */lineBits_dst,
                                    /* 目标图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,ImageColorType_YUV420p=5,ImageColorType_GBR = 6 */ ImageColorType_RGB,
                                    /* 原图数据 */pFrame->data.raw[0]->data, pFrame->data.raw[0]->data + pFrame->sizeWidthHeight[0]
                                    , pFrame->data.raw[0]->data + pFrame->sizeWidthHeight[0] / 4 * 5, /* 原图数据的储存宽度 */lineBits_src,
                                    /* 原图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,ImageColorType_YUV420p=5,ImageColorType_GBR = 6 */ ImageColorType_YUV420p,
                                    /* 图像宽度 */w, /* 图像高度 */h
                                    );

                            image_dib_svae_to_file(pdib, "/fs/project/data/temp/image_dib_copy.bmp");
                            image_dib_delete__OI(&pdib);
                        }

                        if (1) {

                            pVtrack_item->rw.v_videoPath[0] = 0;
                            pVtrack_item->rw.v_trackNum++;
                            sendMsg2Camctl(pVtrack_item, 1);

                            FsPrintf2(1, "============send lock msg==============.\n");
                            fflush(stdout);

                            usleep(2000);

                            sendMsg2Camctl(pVtrack_item, 3);
                            FsPrintf2(1, "============send stop msg==============.\n");

                            //保存模拟视频数据
                            pVtrack_item->rw.v_incStartTime = fs_time_uptime_get();
                            pVtrack_item->rw.v_incEndTime = fs_time_uptime_get() + 15;
                            SaveTrackVideo(pVtrack_item);
                        }

                        remove(filepath);

                        fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                        continue;

                        /*
                        static double t1 = 0.0, t2 = 0.0, t3 = 0.0;
                        if (fs_time_uptime_get() - t1 > 9) {
                            t1 = fs_time_uptime_get();
                            // sendlock2Camcontrol(pVtrack_item);
                            sendMsg2Camctl(pVtrack_item, 1);
                            FsPrintf2(1, "============send lock msg==============.\n");
                            fflush(stdout);
                        }

                        if (fs_time_uptime_get() - t2 > 16) {
                            t2 = fs_time_uptime_get();
                            sendMsg2Camctl(pVtrack_item, 2);
                            FsPrintf2(1, "============send stop msg==============.\n");
                            fflush(stdout);
                        }

                        if (0 && fs_time_uptime_get() - t3 > 20) {
                            t3 = fs_time_uptime_get();

                            sendMsg2Camctl(pVtrack_item, 2);
                            resetParam(pVtrack_item);
                            reSetVideoDectParam(pVtrack_item);

                            FsPrintf2(1, "============ip:%s==============.\n", pVtrack_item->ro.m_config_devinfo.m_camera_ip);
                            fflush(stdout);
                            //sendStop2Camscontrol(pVtrack_item);
                            //FsPrintf2(1, "============send stop msg==============.\n");
                            //fflush(stdout);                   
                        }
                         */
                    }

                    if (0) {
                        if (access("/fs/project/data/temp/test1.bmp", F_OK) && strcmp(pVtrack_item->ro._uuid, "1019") == 0) {

                            // hiki_camera_ctrl_Preset_Opt_Goto(pVtrack_item);

                            system("touch /fs/project/data/temp/test1.bmp");
                            FsPrintf(1, "============hiki_camera_ctrl_Preset_Opt_Goto==============.\n");
                            fflush(stdout);

                            //pVtrack_item->rw.v_findObj = 1;
                            //FsPrintf(1, "============find obj==============.\n");
                            //fflush(stdout);
                        }
                    }


                    //Metro检测
                    if (0) {
                        // 此部分代码需要重写
                        //准备小图数据
                        image_frame_prapare(pFrame, ImageFrame_RAW_2);

                        //获取处理小图的尺寸
                        sw = pFrame->width[2];
                        sh = pFrame->height[2];

                        //for test Decetion car
                        //DectionCar(pVtrack_item,pFrame);

                        //for  derelict
                        /*
                        static int bbbb = 0;
                        bbbb++;
                        if(bbbb > 500)
                        {
                            int init = 0;
                            static int aaa = 0;
                            if(aaa == 0){

                                pVtrack_item->rw.camDetectParam.eventTime = 20;
                                pVtrack_item->rw.camDetectParam.frameRate = 25;

                                pVtrack_item->rw.camDetectParam.s1[0][0] = 163;
                                pVtrack_item->rw.camDetectParam.s1[0][1] = 161;

                                pVtrack_item->rw.camDetectParam.s1[0][2] = 265;
                                pVtrack_item->rw.camDetectParam.s1[0][3] = 151;

                                pVtrack_item->rw.camDetectParam.s1[0][4] = 114;
                                pVtrack_item->rw.camDetectParam.s1[0][5] = 265;

                                pVtrack_item->rw.camDetectParam.s1[0][6] = 295;
                                pVtrack_item->rw.camDetectParam.s1[0][7] = 265;

                                pVtrack_item->rw.camDetectParam.s1[1][0] = -1;
                                pVtrack_item->rw.camDetectParam.s2[0]    = -1;
                                pVtrack_item->rw.camDetectParam.s3[0]    = -1;

                                init = vAVS_InitParam(channelindex, pFrame->data.raw[2]->data, sw, sh, &pVtrack_item->rw.camDetectParam);
                            
                                FsPrintf(1, "============vAVS_InitParam : %d==============.\n",init);
                                fflush(stdout);

                            }

                            aaa++;

                            int re = vAVS_ProcFrame(channelindex, pFrame->index, 0,  pFrame->data.raw[2]->data, sw, sh, &pVtrack_item->rw.camOutData);
                            if(re == 1){

                                int l, r, t, b;
                                l = pVtrack_item->rw.camOutData.nRect[0][0]*4;
                                t = pVtrack_item->rw.camOutData.nRect[0][1]*4;
                                r = pVtrack_item->rw.camOutData.nRect[0][2]*4;
                                b = pVtrack_item->rw.camOutData.nRect[0][3]*4;

                                 DrawTrackRect((char *) pFrame->data.raw[0]->data, pFrame->width[0], pFrame->height[0], pVtrack_item, l, r, t, b);
                                 FsPrintf(1, "============find the obj throwing!!!!!!==============.\n");
                                fflush(stdout);
                            }
                        }
                         */
                        if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                            image_frame_clear_data(pFrame, ImageFrame_H264_0);
                        fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);

                        continue;
                    }

                    //检测是否收到手动停止指令，如果收到，在这里处理...     
                    handleToStopTrack(pVtrack_item);

                    //跟踪准备工作:连接云台，转到目标位置，延迟等待(云台转向需要耗时)
                    int delay = Preparetrack(pVtrack_item, currFrameIndex);
                    if (delay == 1) {
                        fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                        Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                        continue;
                    }

                    //减帧处理(支持1/2原帧率处理和1/3原帧率处理，配置默认1/2) 
                    if (pVtrack_item->ro.m_trackframeInterval == 2) {
                        if (currFrameIndex % 2 == 0) {
                            drawTrackLoop(pVtrack_item, pFrame);
                            if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                                image_frame_clear_data(pFrame, ImageFrame_H264_0);
                            fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                            continue;
                        }
                        currFrameIndex = currFrameIndex / 2;

                    } else if (pVtrack_item->ro.m_trackframeInterval == 3) {
                        if (currFrameIndex % 3 == 0) {
                            drawTrackLoop(pVtrack_item, pFrame);
                            if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                                image_frame_clear_data(pFrame, ImageFrame_H264_0);
                            fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                            Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                            continue;
                        }
                    }

                    {
                        //准备图像数据(根据配置参数zoomTimes：16，4，1获取对应的小图数据)
                        FsObjectImageYUV420P * pYUV420P;
                        if (pVtrack_item->ro.zoomTimes == 16 || pVtrack_item->ro.zoomTimes == 0) { // 1/16原图大小
                            pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RAW_2);
                            //获取处理小图的尺寸
                            sw = pFrame->width[2];
                            sh = pFrame->height[2];
                        } else if (pVtrack_item->ro.zoomTimes == 4) { // 1/4原图大小
                            pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RAW_1);
                            //获取处理小图的尺寸
                            sw = pFrame->width[1];
                            sh = pFrame->height[1];
                        } else { // 原图大小             
                            pYUV420P = (FsObjectImageYUV420P*) image_frame_get_pthreadSafety__IO(pFrame, ImageFrame_RAW_0);
                            //获取处理大图的尺寸
                            sw = pFrame->width[0];
                            sh = pFrame->height[0];
                        }


                        /**********************************************************************
                         雷达导引目标自动跟踪。 返回1：表示雷达目标跟踪结束，在这种情况下，本次循环不做视频自动检测
                         ***********************************************************************/
                        int ret = radar_track(pVtrack_item, pFrame, pYUV420P->data, sw, sh, currFrameIndex);

                        /**********************************************************************
                         视频自动检测跟踪处理
                         * 在设置区域内进行实时的运动检测，如果运动块满足
                         ***********************************************************************/
                        if (ret == 0) {
                            /*
                             * 视频自动检测准备
                             * nnn = 0: 进行视频自动检测
                             */
                            int nnn = auto_prapare_track(pVtrack_item, pFrame, pYUV420P->data, sw, sh, currFrameIndex);
                            if (nnn == 0) {
                                if (pVtrack_item->ro.m_config_devinfo.m_camera_model == 2 || pVtrack_item->ro.m_config_devinfo.m_camera_model == 3) {
                                    //枪机
                                    auto_shotcam_track(pVtrack_item, pFrame, pYUV420P->data, sw, sh, currFrameIndex);
                                } else if (pVtrack_item->ro.m_config_devinfo.m_camera_model == 1) {
                                    //红光球机     
                                    auto_redballcam_track(pVtrack_item, pFrame, pYUV420P->data, sw, sh, currFrameIndex);
                                } else if (pVtrack_item->ro.m_config_devinfo.m_camera_model == 0 || pVtrack_item->ro.m_config_devinfo.m_camera_model == 4) {
                                    //白光球机,白光光电仪    
                                    auto_ballcam_track(pVtrack_item, pFrame, pYUV420P->data, sw, sh, currFrameIndex);
                                } else {
                                    printf("%s %d : unkown camera type \n", __FUNCTION__, __LINE__);
                                }
                            }
                        }
                        fs_Object_delete_pthreadSafety__OI(pYUV420P);
                    }

                    //数据传递...
                    if (ImageFrame_valid_H264_0(pFrame->dataValidType))
                        image_frame_clear_data(pFrame, ImageFrame_H264_0);
                    fs_ObjectList_insert_tail_pthreadSafety(pVtrack_item->ro.__framelistOut, pFrame);
                    Fs_GroupSqrtOut_value_set(pVtrack_item->rw._pGroupSqrtOut, &pVtrack_item->ro._pVtrack->ro._pGroupSqrtOut->groupSqrt_mutex);
                }
            }
        }
    }

    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    if (pVtrack->rw._resetChannel != -1) {
        FsPrintf(FsPrintfIndex, "Invalid resetChannel(=%d)", pVtrack->rw._resetChannel);
        fflush(stdout);
    }
    ////////////////////////////////////////////////////////////////////////////
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);

    /* 资源是多线程共享的,由第一个线程控制 */
    /* 删除各通道 */
    fs_ObjectList_clear_custom(pVtrack->ro.__itemList_, (void (*)(void*))vtrack_private_item_delete__OI);

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
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd,threadCount=%hu.\n", pVtrack->ro._threadName, pVtrack->rw.runStatus, pVtrack->p.threadCount);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pVtrack->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}

///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 运动检测配置项 */
void search_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "searchConfig", "雷达引导跟踪-目标定位", "雷达引导跟踪-目标定位", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "1");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "4");
    }
    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "trackFrameInterval", "算法处理帧率", "算法处理帧率(2：原帧率1/2，3:原帧率1/3,其他值：原帧率)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2", "2", "2");

    void *pNode2 = fs_Config_node_integer_add(pConfig, parent, "zoomTimes", "算法处理图像缩小倍数", "算法处理图像缩小倍数", FsConfig_nodeShowType_default, 0, 0x7, 0, 32, 1);
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_default, 16, "缩小16倍", "缩小16倍");
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 4, "缩小4倍", "缩小4倍");
    fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 1, "原图尺寸", "原图尺寸");

    pNode = fs_Config_node_string_add(pConfig, parent, "searchWidth", "搜索运动块范围", "搜索运动块范围(宽度)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "400", "400", "400");
    pNode = fs_Config_node_string_add(pConfig, parent, "searchHeight", "搜索运动块范围", "搜索运动块范围(高度)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "250", "250", "250");
    pNode = fs_Config_node_string_add(pConfig, parent, "modWidth", "滑动窗口宽", "滑动窗口宽", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "80", "80", "80");
    pNode = fs_Config_node_string_add(pConfig, parent, "modHeight", "滑动窗口高", "滑动窗口高", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "50", "50", "50");
    pNode = fs_Config_node_string_add(pConfig, parent, "modscale", "运动点占有比", "运动点占有比", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "30", "30", "30");
    pNode = fs_Config_node_string_add(pConfig, parent, "Moveth", "运动阈值", "运动阈值", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "30", "30", "30");
    pNode = fs_Config_node_string_add(pConfig, parent, "stopFrameCount", "停止检测帧数", "停止检测帧数", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "250", "250", "250");

    pNode = fs_Config_node_string_add(pConfig, parent, "isAssistCheck", "是否支持辅助检测", "是否支持辅助检测(0为否，其他值为是)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "isAssistFindbottom", "是否重新确定船体位置", "是否重新确定船体位置(0为否，其他值为是)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "AssistParam1", "辅助检测参数1", "辅助检测参数1", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "100", "100", "100");
    pNode = fs_Config_node_string_add(pConfig, parent, "AssistParam2", "辅助检测参数2", "辅助检测参数2", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "AssistParam3", "辅助检测参数3", "辅助检测参数3", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
}

/*云台控制相关配置*/
void holder_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "holderConfig", "雷达引导跟踪-云台控制", "雷达引导跟踪-云台控制", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "1");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "4");
    }

    /*    
        void *pNode = fs_Config_node_string_add(pConfig, parent, "holder_type", "云台类型", "云台类型(1:光电仪,2:海康球机)", 0, 0x7, 1, 5, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
        pNode = fs_Config_node_string_add(pConfig, parent, "holder_ip", "云台IP", "云台IP", 0, 0x7, 1, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "192.168.10.115", "192.168.10.115", "192.168.10.115");
        pNode = fs_Config_node_string_add(pConfig, parent, "holder_port", "云台端口", "云台端口", 0, 0x7, 1, 5, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "4001", "4001", "4001");
        pNode = fs_Config_node_string_add(pConfig, parent, "holder_port1", "云台端口1", "云台端口1", 0, 0x7, 1, 5, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "55302", "55302", "55302");
        pNode = fs_Config_node_string_add(pConfig, parent, "holder_name", "相机用户名", "相机用户名", 0, 0x7, 1, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "admin", "admin", "admin");
        pNode = fs_Config_node_string_add(pConfig, parent, "holder_pwd", "相机密码", "相机密码", 0, 0x7, 1, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12345", "12345", "12345");
     */
    void *pNode = fs_Config_node_string_add(pConfig, parent, "holder_speed", "云台转动速度", "云台转动速度", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "preset_p", "预置位", "预置位(P)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "12", "12");
    pNode = fs_Config_node_string_add(pConfig, parent, "preset_t", "预置位", "预置位(T)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "preset_z", "预置位", "预置位(Z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12", "12", "12");

    pNode = fs_Config_node_string_add(pConfig, parent, "holder_forceStopX", "强制云台停止左右转动", "发送左右转动命令后隔几帧发送停止命令(帧数)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "2", "2", "2");
    pNode = fs_Config_node_string_add(pConfig, parent, "holder_forceStopX_1", "云台两次左右转动的间隔帧数", "前后两次发送转动命令的最少间隔帧数", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12", "12", "12");

    pNode = fs_Config_node_string_add(pConfig, parent, "holder_forceStopY", "强制云台停止上下转动", "发送上下转动命令后隔几帧发送停止命令(帧数)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "holder_forceStopY_1", "云台两次上下转动的间隔帧数", "前后两次发送上下转动命令的最少间隔帧数", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");

    pNode = fs_Config_node_string_add(pConfig, parent, "holder_keeptouchFrames", "云台保持通讯的间隔帧数", "云台保持通讯的间隔帧数", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");

    pNode = fs_Config_node_string_add(pConfig, parent, "ConditionStart", "相机联动参数(转动条件)", "当船移动距离大于此阈值时才转动一次云台", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "10", "10", "10");
    pNode = fs_Config_node_string_add(pConfig, parent, "ConditionStop", "相机联动参数(停止条件)", "当船移动距离小于此阈值时停止转动云台", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");

    pNode = fs_Config_node_string_add(pConfig, parent, "ConditionDir", "运动方向判断条件", "在转动云台前当船移动距离大于此阈值时判断船的行驶方向", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");
}

/*调试配置项*/
void debug_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "debugConfig", "雷达引导跟踪-调试配置", "雷达引导跟踪-调试配置", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "1");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "4");
    }

    void *pNode = fs_Config_node_string_add(pConfig, parent, "savelog_flag", "是否保存打印日志", "是否保存打印日志(0：不保存 1：保存)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "savelog_path", "打印日志保存路径", "打印日志保存路径(绝对路径,250个字节以内)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "/fs/project/data/temp", "/fs/project/data/temp", "/fs/project/data/temp");

    pNode = fs_Config_node_integer_add(pConfig, parent, "limit_z", "限制阈值Z", "限制阈值Z(0到3600)", FsConfig_nodeShowType_default, 0, 0x7, 0, 3600, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    pNode = fs_Config_node_integer_add(pConfig, parent, "limit_p", "限制阈值P", "限制阈值P(0到3600)", FsConfig_nodeShowType_default, 0, 0x7, 0, 3600, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");
    pNode = fs_Config_node_integer_add(pConfig, parent, "limit_t", "限制阈值T", "限制阈值T(0到3600)", FsConfig_nodeShowType_default, 0, 0x7, 0, 3600, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");

    //    pNode = fs_Config_node_string_add(pConfig, parent, "change_z", "调整Z值", "调整Z值",0, 0x7, 1, 5, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "55", "55", "55");
    //    pNode = fs_Config_node_string_add(pConfig, parent, "change_p", "调整P值", "调整P值",0, 0x7, 1, 5, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    //    pNode = fs_Config_node_string_add(pConfig, parent, "change_t", "调整T值", "调整T值",0, 0x7, 1, 5, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");

    pNode = fs_Config_node_string_add(pConfig, parent, "savepic_flag", "是否保存调试图片", "是否保存调试图片(0：不保存 1：保存)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "savepic_path", "调试图片保存路径", "调试图片保存路径(绝对路径,250个字节以内)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "/fs/project/data/temp/search", "/fs/project/data/temp", "/fs/project/data/temp/search");

    pNode = fs_Config_node_string_add(pConfig, parent, "opendebug", "打开调试", "打开调试(0：不打开 >1：打开)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
}

/*跟踪配置项*/
void track_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "trackConfig", "雷达引导跟踪-目标跟踪", "雷达引导跟踪-目标跟踪", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "1");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "4");
    }
    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "trackWidth", "跟踪框大小", "跟踪框大小(宽度)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "80", "80", "80");
    pNode = fs_Config_node_string_add(pConfig, parent, "trackHeight", "跟踪框大小", "跟踪框大小(高度)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "60", "60", "60");

    pNode = fs_Config_node_string_add(pConfig, parent, "track_SearchWidth", "跟踪框的搜索范围", "跟踪框的搜索范围(宽度)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "100", "100", "100");
    pNode = fs_Config_node_string_add(pConfig, parent, "track_SearchHeight", "跟踪框的搜索范围", "跟踪框的搜索范围(高度)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");

    pNode = fs_Config_node_string_add(pConfig, parent, "track_Delay", "跟踪开始延迟帧数", "跟踪开始延迟帧数(25帧为1秒)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "50", "50", "50");

    pNode = fs_Config_node_string_add(pConfig, parent, "trackOverFlag1", "跟踪结束条件1", "跟踪结束条件1(水平方向反方向位移距离)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "60", "60", "60");
    pNode = fs_Config_node_string_add(pConfig, parent, "trackOverFlag2", "跟踪结束条件2", "跟踪结束条件2(垂直方向反方向位移距离)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "30", "30", "30");
    pNode = fs_Config_node_string_add(pConfig, parent, "trackOverFlag3", "跟踪结束条件3", "跟踪结束条件3(目标跟踪最长时限，单位：分钟)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");
    pNode = fs_Config_node_string_add(pConfig, parent, "trackOverFlag4", "跟踪结束条件4", "跟踪结束条件4(云台停止转动时间，单位：秒)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "10", "10", "10");
    pNode = fs_Config_node_string_add(pConfig, parent, "trackOverFlag5", "跟踪结束条件5", "跟踪结束条件5(云台停止转动时间，单位：秒)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "30", "30", "30");

    pNode = fs_Config_node_string_add(pConfig, parent, "trackOverHandle1", "跟踪结束处理1", "跟踪结束处理1(是否回到预置位 0：不回 1：返回)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "trackAgain", "跟踪结束处理2", "跟踪结束处理2(是否需要再次跟踪 0：不回 大于1：跟丢后再次跟踪次数)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");


    //    pNode = fs_Config_node_string_add(pConfig, parent, "Linewidth", "目标跟踪框的边框粗细", "目标跟踪框的边框粗细", 0, 0x7, 1, 5, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "4", "4", "4");
    //    pNode = fs_Config_node_string_add(pConfig, parent, "Linecolor", "目标跟踪框的颜色", "目标跟踪框的颜色(1：红色，2：绿色, 3：蓝色)", 0, 0x7, 1, 5, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    //    pNode = fs_Config_node_string_add(pConfig, parent, "Linerate", "目标跟踪框显示部分比例", "目标跟踪框显示部分比例(1到50)", 0, 0x7, 1, 5, 1);
    //    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");
}

/*叠加配置项*/
void overly_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "overlyConfig", "雷达引导跟踪-字符叠加", "雷达引导跟踪-字符叠加", 0, 0x7);
    {
        void *const condition_group = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group, 1, "moduleMask", FsConfig_Condition_orAnd, "1");
    }
    {
        void *const condition_group1 = fs_Config_condition_group_add(pConfig, parent);
        fs_Config_condition_add_static(pConfig, condition_group1, 1, "moduleMask", FsConfig_Condition_orAnd, "4");
    }
    void *pNode = NULL;
    pNode = fs_Config_node_string_add(pConfig, parent, "OverlyLine", "是否叠加跟踪框", "是否叠加跟踪框(0：否，1：是)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "OverlyTracknum", "是否叠加目标编号", "是否叠加目标编号(0：否，1：是)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "Linewidth", "目标跟踪框的边框粗细", "目标跟踪框的边框粗细", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "4", "4", "4");
    pNode = fs_Config_node_string_add(pConfig, parent, "Linecolor", "目标跟踪框的颜色", "目标跟踪框的颜色(1：红色，2：绿色, 3：蓝色)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "Linerate", "目标跟踪框显示部分比例", "目标跟踪框显示部分比例(1到50)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "15", "15", "15");

    pNode = fs_Config_node_string_add(pConfig, parent, "ObjNum_width", "目标编号字符宽度", "目标编号字符宽度", 0, 0x7, 1, 8, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "36", "36", "36");
    pNode = fs_Config_node_string_add(pConfig, parent, "ObjNum_height", "目标编号字符高度", "目标编号字符高度", 0, 0x7, 1, 8, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "36", "36", "36");

    pNode = fs_Config_node_string_add(pConfig, parent, "ObjNum_color", "目标编号叠加颜色,RGB", "目标编号叠加颜色,RGB,16进制", 0, 0x7, 1, 9, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0xFF0000", "红色", "红色");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "0x00FF00", "绿色", "绿色");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "0x0000FF", "蓝色", "蓝色");

    pNode = fs_Config_node_string_add(pConfig, parent, "ObjNum_pos", "目标编号字符位置", "目标编号字符位置(1:跟踪框顶部，2：跟踪框中心，3：跟踪框底部)", 0, 0x7, 1, 8, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
}

void videoDect_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "videodectConfig", "雷达引导跟踪-视频自动检测跟踪", "雷达引导跟踪-视频自动检测跟踪", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 1, "moduleMask", FsConfig_Condition_orAnd, "4");

    void *pNode = NULL;
    pNode = fs_Config_node_integer_add(pConfig, parent, "video_valid", "是否支持视频自动检测", "是否支持视频自动检测", FsConfig_nodeShowType_default, 0, 0x7, 0, 128, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "不支持", "不支持");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "支持", "支持");
    pNode = fs_Config_node_integer_add(pConfig, parent, "video_upload", "是否上传数据到控制器", "是否上传数据到控制器", FsConfig_nodeShowType_default, 0, 0x7, 0, 128, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "支持", "支持");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "不支持", "不支持");

    pNode = fs_Config_node_string_add(pConfig, parent, "video_camtype", "相机类型", "相机类型(0：枪机 1：球机)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");

    pNode = fs_Config_node_string_add(pConfig, parent, "video_devnumber", "设备编号", "设备编号(3位，第1位代表区域，第2-3位代表相机编号)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "101", "101", "101");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_valid_gray", "视频自动检测有效阈值", "视频自动检测有效阈值(画面的平均灰度大于此阈值才有效)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_valid_time", "视频自动检测屏蔽时段", "视频自动检测屏蔽时段(24小时制:开始屏蔽时间-结束屏蔽时间)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "6", "0-0", "0-0");

    pNode = fs_Config_node_integer_add(pConfig, parent, "video_cammove_valid", "是否支持预置位检测", "是否支持预置位检测", FsConfig_nodeShowType_default, 0, 0x7, 0, 128, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "支持", "支持");
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0, "不支持", "不支持");

    pNode = fs_Config_node_string_add(pConfig, parent, "video_keepTime", "目标存在最短时间(秒)", "目标存在最短时间(秒)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "3", "3", "3");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_moveth", "背景差阈值", "背景差阈值", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "20", "20", "20");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_min_objsize", "目标输出最小尺寸", "目标输出最小尺寸(长-宽)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "40-40", "40-40", "40-40");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_max_objsize", "目标输出最大尺寸", "目标输出最大尺寸(长-宽)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "80-80", "80-80", "80-80");
    pNode = fs_Config_node_string_add(pConfig, parent, "vibe_obj_keepTime", "目标最佳尺寸保持时间(秒)", "目标最佳尺寸保持时间(秒)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "4", "4", "4");
    pNode = fs_Config_node_string_add(pConfig, parent, "vibe_obj_locked_moveMinDis", "目标锁定最小距离", "目标锁定最小距离(像素点,0不做判断)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "vibe_obj_moveMinDis", "目标移动最短距离", "目标移动最短距离(像素点)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "20", "20", "20");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_cloud_size", "过滤云阴影大小", "过滤云阴影大小(长-宽)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "80-80", "80-80", "80-80");

    pNode = fs_Config_node_integer_add(pConfig, parent, "video_floater_flag", "漂浮物检测", "漂浮物检测(1:开启,0:关闭)", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1, "1", "1");

    pNode = fs_Config_node_integer_add(pConfig, parent, "watergrass_factor", "水草特征因子", "水草特征因子(值越小，越灵敏; 0:不做此判断)", FsConfig_nodeShowType_default, 0, 0x7, 0, 10000, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 900, "900", "900");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "video_floater_flag", FsConfig_Condition_equal, "1");

    pNode = fs_Config_node_string_add(pConfig, parent, "re_light_factor", "水面反光特征因子", "水面反光特征因子(值越小，越灵敏; 0-0:不做此判断)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0-0", "0-0", "0-0");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "video_floater_flag", FsConfig_Condition_equal, "1");


    pNode = fs_Config_node_integer_add(pConfig, parent, "floater_group_factor", "群体特征因子", "群体特征因子(多个目标，运动轨迹一致; 0:不做此判断)", FsConfig_nodeShowType_default, 0, 0x7, 0, 10000, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 2, "2", "2");
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "video_floater_flag", FsConfig_Condition_equal, "1");

    pNode = fs_Config_node_integer_add(pConfig, parent, "time_interval", "前后两次告警时间间隔", "前后两次告警时间间隔(0:不做此判断,单位为秒)", FsConfig_nodeShowType_default, 0, 0x7, 0, 10000, 1);
    fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0", "0");


    pNode = fs_Config_node_string_add(pConfig, parent, "video_cammove_time", "相机自动回预置超时", "相机自动回预置超时(单位:秒)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "600", "600", "600");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_polltype", "是否支持轮循", "是否支持轮循(0：不支持 1：支持)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_polltime", "轮循间隔时间", "轮循间隔时间(单位：秒)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "30", "30", "30");
    pNode = fs_Config_node_string_add(pConfig, parent, "video_pollcount", "轮循点个数", "轮循点个数", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "1", "1", "1");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_ptz_1", "轮循点1", "轮循点1(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "132-1-12", "132-1-12", "132-1-12");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_area_1", "轮循点检测范围1", "轮循点检测范围1(l-r-t-b)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12-100-12-100", "12-100-12-100", "12-100-12-100");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_objsize_1", "轮循点目标最小尺寸1", "轮循点目标最小尺寸1(w-h)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "10-10", "10-10", "10-10");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_ptz_2", "轮循点2", "轮循点2(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_area_2", "轮循点检测范围2", "轮循点检测范围2(l-r-t-b)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_objsize_2", "轮循点目标最小尺寸2", "轮循点目标最小尺寸2(w-h)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_ptz_3", "轮循点3", "轮循点3(p-t-z)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_area_3", "轮循点检测范围3", "轮循点检测范围3(l-r-t-b)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_objsize_3", "轮循点目标最小尺寸3", "轮循点目标最小尺寸3(w-h)", 0, 0x7, 1, 128, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0", "0", "0");
}

/*靠岸事件检测配置项*/
void incident_createConfig(FsConfig * const pConfig, void * parent) {

    parent = fs_Config_node_node_add(pConfig, parent, "incidentConfig", "靠岸事件检测", "靠岸事件检测", 0, 0x7);
    fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, parent), 4, "moduleMask", FsConfig_Condition_orAnd, "1");

    void *pNode = NULL;
    //一个预置位ptz检测，对应一个靠岸ptz；预置位ptz下的检测区域，靠岸ptz的岸线区域
    //目标的最小尺寸
    pNode = fs_Config_node_string_add(pConfig, parent, "search_ptz", "视频检测预置位1(p-t-z)", "视频检测预置位1(p-t-z)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "132-55-1200", "132-55-1200", "132-55-1200");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_area", "视频检测范围1(l-r-t-b)", "视频检测范围1(l-r-t-b)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12-100-12-100", "12-100-12-100", "12-100-12-100");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_objsize", "视频检测目标尺寸1(w-h)", "视频检测目标尺寸1(w-h)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12-12", "12-12", "12-12");
    pNode = fs_Config_node_string_add(pConfig, parent, "ashore_ptz", "靠岸预置位1(p-t-z)", "靠岸预置位1(p-t-z)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "132-55-1200", "132-55-1200", "132-55-1200");

    pNode = fs_Config_node_string_add(pConfig, parent, "search_ptz1", "视频检测预置位2(p-t-z)", "视频检测预置位2(p-t-z)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "132-55-1200", "132-55-1200", "132-55-1200");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_area1", "视频检测范围2(l-r-t-b)", "视频检测范围2(l-r-t-b)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12-100-12-100", "12-100-12-100", "12-100-12-100");
    pNode = fs_Config_node_string_add(pConfig, parent, "search_objsize1", "视频检测目标尺寸2(w-h)", "视频检测目标尺寸2(w-h)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "12-12", "12-12", "12-12");
    pNode = fs_Config_node_string_add(pConfig, parent, "ashore_ptz1", "靠岸预置位2(p-t-z)", "靠岸预置位2(p-t-z)", 0, 0x7, 1, 5, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "132-55-1200", "132-55-1200", "132-55-1200");

}

//外部调用

void vtrack_createConfig(FsConfig * const pConfig, void * parent) {
    search_createConfig(pConfig, parent); //目标定位参数
    track_createConfig(pConfig, parent); //跟踪参数
    overly_createConfig(pConfig, parent); //视频叠加参数
    holder_createConfig(pConfig, parent); //云台联动参数

    videoDect_createConfig(pConfig, parent); //相机预置位自动视频检测参数

    debug_createConfig(pConfig, parent); //调试参数
    // incident_createConfig(pConfig, parent);   //靠岸检测参数
}

/* 检查pVtrack模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
int vtrack_check_channel_changed(struct Vtrack * const pVtrack, /* 通道编号,从0开始 */const unsigned int index
        , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
        , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]) {
#define __check_channel_changed_Server Vtrack
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

void debug_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {
    //调试参数配置
    char log[256];
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig savelog_flag", NULL);
        rst->ro.m_config_debug.savelog_flag = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig savelog_path", NULL);
        sprintf(rst->ro.m_config_debug.savelog_path, "%s", pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig savepic_flag", NULL);
        rst->ro.m_config_debug.savepic_flag = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig savepic_path", NULL);
        sprintf(rst->ro.m_config_debug.savepic_path, "%s", pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig opendebug", NULL);
        rst->ro.m_config_debug.m_opendebug = atoi(pstr);
    }

    rst->ro.m_config_debug.m_change_z = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig limit_z", 0, NULL);
    rst->ro.m_config_debug.m_change_t = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig limit_t", 0, NULL);
    rst->ro.m_config_debug.m_change_p = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "debugConfig limit_p", 0, NULL);


    //    pstr = fs_Config_node_string_get_first(pConfig, parent, "debugConfig change_z");
    //    rst->rw.m_debug.m_change_z = atof(pstr), fsFree(pstr);
    //    pstr = fs_Config_node_string_get_first(pConfig, parent, "debugConfig change_p");
    //    rst->rw.m_debug.m_change_p = atof(pstr), fsFree(pstr);
    //    pstr = fs_Config_node_string_get_first(pConfig, parent, "debugConfig change_t");
    //    rst->rw.m_debug.m_change_t = atof(pstr), fsFree(pstr);


    rst->ro.m_compressRate = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "recordConfig jpg_compressRate", 85, NULL);

    FsPrintf2(1, "**************************DebugConfig*********************************\n");
    FsPrintf2(1, "save_flag:%d save_path:%s savepic_flag:%d  savepic_path:%s m_compressRate:%d m_change_z:%d \n", rst->ro.m_config_debug.savelog_flag, rst->ro.m_config_debug.savelog_path,
            rst->ro.m_config_debug.savepic_flag, rst->ro.m_config_debug.savepic_path, rst->ro.m_compressRate, rst->ro.m_config_debug.m_change_z);
    fflush(stdout);
    sprintf(log, "save_flag:%d save_path:%s savepic_flag:%d  savepic_path:%s \n", rst->ro.m_config_debug.savelog_flag, rst->ro.m_config_debug.savelog_path,
            rst->ro.m_config_debug.savepic_flag, rst->ro.m_config_debug.savepic_path);
    MakeLogRecord(log, rst);
    FsPrintf2(1, "**************************DebugConfig*********************************\n");
}

void search_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {
    char log[256];
    //目标定位参数
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig trackFrameInterval", NULL);
        rst->ro.m_trackframeInterval = atoi(pstr);
    }
    //处理抽点
    rst->ro.zoomTimes = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "searchConfig zoomTimes", NULL);

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig searchWidth", NULL);
        rst->ro.m_config_search.searchWidth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig searchHeight", NULL);
        rst->ro.m_config_search.searchHeight = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig modWidth", NULL);
        rst->ro.m_config_search.modWidth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig modHeight", NULL);
        rst->ro.m_config_search.modHeight = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig modscale", NULL);
        rst->ro.m_config_search.scale = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig Moveth", NULL);
        rst->ro.m_config_search.Moveth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig stopFrameCount", NULL);
        rst->ro.m_config_search.stopFrameCount = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig isAssistCheck", NULL);
        rst->ro.m_config_search.isAssistCheck = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig isAssistFindbottom", NULL);
        rst->ro.m_config_search.isAssistfindbottom = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig AssistParam1", NULL);
        rst->ro.m_config_search.assistparam1 = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig AssistParam2", NULL);
        rst->ro.m_config_search.assistparam2 = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "searchConfig AssistParam3", NULL);
        rst->ro.m_config_search.assistparam3 = atoi(pstr);
    }

    FsPrintf2(1, "**************************Assist Check*********************************\n");
    fflush(stdout);

    FsPrintf2(1, "m_trackframeInterval:%d zoomTimes:%d \n", rst->ro.m_trackframeInterval, rst->ro.zoomTimes);
    fflush(stdout);

    FsPrintf2(1, "isAssistCheck:%d isAssistfindbottom:%d assistparam1:%d assistparam2:%d assistparam3:%d\n", rst->ro.m_config_search.isAssistCheck,
            rst->ro.m_config_search.isAssistfindbottom, rst->ro.m_config_search.assistparam1, rst->ro.m_config_search.assistparam2, rst->ro.m_config_search.assistparam3);
    fflush(stdout);
    sprintf(log, "isAssistCheck:%d isAssistfindbottom:%d assistparam1:%d assistparam2:%d assistparam3:%d\n", rst->ro.m_config_search.isAssistCheck,
            rst->ro.m_config_search.isAssistfindbottom, rst->ro.m_config_search.assistparam1, rst->ro.m_config_search.assistparam2, rst->ro.m_config_search.assistparam3);
    MakeLogRecord(log, rst);
    FsPrintf2(1, "**************************Assist Check*********************************\n");
    fflush(stdout);




    FsPrintf2(1, "**************************SearchConfig*********************************\n");
    fflush(stdout);
    FsPrintf2(1, "searchWidth:%d searchHeight:%d modWidth:%d modHeight:%d modscale:%d Moveth:%d stopFrameCount:%d \n", rst->ro.m_config_search.searchWidth,
            rst->ro.m_config_search.searchHeight, rst->ro.m_config_search.modWidth, rst->ro.m_config_search.modHeight, rst->ro.m_config_search.scale,
            rst->ro.m_config_search.Moveth, rst->ro.m_config_search.stopFrameCount);
    fflush(stdout);
    sprintf(log, "searchWidth:%d searchHeight:%d modWidth:%d modHeight:%d modscale:%d Moveth:%d stopFrameCount:%d \n", rst->ro.m_config_search.searchWidth,
            rst->ro.m_config_search.searchHeight, rst->ro.m_config_search.modWidth, rst->ro.m_config_search.modHeight, rst->ro.m_config_search.scale,
            rst->ro.m_config_search.Moveth, rst->ro.m_config_search.stopFrameCount);
    MakeLogRecord(log, rst);
    FsPrintf2(1, "**************************SearchConfig*********************************\n");
    fflush(stdout);

}

void track_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {
    char log[256];
    //跟踪参数
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackOverFlag1", NULL);
        rst->ro.m_config_track.m_trackOverFlag1 = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackOverFlag2", NULL);
        rst->ro.m_config_track.m_trackOverFlag2 = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackOverFlag3", NULL);
        rst->ro.m_config_track.m_trackOverTime = atof(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackOverFlag4", NULL);
        rst->ro.m_config_track.m_trackOverFlag3 = atof(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackOverFlag5", NULL);
        rst->ro.m_config_track.m_trackOverFlag4 = atof(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackOverHandle1", NULL);
        rst->ro.m_config_track.m_trackOverPTZ = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackAgain", NULL);
        rst->ro.m_config_track.m_trackAgainTimes = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig track_Delay", NULL);
        rst->ro.m_config_track.m_trackdelay = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackWidth", NULL);
        rst->ro.m_config_track.m_trackWidth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig trackHeight", NULL);
        rst->ro.m_config_track.m_trackHeight = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig track_SearchWidth", NULL);
        rst->ro.m_config_track.m_trackSearchWidth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "trackConfig track_SearchHeight", NULL);
        rst->ro.m_config_track.m_trackSearchHeight = atoi(pstr);
    }

    FsPrintf2(1, "**************************TrackConfig*********************************\n");
    fflush(stdout);
    FsPrintf2(1, "trackWidth:%d trackHeight:%d track_SearchWidth:%d track_SearchHeight:%d trackOverFlag1:%d trackOverFlag2:%d m_trackOverFlag3:%d m_trackOverFlag4:%d\n", rst->ro.m_config_track.m_trackWidth,
            rst->ro.m_config_track.m_trackHeight, rst->ro.m_config_track.m_trackSearchWidth, rst->ro.m_config_track.m_trackSearchHeight, rst->ro.m_config_track.m_trackOverFlag1, rst->ro.m_config_track.m_trackOverFlag2, rst->ro.m_config_track.m_trackOverFlag3, rst->ro.m_config_track.m_trackOverFlag4);
    fflush(stdout);
    sprintf(log, " trackWidth:%d trackHeight:%d track_SearchWidth:%d track_SearchHeight:%d trackOverFlag1:%d trackOverFlag2:%d \n", rst->ro.m_config_track.m_trackWidth,
            rst->ro.m_config_track.m_trackHeight, rst->ro.m_config_track.m_trackSearchWidth, rst->ro.m_config_track.m_trackSearchHeight, rst->ro.m_config_track.m_trackOverFlag1, rst->ro.m_config_track.m_trackOverFlag2);
    MakeLogRecord(log, rst);
    FsPrintf2(1, "**************************TrackConfig*********************************\n");
    fflush(stdout);
}

void overly_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {
    char log[256];
    //视频叠加参数：
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig OverlyLine", NULL);
        rst->ro.m_config_overly.m_overly_line = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig OverlyTracknum", NULL);
        rst->ro.m_config_overly.m_overly_tracknum = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig Linewidth", NULL);
        rst->ro.m_config_overly.m_overly_linewidth = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig Linecolor", NULL);
        rst->ro.m_config_overly.m_overly_linecolor = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig Linerate", NULL);
        rst->ro.m_config_overly.m_overly_linerate = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig ObjNum_width", NULL);
        rst->ro.m_config_overly.m_overly_tracknum_width = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig ObjNum_height", NULL);
        rst->ro.m_config_overly.m_overly_tracknum_height = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig ObjNum_pos", NULL);
        rst->ro.m_config_overly.m_overly_tracknum_pos = atoi(pstr);
    }
    unsigned int watermarking_color;
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "overlyConfig ObjNum_color", NULL);
        watermarking_color = strtol(pstr, NULL, 16);
        FsPrintf(1, "%#x-%s\n", watermarking_color, pstr);
    }

    /* 文字叠加 */
    if (rst->ro.m_config_overly.m_overly_tracknum == 1) {
        FsPrintf2(1, "----fs_TypeFace_new__IO: m_overly_tracknum_width:%d m_overly_tracknum_height:%d----\n", rst->ro.m_config_overly.m_overly_tracknum_width, rst->ro.m_config_overly.m_overly_tracknum_height);
        fflush(stdout);
        rst->ro.__pTypeFace = fs_TypeFace_new__IO(NULL, rst->ro.m_config_overly.m_overly_tracknum_width, rst->ro.m_config_overly.m_overly_tracknum_height, 1, 0);
        FsPrintf2(1, "----fs_TypeFace_new__IO:__pTypeFace:%p----\n", (void *) rst->ro.__pTypeFace);
        fflush(stdout);

    } else {
        rst->ro.__pTypeFace = NULL;
    }

    rst->ro.m_config_overly.m_overly_tracknum_color = (((unsigned int) (0.257 * (watermarking_color >> 16) + 0.504 * ((watermarking_color >> 8)&0xFF) + 0.098 * (watermarking_color & 0xFF) + 16)) << 16)+
            (((unsigned int) (-0.148 * (watermarking_color >> 16) - 0.291 * ((watermarking_color >> 8)&0xFF) + 0.439 * (watermarking_color & 0xFF) + 128)) << 8) +
            0.439 * (watermarking_color >> 16) - 0.368 * ((watermarking_color >> 8)&0xFF) - 0.071 * (watermarking_color & 0xFF) + 128;

    FsPrintf2(1, "**************************OverlyConfig*********************************\n");
    fflush(stdout);
    FsPrintf2(1, "(m_overly_line:%d m_overly_tracknum:%d) - (linewidth:%d linecolor:%d linerate:%d) - tracknum_width:%d tracknum_height:%d tracknum_pos:%d tracknum_color:%d \n", rst->ro.m_config_overly.m_overly_line,
            rst->ro.m_config_overly.m_overly_tracknum, rst->ro.m_config_overly.m_overly_linewidth,
            rst->ro.m_config_overly.m_overly_linecolor, rst->ro.m_config_overly.m_overly_linerate, rst->ro.m_config_overly.m_overly_tracknum_width, rst->ro.m_config_overly.m_overly_tracknum_height,
            rst->ro.m_config_overly.m_overly_tracknum_pos, rst->ro.m_config_overly.m_overly_tracknum_color);
    fflush(stdout);
    sprintf(log, "(m_overly_line:%d m_overly_tracknum:%d) - (linewidth:%d linecolor:%d linerate:%d) - tracknum_width:%d tracknum_height:%d tracknum_pos:%d tracknum_color:%d \n", rst->ro.m_config_overly.m_overly_line,
            rst->ro.m_config_overly.m_overly_tracknum, rst->ro.m_config_overly.m_overly_linewidth,
            rst->ro.m_config_overly.m_overly_linecolor, rst->ro.m_config_overly.m_overly_linerate, rst->ro.m_config_overly.m_overly_tracknum_width, rst->ro.m_config_overly.m_overly_tracknum_height,
            rst->ro.m_config_overly.m_overly_tracknum_pos, rst->ro.m_config_overly.m_overly_tracknum_color);
    MakeLogRecord(log, rst);
    FsPrintf2(1, "**************************OverlyConfig*********************************\n");
    fflush(stdout);
}

void holder_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {
    char log[256];
    //云台相关配置
    /*
    pstr = fs_Config_node_string_get_first(pConfig, parent, "holderConfig holder_ip");
    sprintf(rst->ro.m_config_holder.m_holder_ip, "%s", pstr);
    sprintf(rst->rw.m_hikiCamera.m_szCameraIP, "%s", pstr);
    fsFree(pstr);
    pstr = fs_Config_node_string_get_first(pConfig, parent, "holderConfig holder_port");
    rst->ro.m_config_holder.m_holder_port = atoi(pstr);
    rst->rw.m_hikiCamera.m_nCameraPort = atoi(pstr);
    fsFree(pstr);
    pstr = fs_Config_node_string_get_first(pConfig, parent, "holderConfig holder_port1");
    rst->ro.m_config_holder.m_holder_port_1 = atoi(pstr);
    fsFree(pstr);
     */
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig holder_speed", NULL);
        rst->ro.m_config_holder.m_holder_speed = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig holder_keeptouchFrames", NULL);
        rst->ro.m_config_holder.m_holder_keeptouchFrames = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig preset_p", NULL);
        rst->ro.m_config_holder.m_p = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig preset_t", NULL);
        rst->ro.m_config_holder.m_t = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig preset_z", NULL);
        rst->ro.m_config_holder.m_z = atoi(pstr);
    }

    /*
    pstr = fs_Config_node_string_get_first(pConfig, parent, "holderConfig holder_type");
    rst->ro.m_config_holder.m_holder_type = atoi(pstr), fsFree(pstr);
    pstr = fs_Config_node_string_get_first(pConfig, parent, "holderConfig holder_name");
    sprintf(rst->ro.m_config_holder.m_holder_name, "%s", pstr);
    sprintf(rst->rw.m_hikiCamera.m_szCameraUid, "%s", pstr);
    fsFree(pstr);
    pstr = fs_Config_node_string_get_first(pConfig, parent, "holderConfig holder_pwd");
    sprintf(rst->ro.m_config_holder.m_holder_pwd, "%s", pstr);
    sprintf(rst->rw.m_hikiCamera.m_szCameraPwd, "%s", pstr);
    fsFree(pstr);
     */

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig holder_forceStopX", NULL);
        rst->ro.m_config_holder.m_holder_forceStopX = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig holder_forceStopX_1", NULL);
        rst->ro.m_config_holder.m_holder_forceStopX_1 = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig holder_forceStopY", NULL);
        rst->ro.m_config_holder.m_holder_forceStopY = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig holder_forceStopY_1", NULL);
        rst->ro.m_config_holder.m_holder_forceStopY_1 = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig ConditionDir", NULL);
        rst->ro.m_config_holder.m_cond_dir = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig ConditionStart", NULL);
        rst->ro.m_config_holder.m_cond_start = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "holderConfig ConditionStop", NULL);
        rst->ro.m_config_holder.m_cond_stop = atoi(pstr);
    }

    FsPrintf2(1, "**************************HolderConfig*********************************\n");
    fflush(stdout);
    FsPrintf2(1, "m_cond_dir:%d m_cond_start:%d m_cond_stop:%d p:%d t:%d z:%d \n", rst->ro.m_config_holder.m_cond_dir,
            rst->ro.m_config_holder.m_cond_start, rst->ro.m_config_holder.m_cond_stop, rst->ro.m_config_holder.m_p, rst->ro.m_config_holder.m_t, rst->ro.m_config_holder.m_z);
    fflush(stdout);
    sprintf(log, "m_cond_dir:%d m_cond_start:%d m_cond_stop:%d p:%d t:%d z:%d \n", rst->ro.m_config_holder.m_cond_dir,
            rst->ro.m_config_holder.m_cond_start, rst->ro.m_config_holder.m_cond_stop, rst->ro.m_config_holder.m_p, rst->ro.m_config_holder.m_t, rst->ro.m_config_holder.m_z);
    MakeLogRecord(log, rst);

    FsPrintf2(1, "m_szCameraUid:%s m_szCameraPwd:%s ip:%s port:%d \n",
            rst->rw.m_hikiCamera.m_szCameraUid, rst->rw.m_hikiCamera.m_szCameraPwd, rst->rw.m_hikiCamera.m_szCameraIP, rst->rw.m_hikiCamera.m_nCameraPort);
    fflush(stdout);
    sprintf(log, "m_szCameraUid:%s m_szCameraPwd:%s ip:%s port:%d \n",
            rst->rw.m_hikiCamera.m_szCameraUid, rst->rw.m_hikiCamera.m_szCameraPwd, rst->rw.m_hikiCamera.m_szCameraIP, rst->rw.m_hikiCamera.m_nCameraPort);
    MakeLogRecord(log, rst);
    FsPrintf2(1, "**************************HolderConfig*********************************\n");
    fflush(stdout);
}

void videodect_getConfig(Vtrack_item* rst, FsConfig *pConfig, const void *const vsysChannel0, const void *const vsysChannel) {


    char log[256];

    rst->ro.m_config_videoDect.m_valid = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_valid", NULL);
    //if (rst->ro.m_config_videoDect.m_valid == 2) rst->ro.m_config_videoDect.m_valid = 0;
    rst->ro.m_config_videoDect.m_upload = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_upload", NULL);
    //if (rst->ro.m_config_videoDect.m_upload == 2) rst->ro.m_config_videoDect.m_upload = 0;

    //    pstr = fs_Config_node_string_get_first(pConfig, parent, "videodectConfig video_valid");
    //    rst->ro.m_config_videoDect.m_valid = atoi(pstr), fsFree(pstr);
    //    pstr = fs_Config_node_string_get_first(pConfig, parent, "videodectConfig video_upload");
    //    rst->ro.m_config_videoDect.m_upload = atoi(pstr), fsFree(pstr);
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_camtype", NULL);
        rst->ro.m_config_videoDect.m_camtpye = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_polltype", NULL);
        rst->ro.m_config_videoDect.m_polltype = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_polltime", NULL);
        rst->ro.m_config_videoDect.m_polltime = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_pollcount", NULL);
        rst->ro.m_config_videoDect.m_ptz_count = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_devnumber", NULL);
        rst->ro.m_config_videoDect.m_devnumber = atoi(pstr);
    }
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_valid_gray", NULL);
        rst->ro.m_config_videoDect.m_avegray = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_cammove_time", NULL);
        rst->ro.m_config_videoDect.m_camera_movetime = atoi(pstr);
    }

    rst->ro.m_config_videoDect.m_floater_valid = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_floater_flag", NULL);
    rst->ro.m_config_videoDect.m_watergrass_factor = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig watergrass_factor", NULL);
    rst->ro.m_config_videoDect.m_floater_group_factor = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig floater_group_factor", NULL);

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_moveth", NULL);
        rst->ro.m_config_videoDect.m_moveth = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_keepTime", NULL);
        rst->ro.m_config_videoDect.m_keepTime = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig vibe_obj_keepTime", NULL);
        rst->ro.m_config_videoDect.vibe_obj_keepTime = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig vibe_obj_moveMinDis", NULL);
        rst->ro.m_config_videoDect.vibe_obj_moveMinDis = atoi(pstr);
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig vibe_obj_locked_moveMinDis", NULL);
        rst->ro.m_config_videoDect.vibe_obj_locked_moveMinDis = atoi(pstr);
    }


    rst->ro.m_config_videoDect.m_time_interval = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig time_interval", NULL);

    //目标输出大小：
    char temp[64];
    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_min_objsize", NULL);
        sprintf(temp, "%s", pstr);
        if (strlen(temp) != 0) {
            sscanf(temp, "%d-%d", &rst->ro.m_config_videoDect.m_objminWidth, &rst->ro.m_config_videoDect.m_objminHeight);
        }
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_max_objsize", NULL);
        sprintf(temp, "%s", pstr);
        if (strlen(temp) != 0) {
            sscanf(temp, "%d-%d", &rst->ro.m_config_videoDect.m_objmaxWidth, &rst->ro.m_config_videoDect.m_objmaxHeight);
        }
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_cloud_size", NULL);
        sprintf(temp, "%s", pstr);
        if (strlen(temp) != 0) {
            sscanf(temp, "%d-%d", &rst->ro.m_config_videoDect.m_cloud_minWidth, &rst->ro.m_config_videoDect.m_cloud_minHeight);
        }
    }

    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig re_light_factor", NULL);
        sprintf(temp, "%s", pstr);
        if (strlen(temp) != 0) {
            sscanf(temp, "%d-%d", &rst->ro.m_config_videoDect.m_re_light_strength, &rst->ro.m_config_videoDect.m_re_light_rate);
        }
    }

    rst->ro.m_config_videoDect.m_camera_movevalid = fs_Config_node_integer_get_mask(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_cammove_valid", NULL);
    //if (rst->ro.m_config_videoDect.m_camera_movevalid == 2) rst->ro.m_config_videoDect.m_camera_movevalid = 0;


    {
        const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, "videodectConfig video_valid_time", NULL);
        sprintf(temp, "%s", pstr);

        if (strlen(temp) != 0) {
            sscanf(temp, "%d-%d", &rst->ro.m_config_videoDect.m_close_starttime, &rst->ro.m_config_videoDect.m_close_endtime);
        }
    }

    FsPrintf2(1, "**************************videodectConfig*********************************\n");
    fflush(stdout);

    FsPrintf2(1, "video_valid:%d m_watergrass_factor:%d re_light_strength:%d m_re_light_rate:%d m_time_interval:%d \n", rst->ro.m_config_videoDect.m_floater_valid,
            rst->ro.m_config_videoDect.m_watergrass_factor, rst->ro.m_config_videoDect.m_re_light_strength, rst->ro.m_config_videoDect.m_re_light_rate,
            rst->ro.m_config_videoDect.m_time_interval);
    fflush(stdout);

    FsPrintf2(1, "m_floater_valid:%d m_upload:%d video_camtype:%d video_polltype:%d video_polltime:%d video_pollcount:%d  m_devnumber:%d avegray:%d close_starttime:%d close_endtime:%d video_cammove_valid :%d video_cammove_time:%d m_keepTime:%d vibe_obj_keepTime:%d\n",
            rst->ro.m_config_videoDect.m_valid, rst->ro.m_config_videoDect.m_upload,
            rst->ro.m_config_videoDect.m_camtpye, rst->ro.m_config_videoDect.m_polltype, rst->ro.m_config_videoDect.m_polltime,
            rst->ro.m_config_videoDect.m_ptz_count, rst->ro.m_config_videoDect.m_devnumber, rst->ro.m_config_videoDect.m_avegray,
            rst->ro.m_config_videoDect.m_close_starttime, rst->ro.m_config_videoDect.m_close_endtime, rst->ro.m_config_videoDect.m_camera_movevalid,
            rst->ro.m_config_videoDect.m_camera_movetime, rst->ro.m_config_videoDect.m_keepTime, rst->ro.m_config_videoDect.vibe_obj_keepTime);
    fflush(stdout);

    int i = 0;
    int errorno = 0;
    for (i = 0; i < rst->ro.m_config_videoDect.m_ptz_count; i++) {
        char nodename[64];
        char message[215];
        sprintf(nodename, "videodectConfig search_ptz_%d", i + 1);
        {
            const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, nodename, NULL);

            sprintf(message, "%s", pstr);

            if (strlen(message) != 0) {
                sscanf(message, "%d-%d-%d", &rst->ro.m_config_videoDect.m_pos[i].search_p,
                        &rst->ro.m_config_videoDect.m_pos[i].search_t,
                        &rst->ro.m_config_videoDect.m_pos[i].search_z);
            } else {
                errorno = 1;
            }
        }

        if (rst->ro.m_config_videoDect.m_pos[i].search_p < 0 || rst->ro.m_config_videoDect.m_pos[i].search_t < 0 || rst->ro.m_config_videoDect.m_pos[i].search_z < 0) {
            errorno = 1;
        }

        sprintf(nodename, "videodectConfig search_area_%d", i + 1);
        {
            const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
            sprintf(message, "%s", pstr);
            if (strlen(message) != 0) {
                sscanf(message, "%d-%d-%d-%d", &rst->ro.m_config_videoDect.m_pos[i].left, &rst->ro.m_config_videoDect.m_pos[i].right,
                        &rst->ro.m_config_videoDect.m_pos[i].top, &rst->ro.m_config_videoDect.m_pos[i].bottom);
            } else {
                errorno = 1;
            }
        }

        sprintf(nodename, "videodectConfig search_objsize_%d", i + 1);
        {
            const char *const pstr = fs_Config_node_string_get_first(pConfig, vsysChannel0, vsysChannel, nodename, NULL);
            sprintf(message, "%s", pstr);
            if (strlen(message) != 0) {
                sscanf(message, "%d-%d", &rst->ro.m_config_videoDect.m_pos[i].modw, &rst->ro.m_config_videoDect.m_pos[i].modh);
            } else {
                errorno = 1;
            }
        }

        if (rst->ro.m_config_videoDect.m_pos[i].left < 0 || rst->ro.m_config_videoDect.m_pos[i].right < 0 ||
                rst->ro.m_config_videoDect.m_pos[i].top < 0 || rst->ro.m_config_videoDect.m_pos[i].bottom < 0) {
            errorno = 1;
        }

        FsPrintf2(1, "search_ptz_%d:(%d %d %d) search_area_%d:(%d %d %d %d) objsize:(%d %d)\n", i, rst->ro.m_config_videoDect.m_pos[i].search_p,
                rst->ro.m_config_videoDect.m_pos[i].search_t, rst->ro.m_config_videoDect.m_pos[i].search_z, i, rst->ro.m_config_videoDect.m_pos[i].left,
                rst->ro.m_config_videoDect.m_pos[i].right, rst->ro.m_config_videoDect.m_pos[i].top, rst->ro.m_config_videoDect.m_pos[i].bottom,
                rst->ro.m_config_videoDect.m_pos[i].modw, rst->ro.m_config_videoDect.m_pos[i].modh);
        fflush(stdout);

        if (errorno == 1) {
            break;
        }
    }

    //如果有参数有问题
    if (errorno == 1) {
        printf("(%s %d) -- the param is error \n", __FUNCTION__, __LINE__);
        rst->ro.m_config_videoDect.m_ptz_count = 0;
    }

    FsPrintf2(1, "**************************videodectConfig*********************************\n");
    fflush(stdout);
}

/* 
 * 创建一个新的vtrack对象;
 * 返回vtrack指针.
 */
struct Vtrack * vtrack_new__IO(/* 线程名 */const char threadName[]
        , /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus
        , /* 数据保存的目录,推荐以'/'结尾 */ const char dir[], /* 配置管理对象 */ void *pConfigManager
        , /* 在有输出数据时向哪个开方分组写入信号 */FsGroupSqrtPthreadSafety * const pGroupSqrtOut) {
    unsigned int len = strlen(dir);
    if (dir[len - 1] == '/')len++;
    else len += 2;
    len += strlen(threadName) + 1;
    struct Vtrack* rst = (struct Vtrack*) fsMalloc(sizeof (struct Vtrack) +len);
    memset(rst, 0, sizeof (struct Vtrack));
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
void vtrack_delete__OI(struct Vtrack * pVtrack) {
    /* 通道链表 */
    fs_ObjectList_delete_unsafe__OI(pVtrack->ro.__itemList_);
    /* 多线程锁 */
    pthread_mutex_destroy(&pVtrack->p.__mutex);

    if (pVtrack) fsFree(pVtrack);

}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void vtrack_startThread(struct Vtrack* pVtrack,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    if (pVtrack->rw.runStatus == 0 && pVtrack->ro.stid == 0) {
        pVtrack->rw.runStatus = 1;
        pVtrack->rw.thisNice = thisNice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pVtrack->ro._threadName,
                (unsigned long) pVtrack->ro._pMonitor,
                pVtrack->ro._parentTid,
                (unsigned long) pVtrack->ro._externRunStatus);
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
        if (pthread_create(&pVtrack->ro.stid, &attr, (void* (*)(void*))vtrack_P_T, pVtrack)) {
            FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
            fflush(stdout);
            FsLogExit();
        }
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n",
                pVtrack->rw.runStatus, pVtrack->ro.stid,
                pVtrack->ro._threadName,
                (unsigned long) pVtrack->ro._pMonitor,
                pVtrack->ro._parentTid,
                (unsigned long) pVtrack->ro._externRunStatus);
        fflush(stdout);
    }
}

/* 结束线程 */

void vtrack_stopThread(struct Vtrack * pVtrack) {
    pVtrack->rw.runStatus = 0;
    if (pVtrack->ro.stid) {
#ifdef FsDebug

        FsPrintf2(1, "Stop Start.\n");
#endif
        pthread_join(pVtrack->ro.stid, 0);
        thread_join_remove_check_pthreadSafety(pVtrack->ro.stid);
        pVtrack->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(1, "Stop successfully.\n");
#endif
    }
}

/* 传入数据帧 */
void vtrack_item_frame_in_pthreadSafety__OI_4(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
#define __item_frame_in_pthreadSafety__OI_4_Server Vtrack
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
unsigned int vtrack_item_frame_out_pthreadSafety__IO_4(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index
        , /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]) {
#define __item_frame_out_pthreadSafety__IO_4_Server Vtrack
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
unsigned int vtrack_item_get_frame_count_pthreadSafety(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index) {
#define __item_frame_out_pthreadSafety__IO_4_Server Vtrack
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
void vtrack_item_reset_groupSqrtOut(struct Vtrack * const pVtrack, /* 获取的通道编号,从0开始 */unsigned int index
        , /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt) {
#define __reset_groupSqrtOut_Server Vtrack
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
