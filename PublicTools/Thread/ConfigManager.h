/* 
 * File:   ConfigManager.h
 * Author: fslib
 *
 * Created on 2013年6月15日, 上午11:19
 */

#ifndef CONFIGMANAGER_H
#define	CONFIGMANAGER_H
#include "../publicDefine.h"
#include "../Fs/Config.h"
#ifdef FsDebug
#include "../Fs/StructList.h"
#endif
#ifdef	__cplusplus
extern "C" {
#endif

    /* 
     * 配置管理
     * 基本模式是收到客户端请求后只发送不超过一条消息通知客户端;
     */
    typedef struct {

        /* 
         * 结构约定(所有指针的对齐方式与malloc相同):
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定);
         *     p中为私有变量,外部不可读也不可写;
         * 变量命名约定:
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀. 
         */
        struct {
            /* 线程名 */
            char *_threadName;
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */
            signed char *_externRunStatus;
            /* ThreadSocketIPv4线程的tid号 */
            pthread_t stid;
            /* 线程同步锁,仅用于读写配置 */
            pthread_mutex_t __mutex;
            /* 配置文件名 */
            char *_configFileName;
            /* 监听有ipv4地址,为0表示监听所有地址 */
            unsigned int _ipv4;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* 创建默认配置时要调用的函数链表 */
            FsStructList *__crateConfigList;
            /* 标记是否需要重新创建配置,0-不需要,1-需要 */
            unsigned char createConfigStatus : 1;
            /* 注册的命令链表 */
            FsObjectList *__cmdList;
            /* 期望连接的命令字链表,使用__cmdList中互斥锁 */
            FsObjectList *__cmdConnectList;
            /* 发送的数据链表 */
            FsObjectList *__sendList;
            /* 有条件的发送的数据链表 */
            FsObjectList *__conditionSendList;
            /* 等待发送数据链表,需要进一步确认是否发送 */
            FsObjectList *__waitSendList;
            /* 掩码链表 */
            FsObjectList *__maskList;
            /* 签名信息 */
            FsObjectList *__signatureList;
            /* 配置文件对象,外部在读取配置时应先锁住,把configUseCount+1后再读取 */
            FsConfig *__pConfig;
            /* 模块掩码 */
            unsigned long long _mask;
            /* 通道数限制 */
            unsigned int _channelCount;
            /* 配置校验和,前4个int是配置的绝对校验和,最后一个int是定时器切换的校验和 */
            unsigned int configCheckSum[5];
            /* 配置正在被使用的数量,只有为0时才会把临时配置文件对象拷贝到配置文件中 */
            unsigned int configUseCount;
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
            unsigned char nice;
        } rw;

        struct {
            /* 临时配置文件对象 */
            FsConfig *__pConfig_tmp;
        } p;
    } ConfigManager;
    /* 
     * 创建一个新的ConfigManager对象;
     * 返回ConfigManager指针.
     */
    ConfigManager *configManager_new__IO(/* 线程名 */const char threadName[],
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *pMonitor,
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid,
            /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *externRunStatus,
            /* 配置文件名 */ const char configFileName[], /* 监听有ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port,
            /* 授权文件名,为空表示无授权文件 */const char authorityFileName[]);
    /* 删除pConfigManager指向的实例对象 */
    void configManager_delete__OI(ConfigManager * const pConfigManager);

    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void configManager_startThread(ConfigManager * const pConfigManager,
            /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char nice);
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
    void configManager_cmd_register(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid */ const char uuid[], /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4,
            /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* extendKey,
            /* 在有用户请求此命令字时的调用函数,成功返回1,失败返回-1,需要引用此连接返回-128,为空表示此命令字不允许远程调用 */ signed char (*cb)(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存 */const unsigned int requestID_3[],
            /* 收到数据的前4字节 */unsigned int head, /* 收到的数据 */FsEbml * const pEbml, /* 客户端发送请求的数据类型,1-ebml数据,2-xml数据,3-json数据 */ char requestDataType, /* 调用函数的指针 */ void* const p),
            /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示此命令字不允许本地调用 */ void* const do_local,
            /* 调用函数的指针 */ void *p);
    /* 注销命令字,cmd+uuid_ipv4必须是唯一值 */
    void configManager_cmd_logoff(void* const pConfigManager, /* 命令字 */ const char cmd[], /* uuid */ const char uuid[], /* 主机ip,为0表示本地主机,非0表示远程主机 */const unsigned int ipv4,
            /* 扩展关键字,主要用于解决cmd+uuid+ipv4有相同值不能添加的问题,但cmd+uuid+ipv4相同是不能正常工作的 */const void* extendKey);
    /* 连接命令字,cmd+uuid+extendKey必须是唯一值 */
    void configManager_cmd_connect(void* const pConfigManager, /* 期望连接的命令字 */ const char cmd[], /* 期望连接的命令字对应的uuid */ const char uuid[],
            /* 扩展关键字,主要用于解决cmd+uuid有相同值不能添加的问题,cmd+uuid相同是可以工作的 */const void* extendKey,
            /* 在期望连接的命令字和uuid改变时调用 */ void (*cb)(/* 命令字 */ const char cmd[], /* uuid */ const char uuid[],
            /* 本地调用的方法,此为函数指针,参数的具体形式请参见具体的命令,为空表示注销此命令字的调用 */ void* const do_local, /* 与do_local关联的对象的指针 */ void *const do_local_p, /* 调用函数的指针 */ void *const p),
            /* 调用函数的指针 */ void *const p);
    /* 断开命令字,调用此函数不能再去操作连接的命令字指针,cmd+uuid+extendKey必须是唯一值 */
    void configManager_cmd_disconnect(void* const pConfigManager, /* 期望连接的命令字 */ const char cmd[], /* 期望连接的命令字对应的uuid */ const char uuid[],
            /* 扩展关键字,主要用于解决cmd+uuid有相同值不能添加的问题,cmd+uuid相同是可以工作的 */const void* extendKey);
    /* 发送数据,sendData中的数据必须为有效值 */
    void configManager_send_pthreadSafety__OI_2(void* const pConfigManager, /* 发送的数据,在尾部用3个int来储存与请求相关的信息,用于识别是发给哪个客户端的数据,数据长度不能为0 */FsString * const sendData);
    /* 发送数据,sendData中的数据必须为有效值,只有当在waiteTime时间内调用condition返回1后数据才会发送 */
    void configManager_send_pthreadSafety__OI_3(void*const pConfigManager, /* 超时时间,储存的是结束的cpu时间 */const double timeOut, /* 发送的数据,在尾部用3个int来储存与请求相关的信息,用于识别是发给哪个客户端的数据 */FsString * const sendData,
            /* 检查数据是否可发送的函数指针,成功返回1,出错返回-1,0表示还需要等待 */signed char (*const condition) (void *const p, /* 发送的数据 */const FsString * const sendData),
            /* 外部指针,可为空 */void *const p, /* 用完后删除外部指针的函数,可为空 */void (*const deleteP) (void *const p));
    /* 发送数据,sendData中的数据必须为有效值,数据不会立即发送,用key+name控制数据的发送和丢弃 */
    void configManager_send_pthreadSafety__OI_4(void* const pConfigManager, /* 关键字,可为空 */const void *key, /* 名字 */const char name[],
            /* 发送的数据,在尾部用3个int来储存与请求相关的信息,用于识别是发给哪个客户端的数据 */FsString * const sendData);
    /* 确认发送和key和name匹配的所有数据 */
    void configManager_send_confirm_pthreadSafety(void* const pConfigManager, /* 关键字,可为空 */const void *key, /* 名字 */const char name[]);
    /* 取消发送和key和name匹配的所有数据 */
    void configManager_send_cancel_pthreadSafety(void* const pConfigManager, /* 关键字,可为空 */const void *key, /* 名字 */const char name[]);
    /* 释放原来保存的连接,必须是原来保存过的连接,否则会死机 */
    void configManager_connect_release(void* const pConfigManager, /* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,应为回调函数传过来的标识 */const unsigned int requestID_3[],
            /* 是否断开连接,0-不断开,1-断开 */const unsigned char close);
    /* 检查保留连接的状态,返回1表示正常,-1表示连接断开,-2-表示发送数据过多 */
    signed char configManager_connect_check(/* 与请求相关的信息,用于识别是发给哪个客户端的数据,用3个int来储存,应为回调函数传过来的标识 */const unsigned int requestID_3[]);
    /* 对保留连接发送数据,发送前应先调用configManager_connect_check检查连接状态 */
    void configManager_connect_send__OI_2(/* 发送的数据,在尾部用3个int来储存与请求相关的信息,用于识别是发给哪个客户端的数据,数据长度不能为0 */FsString * const sendData);

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
    void configManager_mask_connect_pthreadSafety(void* pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[],
            /* 扩展关键字,唯一标识符,断开时要用 */const void* extendKey,
            /* 在掩码发生改变时会回调此函数 */ void (*cb)(/* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 状态,0-全0,1-部分为1 */ const unsigned char status, /* 调用函数的指针 */ void *const p),
            /* 调用函数的指针 */ void *const p);
    /* 断开掩码,maskName+uuid+extendKey必须是唯一值 */
    void configManager_mask_disconnect_pthreadSafety(void* const pConfigManager, /* 掩码名 */ const char maskName[], /* uuid */ const char uuid[], /* 扩展关键字,唯一标识符,断开时要用 */const void* extendKey);
    /* 掩码设置,非线程安全,一般在configManager_mask线程安全的回调函数中使用 */
    void configManager_mask_value_set(/* 注册时返回的掩码指针 */void* const pMask, /* 掩码编号 */const unsigned int maskIndex, /* 值,0-置0,1-置1 */ unsigned int value);
    /* 掩码设置 */
    void configManager_mask_value_set_pthreadSafety(void* const pConfigManager, /* 注册时返回的掩码指针 */void* const pMask, /* 掩码编号 */const unsigned int maskIndex, /* 值,0-置0,1-置1 */ unsigned int value);
    /* 掩码输出 */
    void configManager_mask_out_pthreadSafety(void* const pConfigManager, /* 输出的文件句柄 */FILE * const fd);
    /* 添加pConfigManager中数据区的引用 */
#ifdef FsDebug
    void configManager_config_addRefer_pthreadSafety(ConfigManager* pConfigManager, /* 引用的行数 */int line);
#else
#define configManager_config_addRefer_pthreadSafety(pConfigManager,line) configManager_config_addRefer_pthreadSafety_(pConfigManager)
    void configManager_config_addRefer_pthreadSafety_(ConfigManager* pConfigManager);
#endif
    /* 删除pConfigManager中数据区的引用 */
    void configManager_config_deleteRefer_pthreadSafety(ConfigManager* pConfigManager);
#ifdef FsDebug
    void configManager_test(void *pMonitor);
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIGMANAGER_H */

