/* 
 * File:   ImageFrame.h
 * Author: fslib
 *
 * Created on 2013年4月27日, 下午1:29
 */
#if !defined IMAGEFRAME_H && !defined JAVA_IN_C

#define IMAGEFRAME_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Object.h"
#include "../../PublicTools/Image/ImageJpeg.h"
#include "../../PublicTools/Media/VideoH264.h"
#include "../../PublicTools/Media/VideoH265.h"
#ifdef __cplusplus
extern "C" {
#endif
#define ImageFrame_RAW_0 (0x1LU<<0)
#define ImageFrame_RAW_1 (0x1LU<<1)
#define ImageFrame_RAW_2 (0x1LU<<2)
#define ImageFrame_H264_0 (0x1LU<<3)
#define ImageFrame_H264_1 (0x1LU<<4)
#define ImageFrame_H264_2 (0x1LU<<5)
#define ImageFrame_H265_0 (0x1LU<<6)
#define ImageFrame_H265_1 (0x1LU<<7)
#define ImageFrame_H265_2 (0x1LU<<8)
#define ImageFrame_RGB_0 (0x1LU<<9)
#define ImageFrame_RGB_1 (0x1LU<<10)
#define ImageFrame_RGB_2 (0x1LU<<11)
#define ImageFrame_YCC_0 (0x1LU<<12)
#define ImageFrame_YCC_1 (0x1LU<<13)
#define ImageFrame_YCC_2 (0x1LU<<14)
#define ImageFrame_YUV420P_0 (0x1LU<<15)
#define ImageFrame_YUV420P_1 (0x1LU<<16)
#define ImageFrame_YUV420P_2 (0x1LU<<17)
    /* YCC3(y,cb,cr连续储存) */
#define ImageFrame_YCC3_0 (0x1LU<<18)
#define ImageFrame_YCC3_1 (0x1LU<<19)
#define ImageFrame_YCC3_2 (0x1LU<<20)
#define ImageFrame_Jpeg_0 (0x1LU<<21)
#define ImageFrame_Jpeg_1 (0x1LU<<22)
#define ImageFrame_Jpeg_2 (0x1LU<<23)
#define ImageFrame_Hedge_0 (0x1LU<<24)
#define ImageFrame_Hedge_1 (0x1LU<<25)
#define ImageFrame_Hedge_2 (0x1LU<<26)
#define ImageFrame_Vedge_0 (0x1LU<<27)
#define ImageFrame_Vedge_1 (0x1LU<<28)
#define ImageFrame_Vedge_2 (0x1LU<<29)
#define ImageFrame_STATS (0x1LU<<30)
    /* ImageFrame_STATS锁定,目前只会在本地拉视频时会设置,锁定后pFrame中的FsObjectStats对象在传递过程中不会被覆盖 */
#define ImageFrame_STATS_LOCKED (0x1LU<<31)

    /* 可以单帧转换且消耗cpu比较小的数据,如yuv */
#define ImageFrame_Single_Light_0 (ImageFrame_RAW_0|ImageFrame_RGB_0|ImageFrame_YCC_0|ImageFrame_YUV420P_0|ImageFrame_YCC3_0)
#define ImageFrame_Single_Light_1 (ImageFrame_RAW_1|ImageFrame_RGB_1|ImageFrame_YCC_1|ImageFrame_YUV420P_1|ImageFrame_YCC3_1)
#define ImageFrame_Single_Light_2 (ImageFrame_RAW_2|ImageFrame_RGB_2|ImageFrame_YCC_1|ImageFrame_YUV420P_2|ImageFrame_YCC3_2)
#define ImageFrame_Single_Light (ImageFrame_Single_Light_0|ImageFrame_Single_Light_1|ImageFrame_Single_Light_2)
    /* 可以单帧转换但消耗cpu比较多的数据,如jpeg */
#define ImageFrame_Single_Heavy_0 (ImageFrame_Jpeg_0)
#define ImageFrame_Single_Heavy_1 (ImageFrame_Jpeg_1)
#define ImageFrame_Single_Heavy_2 (ImageFrame_Jpeg_2)
#define ImageFrame_Single_Heavy (ImageFrame_Single_Heavy_0|ImageFrame_Single_Heavy_1|ImageFrame_Single_Heavy_2)
    /* 可以单帧转换的数据,如yuv */
#define ImageFrame_Single_0 (ImageFrame_Single_Light_0|ImageFrame_Single_Heavy_0)
#define ImageFrame_Single_1 (ImageFrame_Single_Light_1|ImageFrame_Single_Heavy_1)
#define ImageFrame_Single_2 (ImageFrame_Single_Light_2|ImageFrame_Single_Heavy_2)
#define ImageFrame_Single (ImageFrame_Single_0|ImageFrame_Single_1|ImageFrame_Single_2)
    /* 不能单帧转换的数据,如h264 */
#define ImageFrame_Relative_0 (ImageFrame_H264_0|ImageFrame_H265_0)
#define ImageFrame_Relative_1 (ImageFrame_H264_1|ImageFrame_H265_1)
#define ImageFrame_Relative_2 (ImageFrame_H264_2|ImageFrame_H265_2)
#define ImageFrame_Relative (ImageFrame_Relative_0|ImageFrame_Relative_1|ImageFrame_Relative_2)
    /* 分析数据 */
#define ImageFrame_Analyse_0 (ImageFrame_Hedge_0|ImageFrame_Vedge_0)
#define ImageFrame_Analyse_1 (ImageFrame_Hedge_1|ImageFrame_Vedge_1)
#define ImageFrame_Analyse_2 (ImageFrame_Hedge_2|ImageFrame_Vedge_2)
#define ImageFrame_Analyse (ImageFrame_Analyse_0|ImageFrame_Analyse_1|ImageFrame_Analyse_2)

    /* 每种图的数据 */
#define ImageFrame_0 (ImageFrame_Single_0|ImageFrame_Relative_0|ImageFrame_Analyse_0)
#define ImageFrame_1 (ImageFrame_Single_1|ImageFrame_Relative_1|ImageFrame_Analyse_1)
#define ImageFrame_2 (ImageFrame_Single_2|ImageFrame_Relative_2|ImageFrame_Analyse_2)
#define ImageFrame_valid_RAW_0(__dataValidType) ((__dataValidType)&ImageFrame_RAW_0)
#define ImageFrame_valid_RAW_1(__dataValidType) ((__dataValidType)&ImageFrame_RAW_1)
#define ImageFrame_valid_RAW_2(__dataValidType) ((__dataValidType)&ImageFrame_RAW_2)
#define ImageFrame_valid_H264_0(__dataValidType) ((__dataValidType)&ImageFrame_H264_0)
#define ImageFrame_valid_H264_1(__dataValidType) ((__dataValidType)&ImageFrame_H264_1)
#define ImageFrame_valid_H264_2(__dataValidType) ((__dataValidType)&ImageFrame_H264_2)
#define ImageFrame_valid_H265_0(__dataValidType) ((__dataValidType)&ImageFrame_H265_0)
#define ImageFrame_valid_H265_1(__dataValidType) ((__dataValidType)&ImageFrame_H265_1)
#define ImageFrame_valid_H265_2(__dataValidType) ((__dataValidType)&ImageFrame_H265_2)
#define ImageFrame_valid_RGB_0(__dataValidType) ((__dataValidType)&ImageFrame_RGB_0)
#define ImageFrame_valid_RGB_1(__dataValidType) ((__dataValidType)&ImageFrame_RGB_1)
#define ImageFrame_valid_RGB_2(__dataValidType) ((__dataValidType)&ImageFrame_RGB_2)
#define ImageFrame_valid_YCC_0(__dataValidType) ((__dataValidType)&ImageFrame_YCC_0)
#define ImageFrame_valid_YCC_1(__dataValidType) ((__dataValidType)&ImageFrame_YCC_1)
#define ImageFrame_valid_YCC_2(__dataValidType) ((__dataValidType)&ImageFrame_YCC_2)
#define ImageFrame_valid_YUV420P_0(__dataValidType) ((__dataValidType)&ImageFrame_YUV420P_0)
#define ImageFrame_valid_YUV420P_1(__dataValidType) ((__dataValidType)&ImageFrame_YUV420P_1)
#define ImageFrame_valid_YUV420P_2(__dataValidType) ((__dataValidType)&ImageFrame_YUV420P_2)
#define ImageFrame_valid_YCC3_0(__dataValidType) ((__dataValidType)&ImageFrame_YCC3_0)
#define ImageFrame_valid_YCC3_1(__dataValidType) ((__dataValidType)&ImageFrame_YCC3_1)
#define ImageFrame_valid_YCC3_2(__dataValidType) ((__dataValidType)&ImageFrame_YCC3_2)
#define ImageFrame_valid_Jpeg_0(__dataValidType) ((__dataValidType)&ImageFrame_Jpeg_0)
#define ImageFrame_valid_Jpeg_1(__dataValidType) ((__dataValidType)&ImageFrame_Jpeg_1)
#define ImageFrame_valid_Jpeg_2(__dataValidType) ((__dataValidType)&ImageFrame_Jpeg_2)
#define ImageFrame_valid_Hedge_0(__dataValidType) ((__dataValidType)&ImageFrame_Hedge_0)
#define ImageFrame_valid_Hedge_1(__dataValidType) ((__dataValidType)&ImageFrame_Hedge_1)
#define ImageFrame_valid_Hedge_2(__dataValidType) ((__dataValidType)&ImageFrame_Hedge_2)
#define ImageFrame_valid_Vedge_0(__dataValidType) ((__dataValidType)&ImageFrame_Vedge_0)
#define ImageFrame_valid_Vedge_1(__dataValidType) ((__dataValidType)&ImageFrame_Vedge_1)
#define ImageFrame_valid_Vedge_2(__dataValidType) ((__dataValidType)&ImageFrame_Vedge_2)
#define ImageFrame_valid_STATS(__dataValidType) ((__dataValidType)&ImageFrame_STATS)
#define ImageFrame_valid_STATS_LOCKED(__dataValidType) ((__dataValidType)&ImageFrame_STATS_LOCKED)
    /* 是否可做为快照数据 */
#define ImageFrame_can_snapshort(___pFrame) ((ImageFrame_Single & (___pFrame)->dataValidType)\
    ||(ImageFrame_valid_H264_0((___pFrame)->dataValidType)&&FsObjectH264_can_snap((___pFrame)->data.h264[0][0]))\
    ||(ImageFrame_valid_H264_1((___pFrame)->dataValidType)&&FsObjectH264_can_snap((___pFrame)->data.h264[1][0]))\
    ||(ImageFrame_valid_H264_2((___pFrame)->dataValidType)&&FsObjectH264_can_snap((___pFrame)->data.h264[2][0]))\
    ||(ImageFrame_valid_H265_0((___pFrame)->dataValidType)&&FsObjectH265_can_snap((___pFrame)->data.h265[0][0]))\
    ||(ImageFrame_valid_H265_1((___pFrame)->dataValidType)&&FsObjectH265_can_snap((___pFrame)->data.h265[1][0]))\
    ||(ImageFrame_valid_H265_2((___pFrame)->dataValidType)&&FsObjectH265_can_snap((___pFrame)->data.h265[2][0])))
#pragma pack(push,1)

    /* 继承FsObject的RAW数据 */
#define FsObjectImageRAW_HeadLen ((unsigned int)FsPointer_to_long(((FsObjectImageRAW*)FsPointer(0))->data))

    typedef struct {
        FsObject object;
        /* 此类型暂不使用 */
        /* raw数据的类型(0-无效;1-yuv420p) */
        unsigned char type : 1;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 1 + 15)&0xF)];
        /* 数据,可外部设置 */
        unsigned char data[0];
    } FsObjectImageRAW;

    /* 继承FsObject的RGB数据 */
    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 数据,可外部设置 */
        unsigned char data[0];
    } FsObjectImageRGB;

    /* 继承FsObject的YCC数据 */
    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 数据,可外部设置 */
        unsigned char data[0];
    } FsObjectImageYCC;

    /* 继承FsObject的YUV420P数据 */
#define FsObjectImageYUV420P_HeadLen ((unsigned int)FsPointer_to_long(((FsObjectImageYUV420P*)FsPointer(0))->data))

    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 数据,可外部设置 */
        unsigned char data[0];
    } FsObjectImageYUV420P;

    /* 继承FsObject的YCC3数据(y,cb,cr连续储存) */
    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 数据,可外部设置 */
        unsigned char data[0];
    } FsObjectImageYCC3;

    /* 继承FsObject的灰度数据 */
    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 数据,可外部设置 */
        unsigned char data[0];
    } FsObjectImageGray;

    /* 继承FsObject的边缘数据 */
    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 数据,可外部设置 */
        short data[0];
    } FsObjectImageEdge;
#define FsObjectImageHedge FsObjectImageEdge
#define FsObjectImageVedge FsObjectImageEdge

    /* 继承FsObject的数据统计,主要用于跟踪数据之后的用途,把结果用于指导数据源进行合理的数据处理 */
    typedef struct {
        FsObject object;
        /* 填充 */
        unsigned char __[0xF - ((sizeof (FsObject) + 15)&0xF)];
        /* 解码掩码,用于跟踪解码需求,调整系统的解码方式 */
        unsigned long long decodeMask;
        /* 设置的解码掩码,用于跟踪解码需求,调整系统的解码方式 */
        unsigned long long decodeMask_set;
        /* 解码跟踪的参数--等待引用处理的次数,当值小于等于0时,把decodeMask_set值更新到decodeMask中 */
        int decodeWaitReferCount;
        /* 解码跟踪的参数--上一次的引用计数,在当前引用计数比decodeLastRefer小时,decodeWaitRefer减去此差值 */
        int decodeLastReferCount;
        /* 标记解码信息有无变化,0-无变化,1-有变化 */
        unsigned char decodeChanged : 1;
    } FsObjectStats;
#pragma pack(pop)

    /* 
     * 图像数据帧对象,继承FsObject结构默认定义大中小三种大小的图像数据;
     * 因结构复杂,请勿直接改变其成员的值;
     * 使用场景:
     *    在数据流传递过程中数据只增不减,仅在数据回收时删除被引用对象;
     */
    typedef struct {
        FsObject object;
        /* 拼接编号,不支持拼接应填0 */
        unsigned char stitchIndex;
        /* 图像拼接的左裁剪像素 */
        short cutLeft;
        /* 图像拼接的右裁剪像素 */
        short cutRight;
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
        unsigned long long dataValidType;
        /* 使用跟踪统计,只能串行执行,遇可能并行执行应设置FsObjectStats中的lock标志位,表示并行过程中此指针不会被替换 */
        FsObjectStats *stats;

        /* 图像数据 */
        struct {
            /* 裸数据 */
            FsObjectImageRAW *raw[3];
            /* 分别是:大[0],中[1],小[2]图的h264数据 */
            FsObjectH264 *h264[3][3];
            /* 分别是:大[0],中[1],小[2]图的h265数据 */
            FsObjectH265 *h265[3][3];
            /* rgb数据 */
            FsObjectImageRGB *rgb[3];
            /* ycc数据 */
            FsObjectImageYCC *ycc[3];
            /* yuv420p数据 */
            FsObjectImageYUV420P *yuv420p[3];
            /* ycc3数据(y,cb,cr连续储存) */
            FsObjectImageYCC3 *ycc3[3];
            /* jpeg数据 */
            FsObjectImageJpeg *jpeg[3];
            /* 水平边界图(sobel)	 */
            FsObjectImageEdge *hedge[3];
            /* 垂直边界图(sobel) */
            FsObjectImageEdge *vedge[3];
        } data; /* 图像数据 */
    } FsObjectImageFrame;

    /*
     * 根据文件创建一个ImageFrame实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL;
     */

    FsObjectImageFrame* image_frame_new__IO(/* 大图宽度 */const unsigned int width_0, /* 大图高度 */const unsigned int height_0
            , /* 中等大小图宽度 */const unsigned int width_1, /* 中等大小图高度 */const unsigned int height_1
            , /* 小图宽度 */const unsigned int width_2, /* 小图高度 */const unsigned int height_2
            , /* 大[0],中[1],小[2]图的宽度*高度的值,为空表示自动计算 */const unsigned int sizeWidthHeight[]
            , /* 帧号 */const unsigned int frameIndex, /* 拼接编号,不支持拼接应填0 */ const unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ const short cutLeft, /* 图像拼接的右裁剪像素 */ const short cutRight, /* 分类号 */const unsigned short classIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime);

    /* 设置pFrame中的数据,不能设置已经被设置的数据 */

    void image_frame_set__OI_2(FsObjectImageFrame * const pFrame, /* 要设置的数据,必须是被支持的有效数据类型 */FsObject * const pObject, /* 可取:大[0],中[1],小[2]图 */const unsigned char sizetype);

    /* 把pFrameSrc的数据设置到pFrame中,不能设置pFrame中已有数据,返回设置的数据掩码 */

    unsigned long long image_frame_set_data(FsObjectImageFrame * const pFrame, /* 把pFrameSrc的数据设置到pFrame中 */FsObjectImageFrame * const pFrameSrc, /* 要设置的数据掩码 */ unsigned long long type);

    /* 设置pFrame中的stats,是否会被更新依赖pFrame中的stats的lock标志位是否被设置 */

    void image_frame_set_stats(FsObjectImageFrame * const pFrame, /* 要设置的数据 */FsObjectStats * const pObjectStats);

    /* 线程安全地设置pFrame中的stats,是否会被更新依赖pFrame中的stats的lock标志位是否被设置 */

    void image_frame_set_stats_pthreadSafety(FsObjectImageFrame * const pFrame, /* 要设置的数据 */FsObjectStats * const pObjectStats);

    /* 为pFrame中的stats加锁标记 */

#define  image_frame_set_stats_locked(pFrame)  (pFrame)->dataValidType |= ImageFrame_STATS_LOCKED

    /* 清空pFrame中的数据 */

    void image_frame_clear_data(FsObjectImageFrame * const pFrame, /* 要清空的数据类型,用宏定义,只能清空一种数据,且此数据必须存在*/const unsigned long long type);

    /* 初始化pObjectBase为FsObjectImageRAW */

    void image_frame_raw_init_from_objectBase(FsObjectBase * const pObjectBase, /* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height
            , /* raw数据的类型(0-无效;1-yuv420p) */ const unsigned char type);

    /*
     * 创建一个FsObjectImageRAW实例;
     * 返回创建的实例指针.
     */

    FsObjectImageRAW* image_frame_raw_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height, /* raw数据的类型(0-无效;1-yuv420p) */ const unsigned char type);

    /*
     * 创建一个FsObjectImageRGB实例;
     * 返回创建的实例指针.
     */

    FsObjectImageRGB* image_frame_rgb_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height);

    /*
     * 创建一个FsObjectImageYCC实例;
     * 返回创建的实例指针.
     */

    FsObjectImageYCC* image_frame_ycc_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height);

    /* 初始化pObjectBase为FsObjectImageYUV420P */

    void image_frame_yuv420p_init_from_objectBase(FsObjectBase * const pObjectBase, /* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height);

    /*
     * 创建一个FsObjectImageYUV420P实例;
     * 返回创建的实例指针.
     */

    FsObjectImageYUV420P* image_frame_yuv420p_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height);

    /*
     * 创建一个FsObjectImageYCC3实例;
     * 返回创建的实例指针.
     */

    FsObjectImageYCC3* image_frame_ycc3_new__IO(/* 图像宽度 */const unsigned int width, /* 图像宽度 */const unsigned int height);

    /* 设置pYCC3的数据 */

    void image_frame_ycc3_set_value(FsObjectImageYCC3 * const pYCC3, /* 图像宽度 */ unsigned int width, /* 图像宽度,不能为0 */ unsigned int height, /* 储存宽度 */ const unsigned int lineBits
            , const unsigned char y, const unsigned char cb, const unsigned char cr);

    /*
     * 创建一个FsObjectImageGray实例;
     * 返回创建的实例指针.
     */

    FsObjectImageGray* image_frame_gray_new__IO(/* 图像宽度 */ unsigned int width, /* 图像宽度 */const unsigned int height
            , /* 类型,只支持FsObject_type_imageGray及子类 */ const enum FsObject_type type);

    /*
     * 创建一个FsObjectImageEdge实例;
     * 返回创建的实例指针.
     */

    FsObjectImageEdge* image_frame_edge_new__IO(/* 图像宽度 */ unsigned int width, /* 图像宽度 */const unsigned int height
            , /* 类型,只支持FsObject_type_imageGray及子类 */ const enum FsObject_type type);

    /*
     * 创建一个FsObjectStats实例;
     * 返回创建的实例指针.
     */

    FsObjectStats* image_frame_stats_new__IO();

    /* 准备pFrame中的各种数据,只支持单个数据类型,成功返回1,失败返回-1 */

    int image_frame_prapare(FsObjectImageFrame * const pFrame, /* 数据类型,只支持单个数据类型 */const unsigned long long type);

    /* 线程安全地准备pFrame中的各种数据,只支持单个数据类型,成功返回1,失败返回-1 */

    int image_frame_prapare_pthreadSafety(FsObjectImageFrame * const pFrame, /* 数据类型,只支持单个数据类型 */const unsigned long long type);

    /* 准备pFrame中的各种数据,支持多个数据类型组合,成功返回1,失败返回-1 */

    int image_frame_prapare_all(FsObjectImageFrame * const pFrame, /* 数据类型,支持多个数据类型组合 */unsigned long long type);

    /* 线程安全地准备pFrame中的各种数据,支持多个数据类型组合,成功返回1,失败返回-1 */

    int image_frame_prapare_all_pthreadSafety(FsObjectImageFrame * const pFrame, /* 数据类型,支持多个数据类型组合 */unsigned long long type);

    /* 获取pFrame中某类型的数据,成功返回数据,失败返回NULL */

    void *image_frame_get__IO(FsObjectImageFrame * const pFrame, /* 数据类型,只支持单个数据类型 */const unsigned long long type);

    /* 线程安全地获取pFrame中某类型的数据,成功返回数据,失败返回NULL */

    void *image_frame_get_pthreadSafety__IO(FsObjectImageFrame * const pFrame, /* 数据类型,只支持单个数据类型 */const unsigned long long type);

    /* 获取pObject中储存的0时区时间,失败返回默认值 */

    double image_frame_get_object_gmtTime(/* 是继承FsObject的对象 */const FsObject * const pObject, /* 如果获取失败返回的默认值 */double defaultTime);

    /* 获取pObject中储存的开机时间,失败返回默认值 */

    double image_frame_get_object_uptime(/* 是继承FsObject的对象 */const FsObject * const pObject, /* 如果获取失败返回的默认值 */double defaultTime);



#ifdef __cplusplus
}
#endif


#endif /* IMAGEFRAME_H */

