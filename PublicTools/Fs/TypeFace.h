/* 
 * File:   TypeFace.h
 * Author: fslib
 *
 * Created on 2013年8月5日, 上午11:34
 */

#if !defined TYPEFACE_H && !defined JAVA_IN_C

#define TYPEFACE_H
#include "../../PublicTools/publicDefine.h"
#include "../../PublicTools/Fs/ObjectList.h"
#ifdef __cplusplus
extern "C" {
#endif
    //#pragma pack(push,2)

    struct FsTypeFace_item {
        /* 字的unicode编码值,0-0x10FFFF */
        unsigned int code;
        /* 左边距 */
        short marginLeft;
        /* 上边距 */
        unsigned short marginTop;
        /* 字体宽度 */
        unsigned short width;
        /* 字体高度 */
        unsigned short height;
        /* 右边距 */
        short marginRight;
        /* 数据区 */
        unsigned char data[];
    };

    typedef struct {
        /* 字体最大宽度 */
        unsigned short width;
        /* 字体最大高度 */
        unsigned short height;
        /* 加粗,0表示不加粗,一般为64 */
        unsigned int bold;
        /* 倾斜,0表示不倾斜,一般为0x5800 */
        unsigned int italic;
        /* 引用次数 */
        unsigned long referenceCount;
        /* 字体数据的缓存缓存链表 */
        FsObjectList *dataList;
        /* 已缓存字体大小 */
        unsigned long bufferSize;
        /* 字体库对象 */
        void *typeObject;
        /* 字体名字 */
        char name[];
    } FsTypeFace;

    struct FsTypeFaceText {
        /* 关联的typeFace */
        FsTypeFace * __pTypeFace;
        /* 文字的位置左上角 */
        unsigned int x, y;
        /* 文字颜色,具体定义由外部控制 */
        unsigned int color;
        /* 文字 */
        char text[];
    };
    //#pragma pack(pop)

    /*
     * 创建一个新的FsTypeFace对象;
     * 成功返回FsTypeFace指针;
     * 失败返回NULL.
     */

    FsTypeFace * fs_TypeFace_new__IO(/* 字库文件名,为空使用默认值 */const char filename[],
            /* 字体最大宽度 */ const unsigned short width,
            /* 字体最大高度 */ const unsigned short height,
            /* 加粗,0表示不加粗 */ const unsigned int bold,
            /* 倾斜,0表示不倾斜 */const unsigned int italic);

    /* 删除pTypeFace指向的实例对象 */

    void fs_TypeFace_delete__OI(FsTypeFace * const pTypeFace);

    /* 为pTypeFace添加引用计数 */

    void fs_TypeFace_addRefer(FsTypeFace * const pTypeFace, /* 添加的引用计数次数 */const unsigned int addReferCount);

    /* 
     * 获取编码为charN的数据;
     * 成功返回FsTypeFace_item指针;
     * 失败返回NULL.
     */

    struct FsTypeFace_item *fs_TypeFace_get(FsTypeFace* pTypeFace, /* 字符的编码码值,应与pTypeFace创建时的字符集相符 */const unsigned int code);

    /*
     * 把字符串写到图pBits上;
     * 成功返回1;
     * 编码错误返回-1;
     * 无效编码返回-2;
     * 字符越界返回-3;
     * str:字符串数据;
     * pbits:图像是类似灰度图这样连续排列的图像;
     * width:图像宽度;
     * height:图像高度;
     * xstep:储存宽度;
     * x;开始位置的x坐标;
     * y:开始位置的y坐标;
     * fcolor:字体颜色.
     */

    const signed char fs_TypeFace_write(FsTypeFace * const pTypeFace, const unsigned char str[], unsigned char pBits[], const unsigned int width, const unsigned int height,
            const int xstep, unsigned int x, unsigned int y, const unsigned char fcolor);

    /*
     * 把字符串写到图pBits上;
     * 成功返回1;
     * 编码错误返回-1;
     * 无效编码返回-2;
     * 字符越界返回-3.
     */

    const signed char fs_TypeFace_write_uv(FsTypeFace * const pTypeFace, /* 要写的字符串数据 */const unsigned char str[], /* 图像是类似yuv420p这样连续排列的图像中的U或V */unsigned char pBits[],
            /* 图像宽度,y分量 */const unsigned int width, /* 图像高度,y分量 */const unsigned int height,
            /* 储存宽度 */const int xstep, /* 开始位置的x坐标,相对于y分量 */unsigned int x, /* 开始位置的y坐标,相对于y分量 */unsigned int y, /* 字体颜色 */ const unsigned char fcolor);

    /*
     * 把字符串写到图pBits上;
     * 成功返回1;
     * 编码错误返回-1;
     * 无效编码返回-2;
     * 字符越界返回-3;
     * str:字符串数据;
     * pBits:图像是类似RGB这样交叉排列的图像;
     * width:图像宽度;
     * height:图像高度;
     * xstep:储存宽度;
     * x;开始位置的x坐标;
     * y:开始位置的y坐标;
     * fcolor:字体颜色.
     */

    const signed char fs_TypeFace_write_3(FsTypeFace * const pTypeFace, const unsigned char str[], unsigned char pBits[], const unsigned int width, const unsigned int height,
            const int xstep, const unsigned int x, const unsigned int y, const unsigned char fcolor);

    /*
     * 创建一个FsTypeFaceText对象
     */

    struct FsTypeFaceText * fs_TypeFace_text_new__IO(/* 关联的pTypeFace对象,内部会增加引用 */FsTypeFace * const pTypeFace, /* 文字左上脚位置 */const unsigned int x, const unsigned int y, /* 文字颜色 */const unsigned int color, /* 文字 */ const char text[]);

    /* 删除pTypeFaceText指向的实例对象 */

    void fs_TypeFace_text_delete__OI(struct FsTypeFaceText * const pTypeFaceText);



#ifdef __cplusplus
}
#endif

#endif /* TYPEFACE_H */

