/* 
 * File:   ImageDib.h
 * Author: fslib
 *
 * Created on 2013年5月15日, 下午1:43
 */

#ifndef IMAGEDIB_H
#define IMAGEDIB_H
#include <pthread.h>
#include "../Fs/String.h"
#include "../Fs/Memery.h"
#include "../publicDefine.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* 图像颜色类型 */
    typedef enum {
        /* 不确定值,一般表示与原图相同 */
        ImageColorType_Unknown = 0,
#define ImageColorType_Unknown ImageColorType_Unknown
        /* 彩色RGB */
        ImageColorType_RGB = 1,
#define ImageColorType_RGB ImageColorType_RGB
        /* 彩色BGR */
        ImageColorType_BGR = 2,
#define ImageColorType_BGR ImageColorType_BGR
        /* 彩色YCbCr */
        ImageColorType_YCbCr = 3,
#define ImageColorType_YCbCr ImageColorType_YCbCr
        /* 灰度图 */
        ImageColorType_Gray = 4,
#define ImageColorType_Gray ImageColorType_Gray
        /* yuv420p图像 */
        ImageColorType_YUV420p = 5,
#define ImageColorType_YUV420p ImageColorType_YUV420p
        /* 彩色BGR */
        ImageColorType_GBR = 6
#define ImageColorType_GBR ImageColorType_GBR
        // IJL_RGBA_FPX = 5,
        //  IJL_YCBCRA_FPX = 6 
    } ImageColorType;

    /* 裸数据对象,包含位图数据和YCbCr数据 */
    typedef struct {
        /* 图像宽度 */
        unsigned int width;
        /* 图像高度 */
        unsigned int height;
        /* 图像颜色类型 */
        ImageColorType colorType;
        /* 图像数据的储存宽度,对y**数据表示y一行的宽度,其他的按比例缩减 */
        unsigned int lineBits;
        /* 图像数据长度 */
        unsigned int dataLenth;
        /* 图像数据 */
        unsigned char *data;
        /* 线程同步锁,仅用于多线程，须在各线程中自已实现 */
        pthread_mutex_t mutex;
    } ImageDib;
    /* 图像拷贝,可实现图像转换 */
    void image_dib_copy(/* 目标图数据 */unsigned char data_dst1[], /* 目标图数据,如yuv用于存u */unsigned char data_dst2[]
            , /* 目标图数据,如yuv用于存v */unsigned char data_dst3[], /* 目标图数据的储存宽度 */const unsigned int lineBits_dst
            , /* 目标图像颜色类型 */ const ImageColorType colorType_dst
            , /* 原图数据 */const unsigned char data_src1[], /* 原图数据,如yuv用于存u */const unsigned char data_src2[]
            , /* 原图数据,如yuv用于存v */const unsigned char data_src3[], /* 原图数据的储存宽度 */const unsigned int lineBits_src
            , /* 原图像颜色类型 */ const ImageColorType colorType_src
            , /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height);
    /*
     * 创建一个ImageDib实例;
     * 返回创建的实例指针;
     * width:图像宽度;
     * height:图像高度;
     * colorType:图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_new__IO(const unsigned int width, const unsigned int height, const ImageColorType colorType, const unsigned long lineBits);
    /*
     * 从BMP图像创建一个ImageDib实例;
     * 返回创建的实例指针;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_new_from_bmp__IO(const unsigned char bmpdata[], const unsigned int bmpLen, ImageColorType dstColorType, const unsigned int lineBits);
    /*
     * 从YCbCr图像创建一个ImageDib实例;
     * width:图像宽度;
     * height:图像高度;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,
     *     此值为ImageColorType_YCbCr时,lineBits无效,值恒为widthXheight;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0;
     * y:YCbCr中的Y;
     * cb:YCbCr中的cb;
     * cr:YCbCr中的cr;
     * yccLineBits:YCbCr中的Y Cb Cr一行占的字节数.
     */
    ImageDib* image_dib_new_from_YCbCr__IO(const unsigned int width, const unsigned int height,
            ImageColorType dstColorType, const unsigned int lineBits, unsigned char *y, unsigned char *cb, unsigned char *cr, const unsigned int yccLineBits);
    /*
     * 从dib图像创建一个ImageDib实例,可实现复制和格式转换;
     * pDib:源图;
     * dstColorType:图像颜色类型,为0时表示与源图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,
     *     此值为ImageColorType_YCbCr时,lineBits无效,值恒为widthXheight;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_new_from_dib__IO(const ImageDib* pDib, ImageColorType dstColorType, const unsigned long lineBits);
    /*
     * 从文件创建一个ImageDib实例;
     * 返回创建的实例指针;
     * fileName:图像文件名;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_new_from_file__IO(const char *fileName, ImageColorType dstColorType, const unsigned int lineBits);
    /*
     * 删除ppDib指向的实例指针指向的对象;
     * 并把ppDib指向的实例指针设为0.
     */
    void image_dib_delete__OI(ImageDib** ppDib);
    /* 删除pDib指向的对象 */
    void image_dib_delete_for_gtk__OI(unsigned char *p, ImageDib* pDib);
    /* 
     * 把pDib按指定的宽度(width)和高度(height)进行快速缩放,边缘可能会有问题;
     * 返回缩放后的ImageDib对像指针;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_scaling_fast__IO(const ImageDib* pDib, const unsigned int width, const unsigned int height, ImageColorType dstColorType, const unsigned int lineBits);
    /* 调整灰度图的大小 */
    void image_dib_resize_gray(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */register const unsigned int dwidth, /* 调整后的高度 */register const unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep,
            /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ register unsigned int sxStep);
    /* 调整灰度图的大小 */
    void image_dib_resize_gray_fast(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */register const unsigned int dwidth, /* 调整后的高度 */register const unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep,
            /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ register unsigned int sxStep);
    /* 
     * 截取pDib在坐标(x1,y1)与(x2,y2)之间图像，不会判断是否越界，且(x1,y1)必须在(x2,y2)左上角;
     * 返回截取到的图像;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_cutByPoint__IO(const ImageDib* pDib, const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2,
            ImageColorType dstColorType, const unsigned int lineBits);
    /* 
     * 在pDib中围绕坐标(x,y)截出最大宽度为maxWidth，最大高度为maxHeight的图像;
     * 返回截取到的图像;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_CutByMaxWidthHeigh__IO(const ImageDib* pDib, const unsigned int x, const unsigned int y,
            const unsigned int maxWidth, const unsigned int maxHeight, const ImageColorType dstColorType, const unsigned int lineBits);
    /* 
     * 把pDib图像上下翻转;
     * pDib必须为有效值且不为空;
     * 返回翻转后的图像指针;
     * dstColorType:图像颜色类型,为0时表示与原图相同,
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_upsideDown__IO(const ImageDib* pDib, const ImageColorType dstColorType, const unsigned int lineBits);
    /* 对单通道的图像调整大小 */
    void image_dib_resize_ch1(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep
            , /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep);
    /* 对单通道的图像调整大小并把图像扩展为3通道 */
    void image_dib_resize_ch1_to_ch3(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep
            , /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep);
    /* 对单通道的图像调整大小并把图像扩展为3通道,原图像的深度为16位 */
    void image_dib_resize_ch1_16bit_to_ch3(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep
            , /* 原图数据 */register const unsigned short sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ unsigned int sxStep
            , /* 转换时从第几个bit开始取[0-15] */const unsigned char startBit);
    /* 对3通道的图像调整大小 */
    void image_dib_resize_ch3(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep
            , /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep);
    /* 对单通道的图像调整大小,旋转,并输出目标图像中掩码区域内的图像 */
    void image_dib_resize_rotate_mask_dst_ch1(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ const unsigned int dheight
            , /* 调整后的储存宽度 */register const unsigned int dxStep
            , /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep
            , /* x方向上的缩放比例 */float x_scaling, /* y方向上的缩放比例 */float y_scaling, /* 旋转的弧度 */ double radian
            , /* 原图中心点在目标图上的坐标x */const int xc, /* 原图中心点在目标图上的坐标y */const int yc
            , /* 目标图像上的掩码,宽高与目标图像相同 */ const unsigned char mask_dst[], /* 掩码的储存宽度 */register const unsigned int mask_dstStep, /* 匹配的掩码值 */ const unsigned char mask_dstValue
            , /* 掩码的外接矩形,分别为左上角的x1,y1,右下角的x2,y2坐标,为空表示全图且函数运算后不回写坐标 */ unsigned int *pMask_dstPosition);
    /* 对单通道的图像调整大小,旋转,并输出目标图像中掩码区域内的图像,掩码图中横向移动一列需要跳过一字节 */
    void image_dib_resize_rotate_mask_skip1_dst_ch1(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ const unsigned int dheight
            , /* 调整后的储存宽度 */register const unsigned int dxStep
            , /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep
            , /* x方向上的缩放比例 */float x_scaling, /* y方向上的缩放比例 */float y_scaling, /* 旋转的弧度 */ double radian
            , /* 原图中心点在目标图上的坐标x */const int xc, /* 原图中心点在目标图上的坐标y */const int yc
            , /* 目标图像上的掩码,宽高与目标图像相同 */ const unsigned char mask_dst[], /* 掩码的储存宽度 */register const unsigned int mask_dstStep, /* 匹配的掩码值 */ const unsigned char mask_dstValue
            , /* 掩码的外接矩形,分别为左上角的x1,y1,右下角的x2,y2坐标,为空表示全图且函数运算后不回写坐标 */ unsigned int *pMask_dstPosition);
    /* 对3通道的图像调整大小,旋转,并输出目标图像中掩码区域内的图像 */
    void image_dib_resize_rotate_mask_dst_ch3(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ const unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep,
            /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep,
            /* x方向上的缩放比例 */float x_scaling, /* y方向上的缩放比例 */float y_scaling, /* 旋转的弧度 */ double radian,
            /* 原图中心点在目标图上的坐标x */const int xc, /* 原图中心点在目标图上的坐标y */const int yc,
            /* 目标图像上的掩码,宽高与目标图像相同 */register const unsigned char mask_dst[], /* 掩码的储存宽度 */register const unsigned int mask_dstStep, /* 匹配的掩码值 */ const unsigned char mask_dstValue,
            /* 掩码的外接矩形,分别为左上角的x1,y1,右下角的x2,y2坐标,为空表示全图且函数运算后不回写坐标 */ unsigned int *pMask_dstPosition);

    /* 对3通道的图像对称,调整大小,旋转,并输出目标图像中掩码区域内的图像 */
    void image_dib_symmetry_resize_rotate_mask_dst_ch3(/* 调整后的数据 */ unsigned char dbuffer[]
            , /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ const unsigned int dheight, /* 调整后的储存宽度 */ const unsigned int dxStep
            , /* 原图数据 */ const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep
            , /* 对称方式,1-左右对称,2-上下对称 */ const unsigned char symmetry, /* x方向上的缩放比例 */float x_scaling, /* y方向上的缩放比例 */float y_scaling, /* 旋转的弧度 */ double radian
            , /* 原图中心点在目标图上的坐标x */const int xc, /* 原图中心点在目标图上的坐标y */const int yc
            , /* 目标图像上的掩码,宽高与目标图像相同 */ const unsigned char mask_dst[], /* 掩码的储存宽度 */ const unsigned int mask_dstStep, /* 匹配的掩码值 */ const unsigned char mask_dstValue
            , /* 掩码的外接矩形,分别为左上角的x1,y1,右下角的x2,y2坐标,为空表示全图且函数运算后不回写坐标 */ unsigned int *pMask_dstPosition);
    /* 单通道的旋转图像 */
    void image_dib_rotate_ch1(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ register const unsigned int dwidth, /* 调整后的高度 */register const unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep,
            /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep,
            /* 旋转的弧度 */ double radian);
    /* 3通道的旋转图像 */
    void image_dib_rotate_ch3(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ register const unsigned int dwidth, /* 调整后的高度 */register const unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep,
            /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ const unsigned int sxStep,
            /* 旋转的弧度 */ double radian);
    /* 旋转图像 */
    void image_dib_rotate(/* 调整后的数据 */ register unsigned char dbuffer[], /* 调整后的宽度 */ register const unsigned int dwidth, /* 调整后的高度 */register const unsigned int dheight, /* 调整后的储存宽度 */register const unsigned int dxStep,
            /* 原图数据 */register const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ register unsigned int sxStep,
            /* 旋转的弧度 */ double radian, /* 通道数 */unsigned char nchannel);
    /* 对称+旋转图像 */
    void image_dib_symmetry_rotate(/* 调整后的数据 */ unsigned char dbuffer[], /* 调整后的宽度 */ const unsigned int dwidth, /* 调整后的高度 */ const unsigned int dheight, /* 调整后的储存宽度 */ const unsigned int dxStep
            , /* 原图数据 */ const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */ unsigned int sxStep
            , /* 对称方式,1-左右对称,2-上下对称 */ const unsigned char symmetry, /* 旋转的弧度 */ double radian, /* 通道数 */unsigned char nchannel);
    /* 对称图像 */
    void image_dib_symmetry_ch1(/* 调整后的数据 */ unsigned char dbuffer[], /* 调整后的储存宽度 */ const unsigned int dxStep, /* 宽度 */ const unsigned int width, /* 高度 */register unsigned int height
            , /* 原图数据 */ const unsigned char sbuffer[], /* 原图的储存宽度 */ unsigned int sxStep
            , /* 对称方式,1-左右对称,2-上下对称 */ const unsigned char symmetry);
    /* 对称图像 */
    void image_dib_symmetry_ch3(/* 调整后的数据 */ unsigned char dbuffer[], /* 调整后的储存宽度 */ const unsigned int dxStep, /* 宽度 */ const unsigned int width, /* 高度 */register unsigned int height
            , /* 原图数据 */ const unsigned char sbuffer[], /* 原图的储存宽度 */ unsigned int sxStep
            , /* 对称方式,1-左右对称,2-上下对称 */ const unsigned char symmetry);
    /* 获取pDib转换为bmp后的数据大小 */
    unsigned int image_dib_get_bmpDataLenth(const ImageDib* pDib);
    /* 把pDib图像转为BMP图像 */
    void image_dib_to_bmpData(const ImageDib* pDib, /* 存放bmp数据的buffer空间 */unsigned char buffer[]);

    /* 把pDib图像转为BMP图像 */
    FsString* image_dib_to_bmpData__IO(const ImageDib* pDib);
    /* 
     * 把pDib保存到文件;
     * fileName:保存的文件名;
     * 无需保存返回0;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2;
     * 转换失败返回-3.
     */
    char image_dib_svae_to_file(const ImageDib* pDib, const char fileName[]);
    /* 设置单通道图像的值 */
    void image_dib_set_ch1(/* 数据 */ register unsigned char buffer[],
            /* 宽度 */ register unsigned int width, /* 高度 */const unsigned int height, /* 储存宽度 */register const unsigned int xStep,
            /* 通道值 */ const unsigned char value);
    /* 设置3通道图像的值 */
    void image_dib_set_ch3(/* 数据 */ register unsigned char buffer[],
            /* 宽度 */ register unsigned int width, /* 高度 */ const unsigned int height, /* 储存宽度 */register const unsigned int xStep,
            /* 通道值,依次为3通道的值 */ const unsigned int value);
    /* 复制单通道图像的值 */
    void image_dib_copy_ch1(/* 目标数据指针 */ register unsigned char dbuffer[], /* 源数据指针 */ register const unsigned char sbuffer[]
            , /* 宽度 */ register unsigned int width, /* 高度 */ register unsigned int height, /* 目标储存宽度 */register const unsigned int dxStep
            , /* 源储存宽度 */register const unsigned int sxStep);
    /* 复制3通道图像的值 */
    void image_dib_copy_ch3(/* 目标数据指针 */ register unsigned char dbuffer[], /* 源数据指针 */ register const unsigned char sbuffer[],
            /* 宽度 */ register unsigned int width, /* 高度 */ register unsigned int height, /* 目标储存宽度 */register const unsigned int dxStep,
            /* 源储存宽度 */register const unsigned int sxStep);
    /* 单通道到3通道转换,单通道的深度为8位 */
    void image_dib_ch1_8bit_to_ch3(/* 目标数据指针 */ register unsigned char dbuffer[], /* 源数据指针 */ register const unsigned char sbuffer[],
            /* 宽度 */ register unsigned int width, /* 高度 */ register unsigned int height, /* 目标储存宽度 */register unsigned int dxStep,
            /* 源储存宽度 */register unsigned int sxStep);
    /* 单通道到3通道转换,单通道的深度为16位 */
    void image_dib_ch1_16bit_to_ch3(/* 目标数据指针 */ register unsigned char dbuffer[], /* 源数据指针 */ register const unsigned short sbuffer[],
            /* 宽度 */ register unsigned int width, /* 高度 */ register unsigned int height, /* 目标储存宽度 */register unsigned int dxStep,
            /* 源储存宽度,必须为2的倍数 */register unsigned int sxStep, /* 转换时从第几个bit开始取[0-15] */const unsigned char startBit);
    /* 单通道到3通道转换,单通道的深度为16位,高8位作为一通道保存 */
    void image_dib_ch1_16bit_to_ch3_hight_color(/* 目标数据指针 */ register unsigned char dbuffer[], /* 源数据指针 */ register const unsigned short sbuffer[],
            /* 宽度 */ register unsigned int width, /* 高度 */ register unsigned int height, /* 目标储存宽度 */register unsigned int dxStep,
            /* 源储存宽度,必须为2的倍数 */register unsigned int sxStep, /* 高8位使用的颜色通道,[0-2] */const unsigned char colorChannel);
    /* 对单通道图像画线,必须保证所有点不越界 */
    void image_dib_draw_lines_ch1(/* 数据指针 */ unsigned char buffer[], /* 储存宽度 */register const unsigned int xStep,
            /* 起始位置x坐标 */unsigned int x1, /* 起始位置y坐标 */unsigned int y1,
            /* 结束位置x坐标 */unsigned int pX2[], /* 结束位置y坐标 */unsigned int pY2[], /* 结束点的数量 */unsigned int pointCount,
            /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */unsigned int color,
            /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type);
    /* 对3通道图像画线,必须保证所有点不越界 */
    void image_dib_draw_lines_ch3(/* 数据指针 */ unsigned char buffer[], /* 储存宽度 */register const unsigned int xStep,
            /* 起始位置x坐标 */unsigned int x1, /* 起始位置y坐标 */unsigned int y1,
            /* 结束位置x坐标 */unsigned int pX2[], /* 结束位置y坐标 */unsigned int pY2[], /* 结束点的数量 */unsigned int pointCount,
            /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */unsigned int color,
            /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type);
    /* 对单通道图像填充 */
    void image_dib_fill_ch1(/* 数据指针 */ unsigned char buffer[]
            , /* 宽度 */ unsigned int width, /* 高度 */ unsigned int height, /* 储存宽度 */ const unsigned int xStep
            , /* 边界颜色 */unsigned char edgeColor, /* 填充颜色 */unsigned char fillColor
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对3通道图像填充 */
    void image_dib_fill_ch3(/* 数据指针 */ unsigned char buffer[]
            , /* 宽度 */ unsigned int width, /* 高度 */ unsigned int height, /* 储存宽度 */ const unsigned int xStep
            , /* 边界颜色 */unsigned int edgeColor, /* 填充颜色 */unsigned int fillColor
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对单通道图像画线并填充,必须保证所有点不越界 */
    void image_dib_draw_lines_and_fill_ch1(/* 数据指针 */ unsigned char buffer[], /* 储存宽度 */register const unsigned int xStep
            , /* 起始位置x坐标 */unsigned int x1, /* 起始位置y坐标 */unsigned int y1
            , /* 结束位置x坐标 */unsigned int pX2[], /* 结束位置y坐标 */unsigned int pY2[], /* 结束点的数量 */unsigned int pointCount
            , /* 线条颜色 */unsigned char lineColor, /* 填充颜色 */unsigned char fillColor, /* 唯一色,填充时基于此色找边界 */unsigned char uniqueColor
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对3通道图像画线并填充,必须保证所有点不越界 */
    void image_dib_draw_lines_and_fill_ch3(/* 数据指针 */ unsigned char buffer[], /* 储存宽度 */register const unsigned int xStep
            , /* 起始位置x坐标 */unsigned int x1, /* 起始位置y坐标 */unsigned int y1
            , /* 结束位置x坐标 */unsigned int pX2[], /* 结束位置y坐标 */unsigned int pY2[], /* 结束点的数量 */unsigned int pointCount
            , /* 线条颜色 */unsigned int lineColor, /* 填充颜色 */unsigned int fillColor, /* 唯一色,填充时基于此色找边界 */unsigned int uniqueColor
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对3通道图像画线,越界的边不画 */
    void image_dib_draw_lines_point_ch3(/* 数据指针 */ unsigned char buffer[], /* 宽度 */ int width, /* 高度 */ int height,
            /* 储存宽度 */register const unsigned int xStep, /* 点,[0]表示x,[1]表示y */ int (*const point) [2], /* 点的数量 */unsigned int pointCount,
            /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */unsigned int color,
            /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type);
    /* 对单通道图像画区域,越界的边不画 */
    void image_dib_draw_area_ch1(/* 数据指针 */ unsigned char buffer[], /* 宽度 */ int width, /* 高度 */ int height,
            /* 储存宽度 */register const unsigned int xStep, /* 点,[0]表示x,[1]表示y */ int (*const point) [2], /* 点的数量 */unsigned int pointCount,
            /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */unsigned int color,
            /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type);
    /* 对3通道图像画区域,越界的边不画 */
    void image_dib_draw_area_ch3(/* 数据指针 */ unsigned char buffer[], /* 宽度 */ int width, /* 高度 */ int height,
            /* 储存宽度 */register const unsigned int xStep, /* 点,[0]表示x,[1]表示y */ int (*const point) [2], /* 点的数量 */unsigned int pointCount,
            /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */unsigned int color,
            /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type);
    /* 对单通道图像区域进行填充,越界的不填充 */
    void image_dib_fill_area_ch1(/* 数据指针,可以是共享buffer中的空间 */ unsigned char buffer_cin[], /* 宽度 */ int width, /* 高度 */ int height
            , /* 储存宽度 */register const unsigned int xStep, /* 点,[0]表示x,[1]表示y */ int (*const point) [2], /* 点的数量 */unsigned int pointCount
            , /* 颜色 */unsigned char color, /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 对3通道图像区域进行填充,越界的不填充 */
    void image_dib_fill_area_ch3(/* 数据指针,可以是共享buffer中的空间 */ unsigned char buffer_cin[], /* 宽度 */ int width, /* 高度 */ int height
            , /* 储存宽度 */register const unsigned int xStep, /* 点,[0]表示x,[1]表示y */ int (*const point) [2], /* 点的数量 */unsigned int pointCount
            , /* 颜色 */unsigned int color, /* 颜色填充方式:0-copy,1-and,2-or,3-nor */unsigned char type
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 旋转图像上的点,目标点与原始点可以使用相同内存 */
    void image_dib_point_rotate(/* 目标点,[0]表示x,[1]表示y */ int (*const point_dst) [2], /* 原始点,[0]表示x,[1]表示y */ int (*const point_src) [2],
            /* 点的数量 */unsigned int pointCount, /* 中心点x坐标 */ int x0, /* 中心点y坐标 */int y0, /* 旋转的弧度 */ double radian);
    /* 图像平滑 */
    void image_dib_average(/* 目标数据指针,可以使用sbuffer,也可以是共享buffer中的空间 */ unsigned short dbuffer_cin[]
            , /* 源数据指针,可以是共享buffer中的空间 */ const unsigned short sbuffer_cin[]
            , /* 源数据指针上面blockHeight/2高度的数据,储存宽度为sxStep,可以是共享buffer中的空间,可为空 */ const unsigned short sbuffer_head_cin[]
            , /* 源数据指针图像结束后blockHeight-blockHeight/2高度的数据,储存宽度为sxStep,可以是共享buffer中的空间,可为空 */ const unsigned short sbuffer_tail_cin[]
            , /* 宽度 */ unsigned int width, /* 高度 */ unsigned int height, /* 目标储存宽度 */ unsigned int dxStep
            , /* 源储存宽度 */ unsigned int sxStep, /* 块的宽度 */unsigned short blockWidth, /* 块的高度 */unsigned short blockHeight
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 图像平滑并二值化 */
    void image_dib_average_and_binaryzation(/* 目标数据指针,可以使用sbuffer,也可以是共享buffer中的空间 */ unsigned char dbuffer_cin[]
            , /* 源数据指针,可以是共享buffer中的空间 */ const unsigned short sbuffer_cin[]
            , /* 源数据指针上面blockHeight/2高度的数据,储存宽度为sxStep,可以是共享buffer中的空间,可为空 */ const unsigned short sbuffer_head_cin[]
            , /* 源数据指针图像结束后blockHeight-blockHeight/2高度的数据,储存宽度为sxStep,可以是共享buffer中的空间,可为空 */ const unsigned short sbuffer_tail_cin[]
            , /* 宽度 */ unsigned int width, /* 高度 */ unsigned int height, /* 目标储存宽度 */ unsigned int dxStep
            , /* 源储存宽度 */ unsigned int sxStep, /* 块的宽度 */unsigned short blockWidth, /* 块的高度 */unsigned short blockHeight
            , /* 二值化阈值 */const unsigned short threshold
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 使用填充的方法查找图像中的目标,返回找到的目标个数,在dbuffer__in_shareBuffer为空时遍历到某行没有特殊点后会退出,第一个目标位置会被填充为1,以此类推 */
    unsigned int image_dib_get_object_by_fill(/* 是多可输出的目标数量,不超过253 */unsigned int targetCount
            , /* 输出目标左上角的x坐标,为空表示不输出 */unsigned int x1[], /* 输出目标左上角的y坐标,x1不为空时输出 */ unsigned int y1[]
            , /* 输出目标右下角的x坐标,x1不为空时输出 */unsigned int x2[], /* 输出目标右下角的y坐标,x1不为空时输出 */unsigned int y2[]
            , /* 目标数据指针,可以和sbuffer__in_shareBuffer使用相同空间,也可以是共享buffer中的空间,为空表示不填充 */ unsigned char dbuffer__in_shareBuffer[]
            , /* 源数据指针,可以是共享buffer中的空间,在dbuffer__in_shareBuffer为空时第一行数据必须有特殊点 */ const unsigned char sbuffer__in_shareBuffer[]
            , /* 宽度 */ unsigned int width, /* 高度 */ unsigned int height, /* 目标储存宽度 */ unsigned int dxStep, /* 源储存宽度 */ unsigned int sxStep
            , /* 填充时的起始颜色 */unsigned char color, /* 填充时的起始颜色 */unsigned char colorStep
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 把rgb转ycc,最高字节不变 */
    unsigned int image_dib_rgb_to_ycc(const unsigned int rgb);
    /* 把ycc转rgb,最高字节不变 */
    unsigned int image_dib_ycc_to_rgb(const unsigned int ycc);
    void image_dib_test();

#ifdef __cplusplus
}
#endif

#endif /* IMAGEDIB_H */

