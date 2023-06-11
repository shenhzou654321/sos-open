/* 
 * File:   Kcp.h
 * Author: fsflib
 *
 * Created on 2019年4月14日, 下午1:01
 */
#if !defined KCP_H && !defined JAVA_IN_C
#define KCP_H
#include "../../PublicTools/publicDefine.h"

#include "../../PublicTools/Fs/Memery.h"
#include "../../PublicTools/Fs/Object.h"
#ifdef __cplusplus
extern "C" {
#endif
    //#define FsKcp_enable_rateCal

    /* 发送包信息 */
    struct FsKcp_SenPpackage {
        /* 发送的数据 */
        void *pSend;
        /* 发送序号开始和结束值 */
        unsigned int sendIndex[2];
        /* 包创建时间 */
        double addTime;
        /* 上一次被发送的时间,为0表示未发送,-1表示已确认收到回执了 */
        double lastSendTime[0];
    };

    /* 收到的数据包信息 */
    struct FsKcp_RecvPpackage {
        FsObjectSingle objectSingle;
        /* 数据长度 */
        unsigned int len;
        /* 数据区 */
        char *data;
        /* data指向的位置前有4字节保留空间,外部可使用;
         * 第4字节为0-表示此数据包是通过kcp收取的(已发送回执);
         * 第4字节为1-表示此数据包是通过kcp收取的(未发送回执);
         * 第4字节为0x80-表示数据包是通过udp收取的;
         */
        unsigned char head[4];
    };

    /* kcp数据
     * 兼容kcp数据和裸rtp数据,在数据确定为裸rtp数据时可以使用fs_Kcp_Raw_*相关的函数,宏可以使用FsKcpRaw*和Fs_Kcp_Raw*的宏
     */
    typedef struct {
        /* 接收到的数据的指针,成员为struct FsKcp_RecvPpackage对象 */
        FsObjectList *recvList_;
        /* 上一次收到可输出数据的时间,使用开机时间 */
        double lastOutTime;
        /* 上一次触发延时超时时接受链表的数据量 */
        unsigned int delayLastRecvListCount;
        /* 在丢包时延时输出的时间,单位毫秒,初始化为1000 */
        unsigned short delayOut;
        /* 接收数据帧的大小 */
        unsigned short recvMTU;
        /* 业务上的下一次接收编号,如rtp的seq,初始化为-1 */
        unsigned int recvSEQ;
        /* send的状态,0-不可发送,1-可发送,初始化为1 */
        unsigned char sendCheck : 1;
        /* 是否需要发送ack数据,0-不需要,1-需要发送 */
        unsigned char sendAck : 1;
        /* 标记收到的数据是否可以输出了,0-需要等待确认哪包数据为第一包,1-第一包数据已确认 */
        unsigned char recvFirst : 1;
        /* 当前已接收到的数据包的个数,只包含连续已确认的个数,不包含丢包后的数据包 */
        unsigned short recvCount;
        /* 接收数据已确认收到数据编号+1(message分片segment的序号+1),初始化为0 */
#define  FsKcpRaw_len  FsPointer_to_long(&((FsKcp*) 0)->ro.recv_una)
        unsigned int recv_una;
        /* 收数据缓存,为0表示只发,最大值为32767 */
        unsigned short recvCountMax;
        /* 通信延时,单位为毫秒,初始化为1000 */
        unsigned short delay;
        /* 当前最多还可发送多少数据,发送一条数据后减1,为0时从数据头开始只发送一条数据,初始化为2048 */
        unsigned short sendCount;
        /* 由于收到ack数据触发发送的数据量 */
        unsigned char sendCountByAck;
        /* 重传数据在ppMsg中的编号 */
        unsigned char reSendMsgIndex;
        /* ppMsg数组的数量 */
        unsigned char sendMsgBufferCount;
        /* 当前发送队列允许的长度,初始化为64 */
        unsigned char sendMsgBufferCountMax;
        /* 一般用于外部记录最后一次发送数据帧的大小,内部不使用,初始化为0 */
        unsigned int externLastFrameSize;
        /* 发送数据的会话编号,低8位应为0 */
        unsigned int sendConv;
        /* 发送包的序号,初始化为0 */
        unsigned int sendIndex;
        /* 发送窗口大小,使用发送序号的最大值表示,初始化为2048 */
        unsigned int sendWnd;
        /* 消息,为FsKcp_P_SenPpackage结构 */
        struct FsKcp_SenPpackage **ppMsg;

        /* 用于记录一个未发送回执的包 */
        struct FsKcp_RecvPpackage *pKcp_RecvPpackage;
    } FsKcp;
#define __FsKcpLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",pKcp=%p,pKcp->recvList_->nodeCount=%lu,lastOutTime=%lf/%u/%hu,sendCheck=%hhu/%hhu/%hhu,recvMTU=%hu/%u,recvCount=%hu/%hu,recv_una=%u,delay=%hu,sendCount=%hu,reSendMsgIndex=%hhu"\
     ",sendMsgBufferCount=%hhu/%hhu,sendConv=%u/%u,sendIndex=%u/%u,ppMsg=%p,pKcp_RecvPpackage=%p.\n",##__VA_ARGS__\
        ,pKcp,pKcp->recvList_->nodeCount,pKcp->lastOutTime,pKcp->delayLastRecvListCount,pKcp->delayOut,pKcp->sendCheck,pKcp->sendAck,pKcp->recvFirst,pKcp->recvMTU,pKcp->recvSEQ,pKcp->recvCount,pKcp->recvCountMax,pKcp->recv_una,pKcp->delay,pKcp->sendCount,pKcp->reSendMsgIndex\
        ,pKcp->sendMsgBufferCount,pKcp->sendMsgBufferCountMax,pKcp->sendConv,pKcp->externLastFrameSize,pKcp->sendIndex,pKcp->sendWnd,pKcp->ppMsg,pKcp->pKcp_RecvPpackage)
#define FsKcpLog(___loglevel,___format, ...) __FsKcpLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsKcpLog2(___loglevel,___format, ...) __FsKcpLog(FsLog2,___loglevel,___format,##__VA_ARGS__)
#define __FsKcpRawLog(___Log,___loglevel,___format, ...)\
     ___Log(___loglevel,5,___format",pKcp=%p,pKcp->recvList_->nodeCount=%lu,lastOutTime=%lf/%u/%hu,sendCheck=%hhu/%hhu/%hhu,recvMTU=%hu/%u,recvCount=%hu.\n",##__VA_ARGS__\
        ,pKcp,pKcp->recvList_->nodeCount,pKcp->lastOutTime,pKcp->delayLastRecvListCount,pKcp->delayOut,pKcp->sendCheck,pKcp->sendAck,pKcp->recvFirst,pKcp->recvMTU,pKcp->recvSEQ,pKcp->recvCount)
#define FsKcpRawLog(___loglevel,___format, ...) __FsKcpRawLog(FsLog,___loglevel,___format,##__VA_ARGS__)
#define FsKcpRawLog2(___loglevel,___format, ...) __FsKcpRawLog(FsLog2,___loglevel,___format,##__VA_ARGS__)

    /* kcp数据头,传输数据使用小端模式,kcp的第一字节必须为0,非0表示非kcp协议 */
    struct FsKcp_head {
#define Fs_Kcp_check(__first_Byte) ((__first_Byte)==0)
        /* 示例
         * 335bc495 51 00 2000 07000000 00000000 00000000 08000000 49276d2068657265
         * 335bc495 51 00 0004 31070000 00000000 01000000 08000000 49276d2068657265
         */
        /* 为一个表示会话编号的整数,和TCP的conv一样,通信双方需保证conv相同,相互的数据包才能够被接受.conv唯一标识一个会话,但通信双方可以同时存在多个会话 */
        unsigned int conv;
#define FsKcp_CMD_PUSH 0x51 //81 // cmd: push data
#define FsKcp_CMD_ACK 0x52 //82 // cmd: ack
#define FsKcp_CMD_WASK 0x53 //83 // cmd: window probe (ask)
#define FsKcp_CMD_WINS 0x54 //84
        /* 用来区分分片的作用.IKCP_CMD_PUSH:数据分片;IKCP_CMD_ACK:ack分片;IKCP_CMD_WASK:请求告知窗口大小;IKCP_CMD_WINS:告知窗口大小 */
        unsigned char cmd;
        /* 用户数据可能会被分成多个KCP包发送,frag标识segment分片ID(在message中的索引,由大到小,0表示最后一个分片) */
        unsigned char frg;
        /* 剩余接收窗口大小(接收窗口大小-接收队列大小),发送方的发送窗口不能超过接收方给出的数值 */
        unsigned short wnd;
        /* message发送时刻的时间戳 */
        unsigned int ts;
        /* message分片segment的序号,按1累次递增 */
        unsigned int sn;
        /* 待接收消息序号(接收滑动窗口左端).对于未丢包的网络来说,una是下一个可接收的序号,如收到sn=10的包,una为11 */
        unsigned int una;
        /* 数据长度 */
        unsigned int len;
        //        /* 在结构体IKCPSEG中,除了上述的包结构的字段外,还定义了几个非常重要的变量: */
        //        /* 下次超时重传的时间戳 */
        //        IUINT32 resendts;
        //        /* 该分片的超时重传等待时间,其计算方法同TCP */
        //        IUINT32 rto;
        //        /* 收到ack时计算的该分片被跳过的累计次数,此字段用于快速重传,自定义需要几次确认开始快速重传 */
        //        IUINT32 fastack;
        //        /* 发送分片的次数,每发送一次加一.发送的次数对RTO的计算有影响,但是比TCP来说,影响会小一些,计算思想类似 */
        //        IUINT32 xmit;
        char data[0];
    };
    /* 初始化raw kcp */
#define Fs_Kcp_Raw_init(___kcp,/* 当前时间 */___thisTime) do{\
    /* 接收到的数据的指针,成员为struct FsKcp_RecvPpackage对象 */\
    (___kcp).recvList_=fs_ObjectList_new_unsafe__IO(6);\
    /* 上一次收到可输出数据的时间 */\
    (___kcp).lastOutTime=___thisTime;\
    /* 上一次触发延时超时时接受链表的数据量 */\
    (___kcp).delayLastRecvListCount=0;\
    /* 在丢包时延时输出的时间,单位毫秒,初始化为1000 */\
    (___kcp).delayOut=1000;\
    /* 接收数据帧的大小 */\
    (___kcp).recvMTU=3 + 1500 - 42 + 1;\
    /* 业务上的下一次接收编号,如rtp的seq,初始化为-1 */\
    (___kcp).recvSEQ=(unsigned int)-1;\
     /* send的状态,0-不可发送,1-可发送,初始化为1 */\
    (___kcp).sendCheck=1;\
    /* 是否需要发送ack数据,0-不需要,1-需要发送 */\
    (___kcp).sendAck=0;\
    /* 标记收到的数据是否可以输出了,0-需要等待确认哪包数据为第一包,1-第一包数据已确认 */\
    (___kcp).recvFirst=1;\
    /* 当前已接收到的数据包的个数,只包含连续已确认的个数,不包含丢包后的数据包 */\
    (___kcp).recvCount=0;\
}while(0)
#define Fs_Kcp_init_oem(___kcp)

    /* 初始化kcp */
#define Fs_Kcp_init(___kcp,/* 当前时间 */___thisTime,/* 收数据缓存,为0表示只发,最大值为32767 */___recvCountMax) do{\
    Fs_Kcp_Raw_init(___kcp,___thisTime);\
    /* 收数据缓存,为0表示只发,最大值为32767 */\
    (___kcp).recvCountMax=(___recvCountMax);\
    /* 接收数据已确认收到数据编号+1(message分片segment的序号+1),初始化为0 */\
    (___kcp).recv_una=0;\
    /* 通信延时,单位为毫秒,初始化为1000 */\
    (___kcp).delay=1000;\
    /* 当前最多还可发送多少数据,发送一条数据后减1,为0时从数据头开始只发送一条数据,初始化为2048 */\
    (___kcp).sendCount=2048;\
    /* 由于收到ack数据触发发送的数据量 */\
    (___kcp).sendCountByAck=0;\
    /* 重传数据在ppMsg中的编号 */\
    (___kcp).reSendMsgIndex=0;\
    /* ppMsg数组的数量 */\
    (___kcp).sendMsgBufferCount=0;\
    /* 当前发送队列允许的长度,初始化为64 */\
    (___kcp).sendMsgBufferCountMax=64;\
    /* 一般用于外部记录最后一次发送数据帧的大小,内部不使用,初始化为0 */\
    (___kcp).externLastFrameSize=0;\
    /* 发送数据的会话编号,低8位应为0 */\
    (___kcp).sendConv=0xaabbcc00U;\
    /* 发送包的序号,初始化为0 */\
    (___kcp).sendIndex=0;\
    /* 发送窗口大小,使用发送序号的最大值表示,初始化为2048 */\
    (___kcp).sendWnd=2048;\
    /* 消息,为FsKcp_P_SenPpackage结构 */\
    (___kcp).ppMsg=NULL;\
    Fs_Kcp_init_oem(___kcp);\
    /* 用于记录一个未发送回执的包 */\
    (___kcp).pKcp_RecvPpackage=NULL;\
}while(0)
    /* 释放FsKCP Raw资源 */
#define Fs_Kcp_Raw_release(___pKcp) do{\
    /* 接收到的数据的指针,成员为struct FsKcp_RecvPpackage对象 */\
    /* while((__pKcp)->recvList_->nodeCount>0){  FsObjectSingle * const pObjectSingle = (FsObjectSingle*)fs_ObjectList_remove_head((__pKcp)->recvList_);\
       FsPrintf(1,"pObjectSingle=%p,referCount=%u/%d\n",pObjectSingle,pObjectSingle->referCount,pObjectSingle->_mallocLenth);\
       fs_ObjectSingle_delete__OI(pObjectSingle);\
            };\
    if(0)*/{fs_ObjectList_delete_unsafe_custom__OI((___pKcp)->recvList_, fs_ObjectSingle_delete__OI);}\
}while (0)

    /* 释放FsKCP资源 */
#define Fs_Kcp_release(___pKcp,/* 释放发送数据包的方法 */___delete_pSend_fun) do{\
    Fs_Kcp_Raw_release(___pKcp);\
    /* 消息 */\
    if((___pKcp)->ppMsg){\
        unsigned int i=(___pKcp)->sendMsgBufferCount;\
        while(i-->0){(___delete_pSend_fun)((___pKcp)->ppMsg[i]->pSend);fsFree((___pKcp)->ppMsg[i]);}\
        fsFree((___pKcp)->ppMsg);\
    }\
    /* 用于记录一个未发送回执的包 */\
    if((___pKcp)->pKcp_RecvPpackage)fs_ObjectSingle_delete__OI((___pKcp)->pKcp_RecvPpackage);\
}while (0)

    /* 清除pKcp中前面packageCount个接受数据包的空间 */
    void fs_Kcp_clear_recvList(FsKcp * const pKcp, unsigned short packageCount);

    /* 检查__pKcp是否可以发送新的数据包 */
#define Fs_Kcp_check_send_new_package(__pKcp) ((__pKcp)->sendCheck)
    /* 发送数据,成功返回发出的数据包的个数,失败返回-1,数据内容采用FsObjectSingle封包,数据格式为rtp */
    int fs_Kcp_send_ObjectSingle_rtp__OI_3(FsKcp * const pKcp, /* socket句柄 */const int sock, /* 发送的数据,Fs_Kcp_check_send_new_package为假时此值应为空 */FsObjectSingle * const pObjectSingle
            , /* 数据相对于pObjectSingle的偏移量 */const unsigned int dataOffet, /* 数据长度相对于pObjectSingle的偏移量 */unsigned int dataLenOffset, /* 最大的发送量 */unsigned int sendCountMax
            , /* 当前的开机时间 */const double thisUpTime, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 收数据,发的数据内容采用FsObjectSingle封包,数据格式为rtp
     * 返回可使用的数据包个数,为0表示无数据可收,返回负数表示出错.
     */
    int fs_Kcp_recv_ObjectSingle_rtp(FsKcp * const pKcp, /* socket句柄 */const int sock
            , /* 发送数据相对于pObjectSingle的偏移量 */const unsigned int dataOffet, /* 发送数据长度相对于pObjectSingle的偏移量 */unsigned int dataLenOffset
            , /* 当前的开机时间 */const double thisUpTime, /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

        /* 发送数据,成功返回发出的数据包的个数,失败返回-1,数据内容采用FsObjectSingle封包,数据格式为rtp,头只包含２字节的长度 */
    int fs_Kcp_send_ObjectSingle_rtp2__OI_3(FsKcp * const pKcp, /* socket句柄 */const int sock, /* 发送的数据,Fs_Kcp_check_send_new_package为假时此值应为空 */FsObjectSingle * const pObjectSingle
            , /* 数据相对于pObjectSingle的偏移量 */const unsigned int dataOffet, /* 数据长度相对于pObjectSingle的偏移量 */unsigned int dataLenOffset, /* 最大的发送量 */unsigned int sendCountMax
            , /* 当前的开机时间 */const double thisUpTime, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

        /* 
     * 收数据,发的数据内容采用FsObjectSingle封包,数据格式为rtp,头只包含２字节的长度
     * 返回可使用的数据包个数,为0表示无数据可收,返回负数表示出错.
     */
    int fs_Kcp_recv_ObjectSingle_rtp2(FsKcp * const pKcp, /* socket句柄 */const int sock
            , /* 发送数据相对于pObjectSingle的偏移量 */const unsigned int dataOffet, /* 发送数据长度相对于pObjectSingle的偏移量 */unsigned int dataLenOffset
            , /* 当前的开机时间 */const double thisUpTime, /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 
     * 收数据,数据确定为非kcp的裸rtp数据;
     * 返回可使用的数据包个数,为0表示无数据可收,返回负数表示出错.
     */
    int fs_Kcp_Raw_recv_ObjectSingle_rtp(FsKcp * const pKcp, /* socket句柄 */const int sock
            , /* 当前的开机时间 */const double thisUpTime, /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer, /* 共享buffer,可为空 */ FsShareBuffer * const pShareBuffer);

    /* 添加udp数据,仅当kcp未收取过数据时才能调用此函数,一般是外部已收取的数据,但数据需要使用kcp处理,成功返回1,数据被忽略返回-1 */
    int fs_Kcp_Raw_add_recvPpackage_udp(FsKcp * const pKcp, /* 数据 */const char data[], /* 数据长度 */ unsigned int dataLen, /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer);

        /* 添加udp数据,仅当kcp未收取过数据时才能调用此函数,一般是外部已收取的数据,但数据需要使用kcp处理,成功返回1,数据被忽略返回-1 */
    int fs_Kcp_Raw_add_recvPpackage_udp_and_check_rtp(FsKcp * const pKcp, /* 数据 */const char data[], /* 数据长度 */ unsigned int dataLen , /* 当前的开机时间 */const double thisUpTime, /* 单线程缓存Buffer,不为空 */FsObjectSingleBuffer * const pSingleBuffer);


#ifdef __cplusplus
}
#endif


#endif /* KCP_H */

