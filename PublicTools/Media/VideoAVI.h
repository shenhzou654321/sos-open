/* 
 * File:   VideoAVI.h
 * Author: 5aii
 *
 * Created on 2014年2月21日, 下午12:33
 */

#ifndef VIDEOAVI_H
#define	VIDEOAVI_H
#include "../publicDefine.h"
#include <stdio.h>
#include "../Fs/ObjectList.h"
#ifdef	__cplusplus
extern "C" {
#endif
    /* AVI的帧信息 */
    struct VideoAVI_video {
        /* 相对于data的绝对偏移量 */
        unsigned long long offset;
        /* 长度 */
        unsigned int lenth;
    };
    /* AVI视频文件对象 */
    typedef struct {
        /* 视频宽度 */
        unsigned int width;
        /* 视频高度 */
        unsigned int height;
        /* 视频内图像编码类型(1:mjpeg;2:h264) */
        unsigned char encodeType;
        /* 帧率 */
        float frameRate;
        /* 视频文件的大小 */
        unsigned long long dataLenth;
        /* 储存数据帧的位置信息,当缓存为0时表示数据为连续块,成员为struct VideoAVI_video */
        FsObjectList *videoList;
        /* 为不定类型指针,可能是文件,也可能是objectList指针,还可能是一块足够大的内存区域,本对象不对此对象作内存释放的操作 */
        void *data;
        /* 内部的缓存空间 */
        char buffer[];
    } VideoAVI;
    /*
     * 创建一个VideoAVI实例用于保存视频到文件,只能调用以video_avi_file开头的函数;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    VideoAVI* video_avi_new_for_file_save__IO(/* 视频宽度 */const unsigned int width, /* 视频高度 */ const unsigned int height,
            /* 视频内图像编码类型(1:mjpeg;2:h264) */ const unsigned char encodeType, /* 打开的文件指针 */FILE *fd);
    /*
     * 读取文件信息创建VideoAVI对象;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    VideoAVI* video_avi_new_from_file__IO(/* 打开的文件指针 */FILE *fd);
    /*
     * 删除ppAVI指向的实例指针指向的对象;
     * 并把ppAVI指向的实例指针设为0.
     */
    void video_avi_delete__OI(VideoAVI **ppAVI);
    /* 输出调试信息 */
    void video_avi_out(VideoAVI *pAVI, FILE *fd);
    /*
     * 把图像数据帧保存到文件,pAVI必须是video_avi_new_for_file的返回值;
     * 成功返回偏移量;
     * 写失败返回-1;
     * fseek失败返回-2.
     */
    long long video_avi_file_save_frame(VideoAVI *pAVI, /* 图像数据帧的长度,如果数据长度改变后会更新此值 */ unsigned int *frameLenth, /* 图像数据帧的内容 */const char frameData[]);
    /* 
     * 把视频尾写入文件;
     * 成功返回1;
     * 写文件失败返回-1;
     * fseek失败返回-2.
     */
    char video_avi_file_save_finish(VideoAVI *pAVI, /* 帧率 */const double frameRate);
#ifdef FsDebug
    void video_avi_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* VIDEOAVI_H */

