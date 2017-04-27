#include "vsys.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Fs/Network.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/FileTool.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/Rtmp.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "capture.h"
#include "decode.h"
#include "encode.h"
#include "record.h"
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有结构开始//////////////////////////////////////

///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

static void vsys_private_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, /* 调用函数的指针 */ const void* const p) {
    void *parent = fs_Config_node_template_add(pConfig, pConfig, "vsys", "视频系统", "hostname", "instanceName", "视频系统,支持定时器", "hostname", "timerControl", 0, 0x7, 100);
    void *pNode = fs_Config_node_string_add(pConfig, parent, "hostname", "所属分布式节点IP", "所属分布式节点IP,必须为公网IP,127.0.0.1是无法实现分布式集群的", 0, 0x7, 0, 16, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "127.0.0.1", "127.0.0.1", "127.0.0.1");

    pNode = fs_Config_node_string_add(pConfig, parent, "instanceName", "实例名", "实例名", 0, 0x7, 0, 64, 1);

    pNode = fs_Config_node_string_add(pConfig, parent, "timerControl", "有效时间", "有效时间,时区+时间区间", 0, 0x7, 24, 33, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "+08 01-01/00:00:00 12-31/23:59:59", "+08 01-01/00:00:00 12-31/23:59:59", "每年");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-01/00:00:00 00-31/23:59:59", "+08 00-01/00:00:00 00-31/23:59:59", "每月");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/00:00:00 00-00/23:59:59", "+08 00-00/00:00:00 00-00/23:59:59", "每天");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/24:00:00 00-00/24:59:59", "+08 00-00/24:00:00 00-00/24:59:59", "每时");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/24:60:00 00-00/24:60:59", "+08 00-00/24:60:00 00-00/24:60:59", "每分");
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 1/00:00:00 7/23:59:59", "+08 1/00:00:00 7/23:59:59", "每周");

    pNode = fs_Config_node_string_add(pConfig, parent, "ntpfreq", "ntp校时间隔(s)", "ntp校时间隔(s),建议60S", 0, 0x7, 0, 300, 1);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");

    pNode = fs_Config_node_string_add(pConfig, parent, "ntpserver", "ntp服务器ip", "ntp服务器ip", 0, 0x7, 0, 16, 5);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "52.166.120.77", "52.166.120.77", "52.166.120.77");
    pNode = fs_Config_node_string_add(pConfig, parent, "rtmpserver", "rtmp服务器监听地址", "rtmp服务器监听地址", 0, 0x7, 0, 21, 5);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.0.0.0:1935", "0.0.0.0:1935", "0.0.0.0:1935");
    pNode = fs_Config_node_string_add(pConfig, parent, "hlsserver", "hls服务器监听地址", "hls服务器监听地址", 0, 0x7, 0, 21, 5);
    fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.0.0.0:16880", "0.0.0.0:16880", "0.0.0.0:16880");
    if (mask & (0x1LLU << 1)) {
        pNode = fs_Config_node_float_add(pConfig, parent, "freedisk", "磁盘保留空间", "磁盘保留空间,小于1表示保留空间为绝对值,如0.1表示空余10%,大于1表示相对其它空间的相对值,如1,其它空间总和为4,表示空余20%", 0, 0x7, 0.00001, 999999999.0, 1);
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0.1, "保留10%", "保留10%");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0.2, "保留20%", "保留20%");
    }

    pNode = fs_Config_node_template_add(pConfig, parent, "vsysChannel", "通道", "uuid", NULL, "通道", NULL, NULL, 0, 0x7, channelCount);

    fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode, "uuid", "通道标识", "通道标识,每个通道应唯一", 0, 0x7, 1, 64, 1),
            FsConfig_nodeValue_default, "0", "0", "0");
    void *pNode1 = fs_Config_node_integer_add(pConfig, pNode, "moduleMask", "增值功能", "增值功能,选1表示开启跟踪,选2表示开启水印叠加", 0, 0x7, 0, 10, 1);
    fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, 0, "默认", "默认");
    if (mask & (0x1LLU << 3)) fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 2, "开启水印", "开启水印");
    // 创建相机配置
    capture_createConfig(pConfig, mask, channelCount, pNode);
    // 创建解码配置
    decode_createConfig(pConfig, pNode);
    // 创建编码配置
    encode_createConfig(pConfig, mask, channelCount, pNode);
    // 创建数据记录配置
    record_createConfig(pConfig, mask, channelCount, pNode);
}

/* 从配置获取通道数 */
static unsigned int vsys_private_get_channelCount(/* 可为空 */FsConfig *pConfig, /* 本地ip地址 */const FsArray * const ipList) {
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

static int vsys_private_ConfigUpdate_and_get_keepspaceConfig_selector(const struct dirent *pDirent) {
    if (DT_DIR == pDirent->d_type && (pDirent->d_name[0] != '.' || (pDirent->d_name[1] != 0 && (pDirent->d_name[1] != '.' || pDirent->d_name[2] != 0))))return 1;
    else return 0;
}

/* 更新配置并获取磁盘配置 */
static FsStringBuilder *vsys_private_ConfigUpdate_and_get_keepspaceConfig__IO(/* 功能掩码,维数和通道数相同 */unsigned long rst_moduleMask[], /* 可为空 */FsConfig *pConfig, /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return NULL;
    /* 通道数 */
    FsObjectList * list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
    if (NULL == list)return NULL;
    void *parent = list->pNode[list->startIndex];
    fs_ObjectList_delete__OI(list);
    list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
    if (NULL == list)return NULL;
    /* 硬盘上已存在的文件夹 */
    struct dirent **namelist = NULL;
    int dirCount = scandir("/fs/project/data/vsys/", &namelist, vsys_private_ConfigUpdate_and_get_keepspaceConfig_selector, 0);
    char buffer[32];
    FsStringBuilder * rst = fs_StringBuilder_new__IO();
    double free_f = fs_Config_node_float_get_first(pConfig, parent, "freedisk", 0.1);
    fs_StringBuilder_apend(rst, "[setup]\r\n");
    unsigned int i = 0, index = 1;
    int j;
    void **ppNode = list->pNode + list->startIndex;
    char *pstr;
    unsigned short proportion;
    unsigned long proportionTotall = 0;
    for (; i < list->nodeCount; i++) {
        parent = *ppNode++;
        rst_moduleMask[i] = fs_Config_node_integer_get_first(pConfig, parent, "moduleMask", 0);
        if (fs_Config_node_integer_get_first(pConfig, parent, "recordConfig recordVideoMode", 0) > 0) {
            pstr = fs_Config_node_string_get_first__IO(pConfig, parent, "uuid");
            if (namelist) {
                for (j = 0; j < dirCount; j++) {
                    if (strcmp(namelist[j]->d_name, pstr) == 0) {
                        free(namelist[j]);
                        dirCount--;
                        if (j < dirCount) memmove(namelist + j, namelist + j + 1, (dirCount - j) * sizeof (void*));
                        break;
                    }
                }
            }
            sprintf(buffer, "dir_%u=", index++);
            fs_StringBuilder_apend(rst, buffer);
            fs_StringBuilder_apend(rst, "/fs/project/data/vsys/");
            fs_StringBuilder_apend(rst, pstr);
            proportion = fs_Config_node_integer_get_first(pConfig, parent, "recordConfig realVideoProportion", 1);
            proportionTotall += proportion;
            sprintf(buffer, "/video,%hu\r\n", proportion);
            fs_StringBuilder_apend(rst, buffer);
            sprintf(buffer, "dir_%u=", index++);
            fs_StringBuilder_apend(rst, buffer);
            fs_StringBuilder_apend(rst, "/fs/project/data/vsys/");
            fs_StringBuilder_apend(rst, pstr);
            proportion = fs_Config_node_integer_get_first(pConfig, parent, "recordConfig incidentProportion", 1);
            proportionTotall += proportion;
            sprintf(buffer, "/incident,%hu\r\n", proportion);
            fs_StringBuilder_apend(rst, buffer);
            free(pstr);
        }
    }
    fs_ObjectList_delete__OI(list);
    if (namelist) {
        for (j = 0; j < dirCount; j++) {
            sprintf(buffer, "dir_%u=", index++);
            fs_StringBuilder_apend(rst, buffer);
            fs_StringBuilder_apend(rst, "/fs/project/data/vsys/");
            fs_StringBuilder_apend(rst, namelist[j]->d_name);
            fs_StringBuilder_apend(rst, ",0\r\n");
            free(namelist[j]);
        }
        free(namelist);
    }
    if (free_f < 1.0) {
        proportionTotall = free_f / (1.0 - free_f) * proportionTotall;
        if (proportionTotall < 1)proportionTotall = 1;
    } else proportionTotall = free_f;
    sprintf(buffer, "free=%lu\r\n", proportionTotall);

    fs_StringBuilder_apend(rst, buffer);
    return rst;
}

static void vsys_private_update_ntpConfig(/* 可为空 */FsConfig *pConfig, const char ntpConfig[], /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return;
    FsObjectList * list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
    if (NULL == list)return;
    void *parent = list->pNode[list->startIndex];
    fs_ObjectList_delete__OI(list);
    fs_dir_make(ntpConfig, 0777);
    FILE * fd = fopen(ntpConfig, "wb");
    if (fd == NULL) {
        FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
        fflush(stdout);
        return;
    }
    if (fwrite("[setup]\n", strlen("[setup]\n"), 1, fd) != 1) {
        FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
        fflush(stdout);
        fclose(fd);
        return;
    }
    {
        char *ntpfreq = fs_Config_node_string_get_first__IO(pConfig, parent, "ntpfreq");
        if (ntpfreq) {
            if (fwrite("ntpfreq=", strlen("ntpfreq="), 1, fd) != 1) {
                FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                fflush(stdout);
                fclose(fd);
                free(ntpfreq);
                return;
            }
            if (fwrite(ntpfreq, strlen(ntpfreq), 1, fd) != 1) {
                FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                fflush(stdout);
                fclose(fd);
                free(ntpfreq);
                return;
            }
            free(ntpfreq);
        }
    }
    {
        FsConfigResult_string *ntpserver = fs_Config_node_string_get__IO(pConfig, parent, "ntpserver");
        if (ntpserver) {
            unsigned int i = 0;
            char buffer[32];
            for (; i < ntpserver->count; i++) {
                sprintf(buffer, "\nntpserver_%u=", i + 1);
                if (fwrite(buffer, strlen(buffer), 1, fd) != 1) {
                    FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                    fflush(stdout);
                    break;
                }
                if (fwrite(ntpserver->data[i], strlen(ntpserver->data[i]), 1, fd) != 1) {
                    FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                    fflush(stdout);
                    break;
                }
            }
            free(ntpserver);
            if (fputc('\n', fd) < 0) {

                FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                fflush(stdout);
            }
        }
    }
    fclose(fd);
}

static void vsys_private_update_server(/* 可为空 */FsConfig * const pConfig, Rtmp * const pRtmp, void * const pHls, /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return;
    void *parentNode = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
    if (NULL == parentNode)return;
    char *server = (char*) parentNode, *ps;
    parentNode = ((FsObjectList*) parentNode)->pNode[((FsObjectList*) parentNode)->startIndex];
    fs_ObjectList_delete__OI((FsObjectList*) server);
    unsigned int ipv4;
    unsigned short ipv4Port;
    /////////////////////////////////rtmp///////////////////////////////////////
    server = fs_Config_node_string_get_first__IO(pConfig, parentNode, "rtmpserver");
    ps = strchr(server, ':');
    if (ps) {
        ipv4 = fs_ipv4_network_get(ps - server, server);
        if (FS_INVALID_IPV4 == ipv4) {
            FsPrintf(1, "Invalid rtmpserver:\"%s\".\n", server);
            ipv4 = 0;
        }
        ipv4Port = atoi(ps + 1);
        if (ipv4Port < 1) {
            FsPrintf(1, "Invalid rtmpserver:\"%s\".\n", server);
            ipv4Port = 1935;
        }
    } else {
        ipv4 = fs_ipv4_network_get(strlen(server), server);
        if (FS_INVALID_IPV4 == ipv4) {
            FsPrintf(1, "Invalid rtmpserver:\"%s\".\n", server);
            ipv4 = 0;
        }
        ipv4Port = 1935;
    }
    free(server);
    rtmp_set_parameter_pthreadSafety(pRtmp, ipv4, ipv4Port);
   
}

/* 创建通道状态数据,返回要更新的链表,前部分为status,之后是ipsrc */
static inline FsObjectList *vsys_private_update_channelStatus_create__IO(struct Vsys * const pVsys, /* 本地ip地址 */const FsArray * const ipList) {
    FsConfig *pConfig = ((ConfigManager*) pVsys->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return NULL;
    FsEbml *rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    struct FsEbml_node *pEbml_node, *parent = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "cluster", FsEbmlNodeType_Struct);
    FsObjectList * list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 1, "vsys");
    char *ps;
    void **ppNode;
    unsigned int i;
    if (list) {
        for (ppNode = list->pNode + list->startIndex, i = list->nodeCount; i > 0; i--) {
            ps = fs_Config_node_string_get_first__IO(pConfig, *ppNode++, "hostname");
            fs_Ebml_node_data_set__OI_3(fs_Ebml_node_addChild(rst, parent, "host", FsEbmlNodeType_String), strlen(ps) + 1, ps);
        }
        fs_ObjectList_delete__OI(list);
    }
    list = fs_Config_node_template__IO(pConfig, pConfig, ipList, 0, "vsys");
    if (list) {
        ps = fs_Config_node_string_get_first__IO(pConfig, list->pNode[list->startIndex], "hostname");
        fs_Ebml_node_data_set__OI_3(fs_Ebml_node_addChild(rst, parent, "host", FsEbmlNodeType_String), strlen(ps) + 1, ps);
        parent = (struct FsEbml_node*) list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
        list = fs_Config_node_template__IO(pConfig, parent, NULL, 0, "vsysChannel");
        if (list) {
            *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "offlineChannelCount", FsEbmlNodeType_Integer)->data.buffer = list->nodeCount;
            parent = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "channelStatus", FsEbmlNodeType_Struct);
            for (ppNode = list->pNode + list->startIndex, i = list->nodeCount; i > 0; i--) {
                ps = fs_Config_node_string_get_first__IO(pConfig, *ppNode++, "uuid");
                pEbml_node = fs_Ebml_node_addChild(rst, parent, "item", FsEbmlNodeType_Struct);
                fs_Ebml_node_data_set__OI_3(fs_Ebml_node_addChild(rst, pEbml_node, "uuid", FsEbmlNodeType_String), strlen(ps) + 1, ps);
                *(long long*) fs_Ebml_node_addChild(rst, pEbml_node, "status", FsEbmlNodeType_Integer)->data.buffer = 0;
                fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, pEbml_node, "ipsrc", FsEbmlNodeType_String), 16, "255.255.255.255");
            }
            fs_ObjectList_delete__OI(list);
        }
    }
    pthread_mutex_lock(&pVsys->ro.__channelStatusDataClientList->mutex);
    if (pVsys->p.channelStatusData)fs_Ebml_delete__OI(pVsys->p.channelStatusData);
    pVsys->p.channelStatusData = rst;
    pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
    return fs_Ebml_node_get__IO(rst, parent, "item status");
}

/* 更新相机状态数据 */
static void inline vsys_private_update_channelStatus(FsEbml * const channelStatusData, struct FsEbml_node **ppNode_channelStatusData,
        /* 判断各通道是否在线,0-表示不在线,和channelStatusIndex相等表示在线 */ const unsigned char channelStatus[], /* 通道数量 */const unsigned int channelCount,
        /* 不在线的相机书量 */unsigned int offlineChannelCount) {
    *(long long*) fs_Ebml_node_get_first(channelStatusData, (struct FsEbml_node *) channelStatusData, "offlineChannelCount")->data.buffer = offlineChannelCount;
    unsigned int i = 0;
    for (; i < channelCount; i++) {
        *(long long*) (*ppNode_channelStatusData++)->data.buffer = (channelStatus[i]&0x8) ? 1 : 0;
    }
}

static inline FsString *vsys_private_channelStatus_sendData_prepare__IO(struct Vsys * const pVsys, /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
        /* 收到数据的前4字节 */const unsigned int head, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ const char requestDataType) {
    FsString * rst;
    if (1 == requestDataType) {
        if (NULL == pVsys->p.channelStatusData_ebml)
            pVsys->p.channelStatusData_ebml = fs_Ebml_to_String__IO(pVsys->p.channelStatusData, 0, 0, NULL);
        rst = fs_String_new__IO(8 + pVsys->p.channelStatusData_ebml->lenth + sizeof (unsigned int)*3);
        rst->lenth -= sizeof (unsigned int)*3;
        memcpy(rst->buffer + 8, pVsys->p.channelStatusData_ebml->buffer, pVsys->p.channelStatusData_ebml->lenth);
        *((unsigned int *) rst->buffer) = head, *((unsigned int *) (rst->buffer + 4)) = rst->lenth - 8;
        rst->buffer[0] &= 0xF0;
        rst->buffer[0] |= 0x3;
    } else if (2 == requestDataType) {
        if (NULL == pVsys->p.channelStatusData_xml) {
            FsXml *pXml = fs_Ebml_convert_to_Xml__IO(pVsys->p.channelStatusData);
            pVsys->p.channelStatusData_xml = fs_Xml_to_String__IO(pXml, 0, 0, "\r\n", NULL, 4);
            fs_Xml_delete__OI(pXml);
        }
        rst = fs_String_new__IO(8 + pVsys->p.channelStatusData_xml->lenth + sizeof (unsigned int)*3);
        rst->lenth -= sizeof (unsigned int)*3;
        memcpy(rst->buffer + 8, pVsys->p.channelStatusData_xml->buffer, pVsys->p.channelStatusData_xml->lenth);
        *((unsigned int *) rst->buffer) = head, *((unsigned int *) (rst->buffer + 4)) = rst->lenth - 8;
        rst->buffer[0] &= 0xF0;
        rst->buffer[0] |= 0xA;
    } else if (3 == requestDataType) {
        if (NULL == pVsys->p.channelStatusData_json) {
            FsJson *pJson = fs_Ebml_convert_to_Json__IO(pVsys->p.channelStatusData);
            pVsys->p.channelStatusData_json = fs_Json_to_String__IO(pJson, 0, 0, "\r\n", NULL, 4);
            fs_Json_delete__OI(pJson);
        }
        rst = fs_String_new__IO(8 + pVsys->p.channelStatusData_json->lenth + sizeof (unsigned int)*3);
        rst->lenth -= sizeof (unsigned int)*3;
        memcpy(rst->buffer + 8, pVsys->p.channelStatusData_json->buffer, pVsys->p.channelStatusData_json->lenth);
        *((unsigned int *) rst->buffer) = head, *((unsigned int *) (rst->buffer + 4)) = rst->lenth - 8;
        rst->buffer[0] &= 0xF0;
        rst->buffer[0] |= 0xA;
    } else {
        /* 程序不会到这 */
        FsPrintf(5, "Invalid requestDataType(=%hhd) data:\n", requestDataType);
        fflush(stdout);
    }
    memcpy(rst->buffer + rst->lenth, requestID_3, sizeof (unsigned int)*3);
    return rst;
}

static void vsys_private_channelStatus_send(struct Vsys * const pVsys) {
    unsigned int *requestData, (*ppNode)[6] = ((unsigned int (*)[6])pVsys->ro.__channelStatusDataClientList->pNode) + pVsys->ro.__channelStatusDataClientList->startIndex;
    unsigned i = pVsys->ro.__channelStatusDataClientList->nodeCount;
    for (; i > 0; i--) {
        requestData = *ppNode++;
        if (configManager_connect_check(requestData) != 1) {
            /* 已断开了 */
#ifdef FsDebug
            FsPrintf2(1, "Client:%#lx,%x disconnect.\n", *(long*) requestData, requestData[2]);
#endif
            configManager_connect_release(pVsys->ro._pConfigManager, requestData, 1);

            i = pVsys->ro.__channelStatusDataClientList->nodeCount - i;
            fs_StructList_remove_at(pVsys->ro.__channelStatusDataClientList, i);
            ppNode = ((unsigned int (*)[6])pVsys->ro.__channelStatusDataClientList->pNode) + pVsys->ro.__channelStatusDataClientList->startIndex + i;
            i = pVsys->ro.__channelStatusDataClientList->nodeCount - i + 1;
        } else {
            configManager_connect_send__OI_2(vsys_private_channelStatus_sendData_prepare__IO(pVsys, requestData, requestData[3], requestData[4]));
        }
    }
}

/* 关闭所有获取通道状态的连接 */
static void vsys_private_channelStatus_close(struct Vsys * const pVsys) {
    unsigned int *requestData, (*ppNode)[6] = ((unsigned int (*)[6])pVsys->ro.__channelStatusDataClientList->pNode) + pVsys->ro.__channelStatusDataClientList->startIndex;
    unsigned i = pVsys->ro.__channelStatusDataClientList->nodeCount;
    for (; i > 0; i--) {
        requestData = *ppNode++;
        configManager_connect_release(pVsys->ro._pConfigManager, requestData, 1);
    }
    fs_StructList_clear(pVsys->ro.__channelStatusDataClientList);
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static signed char vsys_private_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
        /* 收到数据的前4字节 */unsigned int head, /* 收到的数据 */FsEbml *pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* p) {
    struct Vsys *pVsys = (struct Vsys *) p;
    fs_Ebml_out_debug(pEbml, stdout), printf("\n");
    pthread_mutex_lock(&pVsys->ro.__channelStatusDataClientList->mutex);
    if (pVsys->p.channelStatusData) {
        /* 统计下是否有通道断开了连接 */
        {
            unsigned int *requestData, (*ppNode)[6] = ((unsigned int (*)[6])pVsys->ro.__channelStatusDataClientList->pNode) + pVsys->ro.__channelStatusDataClientList->startIndex;
            unsigned i = pVsys->ro.__channelStatusDataClientList->nodeCount;
            for (; i > 0; i--) {
                requestData = *ppNode++;
                if (configManager_connect_check(requestData) != 1) {
                    /* 已断开了 */
#ifdef FsDebug
                    FsPrintf2(1, "Client:%#lx,%x disconnect.\n", *(long*) requestData, requestData[2]);
#endif
                    configManager_connect_release(pVsys->ro._pConfigManager, requestData, 1);

                    i = pVsys->ro.__channelStatusDataClientList->nodeCount - i;
                    fs_StructList_remove_at(pVsys->ro.__channelStatusDataClientList, i);
                    ppNode = ((unsigned int (*)[6])pVsys->ro.__channelStatusDataClientList->pNode) + pVsys->ro.__channelStatusDataClientList->startIndex + i;
                    i = pVsys->ro.__channelStatusDataClientList->nodeCount - i + 1;
                }
            }
        }
        struct FsEbml_node * return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (return_type && 0 < return_type->data.lenth) {
            if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
            else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
        }
        configManager_send_pthreadSafety__OI_2(pVsys->ro._pConfigManager,
                vsys_private_channelStatus_sendData_prepare__IO(pVsys, requestID_3, head, requestDataType));
        unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], head, (unsigned int) requestDataType, 0};
        if (fs_StructList_insert_order(pVsys->ro.__channelStatusDataClientList, data) < 0) {
            pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
            return 1;
        } else {
            pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
            return -128;
        }
    } else {
        pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
        FsPrintf(1, "Vsys is busy.\n");
        fs_Ebml_out_debug(pEbml, stdout), printf("\n");
        fflush(stdout);
        return -1;
    }
}

/* 本线程负责数据采集的业务,并管理各工作线程 */
static void *vsys_private_thread(struct Vsys * const pVsys) {
    /* 创建线程监控 */
    MonitorItem *pMonitorItem = monitorItem_new_and_add(pVsys->ro._pMonitor,
            pVsys->ro._threadName, NULL, pVsys, pVsys->ro._parentTid);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *externRunStatus = pVsys->ro._externRunStatus;
    ConfigManager *pConfigManager = (ConfigManager*) pVsys->ro._pConfigManager;
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */   
    signed char childRun = 1;   
    *(long*)childRun=0xFF;
    double sleepToTime = 0.0;
#define VsysTaskCount 4
    FsMemeryFree *pMemeryFree = fs_memeryFree_new__IO();
    double thisTime, nextShowTime = 0.0;
    unsigned int configCheckSum[4] = {(unsigned int) ((unsigned long) pVsys), 0, 0, 0};
    int resetIndex = 0;
    unsigned int taskResetOkCount = 0;
    FsObjectList *resetList = fs_ObjectList_new__IO(4);
    FsArray *ipList = NULL;
    // 通道数
    int channelCount = 0;
    /* 判断各通道是否在线,(&0x80)--0:表示未检测,1:表示检测中;(&0x70,3位)--用于统计是否在线;(&0x08,1位)--0:表示数据不通,8:表示可收到数据 */
    unsigned char *channelStatus = (unsigned char *) malloc(8), channelStatusIndex = 0x8 | 0x70, channelChange = 0;
    /* 通道是否有效,0-无效,1,-有效 */
    unsigned char *channelValid = channelStatus;
    /* 通道的源地址 */
    unsigned int *ipsrc;
    /* 不在线和相机数量 */
    unsigned int offlineChannelCount = 0xFFFFFFFFU, offlineChannelCount_new = 0;
    /* 当前相机检查的编号 */
    int channelStatusCheckIndex = 0;
    /* 当前相机检查的块大小 */
    int channelStatusCheckBlockSize = 1;
    /* 当前正在检查的相机量 */
    int channelStatusCheckInCount = 0;
    /* 更新通道状态缓存的链表 */
    FsObjectList *channelStatusData_UpdateList = NULL;
    struct FsEbml_node **ppNode_channelStatusData;
    /* 帧率 */
    float *frameRate = (float*) channelStatus, frameRateAvg = 10.0;
    /* 模块掩码 */
    unsigned long *moduleMask = (unsigned long*) channelStatus;
    /* 磁盘管理配置 */
    FsStringBuilder *keepspaceConfig = NULL;
    // rtmp服务器
    Rtmp *pRtmp;
    // 采集
    struct Capture *pCapture;
    // 解码
    struct Decode *pDecode;
    // 编码
    struct Encode *pEncode;
    // 记录
    struct Record *pRecord;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    int i = 0;
    FsObjectImageFrame *pFrame;
    FILE *fd;
    float show_fps = 0.0;
    unsigned int show_c = 0, show_d = 0, show_t = 0, show_e = 0, show_r = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    // 创建配置
    configManager_init_register(pConfigManager, pVsys->ro._threadName, vsys_private_createConfig, pVsys);
    // 注册命令字
    configManager_cmd_register(pConfigManager, "channelStatus_get", "vsys", 0, pVsys, vsys_private_cmd_cb, NULL, pVsys);
    // rtmp服务器
    pRtmp = rtmp_new__IO("Rtmp", pVsys->ro._pMonitor, systemThreadTid, &childRun, 0, 1935, 512);
    rtmp_startThread(pRtmp, 1);
    fs_memeryFree_add_thread(pMemeryFree, (void (*)(void*))rtmp_stopThread, (void (*)(void*)) rtmp_delete__OI, pRtmp);
    // 采集
    pCapture = capture_new__IO("Capture", pVsys->ro._pMonitor, systemThreadTid, &childRun, pConfigManager, 128, pRtmp);
    capture_startThread(pCapture, -1);
    fs_memeryFree_add_thread(pMemeryFree, (void (*)(void*))capture_stopThread, (void (*)(void*)) capture_delete__OI, pCapture);
    // 解码
    pDecode = decode_new__IO("Decode", pVsys->ro._pMonitor, systemThreadTid, &childRun, pConfigManager);
    decode_startThread(pDecode, -1);
    fs_memeryFree_add_thread(pMemeryFree, (void (*)(void*))decode_stopThread, (void (*)(void*)) decode_delete__OI, pDecode);
    // 编码
    pEncode = encode_new__IO("Encode", pVsys->ro._pMonitor, systemThreadTid, &childRun, pConfigManager);
    encode_startThread(pEncode, -1);
    fs_memeryFree_add_thread(pMemeryFree, (void (*)(void*))encode_stopThread, (void (*)(void*)) encode_delete__OI, pEncode);
    // 记录
    pRecord = record_new__IO("Record", pVsys->ro._pMonitor, systemThreadTid, &childRun, "/fs/project/data/vsys/", NULL, pRtmp, NULL, NULL, pConfigManager, 32);
    record_startThread(pRecord, -1);
    fs_memeryFree_add_thread(pMemeryFree, (void (*)(void*))record_stopThread, (void (*)(void*)) record_delete__OI, pRecord);
    ////////////////////////////////////////////////////////////////////////////
    pMonitorItem->rw.line = __LINE__;
    /* 主循环 */
    while (pVsys->rw.runStatus == *externRunStatus || taskResetOkCount != 0) {
        pMonitorItem->rw.cycleAlive = 1;
        pMonitorItem->rw.line = __LINE__;
        if (resetIndex == channelCount)fs_thread_sleepTo(sleepToTime);
        thisTime = fs_time_uptime_get();
        if (frameRateAvg < 5.0)frameRateAvg = 5.0;
        sleepToTime = thisTime + 1.0 / frameRateAvg;
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        if (((!FsMd5CheckSumEqual(configCheckSum, pConfigManager->ro.configCheckSum)) || configCheckSum[4] != pConfigManager->ro.configCheckSum[4]) && resetIndex == channelCount) {
            /* 上一次重置已完成 */
            configCheckSum[0] = pConfigManager->ro.configCheckSum[0], configCheckSum[1] = pConfigManager->ro.configCheckSum[1];
            configCheckSum[2] = pConfigManager->ro.configCheckSum[2], configCheckSum[3] = pConfigManager->ro.configCheckSum[3], configCheckSum[4] = pConfigManager->ro.configCheckSum[4];
            /* 配置有改动 */
            // 读取配置
            configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
            if (ipList)free(ipList);
            ipList = fs_network_ipv4_get_all__IO();
            /* 断开原来的所有取流 */
            for (i = 0; i < channelCount; i++) {
                if (channelStatus[i]&0x80) {
                    record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], 0);
                    channelStatus[i] &= 0x7F;
                }
            }
            channelCount = vsys_private_get_channelCount(pConfigManager->ro.__pConfig, ipList);

            /* 重置任务线程 */
            if (resetIndex != channelCount) {
                if (channelCount > 0) {
                    channelStatus = (unsigned char *) realloc(channelStatus, Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (unsigned int)*channelCount) + Memery_Alignment(sizeof (float)*channelCount) + sizeof (unsigned long)*channelCount);
                } else channelStatus = (unsigned char *) realloc(channelStatus, 8);
                channelValid = channelStatus + channelCount;
                ipsrc = (unsigned int *) (channelStatus + Memery_Alignment(channelCount << 1));
                frameRate = (float*) (channelStatus + Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (unsigned int)*channelCount));
                moduleMask = (unsigned long*) (channelStatus + Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (unsigned int)*channelCount) + Memery_Alignment(sizeof (float)*channelCount));
            }
            if (resetIndex < channelCount)
                memset(channelStatus + resetIndex, 0, Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (unsigned int)*channelCount) + sizeof (float)*channelCount - resetIndex);
            else memset(channelStatus + channelCount, 0, Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (unsigned int)*channelCount) + sizeof (float)*channelCount - channelCount);
            if (keepspaceConfig)fs_StringBuilder_delete__OI(keepspaceConfig);
            keepspaceConfig = vsys_private_ConfigUpdate_and_get_keepspaceConfig__IO(moduleMask, pConfigManager->ro.__pConfig, ipList);
            vsys_private_update_ntpConfig(pConfigManager->ro.__pConfig, "/tmp/config/ntp.cfg", ipList);
            vsys_private_update_server(pConfigManager->ro.__pConfig, pRtmp, NULL, ipList);
            if (channelStatusData_UpdateList)fs_ObjectList_delete__OI(channelStatusData_UpdateList);
            if ((channelStatusData_UpdateList = vsys_private_update_channelStatus_create__IO(pVsys, ipList)) != NULL) {
                ppNode_channelStatusData = (struct FsEbml_node**) channelStatusData_UpdateList->pNode + channelStatusData_UpdateList->startIndex;
            }
            /* 强制刷新一次相机状态 */
            offlineChannelCount = 0xFFFFFFFFU;
            channelStatusCheckIndex = 0;
            channelStatusCheckBlockSize = 1;
            channelStatusCheckInCount = 0;
            resetIndex = -1;
            taskResetOkCount = 0;
            pMonitorItem->rw.line = __LINE__;
        }
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        /* 更新磁盘管理配置 */
        if (keepspaceConfig && access("/tmp/config/keepspace.cfg.update", F_OK)) {
            if (access("/tmp/config", F_OK))mkdir("/tmp/config", 0777);
            if (fs_StringBuilder_save_to_file(keepspaceConfig, "/tmp/config/keepspace.cfg") != 1) {
                FsPrintf(1, "Save keepspace config to the file:\"%s\" failed.\n", "/tmp/config/keepspace.cfg");
                fflush(stdout);
            }
            fs_StringBuilder_delete__OI(keepspaceConfig);
            keepspaceConfig = NULL;
            fd = fopen("/tmp/config/keepspace.cfg.update", "w");
            if (fd)fclose(fd);
            else {
                FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", "/tmp/config/keepspace.cfg.update", strerror(errno));
                fflush(stdout);
            }
        }
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        /* 重置 */
        if (resetIndex < channelCount) {
            pMonitorItem->rw.line = __LINE__;
            memset(channelValid, 1, channelCount);
            for (i = -1; i < channelCount; i++) {
                if (i == resetIndex && 0 == taskResetOkCount)FsPrintf2(1, "Reset Channel(=%d/%d).\n", resetIndex, channelCount);
                // 采集   
                if (i == resetIndex) {
                    if (taskResetOkCount < VsysTaskCount) {
                        if (pVsys->rw.runStatus == *externRunStatus) pCapture->rw.resetChannel = resetIndex + 1, taskResetOkCount++, fs_ObjectList_insert_tail(resetList, pCapture);
                    } else if (pCapture->rw.resetChannel < 0 && fs_ObjectList_remove(resetList, pCapture)>-1) taskResetOkCount++;
                    if (taskResetOkCount == (VsysTaskCount << 1))resetIndex++, taskResetOkCount = 0;
                }
                if (i>-1 && (i == resetIndex || (unsigned int) channelCount != pCapture->ro.__itemList->nodeCount || NULL == pCapture->ro.__itemList->pNode[pCapture->ro.__itemList->startIndex + i])) channelValid[i] = 0;
                // 解码
                if (i == resetIndex) {
                    if (taskResetOkCount < VsysTaskCount) {
                        if (pVsys->rw.runStatus == *externRunStatus)pDecode->rw.resetChannel = resetIndex + 1, taskResetOkCount++, fs_ObjectList_insert_tail(resetList, pDecode);
                    } else if (pDecode->rw.resetChannel < 0 && fs_ObjectList_remove(resetList, pDecode)>-1) taskResetOkCount++;
                    if (taskResetOkCount == (VsysTaskCount << 1))resetIndex++, taskResetOkCount = 0;
                }
                if (i>-1 && (i == resetIndex || (unsigned int) channelCount != pDecode->ro.__itemList->nodeCount || NULL == pDecode->ro.__itemList->pNode[pDecode->ro.__itemList->startIndex + i])) channelValid[i] = 0;
                // 编码
                if (i == resetIndex) {
                    if (taskResetOkCount < VsysTaskCount) {
                        if (pVsys->rw.runStatus == *externRunStatus)pEncode->rw.resetChannel = resetIndex + 1, taskResetOkCount++, fs_ObjectList_insert_tail(resetList, pEncode);
                    } else if (pEncode->rw.resetChannel < 0 && fs_ObjectList_remove(resetList, pEncode)>-1) taskResetOkCount++;
                    if (taskResetOkCount == (VsysTaskCount << 1))resetIndex++, taskResetOkCount = 0;
                }
                if (i>-1 && (i == resetIndex || (unsigned int) channelCount != pEncode->ro.__itemList->nodeCount || NULL == pEncode->ro.__itemList->pNode[pEncode->ro.__itemList->startIndex + i])) channelValid[i] = 0;
                // 记录
                if (i == resetIndex) {
                    if (taskResetOkCount < VsysTaskCount) {
                        if (pVsys->rw.runStatus == *externRunStatus) pRecord->rw.resetChannel = resetIndex + 1, taskResetOkCount++, fs_ObjectList_insert_tail(resetList, pRecord);
                    } else if (pRecord->rw.resetChannel < 0 && fs_ObjectList_remove(resetList, pRecord)>-1) taskResetOkCount++;
                    if (taskResetOkCount == (VsysTaskCount << 1))resetIndex++, taskResetOkCount = 0;
                }
                if (i>-1 && (i == resetIndex || (unsigned int) channelCount != pRecord->ro.__itemList->nodeCount || NULL == pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])) channelValid[i] = 0;
                ////////////////////////////////////////////////////////////////
                if (i == resetIndex && taskResetOkCount < VsysTaskCount && pVsys->rw.runStatus == *externRunStatus) i--;
            }
            if (resetIndex == channelCount) {
                configManager_config_deleteRefer_pthreadSafety(pConfigManager);
                memset(channelValid, 1, channelCount);
                FsPrintf2(1, "Reset Channel(=%d/%d) finish.\n", resetIndex, channelCount);
                if (ipList) {
                    free(ipList);
                    ipList = NULL;
                }
            }
            pMonitorItem->rw.line = __LINE__;
        }
        /* 检查相机是否在线 */
        if (channelStatusCheckInCount < channelStatusCheckBlockSize) {
            //                FsPrintf(1, "channelStatusCheckInCount=%u/%u,channelStatusCheckIndex=%u/%u,%#hhx,%hhu.\n",
            //                        channelStatusCheckInCount, channelStatusCheckBlockSize, channelStatusCheckIndex, channelCount, channelCount > 0 ? channelStatus[0] : 0, channelStatusIndex);
            //                fflush(stdout);
            if (channelStatusCheckIndex < resetIndex) {
                if (0 == (channelStatus[channelStatusCheckIndex]&0x80) &&
                        ((channelStatus[channelStatusCheckIndex]^channelStatusIndex)& 0x70) != 0 &&
                        (((channelStatus[channelStatusCheckIndex] + 0x10)^channelStatusIndex)& 0x70) != 0) {
                    record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + channelStatusCheckIndex], 1);
                    channelStatus[channelStatusCheckIndex] = 0x80 | (channelStatusIndex & 0x70) | (channelStatus[channelStatusCheckIndex]&0xF);
                    channelStatusCheckIndex++;
                    channelStatusCheckBlockSize = ++channelStatusCheckInCount;
                } else {
                    for (channelStatusCheckIndex++; channelStatusCheckIndex < resetIndex && (0 != (channelStatus[channelStatusCheckIndex]&0x80) ||
                            ((channelStatus[channelStatusCheckIndex]^channelStatusIndex)& 0x70) == 0 ||
                            (((channelStatus[channelStatusCheckIndex] + 0x10)^channelStatusIndex)& 0x70) == 0); channelStatusCheckIndex++);
                    if (channelStatusCheckIndex < resetIndex) {
                        record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + channelStatusCheckIndex], 1);
                        channelStatus[channelStatusCheckIndex] = 0x80 | (channelStatusIndex & 0x70) | (channelStatus[channelStatusCheckIndex]&0xF);
                        channelStatusCheckIndex++;
                        channelStatusCheckBlockSize = ++channelStatusCheckInCount;
                    }
                }
            }
            //                FsPrintf2(1, "channelStatusCheckInCount=%u/%u,channelStatusCheckIndex=%u/%u,%#hhx,%hhu.\n",
            //                        channelStatusCheckInCount, channelStatusCheckBlockSize, channelStatusCheckIndex, channelCount, channelCount > 0 ? channelStatus[0] : 0, channelStatusIndex);
            //                fflush(stdout);
        }
        pMonitorItem->rw.line = __LINE__;
        /* 数据帧传送 */
        for (i = 0; i < channelCount; i++) {
            if (1 == channelValid[i]) {
                pMonitorItem->rw.line = __LINE__;
                // 采集 
                while ((pFrame = capture_item_frame_out_pthreadSafety_IO((struct Capture_item *) pCapture->ro.__itemList->pNode[pCapture->ro.__itemList->startIndex + i])) != NULL) {
                    frameRate[i] = pFrame->frameRate;
                    frameRateAvg = frameRateAvg + (pFrame->frameRate - frameRateAvg) / channelCount;
                    if (ipsrc[i] != pFrame->ipsrc) {
                        ipsrc[i] = pFrame->ipsrc, channelChange = 1;
                        pthread_mutex_lock(&pVsys->ro.__channelStatusDataClientList->mutex);
                        ppNode_channelStatusData[i]->parent->child[2]->data.lenth = 1 + sprintf(ppNode_channelStatusData[i]->parent->child[2]->data.buffer, "%hhu.%hhu.%hhu.%hhu",
                                ((unsigned char*) (ipsrc + i))[0], ((unsigned char*) (ipsrc + i))[1], ((unsigned char*) (ipsrc + i))[2], ((unsigned char*) (ipsrc + i))[3]);
                        pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
                    }
                    if (0 == moduleMask[i]) {
                        if (record_item_frame_in_pthreadSafety_OI((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], pFrame) == 1) {
                            if (channelStatus[i]&0x80) {
                                record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], 0);
                                channelStatusCheckInCount--;
                            }
                            channelStatus[i] = (channelStatus[i]&0x07) | channelStatusIndex;
                        }
                    } else decode_item_frame_in_pthreadSafety_OI((struct Decode_item *) pDecode->ro.__itemList->pNode[pDecode->ro.__itemList->startIndex + i], pFrame);
                }
                // 解码 
                while ((pFrame = decode_item_frame_out_pthreadSafety_IO((struct Decode_item *) pDecode->ro.__itemList->pNode[pDecode->ro.__itemList->startIndex + i])) != NULL) {                    
                     encode_item_frame_in_pthreadSafety_OI((struct Encode_item *) pEncode->ro.__itemList->pNode[pEncode->ro.__itemList->startIndex + i], pFrame);
                }
                // 编码 
                while ((pFrame = encode_item_frame_out_pthreadSafety_IO((struct Encode_item *) pEncode->ro.__itemList->pNode[pEncode->ro.__itemList->startIndex + i])) != NULL) {
                    //                if (NULL == pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i]) {
                    //                    FsPrintf(1, "resetIndex=%d/%d,%#lx/%#lx\n", i, channelCount,
                    //                            (long) (pRecord->ro.__itemList->pNode + pRecord->ro.__itemList->startIndex + i),
                    //                            (long) pRecord->ro.__itemList->pNode[ pRecord->ro.__itemList->startIndex + i]);
                    //                    fflush(stdout);
                    //                }
                    if (record_item_frame_in_pthreadSafety_OI((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], pFrame) == 1) {
                        if (channelStatus[i]&0x80) {
                            record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], 0);
                            channelStatusCheckInCount--;
                        }
                        channelStatus[i] = (channelStatus[i]&0x07) | channelStatusIndex;
                    }
                }
                // 记录
                //            if (NULL == pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i]) {
                //                FsPrintf(1, "resetIndex=%d/%d,%#lx/%#lx\n", i, channelCount,
                //                        (long) (pRecord->ro.__itemList->pNode + pRecord->ro.__itemList->startIndex + i),
                //                        (long) pRecord->ro.__itemList->pNode[ pRecord->ro.__itemList->startIndex + i]);
                //                fflush(stdout);
                //            }
                while ((pFrame = record_item_frame_out_pthreadSafety_IO((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])) != NULL) {
                    // 回收
                    fs_Object_delete_pthreadSafety__OI(pFrame);
                }
                pMonitorItem->rw.line = __LINE__;
            }
            /* 统计缓存 */
            if (nextShowTime < thisTime) {
                /* 统计相机状态 */
                if ((channelStatus[i]&0xF) != 0) {
                    if (channelStatus[i]&0x80) {
                        if (((channelStatus[i]^channelStatusIndex)& 0x70) != 0 && (((channelStatus[i] + 0x10)^channelStatusIndex)& 0x70) != 0) {
                            channelChange = 1, channelStatus[i] &= 0xF0;
                            offlineChannelCount_new++;
                        }
                    }
                } else offlineChannelCount_new++;
                //FsPrintf(1, "i=%d/%d,%u/%u,%#hhx/%#hhx\n", i, channelCount, offlineChannelCount_new, offlineChannelCount, channelStatus[i], channelStatusIndex);
                if (1 == channelValid[i]) {
                    //                FsPrintf2(1, "CH:%u %.2ffps C%lu R%lu\n", i + 1, frameRate[i], ((struct Capture_item *) pCapture->ro.__itemList->pNode[pCapture->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount,
                    //                        ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistIn->nodeCount +
                    //                        ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount);
                    pMonitorItem->rw.line = __LINE__;
                    if (show_fps < frameRate[i])show_fps = frameRate[i];
                    if (show_c < ((struct Capture_item *) pCapture->ro.__itemList->pNode[pCapture->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount)show_c =
                            ((struct Capture_item *) pCapture->ro.__itemList->pNode[pCapture->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount;
                    if (show_d < ((struct Decode_item *) pDecode->ro.__itemList->pNode[pDecode->ro.__itemList->startIndex + i])->ro.__pFrameConvert_item->ro.__framelistOut->nodeCount)show_d =
                            ((struct Decode_item *) pDecode->ro.__itemList->pNode[pDecode->ro.__itemList->startIndex + i])->ro.__pFrameConvert_item->ro.__framelistOut->nodeCount;                  
                    if (show_e < ((struct Encode_item *) pEncode->ro.__itemList->pNode[pEncode->ro.__itemList->startIndex + i])->ro.__pFrameConvert_item->ro.__framelistOut->nodeCount)show_e =
                            ((struct Encode_item *) pEncode->ro.__itemList->pNode[pEncode->ro.__itemList->startIndex + i])->ro.__pFrameConvert_item->ro.__framelistOut->nodeCount;
                    if (show_r < ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistIn->nodeCount +
                            ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount)show_r =
                            ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistIn->nodeCount +
                            ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount;
                }
                if (i + 1 == channelCount) {
                    if (resetIndex == channelCount)FsPrintf2(1, "[%d]:%05.2f C%u D%u T%u E%u R%u\n", channelCount, show_fps, show_c, show_d, show_t, show_e, show_r);
                    else FsPrintf2(1, "[%d/%d]:%05.2f C%u D%u T%u E%u R%u\n", resetIndex, channelCount, show_fps, show_c, show_d, show_t, show_e, show_r);
                    show_fps = 0.0;
                    show_c = show_d = show_t = show_e = show_r = 0;
                    if (0 && access("/tmp/showipsrc", F_OK) == 0) {
                        int ttttt = 0;
                        FsPrintf(1, "showipsrc channelCount=%d\n", channelCount);
                        for (; ttttt < channelCount; ttttt++) {
                            printf("[%d]%s-%s/%hhu.%hhu.%hhu.%hhu\n", ttttt, ppNode_channelStatusData[ttttt]->parent->child[0]->data.buffer,
                                    ppNode_channelStatusData[ttttt]->parent->child[2]->data.buffer,
                                    ((unsigned char*) (ipsrc + ttttt))[0], ((unsigned char*) (ipsrc + ttttt))[1], ((unsigned char*) (ipsrc + ttttt))[2], ((unsigned char*) (ipsrc + ttttt))[3]);
                        }
                        printf("\n");
                        FsJson *pJson = fs_Ebml_convert_to_Json__IO(pVsys->p.channelStatusData);
                        fs_Json_save_to_file(pJson, "/tmp/showipsrc.txt", NULL, "\n", 4);
                        fs_Json_delete__OI(pJson);
                    }
                    /* 判断通道状态是否发生改变 */
                    if (channelChange || offlineChannelCount != offlineChannelCount_new) {
                        /* 通道状态发生改变了 */
                        channelChange = 0, offlineChannelCount = offlineChannelCount_new;
                        /* 更新通道数据 */
                        vsys_private_update_channelStatus(pVsys->p.channelStatusData, ppNode_channelStatusData, channelStatus, channelCount, offlineChannelCount);
                        /* 向所有客户端发送数据 */
                        pthread_mutex_lock(&pVsys->ro.__channelStatusDataClientList->mutex);
                        if (pVsys->p.channelStatusData_ebml)fs_String_delete__OI(pVsys->p.channelStatusData_ebml), pVsys->p.channelStatusData_ebml = NULL;
                        if (pVsys->p.channelStatusData_xml)fs_String_delete__OI(pVsys->p.channelStatusData_xml), pVsys->p.channelStatusData_xml = NULL;
                        if (pVsys->p.channelStatusData_json)fs_String_delete__OI(pVsys->p.channelStatusData_json), pVsys->p.channelStatusData_json = NULL;
                        vsys_private_channelStatus_send(pVsys);
                        pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
                    }
                    offlineChannelCount_new = 0;
                    if (channelStatusCheckIndex == channelCount) {
                        /* 完成了一轮检查了 */
                        channelStatusIndex = (channelStatusIndex + 0x10)& 0x7F;
                        channelStatusCheckIndex = 0;
                    } else if (channelStatusCheckInCount == channelStatusCheckBlockSize) channelStatusCheckBlockSize++;
                    nextShowTime = thisTime + 3;
                }
            } else if (channelChange && i + 1 == channelCount) {
                /* 通道状态发生改变了 */
                channelChange = 0;
                /* 向所有客户端发送数据 */
                pthread_mutex_lock(&pVsys->ro.__channelStatusDataClientList->mutex);
                if (pVsys->p.channelStatusData_ebml)fs_String_delete__OI(pVsys->p.channelStatusData_ebml), pVsys->p.channelStatusData_ebml = NULL;
                if (pVsys->p.channelStatusData_xml)fs_String_delete__OI(pVsys->p.channelStatusData_xml), pVsys->p.channelStatusData_xml = NULL;
                if (pVsys->p.channelStatusData_json)fs_String_delete__OI(pVsys->p.channelStatusData_json), pVsys->p.channelStatusData_json = NULL;
                vsys_private_channelStatus_send(pVsys);
                pthread_mutex_unlock(&pVsys->ro.__channelStatusDataClientList->mutex);
            }
        }
        pMonitorItem->rw.line = __LINE__;
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    childRun = -128;
    /* 释放配置引用 */
    if (resetIndex < channelCount) configManager_config_deleteRefer_pthreadSafety(pConfigManager);
    // 注销命令字
    configManager_cmd_logoff(pConfigManager, "channelStatus_get", "vsys", 0, pVsys);
    /* 关闭所有获取通道状态的连接 */
    vsys_private_channelStatus_close(pVsys);
    /* 释放空间 */
    /* 释放全局变量空间 */
    if (ipList)free(ipList);
    fs_ObjectList_delete__OI(resetList);
    if (channelStatus)free(channelStatus);
    if (keepspaceConfig)fs_StringBuilder_delete__OI(keepspaceConfig);
    if (channelStatusData_UpdateList)fs_ObjectList_delete__OI(channelStatusData_UpdateList);
    if (pVsys->p.channelStatusData)fs_Ebml_delete__OI(pVsys->p.channelStatusData), pVsys->p.channelStatusData = NULL;
    if (pVsys->p.channelStatusData_ebml)fs_String_delete__OI(pVsys->p.channelStatusData_ebml), pVsys->p.channelStatusData_ebml = NULL;
    if (pVsys->p.channelStatusData_xml)fs_String_delete__OI(pVsys->p.channelStatusData_xml), pVsys->p.channelStatusData_xml = NULL;
    if (pVsys->p.channelStatusData_json)fs_String_delete__OI(pVsys->p.channelStatusData_json), pVsys->p.channelStatusData_json = NULL;
    fs_memeryFree_delete__OI(pMemeryFree);
    /* 删除配置 */
    configManager_init_logoff(pConfigManager, pVsys->ro._threadName, vsys_private_createConfig, pVsys);
    /* 释放中间变量空间 */
    ////////////////////////////////////////////////////////////////////////////
    FsPrintf2(1, "Thread:\"%s\" exit,runStatus=%hhd.\n", pVsys->ro._threadName, pVsys->rw.runStatus);
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pVsys->ro._pMonitor, pMonitorItem);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 
 * 创建一个新的Vsys对象;
 * 返回Vsys指针.
 */
struct Vsys *vsys_new__IO(/* 线程名 */const char threadName[],
        /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
        /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
        /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
        /* 配置管理对象 */ void *pConfigManager) {
    unsigned int len = strlen(threadName) + 1;
    struct Vsys* rst = (struct Vsys*) malloc(sizeof (struct Vsys) +len);
    memset(rst, 0, sizeof (struct Vsys));
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
    /* 配置管理对象 */
    rst->ro._pConfigManager = pConfigManager;
    /* 获取通道状态的客户端链表 */
    rst->ro.__channelStatusDataClientList = fs_StructList_new__IO(6, sizeof (int)*6);

    return rst;
}

/* 删除pVsys指向的实例对象 */
void vsys_delete__OI(struct Vsys * const pVsys) {

    /* 获取通道状态的客户端链表 */
    fs_StructList_delete__OI(pVsys->ro.__channelStatusDataClientList);

    free(pVsys);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void vsys_startThread(struct Vsys * const pVsys,
        /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice) {
    if (pVsys->rw.runStatus == 0 && pVsys->ro.stid == 0) {
        pVsys->rw.runStatus = 1;
        pVsys->rw.nice = nice;
#ifdef FsDebug
        FsPrintf2(1, "Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n\
  pConfigManager=%#lx\n  channelStatusDataClientList=%#lx\n",
                pVsys->ro._threadName,
                (long) pVsys->ro._pMonitor,
                pVsys->ro._parentTid,
                (long) pVsys->ro._externRunStatus,
                (long) pVsys->ro._pConfigManager,
                (long) pVsys->ro.__channelStatusDataClientList);
#endif
        pthread_attr_t attr;
        /* 初始化线程属性 */
        pthread_attr_init(&attr);
        /* 设置分离状态 */
        //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        /* 设置栈地址 */
        //pthread_attr_setstackaddr ( pthread_attr_t *attr, void *addr );
        /* 设置栈大小 */
        pthread_attr_setstacksize(&attr, 64 * 1024);
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
        pthread_create(&pVsys->ro.stid, &attr, (void* (*)(void*))vsys_private_thread, pVsys);
        pthread_attr_destroy(&attr);
    } else {

        FsPrintf2(1, "The thread is allready running(runStatus=%hhd,tid=%ld),Init Parameter:\n  threadName:\"%s\"\n  pMonitor=%#lx\n  parentTid=%ld\n  externRunStatus=%#lx\n\
  pConfigManager=%#lx\n  channelStatusDataClientList=%#lx\n", pVsys->rw.runStatus, pVsys->ro.stid,
                pVsys->ro._threadName,
                (long) pVsys->ro._pMonitor,
                pVsys->ro._parentTid,
                (long) pVsys->ro._externRunStatus,
                (long) pVsys->ro._pConfigManager,
                (long) pVsys->ro.__channelStatusDataClientList);
        fflush(stdout);
    }
}

/* 结束线程 */
void vsys_stopThread(struct Vsys * const pVsys) {
    pVsys->rw.runStatus = 0;
    if (pVsys->ro.stid) {
#ifdef FsDebug

        FsPrintf2(5, "Stop Start.\n");
#endif
        pthread_join(pVsys->ro.stid, 0);
        pVsys->ro.stid = 0;
#ifdef FsDebug
        FsPrintf2(5, "Stop successfully.\n");
#endif
    }
}
#ifdef FsDebug

void vsys_test(void *pMonitor) {

}
#endif
///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
