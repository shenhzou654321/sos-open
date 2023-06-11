/* 
 * File:   Rtsp.h
 * Author: fslib
 *
 * Created on 2016年10月27日, 上午11:19
 */

#ifndef RTSP_H
#define RTSP_H
#define Rtsp_stats // 开启统计
#define Rtsp_Debug
#define Rtsp_UDP_Enable

#define RtspClient_Debug
#include "../publicDefine.h"
#include <pthread.h>
#include <sys/epoll.h>

#include "../Fs/ObjectList.h"
#include "../Fs/CycleList.h"
#include "../Fs/StructList.h"
#include "../Fs/Network.h"
#include "../Fs/Memery.h"
#include "../Fs/Mapping.h"
#include "../Fs/Kcp.h"
#ifdef __cplusplus
extern "C" {
#endif
#define RtspServer_DemandClient_get(pClient) (((struct RtspServer_item_DemandClient**) (((struct FsCycleListNode*)pClient)+1))[1])
#pragma pack(push,1)

    struct RTP {
        /* 标记,固定为0x24,1字节 */
        unsigned char tag;
        /* 通道,1字节 */
        unsigned char channel;
        /* 数据长度,2字节,高位在前,包含之后的所有数据的长度 */
        unsigned char dataLenth[2];
        /* .... 0000 = Contributing source identifiers count: 0 
         * CSRC 计数(CC):4 比特,CSRC 计数包含了跟在固定头后面 CSRC 识别符的数目.
         */
        unsigned char CC : 4;
        /* ...0 .... = Extension: False
         * 扩展(X):1 比特,若设置扩展比特,固定头(仅)后面跟随一个头扩展;
         * 头扩展包含16比特的长度域,指示扩展项中32比特字的个数,不包括4个字节扩展头(因此零是有效值).
         * RTP固定头之后只允许有一个头扩展.为允许多个互操作实现独立生成不同的头扩展,或某种特定实现有多种不同的头扩展,扩展项的前16比特用以识别标识符或参数.
         * 这16比特的格式由具体实现的上层协议定义.基本的RTP说明并不定义任何头扩展本身.
         */
        unsigned char X : 1;
        /* ..0. .... = Padding: False
         * 填充(P):1 比特,若填料比特被设置,则此包包含一到多个附加在末端的填充比特,填充比特不算作负载的一部分.填充的最后一个字节指明可以忽略多少个填充比特.填充可能用于某些具有固定长度的加密算法.或者用于在底层数据单元中传输多个 RTP 包.
         * 填充长度为1字节,放于rtp包尾部,长度包含这1字节长度,即最小值为1
         */
        unsigned char P : 1;
        /* 10.. .... = Version: RFC 1889 Version (2)
         * 版本(V):2 比特,此域定义了 RTP 的版本.此协议定义的版本是 2.(值 1 被 RTP 草案版本使用,值 0 用在最初"vat"语音工具使用的协议中.)
         */
        unsigned char V : 2;
        /* Payload type: DynamicRTP-Type-96 (96) 
         * 负载类型(PT):7 比特，此域定义了负载的格式,由具体应用决定其解释,协议可以规定负载类型码和负载格式之间一个默认的匹配.其他的负载类型码可以通过非 RTP 方法动态定义.RTP发送端在任意给定时间发出一个单独的 RTP 负载类型;此域不用来复用不同的媒体流;
         * h264此值固定为0x60;
         * aac此值固定为0x61;
         */
        unsigned char PT : 7;
        /* 0... .... = Marker: False 
         * 标志(M):1 比特，标志的解释由具体协议规定。它用来允许在比特流中标记重要的事件,如帧边界;
         * 在帧的最后一个rtp也应该设置此标记为1;
         */
        unsigned char M : 1;
        /* Sequence number: 53949 
         * 序列号(sequence number):16 比特,每发送一个 RTP 数据包,序列号加 1,接收端可以据此检测丢包和重建包序列,序列号的初始值是随机的(不可预测),以使即便在源本身不加密时(有时包要通过翻译器,它会这样做),对加密算法泛知的普通文本攻击也会更加困难.
         */
        unsigned char SequenceNumber[2];
        /* Timestamp: 1992890304 
         * 时间戳(timestamp) ;32 比特,时间戳反映了 RTP 数据包中第一个字节的采样时间.时钟频率依赖于负载数据格式,并在描述文件(profile)中进行描述.也可以通过 RTP 方法对负载格式动态描述.
         * 如果 RTP 包是周期性产生的,那么将使用由采样时钟决定的名义上的采样时刻,而不是读取系统时间.例如,对一个固定速率的音频,采样时钟将在每个周期内增加 1.如果一个音频从输入设备中读取含有 160 个采样周期的块,
         * 那么对每个块,时间戳的值增加 160.时间戳的初始值应当是随机的,就像序号一样.几个连续的 RTP 包如果是同时产生的.如:属于同一个视频帧的 RTP 包,将有相同的序列号.
         * 不同媒体流的 RTP 时间戳可能以不同的速率增长.而且会有独立的随机偏移量.因此,虽然这些时间戳足以重构一个单独的流的时间,但直接比较不同的媒体流的时间戳不能进行同步.
         * 对于每一个媒体,我们把与采样时刻相关联的 RTP 时间戳与来自于参考时钟上的时间戳(NTP)相关联.因此参考时钟的时间戳就是数据的采样时间.(即:RTP 时间戳可用来实现不同媒体流的同步,NTP 时间戳解决了 RTP 时间戳有随机偏移量的问题.)
         * 参考时钟用于同步所有媒体的共同时间.这一时间戳对(RTP 时间戳和 NTP 时间戳),用于判断 RTP 时间戳和 NTP 时间戳的对应关系,以进行媒体流的同步.它们不是在每一个数据包中都被发送,而在发送速率更低的 RTCP 的 SR(发送者报告)中
         * 如果传输的数据是存贮好的,而不是实时采样得到的,那么会使用从参考时钟得到的虚的表示时间线(virtual presentation timeline).以确定存贮数据中的每个媒体下一帧或下一个单元应该呈现的时间.
         * 此种情况下 RTP 时间戳反映了每一个单元应当回放的时间。真正的回放将由接收者决定.
         */
        unsigned char Timestamp[4];
        /* Synchronization Source identifier: 0xd07ec35f (3497968479) 
         * SSRC:32 比特,用以识别同步源.标识符被随机生成,以使在同一个 RTP 会话期中没有任何两个同步源有相同的 SSRC 识别符.尽管多个源选择同一个 SSRC 识别符的概率很低,所有 RTP 实现工具都必须准备检测和解决冲突.
         * 若一个源改变本身的源传输地址,必须选择新的SSRC 识别符,以避免被当作一个环路源.RTP 包流的源,用 RTP 报头中 32 位数值的SSRC 标识符进行标识,使其不依赖于网络地址.
         * 一个同步源的所有包构成了相同计时和序列号空间的一部分,这样接收方就可以把一个同步源的包放在一起,来进行重放.举些同步源的例子，像来自同一信号源的包流的发送方,如麦克风,摄影机,RTP 混频器就是同步源.
         * 一个同步源可能随着时间变化而改变其数据格式,如音频编码.SSRC 标识符是一个随机选取的值,它在特定的 RTP 会话中是全局唯一(globally unique)的.参与者并不需要在一个多媒体会议的所有 RTP 会话中,，使用相同的 SSRC 标识符;
         * SSRC 标识符的绑定通过RTCP.如果参与者在一个 RTP 会话中生成了多个流,例如来自多个摄影机,则每个摄影机都必须标识成单独的同步源.
         */
        unsigned char SSRC[4];

        union {
            /* 对解码后的第一字节:
             * h264中第1位禁止位，值为1表示语法出错;
             * 第2~3位为参考级别;
             * 第4~8为是nal单元类型;
             */
            unsigned char Payload[4];

            /* H264 */
            struct {
                /* 类型:
                 * 单一 NAL 单元模式
                 * 0:未规定
                 * 1:非IDR图像中不采用数据划分的片段
                 * 2:非IDR图像中A类数据划分片段
                 * 3:非IDR图像中B类数据划分片段
                 * 4:非IDR图像中C类数据划分片段
                 * 5:IDR图像的片段
                 * 6:补充增强信息（SEI）
                 * 7:序列参数集（SPS）
                 * 8:图像参数集（PPS）
                 * 9:分割符
                 * 10:序列结束符
                 * 11:流结束符
                 * 12:填充数据
                 * 13:序列参数集扩展
                 * 14:带前缀的NAL单元
                 * 15:子序列参数集
                 * 16–18:保留
                 * 19:不采用数据划分的辅助编码图像片段
                 * 20:编码片段扩展
                 * 21–23:保留
                 * //24–31:在h264中未规定,而在这里表示这是一个特别格式的 NAL 单元
                 * 
                 * 组合封包模式
                 *     即可能是由多个 NAL 单元组成一个 RTP 包. 分别有4种组合方式: STAP-A, STAP-B, MTAP16, MTAP24.
                 *     那么这里的类型值分别是 24, 25, 26 以及 27;
                 *     24     STAP-A    Single-time aggregation packet
                 *     25     STAP-B    Single-time aggregation packet
                 *     26     MTAP16    Multi-time aggregation packet
                 *     27     MTAP24    Multi-time aggregation packet
                 *     
                 *      FU-A      Fragmentation uni
                 *     29     FU-B      Fragmentation unit
                 *     30-31  undefined 
                 * 
                 * 分片封包模式
                 * 用于把一个NALU单元封装成多个RTP包,存在两种类型 FU-A和FU-B.类型值分别是 28 和 29
                 */
                unsigned char FUindicator_type : 5;
                /* 参考级别,与h264中的相同,根据分片NAL单元的NRI域的值设置 */
                unsigned char FUindicator_NRI : 2;
                /* 禁止位,值为1表示语法出错 */
                unsigned char FUindicator_F : 1;
                /* 仅分包时以下定义才有效 */
                /* 类型:
                 * 单一 NAL 单元模式
                 * 0:未规定
                 * 1:非IDR图像中不采用数据划分的片段
                 * 2:非IDR图像中A类数据划分片段
                 * 3:非IDR图像中B类数据划分片段
                 * 4:非IDR图像中C类数据划分片段
                 * 5:IDR图像的片段
                 * 6:补充增强信息（SEI）
                 * 7:序列参数集（SPS）
                 * 8:图像参数集（PPS）
                 * 9:分割符
                 * 10:序列结束符
                 * 11:流结束符
                 * 12:填充数据
                 * 13:序列参数集扩展
                 * 14:带前缀的NAL单元
                 * 15:子序列参数集
                 * 16–18:保留
                 * 19:不采用数据划分的辅助编码图像片段
                 * 20:编码片段扩展
                 * 21–23:保留
                 * //24–31:在h264中未规定,而在这里表示这是一个特别格式的 NAL 单元
                 * 
                 * 组合封包模式
                 *     即可能是由多个 NAL 单元组成一个 RTP 包. 分别有4种组合方式: STAP-A, STAP-B, MTAP16, MTAP24.
                 *     那么这里的类型值分别是 24, 25, 26 以及 27;
                 *     24     STAP-A    Single-time aggregation packet
                 *     25     STAP-B    Single-time aggregation packet
                 *     26     MTAP16    Multi-time aggregation packet
                 *     27     MTAP24    Multi-time aggregation packet
                 *     28     FU-A      Fragmentation uni
                 *     29     FU-B      Fragmentation unit
                 *     30-31  undefined 
                 * 
                 * 分片封包模式
                 * 用于把一个NALU单元封装成多个RTP包,存在两种类型 FU-A和FU-B.类型值分别是 28 和 29
                 */
                unsigned char FUheader_type : 5;
                /* 保留位 */
                unsigned char FUheader_R : 1;
                /* 为1表示结束,当它为1,S不能为1 */
                unsigned char FUheader_E : 1;
                /* 为1表示分片的NAL开始,当它为1时,E不能为1 */
                unsigned char FUheader_S : 1;
            } NALU;

            /* H265 */
            struct {
                /*
                 * 0                   1
                 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
                 * + -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * |F| Type      | LayerId   | TID |
                 * +------------ - +---------------- - + 
                 */
                /* LayerID中的第一位,LayerId一般为0,TID一般为1 */
                unsigned char LayerId : 1;
                /* 类型:
                 * 单一 NAL 单元模式
                 * 0:NAL_TRAIL_N
                 * 1:被参考的后置图像，且非TSA、非STSA的SS编码数据
                 * 19:IDR,可能有RADL图像的IDR图像的SS编码数据
                 * 32:VPS,视频参数集
                 * 33:SPS,序列参数集
                 * 34:PPS,图像参数集
                 * 39:SEI,补充增强信息    
                 * 分片封包模式
                 * 49:用于把一个NALU单元封装成多个RTP包,存类似h264中的 FU-A
                 */
                unsigned char FUindicator_type : 6;
                /* 禁止位,值为1表示语法出错 */
                unsigned char FUindicator_F : 1;
                /* 第二个节字 */
                unsigned char char1;
                /* 仅分包时以下定义才有效 */
                /* 类型:
                 * 0:NAL_TRAIL_N
                 * 1:被参考的后置图像，且非TSA、非STSA的SS编码数据
                 * 19:IDR,可能有RADL图像的IDR图像的SS编码数据
                 * 32:VPS,视频参数集
                 * 33:SPS,序列参数集
                 * 34:PPS,图像参数集
                 * 39:SEI,补充增强信息    
                 */
                unsigned char FUheader_type : 6;
                /* 为1表示结束,当它为1,S不能为1 */
                unsigned char FUheader_E : 1;
                /* 为1表示分片的NAL开始,当它为1时,E不能为1 */
                unsigned char FUheader_S : 1;
            } H265NALU;
        } data;
    };

    struct RTP_old {
        /* 标记,固定为0x24,1字节 */
        unsigned char tag;
        /* 通道,1字节 */
        unsigned char channel;
        /* 数据长度,2字节,高位在前,包含之后的所有数据的长度 */
        unsigned char dataLenth[2];
        /* .... 0000 = Contributing source identifiers count: 0 
         * CSRC 计数(CC):4 比特,CSRC 计数包含了跟在固定头后面 CSRC 识别符的数目.
         */
        unsigned char CC : 4;
        /* ...0 .... = Extension: False
         * 扩展(X):1 比特,若设置扩展比特,固定头(仅)后面跟随一个头扩展;
         * 头扩展包含16比特的长度域,指示扩展项中32比特字的个数,不包括4个字节扩展头(因此零是有效值).
         * RTP固定头之后只允许有一个头扩展.为允许多个互操作实现独立生成不同的头扩展,或某种特定实现有多种不同的头扩展,扩展项的前16比特用以识别标识符或参数.
         * 这16比特的格式由具体实现的上层协议定义.基本的RTP说明并不定义任何头扩展本身.
         */
        unsigned char X : 1;
        /* ..0. .... = Padding: False
         * 填充(P):1 比特,若填料比特被设置,则此包包含一到多个附加在末端的填充比特,填充比特不算作负载的一部分.填充的最后一个字节指明可以忽略多少个填充比特.填充可能用于某些具有固定长度的加密算法.或者用于在底层数据单元中传输多个 RTP 包.
         */
        unsigned char P : 1;
        /* 10.. .... = Version: RFC 1889 Version (2)
         * 版本(V):2 比特,此域定义了 RTP 的版本.此协议定义的版本是 2.(值 1 被 RTP 草案版本使用,值 0 用在最初"vat"语音工具使用的协议中.)
         */
        unsigned char V : 2;
        /* Payload type: DynamicRTP-Type-96 (96) 
         * 负载类型(PT):7 比特，此域定义了负载的格式,由具体应用决定其解释,协议可以规定负载类型码和负载格式之间一个默认的匹配.其他的负载类型码可以通过非 RTP 方法动态定义.RTP发送端在任意给定时间发出一个单独的 RTP 负载类型;此域不用来复用不同的媒体流;
         * h264此值固定为0x60;
         * aac此值固定为0x61;
         */
        unsigned char PT : 7;
        /* 0... .... = Marker: False 
         * 标志(M):1 比特，标志的解释由具体协议规定。它用来允许在比特流中标记重要的事件,如帧边界;
         * 在帧的最后一个rtp也应该设置此标记为1;
         */
        unsigned char M : 1;
        /* Sequence number: 53949 
         * 序列号(sequence number):16 比特,每发送一个 RTP 数据包,序列号加 1,接收端可以据此检测丢包和重建包序列,序列号的初始值是随机的(不可预测),以使即便在源本身不加密时(有时包要通过翻译器,它会这样做),对加密算法泛知的普通文本攻击也会更加困难.
         */
        unsigned char SequenceNumber[2];
        /* Timestamp: 1992890304 
         * 时间戳(timestamp) ;32 比特,时间戳反映了 RTP 数据包中第一个字节的采样时间.时钟频率依赖于负载数据格式,并在描述文件(profile)中进行描述.也可以通过 RTP 方法对负载格式动态描述.
         * 如果 RTP 包是周期性产生的,那么将使用由采样时钟决定的名义上的采样时刻,而不是读取系统时间.例如,对一个固定速率的音频,采样时钟将在每个周期内增加 1.如果一个音频从输入设备中读取含有 160 个采样周期的块,
         * 那么对每个块,时间戳的值增加 160.时间戳的初始值应当是随机的,就像序号一样.几个连续的 RTP 包如果是同时产生的.如:属于同一个视频帧的 RTP 包,将有相同的序列号.
         * 不同媒体流的 RTP 时间戳可能以不同的速率增长.而且会有独立的随机偏移量.因此,虽然这些时间戳足以重构一个单独的流的时间,但直接比较不同的媒体流的时间戳不能进行同步.
         * 对于每一个媒体,我们把与采样时刻相关联的 RTP 时间戳与来自于参考时钟上的时间戳(NTP)相关联.因此参考时钟的时间戳就是数据的采样时间.(即:RTP 时间戳可用来实现不同媒体流的同步,NTP 时间戳解决了 RTP 时间戳有随机偏移量的问题.)
         * 参考时钟用于同步所有媒体的共同时间.这一时间戳对(RTP 时间戳和 NTP 时间戳),用于判断 RTP 时间戳和 NTP 时间戳的对应关系,以进行媒体流的同步.它们不是在每一个数据包中都被发送,而在发送速率更低的 RTCP 的 SR(发送者报告)中
         * 如果传输的数据是存贮好的,而不是实时采样得到的,那么会使用从参考时钟得到的虚的表示时间线(virtual presentation timeline).以确定存贮数据中的每个媒体下一帧或下一个单元应该呈现的时间.
         * 此种情况下 RTP 时间戳反映了每一个单元应当回放的时间。真正的回放将由接收者决定.
         */
        unsigned char Timestamp[4];
        /* Synchronization Source identifier: 0xd07ec35f (3497968479) 
         * SSRC:32 比特,用以识别同步源.标识符被随机生成,以使在同一个 RTP 会话期中没有任何两个同步源有相同的 SSRC 识别符.尽管多个源选择同一个 SSRC 识别符的概率很低,所有 RTP 实现工具都必须准备检测和解决冲突.
         * 若一个源改变本身的源传输地址,必须选择新的SSRC 识别符,以避免被当作一个环路源.RTP 包流的源,用 RTP 报头中 32 位数值的SSRC 标识符进行标识,使其不依赖于网络地址.
         * 一个同步源的所有包构成了相同计时和序列号空间的一部分,这样接收方就可以把一个同步源的包放在一起,来进行重放.举些同步源的例子，像来自同一信号源的包流的发送方,如麦克风,摄影机,RTP 混频器就是同步源.
         * 一个同步源可能随着时间变化而改变其数据格式,如音频编码.SSRC 标识符是一个随机选取的值,它在特定的 RTP 会话中是全局唯一(globally unique)的.参与者并不需要在一个多媒体会议的所有 RTP 会话中,，使用相同的 SSRC 标识符;
         * SSRC 标识符的绑定通过RTCP.如果参与者在一个 RTP 会话中生成了多个流,例如来自多个摄影机,则每个摄影机都必须标识成单独的同步源.
         */
        unsigned char SSRC[4];

        union {
            /* 对解码后的第一字节:
             * h264中第1位禁止位，值为1表示语法出错;
             * 第2~3位为参考级别;
             * 第4~8为是nal单元类型;
             */
            unsigned char Payload[2];

            /* H264 */
            struct {
                /* 类型:
                 * 单一 NAL 单元模式
                 * 0:未规定
                 * 1:非IDR图像中不采用数据划分的片段
                 * 2:非IDR图像中A类数据划分片段
                 * 3:非IDR图像中B类数据划分片段
                 * 4:非IDR图像中C类数据划分片段
                 * 5:IDR图像的片段
                 * 6:补充增强信息（SEI）
                 * 7:序列参数集（SPS）
                 * 8:图像参数集（PPS）
                 * 9:分割符
                 * 10:序列结束符
                 * 11:流结束符
                 * 12:填充数据
                 * 13:序列参数集扩展
                 * 14:带前缀的NAL单元
                 * 15:子序列参数集
                 * 16–18:保留
                 * 19:不采用数据划分的辅助编码图像片段
                 * 20:编码片段扩展
                 * 21–23:保留
                 * //24–31:在h264中未规定,而在这里表示这是一个特别格式的 NAL 单元
                 * 
                 * 组合封包模式
                 *     即可能是由多个 NAL 单元组成一个 RTP 包. 分别有4种组合方式: STAP-A, STAP-B, MTAP16, MTAP24.
                 *     那么这里的类型值分别是 24, 25, 26 以及 27;
                 *     24     STAP-A    Single-time aggregation packet
                 *     25     STAP-B    Single-time aggregation packet
                 *     26     MTAP16    Multi-time aggregation packet
                 *     27     MTAP24    Multi-time aggregation packet
                 *     28     FU-A      Fragmentation uni
                 *     29     FU-B      Fragmentation unit
                 *     30-31  undefined 
                 * 
                 * 分片封包模式
                 * 用于把一个NALU单元封装成多个RTP包,存在两种类型 FU-A和FU-B.类型值分别是 28 和 29
                 */
                unsigned char FUindicator_type : 5;
                /* 参考级别,与h264中的相同,根据分片NAL单元的NRI域的值设置 */
                unsigned char FUindicator_NRI : 2;
                /* 禁止位,值为1表示语法出错 */
                unsigned char FUindicator_F : 1;
                /* 仅分包时以下定义才有效 */
                /* 类型:
                 * 单一 NAL 单元模式
                 * 0:未规定
                 * 1:非IDR图像中不采用数据划分的片段
                 * 2:非IDR图像中A类数据划分片段
                 * 3:非IDR图像中B类数据划分片段
                 * 4:非IDR图像中C类数据划分片段
                 * 5:IDR图像的片段
                 * 6:补充增强信息（SEI）
                 * 7:序列参数集（SPS）
                 * 8:图像参数集（PPS）
                 * 9:分割符
                 * 10:序列结束符
                 * 11:流结束符
                 * 12:填充数据
                 * 13:序列参数集扩展
                 * 14:带前缀的NAL单元
                 * 15:子序列参数集
                 * 16–18:保留
                 * 19:不采用数据划分的辅助编码图像片段
                 * 20:编码片段扩展
                 * 21–23:保留
                 * //24–31:在h264中未规定,而在这里表示这是一个特别格式的 NAL 单元
                 * 
                 * 组合封包模式
                 *     即可能是由多个 NAL 单元组成一个 RTP 包. 分别有4种组合方式: STAP-A, STAP-B, MTAP16, MTAP24.
                 *     那么这里的类型值分别是 24, 25, 26 以及 27;
                 *     24     STAP-A    Single-time aggregation packet
                 *     25     STAP-B    Single-time aggregation packet
                 *     26     MTAP16    Multi-time aggregation packet
                 *     27     MTAP24    Multi-time aggregation packet
                 *     28     FU-A      Fragmentation uni
                 *     29     FU-B      Fragmentation unit
                 *     30-31  undefined 
                 * 
                 * 分片封包模式
                 * 用于把一个NALU单元封装成多个RTP包,存在两种类型 FU-A和FU-B.类型值分别是 28 和 29
                 */
                unsigned char FUheader_type : 5;
                /* 保留位 */
                unsigned char FUheader_R : 1;
                /* 为1表示结束,当它为1,S不能为1 */
                unsigned char FUheader_E : 1;
                /* 为1表示分片的NAL开始,当它为1时,E不能为1 */
                unsigned char FUheader_S : 1;
            } NALU;
        } data;
    };

    /* rtp中的padding,长度最大可定义到248字节 */
    struct Rtsp_rtp_padding {
        /* 采集时间和帧号标记,固定为"TI.fslib",此标记也是检测padding是否为此自定义协议的一个标记 */
        char gmtTimeAndFrameIndex[8];
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
    };
#pragma pack(pop)

    /* Rtsp客户端支持的命令 */
    typedef enum {
        /* 不确定值,不允许设置 */
        RtspClient_Cmd_Unknown = 0,
        /* 重新定位播放时间,参数为double时间 */
        RtspClient_Cmd_jump = 1,
        /* 改变播放模式,参数unsigned char(0-正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送) */
        RtspClient_Cmd_mode = 2
    } _RtspClient_Cmd;

    /* Rtsp服务 */
    typedef struct {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */
            void *_pMonitor;
            /* 
             * 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行;
             * 仅当设置此值为2时,系统可能会重启,其它正数系统可能退出,负数系统一定退出 
             */
            signed char *_externRunStatus;
            /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */
            pthread_t _parentTid;
            /* 管道,用于添加和删除通道或发送数据 */
            int __pipe[2];
            /* 线程名 */
#define Rtsp_get_threadName(___Rtsp) (((char*)((___Rtsp)+1))+(___Rtsp)->ro._threadName_)
            unsigned char _threadName_;
            /* 每个线程最多处理的通道数 */
            unsigned int _channelCountPerThread;
            /* 实时视频缓存的超时时间 */
            float _realBufferTimeOut;
            /* rtsp客户端条目链表,按指针排序 */
            FsObjectList *__itemListClient_;
            /* rtsp本地服务器条目链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__itemListServer;
            /* 集群的通道链表,按url排序,链表长度不等于通道的数量 */
            FsObjectList *__clusterItemListServer;
            /* udp端口对,成员为两个usigned short 大小,记录哪些udp端口对被使用了,不管是自动绑定的还是预分配的都应加入端口对中 */
            FsStructList *__udpPortList;

        } ro;

        struct {
            /* 运行状态:
             *     与_externRunStatus指向值相同-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             * 引用的指针类型必须是(char*);
             * 引用此值的其他线程应实现当自己的运行状态值[0-255]与此值[-128-127]不相等时退出.
             */
            unsigned char runStatus;
            /* 线程分时优先级,有效值为[0-99],越大优先级越高,0表示不改变优先级,此值会影响到子线程,再开始线程时会设置此值为用本线程的优先级 */
            signed char thisNice;
            /* ipv4地址端口号,为0表示不监听仅使用客户端拉流功能 */
            unsigned short _ipv4Port;
            /* 监听有ipv4地址,为0表示监听所有地址 */
            unsigned int _ipv4;
            /* 实时视频缓存的超时时间 */
            float _realBufferTimeOut;
            /* 外部和内部线程异步通信的链表,结构为Rtsp_P_control */
            FsStructList *__controlList;
        } rw;

        struct {
            /* 线程是否在运行(初始化为0,开始线程时置1,线程内部置为3后开始运行,线程退出时置为2,调用结束函数后置为0) */
            unsigned char inRun : 2;
            /* 子线程的运行状态:
             *     1-运行(一般此值为1);
             *     0-停止(手动退出后的状态值);
             *     2-重启(不一定实现);
             *     大于127-引用此值的所有线程都会退出;
             */
            unsigned char childRunStatus;
            /* 当前客户端的工作线程数 */
            unsigned short workThreadClient;
            /* 当前服务端的工作线程数 */
            unsigned short workThreadServer;
            /* 被控制速度的rtsp客户端数量 */
            unsigned int controlSpeedClientCount;
            /* 线程的tid号 */
            pthread_t stid;
            /* 主线程的线程号,由systemThreadTid获得 */
            pthread_t mainTid;
            /* 端口指针,前2字节表示端口个数,第3个字节表示是否有强制主线程建立连接的任务,第4字节表示是否是第一次监听,第一次监听应不设置等待时间
             * 第5-6字节表示上一次端口遍历的索引,使用__udpPortList的同步锁 */
            void *__udpStreamPort;

            /* 新加入的客户端服务条目链表 */
            FsObjectList *newItemListClient;
            /* rtsp推送连接,储存暂不需要流的连接,使用newItemListClient的互斥锁 */
            FsObjectList *clientListPull__newItemListClient;
            /* 线程异步通信的链表,结构为Rtsp_P_control */
            FsStructList *controlListClient;
            /* 客户端线程对象链表 */
            FsObjectList *threadListClient;
            /* 新加入的服务通道条目链表 */
            FsObjectList *newItemListServer;
            /* 本地的服务端通道数量 */
            unsigned long localItemListCount;
            /* 线程异步通信的链表,结构为Rtsp_P_control */
            FsStructList *controlListServer;
            /* 服务端端线程对象链表 */
            FsObjectList *threadListServer;
        } p;
    } Rtsp;
#define __RtspLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,3,___format",p=%p,runStatus=%hhd,tid=%ld\n  pMonitor=%p\n  externRunStatus=%p\n  parentTid=%ld\n  pipe=%d/%d\n  threadName:\"%s\"\n\
  channelCountPerThread=%u\n  itemListClient=%p/%lu\n  itemListServer=%p/%lu\n  clusterItemListServer=%p/%lu  ipv4:\"%hhu.%hhu.%hhu.%hhu:%hu\"\n  realBufferTimeOut=%f\n",##__VA_ARGS__\
        ,pRtsp,((Rtsp*)(pRtsp))->rw.runStatus,((Rtsp*)(pRtsp))->p.stid\
        ,((Rtsp*)(pRtsp))->ro._pMonitor,((Rtsp*)(pRtsp))->ro._externRunStatus,((Rtsp*)(pRtsp))->ro._parentTid,((Rtsp*)(pRtsp))->ro.__pipe[0], ((Rtsp*)(pRtsp))->ro.__pipe[1],Rtsp_get_threadName(pRtsp)\
        ,((Rtsp*)(pRtsp))->ro._channelCountPerThread,((Rtsp*)(pRtsp))->ro.__itemListClient_, ((Rtsp*)(pRtsp))->ro.__itemListClient_->nodeCount\
        ,((Rtsp*)(pRtsp))->ro.__itemListServer, ((Rtsp*)(pRtsp))->ro.__itemListServer->nodeCount,((Rtsp*)(pRtsp))->ro.__clusterItemListServer, ((Rtsp*)(pRtsp))->ro.__clusterItemListServer->nodeCount\
        ,Fs_Network_Ipv4_print(pRtsp->rw._ipv4), pRtsp->rw._ipv4Port, pRtsp->rw._realBufferTimeOut)
#define RtspLog(___loglevel,___format, ...) __RtspLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtspLog2(___loglevel,___format, ...) __RtspLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
    struct RtspClient_P_package;
    struct RtspClient_item_P_channel;

    /* Rtsp客户端项 */
    struct RtspClient_item {
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         * 无锁的定义;
         */
        FsDelete *_delete;

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 视频帧传出时前面预留大小 */
            unsigned int _videoPrefix;
            /* 视频帧传出时后面预留大小 */
            unsigned int _videoSuffix;
            /* 音频帧传出时前面预留大小 */
            unsigned int _audioPrefix;
            /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */
            int (*_cb)(/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
                    , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
                    , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac */unsigned char type
                    , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
                    , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
                    , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
                    , /* 数据的来源ip */unsigned int ip

                    , /* 外部传入的指针 */ void *const p);
            /* 外部的传入指针 */
            void *_externP;
            /* 命令字链表 */
            FsStructList *__cmdList;
            /* 连接的soket,负数表示无效 */
            int __sock;
            /* 已接收的长度 */
            unsigned int recvLen;
            /* 剩下的还要接收的长度 */
            unsigned int recvLenRemain;
            /* 已接收忽略数据的长度,在H264的分包类型为24的RTP包中用于记录已格式化的数据位置 */
            unsigned int recvLenNoUsed;
            /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */
            unsigned char _streamOver;
            /* 状态(小于11,双数只发数据):
             * 0:未连接;
             * 1:尝试连接;
             * 2:已连接成功,正在发送OPTIONS数据;
             * 3:接收OPTIONS数据;
             * 4:正在发送DESCRIBE数据;
             * 5:接收DESCRIBE数据;
             * 6:正在发送SETUP数据;
             * 7:接收SETUP数据;
             * 8:正在发送PLAY数据;
             * 9:转换为接收数据流;
             * 10:发送推流信息数据;
             * 11:接收推流信息数据;
             * 12:发送播放回执;
             * 13:接收数据流;
             * 14:接收与rtp有相同4字节头的ps数据;
             */
            unsigned char status : 4;
            /* 在status大于等于13时检查udp的连接是否完成,完成加入epoll收取数据,0-表示不需要检查,1-需要检查 */
            unsigned char checkUdpConnect : 1;
            /* 心跳类型:
             *     0-GET_PARAMETER rtsp://192.168.1.181/realtime.264/ RTSP/1.0\r\nCSeq: 5\r\nSession: 95246413\r\n\r\n
             *     1-OPTIONS * RTSP/1.0\r\nCSeq: 6\r\nSession: 1953025875\r\nAuthorization: Basic YWRtaW46MTIzNDU=\r\n\r\n
             */
            unsigned char heartbeatType : 1;
            /* 视频标记 */
            unsigned char videoTag;
            /* 音频标记 */
            unsigned char audioTag;
            /* 厂家标记 */
            unsigned char customTag;
            /* 上一次的视频序号,RTP实际为两字节,只判断一字节也能得到正确的结果 */
            unsigned char videoSequenceNumber;
            /* 视频类型,0-无效,1-h264,2-h265 */
            unsigned char videoType : 2;
            /* 接收状态,0-收数据头,1-收数据,2-收忽略数据,3-收Rtp扩展 Extension */
            unsigned char recvStatus : 2;
            /* 是否丢失数据,(1:丢失,0:未丢失) */
            unsigned char lostData : 1;
            /* socket错误检查,0-表示不能容错,1-表示可以容错 */
            unsigned char socketErrorCheck : 1;
            /* 帧结束标记,0-表示未结束,1-表示找到结束标记 */
            unsigned char frameEnd : 1;
            /* 为非标准rtp数据时收rtp包头时的计数变量 */
            unsigned char rtpHeadRecvLen : 3;
            /* 数据包的封包类型,0-标准的rtp,1-只有rtp4字节头,数据为ps封包 */
            unsigned char packageType : 1;
            /* 是否在控制播放速度,(0:未控制,非0:表示控制流的通道号(1表示第一个通道)) */
            unsigned char isControlSpeed;
            /* 为非标准rtp数据时rtp包的剩余长度 */
            unsigned short otherRtpLen;
            /* 帧号,仅使用后三字节 */
            unsigned int frameIndex;
            /* rtp中的padding */
            struct Rtsp_rtp_padding padding;
            /* 包头 */
            struct RTP rtp[2];
        } ro;

        struct {
            /* 最后一次收发数据的时间(CPU的uptime时间),在没有连接时表示上一次尝试的时间 */
            double lastSendOrRecvTime;
            /* 最后一次发送心跳的时间 */
            double lastSendHeartbeatTime;
        } rw;

        struct {
            /* 所属的pRtsp对象,不能根据其是否为空来判断是否加入或移除线程,即此指针仅在添加时赋值 */
            Rtsp *pRtsp;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除,(controlStatus&0x2)-表示是否开始实时流 */
            unsigned char _controlStatus : 2;
            unsigned char : 5;
            /* 此变量标识实时流的开始与结束(实现开始停止不使用busy变量),0-停止,1-开始,底层是否开始由(_controlStatus&0x2)判断 */
            unsigned char _streamStatus_set : 1;
            /* 是否是推流客户端,0-不是推流,1-推流 */
            unsigned char _isPull : 1;
            unsigned char : 6;
            /* 命令是否禁止粘包,默认允许,在等待setup回执时如失败,则设置其为1,表示禁止,暂只实现setup指令禁止粘包 */
            unsigned char cmdStickyForbid : 1;
            /* 主线程标记,仅主线程可修改此值,0-默认值,无意义,1-创建连接错误 */
            unsigned char mainTag;
            /* ipv4地址端口号 */
            unsigned short _ipv4Port;
            /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */
            unsigned int _ipv4;
            /* 发送序号 */
#define RTSP_command_commandSendIndex_add(___commandSendIndex) (___commandSendIndex)=(((___commandSendIndex)+1))
            unsigned int commandSendIndex;
            /* 已发送命令的映射表
             * * 0-63:表示SETUP指令,其中号与流的通道相关
             * 64:OPTIONS指令
             * 65:ANNOUNCE指令
             * 66:RECORD指令
             * 67:DESCRIBE指令
             * 68:PLAY指令
             * 69:心跳GET_PARAMETER
             * 70:心跳OPTIONS
             * 71:跳转指令
             * 72:播放模式指令
             */
#define RTSP_command_OPTIONS 64
#define RTSP_command_ANNOUNCE 65
#define RTSP_command_RECORD 66
#define RTSP_command_DESCRIBE 67
#define RTSP_command_PLAY 68
#define RTSP_command_Heartbeat_GET_PARAMETER 69
#define RTSP_command_Heartbeat_OPTIONS 70
#define RTSP_command_CMD_jmp_PLAY 71
#define RTSP_command_CMD_mode_SET_PARAMETER 72
            Mapping_I8 *__pMapping_I8_cmd;
            /* 域名,可为空 */
            char *_domain;
            /* 用户名,可为空 */
            char* _username;
            /* 密码,可为空 */
            char* _password;
            /* url地址,不可为空 */
            char *_url;
            /* 重定向的服务端信息,为空表示未重定向,对推流的客户端用此指针记录有相同url的下一个客户端 */
            void *__serverInfoRedirect;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程,-1表示与工作线程同步中,-2表示无需与工作线程同步 */
            int pipe_w;
            /* 当前用于接收数据的Buffer,可为空 */
            FsObjectBase *__pObjectBase_recv;
            /* 接收数据的空间大小 */
            unsigned int _bufferlenth;
            /* 接收数据的空间,此变量应清理 */
            char *__buffer;
            /* 认证信息 */
            void *__pAuthorization;
            /* 真实的地址 */
            char *__realUrl_H4;
            /* 视频的编码信息,对h264就是储存sps和pps,前sizeof(unsigned int)字节表示长度,不含头 */
            char *__videoCode_H4;
            /* 会话 */
#define RtspClient_item_check_session
            char *__session_H4;
            /* 发送的数据 */
            struct RtspClient_P_package *__pSend;
            /* 当前的rtp */
            struct RTP *pRtp;
            /* 上一次的rtp */
            struct RTP *pRtpLast;
            /* 接收数据的空间,第一个int表示数据长度,(对于tcp在收帧数据时第二个int表示数据区相对于pChannel->pObjectBase->data偏移量,第三个int表示数据区的长度),在没有数据可收时,但数据长度又不足解析时把数据暂存的空间,为空表示无暂存数据 */
            char *__recvData_Hiii;
            /* 通道数量 */
            unsigned char channelCount;
            /* 通道信息,仅当其使用tcp传输时连接可在线程中移动 */
            struct RtspClient_item_P_channel **pChannel;
        } p;
    };
#define __RtspClient_itemPrintf(___Printf,___format, ...)  \
    ___Printf(FsPrintfIndex,___format",server(sock=%d/%hhu):\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",src:\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",cmdStickyForbid=%hhd,recvData_Hi(=%p):%d/%d/%d/%u/%u/%u/%u,sendLen=%u/%u/%u/%u,frameIndex=%u.\n",##__VA_ARGS__\
        ,pRtspClient_item->ro.__sock, pRtspClient_item->ro.status\
        ,FsStrprint(pRtspClient_item->p._username,""), pRtspClient_item->p._username ? ":" : ""\
        ,pRtspClient_item->p._username ? pRtspClient_item->p._password : "", pRtspClient_item->p._username ? "@" : ""\
        ,FsStrprint(pRtspClient_item->p._domain,""), pRtspClient_item->p._domain ? "(" : "",Fs_Network_Ipv4_print(pRtspClient_item->p._ipv4)\
        ,pRtspClient_item->p._domain ? ")" : "", pRtspClient_item->p._ipv4Port, FsStrprint(pRtspClient_item->p._url,"")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprint(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->username,"")\
            :FsStrprint(pRtspClient_item->p._username,"")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->username?":":"")\
            :(pRtspClient_item->p._username ? ":" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprintBaseTrue((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect,username,password,"")\
            :(pRtspClient_item->p._username ? pRtspClient_item->p._password : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->username?"@":"")\
            :(pRtspClient_item->p._username ? "@" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprint(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->domain,"")\
            :FsStrprint(pRtspClient_item->p._domain,"")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->domain?"(":"")\
            :(pRtspClient_item->p._domain ? "(" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n1\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n1\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n2\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n2\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n3\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n3\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n4\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n4\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->domain?"(":"")\
            :(pRtspClient_item->p._domain ? ")" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->ipv4Port:pRtspClient_item->p._ipv4Port\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprint(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->url,"")\
            :FsStrprint(pRtspClient_item->p._url,"")\
        ,pRtspClient_item->p.cmdStickyForbid\
        ,pRtspClient_item->p.__recvData_Hiii,pRtspClient_item->p.__recvData_Hiii?((int*)pRtspClient_item->p.__recvData_Hiii)[0]:0\
        ,pRtspClient_item->p.__recvData_Hiii?((int*)pRtspClient_item->p.__recvData_Hiii)[1]:0\
        ,pRtspClient_item->p.__recvData_Hiii?((int*)pRtspClient_item->p.__recvData_Hiii)[2]:0\
        ,pRtspClient_item->ro.recvLen,pRtspClient_item->ro.recvLenRemain, pRtspClient_item->ro.recvLenNoUsed, pRtspClient_item->ro.otherRtpLen\
        ,(pRtspClient_item->p.__pSend?pRtspClient_item->p.__pSend->sendPos:0),(pRtspClient_item->p.__pSend?pRtspClient_item->p.__pSend->dataLen:0)\
        ,(pRtspClient_item->p.__pSend?fs_ObjectSingle_mallocLenth_get(pRtspClient_item->p.__pSend):0)\
        ,(pRtspClient_item->p.__pSend?fs_ObjectSingle_referCount_get(pRtspClient_item->p.__pSend):0),pRtspClient_item->ro.frameIndex)

#define RtspClient_itemPrintf(___format, ...) __RtspClient_itemPrintf(FsPrintf,___format,##__VA_ARGS__)
#define RtspClient_itemPrintf2(___format, ...) __RtspClient_itemPrintf(FsPrintf2,___format,##__VA_ARGS__)

#define __RtspClient_itemLog(___Log,___loglevel,___format, ...)\
    ___Log(___loglevel,5,___format",server(sock=%d/%hhu):\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\",src:\"%s%s%s%s%s%s%hhu.%hhu.%hhu.%hhu%s:%hu%s\"\
,busy=%hhu,controlStatus=%hhx,streamStatus_set=%hhx,isPull=%hhu,cmdStickyForbid=%hhd,mainTag=%hhu,pipe_w=%d,recvData_Hi(=%p):%d/%d/%d/%u/%u/%u/%u,sendLen=%u/%u/%u/%u,frameIndex=%u.\n",##__VA_ARGS__\
        ,pRtspClient_item->ro.__sock, pRtspClient_item->ro.status\
        ,FsStrprint(pRtspClient_item->p._username,""), pRtspClient_item->p._username ? ":" : ""\
        ,pRtspClient_item->p._username ? pRtspClient_item->p._password : "", pRtspClient_item->p._username ? "@" : ""\
        ,FsStrprint(pRtspClient_item->p._domain,""), pRtspClient_item->p._domain ? "(" : "",Fs_Network_Ipv4_print(pRtspClient_item->p._ipv4)\
        ,pRtspClient_item->p._domain ? ")" : "", pRtspClient_item->p._ipv4Port, FsStrprint(pRtspClient_item->p._url,"")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprint(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->username,"")\
            :FsStrprint(pRtspClient_item->p._username,"")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->username?":":"")\
            :(pRtspClient_item->p._username ? ":" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprintBaseTrue((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect,username,password,"")\
            :(pRtspClient_item->p._username ? pRtspClient_item->p._password : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->username?"@":"")\
            :(pRtspClient_item->p._username ? "@" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprint(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->domain,"")\
            :FsStrprint(pRtspClient_item->p._domain,"")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->domain?"(":"")\
            :(pRtspClient_item->p._domain ? "(" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n1\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n1\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n2\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n2\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n3\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n3\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((FsNetworkIPv4*) &((struct RtspClient_private_item_serverInfo*) pRtspClient_item->p.__serverInfoRedirect)->ipv4)->n.n4\
            :((FsNetworkIPv4*) &pRtspClient_item->p._ipv4)->n.n4\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->domain?"(":"")\
            :(pRtspClient_item->p._domain ? ")" : "")\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->ipv4Port:pRtspClient_item->p._ipv4Port\
        ,(pRtspClient_item->p.__serverInfoRedirect&&0==pRtspClient_item->p._isPull)?FsStrprint(((struct RtspClient_private_item_serverInfo*)pRtspClient_item->p.__serverInfoRedirect)->url,"")\
            :FsStrprint(pRtspClient_item->p._url,"")\
        ,pRtspClient_item->p.busy,pRtspClient_item->p._controlStatus,pRtspClient_item->p._streamStatus_set,pRtspClient_item->p._isPull,pRtspClient_item->p.cmdStickyForbid,pRtspClient_item->p.mainTag\
        ,pRtspClient_item->p.pipe_w\
        ,pRtspClient_item->p.__recvData_Hiii,pRtspClient_item->p.__recvData_Hiii?((int*)pRtspClient_item->p.__recvData_Hiii)[0]:0\
        ,pRtspClient_item->p.__recvData_Hiii?((int*)pRtspClient_item->p.__recvData_Hiii)[1]:0\
        ,pRtspClient_item->p.__recvData_Hiii?((int*)pRtspClient_item->p.__recvData_Hiii)[2]:0\
        ,pRtspClient_item->ro.recvLen,pRtspClient_item->ro.recvLenRemain, pRtspClient_item->ro.recvLenNoUsed, pRtspClient_item->ro.otherRtpLen\
        ,(pRtspClient_item->p.__pSend?pRtspClient_item->p.__pSend->sendPos:0),(pRtspClient_item->p.__pSend?pRtspClient_item->p.__pSend->dataLen:0)\
        ,(pRtspClient_item->p.__pSend?fs_ObjectSingle_mallocLenth_get(pRtspClient_item->p.__pSend):0)\
        ,(pRtspClient_item->p.__pSend?fs_ObjectSingle_referCount_get(pRtspClient_item->p.__pSend):0),pRtspClient_item->ro.frameIndex)
#define RtspClient_itemLog(___loglevel,___format, ...) __RtspClient_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtspClient_itemLog2(___loglevel,___format, ...) __RtspClient_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* 点播的客户端链表结构 */
    struct RtspServer_item_DemandClient {
        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */

        /* rw放前面,以便RTSP,gb28181的回放共用 */
        struct {
#ifndef __DemandClient_rw 
            unsigned int allMask;
            /* 外部标记是否出错,0-表示正常,1-表示已结束,-1表示出错 */
            signed char error;
            /* 数据发送类型(0--正序正常发送,1-倒序正常发送,2-正序关键帧发送,3-倒序关键帧发送,其他值-无效) */
            unsigned char sendType;
            /* 内部的发送缓存值 */
            unsigned char sendBufferCountMaxIn;
            /* 外部的发送缓存值 */
            unsigned char sendBufferCountMaxOut;
            /* 外部指针 */
            void *externP;
            /* 删除externP的函数指针,为空表示不删除 */
            void (*deleteExternP) (void *externP);
            /* 时长,为0表示未知,用clock格式一般用于按时点播类应用,不为0为文件点播用npt格式 */
            double duration;
            /* 跳转到的时间,为npt格式时表示距离文件开始的时间,为clock时表示1970年1月1号以来经过的秒数,在此值不为0时,调用者应改变视频流的的位置并把此值置0 */
            double jumpTime;
#endif
        } rw;

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 数据链表 */
            FsObjectList *__dataList;
            /* 数据的索引 */
            unsigned short index[32];
            /* 发送数据时rtp里面的timestamp值 */
            unsigned int timestamp[32];
            /* url参数 */
            FsUrlParameter *__pUrlParameter;
        } ro;
    };

    /* rtsp服务器条目,根据URL来划分 */
    struct RtspServer_item {

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 网络地址,为空意义不明确 */
            char *_url;
            /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */
            unsigned char _modelType : 3;
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            /* 是否有新的实时客户端,表示是否要生成数据描述信息,1-有,0-没有 */
            unsigned char newRealClient;
            /* 认证方式,0-无需认证,1-基本认证,2-摘要认证 */
            unsigned char _method;
            /* 主机ip,用于集群,0表示本机,低四位表示ip */
            unsigned int _ipv4;
        } r;

        struct {
#define RtspServer_item_ClusterLen (FsPointer_to_integer(&((struct RtspServer_item*) 0)->ro._passwordGet)+sizeof(void*)*3)
#define RtspServer_item_Cluster_callTimes(___pRtspServer_item) (*(unsigned int*)((&(___pRtspServer_item)->ro._passwordGet)+0))
#define RtspServer_item_Cluster_next(___pRtspServer_item) (*(struct RtspServer_item **)((&(___pRtspServer_item)->ro._passwordGet)+1))
#define RtspServer_item_Cluster_pRtspServer(___pRtspServer_item) (*(Rtsp**)((&(___pRtspServer_item)->ro._passwordGet)+2))
#define RtspServer_item_next(___pRtspServer_item) (0==(___pRtspServer_item)->r._ipv4?(___pRtspServer_item)->p.next:RtspServer_item_Cluster_next(___pRtspServer_item))
#define RtspServer_item_pRtspServer(___pRtspServer_item) (0==(___pRtspServer_item)->r._ipv4?(___pRtspServer_item)->p.pRtsp:RtspServer_item_Cluster_pRtspServer(___pRtspServer_item))
            /* 从数据源获取指定用户名的密码的函数指针,在需要认证时,此函数指针不能为空 */
            signed char (*_passwordGet)(/* 数据源 */void *database, /* 用户名 */const char username[], /* 访问的URL,以/开头,不能为空 */const char url[],
                    /* 密码,成功时会把密码写入此对象中,对象中的lenth代表内存大小,不代表密码长度 */FsString *password);
            /* 如果此指针不为空,在本对象删除时会调用此函数 */
            void (*_databaseDelete)(/* 数据源 */void *database);
            /* 储存用户名和密码的数据源指针 */
            void *_database;
            /* 有客户端时的回掉函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*_cb_client)(/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p);
            /* 有客户端时的回掉函数关联的指针 */
            void *_cb_clientP;
            /* 实时视频数据链表,为空表示不支持实时视频 */
            FsObjectList *__realVideoList;
            /* 连接的实时客户端链表,只有当实时视频数据链表不为空时才有效 */
            FsCycleList __realClientList;
#define  Rtsp_mutex_realClientList(___pRtsp) (___pRtsp)->ro.__mutex_realClientList
            /* 实时客户端的互斥锁,只有当实时视频数据链表不为空时才有效 */
            pthread_mutex_t __mutex_realClientList;
            /* 实时视频数据的索引,只有当实时视频数据链表不为空时才有效 */
            unsigned short *_realVideoIndex;
            /* 发送数据时rtp里面的timestamp值,只有当实时视频数据链表不为空时才有效 */
            unsigned int *_timestamp;
            /* 实时推流客户端,只有当实时视频数据链表不为空时才有效 */
            FsObjectList *__realPushClient_;
        } ro;

        struct {
            /* 
             * 点播客户端链表,仅当有历史数据功能时有效,实时视频数据链表不为空时,此链表储存历史点播的客户端,否则储存文件点播客户端,也可能不支持点播;
             * 储存类型是继承FsCycleListNode的客户端连接对象,外部使用宏RtspServer_DemandClient_get(__demandClientList.node->next)可取得第一个(struct RtspServer_item_DemandClient)类型的点播客户端指针;
             */
            FsCycleList __demandClientList;
            /* 历史客户端的互斥锁,仅当有历史数据功能时有效 */
            pthread_mutex_t __mutex_demandClientList;
            /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */
            unsigned char _streamOver;
        } rw;

        struct {
            /* 上一次实时视频的数量 */
            unsigned int realVideoListCountLast;
            /* 实时数据的掩码 */
            unsigned int mask_realAll;
            /* 实时数据中不是所有帧都为关键帧的掩码 */
            unsigned int mask_realkey;
            /* 管道,用于同工作线程通信的管道,资源属于工作线程 */
            int pipe_w;
            /* 当前所有准备开始实时流的客户端掩码之和 */
            unsigned int newRealStreamMask;
            /* 流的ssrc的个数,当实际流的数量比提供的ssrc数量多时,使用此值对流的编号取余后再查找ssrc */
            unsigned char _ssrcCount : 5;
            /* 是否有udp客户端,0-没有,1-有 */
            unsigned char hasUdpClient : 1;
            /* 是否有推流客户端等待发送ANNOUNCE中  */
            unsigned char hasPushDoSendANNOUNCE : 1;
            /* 统计实时流中的帧率,结果应更接近低频通道 */
            float _realRate_low;
            /* 用于统计realRate_low的辅助变量 */
            float realRate_low_last;
            /* 具有相同url的下一个主机 */
            struct RtspServer_item *next;
            /* 所属的rtsp服务器对象,为空表示未关联任何服务器对象 */
            Rtsp *pRtsp;
            /* 流的ssrc */
            unsigned int _ssrc[0];
        } p;
    };
#define __RtspServer_itemLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",p=%p,url:\"%s\",addr:\"%hhu.%hhu.%hhu.%hhu\".\n",##__VA_ARGS__\
        ,pRtspServer_item,pRtspServer_item->r._url, Fs_Network_Ipv4_print(pRtspServer_item->r._ipv4))
#define RtspServer_itemLog(___loglevel,___format, ...) __RtspServer_itemLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define RtspServer_itemLog2(___loglevel,___format, ...) __RtspServer_itemLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    struct RtspServer_P_client_streamUdp;

    /* rtsp服务端连接socket,因存在udp连接,则在脱离rtsp时就应关闭所有资源 */
    struct RtspServer_P_client {
        /* 继承FsCycleListNode */
        struct FsCycleListNode cycleListNode;
        /* 删除方法,不能直接读写,应使用Fs_Delete_get读,使用Fs_Delete_set写
         * 锁的定义:
         *     实时客户端指pClient->pRtspServer_item->ro.__mutex_realClientList
         *     历史客户端指pClient->pRtspServer_item->ro.__mutex_demandClientList
         */
        FsDelete *_delete;
        /* 点播结构指针,为空表示不为点播客户端,状态为1时,指向推流的url */
        struct RtspServer_item_DemandClient *pDemandClient;
        /* 状态rtsp:0-查找连接所属通道中,1-推流缓存客户端,2-已确定通道,等待验证用户名和密码中,3-等待加入到工作线程中(回调后状态变为4),4-等待工作线程响应中,5-等待播放信息设置中,6-推流预处理中,7-等待播放数据,8-播放中 */
        unsigned char status : 4;
        /* socket的检查状态,&0x1表示读,&0x2表示写 */
        unsigned char socketCheckStatus : 2;
        /* 当前正在尝试流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp,为客户端拉取服务器流时,此值初始化为0,客户端推送数据到服务端时此值应初始化为RtspServer_item_push中的值 */
        unsigned char streamOver : 3;
        /* 客户端的ipv4地址 */
        unsigned int ipv4;
        /* 客户端的ipv4地址端口号 */
        unsigned int ipv4Port;
        /* 连接socket对象,-1表示socket不需要删除 */
        int sock;
        /* 超时时间,对推流客户端来说为下一次发送的数据的时间,对RtspServer_item_push来说为下一次尝试连接的时间,因发送量大,可使用发送时间判断连接通断,否则应使用接收时间判断 */
        double timeout;
        /* 所属的rtsp服务项,在未确定是属于何通道时储存rtsp服务对象 */
        struct RtspServer_item *pRtspServer_item;
        /* 为空表示此客户端发流使用tcp */
        struct RtspServer_P_client_streamUdp *streamUdp;
        /* 发送的数据链表,内容为继承FsObjectSingle的struct Rtsp_P_package指针,不能为空,当为tcp连接时,所有数据包都在此链表中 */
        FsObjectList *list_;
        /* 当前使用发送包,可能为空 */
        struct Rtsp_P_package *pSendData_tcp;
#ifdef Rtsp_stats
        /* 已成功发送的数据量 */
        unsigned long long hasSendByte;
        /* 已接收的数据量 */
        unsigned long long hasRecvByte;
#endif
        /* 缓存数量,udp通信时只计算数据帧的数量 */
        unsigned int bufferCount;

        /* 播放掩码,在发送dataInfo后置0 */
        unsigned int playMask;
        /* 发送的位置 */
        unsigned int pSendData_tcpSendPos;
        /* 
         * 对于某些指令可以需要等待回执
         *     比如收到DESCRIBE指令后,把指令的cseq存于此变量中,待准备好发送数据时使用此编号回执
         */
        unsigned int cseq;
        /* 接收数据的空间,第一个int大小节表示数据长度,第二和第三个int固定为0为同pRtspClient_item兼容,在没有数据可收时,但数据长度又不足解析时把数据暂存的空间,为空表示无暂存数据 */
        char *__recvData_Hiii;
    };

    /* 
     * Rtsp服务推送对象 
     */
    struct RtspServer_item_push {
        /* 继承RTSP服务端的客户端连接对象 */
        struct RtspServer_P_client client;

        /* 
         * 结构约定:
         *     r中为外部可读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即r中的变量值在赋值后可能会修改),在集群应用时,用于存放公共变量;
         *     ro中为外部只读变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读由内部结构决定,即ro中的变量值在赋值后不会修改);
         *     rw中为外部可读写变量(仅限当前指针所指向的结构,内部指针所指向的内容是否可读写由内部结构决定,即rw中的变量值可在外部及内部直接修改,如是指针类变量应提供锁机制);
         *     p中为私有变量,外部不可读也不可写,(即p中的变量可能会被内部修改,对指针类变量,外部不能通过其指针直接访问指针指向的数据);
         * 变量命名约定:
         *     结构中所有变量名在ro,rw,p中要唯一,如不能出现ro中的变量与rw中的变量同名;
         *     变量名不能使用_前缀后缀区分不同变量名,__为分隔符,分隔符后的不作为变量名,如_aa_bb__cc表示aa_bb变量;
         *     凡需要(非0)初始化的变量,都要加上_的前缀;
         *     凡需要内存释放的变量,都要加上__的前缀;
         *     整形变量使用_结尾表示变量值为相对结构体尾部的一个偏移量,最大值表示无效值;
         *     指针变量使用_结尾表示变量指向的对象丧失安全设置,如需要使用其他变量的安全设置应使用__连接其他变量名,如本变量aa使用bb变量的安全设置,应命名为aa__bb.
         */
        struct {
            /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */
            unsigned char _streamOver;
        } ro;

        struct {
            /* 最后一次发送心跳的时间 */
            double lastSendHeartbeatTime;
        } rw;

        struct {
            /* 是否繁忙,1-繁忙,0-空闲,外部接口设置为1,内部线程设置为0 */
            unsigned char busy : 1;
            /* 控制状态,值仅在接口函数中修改,内部线程不修改也不读取此值,(controlStatus&0x1)-表示是否添加或移除 */
            unsigned char _controlStatus : 2;
            /* 状态:
             * 0:未连接;
             * 1:尝试连接;
             * 2:已连接成功,并发送OPTIONS数据后收OPTIONS数据(获取session,可能收到重定向);
             * 3:等待发送ANNOUNCE数据(等待准备好媒体描述信息);
             * 4:已发送ANNOUNCE,等待接收ANNOUNCE数据(获取session);
             * 5:已发送SETUP和RECORD数据等待接收RECORD的回执数据;
             * 6:设置RtspServer_P_client中的状态为6(之前的状态为5),完全按照RtspServer_P_client的逻辑处理,插到pRtspServer_item->ro.__realClientList中,之前状态不在pRtspServer_item->ro.__realClientList中.
             */
            unsigned char status : 3;
            /* 心跳类型:
             *     0-GET_PARAMETER rtsp://192.168.1.181/realtime.264/ RTSP/1.0\r\nCSeq: 5\r\nSession: 95246413\r\n\r\n
             *     1-OPTIONS * RTSP/1.0\r\nCSeq: 6\r\nSession: 1953025875\r\nAuthorization: Basic YWRtaW46MTIzNDU=\r\n\r\n
             */
            unsigned char heartbeatType : 1;

            /* 播放掩码,在status变为6时会使用此值校验播放掩码是否正确 */
            unsigned int playMask;
            /* 发送序号 */
            unsigned int commandSendIndex;
            /* 已发送命令的映射表,见RtspClient中的映射定义 */
            Mapping_I8 *__pMapping_I8_cmd;
            /* 域名,可为空 */
            char *_domain;
            /* 用户名,可为空 */
            char* _username;
            /* 密码,可为空 */
            char* _password;
            /* url地址,可为空 */
            char *_url;
            /* 重定向的服务端信息,为空表示未重定向,和pRtspClient_item中的定义相同 */
            void *__serverInfoRedirect;
            /* 认证信息 */
            void *__pAuthorization;
            /* 真实的地址 */
            char *__realUrl_H4;
            /* 会话 */
            char *__session_H4;
        } p;
    };
    /* 
     * 创建一个新的Rtsp对象;
     * 返回Rtsp指针.
     */
    Rtsp * rtsp_new__IO(/* 监控的线程指针对象,开始线程后把创建的监控节点添加到此对象中 */ void *const pMonitor
            , /* 多线程共有的运行状态,如果此值为空,则引用默认的全局运行状态变量,本线程的运行状态只有与此值相同才能运行 */signed char *const externRunStatus
            , /* 父线程的线程号,由systemThreadTid获得,为0表示无父线程 */ const pthread_t parentTid, /* 线程名 */const char threadName[]
            , /* 监听的ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号,为0表示不监听不支持推流 */const unsigned short ipv4Port
            , /* 每个线程最多处理的通道数 */ const unsigned int channelCountPerThread, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 设置pRtsp的参数(需要线程同步的部分参数) */
    void rtsp_set_parameter_thread_pthreadSafety(Rtsp * const pRtsp, /* 监听的ipv4地址,为0表示监听所有地址 */ const unsigned int ipv4, /* ipv4地址端口号 */const unsigned short ipv4Port
            , /* udp流端口,为空表示禁用udp,多个使用逗号隔开,如8200,8201,8202,8203或8200,8201-8202,8203或8200-8263 */const char udpStreamPort[]
            , /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);
    /* 设置pRtsp的参数(不需要线程同步的部分参数) */
    void rtsp_set_parameter_pthreadSafety(Rtsp * const pRtsp, /* 实时视频缓存的超时时间 */ const float realBufferTimeOut);

    /* 删除pRtsp指向的实例对象 */
    void rtsp_delete__OI(Rtsp * const pRtsp);
    /* 开始线程,必须先设置好初始化时为可写全过程可读区的所有参数 */
    void rtsp_startThread(Rtsp * const pRtsp
            , /* 线程分时优先级,有效值为[-1-99],越大优先级越高,0表示不改变优先级,继承父线程的优先级,-1-表示强制线程的调度模式为分时调度策略,正数强制线程的调度模式为实时分时调度策略 */const signed char thisNice);
    /* 结束线程,只发送信号,不回收线程资源 */

    void rtsp_stopThread_noWait(Rtsp * const pRtsp);

    /* 结束线程 */
    void rtsp_stopThread(Rtsp * const pRtsp);

    /* 
     * 创建一个新的RtspClient_item对象;
     * 返回RtspClient_item指针.
     */
    struct RtspClient_item * rtspClient_item_new__IO(/* 域名,可为空 */ const char domain[], /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4
            , /* ipv4地址端口号,以此值判断是否是推流 */const unsigned short ipv4Port, /* 用户名 */const char username[], /* 密码 */const char password[], /* url地址 */const char url[]
            , /* 视频帧传出时前面预留大小 */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小 */ const unsigned int videoSuffix, /* 音频帧传出时前面预留大小 */ const unsigned int audioPrefix
            , /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */const unsigned char streamOver, /* 取流状态,0-停止取流,1-开始取流 */const unsigned char streamControl
            , /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */ int (*const cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
            , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
            , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac */unsigned char type
            , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
            , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
            , /* 数据的来源ip */unsigned int ip

            , /* 外部传入的指针 */ void *const p), /* 外部传入的指针 */void *externP);
    /* 
     * 创建一个新的RtspClient_item对象;
     * 成功返回RtspClient_item指针;
     * 失败返回NULL.
     */
    struct RtspClient_item *rtspClient_item_new_byURL__IO(/* url地址,如rtsp://admin:admin@127.0.0.1:554/video */const char url[]
            , /* 视频帧传出时前面预留大小 */ const unsigned int videoPrefix, /* 视频帧传出时后面预留大小 */ const unsigned int videoSuffix, /* 音频帧传出时前面预留大小 */ const unsigned int audioPrefix
            , /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */const unsigned char streamOver, /* 取流状态,0-停止取流,1-开始取流 */const unsigned char streamControl
            , /* 收到数据后的回调函数,返回1,表示成功,返回2表示客户端在控制速度,错误返回负数 */ int (*const cb) (/* 收到的数据,如直接使用此内存可增加pData的引用计数 */FsObjectBase *pData
            , /* 真实时间,gmt时间,小于等于0表示无效,此值主要用于跳转 */ double realTime
            , /* 帧号,最大值只能到3个字节 */ unsigned int frameIndex, /* 数据类型,1-h264,2-h265,3-aac */unsigned char type
            , /* 是否是关键帧(-1:无法判断;0;不是;1:是,(&0x2)==0x2-对h264表示包含sps和pps,对h265表示包含vdp,sps和pps) */signed char isKeyFrame, /* 帧率(小于0.00001表示未知) */ float frameRate
            , /* 图像宽度(0表示未知) */ unsigned int imageWidth, /* 图像高度(0表示未知) */ unsigned int imageHeight, /* 拼接编号 */unsigned char stitchIndex
            , /* 图像拼接的左裁剪像素 */ short cutLeft, /* 图像拼接的右裁剪像素 */ short cutRight
            , /* 数据的来源ip */unsigned int ip

            , /* 外部传入的指针 */ void *const p), /* 外部传入的指针 */void *externP);
    /* 删除pRtspClient_item指向的实例对象 */
    void rtspClient_item_delete__OI(struct RtspClient_item * const pRtspClient_item);
    /* 控制流开始停止,必须在添加到服务后才能调用此函数 */
    void rtspClient_item_streamControl_pthreadSafety(struct RtspClient_item * const pRtspClient_item, /* 控制值,0-停止取流,1-开始取流 */const unsigned char value);
    /* 向pRtsp中添加一个rtsp客户端条目 */
    void rtspClient_add_item__OI_2(Rtsp * const pRtsp, struct RtspClient_item * const pRtspClient_item);
    /* 从pRtsp中移除一个rtsp客户端条目,未等待线程真正移除 */
    void rtspClient_remove_item_noWait__IO_2(Rtsp * const pRtsp, struct RtspClient_item * const pRtspClient_item);
    /* 从pRtsp中移除一个rtsp客户端条目 */
    void rtspClient_remove_item__IO_2(Rtsp * const pRtsp, struct RtspClient_item * const pRtspClient_item);
    /* 向pRtspClient_item发送命令,只是加到发送链表,命令是否生效只能在业务层来判断,成功返回1,失败返回-1 */
    signed char rtspClient_item_send_cmd(struct RtspClient_item * const pRtspClient_item, /* 要发送的命令 */const _RtspClient_Cmd cmd,
            /* 参数,此处只的规定参数最多占8个字节,实际类型及占用多少字节由命令字类型决定 */const unsigned long long parameter);
    /* 
     * 创建一个新的RtspServer_item对象;
     * 返回RtspServer_item指针.
     */
    struct RtspServer_item * rtspServer_item_new__IO(/* 网络地址,为空意义不明确 */ const char url[], /* 主机ip,为0表示本地主机,非0表示远程主机,之后的数据仅对此值为0时有效 */const unsigned int ipv4
            , /* 模式类型;1-实时转发,2-历史点播模式,3-实时转发+历史点播模式,4-文件点播模式 */const char modelType, /* 认证方式,0-无需认证,1-基本认证,2-摘要认证 */ const char method
            , /* 从数据源获取指定用户名的密码的函数指针,在需要认证时,此函数指针不能为空 */
            signed char (*passwordGet)(/* 数据源 */void *database, /* 用户名 */const char username[], /* 访问的URL,以/开头,不能为空 */const char url[],
            /* 密码,成功时会把密码写入此对象中,对象中的lenth代表内存大小,不代表密码长度 */FsString * password)
            , /* 如果此指针不为空,在本对象删除时会调用此函数 */ void (*databaseDelete)(/* 数据源 */void *database)
            , /* 储存用户名和密码的数据源指针 */ void *database
            , /* 有客户端时的回调函数,为空表示不回调,对实时是在客户端有无切换时才回调,对历史视频视频有新客户端就回调 */
            void (*const cb_client) (/* 状态,-2-表示无客户端,-1-表示有客户端,大于-1表示历史视频或文件点播新客户端在链表中的位置 */const int index, void *const p)
            , /* 有客户端时的回掉函数关联的指针 */void *const cb_clientP, /* 流的承载协议,&0x1-tcp,&0x2-udp */ const unsigned char streamOver
            , /* 流的ssrc */const unsigned int ssrc[], /* 流的ssrc的个数,可为0,当实际流的数量比提供的ssrc数量多时,使用此值对流的编号取余后再查找ssrc */ unsigned char ssrcCount);
    /* 删除pRtspServer_item指向的实例对象 */
    void rtspServer_item_delete__OI(struct RtspServer_item * const pRtspServer_item);
    /* 向pRtsp中添加一个rtsp服务器条目 */
    void rtspServer_add_item__OI_2(Rtsp * const pRtsp, struct RtspServer_item * const pRtspServer_item);
    /* 从pRtspServer中移除一个rtsp服务器条目 */
    void rtspServer_remove_item__IO_2(Rtsp * const pRtsp, struct RtspServer_item * const pRtspServer_item);
    /* 添加实时数据到pRtspServer_item中 */
    void rtspServer_item_add_frame(struct RtspServer_item * const pRtspServer_item, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);
    /* 添加实时数据到pRtspServer_item中 */
    void rtspServer_item_add_frame__OI_2(struct RtspServer_item * const pRtspServer_item, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);
    /* 添加点播数据到pDemandClient中 */
    void rtspServer_item_DemandClient_add_frame(struct RtspServer_item_DemandClient * const pDemandClient, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);
    /* 添加点播数据到pDemandClient中 */
    void rtspServer_item_DemandClient_add_frame__OI_2(struct RtspServer_item_DemandClient * const pDemandClient, /* 要添加的数据,支持FsObjectH264,FsObjectAAC,必须设置"组轮询检测" */ void *const pObject);
    /* 新建一个推送对象 */
    struct RtspServer_item_push *rtspServer_item_push_new__IO(/* 域名,可为空 */ const char domain[], /* ipv4地址,为0表示无效,不应出现域名和ip同时无效的情况 */const unsigned int ipv4
            , /* ipv4地址端口号 */const unsigned short ipv4Port, /* 用户名 */const char username[], /* 密码 */const char password[], /* url地址 */const char url[]
            , /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */ const unsigned char streamOver);
    /* 
     * 使用URL新建一个推送对象;
     * 成功返回RtspServer_item_push指针;
     * 失败返回NULL.
     */
    /* 新建一个推送对象 */
    struct RtspServer_item_push *rtspServer_item_push_new_byURL__IO(/* url地址,如rtsp://admin:admin@127.0.0.1:554/video */const char url[]
            , /* 流的承载协议,&0x1-tcp,&0x2-udp,&0x4-kcp */ const unsigned char streamOver);
    /* 删除pRtspServer_item_push指向的实例对象 */
    void rtspServer_item_push_delete__OI(struct RtspServer_item_push * const pRtspServer_item_push);
    /* 向pRtspServer_item中添加一个rtsp服务推送对象 */
    void rtspServer_item_add_push__OI_3(/* pRtspServer_item所属的pRtsp,如pRtspServer_item不属于任何pRtsp可传空 */Rtsp * const pRtsp
            , struct RtspServer_item * const pRtspServer_item, struct RtspServer_item_push * const pRtspServer_item_push);
    /* 从pRtspServer_item中移除一个rtsp服务推送对象,未等待线程真正移除 */
    void rtspServer_item_remove_push_noWait__IO_3(/* pRtspServer_item所属的pRtsp,如pRtspServer_item不属于任何pRtsp可传空 */Rtsp * const pRtsp
            , struct RtspServer_item * const pRtspServer_item, struct RtspServer_item_push * const pRtspServer_item_push);
    /* 从pRtspServer_item中移除一个rtsp服务推送对象 */
    void rtspServer_item_remove_push__IO_3(/* pRtspServer_item所属的pRtsp,如pRtspServer_item不属于任何pRtsp可传空 */Rtsp * const pRtsp
            , struct RtspServer_item * const pRtspServer_item, struct RtspServer_item_push * const pRtspServer_item_push);


#ifdef __cplusplus
}
#endif

#endif /* RTSP_H */

