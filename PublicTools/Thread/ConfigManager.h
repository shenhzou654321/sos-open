/* 
 * File:   ConfigManager.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#if !defined CONFIGMANAGER_H && !defined JAVA_IN_C

#include "../../PublicTools/publicDefine.h"
#define CONFIGMANAGER_enable
#define CONFIGMANAGER_H

#include "../../PublicTools/Fs/Config.h"
#include "../../PublicTools/Fs/Memery.h"
#ifdef FsDebug
#include "../../PublicTools/Fs/StructList.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif
    /* 
     * 用户认证掩码设计,总共64种权限
     * 0-15共16种为系统预设;
     * 16-63共48种为组权限; 
     */
    /*
     * ConfigManager权限认证流程(未实现)
     *  tcp权限认证 
     *    c -> s 客户端发送请求指令及用户名
     *    s -> c 
     *      如需要认证则回执出错指令,code为一随机值
     *        c -> s 发送请求指令及用户名及密码,并注明密码的加密方式,支持usename+code+passwd md5认证
     *        s -> c 回执指令执行结果
     *      不需要认证回执指令执行结果
     *    之后通信不再校验用户名及密码
     * 
     *  http权限认证(数据区使用tcp的数据,包含tcp的头)
     *    c -> s 客户端发送请求指令及用户名
     *    s -> c 回执重定向指令,url为tcp中的code值,同时内容字段也包含
     *    c -> s 发送请求指令及用户名及密码,并注明密码的加密方式,支持usename+code+passwd md5认证
     *    s -> c 回执指令执行结果
     *    之后通信不再校验用户名及密码
     */
    /* 调用集群连接协议分类所使用的掩码,在各应用时可重新定义,ConfigManager不使用此定义,集群中的节点也可以使用不同的定义 */
    ////////////////////////////////////////////////////////////////////////////
    /* 各模块(属于PublicTools)发布协议使用classIndex的最小值 */
#define ConfigManager_Module5_protocol_classIndex0 64
#define ConfigManager_Cluster_return_mask 0xF0U

    /* 具体协议 */
    typedef enum {
        /* 通道状态获取 */
        ConfigManager_Cluster_protocol_channelStatus_get = 0x10U,
        /* 视频时间信息获取 */
        ConfigManager_Cluster_protocol_videotimeinfo_get = 0x20U,
        /* GB28181相关 */
        ConfigManager_Cluster_protocol_GB28181 = 0x30U,
        /* 相机控制相关(通知类) */
        ConfigManager_Cluster_protocol_Control_notice = 0x40U,
        /* 相机控制相关(询问类) */
        ConfigManager_Cluster_protocol_Control = 0x50U,
        /* 集群节点间ConfigManager模块使用的值,一般用于配置同步,此值为ConfigManager保留值,不可重定义 */
        ConfigManager_Cluster_protocol_Cluster_ConfigManager = 0xF0U,
    } ConfigManager_Cluster_protocol;

    ////////////////////////////////////////////////////////////////////////////

    /* 用户信息 */
    struct ConfigManager_User {
        /* 用户信息在配置中的节点位置 */
#define ConfigManager_User_node "ConfigManager_User"
        /* 用户信息中用户节点的键名,为模板节点 */
#define ConfigManager_User_user "user"
        /* 用户信息中用户名的键名 */
#define ConfigManager_User_username "username"
        /* 用户信息中密码的键名 */
#define ConfigManager_User_password "password"
        /* 用户信息中权限的键名 */
#define ConfigManager_User_authority "authority"
        /* 权限值 */
        unsigned long long authority;
        /* 此权限的精简注释,不能为空 */
        const char *shortcut;
        /* 此权限的完整注释,可为空 */
        const char *comment;
    };

    /* 
     * 配置管理
     * 基本模式是收到客户端请求后只发送不超过一条消息通知客户端;
     */
    typedef struct {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 
             * 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出 
             */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 线程名 */
#define ConfigManager_get_threadName(___pConfigManager) (((char*)((___pConfigManager)+1))+(___pConfigManager)->ro._threadName_)
            unsigned char _threadName_;
            /* 标记是否需要重新创建配置,0-不需要,1-需要 */
            unsigned char createConfigStatus : 1;
            /* 配置文件名 */
#define ConfigManager_get_configFileName(___pConfigManager) (((char*)((___pConfigManager)+1))+(___pConfigManager)->ro._configFileName_)
            unsigned short _configFileName_;
            /* 系统信息的目录,为空表示不支持系统信息查询指令 */
#define ConfigManager_get_sysinfoDir(___pConfigManager,___nullValue) (!FsNumberIsMax((___pConfigManager)->ro._sysinfoDir_)?(((char*)((___pConfigManager)+1))+(___pConfigManager)->ro._sysinfoDir_):(___nullValue))
            unsigned short _sysinfoDir_;
            /* 默认要监听ipv4地址端口号,为0表示没有默认监听,全使用配置文件,在配置文件也定义此ip及端口时,使用配置文件中的定义 */
#define ConfigManager_default_ipv4Port 16863
            unsigned short _ipv4Port;
            /* 默认要监听的ipv4地址,为0表示监听所有地址,在配置文件也定义此ip及端口时,使用配置文件中的定义 */
            unsigned int _ipv4;
            /* 线程同步锁,仅用于读写配置 */
            pthread_mutex_t __mutex;
            /* 用户权限信息表,为空表示无用户功能,不为空时最后一个数组元素的权限为0 */
            struct ConfigManager_User *__pConfigManager_User;
            /* 监听分类服务器对象,可为空 */
            void *_pBindClassifyServer;
            /* 配置管理权限掩码,为0表示所有用户都有此权限 */
            unsigned long long _configManageAuthorityMask;
            /* 创建默认配置时要调用的函数链表 */
            FsStructList *__crateConfigList;
            /* 注册的命令链表 */
            FsObjectList *__cmdList;
            /* 期望连接的命令字链表,使用__cmdList中互斥锁 */
            FsObjectList *__cmdConnectList__cmdList;
            /* 发布的协议链表 */
            FsObjectList *__protocolList;
            /* 发送的数据链表 */
            FsObjectList *__sendList;
            /* 有条件的发送的数据链表 */
            FsObjectList *__conditionSendList;
            /* 等待发送数据链表,需要进一步确认是否发送 */
            FsObjectList *__waitSendList;
            /* 掩码链表 */
            FsObjectList *__maskList;
            /* 集群连接信息,储存的是SocketPool_item指针,按集群的本地ip排序;
             *     pSocketPool_item->externData的前sizeof(unsigned int)字节储存连接ID,ID只使用31位:
             *         最高位用于设置连接的强制断开;
             *     pSocketPool_item->externData+sizeof(unsigned int)的sizeof(unsigned int)字节储存服务器端发送数据的序号,只使用3字节(高位的三字节);
             *     pSocketPool_item->externData+sizeof(unsigned int)*2的sizeof(unsigned int)字节储存通信的引用计数,操作引用计数时需要使用__clusterConnectionList的互斥锁; 
             *         第二个unsigned int的低字节的储存集群同步状态:
             *              6-可作为集群通信的连接,未验证(不主动发送心跳);
             *              5-表示已通知对方本使用本连接作为集群通信(发送本机的本地ip)(不主动发送心跳);
             *              4-表示已发送版本询问,正等待回执中(不主动发送心跳);
             *              3-已发送新的配置数据,正等待回执中(不主动发送心跳);
             *              2-标记为版本过期,等待被其他客户端同步中;         
             *              1-已准备好集群间通信的所有信息;
             *              0-普通的连接;
             * //pSocketPool_item->externData+sizeof(unsigned int)*2+sizeof(void*)的sizeof(void*)字节储存集群对端报告本地ip表(格式为4字节ip个数+ip+..+ip),仅当第二个unsigned int的低字节不为0时有效;
             * 在通信时遇到冲突时,优先使用本地ip较小的发起的连接;
             */
            FsStructList *__clusterConnectionList;
            /* 注册的获取集群节点连接的链表,使用__clusterConnectionList的互斥锁 */
            FsObjectList *__clusterConnectList_;
            /* 回执数据匹配链表 */
            FsObjectList *__returnList;
            /* 连接出错时的回调链表,使用两个void*储存 */
            FsStructList *__connectErrorList;
            /* 数据分发链表 */
            FsObjectList *__transferList;
            /* 签名信息,不需要锁 */
            FsObjectList *__signatureList_;
            /* 配置发生改变时,会逐个通知此链表中的管道,节点大小为sizeof(int) */
            FsStructList *__configChangePipeList;
            /* 配置文件对象,外部在读取配置时应先锁住,把configUseCount+1后再读取 */
            FsConfig *__pConfig;
            /* 集群中本机的ID,第一个int代表ID个数,使用本地ip来表示ID,外部在读取配置时应先锁住,把configUseCount+1后再读取 */
            unsigned int *__pClusterID_local_hi;
            /* 本机的IP地址和id(id也使用ipv4的方式表示,如ip映射和vpn连接时需要设置),外部在读取配置时应先锁住,把configUseCount+1后再读取 */
            FsArray *__ipList_local;
            /* 模块掩码 */
            unsigned long long _mask;
            /* 配置校验和,前2个int是配置的绝对校验和,最后一个int是定时器切换的校验和,最后一个unsigned int不为0 */
            unsigned int _configCheckSum[3];
            /* 通道数限制 */
            unsigned int _channelCount;
            /* 配置正在被使用的数量,只有为0时才会把临时配置文件对象拷贝到配置文件中 */
            unsigned int configUseCount;
            /* 回执数据掩码 */
            unsigned int _return_mask;
#ifdef FsDebug
            /* 引用信息,用于调试 */
            FsStructList *__referInfo;
#endif
        } ro;

        struct {
            /* 运行状态:
             *     与_externRunStatus指向值相同-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             * 引用的指针类型必须是(char*);
             * 引用此值的其他线程应实现当自己的运行状态值[0-255]与此值[-128-127]不相等时退出.
             */
            unsigned char runStatus;
            /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级,此值会影响到子线程,再开始线程时会设置此值为用本线程的优先级 */
            signed char thisNice;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 线程的tid号 */
            pthread_t stid;
            /* 临时配置文件的类型,在设置__pConfig_tmp时同步设置,0-是配置导入数据,1-是配置合并数据 */
            unsigned char pConfig_tmp_type;
            /* 临时配置文件对象,外部检查到此值不为空时应释放引用 */
            FsEbml *__pConfig_tmp;
            /* 同步时发送的二进制数据 */
            FsObjectBasePthreadSafety *pConfigSyncData;
            /* 系统信息数据改变的编号 */
            unsigned long long sysinfoIndex;
            /* 获取配置的客户端链表 */
            FsStructList *configDataClientList;
            /* 获取系统信息数据的客户端链表 */
            FsStructList *sysinfoDataClientList;
            /* 系统信息数据,为空表示无 */
            FsEbml *sysinfo;
            /* 系统信息数据ebml数据 */
            FsObjectBasePthreadSafety *sysinfo_ebml;
            /* 系统信息数据xml数据 */
            FsObjectBasePthreadSafety *sysinfo_xml;
            /* 系统信息数据json数据 */
            FsObjectBasePthreadSafety *sysinfo_json;
            /* 各种格式的配置缓存数据,修改数据时要使用__mutex锁 */
            FsObjectBasePthreadSafety* __configData[FsConfig_ExportType_max + 1];
        } p;
    } ConfigManager;
#define __ConfigManagerLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",p=%p\n",##__VA_ARGS__\
        ,pConfigManager)
#define ConfigManagerLog(___loglevel,___format, ...) __ConfigManagerLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define ConfigManagerLog2(___loglevel,___format, ...) __ConfigManagerLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

#ifdef CONFIGMANAGER_enable
    /* 
     * 创建一个新的ConfigManager对象;
     * 返回ConfigManager指针.
     */

    ConfigManager *configManager_new__IO__OI_9(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 配置文件名 */ const char configFileName[], /* 系统信息的目录,为空表示不支持系统信息查询指令 */ const char sysinfoDir[]
            , /* 默认要监听ipv4地址端口号,在配置文件也定义此ip及端口时,使用配置文件中的定义 */ const unsigned int ipv4
            , /* 默认要监听ipv4地址端口号,为0表示没有默认监听,全使用配置文件,在配置文件也定义此ip及端口时,使用配置文件中的定义 */const unsigned short ipv4Port
            , /* 用户权限信息表,为空表示无用户功能,不为空时最后一个数组元素的权限为0 */struct ConfigManager_User pConfigManager_User[]
            , /* 监听分类服务器对象,可为空 */ void * const pBindClassifyServer
            , /* 配置管理权限掩码,为0表示所有用户都有此权限 */const unsigned long long configManageAuthorityMask
            , /* 授权文件名,为空表示无授权文件 */const char authorityFileName[], /* 回执数据掩码 */ const unsigned int return_mask
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pConfigManager指向的实例对象 */

    void configManager_delete__OI(ConfigManager * const pConfigManager
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */

    void configManager_startThread(ConfigManager * const pConfigManager
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);

    /* 结束线程,只发送信号,不回收线程资源 */

    void configManager_stopThread_noWait(ConfigManager * const pConfigManager);

    /* 结束线程 */

    void configManager_stopThread(ConfigManager * const pConfigManager);

    /* 注册初始化配置的函数 */

    void configManager_init_register(void* const pConfigManager, /* 相关信息,内部使用这些信息来排序 */const char info[],
            /* 初始化时调用的函数 */void (*createConfig)(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, /* 调用函数的指针 */ const void* const p),
            /* 调用函数的指针 */const void *const p);

    /* 注销初始化配置的函数 */

    void configManager_init_logoff(void* const pConfigManager, /* 相关信息,内部使用这些信息来排序 */const char info[],
            /* 初始化时调用的函数 */void (*createConfig)(FsConfig * const pConfig, /* 掩码 */const unsigned long long mask, /* 通道数 */const unsigned int channelCount, /* 调用函数的指针 */ const void* const p),
            /* 调用函数的指针 */const void *const p);

    /* 注册命令字,cmd+uuid+ipv4必须是唯一值 */

    void configManager_cmd_register(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid,可为空 */ const char uuid[]
            , /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4
            , /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* const extendKey
            , /* 本命令字是否可以设置uuid别名,0-不能设置,1-可设置 */ const unsigned char setAliasEnable
            , /* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */ int (*const cb) (
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
            , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* const p
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示此命令字不允许本地调用 */ void* const do_local, /* 调用函数的指针 */ void *p
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 设置已注册命令字的uuid别名 */

    void configManager_cmd_set_uuid_alias(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid,可为空 */ const char uuid[]
            , /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4
            , /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* const extendKey
            , /* uuid的别名,可为空,设置时,与本命令项匹配的指令应作用于uuidAlias的uuid */ const char uuidAlias[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 注销命令字,cmd+uuid+ipv4必须是唯一值 */

    void configManager_cmd_logoff(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid,可为空 */ const char uuid[], /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4
            , /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* extendKey
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 连接命令字,cmd+uuid+extendKey必须是唯一值 */

    void configManager_cmd_connect(void* const pConfigManager, /* 期望连接的命令字 */ const char cmd[], /* 期望连接的命令字对应的uuid,可为空 */ const char uuid[],
            /* 扩展关键字,主要用于解决cmd+uuid有相同值不能添加的问题,cmd+uuid相同是可以工作的 */const void* extendKey,
            /* 在期望连接的命令字和uuid改变时调用 */ void (*const cb) (/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
            /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p),
            /* 调用函数的指针 */ void *const p);

    /* 断开命令字,调用此函数不能再去操作连接的命令字指针,cmd+uuid+extendKey必须是唯一值 */

    void configManager_cmd_disconnect(void* const pConfigManager, /* 期望连接的命令字 */ const char cmd[], /* 期望连接的命令字对应的uuid,可为空 */ const char uuid[]
            , /* 扩展关键字,主要用于解决cmd+uuid有相同值不能添加的问题,cmd+uuid相同是可以工作的 */const void* extendKey);

    /* 发布协议 */

    void configManager_protocol_publish(void* const pConfigManager, /* 接口名,必须是唯一值 */ const char name[]
            , /* 协议的显示名,可为空 */ const char viewName[], /* 协议类型,必须为非0值,无具体含义,用于显示,值越大越靠前显示 */ const unsigned char classIndex
            , /* 获取此协议的方法 */ FsConfig*(*const get_protocol) ());

    /* 注册命令字并发布协议,cmd+uuid+ipv4必须是唯一值 */

    void configManager_cmd_register_and_protocol_publish(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid,可为空 */ const char uuid[]
            , /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4
            , /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* const extendKey
            , /* 本命令字是否可以设置uuid别名,0-不能设置,1-可设置 */ const unsigned char setAliasEnable
            , /* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */ int (*const cb) (
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 收到数据的前4字节 */unsigned int head
            , /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* const p
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示此命令字不允许本地调用 */ void* const do_local, /* 调用函数的指针 */ void *p
            ////////////////////////////////////////////////////////////////////
            , /* 接口名,必须是唯一值,为空表示不发布协议 */ const char name[]
            , /* 协议的显示名,可为空 */ const char viewName[], /* 协议类型,必须为非0值,无具体含义,用于显示,值越大越靠前显示 */ const unsigned char classIndex
            , /* 获取此协议的方法 */ FsConfig * (*const get_protocol) ()
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 取消已发布的协议 */

    void configManager_protocol_cancel(void* const pConfigManager, /* 接口名,必须是唯一值 */ const char name[]);

    /* 注销命令字并取消已发布的协议,cmd+uuid+ipv4必须是唯一值 */

    void configManager_cmd_logoff_and_protocol_cancel(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid,可为空 */ const char uuid[], /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4,
            /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* extendKey
            ////////////////////////////////////////////////////////////////////
            , /* 接口名,必须是唯一值,为空表示不取消已发布的协议 */ const char name[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 按标准方式发送字符串数据 */

    void configManager_send_pthreadSafety(void* const pConfigManager, /* 数据长度 */const unsigned int dataLen, /* 数据 */ const char data[],
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 发送数据的前4字节 */const unsigned int head);

    /* 发送数据,sendData中的数据必须为有效值 */

    void configManager_send_pthreadSafety__OI_2(void* const pConfigManager, /* 发送的数据,在尾部用3个int来储存与请求相关的信息(需要使用8字节对齐,使用Memery_Alignment宏),用于识别是发给哪个客户端的数据,数据长度不能为0 */FsObjectBase * const sendData);

    /* 发送数据,pEbml按标准方式转换为发送数据 */

    void configManager_send_pthreadSafety__OI_2_default(void* const pConfigManager, /* 数据 */FsEbml * const pEbml
            , /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[]
            , /* 收到数据的前4字节 */unsigned int head, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ const char requestDataType
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 发送数据,sendData中的数据必须为有效值,只有当在waiteTime时间内调用condition返回1后数据才会发送 */

    void configManager_send_pthreadSafety__OI_3(void* const pConfigManager, /* 超时时间,储存的是结束的cpu时间 */const double timeOut,
            /* 发送的数据,在尾部用3个int来储存与请求相关的信息(需要使用8字节对齐,使用Memery_Alignment宏),用于识别是发给哪个客户端的数据,数据长度不能为0 */FsObjectBase * const sendData,
            /* 检查数据是否可发送的函数指针,成功返回1,出错返回-1,0表示还需要等待 */signed char (*const condition) (void *p, /* 发送的数据 */const FsObjectBase * const sendData),
            /* 外部指针,可为空 */void *const p, /* 用完后删除外部指针的函数,可为空 */void (*const deleteP) (void *const p));

    /* 发送数据,sendData中的数据必须为有效值,数据不会立即发送,用key+name控制数据的发送和丢弃 */

    void configManager_send_pthreadSafety_OI_4(void* const pConfigManager, /* 关键字,可为空 */const void *key, /* 名字 */const char name[],
            /* 发送的数据,在尾部用3个int来储存与请求相关的信息(需要使用8字节对齐,使用Memery_Alignment宏),用于识别是发给哪个客户端的数据,数据长度不能为0 */FsObjectBase * const sendData);

    /* 确认发送和key和name匹配的所有数据 */

    void configManager_send_confirm_pthreadSafety(void* const pConfigManager, /* 关键字,可为空 */const void *key, /* 名字 */const char name[]);

    /* 取消发送和key和name匹配的所有数据 */

    void configManager_send_cancel_pthreadSafety(void* const pConfigManager, /* 关键字,可为空 */const void *key, /* 名字 */const char name[]);

    /* 释放原来保存的连接,必须是原来保存过的连接,否则会死机 */

    void configManager_connect_release(void* const pConfigManager, /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,应为回调函数传过来的标识 */const unsigned int requestID_3[],
            /* 是否断开连接,0-不断开,1-断开 */const unsigned char close);

    /* 获取连接的地址信息 */

    void configManager_connect_get_addr(/* 返回ip地址信息,为空不返回 */unsigned int *const rst_ipv4, /* 返回端口信息,为空不返回 */unsigned short *const rst_ipv4Port,
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,应为回调函数传过来的标识 */const unsigned int requestID_3[]);

    /* 检查保留连接的状态,返回1表示正常,-1表示连接断开,-2-表示发送数据过多 */

    int configManager_connect_check(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,应为回调函数传过来的标识 */const unsigned int requestID_3[]);

    /* 对保留连接发送数据,发送前应先调用configManager_connect_check检查连接状态 */

    void configManager_connect_send__OI_2__old(/* 发送的数据,在尾部用3个int来储存与请求相关的信息,用于识别是发给哪个客户端的数据,数据长度不能为0 */FsString * const sendData);

    /* 引用连接的发送数据,除可对已被引用的连接调用外,还可在命令字的回调函数中调用 */

    void configManager_connect_refer_send__OI_2(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
            /* 要发送的数据 */ FsObjectBase * const pObjectBase);

    /* 引用连接的发送数据,除可对已被引用的连接调用外,还可在命令字的回调函数中调用 */

    void configManager_connect_refer_send__OI_3(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
            /* 一次发送的pObjectBase个数 */ const unsigned int count, /* 要发送的数据 */ FsObjectBase * ppObjectBase[]);

    /* 引用连接的发送数据,除可对已被引用的连接调用外,还可在命令字的回调函数中调用 */

    void configManager_conncet_refer_send_buffer(/* 数据长度 */const unsigned int dataLen, /* 数据 */ const char data[]
            , /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[], /* 发送数据的前4字节 */const unsigned int head
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer);

    /* 获取pEbml转换的数据 */

    FsObjectBasePthreadSafety * configManager_conncet_refer_getData__IO(/* 数据区使用ebml,*pData_ebml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_ebml
            , /* 数据区使用xml,*pData_xml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_xml
            , /* 数据区使用json,*pData_json为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_json
            , /* 数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ const char requestDataType
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 引用连接的发送数据,除可对已被引用的连接调用外,还可在命令字的回调函数中调用 */

    void configManager_conncet_refer_sendData(/* 数据区使用ebml,为空时每次都从源数据生成,*pData_ebml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_ebml
            , /* 数据区使用xml,为空时每次都从源数据生成,*pData_xml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_xml
            , /* 数据区使用json,为空时每次都从源数据生成,*pData_json为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_json
            , /* 数据 */FsEbml * const pEbml, /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,需要采用sizeof(void*)对齐 */const unsigned int requestID_3[]
            , /* 收到数据的前4字节,内部不会修改0xFFFFFFF2标识的位,内部错误时会设置0x4位 */const unsigned int head, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ const char requestDataType
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer);

    /* 引用连接的发送数据,源数据可以按需动态生成,除可对已被引用的连接调用外,还可在命令字的回调函数中调用,发送数据返回1,否则返回-1 */

    /* 引用连接的发送数据,源数据可以按需动态生成,除可对已被引用的连接调用外,还可在命令字的回调函数中调用,发送数据返回1,否则返回-1 */
    int configManager_conncet_refer_sendData2(/* 数据区使用ebml,为空时每次都从源数据生成,*pData_ebml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_ebml
            , /* 数据区使用xml,为空时每次都从源数据生成,*pData_xml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_xml
            , /* 数据区使用json,为空时每次都从源数据生成,*pData_json为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_json
            , /* 生成源数据的方法,成功返回FsEbml指针,失败返回NULL */FsEbml * (*ebml_new__IO)(void *p), /* 与生成源数据相关的指针 */void *const p
            , /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,需要采用sizeof(void*)对齐 */const unsigned int requestID_3[]
            , /* 收到数据的前4字节 */const unsigned int head, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ const char requestDataType
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 向所有引用连接的发送数据 */

    void configManager_conncet_refer_send(void* const pConfigManager
            , /* 储存引用连接的链表,节点长度为不少于5int,前3int表示连接标识
            ,第4个int表示head,第5个int表示发送的数据类型(1-ebml数据,2-xml数据,3-json数据),节点大小必须为2个int大小的倍数 */ FsStructList * const pStructList_5int_min
            , /* 数据区使用ebml,为空时每次都从源数据生成,*pData_ebml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_ebml
            , /* 数据区使用xml,为空时每次都从源数据生成,*pData_xml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_xml
            , /* 数据区使用json,为空时每次都从源数据生成,*pData_json为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_json
            , /* 数据 */FsEbml * const pEbml, /* 发送的数据需要或的值,一般用于指定0x4,0x2的标识位 */const unsigned int headOr
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer);

    /* 向所有引用连接的发送数据 */

    void configManager_conncet_refer_send_objectList(void* const pConfigManager
            , /* 储存引用连接的链表,节点长度为不少于5int,前3int表示连接标识,第4个int表示head,第5个int表示发送的数据类型(1-ebml数据,2-xml数据,3-json数据) */ FsObjectList * const pObjectList
            , /* pObjectList中每个节点储存引用数据的偏移量,必须是2个int大小的倍数 */const unsigned int offset
            , /* 数据区使用ebml,为空时每次都从源数据生成,*pData_ebml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_ebml
            , /* 数据区使用xml,为空时每次都从源数据生成,*pData_xml为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_xml
            , /* 数据区使用json,为空时每次都从源数据生成,*pData_json为空时,可能会更新 */FsObjectBasePthreadSafety * * const pData_json
            , /* 数据 */FsEbml * const pEbml, /* 发送的数据需要或的值,一般用于指定0x4,0x2的标识位 */const unsigned int headOr
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer);

    /* 检查所有的引用连接是否有断开的连接 */

    void configManager_conncet_refer_check(void* const pConfigManager, /* 储存引用连接的链表,节点长度为不少于3int,前3int表示连接标识,节点大小必须为2个int大小的倍数 */ FsStructList * const pStructList_3int_min);

    /* 检查所有的引用连接是否有断开的连接,对需要释放引用的每个节点先执行item_do函数 */

    void configManager_conncet_refer_check_custom(void* const pConfigManager, /* 储存引用连接的链表,节点长度为不少于3int,前3int表示连接标识,节点大小必须为2个int大小的倍数 */ FsStructList * const pStructList_3int_min
            , /* 在关闭引用连接前调用此函数 */void (*const item_do) (void * pItem, void *item_do_externP), /* 传给item_do的外部参数 */ void *const item_do_externP);

    /* 关闭所有的引用连接 */

    void configManager_conncet_refer_close(void* const pConfigManager, /* 储存引用连接的链表,节点长度为不少于3int,前3int表示连接标识,节点大小必须为2个int大小的倍数 */ FsStructList * const pStructList_3int_min
            , /* 是否断开连接,0-不断开,1-断开 */const unsigned char close);

    /* 关闭所有的引用连接,对每个节点先执行item_do函数 */

    void configManager_conncet_refer_close_custom(void* const pConfigManager, /* 储存引用连接的链表,节点长度为不少于3int,前3int表示连接标识,节点大小必须为2个int大小的倍数 */ FsStructList * const pStructList_3int_min
            , /* 是否断开连接,0-不断开,1-断开 */const unsigned char close
            , /* 在关闭引用连接前调用此函数 */void (*const item_do) (void * pItem, void *item_do_externP), /* 传给item_do的外部参数 */ void *const item_do_externP);

    /* 关闭所有的引用连接 */

    void configManager_conncet_refer_close_with_data(void* const pConfigManager
            , /* 储存引用连接的链表,节点长度为不少于6int,前3int表示连接标识,第5和第6个int存放FsString指针,节点大小必须为2个int大小的倍数 */ FsStructList * const pStructList_6int_min
            , /* 是否断开连接,0-不断开,1-断开 */const unsigned char close);

    //    /* 获取配置,为空表示失败 */
    //    FsConfig *configManager_get_config__IO(void* pConfigManager, /* 要获取的指定数据区节点,多节点用空格分开,可以是多个空格,但不能以空格开头,为空表示获取全部 */const char pos[]);
    /* 注册掩码,实现多对多的逻辑判断通信,返回掩码的编号 */

#ifdef FsDebug   
    unsigned int configManager_mask_register_pthreadSafety(/* 返回掩码的公共对象,外部只是引用此对象,不能管理此对象的内存 */ void **pRst, void* pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[],
            /* 引用的行数 */int line);
#else
#define configManager_mask_register_pthreadSafety(rst,pConfigManager,maskName,uuid,line) configManager_mask_register_(rst,pConfigManager,maskName,uuid)
    unsigned int configManager_mask_register_(/* 返回掩码的公共对象,外部只是引用此对象,不能管理此对象的内存 */ void **pRst, void* pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[]);
#endif


    /* 注销掩码,maskName+uuid+maskIndex必须是唯一值 */

    void configManager_mask_logoff_pthreadSafety(void* pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 掩码号,注册时的返回值 */ const unsigned int maskIndex);

    /* 连接掩码,在掩码变动时收取数据信息 */

    void configManager_mask_connect_pthreadSafety(void* const pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[]
            , /* 扩展关键字,唯一标识符,断开时要用 */const void* const extendKey
            , /* 在掩码发生改变时会回调此函数 */ void (*const cb) (/* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 状态,0-全0,1-部分为1 */ unsigned char status
            , /* 是否是在connect中回调,connect连接时会把当前的掩码状态回调出来,0-不是,1-是 */ unsigned char isConnectCb, /* 调用函数的指针 */ void * p)
            , /* 调用函数的指针 */ void *const p);

    /* 断开掩码,maskName+uuid+extendKey必须是唯一值 */

    void configManager_mask_disconnect_pthreadSafety(void* const pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 扩展关键字,唯一标识符,断开时要用 */const void* extendKey);

    /* 掩码设置,非线程安全,一般在configManager_mask线程安全的回调函数中使用 */

    void configManager_mask_value_set(/* 注册时返回的掩码指针 */void* const pMask, /* 掩码编号 */const unsigned int maskIndex, /* 值,0-置0,1-置1 */ unsigned int value);

    /* 掩码设置 */

    void configManager_mask_value_set_pthreadSafety(void* const pConfigManager, /* 注册时返回的掩码指针 */void* const pMask, /* 掩码编号 */const unsigned int maskIndex, /* 值,0-置0,1-置1 */ unsigned int value);

    /* 掩码输出 */

    void configManager_mask_out_pthreadSafety(void* const pConfigManager, /* 输出的文件句柄 */FILE * const fd);

    /* 获取集群中通往其他节点的连接,成功返回1,把连接信息写入rst_requestID_3中,不用后需要释放空间,失败返回-1 */

    signed char configManager_cluster_connect_get__IO_1(/* 连接信息,用3个int来储存 */ unsigned int rst_requestID_3[],
            void* const pConfigManager, /* 期望连接的其他主机的本地ip */const unsigned int ipv4);

    /* 获取集群中通往其他节点的连接 */

    void configManager_cluster_connect(void* const pConfigManager, /* 期望连接的其他主机的本地ip,0表示匹配所有集群连接 */const unsigned int ipv4, /* 关键字 */const void* const key
            , /* 在连接创建成功时调用,成功返回1,返回-128增加对连接的引用 */ int (* const cb) (/* 期望连接的其他主机的本地ip */ unsigned int ipv4
            , /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[], /* 是否是连接断开,0-表示新建立连接,1-表示连接断开了 */unsigned char close, /* 调用函数的指针 */ void * p
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 调用函数的指针 */ void *const p);

    /* 获取集群中通往其他节点的连接 */

    void configManager_cluster_disconnect(void* const pConfigManager, /* 期望连接的其他主机的本地ip,0表示匹配所有集群连接 */const unsigned int ipv4, /* 关键字 */const void* const key);

    /* 注册收到回执信息的回调函数 */

    void configManager_return_register(void* const pConfigManager, /* 协议类型 */const ConfigManager_Cluster_protocol type, /* 关键字 */const void* const key
            , /* 在回执匹配掩码时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */ int (*cb)(
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[]
            , /* 收到的数据 */FsObjectBasePthreadSafety * pObjectBasePthreadSafety, /* 调用函数的指针 */ void* p
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 调用函数的指针 */ void *const p);

    /* 注销收到回执信息的回调函数 */

    void configManager_return_logoff(void* const pConfigManager, /* 协议类型的掩码值 */const ConfigManager_Cluster_protocol mask, /* 关键字 */const void* const key);

    /* 注册连接错误信息的回调函数,使用cb+p作为关键字 */

    void configManager_connect_error_register(void* const pConfigManager
            , /* 在有连接断开时调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */ int (*cb)(
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[],
            /* 调用函数的指针 */ void* p, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer), /* 调用函数的指针 */ void *const p);

    /* 注销连接错误信息的回调函数,使用cb+p作为关键字 */

    void configManager_connect_error_logoff(void* const pConfigManager
            , /* 在回执匹配掩码时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */ int (*cb)(
            /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */ const unsigned int requestID_3[],
            /* 调用函数的指针 */ void* p, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer), /* 调用函数的指针 */ void *const p);

    /* 添加pConfigManager中数据区的引用,应在pConfigManager->p.__pConfig_tmp为空时增加引用 */

#ifdef FsDebug
    void configManager_config_addRefer_pthreadSafety(ConfigManager * const pConfigManager, /* 引用的行数 */const int line);
#else
#define configManager_config_addRefer_pthreadSafety(pConfigManager,line) configManager_config_addRefer_pthreadSafety_(pConfigManager)
    void configManager_config_addRefer_pthreadSafety_(ConfigManager * const pConfigManager);
#endif

    /* 删除pConfigManager中数据区的引用,在pConfigManager->p.__pConfig_tmp不为空时应及时删除引用 */

    void configManager_config_deleteRefer_pthreadSafety(ConfigManager * const pConfigManager);

    /* 添加配置变化时的通知管道 */

#ifdef FsDebug
    void configManager_config_change_add_pipeNotify_pthreadSafety(ConfigManager * const pConfigManager, const int pipe, /* 添加的行数 */const int line);
#else
#define configManager_config_change_add_pipeNotify_pthreadSafety(pConfigManager,line) configManager_config_change_add_pipeNotify_pthreadSafety_(pConfigManager)
    void configManager_config_change_add_pipeNotify_pthreadSafety_(ConfigManager * const pConfigManager);
#endif

    /* 删除配置变化时的通知管道 */

    void configManager_config_change_delete_pipeNotify_pthreadSafety(ConfigManager * const pConfigManager, const int pipe);

    /* 获取配置信息,校验和发生改变时返回FsObjectBasePthreadSafety指针,否则返回NULL,返回NULL可能是出错 */

    FsObjectBasePthreadSafety *configManager_config_get_with_sum_pthreadSafety__IO(
            /* 调用都缓存的配置校验和,为空强制返回配置,不为空时会把最新的校验信息写入返回,在校验和发生改变时才返回配置数据 */unsigned int checkSum_2[]
            , ConfigManager * const pConfigManager, /* 要获取的数据类型 */const FsConfig_ExportType type
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 设置配置信息;
     * 成功返回1;
     * 失败返回-1;
     * 繁忙返回-120;
     */

    int configManager_config_set_pthreadSafety__OI(FsEbml * const pEbml, /* 设置类型,0-上传配置,1-合并配置 */const unsigned char type, ConfigManager * const pConfigManager
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 注册数据分发,返回分发操作对象指针,transferName+uuid+extendKey必须是唯一值 */

    void * configManager_transfer_register(void* const pConfigManager, /* 分发名 */ const char transferName[], /* uuid,可为空 */ const char uuid[]
            , /* 扩展关键字,主要用于解决transferName+uuid有相同值不能添加的问题,但transferName+uuid相同是不能正常工作的 */const void* const extendKey);

    /* 注销数据分发 */

    void configManager_transfer_logoff(void* const pConfigManager, /* configManager_transfer_out_register函数的返回值 */void *const pTransfer,
            /* 扩展关键字,主要用于解决transferName+uuid有相同值不能添加的问题,但transferName+uuid相同是不能正常工作的 */const void* const extendKey);

    /* 向pTransfer传递数据的方法,成功返回分发的数量,失败返回-1 */

    int configManager_transfer_out(/* configManager_transfer_out_register函数的返回值 */void *const pTransfer, /* 一次性传递的数据量 */const unsigned int dataCount, /* 传送的数据 */void * ppData[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 连接数据分发,transferName+uuid+extendKey必须是唯一值 */

    void configManager_transfer_connect(void* const pConfigManager, /* 分发名 */ const char transferName[], /* 期望连接的命令字对应的uuid,可为空 */ const char uuid[]
            , /* 扩展关键字,transferName+uuid有相同值不能添加的问题,transferName+uuid相同是可以工作的 */const void* const extendKey
            , /* 在调用configManager_transfer_out函数时使用哪个函数回调数据,成功返回1,失败返回-1 */
            int (* const cb) (/* 调用函数的指针 */ void *const p, /* 一次性传递的数据量 */const unsigned int dataCount, /* 传送的数据 */void * ppData[]
            , /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer)
            , /* 调用函数的指针 */ void *const p);

    /* 断开数据分发,transferName+uuid+extendKey必须是唯一值 */

    void configManager_transfer_disconnect(void* const pConfigManager, /* 分发名 */ const char transferName[], /* 期望连接的命令字对应的uuid,可为空 */ const char uuid[],
            /* 扩展关键字,主要用于解决transferName+uuid有相同值不能添加的问题,transferName+uuid相同是可以工作的 */const void* const extendKey);

    ////////////////////////////////////////////////////////////////////////////
    /* 
     * 储存连接信息,储存的连接可能是无效连接,主要用于调试的泛性数据获取,比如获取a线程中的一些信息,可把连接存于a线程可访问的一变量成员中,在a线程中从此变量成员获得连接后响应回执
     * 使用本结构不对连接加引用,以避免处理连接断开的问题
     */
    struct ConfigManager_connectNode_useOnce {
        /* 下一个连接,为空表示没有下一个连接 */
        struct ConfigManager_connectNode_useOnce *next;
        /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */
        unsigned int requestID[3];
        /* 收到数据的前4字节 */
        unsigned int head;
        /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */
        char return_type;
        /* 命令信息,用于区分命令类型,在具体业务中定义 */
        unsigned char cmdType;
        /* 获取数据的掩码,在具体业务中定义 */
        unsigned int mask;
    };

    /* 
     * 创建一个新的ConfigManager_connectNode_useOnce对象;
     * 返回ConfigManager_connectNode_useOnce指针.
     */

    struct ConfigManager_connectNode_useOnce * configManager_connectNode_useOnce_new__IO(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[]
            , /* 收到数据的前4字节 */unsigned int head, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */const char return_type
            , /* 命令信息,用于区分命令类型,在具体业务中定义 */ const unsigned char cmdType, /* 获取数据的掩码,具体定义在具体业务中定义 */ const unsigned int mask);

    /* 删除struct ConfigManager_connectNode_useOnce对象及next指向的下一个对象 */

    void configManager_connectNode_useOnce_list_delete(/* 不能为空 */struct ConfigManager_connectNode_useOnce *pHead);

#endif

#ifdef CONFIGMANAGER_enable
#ifdef FsDebug
    void configManager_test(void *pMonitor);
#endif
#endif
#ifdef __cplusplus
}
#endif

#endif /* CONFIGMANAGER_H */

