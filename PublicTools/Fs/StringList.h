/* 
 * File:   StringList.h
 * Author: RuiXue
 *
 * Created on 2013年3月5日, 下午8:47
 */
/*
 * 以字符串为主体,实现多字符串的添加,排序等
 */
#ifndef STRINGLIST_H
#define STRINGLIST_H

#include "ObjectList.h"
#include "String.h"


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        /* 数据保存链表 */
        FsObjectList list;
        /* 数据总长度 */
        unsigned long Lenth;
    } FsStringList;

    /*
     * 创建一个FsStringList实例;
     * 返回创建的实例指针.
     */
    FsStringList* fs_StringList_new__IO();
    /*
     * 复制pStringList数据创建一个FsStringList实例;
     * 返回创建的实例指针.
     */
    FsStringList* fs_StringList_copy__IO(const FsStringList * const pStringList);
    /* 删除pStringList指向的实例对象 */
    void fs_StringList_delete__OI(FsStringList* pStringList);
    /*
     * 把str拷贝一份插入到pStringList的头部;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_head(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */const char str[]);
    /*
     * 把str插入到pStringList的头部;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_head__OI_2(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */ char str[]);
    /*
     * 把pInsertList中的节点插到pStringList的头部,并删除pInsertList;
     * 成功返回1;
     * 失败返回-1
     */
    char fs_StringList_insert_head_all__OI_2(FsStringList* pStringList, FsStringList* pInsertList);
    /*
     * 把str地址直接插入到pStringList的position位置;
     * 成功返回1;
     * 失败返回-1
     */
    char fs_StringList_insert_at__OI_3(FsStringList* pStringList, /* 从0开始,必需为有效值,否则程序会异常 */unsigned long position, /* 要插入的字符串,不能为空 */char str[]);
    /*
     * 把str拷贝一份插入到pStringList的position位置;
     * 成功返回1;
     * 失败返回-1.
     */
    char fs_StringList_insert_at(FsStringList* pStringList, /* 从0开始,必需为有效值,否则程序会异常 */unsigned long position, /* 要插入的字符串,不能为空 */ const char str[]);
    /*
     * 把str插入到pStringList的尾部;
     * 成功返回1;
     * 失败返回-1.
     */
    char fs_StringList_insert_tail__OI_2(FsStringList* pStringList, /* 要插入的字符串,不能为空 */char str[]);
    /*
     * 把str按顺序插入到pStringList;
     * 成功返回插入的位置索引,从0开始;
     * 失败返回-1.
     */
    long fs_StringList_insert_order(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */const char str[]
            , /* 排序的比较函数,数据按比较结果从小到大排序:
         *     str1>str2返回1;
         *     str1=str2返回0
         *     否则返回-1;
         */int (*const orderCompare) (const char str1[], const char str2[]));
    /*
     * 把str按顺序插入到pStringList;
     * 成功返回插入的位置索引,从0开始;
     * 失败返回-1.
     */
    long fs_StringList_insert_order2(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */const char str[], /* 数据长度 */const unsigned long strlen
            , /* 排序的比较函数,数据按比较结果从小到大排序:
             *     str1>str2返回1;
             *     str1=str2返回0
             *     否则返回-1;
             */int (*const orderCompare) (const char str1[], const unsigned long str1len, const char str2[], void *const p), /* 比较时传入orderCompare函数的指针 */void *const p);

    /*
     * 把str按顺序插入到pStringList;
     * 成功返回插入的位置索引,从0开始;
     * 失败返回-1.
     */
    long fs_StringList_insert_order__OI_2(FsStringList* pStringList, /* 要插入的字符串,不能为空 */const char str[]
            , /* 排序的比较函数,数据按比较结果从小到大排序:
         *     str1>str2返回1;
         *     str1=str2返回0
         *     否则返回-1;
         */int (*const orderCompare) (const char str1[], const char str2[]));
    /*
     * 把str拷贝一份插入到pStringList的尾部;
     * 成功返回1;
     * 失败返回-1.
     */
    int fs_StringList_insert_tail(FsStringList * const pStringList, /* 字符串长度,不含\0 */const unsigned long strlen, /* 要插入的字符串,不能为空 */const char str[]);
    /*
     * 把str拷贝一份插入到pStringList的尾部;
     * 成功返回1;
     * 失败返回-1.
     */
    int fs_StringList_insert_tail_string(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */const char str[]);
    /*
     * 把pInsertList中的节点插到pStringList的尾部,并删除pInsertList;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_tail_all__OI_2(FsStringList * const pStringList, const FsStringList * const pInsertList);
    /*
     * 把pInsertList中的节点插到pStringList的尾部,只插入pStringList中没有的部分,并删除pInsertList;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_tail_all_unique__OI_2(FsStringList * const pStringList, FsStringList * const pInsertList);
    /*
     * 把str正序插入到pStringList中;
     * 使用此函数所有的节点加入pStringList都必须使用此方法,否则得不到正确结果;
     * 成功返回1;
     * 已在在,未插入返回0;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_ASC__OI_2(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */ char str[]);
    /*
     * 把str拷贝一份正序插入到pStringList中;
     * 使用此函数所有的节点加入pStringList都必须使用此方法,否则得不到正确结果;
     * 成功返回1;
     * 已在在,未插入返回0;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_ASC(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */const char str[]);

    /*
     * 把str倒序插入到pStringList中;
     * 使用此函数所有的节点加入pStringList都必须使用此方法,否则得不到正确结果;
     * 成功返回1;
     * 已在在,未插入返回0;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_DESC__OI_2(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */ char str[]);
    /*
     * 把str拷贝一份倒序插入到pStringList中;
     * 使用此函数所有的节点加入pStringList都必须使用此方法,否则得不到正确结果;
     * 成功返回1;
     * 已在在,未插入返回0;
     * 失败返回-1.
     */
    signed char fs_StringList_insert_DESC(FsStringList * const pStringList, /* 要插入的字符串,不能为空 */const char str[]);
    /* 在pStringList获取索引为index的值 */
    char* fs_StringList_get_at(FsStringList* pStringList, /* 从0开始,必需为有效值,否则程序会异常 */ unsigned long index);
    /* 
     * 在pStringList中的所有节点中查找是否有与str的内存相同的节点;
     * 成功返回第一个节点索引,从0开始;
     * 失败返回-1;
     */
    long fs_StringList_check_has(const FsStringList * const pStringList, const char str[]);
    /*
     * 移除pStringList中的第1个节点,返回移除了的节点指针;
     * 如果pStringList节点个数为0,程序会出错;
     * 调用此函数前务必对pStringList的节点个数进行检查.
     */
    char* fs_StringList_remove_head__IO(FsStringList * const pStringList);
    /*
     * 移除pStringList中的第最后一个节点,返回移除了的节点指针;
     * 如果pStringList节点个数为0,程序会出错;
     * 调用此函数前务必对pStringList的节点个数进行检查.
     */
    char* fs_StringList_remove_tail__IO(FsStringList * const pStringList);
    /*
     * 移除pStringList中的第index个结点,从0开始返回移除了的节点指针;
     * 如果i大于pObjectList的长度,程序会出错;
     * 调用此函数前务必对pObjectList的长度进行检查.
     */
    char* fs_StringList_remove_at__IO(FsStringList * const pStringList, const unsigned long index);
    /*
     * 把pStringList中的节点转为一个字符串输出;
     * 节点个数为0,返回NULL;
     * 成功返回字符串指针;
     * 失败返回NULL.
     */
    char *fs_StringList_to_string__IO(/* 不能为空 */const FsStringList* pStringList, /* 如有多条数据,则在每条数据间插入此值,NULL表示不插入 */const char divideString[]);
    /*
     * 把pStringList中的节点转为一个字符串输出;
     * 节点个数为0,返回NULL;
     * 成功返回FsString指针;
     * 失败返回NULL.
     */
    FsString *fs_StringList_to_String__IO(/* 不能为空 */const FsStringList* pStringList, /* 如有多条数据,则在每条数据间插入此值,NULL表示不插入 */const char divideString[],
            /* 实际数据开始位置距离返回结果的buffer头的字节数 */const unsigned int prefixCount, /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned int suffixCount);

    /*
     * 把pStringList输出到文件流中;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */
    signed char fs_StringList_out(/* 不能为空 */const FsStringList * const pStringList, FILE * const fd, /* 前缀,可为空 */const char prefix[], /* 后缀,可为空 */const char suffix[]);
    /*
     * 把pStringList保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    char fs_StringList_save_to_file(/* 不能为空 */const FsStringList* pStringList, /* 文件名,不能为空 */ const char fileName[], /* 前缀,可为空 */const char prefix[], /* 后缀,可为空 */const char suffix[]);
#ifdef __cplusplus
}
#endif

#endif /* STRINGLIST_H */

