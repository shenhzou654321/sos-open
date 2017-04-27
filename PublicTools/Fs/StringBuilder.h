/* 
 * File:   StringBuilder.h
 * Author: fslib
 *
 * Created on 2013年1月8日, 上午9:37
 */

#ifndef STRINGBUILDER_H
#define	STRINGBUILDER_H
#include "../publicDefine.h"
#include <stdio.h>
#include "ObjectList.h"
#include "String.h"
#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        unsigned long lenth; /* 字符串总长 */
        FsObjectList* list; /* 字符串列表 */
    } FsStringBuilder;
    /* 
     * 创建一个FsStringBuilder实例;
     * 返回创建的实例指针.
     */
    FsStringBuilder* fs_StringBuilder_new__IO();
    /* 删除pStringBuilder指向的实例对象 */
    void fs_StringBuilder_delete__OI(FsStringBuilder* pStringBuilder);
    /* 删除pStringBuilder指向的实例对象,并把数据合并后返回 */
    FsString* fs_StringBuilder_delete__IO__OI(FsStringBuilder* pStringBuilder);
    /*
     * 把string拷贝一份插到pStringBuilder尾部;
     * string不能为空;
     * 如果成功返回1;
     * 如果失败返回-1.
     */
    char fs_StringBuilder_apend(FsStringBuilder* pStringBuilder, const char string[]);
    /*
     * 把string插到pStringBuilder尾部;
     * string不能为空;
     * 如果成功返回1;
     * 如果失败返回-1.
     */
    signed char fs_StringBuilder_apend__OI_2(FsStringBuilder * const pStringBuilder, const char string[]);
    /*
     * 把pString插到pStringBuilder尾部;
     * 如果成功返回1;
     * 如果失败返回-1.
     */
    char fs_StringBuilder_apend_String__OI_2(FsStringBuilder* __restrict pStringBuilder, /* 不能为空 */ FsString* __restrict pString);
    /*
     * 把buffer插到pStringBuilder尾部;
     * 如果成功返回1;
     * 如果失败返回-1.
     */
    char c5aii_StringBuilder_apend_String__OI_3(FsStringBuilder* pStringBuilder, /* buffer长度,不能为0 */const unsigned long bufferLenth, /* 不能为空 */char* __restrict buffer);
    /*
     * 把buffer插到pStringBuilder尾部;
     * buffer不能为空;
     * bufferLenth必须大于0;
     * 如果成功返回1;
     * 如果失败返回-1;
     */
    char fs_StringBuilder_apend_string(FsStringBuilder* pStringBuilder, const unsigned long bufferLenth, const char buffer[]);
    /* 把pStringBuilder合并,必须保证pStringBuilder->lenth>0 */
    void fs_StringBuilder_to_String(const FsStringBuilder* pStringBuilder, /* 合并后的数据存放的空间,必须有足够的空间 */ char buffer[]);
    /*
     * 把pStringBuilder合并成一个FsString;
     * 如果pStringBuilder->lenth为0，则返回NULL;
     */
    FsString* fs_StringBuilder_to_String__IO(FsStringBuilder* pStringBuilder);
    /* 把pStringBuilder倒序合并,必须保证pStringBuilder->lenth>0 */
    void fs_StringBuilder_to_String_reverse(const FsStringBuilder* pStringBuilder, /* 合并后的数据存放的空间,必须有足够的空间 */ char buffer[]);
    /* 把pStringBuilder倒序合并,并加密编码,仅把编码部分写入,必须保证pStringBuilder->lenth>0 */
    void fs_StringBuilder_to_String_encryption_reverse(const FsStringBuilder* pStringBuilder, /* 合并后的数据存放的空间,必须有足够的空间 */char buffer[],
            /* 密码,长度不能为0 */const unsigned char password[],
            /* 需要加密编码的开始下标,从0开始,必须为有效值,包含本身 */const unsigned long begin,
            /* 需要加密编码的结束下标,从0开始,必须为有效值,包含本身 */unsigned long end);
    /* 把pStringBuilder倒序合并,并加密编码,仅把编码部分写入,返回编码区的原校验和,必须保证pStringBuilder->lenth>0 */
    unsigned char fs_StringBuilder_to_String_encryption_reverse_get_sourceCheck(const FsStringBuilder* pStringBuilder, /* 合并后的数据存放的空间,必须有足够的空间 */char buffer[],
            /* 密码,长度不能为0 */const unsigned char password[],
            /* 需要加密编码的开始下标,从0开始,必须为有效值,包含本身 */const unsigned long begin,
            /* 需要加密编码的结束下标,从0开始,必须为有效值,包含本身 */unsigned long end);
    /*
     * 把pStringBuilder倒序合并成一个FsString;
     * 如果pStringBuilder->lenth为0，则返回NULL;
     */
    FsString* fs_StringBuilder_to_String_reverse__IO(const FsStringBuilder* pStringBuilder);
    /*
     * 把pStringBuilder写入文件流fd中;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */
    signed char fs_StringBuilder_out(const FsStringBuilder* pStringBuilder, FILE *fd);
    /*
     * 把pStringBuilder保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    signed char fs_StringBuilder_save_to_file(const FsStringBuilder* pStringBuilder, /* 文件名,不能为空 */ const char fileName[]);

    /*
     * 把pStringBuilder倒序写入文件流fd中;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */
    signed char fs_StringBuilder_out_reverse(const FsStringBuilder* pStringBuilder, FILE *fd);
    /*
     * 把pStringBuilder倒序写入文件流fd中,并加密编码,仅把编码部分写入流;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */
    signed char fs_StringBuilder_out_encryption_reverse(const FsStringBuilder* pStringBuilder, FILE *fd, /* 密码,长度不能为0 */const unsigned char password[],
            /* 需要加密编码的开始下标,从0开始,必须为有效值,包含本身 */const unsigned long begin,
            /* 需要加密编码的结束下标,从0开始,必须为有效值,包含本身 */unsigned long end);
    /*
     * 把pStringBuilder倒序写入文件流fd中,并加密编码,仅把编码部分写入流,并计算编码区的原数据校验和;
     * 如果成功返回1;
     * 如果写文件失败返回-2.
     */
    signed char fs_StringBuilder_out_encryption_reverse_get_sourceCheck(/* 编码区的原数据校验和 */unsigned char *check, const FsStringBuilder* pStringBuilder, FILE *fd,
            /* 密码,长度不能为0 */const unsigned char password[], /* 需要加密编码的开始下标,从0开始,必须为有效值,包含本身 */const unsigned long begin,
            /* 需要加密编码的结束下标,从0开始,必须为有效值,包含本身 */unsigned long end);
    /*
     * 把pStringBuilder倒序保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    char fs_StringBuilder_save_to_file_reverse(const FsStringBuilder* pStringBuilder, const char fileName[]);
    /*
     * 把pObjectList部分合并成一个FsString;
     * 有内容,则返回FsString对象指针;
     * 无内容,则返回NULL.
     */
    FsString* fs_StringBuilder_to_String_part__IO(/* 数据存放的链表,成员只能是FsString的实例 */const FsObjectList* pObjectList, /* 开始索引 */const unsigned long startIndex, /* 结束索引 */const unsigned long endIndex);

#ifdef	__cplusplus
}
#endif

#endif	/* STRINGBUILDER_H */

