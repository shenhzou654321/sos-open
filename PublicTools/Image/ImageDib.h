/* 
 * File:   ImageDib.h
 * Author: fslib
 *
 * Created on 2013年5月15日, 下午1:43
 */

#ifndef IMAGEDIB_H
#define	IMAGEDIB_H
#include <pthread.h>
#include "../Fs/String.h"
#include "../publicDefine.h"
#ifdef	__cplusplus
extern "C" {
#endif
    /* 图像颜色类型 */
    typedef enum {
        /* 不确定值,一般表示与原图相同 */
        ImageColorType_Unknown = 0,
        /* 彩色RGB */
        ImageColorType_RGB = 1,
        /* 彩色BGR */
        ImageColorType_BGR = 2,
        /* 彩色YCbCr */
        ImageColorType_YCbCr = 3,
        /* 灰度图 */
        ImageColorType_Gray = 4,
        /* yuv420p图像 */
        ImageColorType_YUV420p = 5,
        /* 彩色BGR */
        ImageColorType_GBR = 6
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
        /* 图像数据的储存宽度 */
        unsigned int lineBits;
        /* 图像数据长度 */
        unsigned int dataLenth;
        /* 图像数据 */
        unsigned char *data;
        /* 线程同步锁,仅用于多线程，须在各线程中自已实现 */
        pthread_mutex_t mutex;
    } ImageDib;
    /* 图像拷贝,可实现图像转换 */
    void image_dib_copy(/* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 原图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,ImageColorType_YUV420p=5,ImageColorType_GBR = 6 */ const ImageColorType colorType_src,
            /* 原图数据的储存宽度 */const unsigned long lineBits_src, /* 原图数据 */const unsigned char data_src[],
            /* 目标图像颜色类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,ImageColorType_YUV420p=5,ImageColorType_GBR = 6 */ const ImageColorType colorType_dst,
            /* 目标图数据的储存宽度 */const unsigned long lineBits_dst, /* 目标图数据 */unsigned char data_dst[]);
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
     *     支持ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,
     *     此值为ImageColorType_YCbCr时,lineBits无效,值恒为widthXheight;
     * lineBits:图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0.
     */
    ImageDib* image_dib_new_from_bmp__IO(const FsString *pBmp, ImageColorType dstColorType, const unsigned int lineBits);
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
    void image_dib_resize_gray(/* 调整后的数据 */unsigned char dbuffer[], /* 调整后的宽度 */const unsigned int dwidth, /* 调整后的高度 */const unsigned int dheight, /* 调整后的储存宽度 */const unsigned int dxStep,
            /* 原图数据 */const unsigned char sbuffer[], /* 原图宽度 */const unsigned int swidth, /* 原图高度 */const unsigned int sheight, /* 原图的储存宽度 */const unsigned int sxStep);
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

#ifdef	__cplusplus
}
#endif

#endif	/* IMAGEDIB_H */

