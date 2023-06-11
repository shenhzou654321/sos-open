/* 
 * File:   AudioAAC.h
 * Author: fslib.org
 *
 * Created on 2016年2月29日, 下午12:33
 */
#if !defined AUDIOACC_H && !defined JAVA_IN_C

#define AUDIOACC_H
#include "../../PublicTools/publicDefine.h"

#include <stdio.h>
#include "../../PublicTools/Fs/Object.h"
#ifdef __cplusplus
extern "C" {
#endif
#define FsObjectAAC_HeadLen ((unsigned int)FsPointer_to_long(((FsObjectAAC*)FsPointer(0))->data))

    /* 继承FsObject的AAC数据 */
    typedef struct {
        FsObject object;
        /* 分类号,不同的分类号表示数据已无关系,与之相关的调用须重新初始化 */
        unsigned short classIndex;
        /* 帧号,建议只使用3个字节 */
        unsigned int frameIndex;
        /* 帧率 */
        float frameRate;
        /* 表示使用哪个级别的AAC，有些芯片只支持AAC LC,(0:Null;1:AAC Main;2:AAC LC (Low Complexity);3:AAC SSR (Scalable Sample Rate);4:AAC LTP (Long Term Prediction)
         *  5:AOT_SBR;6:AOT_AAC_SCALABLE;7:AOT_TWINVQ;8:AOT_CELP(Code Excited Linear Prediction);9:AOT_HVXC(Harmonic Vector eXcitation Coding)
         *  12:AOT_TTSI(Text-To-Speech Interface);13:AOT_MAINSYNTH(Main Synthesis);14:AOT_WAVESYNTH(Wavetable Synthesis)
         */
        unsigned char profile : 4;
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
        /* 采样率,{96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, -1, -1, 0} */
        unsigned int sampleRate;
        /* 采集时间之开机时间 */
        double capture_uptime;
        /* 采集时间之gmt时间 */
        double capture_gmtTime;
        /* 数据长度 */
        unsigned int datalenth;
        /* 数据,可外部设置 */
        char data[];
    } FsObjectAAC;

    /* AAC音频文件对象 */
    typedef struct {
        int t;
        /* 内部的缓存空间 */
        char buffer[];
    } AudioAAC;
#pragma pack(push,1)

    /* AAC音频,头按7字节计算 */
    struct AudioAAC_ADTS {
        /* 表示是否误码校验,为1表示无校验,头为7字节,为0表示包含2字节的crc校验,头为9字节 */
        unsigned short protection_absent : 1;
        /* always: '00' */
        unsigned short layer : 2;
        /* MPEG标识符,0标识MPEG-4,1标识MPEG-2 */
        unsigned short id : 1;
        /* 同步头,总是0xFFF,,all bits must be 1,代表着一个ADTS帧的开始 */
        unsigned short syncword : 12;
        ////////////////////////////////////////////////////////////////////////
        /* 2bit数据,从这3bit开始的后13bit为长度,长度是指包含AudioAAC_ADTS头的长度 */
        unsigned short len2 : 2;
        /* copywrite_identfication_start */
        unsigned short copywrite_identfication_start : 1;
        /* copywrite_identfication_bit */
        unsigned short copywrite_identfication_bit : 1;
        /* home */
        unsigned short home : 1;
        /* original_copy */
        unsigned short original_copy : 1;
        /* channel_configuration
         * 0: Defined in AOT Specifc Config
         * 1: 1 channel: front-center
         * 2: 2 channels: front-left, front-right
         * 3: 3 channels: front-center, front-left, front-right
         * 4: 4 channels: front-center, front-left, front-right, back-center
         * 5: 5 channels: front-center, front-left, front-right, back-left, back-right
         * 6: 6 channels: front-center, front-left, front-right, back-left, back-right, LFE-channel
         * 7: 8 channels: front-center, front-left, front-right, side-left, side-right, back-left, back-right, LFE-channel
         */
        unsigned short channel_configuration : 3;
        /* pivate_bit */
        unsigned short pivate_bit : 1;
        /* 表示使用的采样率下标，通过这个下标在{96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, -1, -1, 0}数组中查找得知采样率的值 */
        unsigned short sampling_frequency_index : 4;
        /* profile的值等于 Audio Object Type的值减1
         * profile = MPEG-4 Audio Object Type - 1
         * (0:AAC Main;1:AAC LC (Low Complexity);2:AAC SSR (Scalable Sample Rate);3: AAC LTP (Long Term Prediction)
         * ,FsObjectAAC中的profile是+1储存的
         */
        unsigned short profile : 2;
        ////////////////////////////////////////////////////////////////////////
        /*
         * len:11 
         */
        /* 
         * adts_buffer_fullness:11 0x7FF 说明是码率可变的码流
         */
        /*
         * number_of_raw_data_blocks_in_frame：表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧 
         */
        char ___[3];
    };
#define AudioAAC_ADTS_init(___pADTS,/* 传入的值应是FsObjectAAC中profile的值 */___profile,___channels,___sampling_frequency_index) do{\
    ((unsigned short*)(___pADTS))[0]=0xF1FFU;\
    ((unsigned short*)(___pADTS))[1]=(((unsigned short)((___profile)-1))<<6)|((___sampling_frequency_index)<<2)|((___channels)>>2)|(((unsigned short)(___channels))<<14);\
    ((unsigned short*)(___pADTS))[2]=0xFFFFU;\
    ((unsigned char*)(___pADTS))[6]=0xFCU;\
}while(0)
    /* 设置其长度 */
#define AudioAAC_ADTS_set_lenth(___pADTS,___len) do{\
     ((unsigned char*)(___pADTS))[3]=(((unsigned char*)(___pADTS))[3]&0xFC)|((___len)>>11);\
     ((unsigned char*)(___pADTS))[4]=(unsigned char)(((___len)>>3)&0xFF);\
     ((unsigned char*)(___pADTS))[5]=(((unsigned char*)(___pADTS))[5]&0x1F)|(((___len)&0x7)<<5);\
}while(0)
#pragma pack(pop)

    /* 用数据创建一个FsObjectAAC对象 */

    FsObjectAAC *audio_aac_object_new__IO(/* aac数据长度 */const unsigned int datalenth);

    /* 用数据创建一个FsObjectAAC对象 */

    FsObjectAAC *audio_aac_object_new_with_data__IO(/* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */const unsigned int frameIndex,
            /* 帧率 */ float frameRate,
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
             *     8-15: Reserved
             */
            const unsigned char channels,
            /* 采样率,{96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, -1, -1, 0} */const unsigned int sampleRate,
            /* 采集时间之开机时间 */ const double capture_uptime, /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* aac数据长度 */const unsigned int datalenth, /* 数据,长度必须和datalenth相同,为空表示此数据在外部设置 */ char data[]);

    /* 设置pObjectAAC的数据 */

    void audio_aac_object_set_data(FsObjectAAC* pObjectAAC, /* 分类号 */ const unsigned short classIndex, /* 帧号,建议只使用3个字节 */const unsigned int frameIndex,
            /* 帧率 */ float frameRate,
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
             *     8-15: Reserved
             */
            const unsigned char channels,
            /* 采样率,{96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, -1, -1, 0} */const unsigned int sampleRate,
            /* 采集时间之开机时间 */ const double capture_uptime,
            /* 采集时间之gmt时间 */ const double capture_gmtTime,
            /* 数据,长度必须和pObjectAAC->datalenth相同,为空表示此数据在外部设置 */ char data[]);

    /* 从fd中获取所有aac音频帧,成功返回FsObjectList指针,项为FsObjectAAC指针,FsObjectAAC指针个数大于0,失败返回NULL */

    FsObjectList *audio_aac_object_get_from_fd__IO(FILE * const fd, /* 分类号 */ const unsigned short classIndex, /* 开始帧号,最大值只能到3个字节 */ unsigned int frameIndex_start,
            /* 采集开始时间之开机时间 */ double capture_uptime_start, /* 采集开始时间之gmt时间 */ double capture_gmtTime_start,
            /* 文件名,只是用于显示错误信息,可为空 */ const char fileName[]);

    /* 从fileName中获取所有aac音频帧,成功返回FsObjectList指针,项为FsObjectAAC指针,FsObjectAAC指针个数大于0,失败返回NULL */

    FsObjectList * audio_aac_object_get_from_file__IO(/* 文件名 */ const char fileName[],
            /* 分类号 */ const unsigned short classIndex, /* 开始帧号,最大值只能到3个字节 */ const unsigned int frameIndex_start,
            /* 采集开始时间之开机时间 */ const double capture_uptime_start, /* 采集开始时间之gmt时间 */ const double capture_gmtTime_start);



#ifdef __cplusplus
}
#endif

#endif /* AUDIOACC_H */

