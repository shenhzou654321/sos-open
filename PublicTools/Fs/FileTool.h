/* 
 * File:   FileTool.h
 * Author: fslib
 *
 * Created on 2013年1月9日, 上午9:25
 */

#ifndef FILETOOL_H
#define FILETOOL_H
#include "../publicDefine.h"
#include "StringList.h"
#include "Memery.h"


#ifdef __cplusplus
extern "C" {
#endif

    /* 创建目录dir中的每一级目录 */
    void fs_dir_make(/* 要创建的目录名,只有/以前的才会被创建 */const char dir[], /* mode:创建目录的访问权限，默认0777 */unsigned int mode /* = 0777 */);
    /* 输出dirName目录下的信息 */
    void fs_dir_out(/* 目录名,/后的数据会被忽略 */const char dirName_cin[], FILE * const fd
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 判断路径是目录还是文件;
     * 目录返回1;
     * 文件返回0;
     * 错误返回-1.
     */
    signed char fs_path_dir_or_file(/* 路径名 */const char path[]);
    /* 
     * 判断是否为目录
     * 是则返回1;
     * 不是则返回0;
     * 错误返回-1.
     */
    signed char fs_file_is_dir(const char path[]);
    /*
     * 删除name_cin下的所有文件或者目录;
     * 成功返回1,失败返回-1.
     */
    int fs_rm_all(/* 所指向的目录或文件 */ const char name_cin[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 把路径中包含的非linux路径转换为linux路径 */
    char* fs_path_to_linux__IO(char path[]);
    /*
     * 查找文件或目录的绝路径;
     * parentDir:父目录,以/结尾的为目录否则为文件,NULL表示当前程序目录,如果以.开头则会根据当前程序目录重新计算绝对路径;
     * fileName:为文件名,NULL表示返回parentDir或当前程序,
     *     不为空时表示相对于parentDir或当前程序的下一路径,支持以"/"开头,
     *     "."或"./"表示返回parentDir或当前程序目录;
     * 如果parentDir不包含目录且不为空且fileName的有效值为空返回NULL;
     * 目录分割符可以是多个"/";
     */
    char* fs_realLink__IO(const char parentDir[], const char fileName[]);
    /* 获取当前程序的文件名,失败返回NULL */
    char* fs_programName__IO();
    /** 
     * 参考reference_path获取src_path的绝对路径,如src_path为../aa,reference_path为a1/a2/a3,则rst_path为a1/a2/aa,成功返回rst_path的长度,失败返回-1
     */
    int fs_path_absolute(/* 储存目标数据的空间,不大于src_path+reference_path的空间,可以和reference_path相同 */char rst_path[]
            , /* 源路径 */const char src_path[], /* 源路径的长度 */ unsigned int src_pathLen
            , /* 参考路径 */const char reference_path[], /* 参考路径的长度 */ unsigned int reference_pathLen);
    /* 
     * 获取指定目录下指定后缀的所有文件名;
     * 成功且有数据返回FsStringList对象,否则返回NULL.
     */
    FsStringList *fs_dir_get_fileName_with_suffix__IO(/* 全路径的目录名, 必须以"/"结尾 */const char dirName[], /* 文件的后缀,不能为空,区分大小写,如.jpg */const char suffix[],
            /* 结果排序,1:升序,2:降序,在linux下无法排序 */const char order);
    /* 
     * 获取指定目录下指定后缀的所有文件名;
     * 成功且有数据返回FsStringList对象,否则返回NULL.
     */
    FsStringList * fs_dir_get_fileName_match_with_order__IO(/* 全路径的目录名, 必须以"/"结尾 */const char dirName[]
            , /* 文件名匹配的函数指针,匹配返回1,不匹配返回-1 */int (*const match) (const char fileName[])
            , /* 排序的比较函数,数据按比较结果从小到大排序:
         *     str1>str2返回1;
         *     str1=str2返回0
         *     否则返回-1;
         */int (*const orderCompare) (const char str1[], const char str2[]));

    /* 
     * 获取指定目录下指定后缀的所有文件名(绝对路径);
     * 成功且有数据返回FsStringList对象,否则返回NULL.
     */
    FsStringList * fs_dir_get_fileName_all_match(/* 全路径的目录名, 必须以"/"结尾 */ const char dirName[]
            , /* 文件名匹配的函数指针,匹配返回1,不匹配返回-1 */int (*const match) (const char fileName[]));
    /* 
     * 获取指定目录下指定后缀的所有文件名(绝对路径);
     * 成功且有数据返回FsStringList对象,否则返回NULL.
     */
    FsStringList *fs_dir_get_fileName_all_with_suffix__IO(/* 全路径的目录名, 必须以"/"结尾 */const char dirName[], /* 文件的后缀,不能为空,区分大小写,如.jpg */const char suffix[],
            /* 结果排序,1:升序,2:降序,在linux下无法排序 */const char order);
    /* 
     * 获取指定目录下指定后缀的所有文件名;
     * 成功且有数据返回FsStringList对象,否则返回NULL.
     */
    FsStringList *fs_dir_get_fileName_all_match_with_order__IO(/* 全路径的目录名, 必须以"/"结尾 */const char dirName[]
            , /* 文件名匹配的函数指针,匹配返回1,不匹配返回-1 */int (*const match) (const char fileName[])
            , /* 排序的比较函数,数据按比较结果从小到大排序:
             *     str1>str2返回1;
             *     str1=str2返回0
             *     否则返回-1;
             */int (*const orderCompare) (const char str1[], const char str2[]));
    /* 
     * 获取指定目录下指定后缀的所有文件名;
     * 成功且有数据返回FsStringList对象,否则返回NULL.
     */
    FsStringList *fs_dir_get_fileName_all_with_suffix_order__IO(/* 全路径的目录名, 必须以"/"结尾 */const char dirName[], /* 文件的后缀,不能为空,区分大小写,如.jpg */const char suffix[]
            , /* 排序的比较函数,数据按比较结果从小到大排序:
             *     str1>str2返回1;
             *     str1=str2返回0
             *     否则返回-1;
             */int (*orderCompare)(const char str1[], const char str2[]));
    /* 
     * 获取指定目录下的一个目录名(非全路径);
     * 成功返回1;
     * 打开失败或没有目录返回-1;
     * 缓存不够返回-2.
     */
    signed char fs_dir_get_dir_first(/* 存放目录名的缓存 */ char buffer[], /* 存放目录名的缓存大小 */ unsigned int bufferLenth, /* 全路径的目录名, 无须以"/"结尾 */const char dirName[]);
    /* 
     * 获取指定目录下的目录名,文件名及软连接名;
     * 成功且有数据返回FsStringList对象,否则返回NULL;
     *     dirName:全路径的目录名, 无须以"/"结尾;
     *     order:结果排序,1:升序,目录在前,-1:降序,目录在后,0:不排序.
     */
    FsStringList *fs_dir_get_dir_file_link_order__IO(const char dirName[], const char order);
    /* 更新文件的Access和Access时间为当前时间,成功返回1,打开文件失败返回-1,设置时间失败返回-2 */
    signed char fs_file_time_update(/* 文件名 */const char fileName[]);
    /* 清空fd流中缓存的数据 ,主要用于清空标准输入流中的数据 */
    void fs_file_buffer_clear(FILE *fd);
    /* 
     * 从fd输入流中读取一个字符串,遇到\n或0或文件尾结束;
     * 成功返回读取到的字符串指针;
     * 失败返回NULL.  
     */
    char *fs_file_read_string__IO(FILE *fd);
    /* 获取文件大小;
     * 成功返回文件大小;
     * 失败返回-1;
     */
    long fs_file_size_get(const char fileName[]);
    /* 
     * 计算文件的MD5值,结果为二进制数据;
     * 成功返回1;
     * 文件操作失败返回-1.
     */
    int fs_file_md5sum(/* 接收结果的缓存空间,大小为16字节 */unsigned int rst_4[], /* 文件名字,不能为空 */const char fileName[]);
    /* 
     * 调整文件大小,成功返回1,失败返回-1;
     */
    int fs_file_resize(const char fileName[], const unsigned long size);
    /* 
     * 计算文件的MD5值,结果为字符串;
     * 成功返回1;
     * 文件操作失败返回-1.
     */
    int fs_file_md5sum_string(/* 结果储存的buffer,如果rst_32[0]为1结果用大写字母表示,否则用小写字母 */char rst_32[], /* 文件名字,不能为空 */const char fileName[]);
    /* 
     * 获取dirName当前目录(不包含子目录)中的文件个数;
     * 成功返回文件个数;
     * 失败返回-1. 
     */
    long fs_dir_get_fileCount(const char dirName[]);
    /*
     * 计算文件夹内数据的md5校验值,结果为二进制数据(只支持普通目录和普通文件,不能支持软连接和硬连接);
     * 成功返回1;
     * 没有文件返回0;
     * 打开文件夹失败返回-1;
     * 校验文件失败返回-2.
     */
    int fs_dir_md5sum(/* 接收结果的缓存空间,大小为16字节 */unsigned int rst_4[], /* 文件夹名字,不能为空 */const char dirName_cin[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /*
     * 计算文件夹内数据的md5校验值,结果为字符串(只支持普通目录和普通文件,不能支持软连接和硬连接);
     * 成功返回1;
     * 没有文件返回0;
     * 打开文件夹失败返回-1;
     * 校验文件失败返回-2.
     */
    int fs_dir_md5sum_string(/* 结果储存的buffer,如果rst_32[0]为1结果用大写字母表示,否则用小写字母 */char rst_32[], /* 文件夹名字,不能为空 */const char dirName[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /*
     * 计算文件夹内数据的大小(只支持普通目录和普通文件,不能支持软连接和硬连接);
     * 成功返回文件夹数据大小;
     * 打开文件夹失败返回-1.
     */
    long long fs_dir_size(/* 文件夹名字,不能为空 */const char dirName[]);

    /* 
     * 把文件从srcPath拷贝到dstPath;
     * 成功返回1;
     * 打开源文件失败返回-1;
     * 打开目的文件失败返回-2;
     * 读出错返回-3;
     * 写出错返回-4.
     */
    signed char fs_file_copy(/* 源地址,绝对路径 */ const char srcPath[], /* 目的地址,绝对路径 */const char dstPath[]);

    /* 
     * 把文件从srcPath拷贝到dstPath;
     * 成功返回1;
     * 打开源文件失败返回-1;
     * 打开目的文件失败返回-2;
     * 读出错返回-3;
     * 写出错返回-4.
     */
    signed char fs_file_copy_dstChar(/* 源地址,绝对路径 */ char srcPath[], /* 目的地址的哪位与源地址不同 */const unsigned short dstPos, /* 目的地址使用的字符 */char c);
    /* 
     * 复制文件,源路径使用文件指针
     * 成功返回1;
     * 源文件seek出错返回-1;
     * 获取文件位置失败返回-2;
     * 读出错返回-3;
     * 写出错返回-4.
     */
    int fs_file_copy_srcFd(/* 源地址,使用文件句柄 */ FILE * const fd, /* 目的地址,绝对路径 */const char dstPath_cin[]
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);
    /* 
     * 读取文件为FsObjectBase;
     * 成功返回FsObjectBase指针;
     * 失败返回NULL.
     */
    FsObjectBase * fs_file_read__IO(/* 文件名 */ const char filename[], /* 指data的偏移位置,不小于sizeof(FsObjectBase) */ const unsigned int dataOffset
            , /* 实际数据结束位置距离返回结果的data尾的字节数 */const unsigned int suffixCount, /* 为空表示不使用缓存Buffer */FsObjectBaseBuffer * const pObjectBaseBuffer);
    /* 
     * 从文件读取指定长度的数据到buffer;
     * 成功返回1;
     * 打开源文件失败返回-1;
     * 读出错返回-2.
     */
    int fs_file_read(/* 缓存 */char buffer[], /* 要读取数据的大小 */unsigned int bufferLenth, /* 文件名 */ const char fileName[]);
    /* 
     * 从文件读取指定最大长度的数据到buffer;
     * 成功返回1;
     * 打开源文件失败返回-1;
     * 读出错返回-2.
     */
    __attribute__ ((__deprecated__)) signed char fs_file_read_max(/* 缓存 */char buffer[], /* 要读取数据的大小+1 */unsigned int bufferLenth, /* 文件名 */ const char fileName[]);
    /* 
     * 从文件读取指定最大长度的数据到buffer;
     * 成功返回读取到的数据长度;
     * 打开源文件失败返回-1;
     * 读出错返回-2.
     */
    int fs_file_readMax(/* 缓存 */char buffer[], /* 要读取数据的大小 */unsigned int maxLen, /* 文件名 */ const char fileName[]);
    /* 
     * 读取文件数据到共享buffer中,未加结束符0,但已分配结束符0的空间
     * 成功返回读取的数据长度;
     * 修改文件位置失败返-1;
     * 获取文件大小失败返回-2;
     * 读取失败返回-3.
     */
    int fs_file_read_shareBuffer_fd(/* 成功时*rst_data指向读取数据的开始位置 */char **rst_data, /* *rst_data后面的额外空间 */const unsigned int rst_data_tailLen
            , FILE * const fd, /* 偏移量 */const int base, /* 读取的最大长度,0表示读取所有 */ int maxLen
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);
    /* 
     * 读取文件数据到共享buffer中,未加结束符0,但已分配结束符0的空间
     * 成功返回读取的数据长度;
     * 打开文件失败返回-1;
     * fstat失败返回-2;
     * 读取失败返回-3.
     */
    int fs_file_read_shareBuffer(/* 成功时*rst_data指向读取数据的开始位置 */char **rst_data, /* 文件名 */const char filename_cin[], /* 读取的最大长度,0表示全部读取 */ int maxLen
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 读取文件数据到共享buffer中,未加结束符0,但已分配结束符0的空间
     * 成功返回1;
     * 打开文件失败返回-1;
     * 读取失败返回-2;
     * 其他错误返回-3.
     */
    int fs_file_low_level_read_shareBuffer(/* 成功时*rst_data指向读取数据的开始位置 */char **rst_data, /* 文件名 */const char filename[], /* 偏移量 */ unsigned long base, /* 读取的长度 */ unsigned long len
            , /* 共享buffer,不可为空 */ FsShareBuffer * const pShareBuffer);
    /* 写数据,成功返回1,失败返回-2 */
    int fs_file_write_fd(/* 要写入的数据 */const char data[], /* 要写入的数据长度 */size_t dataLen, FILE * const fd);
    /* 
     * 把filename文件中的所有为src的行替换成dist1+dist2;
     * 成功返回1;
     * 打开源文件失败返回-1;
     * 读取源文件失败返回-2;
     * 打开文件写失败返回-3;
     * 写入失败返回-4;
     */
    signed char fs_file_replace_line2(/* 文件名 */const char filename[], /* 原字符串,一般不含换行符 */const char src[],
            /* 被替换成的字符串组成1,一般不含换行符 */const char dist1[], /* 被替换成的字符串组成2,一般不含换行符 */ const char dist2[]);
    /* 
     * 把filename文件中的所有为src的行的下一行替换成dist1+dist2+dist3;
     * 成功返回1;
     * 打开源文件失败返回-1;
     * 读取源文件失败返回-2;
     * 打开文件写失败返回-3;
     * 写入失败返回-4;
     */
    signed char fs_file_replace_line3_next(/* 文件名 */const char filename[], /* 原字符串,一般不含换行符 */const char src[],
            /* 被替换成的字符串组成1,一般不含换行符 */const char dist1[], /* 被替换成的字符串组成2,一般不含换行符,可为空 */ const char dist2[], /* 被替换成的字符串组成3,一般不含换行符,可为空 */ const char dist3[]);
    /* 在parentDir下查找各级文件名与childName长度相同,且最相近的两个文件名 */
    void fs_dir_match_prefect(/* 返回的文件名1,长度就是parentDir+childName的长度,如果没有找到返回长度为0 */char rst_1[],
            /* 返回的文件名2,长度就是parentDir+childName的长度,如果没有找到返回长度为0 */ char rst_2[],
            /* 父目录,以'/'结尾,返回值中固定的串,如/aa/,可以使用externBuffer,rst_1,rst_2的内存 */ const char parentDir[], /* 子名,返回值中可变动的串,如cc/bb/22 */const char childName[],
            /* 函数内部用到的缓存空间,是childName的空间大小的2倍,可为空 */char externBuffer[], /* 查找的模式,0-保证rst_1必须小于给定值,1-保证rst_2必须大于给定值 */const char model);
    /* 在parentDir下查找各级文件名与childName长度相同,且最相近的一个文件夹下的所有文件,尽量包含childName,返回的文件名只包含childName,不包含parentDir */
    FsStringList * fs_dir_match_prefect__IO(/* 父目录,以'/'结尾,返回值中固定的串,如/aa/ */ const char parentDir[]
            , /* 子名,返回值中可变动的串,如cc/bb/22 */const char childName[], /* 查找的模式,0-向前查找,1-向后查找 */const char model);
    /* 在parentDir下查找各级文件名与childName长度相同,且最相近的一个文件夹下的所有文件,尽量包含childName,返回的文件名为parentDir+childName模式 */
    FsStringList *fs_dir_match_prefect_allName__IO(/* 父目录,以'/'结尾,返回值中固定的串,如/aa/ */ const char parentDir[]
            , /* 子名,返回值中可变动的串,如cc/bb/22 */const char childName[], /* 查找的模式,0-向前查找,1-向后查找 */const char model);
    /* 
     * 在parentDir下查找各级文件名在childName1和childName2之间(含两者)且长度相同的文件名;
     * 有返回FsStringList链表;
     * 失败或无匹配返回NULL.
     */
    FsStringList *fs_dir_between__IO(/* parentDir的长度 */const unsigned int parentDirLen, /* 父目录,以'/'结尾,返回值中固定的串,如/aa/ */ const char parentDir[],
            /* 子名1,返回值中可变动的串,如cc/bb/22 */const char childName1[], /* 子名2,返回值中可变动的串,如cc/bb/22 */const char childName2[],
            /* 函数内部用到的缓存空间,长度为parentDir+childName1的长度,可为空,可以和parentDir相同,但不能包含childName1和childName2 */char externBuffer[]);
    /* 
     * 在parentDir下查找各级文件名在childName1和childName2之间(含两者)且长度相同的文件名;
     * 有返回FsStringList链表(正序);
     * 失败或无匹配返回NULL.
     */
    FsStringList *fs_dir_between_ASC__IO(/* parentDir的长度 */const unsigned int parentDirLen, /* 父目录,以'/'结尾,返回值中固定的串,如/aa/ */ const char parentDir[],
            /* 子名1,返回值中可变动的串,如cc/bb/22 */const char childName1[], /* 子名2,返回值中可变动的串,如cc/bb/22 */const char childName2[],
            /* 函数内部用到的缓存空间,长度为parentDir+childName1的长度,可为空,可以和parentDir相同,但不能包含childName1和childName2 */char externBuffer[]);
    /* 
     * 在parentDir下查找各级文件名在childName1和childName2之间(含两者)且长度相同的文件名;
     * 有返回FsStringList链表(倒序);
     * 失败或无匹配返回NULL.
     */
    FsStringList *fs_dir_between_DESC__IO(/* parentDir的长度 */const unsigned int parentDirLen, /* 父目录,以'/'结尾,返回值中固定的串,如/aa/ */ const char parentDir[],
            /* 子名1,返回值中可变动的串,如cc/bb/22 */const char childName1[], /* 子名2,返回值中可变动的串,如cc/bb/22 */const char childName2[],
            /* 函数内部用到的缓存空间,长度为parentDir+childName1的长度,可为空,可以和parentDir相同,但不能包含childName1和childName2 */char externBuffer[]);
    /* 设置程序的最大句柄数 */
    void fs_fileNO_set(/* 最大句柄数,0表示使用默认的最大句柄数,一般为系统支持的最大句柄数 */const unsigned int maxNO);

#ifdef __cplusplus
}
#endif

#endif /* FILETOOL_H */

