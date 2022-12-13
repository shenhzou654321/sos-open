/* 
 * File:   Disk.h
 * Author: fslib
 *
 * Created on 2013年5月20日, 下午1:01
 */
#if !defined DISK_H && !defined JAVA_IN_C

#define DISK_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/ObjectList.h"
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        /* 接口类型(1-普通硬盘;2-USB设备) */
        char type;
        /* model */
        char *model;
        /* 串号SN */
        char *serialNo;
        /* 硬盘大小 */
        unsigned long long size;
    } FsDiskInfo;

    /*
     * 删除ppDiskInfo指向的实例指针指向的对象;
     * 并把ppDiskInfo指向的实例指针设为0.
     */

    void fs_diskInfo_delete__OI(FsDiskInfo** ppDiskInfo);

    /*
     * 获取diskName(如/dev/sda)的磁盘信息;
     * 成功返回获取到的信息;
     * 失败返回NULL;
     * Windows暂不支持.
     */

    FsDiskInfo* fs_diskInfo_get__IO(const char diskName[]);

    /*
     * 获取系统硬盘的磁盘信息;
     * 成功返回获取到的信息;
     * 失败返回NULL;
     * Windows暂不支持.
     */

    FsDiskInfo* fs_diskInfo_root_get__IO();

    /* 
     * 获取系统的所有非移动储存设备信息,返回按大小升序排序,返回的各成员必须用fs_diskInfo_delete__OI释放空间;
     * 成功返回硬盘信息的链表指针;
     * 失败返回NULL.
     */

    FsObjectList *fs_diskInfo_get_all_notRemoveable_order_ASC_by_size__IO();

    /* 
     * 获取硬盘分区的结束位置;
     * 成功返回结束的偏移量;
     * 打开失败返回-1;
     * fseek失败返回-2;
     * 读失败返回-3;
     */

    long long fs_disk_partion_get_end(/* 硬盘名,如/dev/sda */ const char diskname[]);

    /* 获取分区类型,0-表示错误,0x0B-FAT32,0x0C-FAT32,0x83-linux */

    unsigned char fs_disk_partion_get_type(/* 不为空时,返回真实的分区名,如/dev/sda1 */char rst_realDiskPartion[], /* 硬盘名,如/dev/sda1或/dev/disk/by-uuid/12345678-1234-1234-1234-123456789abc */ const char diskname_cin[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 获取硬盘的真实名字,成功返回rst_realDiskPartion指针,失败返回NULL */

    char* fs_disk_get_realName(/* 真实的名字,如/dev/sda1 */char rst_realDiskPartion[], /* 硬盘名,如/dev/sda1或/dev/disk/by-uuid/12345678-1234-1234-1234-123456789abc */ const char diskname_cin[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取硬盘大小;
     * 成功返回硬盘大小;
     * 打开失败返回-1;
     * 获取大小失败返回-2;
     * 未找到硬盘返回-3.
     */

    long long fs_disk_size_get(/* 硬盘名,如/dev/sda,为空表示获取所有硬盘的总容量 */ const char diskname[]);

#ifdef __cplusplus
}
#endif

#endif /* DISK_H */

