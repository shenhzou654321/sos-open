/* 
 * File:   Config.h
 * Author: fslib
 *
 * Created on 2014年3月18日, 下午3:00
 */
#if !defined CONFIG_H && !defined JAVA_IN_C

#define CONFIG_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Ebml.h"
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/Router.h"
#include "../../PublicTools/Fs/StringList.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 配置导出类型 */
    typedef enum {
        /* 不能识别的数据类型 */
        FsConfig_ExportType_unkown = -1,
#define FsConfig_ExportType_unkown FsConfig_ExportType_unkown
        /* config,config输出 */
        FsConfig_ExportType_config = 0x0,
#define FsConfig_ExportType_config FsConfig_ExportType_config
#define FsConfig_ExportType_config_string_0 "config"
        /* config-simple,config精简模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_config_simple = 0x1,
#define FsConfig_ExportType_config_simple FsConfig_ExportType_config_simple
#define FsConfig_ExportType_config_simple_string_1 "config-simple"
        /* config-simple,config精简带校验模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_config_simple_with_sum = 0x2,
#define FsConfig_ExportType_config_simple_with_sum FsConfig_ExportType_config_simple_with_sum
#define FsConfig_ExportType_config_simple_with_sum_string_2 "config-simple-with-sum"
        /* ebml,ebml数据 */
        FsConfig_ExportType_ebml = 0x3,
#define FsConfig_ExportType_ebml FsConfig_ExportType_ebml
#define FsConfig_ExportType_ebml_string_3 "ebml"
        /* ebml-simple,ebml精简模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_ebml_simple = 0x4,
#define FsConfig_ExportType_ebml_simple FsConfig_ExportType_ebml_simple
#define FsConfig_ExportType_ebml_simple_string_4 "ebml-simple"
        /* ebml-simple,ebml精简带校验模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_ebml_simple_with_sum = 0x5,
#define FsConfig_ExportType_ebml_simple_with_sum FsConfig_ExportType_ebml_simple_with_sum
#define FsConfig_ExportType_ebml_simple_with_sum_string_5 "ebml-simple-whtih-sum"
        /* ebml-simple-data,ebml精简数据(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_ebml_simple_data = 0x6,
#define FsConfig_ExportType_ebml_simple_data FsConfig_ExportType_ebml_simple_data
#define FsConfig_ExportType_ebml_simple_data_string_6 "ebml-simple-data"
        /* ebml-simple-data,ebml精简带校验数据(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_ebml_simple_data_with_sum = 0x7,
#define FsConfig_ExportType_ebml_simple_data_with_sum FsConfig_ExportType_ebml_simple_data_with_sum
#define FsConfig_ExportType_ebml_simple_data_with_sum_string_7 "ebml-simple-data-whtih-sum"
        /* ebml-export-simple,ebml自定义部分导出(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_ebml_export_simple = 0x8,
#define FsConfig_ExportType_ebml_export_simple FsConfig_ExportType_ebml_export_simple
#define FsConfig_ExportType_ebml_export_simple_string_8 "ebml-export-simple"
        /* ebml-export-simple-with-comment,ebml自定义部分含注释导出(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_ebml_export_simple_with_comment = 0x9,
#define FsConfig_ExportType_ebml_export_simple_with_comment FsConfig_ExportType_ebml_export_simple_with_comment
#define FsConfig_ExportType_ebml_export_simple_with_comment_string_9 "ebml-export-simple-with-comment"
        /* ebml-export-with-comment,ebml含注释导出 */
        FsConfig_ExportType_ebml_export_with_comment = 0xa,
#define FsConfig_ExportType_ebml_export_with_comment FsConfig_ExportType_ebml_export_with_comment
#define FsConfig_ExportType_ebml_export_with_comment_string_a "ebml-export-with-comment"
        /* ebml-export,ebml导出 */
        FsConfig_ExportType_ebml_export = 0xb,
#define FsConfig_ExportType_ebml_export FsConfig_ExportType_ebml_export
#define FsConfig_ExportType_ebml_export_string_b "ebml-export"
        /* xml,xml完整输出 */
        FsConfig_ExportType_xml = 0xc,
#define FsConfig_ExportType_xml FsConfig_ExportType_xml
#define FsConfig_ExportType_xml_string_c "xml"
        /* xml-simple,xml精简模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_xml_simple = 0xd,
#define FsConfig_ExportType_xml_simple FsConfig_ExportType_xml_simple
#define FsConfig_ExportType_xml_simple_string_d "xml-simple"
        /* xml-simple,xml精简带校验模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_xml_simple_with_sum = 0xe,
#define FsConfig_ExportType_xml_simple_with_sum FsConfig_ExportType_xml_simple_with_sum
#define FsConfig_ExportType_xml_simple_with_sum_string_e "xml-simple-whtih-sum"
        /* xml-simple-data,xml精简数据(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_xml_simple_data = 0xf,
#define FsConfig_ExportType_xml_simple_data FsConfig_ExportType_xml_simple_data
#define FsConfig_ExportType_xml_simple_data_string_f "xml-simple-data"
        /* xml-simple-data,xml精简带校验数据(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_xml_simple_data_with_sum = 0x10,
#define FsConfig_ExportType_xml_simple_data_with_sum FsConfig_ExportType_xml_simple_data_with_sum
#define FsConfig_ExportType_xml_simple_data_with_sum_string_10 "xml-simple-data-whtih-sum"
        /* xml-export-simple,xml自定义部分导出(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_xml_export_simple = 0x11,
#define FsConfig_ExportType_xml_export_simple FsConfig_ExportType_xml_export_simple
#define FsConfig_ExportType_xml_export_simple_string_11 "xml-export-simple"
        /* xml-export-simple-with-comment,xml自定义部分含注释导出(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_xml_export_simple_with_comment = 0x12,
#define FsConfig_ExportType_xml_export_simple_with_comment FsConfig_ExportType_xml_export_simple_with_comment
#define FsConfig_ExportType_xml_export_simple_with_comment_string_12 "xml-export-simple-with-comment"
        /* xml-export-with-comment,xml含注释导出 */
        FsConfig_ExportType_xml_export_with_comment = 0x13,
#define FsConfig_ExportType_xml_export_with_comment FsConfig_ExportType_xml_export_with_comment
#define FsConfig_ExportType_xml_export_with_comment_string_13 "xml-export-with-comment"
        /* xml-export,xml导出 */
        FsConfig_ExportType_xml_export = 0x14,
#define FsConfig_ExportType_xml_export FsConfig_ExportType_xml_export
#define FsConfig_ExportType_xml_export_string_14 "xml-export"
        /* json,json完整输出 */
        FsConfig_ExportType_json = 0x15,
#define FsConfig_ExportType_json FsConfig_ExportType_json
#define FsConfig_ExportType_json_string_15 "json"
        /* json-simple,json精简模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_json_simple = 0x16,
#define FsConfig_ExportType_json_simple FsConfig_ExportType_json_simple
#define FsConfig_ExportType_json_simple_string_16 "json-simple"
        /* json-simple,json精简带校验模式(对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_json_simple_with_sum = 0x17,
#define FsConfig_ExportType_json_simple_with_sum FsConfig_ExportType_json_simple_with_sum
#define FsConfig_ExportType_json_simple_with_sum_string_17 "json-simple-whtih-sum"
        /* json-simple-data,json精简数据(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_json_simple_data = 0x18,
#define FsConfig_ExportType_json_simple_data FsConfig_ExportType_json_simple_data
#define FsConfig_ExportType_json_simple_data_string_18 "json-simple-data"
        /* json-simple-data,json精简带校验数据(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_json_simple_data_with_sum = 0x19,
#define FsConfig_ExportType_json_simple_data_with_sum FsConfig_ExportType_json_simple_data_with_sum
#define FsConfig_ExportType_json_simple_data_with_sum_string_19 "json-simple-data-whtih-sum"
        /* json-export-simple,json自定义部分导出(去除模板定义,并对模板的实例只保留有自定义值的节点) */
        FsConfig_ExportType_json_export_simple = 0x1a,
#define FsConfig_ExportType_json_export_simple FsConfig_ExportType_json_export_simple
#define FsConfig_ExportType_json_export_simple_string_1a "json-export-simple"
        /* json-export,json导出 */
        FsConfig_ExportType_json_export = 0x1b,
#define FsConfig_ExportType_json_export FsConfig_ExportType_json_export
#define FsConfig_ExportType_json_export_string_1b "json-export"
#define FsConfig_ExportType_max FsConfig_ExportType_json_export
    } FsConfig_ExportType;
    /* 节点中的默认值标识 */
#define FsConfig_nodeValue_default "$$defaultValue"
    /* 节点中的可选值标识 */
#define FsConfig_nodeValue_optional "$$optionalValue"
    /* 节点中的黑名单值标识 */
#define FsConfig_nodeValue_blacklist "$$blacklistValue"
    /* 节点默认显示方式,所有类型可用 */
#define FsConfig_nodeShowType_default NULL
    /* 节点16进制显示方式,int类型可用 */
#define FsConfig_nodeShowType_hex "hex"
    /* 条件等于,适用于数字和字符串及二进制(base64编码后判断) */
#define FsConfig_Condition_equal "=="
    /* 条件不等于,适用于数字和字符串及二进制(base64编码后判断) */
#define FsConfig_Condition_equal_false "!="
    /* 条件大于,适用于数字,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_greater ">"
    /* 条件大于等于,适用于数字,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_greaterOrEqual ">="
    /* 条件小于,适用于数字,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_less "<"
    /* 条件小于等于,适用于数字,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_lessOrEqual "<="
    /* 条件按位与,适用于整数,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_and "&"
    /* 条件按位与,结果为0为真,适用于整数,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_and_false "!&"
    /* 条件多值或后再按位与,适用于整数,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_orAnd "|&"
    /* 条件多值或后再按位与,结果为0为真,适用于整数,数字使用16进制表示时按无符号处理 */
#define FsConfig_Condition_orAnd_false "!|&"

    /* 
     * 程序配置信息结构,节点名不能以$开头,以两个$打头的字符串为属性,以一个$打头的字符串为模板;
     * 权限设计:
     *     每个字段均带一个权限位a0,如用户拥有的权限(a&a0)==a0,则表示用户有此字段的读写权限,对于无权限的字段不可写;
     *     每个字段可以带一个读权限位白r0,如用户拥有的权限(a&r0)==r0,则表示用户有此字段读的权限;
     * 用户设计:
     *     用户可在传递的格式化文件中增加ConnectAuth节点,未传递使用连接的历史用户名和权限;
     *     在ConnectAuth节点中增加ConnectUser和ConnectPassword字段传递用户名和密码;
     *     在ConnectAuth节点中可增加ConnectPasswordEncryptionMethod字段指定ConnectPassword的产生方法(对于加密需要的附加字段也存放于ConnectAuth节点中);
     */
    typedef struct {
        /* 继承FsEbml的所有特性 */
        FsEbml ebml;
        /* 用到的定时器下一次改变的时间,GMT时间,外部可改变此值,修改此值需要使用ebml中的锁,建议算法为:缓存上一次配置切换的时间,时间到了需要进行切换(连续判断两次),如系统时间比缓存时间小也应该切换 */
        double timerGMT;
        /* 路由对象,调用者应定时对其更新 */
        FsRouter *pRouter;
    } FsConfig;
#define FsConfigResult_string FsArray
#define FsConfigResult_integer FsArray // 数据指针取 (long long*) (((char*) pInteger) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8))
#define FsConfigResult_float FsArray // 数据指针取 (double*) (((char*) pFloat) + Memery_AlignmentI(sizeof (FsConfigResult_integer), 8))

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

    FsConfig * fs_Config_new_from_string__IO(/* 数据长度 */unsigned long bufferLenth, /* 数据开始地址 */const char buffer[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

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

    FsConfig * fs_Config_new_from_file__IO(/* 文件名 */const char fileName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 从文件创建一个FsConfig实例,返回的数据可能不完整需要导入重构;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */

    FsConfig * fs_Config_new_from_file_direct__IO(/* 文件名 */const char fileName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /*
     * 精简拷贝,带描述信息,pEbml_node_src0和pEbml_node_src可以不是dst成员.
     */

    void fs_Config_simple_copy(FsEbml * const dst, struct FsEbml_node * pEbml_node_dst
            , /* pEbml_node_src对应的描述节点 */const struct FsEbml_node * pEbml_node_src0, const struct FsEbml_node * pEbml_node_src
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 去掉pConfig中的冗余信息;
     * 返回FsConfig对象.
     */

    FsConfig *fs_Config_simple__IO(const FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */const void *const pNode0, /* 抽取的节点,全部抽取,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 去掉冗余信息,每个节点加上$$sum校验字段,带描述信息,pEbml_node_src0和pEbml_node_src可以不是dst成员.
     */

    void fs_Config_simple_copy_with_sum(FsEbml * const dst, struct FsEbml_node * pEbml_node_dst, const FsEbml * const src
            , /* pEbml_node_src对应的描述节点 */const struct FsEbml_node * pEbml_node_src0, const struct FsEbml_node * pEbml_node_src
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 去掉pConfig中的冗余信息,每个节点加上$$sum校验字段;
     * 返回FsConfig对象.
     */

    FsConfig *fs_Config_simple_with_sum__IO(const FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */const void *const pNode0, /* 抽取的节点,全部抽取,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 抽取自定义的值,pEbml_node_src0和pEbml_node_src可以不是dst成员.
     */

    void fs_Config_simple_data_copy(FsEbml * const dst, struct FsEbml_node * pEbml_node_dst
            , /* pEbml_node_src对应的描述节点 */const struct FsEbml_node * pEbml_node_src0, const struct FsEbml_node * pEbml_node_src
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 抽取pConfig中自定义的值;
     * 返回FsEbml对象.
     */

    FsEbml * fs_Config_simple_data__IO(const FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */const void *const pNode0, /* 抽取的节点,全部抽取,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 抽取自定义的值,每个节点加上$$sum校验字段,pEbml_node_src0和pEbml_node_src可以不是dst成员.
     */

    void fs_Config_simple_data_copy_with_sum(FsEbml * const dst, struct FsEbml_node * pEbml_node_dst, const FsEbml * const src
            , /* pEbml_node_src对应的描述节点 */const struct FsEbml_node * pEbml_node_src0, const struct FsEbml_node * pEbml_node_src
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 抽取pConfig中自定义的值,每个节点加上$$sum校验字段;
     * 返回FsEbml对象.
     */

    FsEbml * fs_Config_simple_data_with_sum__IO(const FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */const void *const pNode0, /* 抽取的节点,全部抽取,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 抽取pEbml中pNode0定义的值存于dst中的dstNode节点中 */

    void fs_Config_default_copy(FsEbml * const dst, /* 可为dst */struct FsEbml_node * const dstNode, const FsEbml * const pEbml, /* 描述节点,可把pConfig传入 */ void *const pNode0
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 抽取pEbml中定义信息;
     * 返回FsConfig对象.
     */

    FsConfig *fs_Config_default__IO(const FsEbml * const pEbml, /* 描述节点,可把pConfig传入 */ void *const pNode0
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pConfig指向的实例 */

    void fs_Config_delete__OI(FsConfig * const pConfig
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 去掉pConfig中的所有自定义设置,有删除返回1,没有返回0 */

    unsigned char fs_Config_default(FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */ void *const pNode0, /* 重置的节点,全部重置请把pConfig传入 */ void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 去掉pConfig中的冗余信息,不能直接从此对象中读取节点的值 */

    void fs_Config_simple(FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */ void * const pNode0, /* 去掉冗余信息的节点,全部去除请把pConfig传入 */ void * const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 把pConfig转换成-个Buffer;
     * 返回FsString对象;
     */

    FsString *fs_Config_to_String__IO(const FsConfig * const pConfig, /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount
            , /* 实际数据结束位置距离返回结果的buffer尾的字节数 */const unsigned int suffixCount);

    /*
     * 把pEbml转换成-个Buffer;
     * 返回FsObjectBase对象;
     */

    FsObjectBase * fs_Config_to_ObjectBase__IO(const FsEbml * const pEbml, /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的buffer尾的字节数 */const unsigned int suffixCount);

    /* 
     * 把配置保存到文件中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2;
     * pEbml转换失败返回-3.
     */

    __attribute__ ((__deprecated__)) int fs_Config_save_to_file(const FsConfig * const pConfig, /* 文件名 */const char fileName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 把配置保存到文件中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2;
     * pEbml转换失败返回-3.
     */

    int fs_Config_save_to_file_direct(const FsConfig * const pConfig, /* 文件名 */const char fileName[]);

    /* 获取pNode0节点类型,成功返回类型指针,失败返回NULL */

    struct FsEbml_node * fs_Config_get_node_type(const FsConfig * const pConfig, /* 描述节点,不能把pConfig传入 */const void * const pNode0);

    /* 获取pNode节点的描述节点,child0是pNode描述节点或者pNode描述节点的子描述节点(可以是非直接子节点) */

    struct FsEbml_node* fs_Config_node_get_pNode0_by_child0(FsEbml * const pEbml, const void *const pNode, const void *const child0);

    /* 获取pNode节点的描述节点 */

    const struct FsEbml_node* fs_Config_node_get_pNode0(const FsEbml * const pEbml, const struct FsEbml_node * const pEbml_node);

    /* 获取pNode节点类型,成功返回类型指针,失败返回NULL */

    struct FsEbml_node* fs_Config_node_get_type(const FsConfig * const pConfig, /* 描述节点,可把pConfig传入 */const void * pNode0, /* 查找的节点名,用空格隔开,除最后一级节点,不能为空 */const char nodeName[]);

    /* 设置pNode及父节点的校验和为无效值 */

    void fs_Config_node_set_sum_invalid(FsEbml * const pEbml, /* pNode对应的描述节点,NULL表示需要内部查找 */ struct FsEbml_node * pNode);

    /* 
     * 获取值的校验和,用于2进制数据,返回校验和长度;
     */

#define  fs_Config_node_value_sum_cal(/* 储存校验和的buffer,最少20个字节 *//*const char */ valueSum_20/*[]*/,/* const char */value/*[]*/,/* const unsigned int */valueLen) \
    sprintf(valueSum_20, "%u-%x",(unsigned int)(valueLen), fs_String_buffer_24bitSum((unsigned int)(valueLen), (const char*)(value)))

    /* 
     * 获取pNode0中校验和与valueSum相同的值;
     * 成功返回FsString,失败返回NULL;
     */

    const FsString * fs_Config_node_value_sum_get(/* 节点,definedValue为1时此处应为描述节点 */const struct FsEbml_node * const pNode0, /* 值的校验和 */ const char valueSum[], /* 0-表示在值中查找,1-表示在预设值中查找 */const unsigned char definedValue);

    /* 为节点添加图像属性,支持模板template节点和节点node类型节点 */

    void fs_Config_node_add_property_image(FsConfig * const pConfig, void *const pNode,
            /* 指定根位置,相对于当前节点(指Config中的有效节点)向上跳跃的层数 */const unsigned short rootBack,
            /* 图片的url节点,为相对于本节点向上跳跃rootBack的节点,为空表示不支持 */const char uuid[],
            /* rtsp流的url节点,为相对于本节点向上跳跃rootBack的节点,为空表示不支持 */const char rtsp[]);

    /* 为节点添加位置属性,支持模板template节点和节点node类型节点,返回可以增加属性条件的节点 */

    void* fs_Config_node_add_property_position(FsConfig * const pConfig, void *const pNode,
            /* 位置节点,节点必须在内部,为相对于本节点的节点,为空表示使用本节点 */ const char positionNode[],
            /* 中心点经纬度,节点必须在内部,为相对于本节点的节点 */const char center[],
            /* 外接矩形大小单位米,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char size[],
            /* 外接矩形的颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int rectColor,
            /* 方向,单位角度0-360,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char direction[],
            /* 区域,以中心点为0点的坐标点,单位米,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char area[],
            /* 区域的颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int areaColor);

    /* 为节点添加区域属性,支持模板template节点和节点node类型节点,返回可以增加属性条件的节点 */

    void * fs_Config_node_add_property_area(FsConfig * const pConfig, void *const pNode, /* 菜单名,可为空 */const char menuName[]
            , /* 位置节点1,节点必须在内部,为相对于本节点的节点,为空表示使用本节点 */ const char positionNode1[]
            , /* 位置节点2,节点必须在positionNode1内部,为相对于本节点的节点,为空表示没有positionNode2 */ const char positionNode2[]
            , /* 位置节点3,节点必须在positionNode1内部,为相对于本节点的节点,为空表示没有positionNode3 */ const char positionNode3[]
            , /* 位置节点4,节点必须在positionNode1内部,为相对于本节点的节点,为空表示没有positionNode4 */ const char positionNode4[]
            , /* 区域,以图像左上角为坐标原点,单位为像素点,节点必须在内部,为相对于本节点的节点 */const char area[]
            , /* 区域的颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int areaColor);

    /* 为节点添加图像截取属性,支持模板template节点和节点node类型节点,返回可以增加属性条件的节点 */

    void * fs_Config_node_add_property_split(FsConfig * const pConfig, void *const pNode
            , /* 位置节点,节点必须在内部,为相对于本节点的节点,为空表示使用本节点 */ const char positionNode[]
            , /* 外接矩形大小单位像素,节点必须在内部,为相对于本节点的节点 */const char size[]
            , /* 底图,节点必须在内部,为空表示没有底图 */const char bp[]
            , /* 背景色,值0x80000000表示随机值,在有背景图时,不使用背景色,节点必须在内部,为空表示没有背景色 */const char bg_color[]
            , /* 截取框的颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int splitColor
            , /* 图像来源节点,节点必须在内部,为相对于positionNode的节点,为空表示使用positionNode节点,可以是模板template节点和node类型节点 */ const char memberNode[]
            , /* 图像对称方式,0-不对称,1-左右对称,2-上下对称,节点必须在内部,为相对于memberNode节点的节点,为空表示不支持 */const char member_symmetry[]
            , /* 图像来源的中心点,节点必须在内部,为相对于memberNode节点的节点,为空表示不支持 */const char member_center[]
            , /* 图像来源的外接矩形大小单位像素,节点必须在内部,为相对于memberNode节点的节点,为空表示不支持 */const char member_size[]
            , /* 图像来源的外接矩形的颜色,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int member_sizeColor
            , /* 图像来源的方向,单位角度0-360,节点必须在内部,为相对于memberNode节点的节点,为空表示不支持 */const char member_direction[]
            , /* 图像来源的区域,以来源图像的中心为坐标原点,单位为像素点,节点必须在内部,为相对于memberNode节点的节点,为空表示不支持 */const char member_area[]
            , /* 图像来源的区域,三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int member_areaColor);


    /* 添加一个模板型节点,返回添加的节点,此类型读到的值是存储了模板实例的FsObjectList链表 */

    void *fs_Config_node_template_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode
            , /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 根据节点自定义显示名,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char customViewName[]
            , /* 根据节点自定义显示名的后缀,节点必须在内部,为相对于本节点的节点,为空表示不支持 */const char customViewNameSuffix[]
            , /* 注释 */const char comment[], /* 分布式的主机节点索引,节点必须在内部,为相对于本节点的节点,为空表示不支持分布式 */ const char distributedHostName[]
            , /* 分布式的主机节点的外部地址索引,仅在distributedHostName不为空时有效,节点必须在内部,为相对于本节点的节点,为空表示使用distributedHostName的值 */ const char distributedHostAddr[]
            , /* 时间控制节点索引,节点必须在内部,为相对于本节点的节点,为空表示所有时间有效,当模板的所实例在某时间点都无效时,最后一个强制有效,对指定了此值的获取值时对于同一分布式节点返回第一个合适的节点 */ const char timerControl[]
            , /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write
            , /* 值的最多个数,这里指可实例的最大模板数 */const unsigned int valueCountMax);

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */
    unsigned int fs_Config_node_template_get_comment(const FsConfig * const pConfig, /* 描述节点,不能把pConfig传入 */const struct FsEbml_node * const pNode0
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取模板型节点,无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */

    FsObjectList * fs_Config_node_template__IO(FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode
            , /* 在设置了时间控制参数时,0-表示仅获取第一项有效的(均无效返回第一项),1-表示仅获取第一项有效的(均无效返回空),2-表示仅获取所有有效的(均无效返回第一项),3-表示仅获取所有有效的(均无效返回空) */const unsigned char timerControlType
            , /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList
            , /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]);

    /* 获取模板型节点(使用同组第一个节点位置排序,有timerControl时可保证顺序),无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */

    FsObjectList * fs_Config_node_template_orderFirst__IO(FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode
            , /* 在设置了时间控制参数时,0-表示仅获取第一项有效的(均无效返回第一项),1-表示仅获取第一项有效的(均无效返回空),2-表示仅获取所有有效的(均无效返回第一项),3-表示仅获取所有有效的(均无效返回空) */const unsigned char timerControlType
            , /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList
            , /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]);

    /* 获取模板型节点,忽略头部节点,返回有限数量的节点,无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */

    FsObjectList * fs_Config_node_template_skip__IO(FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode
            , /* 在设置了时间控制参数时,0-表示仅获取第一项有效的(均无效返回第一项),1-表示仅获取第一项有效的(均无效返回空),2-表示仅获取所有有效的(均无效返回第一项),3-表示仅获取所有有效的(均无效返回空) */const unsigned char timerControlType
            , /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList
            , /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]
            , /* 忽略多少个节点不返回 */ unsigned int skip, /* 最多返回多少个结果 */unsigned int count);

    /* 获取模板型节点,忽略头部节点,返回有限数量的节点(使用同组第一个节点位置排序,有timerControl时可保证顺序),无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */

    FsObjectList * fs_Config_node_template_skip_orderFirst__IO(FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode
            , /* 在设置了时间控制参数时,0-表示仅获取第一项有效的(均无效返回第一项),1-表示仅获取第一项有效的(均无效返回空),2-表示仅获取所有有效的(均无效返回第一项),3-表示仅获取所有有效的(均无效返回空) */const unsigned char timerControlType
            , /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList
            , /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]
            , /* 忽略多少个节点不返回 */ unsigned int skip, /* 最多返回多少个结果 */unsigned int count);

    /* 获取模板型节点,只获取匹配的下标为skip的节点,无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */

    const void * fs_Config_node_template_get(FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode
            , /* 在设置了时间控制参数时,0-表示仅获取第一项有效的(均无效返回第一项),1-表示仅获取第一项有效的(均无效返回空),2-表示仅获取所有有效的(均无效返回第一项),3-表示仅获取所有有效的(均无效返回空) */const unsigned char timerControlType
            , /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList
            , /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]
            , /* 返回节点的下标,从0开始 */unsigned int skip);

    /* 获取模板型节点,只获取匹配的下标为skip的节点(使用同组第一个节点位置排序,有timerControl时可保证顺序),无或失败返回NULL,修改timerUptime时会用到ebml中的互斥锁 */

    const void * fs_Config_node_template_get_orderFirst(FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode
            , /* 在设置了时间控制参数时,0-表示仅获取第一项有效的(均无效返回第一项),1-表示仅获取第一项有效的(均无效返回空),2-表示仅获取所有有效的(均无效返回第一项),3-表示仅获取所有有效的(均无效返回空) */const unsigned char timerControlType
            , /* 本地ip地址,不为空时返回用此值过滤过的配置,为空表示返回所有主机的配置 */const FsArray * const ipList
            , /* 在ipList不为空时,0表示获取本地主机配置,为1表示获取远程主机配置 */const unsigned char getCluster, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]
            , /* 返回节点的下标,从0开始 */unsigned int skip);

    /* 添加一个节点型节点,返回添加的节点,此类型读到的值是 */

    void *fs_Config_node_node_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[],
            /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write);

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    unsigned int fs_Config_node_node_get_comment(const FsConfig * const pConfig, /* 描述节点,不能把pConfig传入 */const void *const pNode0
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取节点型节点,也可以用于获取字符串,整数,浮点,二进制节点,不能用于获取模板型节点,无或失败返回NULL */

    const void *fs_Config_node_get_first(const FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const void *const parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[]);

    /* 获取节点型节点,也可以用于获取字符串,整数,浮点,二进制节点,不能用于获取模板型节点,并更新上下文,无或失败返回NULL */

    const void *fs_Config_node_get_first_update_nodeParentList(/* 不为空时返回是否是无效节点,当*isDisabledNode为0时内部会查找,0-不是,1-是 */char *const isDisabledNode, const FsConfig * const pConfig
            , /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0, /* parentNode的上下文,不包含parentNode */FsObjectList * const nodeParentList_
            , /* 父节点,没有请把pConfig传入 */const struct FsEbml_node * parentNode, /* 查找的节点名,用于空格隔开 */const char nodeName[]
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取节点,最后一级节点不获取,无或失败返回NULL */

    const void *fs_Config_node_get_first_skipLast(const FsConfig * const pConfig, /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0
            , /* 父节点,没有请把pConfig传入 */const struct FsEbml_node * parentNode, /* 查找的节点名,用于空格隔开,除最后一级节点外不能有模板节点,pNodeName不为空,*pNodeName也不为空,成功时*pNodeName返回最后一节节点的指针 */const char * * const pNodeName);

    /* 获取节点并更新上下文,最后一级节点不获取,并更新上下文,无或失败返回NULL */

    const void *fs_Config_node_get_first_skipLast_update_nodeParentList(/* 不为空时返回是否是无效节点,当*isDisabledNode为0时内部会查找,0-不是,1-是 */char *const isDisabledNode, const FsConfig * const pConfig
            , /* parentNode对应的描述节点,pParentNode0不为空在查找后通过*pParentNode0返回,*pParentNode0为NULL表示需要内部查找 */const void ** pParentNode0, /* parentNode的上下文,不包含parentNode */FsObjectList * const nodeParentList_
            , /* 父节点,没有请把pConfig传入 */const struct FsEbml_node * parentNode, /* 查找的节点名,用于空格隔开,除最后一级节点外不能有模板节点,pNodeName不为空,*pNodeName也不为空,成功时*pNodeName返回最后一节节点的指针 */const char * * const pNodeName
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个字符串节点,返回添加的节点,此类型读到的值是char*型 */

    void *fs_Config_node_string_add(FsConfig *pConfig, /* 父节点,没有请把pConfig传入 */void *parentNode
            , /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[]
            , /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write
            , /* 最小长度 */const unsigned int min, /* 最大长度 */const unsigned int max, /* 值的最多个数 */const unsigned int valueCountMax);

    /* 为String节点增加默认值 */

    void fs_Config_node_string_add_value(FsConfig *pConfig, void *pNodeString, /* 要添加的值类型,请用宏定义 */const char valueType[]
            , /* 值 */ const char value[], /* 简写 */const char shortcut[], /* 注释 */const char comment[]);

    /* 设置string的节点值,成功返回1,失败返回-1 */

    int fs_Config_node_string_set(FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0, /* 设置值的节点 */ struct FsEbml_node * const pNode
            , /* 设置值的类型,0-更新,1-增加,-1-删除 */const signed char setType, /* 在setType为0表示修改第几个的值,在setType为-1时,如此值为-1表示用value查找删除节点 */ const int updateOrDeleteIndex
            , /* 设置的值 */ const char value[], /* 出错时的回调信息,为空不回调 */void (*const cb_error) (/* 错误信息 */const char errordata[], void *externP), /* 出错回调时的外部参数 */ void *const cb_errorP);

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    unsigned int fs_Config_node_string_get_comment(const FsConfig * const pConfig, /* 描述节点,不能把pConfig传入 */const struct FsEbml_node * const pNode0
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取string节点值的所有值,无或失败返回NULL,返回链表中储存的是FsString指针,FsString指针的空间属于pConfig */

    FsObjectList * fs_Config_node_string_get_list__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取string的节点值,无或失败返回NULL */

    FsConfigResult_string * fs_Config_node_string_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);



    const char *fs_Config_node_string_get_first(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取第一个string的节点值,无或失败返回NULL */

    const FsString *fs_Config_node_string_get_first_String(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取第一个string的节点值,无或失败返回NULL */

    char *fs_Config_node_string_get_first__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取第一个string的节点值,但数据是从headSkip后开始的,尾部也加上tailSkip的空间,无或失败返回NULL */

    char *fs_Config_node_string_get_first_skip__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type
            , /* 头要跳过的数据 */const unsigned int headSkip, /* 尾部要跳过的数据 */const unsigned int tailSkip);


    /* 获取区域的点集(原始值为图像的像素点) */


    /* 点集 */
    struct Fs_Points {
        /* 点的数量 */
        unsigned int count;
        /* 点,[0]表示x,[1]表示y */
        int (point[])[2];
    };
    /* data[i]指向Fs_Points结构 */

#define FsConfigResult_area FsArray
    FsConfigResult_area * fs_Config_node_string_area_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 用长度表示的点集 */


    /* 获取区域的点集(原始值为长度m) */
    struct Fs_fPoints {
        /* 点的数量 */
        unsigned int count;
        /* 点,[0]表示x,[1]表示y */
        double (point[])[2];
    };
    /* data[i]指向Fs_fPoints结构 */
#define FsConfigResult_farea FsArray
    FsConfigResult_farea * fs_Config_node_string_farea_lenth_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取区域的坐标点(原始值为长度m) */

    FsConfigResult_area * fs_Config_node_string_area_lenth_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type
            , /* 比例尺,一个像素点代表的长度,X方向 */ const double zoomX, /* 比例尺,一个像素点代表的长度,Y方向 */ const double zoomY
            , /* 区域参考原点的x坐标 */ int x0, /* 区域参考原点的y坐标 */int y0);

    /* 获取第一个区域的坐标点(原始值为长度m) */

    struct Fs_Points * fs_Config_node_string_area_lenth_get_first__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type
            , /* 比例尺,一个像素点代表的长度,X方向 */ const double zoomX, /* 比例尺,一个像素点代表的长度,Y方向 */ const double zoomY
            , /* 区域参考原点的x坐标 */ int x0, /* 区域参考原点的y坐标 */int y0);

    /* 填充区域到单通道的图像上 */

    void fs_Config_node_string_area_fill_ch1(/* 区域 */FsConfigResult_area * const pArea
            , /* 图像的数据指针 */ unsigned char buffer[], /* 宽度 */unsigned int width, /* 高度 */unsigned int height
            , /* 储存宽度 */ const unsigned int xStep, /* 颜色 */unsigned char color, /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加一个整数节点,返回添加的节点,此类型读到的值是long long型 */

    void *fs_Config_node_integer_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode
            , /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[], /* 显示方式,请用宏定义 */const char showType[]
            , /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write
            , /* 最小值 */const long long min, /* 最大值 */const long long max, /* 值的最多个数 */const unsigned int valueCountMax);

    /* 为int节点增加默认值 */

    void fs_Config_node_integer_add_value(FsConfig *pConfig, void *pNodeInt, /* 要添加的值类型,请用宏定义 */const char valueType[]
            , /* 值 */ const long long value, /* 简写 */const char shortcut[], /* 注释 */const char comment[]);

    /* 设置integer的节点值,成功返回1,失败返回-1 */

    int fs_Config_node_integer_set(FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */const struct FsEbml_node * pNode0, /* 设置值的节点 */ struct FsEbml_node * const pNode
            , /* 设置值的类型,0-更新,1-增加,-1-删除 */const signed char setType, /* 在setType为0表示修改第几个的值,在setType为-1时,如此值为-1表示用value查找删除节点 */ const int updateOrDeleteIndex
            , /* 设置的值 */ const long long value, /* 出错时的回调信息,为空不回调 */void (*const cb_error) (/* 错误信息 */const char errordata[], void *externP), /* 出错回调时的外部参数 */ void *const cb_errorP);

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    unsigned int fs_Config_node_integer_get_comment(const FsConfig * const pConfig, /* 节点,不能把pConfig传入 */const struct FsEbml_node * const pNode0
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);


    /* 获取int节点的所有值,无或失败返回NULL,返回链表中储存的是FsString指针,FsString指针的空间属于pConfig */

    FsObjectList * fs_Config_node_integer_get_list__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取int的节点值,无或失败返回NULL */

    FsConfigResult_integer * fs_Config_node_integer_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 不为空,表示指向与nodeName关联的类型节点 */struct FsEbml_node * const type);

    /* 获取第一个int的节点值,无或失败返回failedValue */

    long long fs_Config_node_integer_get_first(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,可以为空 */const char nodeName[], /* 失败时返回的错误值 */long long failedValue, /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取int的节点值,把所有值按位或后返回 */

    unsigned long long fs_Config_node_integer_get_mask(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 添加一个浮点型节点,返回添加的节点,此类型读到的值是double型 */

    void *fs_Config_node_float_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode,
            /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[],
            /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write,
            /* 最小值 */const double min, /* 最大值 */const double max, /* 值的最多个数 */const unsigned int valueCountMax);

    /* 为float节点增加默认值 */

    void fs_Config_node_float_add_value(FsConfig * const pConfig, void *const pNodeFloat, /* 要添加的值类型,请用宏定义 */const char valueType[]
            , /* 值 */ const double value, /* 简写 */const char shortcut[], /* 注释 */const char comment[]);

    /* 设置float的节点值,成功返回1,失败返回-1 */

    int fs_Config_node_float_set(FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0, /* 设置值的节点 */ struct FsEbml_node * const pNode
            , /* 设置值的类型,0-更新,1-增加,-1-删除 */const signed char setType, /* 在setType为0表示修改第几个的值,在setType为-1时,如此值为-1表示用value查找删除节点 */ const int updateOrDeleteIndex
            , /* 设置的值 */ const double value, /* 出错时的回调信息,为空不回调 */void (*const cb_error) (/* 错误信息 */const char errordata[], void *externP), /* 出错回调时的外部参数 */ void *const cb_errorP);

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    unsigned int fs_Config_node_float_get_comment(const FsConfig * const pConfig, /* 节点,不能把pConfig传入 */const struct FsEbml_node * const pNode0
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取float节点的所有值,无或失败返回NULL,返回链表中储存的是FsString指针,FsString指针的空间属于pConfig */

    FsObjectList * fs_Config_node_float_get_list__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 获取float的节点值,无或失败返回NULL */

    FsConfigResult_float * fs_Config_node_float_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 不为空,表示指向与nodeName关联的类型节点 */struct FsEbml_node * const type);

    /* 获取第一个float的节点值,无或失败返回failedValue */

    double fs_Config_node_float_get_first(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开 */const char nodeName[], /* 失败时返回的错误值 */double failedValue, /* 可以空,不为空时表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);

    /* 添加一个二进制节点(默认从文件加载),转xml或json时使用base64编码,返回添加的节点,此类型读到的值是FsString型 */

    void *fs_Config_node_binary_add(FsConfig * const pConfig, /* 父节点,没有请把pConfig传入 */void *const parentNode
            , /* 节点名,用于查找,不能重复 */const char nodeName[], /* 显示名 */const char viewName[], /* 注释 */const char comment[]
            , /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority, /* 写权限,0x1可读,0x2可选,0x4可写 */const unsigned int write
            , /* 最小长度 */const unsigned long long min, /* 最大长度 */const unsigned long long max, /* 值的最多个数 */const unsigned int valueCountMax);

    /* 设置binary的节点值,成功返回1,失败返回-1 */

    int fs_Config_node_binary_set(FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */const struct FsEbml_node * pNode0, /* 设置值的节点 */ struct FsEbml_node * const pNode
            , /* 设置值的类型,0-更新,1-增加,-1-删除 */const signed char setType, /* 在setType为0表示修改第几个的值,在setType为-1时,如此值为-1表示用value查找删除节点 */ const int updateOrDeleteIndex
            , /* 设置的值 */ const char value[], /* 值的长度 */const unsigned int valueLen, /* 出错时的回调信息,为空不回调 */void (*const cb_error) (/* 错误信息 */const char errordata[], void *externP), /* 出错回调时的外部参数 */ void *const cb_errorP);

    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    unsigned int fs_Config_node_binary_get_comment(const FsConfig * const pConfig, /* 节点,不能把pConfig传入 */const struct FsEbml_node * const pNode0
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取二进制节点的所有值,无或失败返回NULL,返回链表中储存的是FsString指针,FsString指针的空间属于pConfig */

    /* 获取二进制节点的所有值,无或失败返回NULL,返回链表中储存的是FsString指针,FsString指针的空间属于pConfig */
    FsObjectList * fs_Config_node_binary_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 不为空,表示指向与nodeName关联的类型节点 */struct FsEbml_node * type);

    /* 获取二进制节点的第一个值,无或失败返回NULL,返回FsString指针的空间属于pConfig */

    /* 获取二进制节点的第一个值,无或失败返回NULL,返回FsString指针的空间属于pConfig */
    const FsString * fs_Config_node_binary_get_first(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * parentNode0, /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[], /* 不为空,表示指向与nodeName关联的类型节点 */const struct FsEbml_node * type);


    /* 获取pNode0节点的注释,返回数据在共享buffer中的偏移量 */

    unsigned int fs_Config_node0_get_comment(const FsConfig * const pConfig, /* 节点,不能把pConfig传入 */const struct FsEbml_node * const pNode0, /* 不为空,表示type储存pNode的类型 */struct FsEbml_node *type
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加条件组,组内部必须所有的条件都为真,该组的值才为真,组之间只要有一个为真结果就为真 */

    void *fs_Config_condition_group_add(FsConfig * const pConfig, /* 要操作的节点,节点只能是本模块添加的节点,不能为pConfig */const void *const targetNode);

    /* 添加静态条件,组内部必须所有的条件都为真,该组的值才为真,组之间只要有一个为真结果就为真 */

    void fs_Config_condition_add_static(FsConfig * const pConfig, /* 条件组,调用fs_Config_condition_group_add的返回值 */const void *const conditionGroup,
            /* 指定根位置,相对于当前节点(指Config中的有效节点)向上跳跃的层数 */const unsigned short rootBack, /* 相对于指定根节点查找的节点名,用于空格隔开,为空表示无须查找 */const char nodeName[],
            /* 比较方式,请用宏定义 */ const char compare[], /* 值,统一用字符串表示,比较时会根据被比较的节点类型自动转换,只有是字符器节点时才能为空 */const char value[]);

    /* 获取pNode的上下文存于nodeParentList_中 */

    void fs_Config_condition_nodeParentList_get(FsObjectList * const nodeParentList_, /* 根节点,可为pEbml */ const struct FsEbml_node* templateNode, const struct FsEbml_node *pNode, /* templateNode到pNode的距离 */const unsigned int distance);

    /* 获取pNode的上下文存于nodeParentList_中,从pEbml开始 */

    void fs_Config_condition_nodeParentList_get_root(FsObjectList * const nodeParentList_, const FsEbml * const pEbml, /* 根节点,可为pEbml */ const struct FsEbml_node * const templateNode, const struct FsEbml_node * const pNode, /* templateNode到pNode的距离 */const unsigned int distance);

    /* 检查节点在当前值的环境下是否有效,也可用于属性条件判断,有效且与condition有关返回3,有效且与condition无关返回1,无效且与condition有关返回2,无效且与condition无关返回0 */

    unsigned char fs_Config_node_condition_check_with(const FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0
            , /* node的父节点,同一个node因上下文不同结果不同,不包含node */const struct FsEbml_node * const ppNode_nodeParentList[], /* nodeParentList_中有多少个节点属于pNode的上下文 */const unsigned int nodeParentList_nodeCount
            , /* 要检查的节点 */const struct FsEbml_node * const pNode, /* 条件在哪个节点下查找,一般是node0的描述节点,在属性判断时可能是其他 */const struct FsEbml_node * const conditionParent
            , const struct FsEbml_node * const condition
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 检查节点在当前值的环境下是否有效,也可用于属性条件判断,有效返回1,无效返回0 */

    unsigned char fs_Config_node_condition_check(const FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0
            , /* node的父节点,同一个node因上下文不同结果不同,不包含node */const struct FsEbml_node * const ppNode_nodeParentList[], /* nodeParentList_中有多少个节点属于pNode的上下文 */const unsigned int nodeParentList_nodeCount
            , /* 要检查的节点 */const struct FsEbml_node * const pNode, /* 条件在哪个节点下查找,一般和node0相同,在属性判断地一般不同 */const struct FsEbml_node * const conditionParent
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 检查节点在当前值的环境下是否有效,逐级判断,也可用于属性条件判断,有效返回1,无效返回0 */

    unsigned char fs_Config_node_condition_check_all(const FsConfig * const pConfig, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0
            , /* node的父节点,同一个node因上下文不同结果不同,不包含node */const struct FsEbml_node * const ppNode_nodeParentList[], /* nodeParentList_中有多少个节点属于pNode的上下文 */const unsigned int nodeParentList_nodeCount
            , /* 要检查的节点 */const struct FsEbml_node * const pNode, /* 条件在哪个节点下查找,一般和node0相同,在属性判断地一般不同 */const struct FsEbml_node * const conditionParent
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取配置里面的所有分布式主机;
     * 成功返回主机链表,为字符串(127.0.0.1:16863的形式);
     * 无或失败返回NULL.
     */

    FsStringList * fs_Config_distributed_host_get__IO(const FsConfig * const pConfig, /* parentNode对应的描述节点,NULL表示需要内部查找 */const void * const parentNode0
            , /* 父节点,没有请把pConfig传入 */const void *const parentNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 用用户数据导入配置,userConfig必须是已完整解析的对象,userConfig必须是已完整解析的对象,不会失败,使用fs_Config_import_onlyData和fs_Config_merge_onlyData操作过的节点不能使用此函数,0-表示没有导入节点,1-表示导入了节点 */

    unsigned int fs_Config_import(/* 默认配置 */FsEbml * const pEbml, /* 导入的节点,没有请把pConfig传入,使用fs_Config_import_onlyData和fs_Config_merge_onlyData操作过的节点不能使用此函数 */struct FsEbml_node * pNode
            , /* 用户配置,不一定要包含完整数据,支持FsConfig和FsEbml */ const FsEbml * const userConfig, /* 导入对应的用户节点 */const struct FsEbml_node * userNode, /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 用用户数据导入配置,userConfig必须是已完整解析的对象,userConfig必须是已完整解析的对象,userConfig必须是已完整解析的对象,不会失败,只包含值的节点,无描述信息,返回是否有导入节点,0-表示没有导入节点,1-表示导入了节点 */

    unsigned int fs_Config_import_onlyData(/* 默认配置 */FsEbml * const pEbml, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0, /* 导入的节点,没有请把pConfig传入 */struct FsEbml_node * pNode
            , /* 用户配置,不一定要包含完整数据,支持FsConfig和FsEbml */const FsEbml * const userConfig, /* 导入对应的用户节点 */const struct FsEbml_node * userNode, /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 用用户数据合并配置,userConfig必须是已完整解析的对象,不会失败,遇到有冲突的数据使用用户数据替换,使用fs_Config_import_onlyData和fs_Config_merge_onlyData操作过的节点不能使用此函数,0-表示没有导入节点,1-表示导入了节点 */

    unsigned int fs_Config_merge(/* 默认配置 */FsEbml * const pEbml, /* 合并的节点,没有请把pConfig传入,使用fs_Config_import_onlyData和fs_Config_merge_onlyData操作过的节点不能使用此函数 */struct FsEbml_node * pNode
            , /* 用户配置,不一定要包含完整数据,支持FsConfig和FsEbml */const FsEbml * const userConfig, /* 导入对应的用户节点 */const struct FsEbml_node * userNode, /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 用用户数据合并配置,userConfig必须是已完整解析的对象,不会失败,遇到有冲突的数据使用用户数据替换,只包含值的节点,无描述信息,返回是否有导入节点,0-表示没有导入节电,1-表示导入了节点 */

    unsigned int fs_Config_merge_onlyData(/* 默认配置 */FsEbml * const pEbml, /* pNode对应的描述节点,NULL表示需要内部查找 */ const struct FsEbml_node * pNode0, /* 导入的节点,没有请把pConfig传入 */struct FsEbml_node * pNode
            , /* 用户配置,不一定要包含完整数据,支持FsConfig和FsEbml */const FsEbml * const userConfig, /* 导入对应的用户节点 */const struct FsEbml_node * userNode, /* 写权限(有写权限肯定有读权限),为0表示所有用户都有权限 */ const unsigned long long authority
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取pEbml中pNode节点的校验和 */

    unsigned long long fs_Config_get_sum(FsEbml * const pEbml, /* 要计算校验和的根节点,没有请把pConfig传入 */struct FsEbml_node * const pNode);


    /* 获取pConfig中nodeName节点当前时间有效实例的校验和 */

    unsigned long long fs_Config_get_sum_timeControl(FsConfig * const pConfig, /* parentNode对应的描述节点,parentNode0为NULL表示需要内部查找 */const void * parentNode0
            , /* 父节点,没有请把pConfig传入 */const void * parentNode, /* 查找的节点名,用空格隔开,除最后一级节点,前面节点不能为模板节点 */const char nodeName[]);


    /* 只导出自定义配置,把pNode的数据导出放dstNode下 */

    void fs_Config_export_simple(FsEbml * const dst, struct FsEbml_node * const dstNode, const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode);

    /* 只导出自定义配置 */

    FsEbml * fs_Config_export_simple__IO(const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode);

    /* 只导出自定义配置,把pNode的数据导出放dstNode下,含注释 */

    void fs_Config_export_simple_with_comment(FsEbml * const dst, struct FsEbml_node * const dstNode, const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 只导出自定义配置,含注释 */

    FsEbml * fs_Config_export_simple_with_comment__IO(const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 导出配置,把pNode的数据导出放dstNode下,会去除配置里的冗余信息 */

    void fs_Config_export(FsEbml * const dst, struct FsEbml_node * const dstNode, const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode);

    /* 导出配置,会去除配置里的冗余信息 */

    FsEbml * fs_Config_export__IO(const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode);

    /* 导出配置,把pNode的数据导出放dstNode下,会去除配置里的冗余信息,加上适当的注释 */

    void fs_Config_export_with_comment(FsEbml * const dst, struct FsEbml_node * const dstNode, const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * const pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 导出配置,会去除配置里的冗余信息,加上适当的注释 */

    FsEbml * fs_Config_export_with_comment__IO(const FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void *const pNode0, /* 导出的节点,全部导出,请把pConfig传入 */const void *const pNode
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 把字符串type转FsConfig_ExportType类型 */

    FsConfig_ExportType fs_Config_ExportType_get(const char type[]);

    /* 把FsConfig_ExportType类型转字符串 */

    const char* fs_Config_ExportType_to_string(FsConfig_ExportType const type);

    /* 把配置导出为FsObjectBase输出 */

    FsObjectBase * fs_Config_export_objectBase__IO(const FsConfig * const pConfig, const FsConfig_ExportType type
            , /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount
            , /* 实际数据结束位置距离返回结果的buffer尾的字节数 */const unsigned int suffixCount
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 更新pConfig中的工具有效时间,有更新返回1,无更新返回0 */

    int fs_Config_update_validtime_tool(FsConfig * const pConfig);

    /* 把pConfigResult_string输出到fd中 */

    void fs_Config_result_string_out(const FsConfigResult_string* pConfigResult_string, FILE *fd, /* 分隔符 */const char separator[]);

    /* 获取pNode节点中存放split属性的父节点,成功返回节点指针,失败返回NULL */

    void *fs_Config_property_split_get_parent_parameter(FsConfig * const pConfig, void * const pNode);

    /* 获取split的背景图和背景图节点的指针,背景图获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_bp(const FsString * * const pBp, /* 储存大小节点指针的指针,可为空 */struct FsEbml_node * * const pSplit_bp
            , FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, struct FsEbml_node * const pNode
            , /* 参数在哪个节点下,如为空,函数内部重新查找 */struct FsEbml_node * parent_parameter, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0, struct FsEbml_node * pos_node);

    /* 获取split的背景色( 0x80000000表示随机值)和背景色节点的指针,背景色获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_bg_color(unsigned int *const pBg_color, /* 储存大小节点指针的指针,可为空 */struct FsEbml_node * * const pSplit_bg_color
            , FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, struct FsEbml_node * const pNode
            , /* 参数在哪个节点下,如为空,函数内部重新查找 */struct FsEbml_node * parent_parameter, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0, struct FsEbml_node * pos_node);

    /* 获取split的大小和大小节点的指针,大小获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_size(unsigned int *const pW, unsigned int *const pH, /* 储存大小节点指针的指针,可为空 */struct FsEbml_node * * const pSplit_size
            , FsConfig * const pConfig, /* pNode对应的描述节点,为NULL表示需要内部查找 */const void * pNode0, struct FsEbml_node * const pNode
            , /* 参数在哪个节点下,如为空,函数内部重新查找 */struct FsEbml_node * parent_parameter, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0, struct FsEbml_node * pos_node);

    /* 获取split的memberList,成功返回链表,失败返回NULL */

    FsObjectList * fs_Config_property_split_get_memberList__IO(FsConfig * const pConfig
            , struct FsEbml_node * const pNode, /* 参数在哪个节点下,如为空,函数内部重新查找 */struct FsEbml_node * parent_parameter, struct FsEbml_node * const pos_node);

    /* 获取member_symmetry和节点的指针,对称方式获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_member_symmetry(unsigned char *const pSymmetry, /* 储存member_symmetry节点指针的指针,可为空 */struct FsEbml_node * * const pMember_symmetry
            , FsConfig * const pConfig, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0
            , struct FsEbml_node * pos_node, /* 为空表示不用搜索,为pos_node */const char *const member_symmetry_nodename);

    /* 获取member_center和节点的指针,大小获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_member_center(int *const pX, int *const pY, /* 储存member_center节点指针的指针,可为空 */struct FsEbml_node * * const pMember_center
            , FsConfig * const pConfig, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0
            , struct FsEbml_node * pos_node, /* 为空表示不用搜索,为pos_node */const char *const member_center_nodename);

    /* 获取member_size和节点的指针,大小获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_member_size(unsigned int *const pW, unsigned int *const pH, /* 储存member_size节点指针的指针,可为空 */struct FsEbml_node * * const pMember_size
            , FsConfig * const pConfig, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0
            , struct FsEbml_node * pos_node, /* 为空表示不用搜索,为pos_node */const char *const member_size_nodename);

    /* 获取member_direction和节点的指针,大小获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_member_direction(double *const pAngle, /* 储存member_direction节点指针的指针,可为空 */struct FsEbml_node * * const pMember_direction
            , FsConfig * const pConfig, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0
            , struct FsEbml_node * pos_node, /* 为空表示不用搜索,为pos_node */const char *const member_direction_nodename);

    /* 获取member_area和节点的指针,大小获取成功返回1,失败返回-1 */

    int fs_Config_property_split_get_member_area(FsConfigResult_area * * const pArea, /* 储存member_area节点指针的指针,可为空 */struct FsEbml_node * * const pMember_area
            , FsConfig * const pConfig, /* pos_node对应的描述节点,为NULL表示需要内部查找 */const void * pos_node0
            , struct FsEbml_node * pos_node, /* 为空表示不用搜索,为pos_node */const char *const member_area_nodename
            , /* 比例尺,一个像素点代表的长度,X方向 */ const double zoomX, /* 比例尺,一个像素点代表的长度,Y方向 */ const double zoomY
            , /* 区域参考原点的x坐标 */ int x0, /* 区域参考原点的y坐标 */int y0);



#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */

