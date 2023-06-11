/* 
 * File:   Sort.h
 * Author: fslib
 *
 * Created on 2017年04月25日, 上午9:38
 */
#ifndef SORT_H
#define	SORT_H
#include "../publicDefine.h"
#include <dirent.h>
#include "../Fs/Memery.h"
#ifdef	__cplusplus
extern "C" {
#endif
    /* 对 unsigned short 数组排序 */
    void fs_sort_ushort(/* 数据指针,可以是共享buffer中的空间 */unsigned short a__in_shareBuffer[], /* 数组个数,不能为0 */unsigned long count
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对已排序好的 unsigned short 数组去重复,重复值只保留一个,返回去除后的长度 */
    unsigned long fs_sort_unique_ushort(unsigned short a[], /* 数组个数,不能为0 */unsigned long count);
    /* 对 dirent* 数组排序 */
    void fs_sort_dirent(/* 数据指针,可以是共享buffer中的空间 */struct dirent * a__in_shareBuffer[], /* 数组个数,不能为0 */unsigned long count, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对已排序好的 dirent* 数组去重复,重复值只保留一个,返回去除后的长度 */
    unsigned long fs_sort_unique_dirent(struct dirent * a[], /* 数组个数,不能为0 */unsigned long count);
    void fs_sort_test();

#ifdef	__cplusplus
}
#endif

#endif	/* SORT_H */
