#include "vsys.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <float.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../PublicTools/Fs/Calculation.h"
#include "../../PublicTools/Fs/FileTool.h"
#include "../../PublicTools/Fs/GroupSqrt.h"
#include "../../PublicTools/Fs/Log.h"
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/SystemInfo.h"
#include "../../PublicTools/Fs/Time.h"
#include "../../PublicTools/Thread/ConfigManager.h"
#include "../../PublicTools/Thread/FileStream.h"

#include "../../PublicTools/Thread/Hls.h"

#include "../../PublicTools/Thread/GB28181.h"
#include "../../PublicTools/Thread/monitor.h"
#include "../../PublicTools/Thread/Rtsp.h"
#include "../../Modules/gbsdk/gbsdk.h"
#include "../../Modules/sdkPlayBack/sdkPlayBack.h"

#include "capture.h"
#include "decode.h"
#include "vtrack.h"
#include "encode.h"
#include "record.h"
#include "vgunlinkage.h"
#include "vdomelinkage.h"
#include "targetCheck.h"
#include "vehicleDetect.h"
#include "recognition.h"
#include "inflrayObject.h"
#include "ptzObject.h"
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

/* 各模块 */
static struct {
    /* 模块指针 */
    void *p;
    /* 检查模块中指定通道的配置是否发生改变,改变返回1,未改变返回0 */
    int (*check_channel_changed)(void * const p, /* 通道编号,从0开始 */const unsigned int index
            , /* item0对应的描述节点,为空内部会重新查找 */const void *const item0, /* 调用者传入的配置中的一节点 */const void *const item
            , /* item节点的校验和 */ const unsigned long long sum, /* 调用者传入的节点路径如"vsys vsysChannel" */const char itemPath[]);
    /* 视频帧传入方法 */
    void (*frame_in__OI_4)(void * const p, /* 获取的通道编号,从0开始 */unsigned int index, /* 传入的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 传出数据帧,把数据帧的指针放在ppFrame数组中,返回获取到的数据帧数量 */
    unsigned int (*frame_out__IO_4)(void * const p, /* 获取的通道编号,从0开始 */unsigned int index, /* ppFrame可储存的数据帧数量 */unsigned int frameCount, FsObjectImageFrame * ppFrame[]);
    /* 获取模块内当前拥有的图像帧的方法 */
    unsigned int (*get_frame_count)(void * const p, /* 获取的通道编号,从0开始 */unsigned int index);
    /* 重置模块内通道对应的开方分组 */
    void (*reset_groupSqrtOut)(void * const p, /* 获取的通道编号,从0开始 */unsigned int index, /* 新组数据的起始指针 */unsigned char *const groupValue, /* 新组的参数 */ const FsGroupSqrt * const pGroupSqrt);
#define Vsys_P_Module_resetChannel(__module) (*(int*)(((char*)(__module).p)+(__module).resetChannelOffset))
    /* 模块指针p中resetChannel的偏移量 */
    unsigned short resetChannelOffset;
    /* 标记,最多两个字母 */
    char tag[2];
    /* 模块中所有通道的缓存数据帧的最大值 */
    int show_max;
} modules[12];
///////////////////////////定义私有结构结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义私有函数开始//////////////////////////////////////

static void vsys_P_createConfig(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, /* 调用函数的指针 */ const void* const p) {
    void *parent = fs_Config_node_template_add(pConfig, pConfig, "vsys", "视频系统", "hostname", "instanceName", "视频系统,支持定时器", "hostname", "hostrouter", "timerControl", 0, 0x7, 100);
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "hostname", "分布式节点IP", "所属分布式节点IP,指本机IP,127.0.0.1是无法实现分布式集群的", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "127.0.0.1", "127.0.0.1", "127.0.0.1");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "hostrouter", "分布式节点路由", "分布式节点路由,可依据不同的客户ip使用不同的路由链路,格式:'ip/掩码 网关 权重',网关可为域名,权重空时值为1", 0, 0x7, 0, 64, 100);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.11.0/24 192.168.11.214", "192.168.11.0/24 192.168.11.214", "192.168.11.0/24 192.168.11.214");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.11.0/24 g.cn", "192.168.11.0/24 g.cn", "192.168.11.0/24 g.cn");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.11.0/24 g.cn 2", "192.168.11.0/24 g.cn 2", "192.168.11.0/24 g.cn 2");
    }
    {
        fs_Config_node_string_add(pConfig, parent, "instanceName", "实例名", "实例名", 0, 0x7, 0, 64, 1);
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "timerControl", "有效时间", "有效时间,时区+时间区间", 0, 0x7, 24, 33, 2);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "+08 01-01/00:00:00 12-31/23:59:59", "+08 01-01/00:00:00 12-31/23:59:59", "每年");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-01/00:00:00 00-31/23:59:59", "+08 00-01/00:00:00 00-31/23:59:59", "每月");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/00:00:00 00-00/23:59:59", "+08 00-00/00:00:00 00-00/23:59:59", "每天");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/24:00:00 00-00/24:59:59", "+08 00-00/24:00:00 00-00/24:59:59", "每时");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 00-00/24:60:00 00-00/24:60:59", "+08 00-00/24:60:00 00-00/24:60:59", "每分");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "+08 1/00:00:00 7/23:59:59", "+08 1/00:00:00 7/23:59:59", "每周");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "ntpfreq", "ntp校时间隔(s)", "ntp校时间隔(s),建议60S", 0, 0x7, 0, 300, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "5", "5", "5");

    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "ntpserver", "ntp服务器ip", "ntp服务器ip", 0, 0x7, 0, 16, 5);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "120.25.115.20", "120.25.115.20", "120.25.115.20");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "203.107.6.88", "203.107.6.88", "203.107.6.88");
    }
    {
        void *const pNode = fs_Config_node_float_add(pConfig, parent, "realbuffertimeout", "实时视频缓存有效期(S)", "实时视频缓存有效期,单位秒", 0, 0x7, 0, 3600, 1);
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 5.0, "5秒", "5秒");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0.0, "禁用缓存", "禁用缓存");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 10.0, "10秒", "10秒");
    }
    {
        void *const pNode = fs_Config_node_float_add(pConfig, parent, "snapbuffertimeout", "快照缓存有效期(S)", "快照缓存有效期,单位秒", 0, 0x7, 0, 2592000, 1);
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0.0, "永不失效", "永不失效");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 60.0, "1分钟", "1分钟");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 300.0, "5分钟", "5分钟");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 600.0, "10分钟", "10分钟");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3600.0, "1小时", "1小时");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 7200.0, "2小时", "2小时");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 14400.0, "4小时", "4小时");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 28800.0, "8小时", "8小时");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 86400.0, "1天", "1天");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 604800.0, "1周", "1周");
        fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2592000.0, "30天", "30天");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspserver", "rtsp服务器监听地址", "rtsp服务器监听地址", 0, 0x7, 0, 21, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.0.0.0:554", "0.0.0.0:554", "0.0.0.0:554");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, parent, "rtspUdp_streamPort", "rtspUdp收流端口", "rtspUdp收流端口,为空表示禁用udp,多个使用逗号隔开,如8200,8201,8202,8203或8200,8201-8202,8203或8200-8263", 0, 0x7, 1, 128, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "8200-8201", "8200-8201", "8200-8201");
    }

    {
        void *pNode = fs_Config_node_string_add(pConfig, parent, "hlsserver", "hls服务器监听地址", "hls服务器监听地址", 0, 0x7, 0, 21, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "0.0.0.0:16880", "0.0.0.0:16880", "0.0.0.0:16880");
        if (mask & (0x1LLU << 1)) {
            pNode = fs_Config_node_float_add(pConfig, parent, "freedisk", "磁盘保留空间", "磁盘保留空间,小于1表示保留空间为绝对值,如0.1表示空余10%,大于1表示相对其它空间的相对值,如1,其它空间总和为4,表示空余20%", 0, 0x7, 0.00001, 999999999.0, 1);
            fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0.1, "保留10%", "保留10%");
            fs_Config_node_float_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 0.2, "保留20%", "保留20%");
        }
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "cronframethreshold", "计划任务帧阈值", "计划任务帧阈值,只有当系统内各模块的数据帧数量都小于此值时才可能触发系统内的计划任务,如相机状态检测,-128表示禁用", FsConfig_nodeShowType_default, 0, 0x7, -128, 1000, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 5, "5", "5");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, -128, "禁用", "禁用");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 10, "10", "10");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 20, "20", "20");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, parent, "channelstatuscheckmode", "相机状态检测方式", "相机状态检测方式", FsConfig_nodeShowType_default, 0, 0x7, 0, 100, 1);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "闲时检测", "闲时检测");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1路并发检测", "1路并发检测");
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 2, "2路并发检测", "2路并发检测");
    }

    {
        void *const pNode1 = fs_Config_node_node_add(pConfig, parent, "groupConfig", "组织结构", "组织结构", 0, 0x7);
        void *const pNode = fs_Config_node_template_add(pConfig, pNode1, "group", "组织", "groupID", "groupName", "组织", NULL, NULL, NULL, 0, 0x7, 2000);
        {
            /*
             * <Item>
             * <! --业务分组标识,编码采用D.1中的20位ID格式,扩展215类型代表业务分组-->
             * <DeviceID>65010200002150000001</DeviceID>
             * <Name>业务分组名称</Name>
             * <! --填写制定此业务分组所属的系统ID-->
             * <ParentID>65010200002000000001</ParentID>
             * </Item>
             * <Item><! --虚拟组织标识,编码采用D.1中的20位ID格式,扩展216类型代表虚拟组织-->
             * <DeviceID>65010200002160000002</DeviceID>
             * <Name>虚拟组织名称</Name>
             * <! --父节点虚拟组织ID,用于标识虚拟组织间的层级关系,若有父节点虚拟组织则填写此字段-->
             * <ParentID>65010200002160000001</ParentID>
             * <! --虚拟组织所属的业务分组ID-->
             * <BusinessGroupID>65010200002150000001</BusinessGroupID>
             * </Item> 
             */
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode, "groupID", "组织编号", "组织编号", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_default, "65010200002160000001", "65010200002160000001", "65010200002160000001");
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode, "groupName", "组织名", "组织名", 0, 0x7, 1, 128, 1),
                    FsConfig_nodeValue_optional, "视频组1", "视频组1", "视频组1");

            void *pNode2 = fs_Config_node_integer_add(pConfig, pNode, "externType", "扩展类型", "扩展类型,用于标记组织的自定义类型", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_default, 0, "默认", "默认");
            fs_Config_node_integer_add_value(pConfig, pNode2, FsConfig_nodeValue_optional, 1, "相机杆", "相机杆");

            pNode2 = fs_Config_node_string_add(pConfig, pNode, "position_jwd", "经纬度位置", "经纬度位置,经度与纬度之间用逗号隔开如 10.1,20.3", 0, 0x7, 1, 16, 1);
            fs_Config_node_string_add_value(pConfig, pNode2, FsConfig_nodeValue_default, "10.1,20.3", "10.1,20.3", "10.1,20.3");
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode2), 1, "externType", FsConfig_Condition_orAnd, "1");

            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode, "groupParent", "父组织", "父组织,为空表示无父组织", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_optional, "65010200002150000001", "65010200002150000001", "65010200002150000001");
        }
    }
    {
        void *const pNode1 = fs_Config_node_node_add(pConfig, parent, "gb28181Config", "GB28181平台", "GB28181平台接入信息", 0, 0x7);

        fs_Config_node_integer_add_value(pConfig, fs_Config_node_integer_add(pConfig, pNode1, "ipv4_localPort", "SIP服务器本地端口", "SIP服务器本地端口", FsConfig_nodeShowType_default, 0, 0x7, 1, 65535, 1),
                FsConfig_nodeValue_default, 5080, "5080", "5080");
        fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode1, "ipv4_streamPort", "接收流端口", "接收流端口(只使用偶数端口),多个使用逗号隔开,如5061,5062,5063,5064或5061,5062-5063,5064或5061-5064", 0, 0x7, 1, 128, 1),
                FsConfig_nodeValue_default, "5080", "5080", "5080");
        if (Vsys_sdkPlayBack_is_process()) {
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode1, "ipv4_streamPort_PlayBack", "回放接收流端口", "回放接收流端口(只使用偶数端口),在回放使用独立模块运行时需要使用独立端口,多个使用逗号隔开,如5061,5062,5063,5064或5061,5062-5063,5064或5061-5064", 0, 0x7, 1, 128, 1),
                    FsConfig_nodeValue_default, "16100", "16100", "16100");
        }
        {
            void *const pNode = fs_Config_node_integer_add(pConfig, pNode1, "catalog", "目录查询", "目录查询,启用后会查询并缓存目录查询结果", FsConfig_nodeShowType_default, 0, 0x7, 0, 3, 1);
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "关闭", "关闭");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 3, "启用", "启用");
            fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "仅生成连接信息", "仅生成连接信息");
        }

        {
            void * const pNode_sip = fs_Config_node_template_add(pConfig, pNode1, "sip", "sip服务器", "sip_id", "sip_ipv4", "sip服务器,同时支持向其他服务器注册和其他服务器用此sip信息向本机注册", NULL, NULL, NULL, 0, 0x7, 100);
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode_sip, "sip_ipv4", "SIP服务器地址", "SIP服务器地址,为空表示只接受注册", 0, 0x7, 7, 20, 1),
                    FsConfig_nodeValue_optional, "192.168.12.189", "192.168.12.189", "192.168.12.189");
            fs_Config_node_integer_add_value(pConfig, fs_Config_node_integer_add(pConfig, pNode_sip, "sip_ipv4Port", "SIP服务器端口", "SIP服务器端口,0表示只接受注册", FsConfig_nodeShowType_default, 0, 0x7, 0, 65535, 1),
                    FsConfig_nodeValue_optional, 5060, "5060", "5060");
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode_sip, "sip_id", "SIP服务器ID", "SIP服务器ID", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_default, "34020000002000000001", "34020000002000000001", "34020000002000000001");
            /* 13010100002000000061 */
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode_sip, "sip_domain", "SIP服务器域", "SIP服务器域", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_default, "3402000000", "3402000000", "3402000000");
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode_sip, "id", "本机ID", "本机ID", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_default, "34020000001320000001", "34020000001320000001", "34020000001320000001");
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode_sip, "sip_username", "用户名", "用户名", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_default, "34020000001320000001", "34020000001320000001", "34020000001320000001");
            fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode_sip, "sip_password", "密码", "SIP用户认证密码", 0, 0x7, 1, 20, 1),
                    FsConfig_nodeValue_default, "12345678", "12345678", "12345678");
            {
                void *const pNode = fs_Config_node_integer_add(pConfig, pNode_sip, "sipOver", "sip连接协议", "sip连接协议", FsConfig_nodeShowType_default, 0, 0x7, 1, 3, 2);
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 1 << 0, "udp", "udp");
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1 << 1, "tcp", "tcp");
                void *const conditionGroup = fs_Config_condition_group_add(pConfig, pNode);
                fs_Config_condition_add_static(pConfig, conditionGroup, 1, "sip_ipv4", FsConfig_Condition_equal_false, NULL);
                fs_Config_condition_add_static(pConfig, conditionGroup, 1, "sip_ipv4Port", FsConfig_Condition_equal_false, "0");
            }
            {
                void *const pNode = fs_Config_node_integer_add(pConfig, pNode_sip, "customConfig", "自定义配置", "自定义配置,根据对方sip的特性开启特殊处理", FsConfig_nodeShowType_default, 0, 0x7, 0, 7, 3);
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1 << 0, "目录订阅回执中callid增加错误信息", "目录订阅回执中callid增加错误信息");
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1 << 1, "注册即推送目录订阅信息", "注册即推送目录订阅信息");
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1 << 2, "不进行目录查询", "不进行目录查询");
            }
        }
    }
    {
        void *const pNode = fs_Config_node_template_add(pConfig, parent, "vsysChannel", "通道", "uuid", NULL, "通道", NULL, NULL, NULL, 0, 0x7, channelCount);
        fs_Config_node_add_property_position(pConfig, pNode, "positon", "center", "size", 0x0000FF00, "direction", "area", 0x050000FF);
        fs_Config_node_string_add_value(pConfig, fs_Config_node_string_add(pConfig, pNode, "uuid", "通道标识", "通道标识,每个通道应唯一", 0, 0x7, 1, 64, 1),
                FsConfig_nodeValue_default, "0", "0", "0");
        {
            void *const pNode1 = fs_Config_node_integer_add(pConfig, pNode, "status_init", "通道状态初始值", "通道状态初始值,-1-继承原有的占位,0-离线,1-在线", FsConfig_nodeShowType_default, 0, 0x7, -1, 1, 1);
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, -1, "-1-继承原有的占位", "-1-继承原有的占位");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0, "0-离线", "0-离线");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 1, "1-在线", "1-在线");
        }
        {
            void *const pNode1 = fs_Config_node_integer_add(pConfig, pNode, "moduleMask", "增值功能", "增值功能", FsConfig_nodeShowType_default, 0, 0x7, 0, 255, 10);
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_default, 0, "默认", "默认");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 1, "开启雷达跟踪", "开启雷达跟踪");
            if (mask & (0x1LLU << 3)) fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 2, "开启水印", "开启水印");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 4, "视频自动跟踪", "视频自动跟踪");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 8, "开启枪球联动", "开启枪球联动");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0x10U, "运动目标检测", "运动目标检测");

            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0x20U, "车辆检测", "车辆检测");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0x40U, "Inflray目标检测", "Inflray目标检测");
            fs_Config_node_integer_add_value(pConfig, pNode1, FsConfig_nodeValue_optional, 0x80U, "Inflray球机联动", "Inflray球机联动");
        }
        // 创建相机位置配置
        {
            void *const pNode1 = fs_Config_node_node_add(pConfig, pNode, "positon", "相机位置", "相机位置", 0, 0x7);
            fs_Config_node_add_property_image(pConfig, pNode1, 1, "uuid", "recordConfig rtspServerURL");
            fs_Config_node_string_add(pConfig, pNode1, "center", "中心点经纬度", "中心点经纬度,经度与纬度之间用逗号隔开如 10.1,20.3", 0, 0x7, 1, 64, 1);
            fs_Config_node_string_add(pConfig, pNode1, "size", "外接矩形大小(m)", "外接矩形大小单位米,宽高之间用逗号隔开如 1920,1080", 0, 0x7, 1, 64, 1);
            fs_Config_node_float_add(pConfig, pNode1, "direction", "方向(0-360)", "方向,单位角度,逆时针方向为正[-360-360]", 0, 0x7, -360, 360, 1);
            fs_Config_node_string_add(pConfig, pNode1, "area", "区域", "区域,以中心点为0点的坐标点,单位米 (0,0)(1,1)(2,3)", 0, 0x7, 1, 1024, 1);
        }
        // 创建相机距离标定
        {
            void *const pNode1 = fs_Config_node_node_add(pConfig, pNode, "distance", "相机距离标定", "相机位置距离标定", 0, 0x7);
            fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode1), 1, "moduleMask", FsConfig_Condition_orAnd, "64");
            {
                void* const pNode = fs_Config_node_integer_add(pConfig, pNode1, "distance_enable", "是否启用", "是否启用", FsConfig_nodeShowType_default, 0, 0x7, 0, 1, 1);
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0, "0-禁用", "0-禁用");
                fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_optional, 1, "1-启用", "1-启用");
            }
            {
                void* const pNode = fs_Config_node_integer_add(pConfig, pNode1, "start_x", "水平线起点坐标X", "水平线起点坐标X", FsConfig_nodeShowType_default, 0, 0x7, 0, 128000, 1);
                fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "distance_enable", FsConfig_Condition_equal, "1");
            }
            {
                void* const pNode = fs_Config_node_integer_add(pConfig, pNode1, "start_y", "水平线起点坐标Y", "水平线起点坐标Y", FsConfig_nodeShowType_default, 0, 0x7, 0, 128000, 1);
                fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "distance_enable", FsConfig_Condition_equal, "1");
            }
            {
                void* const pNode = fs_Config_node_integer_add(pConfig, pNode1, "end_x", "水平线终点坐标X", "水平线终点坐标Y", FsConfig_nodeShowType_default, 0, 0x7, 0, 128000, 1);
                fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "distance_enable", FsConfig_Condition_equal, "1");
            }
            {
                void* const pNode = fs_Config_node_integer_add(pConfig, pNode1, "end_y", "水平线终点坐标Y", "水平线终点坐标Y", FsConfig_nodeShowType_default, 0, 0x7, 0, 128000, 1);
                fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "distance_enable", FsConfig_Condition_equal, "1");
            }
            {
                void* const pNode = fs_Config_node_float_add(pConfig, pNode1, "device_height", "设备安装高度(米)", "设备安装高度(米)", 0, 0x7, 0.0, 100.0, 1);
                fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "distance_enable", FsConfig_Condition_equal, "1");
            }
            {
                void* const pNode = fs_Config_node_float_add(pConfig, pNode1, "device_field_angle", "设备垂直视场角(度)", "设备垂直视场角(度)", 0, 0x7, 6.4, 12.8, 1);
                fs_Config_condition_add_static(pConfig, fs_Config_condition_group_add(pConfig, pNode), 1, "distance_enable", FsConfig_Condition_equal, "1");
            }
        }
        // 创建相机配置
        capture_createConfig(pConfig, mask, channelCount, pNode);
        // 创建解码配置
        decode_createConfig(pConfig, pNode);

        gunlinkage_createConfig(pConfig, pNode);
        domelinkage_createConfig(pConfig, pNode);
        //运动跟踪配置
        vtrack_createConfig(pConfig, pNode);
        // 运动目标检测配置
        targetCheck_createConfig(pConfig, mask, channelCount, pNode);
        // 车辆检测配置
        vehicleDetect_createConfig(pConfig, mask, channelCount, pNode);
        // Inflray目标检测配置
        inflrayObject_createConfig(pConfig, mask, channelCount, pNode);
        // 报警联动配置
        ptzObject_createConfig(pConfig, mask, channelCount, pNode);
        // 识别配置
        recognition_createConfig(pConfig, mask, channelCount, pNode);
        // 创建编码配置
        encode_createConfig(pConfig, mask, channelCount, pNode);
        // 创建数据记录配置
        record_createConfig(pConfig, mask, channelCount, pNode);
    }
}

/* type为1时返回功能掩码计算的模块掩码 */

static unsigned int vsys_P_module_init(struct Vsys * const pVsys_t0, /* 初始化类型,0-模块初始化,1-使用功能掩码计算模块掩码 */const unsigned char type, FsMemeryFree * const pMemeryFree_t0
        , FsRouter * const pRouter_cluster_t0
        , signed char *const externRunStatus_t0, /* 开方分组 */FsGroupSqrtPthreadSafety * const pGroupSqrt_t0
        , /* rtsp对象 */ Rtsp * const pRtsp_t0
        , /* hls服务器对象 */ Hls * const pHls_t0, /* GB28181对象 */ GB28181 * const pGB28181_t0, unsigned long long moduleMask_t1
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    unsigned int rst = 0;
    // 采集
#define Vsys_P_module_Capture_index 0
#ifndef __vsys_P_module_init_pCapture 
#define pCapture ((struct Capture*)modules[Vsys_P_module_Capture_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Capture_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))capture_check_channel_changed;
        modules[Vsys_P_module_Capture_index].frame_in__OI_4 = NULL;
        modules[Vsys_P_module_Capture_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))capture_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Capture_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) capture_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Capture_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))capture_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Capture_index].resetChannelOffset = FsPointer_to_long(&((struct Capture*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Capture_index].tag[0] = 'C', modules[Vsys_P_module_Capture_index].tag[1] = 0;
        modules[Vsys_P_module_Capture_index].show_max = 0;
        modules[Vsys_P_module_Capture_index].p = capture_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "Capture", pVsys_t0->ro._pConfigManager
                , 128 * pVsys_t0->ro._threadMulti, pGroupSqrt_t0, pRtsp_t0
                , pGB28181_t0);
        capture_startThread(pCapture, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))capture_stopThread, (void (*)(void*)) capture_delete__OI, pCapture);
    } else if (0 == Capture_Mask || (moduleMask_t1 & Capture_Mask) != 0)rst |= 1LLU << Vsys_P_module_Capture_index;
#endif
    // 解码
#define Vsys_P_module_Decode_index 1
#ifndef __vsys_P_module_init_pDecode 
#define pDecode ((struct Decode*)modules[Vsys_P_module_Decode_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Decode_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))decode_check_channel_changed;
        modules[Vsys_P_module_Decode_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))decode_item_frame_in__OI_4;
        modules[Vsys_P_module_Decode_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))decode_item_frame_out__IO_4;
        modules[Vsys_P_module_Decode_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) decode_item_get_frame_count;
        modules[Vsys_P_module_Decode_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))decode_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Decode_index].resetChannelOffset = FsPointer_to_long(&((struct Decode*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Decode_index].tag[0] = 'D', modules[Vsys_P_module_Decode_index].tag[1] = 0;
        modules[Vsys_P_module_Decode_index].show_max = 0;
        modules[Vsys_P_module_Decode_index].p = decode_new__IO("Decode", pVsys_t0->ro._pMonitor, systemThreadTid, externRunStatus_t0, pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        decode_startThread(pDecode, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))decode_stopThread, (void (*)(void*)) decode_delete__OI, pDecode);
    } else if (0 == Decode_Mask || (moduleMask_t1 & Decode_Mask) != 0)rst |= 1LLU << Vsys_P_module_Decode_index;
#endif
    // 枪球联动检测
    // 枪机
#define Vsys_P_module_Gunlinkage_index 2
#ifndef __vsys_P_module_init_pGunlinkage 
#define pGunlinkage ((struct Gunlinkage*)modules[Vsys_P_module_Gunlinkage_index].p)       
    if (0 == type) {
        modules[Vsys_P_module_Gunlinkage_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))gunlinkage_check_channel_changed;
        modules[Vsys_P_module_Gunlinkage_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))gunlinkage_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_Gunlinkage_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))gunlinkage_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Gunlinkage_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) gunlinkage_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Gunlinkage_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))gunlinkage_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Gunlinkage_index].resetChannelOffset = FsPointer_to_long(&((struct Gunlinkage*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Gunlinkage_index].tag[0] = 'G', modules[Vsys_P_module_Gunlinkage_index].tag[1] = 0;
        modules[Vsys_P_module_Gunlinkage_index].show_max = 0;
        modules[Vsys_P_module_Gunlinkage_index].p = gunlinkage_new__IO("Gunlinkage", pVsys_t0->ro._pMonitor, systemThreadTid, externRunStatus_t0, "/fs/project/data/vsys/", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        gunlinkage_startThread(pGunlinkage, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))gunlinkage_stopThread, (void (*)(void*))gunlinkage_delete__OI, pGunlinkage);
    } else if (0 == Gunlinkage_Mask || (moduleMask_t1 & Gunlinkage_Mask) != 0)rst |= 1LLU << Vsys_P_module_Gunlinkage_index;
#endif
    // 球机
#define Vsys_P_module_Domelinkage_index 3
#ifndef __vsys_P_module_init_pDomelinkage 
#define pDomelinkage ((struct Domelinkage*)modules[Vsys_P_module_Domelinkage_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Domelinkage_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))domelinkage_check_channel_changed;
        modules[Vsys_P_module_Domelinkage_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))domelinkage_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_Domelinkage_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))domelinkage_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Domelinkage_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) domelinkage_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Domelinkage_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))domelinkage_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Domelinkage_index].resetChannelOffset = FsPointer_to_long(&((struct Domelinkage*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Domelinkage_index].tag[0] = 'B', modules[Vsys_P_module_Domelinkage_index].tag[1] = 0;
        modules[Vsys_P_module_Domelinkage_index].show_max = 0;
        modules[Vsys_P_module_Domelinkage_index].p = domelinkage_new__IO("Domelinkage", pVsys_t0->ro._pMonitor, systemThreadTid, externRunStatus_t0, "/fs/project/data/vsys/", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        domelinkage_startThread(pDomelinkage, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))domelinkage_stopThread, (void (*)(void*)) domelinkage_delete__OI, pDomelinkage);
    } else if (0 == Domelinkage_Mask || (moduleMask_t1 & Domelinkage_Mask) != 0)rst |= 1LLU << Vsys_P_module_Domelinkage_index;
#endif
    // 目标跟踪检测
#define Vsys_P_module_Vtrack_index 4    
#ifndef __vsys_P_module_init_pVtrack 
#define pVtrack ((struct Vtrack*)modules[Vsys_P_module_Vtrack_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Vtrack_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))vtrack_check_channel_changed;
        modules[Vsys_P_module_Vtrack_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))vtrack_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_Vtrack_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))vtrack_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Vtrack_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) vtrack_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Vtrack_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))vtrack_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Vtrack_index].resetChannelOffset = FsPointer_to_long(&((struct Vtrack*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Vtrack_index].tag[0] = 'T', modules[Vsys_P_module_Vtrack_index].tag[1] = 0;
        modules[Vsys_P_module_Vtrack_index].show_max = 0;
        modules[Vsys_P_module_Vtrack_index].p = vtrack_new__IO("Vtrack", pVsys_t0->ro._pMonitor, systemThreadTid, externRunStatus_t0, "/fs/project/data/vsys/", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        vtrack_startThread(pVtrack, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))vtrack_stopThread, (void (*)(void*)) vtrack_delete__OI, pVtrack);
    } else if (0 == Vtrack_Mask || (moduleMask_t1 & Vtrack_Mask) != 0)rst |= 1LLU << Vsys_P_module_Vtrack_index;
#endif
    // 目标检测
#define Vsys_P_module_TargetCheck_index 5
#ifndef __vsys_P_module_init_pTargetCheck 
#define pTargetCheck ((struct TargetCheck*)modules[Vsys_P_module_TargetCheck_index].p)
    if (0 == type) {
        modules[Vsys_P_module_TargetCheck_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))targetCheck_check_channel_changed;
        modules[Vsys_P_module_TargetCheck_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))targetCheck_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_TargetCheck_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))targetCheck_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_TargetCheck_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) targetCheck_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_TargetCheck_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))targetCheck_item_reset_groupSqrtOut;
        modules[Vsys_P_module_TargetCheck_index].resetChannelOffset = FsPointer_to_long(&((struct TargetCheck*) 0)->rw._resetChannel);
        modules[Vsys_P_module_TargetCheck_index].tag[0] = 'T', modules[Vsys_P_module_TargetCheck_index].tag[1] = 'C';
        modules[Vsys_P_module_TargetCheck_index].show_max = 0;
        modules[Vsys_P_module_TargetCheck_index].p = targetCheck_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "TargetCheck", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        targetCheck_startThread(pTargetCheck, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*)) targetCheck_stopThread, (void (*)(void*)) targetCheck_delete__OI, pTargetCheck);
    } else if (0 == TargetCheck_Mask || (moduleMask_t1 & TargetCheck_Mask) != 0)rst |= 1LLU << Vsys_P_module_TargetCheck_index;
#endif   
    // 车辆检测
#define Vsys_P_module_VehicleDetect_index 6
#ifndef __vsys_P_module_init_pVehicleDetect 
#define pVehicleDetect ((struct VehicleDetect*)modules[Vsys_P_module_VehicleDetect_index].p)
    if (0 == type) {
        modules[Vsys_P_module_VehicleDetect_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))vehicleDetect_check_channel_changed;
        modules[Vsys_P_module_VehicleDetect_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))vehicleDetect_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_VehicleDetect_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))vehicleDetect_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_VehicleDetect_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) vehicleDetect_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_VehicleDetect_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))vehicleDetect_item_reset_groupSqrtOut;
        modules[Vsys_P_module_VehicleDetect_index].resetChannelOffset = FsPointer_to_long(&((struct VehicleDetect*) 0)->rw._resetChannel);
        modules[Vsys_P_module_VehicleDetect_index].tag[0] = 'V', modules[Vsys_P_module_VehicleDetect_index].tag[1] = 'D';
        modules[Vsys_P_module_VehicleDetect_index].show_max = 0;
        modules[Vsys_P_module_VehicleDetect_index].p = vehicleDetect_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "VehicleDetect", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        vehicleDetect_startThread(pVehicleDetect, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*)) vehicleDetect_stopThread, (void (*)(void*)) vehicleDetect_delete__OI, pVehicleDetect);
    } else if (0 == VehicleDetect_Mask || (moduleMask_t1 & VehicleDetect_Mask) != 0)rst |= 1LLU << Vsys_P_module_VehicleDetect_index;
#endif
    // Inflray目标检测
#define Vsys_P_module_InflrayObject_index 7
#ifndef __vsys_P_module_init_InflrayObject 
#define pInflrayObject ((struct InflrayObject*)modules[Vsys_P_module_InflrayObject_index].p)
    if (0 == type) {
        modules[Vsys_P_module_InflrayObject_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))inflrayObject_check_channel_changed;
        modules[Vsys_P_module_InflrayObject_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))inflrayObject_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_InflrayObject_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))inflrayObject_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_InflrayObject_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) inflrayObject_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_InflrayObject_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))inflrayObject_item_reset_groupSqrtOut;
        modules[Vsys_P_module_InflrayObject_index].resetChannelOffset = FsPointer_to_long(&((struct InflrayObject*) 0)->rw._resetChannel);
        modules[Vsys_P_module_InflrayObject_index].tag[0] = 'I', modules[Vsys_P_module_InflrayObject_index].tag[1] = 'O';
        modules[Vsys_P_module_InflrayObject_index].show_max = 0;
        modules[Vsys_P_module_InflrayObject_index].p = inflrayObject_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "InflrayObject", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0, pShareBuffer);
        inflrayObject_startThread(pInflrayObject, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*)) inflrayObject_stopThread, (void (*)(void*)) inflrayObject_delete__OI, pInflrayObject);
    } else if (0 == InflrayObject_Mask || (moduleMask_t1 & InflrayObject_Mask) != 0)rst |= 1LLU << Vsys_P_module_InflrayObject_index;
#endif
    // 报警联动
#define Vsys_P_module_PtzObject_index 8
#ifndef __vsys_P_module_init_PtzObject 
#define pPtzObject ((struct PtzObject*)modules[Vsys_P_module_PtzObject_index].p)
    if (0 == type) {
        modules[Vsys_P_module_PtzObject_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))ptzObject_check_channel_changed;
        modules[Vsys_P_module_PtzObject_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))ptzObject_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_PtzObject_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))ptzObject_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_PtzObject_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) ptzObject_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_PtzObject_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))ptzObject_item_reset_groupSqrtOut;
        modules[Vsys_P_module_PtzObject_index].resetChannelOffset = FsPointer_to_long(&((struct PtzObject*) 0)->rw._resetChannel);
        modules[Vsys_P_module_PtzObject_index].tag[0] = 'P', modules[Vsys_P_module_PtzObject_index].tag[1] = 'O';
        modules[Vsys_P_module_PtzObject_index].show_max = 0;
        modules[Vsys_P_module_PtzObject_index].p = ptzObject_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "PtzObject", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0, pShareBuffer);
        ptzObject_startThread(pPtzObject, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*)) ptzObject_stopThread, (void (*)(void*)) ptzObject_delete__OI, pPtzObject);
    } else if (0 == PtzObject_Mask || (moduleMask_t1 & PtzObject_Mask) != 0)rst |= 1LLU << Vsys_P_module_PtzObject_index;
#endif
    // 识别
#define Vsys_P_module_Recognition_index 9
#ifndef __vsys_P_module_init_pRecognition 
#define pRecognition ((struct Recognition*)modules[Vsys_P_module_Recognition_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Recognition_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))recognition_check_channel_changed;
        modules[Vsys_P_module_Recognition_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))recognition_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_Recognition_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))recognition_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Recognition_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) recognition_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Recognition_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))recognition_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Recognition_index].resetChannelOffset = FsPointer_to_long(&((struct Recognition*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Recognition_index].tag[0] = 'R', modules[Vsys_P_module_Recognition_index].tag[1] = 'G';
        modules[Vsys_P_module_Recognition_index].show_max = 0;
        modules[Vsys_P_module_Recognition_index].p = recognition_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "Recognition", pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        recognition_startThread(pRecognition, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))recognition_stopThread, (void (*)(void*))recognition_delete__OI, pRecognition);
    } else if (0 == Recognition_Mask || (moduleMask_t1 & Recognition_Mask) != 0)rst |= 1LLU << Vsys_P_module_Recognition_index;
#endif
    // 编码
#define Vsys_P_module_Encode_index 10
#ifndef __vsys_P_module_init_pEncode 
#define pEncode ((struct Encode*)modules[Vsys_P_module_Encode_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Encode_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))encode_check_channel_changed;
        modules[Vsys_P_module_Encode_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))encode_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_Encode_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))encode_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Encode_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) encode_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Encode_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))encode_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Encode_index].resetChannelOffset = FsPointer_to_long(&((struct Encode*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Encode_index].tag[0] = 'E', modules[Vsys_P_module_Encode_index].tag[1] = 0;
        modules[Vsys_P_module_Encode_index].show_max = 0;
        modules[Vsys_P_module_Encode_index].p = encode_new__IO("Encode", pVsys_t0->ro._pMonitor, systemThreadTid, externRunStatus_t0, pVsys_t0->ro._pConfigManager, pGroupSqrt_t0);
        encode_startThread(pEncode, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))encode_stopThread, (void (*)(void*)) encode_delete__OI, pEncode);
    } else if (0 == Encode_Mask || (moduleMask_t1 & Encode_Mask) != 0)rst |= 1LLU << Vsys_P_module_Encode_index;
#endif
    // 记录
#define Vsys_P_module_Record_index 11
#ifndef __vsys_P_module_init_pRecord 
#define pRecord ((struct Record*)modules[Vsys_P_module_Record_index].p)
    if (0 == type) {
        modules[Vsys_P_module_Record_index].check_channel_changed = (int (*)(void*, unsigned int, const void*, const void*, long long unsigned int, const char*))record_check_channel_changed;
        modules[Vsys_P_module_Record_index].frame_in__OI_4 = (void (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame * [], FsObjectBaseBuffer * const, FsShareBuffer * const))record_item_frame_in_pthreadSafety__OI_4;
        modules[Vsys_P_module_Record_index].frame_out__IO_4 = (unsigned int (*)(void * const, unsigned int, unsigned int, FsObjectImageFrame *[]))record_item_frame_out_pthreadSafety__IO_4;
        modules[Vsys_P_module_Record_index].get_frame_count = (unsigned int (*)(void * const, unsigned int)) record_item_get_frame_count_pthreadSafety;
        modules[Vsys_P_module_Record_index].reset_groupSqrtOut = (void (*)(void*, unsigned int, unsigned char*, const FsGroupSqrt*))record_item_reset_groupSqrtOut;
        modules[Vsys_P_module_Record_index].resetChannelOffset = FsPointer_to_long(&((struct Record*) 0)->rw._resetChannel);
        modules[Vsys_P_module_Record_index].tag[0] = 'R', modules[Vsys_P_module_Record_index].tag[1] = 0;
        modules[Vsys_P_module_Record_index].show_max = 0;
        modules[Vsys_P_module_Record_index].p = record_new__IO(pVsys_t0->ro._pMonitor, externRunStatus_t0, systemThreadTid, "Record", "/fs/project/data/vsys/", pRtsp_t0
                , pHls_t0, pGB28181_t0, pVsys_t0->ro._pConfigManager
                , 64 * pVsys_t0->ro._threadMulti, pGroupSqrt_t0);
        record_startThread(pRecord, -1);
        fs_memeryFree_add_thread(pMemeryFree_t0, (void (*)(void*))record_stopThread, (void (*)(void*)) record_delete__OI, pRecord);
    } else if (0 == Record_Mask || (moduleMask_t1 & Record_Mask) != 0)rst |= 1LLU << Vsys_P_module_Record_index;
#endif
    return rst;
}

/* 从配置获取通道,返回通道的链表指针 */
static FsObjectList* vsys_P_get_channelCount__IO(/* 通过rst_pVsysChannel0返回vsysChannel0,通道的描述节点 */const void * * const rst_pVsysChannel0, /* 计划任务帧阈值,只有当系统内各模块的数据帧数量都小于此值时才可能触发系统内的计划任务,如相机状态检测 */ short *const pCronfremethreshold
        , /* 相机状态检测方式,设置channelStatusCheckExtra值的最小值 */ unsigned short *const pCannelstatuscheckmode

        , /* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
#define __get_channelCount_vsys_vsysChannel_in_vsys
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

static int vsys_private_ConfigUpdate_and_get_keepspaceConfig_selector(const struct dirent * pDirent) {
    if (DT_DIR == pDirent->d_type && (pDirent->d_name[0] != '.' || (pDirent->d_name[1] != 0 && (pDirent->d_name[1] != '.' || pDirent->d_name[2] != 0))))return 1;
    else return 0;
}

/* 更新配置并获取磁盘配置 */
static FsStringBuilder * vsys_P_ConfigUpdate_and_get_keepspaceConfig__IO(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList
        , /* 共享buffer,不能为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pConfig)return NULL;
    const void *parent0;
    const void *parent;
    {
        /* 通道数 */
        parent0 = pConfig;
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0, ipList, 0, "vsys");
        if (NULL == list)return NULL;
        parent = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
    }
    const void *vsysChannel0 = parent0;
    FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsysChannel0, parent, 0, NULL, 0, "vsysChannel");
    if (NULL == list)return NULL;
    /* 硬盘上已存在的文件夹 */
    struct dirent **namelist = NULL;
    int dirCount = scandir("/fs/project/data/vsys/", &namelist, vsys_private_ConfigUpdate_and_get_keepspaceConfig_selector, 0);
    char *const buffer = Fs_ShareBuffer_simple_fsMalloc(pShareBuffer, sizeof ("/incident,65535\r\n"), 1);
    FsStringBuilder * const rst = fs_StringBuilder_new__IO();
    const double free_f = fs_Config_node_float_get_first(pConfig, parent0, parent, "freedisk", 0.1, NULL);
    fs_StringBuilder_apend(rst, "[setup]\r\n");
#if 1
    fs_StringBuilder_apend_string(rst, FsStringLenData("dir_1=/fs/project/data/syslog,1\r\n"));
    unsigned int index = 2;
    unsigned long proportionTotall = 1;
#else
    unsigned int index = 1;
    unsigned long proportionTotall = 0;
#endif 
    {
        void **ppNode = list->pNode + list->startIndex;
        unsigned int uj = list->nodeCount;
        while (uj-- > 0) {
            const void *const vsysChannel = *ppNode++;
            const void *recordConfig0 = vsysChannel0;
            const void *const recordConfig = fs_Config_node_get_first(pConfig, &recordConfig0, vsysChannel, "recordConfig");
            const int recordVideoMode = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "recordVideoMode", 0, NULL);
            if (0 == recordVideoMode)continue;
            const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
            if (namelist) {
                int i;
                for (i = 0; i < dirCount; i++) {
                    if (strcmp(namelist[i]->d_name, uuid->buffer) == 0) {
                        free(namelist[i]);
                        dirCount--;
                        if (i < dirCount) memmove(namelist + i, namelist + i + 1, (dirCount - i) * sizeof (void*));
                        break;
                    }
                }
            }
            {
                {
                    const int len = sprintf(buffer, "dir_%u=", index++);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                fs_StringBuilder_apend_string(rst, FsStringLenData("/fs/project/data/vsys/"));
                fs_StringBuilder_apend_string(rst, uuid->lenth - 1, uuid->buffer);
                const unsigned short proportion = fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "realVideoProportion", 1, NULL);
                proportionTotall += proportion;
                {
                    const int len = sprintf(buffer, "/video,%hu\r\n", proportion);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
            }
            {
                {
                    const int len = sprintf(buffer, "dir_%u=", index++);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                fs_StringBuilder_apend_string(rst, FsStringLenData("/fs/project/data/vsys/"));
                fs_StringBuilder_apend_string(rst, uuid->lenth - 1, uuid->buffer);
                const unsigned short proportion = recordVideoMode > 2 ? 0 : fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "watchProportion", 1, NULL);
                proportionTotall += proportion;
                {
                    const int len = sprintf(buffer, "/watch,%hu\r\n", proportion);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
            }
            {
                {
                    const int len = sprintf(buffer, "dir_%u=", index++);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                fs_StringBuilder_apend_string(rst, FsStringLenData("/fs/project/data/vsys/"));
                fs_StringBuilder_apend_string(rst, uuid->lenth - 1, uuid->buffer);
                const unsigned short proportion = recordVideoMode > 2 ? 0 : fs_Config_node_integer_get_first(pConfig, recordConfig0, recordConfig, "incidentProportion", 1, NULL);
                proportionTotall += proportion;
                {
                    const int len = sprintf(buffer, "/incident,%hu\r\n", proportion);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
            }
        }
    }
    fs_ObjectList_delete__OI(list);
    if (namelist) {
        int i;
        for (i = 0; i < dirCount; i++) {
            {
                const int len = sprintf(buffer, "dir_%u=", index++);
                fs_StringBuilder_apend_string(rst, len, buffer);
            }
            fs_StringBuilder_apend_string(rst, FsStringLenData("/fs/project/data/vsys/"));
            fs_StringBuilder_apend(rst, namelist[i]->d_name);
            fs_StringBuilder_apend_string(rst, FsStringLenData(",0\r\n"));
            free(namelist[i]);
        }
        free(namelist);
    }
    if (free_f < 1.0) {
        proportionTotall = free_f / (1.0 - free_f) * proportionTotall;
        if (proportionTotall < 1)proportionTotall = 1;
    } else proportionTotall = free_f;
    {
        const int len = sprintf(buffer, "free=%lu\r\n", proportionTotall);
        fs_StringBuilder_apend_string(rst, len, buffer);
    }
    return rst;
}

/* 获取上传数据配置 */
static FsStringBuilder * vsys_private_get_uploadConfig__IO(/* 可为空 */FsConfig * const pConfig, /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return NULL;
    const void *parent0 = pConfig;
    /* 通道数 */
    FsObjectList * list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0, ipList, 0, "vsys");
    if (NULL == list)return NULL;
    const void *parent = list->pNode[list->startIndex];
    fs_ObjectList_delete__OI(list);
    list = fs_Config_node_template__IO(pConfig, &parent0, parent, 0, NULL, 0, "vsysChannel");
    if (NULL == list)return NULL;
    FsStringBuilder * const rst = fs_StringBuilder_new__IO();
    fs_StringBuilder_apend(rst, "[setup]\r\n");
    unsigned int uj = 0, index = 1;
    void **ppNode = list->pNode + list->startIndex;
    for (; uj < list->nodeCount; uj++) {
        parent = *ppNode++;
        if (fs_Config_node_integer_get_first(pConfig, parent0, parent, "recordConfig recordVideoMode", 0, NULL) == 0)continue;
        const void *dataServer0 = parent0;
        FsObjectList * const list1 = fs_Config_node_template__IO(pConfig, &dataServer0, parent, 0, NULL, 0, "recordConfig dataServer");
        if (list1) {
            const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, parent0, parent, "uuid", NULL);
            void **ppNode_dataServer = list1->pNode + list1->startIndex;
            unsigned int ui = list1->nodeCount;
            while (ui-- > 0) {
                void *const dataServer = *ppNode_dataServer++;
                char buffer[32];
                {
                    const int len = sprintf(buffer, "dir_%u=", index++);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                fs_StringBuilder_apend_string(rst, FsStringLenData("/fs/project/data/vsys/"));
                fs_StringBuilder_apend_string(rst, uuid->lenth - 1, uuid->buffer);
                fs_StringBuilder_apend_string(rst, FsStringLenData("/watch,"));
                const FsString * const name = fs_Config_node_string_get_first_String(pConfig, dataServer0, dataServer, "name", NULL);
                fs_StringBuilder_apend_string(rst, name->lenth - 1, name->buffer);
                {
                    const int len = sprintf(buffer, ",%lld,", fs_Config_node_integer_get_first(pConfig, dataServer0, dataServer, "uploadType", 2, NULL));
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                const FsString * const addr = fs_Config_node_string_get_first_String(pConfig, dataServer0, dataServer, "addr", NULL);
                fs_StringBuilder_apend_string(rst, addr->lenth - 1, addr->buffer);
                {
                    const int len = sprintf(buffer, ",%lld\r\n", fs_Config_node_integer_get_first(pConfig, dataServer0, dataServer, "historyTime", 3600 * 24, NULL));
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                {
                    const int len = sprintf(buffer, "dir_%u=", index++);
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                fs_StringBuilder_apend_string(rst, FsStringLenData("/fs/project/data/vsys/"));
                fs_StringBuilder_apend_string(rst, uuid->lenth - 1, uuid->buffer);
                fs_StringBuilder_apend_string(rst, FsStringLenData("/incident,"));
                fs_StringBuilder_apend_string(rst, name->lenth - 1, name->buffer);
                {
                    const int len = sprintf(buffer, ",%lld,", fs_Config_node_integer_get_first(pConfig, dataServer0, dataServer, "uploadType", 2, NULL));
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
                fs_StringBuilder_apend_string(rst, addr->lenth - 1, addr->buffer);
                {
                    const int len = sprintf(buffer, ",%lld\r\n", fs_Config_node_integer_get_first(pConfig, dataServer0, dataServer, "historyTime", 3600 * 24, NULL));
                    fs_StringBuilder_apend_string(rst, len, buffer);
                }
            }
            fs_ObjectList_delete__OI(list1);
        }
    }
    fs_ObjectList_delete__OI(list);
    return rst;
}

/* 初始化通道状态,有修改返回1,无修改返回0 */
static void vsys_P_channelStatus_init(/* 可为空 */FsConfig * const pConfig, /* 储存本地通道节点的链表 */ FsObjectList * const localItemList, const void *const vsysChannel0
        , unsigned char *const channelStatus, const unsigned char channelStatusIndex) {
    if (NULL == pConfig) return;
    const unsigned int localItemList_count = localItemList->nodeCount;
    if (0 == localItemList_count) return;
    unsigned int ui = 0;
    const struct FsEbml_node ** ppNode = (const struct FsEbml_node **) localItemList->pNode + localItemList->startIndex;
    const struct FsEbml_node * const type = (struct FsEbml_node *) fs_Config_node_get_type(pConfig, vsysChannel0, "status_init");
    for (; ui < localItemList_count; ui++) {
        const struct FsEbml_node * const vsysChannel = *ppNode++;
        const long long status_init = fs_Config_node_integer_get_first(pConfig, vsysChannel0, vsysChannel, "status_init", -1, type);
        if (-1 == status_init) {
            channelStatus[ui] &= 0xFEU; // 标记其未探测过
        } else if (1 == status_init) {
            channelStatus[ui] = (channelStatus[ui]&0x06) | channelStatusIndex;
        } else {
            channelStatus[ui] &= 0xF6U;
        }
    }
}

static void vsys_private_update_ntpConfig(/* 可为空 */FsConfig * const pConfig, const char ntpConfig[], /* 本地ip地址 */const FsArray * const ipList) {
    if (NULL == pConfig)return;
    const void* parent0 = pConfig;
    FsObjectList * list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0, ipList, 0, "vsys");
    if (NULL == list)return;
    const void *parent = list->pNode[list->startIndex];
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
        const FsString * const ntpfreq = fs_Config_node_string_get_first_String(pConfig, parent0, parent, "ntpfreq", NULL);
        if (ntpfreq) {
            if (fwrite(FsStringDataLen("ntpfreq="), 1, fd) != 1) {
                FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                fflush(stdout);
                fclose(fd);
                return;
            }
            if (fwrite(ntpfreq->buffer, ntpfreq->lenth - 1, 1, fd) != 1) {
                FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                fflush(stdout);
                fclose(fd);
                return;
            }
        }
    }
    {
        FsConfigResult_string *ntpserver = fs_Config_node_string_get__IO(pConfig, parent0, parent, "ntpserver", NULL);
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
            fsFree(ntpserver);
            if (fputc('\n', fd) < 0) {

                FsPrintf(1, "Write the file:\"%s\" error:\"%s\".\n", ntpConfig, strerror(errno));
                fflush(stdout);
            }
        }
    }
    fclose(fd);
}
static void vsys_P_update_server(/* 可为空 */FsConfig * const pConfig, Rtsp * const pRtsp
        , Hls * const pHls, /* 本地ip地址 */const FsArray * const ipList
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    const void *const vsys = fs_Config_node_template_get(pConfig, &vsys0, pConfig, 0, ipList, 0, "vsys", 0);
    if (NULL == vsys)return;
    const float realbuffertimeout = fs_Config_node_float_get_first(pConfig, vsys0, vsys, "realbuffertimeout", 0.0, NULL);
    /////////////////////////////////rtsp///////////////////////////////////////
    {
        {
            const char* const server = fs_Config_node_string_get_first(pConfig, vsys0, vsys, "rtspserver", NULL);
            if (server) {
                const char * ps = strchr(server, ':');
                unsigned int ipv4;
                unsigned short ipv4Port;
                if (ps) {
                    ipv4 = fs_ipv4_network_get(ps - server, server);
                    if (FS_INVALID_IPV4 == ipv4) {
                        FsPrintf(1, "Invalid rtspserver:\"%s\".\n", server);
                        ipv4 = 0;
                    }
                    ipv4Port = atoi(ps + 1);
                    if (ipv4Port < 1) {
                        FsPrintf(1, "Invalid rtspserver:\"%s\".\n", server);
                        ipv4Port = 554;
                    }
                } else {
                    ipv4 = fs_ipv4_network_get(strlen(server), server);
                    if (FS_INVALID_IPV4 == ipv4) {
                        FsPrintf(1, "Invalid rtspserver:\"%s\".\n", server);
                        ipv4 = 0;
                    }
                    ipv4Port = 554;
                }
                const char *const rtspUdp_streamPort = fs_Config_node_string_get_first(pConfig, vsys0, vsys, "rtspUdp_streamPort", NULL);
                rtsp_set_parameter_thread_pthreadSafety(pRtsp, ipv4, ipv4Port, rtspUdp_streamPort, pShareBuffer);
            }
        }
        if (pRtsp->rw._realBufferTimeOut != realbuffertimeout) rtsp_set_parameter_pthreadSafety(pRtsp, realbuffertimeout);
    }
    //////////////////////////////////hls///////////////////////////////////////
    {
        {
            const char * const server = fs_Config_node_string_get_first(pConfig, vsys0, vsys, "hlsserver", NULL);
            if (server) {
                const char * ps = strchr(server, ':');
                unsigned int ipv4;
                unsigned short ipv4Port;
                if (ps) {
                    ipv4 = fs_ipv4_network_get(ps - server, server);
                    if (FS_INVALID_IPV4 == ipv4) {
                        FsPrintf(1, "Invalid hlsserver:\"%s\".\n", server);
                        ipv4 = 0;
                    }
                    ipv4Port = atoi(ps + 1);
                    if (ipv4Port < 1) {
                        FsPrintf(1, "Invalid hlsserver:\"%s\".\n", server);
                        ipv4Port = 16880;
                    }
                } else {
                    ipv4 = fs_ipv4_network_get(strlen(server), server);
                    if (FS_INVALID_IPV4 == ipv4) {
                        FsPrintf(1, "Invalid hlsserver:\"%s\".\n", server);
                        ipv4 = 0;
                    }
                    ipv4Port = 16880;
                }
                if (pHls->rw._ipv4 != ipv4 || pHls->rw._ipv4Port != ipv4Port)
                    hls_set_parameter_thread_pthreadSafety(pHls, ipv4, ipv4Port);
            }
        }
        if (pHls->rw._realBufferTimeOut != realbuffertimeout) hls_set_parameter_pthreadSafety(pHls, realbuffertimeout);
    }
    ////////////////////////////////////////////////////////////////////////////   
}


static void vsys_P_update_router(/* 可为空 */FsConfig * const pConfig, FsRouter * const pRouter_cluster
        , /* 本地ip地址 */const FsArray * const ipList) {
    fs_Router_clear_pthreadSafety(pRouter_cluster);
    if (NULL == pConfig)return;
    const void *parent0 = pConfig;
    const void *parent;
    {
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &parent0, pConfig, 0, ipList, 0, "vsys");
        if (NULL == list)return;
        parent = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
    }
    {
        int j = 0;
        for (; j < 2; j++) {
            FsRouter * pRouter;
            FsConfigResult_string * pArray;
            if (0 == j) {
                pRouter = pRouter_cluster;
                pArray = fs_Config_node_string_get__IO(pConfig, parent0, parent, "hostrouter", NULL);
            } else {
                continue;
            }
            if (pArray) {
                unsigned int ui = 0;
                for (; ui < pArray->count; ui++) {
                    struct FsRouter_item * const pRouter_item = fs_Router_item_new__IO(&pRouter->itemList.mutex, pArray->data[ui]);
                    if (pRouter_item) {
                        fs_Router_add_item_pthreadSafety__OI_2(pRouter, pRouter_item, pRouter_item);
                    } else {

                        FsPrintf(FsPrintfIndex, "i=%d,j=%d/%d,Invalid data:\"%s\".\n", j, ui, pArray->count, pArray->data[ui]);
                        fflush(stdout);
                    }
                }
                fsFree(pArray);
            }
        }
    }
}

static void vsys_P_update_gb28181(/* 可为空 */FsConfig * const pConfig, GB28181 * const pGB28181, /* 本地ip地址 */const FsArray * const ipList
        , FsRouter * const pRouter_cluster
        , /* *pppNode_sip储存当前创建的sip对象的指针,前四字节表示当前创建的sip数量,为空表示未创建的sip对象个数为0 */ struct GB28181Sip_item ** * const pppNode_sip
        , /* gb28181组织校验和 */unsigned long long *pGB28181GroupSum, /* gb28181服务器校验和 */unsigned long long *pGB28181ServerSum) {
    if (NULL == pConfig)return;
    const void *vsys0 = pConfig;
    const void *vsys;
    {
        FsObjectList * const list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0, ipList, 0, "vsys");
        if (NULL == list)return;
        vsys = list->pNode[list->startIndex];
        fs_ObjectList_delete__OI(list);
    }

    /* 设置gb28181组织 */
    {
        const void *groupConfig0 = vsys0;
        const void *const groupConfig = fs_Config_node_get_first(pConfig, &groupConfig0, vsys, "groupConfig");
        const unsigned long long sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) groupConfig);
        if (*pGB28181GroupSum != sum) {
            *pGB28181GroupSum = sum;
            gb28181_group_clean_pthreadSafety(pGB28181);
            const void *group0 = groupConfig0;
            FsObjectList * const list = fs_Config_node_template__IO(pConfig, &group0, groupConfig, 0, NULL, 0, "group");
            if (list) {
                void **ppNode = list->pNode + list->startIndex;
                unsigned int ui = list->nodeCount;
                while (ui-- > 0) {
                    const void *const group = *ppNode++;
                    const char *const groupID = fs_Config_node_string_get_first(pConfig, group0, group, "groupID", NULL);
                    // FsPrintf(1, "groupID=%s\n", groupID);
                    const char *const groupName = fs_Config_node_string_get_first(pConfig, group0, group, "groupName", NULL);
                    //FsPrintf(1, "groupName=%s\n", groupName);
                    const char *const groupParent = fs_Config_node_string_get_first(pConfig, group0, group, "groupParent", NULL);
                    //FsPrintf(1, "groupParent=%s\n", groupParent);
                    gb28181_group_add_pthreadSafety(pGB28181, groupID, groupName, groupParent);
                    //            FsPrintf(1, "groupID=%s\n", groupID);
                    //            FsPrintf(1, "groupName=%s\n", groupName);
                    //            FsPrintf(1, "groupParent=%s\n", groupParent);
                }
                fs_ObjectList_delete__OI(list);
            }

            FsPrintf2(1, "Print gb28181 group:\n");
            gb28181_group_out(pGB28181, stdout);
        }
    }
    {
        const void *gb28181Config0 = vsys0;
        const void * const gb28181Config = fs_Config_node_get_first(pConfig, &gb28181Config0, vsys, "gb28181Config");
        {
            const float realbuffertimeout = fs_Config_node_float_get_first(pConfig, vsys0, vsys, "realbuffertimeout", 0.0, NULL);
            if (pGB28181->rw._realBufferTimeOut != realbuffertimeout) {
                gb28181_set_parameter_pthreadSafety(pGB28181, realbuffertimeout);
            }
        }
        {
            const unsigned long long sum = fs_Config_get_sum((FsEbml*) pConfig, (struct FsEbml_node*) gb28181Config);
            if (*pGB28181ServerSum != sum) {
                *pGB28181ServerSum = sum;
                /* 设置gb28181服务器参数 */
                {
                    const char *const ipv4_streamPort = fs_Config_node_string_get_first(pConfig, gb28181Config0, gb28181Config, "ipv4_streamPort", NULL);
                    gb28181_set_parameter_thread_pthreadSafety(pGB28181, 0, fs_Config_node_integer_get_first(pConfig, gb28181Config0, gb28181Config, "ipv4_localPort", 0, NULL), ipv4_streamPort
                            , fs_Config_node_integer_get_first(pConfig, gb28181Config0, gb28181Config, "catalog", 0, NULL)

                            , NULL);
                }
                /* 添加sip服务器 */
                const void * sip0 = gb28181Config0;
                FsObjectList * const list = fs_Config_node_template__IO(pConfig, &sip0, gb28181Config, 0, NULL, 0, "sip");
                if (list) {
                    void **ppNode = list->pNode + list->startIndex;
                    unsigned int i;
                    /* 调整sip服务器的个数 */
                    if (NULL == *pppNode_sip) {
                        *pppNode_sip = (struct GB28181Sip_item **) fsMalloc(sizeof (struct GB28181Sip_item *)*(list->nodeCount + 1));
                        memset((*pppNode_sip) + 1, 0, sizeof (struct GB28181Sip_item *)*list->nodeCount);
                        *(unsigned int*) (*pppNode_sip) = list->nodeCount;
                    } else if (list->nodeCount > (i = *(unsigned int*) (*pppNode_sip))) {
                        /* 添加 */
                        *pppNode_sip = (struct GB28181Sip_item **) fsRealloc(*pppNode_sip, sizeof (struct GB28181Sip_item *)*(list->nodeCount + 1));
                        memset((*pppNode_sip) + 1 + i, 0, sizeof (struct GB28181Sip_item *)*(list->nodeCount - i));
                        *(unsigned int*) (*pppNode_sip) = list->nodeCount;
                    } else if (list->nodeCount < i) {
                        /* 减少 */
                        /* 先批量移除 */
                        do {
                            gb28181Sip_remove_item_noWait__IO_2(pGB28181, (*pppNode_sip)[i]);
                        } while (--i > list->nodeCount);
                        /* 再批量删除 */
                        for (i = *(unsigned int*) (*pppNode_sip); i > list->nodeCount; gb28181Sip_item_delete__OI((*pppNode_sip)[i--]));
                        *pppNode_sip = (struct GB28181Sip_item **) fsRealloc(*pppNode_sip, sizeof (struct GB28181Sip_item *)*(i + 1));
                        *(unsigned int*) (*pppNode_sip) = i;
                    }

                    for (i = 0; i < list->nodeCount; i++) {
                        const void * const sip = *ppNode++;
                        unsigned int sip_ipv4;
                        {
                            const FsString * const pString = fs_Config_node_string_get_first_String(pConfig, sip0, sip, "sip_ipv4", NULL);
                            if (pString) {
                                sip_ipv4 = fs_ipv4_network_get(pString->lenth - 1, pString->buffer);
                                if (FS_INVALID_IPV4 == sip_ipv4) {
                                    FsLog(FsLogType_matchError, FsPrintfIndex, "Get \"%s\" failed,i=%u,list->nodeCount=%lu.\n", pString->buffer, i, list->nodeCount);
                                    fflush(stdout);
                                    sip_ipv4 = 0;
                                }
                            } else sip_ipv4 = 0;
                        }
                        const unsigned short sip_ipv4Port = fs_Config_node_integer_get_first(pConfig, sip0, sip, "sip_ipv4Port", 0, NULL);
                        const char *const sip_id = fs_Config_node_string_get_first(pConfig, sip0, sip, "sip_id", NULL);
                        const char *const sip_domain = fs_Config_node_string_get_first(pConfig, sip0, sip, "sip_domain", NULL);
                        const char *const id = fs_Config_node_string_get_first(pConfig, sip0, sip, "id", NULL);
                        const char *const sip_username = fs_Config_node_string_get_first(pConfig, sip0, sip, "sip_username", NULL);
                        const char *const sip_password = fs_Config_node_string_get_first(pConfig, sip0, sip, "sip_password", NULL);
                        const unsigned char sipOver = fs_Config_node_integer_get_mask(pConfig, sip0, sip, "sipOver", NULL);
                        const unsigned char sip_customConfig = fs_Config_node_integer_get_mask(pConfig, sip0, sip, "customConfig", NULL);
                        struct GB28181Sip_item * const pGB28181Sip_item = gb28181Sip_item_new__IO(0, id, sip_id, sip_domain, NULL, sip_username, sip_password, sip_ipv4, sip_ipv4Port, sip_customConfig, sipOver
                                , NULL
                                );
                        if ((*pppNode_sip)[i + 1])gb28181Sip_item_delete__OI((*pppNode_sip)[i + 1]);
                        (*pppNode_sip)[i + 1] = pGB28181Sip_item;

                        gb28181Sip_add_item__OI_2(pGB28181, pGB28181Sip_item);
                    }
                    fs_ObjectList_delete__OI(list);
                } else {
                    /* 清空gb28181服务器的所有sip信息 */
                    if (*pppNode_sip) {
                        unsigned int i = *(unsigned int*) (*pppNode_sip);
                        /* 先批量移除 */
                        do {
                            gb28181Sip_remove_item_noWait__IO_2(pGB28181, (*pppNode_sip)[i]);
                        } while (--i > 0);
                        /* 再批量删除 */
                        i = *(unsigned int*) (*pppNode_sip);
                        do {
                            gb28181Sip_item_delete__OI((*pppNode_sip)[i]);
                        } while (--i > 0);

                        fsFree(*pppNode_sip);
                        *pppNode_sip = NULL;
                    }
                }
            }
        }
    }

    //    if (0) {
    //        unsigned short ipv4Port_local = fs_Config_node_integer_get_first(pConfig, parent, "ipv4Port_local", 0);
    //        char *sip_id = fs_Config_node_string_get_first__IO(pConfig, parent, "sip_ipv4");
    //        unsigned int sip_ipv4 = fs_ipv4_network_get(strlen(sip_id), sip_id);
    //
    //        if (FS_INVALID_IPV4 == sip_ipv4) FsPrintf(1, "Get \"%s\" failed.\n", sip_id), fflush(stdout);
    //        fsFree(sip_id);
    //        unsigned short sip_ipv4Port = fs_Config_node_integer_get_first(pConfig, parent, "sip_ipv4Port", 0);
    //        sip_id = fs_Config_node_string_get_first__IO(pConfig, parent, "sip_id");
    //        char *sip_domain = fs_Config_node_string_get_first__IO(pConfig, parent, "sip_domain");
    //        char *id = fs_Config_node_string_get_first__IO(pConfig, parent, "id");
    //        char *sip_username = fs_Config_node_string_get_first__IO(pConfig, parent, "sip_username");
    //        char *sip_password = fs_Config_node_string_get_first__IO(pConfig, parent, "sip_password");
    //
    //        //__gb28181Server_set_parameter_pthreadSafety(pGB28181Server, ipv4Port_local, id, sip_id, sip_domain, sip_ipv4, sip_ipv4Port, sip_username, sip_password);
    //
    //        fsFree(sip_id), fsFree(sip_domain), fsFree(id), fsFree(sip_username), fsFree(sip_password);
    //    }
}

/* 创建通道状态数据,返回vsysChannel节点,内部顺序为<item><uuid>tt</uuid><status>0</status><checked>0</checked><ipsrc>255.255.255.255</ipsrc></item> */
static struct FsEbml_node * vsys_P_update_channelStatus_create(struct Vsys * const pVsys, /* 本地ip地址 */const FsArray * const ipList, /* 源视频地址 */unsigned int ipsrc[]) {
    FsConfig *pConfig = ((ConfigManager*) pVsys->ro._pConfigManager)->ro.__pConfig;
    if (NULL == pConfig)return NULL;
    FsEbml * rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    if (0 == pVsys->p.channelStatusIndex)pVsys->p.channelStatusIndex++;
    *(unsigned long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "sum", FsEbmlNodeType_Integer)->data.buffer = pVsys->p.channelStatusIndex;
    struct FsEbml_node * parent = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "cluster", FsEbmlNodeType_Struct);
    const void *vsys0 = pConfig;
    FsObjectList * list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0, ipList, 1, "vsys");
    if (list) {
        void **ppNode = list->pNode + list->startIndex;
        unsigned int i = list->nodeCount;
        while (i-- > 0) {
            const FsString * const hostname = fs_Config_node_string_get_first_String(pConfig, vsys0, *ppNode++, "hostname", NULL);
            fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, parent, "host", FsEbmlNodeType_String), hostname->lenth, hostname->buffer);
        }
        fs_ObjectList_delete__OI(list);
    }
    vsys0 = pConfig;
    struct FsEbml_node * channelStatus;
    list = fs_Config_node_template__IO(pConfig, &vsys0, pConfig, 0, ipList, 0, "vsys");
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
            *(long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "offlineChannelCount", FsEbmlNodeType_Integer)->data.buffer = list->nodeCount;
            channelStatus = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "channelStatus", FsEbmlNodeType_Struct);
            void **ppNode = list->pNode + list->startIndex;
            unsigned long uli = 0;
            for (; uli < list->nodeCount; uli++) {
                void *const vsysChannel = *ppNode++;
                struct FsEbml_node * const item = fs_Ebml_node_addChild(rst, channelStatus, "item", FsEbmlNodeType_Struct);
                {
                    const FsString * const uuid = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "uuid", NULL);
                    fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, item, "uuid", FsEbmlNodeType_String), uuid->lenth, uuid->buffer);
                }
                *(long long*) fs_Ebml_node_addChild(rst, item, "status", FsEbmlNodeType_Integer)->data.buffer = 0;
                *(long long*) fs_Ebml_node_addChild(rst, item, "checked", FsEbmlNodeType_Integer)->data.buffer = 0; // 标记相机状态是否已检查过
                {
                    char *const ps = (char*) fsMalloc(16);
                    const int len = sprintf(ps, "%hhu.%hhu.%hhu.%hhu", Fs_Network_Ipv4_print(ipsrc[uli]));
                    fs_Ebml_node_data_set__OI_3(fs_Ebml_node_addChild(rst, item, "ipsrc", FsEbmlNodeType_String), len + 1, ps);
                }
                {
                    const FsString * const gb28181ID = fs_Config_node_string_get_first_String(pConfig, vsysChannel0, vsysChannel, "recordConfig gb28181ID", NULL);
                    if (gb28181ID) fs_Ebml_node_data_set(fs_Ebml_node_addChild(rst, item, "gb28181ID", FsEbmlNodeType_String), gb28181ID->lenth, gb28181ID->buffer);
                }
            }
            fs_ObjectList_delete__OI(list);
        }
    } else channelStatus = NULL;
    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
    if (pVsys->p.channelStatusData)fs_Ebml_delete__OI(pVsys->p.channelStatusData, NULL);
    pVsys->p.channelStatusData = rst;
    pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);

    /* 集群数据 */
    return channelStatus;
}

/* 更新相机状态数据编号 */
#define Vsys_P_update_channelStatusIndex(channelStatusData,/* 通道状态编号 */channelStatusIndex) \
    if(0==++channelStatusIndex)channelStatusIndex++;\
    *(unsigned long long*) fs_Ebml_node_get_first(channelStatusData, (struct FsEbml_node *) channelStatusData, "sum")->data.buffer =channelStatusIndex

/* 更新相机状态数据 */
static void vsys_P_update_channelStatus(FsEbml * const channelStatusData
        , /* 内部顺序为<item><uuid>tt</uuid><status>0</status><checked>0</checked><ipsrc>255.255.255.255</ipsrc></item> */ struct FsEbml_node * const channelStatusNode
        , /* 判断各通道是否在线,0-表示不在线,和channelStatusIndex相等表示在线 */ const unsigned char channelStatus[], /* 通道数量 */const unsigned int channelCount
        , /* 不在线的相机书量 */const unsigned int offlineChannelCount) {
    *(long long*) fs_Ebml_node_get_first(channelStatusData, (struct FsEbml_node *) channelStatusData, "offlineChannelCount")->data.buffer = offlineChannelCount;
    unsigned int ui = 0;
    for (; ui < channelCount; ui++) {
        *(long long*) channelStatusNode->child[ui]->child[1]->data.buffer = (channelStatus[ui]&0x8) ? 1 : 0;
        *(long long*) channelStatusNode->child[ui]->child[2]->data.buffer = (channelStatus[ui]&0x1) ? 1 : 0;
    }
}

static void vsys_P_update_channelStatus_all_node(/* 指向状态的节点指针 */struct FsEbml_node * const pEbml_node_channelStatus
        , /* 集群节点的通道信息 */FsEbml * const pEbml, /* 不在线的相机数量 */unsigned int *const pOfflineChannelCount
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    FsObjectList * const pObjectList = fs_Ebml_node_get__IO(pEbml, (struct FsEbml_node*) pEbml, "channelStatus item");
    if (pObjectList) {
        unsigned int checkIndex = 0, uj = pObjectList->nodeCount, ui;
        struct FsEbml_node **ppNode = (struct FsEbml_node**) pObjectList->pNode + pObjectList->startIndex;
        char error = 0;
        while (uj-- > 0) {
            struct FsEbml_node * pEbml_node = *ppNode++;
            const FsString * const uuid = fs_Ebml_node_get_first_String(pEbml, pEbml_node, "uuid", NULL);
            const long long status = fs_Ebml_node_get_first_Integer(pEbml, pEbml_node, "status", 0);
            const long long checked = fs_Ebml_node_get_first_Integer(pEbml, pEbml_node, "checked", 0);
            const FsString * const ipsrc = fs_Ebml_node_get_first_String(pEbml, pEbml_node, "ipsrc", NULL);
            const FsString * const gb28181ID = fs_Ebml_node_get_first_String(pEbml, pEbml_node, "gb28181ID", NULL);
            if (NULL == uuid || NULL == ipsrc) {
                error = 1;
                continue;
            }
            for (ui = checkIndex; ui < pEbml_node_channelStatus->childNodeCount && strcmp(uuid->buffer, pEbml_node_channelStatus->child[ui]->child[0]->data.buffer) != 0; ui++);
            if (ui < pEbml_node_channelStatus->childNodeCount) {
                /* 找到了 */
#ifndef __vsys_P_update_channelStatus_all_node_do_update 
                if (*(long long*) pEbml_node_channelStatus->child[ui]->child[1]->data.buffer != status) {
                    if (status != 1)(*pOfflineChannelCount)++;
                    else (*pOfflineChannelCount)--;
                    pEbml_node_channelStatus->child[ui]->child[1]->data.buffer[0] = status;
                }
                *(long long*) pEbml_node_channelStatus->child[ui]->child[2]->data.buffer = checked;
                fs_Ebml_node_data_set(pEbml_node_channelStatus->child[ui]->child[3], ipsrc->lenth, ipsrc->buffer);
                if (gb28181ID) {
                    if (pEbml_node_channelStatus->child[ui]->childNodeCount < 5) {
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild(NULL, pEbml_node_channelStatus->child[ui], "gb28181ID", FsEbmlNodeType_String), gb28181ID->lenth, gb28181ID->buffer);
                    } else {
                        fs_Ebml_node_data_set(pEbml_node_channelStatus->child[ui]->child[4], gb28181ID->lenth, gb28181ID->buffer);
                    }
                } else {
                    if (pEbml_node_channelStatus->child[ui]->childNodeCount > 4) {
                        fs_Ebml_node_delete_child_index(NULL, pEbml_node_channelStatus->child[ui], 4);
                    }
                }
                checkIndex = ui + 1;
#endif
            } else {
                /* 没找到在前面查找 */
                for (ui = 0; ui < checkIndex && strcmp(uuid->buffer, pEbml_node_channelStatus->child[ui]->child[0]->data.buffer) != 0; ui++);
                if (ui < checkIndex) {
                    /* 找到了 */
#ifndef __vsys_P_update_channelStatus_all_node_do_update 
                    if (*(long long*) pEbml_node_channelStatus->child[ui]->child[1]->data.buffer != status) {
                        if (status != 1)(*pOfflineChannelCount)++;
                        else (*pOfflineChannelCount)--;
                        pEbml_node_channelStatus->child[ui]->child[1]->data.buffer[0] = status;
                    }
                    *(long long*) pEbml_node_channelStatus->child[ui]->child[2]->data.buffer = checked;
                    fs_Ebml_node_data_set(pEbml_node_channelStatus->child[ui]->child[3], ipsrc->lenth, ipsrc->buffer);
                    if (gb28181ID) {
                        if (pEbml_node_channelStatus->child[ui]->childNodeCount < 5) {
                            fs_Ebml_node_data_set(fs_Ebml_node_addChild(NULL, pEbml_node_channelStatus->child[ui], "gb28181ID", FsEbmlNodeType_String), gb28181ID->lenth, gb28181ID->buffer);
                        } else {
                            fs_Ebml_node_data_set(pEbml_node_channelStatus->child[ui]->child[4], gb28181ID->lenth, gb28181ID->buffer);
                        }
                    } else {
                        if (pEbml_node_channelStatus->child[ui]->childNodeCount > 4) {
                            fs_Ebml_node_delete_child_index(NULL, pEbml_node_channelStatus->child[ui], 4);
                        }
                    }
                    checkIndex = ui + 1;
#endif
                } else {
                    /* 没找到,在末尾插入 */
                    pEbml_node = fs_Ebml_node_addChild(NULL, pEbml_node_channelStatus, "item", FsEbmlNodeType_Struct);
                    fs_Ebml_node_data_set(fs_Ebml_node_addChild(NULL, pEbml_node, "uuid", FsEbmlNodeType_String), uuid->lenth, uuid->buffer);
                    if (status != 1)(*pOfflineChannelCount)++;
                    *(unsigned long long*) fs_Ebml_node_addChild(NULL, pEbml_node, "status", FsEbmlNodeType_Integer)->data.buffer = status;
                    *(unsigned long long*) fs_Ebml_node_addChild(NULL, pEbml_node, "checked", FsEbmlNodeType_Integer)->data.buffer = checked;
                    fs_Ebml_node_data_set(fs_Ebml_node_addChild(NULL, pEbml_node, "ipsrc", FsEbmlNodeType_String), ipsrc->lenth, ipsrc->buffer);
                    if (gb28181ID) {
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild(NULL, pEbml_node_channelStatus->child[ui], "gb28181ID", FsEbmlNodeType_String), gb28181ID->lenth, gb28181ID->buffer);
                    }
                }
            }
        }
        fs_ObjectList_delete__OI(pObjectList);
        if (error) {
            FsPrintf(1, "Invalid Ebml.\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            fflush(stdout);
        }
    }
    //    else {
    //        FsPrintf(1, "Invalid Ebml.\n");
    //        fs_Ebml_out_debug(pEbml, stdout), printf("\n");
    //        fflush(stdout);
    //    }
}

/* 生成一份所有集群主机的相机状态信息,channelStatusDataClientList_all需要锁住后再调用 */
static FsEbml * vsys_P_channelStatus_all_get__IO(struct Vsys * const pVsys
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    unsigned int offlineChannelCount = 0;
    FsEbml * rst = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
    *(unsigned long long*) fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "sum", FsEbmlNodeType_Integer)->data.buffer = pVsys->p.channelStatusIndex_all;
    fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "offlineChannelCount", FsEbmlNodeType_Integer);
    struct FsEbml_node * const pEbml_node_channelStatus = fs_Ebml_node_addChild(rst, (struct FsEbml_node *) rst, "channelStatus", FsEbmlNodeType_Struct);
    /* 先插本地的通道 */
    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
    if (pVsys->p.channelStatusData) vsys_P_update_channelStatus_all_node(pEbml_node_channelStatus, pVsys->p.channelStatusData, &offlineChannelCount, pShareBuffer);
    pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
    /* 添加从其他服务器收到的数据信息 */
    unsigned int *requestData, (*ppNode)[6] = ((unsigned int (*)[6])pVsys->p.channelStatusDataclusterClientList_->pNode) + pVsys->p.channelStatusDataclusterClientList_->startIndex;
    FsString * pString;
    FsEbml * pEbml;
    unsigned int i = pVsys->p.channelStatusDataclusterClientList_->nodeCount;
    unsigned int ipv4;
    unsigned short ipv4port;
    for (; i > 0; i--) {
        requestData = *ppNode++;
        pString = *(FsString**) (requestData + 4);
        if (pString) {
            pEbml = fs_Ebml_new_from_String__IO(pString->lenth, (unsigned char*) pString->buffer, NULL, pShareBuffer);
            if (NULL == pEbml || fs_Ebml_analyzeAll(pEbml, (struct FsEbml_node*) pEbml)) {
                configManager_connect_get_addr(&ipv4, &ipv4port, requestData);
                FsPrintf(1, "Invalid data(len=%lu) from :\"%hhu.%hhu.%hhu.%hhu.%hu\",cluster:\"%hhu.%hhu.%hhu.%hhu\".\n",
                        pString->lenth, Fs_Network_Ipv4_print(ipv4), ipv4port, Fs_Network_Ipv4_print(requestData[3]));
                fflush(stdout);
                if (pEbml)fs_Ebml_delete__OI(pEbml, NULL);
            } else {
                vsys_P_update_channelStatus_all_node(pEbml_node_channelStatus, pEbml, &offlineChannelCount, pShareBuffer);
                fs_Ebml_delete__OI(pEbml, NULL);
            }
        }
    }
    *(long long*) rst->child[1]->data.buffer = offlineChannelCount;

    return rst;
}

/* 更新整个集群的通道的状态信息,需要对channelStatusDataClientList_all加锁 */
static void vsys_P_channelStatus_all_send(struct Vsys * const pVsys
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (pVsys->p.channelStatusData_ebml_all)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_ebml_all), pVsys->p.channelStatusData_ebml_all = NULL;
    if (pVsys->p.channelStatusData_xml_all)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_xml_all), pVsys->p.channelStatusData_xml_all = NULL;
    if (pVsys->p.channelStatusData_json_all)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_json_all), pVsys->p.channelStatusData_json_all = NULL;
    if (0 == ++pVsys->p.channelStatusIndex_all)pVsys->p.channelStatusIndex_all++;
    if (0 == pVsys->p.channelStatusDataClientList->nodeCount) {
        /* 没有客户端 */
        return;
    }
    FsEbml * const pEbml = vsys_P_channelStatus_all_get__IO(pVsys, pShareBuffer);
    configManager_conncet_refer_send(pVsys->ro._pConfigManager, pVsys->p.channelStatusDataClientList_all
            , &pVsys->p.channelStatusData_ebml_all, &pVsys->p.channelStatusData_xml_all, &pVsys->p.channelStatusData_json_all, pEbml, 0x2, pObjectBaseBuffer, pShareBuffer);
    fs_Ebml_delete__OI(pEbml, pShareBuffer);
}

/* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int vsys_P_cb_connect_error(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[]
        , /* 调用函数的指针 */ struct Vsys * const pVsys, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    {
        FsStructList * const channelStatusDataClientList = pVsys->p.channelStatusDataClientList;
        pthread_mutex_lock(&channelStatusDataClientList->mutex);
        configManager_conncet_refer_check(pVsys->ro._pConfigManager, channelStatusDataClientList);
        pthread_mutex_unlock(&channelStatusDataClientList->mutex);
    }
    {
        FsStructList * const channelStatusDataClientList_all = pVsys->p.channelStatusDataClientList_all;
        pthread_mutex_lock(&channelStatusDataClientList_all->mutex);
        configManager_conncet_refer_check(pVsys->ro._pConfigManager, channelStatusDataClientList_all);
        pthread_mutex_unlock(&channelStatusDataClientList_all->mutex);
    }
    return 1;
}

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int vsys_P_cmd_cb(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Vsys * const pVsys
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
    unsigned char getAll = 0;
    {
        const char *const return_type = fs_Ebml_node_get_first_string(pEbml, (struct FsEbml_node*) pEbml, "return_type", NULL);
        if (return_type) {
            if (0 == strcmp("ebml", return_type)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type)) requestDataType = 2;
            else if (0 == strcmp("json", return_type)) requestDataType = 3;
            else if (0 == strcmp("ebml_all", return_type)) requestDataType = 1, getAll = 1;
            else if (0 == strcmp("xml_all", return_type)) requestDataType = 2, getAll = 1;
            else if (0 == strcmp("json_all", return_type)) requestDataType = 3, getAll = 1;
        }
    }
    if (0 == getAll) {
        /* 单机版本 */
        pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
        if (pVsys->p.channelStatusData) {
            configManager_conncet_refer_sendData(&pVsys->p.channelStatusData_ebml, &pVsys->p.channelStatusData_xml, &pVsys->p.channelStatusData_json
                    , pVsys->p.channelStatusData, requestID_3, headType, checkMethod, virtualConnection, head | 0x2, requestDataType, pObjectBaseBuffer, pShareBuffer);
            unsigned int data[6] = {ConfigManager_refer_connect_node_make6_array(requestID_3, headType, checkMethod, virtualConnection, requestDataType, head)};
            if (fs_StructList_insert_order(pVsys->p.channelStatusDataClientList, data) < 0) {
                pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
                return 1;
            } else {
                pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
                return -128;
            }
        } else {
            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
            FsPrintf(1, "Vsys is busy.\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            fflush(stdout);
            return -1;
        }
    } else {
        /* 集群版本 */
        pthread_mutex_lock(&pVsys->p.channelStatusDataClientList_all->mutex);
        if (configManager_conncet_refer_sendData2(&pVsys->p.channelStatusData_ebml_all, &pVsys->p.channelStatusData_xml_all, &pVsys->p.channelStatusData_json_all
                , (FsEbml * (*)(void*))vsys_P_channelStatus_all_get__IO, pVsys, requestID_3, headType, checkMethod, virtualConnection, head | 0x2, requestDataType, pObjectBaseBuffer, pShareBuffer) == 1) {
            unsigned int data[6] = {ConfigManager_refer_connect_node_make6_array(requestID_3, headType, checkMethod, virtualConnection, requestDataType, head)};
            if (fs_StructList_insert_order(pVsys->p.channelStatusDataClientList_all, data) < 0) {
                pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
                return 1;
            } else {
                pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
                return -128;
            }
        } else {
            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
            FsPrintf(1, "Vsys is busy.\n");
            fs_Ebml_out_debug(pEbml, stdout, pShareBuffer), printf("\n");
            fflush(stdout);

            return -1;
        }
    }
}

static FsConfig *vsys_P_protocol_channelStatus_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "channelStatus_get", "channelStatus_get", "channelStatus_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsys", "vsys", "vsys");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "ebml_all", "ebml_all(压缩数据,包含集群内的所有主机)", "ebml_all(压缩数据,包含集群内的所有主机)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml_all", "xml_all(xml数据,包含集群内的所有主机)", "xml_all(xml数据,包含集群内的所有主机)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json_all", "json_all(json数据,包含集群内的所有主机)", "json_all(json数据,包含集群内的所有主机)");
    }
    return pConfig;
}
/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int vsys_P_cmd_cb_vsysInfo(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Vsys * const pVsys
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //fs_Ebml_out_debug(pEbml, stdout), printf("\n");
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
    struct ConfigManager_connectNode_useOnce * const pConnectNode = configManager_connectNode_useOnce_new__IO(requestID_3, headType, checkMethod, virtualConnection, head, requestDataType
            , 0, fs_Ebml_node_get_mask(pEbml, (struct FsEbml_node*) pEbml, "type"));
    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
    pConnectNode->next = (struct ConfigManager_connectNode_useOnce *) pVsys->p.pConnectNode__channelStatusDataClientList;
    pVsys->p.pConnectNode__channelStatusDataClientList = pConnectNode;
    pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);

    return 1;
}


static FsConfig * vsys_P_protocol_vsysInfo_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsysInfo_get", "vsysInfo_get", "vsysInfo_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsys", "vsys", "vsys");
    }
    {
        void *const pNode = fs_Config_node_integer_add(pConfig, pConfig, "type", "数据类型", "数据类型", FsConfig_nodeShowType_default, 0, 0x7, 1, 7, 3);
        fs_Config_node_integer_add_value(pConfig, pNode, FsConfig_nodeValue_default, 0x1, "基本数据", "基本数据");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}

/* 在回执匹配掩码时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */
static int vsys_P_cb_return_channelStatus(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsObjectBasePthreadSafety *pObjectBasePthreadSafety, /* 调用函数的指针 */ struct Vsys * const pVsys
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if ((0x3U | ConfigManager_Cluster_protocol_channelStatus_get) != *(unsigned int *) pObjectBasePthreadSafety->data) {
        if (pObjectBasePthreadSafety->len > 7) {
            if (0 == (pObjectBasePthreadSafety->data[0]&0x1)) {
                VsysLog(FsLogType_info, "Invalid head(=%#x) data(len=%u,%#x,%#x):\"%s\".\n", (0x3U | ConfigManager_Cluster_protocol_channelStatus_get),
                        pObjectBasePthreadSafety->len, ((int*) (pObjectBasePthreadSafety->data))[0], ((int*) (pObjectBasePthreadSafety->data))[1], pObjectBasePthreadSafety->data + 8);
                fflush(stdout);
            } else {
                VsysLog(FsLogType_info, "Invalid head(=%#x) data(len=%u,%#x,%#x).\n", (0x3U | ConfigManager_Cluster_protocol_channelStatus_get),
                        pObjectBasePthreadSafety->len, ((int*) (pObjectBasePthreadSafety->data))[0], ((int*) (pObjectBasePthreadSafety->data))[1]);
                fflush(stdout);
            }
        } else {
            VsysLog(FsLogType_info, "Invalid head(=%#x) data(len=%u,%#x):\"%s\".\n", (0x3U | ConfigManager_Cluster_protocol_channelStatus_get),
                    pObjectBasePthreadSafety->len, ((int*) (pObjectBasePthreadSafety->data))[0], pObjectBasePthreadSafety->data + 8);
            fflush(stdout);
        }
        return -1;
    }
    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList_all->mutex);
    /* 在pVsys->p.channelStatusDataclusterClientList_中查找连接是否来自当前缓存的连接 */
    const long index = fs_StructList_check_exist_order_custom(pVsys->p.channelStatusDataclusterClientList_, requestID_3, sizeof (unsigned int)*3);
    if (index < 0) {
        /* 不是缓存连接,跳过 */
        pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
        VsysLog(FsLogType_info, "Connect not in refer,data(len=%u,%#x,%#x).\n", pObjectBasePthreadSafety->len, ((int*) (pObjectBasePthreadSafety->data))[0], ((int*) (pObjectBasePthreadSafety->data))[1]);
        fflush(stdout);
        return -1;
    }
    FsString **ppString = (FsString**) (((unsigned int (*)[6])pVsys->p.channelStatusDataclusterClientList_->pNode)[pVsys->p.channelStatusDataclusterClientList_->startIndex + index] + 4);
    if (*ppString)fs_String_delete__OI(*ppString);
    *ppString = fs_String_new_form_stringBuffer__IO(pObjectBasePthreadSafety->data + 8, pObjectBasePthreadSafety->len - 8);
    /* 更新并发送集群通道信息 */
    vsys_P_channelStatus_all_send(pVsys, pObjectBaseBuffer, pShareBuffer);
    pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);

    return 1;
}

#ifdef Vsys_stat_out_enable

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int vsys_P_cmd_cb_stats(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ struct Vsys * const pVsys
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //fs_Ebml_out_debug(pEbml, stdout), printf("\n");
    {
        struct FsEbml_node * const return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (return_type && 0 < return_type->data.lenth) {
            if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
            else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
        }
    }
    {
        FsEbml * const pEbml1 = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
        pthread_mutex_lock(&pVsys->p.mutex_ppObjectStats);
        {
            unsigned int i = 0;
            const unsigned int count = pVsys->p.statsCount;
            FsObjectStats **ppObjectStats = pVsys->p.ppObjectStats;
            while (i++<count) {

                const FsObjectStats * const pObjectStats = *ppObjectStats++;
                struct FsEbml_node * const pEbml_node = fs_Ebml_node_addChild(pEbml1, (struct FsEbml_node *) pEbml1, "stat", FsEbmlNodeType_Struct);
                *(long long*) fs_Ebml_node_addChild(pEbml1, pEbml_node, "decodeMask", FsEbmlNodeType_Integer)->data.buffer = pObjectStats->decodeMask;
                *(long long*) fs_Ebml_node_addChild(pEbml1, pEbml_node, "decodeMask_set", FsEbmlNodeType_Integer)->data.buffer = pObjectStats->decodeMask_set;
                *(long long*) fs_Ebml_node_addChild(pEbml1, pEbml_node, "decodeWaitReferCount", FsEbmlNodeType_Integer)->data.buffer = pObjectStats->decodeWaitReferCount;
                *(long long*) fs_Ebml_node_addChild(pEbml1, pEbml_node, "decodeLastReferCount", FsEbmlNodeType_Integer)->data.buffer = pObjectStats->decodeLastReferCount;
                *(long long*) fs_Ebml_node_addChild(pEbml1, pEbml_node, "decodeChanged", FsEbmlNodeType_Integer)->data.buffer = pObjectStats->decodeChanged;
            }
        }
        pthread_mutex_unlock(&pVsys->p.mutex_ppObjectStats);
        configManager_conncet_refer_sendData(NULL, NULL, NULL, pEbml1, requestID_3, headType, checkMethod, virtualConnection, head | 0x2, requestDataType, pObjectBaseBuffer, pShareBuffer);
        fs_Ebml_delete__OI(pEbml1, pShareBuffer);
    }
    return 1;
}

static FsConfig *vsys_P_protocol_stats_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsys_stats", "vsys_stats", "vsys_stats");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsys", "vsys", "vsys");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}

#endif

/* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128 */
static int vsys_P_cmd_cb_router(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 1-8字节头,2-16字节头,4-http无头,5-http+8字节头,6-http+16字节头 */ unsigned char headType
        , /* 头的校验方式,仅使用16字节头时有效,请求与回执应使用相同的校验方式,取值范围1-31  */ unsigned char checkMethod
        , /* 虚拟连接号,仅使用16字节头时有效,使用3字节 */unsigned int virtualConnection, /* 收到数据的前4字节 */unsigned int head
        , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针,[0]--集群路由,[1]--本地ip映射 */ FsRouter * * const ppRouter
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    //fs_Ebml_out_debug(pEbml, stdout), printf("\n");
    {
        struct FsEbml_node * const return_type = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "return_type");
        if (return_type && FsEbmlNodeType_is_String(return_type->type) && 0 < return_type->data.lenth) {
            if (0 == strcmp("ebml", return_type->data.buffer)) requestDataType = 1;
            else if (0 == strcmp("xml", return_type->data.buffer)) requestDataType = 2;
            else if (0 == strcmp("json", return_type->data.buffer)) requestDataType = 3;
        }
    }
    FsRouter *pRouter;
    {
        struct FsEbml_node * const pEbml_node = fs_Ebml_node_get_first(pEbml, (struct FsEbml_node*) pEbml, "table");
        if (pEbml_node && FsEbmlNodeType_is_String(pEbml_node->type) && pEbml_node->data.lenth == sizeof ("cluster") && memcmp(pEbml_node->data.buffer, FsStringDataLen("cluster")) == 0)
            pRouter = ppRouter[0];
        else pRouter = ppRouter[1];
    }
    {
        FsEbml * const pEbml1 = fs_Router_to_ebml_pthreadSafety__IO(pRouter);
        FsObjectList * const list = fs_Ebml_node_get__IO(pEbml, (struct FsEbml_node*) pEbml, "dst");
        if (list) {
            unsigned int i = list->nodeCount;
            struct FsEbml_node **ppNode = (struct FsEbml_node**) list->pNode + list->startIndex;
            for (; i > 0; i--) {
                struct FsEbml_node * const pEbml_node = *ppNode++;
                unsigned int dst;
                if (!FsEbmlNodeType_is_String(pEbml_node->type) || pEbml_node->data.lenth < 2 || (dst = fs_ipv4_network_get(pEbml_node->data.lenth - 1, pEbml_node->data.buffer)) == FS_INVALID_IPV4) {
                    FsPrintf(FsPrintfIndex, "Invalid data,type=%#x,lenth=%lu.\n", pEbml_node->type, pEbml_node->data.lenth);
                    fflush(stdout);
                } else {

                    struct FsEbml_node * const pEbml_node1 = fs_Ebml_node_addChild(pEbml1, (struct FsEbml_node *) pEbml1, "itemCheck", FsEbmlNodeType_Struct);
                    char buffer[sizeof ("192.168.100.101")];
                    fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml1, pEbml_node1, "dst", FsEbmlNodeType_String), pEbml_node->data.lenth, pEbml_node->data.buffer);
                    dst = fs_Router_match_ip_pthreadSafety((void **) buffer, pRouter, dst);
                    fs_Ebml_node_data_set(fs_Ebml_node_addChild(pEbml1, pEbml_node1, "gw", FsEbmlNodeType_String)
                            , sprintf(buffer, "%hhu.%hhu.%hhu.%hhu", Fs_Network_Ipv4_print(dst)) + 1, buffer);
                }
            }
            fs_ObjectList_delete__OI(list);
        }
        configManager_conncet_refer_sendData(NULL, NULL, NULL, pEbml1, requestID_3, headType, checkMethod, virtualConnection, head | 0x2, requestDataType, pObjectBaseBuffer, pShareBuffer);
        fs_Ebml_delete__OI(pEbml1, pShareBuffer);
    }
    return 1;
}

static FsConfig * vsys_P_protocol_router_get() {
    FsConfig * const pConfig = fs_Config_new__IO();
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_type", "命令类型", "命令类型,此字段为开发人员分配,各接口唯一", 0, 0x7, 0, 32, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsys_router_get", "vsys_router_get", "vsys_router_get");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "command_for_uuid", "资源的uuid", "资源的uuid,表示使用此接口操作的资源编号", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "vsys", "vsys", "vsys");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "table", "路由表", "路由表,获取哪个路由表", 0, 0x7, 0, 16, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "cluster", "分布式节点路由", "分布式节点路由");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "localmap", "本地ip映射", "本地ip映射");
    }
    {
        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "dst", "目的IP", "目的IP", 0, 0x7, 0, 16, 10);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "192.168.45.33", "192.168.45.33", "192.168.45.33");
    }
    {

        void *const pNode = fs_Config_node_string_add(pConfig, pConfig, "return_type", "回执类型", "期望的回执类型,缺失或无效时回执的数据类型不确定", 0, 0x7, 0, 64, 1);
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_default, "ebml", "ebml(压缩数据)", "ebml(压缩数据)");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "xml", "xml", "xml");
        fs_Config_node_string_add_value(pConfig, pNode, FsConfig_nodeValue_optional, "json", "json", "json");
    }
    return pConfig;
}

/* 在连接创建成功时调用,成功返回1,返回-128增加对连接的引用 */
static int vsys_P_cluster_connect_cb(/* 期望连接的其他主机的本地ip */ unsigned int ipv4
        , /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ unsigned int requestID_3[]
        , /* 是否是连接断开,0-表示新建立连接,1-表示连接断开了 */unsigned char close, /* 调用函数的指针 */ struct Vsys * const pVsys
        , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    unsigned int data[6] = {requestID_3[0], requestID_3[1], requestID_3[2], ipv4, 0, 0};
    //    FsPrintf(1, "ipv4:\"%hhu.%hhu.%hhu.%hhu\",close=%hhu\n", Fs_Network_Ipv4_print(ipv4), close);
    //    fs_Bd_backtrace_out(stdout);
    if (0 == close) {
        /* 新连接到来 */
        pthread_mutex_lock(&pVsys->p.channelStatusDataClientList_all->mutex);
        if (fs_StructList_insert_order_custom(pVsys->p.channelStatusDataclusterClientList_, data, sizeof (unsigned int)*3) < 0) {
            /* 有相同的连接,跳过 */
            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
            return 1;
        }
        pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
        /* 发送获取相机状态的命令 */
        const unsigned char sendData[] = {0xD5, 0x47, 0xEE, 0x32, 0xA3, 0x9D, 0x9D, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64, 0x5F, 0x74
            , 0x79, 0x70, 0x65, 0x00, 0x05, 0x01, 0x25, 0x63, 0x68, 0x61, 0x6E, 0x6E, 0x65, 0x6C, 0x53, 0x74
            , 0x61, 0x74, 0x75, 0x73, 0x5F, 0x67, 0x65, 0x74, 0x00, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64
            , 0x5F, 0x66, 0x6F, 0x72, 0x5F, 0x75, 0x75, 0x69, 0x64, 0x00, 0x05, 0x01, 0x0B, 0x76, 0x73, 0x79
            , 0x73, 0x00, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6E, 0x5F, 0x74, 0x79, 0x70, 0x65, 0x00, 0x05, 0x01
            , 0x0B, 0x65, 0x62, 0x6D, 0x6C, 0x00, 0x51};
        configManager_conncet_refer_send_buffer(sizeof (sendData), (const char*) sendData, requestID_3, 1, 0, 0, 0x01U | ConfigManager_Cluster_protocol_channelStatus_get, pObjectBaseBuffer);
        return -128;
    } else {
        /* 连接断开 */
        pthread_mutex_lock(&pVsys->p.channelStatusDataClientList_all->mutex);
        long l = fs_StructList_check_exist_order_custom(pVsys->p.channelStatusDataclusterClientList_, data, sizeof (unsigned int)*4);
        if (l < 0) {
            /* 没找到,跳过 */
            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
            return 1;
        } else {
            /* 释放连接引用 */
            configManager_connect_release(pVsys->ro._pConfigManager, requestID_3, 1);
            FsString *pString = *(FsString**) (((unsigned int (*)[6])pVsys->p.channelStatusDataclusterClientList_->pNode)[pVsys->p.channelStatusDataclusterClientList_->startIndex + l] + 4);
            fs_StructList_remove_at(pVsys->p.channelStatusDataclusterClientList_, l);
            if (pString) fs_String_delete__OI(pString);
            /* 更新并发送集群通道信息 */
            vsys_P_channelStatus_all_send(pVsys, pObjectBaseBuffer, pShareBuffer);
            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);

            return 1;
        }
    }
}

/* 获取通道状态信息,编号发生改变时返回FsString指针,否则返回NULL,返回NULL可能是出错 */
static FsObjectBasePthreadSafety * vsys_P_channelStatusGet__IO(/* 调用时缓存的通道最后改变的编号,在发生改变时才返回数据,不为空 */unsigned long long *pChannelStatusIndex
        , struct Vsys * const pVsys, /* 类型 0-ebml,1-xml,2-json */const unsigned char type
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (*pChannelStatusIndex == pVsys->p.channelStatusIndex)return NULL;
    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
    *pChannelStatusIndex = pVsys->p.channelStatusIndex;
    if (pVsys->p.channelStatusData) {
        FsObjectBasePthreadSafety * const rst = configManager_conncet_refer_getData__IO(&pVsys->p.channelStatusData_ebml, &pVsys->p.channelStatusData_xml, &pVsys->p.channelStatusData_json
                , pVsys->p.channelStatusData, type, pShareBuffer);
        if (NULL == rst) {
            FsPrintf(1, "Get channelStatus failed,type=%hhu.\n", type);
            fflush(stdout);
        }
        pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
        return rst;
    } else {
        pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
        FsPrintf(1, "Vsys is busy.\n");
        fflush(stdout);

        return NULL;
    }
}

/* 获取通道状态信息,校验和发生改变时返回FsEbml指针,否则返回NULL,返回NULL可能是出错 */
static FsEbml * vsys_P_channelStatusGetEbml__IO(/* 调用时缓存的通道最后改变的编号,在发生改变时才返回数据,不为空 */unsigned long long *pChannelStatusIndex, struct Vsys * const pVsys
        , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer) {
    if (*pChannelStatusIndex == pVsys->p.channelStatusIndex)return NULL;
    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
    *pChannelStatusIndex = pVsys->p.channelStatusIndex;
    if (pVsys->p.channelStatusData) {
        FsEbml * const rst = fs_Ebml_new_from_Ebml__IO(pVsys->p.channelStatusData);
        pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
        return rst;
    } else {
        pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
        FsPrintf(1, "Vsys is busy.\n");
        fflush(stdout);

        return NULL;
    }
}

/* 本线程负责数据采集的业务,并管理各工作线程 */
static void *vsys_P_T(struct Vsys * const pVsys) {
#undef FsFunctionName
#define FsFunctionName vsys_P_T
    ////////////////////////////////////////////////////////////////////////////
    /* 创建线程监控 */
    void *pMonitor = pVsys->ro._pMonitor;
    if (NULL == pMonitor)pMonitor = monitor_new__IO(NULL, 0, "Monitor", NULL, 0, 5.0, 0.1, 0, NULL, NULL, NULL);
    MonitorItem * const pMonitorItem = monitorItem_new_and_add(pMonitor, Vsys_get_threadName(pVsys), NULL, pVsys, pVsys->ro._pMonitor ? pVsys->ro._parentTid : 0, NULL);
    thread_join_add_check_pthreadSafety(MonitorItem_get_threadName(pMonitorItem), NULL);
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    /* 引用变量 */
    signed char *const externRunStatus = pVsys->ro._externRunStatus;
    ConfigManager * const pConfigManager = (ConfigManager*) pVsys->ro._pConfigManager;
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
#define __vsys_P_T_clean1_1 {if(shareBuffer!=NULL)fsFree(shareBuffer);}
    /* 不可跨线程的缓存buffer */
    //    FsObjectSingleBuffer singleBuffer;
    //    fs_ObjectSingleBuffer_init(&singleBuffer, 2);
#define __vsys_P_T_clean1_2 //fs_ObjectSingleBuffer_release(&singleBuffer);
    /* 可跨线程的缓存buffer,一般用于收数据或传递数据到其他线程 */
    FsObjectBaseBuffer baseBuffer;
    fs_ObjectBaseBuffer_init(&baseBuffer, 2);
#define __vsys_P_T_clean1_3 fs_ObjectBaseBuffer_release(&baseBuffer);
    ////////////////////////////////////////////////////////////////////////////
    /* 全局变量 */
    signed char childRun = 1;
    signed char updateConfig = 0;

#define VsysTaskCount (Vsys_P_module_Record_index+1)
    FsMemeryFree * pMemeryFree;
    double thisTime, nextShowTime = 0.0;
    double sleepToTime = 0.0;
    unsigned short sleepToTimeInterval = 1;
    unsigned short resetSerialCount = 0; // 连续reset次数;
    unsigned int configCheckSum[3] = {(unsigned int) FsPointer_to_integer(pVsys), 0, 0};
    int resetIndex = 0;
    unsigned int taskResetOkCount = 0, taskResetOkCount1 = 0;
    /* 使用掩码标记哪些模块重置的状态;
     *    先对所有模块进行标记为1;
     *    在所有模块都标记后,检查所有模块是否完成重置,重置完成后标记为0;
     *    在所有模块的标记都为0时,表示该通道重置完成,可执行下一个通道的重置工作;
     */
    unsigned short resetMask[(VsysTaskCount + sizeof ( unsigned long long)*8 - 1) / 8 / sizeof ( unsigned long long)*2];
    memset(resetMask, 0, sizeof (resetMask));
    unsigned char resetMaskPos = 0, resetMaskPos1 = sizeof (resetMask) / sizeof (resetMask[0]) / 2;
    /* 本地通道对应的描述节点 */
    const void *vsysChannel0 = NULL;
    /* 储存本地通道节点的链表 */
    FsObjectList * localItemList = NULL;
#define __vsys_P_T_clean1_4 {if (localItemList) fs_ObjectList_delete__OI(localItemList);}
    unsigned long long sum[2] = {0, 0};
    // 通道数
    int channelCount = 0;
    // 计划任务帧阈值,只有当系统内各模块的数据帧数量都小于此值时才可能触发系统内的计划任务,如相机状态检测,-128表示禁用
    short cronframethreshold = 1;

    /* 相机状态检测方式,设置channelStatusCheckExtra值的最小值 */
    unsigned short channelstatuscheckmode = 0;

    /* 计划任务帧阈值的参考show_max */
    int cronframe_show_max = 0;
    /* 开方分组 */
    FsGroupSqrtPthreadSafety groupSqrt;
    Fs_GroupSqrtPthreadSafety_init(&groupSqrt);
#define __vsys_P_T_clean1_5 {Fs_GroupSqrtPthreadSafety_release(&groupSqrt);}    
    // 计划任务帧阈值的补偿值,防止cronframethreshold过小时,计划任务一直不触发,触发后此值置0
    short cronframethresholdCompensate = 0;
    /* 判断各通道是否在线,(&0x80)--0:表示未检测,1:表示检测中;(&0x70,3位)--用于统计是否在线;(&0x08,1位)--0:表示数据不通,8:表示可收到数据;(&0x01,1位)--0:表示未探测,1:已探测 */
    unsigned char *channelStatus = (unsigned char *) fsMalloc(8), channelStatusIndex = 0x9 | 0x70, channelChange = 0;
#define __vsys_P_T_clean1_6 {if (channelStatus) {int i;for (i = 0; i < channelCount; i++){fs_Object_delete_pthreadSafety__OI(ppObjectStats[i]);}fsFree(channelStatus);}}    
    /* 通道是否有效,0-无效,1,-有效 */
    unsigned char *channelValid = channelStatus;
    /* 通道的源地址 */
    unsigned int *ipsrc = NULL;
    /* 不在线的相机数量 */
    unsigned int offlineChannelCount = 0xFFFFFFFFU, offlineChannelCount_new = 0;
    /* 当前相机检查的编号 */
    int channelStatusCheckIndex = 0;
    /* 当前相机检查的块大小 */ //块即是缓存,并发数
    int channelStatusCheckBlockSize = 1;
    /* 当前正在检查的相机量 */
    int channelStatusCheckInCount = 0;
    /* 可以额外检查的数量 */
    int channelStatusCheckExtra = 0;
    /* 用于改变channelStatusCheckExtra的标记变量
     * 在新增检测时,如channelStatusCheckExtraFlag为0需要设置为1;
     * 在获取到正检测通道的视频流时,如channelStatusCheckExtraFlag大于等于1则channelStatusCheckExtraFlag+1;
     * 在3S计时到时:
     *     如channelStatusCheckExtraFlag为0,channelStatusCheckExtra不发生变化;
     *     如channelStatusCheckExtraFlag为1,channelStatusCheckExtra增加;
     *     如channelStatusCheckExtraFlag大于等于2,channelStatusCheckExtra减小;
     */
    int channelStatusCheckExtraFlag = 0;
    /* 通道状态中的channelStatus节点,内部顺序为<item><uuid>tt</uuid><status>0</status><checked>0</checked><ipsrc>255.255.255.255</ipsrc></item> */
    struct FsEbml_node * channelStatusNode = NULL;
#define __vsys_P_T_clean1_7    
    /* 帧率 */
    float *frameRate = (float*) channelStatus, frameRateAvg = 10.0;
    /* 模块掩码 */
    unsigned int *moduleMask = (unsigned int*) channelStatus;
    /* 解码跟踪 */
    FsObjectStats **ppObjectStats = (FsObjectStats **) channelStatus;
    /* 磁盘管理配置 */
    FsStringBuilder * keepspaceConfig = NULL;
#define __vsys_P_T_clean1_8 {if (keepspaceConfig)fs_StringBuilder_delete__OI(keepspaceConfig);}    
    /* 数据上传配置 */
    FsStringBuilder * uploadDataConfig = NULL;
#define __vsys_P_T_clean1_9 {if (uploadDataConfig)fs_StringBuilder_delete__OI(uploadDataConfig);}        
    // [0]:集群路由,[1]:本地ip映射
    FsRouter * pRouter_cluster[2];
    // rtsp服务器
    Rtsp * pRtsp;
    // hls服务器
    Hls * pHls;
    // GB28181服务器
    GB28181 * pGB28181;
    /* 储存当前创建的sip对象的指针,前四字节表示当前创建的sip数量,为空表示未创建的sip对象个数为0 */
    struct GB28181Sip_item **ppNode_sip = NULL;
#define __vsys_P_T_clean1_10 {if (ppNode_sip)fsFree(ppNode_sip);}
    /* gb28181组织校验和 */
    unsigned long long gb28181GroupSum = 0;
    /* gb28181服务器校验和 */
    unsigned long long gb28181ServerSum = 0;
    /* gbsdk回放对象 */
    struct SdkPlayBack_item * pSdkPlayBack_item_gbsdk;

    // 文件点播
    FileStream * pFileStream;
    float show_fps = 0.0;
    int show_fps_index = 0;
    int show_max = 0;
    ////////////////////////////////////////////////////////////////////////////
    /* 中间变量 */
    FILE * fd;
    ////////////////////////////////////////////////////////////////////////////
    /* 初始化一些变量 */
    if (access("/tmp/upload", F_OK))mkdir("/tmp/upload", 0777);
    /* 获取通道状态的客户端链表 */
    pVsys->p.channelStatusDataClientList = fs_StructList_new__IO(6, sizeof (int)*6);
#define __vsys_P_T_clean1_11 {configManager_conncet_refer_close(pConfigManager, pVsys->p.channelStatusDataClientList, 1);\
    fs_StructList_delete__OI(pVsys->p.channelStatusDataClientList);\
    if (pVsys->p.channelStatusData)fs_Ebml_delete__OI(pVsys->p.channelStatusData,&shareBuffer), pVsys->p.channelStatusData = NULL;\
    if (pVsys->p.channelStatusData_ebml)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_ebml), pVsys->p.channelStatusData_ebml = NULL;\
    if (pVsys->p.channelStatusData_xml)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_xml), pVsys->p.channelStatusData_xml = NULL;\
    if (pVsys->p.channelStatusData_json)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_json), pVsys->p.channelStatusData_json = NULL;\
    if (pVsys->p.pConnectNode__channelStatusDataClientList)configManager_connectNode_useOnce_list_delete((struct ConfigManager_connectNode_useOnce*)pVsys->p.pConnectNode__channelStatusDataClientList), pVsys->p.pConnectNode__channelStatusDataClientList = NULL;}    

    pVsys->p.channelStatusDataClientList_all = fs_StructList_new__IO(6, sizeof (int)*6);
#define __vsys_P_T_clean1_12 {configManager_conncet_refer_close(pConfigManager, pVsys->p.channelStatusDataClientList_all, 1);\
    fs_StructList_delete__OI(pVsys->p.channelStatusDataClientList_all);\
    if (pVsys->p.channelStatusData_ebml_all)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_ebml_all), pVsys->p.channelStatusData_ebml_all = NULL;\
    if (pVsys->p.channelStatusData_xml_all)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_xml_all), pVsys->p.channelStatusData_xml_all = NULL;\
    if (pVsys->p.channelStatusData_json_all)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_json_all), pVsys->p.channelStatusData_json_all = NULL;}    
    /* 缓存集群的连接,使用6个int储存一个连接,前3个int为连接标识符,第4个int为集群节点标识符,第5-6个储存收到的ebml转的FsString指针,使用channelStatusDataClientList_all的互斥锁 */
    pVsys->p.channelStatusDataclusterClientList_ = fs_StructList_new__IO(6, sizeof (int)*6);
#define __vsys_P_T_clean1_13 {configManager_conncet_refer_close_with_data(pConfigManager, pVsys->p.channelStatusDataclusterClientList_, 1);\
    fs_StructList_delete__OI(pVsys->p.channelStatusDataclusterClientList_);}
    // 创建配置
    configManager_init_register(pConfigManager, Vsys_get_threadName(pVsys), vsys_P_createConfig, pVsys);
#define __vsys_P_T_clean1_14 {configManager_init_logoff(pConfigManager, Vsys_get_threadName(pVsys), vsys_P_createConfig, pVsys);}
    //    while (pVsys->rw.runStatus == *externRunStatus) {
    //        pMonitorItem->rw.cycleAlive = 1;
    //        pMonitorItem->rw.line = __LINE__;
    //        usleep(10000);
    //    }
    // 注册连接断开时的回调通知
    configManager_connect_error_register(pConfigManager, (int (*)(const unsigned int*, void*, char**))vsys_P_cb_connect_error, pVsys);
#define __vsys_P_T_clean2_1 {configManager_connect_error_logoff(pConfigManager, (int (*)(const unsigned int*, void*, char**))vsys_P_cb_connect_error, pVsys);}
    // 注册命令字
    configManager_cmd_register_and_protocol_publish(pConfigManager, "channelStatus_get", "vsys", 0, pVsys, 0
            , (int(*)(const unsigned int*, unsigned char, unsigned char, unsigned int, unsigned int, FsEbml*, char, void*, FsObjectBaseBuffer * const, FsShareBuffer*)) vsys_P_cmd_cb, NULL, pVsys
            ////////////////////////////////////////////////////////////////////
            , "channelStatus_get", "通道状态获取", 255, vsys_P_protocol_channelStatus_get, &shareBuffer);
#define __vsys_P_T_clean2_2 {configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "channelStatus_get", "vsys", 0, pVsys,"channelStatus_get",&shareBuffer);}
    /* 注册获取线程信息的的命令字,cmd+uuid+ipv4必须是唯一值 */
    configManager_cmd_register_and_protocol_publish(pConfigManager, "vsysInfo_get", "vsys", 0, pVsys, 0
            , (int (* const) (const unsigned int *, unsigned char, unsigned char, unsigned int, unsigned int, FsEbml * const, char, void * const, FsObjectBaseBuffer * const, char * * const))vsys_P_cmd_cb_vsysInfo, NULL, pVsys
            ////////////////////////////////////////////////////////////////
            , "vsysInfo_get", "vsys信息获取", ConfigManager_Module5_protocol_classIndex0, vsys_P_protocol_vsysInfo_get, &shareBuffer);
#define __vsys_P_T_clean2_3 {configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "vsysInfo_get", "vsys", 0, pVsys,"vsysInfo_get",&shareBuffer);}

#ifdef Vsys_stat_out_enable
    /* ppObjectStats线程同步锁 */
    pthread_mutex_init(&pVsys->p.mutex_ppObjectStats, NULL);
#define __vsys_P_T_clean1_15 {pthread_mutex_destroy(&pVsys->p.mutex_ppObjectStats);}
    /* 解码跟踪对象,使用mutex_ppObjectStats的互斥锁 */
    pVsys->p.ppObjectStats = ppObjectStats;
    /* 解码跟踪对象的个数,使用mutex_ppObjectStats的互斥锁 */
    pVsys->p.statsCount = channelCount;
    configManager_cmd_register_and_protocol_publish(pConfigManager, "vsys_stats", "vsys", 0, pVsys, 0
            , (int(*)(const unsigned int*, unsigned char, unsigned char, unsigned int, unsigned int, FsEbml*, char, void*, FsObjectBaseBuffer * const, FsShareBuffer*))vsys_P_cmd_cb_stats, NULL, pVsys
            ////////////////////////////////////////////////////////////////////
            , "vsys_stats", "解码信息获取", 255, vsys_P_protocol_stats_get, &shareBuffer);
#define __vsys_P_T_clean2_4 {configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "vsys_stats", "vsys", 0, pVsys,"vsys_stats",&shareBuffer);}
#else
#define __vsys_P_T_clean1_15
#define __vsys_P_T_clean2_4
#endif
    /* 注册对其他主机相机状态数据处理函数 */
    configManager_return_register(pConfigManager, ConfigManager_Cluster_protocol_channelStatus_get, pVsys
            , (int (*)(const unsigned int*, unsigned char, unsigned char, unsigned int, unsigned int, FsObjectBasePthreadSafety*, void*, FsObjectBaseBuffer*, char**))vsys_P_cb_return_channelStatus, pVsys);
#define __vsys_P_T_clean2_5 {configManager_return_logoff(pConfigManager, ConfigManager_Cluster_protocol_channelStatus_get, pVsys);}
    // 注册获取集群服务器连接的回掉函数
    configManager_cluster_connect(pConfigManager, 0, pVsys, (int (*)(unsigned int, const unsigned int*, unsigned char, void*, FsObjectBaseBuffer*, char**))vsys_P_cluster_connect_cb, pVsys);
#define __vsys_P_T_clean2_6 {configManager_cluster_disconnect(pConfigManager, 0, pVsys);}
    // 集群路由 
    pRouter_cluster[0] = fs_Router_new__IO(2);
#define __vsys_P_T_clean1_16 {fs_Router_delete_pthreadSafety__IO(pRouter_cluster[0]);}
    // 本地ip映射 
#define __vsys_P_T_clean1_17
    configManager_cmd_register_and_protocol_publish(pConfigManager, "vsys_router_get", "vsys", 0, pVsys, 0
            , (int(*)(const unsigned int*, unsigned char, unsigned char, unsigned int, unsigned int, FsEbml*, char, void*, FsObjectBaseBuffer * const, FsShareBuffer*))vsys_P_cmd_cb_router, NULL, pRouter_cluster
            , "vsys_router_get", "路由信息获取", 255, vsys_P_protocol_router_get, &shareBuffer);
#define __vsys_P_T_clean2_7 {configManager_cmd_logoff_and_protocol_cancel(pConfigManager, "vsys_router_get", "vsys", 0, pVsys,"vsys_router_get",&shareBuffer);}
#define __vsys_P_T_clean2_count 7
    // rtsp服务器
    pRtsp = rtsp_new__IO(pVsys->ro._pMonitor, &childRun, systemThreadTid, "Rtsp", 0, 554, 128 * pVsys->ro._threadMulti, 0.0, &shareBuffer);
    rtsp_startThread(pRtsp, 1);
#define __vsys_P_T_clean1_18 {rtsp_stopThread(pRtsp);rtsp_delete__OI(pRtsp);}
#define __vsys_P_T_clean1_19
    // hls服务器
    pHls = hls_new__IO(pVsys->ro._pMonitor, &childRun, systemThreadTid, "Hls", access("/fs/bin/hls.html", F_OK) == 0 ? "/fs/bin/hls.html" : (access("./hls.html", F_OK) == 0 ? "./hls.html" : "/fs/bin/hls.html"), 0, 16880, 128 * pVsys->ro._threadMulti, 0.0
            , (FsObjectBase * (*)(unsigned int*, void*, FsConfig_ExportType, FsShareBuffer * const))configManager_config_get_with_sum_pthreadSafety__IO

            , (int (*)(FsEbml*, unsigned char, void*, char**))configManager_config_set_pthreadSafety__OI, pConfigManager, (FsObjectBasePthreadSafety * (*)(long long unsigned int*, void*, unsigned char, FsShareBuffer * const))vsys_P_channelStatusGet__IO, pVsys);
    hls_startThread(pHls, 0);
#define __vsys_P_T_clean1_20 {hls_stopThread(pHls);hls_delete__OI(pHls);}
    // GB28181服务器
    pGB28181 = gb28181_new__IO(pVsys->ro._pMonitor, &childRun, systemThreadTid, "GB28181", 0, 5080, "15081", 128 * pVsys->ro._threadMulti, 0.0, pConfigManager, pVsys->ro._pBindClassifyServer
            , NULL
            , (FsEbml * (*)(long long unsigned int*, void*, char**))vsys_P_channelStatusGetEbml__IO, pVsys, NULL);
    gb28181_startThread(pGB28181, 0);
#define __vsys_P_T_clean1_21 {gb28181_stopThread(pGB28181);gb28181_delete__OI(pGB28181);}    
    // gbsdk回放对象
    if (pVsys->ro._pSdkPlayBack) {
        GBSDK * const pGBSDK = gbsdk_new__IO(pMonitor, ((SdkPlayBack*) pVsys->ro._pSdkPlayBack)->ro._externRunStatus, systemThreadTid, "gbsdk", 64 * pVsys->ro._threadMulti, pVsys->ro._threadMulti, NULL
                , NULL
                , &shareBuffer);
        pSdkPlayBack_item_gbsdk = sdkPlayBack_item_new__IO__OI_2("/tmp/config/gbsdk_", pGBSDK
                , NULL /* (void (*const) (void *const, FsEbml * const))gbsdk_update_parameter */, (void (*const) (void *const))gbsdk_delete__OI
                , (void (* const) (void * const, const signed char))gbsdk_startThread, (void (*const) (void *const))gbsdk_stopThread_noWait, (void (*const) (void *const))gbsdk_stopThread
                , (void* (*)(const char*, const char*))gbsdk_PlayBack_item_new_byUrl__IO, (void (*const) (void *const))gbsdk_PlayBack_item_delete__OI
                , (void (*const) (void *const, void *const)) gbsdk_PlayBack_add_item__OI_2, (void (*const) (void *const, void *const))gbsdk_PlayBack_remove_item_noWait__IO_2
                , (void (* const) (void * const, void * const, FsObjectSdkPlayBackTask * const))gbsdk_PlayBack_item_updatePlayBackTask_pthreadSafety
                , (void (* const) (void * const, void * const, FsObjectSdkPlayBackTask * const))gbsdk_PlayBack_item_deletePlayBackTask_pthreadSafety__OI_2);
        sdkPlayBack_add_item__OI_2((SdkPlayBack*) pVsys->ro._pSdkPlayBack, pSdkPlayBack_item_gbsdk);
    } else pSdkPlayBack_item_gbsdk = NULL;
#define __vsys_P_T_clean1_22 {if(pSdkPlayBack_item_gbsdk)sdkPlayBack_item_delete__OI(pSdkPlayBack_item_gbsdk);}    


    // 文件点播
    pFileStream = fileStream_new__IO(pVsys->ro._pMonitor, pVsys->ro._externRunStatus, systemThreadTid, "RtspFileStream", "/fs/project/data/", pRtsp, pHls);
    fileStream_startThread(pFileStream, -1);
#define __vsys_P_T_clean1_23 {fileStream_stopThread(pFileStream);fileStream_delete__OI(pFileStream);}
    pMemeryFree = fs_memeryFree_new__IO();
#define __vsys_P_T_clean1_24 {fs_memeryFree_delete__OI(pMemeryFree);}
#define __vsys_P_T_clean1_count 24
    // 初始化其他模块
    vsys_P_module_init(pVsys, 0, pMemeryFree, pRouter_cluster[0]
            , &childRun, &groupSqrt
            , pRtsp
            , pHls, pGB28181, 0, &shareBuffer);
    ////////////////////////////////////////////////////////////////////////////
    pMonitorItem->rw.line = __LINE__;
    /* 主循环 */
    while (pVsys->rw.runStatus == *externRunStatus || taskResetOkCount != 0) {
        pMonitorItem->rw.cycleAlive = 1;
        monitorItem_checkStack(pMonitorItem);
        pMonitorItem->rw.line = __LINE__;
        //        FsPrintf(1,"TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTt\n");
        //        FsLogTag(10);
        while (pVsys->p.pConnectNode__channelStatusDataClientList) {
            /* 线程调试信息 */
#if 1
            pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
            struct ConfigManager_connectNode_useOnce * const pConnectNode = (struct ConfigManager_connectNode_useOnce *) pVsys->p.pConnectNode__channelStatusDataClientList;
            pVsys->p.pConnectNode__channelStatusDataClientList = pConnectNode->next;
            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
            {
                FsEbml * const pEbml = fs_Ebml_new__IO(FsEbmlEncodeMethod_UTF8, 0);
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "threadMulti", FsEbmlNodeType_Integer
                        , "comment", "线程倍频因子,最小值为1")->data.buffer = pVsys->ro._threadMulti;
                *(double*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "frameRateAvg", FsEbmlNodeType_Float
                        , "comment", "平均帧率")->data.buffer = frameRateAvg;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "sleepToTimeInterval", FsEbmlNodeType_Integer
                        , "comment", "程序运行倍率")->data.buffer = sleepToTimeInterval;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "resetSerialCount", FsEbmlNodeType_Integer
                        , "comment", "重置连续次数")->data.buffer = resetSerialCount;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelCount", FsEbmlNodeType_Integer
                        , "comment", "通道数")->data.buffer = channelCount;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "resetIndex", FsEbmlNodeType_Integer
                        , "comment", "当前正重置的通道,-1-表示重置集群")->data.buffer = resetIndex;
                if (resetIndex != channelCount) {
                    struct FsEbml_node * const parent = fs_Ebml_node_addChild(pEbml, (struct FsEbml_node *) pEbml, "resetInfo", FsEbmlNodeType_Struct);
                    unsigned int ui;
                    for (ui = 0; ui<sizeof (modules) / sizeof (modules[0]); ui++) {
                        char buffer[sizeof ("[65535/65535/65535]:vv(0x1122334455667788)-%d")];
                        const int len = sprintf(buffer, "[%u/%u/%d]:%c%c(%p)-%d", ui, taskResetOkCount, VsysTaskCount, modules[ui].tag[0], modules[ui].tag[1] ? modules[ui].tag[1] : ' ', modules[ui].p
                                , fs_NumberArray_getBit_true(resetMask + resetMaskPos, ui));
                        fs_Ebml_node_data_set(fs_Ebml_node_addChild_with_a_property(pEbml, parent, "item", FsEbmlNodeType_String, "comment", "模块信息"), len + 1, buffer);
                    }
                }
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "show_max", FsEbmlNodeType_Integer
                        , "comment", "最近一次统计模块中待处理的最大帧数")->data.buffer = show_max;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "cronframe_show_max", FsEbmlNodeType_Integer
                        , "comment", "计划任务帧阈值的参考show_max")->data.buffer = cronframe_show_max;
                *(long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "cronframethreshold", FsEbmlNodeType_Integer
                        , "comment", "计划任务帧阈值")->data.buffer = cronframethreshold;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "cronframethresholdCompensate", FsEbmlNodeType_Integer
                        , "comment", "计划任务帧阈值的补偿值")->data.buffer = cronframethresholdCompensate;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelstatuscheckmode", FsEbmlNodeType_Integer
                        , "comment", "相机状态检测方式")->data.buffer = channelstatuscheckmode;

                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelStatusCheckInCount", FsEbmlNodeType_Integer
                        , "comment", "当前正在检查的相机量")->data.buffer = channelStatusCheckInCount;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelStatusCheckBlockSize", FsEbmlNodeType_Integer
                        , "comment", "当前相机检查的块大小")->data.buffer = channelStatusCheckBlockSize;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelStatusCheckExtra", FsEbmlNodeType_Integer
                        , "comment", "可以额外检查的数量")->data.buffer = channelStatusCheckExtra;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelStatusCheckExtraFlag", FsEbmlNodeType_Integer
                        , "comment", "用于改变channelStatusCheckExtra的标记变量")->data.buffer = channelStatusCheckExtraFlag;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "channelStatusCheckIndex", FsEbmlNodeType_Integer
                        , "comment", "当前相机检查的编号")->data.buffer = channelStatusCheckIndex;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "offlineChannelCount_new", FsEbmlNodeType_Integer
                        , "comment", "本轮检测中探测到不在线的相机数量")->data.buffer = offlineChannelCount_new;
                *(unsigned long long*) fs_Ebml_node_addChild_with_a_property(pEbml, (struct FsEbml_node*) pEbml, "offlineChannelCount", FsEbmlNodeType_Integer
                        , "comment", "不在线的相机数量")->data.buffer = offlineChannelCount;
                configManager_send_pthreadSafety__OI_2_default(pConfigManager, pEbml
                        , ConfigManager_refer_connect_node_get_requestID_3(pConnectNode->requestData), ConfigManager_refer_connect_node_get_headType(pConnectNode->requestData)
                        , ConfigManager_refer_connect_node_get_checkMethod(pConnectNode->requestData), ConfigManager_refer_connect_node_get_virtualConnection(pConnectNode->requestData)
                        , ConfigManager_refer_connect_node_get_head(pConnectNode->requestData) | 0x2, ConfigManager_refer_connect_node_get_requestDataType(pConnectNode->requestData)
                        , &baseBuffer, &shareBuffer);
            }
            fsFree(pConnectNode);
#endif
        }
        //if (access("/tag_vsys", F_OK)) mkdir("/tag_vsys", 0777), FsPrintf(FsPrintfIndex, "TTTTT,pConfigManager->p.__pConfig_tmp=%p,resetIndex=%u/%u\n", pConfigManager->p.__pConfig_tmp, resetIndex, channelCount);
        fs_thread_sleepTo(sleepToTime);
        if (sleepToTimeInterval > 0)sleepToTimeInterval--;
        thisTime = fs_time_uptime_get();
        if (frameRateAvg < VsysTaskCount)frameRateAvg = VsysTaskCount;
        sleepToTime = thisTime + 1.0 / frameRateAvg / (sleepToTimeInterval + 1.0);
        //FsPrintf(1, "frameRateAvg=%f\n", frameRateAvg);
        pMonitorItem->rw.line = __LINE__;
        ////////////////////////////////////////////////////////////////////////
        // 更新配置
        if ((configCheckSum[0] != pConfigManager->ro._configCheckSum[0] || configCheckSum[1] != pConfigManager->ro._configCheckSum[1] || configCheckSum[2] != pConfigManager->ro._configCheckSum[2])
                && resetIndex == channelCount && NULL == pConfigManager->p.__pConfig_tmp) {
            pMonitorItem->rw.line = __LINE__;
            /* 上一次重置已完成 */
            configCheckSum[0] = pConfigManager->ro._configCheckSum[0], configCheckSum[1] = pConfigManager->ro._configCheckSum[1], configCheckSum[2] = pConfigManager->ro._configCheckSum[2];
            /* 配置有改动 */
            // 读取配置
            configManager_config_addRefer_pthreadSafety(pConfigManager, __LINE__);
            /* 断开原来的所有取流 */
            {
                int i;
                for (i = 0; i < channelCount; i++) {
                    if (channelStatus[i]&0x80) {
                        record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], 0);
                        channelStatus[i] &= 0x7F;
                    }
                }
            }
            if (localItemList)fs_ObjectList_delete__OI(localItemList);
            localItemList = vsys_P_get_channelCount__IO(&vsysChannel0, &cronframethreshold
                    , &channelstatuscheckmode

                    , pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            channelCount = localItemList->nodeCount;
            if (channelCount > 0)sum[0] = sum[1] = fs_Config_get_sum((FsEbml*) pConfigManager->ro.__pConfig, (struct FsEbml_node*) localItemList->pNode[localItemList->startIndex]);

            /* 重置任务线程 */
            FsPrintf2(2, "Reset from %d to %d\n", resetIndex, channelCount);
            if (resetIndex != channelCount) {
                /* 重置开方分组 */
                {
                    pthread_mutex_lock(&groupSqrt.groupSqrt_mutex);
                    Fs_GroupSqrt_resize(&groupSqrt, channelCount);
                    memset(groupSqrt.groupValue, 1, groupSqrt.groupSqrt_group);
                    // 把原来的通道指向新的分组
                    {
                        int j = 0;
                        for (; j < resetIndex; j++) {
                            {
                                unsigned int ui;
                                for (ui = Vsys_P_module_Capture_index; ui <= Vsys_P_module_Record_index; ui++) {
                                    modules[ui].reset_groupSqrtOut(modules[ui].p, j, groupSqrt.groupValue, (FsGroupSqrt*) & groupSqrt);
                                }
                            }
                        }
                    }
                    pthread_mutex_unlock(&groupSqrt.groupSqrt_mutex);
                }
#ifdef Vsys_stat_out_enable
                pthread_mutex_lock(&pVsys->p.mutex_ppObjectStats);
#endif
                if (channelCount > resetIndex) {
                    int i;
#ifndef __vsys_P_T_reset
#ifndef __vsys_P_T_reset_reduce
                    i = Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (ipsrc[0]) * channelCount) + Memery_Alignment(sizeof (frameRate[0]) * channelCount)
                            + Memery_Alignment(sizeof (moduleMask[0]) * channelCount);
                    int j = Memery_Alignment(resetIndex << 1) + Memery_Alignment(sizeof (ipsrc[0]) * resetIndex) + Memery_Alignment(sizeof (frameRate[0]) * resetIndex)
                            + Memery_Alignment(sizeof (moduleMask[0]) * resetIndex);
#endif
                    channelStatus = (unsigned char *) fsRealloc(channelStatus, i + sizeof (FsObjectStats*) * channelCount);
                    ppObjectStats = (FsObjectStats**) (channelStatus + i);
#ifndef __vsys_P_T_reset_reduce
                    memmove(ppObjectStats, channelStatus + j, sizeof (FsObjectStats*) * resetIndex);
                    //memset(moduleMask + resetIndex, 0, sizeof (moduleMask[0]) * channelCount - sizeof (moduleMask[0]) * resetIndex);
#endif
                    i -= Memery_Alignment(sizeof (moduleMask[0]) * channelCount), moduleMask = (unsigned int*) (channelStatus + i);
#ifndef __vsys_P_T_reset_reduce
                    j -= Memery_Alignment(sizeof (moduleMask[0]) * resetIndex), memmove(moduleMask, channelStatus + j, sizeof (moduleMask[0]) * resetIndex);
                    memset(moduleMask + resetIndex, 0, sizeof (moduleMask[0]) * (channelCount - resetIndex));
#endif
                    i -= Memery_Alignment(sizeof (frameRate[0]) * channelCount), frameRate = (float*) (channelStatus + i);
#ifndef __vsys_P_T_reset_reduce
                    j -= Memery_Alignment(sizeof (frameRate[0]) * resetIndex), memmove(frameRate, channelStatus + j, sizeof (frameRate[0]) * resetIndex);
                    memset(frameRate + resetIndex, 0, sizeof (frameRate[0]) * (channelCount - resetIndex));
#endif
                    ipsrc = (unsigned int *) (channelStatus + Memery_Alignment(channelCount << 1));
#ifndef __vsys_P_T_reset_reduce
                    memmove(ipsrc, channelStatus + Memery_Alignment(resetIndex << 1), sizeof (ipsrc[0]) * resetIndex);
                    memset(ipsrc + resetIndex, 255, sizeof (ipsrc[0]) * (channelCount - resetIndex));
#endif
                    channelValid = channelStatus + channelCount;
#ifndef __vsys_P_T_reset_reduce
                    memmove(channelValid, channelStatus + resetIndex, sizeof (channelValid[0]) * resetIndex);
                    memset(channelValid + resetIndex, 0, channelCount - resetIndex);

                    memset(channelStatus + resetIndex, 0, channelCount - resetIndex);

                    while (resetIndex < channelCount)ppObjectStats[resetIndex++] = image_frame_stats_new__IO();
#endif
#ifdef __vsys_P_T_reset_reduce
#undef __vsys_P_T_reset_reduce
#endif 
#endif
                } else if (channelCount > 0) {
                    int i;
                    memmove(channelStatus + channelCount, channelValid, sizeof (channelValid[0]) * channelCount), i = Memery_Alignment(channelCount << 1);
                    memmove(channelStatus + i, ipsrc, sizeof (ipsrc[0]) * channelCount), i += Memery_Alignment(sizeof (ipsrc[0]) * channelCount);
                    memmove(channelStatus + i, frameRate, sizeof (frameRate[0]) * channelCount), i += Memery_Alignment(sizeof (frameRate[0]) * channelCount);
                    memmove(channelStatus + i, moduleMask, sizeof (moduleMask[0]) * channelCount), i += Memery_Alignment(sizeof (moduleMask[0]) * channelCount);
                    memmove(channelStatus + i, ppObjectStats, sizeof (FsObjectStats*) * channelCount);
                    while (resetIndex > channelCount)fs_Object_delete_pthreadSafety__OI(ppObjectStats[--resetIndex]);
#define __vsys_P_T_reset_reduce
#ifndef __vsys_P_T_reset
#ifndef __vsys_P_T_reset_reduce
                    i = Memery_Alignment(channelCount << 1) + Memery_Alignment(sizeof (ipsrc[0]) * channelCount) + Memery_Alignment(sizeof (frameRate[0]) * channelCount)
                            + Memery_Alignment(sizeof (moduleMask[0]) * channelCount);
                    int j = Memery_Alignment(resetIndex << 1) + Memery_Alignment(sizeof (ipsrc[0]) * resetIndex) + Memery_Alignment(sizeof (frameRate[0]) * resetIndex)
                            + Memery_Alignment(sizeof (moduleMask[0]) * resetIndex);
#endif
                    channelStatus = (unsigned char *) fsRealloc(channelStatus, i + sizeof (FsObjectStats*) * channelCount);
                    ppObjectStats = (FsObjectStats**) (channelStatus + i);
#ifndef __vsys_P_T_reset_reduce
                    memmove(ppObjectStats, channelStatus + j, sizeof (FsObjectStats*) * resetIndex);
                    //memset(moduleMask + resetIndex, 0, sizeof (moduleMask[0]) * channelCount - sizeof (moduleMask[0]) * resetIndex);
#endif
                    i -= Memery_Alignment(sizeof (moduleMask[0]) * channelCount), moduleMask = (unsigned int*) (channelStatus + i);
#ifndef __vsys_P_T_reset_reduce
                    j -= Memery_Alignment(sizeof (moduleMask[0]) * resetIndex), memmove(moduleMask, channelStatus + j, sizeof (moduleMask[0]) * resetIndex);
                    memset(moduleMask + resetIndex, 0, sizeof (moduleMask[0]) * (channelCount - resetIndex));
#endif
                    i -= Memery_Alignment(sizeof (frameRate[0]) * channelCount), frameRate = (float*) (channelStatus + i);
#ifndef __vsys_P_T_reset_reduce
                    j -= Memery_Alignment(sizeof (frameRate[0]) * resetIndex), memmove(frameRate, channelStatus + j, sizeof (frameRate[0]) * resetIndex);
                    memset(frameRate + resetIndex, 0, sizeof (frameRate[0]) * (channelCount - resetIndex));
#endif
                    ipsrc = (unsigned int *) (channelStatus + Memery_Alignment(channelCount << 1));
#ifndef __vsys_P_T_reset_reduce
                    memmove(ipsrc, channelStatus + Memery_Alignment(resetIndex << 1), sizeof (ipsrc[0]) * resetIndex);
                    memset(ipsrc + resetIndex, 255, sizeof (ipsrc[0]) * (channelCount - resetIndex));
#endif
                    channelValid = channelStatus + channelCount;
#ifndef __vsys_P_T_reset_reduce
                    memmove(channelValid, channelStatus + resetIndex, sizeof (channelValid[0]) * resetIndex);
                    memset(channelValid + resetIndex, 0, channelCount - resetIndex);

                    memset(channelStatus + resetIndex, 0, channelCount - resetIndex);

                    while (resetIndex < channelCount)ppObjectStats[resetIndex++] = image_frame_stats_new__IO();
#endif
#ifdef __vsys_P_T_reset_reduce
#undef __vsys_P_T_reset_reduce
#endif 
#endif
                } else {
                    while (resetIndex > 0)fs_Object_delete_pthreadSafety__OI(ppObjectStats[--resetIndex]);
                    channelStatus = (unsigned char *) fsRealloc(channelStatus, 8);
                    ppObjectStats = (FsObjectStats **) channelStatus;
                    moduleMask = (unsigned int*) channelStatus;
                    frameRate = (float*) channelStatus;
                    ipsrc = (unsigned int *) channelStatus;
                    channelValid = channelStatus;
                }
#ifdef Vsys_stat_out_enable
                /* 解码跟踪对象,使用mutex_ppObjectStats的互斥锁 */
                pVsys->p.ppObjectStats = ppObjectStats;
                /* 解码跟踪对象的个数,使用mutex_ppObjectStats的互斥锁 */
                pVsys->p.statsCount = channelCount;
                pthread_mutex_unlock(&pVsys->p.mutex_ppObjectStats);
#endif
            }
            if (keepspaceConfig)fs_StringBuilder_delete__OI(keepspaceConfig);
            keepspaceConfig = vsys_P_ConfigUpdate_and_get_keepspaceConfig__IO(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local, &shareBuffer);
            if (uploadDataConfig)fs_StringBuilder_delete__OI(uploadDataConfig);
            uploadDataConfig = vsys_private_get_uploadConfig__IO(pConfigManager->ro.__pConfig, pConfigManager->ro.__ipList_local);
            updateConfig = 1;
            vsys_private_update_ntpConfig(pConfigManager->ro.__pConfig, "/tmp/config/ntp.cfg", pConfigManager->ro.__ipList_local);
            vsys_P_update_server(pConfigManager->ro.__pConfig, pRtsp
                    , pHls, pConfigManager->ro.__ipList_local, &shareBuffer);

            vsys_P_update_gb28181(pConfigManager->ro.__pConfig, pGB28181, pConfigManager->ro.__ipList_local, pRouter_cluster[0]
                    , &ppNode_sip, &gb28181GroupSum, &gb28181ServerSum);
            vsys_P_channelStatus_init(pConfigManager->ro.__pConfig, localItemList, vsysChannel0, channelStatus, channelStatusIndex);
            channelStatusNode = vsys_P_update_channelStatus_create(pVsys, pConfigManager->ro.__ipList_local, ipsrc);
            // 更新路由
            vsys_P_update_router(pConfigManager->ro.__pConfig, pRouter_cluster[0]
                    , pConfigManager->ro.__ipList_local);

            offlineChannelCount = FsNumberMax(offlineChannelCount); // 强制刷新一次相机状态
            channelStatusCheckIndex = 0;
            channelStatusCheckBlockSize = 1;
            channelStatusCheckInCount = 0;
            if (frameRateAvg < (VsysTaskCount << 2))frameRateAvg = (VsysTaskCount << 2);
            resetIndex = -1;
            taskResetOkCount = taskResetOkCount1 = 0;
            pMonitorItem->rw.line = __LINE__;
        }
        pMonitorItem->rw.line = __LINE__;
        if (updateConfig) {
            ////////////////////////////////////////////////////////////////////
            /* 更新磁盘管理配置 */
            if (keepspaceConfig && access("/tmp/config/keepspace.cfg.update", F_OK)) {
                if (NULL == uploadDataConfig)updateConfig = 0;
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
            ////////////////////////////////////////////////////////////////////
            /* 更新上传数据配置 */
            if (uploadDataConfig && access("/tmp/config/upload.cfg.update", F_OK)) {
                if (NULL == keepspaceConfig)updateConfig = 0;
                if (access("/tmp/config", F_OK))mkdir("/tmp/config", 0777);
                if (fs_StringBuilder_save_to_file(uploadDataConfig, "/tmp/config/upload.cfg") != 1) {
                    FsPrintf(1, "Save upload config to the file:\"%s\" failed.\n", "/tmp/config/upload.cfg");
                    fflush(stdout);
                }
                fs_StringBuilder_delete__OI(uploadDataConfig);
                uploadDataConfig = NULL;
                fd = fopen("/tmp/config/upload.cfg.update", "w");
                if (fd)fclose(fd);
                else {
                    FsPrintf(1, "Open the file:\"%s\" error:\"%s\".\n", "/tmp/config/upload.cfg.update", strerror(errno));
                    fflush(stdout);
                }
            }
            pMonitorItem->rw.line = __LINE__;
            ////////////////////////////////////////////////////////////////////
        }

        /* 重置 */
        if (resetIndex < channelCount) {
            if (NULL == pConfigManager->p.__pConfig_tmp || taskResetOkCount != 0)

            {
                pMonitorItem->rw.line = __LINE__;
                int i;
                {
                    for (i = -1; i < channelCount; i++) {
                        if (i == resetIndex) {
                            unsigned int uj;
                            for (uj = 0; uj<sizeof (modules) / sizeof (modules[0]); uj++) {
                                if (taskResetOkCount < VsysTaskCount) {
                                    if (((pVsys->rw.runStatus == *externRunStatus && NULL == pConfigManager->p.__pConfig_tmp) || taskResetOkCount != 0) && fs_NumberArray_getBit_true(resetMask + resetMaskPos, uj) == 0) {
                                        if (resetIndex < 0)Vsys_P_Module_resetChannel(modules[uj]) = resetIndex + 1;
                                        else if (modules[uj].check_channel_changed(modules[uj].p, resetIndex, vsysChannel0, localItemList->pNode[localItemList->startIndex + resetIndex], sum[0], "vsys vsysChannel")) {
                                            Vsys_P_Module_resetChannel(modules[uj]) = resetIndex + 1;
                                            ppObjectStats[resetIndex]->decodeMask = 0;
                                            ppObjectStats[resetIndex]->decodeMask_set = 0;
                                            ppObjectStats[resetIndex]->decodeWaitReferCount = 0;
                                            ppObjectStats[resetIndex]->decodeChanged = 1;
                                        }
                                        fs_NumberArray_setBit_true(resetMask + resetMaskPos, uj), taskResetOkCount++;
                                    }
                                } else if (Vsys_P_Module_resetChannel(modules[uj]) < 0 && fs_NumberArray_getBit_true(resetMask + resetMaskPos, uj)
                                        && (fs_NumberArray_setBit_false(resetMask + resetMaskPos, uj), taskResetOkCount++, resetIndex + 1 < channelCount)
                                        && pVsys->rw.runStatus == *externRunStatus && NULL == pConfigManager->p.__pConfig_tmp) {
                                    if (modules[uj].check_channel_changed(modules[uj].p, resetIndex + 1, vsysChannel0, localItemList->pNode[localItemList->startIndex + resetIndex + 1], sum[1], "vsys vsysChannel")) {
                                        Vsys_P_Module_resetChannel(modules[uj]) = resetIndex + 2;
                                        ppObjectStats[resetIndex + 1]->decodeMask = 0;
                                        ppObjectStats[resetIndex + 1]->decodeMask_set = 0;
                                        ppObjectStats[resetIndex + 1]->decodeWaitReferCount = 0;
                                        ppObjectStats[resetIndex + 1]->decodeChanged = 1;
                                    }
                                    fs_NumberArray_setBit_true(resetMask + resetMaskPos1, uj), taskResetOkCount1++;
                                } else resetSerialCount = 0;
                                if (taskResetOkCount == (VsysTaskCount << 1)) {
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
                                    taskResetOkCount = resetMaskPos, resetMaskPos = resetMaskPos1, resetMaskPos1 = taskResetOkCount;
                                    taskResetOkCount = taskResetOkCount1, taskResetOkCount1 = 0, sum[0] = sum[1];
                                    if (++resetIndex < channelCount) {
                                        /* 获取重置通道的掩码 */
                                        moduleMask[resetIndex] = vsys_P_module_init(NULL, 1, NULL, NULL
                                                , NULL, NULL
                                                , NULL
                                                , NULL, NULL
                                                , fs_Config_node_integer_get_mask(pConfigManager->ro.__pConfig, vsysChannel0, localItemList->pNode[localItemList->startIndex + resetIndex], "moduleMask", NULL), &shareBuffer);
                                        if (resetIndex % 100 == 0 || resetIndex + 100 > channelCount || resetIndex < 100) FsPrintf2(FsPrintfIndex, "Reset Channel(=%d/%d).\n", resetIndex, channelCount);
                                        if (resetIndex + 1 < channelCount)
                                            sum[1] = fs_Config_get_sum((FsEbml*) pConfigManager->ro.__pConfig, (struct FsEbml_node*) localItemList->pNode[localItemList->startIndex + resetIndex + 1]);
                                    }
                                    break;
                                }

                            }
                            if (i == resetIndex && i>-1) {
                                channelValid[resetIndex] = 0;
                            }
                        }
                        else if (i == resetIndex + 1) {
                            channelValid[resetIndex + 1] = 0;

                        }

                        if (i < resetIndex && i>-1) {
                            channelValid[i] = 1;
                        }
                    }
                }
                if (resetIndex == channelCount) {
                    configManager_config_deleteRefer_pthreadSafety(pConfigManager);
                    FsPrintf2(1, "Reset Channel(=%d/%d) finish rtspServer.item.count=%lu/%lu/%lu.\n", resetIndex, channelCount,
                            pRtsp->ro.__itemListServer->nodeCount, pRtsp->ro.__clusterItemListServer->nodeCount, pRtsp->ro.__itemListServer->nodeCount + pRtsp->ro.__clusterItemListServer->nodeCount);
                    fs_ObjectList_delete__OI(localItemList);
                    localItemList = NULL;
                } else if (0 && resetIndex >= 0) {
                    FsPrintf(1, "resetIndex=%d/%d\n", resetIndex, channelCount);
                    unsigned int uj;
                    for (uj = 0; uj<sizeof (modules) / sizeof (modules[0]); uj++) {
                        printf("[%u]:%c%c(%p)-%d\n", uj, modules[uj].tag[0], modules[uj].tag[1], modules[uj].p, fs_NumberArray_getBit_true(resetMask + resetMaskPos, uj));
                    }
                }
                pMonitorItem->rw.line = __LINE__;
            }
            else {
                /* 提前释放配置引用 */
                FsLog2(FsLogType_info, FsPrintfIndex, "Has new config,%d/%d.\n", resetIndex, channelCount);
                resetIndex = channelCount;
                configManager_config_deleteRefer_pthreadSafety(pConfigManager);
                configCheckSum[2] = 0;
            }

        }
        /* 检查相机是否在线 */
        if (channelStatusCheckInCount < channelStatusCheckBlockSize && cronframe_show_max < cronframethreshold + cronframethresholdCompensate) {
            cronframethresholdCompensate = 0;
            cronframe_show_max++;
            //                FsPrintf(1, "channelStatusCheckInCount=%u/%u,channelStatusCheckIndex=%u/%u,%#hhx,%hhu.\n",
            //                        channelStatusCheckInCount, channelStatusCheckBlockSize, channelStatusCheckIndex, channelCount, channelCount > 0 ? channelStatus[0] : 0, channelStatusIndex);
            //                fflush(stdout);
            if (channelStatusCheckIndex < resetIndex) {
                // 程序认为channelStatus[channelStatusCheckIndex] 与 channelStatus[channelStatusCheckIndex] + 0x10这种细微差是相等的（统计数差1）
                // 如果是未检测状态，并且(channelStatus[channelStatusCheckIndex]^channelStatusIndex) != 0x70 认为是不在线的,需要拉流
                if (0 == (channelStatus[channelStatusCheckIndex]&0x80) &&
                        ((channelStatus[channelStatusCheckIndex]^channelStatusIndex)& 0x70) != 0 &&
                        (((channelStatus[channelStatusCheckIndex] + 0x10)^channelStatusIndex)& 0x70) != 0) {
#ifndef __vsys_P_T_checkStatus 
                    pMonitorItem->rw.line = __LINE__;
                    record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + channelStatusCheckIndex], 1);
                    pMonitorItem->rw.line = __LINE__;
                    // 设置channelStatus,保证后4位不变,第1位设置为正在检测状态,2~4位设置为channelStatusIndex
                    channelStatus[channelStatusCheckIndex] = 0x80 | (channelStatusIndex & 0x70) | (channelStatus[channelStatusCheckIndex]&0xF);
                    //FsPrintf(1, "checkStatus %d\n ", channelStatusCheckIndex);
                    channelStatusCheckIndex++;
                    if (0 == channelStatusCheckExtraFlag) {
                        channelStatusCheckExtraFlag = 1;
                    }
                    // 为了防止通道响应时间过长，所以设置通道比channelStatusCheckInCount大1
                    if (channelStatusCheckBlockSize> ++channelStatusCheckInCount + channelStatusCheckExtra) {
                        channelStatusCheckBlockSize = channelStatusCheckInCount + channelStatusCheckExtra;
                    }
#endif
                } else {
                    // 如果通道正在检测,或者是在线的,则检测一下个
                    for (channelStatusCheckIndex++; channelStatusCheckIndex < resetIndex && ((channelStatus[channelStatusCheckIndex]&0x80) != 0
                            || ((channelStatus[channelStatusCheckIndex]^channelStatusIndex)& 0x70) == 0
                            || (((channelStatus[channelStatusCheckIndex] + 0x10)^channelStatusIndex)& 0x70) == 0); channelStatusCheckIndex++) {
                    }
                    if (channelStatusCheckIndex < resetIndex) {
#ifndef __vsys_P_T_checkStatus 
                        pMonitorItem->rw.line = __LINE__;
                        record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + channelStatusCheckIndex], 1);
                        pMonitorItem->rw.line = __LINE__;
                        // 设置channelStatus,保证后4位不变,第1位设置为正在检测状态,2~4位设置为channelStatusIndex
                        channelStatus[channelStatusCheckIndex] = 0x80 | (channelStatusIndex & 0x70) | (channelStatus[channelStatusCheckIndex]&0xF);
                        //FsPrintf(1, "checkStatus %d\n ", channelStatusCheckIndex);
                        channelStatusCheckIndex++;
                        if (0 == channelStatusCheckExtraFlag) {
                            channelStatusCheckExtraFlag = 1;
                        }
                        // 为了防止通道响应时间过长，所以设置通道比channelStatusCheckInCount大1
                        if (channelStatusCheckBlockSize> ++channelStatusCheckInCount + channelStatusCheckExtra) {
                            channelStatusCheckBlockSize = channelStatusCheckInCount + channelStatusCheckExtra;
                        }
#endif
                    }
                }
            }
            //                FsPrintf2(1, "channelStatusCheckInCount=%u/%u,channelStatusCheckIndex=%u/%u,%#hhx,%hhu.\n",
            //                        channelStatusCheckInCount, channelStatusCheckBlockSize, channelStatusCheckIndex, channelCount, channelCount > 0 ? channelStatus[0] : 0, channelStatusIndex);
            //                fflush(stdout);
        }
        pMonitorItem->rw.line = __LINE__;
        /* 数据帧传送 */
        {
            char hasFrame = 0;
            int i;
            for (i = 0; i < channelCount; i++) {
                if (i % groupSqrt.groupSqrt_member == 0) {
                    if (groupSqrt.groupValue[i / groupSqrt.groupSqrt_member])groupSqrt.groupValue[i / groupSqrt.groupSqrt_member] = 0;
                    else if (/* 下一次显示统计的时间未到 */nextShowTime >= thisTime && /*　通道状态未有变化 */0 == channelChange) {
                        /* 跳过当前组 */
                        i += groupSqrt.groupSqrt_member - 1;
                        continue;
                    }
                }
                if (1 == channelValid[i]) {
                    pMonitorItem->rw.line = __LINE__;
                    // 采集
#define __Vsys_P_T_frameCount 8
                    FsObjectImageFrame * ppFrame[__Vsys_P_T_frameCount];
                    unsigned int ui = modules[Vsys_P_module_Capture_index].frame_out__IO_4(pCapture, i, __Vsys_P_T_frameCount, ppFrame);
                    //FsPrintf(1, "i=%d,ui=%u,frameRateAvg=%f\n", i, ui, frameRateAvg), fflush(stdout);
                    if (ui > 0) {
                        hasFrame = 1;
                        /* 设置pFrame的stats属性 */
                        {
                            FsObjectStats * const pObjectStats = ppObjectStats[i];
                            {
                                int referCount = ((FsObject*) pObjectStats)->referCount;
                                if (referCount < pObjectStats->decodeLastReferCount) {
                                    int diff = pObjectStats->decodeWaitReferCount - (pObjectStats->decodeLastReferCount - referCount);
                                    if (diff < 0) {
                                        /* 更新 */
                                        //FsPrintf(1, "i=%d/%d,decodeMask=%llx/%llx,decodeChanged=%hhu,referCount=%d\n", i, channelCount, pObjectStats->decodeMask, pObjectStats->decodeMask_set, pObjectStats->decodeChanged, referCount), fflush(stdout);
                                        if (pObjectStats->decodeMask != pObjectStats->decodeMask_set) {
                                            pObjectStats->decodeMask = pObjectStats->decodeMask_set;
                                            /* 标记解码信息发生变化 */
                                            pObjectStats->decodeChanged = 1;
                                        } else {
                                            /* 标记解码信息无变化 */
                                            pObjectStats->decodeChanged = 0;
                                        }
                                        pObjectStats->decodeMask_set = 0;
                                        pObjectStats->decodeWaitReferCount = referCount;
                                    } else pObjectStats->decodeWaitReferCount = diff;
                                }
                            }
                            if (1 == ui) {
                                image_frame_set_stats_pthreadSafety(ppFrame[0], pObjectStats);
                            } else {
                                unsigned int uj;
                                for (uj = 0; uj < ui; uj++) image_frame_set_stats_pthreadSafety(ppFrame[uj], pObjectStats);
                            }
                            pObjectStats->decodeLastReferCount = ((FsObject*) pObjectStats)->referCount;
                        }
                        //                    for (uj = 0; uj < ui; uj++) {
                        //                        FsPrintf(1, "pFrame=%p,moduleMask[%d]=%x\n", ppFrame[uj], i, moduleMask[i]);
                        //                        fflush(stdout);
                        //                    }
                        frameRate[i] = ppFrame[ui - 1]->frameRate;
                        frameRateAvg = frameRateAvg + (frameRate[i] - frameRateAvg) / channelCount;
                        if (ipsrc[i] != ppFrame[ui - 1]->ipsrc) {
                            ipsrc[i] = ppFrame[ui - 1]->ipsrc, channelChange = 1;
                            pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
                            /* 内部顺序为<item><uuid>tt</uuid><status>0</status><checked>0</checked><ipsrc>255.255.255.255</ipsrc></item> */
                            channelStatusNode->child[i]->child[3]->data.lenth = 1 + sprintf(channelStatusNode->child[i]->child[3]->data.buffer, "%hhu.%hhu.%hhu.%hhu"
                                    , ((unsigned char*) (ipsrc + i))[0], ((unsigned char*) (ipsrc + i))[1], ((unsigned char*) (ipsrc + i))[2], ((unsigned char*) (ipsrc + i))[3]);
                            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
                        }
                    }
                    // 其他模块数据帧之间的传递 
                    {
                        unsigned int uj;
                        for (uj = Vsys_P_module_Capture_index + 1; uj < Vsys_P_module_Record_index; uj++) {
                            if ((moduleMask[i] >> uj)&0x1) {
                                if (ui > 0)modules[uj].frame_in__OI_4(modules[uj].p, i, ui, ppFrame, &baseBuffer, &shareBuffer);
                                ui = modules[uj].frame_out__IO_4(modules[uj].p, i, __Vsys_P_T_frameCount, ppFrame);
                            }
                        }
                    }
                    // 记录
                    if (ui > 0) {
                        if (record_item_frame_in_pthreadSafety__OI_4(pRecord, i, ui, ppFrame, &baseBuffer, &shareBuffer) == 1) {
                            // 如果是正在检测流状态中,停止取流
                            if (channelStatus[i]&0x80) {
                                pMonitorItem->rw.line = __LINE__;
                                record_item_realStream_for_checkStatus((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i], 0);
                                pMonitorItem->rw.line = __LINE__;
                                channelStatusCheckInCount--;
                                if (channelStatusCheckExtraFlag >= 1)channelStatusCheckExtraFlag++;
                            }
                            //FsPrintf(1, "channelStatus[%d]=%hhx\n", i, channelStatus[i]);
                            // 后3位不变,更新相机状态为channelStatusIndex,认为是在线的
                            if (channelStatus[i] != ((channelStatus[i]&0x07) | channelStatusIndex)) {
                                if ((channelStatus[i]&0x1) == 0)offlineChannelCount = FsNumberMax(offlineChannelCount);
                                //FsPrintf(1, "channelStatus[%d]=%hhx,offlineChannelCount=%x\n", i, channelStatus[i], offlineChannelCount);
                                channelStatus[i] = (channelStatus[i]&0x07) | channelStatusIndex;
                            }
                        }
                    }
                    ui = record_item_frame_out_pthreadSafety__IO_4(pRecord, i, __Vsys_P_T_frameCount, ppFrame);
                    /* 删除数据帧 */
                    while (ui-- > 0) fs_Object_delete_pthreadSafety__OI(ppFrame[ui]);
                    pMonitorItem->rw.line = __LINE__;
                }
                /* 统计缓存 */
                // 3秒向客户端更新一次相机状态
                if (nextShowTime < thisTime) {
                    /* 统计相机状态 */
                    // 相机数据是通的
                    if ((channelStatus[i]&0x8) != 0) {
                        // 相机正在检测
                        if (channelStatus[i]&0x80) {
                            // channelStatus[i]与channelStatusIndex的统计计数位不等,认为不在线
                            if (((channelStatus[i]^channelStatusIndex)& 0x70) != 0 && (((channelStatus[i] + 0x10)^channelStatusIndex)& 0x70) != 0) {
                                // 设置为不在线
                                channelChange = 1, channelStatus[i] &= 0xF7;
                                offlineChannelCount_new++;
                            }
                        }
                    } else {
                        if ((channelStatus[i]&0x81) == 0x80) {
                            /* 没有标记为已检测过 */
                            // channelStatus[i]与channelStatusIndex的统计计数位不等,认为不在线
                            if (((channelStatus[i]^channelStatusIndex)& 0x70) != 0 && (((channelStatus[i] + 0x10)^channelStatusIndex)& 0x70) != 0) {
                                // 设置为不在线
                                channelChange = 1, channelStatus[i] |= 0x1;
                            }
                        }
                        offlineChannelCount_new++;
                    }
                    //FsPrintf(1, "i=%d/%d,%u/%u,%#hhx/%#hhx\n", i, channelCount, offlineChannelCount_new, offlineChannelCount, channelStatus[i], channelStatusIndex);
                    if (1 == channelValid[i]) {
                        //                FsPrintf2(1, "CH:%u %.2ffps C%lu R%lu\n", i + 1, frameRate[i], ((struct Capture_item *) pCapture->ro.__itemList->pNode[pCapture->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount,
                        //                        ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistIn->nodeCount +
                        //                        ((struct Record_item *) pRecord->ro.__itemList->pNode[pRecord->ro.__itemList->startIndex + i])->ro.__framelistOut->nodeCount);
                        pMonitorItem->rw.line = __LINE__;
                        if (show_fps < frameRate[i])show_fps = frameRate[i], show_fps_index = i;
                        //FsPrintf2(1, "[%d--%d]:%05.2f", i, channelCount, frameRate[i]);
                        unsigned int uj;
                        for (uj = 0; uj<sizeof (modules) / sizeof (modules[0]); uj++) {
                            const int ui = modules[uj].get_frame_count(modules[uj].p, i);
                            if (modules[uj].show_max < ui)modules[uj].show_max = ui;
                            //FsPrintf2Data(" %s%u", modules[uj].tag, ui);
                        }
                        //printf("\n");
                    }
                    if (i + 1 == channelCount) {
                        if (resetIndex == channelCount) FsPrintf2(1, "[%d-%d]:%05.2f", channelCount, show_fps_index, show_fps);
                        else FsPrintf2(1, "[%d/%d-%d]:%05.2f", resetIndex, channelCount, show_fps_index, show_fps);
                        {
                            unsigned int uj;
                            for (show_max = 0, uj = 0; uj<sizeof (modules) / sizeof (modules[0]); uj++) {
                                const int ui = modules[uj].show_max;
                                modules[uj].show_max = 0;
                                if (show_max < ui)show_max = ui;
                                FsPrintf2Data(" %s%u", modules[uj].tag, ui);
                            }
                        }
                        printf("\n");
                        show_fps = 0.0;
                        //                    FsPrintf(1, " show_max =%u, cronframethreshold=%hu,%d,channelStatusCheckInCount=%d, channelStatusCheckBlockSize=%d\n",
                        //                            show_max, cronframethreshold, show_max < cronframethreshold, channelStatusCheckInCount, channelStatusCheckBlockSize);
                        if (0 && access("/tmp/showipsrc", F_OK) == 0) {
                            int ttttt = 0;
                            FsPrintf(1, "showipsrc channelCount=%d\n", channelCount);
                            for (; ttttt < channelCount; ttttt++) {
                                printf("[%d]%s-%s/%hhu.%hhu.%hhu.%hhu\n", ttttt
                                        , channelStatusNode->child[i]->child[0]->data.buffer, channelStatusNode->child[i]->child[3]->data.buffer,
                                        ((unsigned char*) (ipsrc + ttttt))[0], ((unsigned char*) (ipsrc + ttttt))[1], ((unsigned char*) (ipsrc + ttttt))[2], ((unsigned char*) (ipsrc + ttttt))[3]);
                            }
                            printf("\n");
                            FsJson *pJson = fs_Ebml_convert_to_Json__IO(pVsys->p.channelStatusData);
                            fs_Json_save_to_file(pJson, "/tmp/showipsrc.txt", 0, NULL, FsStringLenData("\n"), 4);
                            fs_Json_delete__OI(pJson, &shareBuffer);
                        }
                        /* 判断通道状态是否发生改变 */
                        if (channelChange || offlineChannelCount != offlineChannelCount_new) {
                            /* 通道状态发生改变了 */
                            channelChange = 0, offlineChannelCount = offlineChannelCount_new;
                            /* 更新通道数据 */
                            vsys_P_update_channelStatus(pVsys->p.channelStatusData, channelStatusNode, channelStatus, channelCount, offlineChannelCount);
                            /* 向所有客户端发送数据 */
                            pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
                            Vsys_P_update_channelStatusIndex(pVsys->p.channelStatusData, pVsys->p.channelStatusIndex);
                            if (pVsys->p.channelStatusData_ebml)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_ebml), pVsys->p.channelStatusData_ebml = NULL;
                            if (pVsys->p.channelStatusData_xml)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_xml), pVsys->p.channelStatusData_xml = NULL;
                            if (pVsys->p.channelStatusData_json)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_json), pVsys->p.channelStatusData_json = NULL;
                            configManager_conncet_refer_send(pConfigManager, pVsys->p.channelStatusDataClientList,
                                    &pVsys->p.channelStatusData_ebml, &pVsys->p.channelStatusData_xml, &pVsys->p.channelStatusData_json, pVsys->p.channelStatusData, 0x2, &baseBuffer, &shareBuffer);
                            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
                            /* 更新并发送集群通道信息 */
                            pthread_mutex_lock(&pVsys->p.channelStatusDataClientList_all->mutex);
                            vsys_P_channelStatus_all_send(pVsys, &baseBuffer, &shareBuffer);
                            pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
                            /* 通知hls相机状态发生改变了 */
                            hls_update_channelStauts_pthreadSafety(pHls);

                            /* 通知gb28181相机状态发生改变了 */
                            gb28181_update_channelStauts_pthreadSafety(pGB28181);
                        }
                        offlineChannelCount_new = 0;
                        if (cronframe_show_max <= show_max) {
                            cronframe_show_max = show_max;
                            if (cronframethreshold>-128) cronframethresholdCompensate++;
                        } else cronframe_show_max = (cronframe_show_max * 3 + show_max) / 4;
                        if (channelStatusCheckIndex == channelCount) {
                            /* 完成了一轮检查了 */ // channelStatusIndex统计计数加1
                            channelStatusIndex = (channelStatusIndex + 0x10)& 0x7F;
                            channelStatusCheckIndex = 0;
                        } else if (channelStatusCheckInCount == channelStatusCheckBlockSize) {
                            if (1 == channelStatusCheckExtraFlag) {
                                channelStatusCheckExtra++;
                                channelStatusCheckBlockSize += 1 + channelStatusCheckExtra / 2;
                            } else channelStatusCheckBlockSize++;
                        }
                        if (channelStatusCheckExtraFlag > 2) {
                            channelStatusCheckExtraFlag = (channelStatusCheckExtraFlag - 1) / 2;
                            if (channelStatusCheckExtra >= channelStatusCheckExtraFlag) {
                                channelStatusCheckExtra -= channelStatusCheckExtraFlag;
                                if (channelStatusCheckExtra < channelstatuscheckmode)channelStatusCheckExtra = channelstatuscheckmode;

                            } else channelStatusCheckExtra += (channelStatusCheckExtraFlag - channelStatusCheckExtra) / 2;
                        }
                        channelStatusCheckExtraFlag = 0;
                        nextShowTime = thisTime + 3.0;
                    }
                } else if (channelChange && i + 1 == channelCount && offlineChannelCount != FsNumberMax(offlineChannelCount) /* 为FsNumberMax(offlineChannelCount)表示强制更新 */) {
                    /* 通道状态发生改变了 */
                    channelChange = 0;
                    /* 向所有客户端发送数据 */
                    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList->mutex);
                    Vsys_P_update_channelStatusIndex(pVsys->p.channelStatusData, pVsys->p.channelStatusIndex);
                    if (pVsys->p.channelStatusData_ebml)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_ebml), pVsys->p.channelStatusData_ebml = NULL;
                    if (pVsys->p.channelStatusData_xml)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_xml), pVsys->p.channelStatusData_xml = NULL;
                    if (pVsys->p.channelStatusData_json)fs_ObjectBasePthreadSafety_delete__OI(pVsys->p.channelStatusData_json), pVsys->p.channelStatusData_json = NULL;
                    configManager_conncet_refer_send(pConfigManager, pVsys->p.channelStatusDataClientList
                            , &pVsys->p.channelStatusData_ebml, &pVsys->p.channelStatusData_xml, &pVsys->p.channelStatusData_json, pVsys->p.channelStatusData, 0x2, &baseBuffer, &shareBuffer);
                    pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList->mutex);
                    /* 更新并发送集群通道信息 */
                    pthread_mutex_lock(&pVsys->p.channelStatusDataClientList_all->mutex);
                    vsys_P_channelStatus_all_send(pVsys, &baseBuffer, &shareBuffer);
                    pthread_mutex_unlock(&pVsys->p.channelStatusDataClientList_all->mutex);
                    /* 通知hls相机状态发生改变了 */
                    hls_update_channelStauts_pthreadSafety(pHls);

                    /* 通知gb28181相机状态发生改变了 */
                    gb28181_update_channelStauts_pthreadSafety(pGB28181);
                }
            }
            if (0 == hasFrame)frameRateAvg *= 0.9999;
        }
        pMonitorItem->rw.line = __LINE__;
    }
    pMonitorItem->rw.line = __LINE__;
    ////////////////////////////////////////////////////////////////////////////
    childRun = -128;
    /* 2类数据:需要在线程标记为结束状态前释放或析构数据,使用*_clean2_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(2))); // 打印析构脚本
    FsClean(2);
    pMonitorItem->rw.line = __LINE__;
    //________________________________________________________________________// 
    /* 释放配置引用 */
    if (resetIndex < channelCount) configManager_config_deleteRefer_pthreadSafety(pConfigManager);
    ////////////////////////////////////////////////////////////////////////////
    /* 1类数据:可在线程标记为结束状态后释放或析构数据,使用*_clean1_*标记 */
    pMonitorItem->rw.line = __LINE__;
    //FsPrintf(FsPrintfIndex,"%s\n", FsMacrosString(FsClean(1))); // 打印析构脚本
    FsClean(1);
    pMonitorItem->rw.line = __LINE__;
#ifdef FsDebug
    FsLog2(FsLogType_threadDebug, FsPrintfIndex, "Thread:\"%s\" exit,runStatus=%hhd.\n", Vsys_get_threadName(pVsys), pVsys->rw.runStatus);
#endif 
    /* 删除线程监控 */
    monitorItem_remove_and_delete(pMonitor, pMonitorItem, NULL);
    if (NULL == pVsys->ro._pMonitor)monitor_delete__OI(pMonitor);

    return NULL;
}
///////////////////////////定义私有函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////定义公共函数开始//////////////////////////////////////

/* 
 * 创建一个新的Vsys对象;
 * 返回Vsys指针.
 */
struct Vsys *vsys_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
        , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
        , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
        , /* 线程倍频因子,最小值为1 */ const unsigned int threadMulti
        , /* 配置管理对象 */ void *const pConfigManager, /* 监听分类服务器对象,可为空 */ void *const pBindClassifyServer
        , /* 回放对象,为空表示内部不创建回放对象 */ void * const pSdkPlayBack) {
#ifdef loglevel
    if (loglevel == FsNumberMax(loglevel)) fs_log_addLevel(&loglevel, "Camera_ctrl_ys_http", NULL);
#endif
    unsigned int len = strlen(threadName) + 1;
    struct Vsys * const rst = (struct Vsys*) fsMalloc(sizeof ( struct Vsys) +len);
    memset(rst, 0, sizeof ( struct Vsys));
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
    /* 线程倍频因子,最小值为1 */
    rst->ro._threadMulti = threadMulti;
    /* 配置管理对象 */
    rst->ro._pConfigManager = pConfigManager;
    /* 监听分类服务器对象,可为空 */
    rst->ro._pBindClassifyServer = pBindClassifyServer;
    /* 回放对象,为空表示内部不创建回放对象 */
    rst->ro._pSdkPlayBack = pSdkPlayBack;

    return rst;
}

/* 删除pVsys指向的实例对象 */
void vsys_delete__OI(struct Vsys * const pVsys) {

    fsFree(pVsys);
}

/* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
void vsys_startThread(struct Vsys * const pVsys
        , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice) {
    //#define __inRun_set_in_thread_start
    //#define __inRun_set_in_thread_end
#define __inRun_set_noinRun
#define __inRun_set_var pVsys
#ifndef __inRun_set 
#ifdef FsDebug
#ifndef __inRun_set_noinRun
    if ((__inRun_set_var)->p.inRun !=
#ifdef __inRun_set_in_thread_start
            1
#elif defined __inRun_set_in_thread_end
            3
#else
            0
#endif
            ) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid inRun(=%hhu).\n", (__inRun_set_var)->p.inRun);
        fflush(stdout);
        FsLogExit();
    }
#endif  
#if !defined __inRun_set_in_thread_start && !defined __inRun_set_in_thread_end
    if ((__inRun_set_var)->p.stid != 0) {
        FsLog(FsLogType_fatal, FsPrintfIndex, "Invalid stid(=%ld).\n", (__inRun_set_var)->p.stid);
        fflush(stdout);
        FsLogExit();
    }
#endif
#endif
#ifndef __inRun_set_noinRun
#ifdef __inRun_set_in_thread_start
    (__inRun_set_var)->p.inRun = 3;
#elif defined __inRun_set_in_thread_end
    (__inRun_set_var)->p.inRun = 2;
#else
    (__inRun_set_var)->p.inRun = 1;
#endif
#endif
#ifdef __inRun_set_in_thread_start
#undef __inRun_set_in_thread_start
#endif
#ifdef __inRun_set_in_thread_end
#undef __inRun_set_in_thread_end
#endif
#ifdef __inRun_set_noinRun
#undef __inRun_set_noinRun
#endif
#undef __inRun_set_var
#endif
    pVsys->rw.runStatus = 1;
    pVsys->rw.thisNice = thisNice;
#ifdef FsDebug
    VsysLog2(FsLogType_threadDebug, "Init Parameter");
#endif
#define __fs_thread_attr_stacksize 64*1024
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
    if (pthread_create(&pVsys->p.stid, &attr, (void* (*)(void*))vsys_P_T, pVsys)) {

        FsLog(FsLogType_fatal, FsPrintfIndex, "Create thread error[%d/%d]:\"%s\".\n", errno, WSAGetLastError(), strerror(errno));
        fflush(stdout);
        FsLogExit();
    }
    pthread_attr_destroy(&attr);
}

/* 结束线程,只发送信号,不回收线程资源 */
void vsys_stopThread_noWait(struct Vsys * const pVsys) {
#define __stopThread_var pVsys
#define __stopThread_noWait
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

/* 结束线程 */
void vsys_stopThread(struct Vsys * const pVsys) {
#define __stopThread_var pVsys
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

///////////////////////////定义公共函数结束//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
