/* 
 * File:   ImageJpeg.h
 * Author: fslib.org
 *
 * Created on 2013年4月18日, 下午3:37
 */
#if !defined IMAGEJPEG_H && !defined JAVA_IN_C

#define IMAGEJPEG_H
#include "../publicDefine.h"

#include "../../PublicTools/Fs/Ebml.h"
#include "../../PublicTools/Image/ImageDib.h"

#define LINUX_ijl

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        unsigned short minor_code[17]; // indicates the value of the smallest Huffman code of length k
        unsigned short major_code[17]; // similar, but the highest code
        unsigned char Length[17]; // k =1-16 ; L[k] indicates the number of Huffman codes of length k
        unsigned char V[65536]; // V[k][j] = Value associated to the j-th Huffman code of length k        
        // High nibble = nr of previous 0 coefficients
        // Low nibble = size (in bits) of the coefficient which will be taken from the data stream
    } ImageJpegHuffman_table;

    typedef struct {
        /* 图像宽度 */
        unsigned short width;
        /* 图像高度 */
        unsigned short height;
        /* jpeg编解码中的宽度 */
        unsigned short roundWidth;
        /* jpeg编解码中的高度 */
        unsigned short roundHeight;
        /* 图象每个组份的精度,只支持8 */
        unsigned char bitPrecision;
        /* 图象颜色的深度,只可能为8或24,不为0表示调用过image_jpeg_analyseTag_base或image_jpeg_analyseTag_all */
        unsigned char bitCount;
        /* JPEG数据 */
        unsigned char *data;
        /* JPEG数据长度 */
        unsigned int dataLenth;

        /* 
         * JPEG标记,储存指针和长度,可以不被初始化;
         * 初始化后必定有APP0(FFE0),DQT(FFDB),DHT(FFC4),SOF0(FFC0),SOS(FFDA).
         */
        struct {

            /* APP0 标记:FFE0 */
            struct {
                /* 版本,只支持0x0201和0x0101 */
                unsigned short version;
                /* Xdensity和Ydensity的单位,0:无单位,1:点数/英寸,2:点数/厘米 */
                unsigned char units;
                /* 水平点密度值 */
                unsigned short Xdensity;
                /* 垂直点密度值 */
                unsigned short Ydensity;
                /* 水平点数 */
                unsigned char Xthumbnail;
                /* 垂直点数 */
                unsigned char Ythumbnail;
            } APP0; /* APP0 标记:FFE0 */
            /* DQT 量化表信息,标记:FFDB,原数据 */
            unsigned char *FFDB;
            unsigned short FFDB_len;
            /* DQT 量化表信息,标记:FFDB */
            float *QT[4];

            /* SOF0 基本DCT,标记:FFC0 */
            struct {
                unsigned char YH;
                unsigned char YV;
                unsigned char YQ_nr;
                unsigned char CbH;
                unsigned char CbV;
                unsigned char CbQ_nr;
                unsigned char CrH;
                unsigned char CrV;
                unsigned char CrQ_nr;
            } SOF0; /* SOF0 基本DCT,标记:FFC0 */
            /* DHT 定义DC Hiffman表,标记:FFC4 */
            ImageJpegHuffman_table * HTDC[3];
            /* DHT 定义AC Hiffman表,标记:FFC4 */
            ImageJpegHuffman_table * HTAC[3];
            /* COM 注解,0为指针指向FFFE,1为数据长度,不包括标记长度 */
            unsigned char*(*FFFE)[2];
            /* COM 注解个数 */
            unsigned short FFFE_count;
            /* DRT 定义重新开始间隔,标记:FFDD */
            unsigned short DRT;

            /* SOS 扫描开始,标记:FFDA */
            struct {
                unsigned char YDC_nr;
                unsigned char CbDC_nr;
                unsigned char CrDC_nr;
                unsigned char YAC_nr;
                unsigned char CbAC_nr;
                unsigned char CrAC_nr;
            } SOS; /* SOS 扫描开始,标记:FFDA */

            /* APPn n>0 为应用段保留 */
            unsigned char* FFEn[16];
            unsigned short FFEn_len[16];
            //            /* JPGn,E为注解 */
            //            unsigned char* FFFn[15];
            //            unsigned short FFFn_len[15];
            /* 数据区开始位置,不为NULL表示已使用image_jpeg_analyseTag_all解析数据 */
            unsigned char *dataStart;
        } jpegTag; /* JPEG标记,储存指针和长度 */
        /* 解码时要用的 */
        unsigned int byte_pos;
    } ImageJpeg;

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
        /* 采集时间之开机时间 */
        double capture_uptime;
        /* 采集时间之gmt时间 */
        double capture_gmtTime;
        /* 数据长度 */
        unsigned int datalenth;
        /* 数据,可外部设置 */
        char *data;
    } FsObjectImageJpeg;
    /*
     * 从内存创建一个ImageJpeg实例;
     * jpegBuffer:jpeg内存指针;
     * jpegBufferLenth:jpegbuffer的长度;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    ImageJpeg* image_jpeg_new_from_string__IO(unsigned char *jpegBuffer, unsigned int jpegBufferLenth);
    /*
     * 从内存创建一个ImageJpeg实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL.
     */
    ImageJpeg* image_jpeg_new_from_String__IO__OI(/* jpeg数据 */FsString *pString);

    /*
     * 根据文件创建一个ImageJpeg实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL;
     * fileName:文件名不能为空.
     */
    ImageJpeg* image_jpeg_new_from_file__IO(const char* fileName);
    /*
     * 根据ImageDib创建一个ImageJpeg实例;
     * 成功返回创建的实例指针; 
     * 失败返回NULL;
     * pDib:不能为空,且须为有效值;
     * imageQuality:图片质量 [0-100],if 0,quality is best,but size is biggest;
     * upsideDown:上下翻转,只能取1(不翻转)和-1(翻转),一般取-1才能获得想要的结果;
     * dstWidth:目标文件宽度,0:自动选择;
     * dstHeight:目标文件高度,0:自动选择.
     */
    ImageJpeg* image_jpeg_new_from_dib__IO(const ImageDib* pDib, const unsigned char imageQuality, const char upsideDown,
            unsigned int dstWidth, unsigned int dstHeight);
    /* 
     * 从pjpeg中删除ebml保存为新的ImageJpeg对象;
     * 成功返回删除ebml的ImageJpeg对象;
     * 失败返回NULL.
     */
    ImageJpeg* image_jpeg_new_from_jpeg_without_ebml__IO(ImageJpeg* pJpeg);

    /*
     * 删除ppJpeg指向的实例指针指向的对象;
     * 并把ppJpeg指向的实例指针设为0.;
     */
    void image_jpeg_delete__OI(ImageJpeg** ppJpeg);
    /*
     * 分析pJpeg中的基本JPEG标记,解析jpegTag外的所有标记和jpegTag中不分配空间的部分成员，分析到SOS结束;
     * 全部分析成功返回1,但并不表示JPG数据正确;
     * 已解析过(bitCount不为0)返回2;
     * 失败返回-1.
     */
    char image_jpeg_analyseTag_base(ImageJpeg* pJpeg);

    /*
     * 分析pJpeg中的所有JPEG标记,不分析头和尾;
     * 成功返回1,但并不表示JPG数据正确;
     * 已解析过(dataStart不为空)返回2;
     * 失败返回-1.
     */
    char image_jpeg_analyseTag_all(ImageJpeg* pJpeg);
    /*
     * 分析pJpeg中的JPEG标记,解析jpegTag外的所有标记和jpegTag中不分配空间的部分成员，分析到SOS结束;
     * 全部分析成功返回1,但并不表示JPG数据正确;
     * 已解析过(bitCount不为0)返回2;
     * 失败返回-1.
     */
#ifdef WINDOWS_ijl
#define image_jpeg_analyseTag(pJpeg) image_jpeg_analyseTag_base(pJpeg)
#else
#ifdef LINUX_ijl
#define image_jpeg_analyseTag(pJpeg) image_jpeg_analyseTag_base(pJpeg)
#else
#define image_jpeg_analyseTag(pJpeg) image_jpeg_analyseTag_all(pJpeg)
#endif
#endif

    /*
     * 把pJpeg保存到文件fileName中;
     * 如果成功返回1;
     * 如果打开文件失败返回-1;
     * 如果写文件失败返回-2.
     */
    char image_jpeg_save_to_file(const ImageJpeg* pJpeg, const char *fileName);
    /*
     * 把pJpeg转换为ImageDib位图;
     * 成功返回ImageDib对象指针;
     * 失败返回NULL.
     */
    ImageDib* image_jpeg_convert_to_dib__IO(ImageJpeg* pJpeg, /* 上下翻转,只能取1(不翻转)和-1(翻转),一般取-1才能获得想要的结果,现此值无效 */char upsideDown,
            /* 转换后的输出类型,ImageColorType_RGB = 1,ImageColorType_BGR = 2,ImageColorType_YCbCr = 3,ImageColorType_Gray = 4,现只支持ImageColorType_YCbCr */const ImageColorType dstColorType,
            /* 目标文件高度,0:表示为decodeHeight+topPadding,不能小于topPadding+decodeHeight */ unsigned short dstHeight,
            /* 解压后的图片,放置的位置距离图片上边缘的距离 */unsigned short topPadding,
            /* 解压高度,0表示全解压,不能大于图片的实际高度,小于图片高度可实现部分解压 */unsigned short decodeHeight,
            /* 图数据的储存宽度,为0时,程序自动计算,非0时,值不能小于最小储存宽度,多余的储存宽度填0,现只支持0 */const unsigned int dstLineBits);
    /* 
     * 把图片附加上EBM数据保存到流中;
     * 成功返回1;
     * 写文件失败返回-2;
     * 分析图像数据错误返回-3.
     */
    signed char image_jpeg_out_with_ebml(/* jpeg数据 */ImageJpeg * const pJpeg, /* 记录信息 */const FsEbml * const pEbml,
            /* 密码,为空或长度为0表示不加密 */ const unsigned char password[], /* 保存记录和图片的文件描述符 */FILE * const fd);
    /* 
     * 把图片附加上EBML数据保存到文件中;
     * 成功返回1;
     * 打开文件失败返回-1;
     * 写文件失败返回-2;
     * 分析图像数据错误返回-3.
     */
    signed char image_jpeg_save_to_file_with_ebml(/* jpeg数据 */ImageJpeg * const pJpeg, /* 记录信息 */const FsEbml * const pEbml,
            /* 密码,为空或长度为0表示不加密 */const unsigned char password[], /* 保存记录和图片的文件描述符 */const char filename[]);
    /* 
     * 提取JPEG中的EBML数据;
     * 成功返回提取到的EBML数据指针;
     * 失败返回NULL.
     */
    FsEbml * image_jpeg_get_ebml__IO(ImageJpeg * const pJpeg, /* 密码,为空或长度为0表示不加密 */const unsigned char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 
     * 提取JPEG中的EBML数据;
     * 成功返回提取到的EBML数据指针;
     * 失败返回NULL.
     */
    FsEbml *image_jpeg_get_ebml_from_file__IO(/* 保存记录和图片的文件名 */const char filename[], /* 密码,为空或长度为0表示不加密 */const unsigned char password[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 
     * 把图片保存到流中,但不保存EBML数据;
     * 成功返回1;
     * 写文件失败返回-2;
     * 分析图像数据错误返回-3.
     */
    char image_jpeg_out_without_ebml(/* jpeg数据 */ImageJpeg *pJpeg, /* 保存记录和图片的文件描述符 */FILE *fd);
    /* 
     * 把图片保存到文件中,但不保存EBML数据;
     * 成功返回1;
     * 打开文件失败返回-1;
     * 写文件失败返回-2;
     * 分析图像数据错误返回-3.
     */
    char image_jpeg_save_to_file_without_ebml(/* jpeg数据 */ImageJpeg *pJpeg, /* 保存记录和图片的文件名 */const char filename[]);
    /* 
     * 把图片附加上EBM数据保存到文件中;
     * 成功返回1;
     * 打开文件失败返回-1;
     * 写文件失败返回-2;
     * 分析图像数据错误返回-3.
     */
    signed char image_jpeg_save_data__OI_1(FsString * const pString, /* 数据标签,读取时也可校验此值,0-表示不校验 */const unsigned int tag, /* 保存记录和图片的文件名 */const char filename[]);

    /* 
     * 获取jpeg文件中附加的数据;
     * 成功返回FsString指针;
     * 失败返回NULL.
     */
    FsString * image_jpeg_get_data__IO(/* 保存记录和图片的文件名 */const char filename[], /* 数据标签,读取时也可校验此值,0-表示不校验 */const unsigned int tag
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 获取jpg的宽度和高度;
     * 成功返回1;
     * 失败返回-1. 
     */
    int image_jpeg_size_get(unsigned int *const width, unsigned int *const height, /* 数据 */const register unsigned char data[], /* 数据长度 */register unsigned int dataLen);
    /* 
     * 压缩图像为jpeg图片;
     * 成功返回压缩后的图像数据长度;
     * 缓存空间太小返回-1;
     * 其他错误返回-2.
     */
    int image_jpeg_compress(/* 结果的储存buffer */unsigned char rstBuffer[], /* 结果的储存buffer大小 */ unsigned long rstBufferSize
            , /* 原始数据 */ const unsigned char srcBffer[], /* 宽度 */const unsigned int width, /* 高度 */const unsigned int height, /* 储存宽度 */ const unsigned int lineBits
            , /* 图片质量 [0-100],0是最好质量 */const unsigned char imageQuality, /* 上下翻转,只能取1(不翻转)和-1(翻转),一般取-1才能获得想要的结果 */const char upsideDown
            , /* 原始数据类型(彩色RGB:1;彩色BGR:2;彩色YCbCr:3;灰度图:4) */ const unsigned char colorType);
    /* 
     * 解压jpg图像;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char image_jpeg_decompress(/* 结果的储存buffer */unsigned char rstBuffer[], /* 结果的储存buffer大小 */ unsigned int rstBufferSize,
            /* 结果数据类型(彩色RGB:1;彩色BGR:2;彩色YCbCr:3;灰度图:4) */ const unsigned char dstColorType,
            /* 原始数据 */ const unsigned char srcBuffer[], /* 原始数据的buffer大小 */ unsigned int srcBufferSize);
    /* 对已使用fs_ObjectBase_init初始化的数据使用本函数初始化为FsObjectImageJpeg对象,仅在pObjectBase的引用计数不大于1时可调用 */
    void image_jpeg_object_init_from_objectBase(void* const pObjectBase, /* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* 数据长度 */const unsigned int datalenth, /* 数据,数据的空间必须是属于pObjectBase的部分 */ char *const data);
    /* 创建一个FsObjectImageJpeg对象 */
    FsObjectImageJpeg * image_jpeg_object_new__IO(/* h264数据长度 */const unsigned int datalenth);

    /* 用数据创建一个FsObjectImageJpeg对象 */

    FsObjectImageJpeg * image_jpeg_object_new_with_data__IO(/* 分类号 */const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex
            , /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height
            , /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime
            , /* jpeg数据长度 */const unsigned int datalenth, /* 数据,长度必须和datalenth相同,为空表示此数据在外部设置 */ const unsigned char data[]);


    /* 
     * 把dib压缩后创建FsObjectImageJpeg对象;
     * 成功返回FsObjectImageJpeg指针;
     * 失败返回NULL.
     */
    FsObjectImageJpeg * image_jpeg_object_new_from_dib__IO(
            /* 原始数据 */ const unsigned char srcBffer[], /* 宽度 */const unsigned int width, /* 高度 */const unsigned int height, /* 储存宽度 */ const unsigned int lineBits,
            /* 图片质量 [0-100],0是最好质量 */const unsigned char imageQuality, /* 上下翻转,只能取1(不翻转)和-1(翻转),一般取-1才能获得想要的结果 */const char upsideDown,
            /* 原始数据类型 */ const ImageColorType colorType,
            /* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime);
    /* 设置pObjectJpeg的数据 */
    void image_jpeg_object_set_data(FsObjectImageJpeg * const pObjectJpeg, /* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */ const unsigned int frameIndex,
            /* 数据的来源ip */const unsigned int ipsrc, /* 帧率 */ const float frameRate, /* 图像宽度 */const unsigned int width, /* 图像高度 */const unsigned int height,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* 数据,长度必须和pObjectJpeg->datalenth相同,为空表示此数据在外部设置 */ const unsigned char data[]);


#ifdef FsDebug
    void image_jpeg_test(unsigned int x, unsigned int y);
#endif
#ifdef __cplusplus
}
#endif

#endif /* IMAGEJPEG_H */

