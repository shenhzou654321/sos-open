/* 
 * File:   Json.h
 * Author: fslib
 *
 * Created on 2013年2月22日, 下午5:03
 */
#ifndef JSON_H
#define	JSON_H

#include "String.h"
#include "ObjectList.h"
#include "Xml.h"

#ifdef	__cplusplus
extern "C" {
#endif
    /*
     * 节点
     */
    struct FsJson_node {
        /* 节点名,此内存空间附加在节点后,无须单独释放内存 */
        char *nodeName;
        /* 未解析的数据 */
        char *value;
        /* 解析了的子节点数目 */
        unsigned long childNodeCount;
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
        /* 未解析的数据 */
        char *value;
        /* 解析了的子节点数目 */
        unsigned long childNodeCount;
        /* 解析了的子节点 */
        struct FsJson_node** child;
        /* 已解析的所有节点列表 */
        FsObjectList* nodeList;
        /* 线程同步锁,仅用于多线程，须在各线程中自已实现 */
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
     * 根据文件创建一个FsJson实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    FsJson* fs_Json_new_from_file__IO(/* 文件名,不能为空 */const char fileName[]);

    /* 删除pJson指向的实例对象 */
    void fs_Json_delete__OI(FsJson* pJson);
    /* 
     * 分析pJson中的pJson_node下的所有节点;
     * 完全分析成功返回NULL;
     * 部分失败返回第一个出错的节点;
     * 可检查pJson_node是否有错误;
     */
    struct FsJson_node* fs_Json_analyzeAll_fast(FsJson* pJson, /* 要分析的节点,分析根节点请把pJson强制转换为FsJson_node传入 */struct FsJson_node* pJson_node);
    /*
     * 向pJson中的节点pJson_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */
    struct FsJson_node * fs_Json_node_addChild1(FsJson * const pJson, /* 不能为空,在根节下查找请把pJson强制转换为FsJson_node传入 */struct FsJson_node * const pJson_node,
            /* 节点名长度 */const unsigned int nodeNameLen, /* 节点名,不能为空 */const char nodeName[]);
    /*
     * 向pJson中的节点pJson_node添加一个子节点;
     * 成功返回添加的节点的指针;
     * 失败返回NULL.
     */
    struct FsJson_node* fs_Json_node_addChild(FsJson * const pJson, /* 不能为空,在根节下查找请把pJson强制转换为FsJson_node传入 */struct FsJson_node * const pJson_node,
            /* 节点名,不能为空 */const char nodeName[]);
    /* 设置pNode节点的值,会删除子节点 */
    void fs_Json_node_set_value(FsJson * const pJson, /* 必须为有效值 */ void* const pNode, /* 可为空 */const char value[]);
    /* 设置pNode节点的值,会删除子节点 */
    void fs_Json_node_set_value__OI_3(FsJson * const pJson, /* 必须为有效值 */ void* const pNode, /* 可为空 */char value[]);
    /*
     * 删除pJson_node节点及子节点;
     * 传入的pJson_node必须为有效值.
     */
    void fs_Json_node_delete(FsJson* pJson, struct FsJson_node *pJson_node);
    /*
     * 把pJson转换成字符串;
     * 返回FsString指针.
     */
    FsString *fs_Json_to_String__IO(const FsJson * const pJson, /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount,
            /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned int suffixCount,
            /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 把pJson转换成字符串;
     * 返回字符串.
     */
    char *fs_Json_to_string__IO(const FsJson * const pJson, /* 前缀,如"/n /n/r",可为空 */ const char prefix[],
            /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
        /*
     * 把pJson保存到文件filename中;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */
    signed char fs_Json_out(const FsJson * const pJson, FILE* const fd,
            /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /*
     * 把pJson保存到文件filename中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    signed char fs_Json_save_to_file(const FsJson * const pJson, /* 文件名,不能为空 */const char filename[],
            /* 前缀,如"/n /n/r",可为空 */ const char prefix[], /* 后缀,如"/n",可为空 */ const char suffix[], /* 缩进增加量 */const unsigned char indent_add);
    /* 
     * 获取pJson中所为nodeName节点的值;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * parentNode必须为有效值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */
    FsObjectList *fs_Json_node_get_all__IO(const FsJson * const pJson, /* 在哪个节点下查找,在根节下查找请把pJson强制转换为FsJson_node传入 */ const struct FsJson_node * const parentNode,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]);
    /* 
     * 获取pJson中所为nodeName节点的值;
     * 支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * parentNode必须为有效值;
     * 成功返回节点值组成的FsArray指针;
     * 失败返回NULL.
     */
    FsArray *fs_Json_node_get_allData__IO(const FsJson * const pJson, /* 在哪个节点下查找,在根节下查找请把pJson强制转换为FsJson_node传入 */ const struct FsJson_node * const parentNode,
            /* 节点名,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值 */const char nodeName[]);
#ifdef FsDebug
    void fs_Json_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* JSON_H */

