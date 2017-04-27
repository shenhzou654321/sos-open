/* 
 * File:   Xml.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#ifndef XML_H
#define	XML_H

#include "String.h"
#include "ObjectList.h"

#ifdef	__cplusplus
extern "C" {
#endif

    /*
     * 节点
     */
    struct FsXml_node {
        /* 节点名,此内存空间附加在节点后,无须单独释放内存 */
        char *nodeName;
        /* 未解析的数据 */
        char* value;
        /* 解析了的子节点数目 */
        unsigned long childNodeCount;
        /* 解析了的子节点 */
        struct FsXml_node** child;
        /* 属性数量 */
        unsigned long propertyCount;
        /* 属性(0:属性名;1:属性值(为空时也要保留结束符),内存连续分配,释放时只须释放属性名的内存) */
        char* (*property)[2];
        /*  父节点 */
        struct FsXml_node* parent;
    };

    /* 
     * FsXml中的成员顺序不能改变,改变无法运行;
     * XML中如果出现多版本或编码的定义,以第一个为准.
     * 解析后再保存只会保存紧接着一个定义的第一条<!-- -->这样的注释,XML中也只会有一个定义;
     */
    typedef struct {
        /* 注释 */
        char *comment;
        /* 未解析的数据 */
        char* value;
        /* 解析了的子节点数目 */
        unsigned long childNodeCount;
        /* 解析了的子节点 */
        struct FsXml_node** child;
        /* 已解析的所有节点列表 */
        FsObjectList* nodeList;
        /* xml版本 */
        char *version;
        /* 编码方式 */
        char *encodeMethod;
        /* 线程同步锁,仅用于多线程，须在各线程中自已实现 */
        pthread_mutex_t mutex;
    } FsXml;
    /*
     * 创建一个FsXml实例;
     * 返回创建的实例指针. 
     */
    FsXml* fs_Xml_new__IO();
    /*
     * 从缓存创建一个FsXml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsXml* fs_Xml_new_from_string_buffer__IO(/* 数据大小,不能为0 */unsigned long bufferLen, /* 不能为空 */const char buffer[]);
    /*
     * 根据字符串创建一个FsXml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsXml* fs_Xml_new_from_string__IO(/* 不能为空 */const char str[]);
    /*
     * 根据文件句柄创建一个FsXml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsXml* fs_Xml_new_from_fd__IO(FILE * const fd);
    /*
     * 根据文件创建一个FsXml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsXml* fs_Xml_new_from_file__IO(/* 文件名,不能为空 */const char fileName[]);
    /* 删除pXml指向的实例对象 */
    void fs_Xml_delete__OI(FsXml* pXml);
    /* 
     * 分析pXml中的pXml_node下的所有节点;
     * pXml_node:要分析的节点,分析根节点请把pXml强制转换为FsXml_node传入;
     * 完全分析成功返回NULL;
     * 部分失败返回第一个出错的节点;
     * 可检查pXml_node是否有错误;
     */
    struct FsXml_node* fs_Xml_analyzeAll(FsXml* pXml, struct FsXml_node* pXml_node);
    /* 
     * 在pXml中已解析的节点中查找parentNode;
     * 成功返回1;
     * 失败返回-1;
     */
    char fs_xml_node_position_checkValid(FsXml* pXml, void* parentNode);
    /* pNode节点添加值 */
    void fs_Xml_node_add_value(/* 必须为有效值 */const void* pNode, /* 必须为有效值 */const char value[]);
    /* 向pXml_node节点添加值,并删除value的空间 */
    void fs_Xml_node_add_value__OI_2(/* 必须为有效值,支持根节点 */struct FsXml_node* pXml_node, /* 必须为有效值,且是动态分配的空间 */ char value[]);
    /*
     * 向pXml中的节点pXml_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */
    struct FsXml_node* fs_Xml_node_addChild1(FsXml* pXml, /* 不能为空,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node* pXml_node,
            /* 节点名长度 */const unsigned int nodeNameLen, /* 节点名,不能为空 */const char nodeName[]);
    /*
     * 向pXml中的节点pXml_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */
    struct FsXml_node* fs_Xml_node_addChild(FsXml * const pXml, /* 不能为空,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node * const pXml_node, /* 节点名,不能为空 */const char nodeName[]);
    /*
     * 向pXml中的节点pXml_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */
    struct FsXml_node* fs_Xml_node_addChild_with_data(FsXml* pXml, /* 不能为空,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node* pXml_node,
            /* 节点名,不能为空 */ const char nodeName[], /* 节点值,不能为空 */const char data[]);
    /*
     * 向pXml中的节点pXml_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */
    struct FsXml_node* fs_Xml_node_addChild_with_data_and_a_property(FsXml* pXml, /* 不能为空,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node* pXml_node,
            /* 节点名,不能为空 */ const char nodeName[], /* 节点值,不能为空 */const char data[],
            /* 属性名,不能为空 */const char propertyName[], /* 属性名,可为空 */const char propertyValue[]);
    /* 设置pNode节点的值,会删除子节点 */
    void fs_Xml_node_set_value__IO_3(FsXml* pXml, /* 必须为有效值 */ void* pNode, /* 可为空 */ char value[]);
    /* 设置pNode节点的值,会删除子节点 */
    void fs_Xml_node_set_value(FsXml* pXml, /* 必须为有效值 */ void* pNode, /* 可为空 */const char value[]);
    /*
     * 在pXml_node节点中添加一个属性,只有在添加的属性不存在时,才能用此方法;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_Xml_node_property_add__OI_2(struct FsXml_node* pXml_node, /* 要添加的属性名,属性名不能已存在,否则会出错,必须是动态分配的空间 */char propertyName[],
            /* 属性值,不能为空,支持"",内存必须是附加到propertyName后的内存 */char propertyValue[]);
    /*
     * 在pXml_node节点中添加一个属性,只有在添加的属性不存在时,才能用此方法;
     * 成功返回1;
     * 失败返回-1.
     */
    char fs_Xml_node_property_add(struct FsXml_node* pXml_node, /* 要添加的属性名,属性名不能已存在,否则会出错 */ const char propertyName[], /* 属性值,不能为空,支持"" */const char propertyValue[]);
    /*
     * 设置pXml_node节点的属性;
     * 属性名不存在,创建属性返回1;
     * 属性名已存在,改变属性返回2;
     * 失败返回-1.
     */
    signed char fs_Xml_node_property_set__OI_2(struct FsXml_node* pXml_node, /* 要添加的属性名,必须是动态分配的空间 */char propertyName[],
            /* 属性值,不能为空,支持"",内存必须是附加到propertyName后的内存 */ char propertyValue[]);
    /*
     * 设置pXml_node节点的属性;
     * 属性名不存在,创建属性返回1;
     * 属性名已存在,改变属性返回2;
     * 失败返回-1.
     */
    signed char fs_Xml_node_property_set(struct FsXml_node* pXml_node, /* 要添加的属性名,属性名不能已存在,否则会出错 */ const char propertyName[],
            /* 属性值,不能为空,支持"",内存必须是附加到propertyName后的内存 */const char propertyValue[]);
    /*
     * 删除pXml_node节点中名为propertyName的属性;
     * 成功返回1;
     * 无此属性返回-1;
     */
    char fs_Xml_node_property_delete(struct FsXml_node* pXml_node, /* 要删除的属性名,不能为空 */const char propertyName[]);
    /*
     * 删除pXml_node节点及子节点;
     * 传入的pXml_node必须为有效值.
     */
    void fs_Xml_node_delete(FsXml* pXml, struct FsXml_node *pXml_node);
    /*
     * 获取所有名为name的节点.
     */
    FsObjectList *fs_Xml_node_get_by_name_all__IO(FsXml* pXml, /* 要获取的节点名 */const char name[]);
    /*
     * 删除名为name的所有节点及子节点.
     */
    void fs_Xml_node_delete_by_name(FsXml* pXml, /* 要删除的节点名 */const char name[]);
    /*
     * 删除名为leafName的所有节点,调用此函数时要保证名为leafName的节点没有子节点,否则会死机.
     */
    void fs_Xml_node_delete_by_leafName(FsXml* pXml, /* 要删除的节点名 */const char leafName[]);
    /*
     * 删除名为leafName的一个节点,随机移除,调用此函数时要保证名为leafName的节点没有子节点,否则会死机.
     */
    void fs_Xml_node_delete_by_leafName_first(FsXml* pXml, /* 要删除的节点名 */const char leafName[]);
    /*
     * 删除pXml_node的子节点;
     * 传入的pXml_node必须为有效值.
     */
    void fs_Xml_node_delete_child(FsXml* pXml, struct FsXml_node *pXml_node);
    /*
     * 把pNode节点转为字符串;
     * 传入的pNode必须为有效值.
     */
    char *fs_Xml_node_to_string__IO(void* pNode, /* 前缀,如"/n /n/r",可为空 */const char prefix[], /* 后缀,如"/n",可为空 */const char suffix[],
            /* 子节点缩进值 */unsigned long indent_child, /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 获取pNode节点的值;
     * 传入的pNode必须为有效值;
     */
    char *fs_Xml_node_get_value__IO(void* pNode, /* 前缀,如"/n /n/r",可为空 */const char prefix[], /* 后缀,如"/n",可为空 */const char suffix[],
            /* 子节点缩进值 */unsigned long indent_child, /* 缩进增加量 */const unsigned char indent_add);
    /* 
     * 解析pXml_node中的下一个子节点;
     * 传入的pXml_node必须为有效值且pXml_node->value不为空,如果第一个<前有其它字符,会删除;
     * 成功返回解析了的子节点;
     * 返回NULL.
     */
    struct FsXml_node* fs_Xml_node_resolve_next_childNode(FsXml *pXml, struct FsXml_node* pXml_node);
    /* 
     * 获取pXml中第一个名为nodeName的节点;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */
    struct FsXml_node *fs_Xml_node_get_first(FsXml* pXml,
            /* 在哪个节点下查找,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node* pXml_node,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]);
    /* 
     * 获取pXml中第一个名为nodeName节点且nodeName中最后一个节点的属性为相应属性的节点;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */
    struct FsXml_node *fs_Xml_node_get_first_with_a_lastProperty(FsXml* pXml, /* 在哪个节点下查找,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node* parentNode,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */ char nodeName[],
            /* nodeName中最后一个节点的属性名,不能为空 */const char propertyName[], /* 属性值,不能为空,支持"" */const char propertyValue[]);
    /* 
     * 获取pXml中所为nodeName节点的值;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * pXml_node必须为有效值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */
    FsObjectList *fs_Xml_node_get_all__IO(FsXml* pXml, /* 在哪个节点下查找,在根节下查找请把pXml强制转换为FsXml_node传入 */ struct FsXml_node* parentNode,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]);
    /* 
     * 获取pXml中第一个名为nodeName节点的值;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点值;
     * 失败返回NULL;
     * 返回的值需在外部手动释放空间.
     */
    char *fs_Xml_node_get_value_string_first__IO(FsXml* pXml, /* 在哪个节点下查找,在根节下查找请把pXml强制转换为FsXml_node传入 */struct FsXml_node* parentNode,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]);
    /* 
     * 获取pXml中第一个名为nodeName节点的值
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点值;
     * 失败返回defaultValue.
     */
    long fs_Xml_node_get_value_number_first(FsXml* pXml, /* 在哪个节点下查找,在根节下查找请把pXml强制转换为FsXml_node传入 */ struct FsXml_node* parentNode,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */ const char nodeName[], long defaultValue);
    /*
     * 把pXml转换成字符串;
     * 返回字符串.
     */
    FsString *fs_Xml_to_String__IO(const FsXml* pXml, /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount,
            /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned int suffixCount,
            /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 把pXml转换成字符串;
     * 返回字符串.
     */
    char *fs_Xml_to_string__IO(const FsXml* pXml, /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 把pXml转换成字符串,不带属性;
     * 返回字符串.
     */
    char *fs_Xml_to_string_noProperty__IO(const FsXml* pXml, /* 前缀,如"/n /n/r",可为空 */ const char prefix[],
            /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 把pXml数据写入文件流fd中;
     * 如果成功返回1;
     * 如果写文件失败返回-2;
     * 获取xml字符串失败返回-3.
     */
    signed char fs_Xml_out(const FsXml* pXml, FILE * const fd,
            /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 把pXml保存到文件filename中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2;
     * 获取xml字符串失败返回-3.
     */
    signed char fs_Xml_save_to_file(const FsXml* pXml, /* 文件名,不能为空 */const char filename[],
            /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
#ifdef FsDebug
    void fs_Xml_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* XML_H */

