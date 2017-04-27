/* 
 * File:   VideoMKV.h
 * Author: 5aii
 *
 * Created on 2014年2月21日, 下午12:33
 */

#ifndef VIDEOMKV_H
#define	VIDEOMKV_H
#include "../publicDefine.h"
#include "../Fs/ObjectList.h"
#include "../Fs/String.h"
#ifdef	__cplusplus
extern "C" {
#endif
    /* 视频 */
#define VideoMKV_trackVideo 0x01
    /* 音频 */
#define VideoMKV_trackAudio 0x02
    /* 字幕 */
#define VideoMKV_trackSubtitle 0x11

    /* 流通道类型 */
    struct VideoMKV_track {
        /* 通道名,可为空 */
        char *name;
        /* 类型(0x01:为视频;0x02:为音频;0x11:为字幕),用VideoMKV_track开头的宏定义 */
        unsigned char type;
        /* 不能重复,从1开始连续递增 */
        unsigned short index;

        union {

            /* 视频流通道类型 */
            struct {
                /* 宽度 */
                unsigned int width;
                /* 高度 */
                unsigned int height;
                /* 编码方式(1:mjpeg;2:h264) */
                unsigned char encodeType;
                /* 帧率 */
                float frameRate;
                /* 编码信息,前4个字节为长度,第5个字节表示ContentEncodings中的长度 */
                char *codecPrivate;
            } video;

            /* 音频流通道类型 */
            struct {
                /* 声道数
                 *     0:Defined in AOT Specifc Config
                 *     1:1 channel:front-center
                 *     2:2 channels:front-left, front-right
                 *     3:3 channels:front-center, front-left, front-right
                 *     4:4 channels:front-center, front-left, front-right, back-center
                 *     5:5 channels:front-center, front-left, front-right, back-left, back-right
                 *     6:6 channels:front-center, front-left, front-right, back-left, back-right, LFE-channel
                 *     7:8 channels:front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
                 */
                unsigned char channels : 3;
                /* 采样率,常见的有 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 */
                unsigned int sampleRate;
                /* 编码方式(1:aac) */
                unsigned char encodeType;
                /* 帧率 */
                float frameRate;
                /* 编码信息,前4个字节为长度,第5个字节保留 */
                unsigned char *codecPrivate;
            } audio;

            /* 字幕流通道类型 */
            struct {
                /* 宽度 */
                unsigned int width;
                /* 高度 */
                unsigned int height;
                /* 字体大小 */
                unsigned short fontSize;
            } subtitle;
        } p;
    };

    /* MKV节点信息 */
    struct VideoMKV_node {
        /* 相对于data的绝对偏移量 */
        unsigned long long offset;
        /* 长度,对h264读取时缓存大小不要小于此值+codecPrivate的长度(最小为本文件定义的含头长度-sizoef(unsigned int),即sps+pps的长度) */
        unsigned int lenth;

        union {

            /* 视频节点,读到的数据是不包含0x01000000的,请根据实际情况添加 */
            struct {
                /* 是否是关键帧(1:是;0:不是) */
                unsigned char keyFrame : 1;
            } video;

            /* 音频节点,读到的数据是不包含音频头的,请根据实际情况添加 */
            struct {
            } audio;

            /* 字幕节点 */
            struct {
                /* 开始时间 */
                float startTime;
                /* 结束时间 */
                float endTime;
                /* 位置x */
                unsigned int x;
                /* 位置y */
                unsigned int y;
            } subtitle;
        } p;
    };

    /* MKV的通道信息,在本类生成外部只读 */
    struct VideoMKV_channel {
        /* 通道名,可为空 */
        char *name;
        /* channel类型,支持视频,音频和字幕,赋值请用宏定义.以VideoMKV_track开头 */
        unsigned char type;
        /* 帧信息,成员类型为struct VideoMKV_node,空间不用当独释放,在申请后加入VideoMKV结构中的tempBufferList中,让其释放 */
        FsObjectList *list;

        union {

            struct {
                /* 视频宽度 */
                unsigned int width;
                /* 视频高度 */
                unsigned int height;
                /* 视频内图像编码类型(1:mjpeg;2:h264) */
                unsigned char encodeType;
                /* 帧率 */
                float frameRate;
                /* 编码信息,前4个字节为长度(不含头的长度,头长5个字节),第5个字节表示ContentEncodings中的长度(H264有可能以00 00 00 01和00 00 01开头,此值可能为3和4) */
                unsigned char *codecPrivate;
            } video;

            /* 音频流通道类型 */
            struct {
                /* 表示使用哪个级别的AAC，有些芯片只支持AAC LC,(0:Null;1:AAC Main;2:AAC LC (Low Complexity);3:AAC SSR (Scalable Sample Rate);4: AAC LTP (Long Term Prediction) */
                unsigned char profile : 3;
                /* 声道数
                 *     0:Defined in AOT Specifc Config
                 *     1:1 channel:front-center
                 *     2:2 channels:front-left, front-right
                 *     3:3 channels:front-center, front-left, front-right
                 *     4:4 channels:front-center, front-left, front-right, back-center
                 *     5:5 channels:front-center, front-left, front-right, back-left, back-right
                 *     6:6 channels:front-center, front-left, front-right, back-left, back-right, LFE-channel
                 *     7:8 channels:front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
                 */
                unsigned char channels : 3;
                /* 采样率,常见的有 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 */
                unsigned int sampleRate;
                /* 编码方式(1:aac) */
                unsigned char encodeType;
                /* 帧率 */
                float frameRate;
                /* 编码信息,前4个字节为长度,第5个字节保留 */
                unsigned char *codecPrivate;
            } audio;

            struct {
                /* 宽度 */
                unsigned int width;
                /* 高度 */
                unsigned int height;
                /* 字体大小 */
                unsigned short fontSize;
                /* 编码信息,前4个字节为长度(不含头的长度,头长5个字节),第5个字节保留 */
                char *codecPrivate;
            } subtitle;
        } p;
    };

    /* MKV视频文件对象,不支持变帧率储存 */
    typedef struct {
        /* 当前视频通道号,0表示无视频 */
        unsigned short videoIndex;
        /* 当前音频通道号,0表示无音频 */
        unsigned short audioIndex;
        /* 当前字幕通道号,0表示无字幕 */
        unsigned short subtitleIndex;
        /* 视频通道数 */
        unsigned short videoCount;
        /* 音频通道数 */
        unsigned short audioCount;
        /* 字幕通道数 */
        unsigned short subtitleCount;
        /* 视频文件的大小,对未写完的文件读取过程中此值可能不准,无参考价值 */
        unsigned long long dataLenth;
        /* 为不定类型指针,可能是文件,也可能是FsObjectList指针,还可能是一块足够大的内存区域,本对象不对此对象作内存释放的操作 */
        void *data;
        /* 内部使用的临时空间,成员也是FsObjectList指针,成员的成员是连续空间,如果成员FsObjectList指针的modifyNodeCount为0表示其成员为连续块 */
        FsObjectList *tempBufferList;
        /* 通道的位置信息,此为数组类型,为空表示此通道不存在,一般都是连续的,0是不存在的,强制转换为(unsigned long*)型用于储存channel的最大通道号,如为1表示后面有一个通道 */
        struct VideoMKV_channel *channel;
        /* 内部的缓存空间 */
        char buffer[];
    } VideoMKV;
    /* 获取mjpeg流的编码信息,返回编码信息数据(前4个字节为长度,第5个字节表示ContentEncodings中的长度) */
    char* video_mkv_codecPrivate_jpeg_get__IO();
    /* 获取h264流的编码信息,传入的数据必须是关键帧,返回编码信息数据(前4个字节为长度,第5个字节表示ContentEncodings中的长度) */
    char* video_mkv_codecPrivate_h264_get__IO(/* h264数据 */unsigned char frameBuffer[]);
    /* 获取aac流的编码信息,成功返回1,失败返回-1 */
    signed char video_mkv_codecPrivate_aac_get_byParameter(/* 编码信息数据(前4个字节为长度,第5个字节保留) */unsigned char rst_7[],
            /* 表示使用哪个级别的AAC，有些芯片只支持AAC LC,(0:Null;1:AAC Main;2:AAC LC (Low Complexity);3:AAC SSR (Scalable Sample Rate);4: AAC LTP (Long Term Prediction) */const unsigned char profile,
            /* 声道数
             *     0:Defined in AOT Specifc Config
             *     1:1 channel:front-center
             *     2:2 channels:front-left, front-right
             *     3:3 channels:front-center, front-left, front-right
             *     4:4 channels:front-center, front-left, front-right, back-center
             *     5:5 channels:front-center, front-left, front-right, back-left, back-right
             *     6:6 channels:front-center, front-left, front-right, back-left, back-right, LFE-channel
             *     7:8 channels:front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
             */const unsigned char channels,
            /* 采样率,{96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, -1, -1, 0} */unsigned int sampleRate);
    /* 获取aac流的编码信息,传入的数据必须是关键帧,返回编码信息数据(前4个字节为长度,第5个字节保留) */
    signed char video_mkv_codecPrivate_aac_get(unsigned char rst_7[], /* aac */unsigned char frameBuffer[]);
    /*
     * 创建一个VideoMKV实例用于保存视频到文件,只能调用以video_mkv_file开头的函数;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    VideoMKV* video_mkv_new_for_file_save__IO(/* 总通道数 */unsigned short trackCount, /* 流通道数据 */struct VideoMKV_track pTrack[],
            /* 默认的视频通道,从1开始,0表示无视频 */const unsigned short videoDefault, /* 默认的音频通道,从1开始,0表示无音频 */const unsigned short audioDefault,
            /* 默认的字幕通道,从1开始,0表示无字幕 */const unsigned short subtitleDefault, /* 打开的文件指针 */FILE *fd);
    /*
     * 创建一个VideoMKV实例用于保存视频到文件,并把fd_copy的主视频流数据拷贝到文件fd中,只能调用以video_mkv_file开头的函数,并只能保存一个视频通道;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    VideoMKV* video_mkv_new_for_file_save_byCopy_video_first__IO(/* 打开的文件指针 */FILE * const fd, /* 复制的第一个文件指针 */FILE * const fd_copy);
    /*
     * 读取文件信息创建VideoAVI对象;
     * 成功返回创建的实例指针;
     * 失败返回NULL.
     */
    VideoMKV* video_mkv_new_from_file__IO(/* 打开的文件指针 */FILE * const fd);
    /* 尝试读取数据帧,有读到新的数据帧返回1,无返回0,出错返回-1 */
    signed char video_mkv_file_try_read(VideoMKV * const pMKV, /* 文件句柄 */FILE * const fd);
    /* 删除pMKV指向的实例对象 */
    void video_mkv_delete__OI(VideoMKV * const pMKV);
    /* 输出调试信息 */
    void video_mkv_out(VideoMKV *pMKV, FILE *fd);
    /*
     * 把图像数据帧保存到文件,pMKV必须是video_mkv_new_for_file的返回值;
     * 成功返回偏移量;
     * 写失败返回-1.
     */
    long long video_mkv_file_save_frame(VideoMKV *pMKV, /* 视频通道号,必须有效 */const unsigned char videoIndex, /* 是否是关键帧,1是关键帧,0不是关键帧,对mjpeg应该都是关键帧 */const unsigned char keyFrame,
            /* 图像数据帧的长度,如果数据长度改变后会更新此值 */ unsigned int *frameLenth, /* 图像数据帧的内容 */const char frameData[]);
    /*
     * 把图像数据帧保存到文件,pMKV必须是video_mkv_new_for_file的返回值;
     * 成功返回偏移量;
     * 写失败返回-1.
     */
    long long video_mkv_file_save_audio(VideoMKV *pMKV, /* 音频通道号,必须有效 */const unsigned char audioIndex,
            /* 图像数据帧的长度,如果数据长度改变后会更新此值 */ unsigned int *frameLenth, /* 图像数据帧的内容 */const char frameData[]);
    /*
     * 把字幕数据保存到文件,pMKV必须是video_mkv_new_for_file的返回值;
     * 成功返回1;
     * 写失败返回-1.
     */
    char video_mkv_file_save_subtitle(VideoMKV *pMKV, /* 字幕道号,必须有效 */const unsigned char subtitleIndex, unsigned int x, unsigned int y,
            /* 字幕的持续时间 */const float time, /* 字幕长度 */const unsigned int subtitleLenth, /* 字幕内容 */const char subtitleData[]);
        /*
     * 把文件内的主视频流保存到文件,pMKV必须是video_mkv_new_for_file的返回值;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char video_mkv_file_save_video_first(VideoMKV * const pMKV, /* 复制的文件指针 */FILE * const fd_copy);
    /* 
     * 把视频尾写入文件;
     * 成功返回1;
     * 写文件失败返回-1.
     */
    char video_mkv_file_save_finish(VideoMKV *pMKV);
    /* 
     * 获取h264关键帧的头数据;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char video_mkv_h264_get_keyData(/* 结果的储存空间,前4字节为数据大小,后跟数据,空间与privateCode占用的空间相同(包含头),可把privateCode传入,实现privateCode同keydata的转换 */unsigned char rst[],
            /* 为结构struct VideoMKV_track中的codecPrivate字段 */unsigned char codePrivate[]);
    /* 
     * 获取h264关键帧的头数据;
     * 成功返回1;
     * 失败返回-1.
     */
    signed char video_mkv_aac_get_head(/* 结果的储存空间,只有7字节,后跟数据,空间不大于privateCode占用的空间相同,可把privateCode传入,实现privateCode同head的转换 */unsigned char rst_7[],
            /* 为结构struct VideoMKV_track中的codecPrivate字段 */const unsigned char codePrivate[], /* 长度 */const unsigned int len);
    /* 
     * 从文件中读取一帧h264数据用于解码或网络传输;
     * 成功返回此数据帧的实际长度;
     * 查找位置失败返回-1;
     * 读取文件失败返回-2;
     * 文件信息错误返回-3.
     */
    long video_mkv_read_h264Stream_from_file(/* 读取到的数据的储存空间,关键帧大小不要小于此值+codecPrivate的长度(不含本文件在前面加的4个字节的长度),非关键帧长度就是此值 */unsigned char rst_buffer[],
            /* 关键帧的头数据信息,为空表示要读取的数据非关键帧,用video_mkv_h264_get_keyData获取 */ unsigned char keyData[],
            /* 数据储存的文件,不能为空 */FILE *fd,
            /* 数据储存位置相对于文件的绝对偏移量 */unsigned long long offset,
            /* 数据在文件中的长度 */unsigned int lenth);
    /* 
     * 从文件中读取一帧aac数据用于解码或网络传输;
     * 成功返回此数据帧的实际长度,为lenth+7;
     * 查找位置失败返回-1;
     * 读取文件失败返回-2;
     * 文件信息错误返回-3.
     */
    long video_mkv_read_aacStream_from_file(/* 读取到的数据的储存空间,大小为lenth+7 */unsigned char rst_buffer[],
            /* aac的codecPrivate数据 */const unsigned char codecPrivate[],
            /* 数据储存的文件,不能为空 */FILE * const fd,
            /* 数据储存位置相对于文件的绝对偏移量 */const unsigned long long offset,
            /* 数据在文件中的长度 */const unsigned int lenth);
    /* 从pMKV中提取aac音频,成功返回FsString指针,失败返回NULL */
    FsString *video_mkv_file_to_aac__IO(VideoMKV * const pMKV, /* 要获取的aac所在pMKV的通道号,为0表示获取默认aac音频 */ unsigned char channel);

#ifdef FsDebug
    void video_mkv_test();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* VIDEOMKV_H */

