/* 
 * File:   Config.h
 * Author: fslib
 *
 * Created on 2014年3月18日, 下午3:00
 */

#ifndef CONFIG_H
#define	CONFIG_H
#include "../publicDefine.h"
#include "Ebml.h"
#include "StringList.h"

#ifdef	__cplusplus
extern "C" {
#endif
    /* 节点中的默认值标识 */
#define FsConfig_nodeValue_default "$$defaultValue"
    /* 节点中的可选值标识 */
#define FsConfig_nodeValue_optional "$$optionalValue"
    /* 节点中的黑名单值标识 */
#define FsConfig_nodeValue_blacklist "$$blacklistValue"
    /* 条件等于,适用于数字和字符串 */
#define FsConfig_Condition_equal "=="
    /* 条件不等于,适用于数字和字符串 */
#define FsConfig_Condition_equal_false "!="
    /* 条件大于,适用于数字 */
#define FsConfig_Condition_greater ">"
    /* 条件大于等于,适用于数字 */
#define FsConfig_Condition_greaterOrEqual ">="
    /* 条件小于,适用于数字 */
#define FsConfig_Condition_less "<"
    /* 条件小于等于,适用于数字 */
#define FsConfig_Condition_lessOrEqual "<="
    /* 条件按位与,适用于整数 */
#define FsConfig_Condition_and "&"
    /* 条件按位与,结果为0为真,适用于整数 */
#define FsConfig_Condition_and_false "!&"
    /* 条件多值或后再按位与,适用于整数 */
#define FsConfig_Condition_orAnd "|&"
    /* 条件多值或后再按位与,结果为0为真,适用于整数 */
#define FsConfig_Condition_orAnd_false "!|&"

    /* 程序配置信息结构,节点名不能以$开头 */
    typedef struct {
        /* 继承FsEbml的所有特性 */
        FsEbml ebml;
        /* 用到的定时器下一次改变的时间,开机时间,外部可改变此值 */
        double timerUptime;
    } FsConfig;
#define FsConfigResult_string FsArray
#define FsConfigResult_integer FsArray
#define FsConfigResult_float FsArray
    /*
     * 创建一个FsConfig实例;
     * 返回创建的实例指针;
     */
    FsConfig *fs_Config_new__IO();
    /*
     * 根据buffer创建一个FsConfig实例;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    FsConfig *fs_Config_new_from_string__IO(/* 数据长度 */unsigned long bufferLenth, /* 数据开始地址 */ char buffer[]);
    /*
     * 从pConfig创建一个FsConfig实例,可实现数据拷贝;
     * 返回创建的实例指针.
     */
    FsConfig *fs_Config_new_from_Config__IO(const FsConfig * const pConfig);
    /*
     * 从文件创建一个FsConfig实例;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    FsConfig *fs_Config_new_from_file__IO(/* 文件名 */const char fileName[]);
    /*
     * 从文件创建一个FsConfig实例,返回的数据可能不完整需要导入重构;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    FsConfig *fs_Config_new_from_file_direct__IO(/* 文件名 */const char fileName[]);
    /*
     * 去掉pConfig中的冗余信息,不能直接从此对象中读取节点的值;
     * 返回FsConfig对象.
     */
    FsConfig *fs_Config_simple__IO(const FsConfig * const pConfig);
    /*
     * 去掉pConfig中的所有自定义设置;
     * 返回FsConfig对象.
     */
    FsConfig *fs_Config_default__IO(const FsConfig * const pConfig);

    /* 删除pConfig指向的实例 */
    void fs_Config_delete__OI(FsConfig * const pConfig);
    /* 去掉pConfig中的冗余信息,不能直接从此对象中读取节点的值 */
    void fs_Config_simple(FsConfig * const pConfig);
    /*
     * 把pConfig转换成-个Buffer;
     * 返回FsString对象;
     */
    FsString *fs_Config_to_String__IO(const FsConfig * const pConfig, /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount,
            /* 实际数据结束位置距离返回结果的buffer尾的字节数 */const unsigned int suffixCount);
    /* 
     * 把配置保存到文件中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2;
     * pEbml转换失败返回-3.
     */
    signed char fs_Config_save_to_file(const FsConfig * const pConfig, /* 文件名 */const char fileName[]);
    /* 
     * 把配置保存到文件中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2;
     * pEbml转换失败返回-3.
     */
    signed char fs_Config_save_to_file_direct(const FsConfig * const pConfig, /* 文件名 */const char fileName[]);
    /* 添加一个模板型节点,返回添加的节点,此类型读到的值是存储了模板实例的FsObjectList链表 */
    void *fs_Config_node_template_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 根据节点自定义显示名,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char customViewName[],
            /* 根据节点自定义显示名的后缀,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char customViewNameSuffix[],
            /* 注释 */const char comment[], /* 分布式的主机节点索引,节点必须在内部,为相对于本节点的节点,为空表示不支持分布式 */ const char distributedHostName[],
            /* 时间控制节点索引,节点必须在内部,为相对于本节点的节点,为空表示所有时间有效,当模板的所实例在某时间点都无效时,最后一个强制有效,对指定了此值的获取值时对于同一分布式节点返回第一个合适的节点 */ const char timerControl[],
            /* 权限,为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write,
            /* 值的最多个数,这里指可实例的最大模板数 */const unsigned int valueCountMax);

    /* 添加一个节点型节点,返回添加的节点,此类型读到的值是 */
    void *fs_Config_node_node_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[],
            /* 权限,为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write);
    /* 获取节点型节点,无或失败返回NULL */
    void *fs_Config_node_get_first(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode,
            /* 查找的节点名,用于空格隔开 */const char nodeName[]);
    /* 添加一个字符串节点,返回添加的节点,此类型读到的值是char*型 */
    void *fs_Config_node_string_add(FsConfig *pConfig, /* 父节点,没有请把pConfig传入 */void *parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[],
            /* 权限,为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write,
            /* 最小长度 */const unsigned int min, /* 最大长度 */const unsigned int max, /* 值的最多个数 */const unsigned int valueCountMax);
    /* 为String节点增加默认值 */
    void fs_Config_node_string_add_value(FsConfig *pConfig, void *pNodeString, /* 要添加的值类型,请用宏定义 */const char valueType[],
            /* 值 */ const char value[], /* 简写 */const char shortcut[], /* 注释 */const char comment[]);
    /* 获取string的节点值,无或失败返回NULL */
    FsConfigResult_string *fs_Config_node_string_get__IO(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode,
            /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[]);
    /* 获取第一个string的节点值,无或失败返回NULL */
    char *fs_Config_node_string_get_first__IO(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[]);
    /* 获取第一个string的节点值,但数据是从headSkip后开始的,尾部也加上tailSkip的空间,无或失败返回NULL */
    char *fs_Config_node_string_get_first_skip__IO(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[],
            /* 头要跳过的数据 */const unsigned int headSkip, /* 尾部要跳过的数据 */const unsigned int tailSkip);
    /* 添加一个整数节点,返回添加的节点,此类型读到的值是long long型 */
    void *fs_Config_node_integer_add(FsConfig *pConfig, /* 父节点,没有请把pConfig传入 */void *parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[],
            /* 权限,为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write,
            /* 最小值 */const long long min, /* 最大值 */const long long max, /* 值的最多个数 */const unsigned int valueCountMax);
    /* 为int节点增加默认值 */
    void fs_Config_node_integer_add_value(FsConfig *pConfig, void *pNodeInt, /* 要添加的值类型,请用宏定义 */const char valueType[],
            /* 值 */ const long long value, /* 简写 */const char shortcut[], /* 注释 */const char comment[]);
    /* 获取int的节点值,无或失败返回NULL */
    FsConfigResult_integer *fs_Config_node_integer_get__IO(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode,
            /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[]);
    /* 获取第一个int的节点值,无或失败返回failedValue */
    long long fs_Config_node_integer_get_first(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[],
            /* 失败时返回的错误值 */long long failedValue);
    /* 获取int的节点值,把所有值按位或后返回 */
    unsigned long long fs_Config_node_integer_get_mask(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[]);
    /* 添加一个浮点型节点,返回添加的节点,此类型读到的值是double型 */
    void *fs_Config_node_float_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[],
            /* 权限,为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write,
            /* 最小值 */const double min, /* 最大值 */const double max, /* 值的最多个数 */const unsigned int valueCountMax);
    /* 为float节点增加默认值 */
    void fs_Config_node_float_add_value(FsConfig * const pConfig, void *const pNodeFloat, /* 要添加的值类型,请用宏定义 */const char valueType[],
            /* 值 */ const double value, /* 简写 */const char shortcut[], /* 注释 */const char comment[]);
    /* 获取int的节点值,无或失败返回NULL */
    FsConfigResult_float *fs_Config_node_float_get__IO(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode,
            /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[]);
    /* 获取第一个float的节点值,无或失败返回failedValue */
    double fs_Config_node_float_get_first(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[],
            /* 失败时返回的错误值 */double failedValue);
    /* 获取模板型节点,无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */
    FsObjectList *fs_Config_node_template__IO(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode,
            /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList,
            /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用于空格隔开 */const char nodeName[]);
    /* 添加条件组,组内部必须所有的条件都为真,该组的值才为真,组之间只要有一个为真结果就为真 */
    void *fs_Config_condition_group_add(FsConfig * const pConfig, /* 要操作的节点,节点只能是本模块添加的节点,不能为pConfig */const void *const targetNode);
    /* 添加静态条件,组内部必须所有的条件都为真,该组的值才为真,组之间只要有一个为真结果就为真 */
    void fs_Config_condition_add_static(FsConfig * const pConfig, /* 条件组,调用fs_Config_condition_group_add的返回值 */const void *const conditionGroup,
            /* 指定根位置,相对于当前节点(指Config中的有效节点)向上跳跃的层数 */const unsigned short rootBack, /* 相对于指定根节点查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[],
            /* 比较方式,请用宏定义 */ const char compare[], /* 值,统一用字符串表示,比较时会根据被比较的节点类型自动转换,只有是字符器节点时才能为空 */const char value[]);
    /* 检查节点在当前值的环境下是否有效,有效返回1,无效返回0 */
    unsigned char fs_Config_node_condition_check(const FsConfig * const pConfig, /* 要检查的节点 */const void *const node);
    /* 
     * 获取配置里面的所有分布式主机;
     * 成功返回主机链表,为字符串(127.0.0.1:16863的形式);
     * 无或失败返回NULL.
     */
    FsStringList *fs_Config_distributed_host_get__IO(const FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */const void *const parentNode);
    /* 用用户数据导入配置,不会失败 */
    void fs_Config_import(/* 默认配置 */FsConfig *pConfig, /* 导入的节点,没有请把pConfig传入 */void *pNode, /* 用户配置,不一定要包含完整数据,支持FsConfig和FsEbml */void *userConfig, /* 导入对应的用户节点 */void *userNode);
    /* 只导出自定义配置,并去除配置里的冗余信息 */
    FsEbml *fs_Config_export_simple__IO(const FsConfig * const pConfig, /* 导入的节点,没有请把pConfig传入 */const void *const pNode);
    /* 导出配置,会去除配置里的冗余信息 */
    FsEbml *fs_Config_export__IO(const FsConfig * const pConfig, /* 导入的节点,没有请把pConfig传入 */const void *const pNode);
    /* 导出配置,会去除配置里的冗余信息,加上适当的注释 */
    FsEbml *fs_Config_export_with_comment__IO(FsConfig *pConfig, /* 导入的节点,没有请把pConfig传入 */void *pNode);
    /* 把pConfigResult_string输出到fd中 */
    void fs_Config_result_string_out(const FsConfigResult_string* pConfigResult_string, FILE *fd, /* 分隔符 */const char separator[]);


#ifdef FsDebug
    void fs_Config_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

