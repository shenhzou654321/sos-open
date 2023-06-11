/* 
 * File:   Json.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#if !defined JSON_H && !defined JAVA_IN_C

#include "../../PublicTools/publicDefine.h"
#define JSON_H
#define JSON_Node_Buffer
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/Object.h"
#include "../../PublicTools/Fs/String.h"
#include "../../PublicTools/Fs/Xml.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * 节点
     */
    struct FsJson_node {
        /* 节点名,此内存空间附加在节点后,无须单独释放内存 */
        char *nodeName;
        /* 未解析的数据,valueLen为0时,此值为空 */
        char* value;
        /* 未解析数据的长度 */
        unsigned long valueLen;
        /* 解析了的子节点数目 */
        unsigned int childNodeCount;
        /* 解析了的子节点 */
        struct FsJson_node** child;
        /* 父节点 */
        struct FsJson_node* parent;
    };

    /* 
     * FsJson中的成员顺序不能改变,改变无法运行
     */
    typedef struct {
        /* 注释 */
        char *comment;
        /* 未解析的数据,valueLen为0时,此值为空 */
        char* value;
        /* 未解析数据的长度 */
        unsigned long valueLen;
        /* 解析了的子节点数目 */
        unsigned int childNodeCount;
        /* 解析了的子节点 */
        struct FsJson_node** child;
#if !defined JSON_Node_Buffer
        /* 已解析的所有节点列表 */
        FsObjectList* nodeList;
#endif
        /* 线程同步锁,仅用于多线程,须在各线程中自已实现 */
        pthread_mutex_t mutex;
    } FsJson;

    /*
     * 创建一个FsJson实例;
     * 返回创建的实例指针. 
     */

    FsJson* fs_Json_new__IO();

    /*
     * 从缓存创建一个FsJson实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsJson* fs_Json_new_from_string_buffer__IO(/* 数据大小,不能为0 */unsigned long bufferLen, /* 不能为空 */const char buffer[]);

    /*
     * 从缓存创建一个FsJson实例并解析;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsJson * fs_Json_new_from_string_buffer_and_analyzeAll__IO(/* 数据大小,不能为0 */unsigned long bufferLen, /* 不能为空 */const char buffer_cin[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件创建一个FsJson实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsJson* fs_Json_new_from_file__IO(/* 文件名,不能为空 */const char fileName[]);

    /* 删除pJson指向的实例对象 */

    void fs_Json_delete__OI(FsJson * const pJson
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 分析pJson中的pJson_node下的所有节点;
     * 完全分析成功返回NULL;
     * 部分失败返回第一个出错的节点;
     * 可检查pJson_node是否有错误;
     */

    struct FsJson_node* fs_Json_analyzeAll_fast(FsJson * const pJson, /* 要分析的节点,分析根节点请把pJson强制转换为FsJson_node传入 */struct FsJson_node* pJson_node);

    /*
     * 向pJson中的节点pJson_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */

    struct FsJson_node * fs_Json_node_addChild1(FsJson * const pJson, /* 不能为空,在根节下查找请把pJson强制转换为FsJson_node传入 */struct FsJson_node * const pJson_node
            , /* 节点名长度 */const unsigned int nodeNameLen, /* 节点名,不能为空 */const char nodeName[]);

    /*
     * 向pJson中的节点pJson_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */

    struct FsJson_node* fs_Json_node_addChild(FsJson * const pJson, /* 不能为空,在根节下查找请把pJson强制转换为FsJson_node传入 */struct FsJson_node * const pJson_node
            , /* 节点名,不能为空 */const char nodeName[]);

    /* 向pNode节点添加值 */

    void fs_Json_node_add_value(/* 必须为有效值 */const void* pNode, /* value的长度 */const unsigned long valueLen, /* 必须为有效值 */const char value[]);

    /* 向pXml_node节点添加值,并删除value的空间 */

    void fs_Json_node_add_value__OI_3(/* 必须为有效值,支持根节点 */struct FsJson_node * const pJson_node, /* value的长度 */const unsigned long valueLen, /* 必须为有效值,且是动态分配的空间,以\0结束 */ char value[]);

    /* 设置pNode节点的值,会删除子节点 */

    void fs_Json_node_set_value(FsJson * const pJson, /* 必须为有效值 */ void* const pNode, const unsigned long valueLen, /* 可为空 */const char value[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 设置pNode节点的值,会删除子节点 */

    void fs_Json_node_set_value__OI_4(FsJson * const pJson, /* 必须为有效值 */ void* const pNode, const unsigned long valueLen, /* 节点值,可为空,不为空时必须以\0结尾 */ char value[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 删除pJson_node节点及子节点;
     * 传入的pJson_node必须为有效值.
     */

    void fs_Json_node_delete(FsJson * const pJson, struct FsJson_node * const pJson_node);

    /*
     * 删除pJson_node下名为name的所有节点及子节点,pJson_node必须已解析,没解析的内容不会被查找.
     */

    void fs_Json_node_delete_by_name(FsJson * const pJson, /* 要查找的的节点,在根节点下查找请把pJson强制转换为FsJson_node传入 */ struct FsJson_node * pJson_node, /* 要删除的节点名 */const char name[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 删除pJson_node下名为name的一个节点及子节点,随机移除,pJson_node必须已解析,没解析的内容不会被查找.
     */

    void fs_Json_node_delete_by_name_first(FsJson * const pJson, /* 要查找的的节点,在根节点下查找请把pJson强制转换为FsJson_node传入 */ struct FsJson_node * pJson_node, /* 要删除的节点名 */const char name[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 调整*ppJson_node节点空间的大小 */

    void fs_Json_node_name_resize(FsJson * const pJson, /* 必须为有效值 */ struct FsJson_node * * const ppJson_node
            , /* 名字长度,可为0,不包含\0的长度 */ const unsigned int nameLen, /* 在父节点中的编号,-1表示未确定 */int inParentIndex);

    /*
     * 把pJson转换成FsStringBuilder;
     * 返回字符串.
     */

    FsStringBuilder * fs_Json_to_StringBuilder__IO(const FsJson * const pJson
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson转换成字符串;
     * 返回FsString指针.
     */

    FsString * fs_Json_to_String__IO(const FsJson * const pJson, /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount
            , /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned int suffixCount
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson转换成字符串;
     * 返回字符串.
     */

    char *fs_Json_to_string__IO(const FsJson * const pJson
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson转换成字符串;
     * 返回FsObjectBase对象.
     */

    FsObjectBase * fs_Json_to_ObjectBase__IO(const FsJson * const pJson, /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson转换成字符串;
     * 返回FsObjectBase对象.
     */

    FsObjectBase * fs_Json_to_ObjectBase_with_buffer__IO(const FsJson * const pJson
            , /* 原缓存,不为空,在(*pObjectBaseBuffer)为空时,会把当前返回的对象的_unownedTag置1并设置到(*pObjectBaseBuffer)中,在(*pObjectBaseBuffer)->referCount为0时返回的数据会使用缓存 */
            FsObjectBase* * const pObjectBaseBuffer, /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson转换成字符串;
     * 返回FsObjectBase对象.
     */

    FsObjectBase * fs_Json_to_ObjectBase_from_buffer__IO(const FsJson * const pJson, /* 缓存Buffer,不为空 */FsObjectBaseBuffer * const pObjectBaseBuffer
            , /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson保存到文件filename中;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */

    int fs_Json_out(const FsJson * const pJson, FILE * const fd
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /*
     * 把pJson保存到文件filename中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */

    int fs_Json_save_to_file(const FsJson * const pJson, /* 文件名,不能为空 */const char filename[]
            , /* prefix长度 */ const unsigned int prefixLen, /* 前缀,如"/n /r/n",可为空 */ const char prefix[]
            , /* suffix长度 */ const unsigned int suffixLen, /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);

    /* 
     * 获取pJson中第一个为nodeName节点的节点;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * parentNode必须为有效值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */

    struct FsJson_node * fs_Json_node_get_first(const FsJson * const pJson, /* 在哪个节点下查找,在根节下查找请把pJson强制转换为FsJson_node传入 */ const struct FsJson_node * const parentNode
            , /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /* 
     * 获取pJson中所为nodeName节点的值;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * parentNode必须为有效值;
     * 成功返回FsObjectList指针;
     * 失败返回NULL.
     */

    FsObjectList * fs_Json_node_get_all__IO(const FsJson * const pJson, /* 在哪个节点下查找,在根节下查找请把pJson强制转换为FsJson_node传入 */ const struct FsJson_node * const parentNode
            , /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取pJson中所为nodeName节点的值;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * parentNode必须为有效值;
     * 成功返回节点值组成的FsArray指针;
     * 失败返回NULL.
     */

    FsArray * fs_Json_node_get_allData__IO(const FsJson * const pJson, /* 在哪个节点下查找,在根节下查找请把pJson强制转换为FsJson_node传入 */ const struct FsJson_node * const parentNode
            , /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



#ifdef __cplusplus
}
#endif

#endif /* JSON_H */

