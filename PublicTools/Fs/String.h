/* 
 * File:   String.h
 * Author: fslib
 *
 * Created on 2013年1月8日, 上午9:24
 */

#ifndef STRING_H
#define	STRING_H
#include "../publicDefine.h"
#include <stdio.h>
#include "ObjectList.h"
#include "Object.h"
#ifdef	__cplusplus
extern "C" {
#endif
#define FsMd5CheckSumEqual(checkSum1,checkSum2) ((checkSum1)[0] == (checkSum2)[0] &&(checkSum1)[1] == (checkSum2)[1] &&(checkSum1)[2] == (checkSum2)[2] &&(checkSum1)[3] == (checkSum2)[3])

    /* 字符串编码类型 */
    typedef enum {
        FsStringCode_Unkown = 0,
#define FsStringCode_Default FsStringCode_Unkown
        /* ASCII */
        FsStringCode_ASCII = 1,
        /* UTF-8 */
        FsStringCode_UTF8 = 2,
        /* GBK */
        FsStringCode_GBK = 3,
    } FsStringCode;

    /* 此对象的指针使用malloc直接分配,可在外部操作,一般不推荐使用 */
    typedef struct {
        char* buffer;
        unsigned long lenth;
    } FsString;

    /* 继承FsObject的String数据 */
    typedef struct {
        FsObject object;
        /* 数据长度 */
        unsigned long lenth;
        /* 数据区,内存大小不可直接修改,必须用函数操作 */
        char* buffer;
    } FsObjectString;
    /*
     * 创建一个FsString实例;
     * bufferLenth:创建时分配的空间大小;
     * 返回创建的实例指针.
     */
    FsString* fs_String_new__IO(const unsigned long bufferLenth);

    /*
     * 把buffer拷备一份保存在FsString中;  
     * buffer:可为空;
     * bufferLenth:buffer的长度,可为0;
     * 返回FsString指针.
     */
    FsString* fs_String_new_form_stringBuffer__IO(const char buffer[], const unsigned long bufferLenth);
    /*
     * 把buffer保存在FsString中;  
     * 返回FsString指针.
     */
    FsString* fs_String_new_form_stringBuffer__IO__OI(/* 可为空,建议在有效数据后加一个0位 */const char buffer[], /* buffer的长度,可为0 */const unsigned long bufferLenth);
    /*
     * 把string拷备一份保存在FsString中;
     * string:不能为空;
     * 返回FsString指针.
     */
    FsString* fs_String_new_form_string__IO(const char string[]);
    /*
     * 打开文件,把文件内容保存在FsString中;
     * 成功返回FsString指针;
     * 失败返回NULL;
     * fileName:文件名,不能为空.
     */
    FsString* fs_String_new_form_file__IO(const char fileName[]);
    /*
     * 打开文件,把文件内容保存在FsString中;
     * 成功返回FsString指针;
     * 失败返回NULL.
     */
    FsString* fs_String_new_form_file2__IO(/* 目录名不能为空 */const char dir[], /* 文件名,不能为空 */const char fileName[]);
    /*
     * 把文件句柄内容保存在FsString中;
     * 成功返回FsString指针;
     * 失败返回NULL.
     */
    FsString* fs_String_new_form_fd__IO(/* 文件句柄,不能为空 */FILE * const fd);
    /*
     * 打开文件,把文件内容保存在FsString中;
     * 成功返回FsString指针;
     * 失败返回NULL;
     * fileName:文件名,不能为空;
     * prefixCount:实际数据开始位置距离返回结果的buffer头的字节数;
     * suffixCount:实际数据结束位置距离返回结果的buffer尾的字节数.
     */
    FsString* fs_String_new_form_file_custom__IO(const char fileName[], const unsigned int prefixCount, const unsigned int suffixCount);
    /* 把pObjectList的FsString成员合并为一个buffer,返回合并后的buffer指针,长度为0返回NULL */
    FsString* fs_String_new_form_list__IO(/* 成员全是FsString,不能为空 */const FsObjectList * const pObjectList);

    /* 删除pString指向的实例对象 */
    void fs_String_delete__OI(FsString* pString);

    /*
     * 删除pString指向的实例对象;
     * 当pString->lenth>0时,返回pString->buffer,否则返回NULL.
     */
    char* fs_String_delete__OI__IO(FsString* pString);

    /*
     * 复制一份pString;
     * 返回复制后的指针;
     * pString:不能为空.
     */
    FsString* fs_String_copy__IO(const FsString* pString);
    /*
     * 把buffer保存在文件中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    signed char fs_String_buffer_save_to_file(/* 要保存的数据长度,必须大于0 */const unsigned long bufferLenth, /* 要保存的数据开始地址 */const char buffer[], /* 文件名 */const char filename[]);
    /*
     * 把pString保存在文件中;
     * 如果成功返回1;
     * 无需保存返回0;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    char fs_String_save_to_file(FsString* pString, /* 文件名 */const char fileName[]);
    /* 
     * 检查字符串的的编码;
     * bufferLen:buffer长度,为0返回FsStringCode_ASCII;
     * buffer:字符串开始位置.
     */
    FsStringCode fs_String_buffer_check_code(unsigned long bufferLen, const unsigned char buffer[]);
    /* 
     * 检查pString的编码;
     * 失败返回0;
     */
    FsStringCode fs_String_check_code(const FsString* pString);
    /*
     * 把srcBuffer用dstCode编码;
     * 成功返回编码后的pString指针;
     * 失败返回NULL;
     */
    FsString* fs_String_buffer_code__IO(/* buffer长度,不能为0 */const unsigned long bufferLen, /* 字符串开始位置 */ const char srcBuffer[],
            /* 为FsStringCode_Unkown表示自动检测 */FsStringCode srcCode, /* 只支持FsStringCode_UTF8和FsStringCode_GBK */const FsStringCode dstCode);
    /*
     * 把pString用dstCode编码；
     * 成功返回编码后的pString指针；
     * 失败返回NULL;
     * srcCode:为FsStringCode_Unkown表示自动检测;  
     * dstCode:只支持FsStringCode_UTF8和FsStringCode_GBK.
     */
    FsString* fs_String_code__IO(const FsString* pString, FsStringCode srcCode, const FsStringCode dstCode);
    /* 获取字符串中的字符个数 */
    unsigned long fs_String_buffer_get_chars(/* buffer长度,为0返回0 */unsigned long bufferLen, /* 字符串开始位置 */const unsigned char buffer[], /* 为FsStringCode_Unkown表示自动检测 */FsStringCode srcCode);
    /* 获取字符串占的字符宽度,汉字占两个字符位,英文占一个字符位 */
    unsigned long fs_String_buffer_get_chars_width(/* buffer长度,为0返回0 */unsigned long bufferLen, /* 字符串开始位置 */const unsigned char buffer[], /* 为FsStringCode_Unkown表示自动检测 */FsStringCode srcCode);
    /*
     * 把buffer用预定义Base64表进行编码;
     * 成功返回编码后的FsString指针;
     * 函数不会失败.
     */
    FsString* fs_String_buffer_encode_by_base64__IO(/* 数据开始指针位置 */const unsigned long bufferLen, /* 数据长度,必须大于0 */const char buffer[]);
    /*
     * 把pString用预定义Base64表进行编码;
     * 成功返回编码后的FsString指针,支持打印;
     * 函数不会失败,当传入数据长度为0时返回NULL.
     */
    FsString* fs_String_encode_by_base64__IO(const FsString* pString);
    /*
     * 把buffer用特定的Base64表进行编码;
     * 函数不会失败,返回编码后的长度.
     */
    unsigned long fs_String_buffer_encode_by_base64_custom_from_string(/* 编码结果储存的空间,必须足够大 */unsigned char rstBuffer[],
            /* 数据长度,必须大于0 */const unsigned long bufferLen, /* 数据开始指针位置 */const char buffer[], /* base64的编码码表,不能为空 */const unsigned char base64[]);
    /*
     * 把buffer用预定义的Base64表进行编码;
     * 函数不会失败,返回编码后的长度.
     */
    unsigned long fs_String_buffer_encode_by_base64(/* 解码结果储存的空间,必须足够大 */unsigned char rstBuffer[],
            /* 数据长度,必须大于0 */const unsigned long bufferLen, /* 数据开始指针位置 */const char buffer[]);
    /*
     * 把buffer用特定的Base64表进行编码;
     * 函数不会失败,返回编码后的FsString指针;
     * bufferLen:数据长度,必须大于0;
     * buffer:数据开始指针位置;
     * base64:base64的编码码表,不能为空.
     */
    FsString* fs_String_encode_by_base64_custom_from_string__IO(const unsigned long bufferLen, const char buffer[], const unsigned char base64[]);
    /*
     * 把pString用特定的Base64表进行编码; 
     * 函数不会失败,当传入数据长度为0时返回NULL;
     * 否则返回编码后的FsString指针;
     * base64:base64的编码码表,不能为空;
     */
    FsString* fs_String_encode_by_base64_custom__IO(const FsString* pString, const unsigned char base64[]);
    /*
     * 用预定义的Base64表解码Base64编码的buffer;
     * 成功返回解码后的长度;
     * 失败返回-1;
     */
    long fs_String_buffer_decode_from_base64(/* 储存解码结果的指针,大小 */char rst_buffer[], /* 解码的数据长度,必须大于0 */unsigned long bufferLen, /* 解码的buffer */const char buffer[]);
    /*
     * 用预定义的Base64表解码Base64编码的buffer;
     * 成功返回解码后的FsString指针;
     * 失败返回NULL;
     * bufferLen:数据长度,必须大于0;
     * buffer:数据开始指针位置.
     */
    FsString* fs_String_decode_from_base64_from_string__IO(unsigned long bufferLen, const char buffer[]);
    /*
     * 用预定义的Base64表解码Base64编码的pString;
     * 成功返回解码后的pString指针;
     * 失败返回NULL.
     */
    FsString* fs_String_decode_from_base64__IO(const FsString* pString);
    /*
     * 用特定的Base64表解码Base64编码的pString;
     * 成功返回解码后的长度;
     * 失败返回-1;
     * rstBuffer:解码结果储存的空间,必须足够大;
     * base64:base64的解码表,不能为空.
     */
    long fs_String_decode_from_base64_custom(unsigned char rstBuffer[], const FsString* pString, const unsigned char base64[]);
    /*
     * 用特定的Base64表解码Base64编码的pString;
     * 成功返回解码后的FsString指针;
     * 失败返回NULL;
     * base64:base64的解码表,不能为空.
     */
    FsString* fs_String_decode_from_base64_custom__IO(const FsString* pString, const unsigned char base64[]);
    /*
     * 查找c在buffer中出现的第一个位置；
     * 成功返回c的索引；
     * 失败返回-1.
     */
    long fs_String_buffer_indexOf(const unsigned long bufferLenth, const char buffer[], const char c);
    /*
     * 查找findBuffer在srcBuffer中出现的第一个位置;
     * 成功返回findBuffer的索引;
     * 失败返回-1.
     */
    long fs_String_buffer_indexOf_stringBuffer(unsigned long srcBufferLenth, const char srcBuffer[], const unsigned long findBufferLenth, const char findBuffer[]);
    /*
     * 查找findString在srcBuffer中出现的第一个位置;
     * 成功返回findBuffer的索引;
     * 失败返回-1.
     */
    long fs_String_buffer_indexOf_string(unsigned long srcBufferLenth, const char srcBuffer[], /* 不能为空 */const char findString[]);
    /*
     * 查找c在buffer中出现的最后一个位置;
     * 成功返回c的索引;
     * 失败返回-1.
     */
    long fs_String_buffer_lastIndexOf(unsigned long bufferLenth, const char buffer[], const char c);
    /*
     * 查找str在buffer中出现的最后一个位置;
     * 成功返回str的索引;
     * 失败返回-1.
     */
    long fs_String_lastIndexOfString(const unsigned long bufferLenth, char buffer[], const char str[]);
    /*
     * 查找findBuffer在srcBuffer中出现的第一个位置;
     * 成功返回指针;
     * 失败返回NULL.
     */
    char* fs_String_buffer_posOf_buffer(unsigned long srcBufferLenth, char srcBuffer[], /* 要查找的字符串长度 */const unsigned long findBufferLenth, /* 不能为空 */const char findBuffer[]);
    /*
     * 查找findString在srcBuffer中出现的第一个位置;
     * 成功返回指针;
     * 失败返回NULL.
     */
    char* fs_String_buffer_posOf_string(unsigned long srcBufferLenth, const char srcBuffer[], /* 不能为空 */const char findString[]);
    /* 
     * 从pString中的b位置开始截取长度为len的串;
     * 返回截取到的串;
     * b和len必须为有效且合理的值.
     */
    FsString* fs_String_sub__IO(const FsString* pString, const unsigned long b, const unsigned long len);
    /* 
     * 判断buffer是否是以str开始;
     * 是则返回1;
     * 不是返回-1;
     * buffer和str不能为空.
     */
    char fs_String_buffer_beginWith_by_String(/* buffer长度,不能为0 */const unsigned long bufferLenth, char buffer[], /* str长度,不能为0 */ unsigned long strLenth, char str[]);
    /* 
     * 判断buffer是否是以str开始;
     * 是则返回1;
     * 不是返回-1;
     */
    char fs_String_buffer_beginWith(/* buffer长度,不能为0 */const unsigned long bufferLenth, /* 不能为空 */char buffer[], /* 不能为空 */ char str[]);
    /* 
     * 判断pString是否是以str开始;
     * 是则返回1;
     * 不是返回-1.
     */
    char fs_String_beginWith(/* 不能为空 */const FsString* pString, /* 不能为空 */ char str[]);
    /* 
     * 判断buffer是否是以str结束;
     * 是则返回1;
     * 不是返回-1.
     */
    char fs_String_buffer_endWith_buffer(/* buffer的长度 */const unsigned long bufferLenth, /* 可以为空 */const char buffer[], /* str的长度 */ const unsigned long strLenth, /* 不能为空 */const char str[]);
    /* 
     * 判断buffer是否是以str结束;
     * 是则返回1;
     * 不是返回-1.
     */
    char fs_String_buffer_endWith(/* buffer的长度 */const unsigned long bufferLenth, /* 可以为空 */const char buffer[], /* 不能为空 */const char str[]);
    /*
     * 判断buffer是否全是数字;
     * 全是返回1;
     * 否则返回-1.
     * 
     */
    signed char fs_String_buffer_isDigit(/* 可以为0 */unsigned char bufferLenth, /* 可以为空 */const char buffer[]);
    /*
     * 把buffer的数字转为长整形输出;
     * bufferLenth:buffer长度,为0表示buffer结尾带0;
     * buffer:字符串缓存;
     * 成功返回转换后的结果;
     * 失败返回defaultValue.
     */
    long fs_String_to_long(unsigned char bufferLenth, char buffer[], long defaultValue);
    /*
     * 判断buffer是否是IPv4地址;
     * 是返回1;
     * 否则返回-1.
     */
    char fs_String_buffer_isIPv4(/* buffer长度,可以为0 */unsigned char bufferLenth, /* 可以为空 */ char buffer[]);
    /* 
     * 把str2连接到str1后面;
     * str1与str2不能为空;
     * 返回连接后的指针.
     */
    FsString *fs_String_cat__IO(const char str1[], const char str2[]);
    /* 获取buffer的校验值,用INT表示 */
    unsigned int fs_String_buffer_intSum(/* 要校验的buffer的长度,可为0 */const unsigned long bufferLenth, /* 要校验的buffer,可为空 */char buffer[]);
    /* 获取buffer的md5校验值,结果为二进制数据 */
    void fs_String_buffer_md5sum(/* 接收结果的缓存空间,大小为16字节 */ unsigned int rst_4[], /* 要校验的buffer的长度,可为0 */ const unsigned long bufferLenth, /* 要校验的buffer,必须为4字节对齐,可为空 */ const char buffer[]);
    /* 获取buffer的md5校验值,结果为二进制数据,长度为16字节 */
    char *fs_String_buffer_md5sum__IO(/* 要校验的buffer的长度,可为0 */const unsigned long bufferLenth, /* 要校验的buffer,必须为4字节对齐,可为空 */const char buffer[]);
    /* 获取buffer的md5校验值,结果为字符串 */
    void fs_String_buffer_md5sum_string(/* 结果储存的buffer,如果rst_32[0]为1结果用大写字母表示,否则用小写字母 */char rst_32[],
            /* 要校验的buffer的长度,可为0 */const unsigned long bufferLenth, /* 要校验的buffer,必须为4字节对齐,可为空 */ const char buffer[]);
    /* 获取buffer的md5校验值,结果为字符串 */
    char *fs_String_buffer_md5sum_string__IO(/* 要校验的buffer的长度,可为0 */const unsigned long bufferLenth,
            /* 要校验的buffer,必须为4字节对齐,可为空 */ const char buffer[],
            /* 为1表示结果中的字母用大写字母表示,其它表示用小写字母表示 */const char upper);
    /* 获取buffer的md5校验值,结果为二进制数据 */
    void fs_String_md5sum(/* 接收结果的缓存空间,大小为16字节 */ unsigned int rst_4[], /* buffer必须为4字节对齐 */const FsString *pString);
    /* 获取pString的md5校验值,结果为字符串 */
    char* fs_String_md5sum_string__IO(/* buffer必须为4字节对齐 */const FsString *pString, /* 为1表示结果中的字母用大写字母表示,其它表示用小写字母表示 */const char upper);
    /* 获取buffer的sha256校验值,结果为二进制数据 */
    void fs_String_buffer_sha256(/* 接收结果的缓存空间,大小为32字节 */unsigned int rst_8[], /* 要校验的buffer的长度,不可为0 */ const unsigned long bufferLenth, /* 要校验的buffer,必须为4字节对齐,不可为空 */ const unsigned char buffer[]);
    /* 获取buffer的hmacsha256校验值,结果为二进制数据 */
    void fs_String_buffer_hmacsha256(/* 接收结果的缓存空间,大小为32字节 */unsigned int rst_8[], /* 加密key,必须为4字节对齐 */const unsigned char key[], /* 加密key的长度,不可为0 */const unsigned int keylen,
            /* 要校验的buffer的长度,不可为0 */ const unsigned long bufferLenth, /* 要校验的buffer,必须为4字节对齐,不可为空 */ const unsigned char buffer[]);
    /* 
     * 比较buf1与buf2的大小;
     * 完全相等返回0;
     * 长度都为0返回0;
     * buf1>buf2返回1;
     * buf1<buf2返回-1.
     */
    signed char fs_String_buffer_cmp(const char buf1[], unsigned long buf1_lenth, const char buf2[], unsigned long buf2_lenth);
    /* 
     * 比较pString1与pString2的大小,都不能为空;
     * 完全相等返回0;
     * pString1>pString2返回1;
     * pString1<pString2返回-1.
     */
    signed char fs_String_cmp(const FsString * const pString1, const FsString * const pString2);
    //    /*
    //     * 把buffer用rsa算法加密;
    //     * 把结果(二进制数据)写入rstBuffer,长度必须足够大;
    //     * 返回写入数据的字节数.
    //     */
    //    unsigned long c5aii_String_buffer_encode_from_rsa__(char rstBuffer[], const unsigned long bufferlen, const char buffer[], /* 加密版本0-255 */unsigned char editon);
    //    /*
    //     * 把buffer用rsa算法加密;
    //     * 返回加密后的数据(二进制数据).
    //     */
    //    C5aiiString *c5aii_String_buffer_encode_from_rsa__IO(const unsigned long bufferlen, const char buffer[], /* 加密版本0-255 */unsigned char editon);
    //    /*
    //     * 把pString用rsa算法加密;
    //     * 返回加密后的数据(二进制数据).
    //     */
    //    C5aiiString *c5aii_String_encode_from_rsa__IO(const C5aiiString *pString, /* 加密版本0-255 */unsigned char editon);
    /* 把buffer用Base64编码后输出到fd中 */
    void fs_String_buffer_out(const unsigned long bufferLenth, const char buffer[], FILE *fd);
    /* 把pString用Base64编码后输出到fd中 */
    void fs_String_out(const FsString *pString, FILE *fd);
    /* 把buffer用16进制输出到fd中 */
    void fs_String_buffer_out_hex(unsigned long bufferLenth, unsigned char buffer[], FILE *fd);
#ifdef FsDebug
    /* 
     * 把多种数据按__format格式合并成一个buffer,要求最后一个参数必须为（~((int)0)）,只用于检查,不会输出到最终结果中;
     * __format:(支持printf的所有格式和其它扩充格式,其中参数支持%-+,表示输出符号并左对齐)
     *     %c:输出一个字符;
     *     %s:输出一个字符串;
     *     %d:输出一个int的值;
     *     %ld:输出一个long的值;
     *     %u:输出一个unsigned int的值; 
     *     %lu:输出一个unsigned long的值;
     *     %x:按十六进制输出一个unsigned int的值; 
     *     %lx:按十六进制输出一个unsigned long的值;
     *     %f:输出一个float或double的值;
     *     %S:输出FsString类型数据,用指针传入;     
     */
#else
    /* 
     * 把多种数据按__format格式合并成一个buffer;
     * __format:(支持printf的所有格式和其它扩充格式,其中参数支持%-+,表示输出符号并左对齐)
     *     %c:输出一个字符;
     *     %s:输出一个字符串;
     *     %d:输出一个int的值;
     *     %ld:输出一个long的值;
     *     %u:输出一个unsigned int的值; 
     *     %lu:输出一个unsigned long的值;
     *     %x:按十六进制输出一个unsigned int的值; 
     *     %lx:按十六进制输出一个unsigned long的值;
     *     %f:输出一个float或double的值;
     *     %S:输出FsString类型数据,用指针传入;          
     */
#endif
    FsString *fs_String_vprint__IO(/* 实际数据开始位置距离返回结果的buffer头的字节数 */ const unsigned long prefixCount,
            /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned long suffixCount, const char * __restrict __format, __gnuc_va_list ap);

#ifdef FsDebug

    /* 
     * 把多种数据按__format格式合并成一个buffer,要求最后一个参数必须为（~((int)0)）,只用于检查,不会输出到最终结果中;
     * __format:(支持printf的所有格式和其它扩充格式,其中参数支持%-+,表示输出符号并左对齐)
     *     %c:输出一个字符;
     *     %s:输出一个字符串;
     *     %d:输出一个int的值;
     *     %ld:输出一个long的值;
     *     %u:输出一个unsigned int的值; 
     *     %lu:输出一个unsigned long的值;
     *     %x:按十六进制输出一个unsigned int的值; 
     *     %lx:按十六进制输出一个unsigned long的值;
     *     %f:输出一个float或double的值;
     *     %S:输出FsString类型数据,用指针传入;     
     */
    FsString *__fs_String_print__IO(/* 实际数据开始位置距离返回结果的buffer头的字节数 */ const unsigned long prefixCount,
            /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned long suffixCount, const char * __restrict __format, ...);
#define fs_String_print__IO(...) __fs_String_print__IO(__VA_ARGS__,~((int)0))
#else

    /* 
     * 把多种数据按__format格式合并成一个buffer;
     * __format:(支持printf的所有格式和其它扩充格式,其中参数支持%-+,表示输出符号并左对齐)
     *     %c:输出一个字符;
     *     %s:输出一个字符串;
     *     %d:输出一个int的值;
     *     %ld:输出一个long的值;
     *     %u:输出一个unsigned int的值; 
     *     %lu:输出一个unsigned long的值;
     *     %x:按十六进制输出一个unsigned int的值; 
     *     %lx:按十六进制输出一个unsigned long的值;
     *     %f:输出一个float或double的值;
     *     %S:输出FsString类型数据,用指针传入;          
     */
    FsString *fs_String_print__IO(/* 实际数据开始位置距离返回结果的buffer头的字节数 */ const unsigned long prefixCount,
            /* 实际数据结束位置距离返回结果的buffer尾的字节数 */ const unsigned long suffixCount, const char * __restrict __format, ...);
#endif
    /* 
     * 把buffer有16进制打印到文件中;
     */
    void fs_String_buffer_printf_hex(/* 数据长度 */unsigned long lenth, /* 数据的开始指针 */const unsigned char buffer[], /* 文件句柄 */FILE *fd);
    /* 
     * 获取INI中特定节点的值(此函数会修改pString的内容,返回值指向的内存为pString所申请的内存,不能删除pString对象);
     * 成功返回第一个键名对应的值;
     * 失败返回NULL.
     */
    char* fs_String_ini_get(FsString *pString, /* 节点名 */const char nodeName[], /* 键名 */const char keyName[]);
    /* 获取buffer的crc校验值 */
    unsigned int fs_String_buffer_crc(/* 数据的开始指针 */unsigned char buffer[], /* 数据长度 */unsigned long lenth);
    /* 获取buffer的crc32/mpeg校验值 */
    unsigned int fs_String_buffer_crc32_mpeg(/* 数据的开始指针 */unsigned char buffer[], /* 数据长度 */unsigned long lenth);
    /* 用数据创建一个FsObjectString对象 */
    FsObjectString *fs_String_object_new__IO(/* 数据区(即buffer)的长度,指定的大小不能动态改变 */const unsigned int datalenth);
    /* 用数据创建一个FsObjectString对象 */
    FsObjectString *fs_String_object_new__IO__OI_2(/* 数据区(即buffer)的长度,不能为0 */const unsigned int datalenth, /* 数据,不能为空 */char buffer[]);
#ifdef FsDebug
    void fs_String_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* STRING_H */

