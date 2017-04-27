/* 
 * File:   VideoH264.h
 * Author: fslib.org
 *
 * Created on 2014年4月9日, 上午9:37
 */

#ifndef VIDEOH264_H
#define	VIDEOH264_H
#include "../publicDefine.h"
#include <stdio.h>
#include "../Fs/Object.h"
#ifdef	__cplusplus
extern "C" {
#endif
#define FsObjectH264_HeadLen ((unsigned int)(long)(((FsObjectH264*)0)->data))
#define FsObjectH264_TailLen 16
#define VideoH264DecodeType_YCC 0
#define VideoH264DecodeType_Y 1 
#define VideoH264DecodeType_YUV420P 2 
#ifndef WINDOWS
    //#define CoreAVC
#ifdef CoreAVC
    /* 表示用socket连接来同步系统,如果要使用新版本的解码库必须设定此值 */
    //#define CoreAVCSock
#endif
#endif

    /* h264解码对象 */
    typedef struct {
        /* 解码库对象 */
    } VideoH264Decode;

    /* 继承FsObject的H264数据 */
    typedef struct {
        FsObject object;
        /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
        unsigned short classIndex;
        /* 帧号,建议只使用3个字节 */
        unsigned int frameIndex;
        /* 数据的来源ip */
        unsigned int ipsrc;
        /* 帧率 */
        float frameRate;
        /* 图像宽度 */
        unsigned int width;
        /* 图像高度 */
        unsigned int height;
        /* 是否为关键帧;
         * 是但不包含sps和pps:1;
         * 不是但包含sps和pps:2;
         * 是且包含sps和pps:3;
         */
        unsigned char keyFrame;
        /* 采集时间之开机时间 */
        double capture_uptime;
        /* 采集时间之gmt时间 */
        double capture_gmtTime;
        /* 数据长度 */
        unsigned int datalenth;
        /* 数据,可外部设置 */
        char data[];
    } FsObjectH264;
    /* 
     * 检查当前帧是否为关键帧;
     * 是但不包含sps和pps返回1;
     * 不是但包含sps和pps返回2;
     * 是且包含sps和pps返回3;
     * 都不是返回0,
     * 错误返回-1.
     */
    signed char video_h264_check_keyFrame(unsigned int bufferLenth, const unsigned char buffer[]);
    /* 
     * 检查当前帧是否为关键帧;
     * 是但不包含sps和pps返回1;
     * 不是但包含sps和pps返回2;
     * 是且包含sps和pps返回3;
     * 都不是返回0,
     * 错误返回-1.
     */
    signed char video_h264_check_keyFrame_and_get_sps_pps(unsigned int bufferLenth, const unsigned char buffer[],
            /* 在包含pps时返回pps */const unsigned char **sps, /* pps的长度 */unsigned char *const spsLen,
            /* 在包含sps时返回sps */const unsigned char **pps, /* sps的长度 */unsigned char *const ppsLen);
    /* 
     * 检查当前帧中的sps和pps;
     * 成功返回1;
     * 错误返回-1.
     */
    signed char video_h264_get_sps_pps(unsigned int bufferLenth, const unsigned char buffer[],
            /* 在包含pps时返回pps */const unsigned char **sps, /* pps的长度 */unsigned char *const spsLen,
            /* 在包含sps时返回sps */const unsigned char **pps, /* sps的长度 */unsigned char *const ppsLen);
    /*
     * 创建一个h264解码对象;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    void*video_h264_decode_new(/* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height);
    /* 删除pH264Decode指向的实例对象 */
    void video_h264_decode_delete(void *pH264Decode);
    /* 
     * 解码h264;
     * 成功返回1;
     * 解码失败返回-1;
     * 需要重置返回-2.
     */
    signed char video_h264_decode(/* 解码对象 */ void *pH264Decode, /* 解码后的数据存放位置,大小必须足够大 */unsigned char rst_buffer[], /* 数据储存宽度,不能小于真实宽度,对于真实宽度为奇数的yuv420p必须不小于真实宽度+1 */unsigned int lineBits,
            /* 输出格式,赋值请用宏定义:
             *     0-ycc;
             *     1-y;
             *     2-yuv420p;
             */const unsigned char outType,
            /* 图像宽度,此值必须同真实值相同才会解码,否则此处会返回真实值宽度 */unsigned int *const width,
            /* 图像高度,此值必须同真实值相同才会解码,否则此处会返回真实值高度 */unsigned int *const height,
            /* 要解码的数据长度 */ const unsigned int bufferLenth, /* 要解码的数据 */const unsigned char buffer[]);
    /*
     * 创建一个h264编码对象;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    void* video_h264_encode_new(/* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height, /* 码率 */const unsigned int kbps, /* 帧率 */const float framerate);
    /* 删除pH264Encode指向的实例对象 */
    void video_h264_encode_delete(void *const pH264Encode);
    /* 
     * 编码h264;
     * 成功返回编码数据的长度;
     * 编码失败返回负数.
     */
    int video_h264_encode(/* 编码对象 */ void *const pH264Encode, /* 编码后的数据存放位置,内存为内部所有,必须尽快拷贝走 */unsigned char * * const rst_buffer,
            /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */ unsigned char *rst_keyFrame, /* 要编码的数据 */const unsigned char buffer[], /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex);
    /* 
     * 编码h264;
     * 成功返回FsObjectH264指针;
     * 编码失败返回NULL.
     */
    FsObjectH264 *video_h264_encode__IO(/* 编码对象 */ void *const pH264Encode, /* 要编码的数据 */const unsigned char buffer[],
            /* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime);

    //    /* 
    //     * 获取h264中的SPS信息;
    //     * 成功返回1;
    //     * 失败返回-1. 
    //     */
    //    char video_h264_sps_get(/* 结果储存的结构指针 */VideoH264SPS *rst, /* 数据指针,可以包含00 00 00 01也可不包含 */unsigned char data[], /* 数据长度 */unsigned char dataLenth);
    /* 
     * 获取h264数据流的宽度和高度;
     * 成功返回1;
     * 失败返回-1. 
     */
    char video_h264_size_get(unsigned int *width, unsigned int *height, /* sps数据指针,可以包含00 00 00 01也可不包含 */unsigned char spsData[], /* sps数据长度 */unsigned char spsLenth);
    /* 用数据创建一个VideoH264Object对象 */
    FsObjectH264 *video_h264_object_new__IO(/* h264数据长度 */const unsigned int datalenth);
    /* 用数据创建一个VideoH264Object对象 */
    FsObjectH264 *video_h264_object_new_with_data__IO(/* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* h264数据长度 */const unsigned int datalenth, /* 数据,长度必须和datalenth相同,为空表示此数据在外部设置 */ char data[]);

    /* 把fsObjectH264Data初始化一个VideoH264Object对象 */
    FsObjectH264 *video_h264_object_init__IO__OI(/* 必须是前预留FsObjectH264_HeadLen和后预留FsObjectH264_TailLen长度用fs_memery_malloc申请的内存 */char fsObjectH264Data[],
            /* fsObjectH264的内存大小 */const unsigned int fsObjectH264DataMemLenth, /* h264数据长度 */const unsigned int datalenth);
    /* 把fsObjectH264Data用数据初始化一个VideoH264Object对象 */
    FsObjectH264 *video_h264_object_init_with_data__IO__OI(/* 必须是前预留FsObjectH264_HeadLen和后预留FsObjectH264_TailLen长度用fs_memery_malloc申请的内存 */char fsObjectH264Data[],
            /* fsObjectH264的内存大小 */const unsigned int fsObjectH264DataMemLenth,
            /* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* h264数据长度 */const unsigned int datalenth);
    /* 重新设置pObjectH264的数据长度,只能比原来的小 */
    void video_h264_object_reset_datalenth_small(FsObjectH264 * const pObjectH264, /* h264数据长度 */const unsigned int datalenth);
    /* 设置pObjectH264的数据 */
    void video_h264_object_set_data(FsObjectH264 * const pObjectH264, /* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* 数据,长度必须和pObjectH264->datalenth相同,为空表示此数据在外部设置 */ char data[]);
    /* 用文件设置pObjectH264的数据,成功返回1,失败返回-1 */
    signed char video_h264_object_set_data_from_file(FsObjectH264* pObjectH264,
            /* 分类号 */ const unsigned short classIndex, /* 帧号,最大值只能到3个字节 */ const unsigned int frameIndex,
            /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 是否为关键帧;
             * 不确定-1;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */ signed char keyFrame,
            /* 采集时间之开机时间 */ const double capture_uptime,
            /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* 文件 */ FILE *fd);
#ifdef FsDebug
    void video_h264_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* VIDEOH264_H */

