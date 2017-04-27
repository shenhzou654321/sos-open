/* 
 * File:   Ebml.h
 * Author: fslib
 *
 * Created on 2013年5月29日, 下午9:35
 * 2015-08-11修改为使用密码时,校验位为原校验和与节点异或的结果,并兼容以前的版本
 */

#ifndef EBML_H
#define	EBML_H

#include "../publicDefine.h"
#include "String.h"
#include "StringBuilder.h"
#include "Xml.h"
#include "Json.h"
#ifdef	__cplusplus
extern "C" {
#endif
    /* 
     * Ebml数字解析方式:
     *     采用变长编码,低位在前高位在后;
     *     从首字节的最后一个为1的位字节解析数字占的字节数;
     *         如1010 0101:最后一个为1,表示此数字的字节数为1,值为:0101 0010;
     *         如1010 0110 1010 0101:倒数一个为1,表示此数字的字节数为2,值为0110 1001 0010 1001.       
     */

    /* FsEbml中的节点类型,解析时不可直接比对,须先按变长解析,再比对,1-7为系统保留的大类型,用户扩展类弄必为其成员. */
    /* 结构结点类类型,表示储存数据也是节点,(type&0x07)==0x01 */
#define FsEbmlNodeType_Struct  0x01
    /* 字符串结点类型,表示储存数据是字符串,(type&0x07)==0x02 */
#define FsEbmlNodeType_String  0x02
    /* 二进制结点类型,表示储存数据是二进制数据,(type&0x07)==0x03 */
#define FsEbmlNodeType_Binary  0x03
    /* 整数结点类型,表示储存数据是整数,数据储存固定为8个字节,(type&0x07)==0x04 */
#define FsEbmlNodeType_Integer  0x04
    /* 浮点结点类型,表示储存数据是浮点数,数据储存固定为8个字节,(type&0x07)==0x05 */
#define FsEbmlNodeType_Float  0x05
    /* 时间结点类型,表示储存数据是浮点数,且表示时间,数据储存固定为8个字节,(type&0xFF)==FsEbmlNodeType_Time */
#define FsEbmlNodeType_Time  (FsEbmlNodeType_Float|0x08)

    /* FsEbml支持的文本编码方式 */
    /* UTF-8(1) */
#define FsEbmlEncodeMethod_UTF8  1
    /* GBK(2) */
#define FsEbmlEncodeMethod_GBK  2

    /* FsEbml中的节点;
     * 文件中保存顺序:
     *     节点名+数据类型(变长编码)+属性数量(变长编码)+数据长度(变长编码)+属性+数据.
     */
    struct FsEbml_node {
        /* 数据,当为整型或浮点型,数据附加到节点后,不当独分配内存,且内存是对齐的,可以直接转换为数字 */
        FsString data;
        /* 解析了的子节点数目,文件中不保存此字段 */
        unsigned long childNodeCount;
        /* 解析了的子节点,文件中不保存此字段 */
        struct FsEbml_node** child;
        /* 节点类型(具体类型见类型列表) */
        unsigned long type;
        /* 节点名,数据附加到节点后,不当独分配内存 */
        char *nodeName;
        /* 属性数量 */
        unsigned long propertyCount;
        /* 属性(0:属性名;1:属性值(为空时也要保留结束符),内存连续分配,释放时只须释放属性名的内存) */
        char* (*property)[2];
        /* 父节点,文件中不保存此字段 */
        struct FsEbml_node* parent;
    };

    /* 
     * FsEbml是一种可变长可扩展的二进制结构,能储存任意数据,仅当全为字符串时,支持转为XML;
     * FsEbml相比XML,使用的储存空间更小,解析速度更快;
     * FsEbml不支持通用文本工具查看;
     * 在32位系统中使用时,请确保打开的文件不超过4G,否则会出错;
     * 储存结构:
     *    FSEBML用BASE32解码后的数据乘以数据长度再和自定义标记按位异或的结果(4字节)+数据长度(变长编码)+版本(变长编码)+文本编码方式(变长编码)+节点+校验位(1字节);
     *    数据长度(变长编码):只包含版本(变长编码)+文本编码方式(变长编码)+节点的长度.
     * FSEBML用BASE32解码后的数据为(低位在前):5 18 4 1 12 11,二进制为01011 01100 00001 00100 10010 00101,16进制为0x16C09245.
     */
    typedef struct {
        /* 文件数据 */
        FsString fileData;
        /* 解析了的子节点数目 */
        unsigned long childNodeCount;
        /* 解析了的子节点 */
        struct FsEbml_node** child;
        /* 版本:从1开始 */
        unsigned int version;
        /* 文本编码方式,为与XML兼容 */
        unsigned int encodeMethod;
        /* 自定义标记,本对象不使用 */
        unsigned int customTag;
        /* 已解析的所有节点列表 */
        FsObjectList* nodeList;
        /* 线程同步锁,仅用于多线程，须在各线程中自已实现 */
        pthread_mutex_t mutex;
    } FsEbml;
    /*
     * 创建一个FsEbml实例;
     * 返回创建的实例指针.
     */
    FsEbml* fs_Ebml_new__IO(/* 文本编码方式,请使用宏定义 */const int encodeMethod, /* 自定义标记,本对象不使用 */const unsigned int customTag);
    /*
     * 创建一个FsEbml实例;
     * 返回创建的实例指针.
     */
    FsEbml* fs_Ebml_new_extern__IO(/* 文本编码方式,请使用宏定义 */const int encodeMethod, /* 自定义标记,本对象不使用 */const unsigned int customTag, /* 尾部追加的大小 */ const unsigned int externSize);
    /*
     * 根据buffer创建一个FsEbml实例;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    FsEbml* fs_Ebml_new_from_String__IO(/* 数据长度 */unsigned long bufferLenth, /* 数据开始地址 */unsigned char buffer[], /* 密码,为空或长度为0表示无密码 */const unsigned char password[]);
    /*
     * 根据buffer创建一个FsEbml实例;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    FsEbml* fs_Ebml_new_from_String_extern__IO(/* 数据长度 */unsigned long bufferLenth, /* 数据开始地址 */unsigned char buffer[], /* 密码,为空或长度为0表示无密码 */const unsigned char password[],
            /* 尾部追加的大小 */ const unsigned int externSize);
    /*
     * 根据文件创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsEbml* fs_Ebml_new_from_file__IO(/* 文件名不能为空 */const char fileName[], /* 密码,为空或长度为0表示无密码 */const char password[]);
    /*
     * 根据文件创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsEbml* fs_Ebml_new_from_file_extern__IO(/* 文件名不能为空 */const char fileName[], /* 密码,为空或长度为0表示无密码 */const char password[], /* 尾部追加的大小 */ const unsigned int externSize);
    /*
     * 从pEbml创建一个FsEbml实例,可实现数据拷贝;
     * 返回创建的实例指针.
     */
    FsEbml* fs_Ebml_new_from_Ebml__IO(const FsEbml * const pEbml);
    /*
     * 从pEbml创建一个FsEbml实例,可实现数据拷贝;
     * 返回创建的实例指针.
     */
    FsEbml* fs_Ebml_new_from_Ebml_extern__IO(const FsEbml * const pEbml, /* 尾部追加的大小 */ const unsigned int externSize);
    /*
     * 根据pXml创建一个FsEbml实例;
     * 成功返回创建的实例指针;
     * 失败返回NULL;  
     * pXml:不能为空且要已完全解析;
     * 对于pXml编码,如果不是UTF-8,则会认为是GBK.
     */
    FsEbml* fs_Ebml_new_from_Xml__IO(const FsXml * const pXml);
    /*
     * 根据pXml创建一个FsEbml实例,忽略xml中的属性;
     * 成功返回创建的实例指针;
     * 失败返回NULL;  
     * pXml:不能为空且要已完全解析;
     * 对于pXml编码,如果不是UTF-8,则会认为是GBK.
     */
    FsEbml* fs_Ebml_new_from_Xml_noProperty__IO(const FsXml * const pXml);
    /*
     * 根据pJson创建一个FsEbml实例;
     * 返回创建的实例指针.
     */
    FsEbml* fs_Ebml_new_from_Json__IO(/* 不能为空且要已完全解析 */const FsJson * const pJson);
    /* 删除pEbml指向的实例 */
    void fs_Ebml_delete__OI(FsEbml * const pEbml);
    /* 
     * 获取此EBML数据的真实长度;
     * 成功返回不小于8的数;
     * 失败返回-1; 
     */
    long fs_Ebml_get_lenth(/* 分析的数据长度,不少于5个字节 */const unsigned long len, /* 分析数据的开始指针 */ const unsigned char buffer[]);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */
    struct FsEbml_node* fs_Ebml_node_addChild(FsEbml* pEbml, struct FsEbml_node* pEbml_node,
            /* 节点名,不能为空 */ const char nodeName[], /* 节点类型.必须为有效值,请使用宏定义 */ const unsigned long ebmlNodeType);
    /*
     * 向pEbml中的节点pEbml_node添加一个子节点;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */
    struct FsEbml_node* fs_Ebml_node_addChild_extern(FsEbml* pEbml, struct FsEbml_node* pEbml_node,
            /* 节点名前缀,可为空 */const char prefix[], /* 节点名,不能为空 */ const char nodeName[], /* 节点名前缀,可为空 */const char suffix[],
            /* 节点类型.必须为有效值,请使用宏定义 */ const unsigned long ebmlNodeType);
    /*
     * 向pEbml中的节点pEbml_node添加一个子节点;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */
    struct FsEbml_node* fs_Ebml_node_addChild_with_a_property(FsEbml* pEbml, struct FsEbml_node* pEbml_node,
            /* 节点名,不能为空 */ const char nodeName[],
            /* 节点类型.必须为有效值,请使用宏定义 */const unsigned long ebmlNodeType,
            /* 属性名,不能为空 */const char propertyName[],
            /* 属性值,可为空 */ const char propertyValue[]);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点,把addNode作为子节点拷贝插入;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */
    struct FsEbml_node* fs_Ebml_node_addChild_from_node(FsEbml* pEbml, struct FsEbml_node* pEbml_node, /* 被插入的节点 */struct FsEbml_node *addNode);
    /*
     * 把addNode的所有子节点拷贝添加到pEbml中的节点pEbml_node下;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入.
     */
    void fs_Ebml_node_addChild_from_node_child(FsEbml* pEbml, struct FsEbml_node* pEbml_node, /* 被插入的节点 */struct FsEbml_node *addNode);
    /*
     * 删除pEbml中的节点pEbml_node;
     * pEbml_node:不能为pEbml对象;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_Ebml_node_delete(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node);
    /*
     * 删除pEbml中pEbml_node下的与nodeName匹配的所有节点;
     * 返回匹配的节点数.
     */
    unsigned long fs_Ebml_node_delete_byString(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node,
            /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);
    /*
     * 删除pEbml中pEbml_node下的与nodeName同名的所有节点;
     * 返回匹配的节点数.
     */
    unsigned long fs_Ebml_node_delete_child_byString(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node,
            /* 节点名 */ const char childNodeName[]);
    /* 设置pEbml_node节点的值,内部释放buffer的空间,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */
    void fs_Ebml_node_data_set__OI_3(struct FsEbml_node * const pEbml_node, /* 长度,如果是字符串,应为字符串长度+1 */const unsigned long lenth, /* 可为空 */char buffer[]);
    /* 设置pEbml_node节点的值,内部释放pString的空间,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */
    void fs_Ebml_node_data_set__OI_2(struct FsEbml_node * const pEbml_node, /* 可为空 */ FsString * const pString);
    /* 设置pEbml_node节点的值,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */
    void fs_Ebml_node_data_set(struct FsEbml_node * const pEbml_node, /* 长度,如果是字符串,应为字符串长度+1 */const unsigned long lenth, /* 可为空 */const char buffer[]);
    /* 设置pEbml_node节点的值,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */
    void fs_Ebml_node_data_set_string(struct FsEbml_node * const pEbml_node, /* 要设置的字符串值,可为空 */ const char string[]);
    /* 
     * 分析pEbml中pEbml_node下的所有节点;
     * 完全分析成功返回NULL;
     * 部分失败返回第一个出错的节点;
     * 可检查pEbml_node是否有错误;
     */
    struct FsEbml_node* fs_Ebml_analyzeAll(FsEbml* pEbml, /* 要分析的节点,分析根节点请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node);


    /*
     * 在pEbml_node节点中添加一个属性;
     * pEbml_node:必须为有效值;
     * 成功返回1;
     * 失败返回-1.
     */
    char fs_Ebml_node_property_add__OI_2(struct FsEbml_node* pEbml_node,
            /* 要添加的属性名,属性名不能已存在,否则会出错 */ char propertyName[], /* 属性值,可为空,内存必须是附加到propertyName后的内存 */ char propertyValue[]);
    /*
     * 在pEbml_node节点中添加一个属性,只有在添加的属性不存在时,才能用此方法;
     * pEbml_node:必须为有效值;
     * 成功返回1;
     * 失败返回-1.
     */
    char fs_Ebml_node_property_add(struct FsEbml_node* pEbml_node, /* 要添加的属性名,属性名不能已存在,否则会出错 */ const char propertyName[], /* 属性值,可为空 */ const char propertyValue[]);

    /* 
     * 解析pEbml_node中的下一个子节点;
     * 传入的pEbml_node必须为有效值且pEbml_node->data.lenth大于0,且要是节点类;
     * 成功返回解析的节点;
     * 失败返回NULL.
     */
    struct FsEbml_node* fs_Ebml_node_resolve_next_childNode(FsEbml *pEbml, struct FsEbml_node* pEbml_node);
    /* 
     * 获取pEbml中pEbml_node下所有为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_Ebml_node_get(/* 返回的节点添加到尾部 */FsObjectList * const rst, FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node,
            /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);
    /* 
     * 获取pEbml中pEbml_node下所有为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点链表指针;
     * 失败返回NULL.
     */
    FsObjectList *fs_Ebml_node_get__IO(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node,
            /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);
    /* 
     * 获取pEbml中pEbml_node下所有为nodeName节点名的节点,可以是非直接子节点,返回的节点不存在父子关系,匹配方式为稀疏匹配;
     * 成功返回节点链表指针;
     * 失败返回NULL.
     */
    FsObjectList *fs_Ebml_node_get_child__IO(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node,
            /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);
    /* 
     * 获取pEbml中第一个名为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */
    struct FsEbml_node *fs_Ebml_node_get_first(FsEbml* pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node,
            /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /* 
     * 获取pEbml中第一个名为nodeName节点且nodeName中最后一个节点的属性为相应属性的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */
    struct FsEbml_node *fs_Ebml_node_get_first_with_a_lastProperty(FsEbml* pEbml, struct FsEbml_node* pEbml_node, /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */const char nodeName[],
            /* nodeName中最后一个节点的属性名,不能为空 */ const char propertyName[], /* 属性值,可为空 */ const char propertyValue[]);
    /*
     * 把pEbml转换为FsXml;
     * 成功返回FsXml的实例指针; 
     * 失败返回NULL.
     */
    FsXml* fs_Ebml_convert_to_Xml__IO(/* 不能为空且要已完全解析 */const FsEbml *pEbml);
    /*
     * 把pEbml转换为FsJson;
     * 成功返回FsJson的实例指针; 
     * 失败返回NULL.
     */
    FsJson* fs_Ebml_convert_to_Json__IO(/* 不能为空且要已完全解析 */const FsEbml * const pEbml);
    /*
     * 把pEbml存放在-个FsStringBuilder中,顺序的反的;
     * 返回FsStringBuilder对象.
     */
    FsStringBuilder *fs_Ebml_to_StringBuilder__IO(const FsEbml* pEbml);
    /*
     * 把pEbml转换成-个Buffer;
     * 返回FsString对象;
     * prefixCount:实际数据开始位置距离返回结果的buffer头的字节数;
     * suffixCount:实际数据结束位置距离返回结果的buffer尾的字节数;
     * password:密码,为空或长度为0表示不加密.
     */
    FsString *fs_Ebml_to_String__IO(const FsEbml* pEbml, const unsigned int prefixCount,
            const unsigned int suffixCount, const char password[]);
    /* 
     * 把pEbml数据写入文件流fd中;
     * 成功返回1;
     * 如果写文件失败返回-2.
     */
    char fs_Ebml_out(const FsEbml* pEbml, FILE *fd, /* 密码,为空或长度为0表示不加密 */const char password[]);
    /*
     * 把pEbml保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    signed char fs_Ebml_save_to_file(const FsEbml* pEbml, /* 文件名 */ const char fileName[], /* 密码,为空或长度为0表示不加密 */const char password[]);
    /* 
     * 获取所有已解析节点的值并储存在连续内存中;
     * 成功返回FsString指针对象;
     * 失败返回NULL.   
     */
    FsString *fs_Ebml_value_all_get__IO(const FsEbml* pEbml);
    /* 把pEbml调试数据写入文件流fd中 */
    void fs_Ebml_out_debug(FsEbml* pEbml, FILE *fd);

#ifdef FsDebug
    void fs_Ebml_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* EBML_H */

