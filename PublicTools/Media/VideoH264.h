/* 
 * File:   VideoH264.h
 * Author: fslib.org
 *
 * Created on 2014年4月9日, 上午9:37
 */

#if !defined VIDEOH264_H && !defined JAVA_IN_C

#define VIDEOH264_H
#include "../../PublicTools/publicDefine.h"

#include <stdio.h>
#include "../../PublicTools/Fs/Object.h"
#ifdef __cplusplus
extern "C" {
#endif
    /* 以下5宏在FsObjectH264,FsObjectH265中的值相等 */
#define FsObjectH264_HeadLen ((unsigned int)FsPointer_to_long(((FsObjectH264*)FsPointer(0))->data))
#define FsObjectH264_TailLen 16
#define VideoH264DecodeType_YCC 0
#define VideoH264DecodeType_Y 1 
#define VideoH264DecodeType_YUV420P 2 
    /* 是否能作为快照数据 */
#define FsObjectH264_can_snap(pH264) (0x3==(0x3&(pH264)->keyFrame))
    //#define CoreAVC
#ifdef CoreAVC
    /* 表示用socket连接来同步系统,如果要使用新版本的解码库必须设定此值 */
    //#define CoreAVCSock
#endif


    /* h264解码对象 */
    typedef struct {
        /* 解码库对象 */
    } VideoH264Decode;

    /* 继承FsObject的H264数据,FsObjectH264,FsObjectH265结构体大小必须保证相等 */
    typedef struct {
        FsObject object;
        /* 是否为关键帧;
         * 是但不包含sps和pps:1;
         * 不是但包含sps和pps:2;
         * 是且包含sps和pps:3;
         */
        unsigned char keyFrame;
        /* 拼接编号,不支持拼接应填0 */
        unsigned char stitchIndex;
        /* 图像拼接的左裁剪像素 */
        short cutLeft;
        /* 图像拼接的右裁剪像素 */
        short cutRight;
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
        /* 采集时间之开机时间 */
        double capture_uptime;
        /* 采集时间之gmt时间 */
        double capture_gmtTime;
        /* 数据长度 */
        unsigned int datalenth;
        /* 数据,可外部设置 */
        char data[0];
    } FsObjectH264;
#define __FsObjectH264Log(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p/%u,keyFrame=%hhu,classIndex=%hu,frameIndex=%u,ipsrc=%x,frameRate=%f,width=%u,height=%u,capture_uptime=%lf,capture_gmtTime=%lf,datalenth=%u.\n",##__VA_ARGS__\
        ,pH264,fs_ObjectBase_referCount_get(pH264),pH264->keyFrame,pH264->classIndex,pH264->frameIndex,pH264->ipsrc,pH264->frameRate\
        ,pH264->width,pH264->height,pH264->capture_uptime,pH264->capture_gmtTime,pH264->datalenth)
#define FsObjectH264Log(___loglevel,___format, ...) __FsObjectH264Log(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsObjectH264Log2(___loglevel,___format, ...) __FsObjectH264Log(FsLog2,___loglevel,___format,##__VA_ARGS__)

#pragma pack(push,1)

    /* 编码h264自定义的数据 */
    struct VideoH264EncodeCustom {
        /* 4字节头,使用00 00 00 01 */
        // unsigned char head[4];
        /* nal类型,标记为0x8C */
        // unsigned char nal_type;
        /* 采集时间和帧号标记,固定为"TI.fslib",此标记也是检测padding是否为此自定义协议的一个标记 */
        // char gmtTimeAndFrameIndex[8];
#ifndef __padding_data 
        double realTime;
        /* 帧号 */
        unsigned int frameIndex_24bits;
        /* 数据类型,1-h264数据,2-h265,3-aac数据,4-忽略数据(仅在收数据时标识,忽略一些不需要的数据,发数据不会设置此标识),255-表示当前包有padding */
        unsigned char type;

        union {

            struct {
                /* 是否为关键帧,-1-无法判断,1-是,0-不是,3-对h264来说包含sps和pps,对h265来说包含vps,sps和pps */
                signed char keyFrame;
                /* 图像宽度,0-表示未知 */
                unsigned short width;
                /* 帧率(小于0.00001表示未知) */
                float frameRate;
                /* 图像高度,0-表示未知 */
                unsigned short height;
                /* 拼接编号,不支持拼接应填0 */
                unsigned char stitchIndex;
                unsigned char _1;
                /* 图像拼接的左裁剪像素 */
                short cutLeft;
                /* 图像拼接的右裁剪像素 */
                short cutRight;
            } video;
        } p;
#endif
        /* 填充1字节 */
        //unsigned char _1;
        /* 填充数据,写入h264流时每2字节插入一个0x8C,末尾也要插入一个0x8C */
        unsigned char fillByte[14];
    };
#pragma pack(pop)

    /* 
     * 检查当前帧是否为关键帧;
     * 是但不包含sps和pps返回1;
     * 不是但包含sps和pps返回2;
     * 是且包含sps和pps返回3;
     * 都不是返回0,
     * 错误返回-1.
     */

    int video_h264_check_keyFrame(unsigned int bufferLenth, const unsigned char buffer[]);

    /* 
     * 检查当前帧是否为关键帧;
     * 是但不包含sps和pps返回1;
     * 不是但包含sps和pps返回2;
     * 是且包含sps和pps返回3;
     * 都不是返回0,
     * 错误返回-1.
     */

    int video_h264_check_keyFrame_and_get_sps_pps(unsigned int bufferLenth, const unsigned char buffer[]
            , /* 在包含pps时返回pps */const unsigned char **sps, /* pps的长度 */unsigned char *const spsLen
            , /* 在包含sps时返回sps */const unsigned char **pps, /* sps的长度 */unsigned char *const ppsLen);

    /* 
     * 获取当前帧中的sps和pps;
     * 成功返回1;
     * 错误返回-1.
     */

    int video_h264_get_sps_pps(unsigned int bufferLenth, const unsigned char buffer[]
            , /* 在包含sps时返回sps */const unsigned char **sps, /* sps的长度 */unsigned char *const spsLen
            , /* 在包含pps时返回pps */const unsigned char **pps, /* pps的长度 */unsigned char *const ppsLen);

    /* 
     * 获取当前帧中的sps;
     * 成功返回1;
     * 错误返回-1.
     */

    int video_h264_get_sps(unsigned int bufferLenth, const unsigned char buffer[]
            , /* 在包含sps时返回sps */const unsigned char **sps, /* sps的长度 */unsigned char *const spsLen);

    /* 
     * 获取当前帧中的pps;
     * 成功返回1;
     * 错误返回-1.
     */

    int video_h264_get_pps(unsigned int bufferLenth, const unsigned char buffer[]
            , /* 在包含pps时返回pps */const unsigned char **pps, /* pps的长度 */unsigned char *const ppsLen);


    /*
     * 创建一个h264解码对象;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */

    void *video_h264_decode_new__IO(/* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height);

    /* 删除pH264Decode指向的实例对象 */

    void video_h264_decode_delete__OI(void *const pH264Decode);

    /* 获取pH264Decode中储存缓存未输出的帧数量变量的指针,内部只使用低2字节 */

    unsigned int* video_h264_decode_get_bufferFrameCount(void *const pH264Decode);

    /* 
     * 解码h264;
     * 成功返回1;
     * 数据被内部引用(如h264解码时,传入的数据帧可能不会立即输出)返回-105;
     * 解码失败返回-1;
     * 需要重置返回-2.
     */

    int video_h264_decode(/* 解码对象 */ void *const pH264Decode, /* 解码后的数据存放位置,大小必须足够大 */unsigned char rst_buffer[], /* 数据储存宽度,不能小于真实宽度,对于真实宽度为奇数的yuv420p必须不小于真实宽度+1 */unsigned int lineBits
            , /* 输出格式,赋值请用宏定义:
             *     0-ycc;
             *     1-y;
             *     2-yuv420p;
             */const unsigned char outType
            , /* 图像宽度,此值必须同真实值相同才会解码,否则此处会返回真实值宽度 */unsigned int *const width
            , /* 图像高度,此值必须同真实值相同才会解码,否则此处会返回真实值高度 */unsigned int *const height
            , /* 要解码的数据长度,为0时表示输出内部缓存数据 */const unsigned int bufferLenth, /* 要解码的数据,bufferLenth为0时此值应为空 */const unsigned char buffer[]);


    /*
     * 创建一个h264编码对象;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */

    void* video_h264_encode_new__IO(/* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height, /* 关键帧间隔,包含关键帧,即1表示每帧都为关键帧 */const unsigned char keyInterval, /* 码率 */const unsigned int kbps, /* 帧率 */const float framerate);

    /* 删除pH264Encode指向的实例对象 */

    void video_h264_encode_delete__OI(void *const pH264Encode);

    /* 
     * 编码h264;
     * 成功返回编码数据的长度;
     * 编码失败返回负数.
     */

    int video_h264_encode(/* 编码对象 */ void *const pH264Encode, /* 编码后的数据存放位置,内存为内部所有,必须尽快拷贝走 */unsigned char * * const rst_buffer
            , /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */ unsigned char *rst_keyFrame, /* 要编码的数据 */const unsigned char buffer[], /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex);

    /* 
     * 编码h264;
     * 成功返回FsObjectH264指针;
     * 编码失败返回NULL.
     */


    FsObjectH264 * video_h264_encode__IO(/* 编码对象 */ void *const pH264Encode, /* 要编码的数据 */const unsigned char buffer[]
            , /* 拼接编号,不支持拼接应填0 */const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight
            , /* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime, /* 是否添加自定义标记,0-不添加,1-添加 */const char addCustomTag);


    //    /* 
    //     * 获取h264中的SPS信息(内部会查找);
    //     * 成功返回1;
    //     * 失败返回-1. 
    //     */
    //#ifdef OJAVA
    //#else
    //    int video_h264_sps_get(/* 结果储存的结构指针 */VideoH264SPS * const rst, /* 数据指针,可以包含00 00 00 01也可不包含 */const unsigned char data[], /* 数据长度 */const unsigned int dataLenth);
    //#endif
    //    /* 
    //     * 获取h264中的PPS信息(内部会查找);
    //     * 成功返回1;
    //     * 失败返回-1. 
    //     */
    //#ifdef OJAVA
    //#else
    //    int video_h264_pps_get(/* 结果储存的结构指针 */VideoH264PPS * const rst, /* 数据指针,可以包含00 00 00 01也可不包含 */const unsigned char data[], /* 数据长度 */const unsigned int dataLenth);
    //#endif
    /* 
     * 获取h264数据流的宽度和高度(内部会查找);
     * 成功返回1;
     * 失败返回-1. 
     */

    int video_h264_size_get(unsigned int *const width, unsigned int *const height, /* sps数据指针,可以包含00 00 00 01也可不包含 */const unsigned char spsData0[], /* sps数据长度 */unsigned int spsLenth0);
    /* 创建一个FsObjectH264对象 */
    FsObjectH264 *video_h264_object_new__IO(/* h264数据长度 */const unsigned int datalenth);

    /* 用数据创建一个FsObjectH264对象 */

    FsObjectH264 *video_h264_object_new_with_data__IO(/* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame, /* 拼接编号,不支持拼接应填0 */const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime
            , /* h264数据长度 */const unsigned int datalenth, /* 数据,长度必须和datalenth相同,为空表示此数据在外部设置 */ char data[]);

    /* 把一个FsObjectBase实例化成FsObjectH264对象,仅在pObjectBase的引用计数不大于1时可调用 */

    void video_h264_object_init_from_objectBase(FsObjectBase * const pObjectBase, /* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame, /* 拼接编号,不支持拼接应填0 */const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime);

    /* 把fsObjectH264Data初始化一个FsObjectH264对象 */

    FsObjectH264 *video_h264_object_init__IO__OI(/* 必须是前预留FsObjectH264_HeadLen和后预留FsObjectH264_TailLen长度用fs_memery_malloc申请的内存 */char fsObjectH264Data[]
            , /* fsObjectH264的内存大小 */const unsigned int fsObjectH264DataMemLenth, /* h264数据长度 */const unsigned int datalenth);

    /* 把fsObjectH264Data用数据初始化一个FsObjectH264对象 */

    FsObjectH264 *video_h264_object_init_with_data__IO__OI(/* 必须是前预留FsObjectH264_HeadLen和后预留FsObjectH264_TailLen长度用fs_memery_malloc申请的内存 */char fsObjectH264Data[]
            , /* fsObjectH264的内存大小 */const unsigned int fsObjectH264DataMemLenth
            , /* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame, /* 拼接编号,不支持拼接应填0 */const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime
            , /* h264数据长度 */const unsigned int datalenth);


    /* 从h264数据中读取拼接编号,失败或不需要拼接都返回0,同时返回拼接的裁剪像素 */

    unsigned char video_h264_get_stitchIndex(/* 图像拼接的左裁剪像素 */ short *cutLeft, /* 图像拼接的右裁剪像素 */ short *cutRight, /* h264数据 */unsigned char data[], /* h264数据长度 */ unsigned int datalenth);

    /* 重新设置pObjectH264的数据长度,只能比原来的小 */

    void video_h264_object_reset_datalenth_small(FsObjectH264 * const pObjectH264, /* h264数据长度 */const unsigned int datalenth);

    /* 设置pObjectH264的数据 */

    void video_h264_object_set_data(FsObjectH264 * const pObjectH264, /* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 是否为关键帧;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */const unsigned char keyFrame, /* 拼接编号,不支持拼接应填0 */const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime
            , /* 数据,长度必须和pObjectH264->datalenth相同,为空表示此数据在外部设置 */ char data[]);

    /* 用文件设置pObjectH264的数据,成功返回1,失败返回-1 */

    int video_h264_object_set_data_from_file(FsObjectH264 * const pObjectH264
            , /* 分类号 */ const unsigned short classIndex, /* 帧号,最大值只能到3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 是否为关键帧;
             * 不确定-1;
             * 是但不包含sps和pps:1;
             * 不是但包含sps和pps:2;
             * 是且包含sps和pps:3;
             */ signed char keyFrame, /* 拼接编号,不支持拼接应填0 */const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime
            , /* 文件 */ FILE *fd);



#ifdef __cplusplus
}
#endif

#endif /* VIDEOH264_H */

