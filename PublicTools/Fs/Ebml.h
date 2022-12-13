/* 
 * File:   Ebml.h
 * Author: fslib
 *
 * Created on 2013年5月29日, 下午9:35
 * 2015-08-11修改为使用密码时,校验位为原校验和与节点异或的结果,并兼容以前的版本
 */

#if !defined EBML_H && !defined JAVA_IN_C

#define EBML_H
#define EBML_Node_Buffer

#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/Json.h"
#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/String.h"
#include "../../PublicTools/Fs/StringBuilder.h"
#include "../../PublicTools/Fs/Xml.h"
#ifdef __cplusplus
extern "C" {
#endif
    /* 
     * Ebml数字解析方式:
     *     采用变长编码,低位在前高位在后;
     *     从首字节的最后一个为1的位字节解析数字占的字节数;
     *         如1010 0101:最后一个为1,表示此数字的字节数为1,值为:0101 0010;
     *         如1010 0110 1010 0101:倒数一个为1,表示此数字的字节数为2,值为0110 1001 0010 1001.       
     */

    /* FsEbml中的节点类型,解析时不可直接比对,须先按变长解析,再比对,1-7为系统保留的大类型,用户扩展类必为其成员. */
    typedef enum {
        /* 结构结点类类型,表示储存数据也是节点,(type&0x07)==0x01 */
        FsEbmlNodeType_Struct = 0x01U,
#define FsEbmlNodeType_Struct  FsEbmlNodeType_Struct
#define FsEbmlNodeType_is_Struct(__type) (FsEbmlNodeType_Struct==((__type)&0x7U))
        /* 字符串结点类型,表示储存数据是字符串,储存时需要储存结束符0,(type&0x07)==0x02 */
        FsEbmlNodeType_String = 0x02U,
#define FsEbmlNodeType_String  FsEbmlNodeType_String
#define FsEbmlNodeType_is_String(__type) (FsEbmlNodeType_String==((__type)&0x7U))
        /* 二进制结点类型,表示储存数据是二进制数据,(type&0x07)==0x03 */
        FsEbmlNodeType_Binary = 0x03U,
#define FsEbmlNodeType_Binary  FsEbmlNodeType_Binary
#define FsEbmlNodeType_is_Binary(__type) (FsEbmlNodeType_Binary==((__type)&0x7U))
        /* 整数结点类型,表示储存数据是整数,数据储存固定为8个字节,(type&0x07)==0x04 */
        FsEbmlNodeType_Integer = 0x04U,
#define FsEbmlNodeType_Integer  FsEbmlNodeType_Integer
#define FsEbmlNodeType_is_Integer(__type) (FsEbmlNodeType_Integer==((__type)&0x7U))
        /* 浮点结点类型,表示储存数据是浮点数,数据储存固定为8个字节,(type&0x07)==0x05 */
        FsEbmlNodeType_Float = 0x05U,
#define FsEbmlNodeType_Float  FsEbmlNodeType_Float
#define FsEbmlNodeType_is_Float(__type) (FsEbmlNodeType_Float==((__type)&0x7U))
        /* 时间结点类型,表示储存数据是浮点数,且表示时间,数据储存固定为8个字节,(type&0xFF)==FsEbmlNodeType_Time */
        FsEbmlNodeType_Time = (FsEbmlNodeType_Float | 0x08U),
#define FsEbmlNodeType_Time  FsEbmlNodeType_Time
#define FsEbmlNodeType_is_Time(__type) (FsEbmlNodeType_Time==((__type)&0xFFU))
        /* IPv4地质结点类型,表示储存数据是ipv4地址+端口号,数据储存固定为8个字节,(type&0xFF)==FsEbmlNodeType_ipv4 */
        FsEbmlNodeType_ipv4 = (FsEbmlNodeType_Integer | 0x08U),
#define FsEbmlNodeType_ipv4  FsEbmlNodeType_ipv4
#define FsEbmlNodeType_is_ipv4(__type) (FsEbmlNodeType_ipv4==((__type)&0xFFU))
    } FsEbmlNodeType;
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
        unsigned int childNodeCount;
        /* 属性数量 */
        unsigned int propertyCount;
        /* 解析了的子节点,文件中不保存此字段 */
        struct FsEbml_node** child;
#define FsEbml_node_get_sum(___pEbml_node) *(unsigned long long*)fs_pointer_to_void_pointer(&((struct FsEbml_node*)(___pEbml_node))->property)
#define FsEbml_node_set_sum_invalid(___pEbml_node) FsEbml_node_get_sum(___pEbml_node)=0
        /* 属性(0:属性名;1:属性值(为空时也要保留结束符),内存连续分配,释放时只须释放属性名的内存) */
        char* (*property)[2];
        /* 在不使用属性的应用中可使用property+_sum的空间储存本节点的校验和 */
        unsigned int _sum;
        /* 节点类型(具体类型见类型列表) */
        FsEbmlNodeType type;
        /* 节点名,数据附加到节点后,不当独分配内存 */
        char *nodeName;
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
        unsigned int childNodeCount;
        /* 自定义标记,本对象不使用 */
        unsigned int customTag;
        /* 解析了的子节点 */
        struct FsEbml_node** child;
        /* 版本:从1开始 */
        unsigned int version;
        /* 文本编码方式,为与XML兼容 */
        unsigned int encodeMethod;
#if !defined EBML_Node_Buffer
        /* 已解析的所有节点列表 */
        FsObjectList* nodeList;
#endif
        /* 线程同步锁,仅用于多线程,须在各线程中自已实现 */
        pthread_mutex_t mutex;
    } FsEbml;

    /* 检查数据是否可能是ebml数据,可能是返回1,否则返回-1 */

    int fs_Ebml_check(/* 数据长度 */unsigned long long bufferLenth, /* 数据开始地址 */unsigned char buffer[]);

    /* 
     * 计算n变长编码的长度;
     * 返回数据长度.
     */

    unsigned int fs_Ebml_numberLenth_get(unsigned long long n);

    /*
     * 获取buffer数据和头长度(第一个节点的偏移位置);
     * 成功返回头长度;
     * 失败返回-1.
     */

    int fs_Ebml_getHeadLen(/* 数据长度 */unsigned long long bufferLenth, /* 数据开始地址 */unsigned char buffer[]);


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

    FsEbml* fs_Ebml_new_from_String__IO(/* 数据长度 */unsigned long long bufferLenth, /* 数据开始地址 */const unsigned char buffer_cin[], /* 密码,为空或长度为0表示无密码 */const unsigned char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 从缓存创建一个FsEbml实例并解析;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_String_and_analyzeAll__IO(/* 数据大小,不能为0 */unsigned long long bufferLenth, /* 不能为空 */const unsigned char buffer_cin[], /* 密码,为空或长度为0表示无密码 */const unsigned char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据buffer创建一个FsEbml实例;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_String_extern__IO(/* 数据长度 */unsigned long long bufferLenth, /* 数据开始地址 */const unsigned char buffer_cin[], /* 密码,为空或长度为0表示无密码 */const unsigned char password[]
            , /* 尾部追加的大小 */ const unsigned int externSize
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 从缓存创建一个FsEbml实例并解析;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_String_extern_and_analyzeAll__IO(/* 数据大小,不能为0 */unsigned long long bufferLenth, /* 不能为空 */const unsigned char buffer_cin[], /* 密码,为空或长度为0表示无密码 */const unsigned char password[]
            , /* 尾部追加的大小 */ const unsigned int externSize
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件句柄创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml * fs_Ebml_new_from_fd__IO(FILE * const fd, /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件句柄创建一个FsEbml实例并解析;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml * fs_Ebml_new_from_fd_and_analyzeAll__IO(FILE * const fd, /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);


    /*
     * 根据文件句柄创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml * fs_Ebml_new_from_fd_extern__IO(FILE * const fd, /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 尾部追加的大小 */ const unsigned int externSize
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件句柄创建一个FsEbml实例并解析;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml * fs_Ebml_new_from_fd_extern_and_analyzeAll__IO(FILE * const fd, /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 尾部追加的大小 */ const unsigned int externSize
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件创建一个FsEbml实例并解析;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_file__IO(/* 文件名不能为空 */const char fileName[], /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_file_and_analyzeAll__IO(/* 文件名不能为空 */const char fileName[], /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_file_extern__IO(/* 文件名不能为空 */const char fileName[], /* 密码,为空或长度为0表示无密码 */const char password[], /* 尾部追加的大小 */ const unsigned int externSize
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 根据文件创建一个FsEbml实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */

    FsEbml* fs_Ebml_new_from_file_extern_and_analyzeAll__IO(/* 文件名不能为空 */const char fileName[], /* 密码,为空或长度为0表示无密码 */const char password[]
            , /* 尾部追加的大小 */ const unsigned int externSize
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

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

    /* 
     * 自动判断数据类型后转为ebml;
     * 成功返回ebml对象指针;
     * 失败返回NULL.
     */

    FsEbml *fs_Ebml_new_auto__IO(/* 在不为空时,把原始数据类型写入*rst_type中,0-ebml,1-xml,2-json */unsigned char *const rst_type, /* 数据长度,不能为0 */unsigned int dataLen, /* 数据 */ const unsigned char data_cin[]
            , /* 获取的ebml是否包含属性信息,在源数据不为ebml时才有效,0-不包含,1-包含 */const char getProperty
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 自动判断数据类型后转为ebml;
     * 成功返回ebml对象指针;
     * 失败返回NULL.
     */

    FsEbml *fs_Ebml_new_auto_from_file__IO(/* 在不为空时,把原始数据类型写入*rst_type中,0-ebml,1-xml,2-json */unsigned char *const rst_type, /* 文件名 */ const char filename_cin[]
            , /* 获取的ebml是否包含属性信息,在源数据不为ebml时才有效,0-不包含,1-包含 */const char getProperty
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 从头8字节判断数据类型后转为ebml;
     * 成功返回ebml对象指针;
     * 失败返回NULL.
     */

    FsEbml *fs_Ebml_new_from_String_with_head__IO(/* 在不为空时,把原始数据类型写入*rst_type中,0-ebml,1-xml,2-json */unsigned char *const rst_type, /* 数据长度,不能为0 */unsigned int dataLen, /* 数据 */ const unsigned char data_cin[]
            , /* 头类型,&0x9为1表示ebml,&0x9为8表示json或xml */ const unsigned char head
            , /* 获取的ebml是否包含属性信息,在源数据不为ebml时才有效,0-不包含,1-包含 */const char getProperty
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pEbml指向的实例 */

    void fs_Ebml_delete__OI(FsEbml * const pEbml
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取此EBML数据的真实长度;
     * 成功返回不小于8的数;
     * 失败返回-1; 
     */

    long long fs_Ebml_get_lenth(/* 分析的数据长度,不少于5个字节 */const unsigned long long len, /* 分析数据的开始指针 */ const unsigned char buffer[]);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */

    struct FsEbml_node* fs_Ebml_node_addChild(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node
            , /* 节点名,不能为空 */ const char nodeName[], /* 节点类型.必须为有效值,请使用宏定义 */ const FsEbmlNodeType ebmlNodeType);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点,添加到头,相比fs_Ebml_node_addChild会消耗更多的cpu;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */

    struct FsEbml_node* fs_Ebml_node_addChildHead(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node
            , /* 节点名,不能为空 */ const char nodeName[], /* 节点类型.必须为有效值,请使用宏定义 */ const FsEbmlNodeType ebmlNodeType);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点并插到index位置;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */

    struct FsEbml_node* fs_Ebml_node_addChild_index(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node, /* 插入的位置,从0开始 */const unsigned long index
            , /* 节点名,不能为空 */ const char nodeName[], /* 节点类型.必须为有效值,请使用宏定义 */ const FsEbmlNodeType ebmlNodeType);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */

    struct FsEbml_node* fs_Ebml_node_addChild_extern(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node
            , /* 节点名前缀,可为空 */const char prefix[], /* 节点名,不能为空 */ const char nodeName[], /* 节点名前缀,可为空 */const char suffix[]
            , /* 节点类型.必须为有效值,请使用宏定义 */ const FsEbmlNodeType ebmlNodeType);

    /*
     * 向pEbml中的节点pEbml_node添加一个子节点;
     * 在根节点下添加请把pEbml强制转换为FsEbml_node传入;
     * 返回添加的节点的指针.
     */

    struct FsEbml_node* fs_Ebml_node_addChild_with_a_property(FsEbml* pEbml, struct FsEbml_node* pEbml_node
            , /* 节点名,不能为空 */ const char nodeName[]
            , /* 节点类型.必须为有效值,请使用宏定义 */const FsEbmlNodeType ebmlNodeType
            , /* 属性名,不能为空 */const char propertyName[]
            , /* 属性值,可为空 */ const char propertyValue[]);


    /*
     * 向pEbml中的节点pEbml_node添加一个子节点,把addNode作为子节点拷贝插入;
     * 返回添加的节点的指针.
     */

    struct FsEbml_node* fs_Ebml_node_addChild_from_node(FsEbml * const pEbml, /* 在根节点下添加请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node
            , /* 被插入的节点 */struct FsEbml_node * const addNode);

    /* 把addNode的所有子节点拷贝添加到pEbml中的节点pEbml_node下 */

    void fs_Ebml_node_addChild_from_node_child(FsEbml * const pEbml, /* 在根节点下添加请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node
            , /* 被插入的节点 */struct FsEbml_node * const addNode);

    /*
     * 删除pEbml中的节点pEbml_node;
     * pEbml_node:不能为pEbml对象;
     * 成功返回1;
     * 失败返回-1.
     */

    int fs_Ebml_node_delete(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node);

    /*
     * 删除pEbml中的节点pEbml_node的所有子节点;
     * 成功返回1;
     * 失败返回-1.
     */

    int fs_Ebml_node_delete_child(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node);

    /*
     * 删除pEbml中的节点pEbml_node的index编号的子节点;
     * 成功返回1;
     * 失败返回-1.
     */

    int fs_Ebml_node_delete_child_index(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node, /* pEbml_node是其父节点中的第几个节点,从0开始 */ unsigned int index);


    /*
     * 删除pEbml中的节点pEbml_node的从begin开始的count个子节点;
     * 成功返回1;
     * 失败返回-1.
     */

    int fs_Ebml_node_delete_child_n(FsEbml * const pEbml, struct FsEbml_node * const pEbml_node, /* 开始位置,从0开始 */unsigned int begin, /* 删除的个数 */unsigned int count);

    /*
     * 删除pEbml中pEbml_node下的与nodeName匹配的所有节点;
     * 返回匹配的节点数.
     */

    unsigned long fs_Ebml_node_delete_byString(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /*
     * 删除pEbml中pEbml_node下的与nodeName同名的所有节点;
     * 返回匹配的节点数.
     */

    unsigned long fs_Ebml_node_delete_child_byString(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node
            , /* 节点名 */ const char childNodeName[]);

    /*
     * 删除pEbml_node下名为name的所有节点及子节点,pEbml_node必须已解析,没解析的内容不会被查找.
     */

    void fs_Ebml_node_delete_by_name(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * pEbml_node, /* 要删除的节点名 */const char name[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /*
     * 删除pEbml_node下名为name的一个节点及子节点,随机移除,pEbml_node必须已解析,没解析的内容不会被查找.
     */

    void fs_Ebml_node_delete_by_name_first(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * pEbml_node, /* 要删除的节点名 */const char name[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 删除pEbml_node下名为name的直接子节点,pEbml_node必须已解析,没解析的内容不会被查找,返回删除的节点数量 */

    unsigned int fs_Ebml_node_delete_direct_child(FsEbml * const pEbml, /* 要查找的的节点,在根节点下查找请把pEbml强制转换为FsEbml_node传入 */struct FsEbml_node * const pEbml_node, /* 要删除的节点名 */const char name[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 调整*ppEbml_node节点空间的大小 */

    void fs_Ebml_node_name_resize(FsEbml * const pEbml, /* 必须为有效值 */ struct FsEbml_node * * const ppEbml_node
            , /* 名字长度,可为0,不包含\0的长度 */ const unsigned int nameLen, /* 在父节点中的编号,-1表示未确定 */int inParentIndex);

    /* 获取pEbml_node节点的值,会清空pEbml_node节点的值,没有值返回空,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */

    FsString *fs_Ebml_node_data_get__IO(struct FsEbml_node * const pEbml_node);

    /* 获取pEbml_node节点的值,即pEbml_node->data.buffer指向的内容,会清空pEbml_node节点的值,没有值返回空,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */

    char *fs_Ebml_node_data_get_data__IO(struct FsEbml_node * const pEbml_node);

    /* 设置pEbml_node节点的值,内部释放buffer的空间,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */

    void fs_Ebml_node_data_set__OI_3(struct FsEbml_node * const pEbml_node, /* 长度,如果是字符串,应为字符串长度+1 */const unsigned long lenth, /* 可为空 */char buffer[]);

    /* 设置pEbml_node节点的值,内部释放pString的空间,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */

    void fs_Ebml_node_data_set__OI_2(struct FsEbml_node * const pEbml_node, /* 可为空 */ FsString * const pString);

    /* 设置pEbml_node节点的值,节点类型只能为FsEbmlNodeType_String或FsEbmlNodeType_Binary的子类型 */

    void fs_Ebml_node_data_set(struct FsEbml_node * const pEbml_node, /* 长度,如果是字符串,应为字符串长度+1,可为0 */const unsigned long lenth, /* 可为空 */const char buffer[]);

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

    int fs_Ebml_node_property_add__OI_2(struct FsEbml_node* pEbml_node,
            /* 要添加的属性名,属性名不能已存在,否则会出错 */ char propertyName[], /* 属性值,不能为空,支持"",内存必须是附加到propertyName后的内存 */ char propertyValue[]);

    /*
     * 在pEbml_node节点中添加一个属性,只有在添加的属性不存在时,才能用此方法;
     * pEbml_node:必须为有效值;
     * 成功返回1;
     * 失败返回-1.
     */

    int fs_Ebml_node_property_add(struct FsEbml_node* pEbml_node, /* 要添加的属性名,属性名不能已存在,否则会出错 */ const char propertyName[], /* 属性值,可为空 */ const char propertyValue[]);

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

    int fs_Ebml_node_get(/* 返回的节点添加到尾部 */FsObjectList * const rst, FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);


    /* 
     * 获取pEbml中pEbml_node下所有为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点链表指针;
     * 失败返回NULL.
     */

    FsObjectList *fs_Ebml_node_get__IO(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /* 
     * 获取pEbml中pEbml_node下所有为nodeName节点的节点的或值,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 返回或值;
     */

    unsigned long long fs_Ebml_node_get_mask(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /* 
     * 获取pEbml中pEbml_node下所有为nodeName节点名的节点,可以是非直接子节点,返回的节点不存在父子关系,匹配方式为稀疏匹配;
     * 成功返回节点链表指针;
     * 失败返回NULL.
     */

    FsObjectList *fs_Ebml_node_get_child__IO(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /* 对节点pEbml_node的已解析的子节点(可以是非直接子节点)中调用check检查,返回1的节点插入到rst_list中,不再对返回为1的节点的子节点进行匹配 */

    void fs_Ebml_node_get_node(/* 把结果指针插入此链表 */FsObjectList * const rst_list, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node
            , /* 检查函数,返回1表示此节点的指针插入rst_list,不再对本节点的子节点检查,返回-1,表示不匹配,要对子节点进行检查 */ signed char (*const check) (struct FsEbml_node * const pEbml_node));

    /* 
     * 在已解析的节点pEbml_node下查找nodeNameArray列出的各节点的指针,每个节点名只查找第一个匹配的,并把匹配的指针写入nodeNameArray中;
     * 成功返回找到的nodeNameArray的个数,对未匹配的节点,对应的位置指针置0;
     */

    unsigned int fs_Ebml_node_get_array(/* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,只支持一级节点查找 */ const char* nodeNameArray[], /* 要查找的节点名的数量,最多64个 */unsigned char arrayCount_max64);

    /* 在已解析的节点pEbml_node下查找直接子节点的第一个下标,成功返回下标,失败返回-1 */

    int fs_Ebml_node_get_child_index_byName(/* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node * const pEbml_node
            , /* 查找的开始下标,必须为有效下标,从0开始 */ int begin, /* 查找的结束下标,必须为有效下标,从0开始 */ int end, /* 直接子节点名 */const char name[]);

    /* 在已解析的节点pEbml_node下查找直接子节点的第一个下标,成功返回找到的第一个目标,失败返回NULL */

    struct FsEbml_node * fs_Ebml_node_get_child_byName(/* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */const struct FsEbml_node * const pEbml_node
            , /* 查找的开始下标,必须为有效下标,从0开始 */unsigned int begin, /* 查找的结束下标,必须为有效下标,从0开始 */unsigned int end, /* 直接子节点名 */const char name[]);

    /* 
     * 获取pEbml中第一个名为nodeName的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */

    struct FsEbml_node * fs_Ebml_node_get_first(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /* 
     * 获取pEbml中第一个名为nodeName且值为value的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */

    struct FsEbml_node *fs_Ebml_node_get_first_with_value(FsEbml* pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[], /* 值的长度,可为0 */const unsigned long valueLen, /* 值,valueLen为0时此值可为空 */const char value[]);

    /* 
     * 获取pEbml中第一个名为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的值(字符串,空间在pEbml内部);
     * 失败返回failedValue.
     */

    const FsString* fs_Ebml_node_get_first_String(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[], /* 失败时返回的错误值 */const FsString * failedValue);

    /* 
     * 获取pEbml中第一个名为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的值(字符串,空间在pEbml内部);
     * 失败返回failedValue.
     */

    const char* fs_Ebml_node_get_first_string(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[], /* 失败时返回的错误值 */const char * failedValue);

    /* 
     * 获取pEbml中第一个名为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的值(Integer类型);
     * 失败返回failedValue.
     */

    long long fs_Ebml_node_get_first_Integer(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[], /* 失败时返回的错误值 */long long failedValue);

    /* 
     * 获取pEbml中第一个名为nodeName节点的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 成功返回节点的值(float类型);
     * 失败返回failedValue.
     */

    double fs_Ebml_node_get_first_Float(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[], /* 失败时返回的错误值 */double failedValue);

    /* 
     * 获取pEbml中第一个名为nodeName节点且nodeName中最后一个节点的属性为相应属性的节点,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;;
     * 成功返回节点的内存地址;
     * 失败返回NULL.
     */

    struct FsEbml_node *fs_Ebml_node_get_first_with_a_lastProperty(FsEbml* pEbml, struct FsEbml_node* pEbml_node, /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */const char nodeName[]
            , /* nodeName中最后一个节点的属性名,不能为空 */ const char propertyName[], /* 属性值,可为空 */ const char propertyValue[]);

    /* 检查pEbml_node1的所有子节点的值在pEbml_node2中都存在,不判断节点类型,成功返回1,失败返回-1 */

    int fs_Ebml_node_child_check_value_in(/* 可以传入FsEbml对象 */const struct FsEbml_node * const pEbml_node1, /* 可以传入FsEbml对象 */const struct FsEbml_node * const pEbml_node2);

    /* 
     * 获取pEbml中名为nodeName节点的节点的数量,支持多节点查找,用空格分开,如a b表示查找第一个a节点下的第一个b节点的值;
     * 返回找到的节点数量.
     */

    unsigned int fs_Ebml_node_get_count(FsEbml * const pEbml, /* 在哪个节点下查找,在根节下查找请把pEbml强制转换为FsEbml_node传入 */ struct FsEbml_node* pEbml_node
            , /* 节点名,多节点用空格分开,可以是多个空格,但不能以空格开头 */ const char nodeName[]);

    /*
     * 把pEbml_node的子节点转换为FsXml;
     * 成功返回FsXml的实例指针; 
     * 失败返回NULL.
     */

    FsXml * fs_Ebml_node_child_convert_to_Xml__IO(/* 可以传入FsEbml对象 */const struct FsEbml_node * const pEbml_node, /* ebml的编码方式 */ const unsigned int encodeMethod);

    /*
     * 把pEbml_node的子节点转换为FsXml;
     * 成功返回FsXml的实例指针; 
     * 失败返回NULL.
     */

    FsXml * fs_Ebml_node_child_convert_to_Xml_filter__IO(/* 可以传入FsEbml对象 */const struct FsEbml_node * const pEbml_node, /* ebml的编码方式 */ const unsigned int encodeMethod
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /*
     * 把pEbml转换为FsXml;
     * 成功返回FsXml的实例指针; 
     * 失败返回NULL.
     */

    FsXml* fs_Ebml_convert_to_Xml__IO(/* 不能为空且要已完全解析 */const FsEbml * const pEbml);

    /*
     * 把pEbml转换为FsXml;
     * 成功返回FsXml的实例指针; 
     * 失败返回NULL.
     */

    FsXml* fs_Ebml_convert_to_Xml_filter__IO(/* 不能为空且要已完全解析 */const FsEbml * const pEbml
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /*
     * 把pEbml转换为FsJson;
     * 成功返回FsJson的实例指针; 
     * 失败返回NULL.
     */

    FsJson* fs_Ebml_convert_to_Json__IO(/* 不能为空且要已完全解析 */const FsEbml * const pEbml);

    /*
     * 把pEbml转换为FsJson;
     * 成功返回FsJson的实例指针; 
     * 失败返回NULL.
     */

    FsJson* fs_Ebml_convert_to_Json_filter__IO(/* 不能为空且要已完全解析 */const FsEbml * const pEbml
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /*
     * 把pEbml存放在-个FsStringBuilder中,顺序的反的;
     * 返回FsStringBuilder对象.
     */

    FsStringBuilder *fs_Ebml_to_StringBuilder__IO(const FsEbml * const pEbml);

    /*
     * 把pEbml存放在-个FsStringBuilder中,顺序的反的;
     * 返回FsStringBuilder对象.
     */

    FsStringBuilder *fs_Ebml_to_StringBuilder_filter__IO(const FsEbml * const pEbml
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /*
     * 把pEbml转换成-个Buffer;
     * 返回FsString对象;
     * prefixCount:实际数据开始位置距离返回结果的buffer头的字节数;
     * suffixCount:实际数据结束位置距离返回结果的buffer尾的字节数;
     * password:密码,为空或长度为0表示不加密.
     */

    FsString *fs_Ebml_to_String__IO(const FsEbml * const pEbml, const unsigned int prefixCount, const unsigned int suffixCount, const char password[]);


    /*
     * 把pEbml转换成-个Buffer;
     * 返回FsObjectBase对象.
     */

    FsObjectBase *fs_Ebml_to_ObjectBase__IO(const FsEbml * const pEbml, /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount, /* 密码,为空或长度为0表示不加密 */const char password[]);

    /*
     * 把pEbml转换成-个Buffer;
     * 返回FsObjectBase对象.
     */

    FsObjectBase *fs_Ebml_to_ObjectBase_with_buffer__IO(const FsEbml * const pEbml
            , /* 原缓存,不为空,在(*pObjectBaseBuffer)为空时,会把当前返回的对象的_unowned置1并设置到(*pObjectBaseBuffer)中,在(*pObjectBaseBuffer)->referCount为0时返回的数据会使用缓存 */
            FsObjectBase* * const pObjectBaseBuffer, /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount, /* 密码,为空或长度为0表示不加密 */const char password[]);


    /* 
     * 把pEbml数据写入文件流fd中;
     * 成功返回1;
     * 如果写文件失败返回-2.
     */

    int fs_Ebml_out(const FsEbml * const pEbml, FILE * const fd, /* 密码,为空或长度为0表示不加密 */const char password[]);

    /* 
     * 把pEbml数据写入文件流fd中;
     * 成功返回1;
     * 如果写文件失败返回-2.
     */

    int fs_Ebml_out_filter(const FsEbml * const pEbml, FILE * const fd, /* 密码,为空或长度为0表示不加密 */const char password[]
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /*
     * 把pEbml保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */

    int fs_Ebml_save_to_file(const FsEbml* pEbml, /* 文件名 */ const char fileName[], /* 密码,为空或长度为0表示不加密 */const char password[]);

    /*
     * 把pEbml保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */

    int fs_Ebml_save_to_file_filter(const FsEbml* pEbml, /* 文件名 */ const char fileName[], /* 密码,为空或长度为0表示不加密 */const char password[]
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /*
     * 把pEbml保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */

    int fs_Ebml_save_to_file_custom(/* 已格式化 */const FsEbml* pEbml, /* 文件名 */ const char fileName[], /* 输出方式,0-ebml,1-xml,2-xml去注释,3-json */const unsigned char outType);

    /*
     * 把pEbml保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */

    int fs_Ebml_save_to_file_custom_filter(/* 已格式化 */const FsEbml* pEbml, /* 文件名 */ const char fileName[], /* 输出方式,0-ebml,1-xml,2-xml去注释,3-json */const unsigned char outType
            , /* 过滤器,返回-1表示不要,返回1表示节点需要,返回3表示节点需要且子节点也需要无须匹配 */int (* const filter) (struct FsEbml_node *pEbml_node));

    /* 
     * 获取所有已解析节点的值并储存在连续内存中;
     * 成功返回FsString指针对象;
     * 失败返回NULL.   
     */

    FsString *fs_Ebml_value_all_get__IO(const FsEbml* pEbml);

    /* 把pEbml调试数据写入文件流fd中 */

    void fs_Ebml_out_debug(FsEbml * const pEbml, FILE * const fd
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);



    void fs_Ebml_test();

#ifdef __cplusplus
}
#endif

#endif/* EBML_H */

