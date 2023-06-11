/* 
 * File:   ImageExtern.h
 * Author: fslib
 *
 * Created on 2019年2月28日, 上午11:34
 */

#ifndef IMAGEEXTERN_H
#define	IMAGEEXTERN_H
#include "../publicDefine.h"
#include "ImageDib.h"
#ifdef	__cplusplus
extern "C" {
#endif

    /* 扩展数据类型 */
    typedef enum {
        /* jpg--特殊数据类型(使用两字节标记数据类型,第一个字节的高4位为非0,不能与其他类型共享一个数据包) */
        ImageExternType_SPECIAL_jpg = 0xD8FF,
#define ImageExternType_SPECIAL_jpg  ImageExternType_SPECIAL_jpg
        /* bmp--特殊数据类型(使用两字节标记数据类型,第一个字节的高4位为非0,不能与其他类型共享一个数据包) */
        ImageExternType_SPECIAL_bmp = 0x4D42,
#define ImageExternType_SPECIAL_bmp  ImageExternType_SPECIAL_bmp
        /* RGB */
        ImageExternType_rgb = 0x01000000U,
#define ImageExternType_rgb  ImageExternType_rgb
        /* Gray */
        ImageExternType_gray = 0x02000000U,
#define ImageExternType_gray  ImageExternType_gray
        /* Line */
        ImageExternType_line = 0x10000100U,
#define ImageExternType_line  ImageExternType_line
        /* text */
        ImageExternType_text = 0x20000100U,
#define ImageExternType_text  ImageExternType_text
        /* 键值对 */
        ImageExternType_key = 0x10000000U,
#define ImageExternType_key  ImageExternType_key
    } ImageExternType;

    /* 键值对的对齐方式 */
    typedef enum {
        /* 按4字节对齐 */
        ImageExternType_keyA4 = 0x4,
#define ImageExternType_keyA4  ImageExternType_keyA4
        /* 按8字节对齐 */
        ImageExternType_keyA8 = 0x8,
#define ImageExternType_keyA8  ImageExternType_keyA8
    } ImageExternType_keyA;
    /* 扩展数据尾部可能填充的空间 */
#define ImageExtern_TailLen 16
    /* 对齐数据时,除key和数据区的长度外,编码时需要附加的长度 */
#define ImageExternType_keyA_headLen(__imageExternType_keyA) (1+1+1+4+(__imageExternType_keyA)-1+1)
    /* 根据键值数据中的值获取值的长度 */
#define ImageExternType_key_dataLen(__data) (*(((unsigned int *)(__data))-1))

    /*
     *  有两类数据,首字节高4位非0的数据,数据内存为特殊数据内容,如FFD8表示jpg数据;
     *      首字节高4位为0的数据,数据格式为:16字节固定头+可变头(可能没有)+数据(可能没有)+填充数据(一般用于注释,格式为\0+数据内容+\0,没有填充数据时为\0,即填充数据的长度为2及以下表示没有填充数据);
     *          16字节固定头按如下格式定义:4字节(首字节高4位固定为0+4位版本号+3字节类型)
     *              +3字节子头长度+1字节填充数据长度
     *              +6字节数据长度(不含16字节头)+2字节层数(层数一般用于显示)
     */
    struct ImageExternBase {
        /* 数据类型,首字节高4位固定为0+4位版本号+3字节类型 */
        unsigned int type;
        /* 子头长度(不包含16字节头的头长度),3字节 */
        unsigned int headLen;
        /* 数据长度(只包含数据区的长度) */
        unsigned long long len;
        /* 数据 */
        const unsigned char *data;
        /* 填充数据长度 */
        unsigned char fillLen;
        /* 层数,一般用于显示 */
        unsigned short layer;
    };
#pragma pack(push,1)

    /* RGB图像 */
    struct ImageExtern_RGB {
        struct ImageExternBase base;
        /* 图像宽度 */
        unsigned int width;
        /* 图像高度 */
        unsigned int height;
        /* 储存宽度 */
        unsigned int lineBits;
        /* 显示时左上角x方向上的边距 */
        unsigned int dx1;
        /* 显示时左上角y方向上的边距 */
        unsigned int dy1;
        /* 显示时右下角x方向上的边距 */
        unsigned int dx2;
        /* 显示时右下角y方向上的边距 */
        unsigned int dy2;
        /* x方向上的缩放比 */
        float zoomX;
        /* y方向上的缩放比 */
        float zoomY;
    };

    /* Gray图像 */
    struct ImageExtern_Gray {
        struct ImageExternBase base;
        /* 图像宽度 */
        unsigned int width;
        /* 图像高度 */
        unsigned int height;
        /* 储存宽度 */
        unsigned int lineBits;
        /* 显示时左上角x方向上的边距 */
        unsigned int dx1;
        /* 显示时左上角y方向上的边距 */
        unsigned int dy1;
        /* 显示时右下角x方向上的边距 */
        unsigned int dx2;
        /* 显示时右下角y方向上的边距 */
        unsigned int dy2;
        /* x方向上的缩放比 */
        float zoomX;
        /* y方向上的缩放比 */
        float zoomY;
        /* 颜色深度,支持8,16(16位时按边缘图处理) */
        unsigned int depth;
    };

    /* 线,数据区按x1,y1,x2,y2排布,占用16字节 */
    struct ImageExtern_Line {
        struct ImageExternBase base;
        /* 图像宽度 */
        unsigned int width;
        /* 图像高度 */
        unsigned int height;
        /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */
        unsigned int color;
        /* x方向上的缩放比 */
        float zoomX;
        /* y方向上的缩放比 */
        float zoomY;
        /* 线的数量 */
        unsigned int lineCount;
    };

    /* 文本,数据区为文本,占用16字节 */
    struct ImageExtern_Text {
        struct ImageExternBase base;
        /* 图像宽度 */
        unsigned int width;
        /* 图像高度 */
        unsigned int height;
        /* 三通道颜色,最高一字节固定为0 */
        unsigned int color;
        /* 文本坐标 */
        unsigned int x, y;
        /* 字体大小 */
        unsigned short charWidth, charHeight;
    };
#pragma pack(pop)
    /* 计算写入jpg数据需要的大空间 */
    unsigned int image_extern_special_jpg_write_size_get(/* 图片长度 */const unsigned int jpgLen);
    /* 把jpg数据写入rst_data中,返回写入的字节数 */
    unsigned int image_extern_special_jpg_write(/* 写入的空间,足够大 */unsigned char rst_data[], const unsigned char jpgData[], /* 图片长度 */const unsigned int jpgLen);
    /* 计算写入bmp数据需要的大空间 */
    unsigned int image_extern_special_bmp_write_size_get(/* 图片长度 */const unsigned int bmpLen);
    /* 把bmp数据写入rst_data中,返回写入的字节数 */
    unsigned int image_extern_special_bmp_write(/* 写入的空间,足够大 */unsigned char rst_data[], const unsigned char bmpData[], /* 图片长度 */const unsigned int bmpLen);
    /* 计算写入rgb数据需要的空间,在rst_fillLen不为空时返回头的大小,为空时返回头+数据+填充的大小 */
    unsigned int image_extern_rgb_write_size_get(/* 可为空,不为空时返回填充长度 */unsigned char *const rst_fillLen, /* 图片长度 */const unsigned int rgbLen
            , /* 注释长度 */const unsigned char commentLen);
    /* 把rgb数据头写入rst_data中,返回存放数据的指针,指针是指向rst_data空间的 */
    unsigned char* image_extern_rgb_write_head(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 填充的空间,足够大,为空表示填充数据写入rst_data_8的尾部 */unsigned char rst_fillData[]
            , /* 层数,一般用于显示 */ unsigned short layer, /* 注释长度 */ unsigned char commentLen, /* 注释 */const char comment[]
            , /* 图像宽度 */ const unsigned int width, /* 图像高度 */const unsigned int height, /* 储存宽度 */const unsigned int lineBits
            , /* 显示时左上角x方向上的边距 */const unsigned int dx1, /* 显示时左上角y方向上的边距 */const unsigned int dy1
            , /* 显示时右下角x方向上的边距 */ const unsigned int dx2, /* 显示时右下角y方向上的边距 */ const unsigned int dy2
            , /* x方向上的缩放比 */ const float zoomX, /* y方向上的缩放比 */ const float zoomY, /* 图片长度 */const unsigned int rgbLen);
    /* 计算写入gray数据需要的空间,在rst_fillLen不为空时返回头的大小,为空时返回头+数据+填充的大小 */
    unsigned int image_extern_gray_write_size_get(/* 可为空,不为空时返回填充长度 */unsigned char *const rst_fillLen, /* 图片长度 */const unsigned int grayLen
            , /* 注释长度 */const unsigned char commentLen);
    /* 把gray数据头写入rst_data中,返回存放数据的指针,指针是指向rst_data空间的 */
    unsigned char* image_extern_gray_write_head(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 填充的空间,足够大,为空表示填充数据写入rst_data_8的尾部 */unsigned char rst_fillData[]
            , /* 层数,一般用于显示 */ unsigned short layer, /* 注释长度 */ unsigned char commentLen, /* 注释 */const char comment[]
            , /* 图像宽度 */ const unsigned int width, /* 图像高度 */const unsigned int height, /* 储存宽度 */const unsigned int lineBits
            , /* 显示时左上角x方向上的边距 */const unsigned int dx1, /* 显示时左上角y方向上的边距 */const unsigned int dy1
            , /* 显示时右下角x方向上的边距 */ const unsigned int dx2, /* 显示时右下角y方向上的边距 */ const unsigned int dy2
            , /* x方向上的缩放比 */ const float zoomX, /* y方向上的缩放比 */ const float zoomY
            , /* 颜色深度,支持8,16(16位时按边缘图处理) */const unsigned int depth, /* 图片长度 */const unsigned int grayLen);
    /* 计算写入line数据需要的空间,在rst_fillLen不为空时返回头的大小,为空时返回头+数据+填充的大小 */
    unsigned int image_extern_line_write_size_get(/* 可为空,不为空时返回填充长度 */unsigned char *const rst_fillLen, /* 线的条数 */const unsigned int lineCount
            , /* 注释长度 */const unsigned char commentLen);
    /* 把line数据头写入rst_data中,返回存放数据的指针,指针是指向rst_data空间的 */
    unsigned char* image_extern_line_write_head(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 填充的空间,足够大,为空表示填充数据写入rst_data_8的尾部 */unsigned char rst_fillData[]
            , /* 层数,一般用于显示 */ unsigned short layer, /* 注释长度 */ unsigned char commentLen, /* 注释 */const char comment[]
            , /* 图像宽度 */ const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 三通道颜色,最高一字节为0表示实线,为1表示间隔一个点的虚线,2表示间隔两个点的虚线,以此类推,最多255个间隔 */ const unsigned int color
            , /* x方向上的缩放比 */ const float zoomX, /* y方向上的缩放比 */ const float zoomY
            , /* 线的条数 */const unsigned int lineCount);
    /* 计算写入text数据需要的空间,在rst_fillLen不为空时返回头的大小,为空时返回头+数据+填充的大小 */
    unsigned int image_extern_text_write_size_get(/* 可为空,不为空时返回填充长度 */unsigned char *const rst_fillLen, /* 数据长度,不包含\0 */const unsigned int dataLen
            , /* 注释长度 */const unsigned char commentLen);
    /* 把text数据头写入rst_data中,返回存放数据的指针,指针是指向rst_data空间的 */
    unsigned char* image_extern_text_write_head(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 填充的空间,足够大,为空表示填充数据写入rst_data_8的尾部 */unsigned char rst_fillData[]
            , /* 层数,一般用于显示 */ unsigned short layer, /* 注释长度 */ unsigned char commentLen, /* 注释 */const char comment[]
            , /* 图像宽度 */ const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 三通道颜色,最高一字节固定为0 */ const unsigned int color, /* 文本坐标 */ const unsigned int x, const unsigned int y, /* 字体大小 */ const unsigned short charWidth, const unsigned short charHeight
            , /* 数据长度,不包含\0 */const unsigned int dataLen);
    /* 计算写入键值数据需要的空间,在rst_fillLen不为空时返回头的大小,为空时返回头+数据+填充的大小 */
    unsigned int image_extern_key_write_size_get(/* 可为空,不为空时返回填充长度 */unsigned char *const rst_fillLen, /* 键值数据大小 */const unsigned int keydataLen
            , /* 注释长度 */const unsigned char commentLen);
    /* 获取数据区的写入位置 */
    unsigned char *image_extern_key_getdataPos(/* 写入的空间,足够大 */unsigned char rst_data_8[]);
    /* 获取写入数据的位置,首次写入应使用image_extern_key_getdataPos指定 */
    unsigned char *image_extern_key_write_data_get_pos(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 键的长度,最大为254 */ unsigned int keyLen
            , /* 对齐方式 */ const ImageExternType_keyA align);
    /* 写入数据,返回下一次的写入位置,首次写入应使用image_extern_key_getdataPos指定 */
    unsigned char *image_extern_key_write_data(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 键的长度,最大为254 */ unsigned int keyLen
            , /* 键 */const char *key, /* 数据长度,字符串不含结束\0的长度 */const unsigned int dataLen, /* 数据,为空表示数据已在外部设置 */const unsigned char *const data
            , /* 对齐方式 */ const ImageExternType_keyA align);
    /* 把键值数据头写入rst_data中,返回存放数据的指针,指针是指向rst_data空间的 */
    unsigned char* image_extern_key_write_head(/* 写入的空间,足够大 */unsigned char rst_data_8[], /* 填充的空间,足够大,为空表示填充数据写入rst_data_8的尾部 */unsigned char rst_fillData[]
            , /* 层数,一般用于显示 */ unsigned short layer, /* 注释长度 */ unsigned char commentLen, /* 注释 */const char comment[]
            , /* 键值数据大小 */const unsigned int keydataLen);
    /* 读取键值数据的值,成功返回读到的关键字数量,失败返回-1 */
    int image_extern_key_read(/* 指向储存结果的指针数组 */char * * const rstValue, /* 结果数组的纬度 */ int rstCount
            , /* 扩展数据长度 */unsigned long long externDataLen, /* 8字节对齐的扩展数据 */const unsigned char externData_a8[]
            , /* 关键字,纬度与rst相同,按升序排序 */const char *const *const key, /* 各关键字对齐方式,纬度与rst相同 */const unsigned char *const align);
    /* 输出键值数据的基本信息 */
    void image_extern_key_info_out(/* 扩展数据长度 */unsigned long long externDataLen, /* 8字节对齐的扩展数据 */const unsigned char externData_a8[], FILE * const fd);
    /* 获取大小,成功返回1,无大小属性返回0,失败返回-1 */
    int image_extern_get_size(/* 返回目标宽度 */unsigned int *const rst_width, /* 返回目标高度 */unsigned int *const rst_height
            , /* 显示掩码 */const unsigned long long layerMask
            , /* 8字节对齐的扩展数据 */const unsigned char externData_a8[], /* 扩展数据长度 */unsigned long long externDataLen);
    /* 把扩展数据绘制到dib上 */
    void image_extern_draw__IO_1(/* 传入原始的画板 */ImageDib * const rst, /* 显示掩码 */const unsigned long long layerMask
            , /* 画板左上角对应的x坐标,缩放比为1.0时的坐标 */unsigned int x0, /* 画板左上角对应的y坐标,缩放比为1.0时的坐标 */ unsigned int y0
            , /* x方向的缩放比 */float drawZoomX, /* x方向的缩放比 */float drawZoomY
            , /* 8字节对齐的扩展数据 */unsigned char externData_a8[], /* 扩展数据长度 */unsigned long long externDataLen
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

#ifdef	__cplusplus
}
#endif

#endif	/* IMAGEEXTERN_H */

