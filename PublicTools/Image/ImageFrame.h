/* 
 * File:   ImageFrame.h
 * Author: fslib
 *
 * Created on 2013年4月27日, 下午1:29
 */

#ifndef IMAGEFRAME_H
#define	IMAGEFRAME_H
#include "../Fs/Object.h"
#include "../Media/VideoH264.h"


#ifdef	__cplusplus
extern "C" {
#endif
#define ImageFrame_RAW_0 (0x1<<0)
#define ImageFrame_RAW_1 (0x1<<1)
#define ImageFrame_RAW_2 (0x1<<2)
#define ImageFrame_H264_0 (0x1<<3)
#define ImageFrame_H264_1 (0x1<<4)
#define ImageFrame_H264_2 (0x1<<5)
#define ImageFrame_RGB_0 (0x1<<6)
#define ImageFrame_RGB_1 (0x1<<7)
#define ImageFrame_RGB_2 (0x1<<8)
    /* 可以单帧转换且消耗cpu比较小的数据,如yuv */
#define ImageFrame_Single_Light_0 (ImageFrame_RAW_0|ImageFrame_RGB_0)
#define ImageFrame_Single_Light_1 (ImageFrame_RAW_1|ImageFrame_RGB_1)
#define ImageFrame_Single_Light_2 (ImageFrame_RAW_2|ImageFrame_RGB_2)
    /* 可以单帧转换但消耗cpu比较多的数据,如jpeg */
    /* 可以单帧转换的数据,如yuv */
#define ImageFrame_Single_0 (ImageFrame_Single_Light_0)
#define ImageFrame_Single_1 (ImageFrame_Single_Light_1)
#define ImageFrame_Single_2 (ImageFrame_Single_Light_2)
    /* 不能单帧转换的数据,如h264 */
#define ImageFrame_Relative_0 (ImageFrame_H264_0)
#define ImageFrame_Relative_1 (ImageFrame_H264_1)
#define ImageFrame_Relative_2 (ImageFrame_H264_2)
    /* 每种图的数据 */
#define ImageFrame_0 (ImageFrame_Single_0|ImageFrame_Relative_0)
#define ImageFrame_1 (ImageFrame_Single_1|ImageFrame_Relative_1)
#define ImageFrame_2 (ImageFrame_Single_2|ImageFrame_Relative_2)
#define ImageFrame_valid_RAW_0(dataValidType) ((dataValidType)&(ImageFrame_RAW_0))
#define ImageFrame_valid_RAW_1(dataValidType) ((dataValidType)&(ImageFrame_RAW_1))
#define ImageFrame_valid_RAW_2(dataValidType) ((dataValidType)&(ImageFrame_RAW_2))
#define ImageFrame_valid_H264_0(dataValidType) ((dataValidType)&(ImageFrame_H264_0))
#define ImageFrame_valid_H264_1(dataValidType) ((dataValidType)&(ImageFrame_H264_1))
#define ImageFrame_valid_H264_2(dataValidType) ((dataValidType)&(ImageFrame_H264_2))
#define ImageFrame_valid_RGB_0(dataValidType) ((dataValidType)&(ImageFrame_RGB_0))
#define ImageFrame_valid_RGB_1(dataValidType) ((dataValidType)&(ImageFrame_RGB_1))
#define ImageFrame_valid_RGB_2(dataValidType) ((dataValidType)&(ImageFrame_RGB_2))

    /* 继承FsObject的RAW数据 */
    typedef struct {
        FsObject object;
        /* raw数据的类型(0-无效;1-yuv420p) */
        unsigned char type : 1;
        /* 数据,可外部设置 */
        unsigned char data[];
    } FsObjectImageRAW;

    /* 继承FsObject的RGB数据 */
    typedef struct {
        FsObject object;
        /* 数据,可外部设置 */
        unsigned char data[];
    } FsObjectImageRGB;

    /* 
     * 图像数据帧对象,继承FsObject结构默认定义大中小三种大小的图像数据;
     * 因结构复杂,请勿直接改变其成员的值;
     * 使用场景:
     *    在数据流传递过程中数据只增不减,仅在数据回收时删除被引用对象;
     */
    typedef struct {
        FsObject object;
        /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
        unsigned short classIndex;
        /* 分别是:大[0],中[1],小[2]图的宽度 */
        unsigned int width[3];
        /* 分别是:大[0],中[1],小[2]图的高度 */
        unsigned int height[3];
        /* 分别是:大[0],中[1],小[2]图的宽度*高度的值 */
        unsigned int sizeWidthHeight[3];
        /* 图像数据帧的帧号 */
        unsigned int index;
        /* 数据的来源ip */
        unsigned int ipsrc;
        /* 帧率 */
        float frameRate;
        /* 采集时间之开机时间 */
        double capture_uptime;
        /* 采集时间之gmt时间 */
        double capture_gmtTime;
        /* 
         * 帧中有效的数据类型(以下为二进制,1代表有,0代表无)
         */
        unsigned long dataValidType;

        /* 图像数据 */
        struct {
            /* 裸数据 */
            FsObjectImageRAW *raw[3];
            /* 分别是:大[0],中[1],小[2]图的h264数据 */
            FsObjectH264 *h264[3][3];
            /* rgb数据 */
            FsObjectImageRGB *rgb[3];

        } data; /* 图像数据 */
    } FsObjectImageFrame;
    /*
     * 根据文件创建一个ImageFrame实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL;
     */
    FsObjectImageFrame* image_frame_new__IO(/* 大图宽度 */const unsigned int width_0, /* 大图高度 */const unsigned int height_0,
            /* 中等大小图宽度 */const unsigned int width_1, /* 中等大小图高度 */const unsigned int height_1,
            /* 小图宽度 */const unsigned int width_2, /* 小图高度 */const unsigned int height_2,
            /* 大[0],中[1],小[2]图的宽度*高度的值,为空表示自动计算 */const unsigned int sizeWidthHeight[],
            /* 帧号 */const unsigned int frameIndex,/* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime);
    /* 设置pFrame中的数据,不能设置已经被设置的数据 */
    void image_frame_set__OI_2(FsObjectImageFrame * const pFrame, /* 要设置的数据,必须是被支持的有效数据类型 */FsObject * const pObject, /* 可取:大[0],中[1],小[2]图 */const unsigned char sizetype);
    /* 清空pFrame中的数据 */
    void image_frame_clear_data(FsObjectImageFrame * const pFrame, /* 要清空的数据类型,用宏定义,只能清空一种数据,且此数据必须存在 */const unsigned long type);
    /*
     * 根据文件创建一个FsObjectImageRAW实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL;
     */
    FsObjectImageRAW* image_frame_raw_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height,
            /* raw数据的类型(0-无效;1-yuv420p) */ const unsigned char type);
    /*
     * 根据文件创建一个FsObjectImageRGB实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL;
     */
    FsObjectImageRGB* image_frame_rgb_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height);
    /* 准备pFrame中的各种数据,成功返回1,失败返回-1 */
    signed char image_frame_prapare(FsObjectImageFrame * const pFrame, const unsigned long type);
#ifdef	__cplusplus
}
#endif

#endif	/* IMAGEFRAME_H */

